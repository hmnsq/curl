#جے_بنا_تعریف سرخی_CURL_SOCKETPAIR_H
#تعریف_کرو سرخی_CURL_SOCKETPAIR_H
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
#ضمن "curl_setup.h"

#جے_بنا_تعریف CURL_DISABLE_SOCKETPAIR

/* return < 0 for failure to initialize */
صحيح Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking);
فراغ Curl_wakeup_destroy(curl_socket_t socks[2]);

/* return 0 on success or errno on failure */
صحيح Curl_wakeup_signal(curl_socket_t socks[2]);

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all);

#وإلا
#تعریف_کرو Curl_wakeup_destroy(x) Curl_nop_stmt
#جے_انت

#جے_انت /* HEADER_CURL_SOCKETPAIR_H */
