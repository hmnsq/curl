#اگر_غیر_تعریف_شدہ سرخی_CURL_تیاری_H
#تعریف_کرو سرخی_CURL_تیاری_H
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

#اگر defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#تعریف_کرو CURL_NO_OLDIES
#اگر_ختم

/* Set default _WIN32_WINNT */
#اگر_تعریف_شدہ __MINGW32__
#شامل <_mingw.h>
#اگر_ختم

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
#اگر defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  اگر !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#اگر_ختم

#اگر_تعریف_شدہ __APPLE__
#شامل <sys/types.h>
#شامل <TargetConditionals.h>
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
#اگر defined(ہدف_OS_MAC) && ہدف_OS_MAC && \
  defined(ہدف_OS_OSX) && !ہدف_OS_OSX && \
  (!defined(ہدف_OS_IPHONE) || !ہدف_OS_IPHONE) && \
  (!defined(ہدف_OS_SIMULATOR) || !ہدف_OS_SIMULATOR)
#تعریف_منسوخ ہدف_OS_OSX
#تعریف_کرو ہدف_OS_OSX ہدف_OS_MAC
#اگر_ختم
#اگر_ختم

#اگر defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#غلطی "mingw-w64 3.0 or greater required"
#اگر_ختم

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#اگر defined(_MSC_VER) && (_MSC_VER < 1600)
#غلطی "Ancient versions of Visual Studio are no longer supported due to bugs."
#اگر_ختم

#اگر_تعریف_شدہ _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#ہدایت warning(disable:4127)
#اگر_غیر_تعریف_شدہ _CRT_SECURE_NO_WARNINGS
#تعریف_کرو _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#اگر_ختم
#اگر_ختم /* _MSC_VER */

