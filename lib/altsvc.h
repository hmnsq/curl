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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#অন্তর্ভাব "llist.h"

সংরচনা Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

সংরচনা althost {
  বর্ণ *host;
  uint16_t port;
  গণনা alpnid alpnid;
};

সংরচনা altsvc {
  সংরচনা althost src;
  সংরচনা althost dst;
  time_t expires;
  সংরচনা Curl_llist_গ্রন্থি গ্রন্থি;
  BIT(persist);
};

সংরচনা altsvcinfo {
  বর্ণ *filename;
  সংরচনা Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

ধ্রুব বর্ণ *Curl_alpnid2str(গণনা alpnid id);
সংরচনা altsvcinfo *Curl_altsvc_init(শূন্য);
CURLcode Curl_altsvc_load(সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *file);
CURLcode Curl_altsvc_save(সংরচনা Curl_easy *data,
                          সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *file);
CURLcode Curl_altsvc_ctrl(সংরচনা Curl_easy *data, ধ্রুব long ctrl);
শূন্য Curl_altsvc_cleanup(সংরচনা altsvcinfo **asi);
CURLcode Curl_altsvc_parse(সংরচনা Curl_easy *data,
                           সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *মান,
                           সংরচনা Curl_peer *origin,
                           গণনা alpnid origin_alpnid);
bool Curl_altsvc_lookup(সংরচনা altsvcinfo *asi,
                        সংরচনা Curl_peer *origin,
                        গণনা alpnid origin_alpnid,
                        সংরচনা altsvc **dstentry,
                        ধ্রুব পূর্ণাঙ্ক versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#অন্যথা
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
