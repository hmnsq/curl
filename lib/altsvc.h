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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ଅନ୍ତର୍ଭାଵ "llist.h"

ସଂରଚନା Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

ସଂରଚନା althost {
  ଅକ୍ଷର *host;
  uint16_t port;
  ଗଣନା alpnid alpnid;
};

ସଂରଚନା altsvc {
  ସଂରଚନା althost src;
  ସଂରଚନା althost dst;
  time_t expires;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି ଗଣ୍ଠି;
  BIT(persist);
};

ସଂରଚନା altsvcinfo {
  ଅକ୍ଷର *filename;
  ସଂରଚନା Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

ଅଚଳ ଅକ୍ଷର *Curl_alpnid2str(ଗଣନା alpnid id);
ସଂରଚନା altsvcinfo *Curl_altsvc_init(ଶୂନ୍ୟ);
CURLcode Curl_altsvc_load(ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *file);
CURLcode Curl_altsvc_save(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *file);
CURLcode Curl_altsvc_ctrl(ସଂରଚନା Curl_easy *data, ଅଚଳ long ctrl);
ଶୂନ୍ୟ Curl_altsvc_cleanup(ସଂରଚନା altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ,
                           ସଂରଚନା Curl_peer *origin,
                           ଗଣନା alpnid origin_alpnid);
bool Curl_altsvc_lookup(ସଂରଚନା altsvcinfo *asi,
                        ସଂରଚନା Curl_peer *origin,
                        ଗଣନା alpnid origin_alpnid,
                        ସଂରଚନା altsvc **dstentry,
                        ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#ଅନ୍ୟଥା
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
