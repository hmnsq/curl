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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#અન્તર્ભાવ "llist.h"

સંરચના Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

સંરચના althost {
  અક્ષર *host;
  uint16_t port;
  ગણના alpnid alpnid;
};

સંરચના altsvc {
  સંરચના althost src;
  સંરચના althost dst;
  time_t expires;
  સંરચના Curl_llist_ગાંઠ ગાંઠ;
  BIT(persist);
};

સંરચના altsvcinfo {
  અક્ષર *filename;
  સંરચના Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

અચલ અક્ષર *Curl_alpnid2str(ગણના alpnid id);
સંરચના altsvcinfo *Curl_altsvc_init(શૂન્ય);
CURLcode Curl_altsvc_load(સંરચના altsvcinfo *asi, અચલ અક્ષર *file);
CURLcode Curl_altsvc_save(સંરચના Curl_easy *data,
                          સંરચના altsvcinfo *asi, અચલ અક્ષર *file);
CURLcode Curl_altsvc_ctrl(સંરચના Curl_easy *data, અચલ long ctrl);
શૂન્ય Curl_altsvc_cleanup(સંરચના altsvcinfo **asi);
CURLcode Curl_altsvc_parse(સંરચના Curl_easy *data,
                           સંરચના altsvcinfo *asi, અચલ અક્ષર *કિંમત,
                           સંરચના Curl_peer *origin,
                           ગણના alpnid origin_alpnid);
bool Curl_altsvc_lookup(સંરચના altsvcinfo *asi,
                        સંરચના Curl_peer *origin,
                        ગણના alpnid origin_alpnid,
                        સંરચના altsvc **dstentry,
                        અચલ પૂર્ણાંક versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#નહીંતર
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
