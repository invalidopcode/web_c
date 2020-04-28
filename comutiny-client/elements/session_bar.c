/* session_bar.c - if user is signed in, overwrites sign-in text with better
         licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#   ifndef WEBC_STDIO
#include <emscripten.h>
#   endif
#include <jsc.h>

int main()
{
  jsc_el_t* el;
  char* session_cookie;
   /*initialise jsc element*/
    el = jsc_el_init("session_bar");
    jsc_el_clear(el);
 
   /*display different bar if session present...*/
    if (NULL == (session_cookie = jsc_get_session_cookie()))
    {
      jsc_el_appendf(el, "| <a href='/control.html'>control panel</a> | <a href='/login.html'>sign in</a>");
    } else {
      jsc_el_appendf(el, "| <a href='/control.html'>control panel</a> | <a href='http://localhost:8880/delete'>sign out</a>");
    }
    jsc_el_write(el);  /*flushes buffer so far*/

   /*cleanup*/
    jsc_el_finish(el);
    free(session_cookie);
  return (0);
}
