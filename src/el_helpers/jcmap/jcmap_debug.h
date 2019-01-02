/*jcmap_debug.h - debug macros for jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_DEBUG_H
#define JCMAP_DEBUG_H
# ifdef JCMAP_DEBUG
#   include <stdio.h>
#
#   define JCDEBUG_ENTER_FN()    printf("entering %s, line %u\n", __func__, __LINE__);
#   define JCDEBUG_LEAVE_FN()    printf("leaving %s, line %u\n", __func__, __LINE__);
#   define JCDEBUG_ERR_FN(x)     printf("ERR is %s, line %u -> %s\n", __func__, __LINE__, x);
#   define JCDEBUG_NEXT_CHAR(x)  printf("next, processing - %c\n", x)
#   define JCDEBUG_PREV_CHAR(x)  printf("prev, back to - %c\n", x)
#   define JCDEBUG_VALID(x)      printf("!!! validated: %c\n", x)
#   define JCDEBUG_INVALID(x)    printf("invalidated: %c\n", x)
#   define JCDEBUG_TESTING(x,y)  printf("TESTING, \"%s\" vs \"%s\"\n", x, y);
#   define JCDEBUG_MATCHED(x)    printf("!!! MATCHED %s\n", x);
#   define JCDEBUG_MAPPING(x)    printf("!!! MAPPING value to offset: %p\n", (void*)x);
# else
#   define JCDEBUG_ENTER_FN()    do {} while(0)
#   define JCDEBUG_LEAVE_FN()    do {} while(0)
#   define JCDEBUG_ERR_FN(x)     do {} while(0)
#   define JCDEBUG_NEXT_CHAR(x)  do {} while(0)
#   define JCDEBUG_PREV_CHAR(x)  do {} while(0)
#   define JCDEBUG_VALID(x)      do {} while(0)
#   define JCDEBUG_INVALID(X)    do {} while(0)
#   define JCDEBUG_TESTING(x,y)  do {} while(0)
#   define JCDEBUG_MATCHED(x)    do {} while(0)
#   define JCDEBUG_MAPPING(x)    do {} while(0)
# endif
#endif
