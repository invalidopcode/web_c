/*jcmap_string.h - function definitions for helper jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_STRING_H
#define JCMAP_STRING_H
# include <stddef.h>

/*functions*/
 int jcmap_string_init(char** str, size_t* limit);
 int jcmap_string_push_c(char** str, size_t* limit, char c);
 int jcmap_string_push_str(char** str, size_t* limit, char* push_str);
#endif

