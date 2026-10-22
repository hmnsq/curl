#ifndef HEADER_CURL_SETUP_H
#define HEADER_CURL_SETUP_H
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

#എങ്കിൽ defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#define CURL_NO_OLDIES
#endif

/* Set default _WIN32_WINNT */
#ifdef __MINGW32__
#അന്തര്ഭാവ <_mingw.h>
#endif

/* Workaround for Homebrew gcc 12.4.0, 13.3.0, 14.1.0, 14.2.0 (initial build)
   that started advertising the `availability` attribute, which then gets used
   by Apple SDK, but, in a way incompatible with gcc, resulting in misc errors
   inside SDK headers, e.g.:
     error: attributes should be specified before the declarator in a function
            definition
     error: expected ',' or '}' before
   Followed by missing declarations.
   Work it around by overriding the built-in feature-check macro used by the
   headers to enable the problematic attributes. This makes the feature check
   fail. Fixed in 14.2.0_1. Disable the workaround if the fix is detected. */
#എങ്കിൽ defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  എങ്കിൽ !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#endif

#ifdef __APPLE__
#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <TargetConditionals.h>
/* Fixup faulty target macro initialization in macOS SDK since v14.4 (as of
   15.0 beta). The SDK target detection in `TargetConditionals.h` correctly
   detects macOS, but fails to set the macro's old name `TARGET_OS_OSX`, then
   continues to set it to a default value of 0. Other parts of the SDK still
   rely on the old name, and with this inconsistency our builds fail due to
   missing declarations. It happens when using mainline llvm older than v18.
   Later versions fixed it by predefining these target macros, avoiding the
   faulty dynamic detection. gcc is not affected (for now) because it lacks
   the necessary dynamic detection features, so the SDK falls back to
   a codepath that sets both the old and new macro to 1. */
#എങ്കിൽ defined(TARGET_OS_MAC) && TARGET_OS_MAC && \
  defined(TARGET_OS_OSX) && !TARGET_OS_OSX && \
  (!defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE) && \
  (!defined(TARGET_OS_SIMULATOR) || !TARGET_OS_SIMULATOR)
#undef TARGET_OS_OSX
#define TARGET_OS_OSX TARGET_OS_MAC
#endif
#endif

#എങ്കിൽ defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#error "mingw-w64 3.0 or greater required"
#endif

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#എങ്കിൽ defined(_MSC_VER) && (_MSC_VER < 1600)
#error "Ancient versions of Visual Studio are no longer supported due to bugs."
#endif

#ifdef _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#pragma warning(disable:4127)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#endif
#endif /* _MSC_VER */

#ifdef _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  എങ്കിൽ (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    എങ്കിൽ WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) &&  \
       !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#undef SECURITY_KERNEL
#undef SECURITY_WIN32
#define SECURITY_WIN32  /* for <sspi.h> */
#endif

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#ifdef HAVE_CONFIG_H

#അന്തര്ഭാവ "curl_config.h"

#അല്ലെങ്കിൽ /* HAVE_CONFIG_H */

#ifdef _WIN32
#  include "config-win32.h"
#endif

#ifdef macintosh
#  include "config-mac.h"
#endif

#ifdef __OS400__
#  include "config-os400.h"
#endif

#endif /* HAVE_CONFIG_H */

#ifdef _WIN32
#  എങ്കിൽ defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build target is Windows Vista (0x0600)
#  endif

#  എങ്കിൽ !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#endif

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#ifdef HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#endif

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#ifdef NEED_THREAD_SAFE
#  ifndef _THREAD_SAFE
#  define _THREAD_SAFE
#  endif
#endif

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#ifdef NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#endif

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#എങ്കിൽ defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#endif

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#ifdef HAVE_THREADS_POSIX
#  include <pthread.h>
#endif

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#ifdef CURL_DISABLE_HTTP
#  ifndef CURL_DISABLE_ALTSVC
#  define CURL_DISABLE_ALTSVC
#  endif
#  ifndef CURL_DISABLE_AWS
#  define CURL_DISABLE_AWS
#  endif
#  ifndef CURL_DISABLE_BASIC_AUTH
#  define CURL_DISABLE_BASIC_AUTH
#  endif
#  ifndef CURL_DISABLE_BEARER_AUTH
#  define CURL_DISABLE_BEARER_AUTH
#  endif
#  ifndef CURL_DISABLE_COOKIES
#  define CURL_DISABLE_COOKIES
#  endif
#  ifndef CURL_DISABLE_DOH
#  define CURL_DISABLE_DOH
#  endif
#  ifndef CURL_DISABLE_FORM_API
#  define CURL_DISABLE_FORM_API
#  endif
#  ifndef CURL_DISABLE_HEADERS_API
#  define CURL_DISABLE_HEADERS_API
#  endif
#  ifndef CURL_DISABLE_HSTS
#  define CURL_DISABLE_HSTS
#  endif
#  ifndef CURL_DISABLE_HTTPSIG
#  define CURL_DISABLE_HTTPSIG
#  endif
#  ifndef CURL_DISABLE_HTTP_AUTH
#  define CURL_DISABLE_HTTP_AUTH
#  endif
#  ifndef CURL_DISABLE_RTSP
#  define CURL_DISABLE_RTSP
#  endif
#  ifndef CURL_DISABLE_WEBSOCKETS
#  define CURL_DISABLE_WEBSOCKETS /* no WebSockets without HTTP present */
#  endif
#endif

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#ifdef __OS400__
#  include "setup-os400.h"
#endif

