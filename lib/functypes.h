#यदि_अपरिभाषितम् शीर्षकम्_CURL_FUNCTYPES_H
#परिभाषय शीर्षकम्_CURL_FUNCTYPES_H
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

#यदि_परिभाषितम् USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#परिभाषय RECV_TYPE_ARG1 SOCKET
#परिभाषय RECV_TYPE_ARG2 वर्ण *
#परिभाषय RECV_TYPE_ARG3 पूर्णाङ्क
#परिभाषय RECV_TYPE_RETV पूर्णाङ्क

/* int send(SOCKET, const char *, int, int); */
#परिभाषय SEND_TYPE_ARG1 SOCKET
#परिभाषय SEND_TYPE_ARG2 वर्ण *
#परिभाषय SEND_TYPE_ARG3 पूर्णाङ्क
#परिभाषय SEND_TYPE_RETV पूर्णाङ्क

#नोचेद्यदि defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#परिभाषय RECV_TYPE_ARG1 दीर्घ
#परिभाषय RECV_TYPE_ARG2 वर्ण *
#परिभाषय RECV_TYPE_ARG3 दीर्घ
#परिभाषय RECV_TYPE_ARG4 दीर्घ
#परिभाषय RECV_TYPE_RETV दीर्घ

/* int send(int, char *, int, int); */
#परिभाषय SEND_TYPE_ARG1 पूर्णाङ्क
#परिभाषय SEND_NONCONST_ARG2
#परिभाषय SEND_TYPE_ARG2 वर्ण *
#परिभाषय SEND_TYPE_ARG3 पूर्णाङ्क
#परिभाषय SEND_TYPE_RETV पूर्णाङ्क
#यदि_अन्तः

#यदि_अपरिभाषितम् RECV_TYPE_ARG1
#परिभाषय RECV_TYPE_ARG1 पूर्णाङ्क
#यदि_अन्तः

#यदि_अपरिभाषितम् RECV_TYPE_ARG2
#परिभाषय RECV_TYPE_ARG2 शून्य *
#यदि_अन्तः

#यदि_अपरिभाषितम् RECV_TYPE_ARG3
#परिभाषय RECV_TYPE_ARG3 size_t
#यदि_अन्तः

#यदि_अपरिभाषितम् RECV_TYPE_ARG4
#परिभाषय RECV_TYPE_ARG4 पूर्णाङ्क
#यदि_अन्तः

#यदि_अपरिभाषितम् RECV_TYPE_RETV
#परिभाषय RECV_TYPE_RETV ssize_t
#यदि_अन्तः

#यदि_अपरिभाषितम् SEND_TYPE_ARG1
#परिभाषय SEND_TYPE_ARG1 पूर्णाङ्क
#यदि_अन्तः

#यदि_अपरिभाषितम् SEND_TYPE_ARG2
#परिभाषय SEND_TYPE_ARG2 शून्य *
#यदि_अन्तः

#यदि_अपरिभाषितम् SEND_TYPE_ARG3
#परिभाषय SEND_TYPE_ARG3 size_t
#यदि_अन्तः

#यदि_अपरिभाषितम् SEND_TYPE_ARG4
#परिभाषय SEND_TYPE_ARG4 पूर्णाङ्क
#यदि_अन्तः

#यदि_अपरिभाषितम् SEND_TYPE_RETV
#परिभाषय SEND_TYPE_RETV ssize_t
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_FUNCTYPES_H */
