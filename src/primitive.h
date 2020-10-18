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

#ifndef __OHCE_SRC_PRIMITIVE_H
#define __OHCE_SRC_PRIMITIVE_H
#include <stdint.h>
#include <stdbool.h>
#include "io.h"

enum ohce_primitive_type {
	OHCE_PRIMITIVE_STATIC = 1,
	OHCE_PRIMITIVE_DYNAMIC,
};

struct ohce_primitive {
	/// static or dynamic
	int type;
	/// internal use,
	/// init it with 0 when construct this struct
	int next;
	/// primitive content
	Str content;

	const char delim1[2]; // start, end: 1 char
	const char delim2[2][2]; // start, end: 2 chars
};

/// Get one primitive
/// if no more primitive, then @p->type is set to 0
void ohce_get_primitive(struct ohce_io *io, struct ohce_primitive *p);

#endif
