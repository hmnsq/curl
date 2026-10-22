#اگر_تعریف_نشده سرآیند_CURL_TIMEVAL_H
#تعریف_کن سرآیند_CURL_TIMEVAL_H
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
#ضمن "curl_setup.h"

#ضمن "curlx/timediff.h"

بنية curltime {
  time_t tv_sec; /* seconds */
  صحيح tv_usec;   /* microseconds */
};

#اگر_تعریف_شده _WIN32
/* For tool or tests, we must initialize before calling curlx_now() */
فراغ curlx_now_init(فراغ);
#پایان_اگر

بنية curltime curlx_now(فراغ);
فراغ curlx_pnow(بنية curltime *pnow);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
timediff_t curlx_timediff_ms(بنية curltime newer, بنية curltime older);
timediff_t curlx_ptimediff_ms(ثابت بنية curltime *newer,
                              ثابت بنية curltime *older);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds, rounded up.
 */
timediff_t curlx_timediff_ceil_ms(بنية curltime newer,
                                  بنية curltime older);

/* Returns milliseconds from microseconds, rounded up. */
timediff_t curlx_us_مقصد_ceil_ms(timediff_t us);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of microseconds.
 */
timediff_t curlx_timediff_us(بنية curltime newer, بنية curltime older);
timediff_t curlx_ptimediff_us(ثابت بنية curltime *newer,
                              ثابت بنية curltime *older);

CURLcode curlx_gmtime(time_t intime, بنية tm *store);

#پایان_اگر /* HEADER_CURL_TIMEVAL_H */
