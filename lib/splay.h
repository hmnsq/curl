#ifndef HEADER_CURL_SPLAY_H
#define HEADER_CURL_SPLAY_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

ସଂରଚନା Curl_easy;

/* only use function calls to access this struct */
ସଂରଚନା Curl_ବୃକ୍ଷ {
  ସଂରଚନା Curl_ବୃକ୍ଷ *smaller; /* smaller node */
  ସଂରଚନା Curl_ବୃକ୍ଷ *larger;  /* larger node */
  ସଂରଚନା Curl_ବୃକ୍ଷ *same;    /* points to the next node with identical key */
  timediff_t ଚାବି;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

ସଂରଚନା Curl_timeouts {
  ସଂରଚନା Curl_ବୃକ୍ଷ *ବୃକ୍ଷ;
  time_t time_base_sec;
};

ଶୂନ୍ୟ Curl_timeouts_init(ସଂରଚନା Curl_timeouts *timeouts,
                        ଅଚଳ ସଂରଚନା curltime *ptime_base);

bool Curl_timeouts_has(ସଂରଚନା Curl_easy *data);

timediff_t Curl_timeouts_offset_us(ସଂରଚନା Curl_timeouts *timeouts,
                                   ଅଚଳ ସଂରଚନା curltime *pts);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_timeouts_next_ms(ସଂରଚନା Curl_timeouts *timeouts,
                          ଅଚଳ ସଂରଚନା curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ସଂରଚନା Curl_timeouts *timeouts,
                                  ଅଚଳ ସଂରଚନା curltime *ts,
                                  uint32_t *pmid);

ଶୂନ୍ୟ Curl_timeouts_add(ସଂରଚନା Curl_timeouts *timeouts,
                       ସଂରଚନା Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ସଂରଚନା Curl_timeouts *timeouts,
                          ସଂରଚନା Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(ସଂରଚନା Curl_ବୃକ୍ଷ *ଗଣ୍ଠି);
#endif

#endif /* HEADER_CURL_SPLAY_H */
