#include "ohce.h"
#include <cutils/Vector/I32Vector.h>
#include <cutils/Vector/StrVector.h>

struct OP {
	I32Vector types;
	StrVector exprs;
	size_t start;
	bool goon;
	struct OhceLexer *OL;
	Str dst;
	SadDict sd;
};

static inline struct OP OP_new();
static void OP_free(struct OP *self);
static inline size_t OP_length(struct OP *self);
static inline void OP_push(struct OP *self, int type, Str expr);
static inline void OP_pop(struct OP *self, int *type, Str *expr);
static inline void OP_dropTail(struct OP *self, size_t n);
static void ohce_expr_next_key(Str expr, size_t start, Str key);
static struct Sad ohce_expr_to_sad(Str expr, SadDict sd);
static bool ohce_eval_expr(Str expr, Str dst, SadDict sd);
static bool ohce_eat_if(struct OP *OP);
static bool ohce_eval_if(struct OP *OP);

// How to parse?
// template file is just a sequence of text, smt, expr, like this:
// tokens --> text smt-begin [text] expr [text] smt-end text expr
int ohce(Str src, Str dst, SadDict sd, const struct OhceDelim *OD)
{
	if (sd == NULL)
		return 1;

	int ret = 0;
	size_t start;
	Str_clear(dst);
	struct OhceLexer OL = OL_create(src, OD);
	struct OP OP = OP_new();
	OP.OL = &OL;
	OP.dst = dst;
	OP.sd = sd;

	while (OL_nextToken(&OL) > 0) {
		switch (OL.curToken.type) {
		case OHCE_TEXT:
			Str_append(dst, OL.curToken.text);
			break;
		case OHCE_EXPR:
			if (!ohce_eval_expr(OL.curToken.expr, dst, sd)) {
				ret = 1;
				goto onError;
			}
			break;
		case OHCE_IF:
			OP.goon = true;
			OP.start = start = OP_length(&OP);
			if (!ohce_eat_if(&OP)) {
				ret = 1;
				goto onError;
			}
			if (!ohce_eval_if(&OP)) {
				ret = 1;
				goto onError;
			}
			OP_dropTail(&OP, OP_length(&OP) - start);
			break;
		}
	}

onError:
	OL_destroy(&OL);
	OP_free(&OP);
	return ret;
}

static void ohce_expr_next_key(Str expr, size_t start, Str key)
{
	Str_clear(key);
	size_t len = Str_getLength(expr);
	const uint8_t *e = Str_data(expr);

	for (size_t i = start; i < len; i++) {
		if (e[i] != '.') {
			Str_push(key, e[i]);
		} else {
			break;
		}
	}
}

static struct Sad ohce_expr_to_sad(Str expr, SadDict sd)
{
	Str key = Str_newWithCapacity(0);
	struct Sad sad = { .type = SAD_UNKNOWN_VALUE_TYPE };
	size_t start = 0;
	size_t expr_len = Str_getLength(expr);

	while (start < expr_len) {
		ohce_expr_next_key(expr, start, key);
		sad = SadDict_getValue(sd, key);
		start += Str_getLength(key);
		if (start < expr_len) {
			if (sad.type != SAD_DICT) {
				sad.type = SAD_UNKNOWN_VALUE_TYPE;
				break;
			} else {
				start += 1;
				sd = sad.dict;
			}
		}
	}

	return sad;
}

static bool ohce_eval_expr(Str expr, Str dst, SadDict sd)
{
	struct Sad s = ohce_expr_to_sad(expr, sd);
	if (s.type == SAD_STRING) {
		Str_append(dst, s.str);
	}
	return true;
}

