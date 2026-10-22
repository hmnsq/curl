#که_نه_تعریف_شوی سرلیک_CURL_CONFIG_MAC_H
#تعریف_کړه سرلیک_CURL_CONFIG_MAC_H
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

#که_نه_تعریف_شوی CURL_OS
#تعریف_کړه CURL_OS "mac"
#د_که_پای

#ضمن <ConditionalMacros.h>

/* Define if you want the built-in manual */
#تعریف_کړه USE_MANUAL              1

#تعریف_کړه HAVE_NETINET_دننه_H       1
#تعریف_کړه HAVE_NETDB_H            1
#تعریف_کړه HAVE_ARPA_INET_H        1
#تعریف_کړه HAVE_UNISTD_H           1
#تعریف_کړه HAVE_NET_IF_H           1
#تعریف_کړه HAVE_SYS_TYPES_H        1
#تعریف_کړه HAVE_GETTIMEOFDAY       1
#تعریف_کړه HAVE_FCNTL_H            1
#تعریف_کړه HAVE_UTIME_H            1
#تعریف_کړه HAVE_SYS_UTIME_H        1
#تعریف_کړه HAVE_SYS_IOCTL_H        1
#تعریف_کړه HAVE_ALARM              1
#تعریف_کړه HAVE_UTIME              1
#تعریف_کړه HAVE_SOCKET             1
#تعریف_کړه HAVE_STRUCT_TIMEVAL     1

#تعریف_کړه HAVE_SIGACTION          1

#تعریف_کړه CURL_DISABLE_LDAP

#تعریف_کړه HAVE_IOCTL_FIONBIO      1

#تعریف_کړه SIZEOF_INT              4
#تعریف_کړه SIZEOF_LONG             4
#تعریف_کړه SIZEOF_اندازه_T           4
#تعریف_کړه SIZEOF_CURL_OFF_T       8

#تعریف_کړه HAVE_RECV 1
#تعریف_کړه RECV_TYPE_ARG1 صحيح
#تعریف_کړه RECV_TYPE_ARG2 فراغ *
#تعریف_کړه RECV_TYPE_ARG3 size_t
#تعریف_کړه RECV_TYPE_ARG4 صحيح
#تعریف_کړه RECV_TYPE_RETV ssize_t

#تعریف_کړه HAVE_SEND 1
#تعریف_کړه SEND_TYPE_ARG1 صحيح
#تعریف_کړه SEND_TYPE_ARG2 فراغ *
#تعریف_کړه SEND_TYPE_ARG3 size_t
#تعریف_کړه SEND_TYPE_ARG4 صحيح
#تعریف_کړه SEND_TYPE_RETV ssize_t

#د_که_پای /* HEADER_CURL_CONFIG_MAC_H */
