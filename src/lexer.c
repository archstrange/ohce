#include "lexer.h"
#include <ctype.h>
#include <string.h>

static int OL_nextTokenInSmt(struct OhceLexer *self);
static int OL_nextTokenInText(struct OhceLexer *self);
static int OL_eatExpr(struct OhceLexer *self);
// return ID length
static size_t OL_nextID(struct OhceLexer *self);
static void OL_idID(struct OhceToken *t);
// check if next chars in ps is @chars
static inline bool OL_checkChars(struct OhceLexer *self,
				 const uint8_t *chars,
				 uint8_t len);
static inline void OL_LSForward(struct OhceLexer *self, size_t step);
static inline void OL_LSSkipSpaces(struct OhceLexer *self);
static inline bool is_id_char(int c);

struct OhceLexer OL_create(Str source, const struct OhceDelim *OD)
{
	struct OhceLexer self = {
		.ps = LS_newStrSource(source, false),
		.curToken = {
			.type = 0,
			.str = Str_newWithCapacity(0)
		},
		.in = OHCE_TEXT,
		.OD = OD,
	};
	return self;
}

void OL_destroy(struct OhceLexer *self)
{
	if (self != NULL) {
		LS_free(self->ps);
		Str_free(self->curToken.str);
	}
}

int OL_nextToken(struct OhceLexer *self)
{
	int ch;
	if ((ch = LS_char(self->ps)) >= 0) {
		switch (self->in) {
		case OHCE_TEXT:
			return OL_nextTokenInText(self);
		case OHCE_EXPR:
			return OL_eatExpr(self);
		case OHCE_SMT:
			return OL_nextTokenInSmt(self);
		default:
			return OHCE_BAD_FORMAT;
		}
	}
	return OHCE_EOS;
}

static int OL_nextTokenInSmt(struct OhceLexer *self)
{
	if (OL_nextID(self)) {
		OL_idID(&self->curToken);
		return self->curToken.type;
	}
	if (OL_checkChars(self, self->OD->smt.e, self->OD->smt.elen)) {
		OL_LSForward(self, self->OD->smt.elen);
		if (OL_checkChars(self, "\n", 1)) OL_LSForward(self, 1);
		self->in = OHCE_TEXT;
		return OL_nextToken(self);
	}
	return OHCE_BAD_FORMAT;
}

static int OL_nextTokenInText(struct OhceLexer *self)
{
	Str_clear(self->curToken.text);
	self->curToken.type = OHCE_TEXT;

	int ch;
	while ((ch = LS_char(self->ps)) >= 0) {
		if (OL_checkChars(self, self->OD->smt.b, self->OD->smt.blen)) {
			OL_LSForward(self, self->OD->smt.blen);
			self->in = OHCE_SMT;
			goto getText;
		}
		if (OL_checkChars(self, self->OD->expr.b, self->OD->expr.blen)) {
			OL_LSForward(self, self->OD->expr.blen);
			self->in = OHCE_EXPR;
			goto getText;
		}
		Str_push(self->curToken.text, ch);
		LS_next(self->ps);
	}

getText:
	if (Str_getLength(self->curToken.text) == 0) {
		if (ch < 0)
			return OHCE_EOS;
		else
			return OL_nextToken(self);
	}
	return self->curToken.type;
}

static int OL_eatExpr(struct OhceLexer *self)
{
	size_t len = OL_nextID(self);
	self->curToken.type = OHCE_EXPR;
	OL_LSSkipSpaces(self);
	if (!OL_checkChars(self, self->OD->expr.e, self->OD->expr.elen))
		self->curToken.type = OHCE_BAD_FORMAT;
	OL_LSForward(self, self->OD->expr.elen);
	self->in = OHCE_TEXT;

	if (len == 0) {
		return OL_nextToken(self);
	}
	return self->curToken.type;
}

static inline bool is_id_char(int c)
{
	if (c >= 'a' && c <= 'z' ||
	    c >= 'A' && c <= 'Z' ||
	    c == '_' || c == '.')
		return true;
	return false;
}

static inline void OL_LSSkipSpaces(struct OhceLexer *self)
{
	while (isspace(LS_char(self->ps))) {
		LS_next(self->ps);
	}
}

static size_t OL_nextID(struct OhceLexer *self)
{
	int c;
	Str_clear(self->curToken.text);
	OL_LSSkipSpaces(self);
	while (is_id_char(c = LS_char(self->ps))) {
		Str_push(self->curToken.text, c);
		LS_next(self->ps);
	}
	return Str_getLength(self->curToken.text);
}

static void OL_idID(struct OhceToken *t)
{
	const uint8_t *id = Str_data(t->str);
	t->type = OHCE_EXPR;
	switch (Str_getLength(t->str)) {
	case 2:
		if (strncmp(id, "if", 2) == 0)
			t->type = OHCE_IF;
		else if (strncmp(id, "in", 2) == 0)
			t->type = OHCE_IN;
		break;
	case 3:
		if (strncmp(id, "not", 3) == 0)
			t->type = OHCE_NOT;
		else if (strncmp(id, "for", 3) == 0)
			t->type = OHCE_FOR;
		break;
	case 4:
		if (strncmp(id, "else", 4) == 0)
			t->type = OHCE_ELSE;
		else if (strncmp(id, "elif", 4) == 0)
			t->type = OHCE_ELIF;
		break;
	case 5:
		if (strncmp(id, "endif", 5) == 0)
			t->type = OHCE_ENDIF;
		break;
	case 6:
		if (strncmp(id, "endfor", 6) == 0)
			t->type = OHCE_ENDFOR;
		break;
	}
}

static inline bool OL_checkChars(struct OhceLexer *self,
				 const uint8_t *chars,
				 uint8_t len)
{
	for (uint8_t i = 0; i < len; i++) {
		if (chars[i] != LS_peek(self->ps, i))
			return false;
	}
	return true;
}

static inline void OL_LSForward(struct OhceLexer *self, size_t step)
{
	for (size_t i = 0; i < step; i++) {
		LS_next(self->ps);
	}
}

