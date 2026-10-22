#నిర్వచితం_కాకపోతే శీర్షిక_CURL_FUNCTYPES_H
#నిర్వచించు శీర్షిక_CURL_FUNCTYPES_H
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

#నిర్వచితమైతే USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#నిర్వచించు RECV_TYPE_ARG1 SOCKET
#నిర్వచించు RECV_TYPE_ARG2 అక్షరం *
#నిర్వచించు RECV_TYPE_ARG3 పూర్ణాంకం
#నిర్వచించు RECV_TYPE_RETV పూర్ణాంకం

/* int send(SOCKET, const char *, int, int); */
#నిర్వచించు SEND_TYPE_ARG1 SOCKET
#నిర్వచించు SEND_TYPE_ARG2 అక్షరం *
#నిర్వచించు SEND_TYPE_ARG3 పూర్ణాంకం
#నిర్వచించు SEND_TYPE_RETV పూర్ణాంకం

#లేదావేళ defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#నిర్వచించు RECV_TYPE_ARG1 దీర్ఘ
#నిర్వచించు RECV_TYPE_ARG2 అక్షరం *
#నిర్వచించు RECV_TYPE_ARG3 దీర్ఘ
#నిర్వచించు RECV_TYPE_ARG4 దీర్ఘ
#నిర్వచించు RECV_TYPE_RETV దీర్ఘ

/* int send(int, char *, int, int); */
#నిర్వచించు SEND_TYPE_ARG1 పూర్ణాంకం
#నిర్వచించు SEND_NONCONST_ARG2
#నిర్వచించు SEND_TYPE_ARG2 అక్షరం *
#నిర్వచించు SEND_TYPE_ARG3 పూర్ణాంకం
#నిర్వచించు SEND_TYPE_RETV పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే RECV_TYPE_ARG1
#నిర్వచించు RECV_TYPE_ARG1 పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే RECV_TYPE_ARG2
#నిర్వచించు RECV_TYPE_ARG2 శూన్య *
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే RECV_TYPE_ARG3
#నిర్వచించు RECV_TYPE_ARG3 size_t
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే RECV_TYPE_ARG4
#నిర్వచించు RECV_TYPE_ARG4 పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే RECV_TYPE_RETV
#నిర్వచించు RECV_TYPE_RETV ssize_t
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SEND_TYPE_ARG1
#నిర్వచించు SEND_TYPE_ARG1 పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SEND_TYPE_ARG2
#నిర్వచించు SEND_TYPE_ARG2 శూన్య *
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SEND_TYPE_ARG3
#నిర్వచించు SEND_TYPE_ARG3 size_t
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SEND_TYPE_ARG4
#నిర్వచించు SEND_TYPE_ARG4 పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SEND_TYPE_RETV
#నిర్వచించు SEND_TYPE_RETV ssize_t
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_FUNCTYPES_H */
