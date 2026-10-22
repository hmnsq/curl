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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "socketpair.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#അന്തര്ഭാവ <sys/eventfd.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  പൂർണ്ണസംഖ്യ efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  എങ്കിൽ(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    തിരികെ -1;
  }
  socks[0] = socks[1] = efd;
  തിരികെ 0;
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#അന്തര്ഭാവ <fcntl.h>
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  തിരികെ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#അല്ലെങ്കിൽ
#എങ്കിൽ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  എങ്കിൽ(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    തിരികെ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  അല്ലെങ്കിൽ
#endif
  എങ്കിൽ(pipe(socks))
    തിരികെ -1;
#ifdef HAVE_FCNTL
  എങ്കിൽ(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    തിരികെ -1;
  }
#endif
  എങ്കിൽ(nonblocking) {
    എങ്കിൽ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      തിരികെ -1;
    }
  }
  തിരികെ 0;
#endif /* HAVE_PIPE2 */
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  പൂർണ്ണസംഖ്യ type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  എങ്കിൽ(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  എങ്കിൽ(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    തിരികെ -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  എങ്കിൽ(nonblocking) {
    എങ്കിൽ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      തിരികെ -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  എങ്കിൽ(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    തിരികെ -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  തിരികെ 0;
}

#അല്ലെങ്കിൽ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#അന്തര്ഭാവ "select.h"   /* for Curl_poll */

സ്ഥിതം പൂർണ്ണസംഖ്യ wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  സംഘം {
    ഘടന sockaddr_in inaddr;
    ഘടന sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = വലുപ്പം(a.inaddr);
  പൂർണ്ണസംഖ്യ reuse = 1;
  ഘടന pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  എങ്കിൽ(listener == CURL_SOCKET_BAD)
    തിരികെ -1;

  memset(&a, 0, വലുപ്പം(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#എങ്കിൽ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (ശൂന്യം)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    പൂർണ്ണസംഖ്യ exclusive = 1;
    എങ്കിൽ(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (അക്ഷരം *)&exclusive, (curl_socklen_t)വലുപ്പം(exclusive)) == -1)
      പോകുക error;
  }
#endif
#അല്ലെങ്കിൽ
  എങ്കിൽ(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (അക്ഷരം *)&reuse, (curl_socklen_t)വലുപ്പം(reuse)) == -1)
    പോകുക error;
#endif
  എങ്കിൽ(bind(listener, &a.addr, വലുപ്പം(a.inaddr)) == -1)
    പോകുക error;
  എങ്കിൽ(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (പൂർണ്ണസംഖ്യ)വലുപ്പം(a.inaddr))
    പോകുക error;
  എങ്കിൽ(listen(listener, 1) == -1)
    പോകുക error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  എങ്കിൽ(socks[0] == CURL_SOCKET_BAD)
    പോകുക error;
  എങ്കിൽ(connect(socks[0], &a.addr, വലുപ്പം(a.inaddr)) == -1)
    പോകുക error;

  /* use non-blocking accept to make sure we do not block forever */
  എങ്കിൽ(curlx_nonblock(listener, TRUE) < 0)
    പോകുക error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (ശൂന്യം)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ശൂന്യ, ശൂന്യ);
  എങ്കിൽ(socks[1] == CURL_SOCKET_BAD)
    പോകുക error;
  അല്ലെങ്കിൽ {
    ഘടന curltime start = curlx_now();
    അക്ഷരം rnd[9];
    അക്ഷരം check[വലുപ്പം(rnd)];
    അക്ഷരം *p = &check[0];
    size_t s = വലുപ്പം(check);

    എങ്കിൽ(Curl_rand(ശൂന്യ, (unsigned അക്ഷരം *)rnd, വലുപ്പം(rnd)))
      പോകുക error;

    /* write data to the socket */
    swrite(socks[0], rnd, വലുപ്പം(rnd));
    /* verify that we read the correct data */
    ചെയ്യുക {
      ssize_t nread;

      /* Do not block forever */
      എങ്കിൽ(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        പോകുക error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (ശൂന്യം)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      എങ്കിൽ(!nread) { /* unexpected close before getting complete `check` */
        പോകുക error;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(nread == -1) { /* error or close */
        പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
        എങ്കിൽ(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          തുടരുക;
        }
        പോകുക error;
      }
      s -= nread;
      എങ്കിൽ(s) {
        p += nread;
        തുടരുക;
      }
      എങ്കിൽ(memcmp(rnd, check, വലുപ്പം(check)))
        പോകുക error;
      നിർത്തുക;
    } ആകുമ്പോൾ(1);
  }

  എങ്കിൽ(nonblocking)
    എങ്കിൽ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      പോകുക error;
#ifdef USE_SO_NOSIGPIPE
  എങ്കിൽ(Curl_sock_nosigpipe(socks[1]) < 0)
    പോകുക error;
#endif
  sclose(listener);
  തിരികെ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  തിരികെ -1;
}

#endif /* choose implementation */

പൂർണ്ണസംഖ്യ Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  തിരികെ wakeup_eventfd(socks, nonblocking);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_PIPE)
  തിരികെ wakeup_pipe(socks, nonblocking);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SOCKETPAIR)
  തിരികെ wakeup_socketpair(socks, nonblocking);
#അല്ലെങ്കിൽ
  തിരികെ wakeup_inet(socks, nonblocking);
#endif
}

#എങ്കിൽ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        ലിഖ
#define wakeup_read         പഠ
#define wakeup_close        close

#അല്ലെങ്കിൽ /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

പൂർണ്ണസംഖ്യ Curl_wakeup_signal(curl_socket_t socks[2])
{
  പൂർണ്ണസംഖ്യ sockerr = 0;
#ifdef USE_EVENTFD
  സ്ഥിരം uint64_t buf[1] = { 1 };
#അല്ലെങ്കിൽ
  സ്ഥിരം അക്ഷരം buf[1] = { 1 };
#endif

  ആകുമ്പോൾ(1) {
    sockerr = 0;
    എങ്കിൽ(wakeup_write(socks[1], buf, വലുപ്പം(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      എങ്കിൽ(sockerr == SOCKEINTR)
        തുടരുക;
#endif
      എങ്കിൽ(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    നിർത്തുക;
  }
  തിരികെ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  അക്ഷരം buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  ചെയ്യുക {
    rc = wakeup_read(socks[0], buf, വലുപ്പം(buf));
    എങ്കിൽ(!rc)
      നിർത്തുക;
    അല്ലെങ്കിൽ എങ്കിൽ(rc < 0) {
      പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      എങ്കിൽ(sockerr == SOCKEINTR)
        തുടരുക;
#endif
      എങ്കിൽ(SOCK_EAGAIN(sockerr))
        നിർത്തുക;
      result = CURLE_READ_ERROR;
      നിർത്തുക;
    }
  } ആകുമ്പോൾ(all);
  തിരികെ result;
}

ശൂന്യം Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  എങ്കിൽ(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  എങ്കിൽ(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
