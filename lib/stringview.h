#ifndef STRINGVIEW_H_
#define STRINGVIEW_H_
#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/////////////////////////////////////////////////////////////
// macros

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)(sv).len, (sv).data

/////////////////////////////////////////////////////////////

typedef struct {
  size_t len;
  const char *data;
} string_view;

string_view sv_from_stream(FILE *stream);
string_view sv_from_str(const char *data);
string_view sv_from_nstr(const char *data, size_t len);
string_view sv_trim(string_view sv);
string_view sv_split(string_view *sv, char delim);
string_view sv_split_predicate(string_view *sv, int pred(int));
string_view sv_split_escaped(string_view *sv, char delim);
string_view sv_split_any(string_view *sv, const char *delims);
int sv_len_utf_8(string_view s);
int sv_cmp(string_view a, string_view b);
int sv_starts_with(string_view a, char *b);

#endif // STRINGVIEW_H_

#ifdef STRINGVIEW_IMPLEMENTATION

string_view sv_from_str(const char *data) {
  return (string_view){.len = strlen(data), .data = data};
}

string_view sv_from_nstr(const char *data, size_t len) {
  size_t data_len = strlen(data);
  if (data_len < len)
    len = data_len;
  return (string_view){.len = len, .data = data};
}

string_view sv_trim(string_view sv) {
  if (!sv.data)
    return sv;
  while (isspace(sv.data[0]) && sv.len > 0) {
    sv.data++;
    sv.len--;
  }
  while (isspace(sv.data[sv.len - 1]) && sv.len > 0)
    sv.len--;
  return sv;
}

string_view sv_split(string_view *sv, char delim) {
  const char *data = sv->data;
  size_t len;
  while (sv->data[0] != delim && sv->len > 0) {
    sv->data++;
    sv->len--;
  }
  if (sv->len > 0) { // get rid of the delimiter
    sv->data++;
    sv->len--;
    len = sv->data - data - 1;
  } else {
    len = sv->data - data;
  }
  return (string_view){.len = len, .data = data};
}

string_view sv_split_predicate(string_view *sv, int pred(int)) {
  const char *data = sv->data;
  size_t len;
  while (!pred(sv->data[0]) && sv->len > 0) {
    sv->data++;
    sv->len--;
  }
  if (sv->len > 0) { // get rid of the delimiter
    sv->data++;
    sv->len--;
    len = sv->data - data - 1;
  } else {
    len = sv->data - data;
  }
  return (string_view){.len = len, .data = data};
}

string_view sv_split_escaped(string_view *sv, char delim) {
  const char *data = sv->data;
  char quote = 0;
  int escaped_char = 0;
  size_t len;
  while ((*sv->data != delim || quote || escaped_char) && sv->len > 0) {
    if (escaped_char)
      escaped_char = 0;
    else {
      if (quote) {
        if (*sv->data == quote)
          quote = 0;
      } else if (*sv->data == '"' || *sv->data == '`') {
        quote = *sv->data;
      }
      if (*sv->data == '\\')
        escaped_char = 1;
    }
    sv->data++;
    sv->len--;
  }
  if (sv->len > 0) { // get rid of the delimiter
    sv->data++;
    sv->len--;
    len = sv->data - data - 1;
  } else {
    len = sv->data - data;
  }
  return (string_view){.len = len, .data = data};
}

int sv_cmp(string_view a, string_view b) {
  return a.len == b.len ? strncmp(a.data, b.data, a.len) : 1;
}

int sv_starts_with(string_view a, char *b) {
  return !strncmp(a.data, b, strlen(b));
}

string_view sv_slurp_stream(FILE *stream) {
  string_view ret = {0};
  char *data = NULL;
  size_t capacity = 0;
  if (isatty(fileno(stream))) {
    char c;
    while (EOF != (c = getc(stream))) {
      if (capacity < ret.len + 1) {
        capacity = (capacity == 0) ? 1024 : capacity * 2;
        data = (char *)realloc(data, capacity);
        if (!data)
          return (string_view){0};
      }
      data[ret.len++] = c;
    }
  } else {
    if (fseek(stream, 0, SEEK_END) < 0)
      printf("Error: %s\n", strerror(ferror(stream)));

    ret.len = ftell(stream);

    if (fseek(stream, 0, SEEK_SET) < 0)
      printf("Error: %s\n", strerror(ferror(stream)));

    data = (char *)malloc(ret.len);
    fread(data, ret.len, 1, stream);
  }
  ret.data = data;
  return ret;
}

int sv_len_utf_8(string_view s) {
  int ret = 0;
  for (; s.len > 0; s.len--)
    if (((*s.data++) & 0xc0) != 0x80)
      ret++;
  return ret;
}

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

#undef STRINGVIEW_IMPLEMENTATION
#endif // STRINGVIEW_IMPLEMENTATION
