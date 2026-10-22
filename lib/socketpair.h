#यदि_अपरिभाषितम् शीर्षकम्_CURL_SOCKETPAIR_H
#परिभाषय शीर्षकम्_CURL_SOCKETPAIR_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_अपरिभाषितम् CURL_DISABLE_SOCKETPAIR

/* return < 0 for failure to initialize */
पूर्णाङ्क Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking);
शून्य Curl_wakeup_destroy(curl_socket_t socks[2]);

/* return 0 on success or errno on failure */
पूर्णाङ्क Curl_wakeup_signal(curl_socket_t socks[2]);

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all);

#अन्यथा
#परिभाषय Curl_wakeup_destroy(x) Curl_nop_stmt
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_SOCKETPAIR_H */
