#నిర్వచితం_కాకపోతే శీర్షిక_CURL_అమరిక_H
#నిర్వచించు శీర్షిక_CURL_అమరిక_H
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

#ఒకవేళ defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#నిర్వచించు CURL_NO_OLDIES
#అయితే_ముగింపు

/* Set default _WIN32_WINNT */
#నిర్వచితమైతే __MINGW32__
#అన్తర్భావ <_mingw.h>
#అయితే_ముగింపు

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
#ఒకవేళ defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  ఒకవేళ !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే __APPLE__
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <TargetConditionals.h>
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
#ఒకవేళ defined(లక్ష్యం_OS_MAC) && లక్ష్యం_OS_MAC && \
  defined(లక్ష్యం_OS_OSX) && !లక్ష్యం_OS_OSX && \
  (!defined(లక్ష్యం_OS_IPHONE) || !లక్ష్యం_OS_IPHONE) && \
  (!defined(లక్ష్యం_OS_SIMULATOR) || !లక్ష్యం_OS_SIMULATOR)
#నిర్వచనం_రద్దు లక్ష్యం_OS_OSX
#నిర్వచించు లక్ష్యం_OS_OSX లక్ష్యం_OS_MAC
#అయితే_ముగింపు
#అయితే_ముగింపు

#ఒకవేళ defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#దోషం "mingw-w64 3.0 or greater required"
#అయితే_ముగింపు

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#ఒకవేళ defined(_MSC_VER) && (_MSC_VER < 1600)
#దోషం "Ancient versions of Visual Studio are no longer supported due to bugs."
#అయితే_ముగింపు

#నిర్వచితమైతే _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#సూచన warning(disable:4127)
#నిర్వచితం_కాకపోతే _CRT_SECURE_NO_WARNINGS
#నిర్వచించు _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#అయితే_ముగింపు
#అయితే_ముగింపు /* _MSC_VER */

