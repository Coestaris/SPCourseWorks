#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdbool.h>

#ifndef e_assert
#define e_assert(v, s) __e_assert(v, #v, s, __FILE__, __LINE__)
#endif

#ifndef e_err
#define e_err(s) __e_err(s, __FILE__, __LINE__)
#endif

// Sets error output stream
void e_set_out(FILE* errout);

// Dynamic assert function that outputs error message when something went wrong
void __e_assert(bool v, const char* assert_str, const char* message, const char* file, size_t line);

// Outputs error and aborts program
void __e_err(const char* message, const char* file, size_t line);

#endif // ERRORS_H
