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
  ᮚᮦᮔ᮪(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ᮝᮜᮤ -1;
  }
  socks[0] = socks[1] = efd;
  ᮝᮜᮤ 0;
}

#elif defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#include <fcntl.h>
#endif

static puur_naa_nka wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  ᮝᮜᮤ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ᮜᮤᮚ
#ᮚᮦᮔ᮪ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  ᮚᮦᮔ᮪(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    ᮝᮜᮤ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ᮜᮤᮚ
#endif
  ᮚᮦᮔ᮪(pipe(socks))
    ᮝᮜᮤ -1;
#ifdef HAVE_FCNTL
  ᮚᮦᮔ᮪(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ᮝᮜᮤ -1;
  }
#endif
  ᮚᮦᮔ᮪(nonblocking) {
    ᮚᮦᮔ᮪(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ᮝᮜᮤ -1;
    }
  }
  ᮝᮜᮤ 0;
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
  ᮚᮦᮔ᮪(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  ᮚᮦᮔ᮪(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    ᮝᮜᮤ -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  ᮚᮦᮔ᮪(nonblocking) {
    ᮚᮦᮔ᮪(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ᮝᮜᮤ -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  ᮚᮦᮔ᮪(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ᮝᮜᮤ -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  ᮝᮜᮤ 0;
}

#ᮜᮤᮚ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

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
  ᮚᮦᮔ᮪(listener == CURL_SOCKET_BAD)
    ᮝᮜᮤ -1;

  memset(&a, 0, parimaa_na(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ᮚᮦᮔ᮪ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (void)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    puur_naa_nka exclusive = 1;
    ᮚᮦᮔ᮪(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (var_na *)&exclusive, (curl_socklen_t)parimaa_na(exclusive)) == -1)
      goto error;
  }
#endif
#ᮜᮤᮚ
  ᮚᮦᮔ᮪(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (var_na *)&reuse, (curl_socklen_t)parimaa_na(reuse)) == -1)
    goto error;
#endif
  ᮚᮦᮔ᮪(bind(listener, &a.addr, parimaa_na(a.inaddr)) == -1)
    goto error;
  ᮚᮦᮔ᮪(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (puur_naa_nka)parimaa_na(a.inaddr))
    goto error;
  ᮚᮦᮔ᮪(listen(listener, 1) == -1)
    goto error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ᮚᮦᮔ᮪(socks[0] == CURL_SOCKET_BAD)
    goto error;
  ᮚᮦᮔ᮪(connect(socks[0], &a.addr, parimaa_na(a.inaddr)) == -1)
    goto error;

  /* use non-blocking accept to make sure we do not block forever */
  ᮚᮦᮔ᮪(curlx_nonblock(listener, TRUE) < 0)
    goto error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (void)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, NULL, NULL);
  ᮚᮦᮔ᮪(socks[1] == CURL_SOCKET_BAD)
    goto error;
  ᮜᮤᮚ {
    sa_nracanaa curltime start = curlx_now();
    var_na rnd[9];
    var_na check[parimaa_na(rnd)];
    var_na *p = &check[0];
    size_t s = parimaa_na(check);

    ᮚᮦᮔ᮪(Curl_rand(NULL, (unsigned var_na *)rnd, parimaa_na(rnd)))
      goto error;

    /* write data to the socket */
    swrite(socks[0], rnd, parimaa_na(rnd));
    /* verify that we read the correct data */
    do {
      ssize_t nread;

      /* Do not block forever */
      ᮚᮦᮔ᮪(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        goto error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (void)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ᮚᮦᮔ᮪(!nread) { /* unexpected close before getting complete `check` */
        goto error;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(nread == -1) { /* error or close */
        puur_naa_nka sockerr = SOCKERRNO;
        ᮚᮦᮔ᮪(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          continue;
        }
        goto error;
      }
      s -= nread;
      ᮚᮦᮔ᮪(s) {
        p += nread;
        continue;
      }
      ᮚᮦᮔ᮪(memcmp(rnd, check, parimaa_na(check)))
        goto error;
      bha_nga;
    } ᮞᮨᮓᮨᮜ(1);
  }

  ᮚᮦᮔ᮪(nonblocking)
    ᮚᮦᮔ᮪(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      goto error;
#ifdef USE_SO_NOSIGPIPE
  ᮚᮦᮔ᮪(Curl_sock_nosigpipe(socks[1]) < 0)
    goto error;
#endif
  sclose(listener);
  ᮝᮜᮤ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  ᮝᮜᮤ -1;
}

#endif /* choose implementation */

puur_naa_nka Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  ᮝᮜᮤ wakeup_eventfd(socks, nonblocking);
#elif defined(HAVE_PIPE)
  ᮝᮜᮤ wakeup_pipe(socks, nonblocking);
#elif defined(HAVE_SOCKETPAIR)
  ᮝᮜᮤ wakeup_socketpair(socks, nonblocking);
#ᮜᮤᮚ
  ᮝᮜᮤ wakeup_inet(socks, nonblocking);
#endif
}

#ᮚᮦᮔ᮪ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        write
#define wakeup_read         read
#define wakeup_close        close

#ᮜᮤᮚ /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

puur_naa_nka Curl_wakeup_signal(curl_socket_t socks[2])
{
  puur_naa_nka sockerr = 0;
#ifdef USE_EVENTFD
  const uint64_t buf[1] = { 1 };
#ᮜᮤᮚ
  const var_na buf[1] = { 1 };
#endif

  ᮞᮨᮓᮨᮜ(1) {
    sockerr = 0;
    ᮚᮦᮔ᮪(wakeup_write(socks[1], buf, parimaa_na(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ᮚᮦᮔ᮪(sockerr == SOCKEINTR)
        continue;
#endif
      ᮚᮦᮔ᮪(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    bha_nga;
  }
  ᮝᮜᮤ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  var_na buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  do {
    rc = wakeup_read(socks[0], buf, parimaa_na(buf));
    ᮚᮦᮔ᮪(!rc)
      bha_nga;
    ᮜᮤᮚ ᮚᮦᮔ᮪(rc < 0) {
      puur_naa_nka sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ᮚᮦᮔ᮪(sockerr == SOCKEINTR)
        continue;
#endif
      ᮚᮦᮔ᮪(SOCK_EAGAIN(sockerr))
        bha_nga;
      result = CURLE_READ_ERROR;
      bha_nga;
    }
  } ᮞᮨᮓᮨᮜ(all);
  ᮝᮜᮤ result;
}

void Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  ᮚᮦᮔ᮪(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  ᮚᮦᮔ᮪(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
