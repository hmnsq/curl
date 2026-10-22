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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "socketpair.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#ಅನ್ತರ್ಭಾವ <sys/eventfd.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  ಪೂರ್ಣಾಙ್ಕ efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  ಯದಿ(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  socks[0] = socks[1] = efd;
  ಪ್ರತ್ಯಾಗಮ 0;
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  ಪ್ರತ್ಯಾಗಮ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ಅನ್ಯಥಾ
#ಯದಿ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  ಯದಿ(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    ಪ್ರತ್ಯಾಗಮ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ಅನ್ಯಥಾ
#endif
  ಯದಿ(pipe(socks))
    ಪ್ರತ್ಯಾಗಮ -1;
#ifdef HAVE_FCNTL
  ಯದಿ(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ಪ್ರತ್ಯಾಗಮ -1;
  }
#endif
  ಯದಿ(nonblocking) {
    ಯದಿ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
#endif /* HAVE_PIPE2 */
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  ಪೂರ್ಣಾಙ್ಕ type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  ಯದಿ(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  ಯದಿ(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    ಪ್ರತ್ಯಾಗಮ -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  ಯದಿ(nonblocking) {
    ಯದಿ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  ಯದಿ(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ಪ್ರತ್ಯಾಗಮ -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  ಪ್ರತ್ಯಾಗಮ 0;
}

#ಅನ್ಯಥಾ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#ಅನ್ತರ್ಭಾವ "select.h"   /* for Curl_poll */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  ಸಙ್ಘ {
    ಸಂರಚನಾ sockaddr_in inaddr;
    ಸಂರಚನಾ sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = ಪರಿಮಾಣ(a.inaddr);
  ಪೂರ್ಣಾಙ್ಕ reuse = 1;
  ಸಂರಚನಾ pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ಯದಿ(listener == CURL_SOCKET_BAD)
    ಪ್ರತ್ಯಾಗಮ -1;

  memset(&a, 0, ಪರಿಮಾಣ(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ಯದಿ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (ಶೂನ್ಯ)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    ಪೂರ್ಣಾಙ್ಕ exclusive = 1;
    ಯದಿ(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (ವರ್ಣ *)&exclusive, (curl_socklen_t)ಪರಿಮಾಣ(exclusive)) == -1)
      ಗಚ್ಛ error;
  }
#endif
#ಅನ್ಯಥಾ
  ಯದಿ(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (ವರ್ಣ *)&reuse, (curl_socklen_t)ಪರಿಮಾಣ(reuse)) == -1)
    ಗಚ್ಛ error;
#endif
  ಯದಿ(bind(listener, &a.addr, ಪರಿಮಾಣ(a.inaddr)) == -1)
    ಗಚ್ಛ error;
  ಯದಿ(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(a.inaddr))
    ಗಚ್ಛ error;
  ಯದಿ(listen(listener, 1) == -1)
    ಗಚ್ಛ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ಯದಿ(socks[0] == CURL_SOCKET_BAD)
    ಗಚ್ಛ error;
  ಯದಿ(connect(socks[0], &a.addr, ಪರಿಮಾಣ(a.inaddr)) == -1)
    ಗಚ್ಛ error;

  /* use non-blocking accept to make sure we do not block forever */
  ಯದಿ(curlx_nonblock(listener, TRUE) < 0)
    ಗಚ್ಛ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (ಶೂನ್ಯ)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ರಿಕ್ತ, ರಿಕ್ತ);
  ಯದಿ(socks[1] == CURL_SOCKET_BAD)
    ಗಚ್ಛ error;
  ಅನ್ಯಥಾ {
    ಸಂರಚನಾ curltime start = curlx_now();
    ವರ್ಣ rnd[9];
    ವರ್ಣ check[ಪರಿಮಾಣ(rnd)];
    ವರ್ಣ *p = &check[0];
    size_t s = ಪರಿಮಾಣ(check);

    ಯದಿ(Curl_rand(ರಿಕ್ತ, (unsigned ವರ್ಣ *)rnd, ಪರಿಮಾಣ(rnd)))
      ಗಚ್ಛ error;

    /* write data to the socket */
    swrite(socks[0], rnd, ಪರಿಮಾಣ(rnd));
    /* verify that we read the correct data */
    ಕುರು {
      ssize_t nread;

      /* Do not block forever */
      ಯದಿ(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        ಗಚ್ಛ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (ಶೂನ್ಯ)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ಯದಿ(!nread) { /* unexpected close before getting complete `check` */
        ಗಚ್ಛ error;
      }
      ಅನ್ಯಥಾ ಯದಿ(nread == -1) { /* error or close */
        ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
        ಯದಿ(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          ಅನುವರ್ತ;
        }
        ಗಚ್ಛ error;
      }
      s -= nread;
      ಯದಿ(s) {
        p += nread;
        ಅನುವರ್ತ;
      }
      ಯದಿ(memcmp(rnd, check, ಪರಿಮಾಣ(check)))
        ಗಚ್ಛ error;
      ಭಙ್ಗ;
    } ಯಾವತ್(1);
  }

  ಯದಿ(nonblocking)
    ಯದಿ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      ಗಚ್ಛ error;
#ifdef USE_SO_NOSIGPIPE
  ಯದಿ(Curl_sock_nosigpipe(socks[1]) < 0)
    ಗಚ್ಛ error;
#endif
  sclose(listener);
  ಪ್ರತ್ಯಾಗಮ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  ಪ್ರತ್ಯಾಗಮ -1;
}

#endif /* choose implementation */

ಪೂರ್ಣಾಙ್ಕ Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  ಪ್ರತ್ಯಾಗಮ wakeup_eventfd(socks, nonblocking);
#ನೋಚೇದ್ಯದಿ defined(HAVE_PIPE)
  ಪ್ರತ್ಯಾಗಮ wakeup_pipe(socks, nonblocking);
#ನೋಚೇದ್ಯದಿ defined(HAVE_SOCKETPAIR)
  ಪ್ರತ್ಯಾಗಮ wakeup_socketpair(socks, nonblocking);
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ wakeup_inet(socks, nonblocking);
#endif
}

#ಯದಿ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        ಲಿಖ
#define wakeup_read         ಪಠ
#define wakeup_close        close

#ಅನ್ಯಥಾ /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

ಪೂರ್ಣಾಙ್ಕ Curl_wakeup_signal(curl_socket_t socks[2])
{
  ಪೂರ್ಣಾಙ್ಕ sockerr = 0;
#ifdef USE_EVENTFD
  ಧ್ರುವ uint64_t buf[1] = { 1 };
#ಅನ್ಯಥಾ
  ಧ್ರುವ ವರ್ಣ buf[1] = { 1 };
#endif

  ಯಾವತ್(1) {
    sockerr = 0;
    ಯದಿ(wakeup_write(socks[1], buf, ಪರಿಮಾಣ(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ಯದಿ(sockerr == SOCKEINTR)
        ಅನುವರ್ತ;
#endif
      ಯದಿ(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  ವರ್ಣ buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  ಕುರು {
    rc = wakeup_read(socks[0], buf, ಪರಿಮಾಣ(buf));
    ಯದಿ(!rc)
      ಭಙ್ಗ;
    ಅನ್ಯಥಾ ಯದಿ(rc < 0) {
      ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ಯದಿ(sockerr == SOCKEINTR)
        ಅನುವರ್ತ;
#endif
      ಯದಿ(SOCK_EAGAIN(sockerr))
        ಭಙ್ಗ;
      result = CURLE_READ_ERROR;
      ಭಙ್ಗ;
    }
  } ಯಾವತ್(all);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  ಯದಿ(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  ಯದಿ(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
