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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "socketpair.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#ଅନ୍ତର୍ଭାଵ <sys/eventfd.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  ଯଦି(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ଫେରନ୍ତୁ -1;
  }
  socks[0] = socks[1] = efd;
  ଫେରନ୍ତୁ 0;
}

#ନଚେତଯଦି defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  ଫେରନ୍ତୁ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ଅନ୍ୟଥା
#ଯଦି defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  ଯଦି(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    ଫେରନ୍ତୁ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ଅନ୍ୟଥା
#endif
  ଯଦି(pipe(socks))
    ଫେରନ୍ତୁ -1;
#ifdef HAVE_FCNTL
  ଯଦି(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ଫେରନ୍ତୁ -1;
  }
#endif
  ଯଦି(nonblocking) {
    ଯଦି(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ଫେରନ୍ତୁ -1;
    }
  }
  ଫେରନ୍ତୁ 0;
#endif /* HAVE_PIPE2 */
}

#ନଚେତଯଦି defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  ଯଦି(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  ଯଦି(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    ଫେରନ୍ତୁ -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  ଯଦି(nonblocking) {
    ଯଦି(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ଫେରନ୍ତୁ -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  ଯଦି(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ଫେରନ୍ତୁ -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  ଫେରନ୍ତୁ 0;
}

#ଅନ୍ୟଥା /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#ଅନ୍ତର୍ଭାଵ "select.h"   /* for Curl_poll */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  ସଂଘ {
    ସଂରଚନା sockaddr_in inaddr;
    ସଂରଚନା sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = ଆକାର(a.inaddr);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା reuse = 1;
  ସଂରଚନା pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ଯଦି(listener == CURL_SOCKET_BAD)
    ଫେରନ୍ତୁ -1;

  memset(&a, 0, ଆକାର(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ଯଦି defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (ଶୂନ୍ୟ)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା exclusive = 1;
    ଯଦି(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (ଅକ୍ଷର *)&exclusive, (curl_socklen_t)ଆକାର(exclusive)) == -1)
      ଯାଅ error;
  }
#endif
#ଅନ୍ୟଥା
  ଯଦି(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (ଅକ୍ଷର *)&reuse, (curl_socklen_t)ଆକାର(reuse)) == -1)
    ଯାଅ error;
#endif
  ଯଦି(bind(listener, &a.addr, ଆକାର(a.inaddr)) == -1)
    ଯାଅ error;
  ଯଦି(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(a.inaddr))
    ଯାଅ error;
  ଯଦି(listen(listener, 1) == -1)
    ଯାଅ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ଯଦି(socks[0] == CURL_SOCKET_BAD)
    ଯାଅ error;
  ଯଦି(connect(socks[0], &a.addr, ଆକାର(a.inaddr)) == -1)
    ଯାଅ error;

  /* use non-blocking accept to make sure we do not block forever */
  ଯଦି(curlx_nonblock(listener, TRUE) < 0)
    ଯାଅ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (ଶୂନ୍ୟ)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ଖାଲି, ଖାଲି);
  ଯଦି(socks[1] == CURL_SOCKET_BAD)
    ଯାଅ error;
  ଅନ୍ୟଥା {
    ସଂରଚନା curltime start = curlx_now();
    ଅକ୍ଷର rnd[9];
    ଅକ୍ଷର check[ଆକାର(rnd)];
    ଅକ୍ଷର *p = &check[0];
    size_t s = ଆକାର(check);

    ଯଦି(Curl_rand(ଖାଲି, (unsigned ଅକ୍ଷର *)rnd, ଆକାର(rnd)))
      ଯାଅ error;

    /* write data to the socket */
    swrite(socks[0], rnd, ଆକାର(rnd));
    /* verify that we read the correct data */
    କର {
      ssize_t nread;

      /* Do not block forever */
      ଯଦି(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        ଯାଅ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (ଶୂନ୍ୟ)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ଯଦି(!nread) { /* unexpected close before getting complete `check` */
        ଯାଅ error;
      }
      ଅନ୍ୟଥା ଯଦି(nread == -1) { /* error or close */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
        ଯଦି(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          ଜାରି;
        }
        ଯାଅ error;
      }
      s -= nread;
      ଯଦି(s) {
        p += nread;
        ଜାରି;
      }
      ଯଦି(memcmp(rnd, check, ଆକାର(check)))
        ଯାଅ error;
      ଭାଙ୍ଗ;
    } ଯେତେବେଳେ(1);
  }

  ଯଦି(nonblocking)
    ଯଦି(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      ଯାଅ error;
#ifdef USE_SO_NOSIGPIPE
  ଯଦି(Curl_sock_nosigpipe(socks[1]) < 0)
    ଯାଅ error;
#endif
  sclose(listener);
  ଫେରନ୍ତୁ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  ଫେରନ୍ତୁ -1;
}

#endif /* choose implementation */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  ଫେରନ୍ତୁ wakeup_eventfd(socks, nonblocking);
#ନଚେତଯଦି defined(HAVE_PIPE)
  ଫେରନ୍ତୁ wakeup_pipe(socks, nonblocking);
#ନଚେତଯଦି defined(HAVE_SOCKETPAIR)
  ଫେରନ୍ତୁ wakeup_socketpair(socks, nonblocking);
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ wakeup_inet(socks, nonblocking);
#endif
}

#ଯଦି defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        ଲିଖ
#define wakeup_read         ପଠ
#define wakeup_close        close

#ଅନ୍ୟଥା /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_wakeup_signal(curl_socket_t socks[2])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = 0;
#ifdef USE_EVENTFD
  ଅଚଳ uint64_t buf[1] = { 1 };
#ଅନ୍ୟଥା
  ଅଚଳ ଅକ୍ଷର buf[1] = { 1 };
#endif

  ଯେତେବେଳେ(1) {
    sockerr = 0;
    ଯଦି(wakeup_write(socks[1], buf, ଆକାର(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ଯଦି(sockerr == SOCKEINTR)
        ଜାରି;
#endif
      ଯଦି(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  ଅକ୍ଷର buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  କର {
    rc = wakeup_read(socks[0], buf, ଆକାର(buf));
    ଯଦି(!rc)
      ଭାଙ୍ଗ;
    ଅନ୍ୟଥା ଯଦି(rc < 0) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      ଯଦି(sockerr == SOCKEINTR)
        ଜାରି;
#endif
      ଯଦି(SOCK_EAGAIN(sockerr))
        ଭାଙ୍ଗ;
      result = CURLE_READ_ERROR;
      ଭାଙ୍ଗ;
    }
  } ଯେତେବେଳେ(all);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  ଯଦି(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  ଯଦି(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
