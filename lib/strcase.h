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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
ବାହ୍ୟ ଅଚଳ unsigned ଅକ୍ଷର Curl_touppermap[256];
ବାହ୍ୟ ଅଚଳ unsigned ଅକ୍ଷର Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
ସ୍ଥିର CURL_INLINE ଅକ୍ଷର Curl_raw_toupper(ଅକ୍ଷର in)
{
  ଫେରନ୍ତୁ (ଅକ୍ଷର)Curl_touppermap[(unsigned ଅକ୍ଷର)in];
}

ସ୍ଥିର CURL_INLINE ଅକ୍ଷର Curl_raw_tolower(ଅକ୍ଷର in)
{
  ଫେରନ୍ତୁ (ଅକ୍ଷର)Curl_tolowermap[(unsigned ଅକ୍ଷର)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

ଶୂନ୍ୟ Curl_strntoupper(ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src, size_t n);
ଶୂନ୍ୟ Curl_strntolower(ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src, size_t n);

bool Curl_safecmp(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_timestrcmp(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b);

#endif /* HEADER_CURL_STRCASE_H */
