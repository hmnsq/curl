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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "socketpair.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "curlx/nonblock.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#నిర్వచితమైతే USE_EVENTFD

#అన్తర్భావ <sys/eventfd.h>

స్థిర పూర్ణాంకం wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  పూర్ణాంకం efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  ఒకవేళ(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    తిరిగి -1;
  }
  socks[0] = socks[1] = efd;
  తిరిగి 0;
}

#లేదావేళ defined(HAVE_PIPE)

#నిర్వచితమైతే HAVE_FCNTL
#అన్తర్భావ <fcntl.h>
#అయితే_ముగింపు

స్థిర పూర్ణాంకం wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#నిర్వచితమైతే HAVE_PIPE2
  తిరిగి pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#లేకపోతే
#ఒకవేళ defined(__APPLE__) && defined(HAVE_BUILTIN_అందుబాటు) && \
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
  ఒకవేళ(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    తిరిగి pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  లేకపోతే
#అయితే_ముగింపు
  ఒకవేళ(pipe(socks))
    తిరిగి -1;
#నిర్వచితమైతే HAVE_FCNTL
  ఒకవేళ(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    తిరిగి -1;
  }
#అయితే_ముగింపు
  ఒకవేళ(nonblocking) {
    ఒకవేళ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      తిరిగి -1;
    }
  }
  తిరిగి 0;
#అయితే_ముగింపు /* HAVE_PIPE2 */
}

#లేదావేళ defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#నిర్వచితం_కాకపోతే USE_UNIX_SOCKETS
#దోషం "unsupported Unix domain and socketpair build combo"
#అయితే_ముగింపు

స్థిర పూర్ణాంకం wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  పూర్ణాంకం type = SOCK_STREAM;
#నిర్వచితమైతే SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_USE_SOCK_NONBLOCK
  ఒకవేళ(nonblocking)
    type |= SOCK_NONBLOCK;
#అయితే_ముగింపు

  ఒకవేళ(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    తిరిగి -1;
#నిర్వచితం_కాకపోతే CURL_USE_SOCK_NONBLOCK
  ఒకవేళ(nonblocking) {
    ఒకవేళ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      తిరిగి -1;
    }
  }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SO_NOSIGPIPE
  ఒకవేళ(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    తిరిగి -1;
  }
#అయితే_ముగింపు /* USE_SO_NOSIGPIPE */

  తిరిగి 0;
}

#లేకపోతే /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h> /* for IPPROTO_TCP */
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే INADDR_LOOPBACK
#నిర్వచించు INADDR_LOOPBACK 0x7f000001
#అయితే_ముగింపు

#అన్తర్భావ "select.h"   /* for Curl_poll */

స్థిర పూర్ణాంకం wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  సంఘం {
    నిర్మాణం sockaddr_in inaddr;
    నిర్మాణం sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = పరిమాణం(a.inaddr);
  పూర్ణాంకం reuse = 1;
  నిర్మాణం pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ఒకవేళ(listener == CURL_SOCKET_BAD)
    తిరిగి -1;

  memset(&a, 0, పరిమాణం(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#ఒకవేళ defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (శూన్య)reuse;
#నిర్వచితమైతే SO_EXCLUSIVEADDRUSE
  {
    పూర్ణాంకం exclusive = 1;
    ఒకవేళ(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (అక్షరం *)&exclusive, (curl_socklen_t)పరిమాణం(exclusive)) == -1)
      వెళ్ళు error;
  }
#అయితే_ముగింపు
#లేకపోతే
  ఒకవేళ(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (అక్షరం *)&reuse, (curl_socklen_t)పరిమాణం(reuse)) == -1)
    వెళ్ళు error;
#అయితే_ముగింపు
  ఒకవేళ(bind(listener, &a.addr, పరిమాణం(a.inaddr)) == -1)
    వెళ్ళు error;
  ఒకవేళ(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (పూర్ణాంకం)పరిమాణం(a.inaddr))
    వెళ్ళు error;
  ఒకవేళ(listen(listener, 1) == -1)
    వెళ్ళు error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  ఒకవేళ(socks[0] == CURL_SOCKET_BAD)
    వెళ్ళు error;
  ఒకవేళ(connect(socks[0], &a.addr, పరిమాణం(a.inaddr)) == -1)
    వెళ్ళు error;

  /* use non-blocking accept to make sure we do not block forever */
  ఒకవేళ(curlx_nonblock(listener, TRUE) < 0)
    వెళ్ళు error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (శూన్య)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, ఖాళీ, ఖాళీ);
  ఒకవేళ(socks[1] == CURL_SOCKET_BAD)
    వెళ్ళు error;
  లేకపోతే {
    నిర్మాణం curltime ప్రారంభం = curlx_now();
    అక్షరం rnd[9];
    అక్షరం check[పరిమాణం(rnd)];
    అక్షరం *p = &check[0];
    size_t s = పరిమాణం(check);

    ఒకవేళ(Curl_rand(ఖాళీ, (గుర్తులేని అక్షరం *)rnd, పరిమాణం(rnd)))
      వెళ్ళు error;

    /* write data to the socket */
    swrite(socks[0], rnd, పరిమాణం(rnd));
    /* verify that we read the correct data */
    చేయి {
      ssize_t nread;

      /* Do not block forever */
      ఒకవేళ(curlx_timediff_ms(curlx_now(), ప్రారంభం) > (60 * 1000))
        వెళ్ళు error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (శూన్య)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      ఒకవేళ(!nread) { /* unexpected close before getting complete `check` */
        వెళ్ళు error;
      }
      లేకపోతే ఒకవేళ(nread == -1) { /* error or close */
        పూర్ణాంకం sockerr = SOCKERRNO;
        ఒకవేళ(SOCK_EAGAIN(sockerr)
#నిర్వచితం_కాకపోతే USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#అయితే_ముగింపు
          ) {
          కొనసాగు;
        }
        వెళ్ళు error;
      }
      s -= nread;
      ఒకవేళ(s) {
        p += nread;
        కొనసాగు;
      }
      ఒకవేళ(memcmp(rnd, check, పరిమాణం(check)))
        వెళ్ళు error;
      ఆపు;
    } అయితే(1);
  }

  ఒకవేళ(nonblocking)
    ఒకవేళ(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      వెళ్ళు error;
#నిర్వచితమైతే USE_SO_NOSIGPIPE
  ఒకవేళ(Curl_sock_nosigpipe(socks[1]) < 0)
    వెళ్ళు error;
#అయితే_ముగింపు
  sclose(listener);
  తిరిగి 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  తిరిగి -1;
}

