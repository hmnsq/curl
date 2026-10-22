#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_H
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

#ਜੇ defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NO_OLDIES
#ਜੇ_ਅੰਤ

/* Set default _WIN32_WINNT */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
#ਅਨ੍ਤਰ੍ਭਾਵ <_mingw.h>
#ਜੇ_ਅੰਤ

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
#ਜੇ defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  ਜੇ !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __APPLE__
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <TargetConditionals.h>
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
#ਜੇ defined(ਨਿਸ਼ਾਨਾ_OS_MAC) && ਨਿਸ਼ਾਨਾ_OS_MAC && \
  defined(ਨਿਸ਼ਾਨਾ_OS_OSX) && !ਨਿਸ਼ਾਨਾ_OS_OSX && \
  (!defined(ਨਿਸ਼ਾਨਾ_OS_IPHONE) || !ਨਿਸ਼ਾਨਾ_OS_IPHONE) && \
  (!defined(ਨਿਸ਼ਾਨਾ_OS_SIMULATOR) || !ਨਿਸ਼ਾਨਾ_OS_SIMULATOR)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਨਿਸ਼ਾਨਾ_OS_OSX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਸ਼ਾਨਾ_OS_OSX ਨਿਸ਼ਾਨਾ_OS_MAC
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#ਗਲਤੀ "mingw-w64 3.0 or greater required"
#ਜੇ_ਅੰਤ

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#ਜੇ defined(_MSC_VER) && (_MSC_VER < 1600)
#ਗਲਤੀ "Ancient versions of Visual Studio are no longer supported due to bugs."
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#ਹਦਾਇਤ warning(disable:4127)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _CRT_SECURE_NO_WARNINGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* _MSC_VER */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_ਔਸਤ
#  define WIN32_LEAN_AND_ਔਸਤ
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  ਜੇ (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    ਜੇ WINAPI_FAMILY_ਹਿੱਸੇ_ਕਰੋ(WINAPI_ਹਿੱਸੇ_ਕਰੋ_APP) &&  \
       !WINAPI_FAMILY_ਹਿੱਸੇ_ਕਰੋ(WINAPI_ਹਿੱਸੇ_ਕਰੋ_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SECURITY_KERNEL
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SECURITY_WIN32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECURITY_WIN32  /* for <sspi.h> */
#ਜੇ_ਅੰਤ

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_config.h"

#ਨਹੀਂਤਾਂ /* HAVE_CONFIG_H */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#  include "config-win32.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ macintosh
#  include "config-mac.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __OS400__
#  include "config-os400.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HAVE_CONFIG_H */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#  ਜੇ defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build ਨਿਸ਼ਾਨਾ is Windows Vista (0x0600)
#  endif

#  ਜੇ !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#ਜੇ_ਅੰਤ

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#ਜੇ_ਅੰਤ

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NEED_THREAD_ਸੁਰੱਖਿਅਤ
#  ifndef THREAD_ਸੁਰੱਖਿਅਤ
#  define THREAD_ਸੁਰੱਖਿਅਤ
#  endif
#ਜੇ_ਅੰਤ

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#ਜੇ_ਅੰਤ

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#ਜੇ defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#ਜੇ_ਅੰਤ

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਧਾਗੇ_POSIX
#  include <pthread.h>
#ਜੇ_ਅੰਤ

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_ਸਿਰਲੇਖਾਂ_API
#  define CURL_DISABLE_ਸਿਰਲੇਖਾਂ_API
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
#ਜੇ_ਅੰਤ

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __OS400__
#  include "setup-os400.h"
#ਜੇ_ਅੰਤ

/*
 * VMS setup file includes some system headers.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#  include "setup-vms.h"
#ਜੇ_ਅੰਤ

/*
 * Windows setup file includes some system headers.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#  include "setup-win32.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <curl/system.h>

#ਜੇ !defined(HAVE_PIPE2) && \
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_PIPE2
#ਜੇ_ਅੰਤ

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CONC_MACROS_(A, B) A ## B
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#ਜੇ_ਅੰਤ

/* based on logic in "curl/mprintf.h" */
#ਜੇ (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#ਜੇ defined(__MINGW32__) && !defined(__clang__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PRINTF(fmt, arg)
#ਜੇ_ਅੰਤ

/* Override default printf mask check rules in "curl/mprintf.h" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਆਰਜ਼ੀ_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#ਜੇ defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#ਜੇ_ਅੰਤ

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#ਜੇ defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  ਜੇ ਨਿਸ਼ਾਨਾ_OS_MAC && !(defined(ਨਿਸ਼ਾਨਾ_OS_IPHONE) && ਨਿਸ਼ਾਨਾ_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  ਜੇ defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    ਜੇ !(defined(__NEWLIB__) || \
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
#  ਜੇ defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       ਪੂਰਨ Curl_amiga_select(ਪੂਰਨ nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, ਬਣਤਰ timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    ਨਹੀਂਤਾਂ
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  ਨਹੀਂਤਾਂ
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#ਜੇ_ਅੰਤ

/* Whether to use eventfd() */
#ਜੇ defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_EVENTFD
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SO_NOSIGPIPE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SO_NOSIGPIPE
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uint32_t ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ
#ਜੇ_ਅੰਤ

/* Disable uintptr_t for targets known to miss it from stdint.h */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __OS400__
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ HAVE_UINTPTR_T
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#ਜਾਂਜੇ defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((ਵਿੱਥ_t)-1)
#ਜਾਂਜੇ defined(__AMIGA__)
#  define curl_lseek(fd, ਵਿੱਥ, whence)  lseek(fd, (off_t)(ਵਿੱਥ), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#ਨਹੀਂਤਾਂ
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIZEOF_ਸਮਾਂ_T
/* assume default size of time_t to be 32 bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZEOF_ਸਮਾਂ_T 4
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  ਜੇ defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  ਨਹੀਂਤਾਂ
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ SIZEOF_CURL_SOCKET_T < 8
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
#  define FMT_SOCKET_T "u"
#ਨਹੀਂਤਾਂ
#  define FMT_SOCKET_T "d"
#ਜੇ_ਅੰਤ
#ਜਾਂਜੇ defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#ਨਹੀਂਤਾਂ
#  define FMT_SOCKET_T "qd"
#ਜੇ_ਅੰਤ

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIZEOF_OFF_T
#  ਜੇ defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    ਜੇ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    ਜੇ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ SIZEOF_CURL_OFF_T < 8
#ਗਲਤੀ "too small curl_off_t"
#ਨਹੀਂਤਾਂ
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#ਜੇ SIZEOF_ਸਮਾਂ_T == 4
#  ifdef HAVE_ਸਮਾਂ_T_UNSIGNED
#  define ਸਮਾਂ_T_MAX UINT_MAX
#  define ਸਮਾਂ_T_MIN 0
#  ਨਹੀਂਤਾਂ
#  define ਸਮਾਂ_T_MAX ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
#  define ਸਮਾਂ_T_MIN INT_MIN
#  endif
#ਨਹੀਂਤਾਂ
#  ifdef HAVE_ਸਮਾਂ_T_UNSIGNED
#  define ਸਮਾਂ_T_MAX 0xFFFFFFFFFFFFFFFF
#  define ਸਮਾਂ_T_MIN 0
#  ਨਹੀਂਤਾਂ
#  define ਸਮਾਂ_T_MAX 0x7FFFFFFFFFFFFFFF
#  define ਸਮਾਂ_T_MIN (-ਸਮਾਂ_T_MAX - 1)
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIZE_MAX
/* some limits.h headers have this defined, some do not */
#ਜੇ defined(SIZEOF_ਮਾਪ_T) && (SIZEOF_ਮਾਪ_T > 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZE_MAX 18446744073709551615U
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIZE_MAX 4294967295U
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#ਜੇ defined(SIZEOF_ਮਾਪ_T) && (SIZEOF_ਮਾਪ_T > 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSIZE_MAX 9223372036854775807
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSIZE_MAX 2147483647
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ SIZEOF_LONG > SIZEOF_ਮਾਪ_T
#ਗਲਤੀ "unexpected: 'long' is larger than 'size_t'"
#ਜੇ_ਅੰਤ

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 ਪੂਰਨ
#  ਨਹੀਂਤਾਂ
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#ਜੇ_ਅੰਤ

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

#  define DIR_CHAR      "\\"

#ਨਹੀਂਤਾਂ /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (ਅੱਖਰ *)(z))
#    include <tcp.h>
#    undef ਸ਼ਬਦ
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     ਬਾਹਰੀ ਬਣਤਰ tm *gmtime_r(ਅਚਲ time_t * ਅਚਲ timep, ਬਣਤਰ tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#ਜੇ_ਅੰਤ /* _WIN32 */

/* We want to use mutex when available. */
#ਜੇ defined(HAVE_ਧਾਗੇ_POSIX) || defined(_WIN32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_MUTEX
#ਜੇ_ਅੰਤ

/* threaded resolver is the only feature requiring threads. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_ਧਾਗੇ
#ਜੇ_ਅੰਤ

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#ਜੇ defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#ਜਾਂਜੇ defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#ਨਹੀਂਤਾਂ
#  define CURLRES_IPV4
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#ਜਾਂਜੇ defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#ਨਹੀਂਤਾਂ
#  define CURLRES_SYNCH
#ਜੇ_ਅੰਤ

/* ---------------------------------------------------------------- */

#ਜੇ defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_LIBIDN2
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#ਗਲਤੀ "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SSL    /* SSL support has been enabled */
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_COEXIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENSSL_COEXIST
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_OLD_WC_ਨਾਂ
#ਜੇ_ਅੰਤ

/* Single point where USE_SPNEGO definition might be defined */
#ਜੇ !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SPNEGO
#ਜੇ_ਅੰਤ

/* Single point where USE_KERBEROS5 definition might be defined */
#ਜੇ !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_KERBEROS5
#ਜੇ_ਅੰਤ

/* Single point where USE_NTLM definition might be defined */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_ENABLE_NTLM
#  ਜੇ (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  ਜੇ defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SSH
#ਜੇ_ਅੰਤ

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#ਜੇ defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HAVE_DIAG
#ਜੇ_ਅੰਤ

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#ਜੇ defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ __attribute__((warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ))
#ਜਾਂਜੇ defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ __attribute__((warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ))
#ਨਹੀਂਤਾਂ
#  define WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ
#ਜੇ_ਅੰਤ

/* noreturn attribute */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_NORETURN
#ਜੇ (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#ਜਾਂਜੇ defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#ਨਹੀਂਤਾਂ
#  define CURL_NORETURN
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/* fallthrough attribute */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FALLTHROUGH
#ਜੇ (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#ਨਹੀਂਤਾਂ
#  define FALLTHROUGH()  ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/*
 * Inclusion of common header files.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਜੇ !defined(_WIN32) || defined(__MINGW32__)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/time.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IO_H
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNISTD_H
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ_ਅੰਤ

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UINTPTR_T
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_UNCONST(p) ((ਖਾਲੀ *)(uintptr_t)(ਅਚਲ ਖਾਲੀ *)(p))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_UNCONST(p) ((ਖਾਲੀ *)(p))  /* Fall back to simple cast */
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __hpux
#  ਜੇ !defined(XOPEN_ਸਰੋਤ_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#      define OLD_APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#      undef APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#    ਨਹੀਂਤਾਂ
#      undef OLD_APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#    endif
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>  /* also for MSG_NOSIGNAL */
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "functypes.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __hpux
#  ਜੇ !defined(XOPEN_ਸਰੋਤ_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#      define APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T OLD_APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#      undef OLD_APP32_64_ਦੋਆਧਾਰੀ_ਅੰਕ_OFF_T
#    endif
#  endif
#ਜੇ_ਅੰਤ

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_STRUCT_TIMEVAL
ਬਣਤਰ timeval {
  ਲੰਮਾ tv_sec;
  ਲੰਮਾ tv_usec;
};
#ਜੇ_ਅੰਤ

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSG_NOSIGNAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_4TH_ARG MSG_NOSIGNAL
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEND_4TH_ARG 0
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __minix
/* Minix does not support recv on TCP sockets */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sread(x, y, z) (ssize_t)ਪਠ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#ਜਾਂਜੇ defined(HAVE_RECV)
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#ਨਹੀਂਤਾਂ /* HAVE_RECV */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ sread
#ਗਲਤੀ "Missing definition of macro sread"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* HAVE_RECV */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __minix
/* Minix does not support send on TCP sockets */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ swrite(x, y, z) (ssize_t)ਲਿਖ((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#ਜਾਂਜੇ defined(HAVE_SEND)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SEND_NONCONST_ARG2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ਅਚਲ SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#ਜੇ_ਅੰਤ /* SEND_NONCONST_ARG2 */
#ਨਹੀਂਤਾਂ /* HAVE_SEND */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ swrite
#ਗਲਤੀ "Missing definition of macro swrite"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#ਜਾਂਜੇ defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#ਜਾਂਜੇ defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#ਜਾਂਜੇ defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#ਨਹੀਂਤਾਂ
#  define CURL_SCLOSE(x)  close(x)
#ਜੇ_ਅੰਤ

/*
 * Stack-independent version of fcntl() on sockets:
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#ਨਹੀਂਤਾਂ
#  define sfcntl  fcntl
#ਜੇ_ਅੰਤ

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#ਜੇ defined(__hpux) && !defined(HAVE_BOOL_T)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#ਜੇ_ਅੰਤ

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
#ਜੇ_ਅੰਤ

/* the type we use for storing a single boolean bit */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curl_ਦੋਆਧਾਰੀ_ਅੰਕ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ(x) curl_ਦੋਆਧਾਰੀ_ਅੰਕ x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRUE true
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FALSE false
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ctype.h"

#ਜੇ defined(DEBUGBUILD) && defined(NDEBUG)
#ਗਲਤੀ "Debug-enabled builds cannot be combined with NDEBUG"
#ਜੇ_ਅੰਤ

/*
 * Macro used to include code only in debug builds.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGF(x) x
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGF(x) ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/*
 * Macro used to include assertion code only in debug builds.
 */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEBUGASSERT
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGASSERT(x) ਪ੍ਰਤਿਜ੍ਞਾ(x)
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGASSERT(x) ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#ਜੇ defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#ਅਨ੍ਤਰ੍ਭਾਵ <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MSVC_CRTDBG_INIT()                                             \
  ਕਰੋ {                                                                      \
    ਸੈੱਟ_error_ਬਹੁਲਕ(ਬਾਹਰਲਾ_ਮੰਜ਼ਿਲ_STDERR);  /* needs stdlib.h */                  \
    Crt_ਸੈੱਟ_Report_ਬਹੁਲਕ(_CRT_ASSERT, CRTDBG_ਬਹੁਲਕ_FILE | CRTDBG_ਬਹੁਲਕ_DEBUG); \
    Crt_ਸੈੱਟ_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_ਸੈੱਟ_Report_ਬਹੁਲਕ(_CRT_ERROR, CRTDBG_ਬਹੁਲਕ_FILE | CRTDBG_ਬਹੁਲਕ_DEBUG);  \
    Crt_ਸੈੱਟ_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_ਸੈੱਟ_Report_ਬਹੁਲਕ(_CRT_WARN, CRTDBG_ਬਹੁਲਕ_FILE | CRTDBG_ਬਹੁਲਕ_DEBUG);   \
    Crt_ਸੈੱਟ_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MSVC_CRTDBG_INIT() ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

#ਜੇ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#ਅਨ੍ਤਰ੍ਭਾਵ <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DUMP_MODULE_PATHS(printf_f)                               \
  ਕਰੋ {                                                                 \
    HANDLE hnd = INVALID_HANDLE_ਮੁੱਲ;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_ਮਾਪ = ਆਕਾਰ(MODULEENTRY32);                                \
                                                                       \
    ਕਰੋ {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } ਜਦੋਂ(hnd == INVALID_HANDLE_ਮੁੱਲ &&                             \
            Get_ਆਖਰੀ_Error() == ERROR_BAD_ਲੰਮਾਈ);                       \
                                                                       \
    ਜੇ(hnd == INVALID_HANDLE_ਮੁੱਲ)                                    \
      ਜਾਓ error;                                                      \
                                                                       \
    ਜੇ(!Module32_ਪਹਿਲਾ(hnd, &mod))                                      \
      ਜਾਓ error;                                                      \
                                                                       \
    ਕਰੋ {                                                               \
      ਅੱਖਰ ਬਫਰ[ਆਕਾਰ(mod.sz_Exe_ਰਸਤਾ) * 2];                          \
      ਅੱਖਰ *ਰਸਤਾ; /* points to stack allocated buffer */               \
      ਜੇ(ਆਕਾਰ(*mod.sz_Exe_ਰਸਤਾ) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        ਜੇ(!Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_ਰਸਤਾ, -1, \
                                ਬਫਰ, ਆਕਾਰ(ਬਫਰ), ਨਿਰਾ, ਨਿਰਾ))   \
          ਜਾਓ error;                                                  \
        ਰਸਤਾ = (ਅੱਖਰ *)ਬਫਰ;                                         \
      }                                                                \
      ਨਹੀਂਤਾਂ                                                             \
        ਰਸਤਾ = (ਅੱਖਰ *)mod.sz_Exe_ਰਸਤਾ;                                  \
      (printf_f)("%s\n", ਰਸਤਾ);                                        \
    } ਜਦੋਂ(Module32_ਅਗਲਾ(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    ਜੇ(hnd != INVALID_HANDLE_ਮੁੱਲ)                                    \
      CloseHandle(hnd);                                                \
  } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DUMP_MODULE_PATHS(printf_f) ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKERRNO         ((ਪੂਰਨ)WSAGet_ਆਖਰੀ_Error())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_SOCKERRNO(x)  WSASet_ਆਖਰੀ_Error((ਪੂਰਨ)(x))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKERRNO         errno
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_SOCKERRNO(x)  (errno = (x))
#ਜੇ_ਅੰਤ

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEACCES        WSAEACCES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEADDRINUSE    WSAEADDRINUSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEBADF         WSAEBADF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKECONNREFUSED  WSAECONNREFUSED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKECONNRESET    WSAECONNRESET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEINPROGRESS   WSAEINPROGRESS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEINTR         WSAEINTR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEINVAL        WSAEINVAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEISCONN       WSAEISCONN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKETIMEDOUT     WSAETIMEDOUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEACCES        EACCES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEADDRINUSE    EADDRINUSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEAFNOSUPPORT  EAFNOSUPPORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEBADF         EBADF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKECONNREFUSED  ECONNREFUSED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKECONNRESET    ECONNRESET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEINPROGRESS   EINPROGRESS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEINTR         EINTR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEINVAL        EINVAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEISCONN       EISCONN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEMSGSIZE      EMSGSIZE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKENOMEM        ENOMEM
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ETIMEDOUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKETIMEDOUT     ETIMEDOUT
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKEWOULDBLOCK   EWOULDBLOCK
#ਜੇ_ਅੰਤ

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#ਜੇ !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#ਜੇ_ਅੰਤ

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ argv_ਵਸਤੂ_t  __char_ptr32
#ਜਾਂਜੇ defined(_UNICODE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ argv_ਵਸਤੂ_t  wchar_t *
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ argv_ਵਸਤੂ_t  ਅੱਖਰ *
#ਜੇ_ਅੰਤ

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/warnless.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#  undef ਪਠ
#  define ਪਠ(fd, buf, ਗਿਣਤੀ_2)  (ssize_t)_read(fd, buf, curlx_uztoui(ਗਿਣਤੀ_2))
#  undef ਲਿਖ
#  define ਲਿਖ(fd, buf, ਗਿਣਤੀ_2) (ssize_t)_write(fd, buf, curlx_uztoui(ਗਿਣਤੀ_2))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#ਜੇ_ਅੰਤ

/*
 * Definition of our NOP statement Object-like macro
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_nop_stmt ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#ਜੇ defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  ਜੇ defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#ਜੇ_ਅੰਤ

/*
 * shutdown() flags for systems that do not define them
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SHUT_RD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHUT_RD 0x00
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SHUT_WR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHUT_WR 0x01
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SHUT_RDWR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHUT_RDWR 0x02
#ਜੇ_ਅੰਤ

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#ਜੇ !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#ਜੇ_ਅੰਤ

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#ਜੇ !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#ਜੇ_ਅੰਤ

/* For MSVC (all versions as of VS2022) */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ STDIN_FILENO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STDIN_FILENO  fileno(stdin)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ STDOUT_FILENO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STDOUT_FILENO  fileno(stdout)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ STDERR_FILENO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STDERR_FILENO  fileno(stderr)
#ਜੇ_ਅੰਤ

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ O_ਦੁਆਧਾਰੀ  /* for _WIN32 || MSDOS */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_O_ਦੁਆਧਾਰੀ O_ਦੁਆਧਾਰੀ
#ਜਾਂਜੇ defined(O_ਦੁਆਧਾਰੀ_2)  /* __CYGWIN__ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_O_ਦੁਆਧਾਰੀ O_ਦੁਆਧਾਰੀ_2
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_O_ਦੁਆਧਾਰੀ 0
#ਜੇ_ਅੰਤ

/* Requires io.h when available */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSDOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_BINMODE(ਧਾਰਾ) (ਖਾਲੀ)setmode(fileno(ਧਾਰਾ), CURL_O_ਦੁਆਧਾਰੀ)
#ਜਾਂਜੇ defined(_WIN32) || defined(__CYGWIN__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_BINMODE(ਧਾਰਾ) (ਖਾਲੀ)_setmode(fileno(ਧਾਰਾ), CURL_O_ਦੁਆਧਾਰੀ)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_BINMODE(ਧਾਰਾ) (ਖਾਲੀ)(ਧਾਰਾ)
#ਜੇ_ਅੰਤ

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#ਜੇ defined(_WIN32) || defined(MSDOS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_READTEXT   "rt"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_WRITETEXT  "wt"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_APPENDTEXT "at"
#ਜਾਂਜੇ defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_READTEXT   "rt"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_WRITETEXT  "w"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_APPENDTEXT "a"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_READTEXT   "r"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_WRITETEXT  "w"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOPEN_APPENDTEXT "a"
#ਜੇ_ਅੰਤ

/* for systems that do not detect this in configure */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T ਪਤਾ_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ
#  ਨਹੀਂਤਾਂ
/* use a sensible default */
#    define CURL_SA_FAMILY_T ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ
#  endif
#ਜੇ_ਅੰਤ

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CSTRLEN(x) (ਆਕਾਰ(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCONST(x) x, CURL_CSTRLEN(x)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ARRAYSIZE(A) (ਆਕਾਰ(A) / ਆਕਾਰ((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRERROR_LEN 256

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DID_ਸਮ੍ਰਿਤੀ_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *(*curl_malloc_callback)(size_t ਮਾਪ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*curl_free_callback)(ਖਾਲੀ *ptr);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *(*curl_realloc_callback)(ਖਾਲੀ *ptr, size_t ਮਾਪ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ *(*curl_strdup_callback)(ਅਚਲ ਅੱਖਰ *str);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *(*curl_calloc_callback)(size_t nmemb, size_t ਮਾਪ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DID_ਸਮ੍ਰਿਤੀ_FUNC_TYPEDEFS
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ curl_malloc_callback Curl_cmalloc;
ਬਾਹਰੀ curl_free_callback Curl_cfree;
ਬਾਹਰੀ curl_realloc_callback Curl_crealloc;
ਬਾਹਰੀ curl_strdup_callback Curl_cstrdup;
ਬਾਹਰੀ curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_safefree(ptr) \
  ਕਰੋ {                      \
    curlx_free(ptr);        \
    (ptr) = ਨਿਰਾ;           \
  } ਜਦੋਂ(0)

#ਅਨ੍ਤਰ੍ਭਾਵ <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MEMDEBUG
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_MEMDEBUG
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  ਜੇ __clang_major__ >= 4
#  define ALLOC_ਮਾਪ(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  ਨਹੀਂਤਾਂ
#  define ALLOC_ਮਾਪ(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#ਜਾਂਜੇ defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_ਮਾਪ(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#ਜਾਂਜੇ defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(ਸੀਮਿਤ)
#  define ALLOC_ਮਾਪ(s)
#  define ALLOC_SIZE2(n, s)
#ਨਹੀਂਤਾਂ
#  define ALLOC_FUNC
#  define ALLOC_ਮਾਪ(s)
#  define ALLOC_SIZE2(n, s)
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN ਖਾਲੀ curl_dbg_free(ਖਾਲੀ *ptr, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_FUNC ALLOC_ਮਾਪ(1)
  ਖਾਲੀ *curl_dbg_malloc(size_t wantedsize, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  ਖਾਲੀ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_ਮਾਪ,
                        ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_ਮਾਪ(2)
  ਖਾਲੀ *curl_dbg_realloc(ਖਾਲੀ *ptr, size_t wantedsize, ਪੂਰਨ ਸਤਰ,
                         ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_FUNC
  ਅੱਖਰ *curl_dbg_strdup(ਅਚਲ ਅੱਖਰ *str, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ਅਚਲ wchar_t *str, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ_ਅੰਤ

CURL_EXTERN ਖਾਲੀ curl_dbg_memdebug(ਅਚਲ ਅੱਖਰ *logname);
CURL_EXTERN ਖਾਲੀ curl_dbg_memlimit(ਲੰਮਾ ਹੱਦਬੰਦੀ);
CURL_EXTERN ਖਾਲੀ curl_dbg_log(ਅਚਲ ਅੱਖਰ *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(ਪੂਰਨ domain, ਪੂਰਨ type, ਪੂਰਨ protocol,
                                          ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ਖਾਲੀ curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ਪੂਰਨ curl_dbg_sclose(curl_socket_t sockfd,
                                ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          ਖਾਲੀ *saddr, ਖਾਲੀ *saddrlen,
                                          ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, ਖਾਲੀ *saddr,
                                           ਖਾਲੀ *saddrlen, ਪੂਰਨ flags,
                                           ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKETPAIR
CURL_EXTERN ਪੂਰਨ curl_dbg_socketpair(ਪੂਰਨ domain, ਪੂਰਨ type, ਪੂਰਨ protocol,
                                    curl_socket_t socket_ਸਦਿਸ਼[2],
                                    ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ_ਅੰਤ

/* FILE functions */
CURL_EXTERN ਪੂਰਨ curl_dbg_fclose(FILE *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ਅਚਲ ਅੱਖਰ *file, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                                            ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ਅਚਲ ਅੱਖਰ *file,
                                              ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE *fh,
                                              ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(ਪੂਰਨ filedes, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                                             ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_FREEADDRINFO(ਅੰਕੜੇ) \
  curl_dbg_freeaddrinfo(ਅੰਕੜੇ, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((ਪੂਰਨ)(domain), type, protocol, __LINE__, __FILE__)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKETPAIR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SOCKETPAIR(domain, type, protocol, socket_ਸਦਿਸ਼) \
  curl_dbg_socketpair((ਪੂਰਨ)(domain), type, protocol, socket_ਸਦਿਸ਼, \
                      __LINE__, __FILE__)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACCEPT4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#ਜੇ_ਅੰਤ

#ਨਹੀਂਤਾਂ /* !CURL_MEMDEBUG */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sclose(x) CURL_SCLOSE(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ fake_sclose(x) Curl_nop_stmt

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_GETADDRINFO getaddrinfo
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_FREEADDRINFO freeaddrinfo
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SOCKET socket
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKETPAIR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SOCKETPAIR socketpair
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ACCEPT accept
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACCEPT4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ACCEPT4 accept4
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CURL_MEMDEBUG */

/* Allocator macros */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_STRDUP_ਨੀਵਾਂ _strdup
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLX_STRDUP_ਨੀਵਾਂ strdup
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_MEMDEBUG

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_malloc(ਮਾਪ)         curl_dbg_malloc(ਮਾਪ, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_calloc(nbelem, ਮਾਪ) \
  curl_dbg_calloc(nbelem, ਮਾਪ, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_realloc(ptr, ਮਾਪ) \
  curl_dbg_realloc(ptr, ਮਾਪ, __LINE__, __FILE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_tcsdup               curlx_strdup
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* _WIN32 */

#ਨਹੀਂਤਾਂ /* !CURL_MEMDEBUG */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BUILDING_LIBCURL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_strdup               Curl_cstrdup
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_malloc               Curl_cmalloc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_calloc               Curl_ccalloc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_realloc              Curl_crealloc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_free                 Curl_cfree
#ਨਹੀਂਤਾਂ /* !BUILDING_LIBCURL */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_strdup               CURLX_STRDUP_ਨੀਵਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_malloc               ਸਮ੍ਰਿਤੀ_ਦਿਓ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_calloc               ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_realloc              realloc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_free                 ਮੁਕਤ
#ਜੇ_ਅੰਤ /* BUILDING_LIBCURL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_tcsdup               curlx_wcsdup
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_tcsdup               curlx_strdup
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* _WIN32 */

#ਜੇ_ਅੰਤ /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#ਜੇ defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
ਬਣਤਰ passwd;
ਪੂਰਨ getpwuid_r(uid_t uid, ਬਣਤਰ passwd *pwd, ਅੱਖਰ *buf,
               size_t buflen, ਬਣਤਰ passwd **ਨਤੀਜਾ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNITTEST
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNITTEST ਸਥਿਰ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_HTTP2
#ਜੇ_ਅੰਤ

#ਜੇ (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
#ਗਲਤੀ "MultiSSL combined with QUIC is not supported"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_HTTP3
#ਜੇ_ਅੰਤ

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __EMSCRIPTEN__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TCP_NODELAY_SUPPORTED
#ਜੇ_ਅੰਤ

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#ਜੇ defined(HAVE_BASENAME) && defined(_WIN32)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ HAVE_BASENAME
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#ਜੇ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#ਜਾਂਜੇ !defined(UNIX_ਰਸਤਾ_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_ਰਸਤਾ_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  ਅੱਖਰ sun_ਰਸਤਾ[UNIX_ਰਸਤਾ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_ਨਿਰਧਾਰਤ_ਕਰਦਾ_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_ਨਿਰਧਾਰਤ_ਕਰਦਾ_WARNING
#    endif
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#ਜਾਂਜੇ defined(ਸਤਰਅੰਦਰ)
#  define CURL_INLINE ਸਤਰਅੰਦਰ /* 'inline' defined, assumed correct */
#ਜਾਂਜੇ defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE ਸਤਰਅੰਦਰ /* 'inline' keyword supported */
#ਜਾਂਜੇ defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE ਸਤਰਅੰਦਰ /* 'inline' keyword supported */
#ਜਾਂਜੇ defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#ਜਾਂਜੇ defined(_MSC_VER)
#  define CURL_INLINE __inline
#ਨਹੀਂਤਾਂ
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /*ਖਾਲੀ_ਥਾਵਾਂ*/
#ਜੇ_ਅੰਤ

/* Detect if compiler supports C99 variadic macros */
#ਜੇ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HAVE_MACRO_VARARG
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_ਲੜੀਆਂ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERBOSE(x) x
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOVERBOSE(x) Curl_nop_stmt
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERBOSE(x) Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOVERBOSE(x) x
#ਜੇ_ਅੰਤ

/* For FreeBSD it is included from curl/curl.h */
#ਜੇ defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#ਜੇ_ਅੰਤ

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#ਜੇ defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_USE_SOCK_NONBLOCK
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਜੇ defined(_MSC_VER) && defined(NTDDI_ਸੰਸਕਰਣ) && \
  (NTDDI_ਸੰਸਕਰਣ >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#ਹਦਾਇਤ comment(lib, "volatileaccessu.lib")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ)  SecureZeroMemory2(buf, ਮਾਪ)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ)  Secure_Zero_ਸਮ੍ਰਿਤੀ(buf, ਮਾਪ)
#ਜੇ_ਅੰਤ
#ਜਾਂਜੇ defined(HAVE_MEMSET_S)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ)  (ਖਾਲੀ)memset_s(buf, ਮਾਪ, 0, ਮਾਪ)
#ਜਾਂਜੇ defined(HAVE_MEMSET_EXPLICIT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ)  (ਖਾਲੀ)memset_explicit(buf, 0, ਮਾਪ)
#ਜਾਂਜੇ defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ)  explicit_bzero(buf, ਮਾਪ)
#ਜਾਂਜੇ defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ)  (ਖਾਲੀ)explicit_memset(buf, 0, ਮਾਪ)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* !_CURL_LOCAL_MEMZERO */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ curlx_memzero_ਨੀਵਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_CURLX_MEMZERO
ਖਾਲੀ curlx_memzero_ਨੀਵਾਂ(ਖਾਲੀ *buf, size_t ਮਾਪ);
#ਜੇ_ਅੰਤ
ਖਾਲੀ curlx_memzero(ਖਾਲੀ *buf, size_t ਮਾਪ);
ਖਾਲੀ curlx_strzero(ਖਾਲੀ *buf);

#ਜੇ_ਅੰਤ /* HEADER_CURL_SETUP_H */
