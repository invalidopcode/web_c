/*json_string_test.c - element to test json mapper

         Licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#   ifndef WEBC_STDIO
#include <emscripten.h>
#include <emscripten/fetch.h>
#   endif
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <jsc.h>
#include <jcmap.h>
#include "json_test.h"
#include "json_unit_test.h"

 const char class_name[] = "json_string_test";

int main(int argc, const char ** argv)
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
    jsc_el_clear(el);
   
   /*write data to elements*/
    jsc_el_appendf(el, "<h1>JSON string test</h1>\n");

   /*test validation*/
    jsc_el_appendf(el, "<h2>Testing validate()...</h2>\n");
    test_json_string_valid(el);

   /*test counting*/
    jsc_el_appendf(el, "<h2>Testing count()...</h2>\n");
    test_json_string_count(el);

   /*test aquire*/
    jsc_el_appendf(el, "<h2>Testing aquire()...</h2>\n");
    test_json_string_aquire(el);

  jsc_el_write(el);
  jsc_el_finish(el);
}

void test_json_string_valid(jsc_el_t* el)
{
  uint8_t* buffer;
  size_t buffer_limit;
  char* data_random = "blah\"a\"";
  char* data_empty = "\"\"";
  char* data_empty_buffer = "";
  char* data_quote = "\"";
  char* data_simple_str = "\"blah blah;{blah{[][#@\"";
  char* data_simple_str_no_quote = "\"blah blah;{blah{[][#@";
  char* data_escape_chars = "\" \\\" \\\\ \\/ \\b \\f \\n\\r \\t \"";
  char* data_unicode = "\"abc\\u2020abc\"";
  char* data_invalid_escape_char = "\"abc\\>abc\"";
  char* data_long;
  size_t i;
   /*simple unit tests*/
    UNIT_TEST_INVALID( data_random );
    UNIT_TEST_VALID( data_empty );
    UNIT_TEST_INVALID( data_empty_buffer );
    UNIT_TEST_INVALID( data_quote );
    UNIT_TEST_VALID( data_simple_str );
    UNIT_TEST_INVALID( data_simple_str_no_quote );
    UNIT_TEST_VALID( data_escape_chars );
    UNIT_TEST_VALID( data_unicode );
    UNIT_TEST_VALID( "\"\\u1111\"" );
    UNIT_TEST_INVALID( data_invalid_escape_char );
    /*TODO: ?utf8 string?*/

   /*error condition - buffer limit 1 less than it should be*/
    UNIT_TEST_START();
    UNIT_TEST_LABEL( "-> low buffer limit");

    buffer = (uint8_t*)( data_simple_str );
    buffer_limit = strlen( data_simple_str) - 2;  /*size->limit means -1, -2 because we want the buffer limit too low for test*/

    if ( ! jcmap_json_validate((uint8_t*)((void*)buffer), buffer_limit) )
      jsc_el_appendf(el, "<span class='test_success'>invalid</span>\n");
    else
      jsc_el_appendf(el, "<span class='test_error'>valid</span>\n");

    UNIT_TEST_END();

   /*very long string*/
    UNIT_TEST_START();
    UNIT_TEST_LABEL("-> very long string");

    data_long = calloc(5000, sizeof(char));
    *data_long = '\"';
    data_long ++;
    for (i=1; i<4998; i++)
    {
      *data_long = '@';
      data_long ++;
    }
    *data_long = '\"';
    data_long ++;
    *data_long = '\0';
    data_long -= 4999;

    buffer = (uint8_t*)( data_long );
    buffer_limit = strlen( data_long) - 1;  /*size->limit means -1*/

    if ( jcmap_json_validate((uint8_t*)((void*)buffer), buffer_limit) )
      UNIT_TEST_SUCCESS("valid");
    else
      UNIT_TEST_FAIL("invalid");

    UNIT_TEST_END();
}

void test_json_string_count(jsc_el_t* el)
{
   /*simple unit tests*/
    UNIT_TEST_COUNT( "\"stringstring\"", 12);
    UNIT_TEST_COUNT( "\"string\\nstring\"", 13);
    UNIT_TEST_COUNT( "\"\\u1111\"", 6);
    UNIT_TEST_COUNT( "\"\\\\\"", 1);
    UNIT_TEST_COUNT( "\"string\\\\\\u0000string\"", 19);
}

void test_json_string_aquire(jsc_el_t* el)
{
   /*simple unit tests*/
    UNIT_TEST_AQUIRE_STRING( "\"\"", "" );
    UNIT_TEST_AQUIRE_STRING( "\"abc\"", "abc" );
    UNIT_TEST_AQUIRE_STRING( "\"123abc\"", "123abc" );
    UNIT_TEST_AQUIRE_STRING( "\"aaaaaaaaaaaaaaa\"",
                             "aaaaaaaaaaaaaaa" );
    UNIT_TEST_AQUIRE_STRING( "\"aaaaaaaaaaaaaaaa\"",
                             "aaaaaaaaaaaaaaaa" );
    UNIT_TEST_AQUIRE_STRING( "\"aaaaaaaaaaaaaaaaa\"",
                             "aaaaaaaaaaaaaaaaa" );
    UNIT_TEST_AQUIRE_STRING( "\"aaaaaaaaaaaaaaaaaaaa\"",
                             "aaaaaaaaaaaaaaaaaaaa" );
    UNIT_TEST_AQUIRE_STRING( "\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"",
                               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" );
    UNIT_TEST_AQUIRE_STRING( "\"ABC\"", "ABC" );
    UNIT_TEST_AQUIRE_STRING( "\"!£$%^&*()[]{}?@;\"", "!£$%^&*()[]{}?@;" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\\\ 123\"", "abc \\ 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\/ 123\"", "abc / 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\b 123\"", "abc \b 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\f 123\"", "abc \f 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\n 123\"", "abc \n 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\r 123\"", "abc \r 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\t 123\"", "abc \t 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\\" 123\"", "abc \" 123" );
    UNIT_TEST_AQUIRE_STRING( "\"abc \\\" \\\\ \\/ \\b \\f \\n \\r \\t123\"", "abc \" \\ / \b \f \n \r \t123" );
    UNIT_TEST_AQUIRE_STRING( "\"\\u20AC\"", "€" );
}

