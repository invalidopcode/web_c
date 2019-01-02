/*json_array_test.c - element to test json mapper

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

 const char class_name[] = "json_array_test";

int main(int argc, const char ** argv)
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
    jsc_el_clear(el);
   
   /*write data to elements*/
    jsc_el_appendf(el, "<h1>JSON array test</h1>\n");

   /*test array validity*/
    jsc_el_appendf(el, "<h2>Testing validate()...</h2>\n");
    test_json_array_valid(el);

   /*test counting*/
    jsc_el_appendf(el, "<h2>Testing count()...</h2>\n");
    test_json_array_count(el);

   /*test array map*/
    jsc_el_appendf(el, "<h2>Testing map()...</h2>\n");
    test_json_array_map(el);

  jsc_el_write(el);
  jsc_el_finish(el);
}

void test_json_array_valid(jsc_el_t* el)
{
   /*basic error conditions*/
    UNIT_TEST_INVALID( "[" );
    UNIT_TEST_INVALID( "]" );

   /*basic unit tests*/
    UNIT_TEST_VALID( "[]" );
    UNIT_TEST_VALID( "[\"a\",\"b\",\"c\"]" );
    UNIT_TEST_VALID( "[\"json\", \"test\", \n\n\"array\"\n]" );
    UNIT_TEST_VALID( "[ [],[],[] ]");
    UNIT_TEST_VALID( "[ [[1,2,3],[1,2,3],[1,2]],[[5,6],[],[]],[] ]");

   /*TODO invalid: buffer limit*/
}

void test_json_array_count(jsc_el_t* el)
{
   /*basic array*/
    UNIT_TEST_COUNT( "[\"json\", \"test\", \"array\"]", 3 );
    UNIT_TEST_COUNT( "[1,2,3,4,5]", 5);
    UNIT_TEST_COUNT( "[[],[]]", 2);
    UNIT_TEST_COUNT( "[\",[],\",[],[],\"a\"]", 4);
    UNIT_TEST_COUNT( "[{\"a\":[1,2]},{},{}]", 3);

   /*nested array 1*/

   /*nested array 2*/

   /*invalid array*/

   /*TODO buffer limit*/
}

 void test_json_array_map(jsc_el_t* el)
{
  struct s_animal {
    char* name;
    char* age;
    char* type;
  };
  struct s_animal *the_animals;
  int test_state;

   /*set up array to map into*/
    the_animals = calloc(3, sizeof(struct s_animal));

   /*try a simple map*/
    UNIT_TEST_MAP_ARR_START( &test_state,
                             "[ {\"name\":\"scruffy\", \"age\":1,\"type\":\"woofer\"},"
                             "  {\"name\":\"meow\", \"age\":2,\"type\":\"kat\"},"
                             "  {\"name\":\"muncher\", \"age\":3,\"type\":\"fluffsack\"} ]",
                             the_animals, sizeof(struct s_animal),
                             JCMAP_STR(s_animal, name),
                             JCMAP_STR(s_animal, age),
                             JCMAP_STR(s_animal, type),
                             JCMAP_END()
                            );
    UNIT_TEST_TABLE_START();
    UNIT_TEST_COLUMN_START();
    UNIT_TEST_ROW( "name" );
    UNIT_TEST_ROW( "age" );
    UNIT_TEST_ROW( "type" );
    UNIT_TEST_COLUMN_END();
    UNIT_TEST_COLUMN_START();
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[0].name, "scruffy");
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[0].age, "1");
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[0].type, "woofer");
    UNIT_TEST_COLUMN_END();
    UNIT_TEST_COLUMN_START();
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[1].name, "meow");
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[1].age, "2");
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[1].type, "kat");
    UNIT_TEST_COLUMN_END();
    UNIT_TEST_COLUMN_START();
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[2].name, "muncher");
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[2].age, "3");
    UNIT_TEST_ROW_STRCMP( &test_state, the_animals[2].type, "fluffsack");
    UNIT_TEST_COLUMN_END();
    UNIT_TEST_TABLE_END();
    UNIT_TEST_MAP_END( );

}

