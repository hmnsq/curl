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

static số_nguyên wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  số_nguyên efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  nếu(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    trả_về -1;
  }
  socks[0] = socks[1] = efd;
  trả_về 0;
}

#elif defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#include <fcntl.h>
#endif

static số_nguyên wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  trả_về pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#không_thì
#nếu defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  nếu(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    trả_về pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  không_thì
#endif
  nếu(pipe(socks))
    trả_về -1;
#ifdef HAVE_FCNTL
  nếu(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    trả_về -1;
  }
#endif
  nếu(nonblocking) {
    nếu(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      trả_về -1;
    }
  }
  trả_về 0;
#endif /* HAVE_PIPE2 */
}

#elif defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

static số_nguyên wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  số_nguyên type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  nếu(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  nếu(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    trả_về -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  nếu(nonblocking) {
    nếu(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      trả_về -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  nếu(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    trả_về -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  trả_về 0;
}

#không_thì /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

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

static số_nguyên wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  union {
    struct sockaddr_in inaddr;
    struct sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = sizeof(a.inaddr);
  số_nguyên reuse = 1;
  struct pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  nếu(listener == CURL_SOCKET_BAD)
    trả_về -1;

  memset(&a, 0, sizeof(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#nếu defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (void)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    số_nguyên exclusive = 1;
    nếu(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (char *)&exclusive, (curl_socklen_t)sizeof(exclusive)) == -1)
      goto error;
  }
#endif
#không_thì
  nếu(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (char *)&reuse, (curl_socklen_t)sizeof(reuse)) == -1)
    goto error;
#endif
  nếu(bind(listener, &a.addr, sizeof(a.inaddr)) == -1)
    goto error;
  nếu(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (số_nguyên)sizeof(a.inaddr))
    goto error;
  nếu(listen(listener, 1) == -1)
    goto error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  nếu(socks[0] == CURL_SOCKET_BAD)
    goto error;
  nếu(connect(socks[0], &a.addr, sizeof(a.inaddr)) == -1)
    goto error;

  /* use non-blocking accept to make sure we do not block forever */
  nếu(curlx_nonblock(listener, TRUE) < 0)
    goto error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (void)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, NULL, NULL);
  nếu(socks[1] == CURL_SOCKET_BAD)
    goto error;
  không_thì {
    struct curltime start = curlx_now();
    char rnd[9];
    char check[sizeof(rnd)];
    char *p = &check[0];
    size_t s = sizeof(check);

    nếu(Curl_rand(NULL, (unsigned char *)rnd, sizeof(rnd)))
      goto error;

    /* write data to the socket */
    swrite(socks[0], rnd, sizeof(rnd));
    /* verify that we read the correct data */
    do {
      ssize_t nread;

      /* Do not block forever */
      nếu(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        goto error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (void)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      nếu(!nread) { /* unexpected close before getting complete `check` */
        goto error;
      }
      không_thì nếu(nread == -1) { /* error or close */
        số_nguyên sockerr = SOCKERRNO;
        nếu(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          continue;
        }
        goto error;
      }
      s -= nread;
      nếu(s) {
        p += nread;
        continue;
      }
      nếu(memcmp(rnd, check, sizeof(check)))
        goto error;
      break;
    } trong_khi(1);
  }

  nếu(nonblocking)
    nếu(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      goto error;
#ifdef USE_SO_NOSIGPIPE
  nếu(Curl_sock_nosigpipe(socks[1]) < 0)
    goto error;
#endif
  sclose(listener);
  trả_về 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  trả_về -1;
}

#endif /* choose implementation */

số_nguyên Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  trả_về wakeup_eventfd(socks, nonblocking);
#elif defined(HAVE_PIPE)
  trả_về wakeup_pipe(socks, nonblocking);
#elif defined(HAVE_SOCKETPAIR)
  trả_về wakeup_socketpair(socks, nonblocking);
#không_thì
  trả_về wakeup_inet(socks, nonblocking);
#endif
}

#nếu defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        write
#define wakeup_read         read
#define wakeup_close        close

#không_thì /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

số_nguyên Curl_wakeup_signal(curl_socket_t socks[2])
{
  số_nguyên sockerr = 0;
#ifdef USE_EVENTFD
  const uint64_t buf[1] = { 1 };
#không_thì
  const char buf[1] = { 1 };
#endif

  trong_khi(1) {
    sockerr = 0;
    nếu(wakeup_write(socks[1], buf, sizeof(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      nếu(sockerr == SOCKEINTR)
        continue;
#endif
      nếu(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    break;
  }
  trả_về sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  char buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  do {
    rc = wakeup_read(socks[0], buf, sizeof(buf));
    nếu(!rc)
      break;
    không_thì nếu(rc < 0) {
      số_nguyên sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      nếu(sockerr == SOCKEINTR)
        continue;
#endif
      nếu(SOCK_EAGAIN(sockerr))
        break;
      result = CURLE_READ_ERROR;
      break;
    }
  } trong_khi(all);
  trả_về result;
}

void Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  nếu(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  nếu(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
