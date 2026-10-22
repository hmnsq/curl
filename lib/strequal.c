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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

സ്ഥിതം പൂർണ്ണസംഖ്യ casecompare(സ്ഥിരം അക്ഷരം *first, സ്ഥിരം അക്ഷരം *second)
{
  ആകുമ്പോൾ(*first) {
    എങ്കിൽ(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      തിരികെ 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  തിരികെ !*first == !*second;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ncasecompare(സ്ഥിരം അക്ഷരം *first, സ്ഥിരം അക്ഷരം *second, size_t max)
{
  ആകുമ്പോൾ(*first && max) {
    എങ്കിൽ(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      തിരികെ 0;
    max--;
    first++;
    second++;
  }
  എങ്കിൽ(max == 0)
    തിരികെ 1; /* they are equal this far */

  തിരികെ Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
}

/*
 * Only "raw" case insensitive strings. This is meant to be locale independent
 * and only compare strings we know are safe for this.
 *
 * The function is capable of comparing a-z case insensitively.
 *
 * Result is 1 if text matches and 0 if not.
 */

/* --- public function --- */
പൂർണ്ണസംഖ്യ curl_strequal(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2)
{
  എങ്കിൽ(s1 && s2)
    /* both pointers point to something then compare them */
    തിരികെ casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  തിരികെ ശൂന്യ == s1 && ശൂന്യ == s2;
}

/* --- public function --- */
പൂർണ്ണസംഖ്യ curl_strnequal(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2, size_t n)
{
  എങ്കിൽ(s1 && s2)
    /* both pointers point to something then compare them */
    തിരികെ ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  തിരികെ ശൂന്യ == s1 && ശൂന്യ == s2 && n;
}
