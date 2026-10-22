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
#समावेश "curl_setup.h"

#समावेश "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

अटल पूर्णांक casecompare(स्थिर अक्षर *first, स्थिर अक्षर *second)
{
  जबतक(*first) {
    अगर(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      वापस 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  वापस !*first == !*second;
}

अटल पूर्णांक ncasecompare(स्थिर अक्षर *first, स्थिर अक्षर *second, माप_प्रकार max)
{
  जबतक(*first && max) {
    अगर(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      वापस 0;
    max--;
    first++;
    second++;
  }
  अगर(max == 0)
    वापस 1; /* they are equal this far */

  वापस Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
पूर्णांक curl_strequal(स्थिर अक्षर *s1, स्थिर अक्षर *s2)
{
  अगर(s1 && s2)
    /* both pointers point to something then compare them */
    वापस casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  वापस शून्य == s1 && शून्य == s2;
}

/* --- public function --- */
पूर्णांक curl_strnequal(स्थिर अक्षर *s1, स्थिर अक्षर *s2, माप_प्रकार n)
{
  अगर(s1 && s2)
    /* both pointers point to something then compare them */
    वापस ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  वापस शून्य == s1 && शून्य == s2 && n;
}
