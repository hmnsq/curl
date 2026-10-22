#यदि_अपरिभाषितम् शीर्षकम्_CURL_CONFIG_MAC_H
#परिभाषय शीर्षकम्_CURL_CONFIG_MAC_H
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

#यदि_अपरिभाषितम् CURL_OS
#परिभाषय CURL_OS "mac"
#यदि_अन्तः

#अन्तर्भाव <ConditionalMacros.h>

/* Define if you want the built-in manual */
#परिभाषय USE_MANUAL              1

#परिभाषय HAVE_NETINET_अन्तर्_H       1
#परिभाषय HAVE_NETDB_H            1
#परिभाषय HAVE_ARPA_INET_H        1
#परिभाषय HAVE_UNISTD_H           1
#परिभाषय HAVE_NET_IF_H           1
#परिभाषय HAVE_SYS_TYPES_H        1
#परिभाषय HAVE_GETTIMEOFDAY       1
#परिभाषय HAVE_FCNTL_H            1
#परिभाषय HAVE_UTIME_H            1
#परिभाषय HAVE_SYS_UTIME_H        1
#परिभाषय HAVE_SYS_IOCTL_H        1
#परिभाषय HAVE_ALARM              1
#परिभाषय HAVE_UTIME              1
#परिभाषय HAVE_SOCKET             1
#परिभाषय HAVE_STRUCT_TIMEVAL     1

#परिभाषय HAVE_SIGACTION          1

#परिभाषय CURL_DISABLE_LDAP

#परिभाषय HAVE_IOCTL_FIONBIO      1

#परिभाषय SIZEOF_INT              4
#परिभाषय SIZEOF_LONG             4
#परिभाषय SIZEOF_आकार_T           4
#परिभाषय SIZEOF_CURL_OFF_T       8

#परिभाषय HAVE_RECV 1
#परिभाषय RECV_TYPE_ARG1 पूर्णाङ्क
#परिभाषय RECV_TYPE_ARG2 शून्य *
#परिभाषय RECV_TYPE_ARG3 size_t
#परिभाषय RECV_TYPE_ARG4 पूर्णाङ्क
#परिभाषय RECV_TYPE_RETV ssize_t

#परिभाषय HAVE_SEND 1
#परिभाषय SEND_TYPE_ARG1 पूर्णाङ्क
#परिभाषय SEND_TYPE_ARG2 शून्य *
#परिभाषय SEND_TYPE_ARG3 size_t
#परिभाषय SEND_TYPE_ARG4 पूर्णाङ्क
#परिभाषय SEND_TYPE_RETV ssize_t

#यदि_अन्तः /* HEADER_CURL_CONFIG_MAC_H */
