#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#define loop for (;;)

char *trim(char *string);
size_t countChar(const char *const string, const char character);
size_t countNChar(
    const char *const string,
    const char character,
    const size_t nBytes);

#endif // !UTILS_H
