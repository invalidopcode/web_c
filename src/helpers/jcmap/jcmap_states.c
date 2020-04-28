/*jcmap_states.c - recursive state functions for parsing utf8 json
         Licensed under cc0 (yes, this is different to everything else!)*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "jcmap_buffer.h"
#include "jcmap_map.h"
#include "jcmap_char.h"
#include "jcmap_debug.h"
#include "jcmap_string.h"
#include "jcmap_states.h"

/*it's a lot of faff typing this out again...*/
# define WHITESPACE_CLEAR() \
  do { \
    if (! jcmap_buffer_whitespace(&(state->buffer), &(state->buffer_limit)) ) \
      INVALIDATE("buffer_limit exceeded"); \
  } while(0)

# define ADVANCE_BUFFER()  \
  do { \
    if (! jcmap_buffer_next(&(state->buffer), &(state->buffer_limit)) ) \
    { \
      if ( false == state->can_end ) \
      { \
        INVALIDATE("buffer_limit exceeded"); \
      } else { \
        VALIDATE(); \
      } \
    } \
  } while(0)

# define VALID_ADVANCE_CONTINUE()  \
    if ( state->status == st_valid ) \
    { \
      state->status = st_pending; \
      ADVANCE_BUFFER(); \
      continue; \
    } \
  do { \
  } while (0)

# define VALID_RETURN()  \
  do { \
    if ( state->status == st_valid ) \
    { \
      JCDEBUG_LEAVE_FN(); \
      return (NULL); \
    } \
  } while (0)
# define INVALID_RETURN( error_string )  /*NOTE error_string must be free()'able -> ie INVALID_RETURN(strdup("errmsg"));*/\
  do { \
    if ( state->status == st_invalid ) \
    { \
      JCDEBUG_LEAVE_FN(); \
      return (error_string); \
    } \
  } while (0)
# define LIMIT_RETURN()  \
    if ( 0 == state->buffer_limit )  \
      INVALIDATE( "buffer_limit exceeded" )
# define LIMIT_VALID()  /*for numbers - no end character expected*/  \
    if ( 0 == state->buffer_limit )  \
      VALIDATE()

# define INVALIDATE( error_string )  \
  do { \
    state->status = st_invalid; \
    JCDEBUG_INVALID( *(state->buffer) ); \
    if (NULL == error_string) \
    { \
      JCDEBUG_LEAVE_FN(); \
      return (NULL); \
    } else { \
      JCDEBUG_ERR_FN(error_string); \
      return (strdup(error_string)); \
    } \
  } while (0)
#define VALIDATE() \
  do { \
    state->status = st_valid; \
    JCDEBUG_VALID( *(state->buffer) ); \
    JCDEBUG_LEAVE_FN(); \
    return (NULL); \
  } while(0)

jcmap_state_t* jcmap_st_create(jcmap_mode_t mode, char* buffer, size_t limit)
{
  jcmap_state_t* state;
   /*initialise everything to 0*/
    state = calloc(1, sizeof(jcmap_state_t));

   /*state basics NOTE - no need to initialise things that should be 0 - calloc sets already*/
    state->status = st_pending;
    state->mode = mode;
    state->buffer = buffer;
    state->buffer_limit = limit;
    state->can_end = false;

   /*aquire state*/
    state->aquire_type = c_null;
    state->aquire_result = NULL;
    state->aquire_buffer_limit = 0;
  return(state);
}

void jcmap_st_set_map(jcmap_state_t* state, jcmap_map_t* map, size_t max_depth)
{
  state->map = map;
  state->map_max_depth = max_depth;
}

void jcmap_st_set_target_object(jcmap_state_t* state, void* target)
{
  state->map_target_object = (uint8_t*) target;
}

void jcmap_st_set_target_array(jcmap_state_t* state, void* target)
{
  state->map_target_array = (uint8_t*) target;
}

void jcmap_st_destroy(jcmap_state_t* state)
{
  free(state);
}

size_t jcmap_st_get_count(jcmap_state_t* state)
{
  return(state->count);
}

