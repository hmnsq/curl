#ifndef HEADER_CURL_SOCKETPAIR_H
#define HEADER_CURL_SOCKETPAIR_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* return < 0 for failure to initialize */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking);
ଶୂନ୍ୟ Curl_wakeup_destroy(curl_socket_t socks[2]);

/* return 0 on success or errno on failure */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_wakeup_signal(curl_socket_t socks[2]);

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all);

#ଅନ୍ୟଥା
#define Curl_wakeup_destroy(x) Curl_nop_stmt
#endif

#endif /* HEADER_CURL_SOCKETPAIR_H */
