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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cutils/xo.h>
#include <sad/read.h>
#include "../lib/eval.h"

struct xo_entry XOS[] = {
#define OPT_INPUT (XOS[0])
	{
		.type = XO_ENTRY_SHORT | XO_ENTRY_LONG | XO_ENTRY_ARG,
		.s = 'i',
		.l = "input",
		.help = "-i | --input OHCE_FILE | -",
	},
#define OPT_OUTPUT (XOS[1])
	{
		.type = XO_ENTRY_SHORT | XO_ENTRY_LONG | XO_ENTRY_ARG,
		.s = 'o',
		.l = "output",
		.help = "-o | --output OUT_FILE | -",
	},
#define OPT_SAD_FILE (XOS[2])
	{
		.type = XO_ENTRY_SHORT | XO_ENTRY_LONG | XO_ENTRY_ARG,
		.s = 's',
		.l = "sad",
		.help = "-s | --sad SAD_FILE | -",
	},
};

static const char USAGE[] =
"Ohce - a simple template engine\n"
"Usage:\n"
"	ohce [-i|--input] OHCE_FILE|- [-o|--output] OUT_FILE|- [-s|--sad] SAD_FILE|-\n"
"\n"
"Ohce simply takes three things: an ohce template file, an output file\n"
"and a Sad file. Ohce uses data from Sad file to fill templates and puts\n"
"result to output. When '-' is used for some of them, Ohce takes stdin or\n"
"stdout for input or output file. Since OHCE_FILE and SAD_FILE are both\n"
"input, you must prefix at least one of them with switch.\n"
;

static FILE *fp_ohce = NULL;
static FILE *fp_out = NULL;
static FILE *fp_sad = NULL;

static const char delim1[2] = "$$";
static const char delim2[2][2] = { "/*", "*/" };

static inline void open_file_arg(const char *arg, FILE **fpp, FILE *fp_def)
{
	if (strcmp(arg, "-") == 0) {
		*fpp = fp_def;
	} else {
		*fpp = fopen(arg, "r");
	}
}

int main(int argc, const char *argv[])
{
	int ret = 0;
	StrVector args = StrVector_new();
	if (xo(argc, argv, XOS, args)) {
		fprintf(stderr, USAGE);
		ret = 1;
		goto done;
	}

	size_t index = 0, args_num = StrVector_getLength(args);
	Str arg_ohce, arg_out, arg_sad;

	if (OPT_INPUT.type & XO_ENTRY_MET) {
		arg_ohce = OPT_INPUT.arg;
	} else if (index < args_num) {
		arg_ohce = StrVector_get(args, index);
		index += 1;
	} else {
		fprintf(stderr, "Error: no ohce file\n");
		ret = 1;
		goto done;
	}

	if (OPT_OUTPUT.type & XO_ENTRY_MET) {
		arg_out = OPT_INPUT.arg;
	} else if (index < args_num) {
		arg_out = StrVector_get(args, index);
		index += 1;
	} else {
		fprintf(stderr, "Error: no output file\n");
		ret = 1;
		goto done;
	}

	if (OPT_SAD_FILE.type & XO_ENTRY_MET) {
		arg_sad = OPT_INPUT.arg;
	} else if (index < args_num) {
		arg_sad = StrVector_get(args, index);
	} else {
		fprintf(stderr, "Error: no sad file\n");
		ret = 1;
		goto done;
	}

	open_file_arg(Str_cstr(arg_ohce), &fp_ohce, stdin);
	open_file_arg(Str_cstr(arg_out), &fp_out, stdout);
	open_file_arg(Str_cstr(arg_sad), &fp_sad, stdin);

	if (!fp_ohce) {
		fprintf(stderr, "Error: failed to open ohce file\n");
		ret = 1;
		goto done;
	}
	if (!fp_out) {
		fprintf(stderr, "Error: failed to open output file\n");
		ret = 1;
		goto done;
	}
	if (!fp_sad) {
		fprintf(stderr, "Error: failed to open sad file\n");
		ret = 1;
		goto done;
	}
	if (fp_ohce == fp_sad) {
		fprintf(stderr, "Error: ohce file and sad file must be different\n");
		ret = 1;
		goto done;
	}

	struct Sad sad = Sad_readFile(fp_sad);
	if (sad.type != SAD_DICT) {
		fprintf(stderr, "Error: wrong sad file, it should present as a sad dict value\n");
		ret = 1;
		goto done;
	}

	struct ohce_io *io = ohce_io_from_file(fp_ohce, fp_out);
	if (!io) {
		fprintf(stderr, "Error: File error, please check the files for input and output\n");
		ret = 1;
		goto done;
	}

	if (ohce_eval(sad.dict, io, delim1, delim2)) {
		fprintf(stderr, "Error: ohcing failed\n");
		ret = 1;
		goto done;
	}

done:
	// Just close some files, leave allocted memory to system to clean
	if (fp_ohce) fclose(fp_ohce);
	if (fp_out) fclose(fp_out);
	if (fp_sad) fclose(fp_sad);
	return ret;
}
