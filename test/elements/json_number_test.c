/*json_number_test.c - element to test json mapper

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

 const char class_name[] = "json_number_test";

int main(int argc, const char ** argv)
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
    jsc_el_clear(el);
   
   /*write data to elements*/
    jsc_el_appendf(el, "<h1>JSON number test</h1>\n");

   /*test array validity*/
    jsc_el_appendf(el, "<h2>Testing validate()...</h2>\n");
    test_json_number_valid(el);

   /*test counting*/
    jsc_el_appendf(el, "<h2>Testing count()...</h2>\n");
    test_json_number_count(el);

   /*test array map*/
    jsc_el_appendf(el, "<h2>Testing aquire()...</h2>\n");
    test_json_number_aquire(el);

  jsc_el_write(el);
  jsc_el_finish(el);
}

void test_json_number_valid(jsc_el_t* el)
{
   /*simple error conditions*/
/*TODO: fix failing unit tests*/
   UNIT_TEST_INVALID( "012345"    );
   UNIT_TEST_INVALID( "0."        );
   UNIT_TEST_INVALID( "+100"      );
   UNIT_TEST_INVALID( "E45"       );
   UNIT_TEST_INVALID( "e45"       );
   UNIT_TEST_INVALID( "00.1"      );
   UNIT_TEST_INVALID( "--1"       );
   UNIT_TEST_INVALID( "0.12.34"   );
   UNIT_TEST_INVALID( "12.34.56"  );
   UNIT_TEST_INVALID( "123-45"    );
   UNIT_TEST_INVALID( "1234."     );
   UNIT_TEST_INVALID( "10e"       );
   UNIT_TEST_INVALID( "10e-"      );
   UNIT_TEST_INVALID( "10e."      );
   UNIT_TEST_INVALID( "10e1.1"    );
   UNIT_TEST_INVALID( "10e-1.1"   );
   UNIT_TEST_INVALID( "10e+"      );
   UNIT_TEST_INVALID( "10e+"      );
   UNIT_TEST_INVALID( "10e1-1"    );
   UNIT_TEST_INVALID( "10e1+1"    );
   UNIT_TEST_INVALID( "10e123e123");

  /*simple unit tests*/
   UNIT_TEST_VALID( "0"           );
   UNIT_TEST_VALID( "1"           );
   UNIT_TEST_VALID( "12345678900" );
   UNIT_TEST_VALID( "0.0123456789");
   UNIT_TEST_VALID( "12.34567890" );
   UNIT_TEST_VALID( "-12345"      );
   UNIT_TEST_VALID( "-1.2345"     );
   UNIT_TEST_VALID( "-0.12345"    );
   UNIT_TEST_VALID( "123e45"      );
   UNIT_TEST_VALID( "123E45"      );
   UNIT_TEST_VALID( "-123e45"     );
   UNIT_TEST_VALID( "123e-45"     );
   UNIT_TEST_VALID( "-123e-45"    );
   UNIT_TEST_VALID( "0.1e10"      );
   UNIT_TEST_VALID( "-0.23e10"    );
   UNIT_TEST_VALID( "-0.23e-10"   );

   /*TODO: buffer limit test*/
}

void test_json_number_count(jsc_el_t* el)
{
   /*simple unit tests*/
    UNIT_TEST_COUNT( "12345", 5);
    UNIT_TEST_COUNT( "12.345", 6);
    UNIT_TEST_COUNT( "-0.12345", 8);
    UNIT_TEST_COUNT( "12E+15", 6);
}

void test_json_number_aquire(jsc_el_t* el)
{
   /*simple unit tests*/
    UNIT_TEST_AQUIRE_STRING( "1", "1" );
    UNIT_TEST_AQUIRE_STRING( "0", "0" );
    UNIT_TEST_AQUIRE_STRING( "123", "123" );
    UNIT_TEST_AQUIRE_STRING( "0"           , "0"           );
    UNIT_TEST_AQUIRE_STRING( "1"           , "1"           );
    UNIT_TEST_AQUIRE_STRING( "12345678900" , "12345678900" );
    UNIT_TEST_AQUIRE_STRING( "0.0123456789", "0.0123456789");
    UNIT_TEST_AQUIRE_STRING( "12.34567890" , "12.34567890" );
    UNIT_TEST_AQUIRE_STRING( "-12345"      , "-12345"      );
    UNIT_TEST_AQUIRE_STRING( "-1.2345"     , "-1.2345"     );
    UNIT_TEST_AQUIRE_STRING( "-0.12345"    , "-0.12345"    );
    UNIT_TEST_AQUIRE_STRING( "123e45"      , "123e45"      );
    UNIT_TEST_AQUIRE_STRING( "123E45"      , "123e45"      );
    UNIT_TEST_AQUIRE_STRING( "-123e45"     , "-123e45"     );
    UNIT_TEST_AQUIRE_STRING( "123e-45"     , "123e-45"     );
    UNIT_TEST_AQUIRE_STRING( "-123e-45"    , "-123e-45"    );
    UNIT_TEST_AQUIRE_STRING( "0.1e10"      , "0.1e10"      );
    UNIT_TEST_AQUIRE_STRING( "-0.23e10"    , "-0.23e10"    );
    UNIT_TEST_AQUIRE_STRING( "-0.23e-10"   , "-0.23e-10"   );
}

