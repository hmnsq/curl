#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_STRCASE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_STRCASE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
ਬਾਹਰੀ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_touppermap[256];
ਬਾਹਰੀ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
ਸਥਿਰ CURL_INLINE ਅੱਖਰ Curl_raw_toupper(ਅੱਖਰ ਵਿੱਚ)
{
  ਵਾਪਸ (ਅੱਖਰ)Curl_touppermap[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਵਿੱਚ];
}

ਸਥਿਰ CURL_INLINE ਅੱਖਰ Curl_raw_tolower(ਅੱਖਰ ਵਿੱਚ)
{
  ਵਾਪਸ (ਅੱਖਰ)Curl_tolowermap[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਵਿੱਚ];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ checkprefix(a, b) curl_strnequal(b, STRCONST(a))

ਖਾਲੀ Curl_strntoupper(ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src, size_t n);
ਖਾਲੀ Curl_strntolower(ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src, size_t n);

bool Curl_safecmp(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b);
ਪੂਰਨ Curl_timestrcmp(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b);

#ਜੇ_ਅੰਤ /* HEADER_CURL_STRCASE_H */
