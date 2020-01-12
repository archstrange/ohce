#ifndef OHCE_LEXER_H
#define OHCE_LEXER_H
#include "idkeyword.h"
#include <cutils/Str.h>

typedef struct __ohce_Lexer *Lexer;

struct LexerConfig {
	struct {
		char begin[5];
		char end[5];
	} statement;
	struct {
		char begin[5];
		char end[5];
	} expr;
};

enum {
	TT_EOS = 0,
	TT_TEXT = 1,
	TT_DOT,
};

typedef struct {
	int type;
	Str str;
} Token;

Lexer Lexer_new(Str src, const struct LexerConfig *cfg);

void Lexer_free(Lexer self);

int Lexer_next(Lexer self);

int Lexer_peekNext(Lexer self);

Token *Lexer_token(Lexer self);

Token *Lexer_peekNextToken(Lexer self);

static inline void Token_init(Token *self)
{
	static Token init = { 0 };
	*self = init;
	self->str = Str_new();
}

static inline void Token_free(Token *self)
{
	if (self != NULL) if (self->str != NULL) Str_free(self->str);
}

static inline void Token_copy(Token *self, const Token *src)
{
	Str str = self->str;
	*self = *src;
	Str_copy(str, src->str);
	self->str = str;
}

#endif

