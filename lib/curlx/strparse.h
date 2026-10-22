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
#അന്തര്ഭാവ "curl_setup.h"

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_അക്കം   8

/* public struct, but all accesses should be done using the provided
   functions */
ഘടന Curl_str {
  സ്ഥിരം അക്ഷരം *str;
  size_t len;
};

ശൂന്യം curlx_str_init(ഘടന Curl_str *out);
ശൂന്യം curlx_str_assign(ഘടന Curl_str *out, സ്ഥിരം അക്ഷരം *str, size_t len);
ശൂന്യം curlx_str_trim(ഘടന Curl_str *out, size_t len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_word(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out, സ്ഥിരം size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_until(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out, സ്ഥിരം size_t max,
                    അക്ഷരം delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_untilnl(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                      സ്ഥിരം size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_quotedword(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                         സ്ഥിരം size_t max);

/* Advance over a single character.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_single(സ്ഥിരം അക്ഷരം **linep, അക്ഷരം byte);

/* Advance over a single space.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_singlespace(സ്ഥിരം അക്ഷരം **linep);

/* Get an unsigned decimal number. Return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_സംഖ്യ(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
പൂർണ്ണസംഖ്യ curlx_str_numblanks(സ്ഥിരം അക്ഷരം **str, curl_off_t *അക്കം);

/* Get an unsigned hexadecimal number. Return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_hex(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_octal(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_newline(സ്ഥിരം അക്ഷരം **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
പൂർണ്ണസംഖ്യ curlx_str_casecompare(ഘടന Curl_str *str, സ്ഥിരം അക്ഷരം *check);
പൂർണ്ണസംഖ്യ curlx_str_cmp(ഘടന Curl_str *str, സ്ഥിരം അക്ഷരം *check);

പൂർണ്ണസംഖ്യ curlx_str_nudge(ഘടന Curl_str *str, size_t അക്കം);

പൂർണ്ണസംഖ്യ curlx_str_cspn(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                   സ്ഥിരം അക്ഷരം *reject);
ശൂന്യം curlx_str_trimblanks(ഘടന Curl_str *out);
ശൂന്യം curlx_str_passblanks(സ്ഥിരം അക്ഷരം **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
ബാഹ്യ സ്ഥിരം unsigned അക്ഷരം curlx_hexasciitable[];
#define curlx_hexval(x) (unsigned അക്ഷരം)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
