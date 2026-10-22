#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_CONFIG_MAC_H
#وصف_ڪريو سرو_CURL_CONFIG_MAC_H
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

#جيڪڏهن_وصف_نه_ٿيل CURL_OS
#وصف_ڪريو CURL_OS "mac"
#جيڪڏهن_پورو

#ضمن <ConditionalMacros.h>

/* Define if you want the built-in manual */
#وصف_ڪريو USE_MANUAL              1

#وصف_ڪريو HAVE_NETINET_اندريون_H       1
#وصف_ڪريو HAVE_NETDB_H            1
#وصف_ڪريو HAVE_ARPA_INET_H        1
#وصف_ڪريو HAVE_UNISTD_H           1
#وصف_ڪريو HAVE_NET_IF_H           1
#وصف_ڪريو HAVE_SYS_TYPES_H        1
#وصف_ڪريو HAVE_GETTIMEOFDAY       1
#وصف_ڪريو HAVE_FCNTL_H            1
#وصف_ڪريو HAVE_UTIME_H            1
#وصف_ڪريو HAVE_SYS_UTIME_H        1
#وصف_ڪريو HAVE_SYS_IOCTL_H        1
#وصف_ڪريو HAVE_ALARM              1
#وصف_ڪريو HAVE_UTIME              1
#وصف_ڪريو HAVE_SOCKET             1
#وصف_ڪريو HAVE_STRUCT_TIMEVAL     1

#وصف_ڪريو HAVE_SIGACTION          1

#وصف_ڪريو CURL_DISABLE_LDAP

#وصف_ڪريو HAVE_IOCTL_FIONBIO      1

#وصف_ڪريو SIZEOF_INT              4
#وصف_ڪريو SIZEOF_LONG             4
#وصف_ڪريو SIZEOF_سائيز_T           4
#وصف_ڪريو SIZEOF_CURL_OFF_T       8

#وصف_ڪريو HAVE_RECV 1
#وصف_ڪريو RECV_TYPE_ARG1 صحيح
#وصف_ڪريو RECV_TYPE_ARG2 فراغ *
#وصف_ڪريو RECV_TYPE_ARG3 size_t
#وصف_ڪريو RECV_TYPE_ARG4 صحيح
#وصف_ڪريو RECV_TYPE_RETV ssize_t

#وصف_ڪريو HAVE_SEND 1
#وصف_ڪريو SEND_TYPE_ARG1 صحيح
#وصف_ڪريو SEND_TYPE_ARG2 فراغ *
#وصف_ڪريو SEND_TYPE_ARG3 size_t
#وصف_ڪريو SEND_TYPE_ARG4 صحيح
#وصف_ڪريو SEND_TYPE_RETV ssize_t

#جيڪڏهن_پورو /* HEADER_CURL_CONFIG_MAC_H */
