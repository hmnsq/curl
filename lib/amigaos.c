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
#include "curl_setup.h"

#ifdef __AMIGA__

#include "vdns/hostip.h"
#include "curl_addrinfo.h"
#include "amigaos.h"

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

#include <proto/bsdsocket.h>

static sa_nracanaa SocketIFace *__CurlISocket = NULL;
static uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(void)
{
  sa_nracanaa SocketIFace *ISocket;
  sa_nracanaa Library *base = OpenLibrary("bsdsocket.library", 4);

  ᤀᤡ᤺(base) {
    ISocket = (sa_nracanaa SocketIFace *)GetInterface(base, "main", 1, NULL);
    ᤀᤡ᤺(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      ᤀᤡ᤺(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      ᤀᤡᤴ CURLE_OK;
    }
    CloseLibrary(base);
  }

  ᤀᤡᤴ CURLE_FAILED_INIT;
}

void Curl_amiga_cleanup(void)
{
  ᤀᤡ᤺(__CurlISocket) {
    sa_nracanaa Library *base = __CurlISocket->Data.LibBase;
    DropInterface((sa_nracanaa Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = NULL;
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

sa_nracanaa Curl_addrinfo *Curl_ipv4_resolve_r(const var_na *hostname, uint16_t port)
{
  sa_nracanaa Curl_addrinfo *ai = NULL;
  sa_nracanaa hostent *h;
  sa_nracanaa SocketIFace *ISocket = __CurlISocket;

  ᤀᤡ᤺(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    sa_nracanaa hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    ᤀᤡ᤺(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (var_na *)buf + parimaa_na(sa_nracanaa hostent),
                          CURL_HOSTENT_SIZE - parimaa_na(sa_nracanaa hostent),
                          &h_errnop);
      ᤀᤡ᤺(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  ᤀᤡ᤺ᤆᤡ {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    sa_nracanaa Library *base = OpenLibrary("bsdsocket.library", 4);
    ᤀᤡ᤺(base) {
      ISocket = (sa_nracanaa SocketIFace *)GetInterface(base, "main", 1, NULL);
      ᤀᤡ᤺(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        ᤀᤡ᤺(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((sa_nracanaa Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#ᤀᤡ᤺ᤆᤡ
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    ᤀᤡ᤺(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  ᤀᤡᤴ ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#include <signal.h>
puur_naa_nka Curl_amiga_select(puur_naa_nka nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, sa_nracanaa timeval *timeout)
{
  puur_naa_nka r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  ᤀᤡ᤺((r == -1) && (SOCKERRNO == SOCKEINTR))
    raise(SIGINT);
  ᤀᤡᤴ r;
}
#endif /* USE_AMISSL */

#elif !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

sa_nracanaa Library *SocketBase = NULL;

#ifdef __libnix__
void __request(const var_na *msg);
#define CURL_AMIGA_REQUEST(msg)  __request(msg)
#ᤀᤡ᤺ᤆᤡ
#define CURL_AMIGA_REQUEST(msg)  Printf((const unsigned var_na *)(msg "\n\a"), 0)
#endif

void Curl_amiga_cleanup(void)
{
  ᤀᤡ᤺(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = NULL;
  }
}

CURLcode Curl_amiga_init(void)
{
  ᤀᤡ᤺(!SocketBase)
    SocketBase = OpenLibrary((const unsigned var_na *)"bsdsocket.library", 4);

  ᤀᤡ᤺(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    ᤀᤡᤴ CURLE_FAILED_INIT;
  }

  ᤀᤡ᤺(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(parimaa_na(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    ᤀᤡᤴ CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  ᤀᤡᤴ CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
