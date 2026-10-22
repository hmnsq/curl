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

#شامل "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

ساکن صحیح casecompare(ثابت حرف *پہلا, ثابت حرف *دوسرا)
{
  جب(*پہلا) {
    اگر(Curl_raw_toupper(*پہلا) != Curl_raw_toupper(*دوسرا))
      /* get out of the loop as soon as they do not match */
      واپس 0;
    پہلا++;
    دوسرا++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  واپس !*پہلا == !*دوسرا;
}

ساکن صحیح ncasecompare(ثابت حرف *پہلا, ثابت حرف *دوسرا, size_t max)
{
  جب(*پہلا && max) {
    اگر(Curl_raw_toupper(*پہلا) != Curl_raw_toupper(*دوسرا))
      واپس 0;
    max--;
    پہلا++;
    دوسرا++;
  }
  اگر(max == 0)
    واپس 1; /* they are equal this far */

  واپس Curl_raw_toupper(*پہلا) == Curl_raw_toupper(*دوسرا);
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
صحیح curl_strequal(ثابت حرف *s1, ثابت حرف *s2)
{
  اگر(s1 && s2)
    /* both pointers point to something then compare them */
    واپس casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  واپس تہی == s1 && تہی == s2;
}

/* --- public function --- */
صحیح curl_strnequal(ثابت حرف *s1, ثابت حرف *s2, size_t n)
{
  اگر(s1 && s2)
    /* both pointers point to something then compare them */
    واپس ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  واپس تہی == s1 && تہی == s2 && n;
}