/*
 * VMS setup file includes some system headers.
 */
#ifdef __VMS
#  include "setup-vms.h"
#endif

/*
 * Windows setup file includes some system headers.
 */
#ifdef _WIN32
#  include "setup-win32.h"
#endif

#അന്തര്ഭാവ <curl/system.h>

#എങ്കിൽ !defined(HAVE_PIPE2) && \
  ((defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
            __MAC_OS_X_VERSION_MAX_ALLOWED >= 270000 && \
    defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
            __MAC_OS_X_VERSION_MIN_REQUIRED >= 270000) || \
   (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
            __IPHONE_OS_VERSION_MAX_ALLOWED >= 270000 && \
    defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
            __IPHONE_OS_VERSION_MIN_REQUIRED >= 270000) || \
   (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
            __TV_OS_VERSION_MAX_ALLOWED >= 270000 && \
    defined(__TV_OS_VERSION_MIN_REQUIRED) && \
            __TV_OS_VERSION_MIN_REQUIRED >= 270000) || \
   (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
            __WATCH_OS_VERSION_MAX_ALLOWED >= 270000 && \
    defined(__WATCH_OS_VERSION_MIN_REQUIRED) && \
            __WATCH_OS_VERSION_MIN_REQUIRED >= 270000) || \
   (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
            __VISION_OS_VERSION_MAX_ALLOWED >= 270000 && \
    defined(__VISION_OS_VERSION_MIN_REQUIRED) && \
            __VISION_OS_VERSION_MIN_REQUIRED >= 270000))
#define HAVE_PIPE2
#endif

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#define CURL_CONC_MACROS_(A, B) A ## B
#define CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#ifdef __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#endif

/* based on logic in "curl/mprintf.h" */
#എങ്കിൽ (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#എങ്കിൽ defined(__MINGW32__) && !defined(__clang__)
#define CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#അല്ലെങ്കിൽ
#define CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#endif
#അല്ലെങ്കിൽ
#define CURL_PRINTF(fmt, arg)
#endif

/* Override default printf mask check rules in "curl/mprintf.h" */
#define CURL_TEMP_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#എങ്കിൽ defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#define __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#endif

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#എങ്കിൽ defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  എങ്കിൽ TARGET_OS_MAC && !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#endif

#ifdef USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  എങ്കിൽ defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#endif

#ifdef USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#endif

#ifdef macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#endif

#ifdef __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    എങ്കിൽ !(defined(__NEWLIB__) || \
          (defined(__CLIB2__) && defined(__THREAD_SAFE)))
       /* disable threaded resolver with clib2 - requires newlib or clib-ts */
#      undef USE_RESOLV_THREADED
#    endif
#  endif
#  include <exec/types.h>
#  include <exec/execbase.h>
#  include <proto/exec.h>
#  include <proto/dos.h>
#  include <unistd.h>
#  എങ്കിൽ defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       പൂർണ്ണസംഖ്യ Curl_amiga_select(പൂർണ്ണസംഖ്യ nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, ഘടന timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    അല്ലെങ്കിൽ
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  അല്ലെങ്കിൽ
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#endif

/* Whether to use eventfd() */
#എങ്കിൽ defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#define USE_EVENTFD
#endif

#ifdef SO_NOSIGPIPE
#define USE_SO_NOSIGPIPE
#endif

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <assert.h>

#ifdef __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#endif

#അന്തര്ഭാവ <stdint.h>
#define HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#ifdef __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#define uint32_t unsigned പൂർണ്ണസംഖ്യ
#endif

/* Disable uintptr_t for targets known to miss it from stdint.h */
#ifdef __OS400__
#undef HAVE_UINTPTR_T
#endif

#അന്തര്ഭാവ <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#അന്തര്ഭാവ <sys/types.h>

#ifdef _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#അല്ലെങ്കിലെങ്കിൽ defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((offset_t)-1)
#അല്ലെങ്കിലെങ്കിൽ defined(__AMIGA__)
#  define curl_lseek(fd, offset, whence)  lseek(fd, (off_t)(offset), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#അല്ലെങ്കിൽ
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#endif

#ifndef SIZEOF_TIME_T
/* assume default size of time_t to be 32 bits */
#define SIZEOF_TIME_T 4
#endif

#ifndef SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  എങ്കിൽ defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  അല്ലെങ്കിൽ
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#endif

#എങ്കിൽ SIZEOF_CURL_SOCKET_T < 8
#ifdef USE_WINSOCK
#  define FMT_SOCKET_T "u"
#അല്ലെങ്കിൽ
#  define FMT_SOCKET_T "d"
#endif
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#അല്ലെങ്കിൽ
#  define FMT_SOCKET_T "qd"
#endif

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#ifndef SIZEOF_OFF_T
#  എങ്കിൽ defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    എങ്കിൽ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    എങ്കിൽ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#endif

