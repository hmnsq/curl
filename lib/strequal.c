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

#अन्तर्भाव "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

स्थिर पूर्णांक casecompare(स्थिरांक अक्षर *first, स्थिरांक अक्षर *second)
{
  पर्यंत(*first) {
    जर(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      परत 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  परत !*first == !*second;
}

स्थिर पूर्णांक ncasecompare(स्थिरांक अक्षर *first, स्थिरांक अक्षर *second, माप_प्रकार max)
{
  पर्यंत(*first && max) {
    जर(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      परत 0;
    max--;
    first++;
    second++;
  }
  जर(max == 0)
    परत 1; /* they are equal this far */

  परत Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
पूर्णांक curl_strequal(स्थिरांक अक्षर *s1, स्थिरांक अक्षर *s2)
{
  जर(s1 && s2)
    /* both pointers point to something then compare them */
    परत casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  परत रिक्त == s1 && रिक्त == s2;
}

/* --- public function --- */
पूर्णांक curl_strnequal(स्थिरांक अक्षर *s1, स्थिरांक अक्षर *s2, माप_प्रकार n)
{
  जर(s1 && s2)
    /* both pointers point to something then compare them */
    परत ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  परत रिक्त == s1 && रिक्त == s2 && n;
}
