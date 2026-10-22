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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "socketpair.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/nonblock.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EVENTFD

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/eventfd.h>

ਸਥਿਰ ਪੂਰਨ wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  ਪੂਰਨ efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  ਜੇ(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ਵਾਪਸ -1;
  }
  socks[0] = socks[1] = efd;
  ਵਾਪਸ 0;
}

#ਜਾਂਜੇ defined(HAVE_PIPE)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PIPE2
  ਵਾਪਸ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ਨਹੀਂਤਾਂ
#ਜੇ defined(__APPLE__) && defined(HAVE_BUILTIN_ਉਪਲਬਧ) && \
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
  ਜੇ(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    ਵਾਪਸ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਜੇ(pipe(socks))
    ਵਾਪਸ -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL
  ਜੇ(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ਵਾਪਸ -1;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(nonblocking) {
    ਜੇ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ਵਾਪਸ -1;
    }
  }
  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ /* HAVE_PIPE2 */
}

#ਜਾਂਜੇ defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
#ਗਲਤੀ "unsupported Unix domain and socketpair build combo"
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  ਪੂਰਨ type = SOCK_STREAM;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_USE_SOCK_NONBLOCK
  ਜੇ(nonblocking)
    type |= SOCK_NONBLOCK;
#ਜੇ_ਅੰਤ

  ਜੇ(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    ਵਾਪਸ -1;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_USE_SOCK_NONBLOCK
  ਜੇ(nonblocking) {
    ਜੇ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ਵਾਪਸ -1;
    }
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SO_NOSIGPIPE
  ਜੇ(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ਵਾਪਸ -1;
  }
#ਜੇ_ਅੰਤ /* USE_SO_NOSIGPIPE */

  ਵਾਪਸ 0;
}

#ਨਹੀਂਤਾਂ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h> /* for IPPROTO_TCP */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ INADDR_LOOPBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INADDR_LOOPBACK 0x7f000001
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"   /* for Curl_poll */

ਸਥਿਰ ਪੂਰਨ wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  ਸੰਘ {
    ਬਣਤਰ sockaddr_in inaddr;
    ਬਣਤਰ sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = ਆਕਾਰ(a.inaddr);
  ਪੂਰਨ reuse = 1;
  ਬਣਤਰ pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ਜੇ(listener == CURL_SOCKET_BAD)
    ਵਾਪਸ -1;

  memset(&a, 0, ਆਕਾਰ(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ਜੇ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (ਖਾਲੀ)reuse;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SO_EXCLUSIVEADDRUSE
  {
    ਪੂਰਨ exclusive = 1;
    ਜੇ(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (ਅੱਖਰ *)&exclusive, (curl_socklen_t)ਆਕਾਰ(exclusive)) == -1)
      ਜਾਓ error;
  }
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
  ਜੇ(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (ਅੱਖਰ *)&reuse, (curl_socklen_t)ਆਕਾਰ(reuse)) == -1)
    ਜਾਓ error;
#ਜੇ_ਅੰਤ
  ਜੇ(bind(listener, &a.addr, ਆਕਾਰ(a.inaddr)) == -1)
    ਜਾਓ error;
  ਜੇ(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (ਪੂਰਨ)ਆਕਾਰ(a.inaddr))
    ਜਾਓ error;
  ਜੇ(listen(listener, 1) == -1)
    ਜਾਓ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ਜੇ(socks[0] == CURL_SOCKET_BAD)
    ਜਾਓ error;
  ਜੇ(connect(socks[0], &a.addr, ਆਕਾਰ(a.inaddr)) == -1)
    ਜਾਓ error;

  /* use non-blocking accept to make sure we do not block forever */
  ਜੇ(curlx_nonblock(listener, TRUE) < 0)
    ਜਾਓ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (ਖਾਲੀ)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ਨਿਰਾ, ਨਿਰਾ);
  ਜੇ(socks[1] == CURL_SOCKET_BAD)
    ਜਾਓ error;
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ curltime ਸ਼ੁਰੂ = curlx_now();
    ਅੱਖਰ rnd[9];
    ਅੱਖਰ check[ਆਕਾਰ(rnd)];
    ਅੱਖਰ *p = &check[0];
    size_t s = ਆਕਾਰ(check);

    ਜੇ(Curl_rand(ਨਿਰਾ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)rnd, ਆਕਾਰ(rnd)))
      ਜਾਓ error;

    /* write data to the socket */
    swrite(socks[0], rnd, ਆਕਾਰ(rnd));
    /* verify that we read the correct data */
    ਕਰੋ {
      ssize_t nread;

      /* Do not block forever */
      ਜੇ(curlx_timediff_ms(curlx_now(), ਸ਼ੁਰੂ) > (60 * 1000))
        ਜਾਓ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (ਖਾਲੀ)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ਜੇ(!nread) { /* unexpected close before getting complete `check` */
        ਜਾਓ error;
      }
      ਨਹੀਂਤਾਂ ਜੇ(nread == -1) { /* error or close */
        ਪੂਰਨ sockerr = SOCKERRNO;
        ਜੇ(SOCK_EAGAIN(sockerr)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#ਜੇ_ਅੰਤ
          ) {
          ਜਾਰੀ;
        }
        ਜਾਓ error;
      }
      s -= nread;
      ਜੇ(s) {
        p += nread;
        ਜਾਰੀ;
      }
      ਜੇ(memcmp(rnd, check, ਆਕਾਰ(check)))
        ਜਾਓ error;
      ਰੋਕੋ;
    } ਜਦੋਂ(1);
  }

  ਜੇ(nonblocking)
    ਜੇ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      ਜਾਓ error;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SO_NOSIGPIPE
  ਜੇ(Curl_sock_nosigpipe(socks[1]) < 0)
    ਜਾਓ error;