jcmap_status_t jcmap_st_get_status(jcmap_state_t* state)
{
  return(state->status);
}

void jcmap_st_set_aquire_type(jcmap_state_t* state, jcmap_target_t aquire_type)
{
  state->aquire_type = aquire_type;
}

void* jcmap_st_get_aquire(jcmap_state_t* state)
{
  return(state->aquire_result);
}

char* jcmap_st_value(jcmap_state_t* state)
{
  jcmap_type_t type;
  JCDEBUG_ENTER_FN();
  char* result;
   /*TODO: find some way to workerise jcmap - maybe better from main program? Or jcmap_async_api.h?*/
   /*detect type of value, then call state function for it*/
    type = jcmap_char_detect_type( *(state->buffer) );
    switch (type)
    {
      case j_object :
        result = jcmap_st_object(state);
        JCDEBUG_LEAVE_FN();
        return (result);
      case j_array :
        result = jcmap_st_array(state);
        JCDEBUG_LEAVE_FN();
        return (result);
      case j_string :
        result = jcmap_st_string(state);
        JCDEBUG_LEAVE_FN();
        return (result);
      case j_number :
        result = jcmap_st_number(state);
        JCDEBUG_LEAVE_FN();
        return (result);
      case j_keyword :
        result = jcmap_st_keyword(state);
        JCDEBUG_LEAVE_FN();
        return (result);
      default :
        INVALIDATE("value expected, none found");
    }
}

char* jcmap_st_value_sep(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*should be a value seperator here - detect character*/
    if ( jcmap_char_value_seperator(*(state->buffer)) )
    {
     /*advance buffer, return error if limit reached*/
      VALIDATE();
    } else {
      INVALIDATE("expected ','");
    }
}

