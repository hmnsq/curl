#ifndef HEADER_CURL_MULTI_EV_H
#define HEADER_CURL_MULTI_EV_H
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
#ಅನ್ತರ್ಭಾವ "hash.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ Curl_multi;
ಸಂರಚನಾ easy_pollset;
ಸಂರಚನಾ uint32_bset;

/* meta key for event pollset at easy handle or connection */
#define CURL_META_MEV_POLLSET   "meta:mev:ps"

ಸಂರಚನಾ curl_multi_ev {
  ಸಂರಚನಾ Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
ಶೂನ್ಯ Curl_multi_ev_init(ಸಂರಚನಾ Curl_multi *multi, size_t hashsize);
ಶೂನ್ಯ Curl_multi_ev_cleanup(ಸಂರಚನಾ Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_assign(ಸಂರಚನಾ Curl_multi *multi, curl_socket_t s,
                               ಶೂನ್ಯ *user_data);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(ಸಂರಚನಾ Curl_multi *multi,
                                         ಸಂರಚನಾ uint32_bset *set);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
ಶೂನ್ಯ Curl_multi_ev_dirty_xfers(ಸಂರಚನಾ Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
ಶೂನ್ಯ Curl_multi_ev_socket_done(ಸಂರಚನಾ Curl_multi *multi,
                               ಸಂರಚನಾ Curl_easy *data, curl_socket_t s);

/* Transfer is removed from the multi */
ಶೂನ್ಯ Curl_multi_ev_xfer_done(ಸಂರಚನಾ Curl_multi *multi,
                             ಸಂರಚನಾ Curl_easy *data);

/* Connection is being destroyed */
ಶೂನ್ಯ Curl_multi_ev_conn_done(ಸಂರಚನಾ Curl_multi *multi,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn);

#endif /* HEADER_CURL_MULTI_EV_H */
