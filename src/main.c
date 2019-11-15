#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ohce.h"

static const char *OHCE_USAGE = "ohce - A template engine\n"
	      "Usage: ohce [OPTION...] ohce-file [sad-file [output-file]]\n"
	      "\n"
	      " If output-file is omitted then stdout is used"
	      " and stdin is used if sad-file too\n"
	      "\n"
	      " Options:\n"
	      "  --smt-start value              statement start token, default is '/*'\n"
	      "  --smt-end value                statement end token, default is '*/'\n"
	      "  --expr-start value             expression start token, default is '$'\n"
	      "  --expr-start value             expression start token, default is '$'\n"
	      "\n"
	      " Ohce tokens must have less than 8 ASCII chars and"
	      " alphabet, number, '.' and '_' characters are invalid."
	      " Statement and expression tokens must be different.\n"
	      ;

int main(int argc, char *argv[])
{
	const char *files[3];
	int nfiles = 0;
	FILE *sadfile = stdin;
	FILE *outfile = stdout;
	FILE *ohcefile = NULL;
	struct OhceDelim OD = OHCE_DEFAULT_DELIM;

	int ret = 0;
	switch (argc) {
	case 4:
		outfile = fopen(argv[3], "wb");
		if (outfile == NULL) {
			fprintf(stderr, "%s: failed to open %s\n",
				strerror(errno), argv[3]);
			ret = 1;
			goto onFileFail;
		}
	case 3:
		sadfile = fopen(argv[2], "rb");
		if (sadfile == NULL) {
			fprintf(stderr, "%s: failed to open %s\n",
				strerror(errno), argv[2]);
			ret = 1;
			goto onFileFail;
		}
	case 2:
		ohcefile = fopen(argv[1], "rb");
		if (ohcefile == NULL) {
			fprintf(stderr, "%s: failed to open %s\n",
				strerror(errno), argv[1]);
			ret = 1;
			goto onFileFail;
		}
		break;
	default:
		printf("%s", OHCE_USAGE);
		return 1;
	}

	struct Sad sad = Sad_fromFILE(sadfile);
	Str src = Str_newWithCapacity(0);
	Str dst = Str_newWithCapacity(0);

	if (sad.type != SAD_DICT) {
		ret = 1;
		goto onOhceFail;
	}

	int c;
	while ((c = getc(ohcefile)) != EOF) {
		Str_push(src, c);
	}

	if (ohce(src, dst, sad.dict, &OHCE_DEFAULT_DELIM)) {
		fprintf(stderr, "Failed to ohce\n");
		goto onOhceFail;
	}

	size_t dst_len = Str_getLength(dst);
	if (fwrite(Str_data(dst), 1, dst_len, outfile) != dst_len) {
		fprintf(stderr,
			"%s: error occured while writing to output\n",
			strerror(errno));
	}

onOhceFail:
	Str_free(src);
	Str_free(dst);
	Sad_destroy(&sad);
onFileFail:
	fclose(outfile);
	fclose(sadfile);
	fclose(ohcefile);
	return ret;
}

