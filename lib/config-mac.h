#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CONFIG_MAC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CONFIG_MAC_H
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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_OS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_OS "mac"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <ConditionalMacros.h>

/* Define if you want the built-in manual */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_MANUAL              1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_NETINET_ਵਿੱਚ_H       1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_NETDB_H            1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ARPA_INET_H        1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_UNISTD_H           1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_NET_IF_H           1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SYS_TYPES_H        1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETTIMEOFDAY       1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_FCNTL_H            1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_UTIME_H            1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SYS_UTIME_H        1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SYS_IOCTL_H        1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_ALARM              1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_UTIME              1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SOCKET             1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_STRUCT_TIMEVAL     1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SIGACTION          1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DISABLE_LDAP

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_IOCTL_FIONBIO      1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_INT              4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_LONG             4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_ਮਾਪ_T           4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_CURL_OFF_T       8

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_RECV 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG1 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG2 ਖਾਲੀ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG3 size_t
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG4 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_RETV ssize_t

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SEND 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG1 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG2 ਖਾਲੀ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG3 size_t
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG4 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_RETV ssize_t

#ਜੇ_ਅੰਤ /* HEADER_CURL_CONFIG_MAC_H */
