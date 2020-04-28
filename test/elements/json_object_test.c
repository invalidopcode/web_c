/*json_object_test.c - element to test json mapper

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

 const char class_name[] = "json_object_test";

int main(int argc, const char ** argv)
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
    jsc_el_clear(el);
   
   /*write data to elements*/
    jsc_el_appendf(el, "<h1>JSON object test</h1>\n");

   /*test array validity*/
    jsc_el_appendf(el, "<h2>Testing validate()...</h2>\n");
    test_json_object_valid(el);

   /*test counting*/
    jsc_el_appendf(el, "<h2>Testing count()...</h2>\n");
    test_json_object_count(el);

   /*test array map*/
    jsc_el_appendf(el, "<h2>Testing map()...</h2>\n");
    test_json_object_map(el);

  jsc_el_write(el);
  jsc_el_finish(el);
}

void test_json_object_valid(jsc_el_t* el)
{
   /*simple error conditions*/
    UNIT_TEST_INVALID( "{" );
    UNIT_TEST_INVALID( "}" );
    UNIT_TEST_INVALID( "}{" );
    UNIT_TEST_INVALID( "}{\"a\":\"b\"}" );
    UNIT_TEST_INVALID( "abc{\"a\":\"b\"}" );
    UNIT_TEST_INVALID( "{a}" );
    UNIT_TEST_INVALID( "{a:}" );
    UNIT_TEST_INVALID( "{a:b}" );
    UNIT_TEST_INVALID( "{a : b}" );
    UNIT_TEST_INVALID( "{\"a}" );
    UNIT_TEST_INVALID( "{\"a\"}" );
    UNIT_TEST_INVALID( "{\"a\":}" );
    UNIT_TEST_INVALID( "{\"a\":\"b}" );
    UNIT_TEST_INVALID( "{\"a\":\"b\",}" );
    UNIT_TEST_INVALID( "{\"a\":\"b\"\"c\":\"d\"}" );
    UNIT_TEST_INVALID( "{\"a\":\"b\",\"c\":\"d\",}" );
    UNIT_TEST_INVALID( "{\"a\":\"b\",\"b\":{}" );
    UNIT_TEST_INVALID( "{\"a\":\"b\",\"b\":[}]}" );

   /*simple unit tests*/
    UNIT_TEST_VALID( "{}" );
    UNIT_TEST_VALID( "{\"a\":\"b\"}" );
    UNIT_TEST_VALID( "{\"a\":\"b\",\"c\":\"d\"}" );
    UNIT_TEST_VALID( "{\"a\":\"b\",\"b\":{}}" );
    UNIT_TEST_VALID( "{\"a\":\"b\",\"b\":\"}\"}" );
    UNIT_TEST_VALID( "{\"a\":\"b\",\"b\":\"{\"}" );
    UNIT_TEST_VALID( "{\"a\":\"b\",\"b\":{\"a\":\"b\", \"b\":\"c\"}}}" );
    UNIT_TEST_VALID( "{\"a\":\"b\",\"b\":{\"a\":\"b\", \"b\":\"c\", \"c\":\"d\"},\"c\":\"d\"}}" );
    UNIT_TEST_VALID( "{\"a\":{\"a\":\"b\",\"b\":[]},\"b\":{\"a\":\"b\", \"b\":\"c\", \"c\":\"d\"},\"c\":\"d\"}}" );
    UNIT_TEST_VALID( "{\"a\":{\"a\":\"b\",\"b\":[1,2,3,4,5]},\"b\":{\"a\":\"b\", \"b\":\"c\", \"c\":\"d\"},\"c\":\"d\"}}" );
    UNIT_TEST_VALID( "{\"a\":{\"a\":\"b\",\"b\":[{\"a\":\"b\",\"b\":\"c\"},{\"a\":\"b\", \"b\":\"c\"},   {},{},{}]},\"b\":{\"a\":\"b\", \"b\":\"c\", \"c\":\"d\"}, \"c\":\"d\"}}" );
    UNIT_TEST_VALID( "{\"a\":{\"a\":\n\"b\",\n\n\"b\":[{\"a\":\"b\",\"b\":\"c\"},{\"a\":\"b\", \"b\":\"c\"},   {},{},{}]},\"b\":{\"a\":\"b\"\t\t,\n \"b\":\"c\", \"c\":\"d\"}, \"c\":\"d\"\n}}" );
}

