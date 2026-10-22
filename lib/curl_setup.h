#که_نه_تعریف_شوی سرلیک_CURL_تیاری_H
#تعریف_کړه سرلیک_CURL_تیاری_H
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
#تعریف_کړه CURL_NO_OLDIES
#د_که_پای

/* Set default _WIN32_WINNT */
#که_تعریف_شوی __MINGW32__
#ضمن <_mingw.h>
#د_که_پای

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
#د_که_پای

#که_تعریف_شوی __APPLE__
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
#إذا defined(موخه_OS_MAC) && موخه_OS_MAC && \
  defined(موخه_OS_OSX) && !موخه_OS_OSX && \
  (!defined(موخه_OS_IPHONE) || !موخه_OS_IPHONE) && \
  (!defined(موخه_OS_SIMULATOR) || !موخه_OS_SIMULATOR)
#تعریف_لغوه موخه_OS_OSX
#تعریف_کړه موخه_OS_OSX موخه_OS_MAC
#د_که_پای
#د_که_پای

#إذا defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#تېروتنه "mingw-w64 3.0 or greater required"
#د_که_پای

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#إذا defined(_MSC_VER) && (_MSC_VER < 1600)
#تېروتنه "Ancient versions of Visual Studio are no longer supported due to bugs."
#د_که_پای

#که_تعریف_شوی _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#لارښوونه warning(disable:4127)
#که_نه_تعریف_شوی _CRT_SECURE_NO_WARNINGS
#تعریف_کړه _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#د_که_پای
#د_که_پای /* _MSC_VER */

#که_تعریف_شوی _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_منځنۍ
#  define WIN32_LEAN_AND_منځنۍ
#  endif
#  ifndef NOGDI
#  define NOGDI
#  endif

