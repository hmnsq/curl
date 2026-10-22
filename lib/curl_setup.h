#यदि_अपरिभाषितम् शीर्षकम्_CURL_सज्जता_H
#परिभाषय शीर्षकम्_CURL_सज्जता_H
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

#यदि defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#परिभाषय CURL_NO_OLDIES
#यदि_अन्तः

/* Set default _WIN32_WINNT */
#यदि_परिभाषितम् __MINGW32__
#अन्तर्भाव <_mingw.h>
#यदि_अन्तः

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
#यदि defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  यदि !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् __APPLE__
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <TargetConditionals.h>
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
#यदि defined(TARGET_OS_MAC) && TARGET_OS_MAC && \
  defined(TARGET_OS_OSX) && !TARGET_OS_OSX && \
  (!defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE) && \
  (!defined(TARGET_OS_SIMULATOR) || !TARGET_OS_SIMULATOR)
#अपरिभाषय TARGET_OS_OSX
#परिभाषय TARGET_OS_OSX TARGET_OS_MAC
#यदि_अन्तः
#यदि_अन्तः

#यदि defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#दोषः "mingw-w64 3.0 or greater required"
#यदि_अन्तः

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#यदि defined(_MSC_VER) && (_MSC_VER < 1600)
#दोषः "Ancient versions of Visual Studio are no longer supported due to bugs."
#यदि_अन्तः

#यदि_परिभाषितम् _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#निर्देशः warning(disable:4127)
#यदि_अपरिभाषितम् _CRT_SECURE_NO_WARNINGS
#परिभाषय _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#यदि_अन्तः
#यदि_अन्तः /* _MSC_VER */

#यदि_परिभाषितम् _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_माध्यम्
#  define WIN32_LEAN_AND_माध्यम्
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  यदि (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    यदि WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) &&  \
       !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#अपरिभाषय SECURITY_KERNEL
#अपरिभाषय SECURITY_WIN32
#परिभाषय SECURITY_WIN32  /* for <sspi.h> */
#यदि_अन्तः

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#यदि_परिभाषितम् HAVE_CONFIG_H

#अन्तर्भाव "curl_config.h"

#अन्यथा /* HAVE_CONFIG_H */

#यदि_परिभाषितम् _WIN32
#  include "config-win32.h"
#यदि_अन्तः

#यदि_परिभाषितम् macintosh
#  include "config-mac.h"
#यदि_अन्तः

#यदि_परिभाषितम् __OS400__
#  include "config-os400.h"
#यदि_अन्तः

#यदि_अन्तः /* HAVE_CONFIG_H */

#यदि_परिभाषितम् _WIN32
#  यदि defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build target is Windows Vista (0x0600)
#  endif

#  यदि !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#यदि_अन्तः

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#यदि_परिभाषितम् HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#यदि_अन्तः

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#यदि_परिभाषितम् NEED_THREAD_SAFE
#  ifndef _THREAD_SAFE
#  define _THREAD_SAFE
#  endif
#यदि_अन्तः

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#यदि_परिभाषितम् NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#यदि_अन्तः

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#यदि defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#यदि_अन्तः

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#यदि_परिभाषितम् HAVE_तन्तवः_POSIX
#  include <pthread.h>
#यदि_अन्तः

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#यदि_परिभाषितम् CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_शीर्षकाणि_API
#  define CURL_DISABLE_शीर्षकाणि_API
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
#यदि_अन्तः

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#यदि_परिभाषितम् __OS400__
#  include "setup-os400.h"
#यदि_अन्तः

/*
 * VMS setup file includes some system headers.
 */
#यदि_परिभाषितम् __VMS
#  include "setup-vms.h"
#यदि_अन्तः

/*
 * Windows setup file includes some system headers.
 */
#यदि_परिभाषितम् _WIN32
#  include "setup-win32.h"
#यदि_अन्तः

#अन्तर्भाव <curl/system.h>

#यदि !defined(HAVE_PIPE2) && \
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
#परिभाषय HAVE_PIPE2
#यदि_अन्तः

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#परिभाषय CURL_CONC_MACROS_(A, B) A ## B
#परिभाषय CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#यदि_परिभाषितम् __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#यदि_अन्तः

/* based on logic in "curl/mprintf.h" */
#यदि (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#यदि defined(__MINGW32__) && !defined(__clang__)
#परिभाषय CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#अन्यथा
#परिभाषय CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#यदि_अन्तः
#अन्यथा
#परिभाषय CURL_PRINTF(fmt, arg)
#यदि_अन्तः

/* Override default printf mask check rules in "curl/mprintf.h" */
#परिभाषय CURL_अस्थायी_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#यदि defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#परिभाषय __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#यदि_अन्तः

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#यदि defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  यदि TARGET_OS_MAC && !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  यदि defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#यदि_अन्तः

#यदि_परिभाषितम् macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#यदि_अन्तः

#यदि_परिभाषितम् __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    यदि !(defined(__NEWLIB__) || \
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
#  यदि defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       पूर्णाङ्क Curl_amiga_select(पूर्णाङ्क nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, संरचना timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    अन्यथा
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  अन्यथा
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#यदि_अन्तः

