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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#അന്തര്ഭാവ "llist.h"

ഘടന Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#define MAX_ALTSVC_ENTRIES 5000

ഘടന althost {
  അക്ഷരം *host;
  uint16_t port;
  എണ്ണൽ alpnid alpnid;
};

ഘടന altsvc {
  ഘടന althost src;
  ഘടന althost dst;
  time_t expires;
  ഘടന Curl_llist_കണ്ണി കണ്ണി;
  BIT(persist);
};

ഘടന altsvcinfo {
  അക്ഷരം *filename;
  ഘടന Curl_llist list; /* list of entries */
  long flags; /* the publicly set bitmask */
};

സ്ഥിരം അക്ഷരം *Curl_alpnid2str(എണ്ണൽ alpnid id);
ഘടന altsvcinfo *Curl_altsvc_init(ശൂന്യം);
CURLcode Curl_altsvc_load(ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *file);
CURLcode Curl_altsvc_save(ഘടന Curl_easy *data,
                          ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *file);
CURLcode Curl_altsvc_ctrl(ഘടന Curl_easy *data, സ്ഥിരം long ctrl);
ശൂന്യം Curl_altsvc_cleanup(ഘടന altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ഘടന Curl_easy *data,
                           ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *മൂല്യം,
                           ഘടന Curl_peer *origin,
                           എണ്ണൽ alpnid origin_alpnid);
bool Curl_altsvc_lookup(ഘടന altsvcinfo *asi,
                        ഘടന Curl_peer *origin,
                        എണ്ണൽ alpnid origin_alpnid,
                        ഘടന altsvc **dstentry,
                        സ്ഥിരം പൂർണ്ണസംഖ്യ versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#അല്ലെങ്കിൽ
/* disabled */
#define Curl_altsvc_save(a, b, c)
#define Curl_altsvc_cleanup(x)
#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#endif /* HEADER_CURL_ALTSVC_H */
