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

#include "io.h"

struct ohce_file_io {
	struct ohce_io api;
	FILE *in, *out;
	// TODO record current char position
};

static bool fileio_get(void *io, uint8_t *c)
{
	struct ohce_file_io *self = io;
	int ch = getc(self->in);
	if (ch == EOF)
		return false;
	*c = ch;
	return true;
}

static bool fileio_put(void *io, uint8_t c)
{
	struct ohce_file_io *self = io;
	int ch = putc(c, self->out);
	return ch != EOF;
}

static void fileio_error(void *io, int err)
{
	struct ohce_file_io *self = io;
	// TODO print error message to stderr
}

struct ohce_io *ohce_io_from_file(FILE *in, FILE *out)
{
	if (!in || !out)
		return NULL;
	if (ferror(in) || ferror(out))
		return NULL;
	struct ohce_file_io *self = malloc(sizeof(*self));
	self->in = in;
	self->out = out;
	self->api.get = fileio_get;
	self->api.put = fileio_put;
	self->api.error = fileio_error;

	return &self->api;
}

void ohce_io_free_file(struct ohce_io *fileio)
{
	if (!fileio)
		return;
	free(fileio);
}