#എങ്കിൽ SIZEOF_CURL_OFF_T < 8
#error "too small curl_off_t"
#അല്ലെങ്കിൽ
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#endif
#define CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#define FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#define FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#എങ്കിൽ SIZEOF_TIME_T == 4
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX UINT_MAX
#  define TIME_T_MIN 0
#  അല്ലെങ്കിൽ
#  define TIME_T_MAX INT_MAX
#  define TIME_T_MIN INT_MIN
#  endif
#അല്ലെങ്കിൽ
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX 0xFFFFFFFFFFFFFFFF
#  define TIME_T_MIN 0
#  അല്ലെങ്കിൽ
#  define TIME_T_MAX 0x7FFFFFFFFFFFFFFF
#  define TIME_T_MIN (-TIME_T_MAX - 1)
#  endif
#endif

#ifndef SIZE_MAX
/* some limits.h headers have this defined, some do not */
#എങ്കിൽ defined(SIZEOF_SIZE_T) && (SIZEOF_SIZE_T > 4)
#define SIZE_MAX 18446744073709551615U
#അല്ലെങ്കിൽ
#define SIZE_MAX 4294967295U
#endif
#endif

#ifndef SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#എങ്കിൽ defined(SIZEOF_SIZE_T) && (SIZEOF_SIZE_T > 4)
#define SSIZE_MAX 9223372036854775807
#അല്ലെങ്കിൽ
#define SSIZE_MAX 2147483647
#endif
#endif

#എങ്കിൽ SIZEOF_LONG > SIZEOF_SIZE_T
#error "unexpected: 'long' is larger than 'size_t'"
#endif

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#ifndef GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 പൂർണ്ണസംഖ്യ
#  അല്ലെങ്കിൽ
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#endif

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#ifdef _WIN32

#  define DIR_CHAR      "\\"

