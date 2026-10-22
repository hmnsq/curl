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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef __AMIGA__

#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "amigaos.h"

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

#অন্তর্ভাৱ <proto/bsdsocket.h>

স্থির সংরচনা SocketIFace *__CurlISocket = রিক্ত;
স্থির uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(শূন্য)
{
  সংরচনা SocketIFace *ISocket;
  সংরচনা Library *base = OpenLibrary("bsdsocket.library", 4);

  যদি(base) {
    ISocket = (সংরচনা SocketIFace *)GetInterface(base, "main", 1, রিক্ত);
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

      ঘূৰাই CURLE_OK;
    }
    CloseLibrary(base);
  }

  ঘূৰাই CURLE_FAILED_INIT;
}

শূন্য Curl_amiga_cleanup(শূন্য)
{
  যদি(__CurlISocket) {
    সংরচনা Library *base = __CurlISocket->Data.LibBase;
    DropInterface((সংরচনা Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = রিক্ত;
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

সংরচনা Curl_addrinfo *Curl_ipv4_resolve_r(ধ্রুৱ ৱর্ণ *hostname, uint16_t port)
{
  সংরচনা Curl_addrinfo *ai = রিক্ত;
  সংরচনা hostent *h;
  সংরচনা SocketIFace *ISocket = __CurlISocket;

  যদি(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    সংরচনা hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    যদি(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (ৱর্ণ *)buf + পরিমাণ(সংরচনা hostent),
                          CURL_HOSTENT_SIZE - পরিমাণ(সংরচনা hostent),
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
    সংরচনা Library *base = OpenLibrary("bsdsocket.library", 4);
    যদি(base) {
      ISocket = (সংরচনা SocketIFace *)GetInterface(base, "main", 1, রিক্ত);
      যদি(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        যদি(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((সংরচনা Interface *)ISocket);
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

  ঘূৰাই ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#অন্তর্ভাৱ <signal.h>
পূর্ণাঙ্ক Curl_amiga_select(পূর্ণাঙ্ক nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, সংরচনা timeval *timeout)
{
  পূর্ণাঙ্ক r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  যদি((r == -1) && (SOCKERRNO == SOCKEINTR))
    উত্থাপয(SIGINT);
  ঘূৰাই r;
}
#endif /* USE_AMISSL */

#নোচেদ্যদি !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

সংরচনা Library *SocketBase = রিক্ত;

#ifdef __libnix__
শূন্য __request(ধ্রুৱ ৱর্ণ *বাৰ্তা);
#define CURL_AMIGA_REQUEST(বাৰ্তা)  __request(বাৰ্তা)
#অন্যথা
#define CURL_AMIGA_REQUEST(বাৰ্তা)  Printf((ধ্রুৱ unsigned ৱর্ণ *)(বাৰ্তা "\n\a"), 0)
#endif

শূন্য Curl_amiga_cleanup(শূন্য)
{
  যদি(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = রিক্ত;
  }
}

CURLcode Curl_amiga_init(শূন্য)
{
  যদি(!SocketBase)
    SocketBase = OpenLibrary((ধ্রুৱ unsigned ৱর্ণ *)"bsdsocket.library", 4);

  যদি(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ঘূৰাই CURLE_FAILED_INIT;
  }

  যদি(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(পরিমাণ(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ঘূৰাই CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  ঘূৰাই CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
