#ifdef __GNUC__
#pragma implementation "errors.h"
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
      fprintf(errout, "[ERROR]: Assertion \"%s\" failed. \n[ERROR]: Message: %s\n[ERROR]: Error occurred at %s:%li",
            assert_str, message, file, line);

      fflush(errout);
      fflush(stdout);
      abort();
   }
}

//
// __e_err()
//
void __e_err(const char* message, const char* file, size_t line)
{
   fprintf(errout, "[ERROR]: %s\n[ERROR]: Error occurred at %s:%li",
           message, file, line);

   fflush(errout);
   fflush(stdout);
   abort();
}

