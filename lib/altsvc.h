#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ALTSVC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ALTSVC_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"

ਬਣਤਰ Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ALTSVC_ENTRIES 5000

ਬਣਤਰ althost {
  ਅੱਖਰ *host;
  uint16_t ਪੋਰਟ;
  ਗਿਣਤੀ alpnid alpnid;
};

ਬਣਤਰ altsvc {
  ਬਣਤਰ althost src;
  ਬਣਤਰ althost dst;
  time_t expires;
  ਬਣਤਰ Curl_llist_ਗੰਢ ਗੰਢ;
  ਦੋਆਧਾਰੀ_ਅੰਕ(persist);
};

ਬਣਤਰ altsvcinfo {
  ਅੱਖਰ *filename;
  ਬਣਤਰ Curl_llist list; /* list of entries */
  ਲੰਮਾ flags; /* the publicly set bitmask */
};

ਅਚਲ ਅੱਖਰ *Curl_alpnid2str(ਗਿਣਤੀ alpnid id);
ਬਣਤਰ altsvcinfo *Curl_altsvc_init(ਖਾਲੀ);
CURLcode Curl_altsvc_ਲੱਦੋ(ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *file);
CURLcode Curl_altsvc_save(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *file);
CURLcode Curl_altsvc_ctrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਲੰਮਾ ctrl);
ਖਾਲੀ Curl_altsvc_cleanup(ਬਣਤਰ altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                           ਬਣਤਰ Curl_peer *origin,
                           ਗਿਣਤੀ alpnid origin_alpnid);
bool Curl_altsvc_lookup(ਬਣਤਰ altsvcinfo *asi,
                        ਬਣਤਰ Curl_peer *origin,
                        ਗਿਣਤੀ alpnid origin_alpnid,
                        ਬਣਤਰ altsvc **dstentry,
                        ਅਚਲ ਪੂਰਨ versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#ਨਹੀਂਤਾਂ
/* disabled */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_altsvc_save(a, b, c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_altsvc_cleanup(x)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#ਜੇ_ਅੰਤ /* HEADER_CURL_ALTSVC_H */
