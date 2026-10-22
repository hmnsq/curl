#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_CONFIG_WIN32_H
#وصف_ڪريو سرو_CURL_CONFIG_WIN32_H
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

#إذا !defined(_MSC_VER) || _MSC_VER > 1800
#غلطي This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#جيڪڏهن_پورو

/*
 * Headers and functions
 */

#وصف_ڪريو HAVE_FCNTL_H 1
#وصف_ڪريو HAVE_IO_H 1
#وصف_ڪريو HAVE_LOCALE_H 1
#إذا _MSC_VER >= 1800
#وصف_ڪريو HAVE_STDBOOL_H 1
#وصف_ڪريو HAVE_BOOL_T 1
#جيڪڏهن_پورو
#وصف_ڪريو HAVE_SYS_TYPES_H 1
#وصف_ڪريو HAVE_SYS_UTIME_H 1

#وصف_ڪريو HAVE_CLOSESOCKET 1
#وصف_ڪريو HAVE_FREEADDRINFO 1
#وصف_ڪريو HAVE_GETADDRINFO 1
#وصف_ڪريو HAVE_GETADDRINFO_THREADSAFE 1
#وصف_ڪريو HAVE_GETHOSTNAME 1
#وصف_ڪريو HAVE_GETPEERNAME 1
#وصف_ڪريو HAVE_GETSOCKNAME 1
#وصف_ڪريو HAVE_IOCTLSOCKET 1
#وصف_ڪريو HAVE_IOCTLSOCKET_FIONBIO 1
#وصف_ڪريو HAVE_SETLOCALE 1
#وصف_ڪريو HAVE_SOCKET 1
#وصف_ڪريو HAVE_UTIME 1
#وصف_ڪريو HAVE_RECV 1
#وصف_ڪريو RECV_TYPE_ARG1 SOCKET
#وصف_ڪريو RECV_TYPE_ARG2 حرف *
#وصف_ڪريو RECV_TYPE_ARG3 صحيح
#وصف_ڪريو RECV_TYPE_ARG4 صحيح
#وصف_ڪريو RECV_TYPE_RETV صحيح
#وصف_ڪريو HAVE_SEND 1
#وصف_ڪريو SEND_TYPE_ARG1 SOCKET
#وصف_ڪريو SEND_TYPE_ARG2 حرف *
#وصف_ڪريو SEND_TYPE_ARG3 صحيح
#وصف_ڪريو SEND_TYPE_ARG4 صحيح
#وصف_ڪريو SEND_TYPE_RETV صحيح
#وصف_ڪريو HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#وصف_ڪريو SIZEOF_INT 4
#وصف_ڪريو SIZEOF_LONG 4
#جيڪڏهن_وصف_ٿيل _WIN64
#  define SIZEOF_سائيز_T 8
#  define ssize_t __int64
#وإلا
#  define SIZEOF_سائيز_T 4
#  define ssize_t صحيح
#جيڪڏهن_پورو
#وصف_ڪريو SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#جيڪڏهن_وصف_نه_ٿيل USE_32_ٻٽو_انگ_وقت_T
#  define SIZEOF_وقت_T 8
#وإلا
#  define SIZEOF_وقت_T 4
#جيڪڏهن_پورو
#وصف_ڪريو SIZEOF_OFF_T 4

#وصف_ڪريو HAVE_STRUCT_SOCKADDR_STORAGE 1
#وصف_ڪريو HAVE_STRUCT_TIMEVAL 1
#وصف_ڪريو HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#إذا !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#جيڪڏهن_پورو

#وصف_ڪريو HAVE_LDAP_SSL 1
#وصف_ڪريو USE_WIN32_LDAP 1
#وصف_ڪريو USE_WIN32_CRYPTO 1
#وصف_ڪريو USE_UNIX_SOCKETS 1

#جيڪڏهن_وصف_نه_ٿيل CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  وإلا
#  define CURL_OS "unknown-pc-win32"
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_CONFIG_WIN32_H */