#അല്ലെങ്കിൽ /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (അക്ഷരം *)(z))
#    include <tcp.h>
#    undef word
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     ബാഹ്യ ഘടന tm *gmtime_r(സ്ഥിരം time_t * സ്ഥിരം timep, ഘടന tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#endif /* _WIN32 */

/* We want to use mutex when available. */
#എങ്കിൽ defined(HAVE_THREADS_POSIX) || defined(_WIN32)
#define USE_MUTEX
#endif

/* threaded resolver is the only feature requiring threads. */
#ifdef USE_RESOLV_THREADED
#define USE_THREADS
#endif

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#എങ്കിൽ defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#അല്ലെങ്കിലെങ്കിൽ defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#അല്ലെങ്കിൽ
#  define CURLRES_IPV4
#endif

#ifdef USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#അല്ലെങ്കിലെങ്കിൽ defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#അല്ലെങ്കിൽ
#  define CURLRES_SYNCH
#endif

/* ---------------------------------------------------------------- */

#എങ്കിൽ defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#define USE_LIBIDN2
#endif

#എങ്കിൽ defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#error "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#endif

#എങ്കിൽ defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#define USE_SSL    /* SSL support has been enabled */
#endif

#എങ്കിൽ defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#ifndef OPENSSL_COEXIST
#define OPENSSL_COEXIST
#endif
#endif

#എങ്കിൽ defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#define NO_OLD_WC_NAMES
#endif

/* Single point where USE_SPNEGO definition might be defined */
#എങ്കിൽ !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#define USE_SPNEGO
#endif

/* Single point where USE_KERBEROS5 definition might be defined */
#എങ്കിൽ !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#define USE_KERBEROS5
#endif

/* Single point where USE_NTLM definition might be defined */
#ifdef CURL_ENABLE_NTLM
#  എങ്കിൽ (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  എങ്കിൽ defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#endif

#എങ്കിൽ defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#define USE_SSH
#endif

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#എങ്കിൽ defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#define CURL_HAVE_DIAG
#endif

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#എങ്കിൽ defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#അല്ലെങ്കിലെങ്കിൽ defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#അല്ലെങ്കിൽ
#  define WARN_UNUSED_RESULT
#endif

/* noreturn attribute */

#ifndef CURL_NORETURN
#എങ്കിൽ (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#അല്ലെങ്കിലെങ്കിൽ defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#അല്ലെങ്കിൽ
#  define CURL_NORETURN
#endif
#endif

/* fallthrough attribute */

#ifndef FALLTHROUGH
#എങ്കിൽ (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#അല്ലെങ്കിൽ
#  define FALLTHROUGH()  ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif
#endif

/*
 * Inclusion of common header files.
 */

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <errno.h>

#അന്തര്ഭാവ <sys/stat.h>

#എങ്കിൽ !defined(_WIN32) || defined(__MINGW32__)
#അന്തര്ഭാവ <sys/time.h>
#endif

#ifdef HAVE_IO_H
#അന്തര്ഭാവ <io.h>
#endif

#ifdef HAVE_FCNTL_H
#അന്തര്ഭാവ <fcntl.h>
#endif

#എങ്കിൽ defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#അന്തര്ഭാവ <stdbool.h>
#endif

#ifdef HAVE_UNISTD_H
#അന്തര്ഭാവ <unistd.h>
#endif

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#ifdef HAVE_UINTPTR_T
#define CURL_UNCONST(p) ((ശൂന്യം *)(uintptr_t)(സ്ഥിരം ശൂന്യം *)(p))
#അല്ലെങ്കിൽ
#define CURL_UNCONST(p) ((ശൂന്യം *)(p))  /* Fall back to simple cast */
#endif

#ifdef USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#endif

#ifdef __hpux
#  എങ്കിൽ !defined(_XOPEN_SOURCE_EXTENDED) || defined(_KERNEL)
#    ifdef _APP32_64BIT_OFF_T
#      define OLD_APP32_64BIT_OFF_T _APP32_64BIT_OFF_T
#      undef _APP32_64BIT_OFF_T
#    അല്ലെങ്കിൽ
#      undef OLD_APP32_64BIT_OFF_T
#    endif
#  endif
#endif

#ifndef _WIN32
#അന്തര്ഭാവ <sys/socket.h>  /* also for MSG_NOSIGNAL */
#endif

#അന്തര്ഭാവ "functypes.h"

#ifdef __hpux
#  എങ്കിൽ !defined(_XOPEN_SOURCE_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64BIT_OFF_T
#      define _APP32_64BIT_OFF_T OLD_APP32_64BIT_OFF_T
#      undef OLD_APP32_64BIT_OFF_T
#    endif
#  endif
#endif

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#ifndef HAVE_STRUCT_TIMEVAL
ഘടന timeval {
  long tv_sec;
  long tv_usec;
};
#endif

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#ifdef MSG_NOSIGNAL
#define SEND_4TH_ARG MSG_NOSIGNAL
#അല്ലെങ്കിൽ
#define SEND_4TH_ARG 0
#endif

#ifdef __minix
/* Minix does not support recv on TCP sockets */
#define sread(x, y, z) (ssize_t)പഠ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_RECV)
/*
 * The definitions for the return type and arguments types
 * of functions recv() and send() belong and come from the
 * configuration file. Do not define them in any other place.
 *
 * HAVE_RECV is defined if you have a function named recv()
 * which is used to read incoming data from sockets. If your
 * function has another name then do not define HAVE_RECV.
 *
 * If HAVE_RECV is defined then RECV_TYPE_ARG1, RECV_TYPE_ARG2,
 * RECV_TYPE_ARG3, RECV_TYPE_ARG4 and RECV_TYPE_RETV must also
 * be defined.
 *
 * HAVE_SEND is defined if you have a function named send()
 * which is used to write outgoing data on a connected socket.
 * If yours has another name then do not define HAVE_SEND.
 *
 * If HAVE_SEND is defined then SEND_TYPE_ARG1, SEND_TYPE_ARG2,
 * SEND_TYPE_ARG3, SEND_TYPE_ARG4 and SEND_TYPE_RETV must also
 * be defined. SEND_NONCONST_ARG2 must also be defined if ARG2
 * does not accept const.
 */

#define sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#അല്ലെങ്കിൽ /* HAVE_RECV */
#ifndef sread
#error "Missing definition of macro sread"
#endif
#endif /* HAVE_RECV */

#ifdef __minix
/* Minix does not support send on TCP sockets */
#define swrite(x, y, z) (ssize_t)ലിഖ((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SEND)
#ifdef SEND_NONCONST_ARG2
#define swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#അല്ലെങ്കിൽ
#define swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (സ്ഥിരം SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#endif /* SEND_NONCONST_ARG2 */
#അല്ലെങ്കിൽ /* HAVE_SEND */
#ifndef swrite
#error "Missing definition of macro swrite"
#endif
#endif /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#ifdef HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#അല്ലെങ്കിലെങ്കിൽ defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#അല്ലെങ്കിലെങ്കിൽ defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#അല്ലെങ്കിൽ
#  define CURL_SCLOSE(x)  close(x)
#endif

/*
 * Stack-independent version of fcntl() on sockets:
 */
#ifdef USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#അല്ലെങ്കിൽ
#  define sfcntl  fcntl
#endif

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#എങ്കിൽ defined(__hpux) && !defined(HAVE_BOOL_T)
തരനിർവചനം പൂർണ്ണസംഖ്യ bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#endif

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#ifndef HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
തരനിർവചനം എണ്ണൽ {
  bool_false = 0,
  bool_true = 1
} bool;

/*
 * Use a define to let 'true' and 'false' use those enums. There
 * are currently no use of true and false in libcurl proper, but
 * there are some in the examples. This will cater for any later
 * code happening to use true and false.
 */
#  define false bool_false
#  define true  bool_true
#  define HAVE_BOOL_T
#endif

/* the type we use for storing a single boolean bit */
തരനിർവചനം unsigned പൂർണ്ണസംഖ്യ curl_bit;
#define BIT(x) curl_bit x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

#അന്തര്ഭാവ "curl_ctype.h"

#എങ്കിൽ defined(DEBUGBUILD) && defined(NDEBUG)
#error "Debug-enabled builds cannot be combined with NDEBUG"
#endif

/*
 * Macro used to include code only in debug builds.
 */
#ifdef DEBUGBUILD
#define DEBUGF(x) x
#അല്ലെങ്കിൽ
#define DEBUGF(x) ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif

/*
 * Macro used to include assertion code only in debug builds.
 */
#undef DEBUGASSERT
#ifdef DEBUGBUILD
#ifdef CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#define DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#അല്ലെങ്കിൽ
#define DEBUGASSERT(x) പ്രതിജ്ഞാ(x)
#endif
#അല്ലെങ്കിൽ
#define DEBUGASSERT(x) ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#എങ്കിൽ defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#അന്തര്ഭാവ <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#define CURL_MSVC_CRTDBG_INIT()                                             \
  ചെയ്യുക {                                                                      \
    _set_error_mode(_OUT_TO_STDERR);  /* needs stdlib.h */                  \
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); \
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);  \
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);   \
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } ആകുമ്പോൾ(0)
#അല്ലെങ്കിൽ
#define CURL_MSVC_CRTDBG_INIT() ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif

