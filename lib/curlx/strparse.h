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
#અન્તર્ભાવ "curl_setup.h"

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_અંક   8

/* public struct, but all accesses should be done using the provided
   functions */
સંરચના Curl_str {
  અચલ અક્ષર *str;
  size_t len;
};

શૂન્ય curlx_str_init(સંરચના Curl_str *out);
શૂન્ય curlx_str_assign(સંરચના Curl_str *out, અચલ અક્ષર *str, size_t len);
શૂન્ય curlx_str_trim(સંરચના Curl_str *out, size_t len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
પૂર્ણાંક curlx_str_word(અચલ અક્ષર **linep, સંરચના Curl_str *out, અચલ size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
પૂર્ણાંક curlx_str_until(અચલ અક્ષર **linep, સંરચના Curl_str *out, અચલ size_t max,
                    અક્ષર delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
પૂર્ણાંક curlx_str_untilnl(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                      અચલ size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
પૂર્ણાંક curlx_str_quotedword(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                         અચલ size_t max);

/* Advance over a single character.
   return non-zero on error */
પૂર્ણાંક curlx_str_single(અચલ અક્ષર **linep, અક્ષર byte);

/* Advance over a single space.
   return non-zero on error */
પૂર્ણાંક curlx_str_singlespace(અચલ અક્ષર **linep);

/* Get an unsigned decimal number. Return non-zero on error */
પૂર્ણાંક curlx_str_સંખ્યા(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
પૂર્ણાંક curlx_str_numblanks(અચલ અક્ષર **str, curl_off_t *અંક);

/* Get an unsigned hexadecimal number. Return non-zero on error */
પૂર્ણાંક curlx_str_hex(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
પૂર્ણાંક curlx_str_octal(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
પૂર્ણાંક curlx_str_newline(અચલ અક્ષર **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
પૂર્ણાંક curlx_str_casecompare(સંરચના Curl_str *str, અચલ અક્ષર *check);
પૂર્ણાંક curlx_str_cmp(સંરચના Curl_str *str, અચલ અક્ષર *check);

પૂર્ણાંક curlx_str_nudge(સંરચના Curl_str *str, size_t અંક);

પૂર્ણાંક curlx_str_cspn(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                   અચલ અક્ષર *reject);
શૂન્ય curlx_str_trimblanks(સંરચના Curl_str *out);
શૂન્ય curlx_str_passblanks(અચલ અક્ષર **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
બાહ્ય અચલ unsigned અક્ષર curlx_hexasciitable[];
#define curlx_hexval(x) (unsigned અક્ષર)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
