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

কাঠামো Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

কাঠামো althost {
  অক্ষর *host;
  uint16_t port;
  গণনা alpnid alpnid;
};

কাঠামো altsvc {
  কাঠামো althost src;
  কাঠামো althost dst;
  time_t expires;
  কাঠামো Curl_llist_গ্রন্থি গ্রন্থি;
  BIT(persist);
};

কাঠামো altsvcinfo {
  অক্ষর *filename;
  কাঠামো Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

ধ্রুবক অক্ষর *Curl_alpnid2str(গণনা alpnid id);
কাঠামো altsvcinfo *Curl_altsvc_init(শূন্য);
CURLcode Curl_altsvc_load(কাঠামো altsvcinfo *asi, ধ্রুবক অক্ষর *file);
CURLcode Curl_altsvc_save(কাঠামো Curl_easy *তথ্য,
                          কাঠামো altsvcinfo *asi, ধ্রুবক অক্ষর *file);
CURLcode Curl_altsvc_ctrl(কাঠামো Curl_easy *তথ্য, ধ্রুবক long ctrl);
শূন্য Curl_altsvc_cleanup(কাঠামো altsvcinfo **asi);
CURLcode Curl_altsvc_parse(কাঠামো Curl_easy *তথ্য,
                           কাঠামো altsvcinfo *asi, ধ্রুবক অক্ষর *মান,
                           কাঠামো Curl_peer *origin,
                           গণনা alpnid origin_alpnid);
bool Curl_altsvc_lookup(কাঠামো altsvcinfo *asi,
                        কাঠামো Curl_peer *origin,
                        গণনা alpnid origin_alpnid,
                        কাঠামো altsvc **dstentry,
                        ধ্রুবক পূর্ণসংখ্যা versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#অন্যথা
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
