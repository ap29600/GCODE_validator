#define _XOPEN_SOURCE 700
#include "../lib/picotest.h"
#include "../src/lexer.c"
#include "../src/whitelist.c"

const char gcode_buf[] =
    "M201 X9000 Y9000 Z500 E10000 ; sets maximum accelerations, mm/sec^2\n"
    "M203 X500 Y500 Z12 E120 ; sets maximum feedrates, mm/sec\n"
    "M204 P1500 R1500 T1500 ; sets acceleration (P, T) and retract acceleration (R), mm/sec^2\n"
    "M205 X10.00 Y10.00 Z0.20 E2.50 ; sets the jerk limits, mm/sec\n"
    "M205 S0 T0 ; sets the minimum extruding and travel feed rate, mm/sec\n"
    "M107\n"
    "M190 S70 ; set bed temperature and wait for it to be reached\n"
    "M104 S235 ; set temperature\n"
    "G28 ; home all axes\n"
    "G1 Z5 F5000 ; lift nozzle\n"
    "M109 S235 ; set temperature and wait for it to be reached\n"
    "G21 ; set units to millimeters\n"
    "G90 ; use absolute coordinates\n"
    "M83 ; use absolute distances for extrusion\n"
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
    "G1 X21.981 Y37.759 E4.83083\n";

const string_view const_gcode_slice = { .data = gcode_buf, .len = sizeof gcode_buf - 1 };

const char * const expected_tokens[] = {
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

const Command expected_commands[] = {
  {{CMD, CMD_M},       201,   {0, 0}},
  {{SUBCMD, SUBCMD_X}, 9000,  {0, 0}},
  {{SUBCMD, SUBCMD_Y}, 9000,  {0, 0}},
  {{SUBCMD, SUBCMD_Z}, 500,   {0, 0}},
  {{SUBCMD, SUBCMD_E}, 10000, {0, 0}},
  {{CMD, CMD_M},       203,   {0, 0}},
  {{SUBCMD, SUBCMD_X}, 500,   {0, 0}},
  {{SUBCMD, SUBCMD_Y}, 500,   {0, 0}},
  {{SUBCMD, SUBCMD_Z}, 12,    {0, 0}},
  {{SUBCMD, SUBCMD_E}, 120,   {0, 0}},
  {{CMD, CMD_M},       204,   {0, 0}},
  {{SUBCMD, SUBCMD_P}, 1500,  {0, 0}},
  {{SUBCMD, SUBCMD_R}, 1500,  {0, 0}},
  {{SUBCMD, SUBCMD_T}, 1500,  {0, 0}},
  {{CMD, CMD_M},       205,   {0, 0}},
  {{SUBCMD, SUBCMD_X}, 10.00, {0, 0}},
  {{SUBCMD, SUBCMD_Y}, 10.00, {0, 0}},
  {{SUBCMD, SUBCMD_Z}, 0.20,  {0, 0}},
  {{SUBCMD, SUBCMD_E}, 2.50,  {0, 0}}
};

const list_entry expected_whitelist[] = {
  { ID_OF( CMD_G, 1.0 ), (1<<SUBCMD_X)|(1<<SUBCMD_Y)|(1<<SUBCMD_Z)|(1<<SUBCMD_E)|(1<<SUBCMD_F) },
  { ID_OF( CMD_G, 21.0  ), 0               },
  { ID_OF( CMD_G, 28.0  ), 0               },
  { ID_OF( CMD_G, 90.0  ), 0               },
  { ID_OF( CMD_G, 92.0  ), (1 << SUBCMD_E) },
  { ID_OF( CMD_M, 82.0  ), 0               },
  { ID_OF( CMD_M, 104.0 ), (1 << SUBCMD_S) },
  { ID_OF( CMD_M, 107.0 ), 0               },
  { ID_OF( CMD_M, 109.0 ), (1 << SUBCMD_S) },
  { ID_OF( CMD_M, 190.0 ), (1 << SUBCMD_S) },
  { ID_OF( CMD_M, 201.0 ), (1<<SUBCMD_X)|(1<<SUBCMD_Y)|(1<<SUBCMD_Z)|(1<<SUBCMD_E) },
  { ID_OF( CMD_M, 203.0 ), (1<<SUBCMD_X)|(1<<SUBCMD_Y)|(1<<SUBCMD_Z)|(1<<SUBCMD_E) },
  { ID_OF( CMD_M, 204.0 ), (1<<SUBCMD_P)|(1<<SUBCMD_R)|(1<<SUBCMD_T) },
  { ID_OF( CMD_M, 205.0 ), (1<<SUBCMD_X)|(1<<SUBCMD_Y)|(1<<SUBCMD_Z)|(1<<SUBCMD_E)|(1<<SUBCMD_S)|(1<<SUBCMD_T) },
};

void tokenize_file() {
  INIT;

  // reset internal state
  next_token(NULL);

  size_t len = sizeof expected_tokens / sizeof expected_tokens[0];
  string_view gcode = const_gcode_slice;
  for (size_t i = 0; i < len; i++) {
    Token token = next_token(&gcode);
    ASSERT_EQ(token.sv, sv_from_str(expected_tokens[i]))
  }

  REPORT;
}


void token_decoding() {
  INIT;

  // reset internal state
  next_token(NULL);
  size_t len = sizeof expected_commands / sizeof expected_commands[0];
  string_view gcode = const_gcode_slice;

  for (size_t i = 0; i < len; i++) {
    Command command = next_command(&gcode);

    ASSERT_EQ((unsigned)command.variant.kind, expected_commands[i].variant.kind);
    ASSERT_EQ((unsigned)command.variant.name, expected_commands[i].variant.name);
    ASSERT_EQ(command.value, expected_commands[i].value);
  }

  REPORT;
}


void whitelist_generation() {
  INIT;

  // reset internal state
  next_token(NULL);
  size_t len = sizeof expected_whitelist / sizeof expected_whitelist[0];
  string_view gcode = const_gcode_slice;

  cmdlist generated_whitelist = build_whitelist(gcode);

  ASSERT_EQ (generated_whitelist.size, len);
  if (generated_whitelist.size < len)
      len = generated_whitelist.size;

  for (size_t i = 0; i < len; i++) {
      Command cmd = id_to_cmd(generated_whitelist.data[i].id);
      Command expected_cmd = id_to_cmd(expected_whitelist[i].id);
      ASSERT_EQ((unsigned)cmd.variant.kind, expected_cmd.variant.kind);
      ASSERT_EQ((unsigned)cmd.variant.name, expected_cmd.variant.name);
      ASSERT_EQ(cmd.value, expected_cmd.value);
      ASSERT_EQ(generated_whitelist.data[i].subcommands, expected_whitelist[i].subcommands);
  }

  REPORT;
}

int main() {
  tokenize_file();
  token_decoding();
  whitelist_generation();
}
