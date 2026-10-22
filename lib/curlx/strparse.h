#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_STRPARSE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_STRPARSE_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿಸು STRE_OK       0
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_BIG      1
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_SHORT    2
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_BEGQUOTE 3
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_ENDQUOTE 4
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_BYTE     5
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_NEWLINE  6
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_ಅತಿಪ್ರವಾಹ 7
#ವ್ಯಾಖ್ಯಾನಿಸು STRE_NO_ಅಂಕೆ   8

/* public struct, but all accesses should be done using the provided
   functions */
ರಚನೆ Curl_str {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
  size_t len;
};

ಶೂನ್ಯ curlx_str_init(ರಚನೆ Curl_str *ಹೊರಹೋಗು);
ಶೂನ್ಯ curlx_str_ನಿಯೋಜಿಸು(ರಚನೆ Curl_str *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len);
ಶೂನ್ಯ curlx_str_trim(ರಚನೆ Curl_str *ಹೊರಹೋಗು, size_t len);

#ವ್ಯಾಖ್ಯಾನಿಸು curlx_str(x)    ((x)->str)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_until(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ size_t max,
                    ಅಕ್ಷರ delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_untilnl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                      ಸ್ಥಿರಾಂಕ size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_quotedword(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                         ಸ್ಥಿರಾಂಕ size_t max);

/* Advance over a single character.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_single(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ಅಕ್ಷರ byte);

/* Advance over a single space.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_singlespace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep);

/* Get an unsigned decimal number. Return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
ಪೂರ್ಣಾಂಕ curlx_str_numblanks(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **str, curl_off_t *ಅಂಕೆ);

/* Get an unsigned hexadecimal number. Return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_hex(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_octal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_newline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
ಪೂರ್ಣಾಂಕ curlx_str_casecompare(ರಚನೆ Curl_str *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check);
ಪೂರ್ಣಾಂಕ curlx_str_cmp(ರಚನೆ Curl_str *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check);

ಪೂರ್ಣಾಂಕ curlx_str_nudge(ರಚನೆ Curl_str *str, size_t ಅಂಕೆ);

ಪೂರ್ಣಾಂಕ curlx_str_cspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *reject);
ಶೂನ್ಯ curlx_str_trimblanks(ರಚನೆ Curl_str *ಹೊರಹೋಗು);
ಶೂನ್ಯ curlx_str_passblanks(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ curlx_hexasciitable[];
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_hexval(x) (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_STRPARSE_H */
