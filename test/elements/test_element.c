/* test_element.c - simple web_c element
         licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#   ifndef WEBC_STDIO
#include <emscripten.h>
#   endif
#include <jsc.h>

int main()
{
  jsc_el_t* el;
   /*initialise jsc element*/
    el = jsc_el_init("test_element");
    jsc_el_clear(el);
  
   /*write to dom*/
    jsc_el_appendf(el, "<h1>test element loaded!</h1>\n");
    jsc_el_appendf(el, "<p class='test_success'>int: %d, string: \"%s\"</p>\n", 1337, "hax");
    jsc_el_write(el);  /*flushes buffer so far*/

   /*jsc element cleanup*/
    jsc_el_finish(el);
  return(0);
}
