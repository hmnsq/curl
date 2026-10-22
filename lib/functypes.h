#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_FUNCTYPES_H
#وصف_ڪريو سرو_CURL_FUNCTYPES_H
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

#جيڪڏهن_وصف_ٿيل USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#وصف_ڪريو RECV_TYPE_ARG1 SOCKET
#وصف_ڪريو RECV_TYPE_ARG2 حرف *
#وصف_ڪريو RECV_TYPE_ARG3 صحيح
#وصف_ڪريو RECV_TYPE_RETV صحيح

/* int send(SOCKET, const char *, int, int); */
#وصف_ڪريو SEND_TYPE_ARG1 SOCKET
#وصف_ڪريو SEND_TYPE_ARG2 حرف *
#وصف_ڪريو SEND_TYPE_ARG3 صحيح
#وصف_ڪريو SEND_TYPE_RETV صحيح

#وإلاإذا defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#وصف_ڪريو RECV_TYPE_ARG1 ڊگهو
#وصف_ڪريو RECV_TYPE_ARG2 حرف *
#وصف_ڪريو RECV_TYPE_ARG3 ڊگهو
#وصف_ڪريو RECV_TYPE_ARG4 ڊگهو
#وصف_ڪريو RECV_TYPE_RETV ڊگهو

/* int send(int, char *, int, int); */
#وصف_ڪريو SEND_TYPE_ARG1 صحيح
#وصف_ڪريو SEND_NONCONST_ARG2
#وصف_ڪريو SEND_TYPE_ARG2 حرف *
#وصف_ڪريو SEND_TYPE_ARG3 صحيح
#وصف_ڪريو SEND_TYPE_RETV صحيح
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل RECV_TYPE_ARG1
#وصف_ڪريو RECV_TYPE_ARG1 صحيح
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل RECV_TYPE_ARG2
#وصف_ڪريو RECV_TYPE_ARG2 فراغ *
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل RECV_TYPE_ARG3
#وصف_ڪريو RECV_TYPE_ARG3 size_t
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل RECV_TYPE_ARG4
#وصف_ڪريو RECV_TYPE_ARG4 صحيح
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل RECV_TYPE_RETV
#وصف_ڪريو RECV_TYPE_RETV ssize_t
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SEND_TYPE_ARG1
#وصف_ڪريو SEND_TYPE_ARG1 صحيح
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SEND_TYPE_ARG2
#وصف_ڪريو SEND_TYPE_ARG2 فراغ *
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SEND_TYPE_ARG3
#وصف_ڪريو SEND_TYPE_ARG3 size_t
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SEND_TYPE_ARG4
#وصف_ڪريو SEND_TYPE_ARG4 صحيح
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SEND_TYPE_RETV
#وصف_ڪريو SEND_TYPE_RETV ssize_t
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_FUNCTYPES_H */
