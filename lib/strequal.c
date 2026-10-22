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

स्थिर पूर्णाङ्क casecompare(ध्रुव वर्ण *first, ध्रुव वर्ण *second)
{
  याव(*first) {
    सचे(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      पटिनिवत्त 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  पटिनिवत्त !*first == !*second;
}

स्थिर पूर्णाङ्क ncasecompare(ध्रुव वर्ण *first, ध्रुव वर्ण *second, size_t max)
{
  याव(*first && max) {
    सचे(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      पटिनिवत्त 0;
    max--;
    first++;
    second++;
  }
  सचे(max == 0)
    पटिनिवत्त 1; /* they are equal this far */

  पटिनिवत्त Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
पूर्णाङ्क curl_strequal(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2)
{
  सचे(s1 && s2)
    /* both pointers point to something then compare them */
    पटिनिवत्त casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  पटिनिवत्त रिक्त == s1 && रिक्त == s2;
}

/* --- public function --- */
पूर्णाङ्क curl_strnequal(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, size_t n)
{
  सचे(s1 && s2)
    /* both pointers point to something then compare them */
    पटिनिवत्त ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  पटिनिवत्त रिक्त == s1 && रिक्त == s2 && n;
}
