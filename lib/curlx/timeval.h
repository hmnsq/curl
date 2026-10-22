#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_TIMEVAL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_TIMEVAL_H
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

ರಚನೆ curltime {
  time_t tv_sec; /* seconds */
  ಪೂರ್ಣಾಂಕ tv_usec;   /* microseconds */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
/* For tool or tests, we must initialize before calling curlx_now() */
ಶೂನ್ಯ curlx_now_init(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ curltime curlx_now(ಶೂನ್ಯ);
ಶೂನ್ಯ curlx_pnow(ರಚನೆ curltime *pnow);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
timediff_t curlx_timediff_ms(ರಚನೆ curltime newer, ರಚನೆ curltime older);
timediff_t curlx_ptimediff_ms(ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *newer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *older);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds, rounded up.
 */
timediff_t curlx_timediff_ceil_ms(ರಚನೆ curltime newer,
                                  ರಚನೆ curltime older);

/* Returns milliseconds from microseconds, rounded up. */
timediff_t curlx_us_ಗಮ್ಯ_ceil_ms(timediff_t us);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of microseconds.
 */
timediff_t curlx_timediff_us(ರಚನೆ curltime newer, ರಚನೆ curltime older);
timediff_t curlx_ptimediff_us(ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *newer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *older);

CURLcode curlx_gmtime(time_t intime, ರಚನೆ tm *store);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_TIMEVAL_H */
