#اگر_غیر_تعریف_شدہ سرخی_CURL_MULTI_EV_H
#تعریف_کرو سرخی_CURL_MULTI_EV_H
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
#شامل "hash.h"

ساخت Curl_easy;
ساخت Curl_multi;
ساخت easy_pollset;
ساخت uint32_bset;

/* meta key for event pollset at easy handle or connection */
#تعریف_کرو CURL_META_MEV_POLLSET   "meta:mev:ps"

ساخت curl_multi_ev {
  ساخت Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
خالی Curl_multi_ev_init(ساخت Curl_multi *multi, size_t hashsize);
خالی Curl_multi_ev_cleanup(ساخت Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_تفویض(ساخت Curl_multi *multi, curl_socket_t s,
                               خالی *user_ڈیٹا);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(ساخت Curl_multi *multi,
                                         ساخت uint32_bset *سیٹ);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
خالی Curl_multi_ev_dirty_xfers(ساخت Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
خالی Curl_multi_ev_socket_مکمل(ساخت Curl_multi *multi,
                               ساخت Curl_easy *ڈیٹا, curl_socket_t s);

/* Transfer is removed from the multi */
خالی Curl_multi_ev_xfer_مکمل(ساخت Curl_multi *multi,
                             ساخت Curl_easy *ڈیٹا);

/* Connection is being destroyed */
خالی Curl_multi_ev_conn_مکمل(ساخت Curl_multi *multi,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn);

#اگر_ختم /* HEADER_CURL_MULTI_EV_H */
