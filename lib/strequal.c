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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

స్థిర పూర్ణాంకం casecompare(స్థిరరాశి అక్షరం *మొదటి, స్థిరరాశి అక్షరం *రెండవ)
{
  అయితే(*మొదటి) {
    ఒకవేళ(Curl_raw_toupper(*మొదటి) != Curl_raw_toupper(*రెండవ))
      /* get out of the loop as soon as they do not match */
      తిరిగి 0;
    మొదటి++;
    రెండవ++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  తిరిగి !*మొదటి == !*రెండవ;
}

స్థిర పూర్ణాంకం ncasecompare(స్థిరరాశి అక్షరం *మొదటి, స్థిరరాశి అక్షరం *రెండవ, size_t max)
{
  అయితే(*మొదటి && max) {
    ఒకవేళ(Curl_raw_toupper(*మొదటి) != Curl_raw_toupper(*రెండవ))
      తిరిగి 0;
    max--;
    మొదటి++;
    రెండవ++;
  }
  ఒకవేళ(max == 0)
    తిరిగి 1; /* they are equal this far */

  తిరిగి Curl_raw_toupper(*మొదటి) == Curl_raw_toupper(*రెండవ);
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
పూర్ణాంకం curl_strequal(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2)
{
  ఒకవేళ(s1 && s2)
    /* both pointers point to something then compare them */
    తిరిగి casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  తిరిగి ఖాళీ == s1 && ఖాళీ == s2;
}

/* --- public function --- */
పూర్ణాంకం curl_strnequal(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2, size_t n)
{
  ఒకవేళ(s1 && s2)
    /* both pointers point to something then compare them */
    తిరిగి ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  తిరిగి ఖాళీ == s1 && ఖాళీ == s2 && n;
}
