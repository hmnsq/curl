#که_نه_تعریف_شوی سرلیک_CURL_FUNCTYPES_H
#تعریف_کړه سرلیک_CURL_FUNCTYPES_H
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

#که_تعریف_شوی USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#تعریف_کړه RECV_TYPE_ARG1 SOCKET
#تعریف_کړه RECV_TYPE_ARG2 حرف *
#تعریف_کړه RECV_TYPE_ARG3 صحيح
#تعریف_کړه RECV_TYPE_RETV صحيح

/* int send(SOCKET, const char *, int, int); */
#تعریف_کړه SEND_TYPE_ARG1 SOCKET
#تعریف_کړه SEND_TYPE_ARG2 حرف *
#تعریف_کړه SEND_TYPE_ARG3 صحيح
#تعریف_کړه SEND_TYPE_RETV صحيح

#وإلاإذا defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#تعریف_کړه RECV_TYPE_ARG1 اوږد
#تعریف_کړه RECV_TYPE_ARG2 حرف *
#تعریف_کړه RECV_TYPE_ARG3 اوږد
#تعریف_کړه RECV_TYPE_ARG4 اوږد
#تعریف_کړه RECV_TYPE_RETV اوږد

/* int send(int, char *, int, int); */
#تعریف_کړه SEND_TYPE_ARG1 صحيح
#تعریف_کړه SEND_NONCONST_ARG2
#تعریف_کړه SEND_TYPE_ARG2 حرف *
#تعریف_کړه SEND_TYPE_ARG3 صحيح
#تعریف_کړه SEND_TYPE_RETV صحيح
#د_که_پای

#که_نه_تعریف_شوی RECV_TYPE_ARG1
#تعریف_کړه RECV_TYPE_ARG1 صحيح
#د_که_پای

#که_نه_تعریف_شوی RECV_TYPE_ARG2
#تعریف_کړه RECV_TYPE_ARG2 فراغ *
#د_که_پای

#که_نه_تعریف_شوی RECV_TYPE_ARG3
#تعریف_کړه RECV_TYPE_ARG3 size_t
#د_که_پای

#که_نه_تعریف_شوی RECV_TYPE_ARG4
#تعریف_کړه RECV_TYPE_ARG4 صحيح
#د_که_پای

#که_نه_تعریف_شوی RECV_TYPE_RETV
#تعریف_کړه RECV_TYPE_RETV ssize_t
#د_که_پای

#که_نه_تعریف_شوی SEND_TYPE_ARG1
#تعریف_کړه SEND_TYPE_ARG1 صحيح
#د_که_پای

#که_نه_تعریف_شوی SEND_TYPE_ARG2
#تعریف_کړه SEND_TYPE_ARG2 فراغ *
#د_که_پای

#که_نه_تعریف_شوی SEND_TYPE_ARG3
#تعریف_کړه SEND_TYPE_ARG3 size_t
#د_که_پای

#که_نه_تعریف_شوی SEND_TYPE_ARG4
#تعریف_کړه SEND_TYPE_ARG4 صحيح
#د_که_پای

#که_نه_تعریف_شوی SEND_TYPE_RETV
#تعریف_کړه SEND_TYPE_RETV ssize_t
#د_که_پای

#د_که_پای /* HEADER_CURL_FUNCTYPES_H */
