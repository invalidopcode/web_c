/*jsc.h - function definitions for helper jsc
         licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#ifndef JSC_H
#define JSC_H
/*includes with needed types*/
# include <res/buffer.h>

/*types*/
 typedef struct {
   char* class_name;
   res_buffer_t* buffer;
 } jsc_el_t;

/*functions*/
 jsc_el_t* jsc_el_init(char *class_name);  /*initialise element. Returns element handle on success, NULL on failure*/
 int jsc_el_appendf(jsc_el_t* el, const char* format, ...);  /*appends format string to buffer - use write to flush to dom. Resturns true on success, false on failure*/
 int jsc_el_write(jsc_el_t* el);  /*flushes buffer to dom, appending to all html elements matching class_name. Resets buffer. Returns true on success, false on failure*/
 int jsc_el_clear(jsc_el_t* el);  /*clears all contents of matching dom elements. Returns true on success, false on failure*/
 int jsc_el_finish(jsc_el_t* el);  /*cleanup memory allocated in init and by buffer*/
#endif

