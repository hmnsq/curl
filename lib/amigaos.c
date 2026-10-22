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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __AMIGA__

#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "amigaos.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PROTO_BSDSOCKET_H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
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

#ਅਨ੍ਤਰ੍ਭਾਵ <proto/bsdsocket.h>

ਸਥਿਰ ਬਣਤਰ SocketIFace *__CurlISocket = ਨਿਰਾ;
ਸਥਿਰ uint32 SocketFeatures = 0;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(ਖਾਲੀ)
{
  ਬਣਤਰ SocketIFace *ISocket;
  ਬਣਤਰ Library *ਅਧਾਰ = OpenLibrary("bsdsocket.library", 4);

  ਜੇ(ਅਧਾਰ) {
    ISocket = (ਬਣਤਰ SocketIFace *)GetInterface(ਅਧਾਰ, "main", 1, ਨਿਰਾ);
    ਜੇ(ISocket) {
      ULONG enabled = 0;

      Socket_ਅਧਾਰ_ਟੈਗ_ਸੂਚੀ(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     ਟੈਗ_ਪੂਰਾ);

      ਜੇ(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      ਵਾਪਸ CURLE_OK;
    }
    CloseLibrary(ਅਧਾਰ);
  }

  ਵਾਪਸ CURLE_FAILED_INIT;
}

ਖਾਲੀ Curl_amiga_cleanup(ਖਾਲੀ)
{
  ਜੇ(__CurlISocket) {
    ਬਣਤਰ Library *ਅਧਾਰ = __CurlISocket->ਅੰਕੜੇ_2.Lib_ਅਧਾਰ;
    DropInterface((ਬਣਤਰ Interface *)__CurlISocket);
    CloseLibrary(ਅਧਾਰ);
    __CurlISocket = ਨਿਰਾ;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

ਬਣਤਰ Curl_addrinfo *Curl_ipv4_resolve_r(ਅਚਲ ਅੱਖਰ *hostname, uint16_t ਪੋਰਟ)
{
  ਬਣਤਰ Curl_addrinfo *ai = ਨਿਰਾ;
  ਬਣਤਰ hostent *h;
  ਬਣਤਰ SocketIFace *ISocket = __CurlISocket;

  ਜੇ(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ਬਣਤਰ hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_ਮਾਪ);
    ਜੇ(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (ਅੱਖਰ *)buf + ਆਕਾਰ(ਬਣਤਰ hostent),
                          CURL_HOSTENT_ਮਾਪ - ਆਕਾਰ(ਬਣਤਰ hostent),
                          &h_errnop);
      ਜੇ(h) {
        ai = Curl_he2ai(h, ਪੋਰਟ);
      }
      curlx_free(buf);
    }
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ਬਣਤਰ Library *ਅਧਾਰ = OpenLibrary("bsdsocket.library", 4);
    ਜੇ(ਅਧਾਰ) {
      ISocket = (ਬਣਤਰ SocketIFace *)GetInterface(ਅਧਾਰ, "main", 1, ਨਿਰਾ);
      ਜੇ(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        ਜੇ(h) {
          ai = Curl_he2ai(h, ਪੋਰਟ);
        }
        DropInterface((ਬਣਤਰ Interface *)ISocket);
      }
      CloseLibrary(ਅਧਾਰ);
    }
#ਨਹੀਂਤਾਂ
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    ਜੇ(h) {
      ai = Curl_he2ai(h, ਪੋਰਟ);
    }
#ਜੇ_ਅੰਤ
  }

  ਵਾਪਸ ai;
}
#ਜੇ_ਅੰਤ /* CURLRES_AMIGA */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_AMISSL
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
ਪੂਰਨ Curl_amiga_select(ਪੂਰਨ nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ਬਣਤਰ timeval *timeout)
{
  ਪੂਰਨ r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  ਜੇ((r == -1) && (SOCKERRNO == SOCKEINTR))
    ਉਠਾਓ(SIGINT);
  ਵਾਪਸ r;
}
#ਜੇ_ਅੰਤ /* USE_AMISSL */

#ਜਾਂਜੇ !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ਬਣਤਰ Library *Socket_ਅਧਾਰ = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __libnix__
ਖਾਲੀ __request(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_AMIGA_REQUEST(ਸੁਨੇਹਾ)  __request(ਸੁਨੇਹਾ)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_AMIGA_REQUEST(ਸੁਨੇਹਾ)  Printf((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)(ਸੁਨੇਹਾ "\n\a"), 0)
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_amiga_cleanup(ਖਾਲੀ)
{
  ਜੇ(Socket_ਅਧਾਰ) {
    CloseLibrary(Socket_ਅਧਾਰ);
    Socket_ਅਧਾਰ = ਨਿਰਾ;
  }
}

CURLcode Curl_amiga_init(ਖਾਲੀ)
{
  ਜੇ(!Socket_ਅਧਾਰ)
    Socket_ਅਧਾਰ = OpenLibrary((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)"bsdsocket.library", 4);

  ਜੇ(!Socket_ਅਧਾਰ) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਜੇ(Socket_ਅਧਾਰ_ਟੈਗ_ਸੂਚੀ(SBTM_SETVAL(SBTC_ERRNOPTR(ਆਕਾਰ(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", ਟੈਗ_ਪੂਰਾ)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __libnix__
  atexit(Curl_amiga_cleanup);
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !USE_AMISSL */

#ਜੇ_ਅੰਤ /* HAVE_PROTO_BSDSOCKET_H */

#ਜੇ_ਅੰਤ /* __AMIGA__ */
