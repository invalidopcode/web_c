/*jcmap_api.h - function definitions for helper jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_API_H
#define JCMAP_API_H
/*includes for needed types*/
# include <stddef.h>
# include "jcmap_map.h"

/*types*/
 typedef struct {
   size_t limit;  /*max. number of entries in map array*/
   size_t len;  /*number of map entries in map array*/
   size_t depth;
   jcmap_map_t* map;
 } jcmap_map_handle_t;

/*macros*/
# define JCMAP_STR( type, name )  #name, offsetof(struct type, name), c_string
# define JCMAP_RAW( type, name )  #name, offsetof(struct type, name), c_raw
# define JCMAP_INT( type, name )  #name, offsetof(struct type, name), c_int
# define JCMAP_END( )             NULL

/*functions*/
 /*NOTE sub objects*/
 int jcmap_json_validate(void* buffer, size_t buffer_limit);
 size_t jcmap_json_count(void* buffer, size_t buffer_limit);
 int jcmap_json_map_object(void* object, jcmap_map_handle_t* map, void* buffer, size_t buffer_limit);
 int jcmap_json_map_array(void* object, size_t object_size, void* array, size_t n_elements, jcmap_map_handle_t* map, void* buffer, size_t buffer_limit);
 void* jcmap_json_aquire(jcmap_target_t target_type, void* buffer, size_t buffer_limit);

 jcmap_map_handle_t* jcmap_map_create(size_t object_size);
 int jcmap_map( void* buffer, size_t buffer_limit, void* target, size_t target_size, ...);
 /*OR: variadic function, using macros above, to enter a mapping all in one function, eg:
  * map = jcmap_map( JCMAP_STR(animal, name),
  *                  JCMAP_INT(animal, age),
  *                  JCMAP_STR(animal, colour),
  *                  JCMAP_RAW(animal, data),
  *                  JCMAP_OBJ(animal, medical_status),
  *                    JCMAP_STR(medical_status, last_checkup),
  *                    JCMAP_STR(medical_status, infected),
  *                    JCMAP_OBJ_END(),
  *                  "owner", offsetof(animal, human), jcmap_str_t,
  *                  JCMAP_END() );
  *could implement variadic fn via general version of fn.s defined below. Those can be
  *moved jcmap_map.c*/
 int jcmap_map_add_raw(jcmap_map_handle_t* map, char* key, ptrdiff_t offset);  /*does auto realloc to fit in*/
 int jcmap_map_add_string(jcmap_map_handle_t* map, char* key, ptrdiff_t offset);
 int jcmap_map_add_int(jcmap_map_handle_t* map, char* key, ptrdiff_t offset);
 int jcmap_map_add_map(jcmap_map_handle_t* root_map, char* key, ptrdiff_t offset, jcmap_map_handle_t* sub_map);
 int jcmap_map_destroy(jcmap_map_handle_t* map);  /*auto frees elements marked as strings too!*/
#endif
