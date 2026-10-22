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

स्थिर पूर्णाङ्क wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  पूर्णाङ्क efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  यदि(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    प्रत्यागम -1;
  }
  socks[0] = socks[1] = efd;
  प्रत्यागम 0;
}

#नोचेद्यदि defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#अन्तर्भाव <fcntl.h>
#endif

स्थिर पूर्णाङ्क wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  प्रत्यागम pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#अन्यथा
#यदि defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  यदि(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    प्रत्यागम pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  अन्यथा
#endif
  यदि(pipe(socks))
    प्रत्यागम -1;
#ifdef HAVE_FCNTL
  यदि(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    प्रत्यागम -1;
  }
#endif
  यदि(nonblocking) {
    यदि(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      प्रत्यागम -1;
    }
  }
  प्रत्यागम 0;
#endif /* HAVE_PIPE2 */
}

#नोचेद्यदि defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

स्थिर पूर्णाङ्क wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  पूर्णाङ्क type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  यदि(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  यदि(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    प्रत्यागम -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  यदि(nonblocking) {
    यदि(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      प्रत्यागम -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  यदि(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    प्रत्यागम -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  प्रत्यागम 0;
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

स्थिर पूर्णाङ्क wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  सङ्घ {
    संरचना sockaddr_in inaddr;
    संरचना sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = परिमाण(a.inaddr);
  पूर्णाङ्क reuse = 1;
  संरचना pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  यदि(listener == CURL_SOCKET_BAD)
    प्रत्यागम -1;

  स_रखो(&a, 0, परिमाण(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#यदि defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (शून्य)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    पूर्णाङ्क exclusive = 1;
    यदि(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (वर्ण *)&exclusive, (curl_socklen_t)परिमाण(exclusive)) == -1)
      गच्छ error;
  }
#endif
#अन्यथा
  यदि(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (वर्ण *)&reuse, (curl_socklen_t)परिमाण(reuse)) == -1)
    गच्छ error;
#endif
  यदि(bind(listener, &a.addr, परिमाण(a.inaddr)) == -1)
    गच्छ error;
  यदि(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (पूर्णाङ्क)परिमाण(a.inaddr))
    गच्छ error;
  यदि(listen(listener, 1) == -1)
    गच्छ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  यदि(socks[0] == CURL_SOCKET_BAD)
    गच्छ error;
  यदि(connect(socks[0], &a.addr, परिमाण(a.inaddr)) == -1)
    गच्छ error;

  /* use non-blocking accept to make sure we do not block forever */
  यदि(curlx_nonblock(listener, TRUE) < 0)
    गच्छ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (शून्य)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, रिक्त, रिक्त);
  यदि(socks[1] == CURL_SOCKET_BAD)
    गच्छ error;
  अन्यथा {
    संरचना curltime start = curlx_now();
    वर्ण rnd[9];
    वर्ण check[परिमाण(rnd)];
    वर्ण *p = &check[0];
    माप_प्रकार s = परिमाण(check);

    यदि(Curl_rand(रिक्त, (अचिन्हित वर्ण *)rnd, परिमाण(rnd)))
      गच्छ error;

    /* write data to the socket */
    swrite(socks[0], rnd, परिमाण(rnd));
    /* verify that we read the correct data */
    कुरु {
      ssize_t nread;

      /* Do not block forever */
      यदि(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        गच्छ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (शून्य)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      यदि(!nread) { /* unexpected close before getting complete `check` */
        गच्छ error;
      }
      अन्यथा यदि(nread == -1) { /* error or close */
        पूर्णाङ्क sockerr = SOCKERRNO;
        यदि(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          अनुवर्त;
        }
        गच्छ error;
      }
      s -= nread;
      यदि(s) {
        p += nread;
        अनुवर्त;
      }
      यदि(स_भेद(rnd, check, परिमाण(check)))
        गच्छ error;
      भङ्ग;
    } यावत्(1);
  }

  यदि(nonblocking)
    यदि(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      गच्छ error;
#ifdef USE_SO_NOSIGPIPE
  यदि(Curl_sock_nosigpipe(socks[1]) < 0)
    गच्छ error;
#endif
  sclose(listener);
  प्रत्यागम 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  प्रत्यागम -1;
}

#endif /* choose implementation */

पूर्णाङ्क Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  प्रत्यागम wakeup_eventfd(socks, nonblocking);
#नोचेद्यदि defined(HAVE_PIPE)
  प्रत्यागम wakeup_pipe(socks, nonblocking);
#नोचेद्यदि defined(HAVE_SOCKETPAIR)
  प्रत्यागम wakeup_socketpair(socks, nonblocking);
#अन्यथा
  प्रत्यागम wakeup_inet(socks, nonblocking);
#endif
}

#यदि defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        लिख
#define wakeup_read         पठ
#define wakeup_close        बंद

#अन्यथा /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

पूर्णाङ्क Curl_wakeup_signal(curl_socket_t socks[2])
{
  पूर्णाङ्क sockerr = 0;
#ifdef USE_EVENTFD
  ध्रुव uint64_t buf[1] = { 1 };
#अन्यथा
  ध्रुव वर्ण buf[1] = { 1 };
#endif

  यावत्(1) {
    sockerr = 0;
    यदि(wakeup_write(socks[1], buf, परिमाण(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      यदि(sockerr == SOCKEINTR)
        अनुवर्त;
#endif
      यदि(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    भङ्ग;
  }
  प्रत्यागम sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  वर्ण buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  कुरु {
    rc = wakeup_read(socks[0], buf, परिमाण(buf));
    यदि(!rc)
      भङ्ग;
    अन्यथा यदि(rc < 0) {
      पूर्णाङ्क sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      यदि(sockerr == SOCKEINTR)
        अनुवर्त;
#endif
      यदि(SOCK_EAGAIN(sockerr))
        भङ्ग;
      result = CURLE_READ_ERROR;
      भङ्ग;
    }
  } यावत्(all);
  प्रत्यागम result;
}

शून्य Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  यदि(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  यदि(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
