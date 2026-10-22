#अगर_अघोषित HEADER_CURL_SETUP_H
#घोषणा HEADER_CURL_SETUP_H
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

#अगर defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#घोषणा CURL_NO_OLDIES
#पूर्ण_अगर

/* Set default _WIN32_WINNT */
#अगर_घोषित __MINGW32__
#समावेश <_mingw.h>
#पूर्ण_अगर

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
#अगर defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  अगर !defined(__has_feature)  /* Keep this PP check separate from others */
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#पूर्ण_अगर

#अगर_घोषित __APPLE__
#समावेश <sys/types.h>
#समावेश <TargetConditionals.h>
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
#अगर defined(TARGET_OS_MAC) && TARGET_OS_MAC && \
  defined(TARGET_OS_OSX) && !TARGET_OS_OSX && \
  (!defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE) && \
  (!defined(TARGET_OS_SIMULATOR) || !TARGET_OS_SIMULATOR)
#अघोषित TARGET_OS_OSX
#घोषणा TARGET_OS_OSX TARGET_OS_MAC
#पूर्ण_अगर
#पूर्ण_अगर

#अगर defined(__MINGW32__) && \
  (!defined(__MINGW64_VERSION_MAJOR) || (__MINGW64_VERSION_MAJOR < 3))
#त्रुटि "mingw-w64 3.0 or greater required"
#पूर्ण_अगर

/* Visual Studio 2010 is the minimum Visual Studio version we support.
   Workarounds for older versions of Visual Studio have been removed. */
#अगर defined(_MSC_VER) && (_MSC_VER < 1600)
#त्रुटि "Ancient versions of Visual Studio are no longer supported due to bugs."
#पूर्ण_अगर

#अगर_घोषित _MSC_VER
/* Disable Visual Studio warnings: 4127 "conditional expression is constant" */
#आशय warning(disable:4127)
#अगर_अघोषित _CRT_SECURE_NO_WARNINGS
#घोषणा _CRT_SECURE_NO_WARNINGS  /* for getenv(), sscanf(), vsnprintf() */
#पूर्ण_अगर
#पूर्ण_अगर /* _MSC_VER */

#अगर_घोषित _WIN32
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
#  अगर (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
     defined(WINAPI_FAMILY)
#    include <winapifamily.h>
#    अगर WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) &&  \
       !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#      define CURL_WINDOWS_UWP
#    endif
#  endif

/* Mandatory to define SECURITY_WIN32 or SECURITY_KERNEL to indicating who is
   compiling the code. */
#अघोषित SECURITY_KERNEL
#अघोषित SECURITY_WIN32
#घोषणा SECURITY_WIN32  /* for <sspi.h> */
#पूर्ण_अगर

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#अगर_घोषित HAVE_CONFIG_H

#समावेश "curl_config.h"

#अन्यथा /* HAVE_CONFIG_H */

#अगर_घोषित _WIN32
#  include "config-win32.h"
#पूर्ण_अगर

#अगर_घोषित macintosh
#  include "config-mac.h"
#पूर्ण_अगर

#अगर_घोषित __OS400__
#  include "config-os400.h"
#पूर्ण_अगर

#पूर्ण_अगर /* HAVE_CONFIG_H */

#अगर_घोषित _WIN32
#  अगर defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#    error The minimum build target is Windows Vista (0x0600)
#  endif

#  अगर !defined(CURL_WINDOWS_UWP) && (defined(_MSC_VER) || defined(__MINGW32__))
#    ifndef HAVE_IF_NAMETOINDEX
#    define HAVE_IF_NAMETOINDEX
#    endif
#  endif
#पूर्ण_अगर

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,    */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

#अगर_घोषित HAVE_LIBZ
#  ifndef ZLIB_CONST
#  define ZLIB_CONST  /* Use z_const. Supported by v1.2.5.2 or greater. */
#  endif
#पूर्ण_अगर

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */
#अगर_घोषित NEED_THREAD_SAFE
#  ifndef _THREAD_SAFE
#  define _THREAD_SAFE
#  endif
#पूर्ण_अगर

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */
#अगर_घोषित NEED_REENTRANT
#  ifndef _REENTRANT
#  define _REENTRANT
#  endif
#पूर्ण_अगर

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#अगर defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#  define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#पूर्ण_अगर

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be included early, so we do not redefine */
#अगर_घोषित HAVE_THREADS_POSIX
#  include <pthread.h>
#पूर्ण_अगर

/*
 * When HTTP is disabled, disable HTTP-only features
 */
#अगर_घोषित CURL_DISABLE_HTTP
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
#पूर्ण_अगर

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */
#अगर_घोषित __OS400__
#  include "setup-os400.h"
#पूर्ण_अगर

/*
 * VMS setup file includes some system headers.
 */
#अगर_घोषित __VMS
#  include "setup-vms.h"
#पूर्ण_अगर

/*
 * Windows setup file includes some system headers.
 */
#अगर_घोषित _WIN32
#  include "setup-win32.h"
#पूर्ण_अगर

#समावेश <curl/system.h>

#अगर !defined(HAVE_PIPE2) && \
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
#घोषणा HAVE_PIPE2
#पूर्ण_अगर

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#घोषणा CURL_CONC_MACROS_(A, B) A ## B
#घोषणा CURL_CONC_MACROS(A, B) CURL_CONC_MACROS_(A, B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that it matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#अगर_घोषित __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#पूर्ण_अगर

/* based on logic in "curl/mprintf.h" */
#अगर (defined(__GNUC__) || defined(__clang__) ||                 \
     defined(__IAR_SYSTEMS_ICC__)) &&                           \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
  !defined(CURL_NO_FMT_CHECKS)
#अगर defined(__MINGW32__) && !defined(__clang__)
#घोषणा CURL_PRINTF(fmt, arg) __attribute__((format(gnu_printf, fmt, arg)))
#अन्यथा
#घोषणा CURL_PRINTF(fmt, arg) __attribute__((format(__printf__, fmt, arg)))
#पूर्ण_अगर
#अन्यथा
#घोषणा CURL_PRINTF(fmt, arg)
#पूर्ण_अगर

/* Override default printf mask check rules in "curl/mprintf.h" */
#घोषणा CURL_अस्थायी_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#अगर defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#घोषणा __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__ \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#पूर्ण_अगर

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#अगर defined(__APPLE__) && !defined(USE_ARES)
#  define USE_RESOLVE_ON_IPS 1
#  अगर TARGET_OS_MAC && !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#पूर्ण_अगर

#अगर_घोषित USE_ARES
#  ifndef CARES_NO_DEPRECATED
#  define CARES_NO_DEPRECATED  /* for ares_getsock() */
#  endif
#  अगर defined(CURL_STATICLIB) && !defined(CARES_STATICLIB) && defined(_WIN32)
#    define CARES_STATICLIB  /* define it before including ares.h */
#  endif
#पूर्ण_अगर

#अगर_घोषित USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#पूर्ण_अगर

