#ifndef OHCE_LEXER_H
#define OHCE_LEXER_H
#include <stdbool.h>
#include <cutils/PS.h>
#include <cutils/Vector/Str.h>

/// ohce delimiters
struct OhceDelim {

	/// statements delimiters
	struct {
		/// begin delimiters
		uint8_t b[8], blen;
		/// end delimiters
		uint8_t e[8], elen;
	} smt;

	/// expressions delimiters
	struct {
		/// begin delimiters
		uint8_t b[8], blen;
		/// end delimiters
		uint8_t e[8], elen;
	} expr;
};

// ohce token type
enum {
	OHCE_BAD_FORMAT = -2,
	OHCE_EOS = -1, // end of source
	OHCE_UNKNOWN_TOKEN = 0,

	OHCE_IF,
	OHCE_NOT,
	OHCE_ELSE,
	OHCE_ELIF,
	OHCE_ENDIF,
	OHCE_FOR,
	OHCE_IN,
	OHCE_ENDFOR,

	OHCE_TEXT,
	OHCE_EXPR,
	OHCE_SMT,
};

struct OhceToken {
	int type;
	union {
		Str text;
		Str expr;
		Str str;
	};
};

struct OhceLexer {
	PS ps;
	struct OhceToken curToken;
	const struct OhceDelim *OD;
	int in;
};

struct OhceLexer OL_create(Str source, const struct OhceDelim *OD);

void OL_destroy(struct OhceLexer *self);

int OL_nextToken(struct OhceLexer *self);

#endif