#എങ്കിൽ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#അന്തര്ഭാവ <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#define CURL_DUMP_MODULE_PATHS(printf_f)                               \
  ചെയ്യുക {                                                                 \
    HANDLE hnd = INVALID_HANDLE_മൂല്യം;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dwSize = വലുപ്പം(MODULEENTRY32);                                \
                                                                       \
    ചെയ്യുക {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } ആകുമ്പോൾ(hnd == INVALID_HANDLE_മൂല്യം &&                             \
            GetLastError() == ERROR_BAD_LENGTH);                       \
                                                                       \
    എങ്കിൽ(hnd == INVALID_HANDLE_മൂല്യം)                                    \
      പോകുക error;                                                      \
                                                                       \
    എങ്കിൽ(!Module32First(hnd, &mod))                                      \
      പോകുക error;                                                      \
                                                                       \
    ചെയ്യുക {                                                               \
      അക്ഷരം buffer[വലുപ്പം(mod.szExePath) * 2];                          \
      അക്ഷരം *path; /* points to stack allocated buffer */               \
      എങ്കിൽ(വലുപ്പം(*mod.szExePath) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        എങ്കിൽ(!WideCharToMultiByte(CP_ACP, 0, (WCHAR *)mod.szExePath, -1, \
                                buffer, വലുപ്പം(buffer), ശൂന്യ, ശൂന്യ))   \
          പോകുക error;                                                  \
        path = (അക്ഷരം *)buffer;                                         \
      }                                                                \
      അല്ലെങ്കിൽ                                                             \
        path = (അക്ഷരം *)mod.szExePath;                                  \
      (printf_f)("%s\n", path);                                        \
    } ആകുമ്പോൾ(Module32Next(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    എങ്കിൽ(hnd != INVALID_HANDLE_മൂല്യം)                                    \
      CloseHandle(hnd);                                                \
  } ആകുമ്പോൾ(0)
#അല്ലെങ്കിൽ
#define CURL_DUMP_MODULE_PATHS(printf_f) ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#ifdef USE_WINSOCK
#define SOCKERRNO         ((പൂർണ്ണസംഖ്യ)WSAGetLastError())
#define SET_SOCKERRNO(x)  WSASetLastError((പൂർണ്ണസംഖ്യ)(x))
#അല്ലെങ്കിൽ
#define SOCKERRNO         errno
#define SET_SOCKERRNO(x)  (errno = (x))
#endif

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#ifdef USE_WINSOCK
#define SOCKEACCES        WSAEACCES
#define SOCKEADDRINUSE    WSAEADDRINUSE
#define SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#define SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#define SOCKEBADF         WSAEBADF
#define SOCKECONNREFUSED  WSAECONNREFUSED
#define SOCKECONNRESET    WSAECONNRESET
#define SOCKEINPROGRESS   WSAEINPROGRESS
#define SOCKEINTR         WSAEINTR
#define SOCKEINVAL        WSAEINVAL
#define SOCKEISCONN       WSAEISCONN
#define SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#define SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#define SOCKETIMEDOUT     WSAETIMEDOUT
#define SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#അല്ലെങ്കിൽ
#define SOCKEACCES        EACCES
#define SOCKEADDRINUSE    EADDRINUSE
#define SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#define SOCKEAFNOSUPPORT  EAFNOSUPPORT
#define SOCKEBADF         EBADF
#define SOCKECONNREFUSED  ECONNREFUSED
#define SOCKECONNRESET    ECONNRESET
#define SOCKEINPROGRESS   EINPROGRESS
#define SOCKEINTR         EINTR
#define SOCKEINVAL        EINVAL
#define SOCKEISCONN       EISCONN
#define SOCKEMSGSIZE      EMSGSIZE
#define SOCKENOMEM        ENOMEM
#ifdef ETIMEDOUT
#define SOCKETIMEDOUT     ETIMEDOUT
#endif
#define SOCKEWOULDBLOCK   EWOULDBLOCK
#endif

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#എങ്കിൽ !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#define SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#അല്ലെങ്കിൽ
#define SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#endif

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#ifdef __VMS
#define argv_item_t  __char_ptr32
#അല്ലെങ്കിലെങ്കിൽ defined(_UNICODE)
#define argv_item_t  wchar_t *
#അല്ലെങ്കിൽ
#define argv_item_t  അക്ഷരം *
#endif

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#define ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#അന്തര്ഭാവ "curlx/warnless.h"

#ifdef _WIN32
#  undef പഠ
#  define പഠ(fd, buf, എണ്ണം)  (ssize_t)_read(fd, buf, curlx_uztoui(എണ്ണം))
#  undef ലിഖ
#  define ലിഖ(fd, buf, എണ്ണം) (ssize_t)_write(fd, buf, curlx_uztoui(എണ്ണം))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#endif

/*
 * Definition of our NOP statement Object-like macro
 */
#ifndef Curl_nop_stmt
#define Curl_nop_stmt ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#എങ്കിൽ defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  എങ്കിൽ defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#endif

/*
 * shutdown() flags for systems that do not define them
 */
#ifndef SHUT_RD
#define SHUT_RD 0x00
#endif

#ifndef SHUT_WR
#define SHUT_WR 0x01
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR 0x02
#endif

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#എങ്കിൽ !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#എങ്കിൽ !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

/* For MSVC (all versions as of VS2022) */
#ifndef STDIN_FILENO
#define STDIN_FILENO  fileno(stdin)
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO  fileno(stdout)
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO  fileno(stderr)
#endif

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#ifdef O_ദ്വിമാന  /* for _WIN32 || MSDOS */
#define CURL_O_ദ്വിമാന O_ദ്വിമാന
#അല്ലെങ്കിലെങ്കിൽ defined(O_ദ്വിമാന_2)  /* __CYGWIN__ */
#define CURL_O_ദ്വിമാന O_ദ്വിമാന_2
#അല്ലെങ്കിൽ
#define CURL_O_ദ്വിമാന 0
#endif

/* Requires io.h when available */
#ifdef MSDOS
#define CURL_BINMODE(stream) (ശൂന്യം)setmode(fileno(stream), CURL_O_ദ്വിമാന)
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32) || defined(__CYGWIN__)
#define CURL_BINMODE(stream) (ശൂന്യം)_setmode(fileno(stream), CURL_O_ദ്വിമാന)
#അല്ലെങ്കിൽ
#define CURL_BINMODE(stream) (ശൂന്യം)(stream)
#endif

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#എങ്കിൽ defined(_WIN32) || defined(MSDOS)
#define FOPEN_READTEXT   "rt"
#define FOPEN_WRITETEXT  "wt"
#define FOPEN_APPENDTEXT "at"
#അല്ലെങ്കിലെങ്കിൽ defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#define FOPEN_READTEXT   "rt"
#define FOPEN_WRITETEXT  "w"
#define FOPEN_APPENDTEXT "a"
#അല്ലെങ്കിൽ
#define FOPEN_READTEXT   "r"
#define FOPEN_WRITETEXT  "w"
#define FOPEN_APPENDTEXT "a"
#endif

/* for systems that do not detect this in configure */
#ifndef CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T ADDRESS_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T unsigned അക്ഷരം
#  അല്ലെങ്കിൽ
/* use a sensible default */
#    define CURL_SA_FAMILY_T unsigned short
#  endif
#endif

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#define CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#define CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#define CURL_CSTRLEN(x) (വലുപ്പം(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#define STRCONST(x) x, CURL_CSTRLEN(x)

#define CURL_ARRAYSIZE(A) (വലുപ്പം(A) / വലുപ്പം((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#define STRERROR_LEN 256

#ifndef CURL_DID_MEMORY_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
തരനിർവചനം ശൂന്യം *(*curl_malloc_callback)(size_t size);
തരനിർവചനം ശൂന്യം (*curl_free_callback)(ശൂന്യം *ptr);
തരനിർവചനം ശൂന്യം *(*curl_realloc_callback)(ശൂന്യം *ptr, size_t size);
തരനിർവചനം അക്ഷരം *(*curl_strdup_callback)(സ്ഥിരം അക്ഷരം *str);
തരനിർവചനം ശൂന്യം *(*curl_calloc_callback)(size_t nmemb, size_t size);
#define CURL_DID_MEMORY_FUNC_TYPEDEFS
#endif

ബാഹ്യ curl_malloc_callback Curl_cmalloc;
ബാഹ്യ curl_free_callback Curl_cfree;
ബാഹ്യ curl_realloc_callback Curl_crealloc;
ബാഹ്യ curl_strdup_callback Curl_cstrdup;
ബാഹ്യ curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#define curlx_safefree(ptr) \
  ചെയ്യുക {                      \
    curlx_free(ptr);        \
    (ptr) = ശൂന്യ;           \
  } ആകുമ്പോൾ(0)

#അന്തര്ഭാവ <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#ifdef DEBUGBUILD
#define CURL_MEMDEBUG
#endif

#ifdef CURL_MEMDEBUG
#ifdef __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  എങ്കിൽ __clang_major__ >= 4
#  define ALLOC_SIZE(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  അല്ലെങ്കിൽ
#  define ALLOC_SIZE(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#അല്ലെങ്കിലെങ്കിൽ defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_SIZE(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#അല്ലെങ്കിലെങ്കിൽ defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(restrict)
#  define ALLOC_SIZE(s)
#  define ALLOC_SIZE2(n, s)
#അല്ലെങ്കിൽ
#  define ALLOC_FUNC
#  define ALLOC_SIZE(s)
#  define ALLOC_SIZE2(n, s)
#endif

ബാഹ്യ FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN ശൂന്യം curl_dbg_free(ശൂന്യം *ptr, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE(1)
  ശൂന്യം *curl_dbg_malloc(size_t wantedsize, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  ശൂന്യം *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                        പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_SIZE(2)
  ശൂന്യം *curl_dbg_realloc(ശൂന്യം *ptr, size_t wantedsize, പൂർണ്ണസംഖ്യ വരി,
                         സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_FUNC
  അക്ഷരം *curl_dbg_strdup(സ്ഥിരം അക്ഷരം *str, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
#എങ്കിൽ defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(സ്ഥിരം wchar_t *str, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
#endif

CURL_EXTERN ശൂന്യം curl_dbg_memdebug(സ്ഥിരം അക്ഷരം *logname);
CURL_EXTERN ശൂന്യം curl_dbg_memlimit(long limit);
CURL_EXTERN ശൂന്യം curl_dbg_log(സ്ഥിരം അക്ഷരം *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(പൂർണ്ണസംഖ്യ domain, പൂർണ്ണസംഖ്യ type, പൂർണ്ണസംഖ്യ protocol,
                                          പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ശൂന്യം curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN പൂർണ്ണസംഖ്യ curl_dbg_sclose(curl_socket_t sockfd,
                                പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          ശൂന്യം *saddr, ശൂന്യം *saddrlen,
                                          പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
#ifdef HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, ശൂന്യം *saddr,
                                           ശൂന്യം *saddrlen, പൂർണ്ണസംഖ്യ flags,
                                           പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
#endif
#ifdef HAVE_SOCKETPAIR
CURL_EXTERN പൂർണ്ണസംഖ്യ curl_dbg_socketpair(പൂർണ്ണസംഖ്യ domain, പൂർണ്ണസംഖ്യ type, പൂർണ്ണസംഖ്യ protocol,
                                    curl_socket_t socket_vector[2],
                                    പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
#endif

/* FILE functions */
CURL_EXTERN പൂർണ്ണസംഖ്യ curl_dbg_fclose(FILE *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(സ്ഥിരം അക്ഷരം *file, സ്ഥിരം അക്ഷരം *mode,
                                            പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(സ്ഥിരം അക്ഷരം *file,
                                              സ്ഥിരം അക്ഷരം *mode, FILE *fh,
                                              പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(പൂർണ്ണസംഖ്യ filedes, സ്ഥിരം അക്ഷരം *mode,
                                             പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source);

#define sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#define fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#define CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#define CURL_FREEADDRINFO(data) \
  curl_dbg_freeaddrinfo(data, __LINE__, __FILE__)
#define CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((പൂർണ്ണസംഖ്യ)(domain), type, protocol, __LINE__, __FILE__)
#ifdef HAVE_SOCKETPAIR
#define CURL_SOCKETPAIR(domain, type, protocol, socket_vector) \
  curl_dbg_socketpair((പൂർണ്ണസംഖ്യ)(domain), type, protocol, socket_vector, \
                      __LINE__, __FILE__)
#endif
#define CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#ifdef HAVE_ACCEPT4
#define CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#endif

#അല്ലെങ്കിൽ /* !CURL_MEMDEBUG */

#define sclose(x) CURL_SCLOSE(x)
#define fake_sclose(x) Curl_nop_stmt

#define CURL_GETADDRINFO getaddrinfo
#define CURL_FREEADDRINFO freeaddrinfo
#define CURL_SOCKET socket
#ifdef HAVE_SOCKETPAIR
#define CURL_SOCKETPAIR socketpair
#endif
#define CURL_ACCEPT accept
#ifdef HAVE_ACCEPT4
#define CURL_ACCEPT4 accept4
#endif

#endif /* CURL_MEMDEBUG */

/* Allocator macros */

#ifdef _WIN32
#define CURLX_STRDUP_താഴെയുള്ള _strdup
#അല്ലെങ്കിൽ
#define CURLX_STRDUP_താഴെയുള്ള strdup
#endif

#ifdef CURL_MEMDEBUG

#define curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#define curlx_malloc(size)         curl_dbg_malloc(size, __LINE__, __FILE__)
#define curlx_calloc(nbelem, size) \
  curl_dbg_calloc(nbelem, size, __LINE__, __FILE__)
#define curlx_realloc(ptr, size) \
  curl_dbg_realloc(ptr, size, __LINE__, __FILE__)
#define curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#ifdef _WIN32
#ifdef UNICODE
#define curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#അല്ലെങ്കിൽ
#define curlx_tcsdup               curlx_strdup
#endif
#endif /* _WIN32 */

#അല്ലെങ്കിൽ /* !CURL_MEMDEBUG */

#ifdef BUILDING_LIBCURL
#define curlx_strdup               Curl_cstrdup
#define curlx_malloc               Curl_cmalloc
#define curlx_calloc               Curl_ccalloc
#define curlx_realloc              Curl_crealloc
#define curlx_free                 Curl_cfree
#അല്ലെങ്കിൽ /* !BUILDING_LIBCURL */
#define curlx_strdup               CURLX_STRDUP_താഴെയുള്ള
#define curlx_malloc               malloc
#define curlx_calloc               calloc
#define curlx_realloc              realloc
#define curlx_free                 സ്വതന്ത്രം
#endif /* BUILDING_LIBCURL */

#ifdef _WIN32
#ifdef UNICODE
#define curlx_tcsdup               curlx_wcsdup
#അല്ലെങ്കിൽ
#define curlx_tcsdup               curlx_strdup
#endif
#endif /* _WIN32 */

#endif /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#എങ്കിൽ defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
ഘടന passwd;
പൂർണ്ണസംഖ്യ getpwuid_r(uid_t uid, ഘടന passwd *pwd, അക്ഷരം *buf,
               size_t buflen, ഘടന passwd **result);
#endif

#ifdef UNITTESTS
#define UNITTEST
#അല്ലെങ്കിൽ
#define UNITTEST സ്ഥിതം
#endif

#ifdef USE_NGHTTP2
#define USE_HTTP2
#endif

#എങ്കിൽ (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#ifdef CURL_WITH_MULTI_SSL
#error "MultiSSL combined with QUIC is not supported"
#endif

#define USE_HTTP3
#endif

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#ifndef __EMSCRIPTEN__
#define CURL_TCP_NODELAY_SUPPORTED
#endif

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#എങ്കിൽ defined(HAVE_BASENAME) && defined(_WIN32)
#undef HAVE_BASENAME
#endif

#എങ്കിൽ defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#എങ്കിൽ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#അല്ലെങ്കിലെങ്കിൽ !defined(UNIX_PATH_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_PATH_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
തരനിർവചനം ഘടന sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  അക്ഷരം sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#endif
#endif

#ifdef USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_DEFINES_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_DEFINES_WARNING
#    endif
#  endif
#endif

#ifdef CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#അല്ലെങ്കിലെങ്കിൽ defined(വരിയിൽ)
#  define CURL_INLINE വരിയിൽ /* 'inline' defined, assumed correct */
#അല്ലെങ്കിലെങ്കിൽ defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE വരിയിൽ /* 'inline' keyword supported */
#അല്ലെങ്കിലെങ്കിൽ defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE വരിയിൽ /* 'inline' keyword supported */
#അല്ലെങ്കിലെങ്കിൽ defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#അല്ലെങ്കിലെങ്കിൽ defined(_MSC_VER)
#  define CURL_INLINE __inline
#അല്ലെങ്കിൽ
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /* empty */
#endif

/* Detect if compiler supports C99 variadic macros */
#എങ്കിൽ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#define CURL_HAVE_MACRO_VARARG
#endif

#എങ്കിൽ !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS))
#define CURLVERBOSE
#define VERBOSE(x) x
#define NOVERBOSE(x) Curl_nop_stmt
#അല്ലെങ്കിൽ
#define VERBOSE(x) Curl_nop_stmt
#define NOVERBOSE(x) x
#endif

/* For FreeBSD it is included from curl/curl.h */
#എങ്കിൽ defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#അന്തര്ഭാവ <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#endif

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#എങ്കിൽ defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#define CURL_USE_SOCK_NONBLOCK
#endif

#ifndef _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#ifdef _WIN32
#എങ്കിൽ defined(_MSC_VER) && defined(NTDDI_VERSION) && \
  (NTDDI_VERSION >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#pragma comment(lib, "volatileaccessu.lib")
#define curlx_memzero_താഴെയുള്ള(buf, size)  SecureZeroMemory2(buf, size)
#അല്ലെങ്കിൽ
#define curlx_memzero_താഴെയുള്ള(buf, size)  SecureZeroMemory(buf, size)
#endif
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_MEMSET_S)
#define curlx_memzero_താഴെയുള്ള(buf, size)  (ശൂന്യം)memset_s(buf, size, 0, size)
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_MEMSET_EXPLICIT)
#define curlx_memzero_താഴെയുള്ള(buf, size)  (ശൂന്യം)memset_explicit(buf, 0, size)
#അല്ലെങ്കിലെങ്കിൽ defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#define curlx_memzero_താഴെയുള്ള(buf, size)  explicit_bzero(buf, size)
#അല്ലെങ്കിലെങ്കിൽ defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#define curlx_memzero_താഴെയുള്ള(buf, size)  (ശൂന്യം)explicit_memset(buf, 0, size)
#endif
#endif /* !_CURL_LOCAL_MEMZERO */

#ifndef curlx_memzero_താഴെയുള്ള
#define USE_CURLX_MEMZERO
ശൂന്യം curlx_memzero_താഴെയുള്ള(ശൂന്യം *buf, size_t size);
#endif
ശൂന്യം curlx_memzero(ശൂന്യം *buf, size_t size);
ശൂന്യം curlx_strzero(ശൂന്യം *buf);

#endif /* HEADER_CURL_SETUP_H */
