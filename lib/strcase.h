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
#અન્તર્ભાવ "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
બાહ્ય અચલ unsigned અક્ષર Curl_touppermap[256];
બાહ્ય અચલ unsigned અક્ષર Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
સ્થિર CURL_INLINE અક્ષર Curl_raw_toupper(અક્ષર in)
{
  પાછા (અક્ષર)Curl_touppermap[(unsigned અક્ષર)in];
}

સ્થિર CURL_INLINE અક્ષર Curl_raw_tolower(અક્ષર in)
{
  પાછા (અક્ષર)Curl_tolowermap[(unsigned અક્ષર)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

શૂન્ય Curl_strntoupper(અક્ષર *dest, અચલ અક્ષર *src, size_t n);
શૂન્ય Curl_strntolower(અક્ષર *dest, અચલ અક્ષર *src, size_t n);

bool Curl_safecmp(અચલ અક્ષર *a, અચલ અક્ષર *b);
પૂર્ણાંક Curl_timestrcmp(અચલ અક્ષર *a, અચલ અક્ષર *b);

#endif /* HEADER_CURL_STRCASE_H */