static bool ohce_eat_if(struct OP *OP)
{
	OP_push(OP, OP->OL->curToken.type, NULL);
	if (OL_nextToken(OP->OL) <= 0)
		return false;

	if (OP->OL->curToken.type == OHCE_NOT) {
		OP_push(OP, OHCE_NOT, NULL);
		OL_nextToken(OP->OL);
	}
	if (OP->OL->curToken.type == OHCE_EXPR) {
		OP_push(OP, OHCE_EXPR, OP->OL->curToken.expr);
	} else {
		return false;
	}
	while (OL_nextToken(OP->OL) > 0) {
		switch (OP->OL->curToken.type) {
		case OHCE_TEXT:
			OP_push(OP, OHCE_TEXT, OP->OL->curToken.text);
			break;
		case OHCE_EXPR:
			OP_push(OP, OHCE_EXPR, OP->OL->curToken.text);
			break;
		case OHCE_IF:
			if (!ohce_eat_if(OP))
				return false;
			break;
		case OHCE_ELIF:
			return ohce_eat_if(OP);
		case OHCE_ELSE:
			OP_push(OP, OHCE_ELSE, NULL);
			break;
		case OHCE_ENDIF:
			OP_push(OP, OHCE_ENDIF, NULL);
			return true;
		case OHCE_FOR:
			// TODO
			exit('F');
		default:
			return false;
		}
	}
	return false;
}

static bool ohce_eval_if(struct OP *OP)
{
#define NOT_BOOL(not, bool) ((not) ? (bool) : !(bool))
	Str e;
	struct Sad sad;
	bool has_not = false, if_true, met_else = false, tmp;
	OP->start += 1;
	if (I32Vector_get(OP->types, OP->start) == OHCE_NOT) { // not
		has_not = true;
		OP->start += 1;
	}
	if (!OP->goon) goto inBody;
	e = StrVector_get(OP->exprs, OP->start);
	sad = ohce_expr_to_sad(e, OP->sd);
	if_true = NOT_BOOL(has_not, sad.type == SAD_UNKNOWN_VALUE_TYPE);

inBody:
	OP->start += 1;
	switch (I32Vector_get(OP->types, OP->start)) {
	case OHCE_TEXT:
		if (OP->goon && if_true) {
			Str_append(OP->dst, StrVector_get(OP->exprs, OP->start));
		}
		goto inBody;
	case OHCE_EXPR:
		if (OP->goon && if_true) {
			sad = ohce_expr_to_sad(StrVector_get(OP->exprs, OP->start), OP->sd);
			if (sad.type == SAD_STRING) {
				Str_append(OP->dst, sad.str);
			}
		}
		goto inBody;
	case OHCE_IF:
		tmp = OP->goon;
		OP->goon = tmp && if_true;
		if (!ohce_eval_if(OP))
			return false;
		OP->goon = tmp;
		goto inBody;
	case OHCE_ELIF:
		if (met_else) return false;
		OP->goon = OP->goon && !if_true;
		return ohce_eval_if(OP);
	case OHCE_ELSE:
		if (met_else) return false;
		met_else = true;
		if_true = !if_true;
		OP->goon = OP->goon && if_true;
		goto inBody;
	case OHCE_ENDIF:
		break;
	case OHCE_FOR:
		// TODO
		exit('F');
	default:
		return false;
	}
	return true;
}

static inline struct OP OP_new()
{
	return (struct OP){
		.types = I32Vector_newWithCapacity(0),
		.exprs = StrVector_newWithCapacity(0),
	};
}

static void OP_free(struct OP *self)
{
	I32Vector_free(self->types);
	StrVector_free(self->exprs);
}

static inline size_t OP_length(struct OP *self)
{
	return I32Vector_getLength(self->types);
}

static inline void OP_push(struct OP *self, int type, Str expr)
{
	I32Vector_push(self->types, type);
	Str e = expr == NULL ? NULL : Str_clone(expr);
	StrVector_push(self->exprs, e);
}

static inline void OP_pop(struct OP *self, int *type, Str *expr)
{
	int t = I32Vector_pop(self->types);
	Str e = StrVector_pop(self->exprs);
	if (type != NULL) *type = t;
	if (expr != NULL) *expr = e;
}

static inline void OP_dropTail(struct OP *self, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		Str e;
		OP_pop(self, NULL, &e);
		Str_free(e);
	}
}