char* jcmap_st_object(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
  char* str_err;
   /*update state struct*/
    state->object_depth ++;
    state->count = 0;

   /*validate the start*/
    str_err = jcmap_st_object_start(state);
    INVALID_RETURN(str_err);
    state->status = st_pending;  /*reset status*/
   
   /*advance buffer 1*/
    ADVANCE_BUFFER();
   
   /*skip any whitespace*/
    WHITESPACE_CLEAR();

   /*already at the end?*/
    str_err = jcmap_st_object_end(state);
    state->count = 0;  /*no items yet, so state's counter should be 0*/
    VALID_RETURN();
    if (NULL != str_err)  /*critical error*/
      INVALID_RETURN(str_err);
    LIMIT_RETURN();
    state->status = st_pending;  /*reset status*/

   /*call loop, status is now either valid or invalid, so just return direct*/
    str_err = jcmap_st_object_loop(state);

  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_object_start(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check 1st character*/
    if (! jcmap_char_object_start( *(state->buffer) ) )
      INVALIDATE("object: expected {");

  VALIDATE();
}

# ifndef MIN
#   define MIN(a, b) (((a) < (b)) ? (a) : (b))
# endif

char* jcmap_st_object_loop(jcmap_state_t* state)
{
  char* str_err;
  size_t count = 0;
  char* name;
  size_t name_count;
  char* name_raw;  /*TODO would b cleaner to save state than mess about with next 4 vars*/
  size_t name_limit;
  char* value_raw;
  size_t value_limit;
  int aquire_this = false;
  char* old_buffer;  /*TODO next 2 also not needed if state saved*/
  size_t old_limit;
  intptr_t map_type;
  intptr_t map_offset;
  char** tmp_ptr;  /*TODO move to other fn*/
  size_t i;
  jcmap_mode_t old_mode;
  JCDEBUG_ENTER_FN();
   /*loop*/
    while (true)
    {
     /*save location of name start, for mapping*/
      name_raw = state->buffer;
      name_limit = state->buffer_limit;

     /*validate name*/
      str_err = jcmap_st_object_loop_name(state);
      INVALID_RETURN(str_err);
      state->status = st_pending;

     /*if mapping, test name (memcmp)*/
      if ( st_map_object == state->mode)
      {
       /*save state*/
        old_buffer = state->buffer;
        old_limit = state->buffer_limit;

       /*set mode to aquire*/
        state->mode = st_aquire;
        state->aquire_type = c_string;
        state->aquire_result = NULL;
        state->aquire_buffer_limit = 0;
        state->buffer = name_raw;
        state->buffer_limit = name_limit;
        state->status = st_pending;

       /*aquire key name & length - needed because string and c variable names may have unicode*/
        str_err=jcmap_st_value(state);
        INVALID_RETURN(str_err);  /*if it didn't work, give up*/
        if (NULL == state->aquire_result)
          return("aquire failed");
        name = (char*)(state->aquire_result);
        name_count = state->count;

       /*restore state*/
        state->mode = st_map_object;
        state->buffer = old_buffer;
        state->buffer_limit = old_limit;
        state->status = st_pending;

       /*search thru map names*/
        for (i=0; i<state->map_size; i++)
        {
         /*log*/
          JCDEBUG_TESTING(name_raw, state->map[i].key);

         /*do sizes match*/
          if (name_count == state->map[i].key_size)
          {
           /*memcmp strings*/
            if ( 0 == memcmp(name, state->map[i].key, MIN( state->map[i].key_size, name_count) ) )
            {
             /*log*/
              JCDEBUG_MATCHED(state->map[i].key);

             /*on match, save offset and type*/
              aquire_this = true;
              map_type = state->map[i].type;
              map_offset = state->map[i].offset;
              break;
            }
          }
        }
        /*free aquired string*/
         free(name);
      }
     
     /*advance buffer 1*/
      ADVANCE_BUFFER();

     /*advance whitespace*/
      WHITESPACE_CLEAR();

     /*validate name seperator*/
      str_err = jcmap_st_object_loop_name_sep(state);
      INVALID_RETURN(str_err);
      state->status = st_pending;

     /*advance buffer 1*/
      ADVANCE_BUFFER();

     /*advance whitespace*/
      WHITESPACE_CLEAR();

     /*save location of value start, for mapping*/
      value_raw = state->buffer;
      value_limit = state->buffer_limit;

     /*validate value*/
      old_mode = state->mode;  /*switching to validate mode, so stash old mode*/
      state->mode = st_validate;
      str_err = jcmap_st_object_loop_value(state);
      INVALID_RETURN(str_err);
      state->status = st_pending;
      state->mode = old_mode;

     /*update count*/
      count++;

     /*if this value is getting mapped, aquire it*/
      if (aquire_this)
      {
        /*TODO: this should have it's own function*/
       /*log*/
        JCDEBUG_MAPPING(map_offset);

       /*save state*/
        old_buffer = state->buffer;
        old_limit = state->buffer_limit;

       /*set mode to aquire*/
        state->mode = st_aquire;
        state->aquire_type = map_type;
        state->aquire_result = NULL;
        state->aquire_buffer_limit = 0;
        state->buffer = value_raw;
        state->buffer_limit = value_limit;
        state->status = st_pending;

       /*if aquire successful, save to offset*/
        str_err=jcmap_st_value(state);
        INVALID_RETURN(str_err);  /*if it didn't work, give up*/
        if (NULL == state->aquire_result)
          return("aquire failed");
        tmp_ptr = (char**)(state->map_target_object + map_offset);
        *tmp_ptr = (char*)(state->aquire_result);

       /*restore state*/
        state->mode = st_map_object;
        state->buffer = old_buffer;
        state->buffer_limit = old_limit;
        state->status = st_pending;
        aquire_this = false;
      }

     /*advance buffer 1*/
      ADVANCE_BUFFER();

     /*advance whitespace*/
      WHITESPACE_CLEAR();

     /*end?*/
      str_err = jcmap_st_object_end(state);
      state->count = count;  /*only place we need to update state's counter, because it's the only valid exit point*/
      VALID_RETURN();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      LIMIT_RETURN();
      state->status = st_pending;

     /*validate value seperator*/
      str_err = jcmap_st_object_loop_value_sep(state);
      INVALID_RETURN(str_err);
      state->status = st_pending;

     /*advance buffer 1*/
      ADVANCE_BUFFER();

     /*advance whitespace*/
      WHITESPACE_CLEAR();
    }
}

char* jcmap_st_object_loop_name(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
  char* str_err = jcmap_st_string(state);  /*sets state, etc for us*/
  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_object_loop_name_sep(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*should be a value seperator here - detect character*/
    if ( jcmap_char_name_seperator(*(state->buffer)) )
    {
     /*advance buffer, return error if limit reached*/
      VALIDATE();
    } else {
      INVALIDATE("object: expected ':'");
    }
}

char* jcmap_st_object_loop_value(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
  char* str_err = jcmap_st_value(state);  /*sets state, etc for us*/
  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_object_loop_value_sep(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
  char* str_err = jcmap_st_value_sep(state);  /*sets state, etc for us*/
  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_object_end(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check 1st character*/
    if (! jcmap_char_object_end( *(state->buffer) ) )
      INVALIDATE(NULL);

  VALIDATE();
}

char* jcmap_st_array(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*update state struct*/
    state->array_depth ++;
    state->count = 0;

   /*validate start*/
    str_err = jcmap_st_array_start(state);
    INVALID_RETURN(str_err);
    state->status = st_pending;  /*reset status*/
   
   /*advance buffer 1*/
    ADVANCE_BUFFER();

   /*skip whitespace*/
    WHITESPACE_CLEAR();

   /*already ended?*/
    str_err = jcmap_st_array_end(state);
    state->count = 0;  /*no items yet, so state's counter should be 0*/
    VALID_RETURN();
    if (NULL != str_err)  /*critical error*/
      INVALID_RETURN(str_err);
    LIMIT_RETURN();
    state->status = st_pending;  /*reset status*/

   /*loop the loop*/
    str_err = jcmap_st_array_loop(state);

  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_array_start(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check 1st character*/
    if (! jcmap_char_array_start( *(state->buffer) ) )
      INVALIDATE("array: expected [");

  VALIDATE();
}

char* jcmap_st_array_loop(jcmap_state_t* state)
{
  char* str_err;
  size_t count = 0;
  JCDEBUG_ENTER_FN();
   /*loop*/
    while (true)
    {
     /*value*/
      str_err = jcmap_st_array_loop_value(state);
      INVALID_RETURN(str_err);
      state->status = st_pending;

     /*update count*/
      count++;

     /*advance buffer 1*/
      ADVANCE_BUFFER();

     /*clear whitespace*/
      WHITESPACE_CLEAR();

     /*end?*/
      str_err = jcmap_st_array_end(state);
      state->count = count;  /*only place we need to update state's counter, because it's the only valid exit point*/
      VALID_RETURN();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      state->status = st_pending;

     /*value seperator*/
      str_err = jcmap_st_array_loop_value_sep(state);
      INVALID_RETURN(str_err);
      state->status = st_pending;
   
     /*advance buffer 1*/
      ADVANCE_BUFFER();

     /*clear whitespace*/
      WHITESPACE_CLEAR();
    }
}

char* jcmap_st_array_loop_value(jcmap_state_t* state)
{
  jcmap_mode_t old_mode;
  JCDEBUG_ENTER_FN();
   /*save state*/
    old_mode = state->mode;
    
   /*should it be mapped to a struct?*/
    if ( (st_map_array == state->mode)
       &&(j_object == jcmap_char_detect_type( *(state->buffer) )) )
    {
     /*set up state to map to object*/
      state->mode = st_map_object;
      state->map_target_object = state->map_target_array;
    } else {
     /*switch to validate mode*/
      state->mode = st_validate;
    }

   /*do the value*/
    char* str_err = jcmap_st_value(state);  /*sets status, etc for us*/

   /*if mapping was done, increase map pointer*/
    if ( st_map_object == state->mode )
      state->map_target_array += (intptr_t)(state->map_target_size);

   /*restore state*/
    state->mode = old_mode;

  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_array_loop_value_sep(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
  char* str_err = jcmap_st_value_sep(state);  /*sets status, etc for us*/
  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_array_end(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check 1st character*/
    if (! jcmap_char_array_end( *(state->buffer) ) )
      INVALIDATE(NULL);

  VALIDATE();
}

char* jcmap_st_string(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*update state struct*/
    state->count = 0;
    state->status = st_pending;

   /*validate the start*/
    str_err = jcmap_st_string_start(state);
    INVALID_RETURN(str_err);
    state->status = st_pending;  /*reset status*/

   /*advance buffer 1*/
    ADVANCE_BUFFER();

   /*let the main loop run*/
    str_err = jcmap_st_string_loop(state);
    INVALID_RETURN(str_err);
 
  JCDEBUG_LEAVE_FN();
  return(NULL);
}

char* jcmap_st_string_start(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check 1st character*/
    if (! jcmap_char_string_start( *(state->buffer) ) )
      INVALIDATE("expected \"");

  VALIDATE();
}

char* jcmap_st_string_loop(jcmap_state_t* state)
{
  char* str_err;
  size_t count = 0;
  JCDEBUG_ENTER_FN();
   /*loop*/
    while (true)
    {
     /*end?*/
      str_err = jcmap_st_string_end(state);
      state->count = count;  /*update state's counter*/
      VALID_RETURN();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      LIMIT_RETURN();
      state->status = st_pending;

     /*escape sequence?*/
      state->count = 0;  /*reset state's counter*/
      str_err = jcmap_st_string_loop_escape(state);
      count += state->count;  /*only increased if a valid escape found*/
      VALID_ADVANCE_CONTINUE();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      LIMIT_RETURN();
      state->status = st_pending;
 
     /*valid character?*/
      state->count = 0;  /*reset state's counter*/
      str_err = jcmap_st_string_loop_character(state);
      INVALID_RETURN(str_err);
      count ++;  /*valid, and only possible to be 1*/
      ADVANCE_BUFFER();
      state->status = st_pending;
    }
}

char* jcmap_st_string_loop_escape(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*escape char?*/
    if (! jcmap_char_string_escape( *(state->buffer) ) )
    {
      state->count = 0;
      INVALIDATE(NULL);  /*NULL signifies it's not critically invalid*/
    }

   /*advance buffer 1*/
    ADVANCE_BUFFER();

   /*unicode? recurse, return*/
    str_err = jcmap_st_string_loop_escape_unicode(state);
    if (NULL != str_err)
      INVALID_RETURN(str_err);  /*malformed unicode escape*/
    state->count = 6;  /*no unicode translation yet*/
    VALID_RETURN();

   /*only 1 possible value for count now...*/
    state->count = 1;  /*aquire should change these to 1 char*/

   /*giant if of doom*/
   /*aquire mode needs different operations...*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (jcmap_char_escape_quote( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '"') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_backslash( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\\') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_forwardslash( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '/') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_backspace( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\b') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_formfeed( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\f') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_linefeed( *(state->buffer )))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\n') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_carriagereturn( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\r') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
      if (jcmap_char_escape_tab( *(state->buffer) ))
      {
        if (! jcmap_string_push_c((char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\t') )
          INVALIDATE("string_push_c failed");
        else
          VALIDATE();
      }
    } else {
      if (jcmap_char_escape_quote( *(state->buffer) ))
        VALIDATE();
  
      if (jcmap_char_escape_backslash( *(state->buffer) ))
        VALIDATE();
  
      if (jcmap_char_escape_forwardslash( *(state->buffer) ))
        VALIDATE();
  
      if (jcmap_char_escape_backspace( *(state->buffer) ))
        VALIDATE();
  
      if (jcmap_char_escape_formfeed( *(state->buffer) ))
        VALIDATE();
  
      if (jcmap_char_escape_linefeed( *(state->buffer )))
        VALIDATE();
  
      if (jcmap_char_escape_carriagereturn( *(state->buffer) ))
        VALIDATE();
  
      if (jcmap_char_escape_tab( *(state->buffer) ))
        VALIDATE();
    }

   /*no match? fail*/
  state->count = 0;
  INVALIDATE("string: invalid escape character");
}

char* jcmap_st_string_loop_escape_unicode(jcmap_state_t* state)
{
  uint16_t i;
  JCDEBUG_ENTER_FN();
   /*check limit - less than 4 and this is pointless*/
    if ( state->buffer_limit < 4 )
      INVALIDATE(NULL);  /*NULL signifies that it's not critically invalid*/

   /*check 1st character*/
    if (! jcmap_char_escape_unicode( *(state->buffer) ) )
      INVALIDATE(NULL);  /*NULL signifies that it's not critically invalid*/

   /*stash pointer to hexcode start*/
     /*TODO*/

   /*check next 4 are hex*/
    for (i=0; i<4; i++)
    {
      ADVANCE_BUFFER();
      if (! jcmap_char_escape_unicode_hex( *(state->buffer) ) )
        INVALIDATE("string: unicode escape: expected hexadecimal digit");  /*CRITICALLY invalid*/
    }

   /*if in aquire mode...*/
     /*TODO*/
     /*test hexcode validity*/
     /*make hexcode into utf8 string*/
     /*push string*/

  state->count = 6;
  VALIDATE();
}

char* jcmap_st_string_loop_character(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*proper character?*/
    if (! jcmap_char( *(state->buffer) ) )
      INVALIDATE("valid utf8 json character expected");

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), *(state->buffer) ) )
        INVALIDATE("string_push_c failed");
    }

  VALIDATE();  
}

char* jcmap_st_string_end(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check character*/
    if (! jcmap_char_string_end( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, make sure there's a string to send back*/
    if (( (NULL == state->aquire_result)
         && (st_aquire == state->mode) )
        && (c_string == state->aquire_type))
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), '\0') )
        INVALIDATE("string_push_c failed");
    }

  VALIDATE();
}

char* jcmap_st_number(jcmap_state_t* state)
{
  char *str_err;
  JCDEBUG_ENTER_FN();
   /*update state struct*/
    state->decimal = false;
    state->count = 0;
    state->status = st_pending;

   /*check it starts right*/
    str_err = jcmap_st_number_start(state);
    INVALID_RETURN(str_err);
    state->status = st_pending;

   /*advance*/
    ADVANCE_BUFFER();

   /*main loop*/
    str_err = jcmap_st_number_loop(state);
  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_number_start(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*-?*/
    str_err = jcmap_st_number_start_minus(state);
    /*why no VALID_RETURN() here? Because following a minus MUST be either a 0, or 1-9...*/
    if (NULL != str_err)  /*critical error*/
      INVALID_RETURN(str_err);
    state->status = st_pending;
     
   /*0?*/
    str_err = jcmap_st_number_start_zero(state);
    VALID_RETURN();
    if (NULL != str_err)  /*critical error*/
      INVALID_RETURN(str_err);
    state->status = st_pending;
     
   /*1-9?*/
    str_err = jcmap_st_number_start_one_nine(state);
    VALID_RETURN();
    if (NULL != str_err)  /*critical error*/
      INVALID_RETURN(str_err);
    state->status = st_pending;
     
   /*invalidate!*/
  state->count = 0;
  INVALIDATE("expected 1-9, 0., or -");
}

char* jcmap_st_number_start_minus(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match chacter?*/  
    if (! jcmap_char_number_minus( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), '-' ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = false;
  ADVANCE_BUFFER();  /*MUST be something after this, so advance buffer*/
  state->count ++;
  VALIDATE();
}

char* jcmap_st_number_start_zero(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*1st character?*/
    if (! jcmap_char_number_zero( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), '0' ) )
        INVALIDATE("string_push_c failed");
    }

   /*advance buffer & update state*/
    state->can_end = true;
    state->count ++;
    ADVANCE_BUFFER();

   /*check 0 is followed by a decimal point*/
    str_err = jcmap_st_number_start_zero_decimal( state );  /*will add to count*/
    INVALID_RETURN(strdup("0 at start of number must be followed by '.'"));

  state->can_end = false;
  VALIDATE();
}

char* jcmap_st_number_start_zero_decimal(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
  char* str_err = jcmap_st_number_loop_decimal(state);  /*sets state, count, etc for us*/
  state->can_end = false;
  JCDEBUG_LEAVE_FN();
  return(str_err);
}

char* jcmap_st_number_start_one_nine(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match character*/
    if (! jcmap_char_number_one_nine( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), *(state->buffer) ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = true;
  state->count ++;
  VALIDATE();
}

char* jcmap_st_number_loop(jcmap_state_t* state)
{
  char* str_err;
  size_t count = state->count;  /*already set by start function*/
  JCDEBUG_ENTER_FN();
  /*TODO: implement counting - only exp side left*/
   /*main loop*/
    while(true)
    {
     /*decimal point?*/
      str_err = jcmap_st_number_loop_decimal(state);
      VALID_ADVANCE_CONTINUE();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      state->status = st_pending;
 
     /*EXP start? ('e' or 'E')*/
      str_err = jcmap_st_number_loop_exp(state);
      VALID_RETURN();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      state->status = st_pending;

     /*digit?*/
      str_err = jcmap_st_number_loop_digit(state);
      VALID_ADVANCE_CONTINUE();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      state->status = st_pending;

     /*valid ending?*/
      str_err = jcmap_st_number_end(state);
      JCDEBUG_LEAVE_FN();
      return(str_err);
    }
}

char* jcmap_st_number_loop_decimal(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*check char*/
    if (! jcmap_char_number_decimal(*(state->buffer)) )
      INVALIDATE(NULL);

   /*decimal already set?*/
    if ( true == state->decimal )
      INVALIDATE("two decimal points not allowed");
   
   /*set decimal*/
    state->decimal = true;

   /*update counter*/
    state->count ++;
 
   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), '.' ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = false;
  VALIDATE();
}

char* jcmap_st_number_loop_digit(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match char?*/
    if (! jcmap_char_number_digit( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*update counter*/
    state->count ++;
 
   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), *(state->buffer) ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = true;
  VALIDATE();
}

char* jcmap_st_number_loop_exp(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*check 1st char(s)*/
    str_err = jcmap_st_number_loop_exp_start(state);
    INVALID_RETURN(str_err);
    state->status = st_pending;

   /*update counter*/
    state->count ++;
 
   /*advance 1 char*/
    ADVANCE_BUFFER();

   /*loop*/
    str_err = jcmap_st_number_loop_exp_loop(state);
    INVALID_RETURN(str_err);
  
  JCDEBUG_LEAVE_FN();
  return(NULL);
}

char* jcmap_st_number_loop_exp_start(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*is there an e?*/
    if ( ! jcmap_char_number_exp_start( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), 'e' ) )
        INVALIDATE("string_push_c failed");
    }

   /*update state and advance 1 char*/
    state->can_end = false;
    state->count ++;
    ADVANCE_BUFFER();

   /*sign?*/
    str_err = jcmap_st_number_loop_exp_start_sign(state);
    /*no valid return here - sign MUST be followed by digit anyway...*/
    if (NULL != str_err)  /*critical error*/
      INVALID_RETURN(str_err);
    state->status = st_pending;  /*reset status*/
    
   /*digit?*/
    str_err = jcmap_st_number_loop_exp_start_digit(state);
    INVALID_RETURN(str_err);
    state->status = st_pending;  /*reset status*/

  state->can_end = true;
  VALIDATE();
}

