#define _XOPEN_SOURCE 700
#include "../lib/picotest.h"
#include "../src/lexer.c"

void tokenize_file() {
  INIT;

  FILE *f = fopen("bin/in.gcode", "r");
  char *tokens[] = {
      "M201",   
      "X9000",
      "Y9000",
      "Z500",
      "E10000",
      "M203",
      "X500",
      "Y500",
      "Z12",
      "E120",
      "M204",
      "P1500",
      "R1500",
      "T1500",
      "M205",
      "X10.00",
      "Y10.00",
      "Z0.20",
      "E2.50"
    };

  string_view sv_file = sv_slurp_stream(f);
  fclose(f);
  for (size_t i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++) {
    Token token = next_token(&sv_file);
    string_view sv = token.sv;
    ASSERT(!sv_cmp(sv, sv_from_str(tokens[i])))
  }

  REPORT;
}

void token_decoding() {
  INIT;
  Command commands[] = {
      {'M', 201,   {0, 0}},
	  {'X', 9000,  {0, 0}},
      {'Y', 9000,  {0, 0}},
	  {'Z', 500,   {0, 0}},
      {'E', 10000, {0, 0}},
	  {'M', 203,   {0, 0}},
      {'X', 500,   {0, 0}},
	  {'Y', 500,   {0, 0}},
      {'Z', 12,    {0, 0}},
	  {'E', 120,   {0, 0}},
      {'M', 204,   {0, 0}},
	  {'P', 1500,  {0, 0}},
      {'R', 1500,  {0, 0}},
	  {'T', 1500,  {0, 0}},
      {'M', 205,   {0, 0}},
	  {'X', 10.00, {0, 0}},
      {'Y', 10.00, {0, 0}},
	  {'Z', 0.20,  {0, 0}},
      {'E', 2.50,  {0, 0}}
    };

  FILE *f = fopen("bin/in.gcode", "r");
  string_view sv_file = sv_slurp_stream(f);
  fclose(f);

  for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    Command command = next_command(&sv_file);
    ASSERT_EQ(command.variant, commands[i].variant);
    ASSERT_EQ(command.value, commands[i].value);

    if (command.variant != commands[i].variant) {
        printf("%zu\n", i);
    }
  }

  REPORT;
}

int main() {
  tokenize_file();
  token_decoding();
}
