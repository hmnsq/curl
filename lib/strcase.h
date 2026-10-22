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
#अन्तर्भाव "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
बाह्य ध्रुव अचिन्हित वर्ण Curl_touppermap[256];
बाह्य ध्रुव अचिन्हित वर्ण Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
स्थिर CURL_INLINE वर्ण Curl_raw_toupper(वर्ण in)
{
  प्रत्यागम (वर्ण)Curl_touppermap[(अचिन्हित वर्ण)in];
}

स्थिर CURL_INLINE वर्ण Curl_raw_tolower(वर्ण in)
{
  प्रत्यागम (वर्ण)Curl_tolowermap[(अचिन्हित वर्ण)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#define checkprefix(a, b) curl_strnequal(b, STRCONST(a))

शून्य Curl_strntoupper(वर्ण *dest, ध्रुव वर्ण *src, माप_प्रकार n);
शून्य Curl_strntolower(वर्ण *dest, ध्रुव वर्ण *src, माप_प्रकार n);

bool Curl_safecmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b);
पूर्णाङ्क Curl_timestrcmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b);

#endif /* HEADER_CURL_STRCASE_H */
