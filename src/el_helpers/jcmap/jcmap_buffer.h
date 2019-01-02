/*jcmap_buffer.h - function definitions for helper jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_BUFFER_H
#define JCMAP_BUFFER_H
# include <stddef.h>
/*functions*/
 int jcmap_buffer_next(char* *buffer, size_t* buffer_limit);
 int jcmap_buffer_prev(char* *buffer, size_t* buffer_limit);
 int jcmap_buffer_whitespace(char* *buffer, size_t* buffer_limit);  /*expects to start on whitespace, advances buffer until there's no more or limit reached*/
#endif
