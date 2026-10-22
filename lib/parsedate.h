#अगर_अघोषित HEADER_CURL_PARSEDATE_H
#घोषणा HEADER_CURL_PARSEDATE_H
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
बाह्य स्थिर अक्षर * स्थिर Curl_wkday[7];
बाह्य स्थिर अक्षर * स्थिर Curl_month[12];

/* Curl_getdate_capped() differs from curl_getdate() in that this returns
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. */

पूर्णांक Curl_getdate_capped(स्थिर अक्षर *p, समय_प्रकार *tp);

#पूर्ण_अगर /* HEADER_CURL_PARSEDATE_H */
