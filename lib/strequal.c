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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "strcase.h"

/*
 * curl_strequal() is for doing "raw" case insensitive strings. This is meant
 * to be locale independent and only compare strings we know are safe for
 * this. See https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for
 * further explanations as to why this function is necessary.
 */

நிலையான முழு casecompare(மாறிலி எழுத்து *first, மாறிலி எழுத்து *second)
{
  போது(*first) {
    எனில்(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they do not match */
      திரும்பு 0;
    first++;
    second++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  திரும்பு !*first == !*second;
}

நிலையான முழு ncasecompare(மாறிலி எழுத்து *first, மாறிலி எழுத்து *second, size_t max)
{
  போது(*first && max) {
    எனில்(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      திரும்பு 0;
    max--;
    first++;
    second++;
  }
  எனில்(max == 0)
    திரும்பு 1; /* they are equal this far */

  திரும்பு Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
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
முழு curl_strequal(மாறிலி எழுத்து *s1, மாறிலி எழுத்து *s2)
{
  எனில்(s1 && s2)
    /* both pointers point to something then compare them */
    திரும்பு casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  திரும்பு வெற்று == s1 && வெற்று == s2;
}

/* --- public function --- */
முழு curl_strnequal(மாறிலி எழுத்து *s1, மாறிலி எழுத்து *s2, size_t n)
{
  எனில்(s1 && s2)
    /* both pointers point to something then compare them */
    திரும்பு ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  திரும்பு வெற்று == s1 && வெற்று == s2 && n;
}