#अगर_घोषित macintosh
#  include <extra/stricmp.h>
#  include <extra/strdup.h>
#पूर्ण_अगर

#अगर_घोषित __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    अगर !(defined(__NEWLIB__) || \
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
#  अगर defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       पूर्णांक Curl_amiga_select(पूर्णांक nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, काष्ठा timeval *timeout);
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
#पूर्ण_अगर

/* Whether to use eventfd() */
#अगर defined(HAVE_EVENTFD) && defined(HAVE_SYS_EVENTFD_H)
#घोषणा USE_EVENTFD
#पूर्ण_अगर

#अगर_घोषित SO_NOSIGPIPE
#घोषणा USE_SO_NOSIGPIPE
#पूर्ण_अगर

#समावेश <stdio.h>
#समावेश <assert.h>

#अगर_घोषित __TANDEM /* for ns*-tandem-nsk systems */
#  ifndef __LP64
#    include <floss.h> /* FLOSS is only used for 32-bit builds. */
#  endif
#पूर्ण_अगर

#समावेश <stdint.h>
#घोषणा HAVE_UINTPTR_T  /* assume uintptr_t is provided by stdint.h */

#अगर_घोषित __DJGPP__
/* By default, DJGPP provides this type as a version of 'unsigned long' which
   forces us to use a define use it in printf() format strings without
   warnings. long and int are both 32 bits for this platform. */
#घोषणा uint32_t अचिन्हित पूर्णांक
#पूर्ण_अगर

/* Disable uintptr_t for targets known to miss it from stdint.h */
#अगर_घोषित __OS400__
#अघोषित HAVE_UINTPTR_T
#पूर्ण_अगर

#समावेश <limits.h>
/* Include after setting system macros that may affect type sizes
   (e.g. 'off_t' or 'time_t'), or suppress warnings
   (e.g. '_CRT_SECURE_NO_WARNINGS`), but before including sys/stat.h */
#समावेश <sys/types.h>

#अगर_घोषित _WIN32
#  ifdef HAVE_IO_H
#  include <io.h>
#  endif
#  include <sys/stat.h>
   /* Large file (>2Gb) support using Win32 functions. */
#  define curl_lseek                      _lseeki64
#  define LSEEK_ERROR                     ((__int64)-1)
#या_अगर defined(__DJGPP__)
   /* Requires DJGPP 2.04 */
#  include <unistd.h>
#  define curl_lseek                      llseek
#  define LSEEK_ERROR                     ((offset_t)-1)
#या_अगर defined(__AMIGA__)
#  define curl_lseek(fd, offset, whence)  lseek(fd, (off_t)(offset), whence)
#  define LSEEK_ERROR                     ((off_t)-1)
#अन्यथा
#  define curl_lseek                      lseek
#  define LSEEK_ERROR                     ((off_t)-1)
#पूर्ण_अगर

#अगर_अघोषित SIZEOF_TIME_T
/* assume default size of time_t to be 32 bits */
#घोषणा SIZEOF_TIME_T 4
#पूर्ण_अगर

#अगर_अघोषित SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  अगर defined(USE_WINSOCK) && defined(_WIN64)
#    define SIZEOF_CURL_SOCKET_T 8
#  अन्यथा
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#पूर्ण_अगर

#अगर SIZEOF_CURL_SOCKET_T < 8
#अगर_घोषित USE_WINSOCK
#  define FMT_SOCKET_T "u"
#अन्यथा
#  define FMT_SOCKET_T "d"
#पूर्ण_अगर
#या_अगर defined(USE_WINSOCK)
#  define FMT_SOCKET_T "zu"
#अन्यथा
#  define FMT_SOCKET_T "qd"
#पूर्ण_अगर

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#अगर_अघोषित SIZEOF_OFF_T
#  अगर defined(__VMS) && !defined(__VAX)
#    ifdef _LARGEFILE
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    ifdef _LARGE_FILES
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    अगर defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    अगर defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#  define SIZEOF_OFF_T 4
#  endif
#पूर्ण_अगर

#अगर SIZEOF_CURL_OFF_T < 8
#त्रुटि "too small curl_off_t"
#अन्यथा
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFF
#पूर्ण_अगर
#घोषणा CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1)

#घोषणा FMT_OFF_T  CURL_FORMAT_CURL_OFF_T
#घोषणा FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#अगर SIZEOF_TIME_T == 4
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX अच_पूर्णांक_उच्च
#  define TIME_T_MIN 0
#  अन्यथा
#  define TIME_T_MAX पूर्णांक_उच्च
#  define TIME_T_MIN पूर्णांक_न्यून
#  endif
#अन्यथा
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX 0xFFFFFFFFFFFFFFFF
#  define TIME_T_MIN 0
#  अन्यथा
#  define TIME_T_MAX 0x7FFFFFFFFFFFFFFF
#  define TIME_T_MIN (-TIME_T_MAX - 1)
#  endif
#पूर्ण_अगर

#अगर_अघोषित SIZE_MAX
/* some limits.h headers have this defined, some do not */
#अगर defined(SIZEOF_आकार_T) && (SIZEOF_आकार_T > 4)
#घोषणा SIZE_MAX 18446744073709551615U
#अन्यथा
#घोषणा SIZE_MAX 4294967295U
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_अघोषित SSIZE_MAX
/* some limits.h headers have this defined, some do not */
#अगर defined(SIZEOF_आकार_T) && (SIZEOF_आकार_T > 4)
#घोषणा SSIZE_MAX 9223372036854775807
#अन्यथा
#घोषणा SSIZE_MAX 2147483647
#पूर्ण_अगर
#पूर्ण_अगर

#अगर SIZEOF_LONG > SIZEOF_आकार_T
#त्रुटि "unexpected: 'long' is larger than 'size_t'"
#पूर्ण_अगर

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */
#अगर_अघोषित GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 पूर्णांक
#  अन्यथा
#    define GETHOSTNAME_TYPE_ARG2 माप_प्रकार
#  endif
#पूर्ण_अगर

/* Below we define some functions. They should
   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
 */

#अगर_घोषित _WIN32

#  define DIR_CHAR      "\\"

#अन्यथा /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n, r, w, x, t)  select_s(n, r, w, x, t)
#    define ioctl(x, y, z)         ioctlsocket(x, y, (अक्षर *)(z))
#    include <tcp.h>
#    undef word
#    undef byte

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     बाह्य काष्ठा पंचांग *gmtime_r(स्थिर समय_प्रकार * स्थिर timep, काष्ठा पंचांग *tmp);
#  endif

#  define DIR_CHAR      "/"

#पूर्ण_अगर /* _WIN32 */

/* We want to use mutex when available. */
#अगर defined(HAVE_THREADS_POSIX) || defined(_WIN32)
#घोषणा USE_MUTEX
#पूर्ण_अगर

/* threaded resolver is the only feature requiring threads. */
#अगर_घोषित USE_RESOLV_THREADED
#घोषणा USE_THREADS
#पूर्ण_अगर

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * Mutually exclusive CURLRES_* definitions.
 */
