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
#شامل "curl_setup.h"

#ifdef __AMIGA__

#شامل "vdns/hostip.h"
#شامل "curl_addrinfo.h"
#شامل "amigaos.h"

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

#شامل <proto/bsdsocket.h>

ایستا ساختار SocketIFace *__CurlISocket = پوچ;
ایستا uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(تهی)
{
  ساختار SocketIFace *ISocket;
  ساختار Library *base = OpenLibrary("bsdsocket.library", 4);

  اگر(base) {
    ISocket = (ساختار SocketIFace *)GetInterface(base, "main", 1, پوچ);
    اگر(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      اگر(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      بازگشت CURLE_OK;
    }
    CloseLibrary(base);
  }

  بازگشت CURLE_FAILED_INIT;
}

تهی Curl_amiga_cleanup(تهی)
{
  اگر(__CurlISocket) {
    ساختار Library *base = __CurlISocket->Data.LibBase;
    DropInterface((ساختار Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = پوچ;
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

ساختار Curl_addrinfo *Curl_ipv4_resolve_r(ثابت نویسه *hostname, uint16_t port)
{
  ساختار Curl_addrinfo *ai = پوچ;
  ساختار hostent *h;
  ساختار SocketIFace *ISocket = __CurlISocket;

  اگر(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ساختار hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    اگر(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (نویسه *)buf + اندازه(ساختار hostent),
                          CURL_HOSTENT_SIZE - اندازه(ساختار hostent),
                          &h_errnop);
      اگر(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  وگرنه {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ساختار Library *base = OpenLibrary("bsdsocket.library", 4);
    اگر(base) {
      ISocket = (ساختار SocketIFace *)GetInterface(base, "main", 1, پوچ);
      اگر(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        اگر(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((ساختار Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#وگرنه
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    اگر(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  بازگشت ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#شامل <signal.h>
صحیح Curl_amiga_select(صحیح nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ساختار timeval *timeout)
{
  صحیح r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  اگر((r == -1) && (SOCKERRNO == SOCKEINTR))
    بالاببر(SIGINT);
  بازگشت r;
}
#endif /* USE_AMISSL */

#وگرنهاگر !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ساختار Library *SocketBase = پوچ;

#ifdef __libnix__
تهی __request(ثابت نویسه *پیام);
#define CURL_AMIGA_REQUEST(پیام)  __request(پیام)
#وگرنه
#define CURL_AMIGA_REQUEST(پیام)  Printf((ثابت unsigned نویسه *)(پیام "\n\a"), 0)
#endif

تهی Curl_amiga_cleanup(تهی)
{
  اگر(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = پوچ;
  }
}

CURLcode Curl_amiga_init(تهی)
{
  اگر(!SocketBase)
    SocketBase = OpenLibrary((ثابت unsigned نویسه *)"bsdsocket.library", 4);

  اگر(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    بازگشت CURLE_FAILED_INIT;
  }

  اگر(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(اندازه(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    بازگشت CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  بازگشت CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
