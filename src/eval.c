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
#include <string.h>

static inline bool ohce_output(struct ohce_io *io, Str str)
{
	for (size_t i = 0, n = Str_getLength(str); i < n; i++) {
		if (!io->put(io, Str_get(str, i)))
		{
			io->error(io, OHCE_IO_ERR_OFAIL);
			return false;
		}
	}
	return true;
}

static bool ohce_eval_dynamics(SadDict sad, struct ohce_io *io, struct ohce_primitive *p)
{
}

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
		if (ohce_output(io, p.content))
			goto again;
		ret = 1;
		break;
	case OHCE_PRIMITIVE_DYNAMIC:
		if (ohce_eval_dynamics(sad, io, &p))
			goto again;
		ret = 2;
		break;
	}

done:
	Str_free(p.content);
	return ret;
}

