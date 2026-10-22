#अगर_अघोषित HEADER_CURL_MULTI_EV_H
#घोषणा HEADER_CURL_MULTI_EV_H
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
#समावेश "hash.h"

काष्ठा Curl_easy;
काष्ठा Curl_multi;
काष्ठा easy_pollset;
काष्ठा uint32_bset;

/* meta key for event pollset at easy handle or connection */
#घोषणा CURL_META_MEV_POLLSET   "meta:mev:ps"

काष्ठा curl_multi_ev {
  काष्ठा Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
व्योम Curl_multi_ev_init(काष्ठा Curl_multi *multi, माप_प्रकार hashsize);
व्योम Curl_multi_ev_cleanup(काष्ठा Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_assign(काष्ठा Curl_multi *multi, curl_socket_t s,
                               व्योम *user_तथ्य);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(काष्ठा Curl_multi *multi,
                                         काष्ठा uint32_bset *set);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
व्योम Curl_multi_ev_dirty_xfers(काष्ठा Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
व्योम Curl_multi_ev_socket_done(काष्ठा Curl_multi *multi,
                               काष्ठा Curl_easy *तथ्य, curl_socket_t s);

/* Transfer is removed from the multi */
व्योम Curl_multi_ev_xfer_done(काष्ठा Curl_multi *multi,
                             काष्ठा Curl_easy *तथ्य);

/* Connection is being destroyed */
व्योम Curl_multi_ev_conn_done(काष्ठा Curl_multi *multi,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn);

#पूर्ण_अगर /* HEADER_CURL_MULTI_EV_H */
