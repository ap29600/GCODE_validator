#ifndef WHITELIST_H_
#define WHITELIST_H_

#include "lexer.h"

typedef struct {
  unsigned id;
  unsigned subcommands;
} list_entry;

typedef struct {
  list_entry *data;
  size_t size;
  size_t capacity;
} cmdlist;

cmdlist build_whitelist(string_view whitelist);
int cmp_ids(const void *a, const void *b); // useful for bsearch

static inline unsigned cmd_to_id(Command cmd) {
  return ((cmd.variant & VAL_MASK) << 16) | ((int)(cmd.value * 10) & 0xffff);
}

static inline Command id_to_cmd(unsigned u) {
  return (Command){
      .variant = (u >> 16) | CMD,
      .value = (double)(u & 0xffff) / 10.0,
       // A command obtained in this way 
       // doesn't have a file position
      .pos = {0},
  };
}


void dump_whitelist(cmdlist whitelist, char *str, size_t max_size);

#endif
