#اگر_تعریف_نشده سرآیند_CURL_CONFIG_WIN32_H
#تعریف_کن سرآیند_CURL_CONFIG_WIN32_H
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
#خطا This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#پایان_اگر

/*
 * Headers and functions
 */

#تعریف_کن HAVE_FCNTL_H 1
#تعریف_کن HAVE_IO_H 1
#تعریف_کن HAVE_LOCALE_H 1
#إذا _MSC_VER >= 1800
#تعریف_کن HAVE_STDBOOL_H 1
#تعریف_کن HAVE_BOOL_T 1
#پایان_اگر
#تعریف_کن HAVE_SYS_TYPES_H 1
#تعریف_کن HAVE_SYS_UTIME_H 1

#تعریف_کن HAVE_CLOSESOCKET 1
#تعریف_کن HAVE_FREEADDRINFO 1
#تعریف_کن HAVE_GETADDRINFO 1
#تعریف_کن HAVE_GETADDRINFO_THREADSAFE 1
#تعریف_کن HAVE_GETHOSTNAME 1
#تعریف_کن HAVE_GETPEERNAME 1
#تعریف_کن HAVE_GETSOCKNAME 1
#تعریف_کن HAVE_IOCTLSOCKET 1
#تعریف_کن HAVE_IOCTLSOCKET_FIONBIO 1
#تعریف_کن HAVE_SETLOCALE 1
#تعریف_کن HAVE_SOCKET 1
#تعریف_کن HAVE_UTIME 1
#تعریف_کن HAVE_RECV 1
#تعریف_کن RECV_TYPE_ARG1 SOCKET
#تعریف_کن RECV_TYPE_ARG2 حرف *
#تعریف_کن RECV_TYPE_ARG3 صحيح
#تعریف_کن RECV_TYPE_ARG4 صحيح
#تعریف_کن RECV_TYPE_RETV صحيح
#تعریف_کن HAVE_SEND 1
#تعریف_کن SEND_TYPE_ARG1 SOCKET
#تعریف_کن SEND_TYPE_ARG2 حرف *
#تعریف_کن SEND_TYPE_ARG3 صحيح
#تعریف_کن SEND_TYPE_ARG4 صحيح
#تعریف_کن SEND_TYPE_RETV صحيح
#تعریف_کن HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#تعریف_کن SIZEOF_INT 4
#تعریف_کن SIZEOF_LONG 4
#اگر_تعریف_شده _WIN64
#  define SIZEOF_اندازه_T 8
#  define ssize_t __int64
#وإلا
#  define SIZEOF_اندازه_T 4
#  define ssize_t صحيح
#پایان_اگر
#تعریف_کن SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#اگر_تعریف_نشده USE_32_بیت_زمان_T
#  define SIZEOF_زمان_T 8
#وإلا
#  define SIZEOF_زمان_T 4
#پایان_اگر
#تعریف_کن SIZEOF_OFF_T 4

#تعریف_کن HAVE_STRUCT_SOCKADDR_STORAGE 1
#تعریف_کن HAVE_STRUCT_TIMEVAL 1
#تعریف_کن HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#إذا !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#پایان_اگر

#تعریف_کن HAVE_LDAP_SSL 1
#تعریف_کن USE_WIN32_LDAP 1
#تعریف_کن USE_WIN32_CRYPTO 1
#تعریف_کن USE_UNIX_SOCKETS 1

#اگر_تعریف_نشده CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  وإلا
#  define CURL_OS "unknown-pc-win32"
#  endif
#پایان_اگر

#پایان_اگر /* HEADER_CURL_CONFIG_WIN32_H */
