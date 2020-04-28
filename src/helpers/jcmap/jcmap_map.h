/*jcmap_map.h - definitions for helper jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_MAP_H
#define JCMAP_MAP_H
# include <stddef.h>

/*typedefs*/
 typedef enum {c_null=0, c_raw, c_string, c_int, c_long, c_uint64, c_uint32, c_uint16, c_object}jcmap_target_t;  /*c types*/
 /*NOTE - object is special case*
  *     - sub objects SHOULD not be pointers to structs but BE structs themselves
  *     - object pointer saved to local variable
  *     - object pointer set to (old_pointer) + offset
  *     - map pointer saved to local variable
  *     - new map pointer loaded from old map
  *     - DON'T reset depth! (max_depth used to stop checking keys past a certain level, where it'd waste time)
  *     - restart recursion
  *     - restore object pointer and map pointer*/

 typedef struct jcmap_map_s{
   char* key;
   size_t key_size;  /*so we don't have to call strlen every time*/
   ptrdiff_t offset;
   jcmap_target_t type;
   struct jcmap_s *sub_map;
 } jcmap_map_t;

#endif
