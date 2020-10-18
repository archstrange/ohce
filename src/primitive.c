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

#include "primitive.h"

static void ohce_on_delime1(struct ohce_io *io, struct ohce_primitive *p)
{
	uint8_t c;
	p->type = OHCE_PRIMITIVE_DYNAMIC;
	p->next = 0;

	while (io->get(io, &c)) {
		if (c != 0 && c == p->delim1[1])
			break;
		Str_push(p->content, c);
	}
}

static void ohce_on_delime2(struct ohce_io *io, struct ohce_primitive *p)
{
	uint8_t c;
	p->type = OHCE_PRIMITIVE_DYNAMIC;
	p->next = 0;

again:
	if (!io->get(io, &c))
		return;

	Str_push(p->content, c);
	if (c == 0)
		goto again;

	if (c == p->delim2[1][0]) {
		if (!io->get(io, &c))
			return;
		if (c == p->delim2[1][1]) {
			Str_pop(p->content);
			return;
		}
		Str_push(p->content, c);
	}

	goto again;
}

// use p->next:
//  1 -> delim1 dynamic
//  2 -> delim2 dynamic
//  other -> static
void ohce_get_primitive(struct ohce_io *io, struct ohce_primitive *p)
{
	p->type = 0;
	Str_clear(p->content);

	switch (p->next) {
	case 1:
		ohce_on_delime1(io, p);
		return;
	case 2:
		ohce_on_delime2(io, p);
		return;
	}

	uint8_t c;

again:
	if (!io->get(io, &c))
		return;

	Str_push(p->content, c);
	p->type = OHCE_PRIMITIVE_STATIC;

	if (c == 0)
		goto again;

	if (c == p->delim1[0]) {
		Str_pop(p->content);
		p->next = 1;
		return;
	}

	if (c == p->delim2[0][0]) {
		if (!io->get(io, &c))
			return;
		if (c == p->delim2[0][1]) {
			Str_pop(p->content);
			p->next = 2;
			return;
		}
		Str_push(p->content, c);
	}

	goto again;
}

