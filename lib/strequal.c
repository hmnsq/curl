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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

স্থির পূর্ণসংখ্যা casecompare(ধ্রুবক অক্ষর *first, ধ্রুবক অক্ষর *second)
{
  যখন(*first) {
    যদি(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      ফেরত 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  ফেরত !*first == !*second;
}

স্থির পূর্ণসংখ্যা ncasecompare(ধ্রুবক অক্ষর *first, ধ্রুবক অক্ষর *second, size_t max)
{
  যখন(*first && max) {
    যদি(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      ফেরত 0;
    max--;
    first++;
    second++;
  }
  যদি(max == 0)
    ফেরত 1; /* they are equal this far */

  ফেরত Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
পূর্ণসংখ্যা curl_strequal(ধ্রুবক অক্ষর *s1, ধ্রুবক অক্ষর *s2)
{
  যদি(s1 && s2)
    /* both pointers point to something then compare them */
    ফেরত casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  ফেরত ফাঁকা == s1 && ফাঁকা == s2;
}

/* --- public function --- */
পূর্ণসংখ্যা curl_strnequal(ধ্রুবক অক্ষর *s1, ধ্রুবক অক্ষর *s2, size_t n)
{
  যদি(s1 && s2)
    /* both pointers point to something then compare them */
    ফেরত ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  ফেরত ফাঁকা == s1 && ফাঁকা == s2 && n;
}