#అయితే_ముగింపు /* choose implementation */

పూర్ణాంకం Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#నిర్వచితమైతే USE_EVENTFD
  తిరిగి wakeup_eventfd(socks, nonblocking);
#లేదావేళ defined(HAVE_PIPE)
  తిరిగి wakeup_pipe(socks, nonblocking);
#లేదావేళ defined(HAVE_SOCKETPAIR)
  తిరిగి wakeup_socketpair(socks, nonblocking);
#లేకపోతే
  తిరిగి wakeup_inet(socks, nonblocking);
#అయితే_ముగింపు
}

#ఒకవేళ defined(USE_EVENTFD) || defined(HAVE_PIPE)

#నిర్వచించు wakeup_write        లిఖ
#నిర్వచించు wakeup_read         పఠ
#నిర్వచించు wakeup_close        close

#లేకపోతే /* !USE_EVENTFD && !HAVE_PIPE */

#నిర్వచించు wakeup_write        swrite
#నిర్వచించు wakeup_read         sread
#నిర్వచించు wakeup_close        sclose

#అయితే_ముగింపు

పూర్ణాంకం Curl_wakeup_signal(curl_socket_t socks[2])
{
  పూర్ణాంకం sockerr = 0;
#నిర్వచితమైతే USE_EVENTFD
  స్థిరరాశి uint64_t buf[1] = { 1 };
#లేకపోతే
  స్థిరరాశి అక్షరం buf[1] = { 1 };
#అయితే_ముగింపు

  అయితే(1) {
    sockerr = 0;
    ఒకవేళ(wakeup_write(socks[1], buf, పరిమాణం(buf)) < 0) {
      sockerr = SOCKERRNO;
#నిర్వచితం_కాకపోతే USE_WINSOCK
      ఒకవేళ(sockerr == SOCKEINTR)
        కొనసాగు;
#అయితే_ముగింపు
      ఒకవేళ(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    ఆపు;
  }
  తిరిగి sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  అక్షరం buf[64];
  ssize_t rc;
  CURLcode ఫలితం = CURLE_OK;

  చేయి {
    rc = wakeup_read(socks[0], buf, పరిమాణం(buf));
    ఒకవేళ(!rc)
      ఆపు;
    లేకపోతే ఒకవేళ(rc < 0) {
      పూర్ణాంకం sockerr = SOCKERRNO;
#నిర్వచితం_కాకపోతే USE_WINSOCK
      ఒకవేళ(sockerr == SOCKEINTR)
        కొనసాగు;
#అయితే_ముగింపు
      ఒకవేళ(SOCK_EAGAIN(sockerr))
        ఆపు;
      ఫలితం = CURLE_READ_ERROR;
      ఆపు;
    }
  } అయితే(all);
  తిరిగి ఫలితం;
}

శూన్య Curl_wakeup_destroy(curl_socket_t socks[2])
{
#నిర్వచితం_కాకపోతే USE_EVENTFD
  ఒకవేళ(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#అయితే_ముగింపు
  ఒకవేళ(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#అయితే_ముగింపు /* !CURL_DISABLE_SOCKETPAIR */
