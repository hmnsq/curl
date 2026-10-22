#अगर_अघोषित HEADER_CURL_STRPARSE_H
#घोषणा HEADER_CURL_STRPARSE_H
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
#समावेश "curl_setup.h"

#घोषणा STRE_OK       0
#घोषणा STRE_BIG      1
#घोषणा STRE_SHORT    2
#घोषणा STRE_BEGQUOTE 3
#घोषणा STRE_ENDQUOTE 4
#घोषणा STRE_BYTE     5
#घोषणा STRE_NEWLINE  6
#घोषणा STRE_OVERFLOW 7
#घोषणा STRE_NO_अंक   8

/* public struct, but all accesses should be done using the provided
   functions */
काष्ठा Curl_str {
  स्थिर अक्षर *str;
  माप_प्रकार len;
};

व्योम curlx_str_init(काष्ठा Curl_str *out);
व्योम curlx_str_assign(काष्ठा Curl_str *out, स्थिर अक्षर *str, माप_प्रकार len);
व्योम curlx_str_trim(काष्ठा Curl_str *out, माप_प्रकार len);

#घोषणा curlx_str(x)    ((x)->str)
#घोषणा curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
पूर्णांक curlx_str_word(स्थिर अक्षर **linep, काष्ठा Curl_str *out, स्थिर माप_प्रकार max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
पूर्णांक curlx_str_until(स्थिर अक्षर **linep, काष्ठा Curl_str *out, स्थिर माप_प्रकार max,
                    अक्षर delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णांक curlx_str_untilnl(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                      स्थिर माप_प्रकार max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णांक curlx_str_quotedword(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                         स्थिर माप_प्रकार max);

/* Advance over a single character.
   return non-zero on error */
पूर्णांक curlx_str_single(स्थिर अक्षर **linep, अक्षर byte);

/* Advance over a single space.
   return non-zero on error */
पूर्णांक curlx_str_singlespace(स्थिर अक्षर **linep);

/* Get an unsigned decimal number. Return non-zero on error */
पूर्णांक curlx_str_संख्या(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
पूर्णांक curlx_str_numblanks(स्थिर अक्षर **str, curl_off_t *अंक);

/* Get an unsigned hexadecimal number. Return non-zero on error */
पूर्णांक curlx_str_hex(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
पूर्णांक curlx_str_octal(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
पूर्णांक curlx_str_newline(स्थिर अक्षर **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
पूर्णांक curlx_str_casecompare(काष्ठा Curl_str *str, स्थिर अक्षर *check);
पूर्णांक curlx_str_cmp(काष्ठा Curl_str *str, स्थिर अक्षर *check);

पूर्णांक curlx_str_nudge(काष्ठा Curl_str *str, माप_प्रकार अंक);

पूर्णांक curlx_str_cspn(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                   स्थिर अक्षर *reject);
व्योम curlx_str_trimblanks(काष्ठा Curl_str *out);
व्योम curlx_str_passblanks(स्थिर अक्षर **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
बाह्य स्थिर अचिन्हित अक्षर curlx_hexasciitable[];
#घोषणा curlx_hexval(x) (अचिन्हित अक्षर)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#पूर्ण_अगर /* HEADER_CURL_STRPARSE_H */
