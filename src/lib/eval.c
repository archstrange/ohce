// ohce -- A simple template engine
// Copyright (C) 2020  Arch Strange <arch.strange@foxmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "eval.h"
#include "primitive.h"
#include "idkeyword.h"
#include <string.h>

static inline bool put_str(struct ohce_io *io, Str str);
static inline size_t skip_white_spaces(Str source, size_t index);

enum ExtraTokenType {
	TT_ERROR = 0,
	TT_END,
};

typedef struct SELF {
	SadDict sad;
	struct ohce_io *io;
	struct ohce_primitive *p;
	size_t index;
	int tt;
	Str token;
} *SELF;

static void next_token(SELF self);

static bool eval_bool(SELF self, bool *res);
static bool eval_if_block(SELF self, bool alive);

static bool eval_id(SELF self);
static bool eval_if(SELF self, bool alive);
static bool eval_for(SELF self, bool alive);

static bool ohce_eval_dynamics(SadDict sad, struct ohce_io *io, struct ohce_primitive *p);

int ohce_eval(SadDict sad, struct ohce_io *io, const char delim1[2], const char delim2[2][2])
{
	struct ohce_primitive p = {
		.content = Str_new(),
	};
	if (delim1)
		memcpy(p.delim1, delim1, 2);
	if (delim2)
		memcpy(p.delim2, delim2, 4);

	int ret = 0;
again:
	ohce_get_primitive(io, &p);
	switch (p.type) {
	case OHCE_PRIMITIVE_STATIC:
		if (put_str(io, p.content))
			goto again;
		ret = 1;
		break;
	case OHCE_PRIMITIVE_DYNAMIC:
		if (ohce_eval_dynamics(sad, io, &p))
			goto again;
		ret = 2;
		break;
	}

	Str_free(p.content);
	return ret;
}

static inline bool put_str(struct ohce_io *io, Str str)
{
	for (size_t i = 0, n = Str_getLength(str); i < n; i++) {
		if (!io->put(io, Str_get(str, i))) {
			io->error(io, OHCE_IO_ERR_OFAIL);
			return false;
		}
	}
	return true;
}

static inline size_t skip_white_spaces(Str source, size_t index)
{
	size_t n = Str_getLength(source);
	for (; index < n; index++) {
		switch (Str_get(source, index)) {
		default: return index;
		case ' ':
		case '\t':
		case '\v':
		case '\n':
		case '\r': break;
		}
	}
	return index;
}

static void next_token(SELF self)
{
	int c;
	int s = 0;
	self->tt = TT_END;
	Str_clear(self->token);
	size_t n = Str_getLength(self->p->content);
	self->index = skip_white_spaces(self->p->content, self->index);
	if (self->index >= n) {
		return;
	}

again:
	if (self->index < n) {
		c = Str_get(self->p->content, self->index);
	} else {
		c = 0;
	}
	Str_push(self->token, c);

	s = st_idkeyword(s, c);
	switch (s) {
	case 0:
	case TT_ID:
	case TT_KW_if: case TT_KW_for:
	case TT_KW_not: case TT_KW_and: case TT_KW_or:
	case TT_KW_else: case TT_KW_elif:
	case TT_KW_endif: case TT_KW_endfor:
		self->tt = s;
		Str_pop(self->token);
		return;
	default:
		self->index += 1;
		goto again;
	}
}

static bool eval_id(SELF self)
{
	struct Sad v;
	if (SadDict_find(self->sad, self->token, &v)) {
		if (v.type == SAD_STRING) {
			put_str(self->io, v.str);
			return true;
		}
	}
	return false;
}

static bool eval_bool(SELF self, bool *res)
{
	struct Sad v;
	bool ret = true;
	bool has_not = false;
	next_token(self);
	if (self->tt == TT_KW_not) {
		has_not = true;
		next_token(self);
	}
	if (self->tt != TT_ID)
		return false;
	*res = SadDict_find(self->sad, self->token, &v);
	*res = has_not ? !*res : *res;
	next_token(self);
	switch (self->tt) {
	case TT_END:
		break;
	case TT_KW_and:
		if (*res)
			ret = eval_bool(self, res);
		else
			*res = false;
		break;
	case TT_KW_or:
		if (*res)
			*res = true;
		else
			ret = eval_bool(self, res);
		break;
	default:
		ret = false;
		break;
	}
	return ret;
}

static bool eval_if_block(SELF self, bool alive)
{
	bool ret = true;
again:
	ohce_get_primitive(self->io, self->p);
	if (self->p->type == OHCE_PRIMITIVE_STATIC) {
		if (alive) {
			ret = put_str(self->io, self->p->content);
		}
	} else if (self->p->type == OHCE_PRIMITIVE_DYNAMIC) {
		self->index = 0;
		next_token(self);
		switch (self->tt) {
		case TT_KW_if:
			ret = eval_if(self, alive);
			break;
		case TT_KW_for:
			ret = eval_for(self, alive);
			break;
		case TT_ID:
			if (alive) ret = eval_id(self);
			break;
		default:
			return true;
		}
	}
	if (!ret) return false;
	goto again;
}

static bool eval_if(SELF self, bool alive)
{
	bool res;
	if (!eval_bool(self, &res))
		return false;
	if (!eval_if_block(self, alive && res))
		return false;
	switch (self->tt) {
	case TT_KW_else:
		if (!eval_if_block(self, alive && !res))
			return false;
		return self->tt == TT_KW_endif;
	case TT_KW_elif:
		return eval_if(self, alive && !res);
	case TT_KW_endif:
		return true;
	default:
		return false;
	}
}

static bool eval_for(SELF self, bool alive)
{
	return false;
}

static bool ohce_eval_dynamics(SadDict sad, struct ohce_io *io, struct ohce_primitive *p)
{
	size_t n = Str_getLength(p->content);
	size_t index = skip_white_spaces(p->content, 0);
	if (index >= n)
		return true;

	bool ret = true;
	struct SELF self;
	self.p = p;
	self.io = io;
	self.sad = sad;
	self.index = index;
	self.token = Str_new();
	next_token(&self);
	switch (self.tt) {
	case TT_ID:
		// TODO error check
		//index = skip_white_spaces(p->content, index);
		ret =  eval_id(&self);
		break;
	case TT_KW_if:
		ret =  eval_if(&self, true);
		break;
	case TT_KW_for:
		ret =  eval_for(&self, true);
		break;
	default:
		ret = false;
	}
	Str_free(self.token);
	return ret;
}

