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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef __AMIGA__

#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "amigaos.h"

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

#അന്തര്ഭാവ <proto/bsdsocket.h>

സ്ഥിതം ഘടന SocketIFace *__CurlISocket = ശൂന്യ;
സ്ഥിതം uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(ശൂന്യം)
{
  ഘടന SocketIFace *ISocket;
  ഘടന Library *base = OpenLibrary("bsdsocket.library", 4);

  എങ്കിൽ(base) {
    ISocket = (ഘടന SocketIFace *)GetInterface(base, "main", 1, ശൂന്യ);
    എങ്കിൽ(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      എങ്കിൽ(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      തിരികെ CURLE_OK;
    }
    CloseLibrary(base);
  }

  തിരികെ CURLE_FAILED_INIT;
}

ശൂന്യം Curl_amiga_cleanup(ശൂന്യം)
{
  എങ്കിൽ(__CurlISocket) {
    ഘടന Library *base = __CurlISocket->Data.LibBase;
    DropInterface((ഘടന Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = ശൂന്യ;
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

ഘടന Curl_addrinfo *Curl_ipv4_resolve_r(സ്ഥിരം അക്ഷരം *hostname, uint16_t port)
{
  ഘടന Curl_addrinfo *ai = ശൂന്യ;
  ഘടന hostent *h;
  ഘടന SocketIFace *ISocket = __CurlISocket;

  എങ്കിൽ(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ഘടന hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    എങ്കിൽ(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (അക്ഷരം *)buf + വലുപ്പം(ഘടന hostent),
                          CURL_HOSTENT_SIZE - വലുപ്പം(ഘടന hostent),
                          &h_errnop);
      എങ്കിൽ(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  അല്ലെങ്കിൽ {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ഘടന Library *base = OpenLibrary("bsdsocket.library", 4);
    എങ്കിൽ(base) {
      ISocket = (ഘടന SocketIFace *)GetInterface(base, "main", 1, ശൂന്യ);
      എങ്കിൽ(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        എങ്കിൽ(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((ഘടന Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#അല്ലെങ്കിൽ
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    എങ്കിൽ(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  തിരികെ ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#അന്തര്ഭാവ <signal.h>
പൂർണ്ണസംഖ്യ Curl_amiga_select(പൂർണ്ണസംഖ്യ nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ഘടന timeval *timeout)
{
  പൂർണ്ണസംഖ്യ r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  എങ്കിൽ((r == -1) && (SOCKERRNO == SOCKEINTR))
    ഉയർത്തുക(SIGINT);
  തിരികെ r;
}
#endif /* USE_AMISSL */

#അല്ലെങ്കിലെങ്കിൽ !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ഘടന Library *SocketBase = ശൂന്യ;

#ifdef __libnix__
ശൂന്യം __request(സ്ഥിരം അക്ഷരം *സന്ദേശം);
#define CURL_AMIGA_REQUEST(സന്ദേശം)  __request(സന്ദേശം)
#അല്ലെങ്കിൽ
#define CURL_AMIGA_REQUEST(സന്ദേശം)  Printf((സ്ഥിരം unsigned അക്ഷരം *)(സന്ദേശം "\n\a"), 0)
#endif

ശൂന്യം Curl_amiga_cleanup(ശൂന്യം)
{
  എങ്കിൽ(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = ശൂന്യ;
  }
}

CURLcode Curl_amiga_init(ശൂന്യം)
{
  എങ്കിൽ(!SocketBase)
    SocketBase = OpenLibrary((സ്ഥിരം unsigned അക്ഷരം *)"bsdsocket.library", 4);

  എങ്കിൽ(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    തിരികെ CURLE_FAILED_INIT;
  }

  എങ്കിൽ(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(വലുപ്പം(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    തിരികെ CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  തിരികെ CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
