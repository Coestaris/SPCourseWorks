#ifdef __GNUC__
#pragma implementation "tokenizer.h"
#endif
#include "tokenizer.h"

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

static bool is_whitespace(char c)
{
   return c == '\r' || c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

static bool is_delimiter(char s)
{
   return is_whitespace(s) || s == '*' || s == ',' || s == '[' || s == ']' || s == '-' || s == '+' || s == '=' || s == ':';
}

static char* trim_local_buffer(char* buffer)
{
   size_t l = strlen(buffer);
   if(l == 0)
      return buffer;

   size_t start = 0;
   size_t end = l - 1;
   while(is_whitespace(buffer[start]))
   {
      start++;
      if(start == end + 1)
         return buffer + start;
   }
   while(is_whitespace(buffer[end]))
   {
      buffer[end] = 0;
      end--;
      if(end == start)
         return buffer + start;
   }
   return buffer + start;
}

static token_t* t_create(char* string, size_t line)
{
   assert(string);
   token_t* t = malloc(sizeof(token_t));
   t->string = strdup(string);
   t->line = line;

   // Determining token type

   return t;
}

//
// t_read()
//
uint8_t* t_read(const char* fn)
{
   FILE* f = fopen(fn, "r");
   assert(f);

   fseek(f, 0, SEEK_END);
   size_t len = ftell(f);
   fseek(f, 0, SEEK_SET);

   assert(len);

   uint8_t* data = malloc(len + 1);
   data[len] = 0;

   assert(fread(data, len, 1, f) == 1);
   fclose(f);

   return data;
}

//
// t_tokenize()
//
list_t* t_tokenize(char* str)
{
   assert(str);
   size_t len = strlen(str);

   assert(len);

   char buffer[100];
   size_t buff_cnt = 0;
   size_t line_cnt = 0;
   list_t* r = list_create();

   for(size_t i = 0; i < len; i++)
   {
      char c = str[i];

      if(c == '\n')
         line_cnt++;

      if(is_delimiter(c))
      {
         // 0 terminate buffer so we can work with it
         buffer[buff_cnt] = 0;
         char* trimmed_buff = trim_local_buffer(buffer);
         if(strlen(trimmed_buff) != 0)
            list_push(r, t_create(trimmed_buff, line_cnt));

         // Make string from char
         buffer[0] = c;
         buffer[1] = 0;
         trimmed_buff = trim_local_buffer(buffer);
         if(strlen(trimmed_buff) != 0)
            list_push(r, t_create(trimmed_buff, line_cnt));

         buff_cnt = 0;
      }
      else buffer[buff_cnt++] = (char)toupper(c);
   }

   return r;
}