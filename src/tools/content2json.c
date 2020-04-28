#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include "jcmap.h"

/*function defs*/
 bool write_result(size_t site_json_len, char* site_json_buffer, size_t page_json_len, char* page_json_buffer, size_t content_type_len, char* content_type_buffer, FILE* f_content);  /*put final json file to stdout*/

 bool utf8_file_valid(FILE* test_file);  /*test if file contains only valid utf8. Takes pointer to stdio stream. returns true on valid, false on invalid. Expects byte-buffered, binary stream . Read begins from current position on stream. Note that stream will need to be rewound after function returns*/
 bool utf8_mem_valid(uint8_t *mem, size_t limit);  /*test if string in memory contains only valid utf8. Takes a pointer to the start of the memory, and a limit where mem + limit is the final valid byte in memory. returns true on valid, false if either invalid character detected or limit is reached part-way through a multibyte character*/

 bool f2jstring(FILE* file_in, FILE* file_out);  /*turns file_in into an escaped json string, outputs to file_out*/

 void normalise_buffer(char* buffer, size_t buffer_limit);  /*remove \0's from buffer, insert one at the end - ie normalise for printing to screen via printf*/
 size_t file_get_json(FILE* file, char* start_token, char* end_token, char* buffer, size_t buffer_limit);  /*returns 0 on failure, length of data on success*/
 bool filestr(FILE* file, char *str);
 int filestr_test(FILE* file, char *str);  /*call when 1st character matches. Returns 0 on match, 1 on fail, 2 on EOF. Unless EOF reached, on return stream is returned to original position*/

/*config constants*/
/*# define ENABLE_DEBUG*/
# define MAX_SITE_JSON_BUFFER     4096
# define MAX_PAGE_JSON_BUFFER     4096
# define MAX_CONTENT_TYPE_BUFFER  128

# define DEFAULT_CONTENT_TYPE      "page"
# define JSON_START_TOKEN          "<!--@WEBC_META>"
# define JSON_END_TOKEN            "<\\-->"  /*why? because \- is invalid json*/
# define CONTENT_TYPE_START_TOKEN  "<!--@WEBC_TYPE="
# define CONTENT_TYPE_END_TOKEN    "\\-->"

/*macros used internally*/
# define IS_4_UTF8(x)    ((x & 0xF8) == 0xF0)  /*high bits are 11110*/
# define IS_3_UTF8(x)    ((x & 0xF0) == 0xE0)  /*high bits are 1110*/
# define IS_2_UTF8(x)    ((x & 0xE0) == 0xC0)  /*high bits are 110*/
# define IS_1_UTF8(x)    (x < 128)             /*highest bit is 0*/
# define IS_UTF8_PART(x) ((x & 0xC0) == 0x80)  /*high bits are 10*/

# ifdef  ENABLE_DEBUG
#   define debugf(...)  fprintf(stderr, __VA_ARGS__)
# else
#   define debugf(...)
# endif

