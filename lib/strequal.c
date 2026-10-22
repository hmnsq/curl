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

#ಅನ್ತರ್ಭಾವ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ casecompare(ಧ್ರುವ ವರ್ಣ *first, ಧ್ರುವ ವರ್ಣ *second)
{
  ಯಾವತ್(*first) {
    ಯದಿ(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      ಪ್ರತ್ಯಾಗಮ 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  ಪ್ರತ್ಯಾಗಮ !*first == !*second;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ncasecompare(ಧ್ರುವ ವರ್ಣ *first, ಧ್ರುವ ವರ್ಣ *second, size_t max)
{
  ಯಾವತ್(*first && max) {
    ಯದಿ(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      ಪ್ರತ್ಯಾಗಮ 0;
    max--;
    first++;
    second++;
  }
  ಯದಿ(max == 0)
    ಪ್ರತ್ಯಾಗಮ 1; /* they are equal this far */

  ಪ್ರತ್ಯಾಗಮ Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
ಪೂರ್ಣಾಙ್ಕ curl_strequal(ಧ್ರುವ ವರ್ಣ *s1, ಧ್ರುವ ವರ್ಣ *s2)
{
  ಯದಿ(s1 && s2)
    /* both pointers point to something then compare them */
    ಪ್ರತ್ಯಾಗಮ casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ == s1 && ರಿಕ್ತ == s2;
}

/* --- public function --- */
ಪೂರ್ಣಾಙ್ಕ curl_strnequal(ಧ್ರುವ ವರ್ಣ *s1, ಧ್ರುವ ವರ್ಣ *s2, size_t n)
{
  ಯದಿ(s1 && s2)
    /* both pointers point to something then compare them */
    ಪ್ರತ್ಯಾಗಮ ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ == s1 && ರಿಕ್ತ == s2 && n;
}
