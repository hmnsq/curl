#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CONFIG_MAC_H
#నిర్వచించు శీర్షిక_CURL_CONFIG_MAC_H
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

#నిర్వచితం_కాకపోతే CURL_OS
#నిర్వచించు CURL_OS "mac"
#అయితే_ముగింపు

#అన్తర్భావ <ConditionalMacros.h>

/* Define if you want the built-in manual */
#నిర్వచించు USE_MANUAL              1

#నిర్వచించు HAVE_NETINET_లోపల_H       1
#నిర్వచించు HAVE_NETDB_H            1
#నిర్వచించు HAVE_ARPA_INET_H        1
#నిర్వచించు HAVE_UNISTD_H           1
#నిర్వచించు HAVE_NET_IF_H           1
#నిర్వచించు HAVE_SYS_TYPES_H        1
#నిర్వచించు HAVE_GETTIMEOFDAY       1
#నిర్వచించు HAVE_FCNTL_H            1
#నిర్వచించు HAVE_UTIME_H            1
#నిర్వచించు HAVE_SYS_UTIME_H        1
#నిర్వచించు HAVE_SYS_IOCTL_H        1
#నిర్వచించు HAVE_ALARM              1
#నిర్వచించు HAVE_UTIME              1
#నిర్వచించు HAVE_SOCKET             1
#నిర్వచించు HAVE_STRUCT_TIMEVAL     1

#నిర్వచించు HAVE_SIGACTION          1

#నిర్వచించు CURL_DISABLE_LDAP

#నిర్వచించు HAVE_IOCTL_FIONBIO      1

#నిర్వచించు SIZEOF_INT              4
#నిర్వచించు SIZEOF_LONG             4
#నిర్వచించు SIZEOF_కొలత_T           4
#నిర్వచించు SIZEOF_CURL_OFF_T       8

#నిర్వచించు HAVE_RECV 1
#నిర్వచించు RECV_TYPE_ARG1 పూర్ణాంకం
#నిర్వచించు RECV_TYPE_ARG2 శూన్య *
#నిర్వచించు RECV_TYPE_ARG3 size_t
#నిర్వచించు RECV_TYPE_ARG4 పూర్ణాంకం
#నిర్వచించు RECV_TYPE_RETV ssize_t

#నిర్వచించు HAVE_SEND 1
#నిర్వచించు SEND_TYPE_ARG1 పూర్ణాంకం
#నిర్వచించు SEND_TYPE_ARG2 శూన్య *
#నిర్వచించు SEND_TYPE_ARG3 size_t
#నిర్వచించు SEND_TYPE_ARG4 పూర్ణాంకం
#నిర్వచించు SEND_TYPE_RETV ssize_t

#అయితే_ముగింపు /* HEADER_CURL_CONFIG_MAC_H */
