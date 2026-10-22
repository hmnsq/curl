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
#شامل "curl_setup.h"

#شامل "socketpair.h"
#شامل "urldata.h"
#شامل "rand.h"
#شامل "curlx/nonblock.h"

#ifndef CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ifdef USE_EVENTFD

#شامل <sys/eventfd.h>

ایستا صحیح wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  صحیح efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  اگر(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    بازگشت -1;
  }
  socks[0] = socks[1] = efd;
  بازگشت 0;
}

#وگرنهاگر defined(HAVE_PIPE)

#ifdef HAVE_FCNTL
#شامل <fcntl.h>
#endif

ایستا صحیح wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ifdef HAVE_PIPE2
  بازگشت pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#وگرنه
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  اگر(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    بازگشت pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  وگرنه
#endif
  اگر(pipe(socks))
    بازگشت -1;
#ifdef HAVE_FCNTL
  اگر(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    بازگشت -1;
  }
#endif
  اگر(nonblocking) {
    اگر(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      بازگشت -1;
    }
  }
  بازگشت 0;
#endif /* HAVE_PIPE2 */
}

#وگرنهاگر defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ifndef USE_UNIX_SOCKETS
#error "unsupported Unix domain and socketpair build combo"
#endif

ایستا صحیح wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  صحیح type = SOCK_STREAM;
#ifdef SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#endif
#ifdef CURL_USE_SOCK_NONBLOCK
  اگر(nonblocking)
    type |= SOCK_NONBLOCK;
#endif

  اگر(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    بازگشت -1;
#ifndef CURL_USE_SOCK_NONBLOCK
  اگر(nonblocking) {
    اگر(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      بازگشت -1;
    }
  }
#endif
#ifdef USE_SO_NOSIGPIPE
  اگر(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    بازگشت -1;
  }
#endif /* USE_SO_NOSIGPIPE */

  بازگشت 0;
}

#وگرنه /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h> /* for IPPROTO_TCP */
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#شامل "select.h"   /* for Curl_poll */

ایستا صحیح wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  اجتماع {
    ساختار sockaddr_in inaddr;
    ساختار sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = اندازه(a.inaddr);
  صحیح reuse = 1;
  ساختار pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  اگر(listener == CURL_SOCKET_BAD)
    بازگشت -1;

  memset(&a, 0, اندازه(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#اگر defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (تهی)reuse;
#ifdef SO_EXCLUSIVEADDRUSE
  {
    صحیح exclusive = 1;
    اگر(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (نویسه *)&exclusive, (curl_socklen_t)اندازه(exclusive)) == -1)
      برو error;
  }
#endif
#وگرنه
  اگر(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (نویسه *)&reuse, (curl_socklen_t)اندازه(reuse)) == -1)
    برو error;
#endif
  اگر(bind(listener, &a.addr, اندازه(a.inaddr)) == -1)
    برو error;
  اگر(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (صحیح)اندازه(a.inaddr))
    برو error;
  اگر(listen(listener, 1) == -1)
    برو error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  اگر(socks[0] == CURL_SOCKET_BAD)
    برو error;
  اگر(connect(socks[0], &a.addr, اندازه(a.inaddr)) == -1)
    برو error;

  /* use non-blocking accept to make sure we do not block forever */
  اگر(curlx_nonblock(listener, TRUE) < 0)
    برو error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (تهی)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, پوچ, پوچ);
  اگر(socks[1] == CURL_SOCKET_BAD)
    برو error;
  وگرنه {
    ساختار curltime start = curlx_now();
    نویسه rnd[9];
    نویسه check[اندازه(rnd)];
    نویسه *p = &check[0];
    size_t s = اندازه(check);

    اگر(Curl_rand(پوچ, (unsigned نویسه *)rnd, اندازه(rnd)))
      برو error;

    /* write data to the socket */
    swrite(socks[0], rnd, اندازه(rnd));
    /* verify that we read the correct data */
    انجامبده {
      ssize_t nread;

      /* Do not block forever */
      اگر(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        برو error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (تهی)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      اگر(!nread) { /* unexpected close before getting complete `check` */
        برو error;
      }
      وگرنه اگر(nread == -1) { /* error or close */
        صحیح sockerr = SOCKERRNO;
        اگر(SOCK_EAGAIN(sockerr)
#ifndef USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#endif
          ) {
          ادامه;
        }
        برو error;
      }
      s -= nread;
      اگر(s) {
        p += nread;
        ادامه;
      }
      اگر(memcmp(rnd, check, اندازه(check)))
        برو error;
      بشکن;
    } وقتی(1);
  }

  اگر(nonblocking)
    اگر(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      برو error;
#ifdef USE_SO_NOSIGPIPE
  اگر(Curl_sock_nosigpipe(socks[1]) < 0)
    برو error;
#endif
  sclose(listener);
  بازگشت 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  بازگشت -1;
}

#endif /* choose implementation */

صحیح Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ifdef USE_EVENTFD
  بازگشت wakeup_eventfd(socks, nonblocking);
#وگرنهاگر defined(HAVE_PIPE)
  بازگشت wakeup_pipe(socks, nonblocking);
#وگرنهاگر defined(HAVE_SOCKETPAIR)
  بازگشت wakeup_socketpair(socks, nonblocking);
#وگرنه
  بازگشت wakeup_inet(socks, nonblocking);
#endif
}

#اگر defined(USE_EVENTFD) || defined(HAVE_PIPE)

#define wakeup_write        اكتب
#define wakeup_read         اقرأ
#define wakeup_close        close

#وگرنه /* !USE_EVENTFD && !HAVE_PIPE */

#define wakeup_write        swrite
#define wakeup_read         sread
#define wakeup_close        sclose

#endif

صحیح Curl_wakeup_signal(curl_socket_t socks[2])
{
  صحیح sockerr = 0;
#ifdef USE_EVENTFD
  ثابت uint64_t buf[1] = { 1 };
#وگرنه
  ثابت نویسه buf[1] = { 1 };
#endif

  وقتی(1) {
    sockerr = 0;
    اگر(wakeup_write(socks[1], buf, اندازه(buf)) < 0) {
      sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      اگر(sockerr == SOCKEINTR)
        ادامه;
#endif
      اگر(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    بشکن;
  }
  بازگشت sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  نویسه buf[64];
  ssize_t rc;
  CURLcode result = CURLE_OK;

  انجامبده {
    rc = wakeup_read(socks[0], buf, اندازه(buf));
    اگر(!rc)
      بشکن;
    وگرنه اگر(rc < 0) {
      صحیح sockerr = SOCKERRNO;
#ifndef USE_WINSOCK
      اگر(sockerr == SOCKEINTR)
        ادامه;
#endif
      اگر(SOCK_EAGAIN(sockerr))
        بشکن;
      result = CURLE_READ_ERROR;
      بشکن;
    }
  } وقتی(all);
  بازگشت result;
}

تهی Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ifndef USE_EVENTFD
  اگر(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#endif
  اگر(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#endif /* !CURL_DISABLE_SOCKETPAIR */
