#ifndef SRC_LEXER
#define SRC_LEXER

#include <stdbool.h>

/* Macros */
#define LEXER_CONTEXT_BUFFER_SIZE 100

/* Token */
typedef enum {
  T_L_PAREN,
  T_R_PAREN,
  T_INDENT,
  T_NEW_LINE,
  T_NAME,
  T_EOF
} TokenType;

extern const char *tokenTypeNames[];

typedef struct {
  TokenType type;
  char *text;
} Token;

Token *newToken(TokenType type, char *text);


/* LexerContext */
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

LexerContext *newLexerContext(BufferRefiller refiller);

Token *lexNextToken(LexerContext *ctx);

#endif
