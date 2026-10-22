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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __AMIGA__

#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "amigaos.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PROTO_BSDSOCKET_H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
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

ಸ್ಥಿರ ರಚನೆ SocketIFace *__CurlISocket = ಖಾಲಿ;
ಸ್ಥಿರ uint32 SocketFeatures = 0;

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(ಶೂನ್ಯ)
{
  ರಚನೆ SocketIFace *ISocket;
  ರಚನೆ Library *ತಳ = OpenLibrary("bsdsocket.library", 4);

  ಆದರೆ(ತಳ) {
    ISocket = (ರಚನೆ SocketIFace *)GetInterface(ತಳ, "main", 1, ಖಾಲಿ);
    ಆದರೆ(ISocket) {
      ULONG enabled = 0;

      Socket_ತಳ_ಟ್ಯಾಗುಗಳು(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     ಟ್ಯಾಗ್_ಮುಗಿದ);

      ಆದರೆ(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    CloseLibrary(ತಳ);
  }

  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

ಶೂನ್ಯ Curl_amiga_cleanup(ಶೂನ್ಯ)
{
  ಆದರೆ(__CurlISocket) {
    ರಚನೆ Library *ತಳ = __CurlISocket->ದತ್ತಾಂಶ_2.Lib_ತಳ;
    DropInterface((ರಚನೆ Interface *)__CurlISocket);
    CloseLibrary(ತಳ);
    __CurlISocket = ಖಾಲಿ;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

ರಚನೆ Curl_addrinfo *Curl_ipv4_resolve_r(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, uint16_t ಪೋರ್ಟ್)
{
  ರಚನೆ Curl_addrinfo *ai = ಖಾಲಿ;
  ರಚನೆ hostent *h;
  ರಚನೆ SocketIFace *ISocket = __CurlISocket;

  ಆದರೆ(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ರಚನೆ hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_ಗಾತ್ರ);
    ಆದರೆ(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (ಅಕ್ಷರ *)buf + ಗಾತ್ರ(ರಚನೆ hostent),
                          CURL_HOSTENT_ಗಾತ್ರ - ಗಾತ್ರ(ರಚನೆ hostent),
                          &h_errnop);
      ಆದರೆ(h) {
        ai = Curl_he2ai(h, ಪೋರ್ಟ್);
      }
      curlx_free(buf);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ರಚನೆ Library *ತಳ = OpenLibrary("bsdsocket.library", 4);
    ಆದರೆ(ತಳ) {
      ISocket = (ರಚನೆ SocketIFace *)GetInterface(ತಳ, "main", 1, ಖಾಲಿ);
      ಆದರೆ(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        ಆದರೆ(h) {
          ai = Curl_he2ai(h, ಪೋರ್ಟ್);
        }
        DropInterface((ರಚನೆ Interface *)ISocket);
      }
      CloseLibrary(ತಳ);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    ಆದರೆ(h) {
      ai = Curl_he2ai(h, ಪೋರ್ಟ್);
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಹಿಂತಿರುಗಿ ai;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLRES_AMIGA */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_AMISSL
#ಅನ್ತರ್ಭಾವ <signal.h>
ಪೂರ್ಣಾಂಕ Curl_amiga_select(ಪೂರ್ಣಾಂಕ nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ರಚನೆ timeval *timeout)
{
  ಪೂರ್ಣಾಂಕ r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  ಆದರೆ((r == -1) && (SOCKERRNO == SOCKEINTR))
    ಎತ್ತು(SIGINT);
  ಹಿಂತಿರುಗಿ r;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_AMISSL */

#ಅಥವಾಆದರೆ !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ರಚನೆ Library *Socket_ತಳ = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __libnix__
ಶೂನ್ಯ __request(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ);
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_AMIGA_REQUEST(ಸಂದೇಶ)  __request(ಸಂದೇಶ)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_AMIGA_REQUEST(ಸಂದೇಶ)  Printf((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)(ಸಂದೇಶ "\n\a"), 0)
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_amiga_cleanup(ಶೂನ್ಯ)
{
  ಆದರೆ(Socket_ತಳ) {
    CloseLibrary(Socket_ತಳ);
    Socket_ತಳ = ಖಾಲಿ;
  }
}

CURLcode Curl_amiga_init(ಶೂನ್ಯ)
{
  ಆದರೆ(!Socket_ತಳ)
    Socket_ತಳ = OpenLibrary((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)"bsdsocket.library", 4);

  ಆದರೆ(!Socket_ತಳ) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆದರೆ(Socket_ತಳ_ಟ್ಯಾಗುಗಳು(SBTM_SETVAL(SBTC_ERRNOPTR(ಗಾತ್ರ(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", ಟ್ಯಾಗ್_ಮುಗಿದ)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __libnix__
  atexit(Curl_amiga_cleanup);
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !USE_AMISSL */

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_PROTO_BSDSOCKET_H */

#ಆದರೆ_ಅಂತ್ಯ /* __AMIGA__ */
