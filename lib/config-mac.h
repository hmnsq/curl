#اگر_تعریف_نشده سرآیند_CURL_CONFIG_MAC_H
#تعریف_کن سرآیند_CURL_CONFIG_MAC_H
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

#اگر_تعریف_نشده CURL_OS
#تعریف_کن CURL_OS "mac"
#پایان_اگر

#ضمن <ConditionalMacros.h>

/* Define if you want the built-in manual */
#تعریف_کن USE_MANUAL              1

#تعریف_کن HAVE_NETINET_درون_H       1
#تعریف_کن HAVE_NETDB_H            1
#تعریف_کن HAVE_ARPA_INET_H        1
#تعریف_کن HAVE_UNISTD_H           1
#تعریف_کن HAVE_NET_IF_H           1
#تعریف_کن HAVE_SYS_TYPES_H        1
#تعریف_کن HAVE_GETTIMEOFDAY       1
#تعریف_کن HAVE_FCNTL_H            1
#تعریف_کن HAVE_UTIME_H            1
#تعریف_کن HAVE_SYS_UTIME_H        1
#تعریف_کن HAVE_SYS_IOCTL_H        1
#تعریف_کن HAVE_ALARM              1
#تعریف_کن HAVE_UTIME              1
#تعریف_کن HAVE_SOCKET             1
#تعریف_کن HAVE_STRUCT_TIMEVAL     1

#تعریف_کن HAVE_SIGACTION          1

#تعریف_کن CURL_DISABLE_LDAP

#تعریف_کن HAVE_IOCTL_FIONBIO      1

#تعریف_کن SIZEOF_INT              4
#تعریف_کن SIZEOF_LONG             4
#تعریف_کن SIZEOF_اندازه_T           4
#تعریف_کن SIZEOF_CURL_OFF_T       8

#تعریف_کن HAVE_RECV 1
#تعریف_کن RECV_TYPE_ARG1 صحيح
#تعریف_کن RECV_TYPE_ARG2 فراغ *
#تعریف_کن RECV_TYPE_ARG3 size_t
#تعریف_کن RECV_TYPE_ARG4 صحيح
#تعریف_کن RECV_TYPE_RETV ssize_t

#تعریف_کن HAVE_SEND 1
#تعریف_کن SEND_TYPE_ARG1 صحيح
#تعریف_کن SEND_TYPE_ARG2 فراغ *
#تعریف_کن SEND_TYPE_ARG3 size_t
#تعریف_کن SEND_TYPE_ARG4 صحيح
#تعریف_کن SEND_TYPE_RETV ssize_t

#پایان_اگر /* HEADER_CURL_CONFIG_MAC_H */
