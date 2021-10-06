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


void dump_whitelist(cmdlist whitelist, char *str, size_t max_size) {
    size_t remaining = max_size;
    for(size_t i = 0; i < whitelist.size; i++) {
        Command cmd = id_to_cmd(whitelist.data[i].id);
        size_t offset = snprintf(str, remaining, "%c%g:", 
                enum_to_char_variant(cmd.variant), cmd.value);
        str += offset;
        remaining -= offset;
        for(size_t j = 0; j < 8 * sizeof(whitelist.data->subcommands); j++) {
            if ((whitelist.data[i].subcommands >> j) & 1) {
                offset = snprintf(str, remaining," %c", 
                        enum_to_char_variant((cmd_type){SUBCMD, j}));
                str += offset;
                remaining -= offset;
            }
        }
        offset = snprintf(str, remaining,"\n");
        str += offset;
        remaining -= offset;
    }
}

cmdlist build_whitelist(string_view source) {
    cmdlist result = {0};
    Command cmd;
    long int current_cmd_index= -1;

    while ((cmd = next_command(&source)).variant.kind != ERR || cmd.variant.name != STREAM_END) {
        size_t id;
        list_entry e;
        list_entry *matching_entry;
        switch (cmd.variant.kind) {
            case CMD: 
                id = cmd_to_id(cmd);
                e = (list_entry){.id = id};
                matching_entry = 
                    bsearch(&e, result.data, result.size, sizeof(list_entry), cmp_ids);
                if (matching_entry)
                    current_cmd_index = matching_entry - result.data;
                else 
                    current_cmd_index = insert(&result, e);

                break;
            case SUBCMD: 
                if (current_cmd_index < 0) {
                    fprintf(stderr, "ERROR: option (%c%g) with no preceding command at %zu:%zu\n",
                            cmd_to_char(cmd.variant), cmd.value, cmd.pos.row, cmd.pos.col);
                    exit(1);
                } else 
                    result.data[current_cmd_index].subcommands |= (1 << (cmd.variant.name));
                break;
            case ERR:
                fprintf(stderr, "ERROR: bad command encountered in whitelist at %zu:%zu",
                        cmd.pos.row, cmd.pos.col);
                // TODO: clean this up to decouple from enum definition
                fprintf(stderr, " with error '%s'\n", (char *[]){
                        "STREAM_END",
                        "INVALID_VARIANT",
                        "LONG_LITERAL",
                        "SHORT_LITERAL"} [cmd.variant.name]
                        );
                exit(1);
            default:
                fprintf(stderr, "ERROR: corrupted data encountered (presumably at %zu:%zu)\n", 
                        cmd.pos.row, cmd.pos.col);
                exit(1);
        }
    }
    return result; 
}
