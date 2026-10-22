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

#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

ਸਥਿਰ ਪੂਰਨ casecompare(ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ, ਅਚਲ ਅੱਖਰ *ਦੂਜਾ)
{
  ਜਦੋਂ(*ਪਹਿਲਾ) {
    ਜੇ(Curl_raw_toupper(*ਪਹਿਲਾ) != Curl_raw_toupper(*ਦੂਜਾ))
      /* get out of the loop as soon as they do not match */
      ਵਾਪਸ 0;
    ਪਹਿਲਾ++;
    ਦੂਜਾ++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  ਵਾਪਸ !*ਪਹਿਲਾ == !*ਦੂਜਾ;
}

ਸਥਿਰ ਪੂਰਨ ncasecompare(ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ, ਅਚਲ ਅੱਖਰ *ਦੂਜਾ, size_t max)
{
  ਜਦੋਂ(*ਪਹਿਲਾ && max) {
    ਜੇ(Curl_raw_toupper(*ਪਹਿਲਾ) != Curl_raw_toupper(*ਦੂਜਾ))
      ਵਾਪਸ 0;
    max--;
    ਪਹਿਲਾ++;
    ਦੂਜਾ++;
  }
  ਜੇ(max == 0)
    ਵਾਪਸ 1; /* they are equal this far */

  ਵਾਪਸ Curl_raw_toupper(*ਪਹਿਲਾ) == Curl_raw_toupper(*ਦੂਜਾ);
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
ਪੂਰਨ curl_strequal(ਅਚਲ ਅੱਖਰ *s1, ਅਚਲ ਅੱਖਰ *s2)
{
  ਜੇ(s1 && s2)
    /* both pointers point to something then compare them */
    ਵਾਪਸ casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  ਵਾਪਸ ਨਿਰਾ == s1 && ਨਿਰਾ == s2;
}

/* --- public function --- */
ਪੂਰਨ curl_strnequal(ਅਚਲ ਅੱਖਰ *s1, ਅਚਲ ਅੱਖਰ *s2, size_t n)
{
  ਜੇ(s1 && s2)
    /* both pointers point to something then compare them */
    ਵਾਪਸ ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  ਵਾਪਸ ਨਿਰਾ == s1 && ਨਿਰਾ == s2 && n;
}
