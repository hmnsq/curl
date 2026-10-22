#अगर_अघोषित HEADER_CURL_ALTSVC_H
#घोषणा HEADER_CURL_ALTSVC_H
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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#समावेश "llist.h"

काष्ठा Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#घोषणा MAX_ALTSVC_ENTRIES 5000

काष्ठा althost {
  अक्षर *host;
  uint16_t port;
  क्रमागत alpnid alpnid;
};

काष्ठा altsvc {
  काष्ठा althost src;
  काष्ठा althost dst;
  समय_प्रकार expires;
  काष्ठा Curl_llist_गाँठ गाँठ;
  BIT(persist);
};

काष्ठा altsvcinfo {
  अक्षर *filename;
  काष्ठा Curl_llist list; /* list of entries */
  दीर्घ flags; /* the publicly set bitmask */
};

स्थिर अक्षर *Curl_alpnid2str(क्रमागत alpnid id);
काष्ठा altsvcinfo *Curl_altsvc_init(व्योम);
CURLcode Curl_altsvc_load(काष्ठा altsvcinfo *asi, स्थिर अक्षर *file);
CURLcode Curl_altsvc_save(काष्ठा Curl_easy *तथ्य,
                          काष्ठा altsvcinfo *asi, स्थिर अक्षर *file);
CURLcode Curl_altsvc_ctrl(काष्ठा Curl_easy *तथ्य, स्थिर दीर्घ ctrl);
व्योम Curl_altsvc_cleanup(काष्ठा altsvcinfo **asi);
CURLcode Curl_altsvc_parse(काष्ठा Curl_easy *तथ्य,
                           काष्ठा altsvcinfo *asi, स्थिर अक्षर *मूल्य,
                           काष्ठा Curl_peer *origin,
                           क्रमागत alpnid origin_alpnid);
bool Curl_altsvc_lookup(काष्ठा altsvcinfo *asi,
                        काष्ठा Curl_peer *origin,
                        क्रमागत alpnid origin_alpnid,
                        काष्ठा altsvc **dstentry,
                        स्थिर पूर्णांक versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#अन्यथा
/* disabled */
#घोषणा Curl_altsvc_save(a, b, c)
#घोषणा Curl_altsvc_cleanup(x)
#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#पूर्ण_अगर /* HEADER_CURL_ALTSVC_H */
