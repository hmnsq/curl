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
#ضمن "curl_setup.h"

#ifdef __AMIGA__

#ضمن "vdns/hostip.h"
#ضمن "curl_addrinfo.h"
#ضمن "amigaos.h"

#ifdef HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#endif

#ifdef HAVE_PROTO_BSDSOCKET_H

#ifdef __amigaos4__
/*
 * AmigaOS 4.x specific code
 */

/*
 * hostip4.c - Curl_ipv4_resolve_r() replacement code
 *
 * Logic that needs to be considered are the following build cases:
 * - newlib networking
 * - clib2 networking
 * - direct bsdsocket.library networking (usually AmiSSL builds)
 * Each with the threaded resolver enabled or not.
 *
 * With the threaded resolver enabled, try to use gethostbyname_r() where
 * available, otherwise (re)open bsdsocket.library and fallback to
 * gethostbyname().
 */

#ضمن <proto/bsdsocket.h>

ساكن بنية SocketIFace *__CurlISocket = عدم;
ساكن uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(فراغ)
{
  بنية SocketIFace *ISocket;
  بنية Library *base = OpenLibrary("bsdsocket.library", 4);

  إذا(base) {
    ISocket = (بنية SocketIFace *)GetInterface(base, "main", 1, عدم);
    إذا(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      إذا(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      أرجع CURLE_OK;
    }
    CloseLibrary(base);
  }

  أرجع CURLE_FAILED_INIT;
}

فراغ Curl_amiga_cleanup(فراغ)
{
  إذا(__CurlISocket) {
    بنية Library *base = __CurlISocket->Data.LibBase;
    DropInterface((بنية Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = عدم;
  }
}

#ifdef CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

بنية Curl_addrinfo *Curl_ipv4_resolve_r(ثابت حرف *hostname, uint16_t port)
{
  بنية Curl_addrinfo *ai = عدم;
  بنية hostent *h;
  بنية SocketIFace *ISocket = __CurlISocket;

  إذا(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    بنية hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    إذا(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (حرف *)buf + حجم(بنية hostent),
                          CURL_HOSTENT_SIZE - حجم(بنية hostent),
                          &h_errnop);
      إذا(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  وإلا {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    بنية Library *base = OpenLibrary("bsdsocket.library", 4);
    إذا(base) {
      ISocket = (بنية SocketIFace *)GetInterface(base, "main", 1, عدم);
      إذا(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        إذا(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((بنية Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#وإلا
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    إذا(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  أرجع ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#ضمن <signal.h>
صحيح Curl_amiga_select(صحيح nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, بنية timeval *timeout)
{
  صحيح r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  إذا((r == -1) && (SOCKERRNO == SOCKEINTR))
    ارفع(SIGINT);
  أرجع r;
}
#endif /* USE_AMISSL */

#وإلاإذا !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

بنية Library *SocketBase = عدم;

#ifdef __libnix__
فراغ __request(ثابت حرف *رسالة);
#define CURL_AMIGA_REQUEST(رسالة)  __request(رسالة)
#وإلا
#define CURL_AMIGA_REQUEST(رسالة)  Printf((ثابت unsigned حرف *)(رسالة "\n\a"), 0)
#endif

فراغ Curl_amiga_cleanup(فراغ)
{
  إذا(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = عدم;
  }
}

CURLcode Curl_amiga_init(فراغ)
{
  إذا(!SocketBase)
    SocketBase = OpenLibrary((ثابت unsigned حرف *)"bsdsocket.library", 4);

  إذا(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(حجم(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    أرجع CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  أرجع CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