#अगर defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#या_अगर defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#अन्यथा
#  define CURLRES_IPV4
#पूर्ण_अगर

#अगर_घोषित USE_RESOLV_THREADED
#  define CURLRES_ASYNCH
#या_अगर defined(USE_RESOLV_ARES)
#  define CURLRES_ASYNCH
/* now undef the stock libc functions to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#अन्यथा
#  define CURLRES_SYNCH
#पूर्ण_अगर

/* ---------------------------------------------------------------- */

#अगर defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#घोषणा USE_LIBIDN2
#पूर्ण_अगर

#अगर defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#त्रुटि "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#पूर्ण_अगर

#अगर defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_RUSTLS)
#घोषणा USE_SSL    /* SSL support has been enabled */
#पूर्ण_अगर

#अगर defined(USE_OPENSSL) && defined(USE_WOLFSSL)
#अगर_अघोषित OPENSSL_COEXIST
#घोषणा OPENSSL_COEXIST
#पूर्ण_अगर
#पूर्ण_अगर

#अगर defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#घोषणा NO_OLD_WC_NAMES
#पूर्ण_अगर

/* Single point where USE_SPNEGO definition might be defined */
#अगर !defined(CURL_DISABLE_NEGOTIATE_AUTH) &&          \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#घोषणा USE_SPNEGO
#पूर्ण_अगर

/* Single point where USE_KERBEROS5 definition might be defined */
#अगर !defined(CURL_DISABLE_KERBEROS_AUTH) &&           \
  (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#घोषणा USE_KERBEROS5
#पूर्ण_अगर

/* Single point where USE_NTLM definition might be defined */
#अगर_घोषित CURL_ENABLE_NTLM
#  अगर (defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)) ||   \
  defined(USE_GNUTLS) ||                                           \
  (defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)) || \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||         \
  (defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  अगर defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#पूर्ण_अगर

#अगर defined(USE_LIBSSH2) || defined(USE_LIBSSH)
#घोषणा USE_SSH
#पूर्ण_अगर

/* GCC <4.6 does not support '#pragma GCC diagnostic push' and does not support
   'pragma GCC diagnostic' inside functions.
   Use CURL_HAVE_DIAG to guard the above in the curl codebase, instead of
   defined(__GNUC__) || defined(__clang__).
 */
#अगर defined(__clang__) || (defined(__GNUC__) && \
  ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))))
#घोषणा CURL_HAVE_DIAG
#पूर्ण_अगर

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */
#अगर defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define WARN_UNUSED_परिणाम __attribute__((warn_unused_परिणाम))
#या_अगर defined(__IAR_SYSTEMS_ICC__) && (__VER__ >= 9040001)
#  define WARN_UNUSED_परिणाम __attribute__((warn_unused_परिणाम))
#अन्यथा
#  define WARN_UNUSED_परिणाम
#पूर्ण_अगर

/* noreturn attribute */

#अगर_अघोषित CURL_NORETURN
#अगर (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#या_अगर defined(_MSC_VER)
#  define CURL_NORETURN  __declspec(noreturn)
#अन्यथा
#  define CURL_NORETURN
#पूर्ण_अगर
#पूर्ण_अगर

/* fallthrough attribute */

#अगर_अघोषित FALLTHROUGH
#अगर (defined(__GNUC__) && __GNUC__ >= 7) ||     \
  (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#अन्यथा
#  define FALLTHROUGH()  करो {} जबतक(0)
#पूर्ण_अगर
#पूर्ण_अगर

/*
 * Inclusion of common header files.
 */

#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <stdarg.h>
#समावेश <time.h>
#समावेश <errno.h>

#समावेश <sys/stat.h>

#अगर !defined(_WIN32) || defined(__MINGW32__)
#समावेश <sys/time.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_IO_H
#समावेश <io.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_FCNTL_H
#समावेश <fcntl.h>
#पूर्ण_अगर

#अगर defined(HAVE_STDBOOL_H) && defined(HAVE_BOOL_T)
#समावेश <stdbool.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_UNISTD_H
#समावेश <unistd.h>
#पूर्ण_अगर

/* Macro to strip 'const' without triggering a compiler warning.
   Use it for APIs that do not or cannot support the const qualifier. */
#अगर_घोषित HAVE_UINTPTR_T
#घोषणा CURL_UNCONST(p) ((व्योम *)(uintptr_t)(स्थिर व्योम *)(p))
#अन्यथा
#घोषणा CURL_UNCONST(p) ((व्योम *)(p))  /* Fall back to simple cast */
#पूर्ण_अगर

#अगर_घोषित USE_SCHANNEL
/* Must set this before <schannel.h> is included directly or indirectly by
   another Windows header. */
#  define SCHANNEL_USE_BLACKLISTS  /* for SCH_CREDENTIALS */
#  include <subauth.h>  /* for [P]UNICODE_STRING in SCH_CREDENTIALS */
#पूर्ण_अगर

#अगर_घोषित __hpux
#  अगर !defined(_XOPEN_SOURCE_EXTENDED) || defined(_KERNEL)
#    ifdef _APP32_64BIT_OFF_T
#      define OLD_APP32_64BIT_OFF_T _APP32_64BIT_OFF_T
#      undef _APP32_64BIT_OFF_T
#    अन्यथा
#      undef OLD_APP32_64BIT_OFF_T
#    endif
#  endif
#पूर्ण_अगर

#अगर_अघोषित _WIN32
#समावेश <sys/socket.h>  /* also for MSG_NOSIGNAL */
#पूर्ण_अगर

#समावेश "functypes.h"

#अगर_घोषित __hpux
#  अगर !defined(_XOPEN_SOURCE_EXTENDED) || defined(_KERNEL)
#    ifdef OLD_APP32_64BIT_OFF_T
#      define _APP32_64BIT_OFF_T OLD_APP32_64BIT_OFF_T
#      undef OLD_APP32_64BIT_OFF_T
#    endif
#  endif
#पूर्ण_अगर

/*
 * Definition of timeval struct for platforms that do not have it.
 */
#अगर_अघोषित HAVE_STRUCT_TIMEVAL
काष्ठा timeval {
  दीर्घ tv_sec;
  दीर्घ tv_usec;
};
#पूर्ण_अगर

/*
 * If we have the MSG_NOSIGNAL define, make sure we use
 * it as the fourth argument of function send()
 */
#अगर_घोषित MSG_NOSIGNAL
#घोषणा SEND_4TH_ARG MSG_NOSIGNAL
#अन्यथा
#घोषणा SEND_4TH_ARG 0
#पूर्ण_अगर

#अगर_घोषित __minix
/* Minix does not support recv on TCP sockets */
#घोषणा sread(x, y, z) (ssize_t)पढ़ो((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z))

#या_अगर defined(HAVE_RECV)
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

#घोषणा sread(x, y, z) (ssize_t)recv((RECV_TYPE_ARG1)(x), \
                                     (RECV_TYPE_ARG2)(y), \
                                     (RECV_TYPE_ARG3)(z), \
                                     (RECV_TYPE_ARG4)(0))
