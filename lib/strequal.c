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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ casecompare(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎರಡನೇ)
{
  ಆಗ(*ಮೊದಲ) {
    ಆದರೆ(Curl_raw_toupper(*ಮೊದಲ) != Curl_raw_toupper(*ಎರಡನೇ))
      /* get out of the loop as soon as they do not match */
      ಹಿಂತಿರುಗಿ 0;
    ಮೊದಲ++;
    ಎರಡನೇ++;
  }
  /* If we are here either the strings are the same or the length is different.
     We can test if the "current" character is non-zero for one and zero
     for the other. Note that the characters may not be exactly the same even
     if they match, we only want to compare zero-ness. */
  ಹಿಂತಿರುಗಿ !*ಮೊದಲ == !*ಎರಡನೇ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ncasecompare(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎರಡನೇ, size_t max)
{
  ಆಗ(*ಮೊದಲ && max) {
    ಆದರೆ(Curl_raw_toupper(*ಮೊದಲ) != Curl_raw_toupper(*ಎರಡನೇ))
      ಹಿಂತಿರುಗಿ 0;
    max--;
    ಮೊದಲ++;
    ಎರಡನೇ++;
  }
  ಆದರೆ(max == 0)
    ಹಿಂತಿರುಗಿ 1; /* they are equal this far */

  ಹಿಂತಿರುಗಿ Curl_raw_toupper(*ಮೊದಲ) == Curl_raw_toupper(*ಎರಡನೇ);
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
ಪೂರ್ಣಾಂಕ curl_strequal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2)
{
  ಆದರೆ(s1 && s2)
    /* both pointers point to something then compare them */
    ಹಿಂತಿರುಗಿ casecompare(s1, s2);

  /* if both pointers are NULL then treat them as equal */
  ಹಿಂತಿರುಗಿ ಖಾಲಿ == s1 && ಖಾಲಿ == s2;
}

/* --- public function --- */
ಪೂರ್ಣಾಂಕ curl_strnequal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2, size_t n)
{
  ಆದರೆ(s1 && s2)
    /* both pointers point to something then compare them */
    ಹಿಂತಿರುಗಿ ncasecompare(s1, s2, n);

  /* if both pointers are NULL then treat them as equal if max is non-zero */
  ಹಿಂತಿರುಗಿ ಖಾಲಿ == s1 && ಖಾಲಿ == s2 && n;
}
