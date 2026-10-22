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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

સ્થિર પૂર્ણાંક casecompare(અચલ અક્ષર *first, અચલ અક્ષર *second)
{
  જ્યારે(*first) {
    જો(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      પાછા 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  પાછા !*first == !*second;
}

સ્થિર પૂર્ણાંક ncasecompare(અચલ અક્ષર *first, અચલ અક્ષર *second, size_t max)
{
  જ્યારે(*first && max) {
    જો(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      પાછા 0;
    max--;
    first++;
    second++;
  }
  જો(max == 0)
    પાછા 1; /* they are equal this far */

  પાછા Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
પૂર્ણાંક curl_strequal(અચલ અક્ષર *s1, અચલ અક્ષર *s2)
{
  જો(s1 && s2)
    /* both pointers point to something then compare them */
    પાછા casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  પાછા ખાલી == s1 && ખાલી == s2;
}

/* --- public function --- */
પૂર્ણાંક curl_strnequal(અચલ અક્ષર *s1, અચલ અક્ષર *s2, size_t n)
{
  જો(s1 && s2)
    /* both pointers point to something then compare them */
    પાછા ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  પાછા ખાલી == s1 && ખાલી == s2 && n;
}
