#جے_بنا_تعریف سرخی_CURL_تیاری_H
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

#إذا defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#تعریف_کرو CURL_NO_OLDIES
#جے_انت

/* Set default _WIN32_WINNT */
#جے_تعریف_شدہ __MINGW32__
#ضمن <_mingw.h>
#جے_انت

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
#جے_انت

#جے_تعریف_شدہ __APPLE__
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
#إذا defined(نشانہ_OS_MAC) && نشانہ_OS_MAC && \
  defined(نشانہ_OS_OSX) && !نشانہ_OS_OSX && \
  (!defined(نشانہ_OS_IPHONE) || !نشانہ_OS_IPHONE) && \
  (!defined(نشانہ_OS_SIMULATOR) || !نشانہ_OS_SIMULATOR)
#تعریف_رد نشانہ_OS_OSX
#تعریف_کرو نشانہ_OS_OSX نشانہ_OS_MAC
#جے_انت
#جے_انت

#إذا defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#غلطی "mingw-w64 3.0 or greater required"
#جے_انت

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#إذا defined(_MSC_VER) && (_MSC_VER < 1600)
#غلطی "Ancient versions of Visual Studio are no longer supported due to bugs."
#جے_انت

#جے_تعریف_شدہ _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#ہدایت warning(disable:4127)
#جے_بنا_تعریف _CRT_SECURE_NO_WARNINGS
#تعریف_کرو _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#جے_انت
#جے_انت /* _MSC_VER */

