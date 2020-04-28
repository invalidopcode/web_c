/*jcmap_string.c - function for building c string
         Licensed under cc0 (yes, this is different to everything else!)*/
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "jcmap_debug.h"
#include "jcmap_string.h"

 #include <stdio.h>

int jcmap_string_push_c(char* *str, size_t *limit, char c)
{
  char *position;
  char *tmp_str;
  JCDEBUG_ENTER_FN();
   /*null pointer? init str*/
    if (NULL == *str)
    {
      *str = malloc(16);
      if (NULL == str)
      {
        JCDEBUG_LEAVE_FN();
        return (false);
      }
      **str = '\0';
      *limit = 15;
    }

   /*search for end of string*/
    position = memchr(*str, '\0', *limit + 1);
    if (NULL == position)
    {
      JCDEBUG_LEAVE_FN();
      return (false);
    }

   /*check limit not reached*/
    if ((position - *str) == *limit)
    {
      tmp_str = realloc(*str, *limit + 1 + 32);  /*increase limit by 32*/
      if (NULL == tmp_str)
      {
        JCDEBUG_LEAVE_FN();
        return (false);
      }
      *str = tmp_str;
      *limit += 32;
    }

   /*write the new character*/
    tmp_str = position;
    *tmp_str = c;
    tmp_str++;
    *tmp_str = '\0';
  JCDEBUG_LEAVE_FN();
  return (true);
}

int jcmap_string_push_str(char** str, size_t* limit, char* push_str)
{
  char *position;
  char *tmp_str;
  size_t push_str_limit;
   /*null pointer? init str*/
    if (NULL == *str)
    {
      *str = malloc(16);
      if (NULL == str)
        return (false);
      **str = '\0';
      *limit = 16;
    }

   /*search for end of string*/
    position = memchr(*str, '\0', *limit + 1);
    if (NULL == position)
      return (false);

   /*check limit not reached*/
    push_str_limit = strlen(push_str) - 1;
    if (( (position - *str) + push_str_limit ) >= *limit)
    {
      tmp_str = realloc(tmp_str, *limit + 1 + push_str_limit + 32);  /*increase limit by strlen + 32*/
      if (NULL == tmp_str)
        return (false);
      *str = tmp_str;
    }

   /*write the new string*/
    strcpy(tmp_str, push_str);
  return(true);
}

