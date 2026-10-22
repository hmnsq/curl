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
#অন্তর্ভাব "curl_setup.h"

#ifdef __AMIGA__

#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "amigaos.h"

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

#অন্তর্ভাব <proto/bsdsocket.h>

স্থির কাঠামো SocketIFace *__CurlISocket = ফাঁকা;
স্থির uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(শূন্য)
{
  কাঠামো SocketIFace *ISocket;
  কাঠামো Library *base = OpenLibrary("bsdsocket.library", 4);

  যদি(base) {
    ISocket = (কাঠামো SocketIFace *)GetInterface(base, "main", 1, ফাঁকা);
    যদি(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      যদি(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      ফেরত CURLE_OK;
    }
    CloseLibrary(base);
  }

  ফেরত CURLE_FAILED_INIT;
}

শূন্য Curl_amiga_cleanup(শূন্য)
{
  যদি(__CurlISocket) {
    কাঠামো Library *base = __CurlISocket->তথ্য_2.LibBase;
    DropInterface((কাঠামো Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = ফাঁকা;
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

কাঠামো Curl_addrinfo *Curl_ipv4_resolve_r(ধ্রুবক অক্ষর *hostname, uint16_t port)
{
  কাঠামো Curl_addrinfo *ai = ফাঁকা;
  কাঠামো hostent *h;
  কাঠামো SocketIFace *ISocket = __CurlISocket;

  যদি(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    কাঠামো hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_আকার);
    যদি(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (অক্ষর *)buf + আকার(কাঠামো hostent),
                          CURL_HOSTENT_আকার - আকার(কাঠামো hostent),
                          &h_errnop);
      যদি(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  অন্যথা {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    কাঠামো Library *base = OpenLibrary("bsdsocket.library", 4);
    যদি(base) {
      ISocket = (কাঠামো SocketIFace *)GetInterface(base, "main", 1, ফাঁকা);
      যদি(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        যদি(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((কাঠামো Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#অন্যথা
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    যদি(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  ফেরত ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#অন্তর্ভাব <signal.h>
পূর্ণসংখ্যা Curl_amiga_select(পূর্ণসংখ্যা nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, কাঠামো timeval *timeout)
{
  পূর্ণসংখ্যা r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  যদি((r == -1) && (SOCKERRNO == SOCKEINTR))
    তোলো(SIGINT);
  ফেরত r;
}
#endif /* USE_AMISSL */

#নতুবা_যদি !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

কাঠামো Library *SocketBase = ফাঁকা;

#ifdef __libnix__
শূন্য __request(ধ্রুবক অক্ষর *বার্তা);
#define CURL_AMIGA_REQUEST(বার্তা)  __request(বার্তা)
#অন্যথা
#define CURL_AMIGA_REQUEST(বার্তা)  Printf((ধ্রুবক unsigned অক্ষর *)(বার্তা "\n\a"), 0)
#endif

শূন্য Curl_amiga_cleanup(শূন্য)
{
  যদি(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = ফাঁকা;
  }
}

CURLcode Curl_amiga_init(শূন্য)
{
  যদি(!SocketBase)
    SocketBase = OpenLibrary((ধ্রুবক unsigned অক্ষর *)"bsdsocket.library", 4);

  যদি(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ফেরত CURLE_FAILED_INIT;
  }

  যদি(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(আকার(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ফেরত CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  ফেরত CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
