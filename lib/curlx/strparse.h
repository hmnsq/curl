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
#অন্তর্ভাব "curl_setup.h"

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_অঙ্ক   8

/* public struct, but all accesses should be done using the provided
   functions */
কাঠামো Curl_str {
  ধ্রুবক অক্ষর *str;
  size_t len;
};

শূন্য curlx_str_init(কাঠামো Curl_str *out);
শূন্য curlx_str_assign(কাঠামো Curl_str *out, ধ্রুবক অক্ষর *str, size_t len);
শূন্য curlx_str_trim(কাঠামো Curl_str *out, size_t len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_word(ধ্রুবক অক্ষর **linep, কাঠামো Curl_str *out, ধ্রুবক size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_until(ধ্রুবক অক্ষর **linep, কাঠামো Curl_str *out, ধ্রুবক size_t max,
                    অক্ষর delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_untilnl(ধ্রুবক অক্ষর **linep, কাঠামো Curl_str *out,
                      ধ্রুবক size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_quotedword(ধ্রুবক অক্ষর **linep, কাঠামো Curl_str *out,
                         ধ্রুবক size_t max);

/* Advance over a single character.
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_single(ধ্রুবক অক্ষর **linep, অক্ষর byte);

/* Advance over a single space.
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_singlespace(ধ্রুবক অক্ষর **linep);

/* Get an unsigned decimal number. Return non-zero on error */
পূর্ণসংখ্যা curlx_str_সংখ্যা(ধ্রুবক অক্ষর **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
পূর্ণসংখ্যা curlx_str_numblanks(ধ্রুবক অক্ষর **str, curl_off_t *অঙ্ক);

/* Get an unsigned hexadecimal number. Return non-zero on error */
পূর্ণসংখ্যা curlx_str_hex(ধ্রুবক অক্ষর **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
পূর্ণসংখ্যা curlx_str_octal(ধ্রুবক অক্ষর **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
পূর্ণসংখ্যা curlx_str_newline(ধ্রুবক অক্ষর **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
পূর্ণসংখ্যা curlx_str_casecompare(কাঠামো Curl_str *str, ধ্রুবক অক্ষর *check);
পূর্ণসংখ্যা curlx_str_cmp(কাঠামো Curl_str *str, ধ্রুবক অক্ষর *check);

পূর্ণসংখ্যা curlx_str_nudge(কাঠামো Curl_str *str, size_t অঙ্ক);

পূর্ণসংখ্যা curlx_str_cspn(ধ্রুবক অক্ষর **linep, কাঠামো Curl_str *out,
                   ধ্রুবক অক্ষর *reject);
শূন্য curlx_str_trimblanks(কাঠামো Curl_str *out);
শূন্য curlx_str_passblanks(ধ্রুবক অক্ষর **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
বাহ্যিক ধ্রুবক unsigned অক্ষর curlx_hexasciitable[];
#define curlx_hexval(x) (unsigned অক্ষর)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
