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

#اگر_تعریف_شدہ __AMIGA__

#شامل "vdns/hostip.h"
#شامل "curl_addrinfo.h"
#شامل "amigaos.h"

#اگر_تعریف_شدہ HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#اگر_ختم

#اگر_تعریف_شدہ HAVE_PROTO_BSDSOCKET_H

#اگر_تعریف_شدہ __amigaos4__
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

ساکن ساخت SocketIFace *__CurlISocket = تہی;
ساکن uint32 SocketFeatures = 0;

#تعریف_کرو HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#تعریف_کرو HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(خالی)
{
  ساخت SocketIFace *ISocket;
  ساخت Library *بنیاد = OpenLibrary("bsdsocket.library", 4);

  اگر(بنیاد) {
    ISocket = (ساخت SocketIFace *)GetInterface(بنیاد, "main", 1, تہی);
    اگر(ISocket) {
      ULONG enabled = 0;

      Socket_بنیاد_لیبل_فہرست(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     لیبل_مکمل);

      اگر(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      واپس CURLE_OK;
    }
    CloseLibrary(بنیاد);
  }

  واپس CURLE_FAILED_INIT;
}

خالی Curl_amiga_cleanup(خالی)
{
  اگر(__CurlISocket) {
    ساخت Library *بنیاد = __CurlISocket->ڈیٹا_2.Lib_بنیاد;
    DropInterface((ساخت Interface *)__CurlISocket);
    CloseLibrary(بنیاد);
    __CurlISocket = تہی;
  }
}

#اگر_تعریف_شدہ CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

ساخت Curl_addrinfo *Curl_ipv4_resolve_r(ثابت حرف *hostname, uint16_t پورٹ)
{
  ساخت Curl_addrinfo *ai = تہی;
  ساخت hostent *h;
  ساخت SocketIFace *ISocket = __CurlISocket;

  اگر(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    ساخت hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_سائز);
    اگر(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (حرف *)buf + حجم(ساخت hostent),
                          CURL_HOSTENT_سائز - حجم(ساخت hostent),
                          &h_errnop);
      اگر(h) {
        ai = Curl_he2ai(h, پورٹ);
      }
      curlx_free(buf);
    }
  }
  ورنہ {
#اگر_تعریف_شدہ USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    ساخت Library *بنیاد = OpenLibrary("bsdsocket.library", 4);
    اگر(بنیاد) {
      ISocket = (ساخت SocketIFace *)GetInterface(بنیاد, "main", 1, تہی);
      اگر(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        اگر(h) {
          ai = Curl_he2ai(h, پورٹ);
        }
        DropInterface((ساخت Interface *)ISocket);
      }
      CloseLibrary(بنیاد);
    }
#ورنہ
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    اگر(h) {
      ai = Curl_he2ai(h, پورٹ);
    }
#اگر_ختم
  }

  واپس ai;
}
#اگر_ختم /* CURLRES_AMIGA */

#اگر_تعریف_شدہ USE_AMISSL
#شامل <signal.h>
صحیح Curl_amiga_select(صحیح nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, ساخت timeval *timeout)
{
  صحیح r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  اگر((r == -1) && (SOCKERRNO == SOCKEINTR))
    اٹھاؤ(SIGINT);
  واپس r;
}
#اگر_ختم /* USE_AMISSL */

#وگرنہ_اگر !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

ساخت Library *Socket_بنیاد = تہی;

#اگر_تعریف_شدہ __libnix__
خالی __request(ثابت حرف *پیغام);
#تعریف_کرو CURL_AMIGA_REQUEST(پیغام)  __request(پیغام)
#ورنہ
#تعریف_کرو CURL_AMIGA_REQUEST(پیغام)  Printf((ثابت غیرعلامتی حرف *)(پیغام "\n\a"), 0)
#اگر_ختم

خالی Curl_amiga_cleanup(خالی)
{
  اگر(Socket_بنیاد) {
    CloseLibrary(Socket_بنیاد);
    Socket_بنیاد = تہی;
  }
}

CURLcode Curl_amiga_init(خالی)
{
  اگر(!Socket_بنیاد)
    Socket_بنیاد = OpenLibrary((ثابت غیرعلامتی حرف *)"bsdsocket.library", 4);

  اگر(!Socket_بنیاد) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    واپس CURLE_FAILED_INIT;
  }

  اگر(Socket_بنیاد_لیبل_فہرست(SBTM_SETVAL(SBTC_ERRNOPTR(حجم(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", لیبل_مکمل)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    واپس CURLE_FAILED_INIT;
  }

#اگر_غیر_تعریف_شدہ __libnix__
  atexit(Curl_amiga_cleanup);
#اگر_ختم

  واپس CURLE_OK;
}

#اگر_تعریف_شدہ __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#اگر_ختم

#اگر_ختم /* !USE_AMISSL */

#اگر_ختم /* HAVE_PROTO_BSDSOCKET_H */

#اگر_ختم /* __AMIGA__ */
