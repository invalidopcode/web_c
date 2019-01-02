/*json_keyword_test.c - element to test json mapper

         Licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <jsc.h>
#include <jcmap.h>
#include "json_test.h"
#include "json_unit_test.h"

 const char class_name[] = "json_keyword_test";

int main(int argc, const char ** argv)
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
    jsc_el_clear(el);
   
   /*write data to elements*/
    jsc_el_appendf(el, "<h1>JSON keyword test</h1>\n");

   /*test array validity*/
    jsc_el_appendf(el, "<h2>Testing validate()...</h2>\n");
    test_json_keyword_valid(el);

   /*test counting*/
    jsc_el_appendf(el, "<h2>Testing count()...</h2>\n");
    test_json_keyword_count(el);

   /*test array map*/
    jsc_el_appendf(el, "<h2>Testing aquire()...</h2>\n");
    test_json_keyword_aquire(el);

  jsc_el_write(el);
  jsc_el_finish(el);
}

void test_json_keyword_valid(jsc_el_t* el)
{
  uint8_t* buffer;
  size_t buffer_limit;
  char* data_true = "true";
  char* data_false = "false";
  char* data_null = "null";
  char* data_invalid_1 = "TRUE";
  char* data_invalid_2 = "kfalse";
  char* data_invalid_3 = "falsse";
   /*basic unit tests*/
    UNIT_TEST_VALID( data_true );
    UNIT_TEST_VALID( data_false );
    UNIT_TEST_VALID( data_null );
    UNIT_TEST_INVALID( data_invalid_1 );
    UNIT_TEST_INVALID( data_invalid_2 );
    UNIT_TEST_INVALID( data_invalid_3 );

   /*error condition - limit-1*/
    UNIT_TEST_START();
    UNIT_TEST_LABEL( "-> low buffer limit");
    buffer = (uint8_t*)( data_false );
    buffer_limit = strlen(data_false) - 2;  /*size->limit means -1*/
    
    if ( jcmap_json_validate((uint8_t*)((void*)buffer), buffer_limit) )
      UNIT_TEST_FAIL( "VALID" );
    else
      UNIT_TEST_SUCCESS( "invalid");
}

void test_json_keyword_count(jsc_el_t* el)
{
   /*simple unit tests*/
    UNIT_TEST_COUNT( "null", 4);
    UNIT_TEST_COUNT( "true", 4);
    UNIT_TEST_COUNT( "false", 5);
}

void test_json_keyword_aquire(jsc_el_t* el)
{
  /*simple unit tests*/
    UNIT_TEST_AQUIRE_STRING( "null", "null");
    UNIT_TEST_AQUIRE_STRING( "true", "true");
    UNIT_TEST_AQUIRE_STRING( "false", "false");
}