#جے_تعریف_شدہ _WIN32
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
#  إذا (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    إذا WINAPI_FAMILY_حصے_کرو(WINAPI_حصے_کرو_APP) &&  \
       !WINAPI_FAMILY_حصے_کرو(WINAPI_حصے_کرو_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#تعریف_رد SECURITY_KERNEL
#تعریف_رد SECURITY_WIN32
#تعریف_کرو SECURITY_WIN32  /* for <sspi.h> */
#جے_انت

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#جے_تعریف_شدہ HAVE_CONFIG_H

#ضمن "curl_config.h"

#وإلا /* HAVE_CONFIG_H */

#جے_تعریف_شدہ _WIN32
#  include "config-win32.h"
#جے_انت

#جے_تعریف_شدہ macintosh
#  include "config-mac.h"
#جے_انت

#جے_تعریف_شدہ __OS400__
#  include "config-os400.h"
#جے_انت

#جے_انت /* HAVE_CONFIG_H */

#جے_تعریف_شدہ _WIN32
#  إذا defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build نشانہ is Windows Vista (0x0600)
#  endif

#  إذا !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#جے_انت

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#جے_تعریف_شدہ HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#جے_انت

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#جے_تعریف_شدہ NEED_THREAD_محفوظ
#  ifndef THREAD_محفوظ
#  define THREAD_محفوظ
#  endif
#جے_انت

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#جے_تعریف_شدہ NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#جے_انت

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#إذا defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#جے_انت

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#جے_تعریف_شدہ HAVE_دھاگے_POSIX
#  include <pthread.h>
#جے_انت

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#جے_تعریف_شدہ CURL_DISABLE_HTTP
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
#جے_انت

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#جے_تعریف_شدہ __OS400__
#  include "setup-os400.h"
#جے_انت

/*
 * VMS setup file includes some system headers.
 */
#جے_تعریف_شدہ __VMS
#  include "setup-vms.h"
#جے_انت

/*
 * Windows setup file includes some system headers.
 */
#جے_تعریف_شدہ _WIN32
#  include "setup-win32.h"
#جے_انت

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
#تعریف_کرو HAVE_PIPE2
#جے_انت

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
#جے_تعریف_شدہ __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#جے_انت

/* based on logic in "curl/mprintf.h" */
#إذا (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#إذا defined(__MINGW32__) && !defined(__clang__)
#تعریف_کرو CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#وإلا
#تعریف_کرو CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#جے_انت
#وإلا
#تعریف_کرو CURL_PRINTF(fmt, arg)
#جے_انت

/* Override default printf mask check rules in "curl/mprintf.h" */
#تعریف_کرو CURL_عارضی_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#إذا defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#تعریف_کرو __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#جے_انت

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#إذا defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  إذا نشانہ_OS_MAC && !(defined(نشانہ_OS_IPHONE) && نشانہ_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#جے_انت

#جے_تعریف_شدہ USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  إذا defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#جے_انت

#جے_تعریف_شدہ USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#جے_انت

#جے_تعریف_شدہ macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#جے_انت

#جے_تعریف_شدہ __AMIGA__
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
#جے_انت

/* Whether to use eventfd() */
#إذا defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#تعریف_کرو USE_EVENTFD
#جے_انت

#جے_تعریف_شدہ SO_NOSIGPIPE
#تعریف_کرو USE_SO_NOSIGPIPE
#جے_انت

#ضمن <stdio.h>
#ضمن <assert.h>

#جے_تعریف_شدہ __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#جے_انت

#ضمن <stdint.h>
#تعریف_کرو HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#جے_تعریف_شدہ __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#تعریف_کرو uint32_t بنانشانی صحيح
#جے_انت

/* Disable uintptr_t for targets known to miss it from stdint.h */
#جے_تعریف_شدہ __OS400__
#تعریف_رد HAVE_UINTPTR_T
#جے_انت

#ضمن <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#ضمن <sys/types.h>

#جے_تعریف_شدہ _WIN32
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
#  define LSEEK_ERROR                     ((وتھ_t)-1)
#وإلاإذا defined(__AMIGA__)
#  define curl_lseek(fd, وتھ, whence)  lseek(fd, (off_t)(وتھ), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#وإلا
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#جے_انت

#جے_بنا_تعریف SIZEOF_ویلا_T
/* assume default size of time_t to be 32 bits */
#تعریف_کرو SIZEOF_ویلا_T 4
#جے_انت

#جے_بنا_تعریف SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  إذا defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  وإلا
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#جے_انت

#إذا SIZEOF_CURL_SOCKET_T < 8
#جے_تعریف_شدہ USE_WINSOCK
#  define FMT_SOCKET_T "u"
#وإلا
#  define FMT_SOCKET_T "d"
#جے_انت
#وإلاإذا defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#وإلا
#  define FMT_SOCKET_T "qd"
#جے_انت

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#جے_بنا_تعریف SIZEOF_OFF_T
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
#جے_انت

#إذا SIZEOF_CURL_OFF_T < 8
#غلطی "too small curl_off_t"
#وإلا
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#جے_انت
#تعریف_کرو CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#تعریف_کرو FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#تعریف_کرو FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#إذا SIZEOF_ویلا_T == 4
#  ifdef HAVE_ویلا_T_UNSIGNED
#  define ویلا_T_MAX UINT_MAX
#  define ویلا_T_MIN 0
#  وإلا
#  define ویلا_T_MAX ودھ_توں_ودھ_عدد
#  define ویلا_T_MIN INT_MIN
#  endif
#وإلا
#  ifdef HAVE_ویلا_T_UNSIGNED
#  define ویلا_T_MAX 0xFFFFFFFFFFFFFFFF
#  define ویلا_T_MIN 0
#  وإلا
#  define ویلا_T_MAX 0x7FFFFFFFFFFFFFFF
#  define ویلا_T_MIN (-ویلا_T_MAX - 1)
#  endif
#جے_انت

#جے_بنا_تعریف SIZE_MAX
/* some limits.h headers have this defined, some do not */
#إذا defined(SIZEOF_ناپ_T) && (SIZEOF_ناپ_T > 4)
#تعریف_کرو SIZE_MAX 18446744073709551615U
#وإلا
#تعریف_کرو SIZE_MAX 4294967295U
#جے_انت
#جے_انت

#جے_بنا_تعریف SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#إذا defined(SIZEOF_ناپ_T) && (SIZEOF_ناپ_T > 4)
#تعریف_کرو SSIZE_MAX 9223372036854775807
#وإلا
#تعریف_کرو SSIZE_MAX 2147483647
#جے_انت
#جے_انت

#إذا SIZEOF_LONG > SIZEOF_ناپ_T
#غلطی "unexpected: 'long' is larger than 'size_t'"
#جے_انت

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#جے_بنا_تعریف GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 صحيح
#  وإلا
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#جے_انت

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#جے_تعریف_شدہ _WIN32

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

#جے_انت /* _WIN32 */

/* We want to use mutex when available. */
#إذا defined(HAVE_دھاگے_POSIX) || defined(_WIN32)
#تعریف_کرو USE_MUTEX
#جے_انت

/* threaded resolver is the only feature requiring threads. */
#جے_تعریف_شدہ USE_RESOLV_THREADED
#تعریف_کرو USE_دھاگے
#جے_انت

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
#جے_انت

#جے_تعریف_شدہ USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#وإلاإذا defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#وإلا
#  define CURLRES_SYNCH
#جے_انت

/* ---------------------------------------------------------------- */

#إذا defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#تعریف_کرو USE_LIBIDN2
#جے_انت

#إذا defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#غلطی "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#جے_انت

#إذا defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#تعریف_کرو USE_SSL    /* SSL support has been enabled */
#جے_انت

#إذا defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#جے_بنا_تعریف OPENSSL_COEXIST
#تعریف_کرو OPENSSL_COEXIST
#جے_انت
#جے_انت

#إذا defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#تعریف_کرو NO_OLD_WC_ناں
#جے_انت

/* Single point where USE_SPNEGO definition might be defined */
#إذا !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#تعریف_کرو USE_SPNEGO
#جے_انت

/* Single point where USE_KERBEROS5 definition might be defined */
#إذا !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#تعریف_کرو USE_KERBEROS5
#جے_انت

/* Single point where USE_NTLM definition might be defined */
#جے_تعریف_شدہ CURL_ENABLE_NTLM
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
#جے_انت

#إذا defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#تعریف_کرو USE_SSH
#جے_انت

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#إذا defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#تعریف_کرو CURL_HAVE_DIAG
#جے_انت

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#إذا defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_بن_ورتیا_نتیجہ __attribute__((warn_بن_ورتیا_نتیجہ))
#وإلاإذا defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_بن_ورتیا_نتیجہ __attribute__((warn_بن_ورتیا_نتیجہ))
#وإلا
#  define WARN_بن_ورتیا_نتیجہ
#جے_انت

/* noreturn attribute */

#جے_بنا_تعریف CURL_NORETURN
#إذا (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#وإلاإذا defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#وإلا
#  define CURL_NORETURN
#جے_انت
#جے_انت

/* fallthrough attribute */

#جے_بنا_تعریف FALLTHROUGH
#إذا (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#وإلا
#  define FALLTHROUGH()  افعل {} طالما(0)
#جے_انت
#جے_انت

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
#جے_انت

#جے_تعریف_شدہ HAVE_IO_H
#ضمن <io.h>
#جے_انت

#جے_تعریف_شدہ HAVE_FCNTL_H
#ضمن <fcntl.h>
#جے_انت

#إذا defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#ضمن <stdbool.h>
#جے_انت

#جے_تعریف_شدہ HAVE_UNISTD_H
#ضمن <unistd.h>
#جے_انت

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#جے_تعریف_شدہ HAVE_UINTPTR_T
#تعریف_کرو CURL_UNCONST(p) ((فراغ *)(uintptr_t)(ثابت فراغ *)(p))
#وإلا
#تعریف_کرو CURL_UNCONST(p) ((فراغ *)(p))  /* Fall back to simple cast */
#جے_انت

#جے_تعریف_شدہ USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#جے_انت

#جے_تعریف_شدہ __hpux
#  إذا !defined(XOPEN_سروت_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_ثنائی_ہندسہ_OFF_T
#      define OLD_APP32_64_ثنائی_ہندسہ_OFF_T APP32_64_ثنائی_ہندسہ_OFF_T
#      undef APP32_64_ثنائی_ہندسہ_OFF_T
#    وإلا
#      undef OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#    endif
#  endif
#جے_انت

#جے_بنا_تعریف _WIN32
#ضمن <sys/socket.h>  /* also for MSG_NOSIGNAL */
#جے_انت

#ضمن "functypes.h"

#جے_تعریف_شدہ __hpux
#  إذا !defined(XOPEN_سروت_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#      define APP32_64_ثنائی_ہندسہ_OFF_T OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#      undef OLD_APP32_64_ثنائی_ہندسہ_OFF_T
#    endif
#  endif
#جے_انت

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#جے_بنا_تعریف HAVE_STRUCT_TIMEVAL
بنية timeval {
  لما tv_sec;
  لما tv_usec;
};
#جے_انت

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#جے_تعریف_شدہ MSG_NOSIGNAL
#تعریف_کرو SEND_4TH_ARG MSG_NOSIGNAL
#وإلا
#تعریف_کرو SEND_4TH_ARG 0
#جے_انت

#جے_تعریف_شدہ __minix
/* Minix does not support recv on TCP sockets */
#تعریف_کرو sread(x, y, z) (ssize_t)اقرأ((RECV_TYPE_ARG1)(x), \
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

#تعریف_کرو sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#وإلا /* HAVE_RECV */
#جے_بنا_تعریف sread
#غلطی "Missing definition of macro sread"
#جے_انت
#جے_انت /* HAVE_RECV */

#جے_تعریف_شدہ __minix
/* Minix does not support send on TCP sockets */
#تعریف_کرو swrite(x, y, z) (ssize_t)اكتب((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#وإلاإذا defined(HAVE_SEND)
#جے_تعریف_شدہ SEND_NONCONST_ARG2
#تعریف_کرو swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#وإلا
#تعریف_کرو swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ثابت SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#جے_انت /* SEND_NONCONST_ARG2 */
#وإلا /* HAVE_SEND */
#جے_بنا_تعریف swrite
#غلطی "Missing definition of macro swrite"
#جے_انت
#جے_انت /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#جے_تعریف_شدہ HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#وإلاإذا defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#وإلاإذا defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#وإلاإذا defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#وإلا
#  define CURL_SCLOSE(x)  close(x)
#جے_انت

/*
 * Stack-independent version of fcntl() on sockets:
 */
#جے_تعریف_شدہ USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#وإلا
#  define sfcntl  fcntl
#جے_انت

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#إذا defined(__hpux) && !defined(HAVE_BOOL_T)
تعريفنوع صحيح bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#جے_انت

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#جے_بنا_تعریف HAVE_BOOL_T
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
#جے_انت

/* the type we use for storing a single boolean bit */
تعريفنوع بنانشانی صحيح curl_ثنائی_ہندسہ;
#تعریف_کرو ثنائی_ہندسہ(x) curl_ثنائی_ہندسہ x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#جے_بنا_تعریف TRUE
#تعریف_کرو TRUE true
#جے_انت
#جے_بنا_تعریف FALSE
#تعریف_کرو FALSE false
#جے_انت

#ضمن "curl_ctype.h"

#إذا defined(DEBUGBUILD) && defined(NDEBUG)
#غلطی "Debug-enabled builds cannot be combined with NDEBUG"
#جے_انت

/*
 * Macro used to include code only in debug builds.
 */
#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو DEBUGF(x) x
#وإلا
#تعریف_کرو DEBUGF(x) افعل {} طالما(0)
#جے_انت

/*
 * Macro used to include assertion code only in debug builds.
 */
#تعریف_رد DEBUGASSERT
#جے_تعریف_شدہ DEBUGBUILD
#جے_تعریف_شدہ CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#تعریف_کرو DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#وإلا
#تعریف_کرو DEBUGASSERT(x) أكد(x)
#جے_انت
#وإلا
#تعریف_کرو DEBUGASSERT(x) افعل {} طالما(0)
#جے_انت

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#إذا defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#ضمن <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#تعریف_کرو CURL_MSVC_CRTDBG_INIT()                                             \
  افعل {                                                                      \
    سیٹ_error_منوال(باہرلا_منزل_STDERR);  /* needs stdlib.h */                  \
    Crt_سیٹ_Report_منوال(_CRT_ASSERT, CRTDBG_منوال_FILE | CRTDBG_منوال_DEBUG); \
    Crt_سیٹ_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_سیٹ_Report_منوال(_CRT_ERROR, CRTDBG_منوال_FILE | CRTDBG_منوال_DEBUG);  \
    Crt_سیٹ_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_سیٹ_Report_منوال(_CRT_WARN, CRTDBG_منوال_FILE | CRTDBG_منوال_DEBUG);   \
    Crt_سیٹ_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } طالما(0)
#وإلا
#تعریف_کرو CURL_MSVC_CRTDBG_INIT() افعل {} طالما(0)
#جے_انت

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#ضمن <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#تعریف_کرو CURL_DUMP_MODULE_PATHS(printf_f)                               \
  افعل {                                                                 \
    HANDLE hnd = INVALID_HANDLE_قدر;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_ناپ = حجم(MODULEENTRY32);                                \
                                                                       \
    افعل {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } طالما(hnd == INVALID_HANDLE_قدر &&                             \
            Get_آخری_Error() == ERROR_BAD_لمائی);                       \
                                                                       \
    إذا(hnd == INVALID_HANDLE_قدر)                                    \
      اذهب error;                                                      \
                                                                       \
    إذا(!Module32_پہلا(hnd, &mod))                                      \
      اذهب error;                                                      \
                                                                       \
    افعل {                                                               \
      حرف بفر[حجم(mod.sz_Exe_رستہ) * 2];                          \
      حرف *رستہ; /* points to stack allocated buffer */               \
      إذا(حجم(*mod.sz_Exe_رستہ) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        إذا(!Wide_Char_منزل_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_رستہ, -1, \
                                بفر, حجم(بفر), عدم, عدم))   \
          اذهب error;                                                  \
        رستہ = (حرف *)بفر;                                         \
      }                                                                \
      وإلا                                                             \
        رستہ = (حرف *)mod.sz_Exe_رستہ;                                  \
      (printf_f)("%s\n", رستہ);                                        \
    } طالما(Module32_اگلا(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    إذا(hnd != INVALID_HANDLE_قدر)                                    \
      CloseHandle(hnd);                                                \
  } طالما(0)
#وإلا
#تعریف_کرو CURL_DUMP_MODULE_PATHS(printf_f) افعل {} طالما(0)
#جے_انت

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#جے_تعریف_شدہ USE_WINSOCK
#تعریف_کرو SOCKERRNO         ((صحيح)WSAGet_آخری_Error())
#تعریف_کرو سیٹ_SOCKERRNO(x)  WSASet_آخری_Error((صحيح)(x))
#وإلا
#تعریف_کرو SOCKERRNO         errno
#تعریف_کرو سیٹ_SOCKERRNO(x)  (errno = (x))
#جے_انت

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#جے_تعریف_شدہ USE_WINSOCK
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
#وإلا
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
#جے_تعریف_شدہ ETIMEDOUT
#تعریف_کرو SOCKETIMEDOUT     ETIMEDOUT
#جے_انت
#تعریف_کرو SOCKEWOULDBLOCK   EWOULDBLOCK
#جے_انت

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#إذا !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#تعریف_کرو SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#وإلا
#تعریف_کرو SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#جے_انت

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#جے_تعریف_شدہ __VMS
#تعریف_کرو argv_شے_t  __char_ptr32
#وإلاإذا defined(_UNICODE)
#تعریف_کرو argv_شے_t  wchar_t *
#وإلا
#تعریف_کرو argv_شے_t  حرف *
#جے_انت

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#تعریف_کرو ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#ضمن "curlx/warnless.h"

#جے_تعریف_شدہ _WIN32
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
#جے_انت

/*
 * Definition of our NOP statement Object-like macro
 */
#جے_بنا_تعریف Curl_nop_stmt
#تعریف_کرو Curl_nop_stmt افعل {} طالما(0)
#جے_انت

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#إذا defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  إذا defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#جے_انت

/*
 * shutdown() flags for systems that do not define them
 */
#جے_بنا_تعریف SHUT_RD
#تعریف_کرو SHUT_RD 0x00
#جے_انت

#جے_بنا_تعریف SHUT_WR
#تعریف_کرو SHUT_WR 0x01
#جے_انت

#جے_بنا_تعریف SHUT_RDWR
#تعریف_کرو SHUT_RDWR 0x02
#جے_انت

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#إذا !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#تعریف_کرو S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#جے_انت

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#إذا !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#تعریف_کرو S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#جے_انت

/* For MSVC (all versions as of VS2022) */
#جے_بنا_تعریف STDIN_FILENO
#تعریف_کرو STDIN_FILENO  fileno(stdin)
#جے_انت
#جے_بنا_تعریف STDOUT_FILENO
#تعریف_کرو STDOUT_FILENO  fileno(stdout)
#جے_انت
#جے_بنا_تعریف STDERR_FILENO
#تعریف_کرو STDERR_FILENO  fileno(stderr)
#جے_انت

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#جے_تعریف_شدہ O_ثنائی  /* for _WIN32 || MSDOS */
#تعریف_کرو CURL_O_ثنائی O_ثنائی
#وإلاإذا defined(O_ثنائی_2)  /* __CYGWIN__ */
#تعریف_کرو CURL_O_ثنائی O_ثنائی_2
#وإلا
#تعریف_کرو CURL_O_ثنائی 0
#جے_انت

/* Requires io.h when available */
#جے_تعریف_شدہ MSDOS
#تعریف_کرو CURL_BINMODE(دھارا) (فراغ)setmode(fileno(دھارا), CURL_O_ثنائی)
#وإلاإذا defined(_WIN32) || defined(__CYGWIN__)
#تعریف_کرو CURL_BINMODE(دھارا) (فراغ)_setmode(fileno(دھارا), CURL_O_ثنائی)
#وإلا
#تعریف_کرو CURL_BINMODE(دھارا) (فراغ)(دھارا)
#جے_انت

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#إذا defined(_WIN32) || defined(MSDOS)
#تعریف_کرو FOPEN_READTEXT   "rt"
#تعریف_کرو FOPEN_WRITETEXT  "wt"
#تعریف_کرو FOPEN_APPENDTEXT "at"
#وإلاإذا defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#تعریف_کرو FOPEN_READTEXT   "rt"
#تعریف_کرو FOPEN_WRITETEXT  "w"
#تعریف_کرو FOPEN_APPENDTEXT "a"
#وإلا
#تعریف_کرو FOPEN_READTEXT   "r"
#تعریف_کرو FOPEN_WRITETEXT  "w"
#تعریف_کرو FOPEN_APPENDTEXT "a"
#جے_انت

/* for systems that do not detect this in configure */
#جے_بنا_تعریف CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T پتہ_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T بنانشانی حرف
#  وإلا
/* use a sensible default */
#    define CURL_SA_FAMILY_T بنانشانی چھوٹا
#  endif
#جے_انت

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

#جے_بنا_تعریف CURL_DID_حافظہ_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
تعريفنوع فراغ *(*curl_malloc_callback)(size_t ناپ);
تعريفنوع فراغ (*curl_free_callback)(فراغ *ptr);
تعريفنوع فراغ *(*curl_realloc_callback)(فراغ *ptr, size_t ناپ);
تعريفنوع حرف *(*curl_strdup_callback)(ثابت حرف *str);
تعريفنوع فراغ *(*curl_calloc_callback)(size_t nmemb, size_t ناپ);
#تعریف_کرو CURL_DID_حافظہ_FUNC_TYPEDEFS
#جے_انت

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
#تعریف_کرو curlx_safefree(ptr) \
  افعل {                      \
    curlx_free(ptr);        \
    (ptr) = عدم;           \
  } طالما(0)

#ضمن <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_MEMDEBUG
#جے_انت

#جے_تعریف_شدہ CURL_MEMDEBUG
#جے_تعریف_شدہ __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  إذا __clang_major__ >= 4
#  define ALLOC_ناپ(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  وإلا
#  define ALLOC_ناپ(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#وإلاإذا defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_ناپ(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#وإلاإذا defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(محدود)
#  define ALLOC_ناپ(s)
#  define ALLOC_SIZE2(n, s)
#وإلا
#  define ALLOC_FUNC
#  define ALLOC_ناپ(s)
#  define ALLOC_SIZE2(n, s)
#جے_انت

خارجي FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN فراغ curl_dbg_free(فراغ *ptr, صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN ALLOC_FUNC ALLOC_ناپ(1)
  فراغ *curl_dbg_malloc(size_t wantedsize, صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_ناپ,
                        صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN ALLOC_ناپ(2)
  فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize, صحيح سطر,
                         ثابت حرف *سروت);
CURL_EXTERN ALLOC_FUNC
  حرف *curl_dbg_strdup(ثابت حرف *str, صحيح سطر, ثابت حرف *سروت);
#إذا defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح سطر, ثابت حرف *سروت);
#جے_انت

CURL_EXTERN فراغ curl_dbg_memdebug(ثابت حرف *logname);
CURL_EXTERN فراغ curl_dbg_memlimit(لما حد_بندی);
CURL_EXTERN فراغ curl_dbg_log(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                                          صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN فراغ curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN صحيح curl_dbg_sclose(curl_socket_t sockfd,
                                صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          فراغ *saddr, فراغ *saddrlen,
                                          صحيح سطر, ثابت حرف *سروت);
#جے_تعریف_شدہ HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr,
                                           فراغ *saddrlen, صحيح flags,
                                           صحيح سطر, ثابت حرف *سروت);
#جے_انت
#جے_تعریف_شدہ HAVE_SOCKETPAIR
CURL_EXTERN صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                                    curl_socket_t socket_سمتیہ[2],
                                    صحيح سطر, ثابت حرف *سروت);
#جے_انت

/* FILE functions */
CURL_EXTERN صحيح curl_dbg_fclose(FILE *file, صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *منوال,
                                            صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ثابت حرف *file,
                                              ثابت حرف *منوال, FILE *fh,
                                              صحيح سطر, ثابت حرف *سروت);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *منوال,
                                             صحيح سطر, ثابت حرف *سروت);

#تعریف_کرو sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#تعریف_کرو fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#تعریف_کرو CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#تعریف_کرو CURL_FREEADDRINFO(انکڑے) \
  curl_dbg_freeaddrinfo(انکڑے, __LINE__, __FILE__)
#تعریف_کرو CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((صحيح)(domain), type, protocol, __LINE__, __FILE__)
#جے_تعریف_شدہ HAVE_SOCKETPAIR
#تعریف_کرو CURL_SOCKETPAIR(domain, type, protocol, socket_سمتیہ) \
  curl_dbg_socketpair((صحيح)(domain), type, protocol, socket_سمتیہ, \
                      __LINE__, __FILE__)
#جے_انت
#تعریف_کرو CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#جے_تعریف_شدہ HAVE_ACCEPT4
#تعریف_کرو CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#جے_انت

#وإلا /* !CURL_MEMDEBUG */

#تعریف_کرو sclose(x) CURL_SCLOSE(x)
#تعریف_کرو fake_sclose(x) Curl_nop_stmt

#تعریف_کرو CURL_GETADDRINFO getaddrinfo
#تعریف_کرو CURL_FREEADDRINFO freeaddrinfo
#تعریف_کرو CURL_SOCKET socket
#جے_تعریف_شدہ HAVE_SOCKETPAIR
#تعریف_کرو CURL_SOCKETPAIR socketpair
#جے_انت
#تعریف_کرو CURL_ACCEPT accept
#جے_تعریف_شدہ HAVE_ACCEPT4
#تعریف_کرو CURL_ACCEPT4 accept4
#جے_انت

#جے_انت /* CURL_MEMDEBUG */

/* Allocator macros */

#جے_تعریف_شدہ _WIN32
#تعریف_کرو CURLX_STRDUP_کم _strdup
#وإلا
#تعریف_کرو CURLX_STRDUP_کم strdup
#جے_انت

#جے_تعریف_شدہ CURL_MEMDEBUG

#تعریف_کرو curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#تعریف_کرو curlx_malloc(ناپ)         curl_dbg_malloc(ناپ, __LINE__, __FILE__)
#تعریف_کرو curlx_calloc(nbelem, ناپ) \
  curl_dbg_calloc(nbelem, ناپ, __LINE__, __FILE__)
#تعریف_کرو curlx_realloc(ptr, ناپ) \
  curl_dbg_realloc(ptr, ناپ, __LINE__, __FILE__)
#تعریف_کرو curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#جے_تعریف_شدہ _WIN32
#جے_تعریف_شدہ UNICODE
#تعریف_کرو curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#وإلا
#تعریف_کرو curlx_tcsdup               curlx_strdup
#جے_انت
#جے_انت /* _WIN32 */

#وإلا /* !CURL_MEMDEBUG */

#جے_تعریف_شدہ BUILDING_LIBCURL
#تعریف_کرو curlx_strdup               Curl_cstrdup
#تعریف_کرو curlx_malloc               Curl_cmalloc
#تعریف_کرو curlx_calloc               Curl_ccalloc
#تعریف_کرو curlx_realloc              Curl_crealloc
#تعریف_کرو curlx_free                 Curl_cfree
#وإلا /* !BUILDING_LIBCURL */
#تعریف_کرو curlx_strdup               CURLX_STRDUP_کم
#تعریف_کرو curlx_malloc               حافظہ_دیو
#تعریف_کرو curlx_calloc               صفر_حافظہ_دیو
#تعریف_کرو curlx_realloc              realloc
#تعریف_کرو curlx_free                 حرر
#جے_انت /* BUILDING_LIBCURL */

#جے_تعریف_شدہ _WIN32
#جے_تعریف_شدہ UNICODE
#تعریف_کرو curlx_tcsdup               curlx_wcsdup
#وإلا
#تعریف_کرو curlx_tcsdup               curlx_strdup
#جے_انت
#جے_انت /* _WIN32 */

#جے_انت /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#إذا defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
بنية passwd;
صحيح getpwuid_r(uid_t uid, بنية passwd *pwd, حرف *buf,
               size_t buflen, بنية passwd **نتیجہ);
#جے_انت

#جے_تعریف_شدہ UNITTESTS
#تعریف_کرو UNITTEST
#وإلا
#تعریف_کرو UNITTEST ساكن
#جے_انت

#جے_تعریف_شدہ USE_NGHTTP2
#تعریف_کرو USE_HTTP2
#جے_انت

#إذا (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#جے_تعریف_شدہ CURL_WITH_MULTI_SSL
#غلطی "MultiSSL combined with QUIC is not supported"
#جے_انت

#تعریف_کرو USE_HTTP3
#جے_انت

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#جے_بنا_تعریف __EMSCRIPTEN__
#تعریف_کرو CURL_TCP_NODELAY_SUPPORTED
#جے_انت

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#إذا defined(HAVE_BASENAME) && defined(_WIN32)
#تعریف_رد HAVE_BASENAME
#جے_انت

#إذا defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#وإلاإذا !defined(UNIX_رستہ_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_رستہ_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  حرف sun_رستہ[UNIX_رستہ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#جے_انت
#جے_انت

#جے_تعریف_شدہ USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_مقرر_کردا_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_مقرر_کردا_WARNING
#    endif
#  endif
#جے_انت

#جے_تعریف_شدہ CURL_INLINE
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
#  define CURL_INLINE /*خالی_تھاواں*/
#جے_انت

/* Detect if compiler supports C99 variadic macros */
#إذا (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#تعریف_کرو CURL_HAVE_MACRO_VARARG
#جے_انت

#إذا !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_لڑیاں))
#تعریف_کرو CURLVERBOSE
#تعریف_کرو VERBOSE(x) x
#تعریف_کرو NOVERBOSE(x) Curl_nop_stmt
#وإلا
#تعریف_کرو VERBOSE(x) Curl_nop_stmt
#تعریف_کرو NOVERBOSE(x) x
#جے_انت

/* For FreeBSD it is included from curl/curl.h */
#إذا defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#ضمن <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#جے_انت

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#إذا defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#تعریف_کرو CURL_USE_SOCK_NONBLOCK
#جے_انت

#جے_بنا_تعریف _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#جے_تعریف_شدہ _WIN32
#إذا defined(_MSC_VER) && defined(NTDDI_نسخہ) && \
  (NTDDI_نسخہ >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#ہدایت comment(lib, "volatileaccessu.lib")
#تعریف_کرو curlx_memzero_کم(buf, ناپ)  SecureZeroMemory2(buf, ناپ)
#وإلا
#تعریف_کرو curlx_memzero_کم(buf, ناپ)  Secure_Zero_حافظہ(buf, ناپ)
#جے_انت
#وإلاإذا defined(HAVE_MEMSET_S)
#تعریف_کرو curlx_memzero_کم(buf, ناپ)  (فراغ)memset_s(buf, ناپ, 0, ناپ)
#وإلاإذا defined(HAVE_MEMSET_EXPLICIT)
#تعریف_کرو curlx_memzero_کم(buf, ناپ)  (فراغ)memset_explicit(buf, 0, ناپ)
#وإلاإذا defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#تعریف_کرو curlx_memzero_کم(buf, ناپ)  explicit_bzero(buf, ناپ)
#وإلاإذا defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#تعریف_کرو curlx_memzero_کم(buf, ناپ)  (فراغ)explicit_memset(buf, 0, ناپ)
#جے_انت
#جے_انت /* !_CURL_LOCAL_MEMZERO */

#جے_بنا_تعریف curlx_memzero_کم
#تعریف_کرو USE_CURLX_MEMZERO
فراغ curlx_memzero_کم(فراغ *buf, size_t ناپ);
#جے_انت
فراغ curlx_memzero(فراغ *buf, size_t ناپ);
فراغ curlx_strzero(فراغ *buf);

#جے_انت /* HEADER_CURL_SETUP_H */
