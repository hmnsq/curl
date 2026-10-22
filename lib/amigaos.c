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

#جے_تعریف_شدہ __AMIGA__

#ضمن "vdns/hostip.h"
#ضمن "curl_addrinfo.h"
#ضمن "amigaos.h"

#جے_تعریف_شدہ HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#جے_انت

#جے_تعریف_شدہ HAVE_PROTO_BSDSOCKET_H

#جے_تعریف_شدہ __amigaos4__
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

#تعریف_کرو HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#تعریف_کرو HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(فراغ)
{
  بنية SocketIFace *ISocket;
  بنية Library *ادھار = OpenLibrary("bsdsocket.library", 4);

  إذا(ادھار) {
    ISocket = (بنية SocketIFace *)GetInterface(ادھار, "main", 1, عدم);
    إذا(ISocket) {
      ULONG enabled = 0;

      Socket_ادھار_ٹیگ_سوچی(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     ٹیگ_پورا);

      إذا(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      أرجع CURLE_OK;
    }
    CloseLibrary(ادھار);
  }

  أرجع CURLE_FAILED_INIT;
}

فراغ Curl_amiga_cleanup(فراغ)
{
  إذا(__CurlISocket) {
    بنية Library *ادھار = __CurlISocket->انکڑے_2.Lib_ادھار;
    DropInterface((بنية Interface *)__CurlISocket);
    CloseLibrary(ادھار);
    __CurlISocket = عدم;
  }
}

#جے_تعریف_شدہ CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

بنية Curl_addrinfo *Curl_ipv4_resolve_r(ثابت حرف *hostname, uint16_t پورٹ)
{
  بنية Curl_addrinfo *ai = عدم;
  بنية hostent *h;
  بنية SocketIFace *ISocket = __CurlISocket;

  إذا(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    بنية hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_ناپ);
    إذا(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (حرف *)buf + حجم(بنية hostent),
                          CURL_HOSTENT_ناپ - حجم(بنية hostent),
                          &h_errnop);
      إذا(h) {
        ai = Curl_he2ai(h, پورٹ);
      }
      curlx_free(buf);
    }
  }
  وإلا {
#جے_تعریف_شدہ USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    بنية Library *ادھار = OpenLibrary("bsdsocket.library", 4);
    إذا(ادھار) {
      ISocket = (بنية SocketIFace *)GetInterface(ادھار, "main", 1, عدم);
      إذا(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        إذا(h) {
          ai = Curl_he2ai(h, پورٹ);
        }
        DropInterface((بنية Interface *)ISocket);
      }
      CloseLibrary(ادھار);
    }
#وإلا
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    إذا(h) {
      ai = Curl_he2ai(h, پورٹ);
    }
#جے_انت
  }

  أرجع ai;
}
#جے_انت /* CURLRES_AMIGA */

#جے_تعریف_شدہ USE_AMISSL
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
#جے_انت /* USE_AMISSL */

#وإلاإذا !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

بنية Library *Socket_ادھار = عدم;

#جے_تعریف_شدہ __libnix__
فراغ __request(ثابت حرف *پیغام);
#تعریف_کرو CURL_AMIGA_REQUEST(پیغام)  __request(پیغام)
#وإلا
#تعریف_کرو CURL_AMIGA_REQUEST(پیغام)  Printf((ثابت بنانشانی حرف *)(پیغام "\n\a"), 0)
#جے_انت

فراغ Curl_amiga_cleanup(فراغ)
{
  إذا(Socket_ادھار) {
    CloseLibrary(Socket_ادھار);
    Socket_ادھار = عدم;
  }
}

CURLcode Curl_amiga_init(فراغ)
{
  إذا(!Socket_ادھار)
    Socket_ادھار = OpenLibrary((ثابت بنانشانی حرف *)"bsdsocket.library", 4);

  إذا(!Socket_ادھار) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Socket_ادھار_ٹیگ_سوچی(SBTM_SETVAL(SBTC_ERRNOPTR(حجم(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", ٹیگ_پورا)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    أرجع CURLE_FAILED_INIT;
  }

#جے_بنا_تعریف __libnix__
  atexit(Curl_amiga_cleanup);
#جے_انت

  أرجع CURLE_OK;
}

#جے_تعریف_شدہ __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#جے_انت

#جے_انت /* !USE_AMISSL */

#جے_انت /* HAVE_PROTO_BSDSOCKET_H */

#جے_انت /* __AMIGA__ */
