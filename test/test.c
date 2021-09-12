#include "../lib/picotest.h"
#include "../src/lexer.c"

void parses_correctly() {
  INIT;

  FILE *f = fopen("in.gcode", "r");
  string_view tokens[] = {
        sv_from_str("M201"),
        sv_from_str("X9000"),
        sv_from_str("Y9000"),
        sv_from_str("Z500"),
        sv_from_str("E10000"),
        sv_from_str("M203"),
        sv_from_str("X500"),
        sv_from_str("Y500"),
        sv_from_str("Z12"),
        sv_from_str("E120"),
        sv_from_str("M204"),
        sv_from_str("P1500"),
        sv_from_str("R1500"),
        sv_from_str("T1500"),
        sv_from_str("M205"),
        sv_from_str("X10.00"),
        sv_from_str("Y10.00"),
        sv_from_str("Z0.20"),
        sv_from_str("E2.50")
    };

  string_view sv_file = sv_slurp_stream(f);

  for (int i = 0; i <sizeof(tokens) / sizeof(tokens[0]) - 1; i++) {
    string_view token = next_token(&sv_file);
    ASSERT(!sv_cmp(token, tokens[i]))
  }

  REPORT;
}

int main() {
    parses_correctly();
}
