#నిర్వచితం_కాకపోతే శీర్షిక_CURL_URLAPI_INT_H
#నిర్వచించు శీర్షిక_CURL_URLAPI_INT_H
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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ <curl/urlapi.h>

/* Internal representation of CURLU. Point to URL-encoded strings. */
నిర్మాణం Curl_URL {
  అక్షరం *scheme;
  అక్షరం *user;
  అక్షరం *password;
  అక్షరం *options; /* IMAP only? */
  అక్షరం *host;
  అక్షరం *zoneid; /* for numerical IPv6 addresses */
  అక్షరం *మార్గం;
  అక్షరం *query;
  అక్షరం *fragment;
  uint16_t portnum; /* the numerical port if present */
  బిట్టు(పోర్టు_present);    /* to support missing port */
  బిట్టు(query_present);    /* to support blank */
  బిట్టు(fragment_present); /* to support blank */
  బిట్టు(guessed_scheme);   /* when a URL without scheme is parsed */
};

#నిర్వచించు HOST_ERROR   (-1) /* out of memory */
#నిర్వచించు HOST_NAME    1
#నిర్వచించు HOST_IPV4    2
#నిర్వచించు HOST_IPV6    3

#నిర్వచించు QUERY_NO      2
#నిర్వచించు QUERY_NOT_YET 3 /* allow to change to query */
#నిర్వచించు QUERY_YES     4

size_t Curl_is_absolute_url(స్థిరరాశి అక్షరం *url, అక్షరం *buf, size_t buflen,
                            bool guess_scheme);

CURLUcode Curl_url_సమితి_authority(CURLU *u, స్థిరరాశి అక్షరం *authority);

CURLUcode Curl_junkscan(స్థిరరాశి అక్షరం *url, size_t *urllen, bool allowspace);

#నిర్వచించు U_CURLU_URLDECODE  (గుర్తులేని పూర్ణాంకం)CURLU_URLDECODE
#నిర్వచించు U_CURLU_మార్గం_AS_IS (గుర్తులేని పూర్ణాంకం)CURLU_మార్గం_AS_IS

bool Curl_url_same_origin(CURLU *పునాది, CURLU *href);

CURLUcode Curl_url_get_పోర్టు(CURLU *u, uint16_t *pport);

#అయితే_ముగింపు /* HEADER_CURL_URLAPI_INT_H */
