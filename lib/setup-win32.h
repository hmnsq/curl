#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_تياري_WIN32_H
#وصف_ڪريو سرو_CURL_تياري_WIN32_H
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

#وصف_رد USE_WINSOCK
/* ---------------------------------------------------------------- */
/*                     Watt-32 TCP/IP SPECIFIC                      */
/* ---------------------------------------------------------------- */
#جيڪڏهن_وصف_ٿيل USE_WATT32
#  include <tcp.h>
#  include <sys/socket.h>
#  undef byte
#  undef لفظ
#  define HAVE_SYS_IOCTL_H
#  define HAVE_NETINET_اندريون_H
#  define HAVE_NETDB_H
#  define HAVE_ARPA_INET_H
#  define SOCKET صحيح
/* ---------------------------------------------------------------- */
/*               BSD-style lwIP TCP/IP stack SPECIFIC               */
/* ---------------------------------------------------------------- */
#وإلاإذا defined(USE_LWIPSOCK)
  /* Define to use BSD-style lwIP TCP/IP stack. */
  /* #define USE_LWIPSOCK 1 */
#  undef HAVE_GETHOSTNAME
#  define HAVE_GETHOSTBYNAME_R
#  define HAVE_GETHOSTBYNAME_R_6
#  undef LWIP_POSIX_SOCKETS_IO_نالا
#  define LWIP_POSIX_SOCKETS_IO_نالا 0
#  undef RECV_TYPE_ARG1
#  define RECV_TYPE_ARG1 صحيح
#  undef RECV_TYPE_ARG3
#  define RECV_TYPE_ARG3 size_t
#  undef SEND_TYPE_ARG1
#  define SEND_TYPE_ARG1 صحيح
#  undef SEND_TYPE_ARG3
#  define SEND_TYPE_ARG3 size_t
#وإلاإذا defined(_WIN32)
#  define USE_WINSOCK 2
#  include <winsock2.h>
#  include <ws2tcpip.h>
#جيڪڏهن_پورو

/*
 * Include header files for Windows builds before redefining anything. Use
 * this preprocessor block only to include or exclude windows.h, winsock2.h or
 * ws2tcpip.h. Any other Windows thing belongs to any other further and
 * independent block. Under Cygwin things work as under Linux (e.g.
 * <sys/socket.h>) and the Winsock headers should never be included when
 * __CYGWIN__ is defined.
 */
#جيڪڏهن_وصف_ٿيل _WIN32
#  إذا defined(UNICODE) && !defined(_UNICODE)
#    error "UNICODE is defined but _UNICODE is not defined"
#  endif
#  إذا defined(_UNICODE) && !defined(UNICODE)
#    error "_UNICODE is defined but UNICODE is not defined"
#  endif
#  include <windows.h>
#  include <winerror.h>
#  include <tchar.h>
#جيڪڏهن_پورو

/*
 * Define _WIN32_WINNT_[OS] symbols because not all Windows build systems have
 * those symbols to compare against, and even those that do may be missing
 * newer symbols.
 */
#جيڪڏهن_وصف_نه_ٿيل _WIN32_WINNT_VISTA
#وصف_ڪريو _WIN32_WINNT_VISTA          0x0600   /* Windows Vista */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل _WIN32_WINNT_WS08
#وصف_ڪريو _WIN32_WINNT_WS08           0x0600   /* Windows Server 2008 */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل _WIN32_WINNT_WIN7
#وصف_ڪريو _WIN32_WINNT_WIN7           0x0601   /* Windows 7 */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل _WIN32_WINNT_WIN8
#وصف_ڪريو _WIN32_WINNT_WIN8           0x0602   /* Windows 8 */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل _WIN32_WINNT_WINBLUE
#وصف_ڪريو _WIN32_WINNT_WINBLUE        0x0603   /* Windows 8.1 */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل _WIN32_WINNT_WIN10
#وصف_ڪريو _WIN32_WINNT_WIN10          0x0A00   /* Windows 10 */
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_SETUP_WIN32_H */
