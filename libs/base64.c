/*
 * base64.h -- base64 encoding and decoding
 *
 * Copyright (c) 2013-2023 David Demelier <markand@malikania.fr>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "base64.h"

int
b64_isbase64(unsigned char ch)
{
	return isalnum(ch) || ch == '+' || ch == '-' || ch == '_' || ch == '/';
}

int
b64_isvalid(unsigned char ch)
{
	return b64_isbase64(ch) || ch == '=';
}

unsigned char
b64_lookup(unsigned char value)
{
	assert(value < 64);

	static const char *table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	return table[value];
}

unsigned char
b64_rlookup(unsigned char ch)
{
	assert(b64_isbase64(ch));

	if (ch >= '0' && ch <= '9')
		return ch + 4;
	if (ch >= 'A' && ch <= 'Z')
		return ch - 65;
	if (ch >= 'a' && ch <= 'z')
		return ch - 71;

	/* '-' is base64url support. */
	return ch == '+' || ch == '-' ? 62U : 63U;
}

size_t
b64_encode(const void *src, size_t srcsz, char *dst, size_t dstsz)
{
	assert(src);
	assert(dst);

	size_t nwritten = 0, count;
	unsigned char inputbuf[3], *s = (unsigned char *)src;

	if (srcsz == (size_t)-1)
		srcsz = strlen((const char *)s);

	while (srcsz && dstsz) {
		inputbuf[0] = inputbuf[1] = inputbuf[2] = 0;
		count = 0;

		while (srcsz && count < 3) {
			inputbuf[count++] = *s++;
			--srcsz;
		}

		if (dstsz < 4) {
			errno = ERANGE;
			return -1;
		}

		*dst++ = b64_lookup(inputbuf[0] >> 2 & 0x3f);
		*dst++ = b64_lookup((inputbuf[0] << 4 & 0x3f) | (inputbuf[1] >> 4 & 0x0f));

		if (count < 2)
			*dst++ = '=';
		else
			*dst++ = b64_lookup((inputbuf[1] << 2 & 0x3c) | (inputbuf[2] >> 6 & 0x03));

		if (count < 3)
			*dst++ = '=';
		else
			*dst++ = b64_lookup(inputbuf[2] & 0x3f);

		nwritten += 4;
		dstsz -= 4;
	}

	/* Not enough room to store '\0'. */
	if (dstsz == 0) {
		errno = ERANGE;
		return -1;
	}

	*dst = '\0';

	return nwritten;
}

size_t
b64_decode(const char *src, size_t srcsz, void *dst, size_t dstsz)
{
	assert(src);
	assert(dst);

	size_t nwritten = 0, i, r;
	unsigned int inputbuf[4] = {0};
	unsigned char *d = (unsigned char *)dst;

	if (srcsz == (size_t)-1)
		srcsz = strlen(src);

	while (srcsz && dstsz) {
		r = 3;

		for (i = 0; srcsz && i < 4; i++) {
			if (*src == '=') {
				/*
				 * '=' is only allowed in last 2 characters,
				 * otherwise it means we need less data.
				 */
				if (i <= 1)
					goto eilseq;

				/* Less data required. */
				--r;
			} else if (!b64_isvalid(*src))
				goto eilseq;

			if (b64_isbase64(*src))
				inputbuf[i] = b64_rlookup(*src);

			++src;
			--srcsz;
		}

		/* Make sure we haven't seen AB=Z as well. */
		if (i != 4 || (src[-2] == '=' && src[-1] != '='))
			goto eilseq;
		if ((size_t)r > dstsz)
			goto erange;

		*d++ = ((inputbuf[0] << 2) & 0xfc) | ((inputbuf[1] >> 4) & 0x03);

		if (r >= 2)
			*d++ = ((inputbuf[1] << 4) & 0xf0) | ((inputbuf[2] >> 2) & 0x0f);
		if (r >= 3)
			*d++ = ((inputbuf[2] << 6) & 0xc0) | (inputbuf[3] & 0x3f);

		nwritten += r;
		dstsz -= r;
	}

	if (srcsz)
		goto erange;

	return nwritten;

eilseq:
	errno = EILSEQ;
	return -1;

erange:
	errno = ERANGE;
	return -1;
}