void test_json_object_count(jsc_el_t* el)
{
   /*simple unit tests*/
    UNIT_TEST_COUNT( "{}", 0);
    UNIT_TEST_COUNT( "{\"b\":\"b\"}", 1);
    UNIT_TEST_COUNT( "{\"a\":\"b\",\"b\":\"b\"}", 2);
    UNIT_TEST_COUNT( "{\"a\":{},\"b\":\"b\",\"c\":-20e3,\"d\":[1,2,3],\"e\":null}", 5);
    UNIT_TEST_COUNT( "{\"a\":\"b\",\"b\":[{\"a\":{},\"b\":{}},{},{}]}", 2);
}

void test_json_object_map(jsc_el_t* el)
{
  struct s_owner
  {
    char* name;
  };
  struct s_detail
  {
    struct s_owner owner;
    char* vaccinated;
  };
  struct s_animal 
  {
    char* name;
    char* age;
    char* nothing;
    char* type;
    char* typed;
    char* typ;
    struct s_detail detail;    
  };
  struct s_animal animal;
  int test_state;
   /*clear test struct*/
    memset(&animal, '\0', sizeof(animal));

   /*try a simple map*/
    UNIT_TEST_MAP_OBJ_START( &test_state,
                             "{\"name\":\"scruffy\",\"age\":20, \"type\":\"doge\"}",
                             &animal, sizeof(animal),
                             JCMAP_STR(s_animal, name),
                             JCMAP_STR(s_animal, age),
                             JCMAP_STR(s_animal, type),
                             JCMAP_STR(s_animal, typed),  /*SHOULDN'T match*/
                             JCMAP_STR(s_animal, typ),  /*SHOULDN'T match*/
                             JCMAP_END()
                            );
    UNIT_TEST_MAP_STRCMP( &test_state, animal.name, "scruffy");
    UNIT_TEST_MAP_STRCMP( &test_state, animal.age, "20");
    UNIT_TEST_MAP_STRCMP( &test_state, animal.nothing, NULL);  /*are values getting mapped that shouldn't?*/
    UNIT_TEST_MAP_STRCMP( &test_state, animal.type, "doge");
    UNIT_TEST_MAP_STRCMP( &test_state, animal.typed, NULL);  /*next 2 test if string matching takes account of length correctly*/
    UNIT_TEST_MAP_STRCMP( &test_state, animal.typ, NULL);
    UNIT_TEST_MAP_END( );

   /*map w/ key one letter too short & one too long*/
    memset(&animal, '\0', sizeof(animal));
    UNIT_TEST_MAP_OBJ_START( &test_state,
                             "{\"nam\":\"scruffy\",\"aged\":20, \"type\":\"doge\"}",
                             &animal, sizeof(animal),
                             JCMAP_STR(s_animal, name),
                             JCMAP_STR(s_animal, age),
                             JCMAP_STR(s_animal, type),
                             JCMAP_END()
                            );
    UNIT_TEST_MAP_STRCMP( &test_state, animal.name, NULL);  /*key is 'nam' not 'name'*/
    UNIT_TEST_MAP_STRCMP( &test_state, animal.age, NULL);  /*key is aged not age*/
    UNIT_TEST_MAP_STRCMP( &test_state, animal.type, "doge");
    UNIT_TEST_MAP_END( );

   /*map with nested key matching (SHOULDN'T get aquired)*/
    memset(&animal, '\0', sizeof(animal));
    UNIT_TEST_MAP_OBJ_START( &test_state,
                            "{\"name\":\"scruffball\", \"somedetails\":{\"age\":1}, \"type\":\"doge\"}",
                            &animal, sizeof(animal),
                            JCMAP_STR(s_animal, name),
                            JCMAP_STR(s_animal, age),
                            JCMAP_STR(s_animal, type),
                            JCMAP_END()
                           );
    UNIT_TEST_MAP_STRCMP( &test_state, animal.name, "scruffball");
    UNIT_TEST_MAP_STRCMP( &test_state, animal.age, NULL);
    UNIT_TEST_MAP_STRCMP( &test_state, animal.type, "doge");

   /*key names contain escape sequences??*/

   /*try a two-level map*/
   /*try a three-level map*/
   /*error condition - json ends in funny places*/
   /*error condition - missing key*/
}

