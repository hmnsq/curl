#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_STRCASE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_STRCASE_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_touppermap[256];
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
ಸ್ಥಿರ CURL_INLINE ಅಕ್ಷರ Curl_raw_toupper(ಅಕ್ಷರ ಒಳ)
{
  ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ)Curl_touppermap[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಒಳ];
}

ಸ್ಥಿರ CURL_INLINE ಅಕ್ಷರ Curl_raw_tolower(ಅಕ್ಷರ ಒಳ)
{
  ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ)Curl_tolowermap[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಒಳ];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#ವ್ಯಾಖ್ಯಾನಿಸು checkprefix(a, b) curl_strnequal(b, STRCONST(a))

ಶೂನ್ಯ Curl_strntoupper(ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t n);
ಶೂನ್ಯ Curl_strntolower(ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t n);

bool Curl_safecmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b);
ಪೂರ್ಣಾಂಕ Curl_timestrcmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_STRCASE_H */
