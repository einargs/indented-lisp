#include <stdio.h>

#include "lexer/lexer.h"

// BufferRefiller
void refillFromStdin(char *startPos, int numToLoad) {
  for (int i = 0; i < numToLoad; i++) {
    int getcOutput = getc(stdin);

    if (getcOutput == EOF) {
      printf("found EOF\n");
      startPos[i] = '\0';
      break;
    } else {
      startPos[i] = (unsigned char) getcOutput;
    }
  }
}

int main() {
  LexerContext *ctx = newLexerContext(refillFromStdin);

  while (!ctx->isSrcDrained) {
    Token *token = lexNextToken(ctx);
    printf("token: %s\n", tokenTypeNames[token->type]);
  }

  printf("fin\n");

  return 0;
}
