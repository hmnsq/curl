#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SPLAY_H
#وصف_ڪريو سرو_CURL_SPLAY_H
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

#ضمن "curlx/timeval.h"

بنية Curl_easy;

/* only use function calls to access this struct */
بنية Curl_وڻ {
  بنية Curl_وڻ *smaller; /* smaller node */
  بنية Curl_وڻ *larger;  /* larger node */
  بنية Curl_وڻ *same;    /* points to the next node with identical key */
  timediff_t ڪنجي;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  ٻٽو_انگ(registered);           /* node is registered in splay tree */
};

بنية Curl_timeouts {
  بنية Curl_وڻ *وڻ;
  time_t وقت_بنياد_sec;
};

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_بنياد);

bool Curl_timeouts_has(بنية Curl_easy *معلومات);

timediff_t Curl_timeouts_وٿي_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts);

صحيح Curl_timeouts_ايندڙ_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_وٿي_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid);

فراغ Curl_timeouts_جوڙيو(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *معلومات,
                       timediff_t وٿي_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *معلومات);

#جيڪڏهن_وصف_ٿيل UNITTESTS
uint32_t Curl_splayget(بنية Curl_وڻ *ڳنڍ);
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_SPLAY_H */
