#include <iostream>

typedef struct {
} Token;

class GcodeParser {
public:
  Token next();
  GcodeParser();

private:
  FILE *source;
};


int main(int argc, char *argv[]) { return 0; }
