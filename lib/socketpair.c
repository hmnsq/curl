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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "socketpair.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EVENTFD

#ಅನ್ತರ್ಭಾವ <sys/eventfd.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  ಪೂರ್ಣಾಂಕ efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  ಆದರೆ(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ಹಿಂತಿರುಗಿ -1;
  }
  socks[0] = socks[1] = efd;
  ಹಿಂತಿರುಗಿ 0;
}

#ಅಥವಾಆದರೆ defined(HAVE_PIPE)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PIPE2
  ಹಿಂತಿರುಗಿ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ defined(__APPLE__) && defined(HAVE_BUILTIN_ಲಭ್ಯ) && \
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
  ಆದರೆ(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    ಹಿಂತಿರುಗಿ pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(pipe(socks))
    ಹಿಂತಿರುಗಿ -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL
  ಆದರೆ(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ಹಿಂತಿರುಗಿ -1;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(nonblocking) {
    ಆದರೆ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ಹಿಂತಿರುಗಿ -1;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_PIPE2 */
}

#ಅಥವಾಆದರೆ defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_UNIX_SOCKETS
#ದೋಷ "unsupported Unix domain and socketpair build combo"
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  ಪೂರ್ಣಾಂಕ type = SOCK_STREAM;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_USE_SOCK_NONBLOCK
  ಆದರೆ(nonblocking)
    type |= SOCK_NONBLOCK;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    ಹಿಂತಿರುಗಿ -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_USE_SOCK_NONBLOCK
  ಆದರೆ(nonblocking) {
    ಆದರೆ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      ಹಿಂತಿರುಗಿ -1;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SO_NOSIGPIPE
  ಆದರೆ(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    ಹಿಂತಿರುಗಿ -1;
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_SO_NOSIGPIPE */

  ಹಿಂತಿರುಗಿ 0;
}

#ಇಲ್ಲದಿದ್ದರೆ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* for IPPROTO_TCP */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ INADDR_LOOPBACK
#ವ್ಯಾಖ್ಯಾನಿಸು INADDR_LOOPBACK 0x7f000001
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "select.h"   /* for Curl_poll */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  ಒಕ್ಕೂಟ {
    ರಚನೆ sockaddr_in inaddr;
    ರಚನೆ sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = ಗಾತ್ರ(a.inaddr);
  ಪೂರ್ಣಾಂಕ reuse = 1;
  ರಚನೆ pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ಆದರೆ(listener == CURL_SOCKET_BAD)
    ಹಿಂತಿರುಗಿ -1;

  memset(&a, 0, ಗಾತ್ರ(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ಆದರೆ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (ಶೂನ್ಯ)reuse;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SO_EXCLUSIVEADDRUSE
  {
    ಪೂರ್ಣಾಂಕ exclusive = 1;
    ಆದರೆ(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (ಅಕ್ಷರ *)&exclusive, (curl_socklen_t)ಗಾತ್ರ(exclusive)) == -1)
      ಹೋಗು error;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (ಅಕ್ಷರ *)&reuse, (curl_socklen_t)ಗಾತ್ರ(reuse)) == -1)
    ಹೋಗು error;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(bind(listener, &a.addr, ಗಾತ್ರ(a.inaddr)) == -1)
    ಹೋಗು error;
  ಆದರೆ(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(a.inaddr))
    ಹೋಗು error;
  ಆದರೆ(listen(listener, 1) == -1)
    ಹೋಗು error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ಆದರೆ(socks[0] == CURL_SOCKET_BAD)
    ಹೋಗು error;
  ಆದರೆ(connect(socks[0], &a.addr, ಗಾತ್ರ(a.inaddr)) == -1)
    ಹೋಗು error;

  /* use non-blocking accept to make sure we do not block forever */
  ಆದರೆ(curlx_nonblock(listener, TRUE) < 0)
    ಹೋಗು error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (ಶೂನ್ಯ)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ಖಾಲಿ, ಖಾಲಿ);
  ಆದರೆ(socks[1] == CURL_SOCKET_BAD)
    ಹೋಗು error;
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ curltime ಆರಂಭ = curlx_now();
    ಅಕ್ಷರ rnd[9];
    ಅಕ್ಷರ check[ಗಾತ್ರ(rnd)];
    ಅಕ್ಷರ *p = &check[0];
    size_t s = ಗಾತ್ರ(check);

    ಆದರೆ(Curl_rand(ಖಾಲಿ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)rnd, ಗಾತ್ರ(rnd)))
      ಹೋಗು error;

    /* write data to the socket */
    swrite(socks[0], rnd, ಗಾತ್ರ(rnd));
    /* verify that we read the correct data */
    ಮಾಡು {
      ssize_t nread;

      /* Do not block forever */
      ಆದರೆ(curlx_timediff_ms(curlx_now(), ಆರಂಭ) > (60 * 1000))
        ಹೋಗು error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (ಶೂನ್ಯ)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ಆದರೆ(!nread) { /* unexpected close before getting complete `check` */
        ಹೋಗು error;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == -1) { /* error or close */
        ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
        ಆದರೆ(SOCK_EAGAIN(sockerr)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#ಆದರೆ_ಅಂತ್ಯ
          ) {
          ಮುಂದುವರಿ;
        }
        ಹೋಗು error;
      }
      s -= nread;
      ಆದರೆ(s) {
        p += nread;
        ಮುಂದುವರಿ;
      }
      ಆದರೆ(memcmp(rnd, check, ಗಾತ್ರ(check)))
        ಹೋಗು error;
      ನಿಲ್ಲಿಸು;
    } ಆಗ(1);
  }

  ಆದರೆ(nonblocking)
    ಆದರೆ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      ಹೋಗು error;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SO_NOSIGPIPE
  ಆದರೆ(Curl_sock_nosigpipe(socks[1]) < 0)
    ಹೋಗು error;
#ಆದರೆ_ಅಂತ್ಯ
  sclose(listener);
  ಹಿಂತಿರುಗಿ 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  ಹಿಂತಿರುಗಿ -1;
}

