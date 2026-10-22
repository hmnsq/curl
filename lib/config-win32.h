#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CONFIG_WIN32_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CONFIG_WIN32_H
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

#ಆದರೆ !defined(_MSC_VER) || _MSC_VER > 1800
#ದೋಷ This manual configuration requires MSVC 2010-2013 (IDE Project builds)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Headers and functions
 */

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_FCNTL_H 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_IO_H 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_LOCALE_H 1
#ಆದರೆ _MSC_VER >= 1800
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_STDBOOL_H 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_BOOL_T 1
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SYS_TYPES_H 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SYS_UTIME_H 1

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_CLOSESOCKET 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_FREEADDRINFO 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETADDRINFO 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETADDRINFO_THREADSAFE 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETHOSTNAME 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETPEERNAME 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_GETSOCKNAME 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_IOCTLSOCKET 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_IOCTLSOCKET_FIONBIO 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SETLOCALE 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SOCKET 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_UTIME 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_RECV 1
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG1 SOCKET
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG2 ಅಕ್ಷರ *
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG3 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_ARG4 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು RECV_TYPE_RETV ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SEND 1
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG1 SOCKET
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG2 ಅಕ್ಷರ *
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG3 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_ARG4 ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_TYPE_RETV ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SIGNAL 1

/*
 * Types and sizes
 */

#ವ್ಯಾಖ್ಯಾನಿಸು SIZEOF_INT 4
#ವ್ಯಾಖ್ಯಾನಿಸು SIZEOF_LONG 4
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN64
#  define SIZEOF_ಗಾತ್ರ_T 8
#  define ssize_t __int64
#ಇಲ್ಲದಿದ್ದರೆ
#  define SIZEOF_ಗಾತ್ರ_T 4
#  define ssize_t ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು SIZEOF_CURL_OFF_T 8
/* Default to 64-bit time_t unless _USE_32BIT_TIME_T is defined */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_32_ಬಿಟ್_ಸಮಯ_T
#  define SIZEOF_ಸಮಯ_T 8
#ಇಲ್ಲದಿದ್ದರೆ
#  define SIZEOF_ಸಮಯ_T 4
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು SIZEOF_OFF_T 4

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_STRUCT_SOCKADDR_STORAGE 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_STRUCT_TIMEVAL 1
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/*
 * Additional definitions
 */

/* Default define to enable threaded asynchronous DNS lookups. */
#ಆದರೆ !defined(USE_RESOLV_THREADED) && !defined(USE_SYNC_DNS)
#  define USE_RESOLV_THREADED 1
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_LDAP_SSL 1
#ವ್ಯಾಖ್ಯಾನಿಸು USE_WIN32_LDAP 1
#ವ್ಯಾಖ್ಯಾನಿಸು USE_WIN32_CRYPTO 1
#ವ್ಯಾಖ್ಯಾನಿಸು USE_UNIX_SOCKETS 1

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_OS
#  ifdef _M_IX86
#  define CURL_OS "i386-pc-win32"
#  elif defined(_M_X64)
#  define CURL_OS "x86_64-pc-win32"
#  ಇಲ್ಲದಿದ್ದರೆ
#  define CURL_OS "unknown-pc-win32"
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CONFIG_WIN32_H */
