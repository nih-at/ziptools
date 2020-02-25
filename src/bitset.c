/*
  bitset.c -- bitset handling functions for ziptools
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

#include "bitset.h"

#include <stdlib.h>
#include <string.h>

typedef struct bitset {
    uint64_t *bits;
    uint64_t size;
} bitset_t;


bitset_t *
bitset_new(uint64_t size) {
    bitset_t *bitset;

    if ((bitset = (bitset_t *)malloc(sizeof(*bitset))) == NULL) {
	return NULL;
    }

    /* TODO: check for overflow */
    if ((bitset->bits = (uint64_t *)calloc(sizeof(uint64_t), (size+63)/64)) == NULL) {
	free(bitset);
	return NULL;
    }

    bitset->size = size;

    return bitset;
}

void
bitset_set_all(bitset_t *bitset) {
    memset(bitset->bits, 0xff, (bitset->size+7)/8);
}

void
bitset_clear_all(bitset_t *bitset) {
    memset(bitset->bits, 0, (bitset->size+7)/8);
}


bool
bitset_is_set(bitset_t *bitset, uint64_t index) {
    if (index >= bitset->size) {
	return false;
    }

    return (bitset->bits[index/64] & (1ULL << (index % 64)));
}

void
bitset_set(bitset_t *bitset, uint64_t index) {
    if (index >= bitset->size) {
	return;
    }

    bitset->bits[index/64] |= (1ULL << (index % 64));
}

void
bitset_clear(bitset_t *bitset, uint64_t index) {
    if (index >= bitset->size) {
	return;
    }

    bitset->bits[index/64] &= ~(1ULL << (index % 64));
}

void
bitset_free(bitset_t *bitset) {
    if (bitset == NULL) {
	return;
    }

    free(bitset->bits);
    free(bitset);
}