int main(int argc, char*argv[])
{
  FILE*  f_content = NULL;
  FILE*  f_site_json = NULL;
  size_t site_json_len;
  char*  site_json_buffer = NULL;
  char*  site_json_buffer_origin = NULL;  /*used for call to free()*/
  size_t page_json_len;
  char*  page_json_buffer = NULL;
  char*  page_json_buffer_origin = NULL;  /*used for call to free()*/
  size_t content_type_len;
  char*  content_type_buffer = NULL;
  char*  content_type_buffer_origin = NULL;  /*used for call free()*/
# define CLEAN_EXIT(retcode) \
    do { \
      if (f_site_json != NULL) fclose(f_site_json); \
      if (f_content != NULL) fclose(f_content); \
      if (site_json_buffer != NULL) free(site_json_buffer_origin); \
      if (page_json_buffer != NULL) free(page_json_buffer_origin); \
      if (content_type_buffer != NULL) free(content_type_buffer_origin); \
      return(retcode); \
    } while (0)

   /*check valid arguments*/
    if ( (argc > 3) || (argc < 2) )
    {
      fprintf(stderr, "bad arguments. Expected: content2json <html content file> [site json file]\n");
  CLEAN_EXIT(-1);
    }

   /*allocate memory*/
    site_json_buffer = malloc(MAX_SITE_JSON_BUFFER);
    site_json_buffer_origin = site_json_buffer;
    if (NULL == site_json_buffer)
  CLEAN_EXIT(-1);

    page_json_buffer = malloc(MAX_PAGE_JSON_BUFFER);
    page_json_buffer_origin = page_json_buffer;
    if (NULL == page_json_buffer)
  CLEAN_EXIT(-1);

    content_type_buffer = malloc(MAX_CONTENT_TYPE_BUFFER);
    content_type_buffer_origin = content_type_buffer;
    if (NULL == content_type_buffer)
  CLEAN_EXIT(-1);

   /*open the files*/
    f_content = fopen(argv[1], "rb");  /*binary mode is just simpler imho*/
    if (NULL == f_content)
    {
      perror("error opening html content");
  CLEAN_EXIT(-1);
    }

    if (argc > 2)  /*only open site json file if specified on command line*/
    {
      f_site_json = fopen(argv[2], "rb");  /*binary mode is simpler imho*/
      if (NULL == f_site_json)
      {
        perror("error opening site json");
  CLEAN_EXIT(-1);
      }
    }

   /*validate both files are utf8*/
    if (utf8_file_valid(f_content))
      rewind(f_content);
    else {
      fprintf(stderr, "error: %s not valid utf8\n", argv[1]);
  CLEAN_EXIT(-1);
    }

    if (NULL != f_site_json)  /*may not be a site json to read...*/
      if (utf8_file_valid(f_site_json))
        rewind(f_site_json);
      else {
        fprintf(stderr, "error: %s not valid utf8\n", argv[2]);
  CLEAN_EXIT(-1);
      }

   /*store site json in buffer, validate json*/
    if (NULL == f_site_json)  /*is there site json to read in?*/
    {
      strcpy(site_json_buffer, "{}");
      site_json_len = strlen(site_json_buffer);
    } else {
     /*get the contents of file*/
      site_json_len = fread(site_json_buffer, 1, MAX_SITE_JSON_BUFFER, f_site_json);

      if (!feof(f_site_json))  /*end of file not reached means it was too big*/
      {
        fprintf(stderr, "error: site json file too big\n");
  CLEAN_EXIT(-1);
      }

      if (ferror(f_site_json))
      {
        perror("error reading site json");
  CLEAN_EXIT(-1);
      }

     /*skip whitespace, fail if no '{' at start of object*/
      while ((site_json_len>2) && (isspace(*site_json_buffer)))  /*>2 because {} is smallest buffer possible*/
      {
        site_json_buffer++;
        site_json_len--;
      }
      if ('{' != *site_json_buffer)
      {
        fprintf(stderr, "error: invalid site json file. Expected: '{'\n");
  CLEAN_EXIT(-1);
      }

     /*run validation on buffer*/
      if (! jcmap_json_validate(site_json_buffer, site_json_len-1))
      {
        fprintf(stderr, "error validating site json\n");
  CLEAN_EXIT(-1);
      }

     /*recount site_json to avoid bug where cruft after valid json object gets printed*/
      site_json_len = jcmap_json_byte_count(site_json_buffer, site_json_len-1);
    }

   /*find page json, store in buffer if found, validate*/
    page_json_len = file_get_json(f_content,
                                  JSON_START_TOKEN,
                                  JSON_END_TOKEN,
                                  page_json_buffer,
                                  MAX_PAGE_JSON_BUFFER-1);
    if (0 == page_json_len)
    {
      strcpy(page_json_buffer, "{}");
      page_json_len = strlen(page_json_buffer);
    } else {
     /*skip whitespace, fail if no '{' at start of object*/
      while ((page_json_len>2) && (isspace(*page_json_buffer)))  /*>2 because {} is smallest buffer possible*/
      {
        page_json_buffer++;
        page_json_len--;
      }
      if ('{' != *page_json_buffer)
      {
        fprintf(stderr, "error: invalid json in file after %s. Expected: '{'\n", JSON_START_TOKEN);
  CLEAN_EXIT(-1);
      }

     /*validate buffer*/
      if (! jcmap_json_validate(page_json_buffer, page_json_len-1))
      {
        fprintf(stderr, "error: WEBC_META contains invalid json\n");
  CLEAN_EXIT(-1);
      }

     /*save count*/
      page_json_len = jcmap_json_byte_count(page_json_buffer, page_json_len-1);
    }

    rewind(f_content);  /*file accessed again, so tidy up!*/

   /*find content type, store in buffer if found, else set to default*/
    content_type_len = file_get_json(f_content,
                                     CONTENT_TYPE_START_TOKEN,
                                     CONTENT_TYPE_END_TOKEN,
                                     content_type_buffer,
                                     MAX_CONTENT_TYPE_BUFFER-1);
                                     
    if (0 == content_type_len)
    {
      strcpy(content_type_buffer, "\"page\"");
      content_type_len = strlen(content_type_buffer);
    } else {
     /*skip whitespace, fail if no " at start of string*/
      while ((content_type_len>2) && (isspace(*content_type_buffer)))  /*>2 because "" is smallest buffer possible*/
      {
        content_type_buffer++;
        content_type_len--;
      }
      if ('"' != *content_type_buffer)
      {
        fprintf(stderr, "error: invalid json in file after %s. Expected: '\"'\n", CONTENT_TYPE_START_TOKEN);
  CLEAN_EXIT(-1);
      }

     /*validate buffer*/
      if (! jcmap_json_validate(content_type_buffer, content_type_len-1))
      {
        fprintf(stderr, "error: content type string invalid\n");
  CLEAN_EXIT(-1);
      }

     /*save count*/
      content_type_len = jcmap_json_byte_count(content_type_buffer, content_type_len-1);
    }

    rewind(f_content);  /*file accessed again, so tidy up!*/

   /*output result*/
    if (! write_result(site_json_len, site_json_buffer,
                       page_json_len, page_json_buffer,
                       content_type_len, content_type_buffer,
                       f_content))
  CLEAN_EXIT(-1);


   /*return success*/
  CLEAN_EXIT(0);
}

