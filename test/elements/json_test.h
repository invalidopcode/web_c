/*json_test.h - functions and structures for json_..._test elements
         Licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#ifndef JSON_TEST_H
#define JSON_TEST_H
 typedef struct {
   char* name;
   char* colour;
   int age;
 } t_animal;

 void cb_test_fetch(emscripten_fetch_t *fetch);
 void cb_test_fetch_error(emscripten_fetch_t *fetch);

 void test_json_string_valid(jsc_el_t* el);
 void test_json_string_count(jsc_el_t* el);
 void test_json_string_aquire(jsc_el_t* el);

 void test_json_number_valid(jsc_el_t* el);
 void test_json_number_count(jsc_el_t* el);
 void test_json_number_aquire(jsc_el_t* el);

 void test_json_object_valid(jsc_el_t* el);
 void test_json_object_count(jsc_el_t* el);
 void test_json_object_map(jsc_el_t* el);

 void test_json_array_valid(jsc_el_t* el);
 void test_json_array_count(jsc_el_t* el);
 void test_json_array_map(jsc_el_t* el);

 void test_json_keyword_valid(jsc_el_t* el);
 void test_json_keyword_count(jsc_el_t* el);
 void test_json_keyword_aquire(jsc_el_t* el);
#endif