#నిర్వచితమైతే _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_సగటు
#  define WIN32_LEAN_AND_సగటు
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  ఒకవేళ (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    ఒకవేళ WINAPI_FAMILY_విభజన(WINAPI_విభజన_APP) &&  \
       !WINAPI_FAMILY_విభజన(WINAPI_విభజన_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#నిర్వచనం_రద్దు SECURITY_KERNEL
#నిర్వచనం_రద్దు SECURITY_WIN32
#నిర్వచించు SECURITY_WIN32  /* for <sspi.h> */
#అయితే_ముగింపు

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#నిర్వచితమైతే HAVE_CONFIG_H

#అన్తర్భావ "curl_config.h"

#లేకపోతే /* HAVE_CONFIG_H */

#నిర్వచితమైతే _WIN32
#  include "config-win32.h"
#అయితే_ముగింపు

#నిర్వచితమైతే macintosh
#  include "config-mac.h"
#అయితే_ముగింపు

#నిర్వచితమైతే __OS400__
#  include "config-os400.h"
#అయితే_ముగింపు

#అయితే_ముగింపు /* HAVE_CONFIG_H */

#నిర్వచితమైతే _WIN32
#  ఒకవేళ defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build లక్ష్యం is Windows Vista (0x0600)
#  endif

#  ఒకవేళ !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#అయితే_ముగింపు

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#నిర్వచితమైతే HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#అయితే_ముగింపు

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#నిర్వచితమైతే NEED_THREAD_సురక్షిత
#  ifndef THREAD_సురక్షిత
#  define THREAD_సురక్షిత
#  endif
#అయితే_ముగింపు

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#నిర్వచితమైతే NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#అయితే_ముగింపు

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#ఒకవేళ defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#అయితే_ముగింపు

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#నిర్వచితమైతే HAVE_దారాలు_POSIX
#  include <pthread.h>
#అయితే_ముగింపు

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#నిర్వచితమైతే CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_శీర్షికలు_API
#  define CURL_DISABLE_శీర్షికలు_API
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
#అయితే_ముగింపు

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#నిర్వచితమైతే __OS400__
#  include "setup-os400.h"
#అయితే_ముగింపు

/*
 * VMS setup file includes some system headers.
 */
#నిర్వచితమైతే __VMS
#  include "setup-vms.h"
#అయితే_ముగింపు

/*
 * Windows setup file includes some system headers.
 */
#నిర్వచితమైతే _WIN32
#  include "setup-win32.h"
#అయితే_ముగింపు

#అన్తర్భావ <curl/system.h>

#ఒకవేళ !defined(HAVE_PIPE2) && \
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
#నిర్వచించు HAVE_PIPE2
#అయితే_ముగింపు

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#నిర్వచించు CURL_CONC_MACROS_(A, B) A ## B
#నిర్వచించు CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#నిర్వచితమైతే __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#అయితే_ముగింపు

/* based on logic in "curl/mprintf.h" */
#ఒకవేళ (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#ఒకవేళ defined(__MINGW32__) && !defined(__clang__)
#నిర్వచించు CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#లేకపోతే
#నిర్వచించు CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#అయితే_ముగింపు
#లేకపోతే
#నిర్వచించు CURL_PRINTF(fmt, arg)
#అయితే_ముగింపు

/* Override default printf mask check rules in "curl/mprintf.h" */
#నిర్వచించు CURL_తాత్కాలిక_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#ఒకవేళ defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#నిర్వచించు __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#అయితే_ముగింపు

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#ఒకవేళ defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  ఒకవేళ లక్ష్యం_OS_MAC && !(defined(లక్ష్యం_OS_IPHONE) && లక్ష్యం_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  ఒకవేళ defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#అయితే_ముగింపు

#నిర్వచితమైతే macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    ఒకవేళ !(defined(__NEWLIB__) || \
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
#  ఒకవేళ defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       పూర్ణాంకం Curl_amiga_select(పూర్ణాంకం nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, నిర్మాణం timeval *timeout);
#      define select(a, b, c, d, e) Curl_amiga_select(a, b, c, d, e)
#    లేకపోతే
#      define select(a, b, c, d, e) WaitSelect(a, b, c, d, e, 0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  లేకపోతే
     /* use libc networking and hence close() and fcntl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#అయితే_ముగింపు

/* Whether to use eventfd() */
#ఒకవేళ defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#నిర్వచించు USE_EVENTFD
#అయితే_ముగింపు

#నిర్వచితమైతే SO_NOSIGPIPE
#నిర్వచించు USE_SO_NOSIGPIPE
#అయితే_ముగింపు

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <assert.h>

#నిర్వచితమైతే __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#అయితే_ముగింపు

#అన్తర్భావ <stdint.h>
#నిర్వచించు HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#నిర్వచితమైతే __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#నిర్వచించు uint32_t గుర్తులేని పూర్ణాంకం
#అయితే_ముగింపు

/* Disable uintptr_t for targets known to miss it from stdint.h */
#నిర్వచితమైతే __OS400__
#నిర్వచనం_రద్దు HAVE_UINTPTR_T
#అయితే_ముగింపు

#అన్తర్భావ <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#అన్తర్భావ <sys/types.h>

#నిర్వచితమైతే _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#లేదావేళ defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((స్థానభ్రంశం_t)-1)
#లేదావేళ defined(__AMIGA__)
#  define curl_lseek(fd, స్థానభ్రంశం, whence)  lseek(fd, (off_t)(స్థానభ్రంశం), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#లేకపోతే
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SIZEOF_సమయం_T
/* assume default size of time_t to be 32 bits */
#నిర్వచించు SIZEOF_సమయం_T 4
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  ఒకవేళ defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  లేకపోతే
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#అయితే_ముగింపు

#ఒకవేళ SIZEOF_CURL_SOCKET_T < 8
#నిర్వచితమైతే USE_WINSOCK
#  define FMT_SOCKET_T "u"
#లేకపోతే
#  define FMT_SOCKET_T "d"
#అయితే_ముగింపు
#లేదావేళ defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#లేకపోతే
#  define FMT_SOCKET_T "qd"
#అయితే_ముగింపు

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#నిర్వచితం_కాకపోతే SIZEOF_OFF_T
#  ఒకవేళ defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    ఒకవేళ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    ఒకవేళ defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#అయితే_ముగింపు

#ఒకవేళ SIZEOF_CURL_OFF_T < 8
#దోషం "too small curl_off_t"
#లేకపోతే
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#అయితే_ముగింపు
#నిర్వచించు CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#నిర్వచించు FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#నిర్వచించు FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#ఒకవేళ SIZEOF_సమయం_T == 4
#  ifdef HAVE_సమయం_T_UNSIGNED
#  define సమయం_T_MAX UINT_MAX
#  define సమయం_T_MIN 0
#  లేకపోతే
#  define సమయం_T_MAX గరిష్ఠ_పూర్ణాంకం
#  define సమయం_T_MIN INT_MIN
#  endif
#లేకపోతే
#  ifdef HAVE_సమయం_T_UNSIGNED
#  define సమయం_T_MAX 0xFFFFFFFFFFFFFFFF
#  define సమయం_T_MIN 0
#  లేకపోతే
#  define సమయం_T_MAX 0x7FFFFFFFFFFFFFFF
#  define సమయం_T_MIN (-సమయం_T_MAX - 1)
#  endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SIZE_MAX
/* some limits.h headers have this defined, some do not */
#ఒకవేళ defined(SIZEOF_కొలత_T) && (SIZEOF_కొలత_T > 4)
#నిర్వచించు SIZE_MAX 18446744073709551615U
#లేకపోతే
#నిర్వచించు SIZE_MAX 4294967295U
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#ఒకవేళ defined(SIZEOF_కొలత_T) && (SIZEOF_కొలత_T > 4)
#నిర్వచించు SSIZE_MAX 9223372036854775807
#లేకపోతే
#నిర్వచించు SSIZE_MAX 2147483647
#అయితే_ముగింపు
#అయితే_ముగింపు

#ఒకవేళ SIZEOF_LONG > SIZEOF_కొలత_T
#దోషం "unexpected: 'long' is larger than 'size_t'"
#అయితే_ముగింపు

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#నిర్వచితం_కాకపోతే GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 పూర్ణాంకం
#  లేకపోతే
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#అయితే_ముగింపు

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#నిర్వచితమైతే _WIN32

#  define DIR_CHAR      "\\"

#లేకపోతే /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (అక్షరం *)(z))
#    include <tcp.h>
#    undef మాట
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     బాహ్య నిర్మాణం tm *gmtime_r(స్థిరరాశి time_t * స్థిరరాశి timep, నిర్మాణం tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#అయితే_ముగింపు /* _WIN32 */

/* We want to use mutex when available. */
#ఒకవేళ defined(HAVE_దారాలు_POSIX) || defined(_WIN32)
#నిర్వచించు USE_MUTEX
#అయితే_ముగింపు

/* threaded resolver is the only feature requiring threads. */
#నిర్వచితమైతే USE_RESOLV_THREADED
#నిర్వచించు USE_దారాలు
#అయితే_ముగింపు

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#ఒకవేళ defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#లేదావేళ defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#లేకపోతే
#  define CURLRES_IPV4
#అయితే_ముగింపు

#నిర్వచితమైతే USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#లేదావేళ defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#లేకపోతే
#  define CURLRES_SYNCH
#అయితే_ముగింపు

/* ---------------------------------------------------------------- */

#ఒకవేళ defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#నిర్వచించు USE_LIBIDN2
#అయితే_ముగింపు

#ఒకవేళ defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#దోషం "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#అయితే_ముగింపు

#ఒకవేళ defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#నిర్వచించు USE_SSL    /* SSL support has been enabled */
#అయితే_ముగింపు

#ఒకవేళ defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#నిర్వచితం_కాకపోతే OPENSSL_COEXIST
#నిర్వచించు OPENSSL_COEXIST
#అయితే_ముగింపు
#అయితే_ముగింపు

#ఒకవేళ defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#నిర్వచించు NO_OLD_WC_పేర్లు
#అయితే_ముగింపు

/* Single point where USE_SPNEGO definition might be defined */
#ఒకవేళ !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#నిర్వచించు USE_SPNEGO
#అయితే_ముగింపు

/* Single point where USE_KERBEROS5 definition might be defined */
#ఒకవేళ !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#నిర్వచించు USE_KERBEROS5
#అయితే_ముగింపు

/* Single point where USE_NTLM definition might be defined */
#నిర్వచితమైతే CURL_ENABLE_NTLM
#  ఒకవేళ (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  ఒకవేళ defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#అయితే_ముగింపు

#ఒకవేళ defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#నిర్వచించు USE_SSH
#అయితే_ముగింపు

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#ఒకవేళ defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#నిర్వచించు CURL_HAVE_DIAG
#అయితే_ముగింపు

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#ఒకవేళ defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_వాడని_ఫలితం __attribute__((warn_వాడని_ఫలితం))
#లేదావేళ defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_వాడని_ఫలితం __attribute__((warn_వాడని_ఫలితం))
#లేకపోతే
#  define WARN_వాడని_ఫలితం
#అయితే_ముగింపు

/* noreturn attribute */

#నిర్వచితం_కాకపోతే CURL_NORETURN
#ఒకవేళ (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#లేదావేళ defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#లేకపోతే
#  define CURL_NORETURN
#అయితే_ముగింపు
#అయితే_ముగింపు

/* fallthrough attribute */

#నిర్వచితం_కాకపోతే FALLTHROUGH
#ఒకవేళ (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#లేకపోతే
#  define FALLTHROUGH()  చేయి {} అయితే(0)
#అయితే_ముగింపు
#అయితే_ముగింపు

/*
 * Inclusion of common header files.
 */

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ <errno.h>

#అన్తర్భావ <sys/stat.h>

#ఒకవేళ !defined(_WIN32) || defined(__MINGW32__)
#అన్తర్భావ <sys/time.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_IO_H
#అన్తర్భావ <io.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_FCNTL_H
#అన్తర్భావ <fcntl.h>
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#అన్తర్భావ <stdbool.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_UNISTD_H
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#నిర్వచితమైతే HAVE_UINTPTR_T
#నిర్వచించు CURL_UNCONST(p) ((శూన్య *)(uintptr_t)(స్థిరరాశి శూన్య *)(p))
#లేకపోతే
#నిర్వచించు CURL_UNCONST(p) ((శూన్య *)(p))  /* Fall back to simple cast */
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#అయితే_ముగింపు

#నిర్వచితమైతే __hpux
#  ఒకవేళ !defined(XOPEN_ఆధారం_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_బిట్టు_OFF_T
#      define OLD_APP32_64_బిట్టు_OFF_T APP32_64_బిట్టు_OFF_T
#      undef APP32_64_బిట్టు_OFF_T
#    లేకపోతే
#      undef OLD_APP32_64_బిట్టు_OFF_T
#    endif
#  endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే _WIN32
#అన్తర్భావ <sys/socket.h>  /* also for MSG_NOSIGNAL */
#అయితే_ముగింపు

#అన్తర్భావ "functypes.h"

#నిర్వచితమైతే __hpux
#  ఒకవేళ !defined(XOPEN_ఆధారం_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_బిట్టు_OFF_T
#      define APP32_64_బిట్టు_OFF_T OLD_APP32_64_బిట్టు_OFF_T
#      undef OLD_APP32_64_బిట్టు_OFF_T
#    endif
#  endif
#అయితే_ముగింపు

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#నిర్వచితం_కాకపోతే HAVE_STRUCT_TIMEVAL
నిర్మాణం timeval {
  దీర్ఘ tv_sec;
  దీర్ఘ tv_usec;
};
#అయితే_ముగింపు

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#నిర్వచితమైతే MSG_NOSIGNAL
#నిర్వచించు SEND_4TH_ARG MSG_NOSIGNAL
#లేకపోతే
#నిర్వచించు SEND_4TH_ARG 0
#అయితే_ముగింపు

#నిర్వచితమైతే __minix
/* Minix does not support recv on TCP sockets */
#నిర్వచించు sread(x, y, z) (ssize_t)పఠ((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#లేదావేళ defined(HAVE_RECV)
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

#నిర్వచించు sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#లేకపోతే /* HAVE_RECV */
#నిర్వచితం_కాకపోతే sread
#దోషం "Missing definition of macro sread"
#అయితే_ముగింపు
#అయితే_ముగింపు /* HAVE_RECV */

#నిర్వచితమైతే __minix
/* Minix does not support send on TCP sockets */
#నిర్వచించు swrite(x, y, z) (ssize_t)లిఖ((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#లేదావేళ defined(HAVE_SEND)
#నిర్వచితమైతే SEND_NONCONST_ARG2
#నిర్వచించు swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#లేకపోతే
#నిర్వచించు swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (స్థిరరాశి SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#అయితే_ముగింపు /* SEND_NONCONST_ARG2 */
#లేకపోతే /* HAVE_SEND */
#నిర్వచితం_కాకపోతే swrite
#దోషం "Missing definition of macro swrite"
#అయితే_ముగింపు
#అయితే_ముగింపు /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#నిర్వచితమైతే HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#లేదావేళ defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#లేదావేళ defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#లేదావేళ defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#లేకపోతే
#  define CURL_SCLOSE(x)  close(x)
#అయితే_ముగింపు

/*
 * Stack-independent version of fcntl() on sockets:
 */
#నిర్వచితమైతే USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#లేకపోతే
#  define sfcntl  fcntl
#అయితే_ముగింపు

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#ఒకవేళ defined(__hpux) && !defined(HAVE_BOOL_T)
రకనిర్వచనం పూర్ణాంకం bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#అయితే_ముగింపు

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#నిర్వచితం_కాకపోతే HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
రకనిర్వచనం గణన {
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
#అయితే_ముగింపు

/* the type we use for storing a single boolean bit */
రకనిర్వచనం గుర్తులేని పూర్ణాంకం curl_బిట్టు;
#నిర్వచించు బిట్టు(x) curl_బిట్టు x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#నిర్వచితం_కాకపోతే TRUE
#నిర్వచించు TRUE true
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FALSE
#నిర్వచించు FALSE false
#అయితే_ముగింపు

#అన్తర్భావ "curl_ctype.h"

#ఒకవేళ defined(DEBUGBUILD) && defined(NDEBUG)
#దోషం "Debug-enabled builds cannot be combined with NDEBUG"
#అయితే_ముగింపు

/*
 * Macro used to include code only in debug builds.
 */
#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు DEBUGF(x) x
#లేకపోతే
#నిర్వచించు DEBUGF(x) చేయి {} అయితే(0)
#అయితే_ముగింపు

/*
 * Macro used to include assertion code only in debug builds.
 */
#నిర్వచనం_రద్దు DEBUGASSERT
#నిర్వచితమైతే DEBUGBUILD
#నిర్వచితమైతే CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#నిర్వచించు DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#లేకపోతే
#నిర్వచించు DEBUGASSERT(x) ప్రతిజ్ఞా(x)
#అయితే_ముగింపు
#లేకపోతే
#నిర్వచించు DEBUGASSERT(x) చేయి {} అయితే(0)
#అయితే_ముగింపు

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#ఒకవేళ defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#అన్తర్భావ <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#నిర్వచించు CURL_MSVC_CRTDBG_INIT()                                             \
  చేయి {                                                                      \
    సమితి_error_బహుళకం(బహిర్గతం_గమ్యం_STDERR);  /* needs stdlib.h */                  \
    Crt_సమితి_Report_బహుళకం(_CRT_ASSERT, CRTDBG_బహుళకం_FILE | CRTDBG_బహుళకం_DEBUG); \
    Crt_సమితి_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_సమితి_Report_బహుళకం(_CRT_ERROR, CRTDBG_బహుళకం_FILE | CRTDBG_బహుళకం_DEBUG);  \
    Crt_సమితి_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_సమితి_Report_బహుళకం(_CRT_WARN, CRTDBG_బహుళకం_FILE | CRTDBG_బహుళకం_DEBUG);   \
    Crt_సమితి_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } అయితే(0)
#లేకపోతే
#నిర్వచించు CURL_MSVC_CRTDBG_INIT() చేయి {} అయితే(0)
#అయితే_ముగింపు

#ఒకవేళ defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#అన్తర్భావ <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#నిర్వచించు CURL_DUMP_MODULE_PATHS(printf_f)                               \
  చేయి {                                                                 \
    HANDLE hnd = INVALID_HANDLE_విలువ;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_కొలత = పరిమాణం(MODULEENTRY32);                                \
                                                                       \
    చేయి {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } అయితే(hnd == INVALID_HANDLE_విలువ &&                             \
            Get_చివరి_Error() == ERROR_BAD_నిడివి);                       \
                                                                       \
    ఒకవేళ(hnd == INVALID_HANDLE_విలువ)                                    \
      వెళ్ళు error;                                                      \
                                                                       \
    ఒకవేళ(!Module32_మొదటి(hnd, &mod))                                      \
      వెళ్ళు error;                                                      \
                                                                       \
    చేయి {                                                               \
      అక్షరం బఫరు[పరిమాణం(mod.sz_Exe_మార్గం) * 2];                          \
      అక్షరం *మార్గం; /* points to stack allocated buffer */               \
      ఒకవేళ(పరిమాణం(*mod.sz_Exe_మార్గం) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        ఒకవేళ(!Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_మార్గం, -1, \
                                బఫరు, పరిమాణం(బఫరు), ఖాళీ, ఖాళీ))   \
          వెళ్ళు error;                                                  \
        మార్గం = (అక్షరం *)బఫరు;                                         \
      }                                                                \
      లేకపోతే                                                             \
        మార్గం = (అక్షరం *)mod.sz_Exe_మార్గం;                                  \
      (printf_f)("%s\n", మార్గం);                                        \
    } అయితే(Module32_తదుపరి(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    ఒకవేళ(hnd != INVALID_HANDLE_విలువ)                                    \
      CloseHandle(hnd);                                                \
  } అయితే(0)
#లేకపోతే
#నిర్వచించు CURL_DUMP_MODULE_PATHS(printf_f) చేయి {} అయితే(0)
#అయితే_ముగింపు

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#నిర్వచితమైతే USE_WINSOCK
#నిర్వచించు SOCKERRNO         ((పూర్ణాంకం)WSAGet_చివరి_Error())
#నిర్వచించు సమితి_SOCKERRNO(x)  WSASet_చివరి_Error((పూర్ణాంకం)(x))
#లేకపోతే
#నిర్వచించు SOCKERRNO         errno
#నిర్వచించు సమితి_SOCKERRNO(x)  (errno = (x))
#అయితే_ముగింపు

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#నిర్వచితమైతే USE_WINSOCK
#నిర్వచించు SOCKEACCES        WSAEACCES
#నిర్వచించు SOCKEADDRINUSE    WSAEADDRINUSE
#నిర్వచించు SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#నిర్వచించు SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#నిర్వచించు SOCKEBADF         WSAEBADF
#నిర్వచించు SOCKECONNREFUSED  WSAECONNREFUSED
#నిర్వచించు SOCKECONNRESET    WSAECONNRESET
#నిర్వచించు SOCKEINPROGRESS   WSAEINPROGRESS
#నిర్వచించు SOCKEINTR         WSAEINTR
#నిర్వచించు SOCKEINVAL        WSAEINVAL
#నిర్వచించు SOCKEISCONN       WSAEISCONN
#నిర్వచించు SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#నిర్వచించు SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#నిర్వచించు SOCKETIMEDOUT     WSAETIMEDOUT
#నిర్వచించు SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#లేకపోతే
#నిర్వచించు SOCKEACCES        EACCES
#నిర్వచించు SOCKEADDRINUSE    EADDRINUSE
#నిర్వచించు SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#నిర్వచించు SOCKEAFNOSUPPORT  EAFNOSUPPORT
#నిర్వచించు SOCKEBADF         EBADF
#నిర్వచించు SOCKECONNREFUSED  ECONNREFUSED
#నిర్వచించు SOCKECONNRESET    ECONNRESET
#నిర్వచించు SOCKEINPROGRESS   EINPROGRESS
#నిర్వచించు SOCKEINTR         EINTR
#నిర్వచించు SOCKEINVAL        EINVAL
#నిర్వచించు SOCKEISCONN       EISCONN
#నిర్వచించు SOCKEMSGSIZE      EMSGSIZE
#నిర్వచించు SOCKENOMEM        ENOMEM
#నిర్వచితమైతే ETIMEDOUT
#నిర్వచించు SOCKETIMEDOUT     ETIMEDOUT
#అయితే_ముగింపు
#నిర్వచించు SOCKEWOULDBLOCK   EWOULDBLOCK
#అయితే_ముగింపు

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#ఒకవేళ !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#నిర్వచించు SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#లేకపోతే
#నిర్వచించు SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#అయితే_ముగింపు

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#నిర్వచితమైతే __VMS
#నిర్వచించు argv_అంశం_t  __char_ptr32
#లేదావేళ defined(_UNICODE)
#నిర్వచించు argv_అంశం_t  wchar_t *
#లేకపోతే
#నిర్వచించు argv_అంశం_t  అక్షరం *
#అయితే_ముగింపు

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#నిర్వచించు ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#అన్తర్భావ "curlx/warnless.h"

#నిర్వచితమైతే _WIN32
#  undef పఠ
#  define పఠ(fd, buf, లెక్క)  (ssize_t)_read(fd, buf, curlx_uztoui(లెక్క))
#  undef లిఖ
#  define లిఖ(fd, buf, లెక్క) (ssize_t)_write(fd, buf, curlx_uztoui(లెక్క))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#అయితే_ముగింపు

/*
 * Definition of our NOP statement Object-like macro
 */
#నిర్వచితం_కాకపోతే Curl_nop_stmt
#నిర్వచించు Curl_nop_stmt చేయి {} అయితే(0)
#అయితే_ముగింపు

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#ఒకవేళ defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  ఒకవేళ defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#అయితే_ముగింపు

/*
 * shutdown() flags for systems that do not define them
 */
#నిర్వచితం_కాకపోతే SHUT_RD
#నిర్వచించు SHUT_RD 0x00
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SHUT_WR
#నిర్వచించు SHUT_WR 0x01
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SHUT_RDWR
#నిర్వచించు SHUT_RDWR 0x02
#అయితే_ముగింపు

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#ఒకవేళ !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#నిర్వచించు S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#అయితే_ముగింపు

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#ఒకవేళ !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#నిర్వచించు S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#అయితే_ముగింపు

/* For MSVC (all versions as of VS2022) */
#నిర్వచితం_కాకపోతే STDIN_FILENO
#నిర్వచించు STDIN_FILENO  fileno(stdin)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే STDOUT_FILENO
#నిర్వచించు STDOUT_FILENO  fileno(stdout)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే STDERR_FILENO
#నిర్వచించు STDERR_FILENO  fileno(stderr)
#అయితే_ముగింపు

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#నిర్వచితమైతే O_ద్విమాన  /* for _WIN32 || MSDOS */
#నిర్వచించు CURL_O_ద్విమాన O_ద్విమాన
#లేదావేళ defined(O_ద్విమాన_2)  /* __CYGWIN__ */
#నిర్వచించు CURL_O_ద్విమాన O_ద్విమాన_2
#లేకపోతే
#నిర్వచించు CURL_O_ద్విమాన 0
#అయితే_ముగింపు

/* Requires io.h when available */
#నిర్వచితమైతే MSDOS
#నిర్వచించు CURL_BINMODE(ప్రవాహం) (శూన్య)setmode(fileno(ప్రవాహం), CURL_O_ద్విమాన)
#లేదావేళ defined(_WIN32) || defined(__CYGWIN__)
#నిర్వచించు CURL_BINMODE(ప్రవాహం) (శూన్య)_setmode(fileno(ప్రవాహం), CURL_O_ద్విమాన)
#లేకపోతే
#నిర్వచించు CURL_BINMODE(ప్రవాహం) (శూన్య)(ప్రవాహం)
#అయితే_ముగింపు

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#ఒకవేళ defined(_WIN32) || defined(MSDOS)
#నిర్వచించు FOPEN_READTEXT   "rt"
#నిర్వచించు FOPEN_WRITETEXT  "wt"
#నిర్వచించు FOPEN_APPENDTEXT "at"
#లేదావేళ defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#నిర్వచించు FOPEN_READTEXT   "rt"
#నిర్వచించు FOPEN_WRITETEXT  "w"
#నిర్వచించు FOPEN_APPENDTEXT "a"
#లేకపోతే
#నిర్వచించు FOPEN_READTEXT   "r"
#నిర్వచించు FOPEN_WRITETEXT  "w"
#నిర్వచించు FOPEN_APPENDTEXT "a"
#అయితే_ముగింపు

/* for systems that do not detect this in configure */
#నిర్వచితం_కాకపోతే CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T చిరునామా_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T గుర్తులేని అక్షరం
#  లేకపోతే
/* use a sensible default */
#    define CURL_SA_FAMILY_T గుర్తులేని పొట్టి
#  endif
#అయితే_ముగింపు

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#నిర్వచించు CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#నిర్వచించు CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#నిర్వచించు CURL_CSTRLEN(x) (పరిమాణం(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#నిర్వచించు STRCONST(x) x, CURL_CSTRLEN(x)

#నిర్వచించు CURL_ARRAYSIZE(A) (పరిమాణం(A) / పరిమాణం((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#నిర్వచించు STRERROR_LEN 256

#నిర్వచితం_కాకపోతే CURL_DID_జ్ఞాపకం_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
రకనిర్వచనం శూన్య *(*curl_malloc_callback)(size_t కొలత);
రకనిర్వచనం శూన్య (*curl_free_callback)(శూన్య *ptr);
రకనిర్వచనం శూన్య *(*curl_realloc_callback)(శూన్య *ptr, size_t కొలత);
రకనిర్వచనం అక్షరం *(*curl_strdup_callback)(స్థిరరాశి అక్షరం *str);
రకనిర్వచనం శూన్య *(*curl_calloc_callback)(size_t nmemb, size_t కొలత);
#నిర్వచించు CURL_DID_జ్ఞాపకం_FUNC_TYPEDEFS
#అయితే_ముగింపు

బాహ్య curl_malloc_callback Curl_cmalloc;
బాహ్య curl_free_callback Curl_cfree;
బాహ్య curl_realloc_callback Curl_crealloc;
బాహ్య curl_strdup_callback Curl_cstrdup;
బాహ్య curl_calloc_callback Curl_ccalloc;

/*
 * curlx_safefree() defined as a macro to allow MemoryTracking feature
 * to log free() calls at same location where curlx_safefree() is used.
 * This macro also assigns NULL to given pointer when free'd.
 */
#నిర్వచించు curlx_safefree(ptr) \
  చేయి {                      \
    curlx_free(ptr);        \
    (ptr) = ఖాళీ;           \
  } అయితే(0)

#అన్తర్భావ <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_MEMDEBUG
#అయితే_ముగింపు

#నిర్వచితమైతే CURL_MEMDEBUG
#నిర్వచితమైతే __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  ఒకవేళ __clang_major__ >= 4
#  define ALLOC_కొలత(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  లేకపోతే
#  define ALLOC_కొలత(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#లేదావేళ defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_కొలత(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#లేదావేళ defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(పరిమిత)
#  define ALLOC_కొలత(s)
#  define ALLOC_SIZE2(n, s)
#లేకపోతే
#  define ALLOC_FUNC
#  define ALLOC_కొలత(s)
#  define ALLOC_SIZE2(n, s)
#అయితే_ముగింపు

బాహ్య FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN శూన్య curl_dbg_free(శూన్య *ptr, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_FUNC ALLOC_కొలత(1)
  శూన్య *curl_dbg_malloc(size_t wantedsize, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  శూన్య *curl_dbg_calloc(size_t wanted_elements, size_t wanted_కొలత,
                        పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_కొలత(2)
  శూన్య *curl_dbg_realloc(శూన్య *ptr, size_t wantedsize, పూర్ణాంకం పంక్తి,
                         స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_FUNC
  అక్షరం *curl_dbg_strdup(స్థిరరాశి అక్షరం *str, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
#ఒకవేళ defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(స్థిరరాశి wchar_t *str, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
#అయితే_ముగింపు

CURL_EXTERN శూన్య curl_dbg_memdebug(స్థిరరాశి అక్షరం *logname);
CURL_EXTERN శూన్య curl_dbg_memlimit(దీర్ఘ హద్దు);
CURL_EXTERN శూన్య curl_dbg_log(స్థిరరాశి అక్షరం *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(పూర్ణాంకం domain, పూర్ణాంకం type, పూర్ణాంకం protocol,
                                          పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN శూన్య curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN పూర్ణాంకం curl_dbg_sclose(curl_socket_t sockfd,
                                పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          శూన్య *saddr, శూన్య *saddrlen,
                                          పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
#నిర్వచితమైతే HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, శూన్య *saddr,
                                           శూన్య *saddrlen, పూర్ణాంకం flags,
                                           పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SOCKETPAIR
CURL_EXTERN పూర్ణాంకం curl_dbg_socketpair(పూర్ణాంకం domain, పూర్ణాంకం type, పూర్ణాంకం protocol,
                                    curl_socket_t socket_సదిశ[2],
                                    పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
#అయితే_ముగింపు

/* FILE functions */
CURL_EXTERN పూర్ణాంకం curl_dbg_fclose(FILE *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(స్థిరరాశి అక్షరం *file, స్థిరరాశి అక్షరం *బహుళకం,
                                            పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(స్థిరరాశి అక్షరం *file,
                                              స్థిరరాశి అక్షరం *బహుళకం, FILE *fh,
                                              పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(పూర్ణాంకం filedes, స్థిరరాశి అక్షరం *బహుళకం,
                                             పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం);

#నిర్వచించు sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#నిర్వచించు fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#నిర్వచించు CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#నిర్వచించు CURL_FREEADDRINFO(దత్తాంశం) \
  curl_dbg_freeaddrinfo(దత్తాంశం, __LINE__, __FILE__)
#నిర్వచించు CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((పూర్ణాంకం)(domain), type, protocol, __LINE__, __FILE__)
#నిర్వచితమైతే HAVE_SOCKETPAIR
#నిర్వచించు CURL_SOCKETPAIR(domain, type, protocol, socket_సదిశ) \
  curl_dbg_socketpair((పూర్ణాంకం)(domain), type, protocol, socket_సదిశ, \
                      __LINE__, __FILE__)
#అయితే_ముగింపు
#నిర్వచించు CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#నిర్వచితమైతే HAVE_ACCEPT4
#నిర్వచించు CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#అయితే_ముగింపు

#లేకపోతే /* !CURL_MEMDEBUG */

#నిర్వచించు sclose(x) CURL_SCLOSE(x)
#నిర్వచించు fake_sclose(x) Curl_nop_stmt

#నిర్వచించు CURL_GETADDRINFO getaddrinfo
#నిర్వచించు CURL_FREEADDRINFO freeaddrinfo
#నిర్వచించు CURL_SOCKET socket
#నిర్వచితమైతే HAVE_SOCKETPAIR
#నిర్వచించు CURL_SOCKETPAIR socketpair
#అయితే_ముగింపు
#నిర్వచించు CURL_ACCEPT accept
#నిర్వచితమైతే HAVE_ACCEPT4
#నిర్వచించు CURL_ACCEPT4 accept4
#అయితే_ముగింపు

#అయితే_ముగింపు /* CURL_MEMDEBUG */

/* Allocator macros */

#నిర్వచితమైతే _WIN32
#నిర్వచించు CURLX_STRDUP_దిగువ _strdup
#లేకపోతే
#నిర్వచించు CURLX_STRDUP_దిగువ strdup
#అయితే_ముగింపు

#నిర్వచితమైతే CURL_MEMDEBUG

#నిర్వచించు curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#నిర్వచించు curlx_malloc(కొలత)         curl_dbg_malloc(కొలత, __LINE__, __FILE__)
#నిర్వచించు curlx_calloc(nbelem, కొలత) \
  curl_dbg_calloc(nbelem, కొలత, __LINE__, __FILE__)
#నిర్వచించు curlx_realloc(ptr, కొలత) \
  curl_dbg_realloc(ptr, కొలత, __LINE__, __FILE__)
#నిర్వచించు curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#నిర్వచితమైతే _WIN32
#నిర్వచితమైతే UNICODE
#నిర్వచించు curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#లేకపోతే
#నిర్వచించు curlx_tcsdup               curlx_strdup
#అయితే_ముగింపు
#అయితే_ముగింపు /* _WIN32 */

#లేకపోతే /* !CURL_MEMDEBUG */

#నిర్వచితమైతే BUILDING_LIBCURL
#నిర్వచించు curlx_strdup               Curl_cstrdup
#నిర్వచించు curlx_malloc               Curl_cmalloc
#నిర్వచించు curlx_calloc               Curl_ccalloc
#నిర్వచించు curlx_realloc              Curl_crealloc
#నిర్వచించు curlx_free                 Curl_cfree
#లేకపోతే /* !BUILDING_LIBCURL */
#నిర్వచించు curlx_strdup               CURLX_STRDUP_దిగువ
#నిర్వచించు curlx_malloc               జ్ఞాపకం_ఇవ్వు
#నిర్వచించు curlx_calloc               సున్నా_జ్ఞాపకం_ఇవ్వు
#నిర్వచించు curlx_realloc              realloc
#నిర్వచించు curlx_free                 విడుదల
#అయితే_ముగింపు /* BUILDING_LIBCURL */

#నిర్వచితమైతే _WIN32
#నిర్వచితమైతే UNICODE
#నిర్వచించు curlx_tcsdup               curlx_wcsdup
#లేకపోతే
#నిర్వచించు curlx_tcsdup               curlx_strdup
#అయితే_ముగింపు
#అయితే_ముగింపు /* _WIN32 */

#అయితే_ముగింపు /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#ఒకవేళ defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
నిర్మాణం passwd;
పూర్ణాంకం getpwuid_r(uid_t uid, నిర్మాణం passwd *pwd, అక్షరం *buf,
               size_t buflen, నిర్మాణం passwd **ఫలితం);
#అయితే_ముగింపు

#నిర్వచితమైతే UNITTESTS
#నిర్వచించు UNITTEST
#లేకపోతే
#నిర్వచించు UNITTEST స్థిర
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NGHTTP2
#నిర్వచించు USE_HTTP2
#అయితే_ముగింపు

#ఒకవేళ (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#నిర్వచితమైతే CURL_WITH_MULTI_SSL
#దోషం "MultiSSL combined with QUIC is not supported"
#అయితే_ముగింపు

#నిర్వచించు USE_HTTP3
#అయితే_ముగింపు

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#నిర్వచితం_కాకపోతే __EMSCRIPTEN__
#నిర్వచించు CURL_TCP_NODELAY_SUPPORTED
#అయితే_ముగింపు

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#ఒకవేళ defined(HAVE_BASENAME) && defined(_WIN32)
#నిర్వచనం_రద్దు HAVE_BASENAME
#అయితే_ముగింపు

#ఒకవేళ defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#ఒకవేళ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#లేదావేళ !defined(UNIX_మార్గం_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_మార్గం_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  అక్షరం sun_మార్గం[UNIX_మార్గం_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_నిర్వచిస్తుంది_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_నిర్వచిస్తుంది_WARNING
#    endif
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#లేదావేళ defined(అంతఃవరుస)
#  define CURL_INLINE అంతఃవరుస /* 'inline' defined, assumed correct */
#లేదావేళ defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE అంతఃవరుస /* 'inline' keyword supported */
#లేదావేళ defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE అంతఃవరుస /* 'inline' keyword supported */
#లేదావేళ defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#లేదావేళ defined(_MSC_VER)
#  define CURL_INLINE __inline
#లేకపోతే
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /*ఖాళీలు*/
#అయితే_ముగింపు

/* Detect if compiler supports C99 variadic macros */
#ఒకవేళ (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#నిర్వచించు CURL_HAVE_MACRO_VARARG
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు))
#నిర్వచించు CURLVERBOSE
#నిర్వచించు VERBOSE(x) x
#నిర్వచించు NOVERBOSE(x) Curl_nop_stmt
#లేకపోతే
#నిర్వచించు VERBOSE(x) Curl_nop_stmt
#నిర్వచించు NOVERBOSE(x) x
#అయితే_ముగింపు

/* For FreeBSD it is included from curl/curl.h */
#ఒకవేళ defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#అన్తర్భావ <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#అయితే_ముగింపు

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#ఒకవేళ defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#నిర్వచించు CURL_USE_SOCK_NONBLOCK
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#నిర్వచితమైతే _WIN32
#ఒకవేళ defined(_MSC_VER) && defined(NTDDI_సంస్కరణ) && \
  (NTDDI_సంస్కరణ >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#సూచన comment(lib, "volatileaccessu.lib")
#నిర్వచించు curlx_memzero_దిగువ(buf, కొలత)  SecureZeroMemory2(buf, కొలత)
#లేకపోతే
#నిర్వచించు curlx_memzero_దిగువ(buf, కొలత)  Secure_Zero_జ్ఞాపకం(buf, కొలత)
#అయితే_ముగింపు
#లేదావేళ defined(HAVE_MEMSET_S)
#నిర్వచించు curlx_memzero_దిగువ(buf, కొలత)  (శూన్య)memset_s(buf, కొలత, 0, కొలత)
#లేదావేళ defined(HAVE_MEMSET_EXPLICIT)
#నిర్వచించు curlx_memzero_దిగువ(buf, కొలత)  (శూన్య)memset_explicit(buf, 0, కొలత)
#లేదావేళ defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#నిర్వచించు curlx_memzero_దిగువ(buf, కొలత)  explicit_bzero(buf, కొలత)
#లేదావేళ defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#నిర్వచించు curlx_memzero_దిగువ(buf, కొలత)  (శూన్య)explicit_memset(buf, 0, కొలత)
#అయితే_ముగింపు
#అయితే_ముగింపు /* !_CURL_LOCAL_MEMZERO */

#నిర్వచితం_కాకపోతే curlx_memzero_దిగువ
#నిర్వచించు USE_CURLX_MEMZERO
శూన్య curlx_memzero_దిగువ(శూన్య *buf, size_t కొలత);
#అయితే_ముగింపు
శూన్య curlx_memzero(శూన్య *buf, size_t కొలత);
శూన్య curlx_strzero(శూన్య *buf);

#అయితే_ముగింపు /* HEADER_CURL_SETUP_H */
