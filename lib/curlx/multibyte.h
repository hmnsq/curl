#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MULTIBYTE_H
#నిర్వచించు శీర్షిక_CURL_MULTIBYTE_H
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

#నిర్వచితమైతే _WIN32

/*
 * Macros curlx_convert_UTF8_to_tchar(), curlx_convert_tchar_to_UTF8()
 * main purpose is to minimize the number of preprocessor conditional
 * directives needed by code using these to differentiate Unicode from
 * non-Unicode builds.
 *
 * In the case of a non-Unicode build the tchar strings are char strings that
 * are duplicated via strdup and remain in whatever the passed in encoding is,
 * which is assumed to be UTF-8 but may be other encoding. Therefore the
 * significance of the conversion functions is primarily for Unicode builds.
 */

#నిర్వచితమైతే UNICODE

/* MultiByte conversions using Windows kernel32 library. */
wchar_t *curlx_convert_UTF8_గమ్యం_wchar(స్థిరరాశి అక్షరం *str_utf8);
అక్షరం *curlx_convert_wchar_గమ్యం_UTF8(స్థిరరాశి wchar_t *str_w);

#నిర్వచించు curlx_convert_UTF8_గమ్యం_tchar(ptr) curlx_convert_UTF8_గమ్యం_wchar(ptr)
#నిర్వచించు curlx_convert_tchar_గమ్యం_UTF8(ptr) curlx_convert_wchar_గమ్యం_UTF8(ptr)

రకనిర్వచనం సంఘం {
  గుర్తులేని పొట్టి       *tchar_ptr;
  స్థిరరాశి గుర్తులేని పొట్టి *const_tchar_ptr;
  గుర్తులేని పొట్టి       *tbyte_ptr;
  స్థిరరాశి గుర్తులేని పొట్టి *const_tbyte_ptr;
} xcharp_u;

#లేకపోతే /* !UNICODE */

#నిర్వచించు curlx_convert_UTF8_గమ్యం_tchar(ptr) curlx_strdup(ptr)
#నిర్వచించు curlx_convert_tchar_గమ్యం_UTF8(ptr) curlx_strdup(ptr)

రకనిర్వచనం సంఘం {
  అక్షరం                *tchar_ptr;
  స్థిరరాశి అక్షరం          *const_tchar_ptr;
  గుర్తులేని అక్షరం       *tbyte_ptr;
  స్థిరరాశి గుర్తులేని అక్షరం *const_tbyte_ptr;
} xcharp_u;

#అయితే_ముగింపు /* UNICODE */
#అయితే_ముగింపు /* _WIN32 */

#అయితే_ముగింపు /* HEADER_CURL_MULTIBYTE_H */