#अन्यथा /* HAVE_RECV */
#अगर_अघोषित sread
#त्रुटि "Missing definition of macro sread"
#पूर्ण_अगर
#पूर्ण_अगर /* HAVE_RECV */

#अगर_घोषित __minix
/* Minix does not support send on TCP sockets */
#घोषणा swrite(x, y, z) (ssize_t)ग_लिखो((SEND_TYPE_ARG1)(x), \
                                       (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                       (SEND_TYPE_ARG3)(z))
#या_अगर defined(HAVE_SEND)
#अगर_घोषित SEND_NONCONST_ARG2
#घोषणा swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (SEND_TYPE_ARG2)CURL_UNCONST(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#अन्यथा
#घोषणा swrite(x, y, z) (ssize_t)send((SEND_TYPE_ARG1)(x), \
                                      (स्थिर SEND_TYPE_ARG2)(y), \
                                      (SEND_TYPE_ARG3)(z), \
                                      (SEND_TYPE_ARG4)(SEND_4TH_ARG))
#पूर्ण_अगर /* SEND_NONCONST_ARG2 */
#अन्यथा /* HAVE_SEND */
#अगर_अघोषित swrite
#त्रुटि "Missing definition of macro swrite"
#पूर्ण_अगर
#पूर्ण_अगर /* HAVE_SEND */

/*
 * Function-like macro definition used to close a socket.
 */
#अगर_घोषित HAVE_CLOSESOCKET
#  define CURL_SCLOSE(x)  closesocket(x)
#या_अगर defined(HAVE_CLOSESOCKET_CAMEL)
#  define CURL_SCLOSE(x)  CloseSocket(x)
#या_अगर defined(MSDOS)  /* Watt-32 */
#  define CURL_SCLOSE(x)  close_s(x)
#या_अगर defined(USE_LWIPSOCK)
#  define CURL_SCLOSE(x)  lwip_close(x)
#अन्यथा
#  define CURL_SCLOSE(x)  बंद(x)
#पूर्ण_अगर

/*
 * Stack-independent version of fcntl() on sockets:
 */
#अगर_घोषित USE_LWIPSOCK
#  define sfcntl  lwip_fcntl
#अन्यथा
#  define sfcntl  fcntl
#पूर्ण_अगर

/*
 * 'bool' stuff compatible with HP-UX headers.
 */
#अगर defined(__hpux) && !defined(HAVE_BOOL_T)
प्रकार पूर्णांक bool;
#  define false 0
#  define true 1
#  define HAVE_BOOL_T
#पूर्ण_अगर

/*
 * 'bool' exists on platforms with <stdbool.h>, i.e. C99 platforms.
 * On non-C99 platforms there is no bool, so define an enum for that.
 * On C99 platforms 'false' and 'true' also exist. Enum uses a
 * global namespace though, so use bool_false and bool_true.
 */
#अगर_अघोषित HAVE_BOOL_T
/* This replacement type does not preserve C99 bool's normalization semantics:
   converting a non-zero value to C99 bool results in 1. This is significant
   when the value is assigned to a bit-field. Make sure to normalize manually
   in such cases. */
प्रकार क्रमागत {
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
#पूर्ण_अगर

/* the type we use for storing a single boolean bit */
प्रकार अचिन्हित पूर्णांक curl_bit;
#घोषणा BIT(x) curl_bit x:1

/*
 * Redefine TRUE and FALSE too, to catch current use. With this
 * change, 'bool found = 1' will give a warning on MIPSPro, but
 * 'bool found = TRUE' will not. Change tested on IRIX/MIPSPro,
 * AIX 5.1/Xlc, Tru64 5.1/cc, w/make test too.
 */
#अगर_अघोषित TRUE
#घोषणा TRUE true
#पूर्ण_अगर
#अगर_अघोषित FALSE
#घोषणा FALSE false
#पूर्ण_अगर

#समावेश "curl_ctype.h"

#अगर defined(DEBUGBUILD) && defined(न_संशोधन)
#त्रुटि "Debug-enabled builds cannot be combined with NDEBUG"
#पूर्ण_अगर

/*
 * Macro used to include code only in debug builds.
 */
#अगर_घोषित DEBUGBUILD
#घोषणा DEBUGF(x) x
#अन्यथा
#घोषणा DEBUGF(x) करो {} जबतक(0)
#पूर्ण_अगर

/*
 * Macro used to include assertion code only in debug builds.
 */
#अघोषित DEBUGASSERT
#अगर_घोषित DEBUGBUILD
#अगर_घोषित CURL_DEBUGASSERT
/* External assertion handler for custom integrations */
#घोषणा DEBUGASSERT(x) CURL_DEBUGASSERT(x)
#अन्यथा
#घोषणा DEBUGASSERT(x) निश्चित(x)
#पूर्ण_अगर
#अन्यथा
#घोषणा DEBUGASSERT(x) करो {} जबतक(0)
#पूर्ण_अगर

/* Macro to configure Visual Studio debug CRT to output errors to stderr and
   the debugger, replacing the interactive GUI dialog. */
#अगर defined(_MSC_VER) && defined(_DEBUG) && defined(CURL_DEBUG_MSVC_CRTDBG)
#समावेश <crtdbg.h> /* for _CrtSetReportFile(), _CRT* macros */
#घोषणा CURL_MSVC_CRTDBG_INIT()                                             \
  करो {                                                                      \
    _set_error_mode(_OUT_TO_STDERR);  /* needs stdlib.h */                  \
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); \
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);                    \
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);  \
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);                     \
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);   \
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);                      \
  } जबतक(0)
#अन्यथा
#घोषणा CURL_MSVC_CRTDBG_INIT() करो {} जबतक(0)
#पूर्ण_अगर

#अगर defined(_WIN32) && !defined(CURL_WINDOWS_UWP)
#समावेश <tlhelp32.h>
/* Print the list of all loaded modules with full paths. */
#घोषणा CURL_DUMP_MODULE_PATHS(printf_f)                               \
  करो {                                                                 \
    HANDLE hnd = INVALID_HANDLE_मूल्य;                                 \
    MODULEENTRY32 mod = { 0 };                                         \
                                                                       \
    mod.dw_आकार = माप(MODULEENTRY32);                                \
                                                                       \
    करो {                                                               \
      hnd = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);            \
    } जबतक(hnd == INVALID_HANDLE_मूल्य &&                             \
            GetLastError() == ERROR_BAD_लंबाई);                       \
                                                                       \
    अगर(hnd == INVALID_HANDLE_मूल्य)                                    \
      जाओ error;                                                      \
                                                                       \
    अगर(!Module32First(hnd, &mod))                                      \
      जाओ error;                                                      \
                                                                       \
    करो {                                                               \
      अक्षर बफर[माप(mod.szExePath) * 2];                          \
      अक्षर *path; /* points to stack allocated buffer */               \
      अगर(माप(*mod.szExePath) > 1) {  /* _UNICODE */                 \
        /* sizeof(mod.szExePath) is the max total bytes of   */        \
        /* wchars. the max total bytes of multibyte chars is */        \
        /* not more than twice that.                         */        \
        अगर(!WideCharToMultiByte(CP_ACP, 0, (WCHAR *)mod.szExePath, -1, \
                                बफर, माप(बफर), शून्य, शून्य))   \
          जाओ error;                                                  \
        path = (अक्षर *)बफर;                                         \
      }                                                                \
      अन्यथा                                                             \
        path = (अक्षर *)mod.szExePath;                                  \
      (printf_f)("%s\n", path);                                        \
    } जबतक(Module32_अग्र(hnd, &mod));                                  \
                                                                       \
