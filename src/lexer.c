#define STRINGVIEW_IMPLEMENTATION
#include "lexer.h"
#include "../lib/stringview.h"


static const char *separators = ";*";

Token next_token(string_view *mem_file) {
  // this persists across calls
  static string_view line = {0};
  static size_t l = 0;

  static const char *line_start = NULL;

  string_view token = {0};

  while (token.len == 0) {
    if (line.len == 0) {
      if (mem_file->len == 0)
        return (Token){0};
      else {
        string_view full_line = sv_split(mem_file, '\n');
        line = sv_trim(sv_split_any(&full_line, separators));
        line_start = line.data;
        l++;
      }
    }
    // get the first word and throw away whitespace that follows it
    token = sv_split_any(&line, " \n\t\v\f\r");
    line = sv_trim(line);
  }
    

  return (Token){
      .sv = token,
      .pos = {.row = l, .col = token.data - line_start},
  };
}

char enum_to_char_variant(unsigned variant) {
  switch (variant) {
    case CMD_G: return 'G';
    case CMD_M: return 'M';
    case CMD_T: return 'T';
    case CMD_N: return 'N';
    case CMD_D: return 'D';
    case SUBCMD_S: return 'S';
    case SUBCMD_X: return 'X';
    case SUBCMD_Y: return 'Y';
    case SUBCMD_Z: return 'Z';
    case SUBCMD_I: return 'I';
    case SUBCMD_R: return 'R';
    case SUBCMD_P: return 'P';
    case SUBCMD_E: return 'E';
    case SUBCMD_F: return 'F';
    default: return 0;
  }
}

unsigned char_variant_to_enum(char c) {
  switch (c) {
    case 'G': return CMD_G;
    case 'M': return CMD_M;
    case 'T': return CMD_T;
    case 'N': return CMD_N;
    case 'D': return CMD_D;
    case 'S': return SUBCMD_S;
    case 'X': return SUBCMD_X;
    case 'Y': return SUBCMD_Y;
    case 'Z': return SUBCMD_Z;
    case 'I': return SUBCMD_I;
    case 'R': return SUBCMD_R;
    case 'P': return SUBCMD_P;
    case 'E': return SUBCMD_E;
    case 'F': return SUBCMD_F;
    default: return INVALID_VARIANT;
  }
}

Command next_command(string_view *mem_file) {
  Token token = next_token(mem_file);
  if (!token.sv.data)
    return (Command){.variant = STREAM_END};
  file_pos pos = token.pos;

  // the first character must be the command variant.
  unsigned variant = char_variant_to_enum(*token.sv.data);
  if (variant == INVALID_VARIANT) return (Command) {.variant = variant, .value = 0.0, .pos = pos};

  // the rest of the token must be a valid floating point number.
  char *end = NULL;
  double value = strtod(token.sv.data + 1, &end);

  // the first character that is not part of a number
  // must be just outside the string_view.
  if (end > token.sv.data + token.sv.len)
    return (Command){
        .variant = LONG_LITERAL,
        .pos = pos,
    };
  if (end < token.sv.data + token.sv.len)
    return (Command){
        .variant = SHORT_LITERAL,
        .pos = pos,
    };

  return (Command){
      .variant = variant,
      .value = value,
      .pos = pos,
  };
}
