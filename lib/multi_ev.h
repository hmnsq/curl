#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_MULTI_EV_H
#وصف_ڪريو سرو_CURL_MULTI_EV_H
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
#ضمن "hash.h"

بنية Curl_easy;
بنية Curl_multi;
بنية easy_pollset;
بنية uint32_bset;

/* meta key for event pollset at easy handle or connection */
#وصف_ڪريو CURL_META_MEV_POLLSET   "meta:mev:ps"

بنية curl_multi_ev {
  بنية Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
فراغ Curl_multi_ev_init(بنية Curl_multi *multi, size_t hashsize);
فراغ Curl_multi_ev_cleanup(بنية Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_سونپيو(بنية Curl_multi *multi, curl_socket_t s,
                               فراغ *user_معلومات);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(بنية Curl_multi *multi,
                                         بنية uint32_bset *سيٽ);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات,
                                    بنية connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
فراغ Curl_multi_ev_dirty_xfers(بنية Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
فراغ Curl_multi_ev_socket_پورو(بنية Curl_multi *multi,
                               بنية Curl_easy *معلومات, curl_socket_t s);

/* Transfer is removed from the multi */
فراغ Curl_multi_ev_xfer_پورو(بنية Curl_multi *multi,
                             بنية Curl_easy *معلومات);

/* Connection is being destroyed */
فراغ Curl_multi_ev_conn_پورو(بنية Curl_multi *multi,
                             بنية Curl_easy *معلومات,
                             بنية connectdata *conn);

#جيڪڏهن_پورو /* HEADER_CURL_MULTI_EV_H */
