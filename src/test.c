/*
  test.c -- test files in archive
  Copyright (C) 2020 Dieter Baron and Thomas Klausner

  This file is part of ziptools.
  The authors can be contacted at <ziptools@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The name of the author may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "unzip.h"

static int test_file(zip_t *archive, size_t index);

int
test_archive(zip_t *archive, bitset_t *selected_files) {
    int ret = 0;

    for (size_t i = 0; i < zip_get_num_entries(archive, 0); i++) {
	zip_stat_t zs;
	const char *name;

	if (!bitset_is_set(selected_files, i)) {
	    continue;
	}

	ret |= test_file(archive, i);
    }

    return ret;
};

static int
test_file(zip_t *archive, size_t index) {
    zip_file_t *file;
    char buf[8192];
    int n;
    int err;

    if ((file = zip_fopen_index(archive, index, 0)) == NULL) {
	fprintf(stderr, "%s: open failed: %s\n", zip_get_name(archive, index, 0), zip_strerror(archive));
	return 1;
    }
    while ((n = zip_fread(file, buf, sizeof(buf))) > 0) {
	/* do nothing */
    }
    if (n < 0) {
	fprintf(stderr, "%s: read failed: %s\n", zip_get_name(archive, index, 0), zip_file_strerror(file));
	zip_fclose(file);
	return 1;
    }

    if ((err = zip_fclose(file)) != 0) {
	fprintf(stderr, "%s: close failed: %d\n", zip_get_name(archive, index, 0), err);
	return 1;
    }

    if (verbose) {
	printf("%s: OK\n", zip_get_name(archive, index, 0));
    }

    return 0;
}
