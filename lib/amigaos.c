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

static struct SocketIFace *__CurlISocket = NULL;
static uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(void)
{
  struct SocketIFace *ISocket;
  struct Library *base = OpenLibrary("bsdsocket.library", 4);

  만약(base) {
    ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
    만약(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      만약(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      반환 CURLE_OK;
    }
    CloseLibrary(base);
  }

  반환 CURLE_FAILED_INIT;
}

void Curl_amiga_cleanup(void)
{
  만약(__CurlISocket) {
    struct Library *base = __CurlISocket->Data.LibBase;
    DropInterface((struct Interface *)__CurlISocket);
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

struct Curl_addrinfo *Curl_ipv4_resolve_r(const char *hostname, uint16_t port)
{
  struct Curl_addrinfo *ai = NULL;
  struct hostent *h;
  struct SocketIFace *ISocket = __CurlISocket;

  만약(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    struct hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    만약(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (char *)buf + sizeof(struct hostent),
                          CURL_HOSTENT_SIZE - sizeof(struct hostent),
                          &h_errnop);
      만약(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  아니면 {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    struct Library *base = OpenLibrary("bsdsocket.library", 4);
    만약(base) {
      ISocket = (struct SocketIFace *)GetInterface(base, "main", 1, NULL);
      만약(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        만약(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((struct Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#아니면
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    만약(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  반환 ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#include <signal.h>
정수 Curl_amiga_select(정수 nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, struct timeval *timeout)
{
  정수 r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  만약((r == -1) && (SOCKERRNO == SOCKEINTR))
    raise(SIGINT);
  반환 r;
}
#endif /* USE_AMISSL */

#elif !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

struct Library *SocketBase = NULL;

#ifdef __libnix__
void __request(const char *msg);
#define CURL_AMIGA_REQUEST(msg)  __request(msg)
#아니면
#define CURL_AMIGA_REQUEST(msg)  Printf((const unsigned char *)(msg "\n\a"), 0)
#endif

void Curl_amiga_cleanup(void)
{
  만약(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = NULL;
  }
}

CURLcode Curl_amiga_init(void)
{
  만약(!SocketBase)
    SocketBase = OpenLibrary((const unsigned char *)"bsdsocket.library", 4);

  만약(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    반환 CURLE_FAILED_INIT;
  }

  만약(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(sizeof(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    반환 CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  반환 CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
