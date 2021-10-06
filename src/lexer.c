#define STRINGVIEW_IMPLEMENTATION
#include "lexer.h"
#include "../lib/stringview.h"


static const char *separators = ";*";

Token next_token(string_view *mem_file) {
  // this persists across calls
  //
  static const char *line_start = NULL;
  static string_view line = {0};
  static int is_first = 1;
  static size_t line_num = 0;

  if (mem_file == NULL) {
      line_start = NULL;
      line = (string_view){0};
      is_first = 1;
      line_num = 0;
      return (Token) {0};
  }

  string_view token = {0};

  while (token.len == 0) {
    if (line.len == 0) {
      if (mem_file->len == 0)
        return (Token){0};
      else {
        string_view full_line = sv_split(mem_file, '\n');
        line = sv_trim(sv_split_any(&full_line, separators));
        line_start = line.data;
        is_first = 1;
        line_num++;
      }
    }
    // get the first word and throw away whitespace that follows it
    token = sv_split_any(&line, " \n\t\v\f\r");
    line = sv_trim(line);
  }

  Token result = {
          .sv = token,
          .pos = {.row = line_num, .col = token.data - line_start},
          .is_first = is_first,
      };

  is_first = 0;

  // after line numbers we should interpret everything as toplevel.
  is_first = token.len && *token.data == 'N';
  
  return result;
}

char cmd_to_char(cmd_type variant) {
  switch (variant.kind) {
      case CMD: 
          return (variant.name) < sizeof cmd_to_char_table ? 
              cmd_to_char_table[variant.name] : 0;
      case SUBCMD:
          return (variant.name) < sizeof subcmd_to_char_table ?
              subcmd_to_char_table[variant.name] : 0;
      default:
          return 0;
  };
}

cmd_type char_to_cmd(char c) {
    return (c >= 'A') && ((size_t)(c - 'A') < sizeof(char_to_cmd_table) / sizeof(char_to_cmd_table[0])) ? 
        char_to_cmd_table[c - 'A'] :
        (cmd_type){ERR, INVALID_VARIANT};
}

cmd_type char_to_subcmd(char c) {
    return (c >= 'A') && ((size_t)(c - 'A') < sizeof(char_to_subcmd_table) / sizeof(char_to_subcmd_table[0])) ? 
        char_to_subcmd_table[c - 'A'] :
        (cmd_type){ERR, INVALID_VARIANT};
}

Command next_command(string_view *mem_file) {
  Token token = next_token(mem_file);
  if (!token.sv.data)
    return (Command){.variant = {ERR, STREAM_END}};
  file_pos pos = token.pos;

  // TODO: detect G and M commands following each other on the same line.
  cmd_type variant = token.is_first ?
      char_to_cmd(*token.sv.data) :
      char_to_subcmd(*token.sv.data);

  if (variant.kind == ERR) 
      return (Command) {
          .variant = variant, 
          .value = 0.0,
          .pos = pos,
      };

  double value;

  if (variant.kind == CMD && variant.name == CMD_N) {
      // avoid generating multiple entries for line number commands
      value = 0;    
  } else {
      // the rest of the token must be a valid floating point number.
      char *end = NULL;
      value = strtod(token.sv.data + 1, &end);

      // the first character that is not part of a number
      // must be just outside the string_view.
      if (end > token.sv.data + token.sv.len)
        return (Command){
            .variant = {ERR, LONG_LITERAL},
            .pos = pos,
        };
      if (end < token.sv.data + token.sv.len)
        return (Command){
            .variant = {ERR, SHORT_LITERAL},
            .pos = pos,
        };
  }

  return (Command){
      .variant = variant,
      .value = value,
      .pos = pos,
  };
}