/* Whether to use eventfd() */
#यदि defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#परिभाषय USE_EVENTFD
#यदि_अन्तः

#यदि_परिभाषितम् SO_NOSIGPIPE
#परिभाषय USE_SO_NOSIGPIPE
#यदि_अन्तः

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <assert.h>

#यदि_परिभाषितम् __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#यदि_अन्तः

#अन्तर्भाव <stdint.h>
#परिभाषय HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#यदि_परिभाषितम् __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#परिभाषय uint32_t अचिह्नित पूर्णाङ्क
#यदि_अन्तः

/* Disable uintptr_t for targets known to miss it from stdint.h */
#यदि_परिभाषितम् __OS400__
#अपरिभाषय HAVE_UINTPTR_T
#यदि_अन्तः

#अन्तर्भाव <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#अन्तर्भाव <sys/types.h>

#यदि_परिभाषितम् _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#नोचेद्यदि defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((विस्थापनम्_t)-1)
#नोचेद्यदि defined(__AMIGA__)
#  define curl_lseek(fd, विस्थापनम्, whence)  lseek(fd, (off_t)(विस्थापनम्), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#अन्यथा
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#यदि_अन्तः

#यदि_अपरिभाषितम् SIZEOF_TIME_T
/* assume default size of time_t to be 32 bits */
#परिभाषय SIZEOF_TIME_T 4
#यदि_अन्तः

#यदि_अपरिभाषितम् SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  यदि defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  अन्यथा
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#यदि_अन्तः

#यदि SIZEOF_CURL_SOCKET_T < 8
#यदि_परिभाषितम् USE_WINSOCK
#  define FMT_SOCKET_T "u"
#अन्यथा
#  define FMT_SOCKET_T "d"
#यदि_अन्तः
#नोचेद्यदि defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#अन्यथा
#  define FMT_SOCKET_T "qd"
#यदि_अन्तः

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#यदि_अपरिभाषितम् SIZEOF_OFF_T
#  यदि defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    यदि defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    यदि defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#यदि_अन्तः

#यदि SIZEOF_CURL_OFF_T < 8
#दोषः "too small curl_off_t"
#अन्यथा
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#यदि_अन्तः
#परिभाषय CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#परिभाषय FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#परिभाषय FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#यदि SIZEOF_TIME_T == 4
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX UINT_MAX
#  define TIME_T_MIN 0
#  अन्यथा
#  define TIME_T_MAX अधिकतम_पूर्णाङ्कः
#  define TIME_T_MIN INT_MIN
#  endif
#अन्यथा
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX 0xFFFFFFFFFFFFFFFF
#  define TIME_T_MIN 0
#  अन्यथा
#  define TIME_T_MAX 0x7FFFFFFFFFFFFFFF
#  define TIME_T_MIN (-TIME_T_MAX - 1)
#  endif
#यदि_अन्तः

#यदि_अपरिभाषितम् SIZE_MAX
/* some limits.h headers have this defined, some do not */
#यदि defined(SIZEOF_आकार_T) && (SIZEOF_आकार_T > 4)
#परिभाषय SIZE_MAX 18446744073709551615U
#अन्यथा
#परिभाषय SIZE_MAX 4294967295U
#यदि_अन्तः
#यदि_अन्तः

#यदि_अपरिभाषितम् SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#यदि defined(SIZEOF_आकार_T) && (SIZEOF_आकार_T > 4)
#परिभाषय SSIZE_MAX 9223372036854775807
#अन्यथा
#परिभाषय SSIZE_MAX 2147483647
#यदि_अन्तः
#यदि_अन्तः

#यदि SIZEOF_LONG > SIZEOF_आकार_T
#दोषः "unexpected: 'long' is larger than 'size_t'"
#यदि_अन्तः

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#यदि_अपरिभाषितम् GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 पूर्णाङ्क
#  अन्यथा
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#यदि_अन्तः

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#यदि_परिभाषितम् _WIN32

#  define DIR_CHAR      "\\"

