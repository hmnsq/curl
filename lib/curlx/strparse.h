#నిర్వచితం_కాకపోతే శీర్షిక_CURL_STRPARSE_H
#నిర్వచించు శీర్షిక_CURL_STRPARSE_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచించు STRE_OK       0
#నిర్వచించు STRE_BIG      1
#నిర్వచించు STRE_SHORT    2
#నిర్వచించు STRE_BEGQUOTE 3
#నిర్వచించు STRE_ENDQUOTE 4
#నిర్వచించు STRE_BYTE     5
#నిర్వచించు STRE_NEWLINE  6
#నిర్వచించు STRE_అధికప్రవాహం 7
#నిర్వచించు STRE_NO_అంకె   8

/* public struct, but all accesses should be done using the provided
   functions */
నిర్మాణం Curl_str {
  స్థిరరాశి అక్షరం *str;
  size_t len;
};

శూన్య curlx_str_init(నిర్మాణం Curl_str *బహిర్గతం);
శూన్య curlx_str_కేటాయించు(నిర్మాణం Curl_str *బహిర్గతం, స్థిరరాశి అక్షరం *str, size_t len);
శూన్య curlx_str_trim(నిర్మాణం Curl_str *బహిర్గతం, size_t len);

#నిర్వచించు curlx_str(x)    ((x)->str)
#నిర్వచించు curlx_strlen(x) ((x)->len)

/* Get a word until the first space
   return non-zero on error */
పూర్ణాంకం curlx_str_మాట(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం, స్థిరరాశి size_t max);

/* Get a word until the first DELIM or end of string
   return non-zero on error */
పూర్ణాంకం curlx_str_until(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం, స్థిరరాశి size_t max,
                    అక్షరం delim);

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
పూర్ణాంకం curlx_str_untilnl(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                      స్థిరరాశి size_t max);

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
పూర్ణాంకం curlx_str_quotedword(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                         స్థిరరాశి size_t max);

/* Advance over a single character.
   return non-zero on error */
పూర్ణాంకం curlx_str_single(స్థిరరాశి అక్షరం **linep, అక్షరం byte);

/* Advance over a single space.
   return non-zero on error */
పూర్ణాంకం curlx_str_singlespace(స్థిరరాశి అక్షరం **linep);

/* Get an unsigned decimal number. Return non-zero on error */
పూర్ణాంకం curlx_str_సంఖ్య(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max);

/* As above with CURL_OFF_T_MAX but also pass leading blanks */
పూర్ణాంకం curlx_str_numblanks(స్థిరరాశి అక్షరం **str, curl_off_t *అంకె);

/* Get an unsigned hexadecimal number. Return non-zero on error */
పూర్ణాంకం curlx_str_hex(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max);

/* Get an unsigned octal number. Return non-zero on error */
పూర్ణాంకం curlx_str_octal(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max);

/* Check for CR or LF
   return non-zero on error */
పూర్ణాంకం curlx_str_newline(స్థిరరాశి అక్షరం **linep);

/* case insensitive compare that the parsed string matches the
   given string. */
పూర్ణాంకం curlx_str_casecompare(నిర్మాణం Curl_str *str, స్థిరరాశి అక్షరం *check);
పూర్ణాంకం curlx_str_cmp(నిర్మాణం Curl_str *str, స్థిరరాశి అక్షరం *check);

పూర్ణాంకం curlx_str_nudge(నిర్మాణం Curl_str *str, size_t అంకె);

పూర్ణాంకం curlx_str_cspn(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                   స్థిరరాశి అక్షరం *reject);
శూన్య curlx_str_trimblanks(నిర్మాణం Curl_str *బహిర్గతం);
శూన్య curlx_str_passblanks(స్థిరరాశి అక్షరం **linep);

/* given a hexadecimal letter, return the binary value. '0' returns 0, 'a'
   returns 10. THIS ONLY WORKS ON VALID HEXADECIMAL LETTER INPUT. Verify
   before calling this. */
బాహ్య స్థిరరాశి గుర్తులేని అక్షరం curlx_hexasciitable[];
#నిర్వచించు curlx_hexval(x) (గుర్తులేని అక్షరం)(curlx_hexasciitable[(x) - '0'] & 0x0f)

#అయితే_ముగింపు /* HEADER_CURL_STRPARSE_H */
