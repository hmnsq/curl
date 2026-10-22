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
#include "curl_setup.h"

#အကယ်၍ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#include "llist.h"

sa_nracanaa Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

sa_nracanaa althost {
  var_na *host;
  uint16_t port;
  ga_nanaa alpnid alpnid;
};

sa_nracanaa altsvc {
  sa_nracanaa althost src;
  sa_nracanaa althost dst;
  time_t expires;
  sa_nracanaa Curl_llist_node node;
  BIT(persist);
};

sa_nracanaa altsvcinfo {
  var_na *filename;
  sa_nracanaa Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

const var_na *Curl_alpnid2str(ga_nanaa alpnid id);
sa_nracanaa altsvcinfo *Curl_altsvc_init(void);
CURLcode Curl_altsvc_load(sa_nracanaa altsvcinfo *asi, const var_na *file);
CURLcode Curl_altsvc_save(sa_nracanaa Curl_easy *data,
                          sa_nracanaa altsvcinfo *asi, const var_na *file);
CURLcode Curl_altsvc_ctrl(sa_nracanaa Curl_easy *data, const long ctrl);
void Curl_altsvc_cleanup(sa_nracanaa altsvcinfo **asi);
CURLcode Curl_altsvc_parse(sa_nracanaa Curl_easy *data,
                           sa_nracanaa altsvcinfo *asi, const var_na *value,
                           sa_nracanaa Curl_peer *origin,
                           ga_nanaa alpnid origin_alpnid);
bool Curl_altsvc_lookup(sa_nracanaa altsvcinfo *asi,
                        sa_nracanaa Curl_peer *origin,
                        ga_nanaa alpnid origin_alpnid,
                        sa_nracanaa altsvc **dstentry,
                        const ကိန်းပြည့် versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#မဟုတ်ပါက
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
