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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef __AMIGA__

#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "amigaos.h"

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

#ଅନ୍ତର୍ଭାଵ <proto/bsdsocket.h>

ସ୍ଥିର ସଂରଚନା SocketIFace *__CurlISocket = ଖାଲି;
ସ୍ଥିର uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(ଶୂନ୍ୟ)
{
  ସଂରଚନା SocketIFace *ISocket;
  ସଂରଚନା Library *base = OpenLibrary("bsdsocket.library", 4);

  ଯଦି(base) {
    ISocket = (ସଂରଚନା SocketIFace *)GetInterface(base, "main", 1, ଖାଲି);
    ଯଦି(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      ଯଦି(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      ଫେରନ୍ତୁ CURLE_OK;
    }
    CloseLibrary(base);
  }

  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

ଶୂନ୍ୟ Curl_amiga_cleanup(ଶୂନ୍ୟ)
{
  ଯଦି(__CurlISocket) {
    ସଂରଚନା Library *base = __CurlISocket->Data.LibBase;
    DropInterface((ସଂରଚନା Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = ଖାଲି;
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

ସଂରଚନା Curl_addrinfo *Curl_ipv4_resolve_r(ଅଚଳ ଅକ୍ଷର *hostname, uint16_t port)
{
  ସଂରଚନା Curl_addrinfo *ai = ଖାଲି;
  ସଂରଚନା hostent *h;
  ସଂରଚନା SocketIFace *ISocket = __CurlISocket;

  ଯଦି(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ସଂରଚନା hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    ଯଦି(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (ଅକ୍ଷର *)buf + ଆକାର(ସଂରଚନା hostent),
                          CURL_HOSTENT_SIZE - ଆକାର(ସଂରଚନା hostent),
                          &h_errnop);
      ଯଦି(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  ଅନ୍ୟଥା {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ସଂରଚନା Library *base = OpenLibrary("bsdsocket.library", 4);
    ଯଦି(base) {
      ISocket = (ସଂରଚନା SocketIFace *)GetInterface(base, "main", 1, ଖାଲି);
      ଯଦି(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        ଯଦି(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((ସଂରଚନା Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#ଅନ୍ୟଥା
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    ଯଦି(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  ଫେରନ୍ତୁ ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#ଅନ୍ତର୍ଭାଵ <signal.h>
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_amiga_select(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ସଂରଚନା timeval *timeout)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  ଯଦି((r == -1) && (SOCKERRNO == SOCKEINTR))
    ଉଠାଅ(SIGINT);
  ଫେରନ୍ତୁ r;
}
#endif /* USE_AMISSL */

#ନଚେତଯଦି !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ସଂରଚନା Library *SocketBase = ଖାଲି;

#ifdef __libnix__
ଶୂନ୍ୟ __request(ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ);
#define CURL_AMIGA_REQUEST(ସନ୍ଦେଶ)  __request(ସନ୍ଦେଶ)
#ଅନ୍ୟଥା
#define CURL_AMIGA_REQUEST(ସନ୍ଦେଶ)  Printf((ଅଚଳ unsigned ଅକ୍ଷର *)(ସନ୍ଦେଶ "\n\a"), 0)
#endif

ଶୂନ୍ୟ Curl_amiga_cleanup(ଶୂନ୍ୟ)
{
  ଯଦି(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = ଖାଲି;
  }
}

CURLcode Curl_amiga_init(ଶୂନ୍ୟ)
{
  ଯଦି(!SocketBase)
    SocketBase = OpenLibrary((ଅଚଳ unsigned ଅକ୍ଷର *)"bsdsocket.library", 4);

  ଯଦି(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଯଦି(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(ଆକାର(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
