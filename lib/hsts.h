#ifndef HEADER_CURL_HSTS_H
#define HEADER_CURL_HSTS_H
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
#अन्तर्भाव "curl_setup.h"

संरचना hsts;

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#अन्तर्भाव "llist.h"

संरचना Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#यदि defined(DEBUGBUILD) || defined(UNITTESTS)
बाह्य समय_प्रकार deltatime;
#endif

संरचना stsentry {
  संरचना Curl_llist_गाँठ गाँठ;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  वर्ण host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
संरचना hsts {
  संरचना Curl_llist list;
  वर्ण *filename;
  अचिन्हित पूर्णाङ्क flags;
};

संरचना hsts *Curl_hsts_init(शून्य);
शून्य Curl_hsts_cleanup(संरचना hsts **hp);
CURLcode Curl_hsts_copy(संरचना hsts *dst, संरचना hsts *src);
CURLcode Curl_hsts_parse(संरचना hsts *h, ध्रुव वर्ण *hostname,
                         ध्रुव वर्ण *header);
CURLcode Curl_hsts_save(संरचना Curl_easy *data, संरचना hsts *h,
                        ध्रुव वर्ण *file);
CURLcode Curl_hsts_loadfile(संरचना Curl_easy *data,
                            संरचना hsts *h, ध्रुव वर्ण *file);
CURLcode Curl_hsts_loadcb(संरचना Curl_easy *data,
                          संरचना hsts *h);
CURLcode Curl_hsts_loadfiles(संरचना Curl_easy *data);

bool Curl_hsts_applies(संरचना hsts *h, ध्रुव संरचना Curl_peer *dest);

#अन्यथा
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
