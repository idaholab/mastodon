#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <vector>

//-------------------------- MACRO UTILITIES BEGIN ---------------------
// A custom assert block for assertions
#define ASSERT(condition, statement, ...)                                                          \
  if (!(condition))                                                                                \
  {                                                                                                \
    fprintf(stderr,                                                                                \
            "[ASSERT] In File: %s, Line: %d => " #statement "\n",                                  \
            __FILE__,                                                                              \
            __LINE__,                                                                              \
            ##__VA_ARGS__);                                                                        \
    abort();                                                                                       \
  }
#endif // _UTILS_H
