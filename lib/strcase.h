#ifndef HEADER_CURL_STRCASE_H
#define HEADER_CURL_STRCASE_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#அந்தர்பாவ "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
வெளி மாறிலி unsigned எழுத்து Curl_touppermap[256];
வெளி மாறிலி unsigned எழுத்து Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
நிலையான CURL_INLINE எழுத்து Curl_raw_toupper(எழுத்து in)
{
  திரும்பு (எழுத்து)Curl_touppermap[(unsigned எழுத்து)in];
}

நிலையான CURL_INLINE எழுத்து Curl_raw_tolower(எழுத்து in)
{
  திரும்பு (எழுத்து)Curl_tolowermap[(unsigned எழுத்து)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

வெறுமை Curl_strntoupper(எழுத்து *dest, மாறிலி எழுத்து *src, size_t n);
வெறுமை Curl_strntolower(எழுத்து *dest, மாறிலி எழுத்து *src, size_t n);

bool Curl_safecmp(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b);
முழு Curl_timestrcmp(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b);

#endif /* HEADER_CURL_STRCASE_H */
