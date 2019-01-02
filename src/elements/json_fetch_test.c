/*json_fetch_test.c - element to test fetching and interpreting json

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

 const char class_name[] = "json_fetch_test";

int main(int argc, const char ** argv)
{
  emscripten_fetch_attr_t fetch_attr_1;
   /*build emscripten fetch object*/
    emscripten_fetch_attr_init(&fetch_attr_1);
    strcpy(fetch_attr_1.requestMethod, "GET");
    fetch_attr_1.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_REPLACE;

   /*set callbacks for fetch - cb_testobject on success*/
    fetch_attr_1.onsuccess = cb_test_fetch;
    fetch_attr_1.onerror = cb_test_fetch_error;

   /*fetch /teststrung.json, file SHOULD be built by site*/
    emscripten_fetch(&fetch_attr_1, "/test_array.json");  /*ASYNC*/

  return(0);
}

void cb_test_fetch(emscripten_fetch_t *fetch)
{
  char* data = (char*)( fetch->data );
  uint8_t* buffer = (uint8_t*)( fetch->data );
  size_t buffer_limit = fetch->numBytes - 2;  /*start count at 0, ignore trailing \0*/
  size_t count;
  jsc_el_t* el;
  struct s_animal {
    char* name;
    char* age;
    char* lives;
    char* colour;
  };
  struct s_animal *the_animals;
  size_t i;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
    jsc_el_clear(el);
   
   /*write data to elements*/
    data[fetch->numBytes - 1] = '\0';
    jsc_el_appendf(el, "<h1>JSON fetch test</h1>\n");
    jsc_el_appendf(el, "<h2>JSON string...</h2>\n");
    jsc_el_appendf(el, "%s\n", data);

   /*test array map*/
    count = jcmap_json_count(buffer, buffer_limit);
    the_animals = calloc(count, sizeof(struct s_animal));
    jsc_el_appendf(el, "<h2>Interpreting contents...</h2>\n");
    jcmap_map(buffer, buffer_limit, the_animals, sizeof(struct s_animal),
              JCMAP_STR(s_animal, name),
              JCMAP_STR(s_animal, age),
              JCMAP_STR(s_animal, lives),
              JCMAP_STR(s_animal, colour),
              JCMAP_END()
             );

    for (i=0; i<count; i++)
    {
      jsc_el_appendf(el, "<p><strong>%s</strong>", the_animals[i].name);
      if (NULL != the_animals[i].colour)
        jsc_el_appendf(el, " is %s;", the_animals[i].colour);
      if (NULL != the_animals[i].age)
        jsc_el_appendf(el, " is %s years old;", the_animals[i].age);
      if (NULL != the_animals[i].lives)
        jsc_el_appendf(el, " has %s lives;", the_animals[i].lives);
      jsc_el_appendf(el, "</p>\n");
    }
    jsc_el_appendf(el, "<p class='test_success'>done</p>\n");

  jsc_el_write(el);
  jsc_el_finish(el);
  emscripten_fetch_close(fetch);
}
void cb_test_fetch_error(emscripten_fetch_t *fetch)
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init((char*)class_name);
   
   /*write error message to dom*/
    jsc_el_appendf(el, "<p class='test_error'>error fetching test array</p>\n");
    jsc_el_write(el);
  jsc_el_finish(el);
  emscripten_fetch_close(fetch);
}

/*TODO: is sync error handling done right on ..._fetch??? & what about attr construction fns?*/

