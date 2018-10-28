/*jsc.c - source of jsc helper
         licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#include <emscripten.h>
#include <jsc.h>

void jsc_class_write(char *class, char *str)
{
  EM_ASM({var i; var elements = document.getElementsByClassName(UTF8ToString($0)); for(i=0; i<elements.length; i++){elements[i].insertAdjacentHTML('beforeend', UTF8ToString($1));}}, class, str);
}