/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
#  إذا (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    إذا WINAPI_FAMILY_برخه_کول(WINAPI_برخه_کول_APP) &&  \
       !WINAPI_FAMILY_برخه_کول(WINAPI_برخه_کول_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#تعریف_لغوه SECURITY_KERNEL
#تعریف_لغوه SECURITY_WIN32
#تعریف_کړه SECURITY_WIN32  /* for <sspi.h> */
#د_که_پای

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#که_تعریف_شوی HAVE_CONFIG_H

#ضمن "curl_config.h"

#وإلا /* HAVE_CONFIG_H */

#که_تعریف_شوی _WIN32
#  include "config-win32.h"
#د_که_پای

#که_تعریف_شوی macintosh
#  include "config-mac.h"
#د_که_پای

#که_تعریف_شوی __OS400__
#  include "config-os400.h"
#د_که_پای

#د_که_پای /* HAVE_CONFIG_H */

#که_تعریف_شوی _WIN32
#  إذا defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build موخه is Windows Vista (0x0600)
#  endif

#  إذا !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#د_که_پای

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#که_تعریف_شوی HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#د_که_پای

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#که_تعریف_شوی NEED_THREAD_خوندي
#  ifndef THREAD_خوندي
#  define THREAD_خوندي
#  endif
#د_که_پای

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#که_تعریف_شوی NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#د_که_پای

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#إذا defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#د_که_پای

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#که_تعریف_شوی HAVE_تارونه_POSIX
#  include <pthread.h>
#د_که_پای

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#که_تعریف_شوی CURL_DISABLE_HTTP
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
#  ifndef CURL_DISABLE_سرلیکونه_API
#  define CURL_DISABLE_سرلیکونه_API
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
#د_که_پای

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#که_تعریف_شوی __OS400__
#  include "setup-os400.h"
#د_که_پای

/*
 * VMS setup file includes some system headers.
 */
#که_تعریف_شوی __VMS
#  include "setup-vms.h"
#د_که_پای

/*
 * Windows setup file includes some system headers.
 */
#که_تعریف_شوی _WIN32
#  include "setup-win32.h"
#د_که_پای

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
#تعریف_کړه HAVE_PIPE2
#د_که_پای

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#تعریف_کړه CURL_CONC_MACROS_(A, B) A ## B
#تعریف_کړه CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#که_تعریف_شوی __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#د_که_پای

/* based on logic in "curl/mprintf.h" */
#إذا (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#إذا defined(__MINGW32__) && !defined(__clang__)
#تعریف_کړه CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#وإلا
#تعریف_کړه CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#د_که_پای
#وإلا
#تعریف_کړه CURL_PRINTF(fmt, arg)
#د_که_پای

/* Override default printf mask check rules in "curl/mprintf.h" */
#تعریف_کړه CURL_لنډمهاله_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#إذا defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#تعریف_کړه __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#د_که_پای

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#إذا defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  إذا موخه_OS_MAC && !(defined(موخه_OS_IPHONE) && موخه_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#د_که_پای

#که_تعریف_شوی USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  إذا defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#د_که_پای

#که_تعریف_شوی USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#د_که_پای

#که_تعریف_شوی macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#د_که_پای

#که_تعریف_شوی __AMIGA__
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
#د_که_پای

/* Whether to use eventfd() */
#إذا defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#تعریف_کړه USE_EVENTFD
#د_که_پای

#که_تعریف_شوی SO_NOSIGPIPE
#تعریف_کړه USE_SO_NOSIGPIPE
#د_که_پای

#ضمن <stdio.h>
#ضمن <assert.h>

#که_تعریف_شوی __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#د_که_پای

#ضمن <stdint.h>
#تعریف_کړه HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#که_تعریف_شوی __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#تعریف_کړه uint32_t بینښه صحيح
#د_که_پای

/* Disable uintptr_t for targets known to miss it from stdint.h */
#که_تعریف_شوی __OS400__
#تعریف_لغوه HAVE_UINTPTR_T
#د_که_پای

#ضمن <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#ضمن <sys/types.h>

#که_تعریف_شوی _WIN32
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
#  define LSEEK_ERROR                     ((دننه_واټن_t)-1)
#وإلاإذا defined(__AMIGA__)
#  define curl_lseek(fd, دننه_واټن, whence)  lseek(fd, (off_t)(دننه_واټن), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#وإلا
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#د_که_پای

#که_نه_تعریف_شوی SIZEOF_وخت_T
/* assume default size of time_t to be 32 bits */
#تعریف_کړه SIZEOF_وخت_T 4
#د_که_پای

#که_نه_تعریف_شوی SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  إذا defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  وإلا
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#د_که_پای

#إذا SIZEOF_CURL_SOCKET_T < 8
#که_تعریف_شوی USE_WINSOCK
#  define FMT_SOCKET_T "u"
#وإلا
#  define FMT_SOCKET_T "d"
#د_که_پای
#وإلاإذا defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#وإلا
#  define FMT_SOCKET_T "qd"
#د_که_پای

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#که_نه_تعریف_شوی SIZEOF_OFF_T
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
#د_که_پای

#إذا SIZEOF_CURL_OFF_T < 8
#تېروتنه "too small curl_off_t"
#وإلا
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#د_که_پای
#تعریف_کړه CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#تعریف_کړه FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#تعریف_کړه FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#إذا SIZEOF_وخت_T == 4
#  ifdef HAVE_وخت_T_UNSIGNED
#  define وخت_T_MAX UINT_MAX
#  define وخت_T_MIN 0
#  وإلا
#  define وخت_T_MAX لوی_عدد
#  define وخت_T_MIN INT_MIN
#  endif
#وإلا
#  ifdef HAVE_وخت_T_UNSIGNED
#  define وخت_T_MAX 0xFFFFFFFFFFFFFFFF
#  define وخت_T_MIN 0
#  وإلا
#  define وخت_T_MAX 0x7FFFFFFFFFFFFFFF
#  define وخت_T_MIN (-وخت_T_MAX - 1)
#  endif
#د_که_پای

#که_نه_تعریف_شوی SIZE_MAX
/* some limits.h headers have this defined, some do not */
#إذا defined(SIZEOF_اندازه_T) && (SIZEOF_اندازه_T > 4)
#تعریف_کړه SIZE_MAX 18446744073709551615U
#وإلا
#تعریف_کړه SIZE_MAX 4294967295U
#د_که_پای
#د_که_پای

#که_نه_تعریف_شوی SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#إذا defined(SIZEOF_اندازه_T) && (SIZEOF_اندازه_T > 4)
#تعریف_کړه SSIZE_MAX 9223372036854775807
#وإلا
#تعریف_کړه SSIZE_MAX 2147483647
#د_که_پای
#د_که_پای

#إذا SIZEOF_LONG > SIZEOF_اندازه_T
#تېروتنه "unexpected: 'long' is larger than 'size_t'"
#د_که_پای

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#که_نه_تعریف_شوی GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 صحيح
#  وإلا
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#د_که_پای

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#که_تعریف_شوی _WIN32

#  define DIR_CHAR      "\\"

#وإلا /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (حرف *)(z))
#    include <tcp.h>
#    undef کلمه
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     خارجي بنية tm *gmtime_r(ثابت time_t * ثابت timep, بنية tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#د_که_پای /* _WIN32 */

/* We want to use mutex when available. */
#إذا defined(HAVE_تارونه_POSIX) || defined(_WIN32)
#تعریف_کړه USE_MUTEX
#د_که_پای

/* threaded resolver is the only feature requiring threads. */
#که_تعریف_شوی USE_RESOLV_THREADED
#تعریف_کړه USE_تارونه
#د_که_پای

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
#د_که_پای

#که_تعریف_شوی USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#وإلاإذا defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#وإلا
#  define CURLRES_SYNCH
#د_که_پای

/* ---------------------------------------------------------------- */

#إذا defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#تعریف_کړه USE_LIBIDN2
#د_که_پای

#إذا defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#تېروتنه "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#د_که_پای

#إذا defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#تعریف_کړه USE_SSL    /* SSL support has been enabled */
#د_که_پای

#إذا defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#که_نه_تعریف_شوی OPENSSL_COEXIST
#تعریف_کړه OPENSSL_COEXIST
#د_که_پای
#د_که_پای

#إذا defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#تعریف_کړه NO_OLD_WC_نومونه
#د_که_پای

/* Single point where USE_SPNEGO definition might be defined */
#إذا !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#تعریف_کړه USE_SPNEGO
#د_که_پای

/* Single point where USE_KERBEROS5 definition might be defined */
#إذا !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#تعریف_کړه USE_KERBEROS5
#د_که_پای

/* Single point where USE_NTLM definition might be defined */
#که_تعریف_شوی CURL_ENABLE_NTLM
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
#د_که_پای

#إذا defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#تعریف_کړه USE_SSH
#د_که_پای

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#إذا defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#تعریف_کړه CURL_HAVE_DIAG
#د_که_پای

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#إذا defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_ناکارول_شوی_پایله __attribute__((warn_ناکارول_شوی_پایله))
#وإلاإذا defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_ناکارول_شوی_پایله __attribute__((warn_ناکارول_شوی_پایله))
#وإلا
#  define WARN_ناکارول_شوی_پایله
#د_که_پای

/* noreturn attribute */

#که_نه_تعریف_شوی CURL_NORETURN
#إذا (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#وإلاإذا defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#وإلا
#  define CURL_NORETURN
#د_که_پای
#د_که_پای

/* fallthrough attribute */

#که_نه_تعریف_شوی FALLTHROUGH
#إذا (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#وإلا
#  define FALLTHROUGH()  افعل {} طالما(0)
#د_که_پای
#د_که_پای

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
#د_که_پای

#که_تعریف_شوی HAVE_IO_H
#ضمن <io.h>
#د_که_پای

#که_تعریف_شوی HAVE_FCNTL_H
#ضمن <fcntl.h>
#د_که_پای

#إذا defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#ضمن <stdbool.h>
#د_که_پای

#که_تعریف_شوی HAVE_UNISTD_H
#ضمن <unistd.h>
#د_که_پای

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#که_تعریف_شوی HAVE_UINTPTR_T
#تعریف_کړه CURL_UNCONST(p) ((فراغ *)(uintptr_t)(ثابت فراغ *)(p))
#وإلا
#تعریف_کړه CURL_UNCONST(p) ((فراغ *)(p))  /* Fall back to simple cast */
#د_که_پای

#که_تعریف_شوی USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#د_که_پای

#که_تعریف_شوی __hpux
#  إذا !defined(XOPEN_سرچینه_EXTENDED) || defined(_KERNEL)
#    ifdef APP32_64_بټ_OFF_T
#      define OLD_APP32_64_بټ_OFF_T APP32_64_بټ_OFF_T
#      undef APP32_64_بټ_OFF_T
#    وإلا
#      undef OLD_APP32_64_بټ_OFF_T
#    endif
#  endif
#د_که_پای

#که_نه_تعریف_شوی _WIN32
#ضمن <sys/socket.h>  /* also for MSG_NOSIGNAL */
#د_که_پای

#ضمن "functypes.h"

#که_تعریف_شوی __hpux
#  إذا !defined(XOPEN_سرچینه_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64_بټ_OFF_T
#      define APP32_64_بټ_OFF_T OLD_APP32_64_بټ_OFF_T
#      undef OLD_APP32_64_بټ_OFF_T
#    endif
#  endif
#د_که_پای

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#که_نه_تعریف_شوی HAVE_STRUCT_TIMEVAL
بنية timeval {
  اوږد tv_sec;
  اوږد tv_usec;
};
#د_که_پای

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#که_تعریف_شوی MSG_NOSIGNAL
#تعریف_کړه SEND_4TH_ARG MSG_NOSIGNAL
#وإلا
#تعریف_کړه SEND_4TH_ARG 0
#د_که_پای

#که_تعریف_شوی __minix
/* Minix does not support recv on TCP sockets */
#تعریف_کړه sread(x, y, z) (ssize_t)اقرأ((RECV_TYPE_ARG1)(x), \
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

#تعریف_کړه sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#وإلا /* HAVE_RECV */
#که_نه_تعریف_شوی sread
#تېروتنه "Missing definition of macro sread"
#د_که_پای
#د_که_پای /* HAVE_RECV */

#که_تعریف_شوی __minix
/* Minix does not support send on TCP sockets */
#تعریف_کړه swrite(x, y, z) (ssize_t)اكتب((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#وإلاإذا defined(HAVE_SEND)
#که_تعریف_شوی SEND_NONCONST_ARG2
#تعریف_کړه swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#وإلا
#تعریف_کړه swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (ثابت SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#د_که_پای /* SEND_NONCONST_ARG2 */
#وإلا /* HAVE_SEND */
#که_نه_تعریف_شوی swrite
#تېروتنه "Missing definition of macro swrite"
#د_که_پای
#د_که_پای /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#که_تعریف_شوی HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#وإلاإذا defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#وإلاإذا defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#وإلاإذا defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#وإلا
#  define CURL_SCLOSE(x)  close(x)
#د_که_پای

/*
 * Stack-independent version of fcntl() on sockets:
 */
#که_تعریف_شوی USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#وإلا
#  define sfcntl  fcntl
#د_که_پای

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#إذا defined(__hpux) && !defined(HAVE_BOOL_T)
تعريفنوع صحيح bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#د_که_پای

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#که_نه_تعریف_شوی HAVE_BOOL_T
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
#د_که_پای

/* the type we use for storing a single boolean bit */
تعريفنوع بینښه صحيح curl_بټ;
#تعریف_کړه بټ(x) curl_بټ x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#که_نه_تعریف_شوی TRUE
#تعریف_کړه TRUE true
#د_که_پای
#که_نه_تعریف_شوی FALSE
#تعریف_کړه FALSE false
#د_که_پای

#ضمن "curl_ctype.h"

#إذا defined(DEBUGBUILD) && defined(NDEBUG)
#تېروتنه "Debug-enabled builds cannot be combined with NDEBUG"
#د_که_پای

/*
 * Macro used to include code only in debug builds.
 */
#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه DEBUGF(x) x
#وإلا
#تعریف_کړه DEBUGF(x) افعل {} طالما(0)
#د_که_پای

/*
 * Macro used to include assertion code only in debug builds.
 */
#تعریف_لغوه DEBUGASSERT
#که_تعریف_شوی DEBUGBUILD
#که_تعریف_شوی CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#تعریف_کړه DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#وإلا
#تعریف_کړه DEBUGASSERT(x) أكد(x)
#د_که_پای
#وإلا
#تعریف_کړه DEBUGASSERT(x) افعل {} طالما(0)
#د_که_پای

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#إذا defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#ضمن <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#تعریف_کړه CURL_MSVC_CRTDBG_INIT()                                             \
  افعل {                                                                      \
    سېټ_error_ډېرپېښ(بهر_منزل_STDERR);  /* needs stdlib.h */                  \
    Crt_سېټ_Report_ډېرپېښ(_CRT_ASSERT, CRTDBG_ډېرپېښ_FILE | CRTDBG_ډېرپېښ_DEBUG); \
    Crt_سېټ_Report_File(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    Crt_سېټ_Report_ډېرپېښ(_CRT_ERROR, CRTDBG_ډېرپېښ_FILE | CRTDBG_ډېرپېښ_DEBUG);  \
    Crt_سېټ_Report_File(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    Crt_سېټ_Report_ډېرپېښ(_CRT_WARN, CRTDBG_ډېرپېښ_FILE | CRTDBG_ډېرپېښ_DEBUG);   \
    Crt_سېټ_Report_File(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } طالما(0)
#وإلا
#تعریف_کړه CURL_MSVC_CRTDBG_INIT() افعل {} طالما(0)
#د_که_پای

#إذا defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#ضمن <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#تعریف_کړه CURL_DUMP_MODULE_PATHS(printf_f)                               \
  افعل {                                                                 \
    HANDLE hnd = INVALID_HANDLE_ارزښت;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_اندازه = حجم(MODULEENTRY32);                                \
                                                                       \
    افعل {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } طالما(hnd == INVALID_HANDLE_ارزښت &&                             \
            Get_وروستی_Error() == ERROR_BAD_اوږدتیا);                       \
                                                                       \
    إذا(hnd == INVALID_HANDLE_ارزښت)                                    \
      اذهب error;                                                      \
                                                                       \
    إذا(!Module32_لومړی(hnd, &mod))                                      \
      اذهب error;                                                      \
                                                                       \
    افعل {                                                               \
      حرف بفر[حجم(mod.sz_Exe_لار) * 2];                          \
      حرف *لار; /* points to stack allocated buffer */               \
      إذا(حجم(*mod.sz_Exe_لار) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        إذا(!Wide_Char_منزل_Multi_Byte(CP_ACP, 0, (WCHAR *)mod.sz_Exe_لار, -1, \
                                بفر, حجم(بفر), عدم, عدم))   \
          اذهب error;                                                  \
        لار = (حرف *)بفر;                                         \
      }                                                                \
      وإلا                                                             \
        لار = (حرف *)mod.sz_Exe_لار;                                  \
      (printf_f)("%s\n", لار);                                        \
    } طالما(Module32_بل(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    إذا(hnd != INVALID_HANDLE_ارزښت)                                    \
      CloseHandle(hnd);                                                \
  } طالما(0)
#وإلا
#تعریف_کړه CURL_DUMP_MODULE_PATHS(printf_f) افعل {} طالما(0)
#د_که_پای

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#که_تعریف_شوی USE_WINSOCK
#تعریف_کړه SOCKERRNO         ((صحيح)WSAGet_وروستی_Error())
#تعریف_کړه سېټ_SOCKERRNO(x)  WSASet_وروستی_Error((صحيح)(x))
#وإلا
#تعریف_کړه SOCKERRNO         errno
#تعریف_کړه سېټ_SOCKERRNO(x)  (errno = (x))
#د_که_پای

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#که_تعریف_شوی USE_WINSOCK
#تعریف_کړه SOCKEACCES        WSAEACCES
#تعریف_کړه SOCKEADDRINUSE    WSAEADDRINUSE
#تعریف_کړه SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#تعریف_کړه SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#تعریف_کړه SOCKEBADF         WSAEBADF
#تعریف_کړه SOCKECONNREFUSED  WSAECONNREFUSED
#تعریف_کړه SOCKECONNRESET    WSAECONNRESET
#تعریف_کړه SOCKEINPROGRESS   WSAEINPROGRESS
#تعریف_کړه SOCKEINTR         WSAEINTR
#تعریف_کړه SOCKEINVAL        WSAEINVAL
#تعریف_کړه SOCKEISCONN       WSAEISCONN
#تعریف_کړه SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#تعریف_کړه SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#تعریف_کړه SOCKETIMEDOUT     WSAETIMEDOUT
#تعریف_کړه SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#وإلا
#تعریف_کړه SOCKEACCES        EACCES
#تعریف_کړه SOCKEADDRINUSE    EADDRINUSE
#تعریف_کړه SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#تعریف_کړه SOCKEAFNOSUPPORT  EAFNOSUPPORT
#تعریف_کړه SOCKEBADF         EBADF
#تعریف_کړه SOCKECONNREFUSED  ECONNREFUSED
#تعریف_کړه SOCKECONNRESET    ECONNRESET
#تعریف_کړه SOCKEINPROGRESS   EINPROGRESS
#تعریف_کړه SOCKEINTR         EINTR
#تعریف_کړه SOCKEINVAL        EINVAL
#تعریف_کړه SOCKEISCONN       EISCONN
#تعریف_کړه SOCKEMSGSIZE      EMSGSIZE
#تعریف_کړه SOCKENOMEM        ENOMEM
#که_تعریف_شوی ETIMEDOUT
#تعریف_کړه SOCKETIMEDOUT     ETIMEDOUT
#د_که_پای
#تعریف_کړه SOCKEWOULDBLOCK   EWOULDBLOCK
#د_که_پای

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#إذا !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#تعریف_کړه SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#وإلا
#تعریف_کړه SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#د_که_پای

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#که_تعریف_شوی __VMS
#تعریف_کړه argv_توکی_t  __char_ptr32
#وإلاإذا defined(_UNICODE)
#تعریف_کړه argv_توکی_t  wchar_t *
#وإلا
#تعریف_کړه argv_توکی_t  حرف *
#د_که_پای

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#تعریف_کړه ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#ضمن "curlx/warnless.h"

#که_تعریف_شوی _WIN32
#  undef اقرأ
#  define اقرأ(fd, buf, شمېر)  (ssize_t)_read(fd, buf, curlx_uztoui(شمېر))
#  undef اكتب
#  define اكتب(fd, buf, شمېر) (ssize_t)_write(fd, buf, curlx_uztoui(شمېر))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#د_که_پای

/*
 * Definition of our NOP statement Object-like macro
 */
#که_نه_تعریف_شوی Curl_nop_stmt
#تعریف_کړه Curl_nop_stmt افعل {} طالما(0)
#د_که_پای

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#إذا defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  إذا defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#د_که_پای

/*
 * shutdown() flags for systems that do not define them
 */
#که_نه_تعریف_شوی SHUT_RD
#تعریف_کړه SHUT_RD 0x00
#د_که_پای

#که_نه_تعریف_شوی SHUT_WR
#تعریف_کړه SHUT_WR 0x01
#د_که_پای

#که_نه_تعریف_شوی SHUT_RDWR
#تعریف_کړه SHUT_RDWR 0x02
#د_که_پای

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#إذا !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#تعریف_کړه S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#د_که_پای

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#إذا !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#تعریف_کړه S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#د_که_پای

/* For MSVC (all versions as of VS2022) */
#که_نه_تعریف_شوی STDIN_FILENO
#تعریف_کړه STDIN_FILENO  fileno(stdin)
#د_که_پای
#که_نه_تعریف_شوی STDOUT_FILENO
#تعریف_کړه STDOUT_FILENO  fileno(stdout)
#د_که_پای
#که_نه_تعریف_شوی STDERR_FILENO
#تعریف_کړه STDERR_FILENO  fileno(stderr)
#د_که_پای

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#که_تعریف_شوی O_دوګونی  /* for _WIN32 || MSDOS */
#تعریف_کړه CURL_O_دوګونی O_دوګونی
#وإلاإذا defined(O_دوګونی_2)  /* __CYGWIN__ */
#تعریف_کړه CURL_O_دوګونی O_دوګونی_2
#وإلا
#تعریف_کړه CURL_O_دوګونی 0
#د_که_پای

/* Requires io.h when available */
#که_تعریف_شوی MSDOS
#تعریف_کړه CURL_BINMODE(بهیر) (فراغ)setmode(fileno(بهیر), CURL_O_دوګونی)
#وإلاإذا defined(_WIN32) || defined(__CYGWIN__)
#تعریف_کړه CURL_BINMODE(بهیر) (فراغ)_setmode(fileno(بهیر), CURL_O_دوګونی)
#وإلا
#تعریف_کړه CURL_BINMODE(بهیر) (فراغ)(بهیر)
#د_که_پای

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#إذا defined(_WIN32) || defined(MSDOS)
#تعریف_کړه FOPEN_READTEXT   "rt"
#تعریف_کړه FOPEN_WRITETEXT  "wt"
#تعریف_کړه FOPEN_APPENDTEXT "at"
#وإلاإذا defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#تعریف_کړه FOPEN_READTEXT   "rt"
#تعریف_کړه FOPEN_WRITETEXT  "w"
#تعریف_کړه FOPEN_APPENDTEXT "a"
#وإلا
#تعریف_کړه FOPEN_READTEXT   "r"
#تعریف_کړه FOPEN_WRITETEXT  "w"
#تعریف_کړه FOPEN_APPENDTEXT "a"
#د_که_پای

/* for systems that do not detect this in configure */
#که_نه_تعریف_شوی CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T پته_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T بینښه حرف
#  وإلا
/* use a sensible default */
#    define CURL_SA_FAMILY_T بینښه لنډ
#  endif
#د_که_پای

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#تعریف_کړه CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#تعریف_کړه CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#تعریف_کړه CURL_CSTRLEN(x) (حجم(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#تعریف_کړه STRCONST(x) x, CURL_CSTRLEN(x)

#تعریف_کړه CURL_ARRAYSIZE(A) (حجم(A) / حجم((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#تعریف_کړه STRERROR_LEN 256

#که_نه_تعریف_شوی CURL_DID_حافظه_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
تعريفنوع فراغ *(*curl_malloc_callback)(size_t اندازه);
تعريفنوع فراغ (*curl_free_callback)(فراغ *ptr);
تعريفنوع فراغ *(*curl_realloc_callback)(فراغ *ptr, size_t اندازه);
تعريفنوع حرف *(*curl_strdup_callback)(ثابت حرف *str);
تعريفنوع فراغ *(*curl_calloc_callback)(size_t nmemb, size_t اندازه);
#تعریف_کړه CURL_DID_حافظه_FUNC_TYPEDEFS
#د_که_پای

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
#تعریف_کړه curlx_safefree(ptr) \
  افعل {                      \
    curlx_free(ptr);        \
    (ptr) = عدم;           \
  } طالما(0)

#ضمن <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه CURL_MEMDEBUG
#د_که_پای

#که_تعریف_شوی CURL_MEMDEBUG
#که_تعریف_شوی __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  إذا __clang_major__ >= 4
#  define ALLOC_اندازه(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  وإلا
#  define ALLOC_اندازه(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#وإلاإذا defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_اندازه(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#وإلاإذا defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(محدود)
#  define ALLOC_اندازه(s)
#  define ALLOC_SIZE2(n, s)
#وإلا
#  define ALLOC_FUNC
#  define ALLOC_اندازه(s)
#  define ALLOC_SIZE2(n, s)
#د_که_پای

خارجي FILE *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN فراغ curl_dbg_free(فراغ *ptr, صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_FUNC ALLOC_اندازه(1)
  فراغ *curl_dbg_malloc(size_t wantedsize, صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_اندازه,
                        صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_اندازه(2)
  فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize, صحيح کرښه,
                         ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_FUNC
  حرف *curl_dbg_strdup(ثابت حرف *str, صحيح کرښه, ثابت حرف *سرچینه);
#إذا defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح کرښه, ثابت حرف *سرچینه);
#د_که_پای

CURL_EXTERN فراغ curl_dbg_memdebug(ثابت حرف *logname);
CURL_EXTERN فراغ curl_dbg_memlimit(اوږد حد);
CURL_EXTERN فراغ curl_dbg_log(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                                          صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN فراغ curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN صحيح curl_dbg_sclose(curl_socket_t sockfd,
                                صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          فراغ *saddr, فراغ *saddrlen,
                                          صحيح کرښه, ثابت حرف *سرچینه);
#که_تعریف_شوی HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr,
                                           فراغ *saddrlen, صحيح flags,
                                           صحيح کرښه, ثابت حرف *سرچینه);
#د_که_پای
#که_تعریف_شوی HAVE_SOCKETPAIR
CURL_EXTERN صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                                    curl_socket_t socket_ویکټور[2],
                                    صحيح کرښه, ثابت حرف *سرچینه);
#د_که_پای

/* FILE functions */
CURL_EXTERN صحيح curl_dbg_fclose(FILE *file, صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *ډېرپېښ,
                                            صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_freopen(ثابت حرف *file,
                                              ثابت حرف *ډېرپېښ, FILE *fh,
                                              صحيح کرښه, ثابت حرف *سرچینه);
CURL_EXTERN ALLOC_FUNC FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *ډېرپېښ,
                                             صحيح کرښه, ثابت حرف *سرچینه);

#تعریف_کړه sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __FILE__)
#تعریف_کړه fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __FILE__)

#تعریف_کړه CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __FILE__)
#تعریف_کړه CURL_FREEADDRINFO(معلومات) \
  curl_dbg_freeaddrinfo(معلومات, __LINE__, __FILE__)
#تعریف_کړه CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((صحيح)(domain), type, protocol, __LINE__, __FILE__)
#که_تعریف_شوی HAVE_SOCKETPAIR
#تعریف_کړه CURL_SOCKETPAIR(domain, type, protocol, socket_ویکټور) \
  curl_dbg_socketpair((صحيح)(domain), type, protocol, socket_ویکټور, \
                      __LINE__, __FILE__)
#د_که_پای
#تعریف_کړه CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __FILE__)
#که_تعریف_شوی HAVE_ACCEPT4
#تعریف_کړه CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __FILE__)
#د_که_پای

#وإلا /* !CURL_MEMDEBUG */

#تعریف_کړه sclose(x) CURL_SCLOSE(x)
#تعریف_کړه fake_sclose(x) Curl_nop_stmt

#تعریف_کړه CURL_GETADDRINFO getaddrinfo
#تعریف_کړه CURL_FREEADDRINFO freeaddrinfo
#تعریف_کړه CURL_SOCKET socket
#که_تعریف_شوی HAVE_SOCKETPAIR
#تعریف_کړه CURL_SOCKETPAIR socketpair
#د_که_پای
#تعریف_کړه CURL_ACCEPT accept
#که_تعریف_شوی HAVE_ACCEPT4
#تعریف_کړه CURL_ACCEPT4 accept4
#د_که_پای

#د_که_پای /* CURL_MEMDEBUG */

/* Allocator macros */

#که_تعریف_شوی _WIN32
#تعریف_کړه CURLX_STRDUP_ټیټ _strdup
#وإلا
#تعریف_کړه CURLX_STRDUP_ټیټ strdup
#د_که_پای

#که_تعریف_شوی CURL_MEMDEBUG

#تعریف_کړه curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __FILE__)
#تعریف_کړه curlx_malloc(اندازه)         curl_dbg_malloc(اندازه, __LINE__, __FILE__)
#تعریف_کړه curlx_calloc(nbelem, اندازه) \
  curl_dbg_calloc(nbelem, اندازه, __LINE__, __FILE__)
#تعریف_کړه curlx_realloc(ptr, اندازه) \
  curl_dbg_realloc(ptr, اندازه, __LINE__, __FILE__)
#تعریف_کړه curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __FILE__)

#که_تعریف_شوی _WIN32
#که_تعریف_شوی UNICODE
#تعریف_کړه curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __FILE__)
#وإلا
#تعریف_کړه curlx_tcsdup               curlx_strdup
#د_که_پای
#د_که_پای /* _WIN32 */

#وإلا /* !CURL_MEMDEBUG */

#که_تعریف_شوی BUILDING_LIBCURL
#تعریف_کړه curlx_strdup               Curl_cstrdup
#تعریف_کړه curlx_malloc               Curl_cmalloc
#تعریف_کړه curlx_calloc               Curl_ccalloc
#تعریف_کړه curlx_realloc              Curl_crealloc
#تعریف_کړه curlx_free                 Curl_cfree
#وإلا /* !BUILDING_LIBCURL */
#تعریف_کړه curlx_strdup               CURLX_STRDUP_ټیټ
#تعریف_کړه curlx_malloc               حافظه_ورکړه
#تعریف_کړه curlx_calloc               حافظه_صفر_ورکړه
#تعریف_کړه curlx_realloc              realloc
#تعریف_کړه curlx_free                 حرر
#د_که_پای /* BUILDING_LIBCURL */

#که_تعریف_شوی _WIN32
#که_تعریف_شوی UNICODE
#تعریف_کړه curlx_tcsdup               curlx_wcsdup
#وإلا
#تعریف_کړه curlx_tcsdup               curlx_strdup
#د_که_پای
#د_که_پای /* _WIN32 */

#د_که_پای /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#إذا defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
بنية passwd;
صحيح getpwuid_r(uid_t uid, بنية passwd *pwd, حرف *buf,
               size_t buflen, بنية passwd **پایله);
#د_که_پای

#که_تعریف_شوی UNITTESTS
#تعریف_کړه UNITTEST
#وإلا
#تعریف_کړه UNITTEST ساكن
#د_که_پای

#که_تعریف_شوی USE_NGHTTP2
#تعریف_کړه USE_HTTP2
#د_که_پای

#إذا (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#که_تعریف_شوی CURL_WITH_MULTI_SSL
#تېروتنه "MultiSSL combined with QUIC is not supported"
#د_که_پای

#تعریف_کړه USE_HTTP3
#د_که_پای

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#که_نه_تعریف_شوی __EMSCRIPTEN__
#تعریف_کړه CURL_TCP_NODELAY_SUPPORTED
#د_که_پای

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#إذا defined(HAVE_BASENAME) && defined(_WIN32)
#تعریف_لغوه HAVE_BASENAME
#د_که_پای

#إذا defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#وإلاإذا !defined(UNIX_لار_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_لار_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  حرف sun_لار[UNIX_لار_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#د_که_پای
#د_که_پای

#که_تعریف_شوی USE_OPENSSL
/* OpenSSL 3 marks these functions deprecated but we have no replacements (yet)
   so tell the compiler to not warn for them:
   - DES_* (for NTLM)
   - EVP_PKEY_get1_RSA, RSA_flags, RSA_free (auto-skipped for OpenSSL built
     with no-deprecated) */
#  define OPENSSL_SUPPRESS_DEPRECATED
#  ifdef _WIN32
/* Silence LibreSSL warnings about wincrypt.h collision. Works in 3.8.2+ */
#    ifndef LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_تعریفوي_WARNING
#    define LIBRESSL_DISABLE_OVERRIDE_WINCRYPT_تعریفوي_WARNING
#    endif
#  endif
#د_که_پای

#که_تعریف_شوی CURL_INLINE
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
#  define CURL_INLINE /*تشه*/
#د_که_پای

/* Detect if compiler supports C99 variadic macros */
#إذا (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#تعریف_کړه CURL_HAVE_MACRO_VARARG
#د_که_پای

#إذا !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_مزي_ګانې))
#تعریف_کړه CURLVERBOSE
#تعریف_کړه VERBOSE(x) x
#تعریف_کړه NOVERBOSE(x) Curl_nop_stmt
#وإلا
#تعریف_کړه VERBOSE(x) Curl_nop_stmt
#تعریف_کړه NOVERBOSE(x) x
#د_که_پای

/* For FreeBSD it is included from curl/curl.h */
#إذا defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#ضمن <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#د_که_پای

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#إذا defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#تعریف_کړه CURL_USE_SOCK_NONBLOCK
#د_که_پای

#که_نه_تعریف_شوی _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#که_تعریف_شوی _WIN32
#إذا defined(_MSC_VER) && defined(NTDDI_نسخه) && \
  (NTDDI_نسخه >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#لارښوونه comment(lib, "volatileaccessu.lib")
#تعریف_کړه curlx_memzero_ټیټ(buf, اندازه)  SecureZeroMemory2(buf, اندازه)
#وإلا
#تعریف_کړه curlx_memzero_ټیټ(buf, اندازه)  Secure_Zero_حافظه(buf, اندازه)
#د_که_پای
#وإلاإذا defined(HAVE_MEMSET_S)
#تعریف_کړه curlx_memzero_ټیټ(buf, اندازه)  (فراغ)memset_s(buf, اندازه, 0, اندازه)
#وإلاإذا defined(HAVE_MEMSET_EXPLICIT)
#تعریف_کړه curlx_memzero_ټیټ(buf, اندازه)  (فراغ)memset_explicit(buf, 0, اندازه)
#وإلاإذا defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#تعریف_کړه curlx_memzero_ټیټ(buf, اندازه)  explicit_bzero(buf, اندازه)
#وإلاإذا defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#تعریف_کړه curlx_memzero_ټیټ(buf, اندازه)  (فراغ)explicit_memset(buf, 0, اندازه)
#د_که_پای
#د_که_پای /* !_CURL_LOCAL_MEMZERO */

#که_نه_تعریف_شوی curlx_memzero_ټیټ
#تعریف_کړه USE_CURLX_MEMZERO
فراغ curlx_memzero_ټیټ(فراغ *buf, size_t اندازه);
#د_که_پای
فراغ curlx_memzero(فراغ *buf, size_t اندازه);
فراغ curlx_strzero(فراغ *buf);

#د_که_پای /* HEADER_CURL_SETUP_H */
