#include <stdio.h>
#include <string.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define GRY "\033[90m"
#define YLW "\033[33m"
#define DEF "\033[39m"

#define FMT(X)                                                                 \
  printf(_Generic((X), int                                                     \
                  : "(int)%d", float                                           \
                  : "(float)%f", double                                        \
                  : "(double)%g", char                                         \
                  : "(char)%c", unsigned long                                  \
                  : "(unsigned long)%lu"),                                     \
         X)

#define ASSERT_EQ(X, Y)                                                        \
  {                                                                            \
    __typeof__(X) x = (X);                                                     \
    __typeof__(Y) y = (Y);                                                     \
    assertions++;                                                              \
    if (x == y)                                                                \
      success++;                                                               \
    else {                                                                     \
      if (assertions - success == 1)                                           \
        printf("\n");                                                          \
      printf("\t" RED "ASSERTION %d FAILED: " GRY "( " DEF #X GRY              \
             " ) == ( " DEF #Y GRY " )\n",                                     \
             assertions);                                                      \
      printf("\t\tLHS: " DEF);                                                 \
      FMT(x);                                                                  \
      printf("\n");                                                            \
      printf(GRY "\t\tRHS: " DEF);                                             \
      FMT(y);                                                                  \
      printf("\n");                                                            \
    }                                                                          \
  }

#define ASSERT(X)                                                              \
  {                                                                            \
    assertions++;                                                              \
    if (X)                                                                     \
      success++;                                                               \
    else {                                                                     \
      if (assertions - success == 1)                                           \
        printf("\n");                                                          \
      printf("\t" RED "ASSERTION FAILED: " GRY "( " DEF #X GRY " )\n" DEF);    \
    }                                                                          \
  }

#define REPORT                                                                 \
  printf("\t%s%d / %d PASSED." DEF "\n",                                       \
         success == assertions ? GRN                                           \
         : success == 0        ? RED                                           \
                               : YLW,                                                 \
         success, assertions);

#define INIT                                                                   \
  int assertions = 0, success = 0;                                             \
  printf(GRN "TEST \"%s\":" DEF, __func__);
