#अगर_अघोषित HEADER_CURL_CONFIG_WIN32_H
#घोषणा HEADER_CURL_CONFIG_WIN32_H
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

#अगर !defined(_MSC_VER) || _MSC_VER > 1800
#त्रुटि This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#पूर्ण_अगर

/*
 * Headers and functions
 */

#घोषणा HAVE_FCNTL_H 1
#घोषणा HAVE_IO_H 1
#घोषणा HAVE_LOCALE_H 1
#अगर _MSC_VER >= 1800
#घोषणा HAVE_STDBOOL_H 1
#घोषणा HAVE_BOOL_T 1
#पूर्ण_अगर
#घोषणा HAVE_SYS_TYPES_H 1
#घोषणा HAVE_SYS_UTIME_H 1

#घोषणा HAVE_CLOSESOCKET 1
#घोषणा HAVE_FREEADDRINFO 1
#घोषणा HAVE_GETADDRINFO 1
#घोषणा HAVE_GETADDRINFO_THREADSAFE 1
#घोषणा HAVE_GETHOSTNAME 1
#घोषणा HAVE_GETPEERNAME 1
#घोषणा HAVE_GETSOCKNAME 1
#घोषणा HAVE_IOCTLSOCKET 1
#घोषणा HAVE_IOCTLSOCKET_FIONBIO 1
#घोषणा HAVE_SETLOCALE 1
#घोषणा HAVE_SOCKET 1
#घोषणा HAVE_UTIME 1
#घोषणा HAVE_RECV 1
#घोषणा RECV_TYPE_ARG1 SOCKET
#घोषणा RECV_TYPE_ARG2 अक्षर *
#घोषणा RECV_TYPE_ARG3 पूर्णांक
#घोषणा RECV_TYPE_ARG4 पूर्णांक
#घोषणा RECV_TYPE_RETV पूर्णांक
#घोषणा HAVE_SEND 1
#घोषणा SEND_TYPE_ARG1 SOCKET
#घोषणा SEND_TYPE_ARG2 अक्षर *
#घोषणा SEND_TYPE_ARG3 पूर्णांक
#घोषणा SEND_TYPE_ARG4 पूर्णांक
#घोषणा SEND_TYPE_RETV पूर्णांक
#घोषणा HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#घोषणा SIZEOF_INT 4
#घोषणा SIZEOF_LONG 4
#अगर_घोषित _WIN64
#  define SIZEOF_आकार_T 8
#  define ssize_t __int64
#अन्यथा
#  define SIZEOF_आकार_T 4
#  define ssize_t पूर्णांक
#पूर्ण_अगर
#घोषणा SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#अगर_अघोषित _USE_32BIT_TIME_T
#  define SIZEOF_TIME_T 8
#अन्यथा
#  define SIZEOF_TIME_T 4
#पूर्ण_अगर
#घोषणा SIZEOF_OFF_T 4

#घोषणा HAVE_STRUCT_SOCKADDR_STORAGE 1
#घोषणा HAVE_STRUCT_TIMEVAL 1
#घोषणा HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#अगर !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#पूर्ण_अगर

#घोषणा HAVE_LDAP_SSL 1
#घोषणा USE_WIN32_LDAP 1
#घोषणा USE_WIN32_CRYPTO 1
#घोषणा USE_UNIX_SOCKETS 1

#अगर_अघोषित CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  अन्यथा
#  define CURL_OS "unknown-pc-win32"
#  endif
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_CONFIG_WIN32_H */
