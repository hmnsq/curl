#నిర్వచితం_కాకపోతే శీర్షిక_CURL_STRCASE_H
#నిర్వచించు శీర్షిక_CURL_STRCASE_H
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

/* Mapping tables for plain ASCII case conversion, defined in strcase.c.
   Declared here so the conversions below inline at every call site without
   relying on LTO or a unity build: casecompare() invokes one of them twice
   per byte compared, where the call costs more than the lookup itself. */
బాహ్య స్థిరరాశి గుర్తులేని అక్షరం Curl_touppermap[256];
బాహ్య స్థిరరాశి గుర్తులేని అక్షరం Curl_tolowermap[256];

/* Portable, consistent toupper/tolower. Do not use toupper()/tolower() from
   <ctype.h>, whose behavior is altered by the current locale. */
స్థిర CURL_INLINE అక్షరం Curl_raw_toupper(అక్షరం లోపల)
{
  తిరిగి (అక్షరం)Curl_touppermap[(గుర్తులేని అక్షరం)లోపల];
}

స్థిర CURL_INLINE అక్షరం Curl_raw_tolower(అక్షరం లోపల)
{
  తిరిగి (అక్షరం)Curl_tolowermap[(గుర్తులేని అక్షరం)లోపల];
}

/* checkprefix() is a shorter version of the above, used when the first
   argument is the string literal */
#నిర్వచించు checkprefix(a, b) curl_strnequal(b, STRCONST(a))

శూన్య Curl_strntoupper(అక్షరం *dest, స్థిరరాశి అక్షరం *src, size_t n);
శూన్య Curl_strntolower(అక్షరం *dest, స్థిరరాశి అక్షరం *src, size_t n);

bool Curl_safecmp(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b);
పూర్ణాంకం Curl_timestrcmp(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b);

#అయితే_ముగింపు /* HEADER_CURL_STRCASE_H */
