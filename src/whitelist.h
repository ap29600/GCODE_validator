#ifndef WHITELIST_H_
#define WHITELIST_H_

#include "lexer.h"

typedef struct {
    unsigned id;
    unsigned subcommands;
} list_entry;

typedef struct {
    list_entry * data;
    size_t size;
    size_t capacity;
} cmdlist;

cmdlist build_whitelist(string_view whitelist);
int cmp_ids (const void*a, const void*b); // useful for bsearch
static inline unsigned cmd_to_id(Command cmd) {
    return (cmd.variant ^ CMD) * 2000 + (int)(cmd.value * 2);
}
void dump_whitelist(cmdlist whitelist, char *str);

#endif
