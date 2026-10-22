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
#include "curl_setup.h"

#include "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

static 整数 casecompare(const char *first, const char *second)
{
  当(*first) {
    如果(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      返回 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  返回 !*first == !*second;
}

static 整数 ncasecompare(const char *first, const char *second, size_t max)
{
  当(*first && max) {
    如果(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      返回 0;
    max--;
    first++;
    second++;
  }
  如果(max == 0)
    返回 1; /* they are equal this far */

  返回 Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
整数 curl_strequal(const char *s1, const char *s2)
{
  如果(s1 && s2)
    /* both pointers point to something then compare them */
    返回 casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  返回 NULL == s1 && NULL == s2;
}

/* --- public function --- */
整数 curl_strnequal(const char *s1, const char *s2, size_t n)
{
  如果(s1 && s2)
    /* both pointers point to something then compare them */
    返回 ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  返回 NULL == s1 && NULL == s2 && n;
}
