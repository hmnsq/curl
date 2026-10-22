#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CONFIG_WIN32_H
#నిర్వచించు శీర్షిక_CURL_CONFIG_WIN32_H
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

/* Handcrafted config file for building via Visual Studio IDE Project Files */

#ఒకవేళ !defined(_MSC_VER) || _MSC_VER > 1800
#దోషం This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#అయితే_ముగింపు

/*
 * Headers and functions
 */

#నిర్వచించు HAVE_FCNTL_H 1
#నిర్వచించు HAVE_IO_H 1
#నిర్వచించు HAVE_LOCALE_H 1
#ఒకవేళ _MSC_VER >= 1800
#నిర్వచించు HAVE_STDBOOL_H 1
#నిర్వచించు HAVE_BOOL_T 1
#అయితే_ముగింపు
#నిర్వచించు HAVE_SYS_TYPES_H 1
#నిర్వచించు HAVE_SYS_UTIME_H 1

#నిర్వచించు HAVE_CLOSESOCKET 1
#నిర్వచించు HAVE_FREEADDRINFO 1
#నిర్వచించు HAVE_GETADDRINFO 1
#నిర్వచించు HAVE_GETADDRINFO_THREADSAFE 1
#నిర్వచించు HAVE_GETHOSTNAME 1
#నిర్వచించు HAVE_GETPEERNAME 1
#నిర్వచించు HAVE_GETSOCKNAME 1
#నిర్వచించు HAVE_IOCTLSOCKET 1
#నిర్వచించు HAVE_IOCTLSOCKET_FIONBIO 1
#నిర్వచించు HAVE_SETLOCALE 1
#నిర్వచించు HAVE_SOCKET 1
#నిర్వచించు HAVE_UTIME 1
#నిర్వచించు HAVE_RECV 1
#నిర్వచించు RECV_TYPE_ARG1 SOCKET
#నిర్వచించు RECV_TYPE_ARG2 అక్షరం *
#నిర్వచించు RECV_TYPE_ARG3 పూర్ణాంకం
#నిర్వచించు RECV_TYPE_ARG4 పూర్ణాంకం
#నిర్వచించు RECV_TYPE_RETV పూర్ణాంకం
#నిర్వచించు HAVE_SEND 1
#నిర్వచించు SEND_TYPE_ARG1 SOCKET
#నిర్వచించు SEND_TYPE_ARG2 అక్షరం *
#నిర్వచించు SEND_TYPE_ARG3 పూర్ణాంకం
#నిర్వచించు SEND_TYPE_ARG4 పూర్ణాంకం
#నిర్వచించు SEND_TYPE_RETV పూర్ణాంకం
#నిర్వచించు HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#నిర్వచించు SIZEOF_INT 4
#నిర్వచించు SIZEOF_LONG 4
#నిర్వచితమైతే _WIN64
#  define SIZEOF_కొలత_T 8
#  define ssize_t __int64
#లేకపోతే
#  define SIZEOF_కొలత_T 4
#  define ssize_t పూర్ణాంకం
#అయితే_ముగింపు
#నిర్వచించు SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#నిర్వచితం_కాకపోతే USE_32_బిట్టు_సమయం_T
#  define SIZEOF_సమయం_T 8
#లేకపోతే
#  define SIZEOF_సమయం_T 4
#అయితే_ముగింపు
#నిర్వచించు SIZEOF_OFF_T 4

#నిర్వచించు HAVE_STRUCT_SOCKADDR_STORAGE 1
#నిర్వచించు HAVE_STRUCT_TIMEVAL 1
#నిర్వచించు HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#ఒకవేళ !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#అయితే_ముగింపు

#నిర్వచించు HAVE_LDAP_SSL 1
#నిర్వచించు USE_WIN32_LDAP 1
#నిర్వచించు USE_WIN32_CRYPTO 1
#నిర్వచించు USE_UNIX_SOCKETS 1

#నిర్వచితం_కాకపోతే CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  లేకపోతే
#  define CURL_OS "unknown-pc-win32"
#  endif
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_CONFIG_WIN32_H */
