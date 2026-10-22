#ifndef HEADER_CURL_ALTSVC_H
#define HEADER_CURL_ALTSVC_H
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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ಅನ್ತರ್ಭಾವ "llist.h"

ಸಂರಚನಾ Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

ಸಂರಚನಾ althost {
  ವರ್ಣ *host;
  uint16_t port;
  ಗಣನಾ alpnid alpnid;
};

ಸಂರಚನಾ altsvc {
  ಸಂರಚನಾ althost src;
  ಸಂರಚನಾ althost dst;
  time_t expires;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು ಗಂಟು;
  BIT(persist);
};

ಸಂರಚನಾ altsvcinfo {
  ವರ್ಣ *filename;
  ಸಂರಚನಾ Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

ಧ್ರುವ ವರ್ಣ *Curl_alpnid2str(ಗಣನಾ alpnid id);
ಸಂರಚನಾ altsvcinfo *Curl_altsvc_init(ಶೂನ್ಯ);
CURLcode Curl_altsvc_load(ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *file);
CURLcode Curl_altsvc_save(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *file);
CURLcode Curl_altsvc_ctrl(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ long ctrl);
ಶೂನ್ಯ Curl_altsvc_cleanup(ಸಂರಚನಾ altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ,
                           ಸಂರಚನಾ Curl_peer *origin,
                           ಗಣನಾ alpnid origin_alpnid);
bool Curl_altsvc_lookup(ಸಂರಚನಾ altsvcinfo *asi,
                        ಸಂರಚನಾ Curl_peer *origin,
                        ಗಣನಾ alpnid origin_alpnid,
                        ಸಂರಚನಾ altsvc **dstentry,
                        ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#ಅನ್ಯಥಾ
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
