#अगर_अघोषित HEADER_CURL_SPLAY_H
#घोषणा HEADER_CURL_SPLAY_H
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

#समावेश "curlx/timeval.h"

काष्ठा Curl_easy;

/* only use function calls to access this struct */
काष्ठा Curl_वृक्ष {
  काष्ठा Curl_वृक्ष *smaller; /* smaller node */
  काष्ठा Curl_वृक्ष *larger;  /* larger node */
  काष्ठा Curl_वृक्ष *same;    /* points to the next node with identical key */
  timediff_t कुंजी;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

काष्ठा Curl_timeouts {
  काष्ठा Curl_वृक्ष *वृक्ष;
  समय_प्रकार time_base_sec;
};

व्योम Curl_timeouts_init(काष्ठा Curl_timeouts *timeouts,
                        स्थिर काष्ठा curltime *ptime_base);

bool Curl_timeouts_has(काष्ठा Curl_easy *तथ्य);

timediff_t Curl_timeouts_offset_us(काष्ठा Curl_timeouts *timeouts,
                                   स्थिर काष्ठा curltime *pts);

पूर्णांक Curl_timeouts_अग्र_ms(काष्ठा Curl_timeouts *timeouts,
                          स्थिर काष्ठा curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(काष्ठा Curl_timeouts *timeouts,
                                  स्थिर काष्ठा curltime *ts,
                                  uint32_t *pmid);

व्योम Curl_timeouts_add(काष्ठा Curl_timeouts *timeouts,
                       काष्ठा Curl_easy *तथ्य,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(काष्ठा Curl_timeouts *timeouts,
                          काष्ठा Curl_easy *तथ्य);

#अगर_घोषित UNITTESTS
uint32_t Curl_splayget(काष्ठा Curl_वृक्ष *गाँठ);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_SPLAY_H */
