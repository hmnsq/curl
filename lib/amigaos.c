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
#समावेश "curl_setup.h"

#अगर_घोषित __AMIGA__

#समावेश "vdns/hostip.h"
#समावेश "curl_addrinfo.h"
#समावेश "amigaos.h"

#अगर_घोषित HAVE_PROTO_BSDSOCKET_H
#  ifdef __amigaos4__
#    include <bsdsocket/socketbasetags.h>
#  elif !defined(USE_AMISSL)
#    include <amitcp/socketbasetags.h>
#  endif
#  ifdef __libnix__
#    include <stabs.h>
#  endif
#पूर्ण_अगर

#अगर_घोषित HAVE_PROTO_BSDSOCKET_H

#अगर_घोषित __amigaos4__
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

#समावेश <proto/bsdsocket.h>

अटल काष्ठा SocketIFace *__CurlISocket = शून्य;
अटल uint32 SocketFeatures = 0;

#घोषणा HAVE_BSDSOCKET_GETHOSTBYNAME_R 0x01
#घोषणा HAVE_BSDSOCKET_GETADDRINFO     0x02

CURLcode Curl_amiga_init(व्योम)
{
  काष्ठा SocketIFace *ISocket;
  काष्ठा Library *base = OpenLibrary("bsdsocket.library", 4);

  अगर(base) {
    ISocket = (काष्ठा SocketIFace *)GetInterface(base, "main", 1, शून्य);
    अगर(ISocket) {
      ULONG enabled = 0;

      SocketBaseTags(SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES), TRUE,
                     SBTM_GETREF(SBTC_HAVE_GETHOSTADDR_R_API), (ULONG)&enabled,
                     TAG_DONE);

      अगर(enabled) {
        SocketFeatures |= HAVE_BSDSOCKET_GETHOSTBYNAME_R;
      }

      __CurlISocket = ISocket;

      निकास_पर(Curl_amiga_cleanup);

      वापस CURLE_OK;
    }
    CloseLibrary(base);
  }

  वापस CURLE_FAILED_INIT;
}

व्योम Curl_amiga_cleanup(व्योम)
{
  अगर(__CurlISocket) {
    काष्ठा Library *base = __CurlISocket->तथ्य_2.LibBase;
    DropInterface((काष्ठा Interface *)__CurlISocket);
    CloseLibrary(base);
    __CurlISocket = शून्य;
  }
}

#अगर_घोषित CURLRES_AMIGA
/*
 * Because we need to handle the different cases in hostip4.c at runtime,
 * not at compile-time, based on what was detected in Curl_amiga_init(),
 * we replace it completely with our own as to not complicate the baseline
 * code. Assumes malloc/calloc/free are thread-safe because Curl_he2ai()
 * allocates memory also.
 */

काष्ठा Curl_addrinfo *Curl_ipv4_resolve_r(स्थिर अक्षर *hostname, uint16_t port)
{
  काष्ठा Curl_addrinfo *ai = शून्य;
  काष्ठा hostent *h;
  काष्ठा SocketIFace *ISocket = __CurlISocket;

  अगर(SocketFeatures & HAVE_BSDSOCKET_GETHOSTBYNAME_R) {
    LONG h_errnop = 0;
    काष्ठा hostent *buf;

    buf = curlx_calloc(1, CURL_HOSTENT_आकार);
    अगर(buf) {
      h = gethostbyname_r((STRPTR)hostname, buf,
                          (अक्षर *)buf + माप(काष्ठा hostent),
                          CURL_HOSTENT_आकार - माप(काष्ठा hostent),
                          &h_errnop);
      अगर(h) {
        ai = Curl_he2ai(h, port);
      }
      curlx_free(buf);
    }
  }
  अन्यथा {
#अगर_घोषित USE_RESOLV_THREADED
    /* gethostbyname() is not thread-safe, so we need to reopen bsdsocket
     * on the thread's context
     */
    काष्ठा Library *base = OpenLibrary("bsdsocket.library", 4);
    अगर(base) {
      ISocket = (काष्ठा SocketIFace *)GetInterface(base, "main", 1, शून्य);
      अगर(ISocket) {
        h = gethostbyname((STRPTR)hostname);
        अगर(h) {
          ai = Curl_he2ai(h, port);
        }
        DropInterface((काष्ठा Interface *)ISocket);
      }
      CloseLibrary(base);
    }
#अन्यथा
    /* not using threaded resolver - safe to use this as-is */
    h = gethostbyname(hostname);
    अगर(h) {
      ai = Curl_he2ai(h, port);
    }
#पूर्ण_अगर
  }

  वापस ai;
}
#पूर्ण_अगर /* CURLRES_AMIGA */

#अगर_घोषित USE_AMISSL
#समावेश <signal.h>
पूर्णांक Curl_amiga_select(पूर्णांक nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *errorfds, काष्ठा timeval *timeout)
{
  पूर्णांक r = WaitSelect(nfds, readfds, writefds, errorfds, timeout, 0);
  /* Ensure Ctrl-C signal is actioned */
  अगर((r == -1) && (SOCKERRNO == SOCKEINTR))
    उठाओ(संक_विघ्न);
  वापस r;
}
#पूर्ण_अगर /* USE_AMISSL */

#या_अगर !defined(USE_AMISSL) /* __amigaos4__ */
/*
 * Amiga OS3 specific code
 */

काष्ठा Library *SocketBase = शून्य;

#अगर_घोषित __libnix__
व्योम __request(स्थिर अक्षर *संदेश);
#घोषणा CURL_AMIGA_REQUEST(संदेश)  __request(संदेश)
#अन्यथा
#घोषणा CURL_AMIGA_REQUEST(संदेश)  Printf((स्थिर अचिन्हित अक्षर *)(संदेश "\n\a"), 0)
#पूर्ण_अगर

व्योम Curl_amiga_cleanup(व्योम)
{
  अगर(SocketBase) {
    CloseLibrary(SocketBase);
    SocketBase = शून्य;
  }
}

CURLcode Curl_amiga_init(व्योम)
{
  अगर(!SocketBase)
    SocketBase = OpenLibrary((स्थिर अचिन्हित अक्षर *)"bsdsocket.library", 4);

  अगर(!SocketBase) {
    CURL_AMIGA_REQUEST("No TCP/IP Stack running!");
    वापस CURLE_FAILED_INIT;
  }

  अगर(SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOPTR(माप(त्रुटि_सं))), (ULONG)&त्रुटि_सं,
                    SBTM_SETVAL(SBTC_LOGTAGPTR), (ULONG)"curl", TAG_DONE)) {
    CURL_AMIGA_REQUEST("SocketBaseTags ERROR");
    वापस CURLE_FAILED_INIT;
  }

#अगर_अघोषित __libnix__
  निकास_पर(Curl_amiga_cleanup);
#पूर्ण_अगर

  वापस CURLE_OK;
}

#अगर_घोषित __libnix__
ADD2EXIT(Curl_amiga_cleanup, -50);
#पूर्ण_अगर

#पूर्ण_अगर /* !USE_AMISSL */

#पूर्ण_अगर /* HAVE_PROTO_BSDSOCKET_H */

#पूर्ण_अगर /* __AMIGA__ */
