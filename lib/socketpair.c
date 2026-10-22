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

static puur_naa_nka wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  puur_naa_nka efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  ᬬᬾᬦ᭄(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ᬯᬮᬶ -1;
  }
  socks[0] = socks[1] = efd;
  ᬯᬮᬶ 0;
}

#elif defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#include <fcntl.h>
#endif

static puur_naa_nka wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  ᬯᬮᬶ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ᬮᬶᬬ
#ᬬᬾᬦ᭄ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  ᬬᬾᬦ᭄(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    ᬯᬮᬶ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ᬮᬶᬬ
#endif
  ᬬᬾᬦ᭄(pipe(socks))
    ᬯᬮᬶ -1;
#ifdef HAVE_FCNTL
  ᬬᬾᬦ᭄(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ᬯᬮᬶ -1;
  }
#endif
  ᬬᬾᬦ᭄(nonblocking) {
    ᬬᬾᬦ᭄(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ᬯᬮᬶ -1;
    }
  }
  ᬯᬮᬶ 0;
#endif /* HAVE_PIPE2 */
}

#elif defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

static puur_naa_nka wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  puur_naa_nka type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  ᬬᬾᬦ᭄(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  ᬬᬾᬦ᭄(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    ᬯᬮᬶ -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  ᬬᬾᬦ᭄(nonblocking) {
    ᬬᬾᬦ᭄(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ᬯᬮᬶ -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  ᬬᬾᬦ᭄(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ᬯᬮᬶ -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  ᬯᬮᬶ 0;
}

#ᬮᬶᬬ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

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

static puur_naa_nka wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  sa_ngha {
    sa_nracanaa sockaddr_in inaddr;
    sa_nracanaa sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = parimaa_na(a.inaddr);
  puur_naa_nka reuse = 1;
  sa_nracanaa pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ᬬᬾᬦ᭄(listener == CURL_SOCKET_BAD)
    ᬯᬮᬶ -1;

  memset(&a, 0, parimaa_na(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ᬬᬾᬦ᭄ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (void)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    puur_naa_nka exclusive = 1;
    ᬬᬾᬦ᭄(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (var_na *)&exclusive, (curl_socklen_t)parimaa_na(exclusive)) == -1)
      goto error;
  }
#endif
#ᬮᬶᬬ
  ᬬᬾᬦ᭄(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (var_na *)&reuse, (curl_socklen_t)parimaa_na(reuse)) == -1)
    goto error;
#endif
  ᬬᬾᬦ᭄(bind(listener, &a.addr, parimaa_na(a.inaddr)) == -1)
    goto error;
  ᬬᬾᬦ᭄(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (puur_naa_nka)parimaa_na(a.inaddr))
    goto error;
  ᬬᬾᬦ᭄(listen(listener, 1) == -1)
    goto error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ᬬᬾᬦ᭄(socks[0] == CURL_SOCKET_BAD)
    goto error;
  ᬬᬾᬦ᭄(connect(socks[0], &a.addr, parimaa_na(a.inaddr)) == -1)
    goto error;

  /* use non-blocking accept to make sure we do not block forever */
  ᬬᬾᬦ᭄(curlx_nonblock(listener, TRUE) < 0)
    goto error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (void)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, NULL, NULL);
  ᬬᬾᬦ᭄(socks[1] == CURL_SOCKET_BAD)
    goto error;
  ᬮᬶᬬ {
    sa_nracanaa curltime start = curlx_now();
    var_na rnd[9];
    var_na check[parimaa_na(rnd)];
    var_na *p = &check[0];
    size_t s = parimaa_na(check);

    ᬬᬾᬦ᭄(Curl_rand(NULL, (unsigned var_na *)rnd, parimaa_na(rnd)))
      goto error;

    /* write data to the socket */
    swrite(socks[0], rnd, parimaa_na(rnd));
    /* verify that we read the correct data */
    do {
      ssize_t nread;

      /* Do not block forever */
      ᬬᬾᬦ᭄(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        goto error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (void)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ᬬᬾᬦ᭄(!nread) { /* unexpected close before getting complete `check` */
        goto error;
      }
      ᬮᬶᬬ ᬬᬾᬦ᭄(nread == -1) { /* error or close */
        puur_naa_nka sockerr = SOCKERRNO;
        ᬬᬾᬦ᭄(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          continue;
        }
        goto error;
      }
      s -= nread;
      ᬬᬾᬦ᭄(s) {
        p += nread;
        continue;
      }
      ᬬᬾᬦ᭄(memcmp(rnd, check, parimaa_na(check)))
        goto error;
      bha_nga;
    } ᬲᭂᬤᭂᬮ(1);
  }

  ᬬᬾᬦ᭄(nonblocking)
    ᬬᬾᬦ᭄(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      goto error;
#ifdef USE_SO_NOSIGPIPE
  ᬬᬾᬦ᭄(Curl_sock_nosigpipe(socks[1]) < 0)
    goto error;
#endif
  sclose(listener);
  ᬯᬮᬶ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  ᬯᬮᬶ -1;
}

#endif /* choose implementation */

puur_naa_nka Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  ᬯᬮᬶ wakeup_eventfd(socks, nonblocking);
#elif defined(HAVE_PIPE)
  ᬯᬮᬶ wakeup_pipe(socks, nonblocking);
#elif defined(HAVE_SOCKETPAIR)
  ᬯᬮᬶ wakeup_socketpair(socks, nonblocking);
#ᬮᬶᬬ
  ᬯᬮᬶ wakeup_inet(socks, nonblocking);
#endif
}

#ᬬᬾᬦ᭄ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        write
#define wakeup_read         read
#define wakeup_close        close

#ᬮᬶᬬ /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

puur_naa_nka Curl_wakeup_signal(curl_socket_t socks[2])
{
  puur_naa_nka sockerr = 0;
#ifdef USE_EVENTFD
  const uint64_t buf[1] = { 1 };
#ᬮᬶᬬ
  const var_na buf[1] = { 1 };
#endif

  ᬲᭂᬤᭂᬮ(1) {
    sockerr = 0;
    ᬬᬾᬦ᭄(wakeup_write(socks[1], buf, parimaa_na(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ᬬᬾᬦ᭄(sockerr == SOCKEINTR)
        continue;
#endif
      ᬬᬾᬦ᭄(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    bha_nga;
  }
  ᬯᬮᬶ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  var_na buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  do {
    rc = wakeup_read(socks[0], buf, parimaa_na(buf));
    ᬬᬾᬦ᭄(!rc)
      bha_nga;
    ᬮᬶᬬ ᬬᬾᬦ᭄(rc < 0) {
      puur_naa_nka sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ᬬᬾᬦ᭄(sockerr == SOCKEINTR)
        continue;
#endif
      ᬬᬾᬦ᭄(SOCK_EAGAIN(sockerr))
        bha_nga;
      result = CURLE_READ_ERROR;
      bha_nga;
    }
  } ᬲᭂᬤᭂᬮ(all);
  ᬯᬮᬶ result;
}

void Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  ᬬᬾᬦ᭄(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  ᬬᬾᬦ᭄(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