error:                                                                 \
    अगर(hnd != INVALID_HANDLE_मूल्य)                                    \
      CloseHandle(hnd);                                                \
  } जबतक(0)
#अन्यथा
#घोषणा CURL_DUMP_MODULE_PATHS(printf_f) करो {} जबतक(0)
#पूर्ण_अगर

/*
 * Macro SOCKERRNO / SET_SOCKERRNO() returns / sets the *socket-related* errno
 * (or equivalent) on this platform to hide platform details to code using it.
 */
#अगर_घोषित USE_WINSOCK
#घोषणा SOCKERRNO         ((पूर्णांक)WSAGetLastError())
#घोषणा SET_SOCKERRNO(x)  WSASetLastError((पूर्णांक)(x))
#अन्यथा
#घोषणा SOCKERRNO         त्रुटि_सं
#घोषणा SET_SOCKERRNO(x)  (त्रुटि_सं = (x))
#पूर्ण_अगर

/*
 * Portable error number symbolic names defined to Winsock error codes.
 */
#अगर_घोषित USE_WINSOCK
#घोषणा SOCKEACCES        WSAEACCES
#घोषणा SOCKEADDRINUSE    WSAEADDRINUSE
#घोषणा SOCKEADDRNOTAVAIL WSAEADDRNOTAVAIL
#घोषणा SOCKEAFNOSUPPORT  WSAEAFNOSUPPORT
#घोषणा SOCKEBADF         WSAEBADF
#घोषणा SOCKECONNREFUSED  WSAECONNREFUSED
#घोषणा SOCKECONNRESET    WSAECONNRESET
#घोषणा SOCKEINPROGRESS   WSAEINPROGRESS
#घोषणा SOCKEINTR         WSAEINTR
#घोषणा SOCKEINVAL        WSAEINVAL
#घोषणा SOCKEISCONN       WSAEISCONN
#घोषणा SOCKEMSGSIZE      WSAEMSGSIZE
/* Use literal value to work around clang-tidy <=20 misreporting
   'readability-uppercase-literal-suffix' with mingw-w64 headers */
#घोषणा SOCKENOMEM        8L  /* WSA_NOT_ENOUGH_MEMORY */
#घोषणा SOCKETIMEDOUT     WSAETIMEDOUT
#घोषणा SOCKEWOULDBLOCK   WSAEWOULDBLOCK
#अन्यथा
#घोषणा SOCKEACCES        EACCES
#घोषणा SOCKEADDRINUSE    EADDRINUSE
#घोषणा SOCKEADDRNOTAVAIL EADDRNOTAVAIL
#घोषणा SOCKEAFNOSUPPORT  EAFNOSUPPORT
#घोषणा SOCKEBADF         EBADF
#घोषणा SOCKECONNREFUSED  ECONNREFUSED
#घोषणा SOCKECONNRESET    ECONNRESET
#घोषणा SOCKEINPROGRESS   EINPROGRESS
#घोषणा SOCKEINTR         EINTR
#घोषणा SOCKEINVAL        EINVAL
#घोषणा SOCKEISCONN       EISCONN
#घोषणा SOCKEMSGSIZE      EMSGSIZE
#घोषणा SOCKENOMEM        ENOMEM
#अगर_घोषित ETIMEDOUT
#घोषणा SOCKETIMEDOUT     ETIMEDOUT
#पूर्ण_अगर
#घोषणा SOCKEWOULDBLOCK   EWOULDBLOCK
#पूर्ण_अगर

/* The socket error may be EWOULDBLOCK or on some systems EAGAIN when
   it returned due to its inability to send/read data without blocking.
   We treat both error codes the same here. */
#अगर !defined(USE_WINSOCK) && EAGAIN != SOCKEWOULDBLOCK
#घोषणा SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK || (e) == EAGAIN)
#अन्यथा
#घोषणा SOCK_EAGAIN(e) ((e) == SOCKEWOULDBLOCK)
#पूर्ण_अगर

/*
 * Macro argv_item_t hides platform details to code using it.
 */
#अगर_घोषित __VMS
#घोषणा argv_item_t  __char_ptr32
#या_अगर defined(_UNICODE)
#घोषणा argv_item_t  ब_अक्षर_प्रकार *
#अन्यथा
#घोषणा argv_item_t  अक्षर *
#पूर्ण_अगर

/*
 * We use this ZERO_NULL to avoid picky compiler warnings,
 * when assigning a NULL pointer to a function pointer var.
 */
#घोषणा ZERO_NULL 0

/*
 * Macros and functions to safely suppress warnings
 */
#समावेश "curlx/warnless.h"

#अगर_घोषित _WIN32
#  undef पढ़ो
#  define पढ़ो(fd, buf, गिनती)  (ssize_t)_read(fd, buf, curlx_uztoui(गिनती))
#  undef ग_लिखो
#  define ग_लिखो(fd, buf, गिनती) (ssize_t)_write(fd, buf, curlx_uztoui(गिनती))
/* Avoid VS2005+ _CRT_NONSTDC_NO_DEPRECATE warnings about non-portable funcs */
#  undef fileno
#  define fileno(fh) _fileno(fh)
#  undef unlink
#  define unlink(fn) _unlink(fn)
#  undef isatty
#  define isatty(fd) _isatty(fd)
#पूर्ण_अगर

/*
 * Definition of our NOP statement Object-like macro
 */
#अगर_अघोषित Curl_nop_stmt
#घोषणा Curl_nop_stmt करो {} जबतक(0)
#पूर्ण_अगर

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */
#अगर defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  अगर defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist"
#  endif
#पूर्ण_अगर

/*
 * shutdown() flags for systems that do not define them
 */
#अगर_अघोषित SHUT_RD
#घोषणा SHUT_RD 0x00
#पूर्ण_अगर

#अगर_अघोषित SHUT_WR
#घोषणा SHUT_WR 0x01
#पूर्ण_अगर

#अगर_अघोषित SHUT_RDWR
#घोषणा SHUT_RDWR 0x02
#पूर्ण_अगर

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#अगर !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#घोषणा S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#पूर्ण_अगर

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#अगर !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#घोषणा S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#पूर्ण_अगर

