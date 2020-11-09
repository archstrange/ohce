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

#ifndef __OHCE_SRC_IO_H
#define __OHCE_SRC_IO_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <cutils/Str.h>

enum ohce_io_error {
	OHCE_IO_ERR_IFAIL,
	OHCE_IO_ERR_OFAIL,
};

struct ohce_io {
	bool (*get)(void *ohce_io, uint8_t *c);
	bool (*put)(void *ohce_io, uint8_t c);
	void (*error)(void *ohce_io, int err);
};

struct ohce_io *ohce_io_from_file(FILE *in, FILE *out);

void ohce_io_free_file(struct ohce_io *fileio);

struct ohce_io *ohce_io_from_str(Str in, Str out);

void ohce_io_free_str(struct ohce_io *strio);

#endif
