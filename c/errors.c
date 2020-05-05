#if __linux__
	#ifdef __GNUC__
	#pragma implementation "errors.h"
	#endif
#else
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "errors.h"

#include <stdlib.h>

// Output error stream
static FILE* errout;

//
// e_set_out()
//
void e_set_out(FILE* out)
{
   errout = out;
}

//
// __e_assert()
//
void __e_assert(bool v, const char* assert_str, const char* message, const char* file, size_t line)
{
   if(!v)
   {
	  FILE* output;

	  // If something went awfully wrong, just output to a stderr...
	  if (!errout) output = stderr;
	  else output = errout;

      fprintf(output, "[ERROR]: Assertion \"%s\" failed. \n[ERROR]: Message: %s\n[ERROR]: Error occurred at %s:%li",
            assert_str, message, file, line);

      fflush(output);
      fflush(stdout);
#if __linux__
      abort();
#else 
	  exit(1);
#endif
   }
}

//
// __e_err()
//
void __e_err(const char* message, const char* file, size_t line)
{
   FILE* output;

   // If something went awfully wrong, just output to a stderr...
   if (!errout) output = stderr;
   else output = errout;

   fprintf(output, "[ERROR]: %s\n[ERROR]: Error occurred at %s:%li",
           message, file, line);

   fflush(output);
   fflush(stdout);
#if __linux__
   abort();
#else 
   exit(1);
#endif
}

