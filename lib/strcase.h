#अगर_अघोषित HEADER_CURL_STRCASE_H
#घोषणा HEADER_CURL_STRCASE_H
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
#समावेश "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
बाह्य स्थिर अचिन्हित अक्षर Curl_touppermap[256];
बाह्य स्थिर अचिन्हित अक्षर Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
अटल CURL_INLINE अक्षर Curl_raw_toupper(अक्षर in)
{
  वापस (अक्षर)Curl_touppermap[(अचिन्हित अक्षर)in];
}

अटल CURL_INLINE अक्षर Curl_raw_tolower(अक्षर in)
{
  वापस (अक्षर)Curl_tolowermap[(अचिन्हित अक्षर)in];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#घोषणा checkprefix(a, b) curl_strnequal(b, STRCONST(a))

व्योम Curl_strntoupper(अक्षर *dest, स्थिर अक्षर *src, माप_प्रकार n);
व्योम Curl_strntolower(अक्षर *dest, स्थिर अक्षर *src, माप_प्रकार n);

bool Curl_safecmp(स्थिर अक्षर *a, स्थिर अक्षर *b);
पूर्णांक Curl_timestrcmp(स्थिर अक्षर *a, स्थिर अक्षर *b);

#पूर्ण_अगर /* HEADER_CURL_STRCASE_H */
