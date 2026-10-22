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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef __AMIGA__

#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "amigaos.h"

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

#ಅನ್ತರ್ಭಾವ <proto/bsdsocket.h>

ಸ್ಥಿರ ಸಂರಚನಾ SocketIFace *__CurlISocket = ರಿಕ್ತ;
ಸ್ಥಿರ uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ SocketIFace *ISocket;
  ಸಂರಚನಾ Library *base = OpenLibrary("bsdsocket.library", 4);

  ಯದಿ(base) {
    ISocket = (ಸಂರಚನಾ SocketIFace *)GetInterface(base, "main", 1, ರಿಕ್ತ);
    ಯದಿ(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      ಯದಿ(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    CloseLibrary(base);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}

ಶೂನ್ಯ Curl_amiga_cleanup(ಶೂನ್ಯ)
{
  ಯದಿ(__CurlISocket) {
    ಸಂರಚನಾ Library *base = __CurlISocket->Data.LibBase;
    DropInterface((ಸಂರಚನಾ Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = ರಿಕ್ತ;
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

ಸಂರಚನಾ Curl_addrinfo *Curl_ipv4_resolve_r(ಧ್ರುವ ವರ್ಣ *hostname, uint16_t port)
{
  ಸಂರಚನಾ Curl_addrinfo *ai = ರಿಕ್ತ;
  ಸಂರಚನಾ hostent *h;
  ಸಂರಚನಾ SocketIFace *ISocket = __CurlISocket;

  ಯದಿ(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ಸಂರಚನಾ hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    ಯದಿ(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (ವರ್ಣ *)buf + ಪರಿಮಾಣ(ಸಂರಚನಾ hostent),
                          CURL_HOSTENT_SIZE - ಪರಿಮಾಣ(ಸಂರಚನಾ hostent),
                          &h_errnop);
      ಯದಿ(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  ಅನ್ಯಥಾ {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ಸಂರಚನಾ Library *base = OpenLibrary("bsdsocket.library", 4);
    ಯದಿ(base) {
      ISocket = (ಸಂರಚನಾ SocketIFace *)GetInterface(base, "main", 1, ರಿಕ್ತ);
      ಯದಿ(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        ಯದಿ(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((ಸಂರಚನಾ Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#ಅನ್ಯಥಾ
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    ಯದಿ(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  ಪ್ರತ್ಯಾಗಮ ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#ಅನ್ತರ್ಭಾವ <signal.h>
ಪೂರ್ಣಾಙ್ಕ Curl_amiga_select(ಪೂರ್ಣಾಙ್ಕ nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ಸಂರಚನಾ timeval *timeout)
{
  ಪೂರ್ಣಾಙ್ಕ r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  ಯದಿ((r == -1) && (SOCKERRNO == SOCKEINTR))
    ಉತ್ಥಾಪಯ(SIGINT);
  ಪ್ರತ್ಯಾಗಮ r;
}
#endif /* USE_AMISSL */

#ನೋಚೇದ್ಯದಿ !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ಸಂರಚನಾ Library *SocketBase = ರಿಕ್ತ;

#ifdef __libnix__
ಶೂನ್ಯ __request(ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ);
#define CURL_AMIGA_REQUEST(ಸಂದೇಶ)  __request(ಸಂದೇಶ)
#ಅನ್ಯಥಾ
#define CURL_AMIGA_REQUEST(ಸಂದೇಶ)  Printf((ಧ್ರುವ unsigned ವರ್ಣ *)(ಸಂದೇಶ "\n\a"), 0)
#endif

ಶೂನ್ಯ Curl_amiga_cleanup(ಶೂನ್ಯ)
{
  ಯದಿ(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = ರಿಕ್ತ;
  }
}

CURLcode Curl_amiga_init(ಶೂನ್ಯ)
{
  ಯದಿ(!SocketBase)
    SocketBase = OpenLibrary((ಧ್ರುವ unsigned ವರ್ಣ *)"bsdsocket.library", 4);

  ಯದಿ(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಯದಿ(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(ಪರಿಮಾಣ(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
