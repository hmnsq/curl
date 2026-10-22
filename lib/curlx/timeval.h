#अगर_अघोषित HEADER_CURL_TIMEVAL_H
#घोषणा HEADER_CURL_TIMEVAL_H
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
#समावेश "curl_setup.h"

#समावेश "curlx/timediff.h"

काष्ठा curltime {
  समय_प्रकार tv_sec; /* seconds */
  पूर्णांक tv_usec;   /* microseconds */
};

#अगर_घोषित _WIN32
/* For tool or tests, we must initialize before calling curlx_now() */
व्योम curlx_now_init(व्योम);
#पूर्ण_अगर

काष्ठा curltime curlx_now(व्योम);
व्योम curlx_pnow(काष्ठा curltime *pnow);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds.
 */
timediff_t curlx_timediff_ms(काष्ठा curltime newer, काष्ठा curltime older);
timediff_t curlx_ptimediff_ms(स्थिर काष्ठा curltime *newer,
                              स्थिर काष्ठा curltime *older);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of milliseconds, rounded up.
 */
timediff_t curlx_timediff_ceil_ms(काष्ठा curltime newer,
                                  काष्ठा curltime older);

/* Returns milliseconds from microseconds, rounded up. */
timediff_t curlx_us_to_ceil_ms(timediff_t us);

/*
 * Make sure that the first argument (newer) is the more recent time and older
 * is the older time, as otherwise you get a weird negative time-diff back...
 *
 * Returns: the time difference in number of microseconds.
 */
timediff_t curlx_timediff_us(काष्ठा curltime newer, काष्ठा curltime older);
timediff_t curlx_ptimediff_us(स्थिर काष्ठा curltime *newer,
                              स्थिर काष्ठा curltime *older);

CURLcode curlx_gmtime(समय_प्रकार intime, काष्ठा पंचांग *store);

#पूर्ण_अगर /* HEADER_CURL_TIMEVAL_H */
