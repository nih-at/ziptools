/*
  list.c -- list files in archive
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

int
list_archive(zip_t *archive, bitset_t *selected_files) {
    int ret = 0;

    if (verbose) {
	printf("Size\tCompressed\tRatio\tCRC\t\tTimestamp\t\tName\n");
    }
    else {
	printf("Size\tTimestamp\t\tName\n");
    }

    for (size_t i = 0; i < zip_get_num_entries(archive, 0); i++) {
	zip_stat_t zs;
	struct tm *tm;
	char timebuf[80];

	if (!bitset_is_set(selected_files, i)) {
	    continue;
	}

	if (zip_stat_index(archive, i, 0, &zs) < 0) {
	    /* TODO: error */
	    ret = 1;
	    continue;
	}

	if ((tm = gmtime(&zs.mtime)) == NULL) {
	    /* TODO: error */
	    ret = 1;
	    continue;
	}
	strftime(timebuf, sizeof(timebuf), "%F %H:%M:%S", tm);
	if (verbose) {
	    printf("%" PRIu64 "\t%" PRIu64 "\t\t%3.2f%%\t%08x\t%s\t%s\n", zs.size, zs.comp_size, (float)100 * zs.comp_size / zs.size, zs.crc, timebuf, zs.name);
	}
	else {
	    printf("%" PRIu64 "\t%s\t%s\n", zs.size, timebuf, zs.name);
	}
    }

    return ret;
};
