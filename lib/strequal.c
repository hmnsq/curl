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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା casecompare(ଅଚଳ ଅକ୍ଷର *first, ଅଚଳ ଅକ୍ଷର *second)
{
  ଯେତେବେଳେ(*first) {
    ଯଦି(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      ଫେରନ୍ତୁ 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  ଫେରନ୍ତୁ !*first == !*second;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ncasecompare(ଅଚଳ ଅକ୍ଷର *first, ଅଚଳ ଅକ୍ଷର *second, size_t max)
{
  ଯେତେବେଳେ(*first && max) {
    ଯଦି(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      ଫେରନ୍ତୁ 0;
    max--;
    first++;
    second++;
  }
  ଯଦି(max == 0)
    ଫେରନ୍ତୁ 1; /* they are equal this far */

  ଫେରନ୍ତୁ Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_strequal(ଅଚଳ ଅକ୍ଷର *s1, ଅଚଳ ଅକ୍ଷର *s2)
{
  ଯଦି(s1 && s2)
    /* both pointers point to something then compare them */
    ଫେରନ୍ତୁ casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  ଫେରନ୍ତୁ ଖାଲି == s1 && ଖାଲି == s2;
}

/* --- public function --- */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_strnequal(ଅଚଳ ଅକ୍ଷର *s1, ଅଚଳ ଅକ୍ଷର *s2, size_t n)
{
  ଯଦି(s1 && s2)
    /* both pointers point to something then compare them */
    ଫେରନ୍ତୁ ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  ଫେରନ୍ତୁ ଖାଲି == s1 && ଖାଲି == s2 && n;
}
