#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CONFIG_WIN32_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CONFIG_WIN32_H
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

#ਜੇ !defined(_MSC_VER) || _MSC_VER > 1800
#ਗਲਤੀ This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#ਜੇ_ਅੰਤ

/*
 * Headers and functions
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_FCNTL_H 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_IO_H 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_LOCALE_H 1
#ਜੇ _MSC_VER >= 1800
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_STDBOOL_H 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_BOOL_T 1
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SYS_TYPES_H 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SYS_UTIME_H 1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_CLOSESOCKET 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_FREEADDRINFO 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETADDRINFO 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETADDRINFO_THREADSAFE 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETHOSTNAME 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETPEERNAME 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_GETSOCKNAME 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_IOCTLSOCKET 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_IOCTLSOCKET_FIONBIO 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SETLOCALE 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SOCKET 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_UTIME 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_RECV 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG1 SOCKET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG2 ਅੱਖਰ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG3 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_ARG4 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RECV_TYPE_RETV ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SEND 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG1 SOCKET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG2 ਅੱਖਰ *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG3 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_ARG4 ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_TYPE_RETV ਪੂਰਨ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_INT 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_LONG 4
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN64
#  define SIZEOF_ਮਾਪ_T 8
#  define ssize_t __int64
#ਨਹੀਂਤਾਂ
#  define SIZEOF_ਮਾਪ_T 4
#  define ssize_t ਪੂਰਨ
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_32_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸਮਾਂ_T
#  define SIZEOF_ਸਮਾਂ_T 8
#ਨਹੀਂਤਾਂ
#  define SIZEOF_ਸਮਾਂ_T 4
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_OFF_T 4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_STRUCT_SOCKADDR_STORAGE 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_STRUCT_TIMEVAL 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#ਜੇ !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_LDAP_SSL 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_WIN32_LDAP 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_WIN32_CRYPTO 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_UNIX_SOCKETS 1

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  ਨਹੀਂਤਾਂ
#  define CURL_OS "unknown-pc-win32"
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_CONFIG_WIN32_H */
