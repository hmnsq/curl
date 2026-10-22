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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#شامل "llist.h"

ساختار Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

ساختار althost {
  نویسه *host;
  uint16_t port;
  شمارش alpnid alpnid;
};

ساختار altsvc {
  ساختار althost src;
  ساختار althost dst;
  time_t expires;
  ساختار Curl_llist_گره گره;
  BIT(persist);
};

ساختار altsvcinfo {
  نویسه *filename;
  ساختار Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

ثابت نویسه *Curl_alpnid2str(شمارش alpnid id);
ساختار altsvcinfo *Curl_altsvc_init(تهی);
CURLcode Curl_altsvc_load(ساختار altsvcinfo *asi, ثابت نویسه *file);
CURLcode Curl_altsvc_save(ساختار Curl_easy *data,
                          ساختار altsvcinfo *asi, ثابت نویسه *file);
CURLcode Curl_altsvc_ctrl(ساختار Curl_easy *data, ثابت long ctrl);
تهی Curl_altsvc_cleanup(ساختار altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ساختار Curl_easy *data,
                           ساختار altsvcinfo *asi, ثابت نویسه *مقدار,
                           ساختار Curl_peer *origin,
                           شمارش alpnid origin_alpnid);
bool Curl_altsvc_lookup(ساختار altsvcinfo *asi,
                        ساختار Curl_peer *origin,
                        شمارش alpnid origin_alpnid,
                        ساختار altsvc **dstentry,
                        ثابت صحیح versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#وگرنه
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