#ਜੇ_ਅੰਤ
  sclose(listener);
  ਵਾਪਸ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  ਵਾਪਸ -1;
}

#ਜੇ_ਅੰਤ /* choose implementation */

ਪੂਰਨ Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EVENTFD
  ਵਾਪਸ wakeup_eventfd(socks, nonblocking);
#ਜਾਂਜੇ defined(HAVE_PIPE)
  ਵਾਪਸ wakeup_pipe(socks, nonblocking);
#ਜਾਂਜੇ defined(HAVE_SOCKETPAIR)
  ਵਾਪਸ wakeup_socketpair(socks, nonblocking);
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ wakeup_inet(socks, nonblocking);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wakeup_write        ਲਿਖ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wakeup_read         ਪਠ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wakeup_close        close

#ਨਹੀਂਤਾਂ /* !USE_EVENTFD && !HAVE_PIPE */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wakeup_write        swrite
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wakeup_read         sread
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wakeup_close        sclose

#ਜੇ_ਅੰਤ

ਪੂਰਨ Curl_wakeup_signal(curl_socket_t socks[2])
{
  ਪੂਰਨ sockerr = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EVENTFD
  ਅਚਲ uint64_t buf[1] = { 1 };
#ਨਹੀਂਤਾਂ
  ਅਚਲ ਅੱਖਰ buf[1] = { 1 };
#ਜੇ_ਅੰਤ

  ਜਦੋਂ(1) {
    sockerr = 0;
    ਜੇ(wakeup_write(socks[1], buf, ਆਕਾਰ(buf)) < 0) {
      sockerr = SOCKERRNO;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
      ਜੇ(sockerr == SOCKEINTR)
        ਜਾਰੀ;
#ਜੇ_ਅੰਤ
      ਜੇ(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    ਰੋਕੋ;
  }
  ਵਾਪਸ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  ਅੱਖਰ buf[64];
  ssize_t rc;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਕਰੋ {
    rc = wakeup_read(socks[0], buf, ਆਕਾਰ(buf));
    ਜੇ(!rc)
      ਰੋਕੋ;
    ਨਹੀਂਤਾਂ ਜੇ(rc < 0) {
      ਪੂਰਨ sockerr = SOCKERRNO;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
      ਜੇ(sockerr == SOCKEINTR)
        ਜਾਰੀ;
#ਜੇ_ਅੰਤ
      ਜੇ(SOCK_EAGAIN(sockerr))
        ਰੋਕੋ;
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਰੋਕੋ;
    }
  } ਜਦੋਂ(all);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_EVENTFD
  ਜੇ(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#ਜੇ_ਅੰਤ
  ਜੇ(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_SOCKETPAIR */
