#اگر_تعریف_نشده سرآیند_CURL_FUNCTYPES_H
#تعریف_کن سرآیند_CURL_FUNCTYPES_H
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

#اگر_تعریف_شده USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#تعریف_کن RECV_TYPE_ARG1 SOCKET
#تعریف_کن RECV_TYPE_ARG2 حرف *
#تعریف_کن RECV_TYPE_ARG3 صحيح
#تعریف_کن RECV_TYPE_RETV صحيح

/* int send(SOCKET, const char *, int, int); */
#تعریف_کن SEND_TYPE_ARG1 SOCKET
#تعریف_کن SEND_TYPE_ARG2 حرف *
#تعریف_کن SEND_TYPE_ARG3 صحيح
#تعریف_کن SEND_TYPE_RETV صحيح

#وإلاإذا defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#تعریف_کن RECV_TYPE_ARG1 دراز
#تعریف_کن RECV_TYPE_ARG2 حرف *
#تعریف_کن RECV_TYPE_ARG3 دراز
#تعریف_کن RECV_TYPE_ARG4 دراز
#تعریف_کن RECV_TYPE_RETV دراز

/* int send(int, char *, int, int); */
#تعریف_کن SEND_TYPE_ARG1 صحيح
#تعریف_کن SEND_NONCONST_ARG2
#تعریف_کن SEND_TYPE_ARG2 حرف *
#تعریف_کن SEND_TYPE_ARG3 صحيح
#تعریف_کن SEND_TYPE_RETV صحيح
#پایان_اگر

#اگر_تعریف_نشده RECV_TYPE_ARG1
#تعریف_کن RECV_TYPE_ARG1 صحيح
#پایان_اگر

#اگر_تعریف_نشده RECV_TYPE_ARG2
#تعریف_کن RECV_TYPE_ARG2 فراغ *
#پایان_اگر

#اگر_تعریف_نشده RECV_TYPE_ARG3
#تعریف_کن RECV_TYPE_ARG3 size_t
#پایان_اگر

#اگر_تعریف_نشده RECV_TYPE_ARG4
#تعریف_کن RECV_TYPE_ARG4 صحيح
#پایان_اگر

#اگر_تعریف_نشده RECV_TYPE_RETV
#تعریف_کن RECV_TYPE_RETV ssize_t
#پایان_اگر

#اگر_تعریف_نشده SEND_TYPE_ARG1
#تعریف_کن SEND_TYPE_ARG1 صحيح
#پایان_اگر

#اگر_تعریف_نشده SEND_TYPE_ARG2
#تعریف_کن SEND_TYPE_ARG2 فراغ *
#پایان_اگر

#اگر_تعریف_نشده SEND_TYPE_ARG3
#تعریف_کن SEND_TYPE_ARG3 size_t
#پایان_اگر

#اگر_تعریف_نشده SEND_TYPE_ARG4
#تعریف_کن SEND_TYPE_ARG4 صحيح
#پایان_اگر

#اگر_تعریف_نشده SEND_TYPE_RETV
#تعریف_کن SEND_TYPE_RETV ssize_t
#پایان_اگر

#پایان_اگر /* HEADER_CURL_FUNCTYPES_H */
