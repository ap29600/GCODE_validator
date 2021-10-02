#ifndef LEXER_H_
#define LEXER_H_

#include "../lib/stringview.h"

typedef struct {
  size_t row;
  size_t col;
} file_pos;

typedef struct {
  string_view sv;
  file_pos pos;
} Token;

typedef struct {
  unsigned variant;
  double value;
  file_pos pos;
} Command;

#define STREAM_END      0 // the last token was already parsed.
#define INVALID_VARIANT 1 // the token starts with an invalid character.
#define LONG_LITERAL    2 // the double literal overflows the token.
#define SHORT_LITERAL   3 // the double literal ends before the token does.

#define CMD (1U << 30)
#define CMD_G (CMD | 0U)
#define CMD_M (CMD | 1U)
#define CMD_T (CMD | 2U)
#define CMD_N (CMD | 3U)
#define CMD_D (CMD | 4U)

#define SUBCMD (1U << 31)
#define SUBCMD_S (SUBCMD | 0U)
#define SUBCMD_X (SUBCMD | 1U)
#define SUBCMD_Y (SUBCMD | 2U)
#define SUBCMD_Z (SUBCMD | 3U)
#define SUBCMD_I (SUBCMD | 4U)
#define SUBCMD_R (SUBCMD | 5U)
#define SUBCMD_P (SUBCMD | 6U)
#define SUBCMD_E (SUBCMD | 7U)
#define SUBCMD_F (SUBCMD | 8U)

#define VAL_MASK (~(SUBCMD | CMD))

static const char enum_to_char_lookup[26] = {
    [CMD_G & VAL_MASK] = 'G',
    [CMD_M & VAL_MASK] = 'M',
    [CMD_T & VAL_MASK] = 'T',
    [CMD_N & VAL_MASK] = 'N',
    [CMD_D & VAL_MASK] = 'D',
    [(SUBCMD_S & VAL_MASK) + 10 ] = 'S',
    [(SUBCMD_X & VAL_MASK) + 10 ] = 'X',
    [(SUBCMD_Y & VAL_MASK) + 10 ] = 'Y',
    [(SUBCMD_Z & VAL_MASK) + 10 ] = 'Z',
    [(SUBCMD_I & VAL_MASK) + 10 ] = 'I',
    [(SUBCMD_R & VAL_MASK) + 10 ] = 'R',
    [(SUBCMD_P & VAL_MASK) + 10 ] = 'P',
    [(SUBCMD_E & VAL_MASK) + 10 ] = 'E',
    [(SUBCMD_F & VAL_MASK) + 10 ] = 'F'
};

static const unsigned char_to_enum_lookup[26] = {
    ['G'-'A'] = CMD_G,
    ['M'-'A'] = CMD_M,
    ['T'-'A'] = CMD_T,
    ['N'-'A'] = CMD_N,
    ['D'-'A'] = CMD_D,
    ['S'-'A'] = SUBCMD_S,
    ['X'-'A'] = SUBCMD_X,
    ['Y'-'A'] = SUBCMD_Y,
    ['Z'-'A'] = SUBCMD_Z,
    ['I'-'A'] = SUBCMD_I,
    ['R'-'A'] = SUBCMD_R,
    ['P'-'A'] = SUBCMD_P,
    ['E'-'A'] = SUBCMD_E,
    ['F'-'A'] = SUBCMD_F 
};


Token next_token(string_view *mem_file);
Command next_command(string_view *mem_file);
char enum_to_char_variant(unsigned);
unsigned char_variant_to_enum(char);

#endif
