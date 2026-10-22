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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curlx/timeval.h"

संरचना Curl_easy;

/* only use function calls to access this struct */
संरचना Curl_वृक्ष {
  संरचना Curl_वृक्ष *smaller; /* smaller node */
  संरचना Curl_वृक्ष *larger;  /* larger node */
  संरचना Curl_वृक्ष *same;    /* points to the next node with identical key */
  timediff_t कुंजी;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

संरचना Curl_timeouts {
  संरचना Curl_वृक्ष *वृक्ष;
  time_t time_base_sec;
};

शून्य Curl_timeouts_init(संरचना Curl_timeouts *timeouts,
                        ध्रुव संरचना curltime *ptime_base);

bool Curl_timeouts_has(संरचना Curl_easy *data);

timediff_t Curl_timeouts_offset_us(संरचना Curl_timeouts *timeouts,
                                   ध्रुव संरचना curltime *pts);

पूर्णाङ्क Curl_timeouts_next_ms(संरचना Curl_timeouts *timeouts,
                          ध्रुव संरचना curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(संरचना Curl_timeouts *timeouts,
                                  ध्रुव संरचना curltime *ts,
                                  uint32_t *pmid);

शून्य Curl_timeouts_add(संरचना Curl_timeouts *timeouts,
                       संरचना Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(संरचना Curl_timeouts *timeouts,
                          संरचना Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(संरचना Curl_वृक्ष *गाँठ);
#endif

#endif /* HEADER_CURL_SPLAY_H */
