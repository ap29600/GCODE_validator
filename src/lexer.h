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
  int is_first;
} Token;


typedef enum {
    STREAM_END,      // the last token was already parsed.
    INVALID_VARIANT, // the token starts with an invalid character.
    LONG_LITERAL,    // the double literal overflows the token.
    SHORT_LITERAL,   // the double literal ends before the token does.
} err_name;

typedef enum {
    CMD_G = 0, 
    CMD_M, 
    CMD_T, 
    CMD_N, 
    CMD_D,
} cmd_name;

typedef enum {
    SUBCMD_S = 0, 
    SUBCMD_X, 
    SUBCMD_Y, 
    SUBCMD_Z, 
    SUBCMD_I,
    SUBCMD_R,
    SUBCMD_P,
    SUBCMD_E,
    SUBCMD_F,
    SUBCMD_T,
} subcmd_name;

#define ERR 0
#define CMD 1
#define SUBCMD 2

typedef struct {
    unsigned kind : 2;
    unsigned name : 30;
} cmd_type;

typedef struct {
  cmd_type variant;
  double value;
  file_pos pos;
} Command;

static const char cmd_to_char_table[] = {
    [CMD_G] = 'G',
    [CMD_M] = 'M',
    [CMD_T] = 'T',
    [CMD_N] = 'N',
    [CMD_D] = 'D',
};

static const char subcmd_to_char_table[] = {
    [SUBCMD_S] = 'S',
    [SUBCMD_X] = 'X',
    [SUBCMD_Y] = 'Y',
    [SUBCMD_Z] = 'Z',
    [SUBCMD_I] = 'I',
    [SUBCMD_R] = 'R',
    [SUBCMD_P] = 'P',
    [SUBCMD_E] = 'E',
    [SUBCMD_F] = 'F',
    [SUBCMD_T] = 'T',
};

static const cmd_type char_to_cmd_table[] = {
    ['G'-'A'] = {CMD, CMD_G},
    ['M'-'A'] = {CMD, CMD_M},
    ['T'-'A'] = {CMD, CMD_T},
    ['N'-'A'] = {CMD, CMD_N},
    ['D'-'A'] = {CMD, CMD_D},
};

static const cmd_type char_to_subcmd_table[] = {
    ['S'-'A'] = {SUBCMD, SUBCMD_S},
    ['X'-'A'] = {SUBCMD, SUBCMD_X},
    ['Y'-'A'] = {SUBCMD, SUBCMD_Y},
    ['Z'-'A'] = {SUBCMD, SUBCMD_Z},
    ['I'-'A'] = {SUBCMD, SUBCMD_I},
    ['R'-'A'] = {SUBCMD, SUBCMD_R},
    ['P'-'A'] = {SUBCMD, SUBCMD_P},
    ['E'-'A'] = {SUBCMD, SUBCMD_E},
    ['F'-'A'] = {SUBCMD, SUBCMD_F},
    ['T'-'A'] = {SUBCMD, SUBCMD_T},
};


Token next_token(string_view *mem_file);
Command next_command(string_view *mem_file);

cmd_type char_to_cmd(char);
cmd_type char_to_subcmd(char);


char cmd_to_char(cmd_type);

inline char enum_to_char_variant(cmd_type variant) {
    return cmd_to_char(variant);
}
#endif
