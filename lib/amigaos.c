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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే __AMIGA__

#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "amigaos.h"

#నిర్వచితమైతే HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_PROTO_BSDSOCKET_H

#నిర్వచితమైతే __amigaos4__
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

#అన్తర్భావ <proto/bsdsocket.h>

స్థిర నిర్మాణం SocketIFace *__CurlISocket = ఖాళీ;
స్థిర uint32 SocketFeatures = 0;

#నిర్వచించు HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#నిర్వచించు HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(శూన్య)
{
  నిర్మాణం SocketIFace *ISocket;
  నిర్మాణం Library *పునాది = OpenLibrary("bsdsocket.library", 4);

  ఒకవేళ(పునాది) {
    ISocket = (నిర్మాణం SocketIFace *)GetInterface(పునాది, "main", 1, ఖాళీ);
    ఒకవేళ(ISocket) {
      ULONG enabled = 0;

      Socket_పునాది_ట్యాగులు(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     ట్యాగ్_పూర్తి);

      ఒకవేళ(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      తిరిగి CURLE_OK;
    }
    CloseLibrary(పునాది);
  }

  తిరిగి CURLE_FAILED_INIT;
}

శూన్య Curl_amiga_cleanup(శూన్య)
{
  ఒకవేళ(__CurlISocket) {
    నిర్మాణం Library *పునాది = __CurlISocket->దత్తాంశం_2.Lib_పునాది;
    DropInterface((నిర్మాణం Interface *)__CurlISocket);
    CloseLibrary(పునాది);
    __CurlISocket = ఖాళీ;
  }
}

#నిర్వచితమైతే CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

నిర్మాణం Curl_addrinfo *Curl_ipv4_resolve_r(స్థిరరాశి అక్షరం *hostname, uint16_t పోర్టు)
{
  నిర్మాణం Curl_addrinfo *ai = ఖాళీ;
  నిర్మాణం hostent *h;
  నిర్మాణం SocketIFace *ISocket = __CurlISocket;

  ఒకవేళ(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    నిర్మాణం hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_కొలత);
    ఒకవేళ(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (అక్షరం *)buf + పరిమాణం(నిర్మాణం hostent),
                          CURL_HOSTENT_కొలత - పరిమాణం(నిర్మాణం hostent),
                          &h_errnop);
      ఒకవేళ(h) {
        ai = Curl_he2ai(h, పోర్టు);
      }
      curlx_free(buf);
    }
  }
  లేకపోతే {
#నిర్వచితమైతే USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    నిర్మాణం Library *పునాది = OpenLibrary("bsdsocket.library", 4);
    ఒకవేళ(పునాది) {
      ISocket = (నిర్మాణం SocketIFace *)GetInterface(పునాది, "main", 1, ఖాళీ);
      ఒకవేళ(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        ఒకవేళ(h) {
          ai = Curl_he2ai(h, పోర్టు);
        }
        DropInterface((నిర్మాణం Interface *)ISocket);
      }
      CloseLibrary(పునాది);
    }
#లేకపోతే
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    ఒకవేళ(h) {
      ai = Curl_he2ai(h, పోర్టు);
    }
#అయితే_ముగింపు
  }

  తిరిగి ai;
}
#అయితే_ముగింపు /* CURLRES_AMIGA */

#నిర్వచితమైతే USE_AMISSL
#అన్తర్భావ <signal.h>
పూర్ణాంకం Curl_amiga_select(పూర్ణాంకం nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, నిర్మాణం timeval *timeout)
{
  పూర్ణాంకం r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  ఒకవేళ((r == -1) && (SOCKERRNO == SOCKEINTR))
    లేపు(SIGINT);
  తిరిగి r;
}
#అయితే_ముగింపు /* USE_AMISSL */

#లేదావేళ !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

నిర్మాణం Library *Socket_పునాది = ఖాళీ;

#నిర్వచితమైతే __libnix__
శూన్య __request(స్థిరరాశి అక్షరం *సందేశం);
#నిర్వచించు CURL_AMIGA_REQUEST(సందేశం)  __request(సందేశం)
#లేకపోతే
#నిర్వచించు CURL_AMIGA_REQUEST(సందేశం)  Printf((స్థిరరాశి గుర్తులేని అక్షరం *)(సందేశం "\n\a"), 0)
#అయితే_ముగింపు

శూన్య Curl_amiga_cleanup(శూన్య)
{
  ఒకవేళ(Socket_పునాది) {
    CloseLibrary(Socket_పునాది);
    Socket_పునాది = ఖాళీ;
  }
}

CURLcode Curl_amiga_init(శూన్య)
{
  ఒకవేళ(!Socket_పునాది)
    Socket_పునాది = OpenLibrary((స్థిరరాశి గుర్తులేని అక్షరం *)"bsdsocket.library", 4);

  ఒకవేళ(!Socket_పునాది) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    తిరిగి CURLE_FAILED_INIT;
  }

  ఒకవేళ(Socket_పునాది_ట్యాగులు(SBTM_SETVAL(SBTC_ERRNOPTR(పరిమాణం(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", ట్యాగ్_పూర్తి)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    తిరిగి CURLE_FAILED_INIT;
  }

#నిర్వచితం_కాకపోతే __libnix__
  atexit(Curl_amiga_cleanup);
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#అయితే_ముగింపు

#అయితే_ముగింపు /* !USE_AMISSL */

#అయితే_ముగింపు /* HAVE_PROTO_BSDSOCKET_H */

#అయితే_ముగింపు /* __AMIGA__ */