/* For MSVC (all versions as of VS2022) */
#अगर_अघोषित STDIN_FILENO
#घोषणा STDIN_FILENO  fileno(मानक_निवेश)
#पूर्ण_अगर
#अगर_अघोषित STDOUT_FILENO
#घोषणा STDOUT_FILENO  fileno(मानक_निकास)
#पूर्ण_अगर
#अगर_अघोषित STDERR_FILENO
#घोषणा STDERR_FILENO  fileno(मानक_त्रुटि)
#पूर्ण_अगर

/* Since O_BINARY is used in bitmasks, setting it to zero makes it usable in
   source code but yet it does not ruin anything */
#अगर_घोषित O_द्विआधारी  /* for _WIN32 || MSDOS */
#घोषणा CURL_O_द्विआधारी O_द्विआधारी
#या_अगर defined(O_द्विआधारी_2)  /* __CYGWIN__ */
#घोषणा CURL_O_द्विआधारी O_द्विआधारी_2
#अन्यथा
#घोषणा CURL_O_द्विआधारी 0
#पूर्ण_अगर

/* Requires io.h when available */
#अगर_घोषित MSDOS
#घोषणा CURL_BINMODE(stream) (व्योम)setmode(fileno(stream), CURL_O_द्विआधारी)
#या_अगर defined(_WIN32) || defined(__CYGWIN__)
#घोषणा CURL_BINMODE(stream) (व्योम)_setmode(fileno(stream), CURL_O_द्विआधारी)
#अन्यथा
#घोषणा CURL_BINMODE(stream) (व्योम)(stream)
#पूर्ण_अगर

/* In Windows the default file mode is text but an application can override it.
   Therefore we specify it explicitly. https://github.com/curl/curl/pull/258 */
#अगर defined(_WIN32) || defined(MSDOS)
#घोषणा FOPEN_READTEXT   "rt"
#घोषणा FOPEN_WRITETEXT  "wt"
#घोषणा FOPEN_APPENDTEXT "at"
#या_अगर defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
   https://cygwin.com/cygwin-ug-net/using-textbinary.html
   For write we want our output to have line endings of LF and be compatible
   with other Cygwin utilities. For read we want to handle input that may have
   line endings either CRLF or LF so 't' is appropriate. */
#घोषणा FOPEN_READTEXT   "rt"
#घोषणा FOPEN_WRITETEXT  "w"
#घोषणा FOPEN_APPENDTEXT "a"
#अन्यथा
#घोषणा FOPEN_READTEXT   "r"
#घोषणा FOPEN_WRITETEXT  "w"
#घोषणा FOPEN_APPENDTEXT "a"
#पूर्ण_अगर

/* for systems that do not detect this in configure */
#अगर_अघोषित CURL_SA_FAMILY_T
#  ifdef USE_WINSOCK
#    define CURL_SA_FAMILY_T ADDRESS_FAMILY
#  elif defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(__AMIGA__)
#    define CURL_SA_FAMILY_T अचिन्हित अक्षर
#  अन्यथा
/* use a sensible default */
#    define CURL_SA_FAMILY_T अचिन्हित लघु
#  endif
#पूर्ण_अगर

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#घोषणा CURLMAX(x, y) ((x) > (y) ? (x) : (y))
#घोषणा CURLMIN(x, y) ((x) < (y) ? (x) : (y))

/* Convenience macro to provide the length of a string literal size without
   the null-terminator. Equivalent to strlen() for constant strings. */
#घोषणा CURL_CSTRLEN(x) (माप(x) - 1)

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#घोषणा STRCONST(x) x, CURL_CSTRLEN(x)

#घोषणा CURL_ARRAYSIZE(A) (माप(A) / माप((A)[0]))

/* Buffer size for error messages retrieved via
   curlx_strerror() and Curl_sspi_strerror() */
#घोषणा STRERROR_LEN 256

#अगर_अघोषित CURL_DID_MEMORY_FUNC_TYPEDEFS /* only if not already done */
/*
 * The following memory function replacement typedef's are COPIED from
 * curl/curl.h and MUST match the originals. We copy them to avoid having to
 * include curl/curl.h here. We avoid that include since it includes stdio.h
 * and other headers that may get messed up with defines done here.
 */
प्रकार व्योम *(*curl_malloc_callback)(माप_प्रकार आकार);
प्रकार व्योम (*curl_free_callback)(व्योम *ptr);
प्रकार व्योम *(*curl_realloc_callback)(व्योम *ptr, माप_प्रकार आकार);
प्रकार अक्षर *(*curl_strdup_callback)(स्थिर अक्षर *str);
प्रकार व्योम *(*curl_calloc_callback)(माप_प्रकार nmemb, माप_प्रकार आकार);
#घोषणा CURL_DID_MEMORY_FUNC_TYPEDEFS
#पूर्ण_अगर

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
#घोषणा curlx_safefree(ptr) \
  करो {                      \
    curlx_free(ptr);        \
    (ptr) = शून्य;           \
  } जबतक(0)

#समावेश <curl/curl.h> /* for CURL_EXTERN, curl_socket_t, mprintf.h */

#अगर_घोषित DEBUGBUILD
#घोषणा CURL_MEMDEBUG
#पूर्ण_अगर

#अगर_घोषित CURL_MEMDEBUG
#अगर_घोषित __clang__
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  अगर __clang_major__ >= 4
#  define ALLOC_आकार(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#  अन्यथा
#  define ALLOC_आकार(s)
#  define ALLOC_SIZE2(n, s)
#  endif
#या_अगर defined(__GNUC__) && __GNUC__ >= 3
#  define ALLOC_FUNC         __attribute__((__malloc__))
#  define ALLOC_आकार(s)      __attribute__((__alloc_size__(s)))
#  define ALLOC_SIZE2(n, s)  __attribute__((__alloc_size__(n, s)))
#या_अगर defined(_MSC_VER)
#  define ALLOC_FUNC         __declspec(restrict)
#  define ALLOC_आकार(s)
#  define ALLOC_SIZE2(n, s)
#अन्यथा
#  define ALLOC_FUNC
#  define ALLOC_आकार(s)
#  define ALLOC_SIZE2(n, s)
#पूर्ण_अगर

बाह्य खाता *curl_dbg_logfile;

