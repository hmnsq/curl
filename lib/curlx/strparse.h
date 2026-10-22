#اگر_تعریف_نشده سرآیند_CURL_STRPARSE_H
#تعریف_کن سرآیند_CURL_STRPARSE_H
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

#تعریف_کن STRE_OK       0
#تعریف_کن STRE_BIG      1
#تعریف_کن STRE_SHORT    2
#تعریف_کن STRE_BEGQUOTE 3
#تعریف_کن STRE_ENDQUOTE 4
#تعریف_کن STRE_BYTE     5
#تعریف_کن STRE_NEWLINE  6
#تعریف_کن STRE_سرریز 7
#تعریف_کن STRE_NO_رقم   8

/* public struct, but all accesses should be done using the provided
   functions */
بنية Curl_str {
  ثابت حرف *str;
  size_t len;
};

فراغ curlx_str_init(بنية Curl_str *بیرون);
فراغ curlx_str_تخصیص_بده(بنية Curl_str *بیرون, ثابت حرف *str, size_t len);
فراغ curlx_str_trim(بنية Curl_str *بیرون, size_t len);

#تعریف_کن curlx_str(x)    ((x)->str)
#تعریف_کن curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
صحيح curlx_str_کلمه(ثابت حرف **linep, بنية Curl_str *بیرون, ثابت size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
صحيح curlx_str_until(ثابت حرف **linep, بنية Curl_str *بیرون, ثابت size_t max,
                    حرف delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_untilnl(ثابت حرف **linep, بنية Curl_str *بیرون,
                      ثابت size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحيح curlx_str_quotedword(ثابت حرف **linep, بنية Curl_str *بیرون,
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
صحيح curlx_str_numblanks(ثابت حرف **str, curl_off_t *رقم);

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

صحيح curlx_str_nudge(بنية Curl_str *str, size_t رقم);

صحيح curlx_str_cspn(ثابت حرف **linep, بنية Curl_str *بیرون,
                   ثابت حرف *reject);
فراغ curlx_str_trimblanks(بنية Curl_str *بیرون);
فراغ curlx_str_passblanks(ثابت حرف **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
خارجي ثابت بیعلامت حرف curlx_hexasciitable[];
#تعریف_کن curlx_hexval(x) (بیعلامت حرف)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#پایان_اگر /* HEADER_CURL_STRPARSE_H */