bool write_result(size_t site_json_len, char* site_json_buffer,
                  size_t page_json_len, char* page_json_buffer,
                  size_t content_type_len, char* content_type_buffer,
                  FILE* f_content)
{
  size_t bytes_written;
   /*site json*/
    printf("{\"site\":");
    bytes_written = fwrite(site_json_buffer, 1, site_json_len, stdout);
    if (bytes_written != site_json_len)
    {
      perror("error writing site json to stdout");
  return(false);
    }

   /*page json*/
    printf(", \"page\":");
    bytes_written = fwrite(page_json_buffer, 1, page_json_len, stdout);
    if (bytes_written != page_json_len)
    {
      perror("error writing WEBC_META json to stdout");
  return(false);
    }

   /*content type*/
    printf(", \"content_type\":");
    bytes_written = fwrite(content_type_buffer, 1, content_type_len, stdout);
    if (bytes_written != content_type_len)
    {
      perror("error writing content type to stdout");
  return(false);
    }

   /*content*/
    printf(", \"content\":\"");
    f2jstring(f_content, stdout);
    printf("\"");

   /*end*/
    printf("}\n");
  return(true);
}


bool utf8_file_valid(FILE* test_file)
{
  uint8_t part_counter=0;
  int c=EOF;
    while (EOF != (c=fgetc(test_file)))
    {
     /*are we expecting part of a multibyte character?*/
      if (part_counter > 0)
      {
       /*if not a valid utf8 part, fail*/
        if (! IS_UTF8_PART(c))
  return(false);
        else
          part_counter--;

     /*expecting either a lead byte or a 1-byte character...*/
      } else {
       /*1-byte utf8 character*/
        if (IS_1_UTF8(c))
        {
         /*7-bit ascii, nothing to do*/
        } else {

       /*2-byte utf8 character*/
        if (IS_2_UTF8(c))
        {
          part_counter = 1;  /*expecting 1 more byte-part*/
        } else {

       /*3-byte utf8 character*/
        if (IS_3_UTF8(c))
        {
          part_counter = 2;  /*expecting 2 more byte-parts*/
        } else {

       /*4-byte utf8 character*/
        if (IS_4_UTF8(c))
        {
          part_counter = 3;  /*expecting 3 more byte-parts*/
        } else {

       /*invalid character, fail*/
  return(false);
        }}}}
      }
    }

   /*reached the end - fail if in the middle of a multi-byte character*/
    if (part_counter > 0)
  return(false);
    else
  return(true);
}

bool utf8_mem_valid(uint8_t *mem, size_t limit)
{
  uint8_t part_counter=0;
    while (true)
    {
     /*are we expecting part of a multibyte character?*/
      if (part_counter > 0)
      {
       /*if not a valid utf8 part, fail*/
        if (! IS_UTF8_PART(*mem))
  return(false);
        else
          part_counter--;

     /*expecting either a lead byte or a 1-byte character...*/
      } else {
       /*1-byte utf8 character*/
        if (IS_1_UTF8(*mem))
        {
         /*7-bit ascii, nothing to do*/
        } else {

       /*2-byte utf8 character*/
        if (IS_2_UTF8(*mem))
        {
          part_counter = 1;  /*expecting 1 more byte-part*/
        } else {

       /*3-byte utf8 character*/
        if (IS_3_UTF8(*mem))
        {
          part_counter = 2;  /*expecting 2 more byte-parts*/
        } else {

       /*4-byte utf8 character*/
        if (IS_4_UTF8(*mem))
        {
          part_counter = 3;  /*expecting 3 more byte-parts*/
        } else {

       /*invalid character, fail*/
  return(false);
        }}}}
      }

     /*byte done, move to next byte*/
      if (limit == 0)  /*end of string? Exit loop*/
        break;
      mem++;
      limit--;
    }

   /*reached the end - fail if in the middle of a multi-byte character*/
    if (part_counter > 0)
  return(false);
    else
  return(true);
}

