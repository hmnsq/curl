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
#અન્તર્ભાવ "curl_setup.h"

#ifdef __AMIGA__

#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "amigaos.h"

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

#અન્તર્ભાવ <proto/bsdsocket.h>

સ્થિર સંરચના SocketIFace *__CurlISocket = ખાલી;
સ્થિર uint32 SocketFeatures = 0;

#define HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#define HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(શૂન્ય)
{
  સંરચના SocketIFace *ISocket;
  સંરચના Library *base = OpenLibrary("bsdsocket.library", 4);

  જો(base) {
    ISocket = (સંરચના SocketIFace *)GetInterface(base, "main", 1, ખાલી);
    જો(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      જો(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      પાછા CURLE_OK;
    }
    CloseLibrary(base);
  }

  પાછા CURLE_FAILED_INIT;
}

શૂન્ય Curl_amiga_cleanup(શૂન્ય)
{
  જો(__CurlISocket) {
    સંરચના Library *base = __CurlISocket->Data.LibBase;
    DropInterface((સંરચના Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = ખાલી;
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

સંરચના Curl_addrinfo *Curl_ipv4_resolve_r(અચલ અક્ષર *hostname, uint16_t port)
{
  સંરચના Curl_addrinfo *ai = ખાલી;
  સંરચના hostent *h;
  સંરચના SocketIFace *ISocket = __CurlISocket;

  જો(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    સંરચના hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_SIZE);
    જો(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (અક્ષર *)buf + કદ(સંરચના hostent),
                          CURL_HOSTENT_SIZE - કદ(સંરચના hostent),
                          &h_errnop);
      જો(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  નહીંતર {
#ifdef USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    સંરચના Library *base = OpenLibrary("bsdsocket.library", 4);
    જો(base) {
      ISocket = (સંરચના SocketIFace *)GetInterface(base, "main", 1, ખાલી);
      જો(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        જો(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((સંરચના Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#નહીંતર
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    જો(h) {
      ai = Curl_he2ai(h, port);
    }
#endif
  }

  પાછા ai;
}
#endif /* CURLRES_AMIGA */

#ifdef USE_AMISSL
#અન્તર્ભાવ <signal.h>
પૂર્ણાંક Curl_amiga_select(પૂર્ણાંક nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, સંરચના timeval *timeout)
{
  પૂર્ણાંક r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  જો((r == -1) && (SOCKERRNO == SOCKEINTR))
    ઉઠાવો(SIGINT);
  પાછા r;
}
#endif /* USE_AMISSL */

#અથવાજો !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

સંરચના Library *SocketBase = ખાલી;

#ifdef __libnix__
શૂન્ય __request(અચલ અક્ષર *સંદેશ);
#define CURL_AMIGA_REQUEST(સંદેશ)  __request(સંદેશ)
#નહીંતર
#define CURL_AMIGA_REQUEST(સંદેશ)  Printf((અચલ unsigned અક્ષર *)(સંદેશ "\n\a"), 0)
#endif

શૂન્ય Curl_amiga_cleanup(શૂન્ય)
{
  જો(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = ખાલી;
  }
}

CURLcode Curl_amiga_init(શૂન્ય)
{
  જો(!SocketBase)
    SocketBase = OpenLibrary((અચલ unsigned અક્ષર *)"bsdsocket.library", 4);

  જો(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    પાછા CURLE_FAILED_INIT;
  }

  જો(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(કદ(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    પાછા CURLE_FAILED_INIT;
  }

#ifndef __libnix__
  atexit(Curl_amiga_cleanup);
#endif

  પાછા CURLE_OK;
}

#ifdef __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#endif

#endif /* !USE_AMISSL */

#endif /* HAVE_PROTO_BSDSOCKET_H */

#endif /* __AMIGA__ */
