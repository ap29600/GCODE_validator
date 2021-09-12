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

string_view next_token(string_view *mem_file) {
  // this persists across calls
  static string_view line = {0};

  string_view token = {0}, token_tmp = {0};

  while (token.len == 0) {
    if (line.len == 0) {
      if (mem_file->len == 0)
        return (string_view){0};
      else
        line = sv_trim(sv_split(mem_file, '\n'));
    }

    // get the first word and throw away whitespace that follows it
    token_tmp = sv_split_predicate(&line, isspace);
    line = sv_trim(line);

    token = sv_split_any(&token_tmp, separators);
    // token_data is NULLed if the delimiter is not found,
    // so this check only passes if it is found.
    if (token_tmp.data)
      line = (string_view){0};
  }

  return token;
}

