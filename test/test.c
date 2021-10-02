#define _XOPEN_SOURCE 700
#include "../lib/picotest.h"
#include "../src/lexer.c"
#include "../src/whitelist.c"

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
  const char * begin = sv_file.data;
  fclose(f);
  for (size_t i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++) {
    Token token = next_token(&sv_file);
    ASSERT_EQ(token.sv, sv_from_str(tokens[i]))
  }

  free((char*)begin);
  REPORT;
}

void token_decoding() {
  INIT;
  Command commands[] = {
      {CMD_M,    201,   {0, 0}},
	  {SUBCMD_X, 9000,  {0, 0}},
      {SUBCMD_Y, 9000,  {0, 0}},
	  {SUBCMD_Z, 500,   {0, 0}},
      {SUBCMD_E, 10000, {0, 0}},
	  {CMD_M,    203,   {0, 0}},
      {SUBCMD_X, 500,   {0, 0}},
	  {SUBCMD_Y, 500,   {0, 0}},
      {SUBCMD_Z, 12,    {0, 0}},
	  {SUBCMD_E, 120,   {0, 0}},
      {CMD_M,    204,   {0, 0}},
	  {SUBCMD_P, 1500,  {0, 0}},
      {SUBCMD_R, 1500,  {0, 0}},
	  {CMD_T,    1500,  {0, 0}},
      {CMD_M,    205,   {0, 0}},
	  {SUBCMD_X, 10.00, {0, 0}},
      {SUBCMD_Y, 10.00, {0, 0}},
	  {SUBCMD_Z, 0.20,  {0, 0}},
      {SUBCMD_E, 2.50,  {0, 0}}
    };

  FILE *f = fopen("bin/in.gcode", "r");
  string_view sv_file = sv_slurp_stream(f);
  const char * begin = sv_file.data;
  fclose(f);

  for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    Command command = next_command(&sv_file);
    ASSERT_EQ(command.variant, commands[i].variant);
    ASSERT_EQ(command.value, commands[i].value);

    if (command.variant != commands[i].variant) {
        printf("%zu\n", i);
    }
  }

  free((char*)begin);
  REPORT;
}

void whitelist_generation() {
  INIT;

  string_view sv = sv_from_str(
          "M104 S235 ; set temperature\n"
          "G28 ; home all axes\n"
          "G1 Z5 F5000 ; lift nozzle\n"
          "M109 S235 ; set temperature and wait for it to be reached\n"
          "G21 ; set units to millimeters\n"
          "G90 ; use absolute coordinates\n"
          "M82 ; use absolute distances for extrusion\n"
          "G92 E0\n"
          "; Filament gcode\n"
          "G1 Z0.350 F7800.000\n"
          "G1 E-2.00000 F2400.00000\n"
          "G92 E0\n"
          "G1 X21.102 Y-27.921 F7800.000\n"
          "G1 E2.00000 F2400.00000\n"
          "G1 F1800\n"
          "G1 X21.867 Y-26.564 E2.06685\n"
          "G1 X22.200 Y-25.450 E2.11671\n"
          "G1 X22.318 Y-24.294 E2.16658\n"
          "G1 X22.311 Y36.056 E4.75624\n"
          "G1 X22.206 Y36.919 E4.79353\n"
          "G1 X21.981 Y37.759 E4.83083\n");

  const char * expected_whitelist = 
          "G1: X Y Z E F\n"
          "G21:\n"
          "G28:\n"
          "G90:\n"
          "G92: E\n"
          "M82:\n"
          "M104: S\n"
          "M109: S\n";

  cmdlist l = build_whitelist(sv);
  char list_str[1024];
  dump_whitelist(l, list_str);
  ASSERT_EQ(sv_from_str(list_str), sv_from_str(expected_whitelist));
  REPORT;
}

int main() {
  tokenize_file();
  token_decoding();
  whitelist_generation();
}
