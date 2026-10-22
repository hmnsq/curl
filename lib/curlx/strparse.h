#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_STRPARSE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_STRPARSE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_OK       0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_BIG      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_SHORT    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_BEGQUOTE 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_ENDQUOTE 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_BYTE     5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_NEWLINE  6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_ਉਛਾਲ 7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRE_NO_ਅੰਕ   8

/* public struct, but all accesses should be done using the provided
   functions */
ਬਣਤਰ Curl_str {
  ਅਚਲ ਅੱਖਰ *str;
  size_t len;
};

ਖਾਲੀ curlx_str_init(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ);
ਖਾਲੀ curlx_str_ਸੌਂਪੋ(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, ਅਚਲ ਅੱਖਰ *str, size_t len);
ਖਾਲੀ curlx_str_trim(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, size_t len);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_str(x)    ((x)->str)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
ਪੂਰਨ curlx_str_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, ਅਚਲ size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
ਪੂਰਨ curlx_str_until(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, ਅਚਲ size_t max,
                    ਅੱਖਰ delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ਪੂਰਨ curlx_str_untilnl(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                      ਅਚਲ size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ਪੂਰਨ curlx_str_quotedword(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                         ਅਚਲ size_t max);

/* Advance over a single character.
   return non-zero on error */
ਪੂਰਨ curlx_str_single(ਅਚਲ ਅੱਖਰ **linep, ਅੱਖਰ byte);

/* Advance over a single space.
   return non-zero on error */
ਪੂਰਨ curlx_str_singlespace(ਅਚਲ ਅੱਖਰ **linep);

/* Get an unsigned decimal number. Return non-zero on error */
ਪੂਰਨ curlx_str_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
ਪੂਰਨ curlx_str_numblanks(ਅਚਲ ਅੱਖਰ **str, curl_off_t *ਅੰਕ);

/* Get an unsigned hexadecimal number. Return non-zero on error */
ਪੂਰਨ curlx_str_hex(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
ਪੂਰਨ curlx_str_octal(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
ਪੂਰਨ curlx_str_newline(ਅਚਲ ਅੱਖਰ **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
ਪੂਰਨ curlx_str_casecompare(ਬਣਤਰ Curl_str *str, ਅਚਲ ਅੱਖਰ *check);
ਪੂਰਨ curlx_str_cmp(ਬਣਤਰ Curl_str *str, ਅਚਲ ਅੱਖਰ *check);

ਪੂਰਨ curlx_str_nudge(ਬਣਤਰ Curl_str *str, size_t ਅੰਕ);

ਪੂਰਨ curlx_str_cspn(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                   ਅਚਲ ਅੱਖਰ *reject);
ਖਾਲੀ curlx_str_trimblanks(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ);
ਖਾਲੀ curlx_str_passblanks(ਅਚਲ ਅੱਖਰ **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
ਬਾਹਰੀ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ curlx_hexasciitable[];
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_hexval(x) (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#ਜੇ_ਅੰਤ /* HEADER_CURL_STRPARSE_H */
