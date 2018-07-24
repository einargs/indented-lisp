#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LEXER_CONTEXT_BUFFER_SIZE 100

bool isUpperCase(char target) {
  return 65 <= target && target <= 90;
}

bool isLowerCase(char target) {
  return 97 <= target && target <= 122;
}

bool isUnquotedName(char target) {
  return isLowerCase(target) || isUpperCase(target);
}

typedef enum {
  T_L_PAREN,
  T_R_PAREN,
  T_INDENT,
  T_NEW_LINE,
  T_NAME,
  T_EOF
} TokenType;

char* tokenTypeNames[6] = {
  "T_L_PAREN",
  "T_R_PAREN",
  "T_INDENT",
  "T_NEW_LINE",
  "T_NAME",
  "T_EOF"
};

typedef struct {
  TokenType type;
  char *text;
} Token;

Token *newToken(TokenType type, char *text) {
  Token *token = malloc(sizeof(Token));

  token->type = type;
  token->text = text;

  return token;
}

// Takes a pointer and an integer indicating how many
// new characters should be loaded in.
typedef void (*BufferRefiller)(char *startPtr, int numToLoad);

typedef enum {
  BASE_STATE = 0,
  LEXING_INDENT = 1,
  LEXING_NAME = 2
} LexerState;

typedef struct {
  int length;
  LexerState state;
  bool isSrcDrained;
  BufferRefiller refiller;
  char *fullBuffer;
  char **input;
  bool isTokenCompleted;
  Token *token;
} LexerContext;

void refillBuffer(LexerContext *ctx) {
  ctx->refiller(ctx->fullBuffer, LEXER_CONTEXT_BUFFER_SIZE);
}

LexerContext *newLexerContext(BufferRefiller refiller) {
  LexerContext *ctx = malloc(sizeof(LexerContext));

  ctx->length = 0;
  ctx->state = BASE_STATE;
  ctx->isSrcDrained = false;
  ctx->refiller = refiller;
  ctx->fullBuffer = malloc(sizeof(char) * LEXER_CONTEXT_BUFFER_SIZE);
  ctx->input = malloc(sizeof(char*));
  *ctx->input = ctx->fullBuffer;
  ctx->isTokenCompleted = false;
  ctx->token = NULL;

  // Prepare the buffer
  refillBuffer(ctx);

  return ctx;
}

void markSrcAsDrained(LexerContext *ctx) {
  ctx->isSrcDrained = true;
}

void acceptChar(LexerContext *ctx) {
  ctx->length++;
}

void dropCurrentChars(LexerContext *ctx) {
  *ctx->input = *ctx->input + ctx->length;
  ctx->length = 0;
}

void commitToken(LexerContext *ctx, TokenType type) {
  // Copy the token text
  char *tokenText = malloc(sizeof(char) * (ctx->length+1));
  strncpy(tokenText, *ctx->input, ctx->length);
  tokenText[ctx->length] = '\0'; // Add null terminator

  //
  ctx->token = newToken(type, tokenText);
  ctx->isTokenCompleted = true;

  // Drop the characters that made up the token.
  dropCurrentChars(ctx);
}

Token *pullToken(LexerContext *ctx) {
  Token *token = ctx->token;

  // Reset the token information
  ctx->token = NULL;
  ctx->isTokenCompleted = false;

  return token;
}

void moveToState(LexerContext *ctx, LexerState newState) {
  ctx->state = newState;
}

typedef void (*LexerAction)(LexerContext *ctx, char nextChar);

void baseStateAction(LexerContext *ctx, char nextChar) {
  if (nextChar == '(') {
    acceptChar(ctx);
    commitToken(ctx, T_L_PAREN);
  } else if (nextChar == ')') {
    acceptChar(ctx);
    commitToken(ctx, T_R_PAREN);
  } else if (nextChar == '\n') {
    acceptChar(ctx);
    commitToken(ctx, T_NEW_LINE);
  } else if (nextChar == '\0') {
    markSrcAsDrained(ctx);
    acceptChar(ctx);
    commitToken(ctx, T_EOF);
  } else if (nextChar == ' ') {
    acceptChar(ctx);
    moveToState(ctx, LEXING_INDENT);
  } else if (isUnquotedName(nextChar)) {
    acceptChar(ctx);
    moveToState(ctx, LEXING_NAME);
  }
}

void lexingIndentAction(LexerContext *ctx, char nextChar) {
  if (nextChar == ' ') {
    acceptChar(ctx);
    commitToken(ctx, T_INDENT);
  } else {
    dropCurrentChars(ctx);
  }
  moveToState(ctx, BASE_STATE);
}

void lexingNameAction(LexerContext *ctx, char nextChar) {
  if (isUnquotedName(nextChar)) {
    acceptChar(ctx);
  } else {
    commitToken(ctx, T_NAME);
    moveToState(ctx, BASE_STATE);
  }
}

LexerAction lexerEntranceActions[] = {
  baseStateAction, // BASE_STATE
  lexingIndentAction, // LEXING_INDENT
  lexingNameAction // LEXING_NAME
};


void stepLexer(LexerContext *ctx) {
  if (ctx->length == LEXER_CONTEXT_BUFFER_SIZE) {
    refillBuffer(ctx);
  }

  char nextChar = (*ctx->input)[ctx->length];
  //printf("Next char is: %c\n");
  lexerEntranceActions[ctx->state](ctx, nextChar);
}

Token *lexNextToken(LexerContext *ctx) {
  while (!ctx->isTokenCompleted) {
    stepLexer(ctx);
  }

  return pullToken(ctx);
}


//
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
