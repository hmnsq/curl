#अगर_अघोषित HEADER_CURL_HSTS_H
#घोषणा HEADER_CURL_HSTS_H
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

काष्ठा hsts;

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#समावेश "llist.h"

काष्ठा Curl_peer;

#घोषणा MAX_HSTS_ENTRIES 10000

#अगर defined(DEBUGBUILD) || defined(UNITTESTS)
बाह्य समय_प्रकार deltatime;
#पूर्ण_अगर

काष्ठा stsentry {
  काष्ठा Curl_llist_गाँठ गाँठ;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  अक्षर host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
काष्ठा hsts {
  काष्ठा Curl_llist list;
  अक्षर *filename;
  अचिन्हित पूर्णांक flags;
};

काष्ठा hsts *Curl_hsts_init(व्योम);
व्योम Curl_hsts_cleanup(काष्ठा hsts **hp);
CURLcode Curl_hsts_copy(काष्ठा hsts *dst, काष्ठा hsts *src);
CURLcode Curl_hsts_parse(काष्ठा hsts *h, स्थिर अक्षर *hostname,
                         स्थिर अक्षर *header);
CURLcode Curl_hsts_save(काष्ठा Curl_easy *तथ्य, काष्ठा hsts *h,
                        स्थिर अक्षर *file);
CURLcode Curl_hsts_loadfile(काष्ठा Curl_easy *तथ्य,
                            काष्ठा hsts *h, स्थिर अक्षर *file);
CURLcode Curl_hsts_loadcb(काष्ठा Curl_easy *तथ्य,
                          काष्ठा hsts *h);
CURLcode Curl_hsts_loadfiles(काष्ठा Curl_easy *तथ्य);

bool Curl_hsts_applies(काष्ठा hsts *h, स्थिर काष्ठा Curl_peer *dest);

#अन्यथा
#घोषणा Curl_hsts_cleanup(x)
#घोषणा Curl_hsts_loadcb(x, y) CURLE_OK
#घोषणा Curl_hsts_save(x, y, z)
#घोषणा Curl_hsts_loadfiles(x) CURLE_OK
#पूर्ण_अगर /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#पूर्ण_अगर /* HEADER_CURL_HSTS_H */
