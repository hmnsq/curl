#اگر_تعریف_نشده سرآیند_CURL_PARSEDATE_H
#تعریف_کن سرآیند_CURL_PARSEDATE_H
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
خارجي ثابت حرف * ثابت Curl_wkday[7];
خارجي ثابت حرف * ثابت Curl_month[12];

/* Curl_getdate_capped() differs from curl_getdate() in that this returns
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. */

صحيح Curl_getdate_capped(ثابت حرف *p, time_t *tp);

#پایان_اگر /* HEADER_CURL_PARSEDATE_H */
