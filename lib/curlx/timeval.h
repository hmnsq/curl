#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_TIMEVAL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_TIMEVAL_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timediff.h"

ਬਣਤਰ curltime {
  time_t tv_sec; /* seconds */
  ਪੂਰਨ tv_usec;   /* microseconds */
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
/* For tool or tests, we must initialize before calling curlx_now() */
ਖਾਲੀ curlx_now_init(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਬਣਤਰ curltime curlx_now(ਖਾਲੀ);
ਖਾਲੀ curlx_pnow(ਬਣਤਰ curltime *pnow);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
timediff_t curlx_timediff_ms(ਬਣਤਰ curltime newer, ਬਣਤਰ curltime older);
timediff_t curlx_ptimediff_ms(ਅਚਲ ਬਣਤਰ curltime *newer,
                              ਅਚਲ ਬਣਤਰ curltime *older);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds, rounded up.
 */
timediff_t curlx_timediff_ceil_ms(ਬਣਤਰ curltime newer,
                                  ਬਣਤਰ curltime older);

/* Returns milliseconds from microseconds, rounded up. */
timediff_t curlx_us_ਮੰਜ਼ਿਲ_ceil_ms(timediff_t us);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of microseconds.
 */
timediff_t curlx_timediff_us(ਬਣਤਰ curltime newer, ਬਣਤਰ curltime older);
timediff_t curlx_ptimediff_us(ਅਚਲ ਬਣਤਰ curltime *newer,
                              ਅਚਲ ਬਣਤਰ curltime *older);

CURLcode curlx_gmtime(time_t intime, ਬਣਤਰ tm *store);

#ਜੇ_ਅੰਤ /* HEADER_CURL_TIMEVAL_H */
