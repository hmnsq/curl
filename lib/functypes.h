#अगर_अघोषित HEADER_CURL_FUNCTYPES_H
#घोषणा HEADER_CURL_FUNCTYPES_H
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

#अगर_घोषित USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#घोषणा RECV_TYPE_ARG1 SOCKET
#घोषणा RECV_TYPE_ARG2 अक्षर *
#घोषणा RECV_TYPE_ARG3 पूर्णांक
#घोषणा RECV_TYPE_RETV पूर्णांक

/* int send(SOCKET, const char *, int, int); */
#घोषणा SEND_TYPE_ARG1 SOCKET
#घोषणा SEND_TYPE_ARG2 अक्षर *
#घोषणा SEND_TYPE_ARG3 पूर्णांक
#घोषणा SEND_TYPE_RETV पूर्णांक

#या_अगर defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#घोषणा RECV_TYPE_ARG1 दीर्घ
#घोषणा RECV_TYPE_ARG2 अक्षर *
#घोषणा RECV_TYPE_ARG3 दीर्घ
#घोषणा RECV_TYPE_ARG4 दीर्घ
#घोषणा RECV_TYPE_RETV दीर्घ

/* int send(int, char *, int, int); */
#घोषणा SEND_TYPE_ARG1 पूर्णांक
#घोषणा SEND_NONCONST_ARG2
#घोषणा SEND_TYPE_ARG2 अक्षर *
#घोषणा SEND_TYPE_ARG3 पूर्णांक
#घोषणा SEND_TYPE_RETV पूर्णांक
#पूर्ण_अगर

#अगर_अघोषित RECV_TYPE_ARG1
#घोषणा RECV_TYPE_ARG1 पूर्णांक
#पूर्ण_अगर

#अगर_अघोषित RECV_TYPE_ARG2
#घोषणा RECV_TYPE_ARG2 व्योम *
#पूर्ण_अगर

#अगर_अघोषित RECV_TYPE_ARG3
#घोषणा RECV_TYPE_ARG3 माप_प्रकार
#पूर्ण_अगर

#अगर_अघोषित RECV_TYPE_ARG4
#घोषणा RECV_TYPE_ARG4 पूर्णांक
#पूर्ण_अगर

#अगर_अघोषित RECV_TYPE_RETV
#घोषणा RECV_TYPE_RETV ssize_t
#पूर्ण_अगर

#अगर_अघोषित SEND_TYPE_ARG1
#घोषणा SEND_TYPE_ARG1 पूर्णांक
#पूर्ण_अगर

#अगर_अघोषित SEND_TYPE_ARG2
#घोषणा SEND_TYPE_ARG2 व्योम *
#पूर्ण_अगर

#अगर_अघोषित SEND_TYPE_ARG3
#घोषणा SEND_TYPE_ARG3 माप_प्रकार
#पूर्ण_अगर

#अगर_अघोषित SEND_TYPE_ARG4
#घोषणा SEND_TYPE_ARG4 पूर्णांक
#पूर्ण_अगर

#अगर_अघोषित SEND_TYPE_RETV
#घोषणा SEND_TYPE_RETV ssize_t
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_FUNCTYPES_H */
