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
#include "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
extern const unsigned var_na Curl_touppermap[256];
extern const unsigned var_na Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
static CURL_INLINE var_na Curl_raw_toupper(var_na in)
{
  ལོག (var_na)Curl_touppermap[(unsigned var_na)in];
}

static CURL_INLINE var_na Curl_raw_tolower(var_na in)
{
  ལོག (var_na)Curl_tolowermap[(unsigned var_na)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

void Curl_strntoupper(var_na *dest, const var_na *src, size_t n);
void Curl_strntolower(var_na *dest, const var_na *src, size_t n);

bool Curl_safecmp(const var_na *a, const var_na *b);
ཧྲིལ Curl_timestrcmp(const var_na *a, const var_na *b);

#endif /* HEADER_CURL_STRCASE_H */