#اگر_تعریف_شدہ _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_اوسط
#  define WIN32_LEAN_AND_اوسط
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  اگر (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    اگر WINAPI_FAMILY_تقسیم(WINAPI_تقسیم_APP) &&  \
       !WINAPI_FAMILY_تقسیم(WINAPI_تقسیم_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#تعریف_منسوخ SECURITY_KERNEL
#تعریف_منسوخ SECURITY_WIN32
#تعریف_کرو SECURITY_WIN32  /* for <sspi.h> */
#اگر_ختم

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#اگر_تعریف_شدہ HAVE_CONFIG_H

#شامل "curl_config.h"

#ورنہ /* HAVE_CONFIG_H */

#اگر_تعریف_شدہ _WIN32
#  include "config-win32.h"
#اگر_ختم

#اگر_تعریف_شدہ macintosh
#  include "config-mac.h"
#اگر_ختم

#اگر_تعریف_شدہ __OS400__
#  include "config-os400.h"
#اگر_ختم

#اگر_ختم /* HAVE_CONFIG_H */

#اگر_تعریف_شدہ _WIN32
#  اگر defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build ہدف is Windows Vista (0x0600)
#  endif

#  اگر !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#اگر_ختم

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#اگر_تعریف_شدہ HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#اگر_ختم

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#اگر_تعریف_شدہ NEED_THREAD_محفوظ
#  ifndef THREAD_محفوظ
#  define THREAD_محفوظ
#  endif
#اگر_ختم

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#اگر_تعریف_شدہ NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#اگر_ختم

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#اگر defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#اگر_ختم

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#اگر_تعریف_شدہ HAVE_دھاگے_POSIX
#  include <pthread.h>
#اگر_ختم

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#اگر_تعریف_شدہ CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_سرخیاں_API
#  define CURL_DISABLE_سرخیاں_API
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
#اگر_ختم

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#اگر_تعریف_شدہ __OS400__
#  include "setup-os400.h"
#اگر_ختم

/*
 * VMS setup file includes some system headers.
 */
#اگر_تعریف_شدہ __VMS
#  include "setup-vms.h"
#اگر_ختم

/*
 * Windows setup file includes some system headers.
 */
#اگر_تعریف_شدہ _WIN32
#  include "setup-win32.h"
#اگر_ختم

#شامل <curl/system.h>

#اگر !defined(HAVE_PIPE2) && \
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
#تعریف_کرو HAVE_PIPE2
#اگر_ختم

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#تعریف_کرو CURL_CONC_MACROS_(A, B) A ## B
#تعریف_کرو CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#اگر_تعریف_شدہ __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#اگر_ختم

/* based on logic in "curl/mprintf.h" */
#اگر (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#اگر defined(__MINGW32__) && !defined(__clang__)
#تعریف_کرو CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#ورنہ
#تعریف_کرو CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#اگر_ختم
#ورنہ
#تعریف_کرو CURL_PRINTF(fmt, arg)
#اگر_ختم

/* Override default printf mask check rules in "curl/mprintf.h" */
#تعریف_کرو CURL_عارضی_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#اگر defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#تعریف_کرو __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#اگر_ختم

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#اگر defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  اگر ہدف_OS_MAC && !(defined(ہدف_OS_IPHONE) && ہدف_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#اگر_ختم

#اگر_تعریف_شدہ USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  اگر defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#اگر_ختم

#اگر_تعریف_شدہ USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#اگر_ختم

#اگر_تعریف_شدہ macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#اگر_ختم

#اگر_تعریف_شدہ __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    اگر !(defined(__NEWLIB__) || \
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
#  اگر defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       صحیح Curl_amiga_select(صحیح nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, ساخت timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    ورنہ
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  ورنہ
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#اگر_ختم

/* Whether to use eventfd() */
#اگر defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#تعریف_کرو USE_EVENTFD
#اگر_ختم

#اگر_تعریف_شدہ SO_NOSIGPIPE
#تعریف_کرو USE_SO_NOSIGPIPE
#اگر_ختم

#شامل <stdio.h>
#شامل <assert.h>

#اگر_تعریف_شدہ __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#اگر_ختم

#شامل <stdint.h>
#تعریف_کرو HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#اگر_تعریف_شدہ __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#تعریف_کرو uint32_t غیرعلامتی صحیح
#اگر_ختم

/* Disable uintptr_t for targets known to miss it from stdint.h */
#اگر_تعریف_شدہ __OS400__
#تعریف_منسوخ HAVE_UINTPTR_T
#اگر_ختم

#شامل <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#شامل <sys/types.h>

#اگر_تعریف_شدہ _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#وگرنہ_اگر defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((انحراف_t)-1)
#وگرنہ_اگر defined(__AMIGA__)
#  define curl_lseek(fd, انحراف, whence)  lseek(fd, (off_t)(انحراف), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#ورنہ
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#اگر_ختم

#اگر_غیر_تعریف_شدہ SIZEOF_وقت_T
/* assume default size of time_t to be 32 bits */
#تعریف_کرو SIZEOF_وقت_T 4
#اگر_ختم

#اگر_غیر_تعریف_شدہ SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  اگر defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  ورنہ
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#اگر_ختم

#اگر SIZEOF_CURL_SOCKET_T < 8
#اگر_تعریف_شدہ USE_WINSOCK
#  define FMT_SOCKET_T "u"
#ورنہ
#  define FMT_SOCKET_T "d"
#اگر_ختم
#وگرنہ_اگر defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#ورنہ
#  define FMT_SOCKET_T "qd"
#اگر_ختم

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#اگر_غیر_تعریف_شدہ SIZEOF_OFF_T
#  اگر defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    اگر defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    اگر defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#اگر_ختم

#اگر SIZEOF_CURL_OFF_T < 8
#غلطی "too small curl_off_t"
#ورنہ
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#اگر_ختم
#تعریف_کرو CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#تعریف_کرو FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#تعریف_کرو FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#اگر SIZEOF_وقت_T == 4
#  ifdef HAVE_وقت_T_UNSIGNED
#  define وقت_T_MAX UINT_MAX
#  define وقت_T_MIN 0
#  ورنہ
#  define وقت_T_MAX زیادہ_سے_زیادہ_عدد
#  define وقت_T_MIN INT_MIN
#  endif
#ورنہ
#  ifdef HAVE_وقت_T_UNSIGNED
#  define وقت_T_MAX 0xFFFFFFFFFFFFFFFF
#  define وقت_T_MIN 0
#  ورنہ
#  define وقت_T_MAX 0x7FFFFFFFFFFFFFFF
#  define وقت_T_MIN (-وقت_T_MAX - 1)
#  endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ SIZE_MAX
/* some limits.h headers have this defined, some do not */
#اگر defined(SIZEOF_سائز_T) && (SIZEOF_سائز_T > 4)
#تعریف_کرو SIZE_MAX 18446744073709551615U
#ورنہ
#تعریف_کرو SIZE_MAX 4294967295U
#اگر_ختم
#اگر_ختم

#اگر_غیر_تعریف_شدہ SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#اگر defined(SIZEOF_سائز_T) && (SIZEOF_سائز_T > 4)
#تعریف_کرو SSIZE_MAX 9223372036854775807
#ورنہ
#تعریف_کرو SSIZE_MAX 2147483647
#اگر_ختم
#اگر_ختم

#اگر SIZEOF_LONG > SIZEOF_سائز_T
#غلطی "unexpected: 'long' is larger than 'size_t'"
#اگر_ختم

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#اگر_غیر_تعریف_شدہ GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 صحیح
#  ورنہ
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#اگر_ختم

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#اگر_تعریف_شدہ _WIN32

#  define DIR_CHAR      "\\"

#ورنہ /* _WIN32 */

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
     خارجی ساخت tm *gmtime_r(ثابت time_t * ثابت timep, ساخت tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#اگر_ختم /* _WIN32 */

/* We want to use mutex when available. */
#اگر defined(HAVE_دھاگے_POSIX) || defined(_WIN32)
#تعریف_کرو USE_MUTEX
#اگر_ختم

/* threaded resolver is the only feature requiring threads. */
#اگر_تعریف_شدہ USE_RESOLV_THREADED
#تعریف_کرو USE_دھاگے
#اگر_ختم

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#اگر defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#وگرنہ_اگر defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#ورنہ
#  define CURLRES_IPV4
#اگر_ختم

#اگر_تعریف_شدہ USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#وگرنہ_اگر defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#ورنہ
#  define CURLRES_SYNCH
#اگر_ختم

/* ---------------------------------------------------------------- */

#اگر defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#تعریف_کرو USE_LIBIDN2
#اگر_ختم

#اگر defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#غلطی "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#اگر_ختم

#اگر defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#تعریف_کرو USE_SSL    /* SSL support has been enabled */
#اگر_ختم

#اگر defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#اگر_غیر_تعریف_شدہ OPENSSL_COEXIST
#تعریف_کرو OPENSSL_COEXIST
#اگر_ختم
#اگر_ختم

#اگر defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#تعریف_کرو NO_OLD_WC_نام
#اگر_ختم

/* Single point where USE_SPNEGO definition might be defined */
#اگر !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#تعریف_کرو USE_SPNEGO
#اگر_ختم

/* Single point where USE_KERBEROS5 definition might be defined */
#اگر !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#تعریف_کرو USE_KERBEROS5
#اگر_ختم

/* Single point where USE_NTLM definition might be defined */
#اگر_تعریف_شدہ CURL_ENABLE_NTLM
#  اگر (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  اگر defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#اگر_ختم

#اگر defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#تعریف_کرو USE_SSH
#اگر_ختم

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#اگر defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#تعریف_کرو CURL_HAVE_DIAG
#اگر_ختم

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#اگر defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_غیر_مستعمل_نتیجہ __attribute__((warn_غیر_مستعمل_نتیجہ))
#وگرنہ_اگر defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_غیر_مستعمل_نتیجہ __attribute__((warn_غیر_مستعمل_نتیجہ))
#ورنہ
#  define WARN_غیر_مستعمل_نتیجہ
#اگر_ختم

/* noreturn attribute */

#اگر_غیر_تعریف_شدہ CURL_NORETURN
#اگر (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#وگرنہ_اگر defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#ورنہ
#  define CURL_NORETURN
#اگر_ختم
#اگر_ختم

/* fallthrough attribute */

#اگر_غیر_تعریف_شدہ FALLTHROUGH
#اگر (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#ورنہ
#  define FALLTHROUGH()  کرو {} جب(0)
#اگر_ختم
#اگر_ختم

/*
 * Inclusion of common header files.
 */

#شامل <stdlib.h>
#شامل <string.h>
#شامل <stdarg.h>
#شامل <time.h>
#شامل <errno.h>

#شامل <sys/stat.h>

#اگر !defined(_WIN32) || defined(__MINGW32__)
#شامل <sys/time.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_IO_H
#شامل <io.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_FCNTL_H
#شامل <fcntl.h>
#اگر_ختم

#اگر defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#شامل <stdbool.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_UNISTD_H
#شامل <unistd.h>
#اگر_ختم

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#اگر_تعریف_شدہ HAVE_UINTPTR_T
#تعریف_کرو CURL_UNCONST(p) ((خالی *)(uintptr_t)(ثابت خالی *)(p))
#ورنہ
#تعریف_کرو CURL_UNCONST(p) ((خالی *)(p))  /* Fall back to simple cast */
#اگر_ختم

#اگر_تعریف_شدہ USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#اگر_ختم

#اگر_تعریف_شدہ __hpux
#  اگر !defined(XOPEN_ماخذ_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_ثنائی_ہندسہ_OFF_T
#      define OLD_APP32_64_ثنائی_ہندسہ_OFF_T APP32_64_ثنائی_ہندسہ_OFF_T
#      undef APP32_64_ثنائی_ہندسہ_OFF_T
#    ورنہ
#      undef OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#    endif
#  endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ _WIN32
#شامل <sys/socket.h>  /* also for MSG_NOSIGNAL */
#اگر_ختم

#شامل "functypes.h"

#اگر_تعریف_شدہ __hpux
#  اگر !defined(XOPEN_ماخذ_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#      define APP32_64_ثنائی_ہندسہ_OFF_T OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#      undef OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#    endif
#  endif
#اگر_ختم

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#اگر_غیر_تعریف_شدہ HAVE_STRUCT_TIMEVAL
ساخت timeval {
  طویل tv_sec;
  طویل tv_usec;
};
#اگر_ختم

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#اگر_تعریف_شدہ MSG_NOSIGNAL
#تعریف_کرو SEND_4TH_ARG MSG_NOSIGNAL
#ورنہ
#تعریف_کرو SEND_4TH_ARG 0
#اگر_ختم

#اگر_تعریف_شدہ __minix
/* Minix does not support recv on TCP sockets */
#تعریف_کرو sread(x, y, z) (ssize_t)اقرأ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#وگرنہ_اگر defined(HAVE_RECV)
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

#تعریف_کرو sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#ورنہ /* HAVE_RECV */
#اگر_غیر_تعریف_شدہ sread
#غلطی "Missing definition of macro sread"
#اگر_ختم
#اگر_ختم /* HAVE_RECV */

#اگر_تعریف_شدہ __minix
/* Minix does not support send on TCP sockets */
#تعریف_کرو swrite(x, y, z) (ssize_t)اكتب((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#وگرنہ_اگر defined(HAVE_SEND)
#اگر_تعریف_شدہ SEND_NONCONST_ARG2
#تعریف_کرو swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#ورنہ
#تعریف_کرو swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ثابت SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#اگر_ختم /* SEND_NONCONST_ARG2 */
#ورنہ /* HAVE_SEND */
#اگر_غیر_تعریف_شدہ swrite
#غلطی "Missing definition of macro swrite"
#اگر_ختم
#اگر_ختم /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#اگر_تعریف_شدہ HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#وگرنہ_اگر defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#وگرنہ_اگر defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#وگرنہ_اگر defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#ورنہ
#  define CURL_SCLOSE(x)  close(x)
#اگر_ختم

/*
 * Stack-independent version of fcntl() on sockets:
 */
#اگر_تعریف_شدہ USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#ورنہ
#  define sfcntl  fcntl
#اگر_ختم

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#اگر defined(__hpux) && !defined(HAVE_BOOL_T)
قسمتعریف صحیح bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#اگر_ختم

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#اگر_غیر_تعریف_شدہ HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
قسمتعریف شمار {
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
#اگر_ختم

/* the type we use for storing a single boolean bit */
قسمتعریف غیرعلامتی صحیح curl_ثنائی_ہندسہ;
#تعریف_کرو ثنائی_ہندسہ(x) curl_ثنائی_ہندسہ x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#اگر_غیر_تعریف_شدہ TRUE
#تعریف_کرو TRUE true
#اگر_ختم
#اگر_غیر_تعریف_شدہ FALSE
#تعریف_کرو FALSE false
#اگر_ختم

#شامل "curl_ctype.h"

#اگر defined(DEBUGBUILD) && defined(NDEBUG)
#غلطی "Debug-enabled builds cannot be combined with NDEBUG"
#اگر_ختم

/*
 * Macro used to include code only in debug builds.
 */
#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو DEBUGF(x) x
#ورنہ
#تعریف_کرو DEBUGF(x) کرو {} جب(0)
#اگر_ختم

/*
 * Macro used to include assertion code only in debug builds.
 */
#تعریف_منسوخ DEBUGASSERT
#اگر_تعریف_شدہ DEBUGBUILD
#اگر_تعریف_شدہ CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#تعریف_کرو DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#ورنہ
#تعریف_کرو DEBUGASSERT(x) أكد(x)
#اگر_ختم
#ورنہ
#تعریف_کرو DEBUGASSERT(x) کرو {} جب(0)
#اگر_ختم

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#اگر defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#شامل <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#تعریف_کرو CURL_MSVC_CRTDBG_INIT()                                             \
  کرو {                                                                      \
    سیٹ_error_منوال(برآمد_مقصد_STDERR);  /* needs stdlib.h */                  \
    Crt_سیٹ_Report_منوال(_CRT_ASSERT, CRTDBG_منوال_FILE | CRTDBG_منوال_DEBUG); \
    Crt_سیٹ_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_سیٹ_Report_منوال(_CRT_ERROR, CRTDBG_منوال_FILE | CRTDBG_منوال_DEBUG);  \
    Crt_سیٹ_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_سیٹ_Report_منوال(_CRT_WARN, CRTDBG_منوال_FILE | CRTDBG_منوال_DEBUG);   \
    Crt_سیٹ_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } جب(0)
#ورنہ
#تعریف_کرو CURL_MSVC_CRTDBG_INIT() کرو {} جب(0)
#اگر_ختم

#اگر defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#شامل <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#تعریف_کرو CURL_DUMP_MODULE_PATHS(printf_f)                               \
  کرو {                                                                 \
    HANDLE hnd = INVALID_HANDLE_قدر;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_سائز = حجم(MODULEENTRY32);                                \
                                                                       \
    کرو {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } جب(hnd == INVALID_HANDLE_قدر &&                             \
            Get_آخری_Error() == ERROR_BAD_لمبائی);                       \
                                                                       \
    اگر(hnd == INVALID_HANDLE_قدر)                                    \
      جاؤ error;                                                      \
                                                                       \
    اگر(!Module32_پہلا(hnd, &mod))                                      \
      جاؤ error;                                                      \
                                                                       \
    کرو {                                                               \
      حرف بفر[حجم(mod.sz_Exe_راستہ) * 2];                          \
      حرف *راستہ; /* points to stack allocated buffer */               \
      اگر(حجم(*mod.sz_Exe_راستہ) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        اگر(!Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_راستہ, -1, \
                                بفر, حجم(بفر), تہی, تہی))   \
          جاؤ error;                                                  \
        راستہ = (حرف *)بفر;                                         \
      }                                                                \
      ورنہ                                                             \
        راستہ = (حرف *)mod.sz_Exe_راستہ;                                  \
      (printf_f)("%s\n", راستہ);                                        \
    } جب(Module32_اگلا(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    اگر(hnd != INVALID_HANDLE_قدر)                                    \
      CloseHandle(hnd);                                                \
  } جب(0)
#ورنہ
#تعریف_کرو CURL_DUMP_MODULE_PATHS(printf_f) کرو {} جب(0)
#اگر_ختم

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#اگر_تعریف_شدہ USE_WINSOCK
#تعریف_کرو SOCKERRNO         ((صحیح)WSAGet_آخری_Error())
#تعریف_کرو سیٹ_SOCKERRNO(x)  WSASet_آخری_Error((صحیح)(x))
#ورنہ
#تعریف_کرو SOCKERRNO         errno
#تعریف_کرو سیٹ_SOCKERRNO(x)  (errno = (x))
#اگر_ختم

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#اگر_تعریف_شدہ USE_WINSOCK
#تعریف_کرو SOCKEACCES        WSAEACCES
#تعریف_کرو SOCKEADDRINUSE    WSAEADDRINUSE
#تعریف_کرو SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#تعریف_کرو SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#تعریف_کرو SOCKEBADF         WSAEBADF
#تعریف_کرو SOCKECONNREFUSED  WSAECONNREFUSED
#تعریف_کرو SOCKECONNRESET    WSAECONNRESET
#تعریف_کرو SOCKEINPROGRESS   WSAEINPROGRESS
#تعریف_کرو SOCKEINTR         WSAEINTR
#تعریف_کرو SOCKEINVAL        WSAEINVAL
#تعریف_کرو SOCKEISCONN       WSAEISCONN
#تعریف_کرو SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#تعریف_کرو SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#تعریف_کرو SOCKETIMEDOUT     WSAETIMEDOUT
#تعریف_کرو SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#ورنہ
#تعریف_کرو SOCKEACCES        EACCES
#تعریف_کرو SOCKEADDRINUSE    EADDRINUSE
#تعریف_کرو SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#تعریف_کرو SOCKEAFNOSUPPORT  EAFNOSUPPORT
#تعریف_کرو SOCKEBADF         EBADF
#تعریف_کرو SOCKECONNREFUSED  ECONNREFUSED
#تعریف_کرو SOCKECONNRESET    ECONNRESET
#تعریف_کرو SOCKEINPROGRESS   EINPROGRESS
#تعریف_کرو SOCKEINTR         EINTR
#تعریف_کرو SOCKEINVAL        EINVAL
#تعریف_کرو SOCKEISCONN       EISCONN
#تعریف_کرو SOCKEMSGSIZE      EMSGSIZE
#تعریف_کرو SOCKENOMEM        ENOMEM
#اگر_تعریف_شدہ ETIMEDOUT
#تعریف_کرو SOCKETIMEDOUT     ETIMEDOUT
#اگر_ختم
#تعریف_کرو SOCKEWOULDBLOCK   EWOULDBLOCK
#اگر_ختم

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#اگر !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#تعریف_کرو SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#ورنہ
#تعریف_کرو SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#اگر_ختم

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#اگر_تعریف_شدہ __VMS
#تعریف_کرو argv_شے_t  __char_ptr32
#وگرنہ_اگر defined(_UNICODE)
#تعریف_کرو argv_شے_t  wchar_t *
#ورنہ
#تعریف_کرو argv_شے_t  حرف *
#اگر_ختم

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#تعریف_کرو ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#شامل "curlx/warnless.h"

#اگر_تعریف_شدہ _WIN32
#  undef اقرأ
#  define اقرأ(fd, buf, گنتی)  (ssize_t)_read(fd, buf, curlx_uztoui(گنتی))
#  undef اكتب
#  define اكتب(fd, buf, گنتی) (ssize_t)_write(fd, buf, curlx_uztoui(گنتی))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#اگر_ختم

/*
 * Definition of our NOP statement Object-like macro
 */
#اگر_غیر_تعریف_شدہ Curl_nop_stmt
#تعریف_کرو Curl_nop_stmt کرو {} جب(0)
#اگر_ختم

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#اگر defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  اگر defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#اگر_ختم

/*
 * shutdown() flags for systems that do not define them
 */
#اگر_غیر_تعریف_شدہ SHUT_RD
#تعریف_کرو SHUT_RD 0x00
#اگر_ختم

#اگر_غیر_تعریف_شدہ SHUT_WR
#تعریف_کرو SHUT_WR 0x01
#اگر_ختم

#اگر_غیر_تعریف_شدہ SHUT_RDWR
#تعریف_کرو SHUT_RDWR 0x02
#اگر_ختم

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#اگر !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#تعریف_کرو S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#اگر_ختم

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#اگر !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#تعریف_کرو S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#اگر_ختم

/* For MSVC (all versions as of VS2022) */
#اگر_غیر_تعریف_شدہ STDIN_FILENO
#تعریف_کرو STDIN_FILENO  fileno(stdin)
#اگر_ختم
#اگر_غیر_تعریف_شدہ STDOUT_FILENO
#تعریف_کرو STDOUT_FILENO  fileno(stdout)
#اگر_ختم
#اگر_غیر_تعریف_شدہ STDERR_FILENO
#تعریف_کرو STDERR_FILENO  fileno(stderr)
#اگر_ختم

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#اگر_تعریف_شدہ O_ثنائی  /* for _WIN32 || MSDOS */
#تعریف_کرو CURL_O_ثنائی O_ثنائی
#وگرنہ_اگر defined(O_ثنائی_2)  /* __CYGWIN__ */
#تعریف_کرو CURL_O_ثنائی O_ثنائی_2
#ورنہ
#تعریف_کرو CURL_O_ثنائی 0
#اگر_ختم

/* Requires io.h when available */
#اگر_تعریف_شدہ MSDOS
#تعریف_کرو CURL_BINMODE(دھارا) (خالی)setmode(fileno(دھارا), CURL_O_ثنائی)
#وگرنہ_اگر defined(_WIN32) || defined(__CYGWIN__)
#تعریف_کرو CURL_BINMODE(دھارا) (خالی)_setmode(fileno(دھارا), CURL_O_ثنائی)
#ورنہ
#تعریف_کرو CURL_BINMODE(دھارا) (خالی)(دھارا)
#اگر_ختم

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#اگر defined(_WIN32) || defined(MSDOS)
#تعریف_کرو FOPEN_READTEXT   "rt"
#تعریف_کرو FOPEN_WRITETEXT  "wt"
#تعریف_کرو FOPEN_APPENDTEXT "at"
#وگرنہ_اگر defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#تعریف_کرو FOPEN_READTEXT   "rt"
#تعریف_کرو FOPEN_WRITETEXT  "w"
#تعریف_کرو FOPEN_APPENDTEXT "a"
#ورنہ
#تعریف_کرو FOPEN_READTEXT   "r"
#تعریف_کرو FOPEN_WRITETEXT  "w"
#تعریف_کرو FOPEN_APPENDTEXT "a"
#اگر_ختم

/* for systems that do not detect this in configure */
#اگر_غیر_تعریف_شدہ CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T پتہ_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T غیرعلامتی حرف
#  ورنہ
/* use a sensible default */
#    define CURL_SA_FAMILY_T غیرعلامتی مختصر
#  endif
#اگر_ختم

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#تعریف_کرو CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#تعریف_کرو CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#تعریف_کرو CURL_CSTRLEN(x) (حجم(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#تعریف_کرو STRCONST(x) x, CURL_CSTRLEN(x)

#تعریف_کرو CURL_ARRAYSIZE(A) (حجم(A) / حجم((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#تعریف_کرو STRERROR_LEN 256

#اگر_غیر_تعریف_شدہ CURL_DID_حافظہ_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
قسمتعریف خالی *(*curl_malloc_callback)(size_t سائز);
قسمتعریف خالی (*curl_free_callback)(خالی *ptr);
قسمتعریف خالی *(*curl_realloc_callback)(خالی *ptr, size_t سائز);
قسمتعریف حرف *(*curl_strdup_callback)(ثابت حرف *str);
قسمتعریف خالی *(*curl_calloc_callback)(size_t nmemb, size_t سائز);
#تعریف_کرو CURL_DID_حافظہ_FUNC_TYPEDEFS
#اگر_ختم

خارجی curl_malloc_callback Curl_cmalloc;
خارجی curl_free_callback Curl_cfree;
خارجی curl_realloc_callback Curl_crealloc;
خارجی curl_strdup_callback Curl_cstrdup;
خارجی curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#تعریف_کرو curlx_safefree(ptr) \
  کرو {                      \
    curlx_free(ptr);        \
    (ptr) = تہی;           \
  } جب(0)

#شامل <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_MEMDEBUG
#اگر_ختم

#اگر_تعریف_شدہ CURL_MEMDEBUG
#اگر_تعریف_شدہ __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  اگر __clang_major__ >= 4
#  define ALLOC_سائز(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  ورنہ
#  define ALLOC_سائز(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#وگرنہ_اگر defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_سائز(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#وگرنہ_اگر defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(محدود)
#  define ALLOC_سائز(s)
#  define ALLOC_SIZE2(n, s)
#ورنہ
#  define ALLOC_FUNC
#  define ALLOC_سائز(s)
#  define ALLOC_SIZE2(n, s)
#اگر_ختم

خارجی FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN خالی curl_dbg_free(خالی *ptr, صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_FUNC ALLOC_سائز(1)
  خالی *curl_dbg_malloc(size_t wantedsize, صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  خالی *curl_dbg_calloc(size_t wanted_elements, size_t wanted_سائز,
                        صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_سائز(2)
  خالی *curl_dbg_realloc(خالی *ptr, size_t wantedsize, صحیح سطر,
                         ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_FUNC
  حرف *curl_dbg_strdup(ثابت حرف *str, صحیح سطر, ثابت حرف *ماخذ);
#اگر defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحیح سطر, ثابت حرف *ماخذ);
#اگر_ختم

CURL_EXTERN خالی curl_dbg_memdebug(ثابت حرف *logname);
CURL_EXTERN خالی curl_dbg_memlimit(طویل حد_بندی);
CURL_EXTERN خالی curl_dbg_log(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(صحیح domain, صحیح type, صحیح protocol,
                                          صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN خالی curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN صحیح curl_dbg_sclose(curl_socket_t sockfd,
                                صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          خالی *saddr, خالی *saddrlen,
                                          صحیح سطر, ثابت حرف *ماخذ);
#اگر_تعریف_شدہ HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, خالی *saddr,
                                           خالی *saddrlen, صحیح flags,
                                           صحیح سطر, ثابت حرف *ماخذ);
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SOCKETPAIR
CURL_EXTERN صحیح curl_dbg_socketpair(صحیح domain, صحیح type, صحیح protocol,
                                    curl_socket_t socket_سمتیہ[2],
                                    صحیح سطر, ثابت حرف *ماخذ);
#اگر_ختم

/* FILE functions */
CURL_EXTERN صحیح curl_dbg_fclose(FILE *file, صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *منوال,
                                            صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ثابت حرف *file,
                                              ثابت حرف *منوال, FILE *fh,
                                              صحیح سطر, ثابت حرف *ماخذ);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(صحیح filedes, ثابت حرف *منوال,
                                             صحیح سطر, ثابت حرف *ماخذ);

#تعریف_کرو sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#تعریف_کرو fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#تعریف_کرو CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#تعریف_کرو CURL_FREEADDRINFO(ڈیٹا) \
  curl_dbg_freeaddrinfo(ڈیٹا, __LINE__, __FILE__)
#تعریف_کرو CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((صحیح)(domain), type, protocol, __LINE__, __FILE__)
#اگر_تعریف_شدہ HAVE_SOCKETPAIR
#تعریف_کرو CURL_SOCKETPAIR(domain, type, protocol, socket_سمتیہ) \
  curl_dbg_socketpair((صحیح)(domain), type, protocol, socket_سمتیہ, \
                      __LINE__, __FILE__)
#اگر_ختم
#تعریف_کرو CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#اگر_تعریف_شدہ HAVE_ACCEPT4
#تعریف_کرو CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#اگر_ختم

#ورنہ /* !CURL_MEMDEBUG */

#تعریف_کرو sclose(x) CURL_SCLOSE(x)
#تعریف_کرو fake_sclose(x) Curl_nop_stmt

#تعریف_کرو CURL_GETADDRINFO getaddrinfo
#تعریف_کرو CURL_FREEADDRINFO freeaddrinfo
#تعریف_کرو CURL_SOCKET socket
#اگر_تعریف_شدہ HAVE_SOCKETPAIR
#تعریف_کرو CURL_SOCKETPAIR socketpair
#اگر_ختم
#تعریف_کرو CURL_ACCEPT accept
#اگر_تعریف_شدہ HAVE_ACCEPT4
#تعریف_کرو CURL_ACCEPT4 accept4
#اگر_ختم

#اگر_ختم /* CURL_MEMDEBUG */

/* Allocator macros */

#اگر_تعریف_شدہ _WIN32
#تعریف_کرو CURLX_STRDUP_کم _strdup
#ورنہ
#تعریف_کرو CURLX_STRDUP_کم strdup
#اگر_ختم

#اگر_تعریف_شدہ CURL_MEMDEBUG

#تعریف_کرو curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#تعریف_کرو curlx_malloc(سائز)         curl_dbg_malloc(سائز, __LINE__, __FILE__)
#تعریف_کرو curlx_calloc(nbelem, سائز) \
  curl_dbg_calloc(nbelem, سائز, __LINE__, __FILE__)
#تعریف_کرو curlx_realloc(ptr, سائز) \
  curl_dbg_realloc(ptr, سائز, __LINE__, __FILE__)
#تعریف_کرو curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#اگر_تعریف_شدہ _WIN32
#اگر_تعریف_شدہ UNICODE
#تعریف_کرو curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#ورنہ
#تعریف_کرو curlx_tcsdup               curlx_strdup
#اگر_ختم
#اگر_ختم /* _WIN32 */

#ورنہ /* !CURL_MEMDEBUG */

#اگر_تعریف_شدہ BUILDING_LIBCURL
#تعریف_کرو curlx_strdup               Curl_cstrdup
#تعریف_کرو curlx_malloc               Curl_cmalloc
#تعریف_کرو curlx_calloc               Curl_ccalloc
#تعریف_کرو curlx_realloc              Curl_crealloc
#تعریف_کرو curlx_free                 Curl_cfree
#ورنہ /* !BUILDING_LIBCURL */
#تعریف_کرو curlx_strdup               CURLX_STRDUP_کم
#تعریف_کرو curlx_malloc               حافظہ_دو
#تعریف_کرو curlx_calloc               صفر_حافظہ_دو
#تعریف_کرو curlx_realloc              realloc
#تعریف_کرو curlx_free                 آزاد
#اگر_ختم /* BUILDING_LIBCURL */

#اگر_تعریف_شدہ _WIN32
#اگر_تعریف_شدہ UNICODE
#تعریف_کرو curlx_tcsdup               curlx_wcsdup
#ورنہ
#تعریف_کرو curlx_tcsdup               curlx_strdup
#اگر_ختم
#اگر_ختم /* _WIN32 */

#اگر_ختم /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#اگر defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
ساخت passwd;
صحیح getpwuid_r(uid_t uid, ساخت passwd *pwd, حرف *buf,
               size_t buflen, ساخت passwd **نتیجہ);
#اگر_ختم

#اگر_تعریف_شدہ UNITTESTS
#تعریف_کرو UNITTEST
#ورنہ
#تعریف_کرو UNITTEST ساکن
#اگر_ختم

#اگر_تعریف_شدہ USE_NGHTTP2
#تعریف_کرو USE_HTTP2
#اگر_ختم

#اگر (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
#غلطی "MultiSSL combined with QUIC is not supported"
#اگر_ختم

#تعریف_کرو USE_HTTP3
#اگر_ختم

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#اگر_غیر_تعریف_شدہ __EMSCRIPTEN__
#تعریف_کرو CURL_TCP_NODELAY_SUPPORTED
#اگر_ختم

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#اگر defined(HAVE_BASENAME) && defined(_WIN32)
#تعریف_منسوخ HAVE_BASENAME
#اگر_ختم

#اگر defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#وگرنہ_اگر !defined(UNIX_راستہ_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_راستہ_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
قسمتعریف ساخت sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  حرف sun_راستہ[UNIX_راستہ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_متعین_کرتا_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_متعین_کرتا_WARNING
#    endif
#  endif
#اگر_ختم

#اگر_تعریف_شدہ CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#وگرنہ_اگر defined(اندرونی)
#  define CURL_INLINE اندرونی /* 'inline' defined, assumed correct */
#وگرنہ_اگر defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE اندرونی /* 'inline' keyword supported */
#وگرنہ_اگر defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE اندرونی /* 'inline' keyword supported */
#وگرنہ_اگر defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#وگرنہ_اگر defined(_MSC_VER)
#  define CURL_INLINE __inline
#ورنہ
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /*خالی_خانے*/
#اگر_ختم

/* Detect if compiler supports C99 variadic macros */
#اگر (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#تعریف_کرو CURL_HAVE_MACRO_VARARG
#اگر_ختم

#اگر !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_لڑیاں))
#تعریف_کرو CURLVERBOSE
#تعریف_کرو VERBOSE(x) x
#تعریف_کرو NOVERBOSE(x) Curl_nop_stmt
#ورنہ
#تعریف_کرو VERBOSE(x) Curl_nop_stmt
#تعریف_کرو NOVERBOSE(x) x
#اگر_ختم

/* For FreeBSD it is included from curl/curl.h */
#اگر defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#شامل <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#اگر_ختم

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#اگر defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#تعریف_کرو CURL_USE_SOCK_NONBLOCK
#اگر_ختم

#اگر_غیر_تعریف_شدہ _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#اگر_تعریف_شدہ _WIN32
#اگر defined(_MSC_VER) && defined(NTDDI_نسخہ) && \
  (NTDDI_نسخہ >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#ہدایت comment(lib, "volatileaccessu.lib")
#تعریف_کرو curlx_memzero_کم(buf, سائز)  SecureZeroMemory2(buf, سائز)
#ورنہ
#تعریف_کرو curlx_memzero_کم(buf, سائز)  Secure_Zero_حافظہ(buf, سائز)
#اگر_ختم
#وگرنہ_اگر defined(HAVE_MEMSET_S)
#تعریف_کرو curlx_memzero_کم(buf, سائز)  (خالی)memset_s(buf, سائز, 0, سائز)
#وگرنہ_اگر defined(HAVE_MEMSET_EXPLICIT)
#تعریف_کرو curlx_memzero_کم(buf, سائز)  (خالی)memset_explicit(buf, 0, سائز)
#وگرنہ_اگر defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#تعریف_کرو curlx_memzero_کم(buf, سائز)  explicit_bzero(buf, سائز)
#وگرنہ_اگر defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#تعریف_کرو curlx_memzero_کم(buf, سائز)  (خالی)explicit_memset(buf, 0, سائز)
#اگر_ختم
#اگر_ختم /* !_CURL_LOCAL_MEMZERO */

#اگر_غیر_تعریف_شدہ curlx_memzero_کم
#تعریف_کرو USE_CURLX_MEMZERO
خالی curlx_memzero_کم(خالی *buf, size_t سائز);
#اگر_ختم
خالی curlx_memzero(خالی *buf, size_t سائز);
خالی curlx_strzero(خالی *buf);

#اگر_ختم /* HEADER_CURL_SETUP_H */
