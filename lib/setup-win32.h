#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_WIN32_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_WIN32_H
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

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ USE_WINSOCK
/* ---------------------------------------------------------------- */
/*                     Watt-32 TCP/IP SPECIFIC                      */
/* ---------------------------------------------------------------- */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATT32
#  include <tcp.h>
#  include <sys/socket.h>
#  undef byte
#  undef ਸ਼ਬਦ
#  define HAVE_SYS_IOCTL_H
#  define HAVE_NETINET_ਵਿੱਚ_H
#  define HAVE_NETDB_H
#  define HAVE_ARPA_INET_H
#  define SOCKET ਪੂਰਨ
/* ---------------------------------------------------------------- */
/*               BSD-style lwIP TCP/IP stack SPECIFIC               */
/* ---------------------------------------------------------------- */
#ਜਾਂਜੇ defined(USE_LWIPSOCK)
  /* Define to use BSD-style lwIP TCP/IP stack. */
  /* #define USE_LWIPSOCK 1 */
#  undef HAVE_GETHOSTNAME
#  define HAVE_GETHOSTBYNAME_R
#  define HAVE_GETHOSTBYNAME_R_6
#  undef LWIP_POSIX_SOCKETS_IO_ਨਾਂ
#  define LWIP_POSIX_SOCKETS_IO_ਨਾਂ 0
#  undef RECV_TYPE_ARG1
#  define RECV_TYPE_ARG1 ਪੂਰਨ
#  undef RECV_TYPE_ARG3
#  define RECV_TYPE_ARG3 size_t
#  undef SEND_TYPE_ARG1
#  define SEND_TYPE_ARG1 ਪੂਰਨ
#  undef SEND_TYPE_ARG3
#  define SEND_TYPE_ARG3 size_t
#ਜਾਂਜੇ defined(_WIN32)
#  define USE_WINSOCK 2
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ਜੇ_ਅੰਤ

/*
 * Include header files for Windows builds before redefining anything. Use
 * this preprocessor block only to include or exclude windows.h, winsock2.h or
 * ws2tcpip.h. Any other Windows thing belongs to any other further and
 * independent block. Under Cygwin things work as under Linux (e.g.
 * <sys/socket.h>) and the Winsock headers should never be included when
 * __CYGWIN__ is defined.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#  ਜੇ defined(UNICODE) && !defined(_UNICODE)
#    error "UNICODE is defined but _UNICODE is not defined"
#  endif
#  ਜੇ defined(_UNICODE) && !defined(UNICODE)
#    error "_UNICODE is defined but UNICODE is not defined"
#  endif
#  include <windows.h>
#  include <winerror.h>
#  include <tchar.h>
#ਜੇ_ਅੰਤ

/*
 * Define _WIN32_WINNT_[OS] symbols because not all Windows build systems have
 * those symbols to compare against, and even those that do may be missing
 * newer symbols.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32_WINNT_VISTA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _WIN32_WINNT_VISTA          0x0600   /* Windows Vista */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32_WINNT_WS08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _WIN32_WINNT_WS08           0x0600   /* Windows Server 2008 */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32_WINNT_WIN7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _WIN32_WINNT_WIN7           0x0601   /* Windows 7 */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32_WINNT_WIN8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _WIN32_WINNT_WIN8           0x0602   /* Windows 8 */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32_WINNT_WINBLUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _WIN32_WINNT_WINBLUE        0x0603   /* Windows 8.1 */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32_WINNT_WIN10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _WIN32_WINNT_WIN10          0x0A00   /* Windows 10 */
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_SETUP_WIN32_H */
