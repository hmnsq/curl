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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "socketpair.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#অন্তর্ভাব <sys/eventfd.h>

স্থির পূর্ণাঙ্ক wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  পূর্ণাঙ্ক efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  যদি(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    প্রত্যাগম -1;
  }
  socks[0] = socks[1] = efd;
  প্রত্যাগম 0;
}

#নোচেদ্যদি defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#অন্তর্ভাব <fcntl.h>
#endif

স্থির পূর্ণাঙ্ক wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  প্রত্যাগম pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#অন্যথা
#যদি defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  যদি(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    প্রত্যাগম pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  অন্যথা
#endif
  যদি(pipe(socks))
    প্রত্যাগম -1;
#ifdef HAVE_FCNTL
  যদি(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    প্রত্যাগম -1;
  }
#endif
  যদি(nonblocking) {
    যদি(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      প্রত্যাগম -1;
    }
  }
  প্রত্যাগম 0;
#endif /* HAVE_PIPE2 */
}

#নোচেদ্যদি defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

স্থির পূর্ণাঙ্ক wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  পূর্ণাঙ্ক type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  যদি(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  যদি(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    প্রত্যাগম -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  যদি(nonblocking) {
    যদি(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      প্রত্যাগম -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  যদি(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    প্রত্যাগম -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  প্রত্যাগম 0;
}

#অন্যথা /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#অন্তর্ভাব "select.h"   /* for Curl_poll */

স্থির পূর্ণাঙ্ক wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  সঙ্ঘ {
    সংরচনা sockaddr_in inaddr;
    সংরচনা sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = পরিমাণ(a.inaddr);
  পূর্ণাঙ্ক reuse = 1;
  সংরচনা pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  যদি(listener == CURL_SOCKET_BAD)
    প্রত্যাগম -1;

  memset(&a, 0, পরিমাণ(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#যদি defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (শূন্য)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    পূর্ণাঙ্ক exclusive = 1;
    যদি(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (বর্ণ *)&exclusive, (curl_socklen_t)পরিমাণ(exclusive)) == -1)
      গচ্ছ error;
  }
#endif
#অন্যথা
  যদি(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (বর্ণ *)&reuse, (curl_socklen_t)পরিমাণ(reuse)) == -1)
    গচ্ছ error;
#endif
  যদি(bind(listener, &a.addr, পরিমাণ(a.inaddr)) == -1)
    গচ্ছ error;
  যদি(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (পূর্ণাঙ্ক)পরিমাণ(a.inaddr))
    গচ্ছ error;
  যদি(listen(listener, 1) == -1)
    গচ্ছ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  যদি(socks[0] == CURL_SOCKET_BAD)
    গচ্ছ error;
  যদি(connect(socks[0], &a.addr, পরিমাণ(a.inaddr)) == -1)
    গচ্ছ error;

  /* use non-blocking accept to make sure we do not block forever */
  যদি(curlx_nonblock(listener, TRUE) < 0)
    গচ্ছ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (শূন্য)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, রিক্ত, রিক্ত);
  যদি(socks[1] == CURL_SOCKET_BAD)
    গচ্ছ error;
  অন্যথা {
    সংরচনা curltime start = curlx_now();
    বর্ণ rnd[9];
    বর্ণ check[পরিমাণ(rnd)];
    বর্ণ *p = &check[0];
    size_t s = পরিমাণ(check);

    যদি(Curl_rand(রিক্ত, (unsigned বর্ণ *)rnd, পরিমাণ(rnd)))
      গচ্ছ error;

    /* write data to the socket */
    swrite(socks[0], rnd, পরিমাণ(rnd));
    /* verify that we read the correct data */
    কুরু {
      ssize_t nread;

      /* Do not block forever */
      যদি(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        গচ্ছ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (শূন্য)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      যদি(!nread) { /* unexpected close before getting complete `check` */
        গচ্ছ error;
      }
      অন্যথা যদি(nread == -1) { /* error or close */
        পূর্ণাঙ্ক sockerr = SOCKERRNO;
        যদি(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          অনুবর্ত;
        }
        গচ্ছ error;
      }
      s -= nread;
      যদি(s) {
        p += nread;
        অনুবর্ত;
      }
      যদি(memcmp(rnd, check, পরিমাণ(check)))
        গচ্ছ error;
      ভঙ্গ;
    } যাবত্(1);
  }

  যদি(nonblocking)
    যদি(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      গচ্ছ error;
#ifdef USE_SO_NOSIGPIPE
  যদি(Curl_sock_nosigpipe(socks[1]) < 0)
    গচ্ছ error;
#endif
  sclose(listener);
  প্রত্যাগম 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  প্রত্যাগম -1;
}

#endif /* choose implementation */

পূর্ণাঙ্ক Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  প্রত্যাগম wakeup_eventfd(socks, nonblocking);
#নোচেদ্যদি defined(HAVE_PIPE)
  প্রত্যাগম wakeup_pipe(socks, nonblocking);
#নোচেদ্যদি defined(HAVE_SOCKETPAIR)
  প্রত্যাগম wakeup_socketpair(socks, nonblocking);
#অন্যথা
  প্রত্যাগম wakeup_inet(socks, nonblocking);
#endif
}

#যদি defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        লিখ
#define wakeup_read         পঠ
#define wakeup_close        close

#অন্যথা /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

পূর্ণাঙ্ক Curl_wakeup_signal(curl_socket_t socks[2])
{
  পূর্ণাঙ্ক sockerr = 0;
#ifdef USE_EVENTFD
  ধ্রুব uint64_t buf[1] = { 1 };
#অন্যথা
  ধ্রুব বর্ণ buf[1] = { 1 };
#endif

  যাবত্(1) {
    sockerr = 0;
    যদি(wakeup_write(socks[1], buf, পরিমাণ(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      যদি(sockerr == SOCKEINTR)
        অনুবর্ত;
#endif
      যদি(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    ভঙ্গ;
  }
  প্রত্যাগম sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  বর্ণ buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  কুরু {
    rc = wakeup_read(socks[0], buf, পরিমাণ(buf));
    যদি(!rc)
      ভঙ্গ;
    অন্যথা যদি(rc < 0) {
      পূর্ণাঙ্ক sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      যদি(sockerr == SOCKEINTR)
        অনুবর্ত;
#endif
      যদি(SOCK_EAGAIN(sockerr))
        ভঙ্গ;
      result = CURLE_READ_ERROR;
      ভঙ্গ;
    }
  } যাবত্(all);
  প্রত্যাগম result;
}

শূন্য Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  যদি(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  যদি(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