#ಆದರೆ_ಅಂತ್ಯ /* choose implementation */

ಪೂರ್ಣಾಂಕ Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EVENTFD
  ಹಿಂತಿರುಗಿ wakeup_eventfd(socks, nonblocking);
#ಅಥವಾಆದರೆ defined(HAVE_PIPE)
  ಹಿಂತಿರುಗಿ wakeup_pipe(socks, nonblocking);
#ಅಥವಾಆದರೆ defined(HAVE_SOCKETPAIR)
  ಹಿಂತಿರುಗಿ wakeup_socketpair(socks, nonblocking);
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ wakeup_inet(socks, nonblocking);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#ವ್ಯಾಖ್ಯಾನಿಸು wakeup_write        ಲಿಖ
#ವ್ಯಾಖ್ಯಾನಿಸು wakeup_read         ಪಠ
#ವ್ಯಾಖ್ಯಾನಿಸು wakeup_close        close

#ಇಲ್ಲದಿದ್ದರೆ /* !USE_EVENTFD && !HAVE_PIPE */

#ವ್ಯಾಖ್ಯಾನಿಸು wakeup_write        swrite
#ವ್ಯಾಖ್ಯಾನಿಸು wakeup_read         sread
#ವ್ಯಾಖ್ಯಾನಿಸು wakeup_close        sclose

#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ Curl_wakeup_signal(curl_socket_t socks[2])
{
  ಪೂರ್ಣಾಂಕ sockerr = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EVENTFD
  ಸ್ಥಿರಾಂಕ uint64_t buf[1] = { 1 };
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ buf[1] = { 1 };
#ಆದರೆ_ಅಂತ್ಯ

  ಆಗ(1) {
    sockerr = 0;
    ಆದರೆ(wakeup_write(socks[1], buf, ಗಾತ್ರ(buf)) < 0) {
      sockerr = SOCKERRNO;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINSOCK
      ಆದರೆ(sockerr == SOCKEINTR)
        ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  ಅಕ್ಷರ buf[64];
  ssize_t rc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಮಾಡು {
    rc = wakeup_read(socks[0], buf, ಗಾತ್ರ(buf));
    ಆದರೆ(!rc)
      ನಿಲ್ಲಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc < 0) {
      ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINSOCK
      ಆದರೆ(sockerr == SOCKEINTR)
        ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(SOCK_EAGAIN(sockerr))
        ನಿಲ್ಲಿಸು;
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ನಿಲ್ಲಿಸು;
    }
  } ಆಗ(all);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_wakeup_destroy(curl_socket_t socks[2])
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_EVENTFD
  ಆದರೆ(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_SOCKETPAIR */
