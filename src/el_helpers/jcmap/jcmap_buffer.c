/*jcmap_buffer.c - functions for jcmap buffer handling
         Licensed under cc0 (yes, this is different to everything else!)*/
#include <stdbool.h>
#include "jcmap_buffer.h"
#include "jcmap_char.h"
#include "jcmap_debug.h"

int jcmap_buffer_next(char* *buffer, size_t* buffer_limit)
{
 JCDEBUG_ENTER_FN();
   /*check buffer_limit*/
    if ( *buffer_limit == 0 )
    {
      JCDEBUG_ERR_FN("buffer_limit exceeded");
      return(false);
    }

   /*increase values*/
    (*buffer) ++;
    (*buffer_limit) --;
    JCDEBUG_NEXT_CHAR(**buffer);

  JCDEBUG_LEAVE_FN();
  return (true);
}

int jcmap_buffer_prev(char* *buffer, size_t* buffer_limit)
{
  JCDEBUG_ENTER_FN();
   /*nothing to check, just set values and exit*/
    (*buffer) --;
    (*buffer_limit) ++;
    JCDEBUG_PREV_CHAR(**buffer);

  JCDEBUG_LEAVE_FN();
  return (true);
}

int jcmap_buffer_whitespace(char* *buffer, size_t* buffer_limit)
{
  JCDEBUG_ENTER_FN();

  while ( (0 < *buffer_limit)
        &&(jcmap_char_whitespace(**buffer)) )
  {
    jcmap_buffer_next(buffer, buffer_limit);
  }

  if ( (0 == *buffer_limit) && jcmap_char_whitespace(**buffer) )  /*at buffer limit and still in whitespace*/
  {
    JCDEBUG_ERR_FN("buffer_limit reached");
    return(false);
  } else {
    JCDEBUG_LEAVE_FN();
    return(true);
  }
}

