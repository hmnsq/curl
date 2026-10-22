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
#அந்தர்பாவ "curl_setup.h"

கட்டமைப்பு hsts;

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#அந்தர்பாவ "llist.h"

கட்டமைப்பு Curl_peer;

#define MAX_HSTS_ENTRIES 10000

#எனில் defined(DEBUGBUILD) || defined(UNITTESTS)
வெளி time_t deltatime;
#endif

கட்டமைப்பு stsentry {
  கட்டமைப்பு Curl_llist_முனை முனை;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  BIT(includeSubDomains);
  எழுத்து host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
கட்டமைப்பு hsts {
  கட்டமைப்பு Curl_llist list;
  எழுத்து *filename;
  unsigned முழு flags;
};

கட்டமைப்பு hsts *Curl_hsts_init(வெறுமை);
வெறுமை Curl_hsts_cleanup(கட்டமைப்பு hsts **hp);
CURLcode Curl_hsts_copy(கட்டமைப்பு hsts *dst, கட்டமைப்பு hsts *src);
CURLcode Curl_hsts_parse(கட்டமைப்பு hsts *h, மாறிலி எழுத்து *hostname,
                         மாறிலி எழுத்து *header);
CURLcode Curl_hsts_save(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு hsts *h,
                        மாறிலி எழுத்து *file);
CURLcode Curl_hsts_loadfile(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு hsts *h, மாறிலி எழுத்து *file);
CURLcode Curl_hsts_loadcb(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு hsts *h);
CURLcode Curl_hsts_loadfiles(கட்டமைப்பு Curl_easy *data);

bool Curl_hsts_applies(கட்டமைப்பு hsts *h, மாறிலி கட்டமைப்பு Curl_peer *dest);

#இல்லையெனில்
#define Curl_hsts_cleanup(x)
#define Curl_hsts_loadcb(x, y) CURLE_OK
#define Curl_hsts_save(x, y, z)
#define Curl_hsts_loadfiles(x) CURLE_OK
#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#endif /* HEADER_CURL_HSTS_H */
