#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_تياري_H
#وصف_ڪريو سرو_CURL_تياري_H
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

#إذا defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#وصف_ڪريو CURL_NO_OLDIES
#جيڪڏهن_پورو

/* Set default _WIN32_WINNT */
#جيڪڏهن_وصف_ٿيل __MINGW32__
#ضمن <_mingw.h>
#جيڪڏهن_پورو

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
#إذا defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  إذا !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __APPLE__
#ضمن <sys/types.h>
#ضمن <TargetConditionals.h>
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
#إذا defined(هدف_OS_MAC) && هدف_OS_MAC && \
  defined(هدف_OS_OSX) && !هدف_OS_OSX && \
  (!defined(هدف_OS_IPHONE) || !هدف_OS_IPHONE) && \
  (!defined(هدف_OS_SIMULATOR) || !هدف_OS_SIMULATOR)
#وصف_رد هدف_OS_OSX
#وصف_ڪريو هدف_OS_OSX هدف_OS_MAC
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#إذا defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#غلطي "mingw-w64 3.0 or greater required"
#جيڪڏهن_پورو

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#إذا defined(_MSC_VER) && (_MSC_VER < 1600)
#غلطي "Ancient versions of Visual Studio are no longer supported due to bugs."
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#هدايت warning(disable:4127)
#جيڪڏهن_وصف_نه_ٿيل _CRT_SECURE_NO_WARNINGS
#وصف_ڪريو _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* _MSC_VER */

