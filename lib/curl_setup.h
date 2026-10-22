#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಸಿದ್ಧತೆ_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಸಿದ್ಧತೆ_H
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

#ಆದರೆ defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NO_OLDIES
#ಆದರೆ_ಅಂತ್ಯ

/* Set default _WIN32_WINNT */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
#ಅನ್ತರ್ಭಾವ <_mingw.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ಆದರೆ defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  ಆದರೆ !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __APPLE__
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <TargetConditionals.h>
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
#ಆದರೆ defined(ಗುರಿ_OS_MAC) && ಗುರಿ_OS_MAC && \
  defined(ಗುರಿ_OS_OSX) && !ಗುರಿ_OS_OSX && \
  (!defined(ಗುರಿ_OS_IPHONE) || !ಗುರಿ_OS_IPHONE) && \
  (!defined(ಗುರಿ_OS_SIMULATOR) || !ಗುರಿ_OS_SIMULATOR)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಗುರಿ_OS_OSX
#ವ್ಯಾಖ್ಯಾನಿಸು ಗುರಿ_OS_OSX ಗುರಿ_OS_MAC
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#ದೋಷ "mingw-w64 3.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#ಆದರೆ defined(_MSC_VER) && (_MSC_VER < 1600)
#ದೋಷ "Ancient versions of Visual Studio are no longer supported due to bugs."
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#ಸೂಚನೆ warning(disable:4127)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _CRT_SECURE_NO_WARNINGS
#ವ್ಯಾಖ್ಯಾನಿಸು _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* _MSC_VER */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_ಸರಾಸರಿ
#  define WIN32_LEAN_AND_ಸರಾಸರಿ
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  ಆದರೆ (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    ಆದರೆ WINAPI_FAMILY_ವಿಭಜನೆ(WINAPI_ವಿಭಜನೆ_APP) &&  \
       !WINAPI_FAMILY_ವಿಭಜನೆ(WINAPI_ವಿಭಜನೆ_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SECURITY_KERNEL
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SECURITY_WIN32
#ವ್ಯಾಖ್ಯಾನಿಸು SECURITY_WIN32  /* for <sspi.h> */
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CONFIG_H

#ಅನ್ತರ್ಭಾವ "curl_config.h"

#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_CONFIG_H */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#  include "config-win32.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ macintosh
#  include "config-mac.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __OS400__
#  include "config-os400.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_CONFIG_H */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#  ಆದರೆ defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build ಗುರಿ is Windows Vista (0x0600)
#  endif

#  ಆದರೆ !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NEED_THREAD_ಸುರಕ್ಷಿತ
#  ifndef THREAD_ಸುರಕ್ಷಿತ
#  define THREAD_ಸುರಕ್ಷಿತ
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#ಆದರೆ defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಎಳೆಗಳು_POSIX
#  include <pthread.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_ಶೀರ್ಷಿಕೆಗಳು_API
#  define CURL_DISABLE_ಶೀರ್ಷಿಕೆಗಳು_API
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
#ಆದರೆ_ಅಂತ್ಯ

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __OS400__
#  include "setup-os400.h"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * VMS setup file includes some system headers.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#  include "setup-vms.h"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Windows setup file includes some system headers.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#  include "setup-win32.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <curl/system.h>

#ಆದರೆ !defined(HAVE_PIPE2) && \
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
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_PIPE2
#ಆದರೆ_ಅಂತ್ಯ

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CONC_MACROS_(A, B) A ## B
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#ಆದರೆ_ಅಂತ್ಯ

/* based on logic in "curl/mprintf.h" */
#ಆದರೆ (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#ಆದರೆ defined(__MINGW32__) && !defined(__clang__)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PRINTF(fmt, arg)
#ಆದರೆ_ಅಂತ್ಯ

/* Override default printf mask check rules in "curl/mprintf.h" */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ತಾತ್ಕಾಲಿಕ_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#ಆದರೆ defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#ವ್ಯಾಖ್ಯಾನಿಸು __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#ಆದರೆ defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  ಆದರೆ ಗುರಿ_OS_MAC && !(defined(ಗುರಿ_OS_IPHONE) && ಗುರಿ_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  ಆದರೆ defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    ಆದರೆ !(defined(__NEWLIB__) || \
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
#  ಆದರೆ defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       ಪೂರ್ಣಾಂಕ Curl_amiga_select(ಪೂರ್ಣಾಂಕ nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, ರಚನೆ timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    ಇಲ್ಲದಿದ್ದರೆ
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  ಇಲ್ಲದಿದ್ದರೆ
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#ಆದರೆ_ಅಂತ್ಯ

/* Whether to use eventfd() */
#ಆದರೆ defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_EVENTFD
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SO_NOSIGPIPE
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SO_NOSIGPIPE
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <assert.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdint.h>
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#ವ್ಯಾಖ್ಯಾನಿಸು uint32_t ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

/* Disable uintptr_t for targets known to miss it from stdint.h */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __OS400__
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು HAVE_UINTPTR_T
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#ಅನ್ತರ್ಭಾವ <sys/types.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#ಅಥವಾಆದರೆ defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((ಸ್ಥಾನಾಂತರ_t)-1)
#ಅಥವಾಆದರೆ defined(__AMIGA__)
#  define curl_lseek(fd, ಸ್ಥಾನಾಂತರ, whence)  lseek(fd, (off_t)(ಸ್ಥಾನಾಂತರ), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#ಇಲ್ಲದಿದ್ದರೆ
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIZEOF_ಸಮಯ_T
/* assume default size of time_t to be 32 bits */
#ವ್ಯಾಖ್ಯಾನಿಸು SIZEOF_ಸಮಯ_T 4
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  ಆದರೆ defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  ಇಲ್ಲದಿದ್ದರೆ
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ SIZEOF_CURL_SOCKET_T < 8
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
#  define FMT_SOCKET_T "u"
#ಇಲ್ಲದಿದ್ದರೆ
#  define FMT_SOCKET_T "d"
#ಆದರೆ_ಅಂತ್ಯ
#ಅಥವಾಆದರೆ defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#ಇಲ್ಲದಿದ್ದರೆ
#  define FMT_SOCKET_T "qd"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIZEOF_OFF_T
#  ಆದರೆ defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    ಆದರೆ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    ಆದರೆ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ SIZEOF_CURL_OFF_T < 8
#ದೋಷ "too small curl_off_t"
#ಇಲ್ಲದಿದ್ದರೆ
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#ವ್ಯಾಖ್ಯಾನಿಸು FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#ವ್ಯಾಖ್ಯಾನಿಸು FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#ಆದರೆ SIZEOF_ಸಮಯ_T == 4
#  ifdef HAVE_ಸಮಯ_T_UNSIGNED
#  define ಸಮಯ_T_MAX UINT_MAX
#  define ಸಮಯ_T_MIN 0
#  ಇಲ್ಲದಿದ್ದರೆ
#  define ಸಮಯ_T_MAX ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
#  define ಸಮಯ_T_MIN INT_MIN
#  endif
#ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_ಸಮಯ_T_UNSIGNED
#  define ಸಮಯ_T_MAX 0xFFFFFFFFFFFFFFFF
#  define ಸಮಯ_T_MIN 0
#  ಇಲ್ಲದಿದ್ದರೆ
#  define ಸಮಯ_T_MAX 0x7FFFFFFFFFFFFFFF
#  define ಸಮಯ_T_MIN (-ಸಮಯ_T_MAX - 1)
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIZE_MAX
/* some limits.h headers have this defined, some do not */
#ಆದರೆ defined(SIZEOF_ಗಾತ್ರ_T) && (SIZEOF_ಗಾತ್ರ_T > 4)
#ವ್ಯಾಖ್ಯಾನಿಸು SIZE_MAX 18446744073709551615U
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SIZE_MAX 4294967295U
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#ಆದರೆ defined(SIZEOF_ಗಾತ್ರ_T) && (SIZEOF_ಗಾತ್ರ_T > 4)
#ವ್ಯಾಖ್ಯಾನಿಸು SSIZE_MAX 9223372036854775807
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SSIZE_MAX 2147483647
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ SIZEOF_LONG > SIZEOF_ಗಾತ್ರ_T
#ದೋಷ "unexpected: 'long' is larger than 'size_t'"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 ಪೂರ್ಣಾಂಕ
#  ಇಲ್ಲದಿದ್ದರೆ
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

#  define DIR_CHAR      "\\"

#ಇಲ್ಲದಿದ್ದರೆ /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (ಅಕ್ಷರ *)(z))
#    include <tcp.h>
#    undef ಶಬ್ದ
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     ಬಾಹ್ಯ ರಚನೆ tm *gmtime_r(ಸ್ಥಿರಾಂಕ time_t * ಸ್ಥಿರಾಂಕ timep, ರಚನೆ tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

/* We want to use mutex when available. */
#ಆದರೆ defined(HAVE_ಎಳೆಗಳು_POSIX) || defined(_WIN32)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_MUTEX
#ಆದರೆ_ಅಂತ್ಯ

/* threaded resolver is the only feature requiring threads. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
#ವ್ಯಾಖ್ಯಾನಿಸು USE_ಎಳೆಗಳು
#ಆದರೆ_ಅಂತ್ಯ

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#ಆದರೆ defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#ಅಥವಾಆದರೆ defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#ಇಲ್ಲದಿದ್ದರೆ
#  define CURLRES_IPV4
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#ಅಥವಾಆದರೆ defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#ಇಲ್ಲದಿದ್ದರೆ
#  define CURLRES_SYNCH
#ಆದರೆ_ಅಂತ್ಯ

/* ---------------------------------------------------------------- */

#ಆದರೆ defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_LIBIDN2
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#ದೋಷ "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SSL    /* SSL support has been enabled */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_COEXIST
#ವ್ಯಾಖ್ಯಾನಿಸು OPENSSL_COEXIST
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#ವ್ಯಾಖ್ಯಾನಿಸು NO_OLD_WC_ಹೆಸರುಗಳು
#ಆದರೆ_ಅಂತ್ಯ

/* Single point where USE_SPNEGO definition might be defined */
#ಆದರೆ !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SPNEGO
#ಆದರೆ_ಅಂತ್ಯ

/* Single point where USE_KERBEROS5 definition might be defined */
#ಆದರೆ !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#ವ್ಯಾಖ್ಯಾನಿಸು USE_KERBEROS5
#ಆದರೆ_ಅಂತ್ಯ

/* Single point where USE_NTLM definition might be defined */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_ENABLE_NTLM
#  ಆದರೆ (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  ಆದರೆ defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SSH
#ಆದರೆ_ಅಂತ್ಯ

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#ಆದರೆ defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HAVE_DIAG
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#ಆದರೆ defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_ಬಳಸದ_ಫಲಿತಾಂಶ __attribute__((warn_ಬಳಸದ_ಫಲಿತಾಂಶ))
#ಅಥವಾಆದರೆ defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_ಬಳಸದ_ಫಲಿತಾಂಶ __attribute__((warn_ಬಳಸದ_ಫಲಿತಾಂಶ))
#ಇಲ್ಲದಿದ್ದರೆ
#  define WARN_ಬಳಸದ_ಫಲಿತಾಂಶ
#ಆದರೆ_ಅಂತ್ಯ

/* noreturn attribute */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_NORETURN
#ಆದರೆ (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#ಅಥವಾಆದರೆ defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#ಇಲ್ಲದಿದ್ದರೆ
#  define CURL_NORETURN
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/* fallthrough attribute */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FALLTHROUGH
#ಆದರೆ (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#ಇಲ್ಲದಿದ್ದರೆ
#  define FALLTHROUGH()  ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Inclusion of common header files.
 */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಆದರೆ !defined(_WIN32) || defined(__MINGW32__)
#ಅನ್ತರ್ಭಾವ <sys/time.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IO_H
#ಅನ್ತರ್ಭಾವ <io.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNISTD_H
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UINTPTR_T
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_UNCONST(p) ((ಶೂನ್ಯ *)(uintptr_t)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)(p))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_UNCONST(p) ((ಶೂನ್ಯ *)(p))  /* Fall back to simple cast */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __hpux
#  ಆದರೆ !defined(XOPEN_ಮೂಲ_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_ಬಿಟ್_OFF_T
#      define OLD_APP32_64_ಬಿಟ್_OFF_T APP32_64_ಬಿಟ್_OFF_T
#      undef APP32_64_ಬಿಟ್_OFF_T
#    ಇಲ್ಲದಿದ್ದರೆ
#      undef OLD_APP32_64_ಬಿಟ್_OFF_T
#    endif
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _WIN32
#ಅನ್ತರ್ಭಾವ <sys/socket.h>  /* also for MSG_NOSIGNAL */
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "functypes.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __hpux
#  ಆದರೆ !defined(XOPEN_ಮೂಲ_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_ಬಿಟ್_OFF_T
#      define APP32_64_ಬಿಟ್_OFF_T OLD_APP32_64_ಬಿಟ್_OFF_T
#      undef OLD_APP32_64_ಬಿಟ್_OFF_T
#    endif
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_STRUCT_TIMEVAL
ರಚನೆ timeval {
  ದೀರ್ಘ tv_sec;
  ದೀರ್ಘ tv_usec;
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSG_NOSIGNAL
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_4TH_ARG MSG_NOSIGNAL
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SEND_4TH_ARG 0
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __minix
/* Minix does not support recv on TCP sockets */
#ವ್ಯಾಖ್ಯಾನಿಸು sread(x, y, z) (ssize_t)ಪಠ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#ಅಥವಾಆದರೆ defined(HAVE_RECV)
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

#ವ್ಯಾಖ್ಯಾನಿಸು sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_RECV */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ sread
#ದೋಷ "Missing definition of macro sread"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_RECV */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __minix
/* Minix does not support send on TCP sockets */
#ವ್ಯಾಖ್ಯಾನಿಸು swrite(x, y, z) (ssize_t)ಲಿಖ((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#ಅಥವಾಆದರೆ defined(HAVE_SEND)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SEND_NONCONST_ARG2
#ವ್ಯಾಖ್ಯಾನಿಸು swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ಸ್ಥಿರಾಂಕ SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#ಆದರೆ_ಅಂತ್ಯ /* SEND_NONCONST_ARG2 */
#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_SEND */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ swrite
#ದೋಷ "Missing definition of macro swrite"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#ಅಥವಾಆದರೆ defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#ಅಥವಾಆದರೆ defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#ಅಥವಾಆದರೆ defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#ಇಲ್ಲದಿದ್ದರೆ
#  define CURL_SCLOSE(x)  close(x)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Stack-independent version of fcntl() on sockets:
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#ಇಲ್ಲದಿದ್ದರೆ
#  define sfcntl  fcntl
#ಆದರೆ_ಅಂತ್ಯ

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#ಆದರೆ defined(__hpux) && !defined(HAVE_BOOL_T)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#ಆದರೆ_ಅಂತ್ಯ

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
#ಆದರೆ_ಅಂತ್ಯ

/* the type we use for storing a single boolean bit */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curl_ಬಿಟ್;
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್(x) curl_ಬಿಟ್ x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು TRUE true
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು FALSE false
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curl_ctype.h"

#ಆದರೆ defined(DEBUGBUILD) && defined(NDEBUG)
#ದೋಷ "Debug-enabled builds cannot be combined with NDEBUG"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Macro used to include code only in debug builds.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGF(x) x
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGF(x) ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Macro used to include assertion code only in debug builds.
 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEBUGASSERT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGASSERT(x) ಪ್ರತಿಜ್ಞಾ(x)
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGASSERT(x) ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#ಆದರೆ defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#ಅನ್ತರ್ಭಾವ <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MSVC_CRTDBG_INIT()                                             \
  ಮಾಡು {                                                                      \
    ಗಣ_error_ಬಹುಲಕ(ಹೊರಹೋಗು_ಗಮ್ಯ_STDERR);  /* needs stdlib.h */                  \
    Crt_ಗಣ_Report_ಬಹುಲಕ(_CRT_ASSERT, CRTDBG_ಬಹುಲಕ_FILE | CRTDBG_ಬಹುಲಕ_DEBUG); \
    Crt_ಗಣ_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_ಗಣ_Report_ಬಹುಲಕ(_CRT_ERROR, CRTDBG_ಬಹುಲಕ_FILE | CRTDBG_ಬಹುಲಕ_DEBUG);  \
    Crt_ಗಣ_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_ಗಣ_Report_ಬಹುಲಕ(_CRT_WARN, CRTDBG_ಬಹುಲಕ_FILE | CRTDBG_ಬಹುಲಕ_DEBUG);   \
    Crt_ಗಣ_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MSVC_CRTDBG_INIT() ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#ಅನ್ತರ್ಭಾವ <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DUMP_MODULE_PATHS(printf_f)                               \
  ಮಾಡು {                                                                 \
    HANDLE hnd = INVALID_HANDLE_ಮೌಲ್ಯ;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_ಗಾತ್ರ = ಗಾತ್ರ(MODULEENTRY32);                                \
                                                                       \
    ಮಾಡು {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } ಆಗ(hnd == INVALID_HANDLE_ಮೌಲ್ಯ &&                             \
            Get_ಕೊನೆಯ_Error() == ERROR_BAD_ಉದ್ದಳತೆ);                       \
                                                                       \
    ಆದರೆ(hnd == INVALID_HANDLE_ಮೌಲ್ಯ)                                    \
      ಹೋಗು error;                                                      \
                                                                       \
    ಆದರೆ(!Module32_ಮೊದಲ(hnd, &mod))                                      \
      ಹೋಗು error;                                                      \
                                                                       \
    ಮಾಡು {                                                               \
      ಅಕ್ಷರ ಬಫರ್[ಗಾತ್ರ(mod.sz_Exe_ಮಾರ್ಗ) * 2];                          \
      ಅಕ್ಷರ *ಮಾರ್ಗ; /* points to stack allocated buffer */               \
      ಆದರೆ(ಗಾತ್ರ(*mod.sz_Exe_ಮಾರ್ಗ) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        ಆದರೆ(!Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_ಮಾರ್ಗ, -1, \
                                ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), ಖಾಲಿ, ಖಾಲಿ))   \
          ಹೋಗು error;                                                  \
        ಮಾರ್ಗ = (ಅಕ್ಷರ *)ಬಫರ್;                                         \
      }                                                                \
      ಇಲ್ಲದಿದ್ದರೆ                                                             \
        ಮಾರ್ಗ = (ಅಕ್ಷರ *)mod.sz_Exe_ಮಾರ್ಗ;                                  \
      (printf_f)("%s\n", ಮಾರ್ಗ);                                        \
    } ಆಗ(Module32_ಮುಂದಿನ(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    ಆದರೆ(hnd != INVALID_HANDLE_ಮೌಲ್ಯ)                                    \
      CloseHandle(hnd);                                                \
  } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DUMP_MODULE_PATHS(printf_f) ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKERRNO         ((ಪೂರ್ಣಾಂಕ)WSAGet_ಕೊನೆಯ_Error())
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_SOCKERRNO(x)  WSASet_ಕೊನೆಯ_Error((ಪೂರ್ಣಾಂಕ)(x))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKERRNO         errno
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_SOCKERRNO(x)  (errno = (x))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEACCES        WSAEACCES
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEADDRINUSE    WSAEADDRINUSE
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEBADF         WSAEBADF
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKECONNREFUSED  WSAECONNREFUSED
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKECONNRESET    WSAECONNRESET
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEINPROGRESS   WSAEINPROGRESS
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEINTR         WSAEINTR
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEINVAL        WSAEINVAL
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEISCONN       WSAEISCONN
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKETIMEDOUT     WSAETIMEDOUT
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEACCES        EACCES
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEADDRINUSE    EADDRINUSE
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEAFNOSUPPORT  EAFNOSUPPORT
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEBADF         EBADF
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKECONNREFUSED  ECONNREFUSED
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKECONNRESET    ECONNRESET
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEINPROGRESS   EINPROGRESS
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEINTR         EINTR
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEINVAL        EINVAL
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEISCONN       EISCONN
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEMSGSIZE      EMSGSIZE
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKENOMEM        ENOMEM
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ETIMEDOUT
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKETIMEDOUT     ETIMEDOUT
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKEWOULDBLOCK   EWOULDBLOCK
#ಆದರೆ_ಅಂತ್ಯ

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#ಆದರೆ !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#ವ್ಯಾಖ್ಯಾನಿಸು SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ವ್ಯಾಖ್ಯಾನಿಸು argv_ಅಂಶ_t  __char_ptr32
#ಅಥವಾಆದರೆ defined(_UNICODE)
#ವ್ಯಾಖ್ಯಾನಿಸು argv_ಅಂಶ_t  wchar_t *
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು argv_ಅಂಶ_t  ಅಕ್ಷರ *
#ಆದರೆ_ಅಂತ್ಯ

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#ಅನ್ತರ್ಭಾವ "curlx/warnless.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#  undef ಪಠ
#  define ಪಠ(fd, buf, ಎಣಿಕೆ_2)  (ssize_t)_read(fd, buf, curlx_uztoui(ಎಣಿಕೆ_2))
#  undef ಲಿಖ
#  define ಲಿಖ(fd, buf, ಎಣಿಕೆ_2) (ssize_t)_write(fd, buf, curlx_uztoui(ಎಣಿಕೆ_2))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Definition of our NOP statement Object-like macro
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_nop_stmt ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#ಆದರೆ defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  ಆದರೆ defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * shutdown() flags for systems that do not define them
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SHUT_RD
#ವ್ಯಾಖ್ಯಾನಿಸು SHUT_RD 0x00
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SHUT_WR
#ವ್ಯಾಖ್ಯಾನಿಸು SHUT_WR 0x01
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SHUT_RDWR
#ವ್ಯಾಖ್ಯಾನಿಸು SHUT_RDWR 0x02
#ಆದರೆ_ಅಂತ್ಯ

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#ಆದರೆ !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#ವ್ಯಾಖ್ಯಾನಿಸು S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#ಆದರೆ_ಅಂತ್ಯ

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#ಆದರೆ !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#ವ್ಯಾಖ್ಯಾನಿಸು S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#ಆದರೆ_ಅಂತ್ಯ

/* For MSVC (all versions as of VS2022) */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ STDIN_FILENO
#ವ್ಯಾಖ್ಯಾನಿಸು STDIN_FILENO  fileno(stdin)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ STDOUT_FILENO
#ವ್ಯಾಖ್ಯಾನಿಸು STDOUT_FILENO  fileno(stdout)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ STDERR_FILENO
#ವ್ಯಾಖ್ಯಾನಿಸು STDERR_FILENO  fileno(stderr)
#ಆದರೆ_ಅಂತ್ಯ

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ O_ದ್ವಿಮಾನ  /* for _WIN32 || MSDOS */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_O_ದ್ವಿಮಾನ O_ದ್ವಿಮಾನ
#ಅಥವಾಆದರೆ defined(O_ದ್ವಿಮಾನ_2)  /* __CYGWIN__ */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_O_ದ್ವಿಮಾನ O_ದ್ವಿಮಾನ_2
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_O_ದ್ವಿಮಾನ 0
#ಆದರೆ_ಅಂತ್ಯ

/* Requires io.h when available */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSDOS
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_BINMODE(ಹರಿವು) (ಶೂನ್ಯ)setmode(fileno(ಹರಿವು), CURL_O_ದ್ವಿಮಾನ)
#ಅಥವಾಆದರೆ defined(_WIN32) || defined(__CYGWIN__)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_BINMODE(ಹರಿವು) (ಶೂನ್ಯ)_setmode(fileno(ಹರಿವು), CURL_O_ದ್ವಿಮಾನ)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_BINMODE(ಹರಿವು) (ಶೂನ್ಯ)(ಹರಿವು)
#ಆದರೆ_ಅಂತ್ಯ

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#ಆದರೆ defined(_WIN32) || defined(MSDOS)
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_READTEXT   "rt"
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_WRITETEXT  "wt"
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_APPENDTEXT "at"
#ಅಥವಾಆದರೆ defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_READTEXT   "rt"
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_WRITETEXT  "w"
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_APPENDTEXT "a"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_READTEXT   "r"
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_WRITETEXT  "w"
#ವ್ಯಾಖ್ಯಾನಿಸು FOPEN_APPENDTEXT "a"
#ಆದರೆ_ಅಂತ್ಯ

/* for systems that do not detect this in configure */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T ವಿಳಾಸ_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ
#  ಇಲ್ಲದಿದ್ದರೆ
/* use a sensible default */
#    define CURL_SA_FAMILY_T ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#ವ್ಯಾಖ್ಯಾನಿಸು CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CSTRLEN(x) (ಗಾತ್ರ(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#ವ್ಯಾಖ್ಯಾನಿಸು STRCONST(x) x, CURL_CSTRLEN(x)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ARRAYSIZE(A) (ಗಾತ್ರ(A) / ಗಾತ್ರ((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#ವ್ಯಾಖ್ಯಾನಿಸು STRERROR_LEN 256

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DID_ಸ್ಮರಣೆ_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *(*curl_malloc_callback)(size_t ಗಾತ್ರ_2);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*curl_free_callback)(ಶೂನ್ಯ *ptr);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *(*curl_realloc_callback)(ಶೂನ್ಯ *ptr, size_t ಗಾತ್ರ_2);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ *(*curl_strdup_callback)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *(*curl_calloc_callback)(size_t nmemb, size_t ಗಾತ್ರ_2);
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DID_ಸ್ಮರಣೆ_FUNC_TYPEDEFS
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ curl_malloc_callback Curl_cmalloc;
ಬಾಹ್ಯ curl_free_callback Curl_cfree;
ಬಾಹ್ಯ curl_realloc_callback Curl_crealloc;
ಬಾಹ್ಯ curl_strdup_callback Curl_cstrdup;
ಬಾಹ್ಯ curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_safefree(ptr) \
  ಮಾಡು {                      \
    curlx_free(ptr);        \
    (ptr) = ಖಾಲಿ;           \
  } ಆಗ(0)

#ಅನ್ತರ್ಭಾವ <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MEMDEBUG
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_MEMDEBUG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  ಆದರೆ __clang_major__ >= 4
#  define ALLOC_ಗಾತ್ರ(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  ಇಲ್ಲದಿದ್ದರೆ
#  define ALLOC_ಗಾತ್ರ(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#ಅಥವಾಆದರೆ defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_ಗಾತ್ರ(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#ಅಥವಾಆದರೆ defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(ಸೀಮಿತ)
#  define ALLOC_ಗಾತ್ರ(s)
#  define ALLOC_SIZE2(n, s)
#ಇಲ್ಲದಿದ್ದರೆ
#  define ALLOC_FUNC
#  define ALLOC_ಗಾತ್ರ(s)
#  define ALLOC_SIZE2(n, s)
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN ಶೂನ್ಯ curl_dbg_free(ಶೂನ್ಯ *ptr, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_FUNC ALLOC_ಗಾತ್ರ(1)
  ಶೂನ್ಯ *curl_dbg_malloc(size_t wantedsize, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  ಶೂನ್ಯ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_ಗಾತ್ರ,
                        ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_ಗಾತ್ರ(2)
  ಶೂನ್ಯ *curl_dbg_realloc(ಶೂನ್ಯ *ptr, size_t wantedsize, ಪೂರ್ಣಾಂಕ ಸಾಲು,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_FUNC
  ಅಕ್ಷರ *curl_dbg_strdup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ಆದರೆ defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ಸ್ಥಿರಾಂಕ wchar_t *str, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ಆದರೆ_ಅಂತ್ಯ

CURL_EXTERN ಶೂನ್ಯ curl_dbg_memdebug(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *logname);
CURL_EXTERN ಶೂನ್ಯ curl_dbg_memlimit(ದೀರ್ಘ ಎಲ್ಲೆ);
CURL_EXTERN ಶೂನ್ಯ curl_dbg_log(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(ಪೂರ್ಣಾಂಕ domain, ಪೂರ್ಣಾಂಕ type, ಪೂರ್ಣಾಂಕ protocol,
                                          ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ಶೂನ್ಯ curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ಪೂರ್ಣಾಂಕ curl_dbg_sclose(curl_socket_t sockfd,
                                ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          ಶೂನ್ಯ *saddr, ಶೂನ್ಯ *saddrlen,
                                          ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, ಶೂನ್ಯ *saddr,
                                           ಶೂನ್ಯ *saddrlen, ಪೂರ್ಣಾಂಕ flags,
                                           ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKETPAIR
CURL_EXTERN ಪೂರ್ಣಾಂಕ curl_dbg_socketpair(ಪೂರ್ಣಾಂಕ domain, ಪೂರ್ಣಾಂಕ type, ಪೂರ್ಣಾಂಕ protocol,
                                    curl_socket_t socket_ಸದಿಶ[2],
                                    ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ಆದರೆ_ಅಂತ್ಯ

/* FILE functions */
CURL_EXTERN ಪೂರ್ಣಾಂಕ curl_dbg_fclose(FILE *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                                            ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE *fh,
                                              ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(ಪೂರ್ಣಾಂಕ filedes, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                                             ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);

#ವ್ಯಾಖ್ಯಾನಿಸು sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_FREEADDRINFO(ದತ್ತಾಂಶ) \
  curl_dbg_freeaddrinfo(ದತ್ತಾಂಶ, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((ಪೂರ್ಣಾಂಕ)(domain), type, protocol, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKETPAIR
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SOCKETPAIR(domain, type, protocol, socket_ಸದಿಶ) \
  curl_dbg_socketpair((ಪೂರ್ಣಾಂಕ)(domain), type, protocol, socket_ಸದಿಶ, \
                      __LINE__, __FILE__)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACCEPT4
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#ಆದರೆ_ಅಂತ್ಯ

#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_MEMDEBUG */

#ವ್ಯಾಖ್ಯಾನಿಸು sclose(x) CURL_SCLOSE(x)
#ವ್ಯಾಖ್ಯಾನಿಸು fake_sclose(x) Curl_nop_stmt

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_GETADDRINFO getaddrinfo
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_FREEADDRINFO freeaddrinfo
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SOCKET socket
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKETPAIR
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SOCKETPAIR socketpair
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ACCEPT accept
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACCEPT4
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ACCEPT4 accept4
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CURL_MEMDEBUG */

/* Allocator macros */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_STRDUP_ಕೆಳಗಿನ _strdup
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURLX_STRDUP_ಕೆಳಗಿನ strdup
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_MEMDEBUG

#ವ್ಯಾಖ್ಯಾನಿಸು curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_malloc(ಗಾತ್ರ_2)         curl_dbg_malloc(ಗಾತ್ರ_2, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_calloc(nbelem, ಗಾತ್ರ_2) \
  curl_dbg_calloc(nbelem, ಗಾತ್ರ_2, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_realloc(ptr, ಗಾತ್ರ_2) \
  curl_dbg_realloc(ptr, ಗಾತ್ರ_2, __LINE__, __FILE__)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_tcsdup               curlx_strdup
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_MEMDEBUG */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BUILDING_LIBCURL
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_strdup               Curl_cstrdup
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_malloc               Curl_cmalloc
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_calloc               Curl_ccalloc
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_realloc              Curl_crealloc
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_free                 Curl_cfree
#ಇಲ್ಲದಿದ್ದರೆ /* !BUILDING_LIBCURL */
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_strdup               CURLX_STRDUP_ಕೆಳಗಿನ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_malloc               ಸ್ಮರಣೆ_ಕೊಡು
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_calloc               ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_realloc              realloc
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_free                 ಬಿಡುಗಡೆ
#ಆದರೆ_ಅಂತ್ಯ /* BUILDING_LIBCURL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_tcsdup               curlx_wcsdup
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_tcsdup               curlx_strdup
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

#ಆದರೆ_ಅಂತ್ಯ /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#ಆದರೆ defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
ರಚನೆ passwd;
ಪೂರ್ಣಾಂಕ getpwuid_r(uid_t uid, ರಚನೆ passwd *pwd, ಅಕ್ಷರ *buf,
               size_t buflen, ರಚನೆ passwd **ಫಲಿತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
#ವ್ಯಾಖ್ಯಾನಿಸು UNITTEST
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು UNITTEST ಸ್ಥಿರ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
#ವ್ಯಾಖ್ಯಾನಿಸು USE_HTTP2
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
#ದೋಷ "MultiSSL combined with QUIC is not supported"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು USE_HTTP3
#ಆದರೆ_ಅಂತ್ಯ

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __EMSCRIPTEN__
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TCP_NODELAY_SUPPORTED
#ಆದರೆ_ಅಂತ್ಯ

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#ಆದರೆ defined(HAVE_BASENAME) && defined(_WIN32)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು HAVE_BASENAME
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#ಆದರೆ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#ಅಥವಾಆದರೆ !defined(UNIX_ಮಾರ್ಗ_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_ಮಾರ್ಗ_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  ಅಕ್ಷರ sun_ಮಾರ್ಗ[UNIX_ಮಾರ್ಗ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_ನಿರೂಪಿಸುತ್ತದೆ_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_ನಿರೂಪಿಸುತ್ತದೆ_WARNING
#    endif
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#ಅಥವಾಆದರೆ defined(ಸಾಲೊಳಗೆ)
#  define CURL_INLINE ಸಾಲೊಳಗೆ /* 'inline' defined, assumed correct */
#ಅಥವಾಆದರೆ defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE ಸಾಲೊಳಗೆ /* 'inline' keyword supported */
#ಅಥವಾಆದರೆ defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE ಸಾಲೊಳಗೆ /* 'inline' keyword supported */
#ಅಥವಾಆದರೆ defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#ಅಥವಾಆದರೆ defined(_MSC_VER)
#  define CURL_INLINE __inline
#ಇಲ್ಲದಿದ್ದರೆ
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /*ಖಾಲಿಗಳು*/
#ಆದರೆ_ಅಂತ್ಯ

/* Detect if compiler supports C99 variadic macros */
#ಆದರೆ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HAVE_MACRO_VARARG
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು))
#ವ್ಯಾಖ್ಯಾನಿಸು CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು VERBOSE(x) x
#ವ್ಯಾಖ್ಯಾನಿಸು NOVERBOSE(x) Curl_nop_stmt
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು VERBOSE(x) Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು NOVERBOSE(x) x
#ಆದರೆ_ಅಂತ್ಯ

/* For FreeBSD it is included from curl/curl.h */
#ಆದರೆ defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#ಅನ್ತರ್ಭಾವ <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#ಆದರೆ_ಅಂತ್ಯ

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#ಆದರೆ defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_USE_SOCK_NONBLOCK
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ಆದರೆ defined(_MSC_VER) && defined(NTDDI_ಆವೃತ್ತಿ) && \
  (NTDDI_ಆವೃತ್ತಿ >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#ಸೂಚನೆ comment(lib, "volatileaccessu.lib")
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2)  SecureZeroMemory2(buf, ಗಾತ್ರ_2)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2)  Secure_Zero_ಸ್ಮರಣೆ(buf, ಗಾತ್ರ_2)
#ಆದರೆ_ಅಂತ್ಯ
#ಅಥವಾಆದರೆ defined(HAVE_MEMSET_S)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2)  (ಶೂನ್ಯ)memset_s(buf, ಗಾತ್ರ_2, 0, ಗಾತ್ರ_2)
#ಅಥವಾಆದರೆ defined(HAVE_MEMSET_EXPLICIT)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2)  (ಶೂನ್ಯ)memset_explicit(buf, 0, ಗಾತ್ರ_2)
#ಅಥವಾಆದರೆ defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2)  explicit_bzero(buf, ಗಾತ್ರ_2)
#ಅಥವಾಆದರೆ defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2)  (ಶೂನ್ಯ)explicit_memset(buf, 0, ಗಾತ್ರ_2)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* !_CURL_LOCAL_MEMZERO */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ curlx_memzero_ಕೆಳಗಿನ
#ವ್ಯಾಖ್ಯಾನಿಸು USE_CURLX_MEMZERO
ಶೂನ್ಯ curlx_memzero_ಕೆಳಗಿನ(ಶೂನ್ಯ *buf, size_t ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ curlx_memzero(ಶೂನ್ಯ *buf, size_t ಗಾತ್ರ_2);
ಶೂನ್ಯ curlx_strzero(ಶೂನ್ಯ *buf);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SETUP_H */
