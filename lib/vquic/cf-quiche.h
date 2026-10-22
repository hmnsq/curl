#ifndef HEADER_CURL_VQUIC_CF_QUICHE_H
#define HEADER_CURL_VQUIC_CF_QUICHE_H
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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#अन्तर्भाव <quiche.h>
#अन्तर्भाव <openssl/ssl.h>

संरचना Curl_cfilter;
संरचना Curl_easy;

शून्य Curl_quiche_ver(वर्ण *p, माप_प्रकार len);

CURLcode Curl_cf_quiche_create(संरचना Curl_cfilter **pcf,
                               संरचना Curl_easy *data,
                               संरचना Curl_peer *origin,
                               संरचना Curl_peer *peer,
                               संरचना connectdata *conn,
                               संरचना Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_जोड़ो_after(संरचना Curl_cfilter *cf_at,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     संरचना Curl_cfilter **pcf);

#endif

#endif /* HEADER_CURL_VQUIC_CF_QUICHE_H */
