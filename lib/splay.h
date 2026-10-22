#اگر_تعریف_نشده سرآیند_CURL_SPLAY_H
#تعریف_کن سرآیند_CURL_SPLAY_H
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
بنية Curl_درخت {
  بنية Curl_درخت *smaller; /* smaller node */
  بنية Curl_درخت *larger;  /* larger node */
  بنية Curl_درخت *same;    /* points to the next node with identical key */
  timediff_t کلید;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  بیت(registered);           /* node is registered in splay tree */
};

بنية Curl_timeouts {
  بنية Curl_درخت *درخت;
  time_t زمان_پایه_sec;
};

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_پایه);

bool Curl_timeouts_has(بنية Curl_easy *داده);

timediff_t Curl_timeouts_جابه_جایی_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts);

صحيح Curl_timeouts_بعدی_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_جابه_جایی_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid);

فراغ Curl_timeouts_جمع_کن(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *داده,
                       timediff_t جابه_جایی_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *داده);

#اگر_تعریف_شده UNITTESTS
uint32_t Curl_splayget(بنية Curl_درخت *گره);
#پایان_اگر

#پایان_اگر /* HEADER_CURL_SPLAY_H */
