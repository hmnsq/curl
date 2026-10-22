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
#अन्तर्भाव "curl_setup.h"

#define STRE_OK       0
#define STRE_BIG      1
#define STRE_SHORT    2
#define STRE_BEGQUOTE 3
#define STRE_ENDQUOTE 4
#define STRE_BYTE     5
#define STRE_NEWLINE  6
#define STRE_OVERFLOW 7
#define STRE_NO_अङ्क   8

/* public struct, but all accesses should be done using the provided
   functions */
संरचना Curl_str {
  अचर वर्ण *str;
  माप_प्रकार len;
};

शून्य curlx_str_init(संरचना Curl_str *out);
शून्य curlx_str_assign(संरचना Curl_str *out, अचर वर्ण *str, माप_प्रकार len);
शून्य curlx_str_trim(संरचना Curl_str *out, माप_प्रकार len);

#define curlx_str(x)    ((x)->str)
#define curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
पूर्णाङ्क curlx_str_word(अचर वर्ण **linep, संरचना Curl_str *out, अचर माप_प्रकार max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
पूर्णाङ्क curlx_str_until(अचर वर्ण **linep, संरचना Curl_str *out, अचर माप_प्रकार max,
                    वर्ण delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_untilnl(अचर वर्ण **linep, संरचना Curl_str *out,
                      अचर माप_प्रकार max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णाङ्क curlx_str_quotedword(अचर वर्ण **linep, संरचना Curl_str *out,
                         अचर माप_प्रकार max);

/* Advance over a single character.
   return non-zero on error */
पूर्णाङ्क curlx_str_single(अचर वर्ण **linep, वर्ण byte);

/* Advance over a single space.
   return non-zero on error */
पूर्णाङ्क curlx_str_singlespace(अचर वर्ण **linep);

/* Get an unsigned decimal number. Return non-zero on error */
पूर्णाङ्क curlx_str_सङ्ख्या(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
पूर्णाङ्क curlx_str_numblanks(अचर वर्ण **str, curl_off_t *अङ्क);

/* Get an unsigned hexadecimal number. Return non-zero on error */
पूर्णाङ्क curlx_str_hex(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
पूर्णाङ्क curlx_str_octal(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
पूर्णाङ्क curlx_str_newline(अचर वर्ण **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
पूर्णाङ्क curlx_str_casecompare(संरचना Curl_str *str, अचर वर्ण *check);
पूर्णाङ्क curlx_str_cmp(संरचना Curl_str *str, अचर वर्ण *check);

पूर्णाङ्क curlx_str_nudge(संरचना Curl_str *str, माप_प्रकार अङ्क);

पूर्णाङ्क curlx_str_cspn(अचर वर्ण **linep, संरचना Curl_str *out,
                   अचर वर्ण *reject);
शून्य curlx_str_trimblanks(संरचना Curl_str *out);
शून्य curlx_str_passblanks(अचर वर्ण **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
बाह्य अचर अचिन्हित वर्ण curlx_hexasciitable[];
#define curlx_hexval(x) (अचिन्हित वर्ण)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#endif /* HEADER_CURL_STRPARSE_H */
