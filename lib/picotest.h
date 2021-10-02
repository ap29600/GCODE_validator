#include "stringview.h"
#include <stdio.h>
#include <string.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define GRY "\033[90m"
#define YLW "\033[33m"
#define DEF "\033[39m"

static void int_print(const int p) { printf("(int)%d", p); }
static void float_print(const float p) { printf("(float)%f", p); }
static void double_print(const double p) { printf("(double)%g", p); }
static void char_print(const char p) { printf("(char)'%c'", p); }
static void unsigned_long_print(const unsigned long p) { printf("(int)%zu", p); }
static void unsigned_print(const unsigned p) { printf("(int)%u", p); }
static void sv_print(const string_view p) {
  printf("(string_view)\"" SV_FMT "\"", SV_ARG(p));
}

static int int_cmp(int a,  int b) {return a-b;}
static int unsigned_long_cmp(long unsigned a, long unsigned b) {return a-b;}
static int unsigned_cmp(unsigned a, unsigned b) {return a-b;}
static int char_cmp(char a,  char b) {return a-b;}
static int double_cmp(double a,  double b) {return a-b;}
static int float_cmp(float a,  float b) {return a-b;}


#define FMT(X) _Generic((X),                                                   \
    int:           int_print,                                                  \
    float:         float_print,                                                \
    double:        double_print,                                               \
    char:          char_print,                                                 \
    unsigned long: unsigned_long_print,                                        \
    unsigned:      unsigned_print,                                             \
    string_view:   sv_print                                                    \
  )(X)

#define EQ(X, Y) _Generic((X),                                                 \
    int:           int_cmp,                                                    \
    float:         float_cmp,                                                  \
    double:        double_cmp,                                                 \
    char:          char_cmp,                                                   \
    unsigned long: unsigned_long_cmp,                                          \
    unsigned:      unsigned_cmp,                                               \
    string_view:   sv_cmp                                                      \
  )(X, Y) == 0

#define ASSERT_EQ(X, Y)                                                        \
  {                                                                            \
    __typeof__(X) x = (X);                                                     \
    __typeof__(X) y = (Y);                                                     \
    assertions++;                                                              \
    if (EQ(x, y))                                                              \
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
                               : YLW,                                          \
         success, assertions);

#define INIT                                                                   \
  int assertions = 0, success = 0;                                             \
  printf(GRN "TEST \"%s\":" DEF, __func__);