bool f2jstring(FILE* file_in, FILE* file_out)
{
  int c;
   /*loop through file byte by byte*/
    while ( EOF != (c=fgetc(file_in)) )
    {
      if (c < 0x1F)  /*control character*/
      {
        switch (c)  /*try nice output first, default to \u.... form*/
        {
          case '\b': fputs("\\b" , file_out); break;
          case '\f': fputs("\\f" , file_out); break;
          case '\n': fputs("\\n" , file_out); break;
          case '\r': fputs("\\r" , file_out); break;
          case '\t': fputs("\\t" , file_out); break;
          default:
            fprintf(file_out, "\\u%04x", c);
        }
      } else {  /*non-control utf8 character*/
        switch (c)
        {
          case '"': fputs("\\\"", file_out); break;
          case '\\': fputs("\\\\", file_out); break;
          default:
            fputc(c, file_out);  /*normal utf8 character - crack on!*/
        }
      }
    }
  return(true);
}

void normalise_buffer(char* buffer, size_t buffer_limit)
{
  size_t i;
   /*replace any null characters in buffer with spaces*/
    for (i=0; i<buffer_limit; i++)
      if ( '\0' == buffer[i] )
        buffer[i] = '\x20';

   /*add trailing \0*/
    buffer[buffer_limit] = '\0';
}

size_t file_get_json(FILE* file, char* start_token, char* end_token, char* buffer, size_t buffer_limit)
{
  size_t data_start;
  size_t data_len = 0;
   /*scan for start string, mark*/
    if ( ! filestr(file, start_token) )
    {
      debugf("ERROR: start_token not found\n");
  return(0);
    } else {
      data_start = ftell(file) + strlen(start_token);  /*start position*/
    }

   /*scan for end string, mark*/
    fseek(file, strlen(start_token), SEEK_CUR);  /*skip ahead to end of start token*/

    if ( ! filestr(file, end_token) )
    {
      debugf("ERROR: end_token not found\n");
  return(0);
    } else {
      data_len = ftell(file) - data_start;
    }

   /*move to start of data*/
    fseek(file, data_start, SEEK_SET);

   /*check buffer can fit data*/
    if (data_len > buffer_limit)
    {
      debugf("ERROR: data too large\n");
  return(0);
    }

   /*read data into buffer*/
    if (fread(buffer, 1, data_len, file) != data_len)
    {
      debugf("error reading file data: %s", strerror(errno));
  return(0);
    }

  return(data_len);
}

bool filestr(FILE* file, char *str)
{
  int c;
   /*loop over all characters in file*/
    while ( EOF != ( c=fgetc(file) ) )
    {
     /*test if string matches current position in file*/
      if (c == *str)  /*first char in string matches*/
      {
        switch (filestr_test(file, str+1))  /*test the rest of the characters in string*/
        {
          case 1:  /*not matched*/
            /*nothing to do - just on to next character*/
          break;

          case 0:  /*matched!*/
            fseek(file, -1, SEEK_CUR);  /*go back 1 step in file*/
  return(true);
          break;

          default:  /*ie EOF reached*/
  return(false);  /*no match possible*/
        }
      }
    }

  return(false);  /*no match found in loop*/
}

int filestr_test(FILE* file, char *str)  /*call when 1st character matches. Returns 0 on match, 1 on fail, 2 on EOF. Unless EOF reached, on return stream is returned to original position*/
{
  fpos_t start_position;
  char c;
   /*save position in stream*/
    fgetpos(file, &start_position);

   /*compare each char in string with stream*/
    while ('\0' != *str)  /*scan till end of string*/
    {
      if ( EOF == ( c=fgetc(file )) )  /*get char from file, fail if EOF reached*/
  return(2);

      if (c != *str)  /*compare current chars from file and string*/
      {
        fsetpos(file, &start_position);  /*restore stream position*/
  return(1);
      }

      str++;  /*iterate to next char in string*/
    }

   /*success*/
    fsetpos(file, &start_position);
  return(0);
}