#جيڪڏهن_وصف_ٿيل _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_سراسري
#  define WIN32_LEAN_AND_سراسري
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  إذا (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    إذا WINAPI_FAMILY_حصا_ڪريو(WINAPI_حصا_ڪريو_APP) &&  \
       !WINAPI_FAMILY_حصا_ڪريو(WINAPI_حصا_ڪريو_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#وصف_رد SECURITY_KERNEL
#وصف_رد SECURITY_WIN32
#وصف_ڪريو SECURITY_WIN32  /* for <sspi.h> */
#جيڪڏهن_پورو

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#جيڪڏهن_وصف_ٿيل HAVE_CONFIG_H

#ضمن "curl_config.h"

#وإلا /* HAVE_CONFIG_H */

#جيڪڏهن_وصف_ٿيل _WIN32
#  include "config-win32.h"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل macintosh
#  include "config-mac.h"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __OS400__
#  include "config-os400.h"
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HAVE_CONFIG_H */

#جيڪڏهن_وصف_ٿيل _WIN32
#  إذا defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build هدف is Windows Vista (0x0600)
#  endif

#  إذا !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#جيڪڏهن_پورو

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#جيڪڏهن_پورو

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#جيڪڏهن_وصف_ٿيل NEED_THREAD_محفوظ
#  ifndef THREAD_محفوظ
#  define THREAD_محفوظ
#  endif
#جيڪڏهن_پورو

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#جيڪڏهن_وصف_ٿيل NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#جيڪڏهن_پورو

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#إذا defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#جيڪڏهن_پورو

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#جيڪڏهن_وصف_ٿيل HAVE_ڌاڳا_POSIX
#  include <pthread.h>
#جيڪڏهن_پورو

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#جيڪڏهن_وصف_ٿيل CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_سرا_API
#  define CURL_DISABLE_سرا_API
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
#جيڪڏهن_پورو

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#جيڪڏهن_وصف_ٿيل __OS400__
#  include "setup-os400.h"
#جيڪڏهن_پورو

/*
 * VMS setup file includes some system headers.
 */
#جيڪڏهن_وصف_ٿيل __VMS
#  include "setup-vms.h"
#جيڪڏهن_پورو

/*
 * Windows setup file includes some system headers.
 */
#جيڪڏهن_وصف_ٿيل _WIN32
#  include "setup-win32.h"
#جيڪڏهن_پورو

#ضمن <curl/system.h>

#إذا !defined(HAVE_PIPE2) && \
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
#وصف_ڪريو HAVE_PIPE2
#جيڪڏهن_پورو

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#وصف_ڪريو CURL_CONC_MACROS_(A, B) A ## B
#وصف_ڪريو CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#جيڪڏهن_وصف_ٿيل __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#جيڪڏهن_پورو

/* based on logic in "curl/mprintf.h" */
#إذا (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#إذا defined(__MINGW32__) && !defined(__clang__)
#وصف_ڪريو CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#وإلا
#وصف_ڪريو CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#جيڪڏهن_پورو
#وإلا
#وصف_ڪريو CURL_PRINTF(fmt, arg)
#جيڪڏهن_پورو

/* Override default printf mask check rules in "curl/mprintf.h" */
#وصف_ڪريو CURL_عارضي_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#إذا defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#وصف_ڪريو __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#جيڪڏهن_پورو

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#إذا defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  إذا هدف_OS_MAC && !(defined(هدف_OS_IPHONE) && هدف_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  إذا defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    إذا !(defined(__NEWLIB__) || \
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
#  إذا defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       صحيح Curl_amiga_select(صحيح nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, بنية timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    وإلا
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  وإلا
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#جيڪڏهن_پورو

/* Whether to use eventfd() */
#إذا defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#وصف_ڪريو USE_EVENTFD
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SO_NOSIGPIPE
#وصف_ڪريو USE_SO_NOSIGPIPE
#جيڪڏهن_پورو

#ضمن <stdio.h>
#ضمن <assert.h>

#جيڪڏهن_وصف_ٿيل __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#جيڪڏهن_پورو

#ضمن <stdint.h>
#وصف_ڪريو HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#جيڪڏهن_وصف_ٿيل __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#وصف_ڪريو uint32_t بنانشان صحيح
#جيڪڏهن_پورو

/* Disable uintptr_t for targets known to miss it from stdint.h */
#جيڪڏهن_وصف_ٿيل __OS400__
#وصف_رد HAVE_UINTPTR_T
#جيڪڏهن_پورو

#ضمن <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#ضمن <sys/types.h>

#جيڪڏهن_وصف_ٿيل _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#وإلاإذا defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((وٿي_t)-1)
#وإلاإذا defined(__AMIGA__)
#  define curl_lseek(fd, وٿي, whence)  lseek(fd, (off_t)(وٿي), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#وإلا
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SIZEOF_وقت_T
/* assume default size of time_t to be 32 bits */
#وصف_ڪريو SIZEOF_وقت_T 4
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  إذا defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  وإلا
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#جيڪڏهن_پورو

#إذا SIZEOF_CURL_SOCKET_T < 8
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
#  define FMT_SOCKET_T "u"
#وإلا
#  define FMT_SOCKET_T "d"
#جيڪڏهن_پورو
#وإلاإذا defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#وإلا
#  define FMT_SOCKET_T "qd"
#جيڪڏهن_پورو

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#جيڪڏهن_وصف_نه_ٿيل SIZEOF_OFF_T
#  إذا defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    إذا defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    إذا defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#جيڪڏهن_پورو

#إذا SIZEOF_CURL_OFF_T < 8
#غلطي "too small curl_off_t"
#وإلا
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#جيڪڏهن_پورو
#وصف_ڪريو CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#وصف_ڪريو FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#وصف_ڪريو FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#إذا SIZEOF_وقت_T == 4
#  ifdef HAVE_وقت_T_UNSIGNED
#  define وقت_T_MAX UINT_MAX
#  define وقت_T_MIN 0
#  وإلا
#  define وقت_T_MAX وڌ_کان_وڌ_عدد
#  define وقت_T_MIN INT_MIN
#  endif
#وإلا
#  ifdef HAVE_وقت_T_UNSIGNED
#  define وقت_T_MAX 0xFFFFFFFFFFFFFFFF
#  define وقت_T_MIN 0
#  وإلا
#  define وقت_T_MAX 0x7FFFFFFFFFFFFFFF
#  define وقت_T_MIN (-وقت_T_MAX - 1)
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SIZE_MAX
/* some limits.h headers have this defined, some do not */
#إذا defined(SIZEOF_سائيز_T) && (SIZEOF_سائيز_T > 4)
#وصف_ڪريو SIZE_MAX 18446744073709551615U
#وإلا
#وصف_ڪريو SIZE_MAX 4294967295U
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#إذا defined(SIZEOF_سائيز_T) && (SIZEOF_سائيز_T > 4)
#وصف_ڪريو SSIZE_MAX 9223372036854775807
#وإلا
#وصف_ڪريو SSIZE_MAX 2147483647
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#إذا SIZEOF_LONG > SIZEOF_سائيز_T
#غلطي "unexpected: 'long' is larger than 'size_t'"
#جيڪڏهن_پورو

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#جيڪڏهن_وصف_نه_ٿيل GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 صحيح
#  وإلا
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#جيڪڏهن_پورو

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#جيڪڏهن_وصف_ٿيل _WIN32

#  define DIR_CHAR      "\\"

#وإلا /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (حرف *)(z))
#    include <tcp.h>
#    undef لفظ
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     خارجي بنية tm *gmtime_r(ثابت time_t * ثابت timep, بنية tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#جيڪڏهن_پورو /* _WIN32 */

/* We want to use mutex when available. */
#إذا defined(HAVE_ڌاڳا_POSIX) || defined(_WIN32)
#وصف_ڪريو USE_MUTEX
#جيڪڏهن_پورو

/* threaded resolver is the only feature requiring threads. */
#جيڪڏهن_وصف_ٿيل USE_RESOLV_THREADED
#وصف_ڪريو USE_ڌاڳا
#جيڪڏهن_پورو

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#إذا defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#وإلاإذا defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#وإلا
#  define CURLRES_IPV4
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#وإلاإذا defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#وإلا
#  define CURLRES_SYNCH
#جيڪڏهن_پورو

/* ---------------------------------------------------------------- */

#إذا defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#وصف_ڪريو USE_LIBIDN2
#جيڪڏهن_پورو

#إذا defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#غلطي "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#جيڪڏهن_پورو

#إذا defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#وصف_ڪريو USE_SSL    /* SSL support has been enabled */
#جيڪڏهن_پورو

#إذا defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#جيڪڏهن_وصف_نه_ٿيل OPENSSL_COEXIST
#وصف_ڪريو OPENSSL_COEXIST
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#إذا defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#وصف_ڪريو NO_OLD_WC_نالا
#جيڪڏهن_پورو

/* Single point where USE_SPNEGO definition might be defined */
#إذا !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#وصف_ڪريو USE_SPNEGO
#جيڪڏهن_پورو

/* Single point where USE_KERBEROS5 definition might be defined */
#إذا !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#وصف_ڪريو USE_KERBEROS5
#جيڪڏهن_پورو

/* Single point where USE_NTLM definition might be defined */
#جيڪڏهن_وصف_ٿيل CURL_ENABLE_NTLM
#  إذا (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  إذا defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#جيڪڏهن_پورو

#إذا defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#وصف_ڪريو USE_SSH
#جيڪڏهن_پورو

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#إذا defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#وصف_ڪريو CURL_HAVE_DIAG
#جيڪڏهن_پورو

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#إذا defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_اڻ_استعمال_ٿيل_نتيجو __attribute__((warn_اڻ_استعمال_ٿيل_نتيجو))
#وإلاإذا defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_اڻ_استعمال_ٿيل_نتيجو __attribute__((warn_اڻ_استعمال_ٿيل_نتيجو))
#وإلا
#  define WARN_اڻ_استعمال_ٿيل_نتيجو
#جيڪڏهن_پورو

/* noreturn attribute */

#جيڪڏهن_وصف_نه_ٿيل CURL_NORETURN
#إذا (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#وإلاإذا defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#وإلا
#  define CURL_NORETURN
#جيڪڏهن_پورو
#جيڪڏهن_پورو

/* fallthrough attribute */

#جيڪڏهن_وصف_نه_ٿيل FALLTHROUGH
#إذا (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#وإلا
#  define FALLTHROUGH()  افعل {} طالما(0)
#جيڪڏهن_پورو
#جيڪڏهن_پورو

/*
 * Inclusion of common header files.
 */

#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <stdarg.h>
#ضمن <time.h>
#ضمن <errno.h>

#ضمن <sys/stat.h>

#إذا !defined(_WIN32) || defined(__MINGW32__)
#ضمن <sys/time.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_IO_H
#ضمن <io.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_FCNTL_H
#ضمن <fcntl.h>
#جيڪڏهن_پورو

#إذا defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#ضمن <stdbool.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_UNISTD_H
#ضمن <unistd.h>
#جيڪڏهن_پورو

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#جيڪڏهن_وصف_ٿيل HAVE_UINTPTR_T
#وصف_ڪريو CURL_UNCONST(p) ((فراغ *)(uintptr_t)(ثابت فراغ *)(p))
#وإلا
#وصف_ڪريو CURL_UNCONST(p) ((فراغ *)(p))  /* Fall back to simple cast */
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __hpux
#  إذا !defined(XOPEN_ذريعو_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_ٻٽو_انگ_OFF_T
#      define OLD_APP32_64_ٻٽو_انگ_OFF_T APP32_64_ٻٽو_انگ_OFF_T
#      undef APP32_64_ٻٽو_انگ_OFF_T
#    وإلا
#      undef OLD_APP32_64_ٻٽو_انگ_OFF_T
#    endif
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل _WIN32
#ضمن <sys/socket.h>  /* also for MSG_NOSIGNAL */
#جيڪڏهن_پورو

#ضمن "functypes.h"

#جيڪڏهن_وصف_ٿيل __hpux
#  إذا !defined(XOPEN_ذريعو_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_ٻٽو_انگ_OFF_T
#      define APP32_64_ٻٽو_انگ_OFF_T OLD_APP32_64_ٻٽو_انگ_OFF_T
#      undef OLD_APP32_64_ٻٽو_انگ_OFF_T
#    endif
#  endif
#جيڪڏهن_پورو

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#جيڪڏهن_وصف_نه_ٿيل HAVE_STRUCT_TIMEVAL
بنية timeval {
  ڊگهو tv_sec;
  ڊگهو tv_usec;
};
#جيڪڏهن_پورو

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#جيڪڏهن_وصف_ٿيل MSG_NOSIGNAL
#وصف_ڪريو SEND_4TH_ARG MSG_NOSIGNAL
#وإلا
#وصف_ڪريو SEND_4TH_ARG 0
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __minix
/* Minix does not support recv on TCP sockets */
#وصف_ڪريو sread(x, y, z) (ssize_t)اقرأ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#وإلاإذا defined(HAVE_RECV)
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

#وصف_ڪريو sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#وإلا /* HAVE_RECV */
#جيڪڏهن_وصف_نه_ٿيل sread
#غلطي "Missing definition of macro sread"
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* HAVE_RECV */

#جيڪڏهن_وصف_ٿيل __minix
/* Minix does not support send on TCP sockets */
#وصف_ڪريو swrite(x, y, z) (ssize_t)اكتب((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#وإلاإذا defined(HAVE_SEND)
#جيڪڏهن_وصف_ٿيل SEND_NONCONST_ARG2
#وصف_ڪريو swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#وإلا
#وصف_ڪريو swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ثابت SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#جيڪڏهن_پورو /* SEND_NONCONST_ARG2 */
#وإلا /* HAVE_SEND */
#جيڪڏهن_وصف_نه_ٿيل swrite
#غلطي "Missing definition of macro swrite"
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#جيڪڏهن_وصف_ٿيل HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#وإلاإذا defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#وإلاإذا defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#وإلاإذا defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#وإلا
#  define CURL_SCLOSE(x)  close(x)
#جيڪڏهن_پورو

/*
 * Stack-independent version of fcntl() on sockets:
 */
#جيڪڏهن_وصف_ٿيل USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#وإلا
#  define sfcntl  fcntl
#جيڪڏهن_پورو

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#إذا defined(__hpux) && !defined(HAVE_BOOL_T)
تعريفنوع صحيح bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#جيڪڏهن_پورو

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#جيڪڏهن_وصف_نه_ٿيل HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
تعريفنوع تعداد {
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
#جيڪڏهن_پورو

/* the type we use for storing a single boolean bit */
تعريفنوع بنانشان صحيح curl_ٻٽو_انگ;
#وصف_ڪريو ٻٽو_انگ(x) curl_ٻٽو_انگ x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#جيڪڏهن_وصف_نه_ٿيل TRUE
#وصف_ڪريو TRUE true
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل FALSE
#وصف_ڪريو FALSE false
#جيڪڏهن_پورو

#ضمن "curl_ctype.h"

#إذا defined(DEBUGBUILD) && defined(NDEBUG)
#غلطي "Debug-enabled builds cannot be combined with NDEBUG"
#جيڪڏهن_پورو

/*
 * Macro used to include code only in debug builds.
 */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو DEBUGF(x) x
#وإلا
#وصف_ڪريو DEBUGF(x) افعل {} طالما(0)
#جيڪڏهن_پورو

/*
 * Macro used to include assertion code only in debug builds.
 */
#وصف_رد DEBUGASSERT
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#جيڪڏهن_وصف_ٿيل CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#وصف_ڪريو DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#وإلا
#وصف_ڪريو DEBUGASSERT(x) أكد(x)
#جيڪڏهن_پورو
#وإلا
#وصف_ڪريو DEBUGASSERT(x) افعل {} طالما(0)
#جيڪڏهن_پورو

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#إذا defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#ضمن <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#وصف_ڪريو CURL_MSVC_CRTDBG_INIT()                                             \
  افعل {                                                                      \
    سيٽ_error_گهڻائي(ٻاهريون_منزل_STDERR);  /* needs stdlib.h */                  \
    Crt_سيٽ_Report_گهڻائي(_CRT_ASSERT, CRTDBG_گهڻائي_FILE | CRTDBG_گهڻائي_DEBUG); \
    Crt_سيٽ_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_سيٽ_Report_گهڻائي(_CRT_ERROR, CRTDBG_گهڻائي_FILE | CRTDBG_گهڻائي_DEBUG);  \
    Crt_سيٽ_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_سيٽ_Report_گهڻائي(_CRT_WARN, CRTDBG_گهڻائي_FILE | CRTDBG_گهڻائي_DEBUG);   \
    Crt_سيٽ_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } طالما(0)
#وإلا
#وصف_ڪريو CURL_MSVC_CRTDBG_INIT() افعل {} طالما(0)
#جيڪڏهن_پورو

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#ضمن <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#وصف_ڪريو CURL_DUMP_MODULE_PATHS(printf_f)                               \
  افعل {                                                                 \
    HANDLE hnd = INVALID_HANDLE_قدر;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_سائيز = حجم(MODULEENTRY32);                                \
                                                                       \
    افعل {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } طالما(hnd == INVALID_HANDLE_قدر &&                             \
            Get_آخري_Error() == ERROR_BAD_ڊيگهائي);                       \
                                                                       \
    إذا(hnd == INVALID_HANDLE_قدر)                                    \
      اذهب error;                                                      \
                                                                       \
    إذا(!Module32_پهريون(hnd, &mod))                                      \
      اذهب error;                                                      \
                                                                       \
    افعل {                                                               \
      حرف بفر[حجم(mod.sz_Exe_رستو) * 2];                          \
      حرف *رستو; /* points to stack allocated buffer */               \
      إذا(حجم(*mod.sz_Exe_رستو) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        إذا(!Wide_Char_منزل_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_رستو, -1, \
                                بفر, حجم(بفر), عدم, عدم))   \
          اذهب error;                                                  \
        رستو = (حرف *)بفر;                                         \
      }                                                                \
      وإلا                                                             \
        رستو = (حرف *)mod.sz_Exe_رستو;                                  \
      (printf_f)("%s\n", رستو);                                        \
    } طالما(Module32_ايندڙ(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    إذا(hnd != INVALID_HANDLE_قدر)                                    \
      CloseHandle(hnd);                                                \
  } طالما(0)
#وإلا
#وصف_ڪريو CURL_DUMP_MODULE_PATHS(printf_f) افعل {} طالما(0)
#جيڪڏهن_پورو

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
#وصف_ڪريو SOCKERRNO         ((صحيح)WSAGet_آخري_Error())
#وصف_ڪريو سيٽ_SOCKERRNO(x)  WSASet_آخري_Error((صحيح)(x))
#وإلا
#وصف_ڪريو SOCKERRNO         errno
#وصف_ڪريو سيٽ_SOCKERRNO(x)  (errno = (x))
#جيڪڏهن_پورو

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
#وصف_ڪريو SOCKEACCES        WSAEACCES
#وصف_ڪريو SOCKEADDRINUSE    WSAEADDRINUSE
#وصف_ڪريو SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#وصف_ڪريو SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#وصف_ڪريو SOCKEBADF         WSAEBADF
#وصف_ڪريو SOCKECONNREFUSED  WSAECONNREFUSED
#وصف_ڪريو SOCKECONNRESET    WSAECONNRESET
#وصف_ڪريو SOCKEINPROGRESS   WSAEINPROGRESS
#وصف_ڪريو SOCKEINTR         WSAEINTR
#وصف_ڪريو SOCKEINVAL        WSAEINVAL
#وصف_ڪريو SOCKEISCONN       WSAEISCONN
#وصف_ڪريو SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#وصف_ڪريو SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#وصف_ڪريو SOCKETIMEDOUT     WSAETIMEDOUT
#وصف_ڪريو SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#وإلا
#وصف_ڪريو SOCKEACCES        EACCES
#وصف_ڪريو SOCKEADDRINUSE    EADDRINUSE
#وصف_ڪريو SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#وصف_ڪريو SOCKEAFNOSUPPORT  EAFNOSUPPORT
#وصف_ڪريو SOCKEBADF         EBADF
#وصف_ڪريو SOCKECONNREFUSED  ECONNREFUSED
#وصف_ڪريو SOCKECONNRESET    ECONNRESET
#وصف_ڪريو SOCKEINPROGRESS   EINPROGRESS
#وصف_ڪريو SOCKEINTR         EINTR
#وصف_ڪريو SOCKEINVAL        EINVAL
#وصف_ڪريو SOCKEISCONN       EISCONN
#وصف_ڪريو SOCKEMSGSIZE      EMSGSIZE
#وصف_ڪريو SOCKENOMEM        ENOMEM
#جيڪڏهن_وصف_ٿيل ETIMEDOUT
#وصف_ڪريو SOCKETIMEDOUT     ETIMEDOUT
#جيڪڏهن_پورو
#وصف_ڪريو SOCKEWOULDBLOCK   EWOULDBLOCK
#جيڪڏهن_پورو

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#إذا !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#وصف_ڪريو SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#وإلا
#وصف_ڪريو SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#جيڪڏهن_پورو

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#جيڪڏهن_وصف_ٿيل __VMS
#وصف_ڪريو argv_شي_t  __char_ptr32
#وإلاإذا defined(_UNICODE)
#وصف_ڪريو argv_شي_t  wchar_t *
#وإلا
#وصف_ڪريو argv_شي_t  حرف *
#جيڪڏهن_پورو

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#وصف_ڪريو ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#ضمن "curlx/warnless.h"

#جيڪڏهن_وصف_ٿيل _WIN32
#  undef اقرأ
#  define اقرأ(fd, buf, ڳڻپ)  (ssize_t)_read(fd, buf, curlx_uztoui(ڳڻپ))
#  undef اكتب
#  define اكتب(fd, buf, ڳڻپ) (ssize_t)_write(fd, buf, curlx_uztoui(ڳڻپ))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#جيڪڏهن_پورو

/*
 * Definition of our NOP statement Object-like macro
 */
#جيڪڏهن_وصف_نه_ٿيل Curl_nop_stmt
#وصف_ڪريو Curl_nop_stmt افعل {} طالما(0)
#جيڪڏهن_پورو

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#إذا defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  إذا defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#جيڪڏهن_پورو

/*
 * shutdown() flags for systems that do not define them
 */
#جيڪڏهن_وصف_نه_ٿيل SHUT_RD
#وصف_ڪريو SHUT_RD 0x00
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SHUT_WR
#وصف_ڪريو SHUT_WR 0x01
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SHUT_RDWR
#وصف_ڪريو SHUT_RDWR 0x02
#جيڪڏهن_پورو

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#إذا !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#وصف_ڪريو S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#جيڪڏهن_پورو

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#إذا !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#وصف_ڪريو S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#جيڪڏهن_پورو

/* For MSVC (all versions as of VS2022) */
#جيڪڏهن_وصف_نه_ٿيل STDIN_FILENO
#وصف_ڪريو STDIN_FILENO  fileno(stdin)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل STDOUT_FILENO
#وصف_ڪريو STDOUT_FILENO  fileno(stdout)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل STDERR_FILENO
#وصف_ڪريو STDERR_FILENO  fileno(stderr)
#جيڪڏهن_پورو

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#جيڪڏهن_وصف_ٿيل O_ٻٽو  /* for _WIN32 || MSDOS */
#وصف_ڪريو CURL_O_ٻٽو O_ٻٽو
#وإلاإذا defined(O_ٻٽو_2)  /* __CYGWIN__ */
#وصف_ڪريو CURL_O_ٻٽو O_ٻٽو_2
#وإلا
#وصف_ڪريو CURL_O_ٻٽو 0
#جيڪڏهن_پورو

/* Requires io.h when available */
#جيڪڏهن_وصف_ٿيل MSDOS
#وصف_ڪريو CURL_BINMODE(وهڪرو) (فراغ)setmode(fileno(وهڪرو), CURL_O_ٻٽو)
#وإلاإذا defined(_WIN32) || defined(__CYGWIN__)
#وصف_ڪريو CURL_BINMODE(وهڪرو) (فراغ)_setmode(fileno(وهڪرو), CURL_O_ٻٽو)
#وإلا
#وصف_ڪريو CURL_BINMODE(وهڪرو) (فراغ)(وهڪرو)
#جيڪڏهن_پورو

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#إذا defined(_WIN32) || defined(MSDOS)
#وصف_ڪريو FOPEN_READTEXT   "rt"
#وصف_ڪريو FOPEN_WRITETEXT  "wt"
#وصف_ڪريو FOPEN_APPENDTEXT "at"
#وإلاإذا defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#وصف_ڪريو FOPEN_READTEXT   "rt"
#وصف_ڪريو FOPEN_WRITETEXT  "w"
#وصف_ڪريو FOPEN_APPENDTEXT "a"
#وإلا
#وصف_ڪريو FOPEN_READTEXT   "r"
#وصف_ڪريو FOPEN_WRITETEXT  "w"
#وصف_ڪريو FOPEN_APPENDTEXT "a"
#جيڪڏهن_پورو

/* for systems that do not detect this in configure */
#جيڪڏهن_وصف_نه_ٿيل CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T پتو_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T بنانشان حرف
#  وإلا
/* use a sensible default */
#    define CURL_SA_FAMILY_T بنانشان ننڍو
#  endif
#جيڪڏهن_پورو

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#وصف_ڪريو CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#وصف_ڪريو CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#وصف_ڪريو CURL_CSTRLEN(x) (حجم(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#وصف_ڪريو STRCONST(x) x, CURL_CSTRLEN(x)

#وصف_ڪريو CURL_ARRAYSIZE(A) (حجم(A) / حجم((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#وصف_ڪريو STRERROR_LEN 256

#جيڪڏهن_وصف_نه_ٿيل CURL_DID_يادگيري_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
تعريفنوع فراغ *(*curl_malloc_callback)(size_t سائيز);
تعريفنوع فراغ (*curl_free_callback)(فراغ *ptr);
تعريفنوع فراغ *(*curl_realloc_callback)(فراغ *ptr, size_t سائيز);
تعريفنوع حرف *(*curl_strdup_callback)(ثابت حرف *str);
تعريفنوع فراغ *(*curl_calloc_callback)(size_t nmemb, size_t سائيز);
#وصف_ڪريو CURL_DID_يادگيري_FUNC_TYPEDEFS
#جيڪڏهن_پورو

خارجي curl_malloc_callback Curl_cmalloc;
خارجي curl_free_callback Curl_cfree;
خارجي curl_realloc_callback Curl_crealloc;
خارجي curl_strdup_callback Curl_cstrdup;
خارجي curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#وصف_ڪريو curlx_safefree(ptr) \
  افعل {                      \
    curlx_free(ptr);        \
    (ptr) = عدم;           \
  } طالما(0)

#ضمن <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو CURL_MEMDEBUG
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURL_MEMDEBUG
#جيڪڏهن_وصف_ٿيل __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  إذا __clang_major__ >= 4
#  define ALLOC_سائيز(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  وإلا
#  define ALLOC_سائيز(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#وإلاإذا defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_سائيز(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#وإلاإذا defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(محدود)
#  define ALLOC_سائيز(s)
#  define ALLOC_SIZE2(n, s)
#وإلا
#  define ALLOC_FUNC
#  define ALLOC_سائيز(s)
#  define ALLOC_SIZE2(n, s)
#جيڪڏهن_پورو

خارجي FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN فراغ curl_dbg_free(فراغ *ptr, صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_FUNC ALLOC_سائيز(1)
  فراغ *curl_dbg_malloc(size_t wantedsize, صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_سائيز,
                        صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_سائيز(2)
  فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize, صحيح سٽ,
                         ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_FUNC
  حرف *curl_dbg_strdup(ثابت حرف *str, صحيح سٽ, ثابت حرف *ذريعو);
#إذا defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح سٽ, ثابت حرف *ذريعو);
#جيڪڏهن_پورو

CURL_EXTERN فراغ curl_dbg_memdebug(ثابت حرف *logname);
CURL_EXTERN فراغ curl_dbg_memlimit(ڊگهو حد_بندي);
CURL_EXTERN فراغ curl_dbg_log(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                                          صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN فراغ curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN صحيح curl_dbg_sclose(curl_socket_t sockfd,
                                صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          فراغ *saddr, فراغ *saddrlen,
                                          صحيح سٽ, ثابت حرف *ذريعو);
#جيڪڏهن_وصف_ٿيل HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr,
                                           فراغ *saddrlen, صحيح flags,
                                           صحيح سٽ, ثابت حرف *ذريعو);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SOCKETPAIR
CURL_EXTERN صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                                    curl_socket_t socket_ويڪٽر[2],
                                    صحيح سٽ, ثابت حرف *ذريعو);
#جيڪڏهن_پورو

/* FILE functions */
CURL_EXTERN صحيح curl_dbg_fclose(FILE *file, صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *گهڻائي,
                                            صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ثابت حرف *file,
                                              ثابت حرف *گهڻائي, FILE *fh,
                                              صحيح سٽ, ثابت حرف *ذريعو);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *گهڻائي,
                                             صحيح سٽ, ثابت حرف *ذريعو);

#وصف_ڪريو sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#وصف_ڪريو fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#وصف_ڪريو CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#وصف_ڪريو CURL_FREEADDRINFO(معلومات) \
  curl_dbg_freeaddrinfo(معلومات, __LINE__, __FILE__)
#وصف_ڪريو CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((صحيح)(domain), type, protocol, __LINE__, __FILE__)
#جيڪڏهن_وصف_ٿيل HAVE_SOCKETPAIR
#وصف_ڪريو CURL_SOCKETPAIR(domain, type, protocol, socket_ويڪٽر) \
  curl_dbg_socketpair((صحيح)(domain), type, protocol, socket_ويڪٽر, \
                      __LINE__, __FILE__)
#جيڪڏهن_پورو
#وصف_ڪريو CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#جيڪڏهن_وصف_ٿيل HAVE_ACCEPT4
#وصف_ڪريو CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#جيڪڏهن_پورو

#وإلا /* !CURL_MEMDEBUG */

#وصف_ڪريو sclose(x) CURL_SCLOSE(x)
#وصف_ڪريو fake_sclose(x) Curl_nop_stmt

#وصف_ڪريو CURL_GETADDRINFO getaddrinfo
#وصف_ڪريو CURL_FREEADDRINFO freeaddrinfo
#وصف_ڪريو CURL_SOCKET socket
#جيڪڏهن_وصف_ٿيل HAVE_SOCKETPAIR
#وصف_ڪريو CURL_SOCKETPAIR socketpair
#جيڪڏهن_پورو
#وصف_ڪريو CURL_ACCEPT accept
#جيڪڏهن_وصف_ٿيل HAVE_ACCEPT4
#وصف_ڪريو CURL_ACCEPT4 accept4
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CURL_MEMDEBUG */

/* Allocator macros */

#جيڪڏهن_وصف_ٿيل _WIN32
#وصف_ڪريو CURLX_STRDUP_هيٺيون _strdup
#وإلا
#وصف_ڪريو CURLX_STRDUP_هيٺيون strdup
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURL_MEMDEBUG

#وصف_ڪريو curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#وصف_ڪريو curlx_malloc(سائيز)         curl_dbg_malloc(سائيز, __LINE__, __FILE__)
#وصف_ڪريو curlx_calloc(nbelem, سائيز) \
  curl_dbg_calloc(nbelem, سائيز, __LINE__, __FILE__)
#وصف_ڪريو curlx_realloc(ptr, سائيز) \
  curl_dbg_realloc(ptr, سائيز, __LINE__, __FILE__)
#وصف_ڪريو curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#جيڪڏهن_وصف_ٿيل _WIN32
#جيڪڏهن_وصف_ٿيل UNICODE
#وصف_ڪريو curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#وإلا
#وصف_ڪريو curlx_tcsdup               curlx_strdup
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* _WIN32 */

#وإلا /* !CURL_MEMDEBUG */

#جيڪڏهن_وصف_ٿيل BUILDING_LIBCURL
#وصف_ڪريو curlx_strdup               Curl_cstrdup
#وصف_ڪريو curlx_malloc               Curl_cmalloc
#وصف_ڪريو curlx_calloc               Curl_ccalloc
#وصف_ڪريو curlx_realloc              Curl_crealloc
#وصف_ڪريو curlx_free                 Curl_cfree
#وإلا /* !BUILDING_LIBCURL */
#وصف_ڪريو curlx_strdup               CURLX_STRDUP_هيٺيون
#وصف_ڪريو curlx_malloc               يادگيري_ڏيو
#وصف_ڪريو curlx_calloc               صفر_يادگيري_ڏيو
#وصف_ڪريو curlx_realloc              realloc
#وصف_ڪريو curlx_free                 حرر
#جيڪڏهن_پورو /* BUILDING_LIBCURL */

#جيڪڏهن_وصف_ٿيل _WIN32
#جيڪڏهن_وصف_ٿيل UNICODE
#وصف_ڪريو curlx_tcsdup               curlx_wcsdup
#وإلا
#وصف_ڪريو curlx_tcsdup               curlx_strdup
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* _WIN32 */

#جيڪڏهن_پورو /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#إذا defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
بنية passwd;
صحيح getpwuid_r(uid_t uid, بنية passwd *pwd, حرف *buf,
               size_t buflen, بنية passwd **نتيجو);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل UNITTESTS
#وصف_ڪريو UNITTEST
#وإلا
#وصف_ڪريو UNITTEST ساكن
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NGHTTP2
#وصف_ڪريو USE_HTTP2
#جيڪڏهن_پورو

#إذا (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
#غلطي "MultiSSL combined with QUIC is not supported"
#جيڪڏهن_پورو

#وصف_ڪريو USE_HTTP3
#جيڪڏهن_پورو

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#جيڪڏهن_وصف_نه_ٿيل __EMSCRIPTEN__
#وصف_ڪريو CURL_TCP_NODELAY_SUPPORTED
#جيڪڏهن_پورو

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#إذا defined(HAVE_BASENAME) && defined(_WIN32)
#وصف_رد HAVE_BASENAME
#جيڪڏهن_پورو

#إذا defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#وإلاإذا !defined(UNIX_رستو_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_رستو_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  حرف sun_رستو[UNIX_رستو_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_مقرر_ڪري_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_مقرر_ڪري_WARNING
#    endif
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#وإلاإذا defined(مضمن)
#  define CURL_INLINE مضمن /* 'inline' defined, assumed correct */
#وإلاإذا defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE مضمن /* 'inline' keyword supported */
#وإلاإذا defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE مضمن /* 'inline' keyword supported */
#وإلاإذا defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#وإلاإذا defined(_MSC_VER)
#  define CURL_INLINE __inline
#وإلا
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /*خالي_جايون*/
#جيڪڏهن_پورو

/* Detect if compiler supports C99 variadic macros */
#إذا (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#وصف_ڪريو CURL_HAVE_MACRO_VARARG
#جيڪڏهن_پورو

#إذا !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_لڙيون))
#وصف_ڪريو CURLVERBOSE
#وصف_ڪريو VERBOSE(x) x
#وصف_ڪريو NOVERBOSE(x) Curl_nop_stmt
#وإلا
#وصف_ڪريو VERBOSE(x) Curl_nop_stmt
#وصف_ڪريو NOVERBOSE(x) x
#جيڪڏهن_پورو

/* For FreeBSD it is included from curl/curl.h */
#إذا defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#ضمن <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#جيڪڏهن_پورو

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#إذا defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#وصف_ڪريو CURL_USE_SOCK_NONBLOCK
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#جيڪڏهن_وصف_ٿيل _WIN32
#إذا defined(_MSC_VER) && defined(NTDDI_نسخو) && \
  (NTDDI_نسخو >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#هدايت comment(lib, "volatileaccessu.lib")
#وصف_ڪريو curlx_memzero_هيٺيون(buf, سائيز)  SecureZeroMemory2(buf, سائيز)
#وإلا
#وصف_ڪريو curlx_memzero_هيٺيون(buf, سائيز)  Secure_Zero_يادگيري(buf, سائيز)
#جيڪڏهن_پورو
#وإلاإذا defined(HAVE_MEMSET_S)
#وصف_ڪريو curlx_memzero_هيٺيون(buf, سائيز)  (فراغ)memset_s(buf, سائيز, 0, سائيز)
#وإلاإذا defined(HAVE_MEMSET_EXPLICIT)
#وصف_ڪريو curlx_memzero_هيٺيون(buf, سائيز)  (فراغ)memset_explicit(buf, 0, سائيز)
#وإلاإذا defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#وصف_ڪريو curlx_memzero_هيٺيون(buf, سائيز)  explicit_bzero(buf, سائيز)
#وإلاإذا defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#وصف_ڪريو curlx_memzero_هيٺيون(buf, سائيز)  (فراغ)explicit_memset(buf, 0, سائيز)
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* !_CURL_LOCAL_MEMZERO */

#جيڪڏهن_وصف_نه_ٿيل curlx_memzero_هيٺيون
#وصف_ڪريو USE_CURLX_MEMZERO
فراغ curlx_memzero_هيٺيون(فراغ *buf, size_t سائيز);
#جيڪڏهن_پورو
فراغ curlx_memzero(فراغ *buf, size_t سائيز);
فراغ curlx_strzero(فراغ *buf);

#جيڪڏهن_پورو /* HEADER_CURL_SETUP_H */
