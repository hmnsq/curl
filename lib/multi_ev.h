#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_MULTI_EV_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_MULTI_EV_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_multi;
ਬਣਤਰ easy_pollset;
ਬਣਤਰ uint32_bset;

/* meta key for event pollset at easy handle or connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_MEV_POLLSET   "meta:mev:ps"

ਬਣਤਰ curl_multi_ev {
  ਬਣਤਰ Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
ਖਾਲੀ Curl_multi_ev_init(ਬਣਤਰ Curl_multi *multi, size_t hashsize);
ਖਾਲੀ Curl_multi_ev_cleanup(ਬਣਤਰ Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_ਸੌਂਪੋ(ਬਣਤਰ Curl_multi *multi, curl_socket_t s,
                               ਖਾਲੀ *user_ਅੰਕੜੇ);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(ਬਣਤਰ Curl_multi *multi,
                                         ਬਣਤਰ uint32_bset *ਸੈੱਟ);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
ਖਾਲੀ Curl_multi_ev_dirty_xfers(ਬਣਤਰ Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
ਖਾਲੀ Curl_multi_ev_socket_ਪੂਰਾ(ਬਣਤਰ Curl_multi *multi,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_socket_t s);

/* Transfer is removed from the multi */
ਖਾਲੀ Curl_multi_ev_xfer_ਪੂਰਾ(ਬਣਤਰ Curl_multi *multi,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Connection is being destroyed */
ਖਾਲੀ Curl_multi_ev_conn_ਪੂਰਾ(ਬਣਤਰ Curl_multi *multi,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn);

#ਜੇ_ਅੰਤ /* HEADER_CURL_MULTI_EV_H */
