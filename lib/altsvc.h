#که_نه_تعریف_شوی سرلیک_CURL_ALTSVC_H
#تعریف_کړه سرلیک_CURL_ALTSVC_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ضمن "llist.h"

بنية Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#تعریف_کړه MAX_ALTSVC_ENTRIES 5000

بنية althost {
  حرف *host;
  uint16_t پورټ;
  تعداد alpnid alpnid;
};

بنية altsvc {
  بنية althost src;
  بنية althost dst;
  time_t expires;
  بنية Curl_llist_غوټه غوټه;
  بټ(persist);
};

بنية altsvcinfo {
  حرف *filename;
  بنية Curl_llist list; /* list of entries */
  اوږد flags; /* the publicly set bitmask */
};

ثابت حرف *Curl_alpnid2str(تعداد alpnid id);
بنية altsvcinfo *Curl_altsvc_init(فراغ);
CURLcode Curl_altsvc_بارول(بنية altsvcinfo *asi, ثابت حرف *file);
CURLcode Curl_altsvc_save(بنية Curl_easy *معلومات,
                          بنية altsvcinfo *asi, ثابت حرف *file);
CURLcode Curl_altsvc_ctrl(بنية Curl_easy *معلومات, ثابت اوږد ctrl);
فراغ Curl_altsvc_cleanup(بنية altsvcinfo **asi);
CURLcode Curl_altsvc_parse(بنية Curl_easy *معلومات,
                           بنية altsvcinfo *asi, ثابت حرف *ارزښت,
                           بنية Curl_peer *origin,
                           تعداد alpnid origin_alpnid);
bool Curl_altsvc_lookup(بنية altsvcinfo *asi,
                        بنية Curl_peer *origin,
                        تعداد alpnid origin_alpnid,
                        بنية altsvc **dstentry,
                        ثابت صحيح versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#وإلا
/* disabled */
#تعریف_کړه Curl_altsvc_save(a, b, c)
#تعریف_کړه Curl_altsvc_cleanup(x)
#د_که_پای /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#د_که_پای /* HEADER_CURL_ALTSVC_H */
