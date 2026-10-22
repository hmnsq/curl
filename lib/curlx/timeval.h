#ifndef HEADER_CURL_TIMEVAL_H
#define HEADER_CURL_TIMEVAL_H
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

#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"

ಸಂರಚನಾ curltime {
  time_t tv_sec; /* seconds */
  ಪೂರ್ಣಾಙ್ಕ tv_usec;   /* microseconds */
};

#ifdef _WIN32
/* For tool or tests, we must initialize before calling curlx_now() */
ಶೂನ್ಯ curlx_now_init(ಶೂನ್ಯ);
#endif

ಸಂರಚನಾ curltime curlx_now(ಶೂನ್ಯ);
ಶೂನ್ಯ curlx_pnow(ಸಂರಚನಾ curltime *pnow);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
timediff_t curlx_timediff_ms(ಸಂರಚನಾ curltime newer, ಸಂರಚನಾ curltime older);
timediff_t curlx_ptimediff_ms(ಧ್ರುವ ಸಂರಚನಾ curltime *newer,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *older);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds, rounded up.
 */
timediff_t curlx_timediff_ceil_ms(ಸಂರಚನಾ curltime newer,
                                  ಸಂರಚನಾ curltime older);

/* Returns milliseconds from microseconds, rounded up. */
timediff_t curlx_us_to_ceil_ms(timediff_t us);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of microseconds.
 */
timediff_t curlx_timediff_us(ಸಂರಚನಾ curltime newer, ಸಂರಚನಾ curltime older);
timediff_t curlx_ptimediff_us(ಧ್ರುವ ಸಂರಚನಾ curltime *newer,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *older);

CURLcode curlx_gmtime(time_t intime, ಸಂರಚನಾ tm *store);

#endif /* HEADER_CURL_TIMEVAL_H */
