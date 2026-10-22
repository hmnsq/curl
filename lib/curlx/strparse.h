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
#ضمن "curl_setup.h"

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_عدد   8

/* public struct, but all accesses should be done using the provided
   functions */
بنية Curl_str {
  ثابت حرف *str;
  size_t len;
};

فراغ curlx_str_init(بنية Curl_str *out);
فراغ curlx_str_assign(بنية Curl_str *out, ثابت حرف *str, size_t len);
فراغ curlx_str_trim(بنية Curl_str *out, size_t len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
صحيح curlx_str_word(ثابت حرف **linep, بنية Curl_str *out, ثابت size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
صحيح curlx_str_until(ثابت حرف **linep, بنية Curl_str *out, ثابت size_t max,
                    حرف delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_untilnl(ثابت حرف **linep, بنية Curl_str *out,
                      ثابت size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحيح curlx_str_quotedword(ثابت حرف **linep, بنية Curl_str *out,
                         ثابت size_t max);

/* Advance over a single character.
   return non-zero on error */
صحيح curlx_str_single(ثابت حرف **linep, حرف byte);

/* Advance over a single space.
   return non-zero on error */
صحيح curlx_str_singlespace(ثابت حرف **linep);

/* Get an unsigned decimal number. Return non-zero on error */
صحيح curlx_str_نمبر(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
صحيح curlx_str_numblanks(ثابت حرف **str, curl_off_t *عدد);

/* Get an unsigned hexadecimal number. Return non-zero on error */
صحيح curlx_str_hex(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
صحيح curlx_str_octal(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
صحيح curlx_str_newline(ثابت حرف **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
صحيح curlx_str_casecompare(بنية Curl_str *str, ثابت حرف *check);
صحيح curlx_str_cmp(بنية Curl_str *str, ثابت حرف *check);

صحيح curlx_str_nudge(بنية Curl_str *str, size_t عدد);

صحيح curlx_str_cspn(ثابت حرف **linep, بنية Curl_str *out,
                   ثابت حرف *reject);
فراغ curlx_str_trimblanks(بنية Curl_str *out);
فراغ curlx_str_passblanks(ثابت حرف **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
خارجي ثابت unsigned حرف curlx_hexasciitable[];
#define curlx_hexval(x) (unsigned حرف)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
