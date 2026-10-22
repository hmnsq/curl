#నిర్వచితం_కాకపోతే శీర్షిక_CURL_TIMEVAL_H
#నిర్వచించు శీర్షిక_CURL_TIMEVAL_H
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

#అన్తర్భావ "curlx/timediff.h"

నిర్మాణం curltime {
  time_t tv_sec; /* seconds */
  పూర్ణాంకం tv_usec;   /* microseconds */
};

#నిర్వచితమైతే _WIN32
/* For tool or tests, we must initialize before calling curlx_now() */
శూన్య curlx_now_init(శూన్య);
#అయితే_ముగింపు

నిర్మాణం curltime curlx_now(శూన్య);
శూన్య curlx_pnow(నిర్మాణం curltime *pnow);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
timediff_t curlx_timediff_ms(నిర్మాణం curltime newer, నిర్మాణం curltime older);
timediff_t curlx_ptimediff_ms(స్థిరరాశి నిర్మాణం curltime *newer,
                              స్థిరరాశి నిర్మాణం curltime *older);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds, rounded up.
 */
timediff_t curlx_timediff_ceil_ms(నిర్మాణం curltime newer,
                                  నిర్మాణం curltime older);

/* Returns milliseconds from microseconds, rounded up. */
timediff_t curlx_us_గమ్యం_ceil_ms(timediff_t us);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of microseconds.
 */
timediff_t curlx_timediff_us(నిర్మాణం curltime newer, నిర్మాణం curltime older);
timediff_t curlx_ptimediff_us(స్థిరరాశి నిర్మాణం curltime *newer,
                              స్థిరరాశి నిర్మాణం curltime *older);

CURLcode curlx_gmtime(time_t intime, నిర్మాణం tm *store);

#అయితే_ముగింపు /* HEADER_CURL_TIMEVAL_H */
