#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_FUNCTYPES_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_FUNCTYPES_H
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
/* defaults:

   ssize_t recv(int, void *, size_t, int);
   ssize_t send(int, const void *, size_t, int);

   If other argument or return types are needed:

   1. For systems that run configure or cmake, the alternatives are provided
      here.
   2. For systems with config-*.h files, define them there.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG1 SOCKET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG2 ਅੱਖਰ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG3 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_RETV ਪੂਰਨ

/* int send(SOCKET, const char *, int, int); */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG1 SOCKET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG2 ਅੱਖਰ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG3 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_RETV ਪੂਰਨ

#ਜਾਂਜੇ defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG1 ਲੰਮਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG2 ਅੱਖਰ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG3 ਲੰਮਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG4 ਲੰਮਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_RETV ਲੰਮਾ

/* int send(int, char *, int, int); */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG1 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_NONCONST_ARG2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG2 ਅੱਖਰ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG3 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_RETV ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RECV_TYPE_ARG1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG1 ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RECV_TYPE_ARG2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG2 ਖਾਲੀ *
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RECV_TYPE_ARG3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG3 size_t
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RECV_TYPE_ARG4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG4 ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RECV_TYPE_RETV
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_RETV ssize_t
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEND_TYPE_ARG1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG1 ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEND_TYPE_ARG2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG2 ਖਾਲੀ *
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEND_TYPE_ARG3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG3 size_t
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEND_TYPE_ARG4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG4 ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEND_TYPE_RETV
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_RETV ssize_t
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_FUNCTYPES_H */
