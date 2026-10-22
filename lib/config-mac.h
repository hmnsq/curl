#अगर_अघोषित HEADER_CURL_CONFIG_MAC_H
#घोषणा HEADER_CURL_CONFIG_MAC_H
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

/* =================================================================== */
/*                Handcrafted config file for Mac OS 9                 */
/* =================================================================== */
/*   On macOS you must run configure to generate curl_config.h file    */
/* =================================================================== */

#अगर_अघोषित CURL_OS
#घोषणा CURL_OS "mac"
#पूर्ण_अगर

#समावेश <ConditionalMacros.h>

/* Define if you want the built-in manual */
#घोषणा USE_MANUAL              1

#घोषणा HAVE_NETINET_IN_H       1
#घोषणा HAVE_NETDB_H            1
#घोषणा HAVE_ARPA_INET_H        1
#घोषणा HAVE_UNISTD_H           1
#घोषणा HAVE_NET_IF_H           1
#घोषणा HAVE_SYS_TYPES_H        1
#घोषणा HAVE_GETTIMEOFDAY       1
#घोषणा HAVE_FCNTL_H            1
#घोषणा HAVE_UTIME_H            1
#घोषणा HAVE_SYS_UTIME_H        1
#घोषणा HAVE_SYS_IOCTL_H        1
#घोषणा HAVE_ALARM              1
#घोषणा HAVE_UTIME              1
#घोषणा HAVE_SOCKET             1
#घोषणा HAVE_STRUCT_TIMEVAL     1

#घोषणा HAVE_SIGACTION          1

#घोषणा CURL_DISABLE_LDAP

#घोषणा HAVE_IOCTL_FIONBIO      1

#घोषणा SIZEOF_INT              4
#घोषणा SIZEOF_LONG             4
#घोषणा SIZEOF_आकार_T           4
#घोषणा SIZEOF_CURL_OFF_T       8

#घोषणा HAVE_RECV 1
#घोषणा RECV_TYPE_ARG1 पूर्णांक
#घोषणा RECV_TYPE_ARG2 व्योम *
#घोषणा RECV_TYPE_ARG3 माप_प्रकार
#घोषणा RECV_TYPE_ARG4 पूर्णांक
#घोषणा RECV_TYPE_RETV ssize_t

#घोषणा HAVE_SEND 1
#घोषणा SEND_TYPE_ARG1 पूर्णांक
#घोषणा SEND_TYPE_ARG2 व्योम *
#घोषणा SEND_TYPE_ARG3 माप_प्रकार
#घोषणा SEND_TYPE_ARG4 पूर्णांक
#घोषणा SEND_TYPE_RETV ssize_t

#पूर्ण_अगर /* HEADER_CURL_CONFIG_MAC_H */
