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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "socketpair.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#அந்தர்பாவ <sys/eventfd.h>

நிலையான முழு wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  முழு efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  எனில்(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    திரும்பு -1;
  }
  socks[0] = socks[1] = efd;
  திரும்பு 0;
}

#அல்லதெனில் defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#அந்தர்பாவ <fcntl.h>
#endif

நிலையான முழு wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  திரும்பு pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#இல்லையெனில்
#எனில் defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  எனில்(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    திரும்பு pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  இல்லையெனில்
#endif
  எனில்(pipe(socks))
    திரும்பு -1;
#ifdef HAVE_FCNTL
  எனில்(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    திரும்பு -1;
  }
#endif
  எனில்(nonblocking) {
    எனில்(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      திரும்பு -1;
    }
  }
  திரும்பு 0;
#endif /* HAVE_PIPE2 */
}

#அல்லதெனில் defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

நிலையான முழு wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  முழு type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  எனில்(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  எனில்(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    திரும்பு -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  எனில்(nonblocking) {
    எனில்(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      திரும்பு -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  எனில்(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    திரும்பு -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  திரும்பு 0;
}

#இல்லையெனில் /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#அந்தர்பாவ "select.h"   /* for Curl_poll */

நிலையான முழு wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  ஒன்றிப்பு {
    கட்டமைப்பு sockaddr_in inaddr;
    கட்டமைப்பு sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = அளவு(a.inaddr);
  முழு reuse = 1;
  கட்டமைப்பு pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  எனில்(listener == CURL_SOCKET_BAD)
    திரும்பு -1;

  memset(&a, 0, அளவு(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#எனில் defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (வெறுமை)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    முழு exclusive = 1;
    எனில்(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (எழுத்து *)&exclusive, (curl_socklen_t)அளவு(exclusive)) == -1)
      செல் error;
  }
#endif
#இல்லையெனில்
  எனில்(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (எழுத்து *)&reuse, (curl_socklen_t)அளவு(reuse)) == -1)
    செல் error;
#endif
  எனில்(bind(listener, &a.addr, அளவு(a.inaddr)) == -1)
    செல் error;
  எனில்(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (முழு)அளவு(a.inaddr))
    செல் error;
  எனில்(listen(listener, 1) == -1)
    செல் error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  எனில்(socks[0] == CURL_SOCKET_BAD)
    செல் error;
  எனில்(connect(socks[0], &a.addr, அளவு(a.inaddr)) == -1)
    செல் error;

  /* use non-blocking accept to make sure we do not block forever */
  எனில்(curlx_nonblock(listener, TRUE) < 0)
    செல் error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (வெறுமை)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, வெற்று, வெற்று);
  எனில்(socks[1] == CURL_SOCKET_BAD)
    செல் error;
  இல்லையெனில் {
    கட்டமைப்பு curltime start = curlx_now();
    எழுத்து rnd[9];
    எழுத்து check[அளவு(rnd)];
    எழுத்து *p = &check[0];
    size_t s = அளவு(check);

    எனில்(Curl_rand(வெற்று, (unsigned எழுத்து *)rnd, அளவு(rnd)))
      செல் error;

    /* write data to the socket */
    swrite(socks[0], rnd, அளவு(rnd));
    /* verify that we read the correct data */
    செய் {
      ssize_t nread;

      /* Do not block forever */
      எனில்(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        செல் error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (வெறுமை)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      எனில்(!nread) { /* unexpected close before getting complete `check` */
        செல் error;
      }
      இல்லையெனில் எனில்(nread == -1) { /* error or close */
        முழு sockerr = SOCKERRNO;
        எனில்(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          தொடர்;
        }
        செல் error;
      }
      s -= nread;
      எனில்(s) {
        p += nread;
        தொடர்;
      }
      எனில்(memcmp(rnd, check, அளவு(check)))
        செல் error;
      நிறுத்து;
    } போது(1);
  }

  எனில்(nonblocking)
    எனில்(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      செல் error;
#ifdef USE_SO_NOSIGPIPE
  எனில்(Curl_sock_nosigpipe(socks[1]) < 0)
    செல் error;
#endif
  sclose(listener);
  திரும்பு 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  திரும்பு -1;
}

#endif /* choose implementation */

முழு Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  திரும்பு wakeup_eventfd(socks, nonblocking);
#அல்லதெனில் defined(HAVE_PIPE)
  திரும்பு wakeup_pipe(socks, nonblocking);
#அல்லதெனில் defined(HAVE_SOCKETPAIR)
  திரும்பு wakeup_socketpair(socks, nonblocking);
#இல்லையெனில்
  திரும்பு wakeup_inet(socks, nonblocking);
#endif
}

#எனில் defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        லிக
#define wakeup_read         பட
#define wakeup_close        close

#இல்லையெனில் /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

முழு Curl_wakeup_signal(curl_socket_t socks[2])
{
  முழு sockerr = 0;
#ifdef USE_EVENTFD
  மாறிலி uint64_t buf[1] = { 1 };
#இல்லையெனில்
  மாறிலி எழுத்து buf[1] = { 1 };
#endif

  போது(1) {
    sockerr = 0;
    எனில்(wakeup_write(socks[1], buf, அளவு(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      எனில்(sockerr == SOCKEINTR)
        தொடர்;
#endif
      எனில்(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    நிறுத்து;
  }
  திரும்பு sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  எழுத்து buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  செய் {
    rc = wakeup_read(socks[0], buf, அளவு(buf));
    எனில்(!rc)
      நிறுத்து;
    இல்லையெனில் எனில்(rc < 0) {
      முழு sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      எனில்(sockerr == SOCKEINTR)
        தொடர்;
#endif
      எனில்(SOCK_EAGAIN(sockerr))
        நிறுத்து;
      result = CURLE_READ_ERROR;
      நிறுத்து;
    }
  } போது(all);
  திரும்பு result;
}

வெறுமை Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  எனில்(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  எனில்(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
