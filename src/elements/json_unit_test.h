/*json_unit_test.h - macros for jcmap unit tests
         Licensed under the GNU General Public License v3.0
         see LICENSE file for more information*/
#ifndef JSON_UNIT_TEST_H
#define JSON_UNIT_TEST_H

# include <stdbool.h>

# define MAX_LABEL_WIDTH         60

# define UNIT_TEST_START()       jsc_el_appendf(el, "<div class='test_unit'>")
# define UNIT_TEST_END()         jsc_el_appendf(el, "</div>")
# define UNIT_SUBTEST_START()    jsc_el_appendf(el, "<div class='test_sub_unit'>")
# define UNIT_SUBTEST_END()      jsc_el_appendf(el, "</div>")
# define UNIT_TEST_LABEL(NAME)   jsc_el_appendf(el, "%s:\n", NAME)
# define UNIT_TEST_SUCCESS(MSG)  jsc_el_appendf(el, "<span class='test_success'>%s</span>\n", MSG)
# define UNIT_TEST_SUCCESS_S(MSG)  jsc_el_appendf(el, "<span class='test_success'>\"%s\"</span>\n", MSG)
# define UNIT_TEST_SUCCESS_N(_n) jsc_el_appendf(el, "<span class='test_success'>%lu</span>\n", _n)
# define UNIT_TEST_FAIL(MSG)     jsc_el_appendf(el, "<span class='test_error'>%s</span>\n", MSG)
# define UNIT_TEST_FAIL_S(MSG)   jsc_el_appendf(el, "<span class='test_error'>\"%s\"</span>\n", MSG)
# define UNIT_TEST_FAIL_N(_n)    jsc_el_appendf(el, "<span class='test_error'>%lu</span>\n", _n)

# define UNIT_TEST_TABLE_START()   jsc_el_appendf(el, "<table>")
# define UNIT_TEST_TABLE_END()     jsc_el_appendf(el, "</table>")
# define UNIT_TEST_COLUMN_START()  jsc_el_appendf(el, "<tr>")
# define UNIT_TEST_COLUMN_END()    jsc_el_appendf(el, "</tr>")
# define UNIT_TEST_ROW_START()     jsc_el_appendf(el, "<td>")
# define UNIT_TEST_ROW_END()       jsc_el_appendf(el, "</td>")
# define UNIT_TEST_ROW( STR )      jsc_el_appendf(el, "<th>%s</th>", STR)

# define UNIT_TEST_VALID(DATA) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size->limit means -1*/ \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      if ( jcmap_json_validate((uint8_t*)((void*)_buffer), _buffer_limit) ) \
        UNIT_TEST_SUCCESS( "valid" ); \
      else \
        UNIT_TEST_FAIL( "invalid" ); \
      UNIT_TEST_END(); \
  } while (0)


# define UNIT_TEST_INVALID(DATA) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size->limit means -1*/ \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      if ( ! jcmap_json_validate((uint8_t*)((void*)_buffer), _buffer_limit) ) \
        UNIT_TEST_SUCCESS( "invalid" ); \
      else \
        UNIT_TEST_FAIL( "valid" ); \
      UNIT_TEST_END(); \
  } while (0)

# define UNIT_TEST_COUNT(DATA, EXPECTED_COUNT) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size->limit means -1*/ \
    size_t count; \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      if ( EXPECTED_COUNT == (count=jcmap_json_count((uint8_t*)((void*)_buffer), _buffer_limit)) ) \
        UNIT_TEST_SUCCESS_N( count ); \
      else \
        UNIT_TEST_FAIL_N( count ); \
      UNIT_TEST_END(); \
  } while (0)

# define UNIT_TEST_COUNT_NOT(DATA, EXPECTED_COUNT) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size->limit means -1*/ \
    size_t count; \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      if ( EXPECTED_COUNT != (count=jcmap_json_count((uint8_t*)((void*)_buffer), _buffer_limit)) ) \
        UNIT_TEST_SUCCESS_N( count ); \
      else \
        UNIT_TEST_FAIL_N( count ); \
      UNIT_TEST_END(); \
  } while (0)

#endif

#define UNIT_TEST_AQUIRE_STRING(DATA, EXPECTED_STRING) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size->limit means -1*/ \
    char* _aquired_str; \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      _aquired_str = (char*) jcmap_json_aquire(c_string, (uint8_t*)((void*)_buffer), _buffer_limit); \
      if (0 == strcmp(_aquired_str, EXPECTED_STRING)) \
        UNIT_TEST_SUCCESS_S( _aquired_str ); \
      else \
        UNIT_TEST_FAIL_S( _aquired_str ); \
      UNIT_TEST_END(); \
      free( _aquired_str ); \
  } while (0)

#define UNIT_TEST_AQUIRE_INT(DATA, EXPECTED_INT) \
  do { \
  } while (0)

#define UNIT_TEST_MAP_OBJ_START(TEST_STATE, DATA, TARGET, TARGET_SIZE, ...) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size to limit means -1*/ \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      if ( jcmap_map(_buffer, _buffer_limit, TARGET, TARGET_SIZE, __VA_ARGS__) ) \
      { \
        UNIT_TEST_SUCCESS( "valid" ); \
        *( TEST_STATE ) = true; /*sub-tests can proceed*/\
      } else { \
        UNIT_TEST_FAIL( "invalid" ); \
        *( TEST_STATE ) = false; \
      } \
  } while (0)
#define UNIT_TEST_MAP_STRCMP(TEST_STATE, VALUE, EXPECTED) \
  do { \
    if ( true == *(TEST_STATE) ) \
    { \
      UNIT_SUBTEST_START(); \
      UNIT_TEST_LABEL( #VALUE ); \
      if (0 == strcmp(EXPECTED, VALUE)) \
      { \
        UNIT_TEST_SUCCESS_S( VALUE ); \
      } else { \
        UNIT_TEST_FAIL_S( VALUE ); \
      } \
      UNIT_SUBTEST_END(); \
    } \
  } while (0)
#define UNIT_TEST_MAP_END() \
  do { \
    UNIT_TEST_END(); \
  } while (0)

#define UNIT_TEST_MAP_ARR_START(TEST_STATE, DATA, TARGET, TARGET_SIZE, ...) \
  do { \
    uint8_t* _buffer = (uint8_t*)( DATA ); \
    size_t _buffer_limit = strlen(DATA) - 1;  /*size to limit means -1*/ \
      UNIT_TEST_START(); \
      if (strlen(DATA) < MAX_LABEL_WIDTH) \
        UNIT_TEST_LABEL(DATA); \
      else \
        UNIT_TEST_LABEL( "-> (long string)" ); \
      if ( jcmap_map(_buffer, _buffer_limit, TARGET, TARGET_SIZE, __VA_ARGS__) ) \
      { \
        UNIT_TEST_SUCCESS( "valid" ); \
        *( TEST_STATE ) = true; /*sub-tests can proceed*/\
      } else { \
        UNIT_TEST_FAIL( "invalid" ); \
        *( TEST_STATE ) = false; \
      } \
  } while (0)
#define UNIT_TEST_ROW_STRCMP(TEST_STATE, VALUE, EXPECTED) \
  do { \
    if ( true == *(TEST_STATE) ) \
    { \
      UNIT_TEST_ROW_START(); \
      if (0 == strcmp(EXPECTED, VALUE)) \
      { \
        UNIT_TEST_SUCCESS_S( VALUE ); \
      } else { \
        UNIT_TEST_FAIL_S( VALUE ); \
      } \
      UNIT_TEST_ROW_END(); \
    } \
  } while (0)

