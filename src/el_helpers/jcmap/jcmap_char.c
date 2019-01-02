/*jcmap_char.c - detect characters for jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#include <ctype.h>
#include <stdbool.h>
#include "jcmap_char.h"

int jcmap_char_whitespace(char character)
{
  if ( (' ' == character)
     ||( ('\t' == character)
       ||( ('\n' == character)
         ||('\r' == character)
         )
       )
     )
    return (true);
  else
    return (false);
}
int jcmap_char_name_seperator(char character)
{
  if ( ':' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_value_seperator(char character)
{
  if ( ',' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_object_start(char character)
{
  if ( '{' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_object_end(char character)
{
  if ( '}' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_array_start(char character)
{
  if ( '[' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_array_end(char character)
{
  if ( ']' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_string_start(char character)
{
  if ( '"' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_string_escape(char character)
{
  if ( '\\' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_quote(char character)
{
  if ( '"' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_backslash(char character)
{
  if ( '\\' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_forwardslash(char character)
{
  if ( '/' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_backspace(char character)
{
  if ( 'b' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_formfeed(char character)
{
  if ( 'f' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_linefeed(char character)
{
  if ( 'n' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_carriagereturn(char character)
{
  if ( 'r' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_tab(char character)
{
  if ( 't' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_unicode(char character)
{
  if ( 'u' == character )
    return (true);
  else
    return (false);
}

int jcmap_char_escape_unicode_hex(char character)
{
  return ( isxdigit(character) );
}

int jcmap_char(char character)
{
  return ( character != '\0' );
}

int jcmap_char_string_end(char character)
{
  return ( jcmap_char_string_start(character) );
}

int jcmap_char_number_start(char character)
{
  if ( isdigit(character)
     ||('-' == character) )
    return (true);
  else
    return (false);
}

int jcmap_char_number_zero(char character)
{
  return ( '0' == character );
}

int jcmap_char_number_decimal(char character)
{
  return ( '.' == character);
}

int jcmap_char_number_minus(char character)
{
  return ( '-' == character);
}

int jcmap_char_number_one_nine(char character)
{
  return ( ('0'!=character) && isdigit(character) );
}

int jcmap_char_number_exp_start(char character)
{
  if ( ('e' == character)
     ||('E' == character) )
    return (true);
  else
    return (false);
}

int jcmap_char_number_exp_plus_minus(char character)
{
  if ( ('+' == character)
     ||('-' == character) )
    return (true);
  else
    return (false);
}

int jcmap_char_number_exp_after(char character)
{
  return (jcmap_char_number_after(character));
}

int jcmap_char_number_digit(char character)
{
  return ( isdigit(character) );
}

int jcmap_char_number_after(char character)
{
  if ( jcmap_char_whitespace(character)
     ||( jcmap_char_value_seperator(character)
       ||( jcmap_char_object_end(character)
         ||jcmap_char_array_end(character)
         )
       )
     )
    return (true);
  else
    return (false);
}

int jcmap_char_true_start(char character)
{
  return ( 't' == character );  /*only keyword that starts with t...*/
}

int jcmap_char_false_start(char character)
{
  return ( 'f' == character );
}

int jcmap_char_null_start(char character)
{
  return ( 'n' == character );
}

jcmap_type_t jcmap_char_detect_type(char character)
{
  if (jcmap_char_object_start(character))
    return (j_object);
  if (jcmap_char_array_start(character))
    return (j_array);
  if (jcmap_char_string_start(character))
    return (j_string);
  if (jcmap_char_number_start(character))
    return (j_number);
  if ( jcmap_char_true_start(character)
     ||(jcmap_char_false_start(character)
       ||jcmap_char_null_start(character)
       )
     )
    return(j_keyword);
  return(j_none);
}

