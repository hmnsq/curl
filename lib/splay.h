#اگر_غیر_تعریف_شدہ سرخی_CURL_SPLAY_H
#تعریف_کرو سرخی_CURL_SPLAY_H
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
#شامل "curl_setup.h"

#شامل "curlx/timeval.h"

ساخت Curl_easy;

/* only use function calls to access this struct */
ساخت Curl_درخت {
  ساخت Curl_درخت *smaller; /* smaller node */
  ساخت Curl_درخت *larger;  /* larger node */
  ساخت Curl_درخت *same;    /* points to the next node with identical key */
  timediff_t کلید;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  ثنائی_ہندسہ(registered);           /* node is registered in splay tree */
};

ساخت Curl_timeouts {
  ساخت Curl_درخت *درخت;
  time_t وقت_بنیاد_sec;
};

خالی Curl_timeouts_init(ساخت Curl_timeouts *timeouts,
                        ثابت ساخت curltime *ptime_بنیاد);

bool Curl_timeouts_has(ساخت Curl_easy *ڈیٹا);

timediff_t Curl_timeouts_انحراف_us(ساخت Curl_timeouts *timeouts,
                                   ثابت ساخت curltime *pts);

صحیح Curl_timeouts_اگلا_ms(ساخت Curl_timeouts *timeouts,
                          ثابت ساخت curltime *pnow,
                          timediff_t *pexpire_انحراف_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ساخت Curl_timeouts *timeouts,
                                  ثابت ساخت curltime *ts,
                                  uint32_t *pmid);

خالی Curl_timeouts_جوڑو(ساخت Curl_timeouts *timeouts,
                       ساخت Curl_easy *ڈیٹا,
                       timediff_t انحراف_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ساخت Curl_timeouts *timeouts,
                          ساخت Curl_easy *ڈیٹا);

#اگر_تعریف_شدہ UNITTESTS
uint32_t Curl_splayget(ساخت Curl_درخت *گرہ);
#اگر_ختم

#اگر_ختم /* HEADER_CURL_SPLAY_H */
