#ifndef OHCE_OHCE_H
#define OHCE_OHCE_H
#include <sad/sad.h>
#include "lexer.h"

static struct OhceDelim OHCE_DEFAULT_DELIM = {
	.smt = {
		.b = "/*", .blen = 2,
		.e = "*/", .elen = 2,
	},
	.expr = {
		.b = "$", .blen = 1,
		.e = "$", .elen = 1,
	},
};

int ohce(Str src, Str dst, SadDict sd, const struct OhceDelim *OD);

#endif

