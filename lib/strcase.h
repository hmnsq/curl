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
#شامل "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
بیرونی ثابت unsigned نویسه Curl_touppermap[256];
بیرونی ثابت unsigned نویسه Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
ایستا CURL_INLINE نویسه Curl_raw_toupper(نویسه in)
{
  بازگشت (نویسه)Curl_touppermap[(unsigned نویسه)in];
}

ایستا CURL_INLINE نویسه Curl_raw_tolower(نویسه in)
{
  بازگشت (نویسه)Curl_tolowermap[(unsigned نویسه)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

تهی Curl_strntoupper(نویسه *dest, ثابت نویسه *src, size_t n);
تهی Curl_strntolower(نویسه *dest, ثابت نویسه *src, size_t n);

bool Curl_safecmp(ثابت نویسه *a, ثابت نویسه *b);
صحیح Curl_timestrcmp(ثابت نویسه *a, ثابت نویسه *b);

#endif /* HEADER_CURL_STRCASE_H */
