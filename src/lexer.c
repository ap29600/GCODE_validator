#define STRINGVIEW_IMPLEMENTATION
#include "../lib/stringview.h"

const char *separators = ";*";

string_view sv_split_any(string_view *sv, const char *delims) {
  const char *data = sv->data;
  size_t len;

  while (sv->len > 0) {
    for (size_t i = 0; delims[i]; i++)
      if (sv->data[0] == delims[i])
        goto FOUND;
    sv->data++;
    sv->len--;
  }
FOUND:

  len = sv->data - data;

  if (sv->len > 0) { // get rid of the delimiter
    sv->data++;
    sv->len--;
  } else
    // if delimiter is not found, the rest of the line
    // should be an invalid string_view
    sv->data = NULL;

  return (string_view){.len = len, .data = data};
}

typedef struct {
  size_t row;
  size_t col;
} file_pos;

typedef struct {
  string_view sv;
  file_pos pos;
} Token;

Token next_token(string_view *mem_file) {
  // this persists across calls
  static string_view line = {0};
  static size_t l = 0;

  const char *line_start = NULL;

  string_view token = {0};

  while (token.len == 0) {
    if (line.len == 0) {
      if (mem_file->len == 0)
        return (Token){0};
      else {
        string_view line_full = sv_split(mem_file, '\n');
        line = sv_trim(sv_split_any(&line_full, "*;"));
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

// since command variants are characters, we can have up to
// 64 enum values in the variant field to be interpreted as
// error types.

#define STREAM_END 0
#define INVALID_VARIANT 1
#define LONG_LITERAL 2
#define SHORT_LITERAL 3

typedef struct {
  char variant;
  double value;
  file_pos pos;
} Command;

Command next_command(string_view *mem_file) {
  Token token = next_token(mem_file);
  if (!token.sv.data)
    return (Command){
        .variant = STREAM_END,
    };

  // the first character must be the command variant.
  if (!strchr("GMNSXYZIRPTE", *token.sv.data))
    return (Command){
        .variant = INVALID_VARIANT,
        .pos = token.pos,
    };

  char variant = *token.sv.data;

  // the rest of the token must be a valid floating point number.
  char *end = NULL;
  double value = strtod(token.sv.data + 1, &end);

  // the first character that is not part of a number
  // must be just outside the string_view.
  if (end > token.sv.data + token.sv.len)
    return (Command){
        .variant = LONG_LITERAL,
        .pos = token.pos,
    };
  if (end < token.sv.data + token.sv.len)
    return (Command){
        .variant = SHORT_LITERAL,
        .pos = token.pos,
    };

  return (Command){
      .variant = variant,
      .value = value,
      .pos = token.pos,
  };
}
