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

Token next_token(string_view *mem_file);
Command next_command(string_view *mem_file);
char enum_to_char_variant(unsigned);
unsigned char_variant_to_enum(char);

#endif
