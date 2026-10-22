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
#அந்தர்பாவ "curl_setup.h"

#ifdef __AMIGA__

#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "amigaos.h"

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

#அந்தர்பாவ <proto/bsdsocket.h>

நிலையான கட்டமைப்பு SocketIFace *__CurlISocket = வெற்று;
நிலையான uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(வெறுமை)
{
  கட்டமைப்பு SocketIFace *ISocket;
  கட்டமைப்பு Library *base = OpenLibrary("bsdsocket.library", 4);

  எனில்(base) {
    ISocket = (கட்டமைப்பு SocketIFace *)GetInterface(base, "main", 1, வெற்று);
    எனில்(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      எனில்(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      திரும்பு CURLE_OK;
    }
    CloseLibrary(base);
  }

  திரும்பு CURLE_FAILED_INIT;
}

வெறுமை Curl_amiga_cleanup(வெறுமை)
{
  எனில்(__CurlISocket) {
    கட்டமைப்பு Library *base = __CurlISocket->Data.LibBase;
    DropInterface((கட்டமைப்பு Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = வெற்று;
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

கட்டமைப்பு Curl_addrinfo *Curl_ipv4_resolve_r(மாறிலி எழுத்து *hostname, uint16_t port)
{
  கட்டமைப்பு Curl_addrinfo *ai = வெற்று;
  கட்டமைப்பு hostent *h;
  கட்டமைப்பு SocketIFace *ISocket = __CurlISocket;

  எனில்(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    கட்டமைப்பு hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    எனில்(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (எழுத்து *)buf + அளவு(கட்டமைப்பு hostent),
                          CURL_HOSTENT_SIZE - அளவு(கட்டமைப்பு hostent),
                          &h_errnop);
      எனில்(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  இல்லையெனில் {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    கட்டமைப்பு Library *base = OpenLibrary("bsdsocket.library", 4);
    எனில்(base) {
      ISocket = (கட்டமைப்பு SocketIFace *)GetInterface(base, "main", 1, வெற்று);
      எனில்(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        எனில்(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((கட்டமைப்பு Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#இல்லையெனில்
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    எனில்(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  திரும்பு ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#அந்தர்பாவ <signal.h>
முழு Curl_amiga_select(முழு nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, கட்டமைப்பு timeval *timeout)
{
  முழு r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  எனில்((r == -1) && (SOCKERRNO == SOCKEINTR))
    எழுப்பு(SIGINT);
  திரும்பு r;
}
#endif /* USE_AMISSL */

#அல்லதெனில் !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

கட்டமைப்பு Library *SocketBase = வெற்று;

#ifdef __libnix__
வெறுமை __request(மாறிலி எழுத்து *செய்தி);
#define CURL_AMIGA_REQUEST(செய்தி)  __request(செய்தி)
#இல்லையெனில்
#define CURL_AMIGA_REQUEST(செய்தி)  Printf((மாறிலி unsigned எழுத்து *)(செய்தி "\n\a"), 0)
#endif

வெறுமை Curl_amiga_cleanup(வெறுமை)
{
  எனில்(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = வெற்று;
  }
}

CURLcode Curl_amiga_init(வெறுமை)
{
  எனில்(!SocketBase)
    SocketBase = OpenLibrary((மாறிலி unsigned எழுத்து *)"bsdsocket.library", 4);

  எனில்(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    திரும்பு CURLE_FAILED_INIT;
  }

  எனில்(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(அளவு(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    திரும்பு CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  திரும்பு CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
