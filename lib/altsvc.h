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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#அந்தர்பாவ "llist.h"

கட்டமைப்பு Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

கட்டமைப்பு althost {
  எழுத்து *host;
  uint16_t port;
  எண்ணிக்கை alpnid alpnid;
};

கட்டமைப்பு altsvc {
  கட்டமைப்பு althost src;
  கட்டமைப்பு althost dst;
  time_t expires;
  கட்டமைப்பு Curl_llist_முனை முனை;
  BIT(persist);
};

கட்டமைப்பு altsvcinfo {
  எழுத்து *filename;
  கட்டமைப்பு Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

மாறிலி எழுத்து *Curl_alpnid2str(எண்ணிக்கை alpnid id);
கட்டமைப்பு altsvcinfo *Curl_altsvc_init(வெறுமை);
CURLcode Curl_altsvc_load(கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *file);
CURLcode Curl_altsvc_save(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *file);
CURLcode Curl_altsvc_ctrl(கட்டமைப்பு Curl_easy *data, மாறிலி long ctrl);
வெறுமை Curl_altsvc_cleanup(கட்டமைப்பு altsvcinfo **asi);
CURLcode Curl_altsvc_parse(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *மதிப்பு,
                           கட்டமைப்பு Curl_peer *origin,
                           எண்ணிக்கை alpnid origin_alpnid);
bool Curl_altsvc_lookup(கட்டமைப்பு altsvcinfo *asi,
                        கட்டமைப்பு Curl_peer *origin,
                        எண்ணிக்கை alpnid origin_alpnid,
                        கட்டமைப்பு altsvc **dstentry,
                        மாறிலி முழு versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#இல்லையெனில்
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
