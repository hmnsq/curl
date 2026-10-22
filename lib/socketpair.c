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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "socketpair.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#અન્તર્ભાવ <sys/eventfd.h>

સ્થિર પૂર્ણાંક wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  પૂર્ણાંક efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  જો(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    પાછા -1;
  }
  socks[0] = socks[1] = efd;
  પાછા 0;
}

#અથવાજો defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#અન્તર્ભાવ <fcntl.h>
#endif

સ્થિર પૂર્ણાંક wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  પાછા pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#નહીંતર
#જો defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  જો(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    પાછા pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  નહીંતર
#endif
  જો(pipe(socks))
    પાછા -1;
#ifdef HAVE_FCNTL
  જો(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    પાછા -1;
  }
#endif
  જો(nonblocking) {
    જો(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      પાછા -1;
    }
  }
  પાછા 0;
#endif /* HAVE_PIPE2 */
}

#અથવાજો defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

સ્થિર પૂર્ણાંક wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  પૂર્ણાંક type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  જો(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  જો(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    પાછા -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  જો(nonblocking) {
    જો(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      પાછા -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  જો(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    પાછા -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  પાછા 0;
}

#નહીંતર /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#અન્તર્ભાવ "select.h"   /* for Curl_poll */

સ્થિર પૂર્ણાંક wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  સંઘ {
    સંરચના sockaddr_in inaddr;
    સંરચના sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = કદ(a.inaddr);
  પૂર્ણાંક reuse = 1;
  સંરચના pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  જો(listener == CURL_SOCKET_BAD)
    પાછા -1;

  memset(&a, 0, કદ(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#જો defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (શૂન્ય)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    પૂર્ણાંક exclusive = 1;
    જો(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (અક્ષર *)&exclusive, (curl_socklen_t)કદ(exclusive)) == -1)
      જાઓ error;
  }
#endif
#નહીંતર
  જો(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (અક્ષર *)&reuse, (curl_socklen_t)કદ(reuse)) == -1)
    જાઓ error;
#endif
  જો(bind(listener, &a.addr, કદ(a.inaddr)) == -1)
    જાઓ error;
  જો(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (પૂર્ણાંક)કદ(a.inaddr))
    જાઓ error;
  જો(listen(listener, 1) == -1)
    જાઓ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  જો(socks[0] == CURL_SOCKET_BAD)
    જાઓ error;
  જો(connect(socks[0], &a.addr, કદ(a.inaddr)) == -1)
    જાઓ error;

  /* use non-blocking accept to make sure we do not block forever */
  જો(curlx_nonblock(listener, TRUE) < 0)
    જાઓ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (શૂન્ય)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ખાલી, ખાલી);
  જો(socks[1] == CURL_SOCKET_BAD)
    જાઓ error;
  નહીંતર {
    સંરચના curltime start = curlx_now();
    અક્ષર rnd[9];
    અક્ષર check[કદ(rnd)];
    અક્ષર *p = &check[0];
    size_t s = કદ(check);

    જો(Curl_rand(ખાલી, (unsigned અક્ષર *)rnd, કદ(rnd)))
      જાઓ error;

    /* write data to the socket */
    swrite(socks[0], rnd, કદ(rnd));
    /* verify that we read the correct data */
    કરો {
      ssize_t nread;

      /* Do not block forever */
      જો(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        જાઓ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (શૂન્ય)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      જો(!nread) { /* unexpected close before getting complete `check` */
        જાઓ error;
      }
      નહીંતર જો(nread == -1) { /* error or close */
        પૂર્ણાંક sockerr = SOCKERRNO;
        જો(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          ચાલુ;
        }
        જાઓ error;
      }
      s -= nread;
      જો(s) {
        p += nread;
        ચાલુ;
      }
      જો(memcmp(rnd, check, કદ(check)))
        જાઓ error;
      તોડો;
    } જ્યારે(1);
  }

  જો(nonblocking)
    જો(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      જાઓ error;
#ifdef USE_SO_NOSIGPIPE
  જો(Curl_sock_nosigpipe(socks[1]) < 0)
    જાઓ error;
#endif
  sclose(listener);
  પાછા 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  પાછા -1;
}

#endif /* choose implementation */

પૂર્ણાંક Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  પાછા wakeup_eventfd(socks, nonblocking);
#અથવાજો defined(HAVE_PIPE)
  પાછા wakeup_pipe(socks, nonblocking);
#અથવાજો defined(HAVE_SOCKETPAIR)
  પાછા wakeup_socketpair(socks, nonblocking);
#નહીંતર
  પાછા wakeup_inet(socks, nonblocking);
#endif
}

#જો defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        લિખ
#define wakeup_read         પઠ
#define wakeup_close        close

#નહીંતર /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

પૂર્ણાંક Curl_wakeup_signal(curl_socket_t socks[2])
{
  પૂર્ણાંક sockerr = 0;
#ifdef USE_EVENTFD
  અચલ uint64_t buf[1] = { 1 };
#નહીંતર
  અચલ અક્ષર buf[1] = { 1 };
#endif

  જ્યારે(1) {
    sockerr = 0;
    જો(wakeup_write(socks[1], buf, કદ(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      જો(sockerr == SOCKEINTR)
        ચાલુ;
#endif
      જો(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    તોડો;
  }
  પાછા sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  અક્ષર buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  કરો {
    rc = wakeup_read(socks[0], buf, કદ(buf));
    જો(!rc)
      તોડો;
    નહીંતર જો(rc < 0) {
      પૂર્ણાંક sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      જો(sockerr == SOCKEINTR)
        ચાલુ;
#endif
      જો(SOCK_EAGAIN(sockerr))
        તોડો;
      result = CURLE_READ_ERROR;
      તોડો;
    }
  } જ્યારે(all);
  પાછા result;
}

શૂન્ય Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  જો(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  જો(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
