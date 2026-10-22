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
#include "curl_setup.h"

#include "socketpair.h"
#include "urldata.h"
#include "rand.h"
#include "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#include <sys/eventfd.h>

static 정수 wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  정수 efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  만약(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    반환 -1;
  }
  socks[0] = socks[1] = efd;
  반환 0;
}

#elif defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#include <fcntl.h>
#endif

static 정수 wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  반환 pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#아니면
#만약 defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  만약(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    반환 pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  아니면
#endif
  만약(pipe(socks))
    반환 -1;
#ifdef HAVE_FCNTL
  만약(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    반환 -1;
  }
#endif
  만약(nonblocking) {
    만약(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      반환 -1;
    }
  }
  반환 0;
#endif /* HAVE_PIPE2 */
}

#elif defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

static 정수 wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  정수 type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  만약(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  만약(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    반환 -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  만약(nonblocking) {
    만약(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      반환 -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  만약(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    반환 -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  반환 0;
}

#아니면 /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#include "select.h"   /* for Curl_poll */

static 정수 wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  union {
    struct sockaddr_in inaddr;
    struct sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = sizeof(a.inaddr);
  정수 reuse = 1;
  struct pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  만약(listener == CURL_SOCKET_BAD)
    반환 -1;

  memset(&a, 0, sizeof(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#만약 defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (void)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    정수 exclusive = 1;
    만약(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (char *)&exclusive, (curl_socklen_t)sizeof(exclusive)) == -1)
      goto error;
  }
#endif
#아니면
  만약(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (char *)&reuse, (curl_socklen_t)sizeof(reuse)) == -1)
    goto error;
#endif
  만약(bind(listener, &a.addr, sizeof(a.inaddr)) == -1)
    goto error;
  만약(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (정수)sizeof(a.inaddr))
    goto error;
  만약(listen(listener, 1) == -1)
    goto error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  만약(socks[0] == CURL_SOCKET_BAD)
    goto error;
  만약(connect(socks[0], &a.addr, sizeof(a.inaddr)) == -1)
    goto error;

  /* use non-blocking accept to make sure we do not block forever */
  만약(curlx_nonblock(listener, TRUE) < 0)
    goto error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (void)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, NULL, NULL);
  만약(socks[1] == CURL_SOCKET_BAD)
    goto error;
  아니면 {
    struct curltime start = curlx_now();
    char rnd[9];
    char check[sizeof(rnd)];
    char *p = &check[0];
    size_t s = sizeof(check);

    만약(Curl_rand(NULL, (unsigned char *)rnd, sizeof(rnd)))
      goto error;

    /* write data to the socket */
    swrite(socks[0], rnd, sizeof(rnd));
    /* verify that we read the correct data */
    do {
      ssize_t nread;

      /* Do not block forever */
      만약(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        goto error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (void)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      만약(!nread) { /* unexpected close before getting complete `check` */
        goto error;
      }
      아니면 만약(nread == -1) { /* error or close */
        정수 sockerr = SOCKERRNO;
        만약(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          continue;
        }
        goto error;
      }
      s -= nread;
      만약(s) {
        p += nread;
        continue;
      }
      만약(memcmp(rnd, check, sizeof(check)))
        goto error;
      break;
    } 동안(1);
  }

  만약(nonblocking)
    만약(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      goto error;
#ifdef USE_SO_NOSIGPIPE
  만약(Curl_sock_nosigpipe(socks[1]) < 0)
    goto error;
#endif
  sclose(listener);
  반환 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  반환 -1;
}

#endif /* choose implementation */

정수 Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  반환 wakeup_eventfd(socks, nonblocking);
#elif defined(HAVE_PIPE)
  반환 wakeup_pipe(socks, nonblocking);
#elif defined(HAVE_SOCKETPAIR)
  반환 wakeup_socketpair(socks, nonblocking);
#아니면
  반환 wakeup_inet(socks, nonblocking);
#endif
}

#만약 defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        write
#define wakeup_read         read
#define wakeup_close        close

#아니면 /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

정수 Curl_wakeup_signal(curl_socket_t socks[2])
{
  정수 sockerr = 0;
#ifdef USE_EVENTFD
  const uint64_t buf[1] = { 1 };
#아니면
  const char buf[1] = { 1 };
#endif

  동안(1) {
    sockerr = 0;
    만약(wakeup_write(socks[1], buf, sizeof(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      만약(sockerr == SOCKEINTR)
        continue;
#endif
      만약(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    break;
  }
  반환 sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  char buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  do {
    rc = wakeup_read(socks[0], buf, sizeof(buf));
    만약(!rc)
      break;
    아니면 만약(rc < 0) {
      정수 sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      만약(sockerr == SOCKEINTR)
        continue;
#endif
      만약(SOCK_EAGAIN(sockerr))
        break;
      result = CURLE_READ_ERROR;
      break;
    }
  } 동안(all);
  반환 result;
}

void Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  만약(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  만약(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
