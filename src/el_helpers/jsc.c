/*jsc.c - source of jsc helper
         licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#include <emscripten.h>
#include <stdlib.h>
#include <stdbool.h>
#include <res/buffer.h>
#include <jsc.h>

jsc_el_t* jsc_el_init(char *class_name)
{
  jsc_el_t* el;
   /*allocate memory for el structure and buffer*/
    el = calloc(1, sizeof(jsc_el_t));
    if (NULL == el)
      return (NULL);

    el->class_name = class_name;
    el->buffer = res_buffer_create(1024);
    if (NULL == el->buffer)
    {
      free(el);
      return (NULL);
    }

   /*set start of buffer to string NULL terminator*/
    *(char*)(res_buffer_get(el->buffer)) = '\0';

  return (el);
}

int jsc_el_finish(jsc_el_t* el)
{
  res_buffer_destroy(el->buffer);
  free(el);
  return (true);
}

int jsc_el_appendf(jsc_el_t* el, const char* format, ...)
{
  va_list args;
  va_start(args, format);
   /*very basic vargs wrapper function*/
    if (0 != _res_buffer_vappendf(el->buffer,
                                  format,
                                  &args))
    {
      va_end(args);
      return (false);
    } else {
      va_end(args);
      return (true);
    }
}

int jsc_el_write(jsc_el_t* el)
{
 /*go to start of buffer*/
  if ( 0 != res_buffer_reset(el->buffer) )
    return (false);

 /*write it to the DOM as a string*/
  EM_ASM({var i; var elements = document.getElementsByClassName(UTF8ToString($0)); for(i=0; i<elements.length; i++){elements[i].insertAdjacentHTML('beforeend', UTF8ToString($1));}}, el->class_name, res_buffer_get(el->buffer));

 /*return success*/
  return (true);
}

int jsc_el_clear(jsc_el_t* el)
{
  EM_ASM({var i; var elements = document.getElementsByClassName(UTF8ToString($0)); for(i=0; i<elements.length; i++){elements[i].innerHTML = UTF8ToString($1);}}, el->class_name, "");

 /*return success*/
  return (true);
}
