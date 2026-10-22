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
#ضمن "curl_setup.h"

#ضمن "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

ساكن صحيح casecompare(ثابت حرف *پہلا, ثابت حرف *دوجا)
{
  طالما(*پہلا) {
    إذا(Curl_raw_toupper(*پہلا) != Curl_raw_toupper(*دوجا))
      /* get out of the loop as soon as they do not match */
      أرجع 0;
    پہلا++;
    دوجا++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  أرجع !*پہلا == !*دوجا;
}

ساكن صحيح ncasecompare(ثابت حرف *پہلا, ثابت حرف *دوجا, size_t max)
{
  طالما(*پہلا && max) {
    إذا(Curl_raw_toupper(*پہلا) != Curl_raw_toupper(*دوجا))
      أرجع 0;
    max--;
    پہلا++;
    دوجا++;
  }
  إذا(max == 0)
    أرجع 1; /* they are equal this far */

  أرجع Curl_raw_toupper(*پہلا) == Curl_raw_toupper(*دوجا);
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
صحيح curl_strequal(ثابت حرف *s1, ثابت حرف *s2)
{
  إذا(s1 && s2)
    /* both pointers point to something then compare them */
    أرجع casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  أرجع عدم == s1 && عدم == s2;
}

/* --- public function --- */
صحيح curl_strnequal(ثابت حرف *s1, ثابت حرف *s2, size_t n)
{
  إذا(s1 && s2)
    /* both pointers point to something then compare them */
    أرجع ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  أرجع عدم == s1 && عدم == s2 && n;
}
