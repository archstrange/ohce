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

#ifndef __OHCE_SRC_EVAL_H
#define __OHCE_SRC_EVAL_H
#include "io.h"
#include <sad/sad.h>

/// Pass delim1 or delim2 with NULL if you don't use it(them)
int ohce_eval(SadDict sad, struct ohce_io *io, const char delim1[2], const char delim2[2][2]);

#endif
