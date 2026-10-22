#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_FUNCTYPES_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_FUNCTYPES_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
/* int recv(SOCKET, char *, int, int) */
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG1 SOCKET
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG2 ಅಕ್ಷರ *
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG3 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_RETV ಪೂರ್ಣಾಂಕ

/* int send(SOCKET, const char *, int, int); */
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG1 SOCKET
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG2 ಅಕ್ಷರ *
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG3 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_RETV ಪೂರ್ಣಾಂಕ

#ಅಥವಾಆದರೆ defined(__AMIGA__) /* Any AmigaOS flavor */

/* long recv(long, char *, long, long); */
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG1 ದೀರ್ಘ
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG2 ಅಕ್ಷರ *
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG3 ದೀರ್ಘ
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG4 ದೀರ್ಘ
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_RETV ದೀರ್ಘ

/* int send(int, char *, int, int); */
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG1 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_NONCONST_ARG2
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG2 ಅಕ್ಷರ *
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG3 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_RETV ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RECV_TYPE_ARG1
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG1 ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RECV_TYPE_ARG2
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG2 ಶೂನ್ಯ *
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RECV_TYPE_ARG3
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG3 size_t
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RECV_TYPE_ARG4
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG4 ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RECV_TYPE_RETV
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_RETV ssize_t
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEND_TYPE_ARG1
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG1 ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEND_TYPE_ARG2
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG2 ಶೂನ್ಯ *
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEND_TYPE_ARG3
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG3 size_t
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEND_TYPE_ARG4
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG4 ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEND_TYPE_RETV
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_RETV ssize_t
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_FUNCTYPES_H */
