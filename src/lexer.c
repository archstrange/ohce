#include "lexer.h"

static bool inline validate_delim(const char *delim);
static bool validate_config(const struct LexerConfig *cfg);

struct __ohce_Lexer {
	struct LexerConfig cfg;
	uint8_t smt_begin_len,
		smt_end_len,
		expr_begin_len,
		expr_end_len;

	Str src;
	size_t index;
	Token token, next_token;
	bool peeked;

	size_t smt_begin_index,
	       smt_end_index,
	       expr_begin_index,
	       expr_end_index;
};

static int Lexer_getNextToken(Lexer self, Token *token);

Lexer Lexer_new(Str src, const struct LexerConfig *cfg)
{
	Lexer self = calloc(1, sizeof(*self));
	self->cfg = *cfg;
	self->smt_begin_len = CStr_len(cfg->statement.begin);
	self->smt_end_len = CStr_len(cfg->statement.end);
	self->expr_begin_len = CStr_len(cfg->expr.begin);
	self->expr_end_len = CStr_len(cfg->expr.end);
	self->src = Str_clone(src);
	Str_asCStr(self->src);
	Token_init(&self->token);
	Token_init(&self->next_token);

	return self;
}

void Lexer_free(Lexer self)
{
	if (self == NULL) return;
	Str_free(self->src);
	Token_free(&self->token);
	Token_free(&self->next_token);
}

static int Lexer_getNextToken(Lexer self, Token *token)
{

}

static bool inline validate_delim(const char *delim)
{
	for (int i = 0; i < 5; i++) {
		if (delim[i] == 0)
			return i != 0;
		if (delim[i] >= 'a' && delim[i] <= 'z' ||
		    delim[i] >= 'A' && delim[i] <= 'Z' ||
		    delim[i] >= '0' && delim[i] <= '9' ||
		    delim[i] == '_' || delim[i] == '-')
			return false;
	}
	return false;
}

static bool validate_config(const struct LexerConfig *cfg)
{
	if (!validate_delim(cfg->statement.begin)) return false;
	if (!validate_delim(cfg->statement.end)) return false;
	if (!validate_delim(cfg->expr.begin)) return false;
	if (!validate_delim(cfg->expr.end)) return false;
}

