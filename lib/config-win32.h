#यदि_अपरिभाषितम् शीर्षकम्_CURL_CONFIG_WIN32_H
#परिभाषय शीर्षकम्_CURL_CONFIG_WIN32_H
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

#यदि !defined(_MSC_VER) || _MSC_VER > 1800
#दोषः This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#यदि_अन्तः

/*
 * Headers and functions
 */

#परिभाषय HAVE_FCNTL_H 1
#परिभाषय HAVE_IO_H 1
#परिभाषय HAVE_LOCALE_H 1
#यदि _MSC_VER >= 1800
#परिभाषय HAVE_STDBOOL_H 1
#परिभाषय HAVE_BOOL_T 1
#यदि_अन्तः
#परिभाषय HAVE_SYS_TYPES_H 1
#परिभाषय HAVE_SYS_UTIME_H 1

#परिभाषय HAVE_CLOSESOCKET 1
#परिभाषय HAVE_FREEADDRINFO 1
#परिभाषय HAVE_GETADDRINFO 1
#परिभाषय HAVE_GETADDRINFO_THREADSAFE 1
#परिभाषय HAVE_GETHOSTNAME 1
#परिभाषय HAVE_GETPEERNAME 1
#परिभाषय HAVE_GETSOCKNAME 1
#परिभाषय HAVE_IOCTLSOCKET 1
#परिभाषय HAVE_IOCTLSOCKET_FIONBIO 1
#परिभाषय HAVE_SETLOCALE 1
#परिभाषय HAVE_SOCKET 1
#परिभाषय HAVE_UTIME 1
#परिभाषय HAVE_RECV 1
#परिभाषय RECV_TYPE_ARG1 SOCKET
#परिभाषय RECV_TYPE_ARG2 वर्ण *
#परिभाषय RECV_TYPE_ARG3 पूर्णाङ्क
#परिभाषय RECV_TYPE_ARG4 पूर्णाङ्क
#परिभाषय RECV_TYPE_RETV पूर्णाङ्क
#परिभाषय HAVE_SEND 1
#परिभाषय SEND_TYPE_ARG1 SOCKET
#परिभाषय SEND_TYPE_ARG2 वर्ण *
#परिभाषय SEND_TYPE_ARG3 पूर्णाङ्क
#परिभाषय SEND_TYPE_ARG4 पूर्णाङ्क
#परिभाषय SEND_TYPE_RETV पूर्णाङ्क
#परिभाषय HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#परिभाषय SIZEOF_INT 4
#परिभाषय SIZEOF_LONG 4
#यदि_परिभाषितम् _WIN64
#  define SIZEOF_आकार_T 8
#  define ssize_t __int64
#अन्यथा
#  define SIZEOF_आकार_T 4
#  define ssize_t पूर्णाङ्क
#यदि_अन्तः
#परिभाषय SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#यदि_अपरिभाषितम् _USE_32BIT_TIME_T
#  define SIZEOF_TIME_T 8
#अन्यथा
#  define SIZEOF_TIME_T 4
#यदि_अन्तः
#परिभाषय SIZEOF_OFF_T 4

#परिभाषय HAVE_STRUCT_SOCKADDR_STORAGE 1
#परिभाषय HAVE_STRUCT_TIMEVAL 1
#परिभाषय HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#यदि !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#यदि_अन्तः

#परिभाषय HAVE_LDAP_SSL 1
#परिभाषय USE_WIN32_LDAP 1
#परिभाषय USE_WIN32_CRYPTO 1
#परिभाषय USE_UNIX_SOCKETS 1

#यदि_अपरिभाषितम् CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  अन्यथा
#  define CURL_OS "unknown-pc-win32"
#  endif
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_CONFIG_WIN32_H */
