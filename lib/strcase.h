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
#অন্তর্ভাব "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
বাহ্যিক ধ্রুবক unsigned অক্ষর Curl_touppermap[256];
বাহ্যিক ধ্রুবক unsigned অক্ষর Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
স্থির CURL_INLINE অক্ষর Curl_raw_toupper(অক্ষর in)
{
  ফেরত (অক্ষর)Curl_touppermap[(unsigned অক্ষর)in];
}

স্থির CURL_INLINE অক্ষর Curl_raw_tolower(অক্ষর in)
{
  ফেরত (অক্ষর)Curl_tolowermap[(unsigned অক্ষর)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

শূন্য Curl_strntoupper(অক্ষর *dest, ধ্রুবক অক্ষর *src, size_t n);
শূন্য Curl_strntolower(অক্ষর *dest, ধ্রুবক অক্ষর *src, size_t n);

bool Curl_safecmp(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b);
পূর্ণসংখ্যা Curl_timestrcmp(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b);

#endif /* HEADER_CURL_STRCASE_H */
