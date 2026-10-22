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
#ضمن "curl_setup.h"

#که_تعریف_شوی __AMIGA__

#ضمن "vdns/hostip.h"
#ضمن "curl_addrinfo.h"
#ضمن "amigaos.h"

#که_تعریف_شوی HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#د_که_پای

#که_تعریف_شوی HAVE_PROTO_BSDSOCKET_H

#که_تعریف_شوی __amigaos4__
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

#ضمن <proto/bsdsocket.h>

ساكن بنية SocketIFace *__CurlISocket = عدم;
ساكن uint32 SocketFeatures = 0;

#تعریف_کړه HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#تعریف_کړه HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(فراغ)
{
  بنية SocketIFace *ISocket;
  بنية Library *بنسټ = OpenLibrary("bsdsocket.library", 4);

  إذا(بنسټ) {
    ISocket = (بنية SocketIFace *)GetInterface(بنسټ, "main", 1, عدم);
    إذا(ISocket) {
      ULONG enabled = 0;

      Socket_بنسټ_نښانونه(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     نښان_بشپړ);

      إذا(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      أرجع CURLE_OK;
    }
    CloseLibrary(بنسټ);
  }

  أرجع CURLE_FAILED_INIT;
}

فراغ Curl_amiga_cleanup(فراغ)
{
  إذا(__CurlISocket) {
    بنية Library *بنسټ = __CurlISocket->معلومات_2.Lib_بنسټ;
    DropInterface((بنية Interface *)__CurlISocket);
    CloseLibrary(بنسټ);
    __CurlISocket = عدم;
  }
}

#که_تعریف_شوی CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

بنية Curl_addrinfo *Curl_ipv4_resolve_r(ثابت حرف *hostname, uint16_t پورټ)
{
  بنية Curl_addrinfo *ai = عدم;
  بنية hostent *h;
  بنية SocketIFace *ISocket = __CurlISocket;

  إذا(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    بنية hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_اندازه);
    إذا(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (حرف *)buf + حجم(بنية hostent),
                          CURL_HOSTENT_اندازه - حجم(بنية hostent),
                          &h_errnop);
      إذا(h) {
        ai = Curl_he2ai(h, پورټ);
      }
      curlx_free(buf);
    }
  }
  وإلا {
#که_تعریف_شوی USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    بنية Library *بنسټ = OpenLibrary("bsdsocket.library", 4);
    إذا(بنسټ) {
      ISocket = (بنية SocketIFace *)GetInterface(بنسټ, "main", 1, عدم);
      إذا(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        إذا(h) {
          ai = Curl_he2ai(h, پورټ);
        }
        DropInterface((بنية Interface *)ISocket);
      }
      CloseLibrary(بنسټ);
    }
#وإلا
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    إذا(h) {
      ai = Curl_he2ai(h, پورټ);
    }
#د_که_پای
  }

  أرجع ai;
}
#د_که_پای /* CURLRES_AMIGA */

#که_تعریف_شوی USE_AMISSL
#ضمن <signal.h>
صحيح Curl_amiga_select(صحيح nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, بنية timeval *timeout)
{
  صحيح r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  إذا((r == -1) && (SOCKERRNO == SOCKEINTR))
    ارفع(SIGINT);
  أرجع r;
}
#د_که_پای /* USE_AMISSL */

#وإلاإذا !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

بنية Library *Socket_بنسټ = عدم;

#که_تعریف_شوی __libnix__
فراغ __request(ثابت حرف *پیغام);
#تعریف_کړه CURL_AMIGA_REQUEST(پیغام)  __request(پیغام)
#وإلا
#تعریف_کړه CURL_AMIGA_REQUEST(پیغام)  Printf((ثابت بینښه حرف *)(پیغام "\n\a"), 0)
#د_که_پای

فراغ Curl_amiga_cleanup(فراغ)
{
  إذا(Socket_بنسټ) {
    CloseLibrary(Socket_بنسټ);
    Socket_بنسټ = عدم;
  }
}

CURLcode Curl_amiga_init(فراغ)
{
  إذا(!Socket_بنسټ)
    Socket_بنسټ = OpenLibrary((ثابت بینښه حرف *)"bsdsocket.library", 4);

  إذا(!Socket_بنسټ) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Socket_بنسټ_نښانونه(SBTM_SETVAL(SBTC_ERRNOPTR(حجم(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", نښان_بشپړ)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    أرجع CURLE_FAILED_INIT;
  }

#که_نه_تعریف_شوی __libnix__
  atexit(Curl_amiga_cleanup);
#د_که_پای

  أرجع CURLE_OK;
}

#که_تعریف_شوی __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#د_که_پای

#د_که_پای /* !USE_AMISSL */

#د_که_پای /* HAVE_PROTO_BSDSOCKET_H */

#د_که_پای /* __AMIGA__ */