/* memory functions */
CURL_EXTERN व्योम curl_dbg_free(व्योम *ptr, पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN ALLOC_FUNC ALLOC_आकार(1)
  व्योम *curl_dbg_malloc(माप_प्रकार wantedsize, पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN ALLOC_FUNC ALLOC_SIZE2(1, 2)
  व्योम *curl_dbg_calloc(माप_प्रकार wanted_elements, माप_प्रकार wanted_आकार,
                        पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN ALLOC_आकार(2)
  व्योम *curl_dbg_realloc(व्योम *ptr, माप_प्रकार wantedsize, पूर्णांक पंक्ति,
                         स्थिर अक्षर *source);
CURL_EXTERN ALLOC_FUNC
  अक्षर *curl_dbg_strdup(स्थिर अक्षर *str, पूर्णांक पंक्ति, स्थिर अक्षर *source);
#अगर defined(_WIN32) && defined(UNICODE)
CURL_EXTERN ALLOC_FUNC
  ब_अक्षर_प्रकार *curl_dbg_wcsdup(स्थिर ब_अक्षर_प्रकार *str, पूर्णांक पंक्ति, स्थिर अक्षर *source);
#पूर्ण_अगर

CURL_EXTERN व्योम curl_dbg_memdebug(स्थिर अक्षर *logname);
CURL_EXTERN व्योम curl_dbg_memlimit(दीर्घ limit);
CURL_EXTERN व्योम curl_dbg_log(स्थिर अक्षर *format, ...) CURL_PRINTF(1, 2);

/* file descriptor manipulators */
CURL_EXTERN curl_socket_t curl_dbg_socket(पूर्णांक domain, पूर्णांक type, पूर्णांक protocol,
                                          पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN व्योम curl_dbg_mark_sclose(curl_socket_t sockfd,
                                      पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN पूर्णांक curl_dbg_sclose(curl_socket_t sockfd,
                                पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN curl_socket_t curl_dbg_accept(curl_socket_t s,
                                          व्योम *saddr, व्योम *saddrlen,
                                          पूर्णांक पंक्ति, स्थिर अक्षर *source);
#अगर_घोषित HAVE_ACCEPT4
CURL_EXTERN curl_socket_t curl_dbg_accept4(curl_socket_t s, व्योम *saddr,
                                           व्योम *saddrlen, पूर्णांक flags,
                                           पूर्णांक पंक्ति, स्थिर अक्षर *source);
#पूर्ण_अगर
#अगर_घोषित HAVE_SOCKETPAIR
CURL_EXTERN पूर्णांक curl_dbg_socketpair(पूर्णांक domain, पूर्णांक type, पूर्णांक protocol,
                                    curl_socket_t socket_vector[2],
                                    पूर्णांक पंक्ति, स्थिर अक्षर *source);
#पूर्ण_अगर

/* FILE functions */
CURL_EXTERN पूर्णांक curl_dbg_fclose(खाता *file, पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN ALLOC_FUNC खाता *curl_dbg_fopen(स्थिर अक्षर *file, स्थिर अक्षर *mode,
                                            पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN ALLOC_FUNC खाता *curl_dbg_freopen(स्थिर अक्षर *file,
                                              स्थिर अक्षर *mode, खाता *fh,
                                              पूर्णांक पंक्ति, स्थिर अक्षर *source);
CURL_EXTERN ALLOC_FUNC खाता *curl_dbg_fdopen(पूर्णांक filedes, स्थिर अक्षर *mode,
                                             पूर्णांक पंक्ति, स्थिर अक्षर *source);

#घोषणा sclose(sockfd) curl_dbg_sclose(sockfd, __LINE__, __खाता__)
#घोषणा fake_sclose(sockfd) curl_dbg_mark_sclose(sockfd, __LINE__, __खाता__)

#घोषणा CURL_GETADDRINFO(host, serv, hint, res) \
  curl_dbg_getaddrinfo(host, serv, hint, res, __LINE__, __खाता__)
#घोषणा CURL_FREEADDRINFO(तथ्य) \
  curl_dbg_freeaddrinfo(तथ्य, __LINE__, __खाता__)
#घोषणा CURL_SOCKET(domain, type, protocol) \
  curl_dbg_socket((पूर्णांक)(domain), type, protocol, __LINE__, __खाता__)
#अगर_घोषित HAVE_SOCKETPAIR
#घोषणा CURL_SOCKETPAIR(domain, type, protocol, socket_vector) \
  curl_dbg_socketpair((पूर्णांक)(domain), type, protocol, socket_vector, \
                      __LINE__, __खाता__)
#पूर्ण_अगर
#घोषणा CURL_ACCEPT(sock, addr, len) \
  curl_dbg_accept(sock, addr, len, __LINE__, __खाता__)
#अगर_घोषित HAVE_ACCEPT4
#घोषणा CURL_ACCEPT4(sock, addr, len, flags) \
  curl_dbg_accept4(sock, addr, len, flags, __LINE__, __खाता__)
#पूर्ण_अगर

#अन्यथा /* !CURL_MEMDEBUG */

#घोषणा sclose(x) CURL_SCLOSE(x)
#घोषणा fake_sclose(x) Curl_nop_stmt

#घोषणा CURL_GETADDRINFO getaddrinfo
#घोषणा CURL_FREEADDRINFO freeaddrinfo
#घोषणा CURL_SOCKET socket
#अगर_घोषित HAVE_SOCKETPAIR
#घोषणा CURL_SOCKETPAIR socketpair
#पूर्ण_अगर
#घोषणा CURL_ACCEPT accept
#अगर_घोषित HAVE_ACCEPT4
#घोषणा CURL_ACCEPT4 accept4
#पूर्ण_अगर

#पूर्ण_अगर /* CURL_MEMDEBUG */

/* Allocator macros */

#अगर_घोषित _WIN32
#घोषणा CURLX_STRDUP_निचला _strdup
#अन्यथा
#घोषणा CURLX_STRDUP_निचला strdup
#पूर्ण_अगर

#अगर_घोषित CURL_MEMDEBUG

#घोषणा curlx_strdup(ptr)          curl_dbg_strdup(ptr, __LINE__, __खाता__)
#घोषणा curlx_malloc(आकार)         curl_dbg_malloc(आकार, __LINE__, __खाता__)
#घोषणा curlx_calloc(nbelem, आकार) \
  curl_dbg_calloc(nbelem, आकार, __LINE__, __खाता__)
#घोषणा curlx_realloc(ptr, आकार) \
  curl_dbg_realloc(ptr, आकार, __LINE__, __खाता__)
#घोषणा curlx_free(ptr)            curl_dbg_free(ptr, __LINE__, __खाता__)

#अगर_घोषित _WIN32
#अगर_घोषित UNICODE
#घोषणा curlx_tcsdup(ptr)          curl_dbg_wcsdup(ptr, __LINE__, __खाता__)
#अन्यथा
#घोषणा curlx_tcsdup               curlx_strdup
#पूर्ण_अगर
#पूर्ण_अगर /* _WIN32 */

#अन्यथा /* !CURL_MEMDEBUG */

#अगर_घोषित BUILDING_LIBCURL
#घोषणा curlx_strdup               Curl_cstrdup
#घोषणा curlx_malloc               Curl_cmalloc
#घोषणा curlx_calloc               Curl_ccalloc
#घोषणा curlx_realloc              Curl_crealloc
#घोषणा curlx_free                 Curl_cfree
#अन्यथा /* !BUILDING_LIBCURL */
#घोषणा curlx_strdup               CURLX_STRDUP_निचला
#घोषणा curlx_malloc               दो_स्मृति
#घोषणा curlx_calloc               सुस्मृति
#घोषणा curlx_realloc              पुनः_स्मृति
#घोषणा curlx_free                 मुक्त
#पूर्ण_अगर /* BUILDING_LIBCURL */

#अगर_घोषित _WIN32
#अगर_घोषित UNICODE
#घोषणा curlx_tcsdup               curlx_wcsdup
#अन्यथा
#घोषणा curlx_tcsdup               curlx_strdup
#पूर्ण_अगर
#पूर्ण_अगर /* _WIN32 */

#पूर्ण_अगर /* CURL_MEMDEBUG */

/* Some versions of the Android NDK is missing the declaration */
#अगर defined(HAVE_GETPWUID_R) && \
  defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
काष्ठा passwd;
पूर्णांक getpwuid_r(uid_t uid, काष्ठा passwd *pwd, अक्षर *buf,
               माप_प्रकार buflen, काष्ठा passwd **परिणाम);
#पूर्ण_अगर

#अगर_घोषित UNITTESTS
#घोषणा UNITTEST
#अन्यथा
#घोषणा UNITTEST अटल
#पूर्ण_अगर

#अगर_घोषित USE_NGHTTP2
#घोषणा USE_HTTP2
#पूर्ण_अगर

#अगर (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || defined(USE_QUICHE)

#अगर_घोषित CURL_WITH_MULTI_SSL
#त्रुटि "MultiSSL combined with QUIC is not supported"
#पूर्ण_अगर

#घोषणा USE_HTTP3
#पूर्ण_अगर

/* WebAssembly builds have TCP_NODELAY, but runtime support is missing. */
#अगर_अघोषित __EMSCRIPTEN__
#घोषणा CURL_TCP_NODELAY_SUPPORTED
#पूर्ण_अगर

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#अगर defined(HAVE_BASENAME) && defined(_WIN32)
#अघोषित HAVE_BASENAME
#पूर्ण_अगर

#अगर defined(USE_UNIX_SOCKETS) && defined(_WIN32)
/* Offered by mingw-w64 v10+, MS SDK 10.0.16299.0/VS2017 15.4+ */
#अगर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR >= 10)
#  include <afunix.h>
#या_अगर !defined(UNIX_PATH_MAX) /* Replicate logic present in afunix.h */
#  define UNIX_PATH_MAX 108
/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा sockaddr_un {
  CURL_SA_FAMILY_T sun_family;
  अक्षर sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित USE_OPENSSL
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
#पूर्ण_अगर

#अगर_घोषित CURL_INLINE
/* 'CURL_INLINE' defined, use as-is */
#या_अगर defined(अंतरभूत)
#  define CURL_INLINE अंतरभूत /* 'inline' defined, assumed correct */
#या_अगर defined(__cplusplus)
/* The code is compiled with C++ compiler.
   C++ always supports 'inline'. */
#  define CURL_INLINE अंतरभूत /* 'inline' keyword supported */
#या_अगर defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* C99 (and later) supports 'inline' keyword */
#  define CURL_INLINE अंतरभूत /* 'inline' keyword supported */
#या_अगर defined(__GNUC__) && __GNUC__ >= 3
/* GCC supports '__inline__' as an extension */
#  define CURL_INLINE __inline__
#या_अगर defined(_MSC_VER)
#  define CURL_INLINE __inline
#अन्यथा
/* Probably 'inline' is not supported by compiler.
   Define to the empty string to be on the safe side. */
#  define CURL_INLINE /* empty */
#पूर्ण_अगर

/* Detect if compiler supports C99 variadic macros */
#अगर (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  defined(_MSC_VER)
#घोषणा CURL_HAVE_MACRO_VARARG
#पूर्ण_अगर

#अगर !defined(CURL_HAVE_MACRO_VARARG) || \
  (defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS))
#घोषणा CURLVERBOSE
#घोषणा VERBOSE(x) x
#घोषणा NOVERBOSE(x) Curl_nop_stmt
#अन्यथा
#घोषणा VERBOSE(x) Curl_nop_stmt
#घोषणा NOVERBOSE(x) x
#पूर्ण_अगर

/* For FreeBSD it is included from curl/curl.h */
#अगर defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#समावेश <sys/param.h>  /* for __DragonFly_version, OpenBSD,
                           __NetBSD_Version__ */
#पूर्ण_अगर

/* NetBSD before 6.1 did not set SS_NBIO for SOCK_NONBLOCK. */
#अगर defined(SOCK_NONBLOCK) && \
  (!defined(__NetBSD__) || (__NetBSD_Version__ >= 601000000))
#घोषणा CURL_USE_SOCK_NONBLOCK
#पूर्ण_अगर

#अगर_अघोषित _CURL_LOCAL_MEMZERO /* to be removed after a couple of releases */
#अगर_घोषित _WIN32
#अगर defined(_MSC_VER) && defined(NTDDI_VERSION) && \
  (NTDDI_VERSION >= 0x0A000010) /* MS SDK 10.0.26100.0+ */
#आशय comment(lib, "volatileaccessu.lib")
#घोषणा curlx_memzero_निचला(buf, आकार)  SecureZeroMemory2(buf, आकार)
#अन्यथा
#घोषणा curlx_memzero_निचला(buf, आकार)  SecureZeroMemory(buf, आकार)
#पूर्ण_अगर
#या_अगर defined(HAVE_MEMSET_S)
#घोषणा curlx_memzero_निचला(buf, आकार)  (व्योम)memset_s(buf, आकार, 0, आकार)
#या_अगर defined(HAVE_MEMSET_EXPLICIT)
#घोषणा curlx_memzero_निचला(buf, आकार)  (व्योम)memset_explicit(buf, 0, आकार)
#या_अगर defined(__CYGWIN__) || \
  (defined(__NEWLIB__) && !defined(__CLIB2__)) || \
  (defined(__GLIBC__) && \
    (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25))) || \
  (defined(__DragonFly__) && __DragonFly_version >= 500600 /* 5.6+ */) || \
  (defined(__FreeBSD__) && __FreeBSD_version >= 1100037 /* 11.0+ */) || \
  (defined(__OpenBSD__) && OpenBSD >= 201405 /* 5.5+ */)
#घोषणा curlx_memzero_निचला(buf, आकार)  explicit_bzero(buf, आकार)
#या_अगर defined(__NetBSD__) && __NetBSD_Version__ >= 702000000 /* 7.2+ */
#घोषणा curlx_memzero_निचला(buf, आकार)  (व्योम)explicit_memset(buf, 0, आकार)
#पूर्ण_अगर
#पूर्ण_अगर /* !_CURL_LOCAL_MEMZERO */

#अगर_अघोषित curlx_memzero_निचला
#घोषणा USE_CURLX_MEMZERO
व्योम curlx_memzero_निचला(व्योम *buf, माप_प्रकार आकार);
#पूर्ण_अगर
व्योम curlx_memzero(व्योम *buf, माप_प्रकार आकार);
व्योम curlx_strzero(व्योम *buf);

#पूर्ण_अगर /* HEADER_CURL_SETUP_H */
