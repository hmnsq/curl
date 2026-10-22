#اگر_غیر_تعریف_شدہ سرخی_CURL_STRPARSE_H
#تعریف_کرو سرخی_CURL_STRPARSE_H
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
#شامل "curl_setup.h"

#تعریف_کرو STRE_OK       0
#تعریف_کرو STRE_BIG      1
#تعریف_کرو STRE_SHORT    2
#تعریف_کرو STRE_BEGQUOTE 3
#تعریف_کرو STRE_ENDQUOTE 4
#تعریف_کرو STRE_BYTE     5
#تعریف_کرو STRE_NEWLINE  6
#تعریف_کرو STRE_حد_سے_تجاوز 7
#تعریف_کرو STRE_NO_عدد   8

/* public struct, but all accesses should be done using the provided
   functions */
ساخت Curl_str {
  ثابت حرف *str;
  size_t len;
};

خالی curlx_str_init(ساخت Curl_str *برآمد);
خالی curlx_str_تفویض(ساخت Curl_str *برآمد, ثابت حرف *str, size_t len);
خالی curlx_str_trim(ساخت Curl_str *برآمد, size_t len);

#تعریف_کرو curlx_str(x)    ((x)->str)
#تعریف_کرو curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
صحیح curlx_str_لفظ(ثابت حرف **linep, ساخت Curl_str *برآمد, ثابت size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
صحیح curlx_str_until(ثابت حرف **linep, ساخت Curl_str *برآمد, ثابت size_t max,
                    حرف delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحیح curlx_str_untilnl(ثابت حرف **linep, ساخت Curl_str *برآمد,
                      ثابت size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحیح curlx_str_quotedword(ثابت حرف **linep, ساخت Curl_str *برآمد,
                         ثابت size_t max);

/* Advance over a single character.
   return non-zero on error */
صحیح curlx_str_single(ثابت حرف **linep, حرف byte);

/* Advance over a single space.
   return non-zero on error */
صحیح curlx_str_singlespace(ثابت حرف **linep);

/* Get an unsigned decimal number. Return non-zero on error */
صحیح curlx_str_نمبر(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
صحیح curlx_str_numblanks(ثابت حرف **str, curl_off_t *عدد);

/* Get an unsigned hexadecimal number. Return non-zero on error */
صحیح curlx_str_hex(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
صحیح curlx_str_octal(ثابت حرف **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
صحیح curlx_str_newline(ثابت حرف **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
صحیح curlx_str_casecompare(ساخت Curl_str *str, ثابت حرف *check);
صحیح curlx_str_cmp(ساخت Curl_str *str, ثابت حرف *check);

صحیح curlx_str_nudge(ساخت Curl_str *str, size_t عدد);

صحیح curlx_str_cspn(ثابت حرف **linep, ساخت Curl_str *برآمد,
                   ثابت حرف *reject);
خالی curlx_str_trimblanks(ساخت Curl_str *برآمد);
خالی curlx_str_passblanks(ثابت حرف **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
خارجی ثابت غیرعلامتی حرف curlx_hexasciitable[];
#تعریف_کرو curlx_hexval(x) (غیرعلامتی حرف)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#اگر_ختم /* HEADER_CURL_STRPARSE_H */
