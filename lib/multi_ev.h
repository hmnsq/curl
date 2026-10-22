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
#अन्तर्भाव "hash.h"

संरचना Curl_easy;
संरचना Curl_multi;
संरचना easy_pollset;
संरचना uint32_bset;

/* meta key for event pollset at easy handle or connection */
#define CURL_META_MEV_POLLSET   "meta:mev:ps"

संरचना curl_multi_ev {
  संरचना Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
शून्य Curl_multi_ev_init(संरचना Curl_multi *multi, माप_प्रकार hashsize);
शून्य Curl_multi_ev_cleanup(संरचना Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_assign(संरचना Curl_multi *multi, curl_socket_t s,
                               शून्य *user_data);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *data);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(संरचना Curl_multi *multi,
                                         संरचना uint32_bset *set);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *data,
                                    संरचना connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
शून्य Curl_multi_ev_dirty_xfers(संरचना Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
शून्य Curl_multi_ev_socket_done(संरचना Curl_multi *multi,
                               संरचना Curl_easy *data, curl_socket_t s);

/* Transfer is removed from the multi */
शून्य Curl_multi_ev_xfer_done(संरचना Curl_multi *multi,
                             संरचना Curl_easy *data);

/* Connection is being destroyed */
शून्य Curl_multi_ev_conn_done(संरचना Curl_multi *multi,
                             संरचना Curl_easy *data,
                             संरचना connectdata *conn);

#endif /* HEADER_CURL_MULTI_EV_H */
