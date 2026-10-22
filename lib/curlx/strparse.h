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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_ଅଙ୍କ   8

/* public struct, but all accesses should be done using the provided
   functions */
ସଂରଚନା Curl_str {
  ଅଚଳ ଅକ୍ଷର *str;
  size_t len;
};

ଶୂନ୍ୟ curlx_str_init(ସଂରଚନା Curl_str *out);
ଶୂନ୍ୟ curlx_str_assign(ସଂରଚନା Curl_str *out, ଅଚଳ ଅକ୍ଷର *str, size_t len);
ଶୂନ୍ୟ curlx_str_trim(ସଂରଚନା Curl_str *out, size_t len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_word(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out, ଅଚଳ size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_until(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out, ଅଚଳ size_t max,
                    ଅକ୍ଷର delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_untilnl(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                      ଅଚଳ size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_quotedword(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                         ଅଚଳ size_t max);

/* Advance over a single character.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_single(ଅଚଳ ଅକ୍ଷର **linep, ଅକ୍ଷର byte);

/* Advance over a single space.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_singlespace(ଅଚଳ ଅକ୍ଷର **linep);

/* Get an unsigned decimal number. Return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_numblanks(ଅଚଳ ଅକ୍ଷର **str, curl_off_t *ଅଙ୍କ);

/* Get an unsigned hexadecimal number. Return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_hex(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_octal(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_newline(ଅଚଳ ଅକ୍ଷର **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_casecompare(ସଂରଚନା Curl_str *str, ଅଚଳ ଅକ୍ଷର *check);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_cmp(ସଂରଚନା Curl_str *str, ଅଚଳ ଅକ୍ଷର *check);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_nudge(ସଂରଚନା Curl_str *str, size_t ଅଙ୍କ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_cspn(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                   ଅଚଳ ଅକ୍ଷର *reject);
ଶୂନ୍ୟ curlx_str_trimblanks(ସଂରଚନା Curl_str *out);
ଶୂନ୍ୟ curlx_str_passblanks(ଅଚଳ ଅକ୍ଷର **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
ବାହ୍ୟ ଅଚଳ unsigned ଅକ୍ଷର curlx_hexasciitable[];
#define curlx_hexval(x) (unsigned ଅକ୍ଷର)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
