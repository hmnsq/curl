#اگر_غیر_تعریف_شدہ سرخی_CURL_ALTSVC_H
#تعریف_کرو سرخی_CURL_ALTSVC_H
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

ساخت Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#تعریف_کرو MAX_ALTSVC_ENTRIES 5000

ساخت althost {
  حرف *host;
  uint16_t پورٹ;
  شمار alpnid alpnid;
};

ساخت altsvc {
  ساخت althost src;
  ساخت althost dst;
  time_t expires;
  ساخت Curl_llist_گرہ گرہ;
  ثنائی_ہندسہ(persist);
};

ساخت altsvcinfo {
  حرف *filename;
  ساخت Curl_llist list; /* list of entries */
  طویل flags; /* the publicly set bitmask */
};

ثابت حرف *Curl_alpnid2str(شمار alpnid id);
ساخت altsvcinfo *Curl_altsvc_init(خالی);
CURLcode Curl_altsvc_بھرو(ساخت altsvcinfo *asi, ثابت حرف *file);
CURLcode Curl_altsvc_save(ساخت Curl_easy *ڈیٹا,
                          ساخت altsvcinfo *asi, ثابت حرف *file);
CURLcode Curl_altsvc_ctrl(ساخت Curl_easy *ڈیٹا, ثابت طویل ctrl);
خالی Curl_altsvc_cleanup(ساخت altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ساخت Curl_easy *ڈیٹا,
                           ساخت altsvcinfo *asi, ثابت حرف *قدر,
                           ساخت Curl_peer *origin,
                           شمار alpnid origin_alpnid);
bool Curl_altsvc_lookup(ساخت altsvcinfo *asi,
                        ساخت Curl_peer *origin,
                        شمار alpnid origin_alpnid,
                        ساخت altsvc **dstentry,
                        ثابت صحیح versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#ورنہ
/* disabled */
#تعریف_کرو Curl_altsvc_save(a, b, c)
#تعریف_کرو Curl_altsvc_cleanup(x)
#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#اگر_ختم /* HEADER_CURL_ALTSVC_H */
