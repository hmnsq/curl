#اگر_غیر_تعریف_شدہ سرخی_CURL_FUNCTYPES_H
#تعریف_کرو سرخی_CURL_FUNCTYPES_H
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

#اگر_تعریف_شدہ USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#تعریف_کرو RECV_TYPE_ARG1 SOCKET
#تعریف_کرو RECV_TYPE_ARG2 حرف *
#تعریف_کرو RECV_TYPE_ARG3 صحیح
#تعریف_کرو RECV_TYPE_RETV صحیح

/* int send(SOCKET, const char *, int, int); */
#تعریف_کرو SEND_TYPE_ARG1 SOCKET
#تعریف_کرو SEND_TYPE_ARG2 حرف *
#تعریف_کرو SEND_TYPE_ARG3 صحیح
#تعریف_کرو SEND_TYPE_RETV صحیح

#وگرنہ_اگر defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#تعریف_کرو RECV_TYPE_ARG1 طویل
#تعریف_کرو RECV_TYPE_ARG2 حرف *
#تعریف_کرو RECV_TYPE_ARG3 طویل
#تعریف_کرو RECV_TYPE_ARG4 طویل
#تعریف_کرو RECV_TYPE_RETV طویل

/* int send(int, char *, int, int); */
#تعریف_کرو SEND_TYPE_ARG1 صحیح
#تعریف_کرو SEND_NONCONST_ARG2
#تعریف_کرو SEND_TYPE_ARG2 حرف *
#تعریف_کرو SEND_TYPE_ARG3 صحیح
#تعریف_کرو SEND_TYPE_RETV صحیح
#اگر_ختم

#اگر_غیر_تعریف_شدہ RECV_TYPE_ARG1
#تعریف_کرو RECV_TYPE_ARG1 صحیح
#اگر_ختم

#اگر_غیر_تعریف_شدہ RECV_TYPE_ARG2
#تعریف_کرو RECV_TYPE_ARG2 خالی *
#اگر_ختم

#اگر_غیر_تعریف_شدہ RECV_TYPE_ARG3
#تعریف_کرو RECV_TYPE_ARG3 size_t
#اگر_ختم

#اگر_غیر_تعریف_شدہ RECV_TYPE_ARG4
#تعریف_کرو RECV_TYPE_ARG4 صحیح
#اگر_ختم

#اگر_غیر_تعریف_شدہ RECV_TYPE_RETV
#تعریف_کرو RECV_TYPE_RETV ssize_t
#اگر_ختم

#اگر_غیر_تعریف_شدہ SEND_TYPE_ARG1
#تعریف_کرو SEND_TYPE_ARG1 صحیح
#اگر_ختم

#اگر_غیر_تعریف_شدہ SEND_TYPE_ARG2
#تعریف_کرو SEND_TYPE_ARG2 خالی *
#اگر_ختم

#اگر_غیر_تعریف_شدہ SEND_TYPE_ARG3
#تعریف_کرو SEND_TYPE_ARG3 size_t
#اگر_ختم

#اگر_غیر_تعریف_شدہ SEND_TYPE_ARG4
#تعریف_کرو SEND_TYPE_ARG4 صحیح
#اگر_ختم

#اگر_غیر_تعریف_شدہ SEND_TYPE_RETV
#تعریف_کرو SEND_TYPE_RETV ssize_t
#اگر_ختم

#اگر_ختم /* HEADER_CURL_FUNCTYPES_H */
