/*jcmap_char.h - function definitions for helper jcmap
         Licensed under cc0 (yes, this is different to everything else!)*/
#ifndef JCMAP_CHECKS_H
#define JCMAP_CHECKS_H
/*typedefs*/
 typedef enum {j_object, j_array, j_string, j_number, j_keyword, j_none}jcmap_type_t;  /*json types*/
/*functions*/
 int jcmap_char_whitespace(char character);
 int jcmap_char_name_seperator(char character);
 int jcmap_char_value_seperator(char character);

 int jcmap_char_object_start(char character);
 int jcmap_char_object_end(char character);
 int jcmap_char_array_start(char character);
 int jcmap_char_array_end(char character);
 int jcmap_char_string_start(char character);
 int jcmap_char_string_escape(char character);
 int jcmap_char_escape_quote(char character);
 int jcmap_char_escape_backslash(char character);
 int jcmap_char_escape_forwardslash(char character);
 int jcmap_char_escape_backspace(char character);
 int jcmap_char_escape_formfeed(char character);
 int jcmap_char_escape_linefeed(char character);
 int jcmap_char_escape_carriagereturn(char character);
 int jcmap_char_escape_tab(char character);
 int jcmap_char_escape_unicode(char character);
 int jcmap_char_escape_unicode_hex(char character);
 int jcmap_char(char character);
 int jcmap_char_string_end(char character);
 int jcmap_char_number_start(char character);
 int jcmap_char_number_zero(char character);
 int jcmap_char_number_decimal(char character);
 int jcmap_char_number_minus(char character);
 int jcmap_char_number_one_nine(char character);
 int jcmap_char_number_exp_start(char character);
 int jcmap_char_number_exp_plus_minus(char character);
 int jcmap_char_number_exp_after(char character);
 int jcmap_char_number_digit(char character);
 int jcmap_char_number_after(char character);
 int jcmap_char_true_start(char character);
 int jcmap_char_false_start(char character);
 int jcmap_char_null_start(char character);

 jcmap_type_t jcmap_char_detect_type(char character);
#endif
