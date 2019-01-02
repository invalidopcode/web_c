/*jcmap_api.c - functions for clean use of jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jcmap_api.h"
#include "jcmap_states.h"
#include "jcmap_map.h"
#include "jcmap_char.h"


int jcmap_json_validate(void* buffer, size_t buffer_limit)
{
  jcmap_state_t* state;
  char* str_err;
   /*initialise state*/
    state = jcmap_st_create(st_validate, buffer, buffer_limit);
    if (NULL == state)
      return(false);

   /*get result*/
    str_err = jcmap_st_value(state);
    if (st_valid != jcmap_st_get_status(state))
    {
      jcmap_st_destroy(state);
      if (NULL != str_err)
      {
        printf("error validating json - %s\n", str_err);
        free(str_err);
      }
      return(false);
    }

   /*free state*/
    jcmap_st_destroy(state);
  return(true);
}

size_t jcmap_json_count(void* buffer, size_t buffer_limit)
{
  jcmap_state_t* state;
  char* str_err;
  size_t count = 0;
   /*initialise state*/
    state = jcmap_st_create(st_validate, buffer, buffer_limit);
    if (NULL == state)
    {
      errno = EAGAIN;  /*unknown reason, so this is the best we can do*/
      return(0);
    }

   /*get result*/
    str_err = jcmap_st_value(state);  /*counting is done automatically*/
    if (st_valid != jcmap_st_get_status(state))
    {
      jcmap_st_destroy(state);
      printf("error validating json - %s\n", str_err);
      errno = EINVAL;
      return(0);
    }

    count = jcmap_st_get_count( state );

   /*free state*/
    jcmap_st_destroy(state);
  return(count);
}

int jcmap_json_map_object(void* object, jcmap_map_handle_t* map, void* buffer, size_t buffer_limit)
{
  char* str_err;
  jcmap_state_t* state;
   /*initialise state structure*/
    state = jcmap_st_create(st_map_object, buffer, buffer_limit);
    if (NULL == state)
      return(false);
    state->map = map->map;
    state->map_max_depth = map->depth;
    state->map_target_object = (uint8_t*)object;

   /*run the map*/
    str_err = jcmap_st_object(state);
    if (st_valid != state->status)
    {
      jcmap_st_destroy(state);
      printf("error validating json - %s\n", str_err);
      return(false);
    }

  return(true);
}

int jcmap_json_map_array(void* object, size_t object_size, void* array, size_t n_elements, jcmap_map_handle_t* map, void* buffer, size_t buffer_limit)
{
  char* str_err;
  jcmap_state_t* state;
   /*initialise state structure*/
    state = jcmap_st_create(st_map_array, buffer, buffer_limit);
    if (NULL == state)
      return(false);
    state->map = map->map;
    state->map_max_depth = map->depth;
    state->map_target_object = (uint8_t*)object;
    state->map_target_array = (uint8_t*)array;
    state->map_target_size = object_size;

   /*run the map*/
    str_err = jcmap_st_array(state);
    if (st_valid != state->status)
    {
      jcmap_st_destroy(state);
      printf("error validating json - %s\n", str_err);
      return(false);
    }

  return(true);
}

void* jcmap_json_aquire(jcmap_target_t target_type, void* buffer, size_t buffer_limit)
{
  jcmap_state_t* state;
  char* str_err;
  void* aquired;
   /*initialise state*/
    state = jcmap_st_create(st_aquire, buffer, buffer_limit);
    if (NULL == state)
      return(false);
    jcmap_st_set_aquire_type(state, target_type);

   /*get result*/
    str_err = jcmap_st_value(state);
    if (st_valid != jcmap_st_get_status(state))
    {
      jcmap_st_destroy(state);
      if (NULL != str_err)
      {
        printf("error validating json - %s\n", str_err);
        free(str_err);
      }
    }

    aquired = jcmap_st_get_aquire( state );

   /*free state*/
    jcmap_st_destroy(state);
  return(aquired);
}

int jcmap_map( void* buffer, size_t buffer_limit, void* target, size_t target_size, ...)
{
  va_list args;
  va_list tmp_args;
  va_start(args, target_size);
  va_copy(tmp_args, args);
  size_t n_args = 0;
  size_t i = 0;
  jcmap_map_t* map;
  char* str_err;
  jcmap_state_t* state;
  jcmap_type_t type;
   /*build map from varguments*/
    while (true)
    {
      if (NULL == va_arg(tmp_args, char*))  /*end of mapping arguments*/
        break;
      va_arg(tmp_args, intptr_t);
      va_arg(tmp_args, jcmap_target_t);
      n_args++;
    }

    va_end(tmp_args);

    if (0 == n_args)
    {
      va_end(args);
      return(false);  /*need at least one item in map*/
    }

    map = calloc(n_args, sizeof(jcmap_map_t));

    for (i=0; i<n_args; i++)
    {
      map[i].key = va_arg(args, char*);
      map[i].key_size = strlen(map[i].key);
      map[i].offset = va_arg(args, intptr_t);
      map[i].type = va_arg(args, jcmap_target_t);
    }

    va_end(args);

   /*initialise state, set to map*/
    type = jcmap_char_detect_type( *(char*)buffer );
    switch (type)
    {
      case j_object :
        state = jcmap_st_create(st_map_object, buffer, buffer_limit);
        if (NULL == state)
          return (false);
        state->map_target_object = target;
        break;

      case j_array :
        state = jcmap_st_create(st_map_array, buffer, buffer_limit);
        if (NULL == state)
          return (false);
        state->map_target_array = target;
        break;

      default:
        return (false);
    }

    state->map = map;
    state->map_size = n_args;
    state->map_max_depth = 1;  /*TODO???*/
    state->map_target_size = target_size;

   /*run the mapping*/
    str_err = jcmap_st_value(state);
    if (st_valid != jcmap_st_get_status(state))
    {
      jcmap_st_destroy(state);
      if (NULL != str_err)
      {
        printf("error mapping json - %s\n", str_err);
        free(str_err);
      }
      return(false);
    }

   /*free state*/
    jcmap_st_destroy(state);
  return(true);
}

