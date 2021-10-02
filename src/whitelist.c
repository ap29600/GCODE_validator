#include "whitelist.h"

int cmp_ids (const void*a, const void*b) {
    const unsigned a_id = ((const list_entry*)a)->id;
    const unsigned b_id = ((const list_entry*)b)->id;
    return (a_id > b_id) - (b_id > a_id) ;
}

static long int insert(cmdlist *result, list_entry e) {
    if (result->capacity < result->size + 1) {
        result->capacity = 
            result->capacity ?
            (result->capacity * 2):
            100;
        result->data = realloc(result->data, result->capacity);
    }

    size_t pos = result->size++;
    result->data[pos] = e;
    while(pos > 0 && result->data[pos].id < result->data[pos-1].id) {
        list_entry tmp = result->data[pos];
        result->data[pos] = result->data[pos-1];
        result->data[pos-1] = tmp;
        pos--;
    }
    return pos;
}

void dump_whitelist(cmdlist whitelist, char *str) {
    for(size_t i = 0; i < whitelist.size; i++) {
        size_t offset = sprintf(str, "%c%g:", 
                enum_to_char_variant((whitelist.data[i].id / 2000U) ^ CMD),
                (whitelist.data[i].id % 2000) / 2.0); 
        str += offset;
        for(size_t j = 0; j < 8 * sizeof(whitelist.data->subcommands); j++) {
            if ((whitelist.data[i].subcommands >> j) & 1) {
                offset = sprintf(str, " %c", enum_to_char_variant(SUBCMD | j));
                str += offset;
            }
        }
        offset = sprintf(str, "\n");
        str += offset;
    }
}

cmdlist build_whitelist(string_view source) {
    cmdlist result = {0};
    Command cmd;
    long int current_cmd_index= -1;
    while ((cmd = next_command(&source)).variant != STREAM_END) {
        if ((cmd.variant & (CMD | SUBCMD)) == 0) {
            fprintf(stderr, "ERROR: bad command encountered in whitelist at %zu:%zu",
                    cmd.pos.row, cmd.pos.col);
            // TODO: clean this up to decouple from enum definition
            fprintf(stderr, " with error '%s'\n", (char *[]){
                    "STREAM_END",
                    "INVALID_VARIANT",
                    "LONG_LITERAL",
                    "SHORT_LITERAL"} [cmd.variant]
                    );
            exit(1);
        }

        if (cmd.variant & CMD) {
            size_t id = cmd_to_id(cmd);

            list_entry e = {.id = id};
            list_entry *matching_entry = 
                bsearch(&e, result.data, result.size, sizeof(list_entry),cmp_ids);

            if (!matching_entry)
                current_cmd_index = insert(&result, e);
            else 
                current_cmd_index = matching_entry - result.data;
        } else /* SUBCMD */{
            if (current_cmd_index < 0) {
                fprintf(stderr, "ERROR: option with no preceding command at %zu:%zu\n",
                        cmd.pos.row, cmd.pos.col);
                exit(1);
            } else {
                result.data[current_cmd_index].subcommands |= (1 << (cmd.variant & VAL_MASK));
            }
        }
    }
    return result; 
}
