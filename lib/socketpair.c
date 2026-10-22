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

#अन्तर्भाव "socketpair.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#अन्तर्भाव <sys/eventfd.h>

स्थिर पूर्णांक wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  पूर्णांक efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  जर(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    परत -1;
  }
  socks[0] = socks[1] = efd;
  परत 0;
}

#किंवाजर defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#अन्तर्भाव <fcntl.h>
#endif

स्थिर पूर्णांक wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  परत pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#अन्यथा
#जर defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  ((defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
            __MAC_OS_X_VERSION_MAX_ALLOWED >= 270000) || \
   (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
            __IPHONE_OS_VERSION_MAX_ALLOWED >= 270000) || \
   (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
            __TV_OS_VERSION_MAX_ALLOWED >= 270000) || \
   (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
            __WATCH_OS_VERSION_MAX_ALLOWED >= 270000) || \
   (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
            __VISION_OS_VERSION_MAX_ALLOWED >= 270000))
  जर(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    परत pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  अन्यथा
#endif
  जर(pipe(socks))
    परत -1;
#ifdef HAVE_FCNTL
  जर(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    परत -1;
  }
#endif
  जर(nonblocking) {
    जर(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      परत -1;
    }
  }
  परत 0;
#endif /* HAVE_PIPE2 */
}

#किंवाजर defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

स्थिर पूर्णांक wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  पूर्णांक type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  जर(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  जर(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    परत -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  जर(nonblocking) {
    जर(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      परत -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  जर(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    परत -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  परत 0;
}

#अन्यथा /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#अन्तर्भाव "select.h"   /* for Curl_poll */

स्थिर पूर्णांक wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  संघ {
    संरचना sockaddr_in inaddr;
    संरचना sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = आकार(a.inaddr);
  पूर्णांक reuse = 1;
  संरचना pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  जर(listener == CURL_SOCKET_BAD)
    परत -1;

  स_रखो(&a, 0, आकार(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#जर defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (शून्य)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    पूर्णांक exclusive = 1;
    जर(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (अक्षर *)&exclusive, (curl_socklen_t)आकार(exclusive)) == -1)
      जा error;
  }
#endif
#अन्यथा
  जर(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (अक्षर *)&reuse, (curl_socklen_t)आकार(reuse)) == -1)
    जा error;
#endif
  जर(bind(listener, &a.addr, आकार(a.inaddr)) == -1)
    जा error;
  जर(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (पूर्णांक)आकार(a.inaddr))
    जा error;
  जर(listen(listener, 1) == -1)
    जा error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  जर(socks[0] == CURL_SOCKET_BAD)
    जा error;
  जर(connect(socks[0], &a.addr, आकार(a.inaddr)) == -1)
    जा error;

  /* use non-blocking accept to make sure we do not block forever */
  जर(curlx_nonblock(listener, TRUE) < 0)
    जा error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (शून्य)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, रिक्त, रिक्त);
  जर(socks[1] == CURL_SOCKET_BAD)
    जा error;
  अन्यथा {
    संरचना curltime start = curlx_now();
    अक्षर rnd[9];
    अक्षर check[आकार(rnd)];
    अक्षर *p = &check[0];
    माप_प्रकार s = आकार(check);

    जर(Curl_rand(रिक्त, (अचिन्हित अक्षर *)rnd, आकार(rnd)))
      जा error;

    /* write data to the socket */
    swrite(socks[0], rnd, आकार(rnd));
    /* verify that we read the correct data */
    करा {
      ssize_t nread;

      /* Do not block forever */
      जर(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        जा error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (शून्य)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      जर(!nread) { /* unexpected close before getting complete `check` */
        जा error;
      }
      अन्यथा जर(nread == -1) { /* error or close */
        पूर्णांक sockerr = SOCKERRNO;
        जर(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          सुरूठेवा;
        }
        जा error;
      }
      s -= nread;
      जर(s) {
        p += nread;
        सुरूठेवा;
      }
      जर(स_भेद(rnd, check, आकार(check)))
        जा error;
      थांबा;
    } पर्यंत(1);
  }

  जर(nonblocking)
    जर(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      जा error;
#ifdef USE_SO_NOSIGPIPE
  जर(Curl_sock_nosigpipe(socks[1]) < 0)
    जा error;
#endif
  sclose(listener);
  परत 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  परत -1;
}

#endif /* choose implementation */

पूर्णांक Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  परत wakeup_eventfd(socks, nonblocking);
#किंवाजर defined(HAVE_PIPE)
  परत wakeup_pipe(socks, nonblocking);
#किंवाजर defined(HAVE_SOCKETPAIR)
  परत wakeup_socketpair(socks, nonblocking);
#अन्यथा
  परत wakeup_inet(socks, nonblocking);
#endif
}

#जर defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        लिख
#define wakeup_read         पठ
#define wakeup_close        बंद

#अन्यथा /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

पूर्णांक Curl_wakeup_signal(curl_socket_t socks[2])
{
  पूर्णांक sockerr = 0;
#ifdef USE_EVENTFD
  स्थिरांक uint64_t buf[1] = { 1 };
#अन्यथा
  स्थिरांक अक्षर buf[1] = { 1 };
#endif

  पर्यंत(1) {
    sockerr = 0;
    जर(wakeup_write(socks[1], buf, आकार(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      जर(sockerr == SOCKEINTR)
        सुरूठेवा;
#endif
      जर(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    थांबा;
  }
  परत sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  अक्षर buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  करा {
    rc = wakeup_read(socks[0], buf, आकार(buf));
    जर(!rc)
      थांबा;
    अन्यथा जर(rc < 0) {
      पूर्णांक sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      जर(sockerr == SOCKEINTR)
        सुरूठेवा;
#endif
      जर(SOCK_EAGAIN(sockerr))
        थांबा;
      result = CURLE_READ_ERROR;
      थांबा;
    }
  } पर्यंत(all);
  परत result;
}

शून्य Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  जर(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  जर(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