#अन्यथा /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (वर्ण *)(z))
#    include <tcp.h>
#    undef word
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     बाह्य संरचना tm *gmtime_r(ध्रुव time_t * ध्रुव timep, संरचना tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#यदि_अन्तः /* _WIN32 */

/* We want to use mutex when available. */
#यदि defined(HAVE_तन्तवः_POSIX) || defined(_WIN32)
#परिभाषय USE_MUTEX
#यदि_अन्तः

/* threaded resolver is the only feature requiring threads. */
#यदि_परिभाषितम् USE_RESOLV_THREADED
#परिभाषय USE_तन्तवः
#यदि_अन्तः

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#यदि defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#नोचेद्यदि defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#अन्यथा
#  define CURLRES_IPV4
#यदि_अन्तः

#यदि_परिभाषितम् USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#नोचेद्यदि defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#अन्यथा
#  define CURLRES_SYNCH
#यदि_अन्तः

/* ---------------------------------------------------------------- */

#यदि defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#परिभाषय USE_LIBIDN2
#यदि_अन्तः

#यदि defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#दोषः "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#यदि_अन्तः

#यदि defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#परिभाषय USE_SSL    /* SSL support has been enabled */
#यदि_अन्तः

#यदि defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#यदि_अपरिभाषितम् OPENSSL_COEXIST
#परिभाषय OPENSSL_COEXIST
#यदि_अन्तः
#यदि_अन्तः

#यदि defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#परिभाषय NO_OLD_WC_नामानि
#यदि_अन्तः

/* Single point where USE_SPNEGO definition might be defined */
#यदि !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#परिभाषय USE_SPNEGO
#यदि_अन्तः

/* Single point where USE_KERBEROS5 definition might be defined */
#यदि !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#परिभाषय USE_KERBEROS5
#यदि_अन्तः

/* Single point where USE_NTLM definition might be defined */
#यदि_परिभाषितम् CURL_ENABLE_NTLM
#  यदि (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  यदि defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#यदि_अन्तः

#यदि defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#परिभाषय USE_SSH
#यदि_अन्तः

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#यदि defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#परिभाषय CURL_HAVE_DIAG
#यदि_अन्तः

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#यदि defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_अप्रयुक्तम्_परिणाम __attribute__((warn_अप्रयुक्तम्_परिणाम))
#नोचेद्यदि defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_अप्रयुक्तम्_परिणाम __attribute__((warn_अप्रयुक्तम्_परिणाम))
#अन्यथा
#  define WARN_अप्रयुक्तम्_परिणाम
#यदि_अन्तः

/* noreturn attribute */

#यदि_अपरिभाषितम् CURL_NORETURN
#यदि (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#नोचेद्यदि defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#अन्यथा
#  define CURL_NORETURN
#यदि_अन्तः
#यदि_अन्तः

/* fallthrough attribute */

#यदि_अपरिभाषितम् FALLTHROUGH
#यदि (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#अन्यथा
#  define FALLTHROUGH()  कुरु {} यावत्(0)
#यदि_अन्तः
#यदि_अन्तः

/*
 * Inclusion of common header files.
 */

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव <errno.h>

#अन्तर्भाव <sys/stat.h>

#यदि !defined(_WIN32) || defined(__MINGW32__)
#अन्तर्भाव <sys/time.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_IO_H
#अन्तर्भाव <io.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>
#यदि_अन्तः

#यदि defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#अन्तर्भाव <stdbool.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_UNISTD_H
#अन्तर्भाव <unistd.h>
#यदि_अन्तः

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#यदि_परिभाषितम् HAVE_UINTPTR_T
#परिभाषय CURL_UNCONST(p) ((शून्य *)(uintptr_t)(ध्रुव शून्य *)(p))
#अन्यथा
#परिभाषय CURL_UNCONST(p) ((शून्य *)(p))  /* Fall back to simple cast */
#यदि_अन्तः

#यदि_परिभाषितम् USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#यदि_अन्तः

#यदि_परिभाषितम् __hpux
#  यदि !defined(_XOPEN_SOURCE_EXTENDED) || defined(_KERNEL)
#    ifdef _APP32_64BIT_OFF_T
#      define OLD_APP32_64BIT_OFF_T _APP32_64BIT_OFF_T
#      undef _APP32_64BIT_OFF_T
#    अन्यथा
#      undef OLD_APP32_64BIT_OFF_T
#    endif
#  endif
#यदि_अन्तः

#यदि_अपरिभाषितम् _WIN32
#अन्तर्भाव <sys/socket.h>  /* also for MSG_NOSIGNAL */
#यदि_अन्तः

#अन्तर्भाव "functypes.h"

#यदि_परिभाषितम् __hpux
#  यदि !defined(_XOPEN_SOURCE_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64BIT_OFF_T
#      define _APP32_64BIT_OFF_T OLD_APP32_64BIT_OFF_T
#      undef OLD_APP32_64BIT_OFF_T
#    endif
#  endif
#यदि_अन्तः

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#यदि_अपरिभाषितम् HAVE_STRUCT_TIMEVAL
संरचना timeval {
  दीर्घ tv_sec;
  दीर्घ tv_usec;
};
#यदि_अन्तः

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#यदि_परिभाषितम् MSG_NOSIGNAL
#परिभाषय SEND_4TH_ARG MSG_NOSIGNAL
#अन्यथा
#परिभाषय SEND_4TH_ARG 0
#यदि_अन्तः

#यदि_परिभाषितम् __minix
/* Minix does not support recv on TCP sockets */
#परिभाषय sread(x, y, z) (ssize_t)पठ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#नोचेद्यदि defined(HAVE_RECV)
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

#परिभाषय sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#अन्यथा /* HAVE_RECV */
#यदि_अपरिभाषितम् sread
#दोषः "Missing definition of macro sread"
#यदि_अन्तः
#यदि_अन्तः /* HAVE_RECV */

#यदि_परिभाषितम् __minix
/* Minix does not support send on TCP sockets */
#परिभाषय swrite(x, y, z) (ssize_t)लिख((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#नोचेद्यदि defined(HAVE_SEND)
#यदि_परिभाषितम् SEND_NONCONST_ARG2
#परिभाषय swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#अन्यथा
#परिभाषय swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ध्रुव SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#यदि_अन्तः /* SEND_NONCONST_ARG2 */
#अन्यथा /* HAVE_SEND */
#यदि_अपरिभाषितम् swrite
#दोषः "Missing definition of macro swrite"
#यदि_अन्तः
#यदि_अन्तः /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#यदि_परिभाषितम् HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#नोचेद्यदि defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#नोचेद्यदि defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#नोचेद्यदि defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#अन्यथा
#  define CURL_SCLOSE(x)  close(x)
#यदि_अन्तः

/*
 * Stack-independent version of fcntl() on sockets:
 */
#यदि_परिभाषितम् USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#अन्यथा
#  define sfcntl  fcntl
#यदि_अन्तः

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#यदि defined(__hpux) && !defined(HAVE_BOOL_T)
प्रकारनिर्देश पूर्णाङ्क bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#यदि_अन्तः

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#यदि_अपरिभाषितम् HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
प्रकारनिर्देश गणना {
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
#यदि_अन्तः

/* the type we use for storing a single boolean bit */
प्रकारनिर्देश अचिह्नित पूर्णाङ्क curl_bit;
#परिभाषय BIT(x) curl_bit x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#यदि_अपरिभाषितम् TRUE
#परिभाषय TRUE true
#यदि_अन्तः
#यदि_अपरिभाषितम् FALSE
#परिभाषय FALSE false
#यदि_अन्तः

#अन्तर्भाव "curl_ctype.h"

#यदि defined(DEBUGBUILD) && defined(NDEBUG)
#दोषः "Debug-enabled builds cannot be combined with NDEBUG"
#यदि_अन्तः

/*
 * Macro used to include code only in debug builds.
 */
#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय DEBUGF(x) x
#अन्यथा
#परिभाषय DEBUGF(x) कुरु {} यावत्(0)
#यदि_अन्तः

/*
 * Macro used to include assertion code only in debug builds.
 */
#अपरिभाषय DEBUGASSERT
#यदि_परिभाषितम् DEBUGBUILD
#यदि_परिभाषितम् CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#परिभाषय DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#अन्यथा
#परिभाषय DEBUGASSERT(x) प्रतिज्ञा(x)
#यदि_अन्तः
#अन्यथा
#परिभाषय DEBUGASSERT(x) कुरु {} यावत्(0)
#यदि_अन्तः

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#यदि defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#अन्तर्भाव <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#परिभाषय CURL_MSVC_CRTDBG_INIT()                                             \
  कुरु {                                                                      \
    समुच्चयः_error_बहुलकम्(बहिर्गमनम्_गन्तव्यम्_STDERR);  /* needs stdlib.h */                  \
    Crt_समुच्चयः_Report_बहुलकम्(_CRT_ASSERT, CRTDBG_बहुलकम्_FILE | CRTDBG_बहुलकम्_DEBUG); \
    Crt_समुच्चयः_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_समुच्चयः_Report_बहुलकम्(_CRT_ERROR, CRTDBG_बहुलकम्_FILE | CRTDBG_बहुलकम्_DEBUG);  \
    Crt_समुच्चयः_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_समुच्चयः_Report_बहुलकम्(_CRT_WARN, CRTDBG_बहुलकम्_FILE | CRTDBG_बहुलकम्_DEBUG);   \
    Crt_समुच्चयः_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } यावत्(0)
#अन्यथा
#परिभाषय CURL_MSVC_CRTDBG_INIT() कुरु {} यावत्(0)
#यदि_अन्तः

#यदि defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#अन्तर्भाव <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#परिभाषय CURL_DUMP_MODULE_PATHS(printf_f)                               \
  कुरु {                                                                 \
    HANDLE hnd = INVALID_HANDLE_मानम्;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_आकार = परिमाण(MODULEENTRY32);                                \
                                                                       \
    कुरु {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } यावत्(hnd == INVALID_HANDLE_मानम् &&                             \
            Get_चरमः_Error() == ERROR_BAD_दीर्घता);                       \
                                                                       \
    यदि(hnd == INVALID_HANDLE_मानम्)                                    \
      गच्छ error;                                                      \
                                                                       \
    यदि(!Module32First(hnd, &mod))                                      \
      गच्छ error;                                                      \
                                                                       \
    कुरु {                                                               \
      वर्ण बफर[परिमाण(mod.sz_Exe_मार्गः) * 2];                          \
      वर्ण *मार्गः; /* points to stack allocated buffer */               \
      यदि(परिमाण(*mod.sz_Exe_मार्गः) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        यदि(!Wide_Char_गन्तव्यम्_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_मार्गः, -1, \
                                बफर, परिमाण(बफर), रिक्त, रिक्त))   \
          गच्छ error;                                                  \
        मार्गः = (वर्ण *)बफर;                                         \
      }                                                                \
      अन्यथा                                                             \
        मार्गः = (वर्ण *)mod.sz_Exe_मार्गः;                                  \
      (printf_f)("%s\n", मार्गः);                                        \
    } यावत्(Module32_अग्र(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    यदि(hnd != INVALID_HANDLE_मानम्)                                    \
      CloseHandle(hnd);                                                \
  } यावत्(0)
#अन्यथा
#परिभाषय CURL_DUMP_MODULE_PATHS(printf_f) कुरु {} यावत्(0)
#यदि_अन्तः

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#यदि_परिभाषितम् USE_WINSOCK
#परिभाषय SOCKERRNO         ((पूर्णाङ्क)WSAGet_चरमः_Error())
#परिभाषय समुच्चयः_SOCKERRNO(x)  WSASet_चरमः_Error((पूर्णाङ्क)(x))
#अन्यथा
#परिभाषय SOCKERRNO         errno
#परिभाषय समुच्चयः_SOCKERRNO(x)  (errno = (x))
#यदि_अन्तः

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#यदि_परिभाषितम् USE_WINSOCK
#परिभाषय SOCKEACCES        WSAEACCES
#परिभाषय SOCKEADDRINUSE    WSAEADDRINUSE
#परिभाषय SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#परिभाषय SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#परिभाषय SOCKEBADF         WSAEBADF
#परिभाषय SOCKECONNREFUSED  WSAECONNREFUSED
#परिभाषय SOCKECONNRESET    WSAECONNRESET
#परिभाषय SOCKEINPROGRESS   WSAEINPROGRESS
#परिभाषय SOCKEINTR         WSAEINTR
#परिभाषय SOCKEINVAL        WSAEINVAL
#परिभाषय SOCKEISCONN       WSAEISCONN
#परिभाषय SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#परिभाषय SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#परिभाषय SOCKETIMEDOUT     WSAETIMEDOUT
#परिभाषय SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#अन्यथा
#परिभाषय SOCKEACCES        EACCES
#परिभाषय SOCKEADDRINUSE    EADDRINUSE
#परिभाषय SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#परिभाषय SOCKEAFNOSUPPORT  EAFNOSUPPORT
#परिभाषय SOCKEBADF         EBADF
#परिभाषय SOCKECONNREFUSED  ECONNREFUSED
#परिभाषय SOCKECONNRESET    ECONNRESET
#परिभाषय SOCKEINPROGRESS   EINPROGRESS
#परिभाषय SOCKEINTR         EINTR
#परिभाषय SOCKEINVAL        EINVAL
#परिभाषय SOCKEISCONN       EISCONN
#परिभाषय SOCKEMSGSIZE      EMSGSIZE
#परिभाषय SOCKENOMEM        ENOMEM
#यदि_परिभाषितम् ETIMEDOUT
#परिभाषय SOCKETIMEDOUT     ETIMEDOUT
#यदि_अन्तः
#परिभाषय SOCKEWOULDBLOCK   EWOULDBLOCK
#यदि_अन्तः

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#यदि !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#परिभाषय SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#अन्यथा
#परिभाषय SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#यदि_अन्तः

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#यदि_परिभाषितम् __VMS
#परिभाषय argv_item_t  __char_ptr32
#नोचेद्यदि defined(_UNICODE)
#परिभाषय argv_item_t  wchar_t *
#अन्यथा
#परिभाषय argv_item_t  वर्ण *
#यदि_अन्तः

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#परिभाषय ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#अन्तर्भाव "curlx/warnless.h"

#यदि_परिभाषितम् _WIN32
#  undef पठ
#  define पठ(fd, buf, गणनम्)  (ssize_t)_read(fd, buf, curlx_uztoui(गणनम्))
#  undef लिख
#  define लिख(fd, buf, गणनम्) (ssize_t)_write(fd, buf, curlx_uztoui(गणनम्))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#यदि_अन्तः

/*
 * Definition of our NOP statement Object-like macro
 */
#यदि_अपरिभाषितम् Curl_nop_stmt
#परिभाषय Curl_nop_stmt कुरु {} यावत्(0)
#यदि_अन्तः

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#यदि defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  यदि defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#यदि_अन्तः

/*
 * shutdown() flags for systems that do not define them
 */
#यदि_अपरिभाषितम् SHUT_RD
#परिभाषय SHUT_RD 0x00
#यदि_अन्तः

#यदि_अपरिभाषितम् SHUT_WR
#परिभाषय SHUT_WR 0x01
#यदि_अन्तः

#यदि_अपरिभाषितम् SHUT_RDWR
#परिभाषय SHUT_RDWR 0x02
#यदि_अन्तः

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#यदि !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#परिभाषय S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#यदि_अन्तः

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#यदि !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#परिभाषय S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#यदि_अन्तः

/* For MSVC (all versions as of VS2022) */
#यदि_अपरिभाषितम् STDIN_FILENO
#परिभाषय STDIN_FILENO  fileno(stdin)
#यदि_अन्तः
#यदि_अपरिभाषितम् STDOUT_FILENO
#परिभाषय STDOUT_FILENO  fileno(stdout)
#यदि_अन्तः
#यदि_अपरिभाषितम् STDERR_FILENO
#परिभाषय STDERR_FILENO  fileno(stderr)
#यदि_अन्तः

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#यदि_परिभाषितम् O_द्विमान  /* for _WIN32 || MSDOS */
#परिभाषय CURL_O_द्विमान O_द्विमान
#नोचेद्यदि defined(O_द्विमान_2)  /* __CYGWIN__ */
#परिभाषय CURL_O_द्विमान O_द्विमान_2
#अन्यथा
#परिभाषय CURL_O_द्विमान 0
#यदि_अन्तः

/* Requires io.h when available */
#यदि_परिभाषितम् MSDOS
#परिभाषय CURL_BINMODE(प्रवाहः) (शून्य)setmode(fileno(प्रवाहः), CURL_O_द्विमान)
#नोचेद्यदि defined(_WIN32) || defined(__CYGWIN__)
#परिभाषय CURL_BINMODE(प्रवाहः) (शून्य)_setmode(fileno(प्रवाहः), CURL_O_द्विमान)
#अन्यथा
#परिभाषय CURL_BINMODE(प्रवाहः) (शून्य)(प्रवाहः)
#यदि_अन्तः

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#यदि defined(_WIN32) || defined(MSDOS)
#परिभाषय FOPEN_READTEXT   "rt"
#परिभाषय FOPEN_WRITETEXT  "wt"
#परिभाषय FOPEN_APPENDTEXT "at"
#नोचेद्यदि defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#परिभाषय FOPEN_READTEXT   "rt"
#परिभाषय FOPEN_WRITETEXT  "w"
#परिभाषय FOPEN_APPENDTEXT "a"
#अन्यथा
#परिभाषय FOPEN_READTEXT   "r"
#परिभाषय FOPEN_WRITETEXT  "w"
#परिभाषय FOPEN_APPENDTEXT "a"
#यदि_अन्तः

/* for systems that do not detect this in configure */
#यदि_अपरिभाषितम् CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T स्थानाङ्कः_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T अचिह्नित वर्ण
#  अन्यथा
/* use a sensible default */
#    define CURL_SA_FAMILY_T अचिह्नित ह्रस्व
#  endif
#यदि_अन्तः

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#परिभाषय CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#परिभाषय CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#परिभाषय CURL_CSTRLEN(x) (परिमाण(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#परिभाषय STRCONST(x) x, CURL_CSTRLEN(x)

#परिभाषय CURL_ARRAYSIZE(A) (परिमाण(A) / परिमाण((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#परिभाषय STRERROR_LEN 256

#यदि_अपरिभाषितम् CURL_DID_स्मृतिः_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
प्रकारनिर्देश शून्य *(*curl_malloc_callback)(size_t आकार);
प्रकारनिर्देश शून्य (*curl_free_callback)(शून्य *ptr);
प्रकारनिर्देश शून्य *(*curl_realloc_callback)(शून्य *ptr, size_t आकार);
प्रकारनिर्देश वर्ण *(*curl_strdup_callback)(ध्रुव वर्ण *str);
प्रकारनिर्देश शून्य *(*curl_calloc_callback)(size_t nmemb, size_t आकार);
#परिभाषय CURL_DID_स्मृतिः_FUNC_TYPEDEFS
#यदि_अन्तः

बाह्य curl_malloc_callback Curl_cmalloc;
बाह्य curl_free_callback Curl_cfree;
बाह्य curl_realloc_callback Curl_crealloc;
बाह्य curl_strdup_callback Curl_cstrdup;
बाह्य curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#परिभाषय curlx_safefree(ptr) \
  कुरु {                      \
    curlx_free(ptr);        \
    (ptr) = रिक्त;           \
  } यावत्(0)

#अन्तर्भाव <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय CURL_MEMDEBUG
#यदि_अन्तः

#यदि_परिभाषितम् CURL_MEMDEBUG
#यदि_परिभाषितम् __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  यदि __clang_major__ >= 4
#  define ALLOC_आकार(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  अन्यथा
#  define ALLOC_आकार(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#नोचेद्यदि defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_आकार(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#नोचेद्यदि defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(सीमित)
#  define ALLOC_आकार(s)
#  define ALLOC_SIZE2(n, s)
#अन्यथा
#  define ALLOC_FUNC
#  define ALLOC_आकार(s)
#  define ALLOC_SIZE2(n, s)
#यदि_अन्तः

बाह्य FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN शून्य curl_dbg_free(शून्य *ptr, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_FUNC ALLOC_आकार(1)
  शून्य *curl_dbg_malloc(size_t wantedsize, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  शून्य *curl_dbg_calloc(size_t wanted_elements, size_t wanted_आकार,
                        पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_आकार(2)
  शून्य *curl_dbg_realloc(शून्य *ptr, size_t wantedsize, पूर्णाङ्क पङ्क्तिः,
                         ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_FUNC
  वर्ण *curl_dbg_strdup(ध्रुव वर्ण *str, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
#यदि defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ध्रुव wchar_t *str, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
#यदि_अन्तः

CURL_EXTERN शून्य curl_dbg_memdebug(ध्रुव वर्ण *logname);
CURL_EXTERN शून्य curl_dbg_memlimit(दीर्घ सीमा);
CURL_EXTERN शून्य curl_dbg_log(ध्रुव वर्ण *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(पूर्णाङ्क domain, पूर्णाङ्क type, पूर्णाङ्क protocol,
                                          पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN शून्य curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN पूर्णाङ्क curl_dbg_sclose(curl_socket_t sockfd,
                                पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          शून्य *saddr, शून्य *saddrlen,
                                          पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
#यदि_परिभाषितम् HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, शून्य *saddr,
                                           शून्य *saddrlen, पूर्णाङ्क flags,
                                           पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SOCKETPAIR
CURL_EXTERN पूर्णाङ्क curl_dbg_socketpair(पूर्णाङ्क domain, पूर्णाङ्क type, पूर्णाङ्क protocol,
                                    curl_socket_t socket_सदिशम्[2],
                                    पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
#यदि_अन्तः

/* FILE functions */
CURL_EXTERN पूर्णाङ्क curl_dbg_fclose(FILE *file, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ध्रुव वर्ण *file, ध्रुव वर्ण *बहुलकम्,
                                            पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ध्रुव वर्ण *file,
                                              ध्रुव वर्ण *बहुलकम्, FILE *fh,
                                              पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(पूर्णाङ्क filedes, ध्रुव वर्ण *बहुलकम्,
                                             पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source);

#परिभाषय sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#परिभाषय fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#परिभाषय CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#परिभाषय CURL_FREEADDRINFO(दत्त) \
  curl_dbg_freeaddrinfo(दत्त, __LINE__, __FILE__)
#परिभाषय CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((पूर्णाङ्क)(domain), type, protocol, __LINE__, __FILE__)
#यदि_परिभाषितम् HAVE_SOCKETPAIR
#परिभाषय CURL_SOCKETPAIR(domain, type, protocol, socket_सदिशम्) \
  curl_dbg_socketpair((पूर्णाङ्क)(domain), type, protocol, socket_सदिशम्, \
                      __LINE__, __FILE__)
#यदि_अन्तः
#परिभाषय CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#यदि_परिभाषितम् HAVE_ACCEPT4
#परिभाषय CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#यदि_अन्तः

#अन्यथा /* !CURL_MEMDEBUG */

#परिभाषय sclose(x) CURL_SCLOSE(x)
#परिभाषय fake_sclose(x) Curl_nop_stmt

#परिभाषय CURL_GETADDRINFO getaddrinfo
#परिभाषय CURL_FREEADDRINFO freeaddrinfo
#परिभाषय CURL_SOCKET socket
#यदि_परिभाषितम् HAVE_SOCKETPAIR
#परिभाषय CURL_SOCKETPAIR socketpair
#यदि_अन्तः
#परिभाषय CURL_ACCEPT accept
#यदि_परिभाषितम् HAVE_ACCEPT4
#परिभाषय CURL_ACCEPT4 accept4
#यदि_अन्तः

#यदि_अन्तः /* CURL_MEMDEBUG */

/* Allocator macros */

#यदि_परिभाषितम् _WIN32
#परिभाषय CURLX_STRDUP_नीचः _strdup
#अन्यथा
#परिभाषय CURLX_STRDUP_नीचः strdup
#यदि_अन्तः

#यदि_परिभाषितम् CURL_MEMDEBUG

#परिभाषय curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#परिभाषय curlx_malloc(आकार)         curl_dbg_malloc(आकार, __LINE__, __FILE__)
#परिभाषय curlx_calloc(nbelem, आकार) \
  curl_dbg_calloc(nbelem, आकार, __LINE__, __FILE__)
#परिभाषय curlx_realloc(ptr, आकार) \
  curl_dbg_realloc(ptr, आकार, __LINE__, __FILE__)
#परिभाषय curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#यदि_परिभाषितम् _WIN32
#यदि_परिभाषितम् UNICODE
#परिभाषय curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#अन्यथा
#परिभाषय curlx_tcsdup               curlx_strdup
#यदि_अन्तः
#यदि_अन्तः /* _WIN32 */

#अन्यथा /* !CURL_MEMDEBUG */

#यदि_परिभाषितम् BUILDING_LIBCURL
#परिभाषय curlx_strdup               Curl_cstrdup
#परिभाषय curlx_malloc               Curl_cmalloc
#परिभाषय curlx_calloc               Curl_ccalloc
#परिभाषय curlx_realloc              Curl_crealloc
#परिभाषय curlx_free                 Curl_cfree
#अन्यथा /* !BUILDING_LIBCURL */
#परिभाषय curlx_strdup               CURLX_STRDUP_नीचः
#परिभाषय curlx_malloc               स्मृतिं_देहि
#परिभाषय curlx_calloc               शून्यस्मृतिं_देहि
#परिभाषय curlx_realloc              realloc
#परिभाषय curlx_free                 मुक्त
#यदि_अन्तः /* BUILDING_LIBCURL */

#यदि_परिभाषितम् _WIN32
#यदि_परिभाषितम् UNICODE
#परिभाषय curlx_tcsdup               curlx_wcsdup
#अन्यथा
#परिभाषय curlx_tcsdup               curlx_strdup
#यदि_अन्तः
#यदि_अन्तः /* _WIN32 */

#यदि_अन्तः /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#यदि defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
संरचना passwd;
पूर्णाङ्क getpwuid_r(uid_t uid, संरचना passwd *pwd, वर्ण *buf,
               size_t buflen, संरचना passwd **परिणाम);
#यदि_अन्तः

#यदि_परिभाषितम् UNITTESTS
#परिभाषय UNITTEST
#अन्यथा
#परिभाषय UNITTEST स्थिर
#यदि_अन्तः

#यदि_परिभाषितम् USE_NGHTTP2
#परिभाषय USE_HTTP2
#यदि_अन्तः

#यदि (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
#दोषः "MultiSSL combined with QUIC is not supported"
#यदि_अन्तः

#परिभाषय USE_HTTP3
#यदि_अन्तः

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#यदि_अपरिभाषितम् __EMSCRIPTEN__
#परिभाषय CURL_TCP_NODELAY_SUPPORTED
#यदि_अन्तः

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#यदि defined(HAVE_BASENAME) && defined(_WIN32)
#अपरिभाषय HAVE_BASENAME
#यदि_अन्तः

#यदि defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#नोचेद्यदि !defined(UNIX_मार्गः_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_मार्गः_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  वर्ण sun_मार्गः[UNIX_मार्गः_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_परिभाषते_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_परिभाषते_WARNING
#    endif
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#नोचेद्यदि defined(अन्तःस्थ)
#  define CURL_INLINE अन्तःस्थ /* 'inline' defined, assumed correct */
#नोचेद्यदि defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE अन्तःस्थ /* 'inline' keyword supported */
#नोचेद्यदि defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE अन्तःस्थ /* 'inline' keyword supported */
#नोचेद्यदि defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#नोचेद्यदि defined(_MSC_VER)
#  define CURL_INLINE __inline
#अन्यथा
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /*रिक्तानि*/
#यदि_अन्तः

/* Detect if compiler supports C99 variadic macros */
#यदि (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#परिभाषय CURL_HAVE_MACRO_VARARG
#यदि_अन्तः

#यदि !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_वाक्यानि))
#परिभाषय CURLVERBOSE
#परिभाषय VERBOSE(x) x
#परिभाषय NOVERBOSE(x) Curl_nop_stmt
#अन्यथा
#परिभाषय VERBOSE(x) Curl_nop_stmt
#परिभाषय NOVERBOSE(x) x
#यदि_अन्तः

/* For FreeBSD it is included from curl/curl.h */
#यदि defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#अन्तर्भाव <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#यदि_अन्तः

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#यदि defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#परिभाषय CURL_USE_SOCK_NONBLOCK
#यदि_अन्तः

#यदि_अपरिभाषितम् _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#यदि_परिभाषितम् _WIN32
#यदि defined(_MSC_VER) && defined(NTDDI_संस्करणम्) && \
  (NTDDI_संस्करणम् >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#निर्देशः comment(lib, "volatileaccessu.lib")
#परिभाषय curlx_memzero_नीचः(buf, आकार)  SecureZeroMemory2(buf, आकार)
#अन्यथा
#परिभाषय curlx_memzero_नीचः(buf, आकार)  Secure_Zero_स्मृतिः(buf, आकार)
#यदि_अन्तः
#नोचेद्यदि defined(HAVE_MEMSET_S)
#परिभाषय curlx_memzero_नीचः(buf, आकार)  (शून्य)memset_s(buf, आकार, 0, आकार)
#नोचेद्यदि defined(HAVE_MEMSET_EXPLICIT)
#परिभाषय curlx_memzero_नीचः(buf, आकार)  (शून्य)memset_explicit(buf, 0, आकार)
#नोचेद्यदि defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#परिभाषय curlx_memzero_नीचः(buf, आकार)  explicit_bzero(buf, आकार)
#नोचेद्यदि defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#परिभाषय curlx_memzero_नीचः(buf, आकार)  (शून्य)explicit_memset(buf, 0, आकार)
#यदि_अन्तः
#यदि_अन्तः /* !_CURL_LOCAL_MEMZERO */

#यदि_अपरिभाषितम् curlx_memzero_नीचः
#परिभाषय USE_CURLX_MEMZERO
शून्य curlx_memzero_नीचः(शून्य *buf, size_t आकार);
#यदि_अन्तः
शून्य curlx_memzero(शून्य *buf, size_t आकार);
शून्य curlx_strzero(शून्य *buf);

#यदि_अन्तः /* HEADER_CURL_SETUP_H */
