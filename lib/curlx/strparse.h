#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_STRPARSE_H
#وصف_ڪريو سرو_CURL_STRPARSE_H
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

#وصف_ڪريو STRE_OK       0
#وصف_ڪريو STRE_BIG      1
#وصف_ڪريو STRE_SHORT    2
#وصف_ڪريو STRE_BEGQUOTE 3
#وصف_ڪريو STRE_ENDQUOTE 4
#وصف_ڪريو STRE_BYTE     5
#وصف_ڪريو STRE_NEWLINE  6
#وصف_ڪريو STRE_حد_کان_مٿي 7
#وصف_ڪريو STRE_NO_انگ   8

/* public struct, but all accesses should be done using the provided
   functions */
بنية Curl_str {
  ثابت حرف *str;
  size_t len;
};

فراغ curlx_str_init(بنية Curl_str *ٻاهريون);
فراغ curlx_str_سونپيو(بنية Curl_str *ٻاهريون, ثابت حرف *str, size_t len);
فراغ curlx_str_trim(بنية Curl_str *ٻاهريون, size_t len);

#وصف_ڪريو curlx_str(x)    ((x)->str)
#وصف_ڪريو curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
صحيح curlx_str_لفظ(ثابت حرف **linep, بنية Curl_str *ٻاهريون, ثابت size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
صحيح curlx_str_until(ثابت حرف **linep, بنية Curl_str *ٻاهريون, ثابت size_t max,
                    حرف delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_untilnl(ثابت حرف **linep, بنية Curl_str *ٻاهريون,
                      ثابت size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحيح curlx_str_quotedword(ثابت حرف **linep, بنية Curl_str *ٻاهريون,
                         ثابت size_t max);

/* Advance over a single character.
   return non-zero on error */
صحيح curlx_str_single(ثابت حرف **linep, حرف byte);

/* Advance over a single space.
   return non-zero on error */
صحيح curlx_str_singlespace(ثابت حرف **linep);

/* Get an unsigned decimal number. Return non-zero on error */
صحيح curlx_str_عدد(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
صحيح curlx_str_numblanks(ثابت حرف **str, curl_off_t *انگ);

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

صحيح curlx_str_nudge(بنية Curl_str *str, size_t انگ);

صحيح curlx_str_cspn(ثابت حرف **linep, بنية Curl_str *ٻاهريون,
                   ثابت حرف *reject);
فراغ curlx_str_trimblanks(بنية Curl_str *ٻاهريون);
فراغ curlx_str_passblanks(ثابت حرف **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
خارجي ثابت بنانشان حرف curlx_hexasciitable[];
#وصف_ڪريو curlx_hexval(x) (بنانشان حرف)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#جيڪڏهن_پورو /* HEADER_CURL_STRPARSE_H */