char* jcmap_st_number_loop_exp_start_sign(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match char?*/
    if (! jcmap_char_number_exp_plus_minus( *(state->buffer) ) )
    {
      INVALIDATE(NULL);
    }

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), '-' ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = false;
  state->count ++;
  ADVANCE_BUFFER();
  VALIDATE();
}

char* jcmap_st_number_loop_exp_start_digit(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match char?*/
    if (! jcmap_char_number_digit( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), *(state->buffer) ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = true;
  VALIDATE();
}

char* jcmap_st_number_loop_exp_loop(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
   /*loop*/
    while(true)
    {
     /*valid digit?*/
      str_err = jcmap_st_number_loop_digit(state);
      VALID_ADVANCE_CONTINUE();
      if (NULL != str_err)  /*critical error*/
        INVALID_RETURN(str_err);
      LIMIT_VALID();
      state->status = st_pending;

     /*valid end?*/
      str_err = jcmap_st_number_loop_exp_end(state);
      VALID_RETURN();
      INVALID_RETURN(str_err);
    }
}

char* jcmap_st_number_loop_exp_loop_digit(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match char?*/
    if (! jcmap_char_number_digit( *(state->buffer) ) )
      INVALIDATE(NULL);

   /*if in aquire mode, push*/
    if ( (st_aquire == state->mode) 
      && (c_string == state->aquire_type) )
    {
      if (! jcmap_string_push_c( (char**)&(state->aquire_result), &(state->aquire_buffer_limit), *(state->buffer) ) )
        INVALIDATE("string_push_c failed");
    }

  state->can_end = true;
  VALIDATE();
}

char* jcmap_st_number_loop_exp_end(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match char?*/
    if ( ! jcmap_char_number_after( *(state->buffer) ) )
      INVALIDATE("end of number, expected ' ', ',', ], or }");

   /*are we able to end here?*/
    if ( true != state->can_end )
      INVALIDATE("end not syntactically valid");

   /*shift buffer back one*/
    jcmap_buffer_prev( &(state->buffer), &(state->buffer_limit) );
  VALIDATE();  
}

char* jcmap_st_number_end(jcmap_state_t* state)
{
  JCDEBUG_ENTER_FN();
   /*match char?*/
    if ( ! jcmap_char_number_after( *(state->buffer) ) )
      INVALIDATE("end of number, expected ' ', ',', ], or }");
   /*shift buffer back one*/
    jcmap_buffer_prev( &(state->buffer), &(state->buffer_limit) );
  VALIDATE();  
}

char* jcmap_st_keyword(jcmap_state_t* state)
{
  char* str_err;
  JCDEBUG_ENTER_FN();
  size_t i;
   /*update state struct*/
    state->count = 0;
    state->status = st_pending;

   /*limit less than 3?*/
    if (state->buffer_limit < 3)
      INVALIDATE("buffer_limit exceeded");

   /*raw aquire? then set pointer here*/
    /*TODO*/

   /*memcmp w/ true*/
    if ( 0 == memcmp(state->buffer, "true", 4))
    {
      for ( i=0; i<3; i++)
      {
        ADVANCE_BUFFER();
      }
      state->count = 4;

     /*aquire logic*/
      if ( (st_aquire == state->mode) && (c_string == state->aquire_type) )
      {
        state->aquire_result = (void*)strdup("true");
        if (NULL == state->aquire_result)
          INVALIDATE("enomem");
        state->aquire_buffer_limit = 4;
      }

     /*all good - validate*/
      VALIDATE();
    }

   /*memcmp w/ null*/
    if ( 0 == memcmp(state->buffer, "null", 4))
    {
      for ( i=0; i<3; i++)
      {
        ADVANCE_BUFFER();
      }
      state->count = 4;

     /*aquire logic*/
      if ( (st_aquire == state->mode) && (c_string == state->aquire_type) )
      {
        state->aquire_result = (void*)strdup("null");
        if (NULL == state->aquire_result)
          INVALIDATE("enomem");
        state->aquire_buffer_limit = 4;
      }

     /*all good - validate*/
      VALIDATE();
    }

   /*limit less than 4?*/
    if (state->buffer_limit < 4)
      INVALIDATE("buffer_limit exceeded");

   /*memcmp w/ false*/
    if ( 0 == memcmp(state->buffer, "false", 5))
    {
      for ( i=0; i<4; i++)
      {
        ADVANCE_BUFFER();
      }
      state->count = 5;

     /*aquire logic*/
      if ( (st_aquire == state->mode) && (c_string == state->aquire_type) )
      {
        state->aquire_result = (void*)strdup("false");
        if (NULL == state->aquire_result)
          INVALIDATE("enomem");
        state->aquire_buffer_limit = 5;
      }

     /*all good - validate*/
      VALIDATE();
    }

   /*invalid!*/
  INVALIDATE("expected: true, false, or null");
}

