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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् __AMIGA__

#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "amigaos.h"

#यदि_परिभाषितम् HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_PROTO_BSDSOCKET_H

#यदि_परिभाषितम् __amigaos4__
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

#अन्तर्भाव <proto/bsdsocket.h>

स्थिर संरचना SocketIFace *__CurlISocket = रिक्त;
स्थिर uint32 SocketFeatures = 0;

#परिभाषय HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#परिभाषय HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(शून्य)
{
  संरचना SocketIFace *ISocket;
  संरचना Library *आधारः = OpenLibrary("bsdsocket.library", 4);

  यदि(आधारः) {
    ISocket = (संरचना SocketIFace *)GetInterface(आधारः, "main", 1, रिक्त);
    यदि(ISocket) {
      ULONG enabled = 0;

      Socket_आधारः_चिह्नकानि(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     चिह्नकम्_समाप्तम्);

      यदि(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      atexit(Curl_amiga_cleanup);

      प्रत्यागम CURLE_OK;
    }
    CloseLibrary(आधारः);
  }

  प्रत्यागम CURLE_FAILED_INIT;
}

शून्य Curl_amiga_cleanup(शून्य)
{
  यदि(__CurlISocket) {
    संरचना Library *आधारः = __CurlISocket->दत्त_2.Lib_आधारः;
    DropInterface((संरचना Interface *)__CurlISocket);
    CloseLibrary(आधारः);
    __CurlISocket = रिक्त;
  }
}

#यदि_परिभाषितम् CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

संरचना Curl_addrinfo *Curl_ipv4_resolve_r(ध्रुव वर्ण *hostname, uint16_t पत्तनम्)
{
  संरचना Curl_addrinfo *ai = रिक्त;
  संरचना hostent *h;
  संरचना SocketIFace *ISocket = __CurlISocket;

  यदि(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    संरचना hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_आकार);
    यदि(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (वर्ण *)buf + परिमाण(संरचना hostent),
                          CURL_HOSTENT_आकार - परिमाण(संरचना hostent),
                          &h_errnop);
      यदि(h) {
        ai = Curl_he2ai(h, पत्तनम्);
      }
      curlx_free(buf);
    }
  }
  अन्यथा {
#यदि_परिभाषितम् USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    संरचना Library *आधारः = OpenLibrary("bsdsocket.library", 4);
    यदि(आधारः) {
      ISocket = (संरचना SocketIFace *)GetInterface(आधारः, "main", 1, रिक्त);
      यदि(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        यदि(h) {
          ai = Curl_he2ai(h, पत्तनम्);
        }
        DropInterface((संरचना Interface *)ISocket);
      }
      CloseLibrary(आधारः);
    }
#अन्यथा
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    यदि(h) {
      ai = Curl_he2ai(h, पत्तनम्);
    }
#यदि_अन्तः
  }

  प्रत्यागम ai;
}
#यदि_अन्तः /* CURLRES_AMIGA */

#यदि_परिभाषितम् USE_AMISSL
#अन्तर्भाव <signal.h>
पूर्णाङ्क Curl_amiga_select(पूर्णाङ्क nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, संरचना timeval *timeout)
{
  पूर्णाङ्क r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  यदि((r == -1) && (SOCKERRNO == SOCKEINTR))
    उत्थापय(SIGINT);
  प्रत्यागम r;
}
#यदि_अन्तः /* USE_AMISSL */

#नोचेद्यदि !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

संरचना Library *Socket_आधारः = रिक्त;

#यदि_परिभाषितम् __libnix__
शून्य __request(ध्रुव वर्ण *सन्देशः);
#परिभाषय CURL_AMIGA_REQUEST(सन्देशः)  __request(सन्देशः)
#अन्यथा
#परिभाषय CURL_AMIGA_REQUEST(सन्देशः)  Printf((ध्रुव अचिह्नित वर्ण *)(सन्देशः "\n\a"), 0)
#यदि_अन्तः

शून्य Curl_amiga_cleanup(शून्य)
{
  यदि(Socket_आधारः) {
    CloseLibrary(Socket_आधारः);
    Socket_आधारः = रिक्त;
  }
}

CURLcode Curl_amiga_init(शून्य)
{
  यदि(!Socket_आधारः)
    Socket_आधारः = OpenLibrary((ध्रुव अचिह्नित वर्ण *)"bsdsocket.library", 4);

  यदि(!Socket_आधारः) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(Socket_आधारः_चिह्नकानि(SBTM_SETVAL(SBTC_ERRNOPTR(परिमाण(errno))), (ULONG)&errno,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", चिह्नकम्_समाप्तम्)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    प्रत्यागम CURLE_FAILED_INIT;
  }

#यदि_अपरिभाषितम् __libnix__
  atexit(Curl_amiga_cleanup);
#यदि_अन्तः

  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#यदि_अन्तः

#यदि_अन्तः /* !USE_AMISSL */

#यदि_अन्तः /* HAVE_PROTO_BSDSOCKET_H */

#यदि_अन्तः /* __AMIGA__ */
