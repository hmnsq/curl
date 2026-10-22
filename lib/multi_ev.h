#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MULTI_EV_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MULTI_EV_H
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

ರಚನೆ Curl_easy;
ರಚನೆ Curl_multi;
ರಚನೆ easy_pollset;
ರಚನೆ uint32_bset;

/* meta key for event pollset at easy handle or connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_MEV_POLLSET   "meta:mev:ps"

ರಚನೆ curl_multi_ev {
  ರಚನೆ Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
ಶೂನ್ಯ Curl_multi_ev_init(ರಚನೆ Curl_multi *multi, size_t hashsize);
ಶೂನ್ಯ Curl_multi_ev_cleanup(ರಚನೆ Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_ನಿಯೋಜಿಸು(ರಚನೆ Curl_multi *multi, curl_socket_t s,
                               ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(ರಚನೆ Curl_multi *multi,
                                         ರಚನೆ uint32_bset *ಗಣ);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
ಶೂನ್ಯ Curl_multi_ev_dirty_xfers(ರಚನೆ Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
ಶೂನ್ಯ Curl_multi_ev_socket_ಮುಗಿದ(ರಚನೆ Curl_multi *multi,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t s);

/* Transfer is removed from the multi */
ಶೂನ್ಯ Curl_multi_ev_xfer_ಮುಗಿದ(ರಚನೆ Curl_multi *multi,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Connection is being destroyed */
ಶೂನ್ಯ Curl_multi_ev_conn_ಮುಗಿದ(ರಚನೆ Curl_multi *multi,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MULTI_EV_H */
