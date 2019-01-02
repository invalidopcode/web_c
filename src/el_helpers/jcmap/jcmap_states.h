/*jcmap_states.h - function definitions for helper jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_STATES_H
#define JCMAP_STATES_H
/*includes w/ needed types*/
# include <stdint.h>
# include <stddef.h>
# include "jcmap_map.h"

/*TODO: design locate mode - stops at start of value when key matches (no need for depth! caller can repeatedly call using strtok)*/
/*NOTE: all state values undefined for invalid json, except validation obv*/
/*(EXCEPT for 2nd level string and number validators, where invalid count is set to 0)*/

/*state-related types*/
 typedef enum {st_pending, st_invalid, st_valid}jcmap_status_t;
 typedef enum {st_validate, st_map_object, st_map_array, st_aquire}jcmap_mode_t;
 typedef struct {
  /*main state tracking vars*/
   jcmap_status_t status;
   jcmap_mode_t mode;
   char *buffer;
   size_t buffer_limit;
   int can_end;  /*bool, indicates whether it's ok to end on limit reached. SHOULD normally be false, except for number processing*/

  /*counters*/
   size_t object_depth;  /*starts at 0, ++'d by object_start*/
   size_t array_depth;  /*starts at 0, ++'d by array_start. Saved and set to 0 when processing object inside array*/
   size_t count;  /*each type keeps a local count - raw characters for string and numbers, key-values for objects, and items for array. On end, each will set count. As such, this value will contain top-level count at end. Intermediate values are used internally for map construction*/
   int decimal;  /*set to true when decimal hit in number - only 1 is valid*/

  /*mapping vars*/
   jcmap_map_t* map;
   size_t map_size;  /*n entries in map*/
   size_t map_max_depth;
   uint8_t *map_target_object;
   uint8_t *map_target_array;
   size_t map_target_size;  /*size of WHOLE object inc nested structs, used to go to next one in array*/

  /*aquire vars*/
   jcmap_target_t aquire_type;  /*c_null implies no aquiring to be done*/
   void* aquire_result;
   size_t aquire_buffer_limit;
   //char* aquire_result_string;  /*next 3 for storing results of the different possible aquire states. String must be free'd*/
   //uint8_t* aquire_result_raw;  /*points somewhere on the raw json data buffer given us*/
   //int aquire_result_int;  /*must *NOT* be free'd*/
 } jcmap_state_t;

/*state functions*/
 jcmap_state_t* jcmap_st_create(jcmap_mode_t mode, char* buffer, size_t limit);
 void jcmap_st_set_map(jcmap_state_t* state, jcmap_map_t* map, size_t max_depth);
 void jcmap_st_set_target_object(jcmap_state_t* state, void* target);
 void jcmap_st_set_target_array(jcmap_state_t* state, void* target);
 void jcmap_st_destroy(jcmap_state_t* state);
 size_t jcmap_st_get_count(jcmap_state_t* state);
 jcmap_status_t jcmap_st_get_status(jcmap_state_t* state);
 void jcmap_st_set_aquire_type(jcmap_state_t* state, jcmap_target_t aquire_type);
 void* jcmap_st_get_aquire(jcmap_state_t* state);

/*ALL take pointer to state structure, update it with results, etc, and return NULL on success or error string on failure*/
 char* jcmap_st_value(jcmap_state_t* state);
 char* jcmap_st_value_sep(jcmap_state_t* state);
 char* jcmap_st_object(jcmap_state_t* state);
 char* jcmap_st_object_start(jcmap_state_t* state);
 char* jcmap_st_object_loop(jcmap_state_t* state);
 char* jcmap_st_object_loop_name(jcmap_state_t* state);  /*-> jcmap_st_string*/
 char* jcmap_st_object_loop_name_sep(jcmap_state_t* state);
 char* jcmap_st_object_loop_value(jcmap_state_t* state);  /*-> jcmap_st_value*/
 char* jcmap_st_object_loop_value_sep(jcmap_state_t* state);  /*-> jcmap_st_value_sep*/
 char* jcmap_st_object_end(jcmap_state_t* state);
 char* jcmap_st_array(jcmap_state_t* state);
 char* jcmap_st_array_start(jcmap_state_t* state);
 char* jcmap_st_array_loop(jcmap_state_t* state);
 char* jcmap_st_array_loop_value(jcmap_state_t* state);  /*-> jcmap_st_value*/
 char* jcmap_st_array_loop_value_sep(jcmap_state_t* state);  /*-> jcmap_st_value_sep*/
 char* jcmap_st_array_end(jcmap_state_t* state);
 char* jcmap_st_string(jcmap_state_t* state);
 char* jcmap_st_string_start(jcmap_state_t* state);
 char* jcmap_st_string_loop(jcmap_state_t* state);
 char* jcmap_st_string_loop_escape(jcmap_state_t* state);
 char* jcmap_st_string_loop_escape_unicode(jcmap_state_t* state);
 char* jcmap_st_string_loop_character(jcmap_state_t* state);
 char* jcmap_st_string_end(jcmap_state_t* state);
 char* jcmap_st_number(jcmap_state_t* state);
 char* jcmap_st_number_start(jcmap_state_t* state);
 char* jcmap_st_number_start_zero(jcmap_state_t* state);
 char* jcmap_st_number_start_zero_decimal(jcmap_state_t* state);
 char* jcmap_st_number_start_minus(jcmap_state_t* state);
 char* jcmap_st_number_start_one_nine(jcmap_state_t* state);
 char* jcmap_st_number_loop(jcmap_state_t* state);
 char* jcmap_st_number_loop_decimal(jcmap_state_t* state);
 char* jcmap_st_number_loop_digit(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp_start(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp_start_sign(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp_start_digit(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp_loop(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp_loop_digit(jcmap_state_t* state);
 char* jcmap_st_number_loop_exp_end(jcmap_state_t* state);
 char* jcmap_st_number_end(jcmap_state_t* state);
 char* jcmap_st_keyword(jcmap_state_t* state);
#endif
