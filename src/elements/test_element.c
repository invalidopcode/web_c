/* test_element.c - simple web_c element
         licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#include <emscripten.h>
#include <jsc.h>

int main()
{
  jsc_class_write("test_element", "<h1>test data loaded!</h1>");
  return(0);
}
