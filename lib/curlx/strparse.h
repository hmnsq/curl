#ifndef HEADER_CURL_STRPARSE_H
#define HEADER_CURL_STRPARSE_H
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

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_ಅಂಕೆ   8

/* public struct, but all accesses should be done using the provided
   functions */
ಸಂರಚನಾ Curl_str {
  ಧ್ರುವ ವರ್ಣ *str;
  size_t len;
};

ಶೂನ್ಯ curlx_str_init(ಸಂರಚನಾ Curl_str *out);
ಶೂನ್ಯ curlx_str_assign(ಸಂರಚನಾ Curl_str *out, ಧ್ರುವ ವರ್ಣ *str, size_t len);
ಶೂನ್ಯ curlx_str_trim(ಸಂರಚನಾ Curl_str *out, size_t len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_word(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out, ಧ್ರುವ size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_until(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out, ಧ್ರುವ size_t max,
                    ವರ್ಣ delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_untilnl(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                      ಧ್ರುವ size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_quotedword(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                         ಧ್ರುವ size_t max);

/* Advance over a single character.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_single(ಧ್ರುವ ವರ್ಣ **linep, ವರ್ಣ byte);

/* Advance over a single space.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_singlespace(ಧ್ರುವ ವರ್ಣ **linep);

/* Get an unsigned decimal number. Return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_ಸಂಖ್ಯೆ(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
ಪೂರ್ಣಾಙ್ಕ curlx_str_numblanks(ಧ್ರುವ ವರ್ಣ **str, curl_off_t *ಅಂಕೆ);

/* Get an unsigned hexadecimal number. Return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_hex(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_octal(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_newline(ಧ್ರುವ ವರ್ಣ **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
ಪೂರ್ಣಾಙ್ಕ curlx_str_casecompare(ಸಂರಚನಾ Curl_str *str, ಧ್ರುವ ವರ್ಣ *check);
ಪೂರ್ಣಾಙ್ಕ curlx_str_cmp(ಸಂರಚನಾ Curl_str *str, ಧ್ರುವ ವರ್ಣ *check);

ಪೂರ್ಣಾಙ್ಕ curlx_str_nudge(ಸಂರಚನಾ Curl_str *str, size_t ಅಂಕೆ);

ಪೂರ್ಣಾಙ್ಕ curlx_str_cspn(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                   ಧ್ರುವ ವರ್ಣ *reject);
ಶೂನ್ಯ curlx_str_trimblanks(ಸಂರಚನಾ Curl_str *out);
ಶೂನ್ಯ curlx_str_passblanks(ಧ್ರುವ ವರ್ಣ **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
ಬಾಹ್ಯ ಧ್ರುವ unsigned ವರ್ಣ curlx_hexasciitable[];
#define curlx_hexval(x) (unsigned ವರ್ಣ)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
