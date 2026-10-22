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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#اگر_تعریف_شدہ USE_EVENTFD

#شامل <sys/eventfd.h>

ساکن صحیح wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  صحیح efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  اگر(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    واپس -1;
  }
  socks[0] = socks[1] = efd;
  واپس 0;
}

#وگرنہ_اگر defined(HAVE_PIPE)

#اگر_تعریف_شدہ HAVE_FCNTL
#شامل <fcntl.h>
#اگر_ختم

ساکن صحیح wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#اگر_تعریف_شدہ HAVE_PIPE2
  واپس pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#ورنہ
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_دستیاب) && \
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
    واپس pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  ورنہ
#اگر_ختم
  اگر(pipe(socks))
    واپس -1;
#اگر_تعریف_شدہ HAVE_FCNTL
  اگر(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    واپس -1;
  }
#اگر_ختم
  اگر(nonblocking) {
    اگر(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      واپس -1;
    }
  }
  واپس 0;
#اگر_ختم /* HAVE_PIPE2 */
}

#وگرنہ_اگر defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#اگر_غیر_تعریف_شدہ USE_UNIX_SOCKETS
#غلطی "unsupported Unix domain and socketpair build combo"
#اگر_ختم

ساکن صحیح wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  صحیح type = SOCK_STREAM;
#اگر_تعریف_شدہ SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#اگر_ختم
#اگر_تعریف_شدہ CURL_USE_SOCK_NONBLOCK
  اگر(nonblocking)
    type |= SOCK_NONBLOCK;
#اگر_ختم

  اگر(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    واپس -1;
#اگر_غیر_تعریف_شدہ CURL_USE_SOCK_NONBLOCK
  اگر(nonblocking) {
    اگر(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      واپس -1;
    }
  }
#اگر_ختم
#اگر_تعریف_شدہ USE_SO_NOSIGPIPE
  اگر(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    واپس -1;
  }
#اگر_ختم /* USE_SO_NOSIGPIPE */

  واپس 0;
}

#ورنہ /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h> /* for IPPROTO_TCP */
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ INADDR_LOOPBACK
#تعریف_کرو INADDR_LOOPBACK 0x7f000001
#اگر_ختم

#شامل "select.h"   /* for Curl_poll */

ساکن صحیح wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  اتحاد {
    ساخت sockaddr_in inaddr;
    ساخت sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = حجم(a.inaddr);
  صحیح reuse = 1;
  ساخت pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  اگر(listener == CURL_SOCKET_BAD)
    واپس -1;

  memset(&a, 0, حجم(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#اگر defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (خالی)reuse;
#اگر_تعریف_شدہ SO_EXCLUSIVEADDRUSE
  {
    صحیح exclusive = 1;
    اگر(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (حرف *)&exclusive, (curl_socklen_t)حجم(exclusive)) == -1)
      جاؤ error;
  }
#اگر_ختم
#ورنہ
  اگر(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (حرف *)&reuse, (curl_socklen_t)حجم(reuse)) == -1)
    جاؤ error;
#اگر_ختم
  اگر(bind(listener, &a.addr, حجم(a.inaddr)) == -1)
    جاؤ error;
  اگر(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (صحیح)حجم(a.inaddr))
    جاؤ error;
  اگر(listen(listener, 1) == -1)
    جاؤ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  اگر(socks[0] == CURL_SOCKET_BAD)
    جاؤ error;
  اگر(connect(socks[0], &a.addr, حجم(a.inaddr)) == -1)
    جاؤ error;

  /* use non-blocking accept to make sure we do not block forever */
  اگر(curlx_nonblock(listener, TRUE) < 0)
    جاؤ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (خالی)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, تہی, تہی);
  اگر(socks[1] == CURL_SOCKET_BAD)
    جاؤ error;
  ورنہ {
    ساخت curltime آغاز = curlx_now();
    حرف rnd[9];
    حرف check[حجم(rnd)];
    حرف *p = &check[0];
    size_t s = حجم(check);

    اگر(Curl_rand(تہی, (غیرعلامتی حرف *)rnd, حجم(rnd)))
      جاؤ error;

    /* write data to the socket */
    swrite(socks[0], rnd, حجم(rnd));
    /* verify that we read the correct data */
    کرو {
      ssize_t nread;

      /* Do not block forever */
      اگر(curlx_timediff_ms(curlx_now(), آغاز) > (60 * 1000))
        جاؤ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (خالی)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      اگر(!nread) { /* unexpected close before getting complete `check` */
        جاؤ error;
      }
      ورنہ اگر(nread == -1) { /* error or close */
        صحیح sockerr = SOCKERRNO;
        اگر(SOCK_EAGAIN(sockerr)
#اگر_غیر_تعریف_شدہ USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#اگر_ختم
          ) {
          جاری;
        }
        جاؤ error;
      }
      s -= nread;
      اگر(s) {
        p += nread;
        جاری;
      }
      اگر(memcmp(rnd, check, حجم(check)))
        جاؤ error;
      توڑو;
    } جب(1);
  }

  اگر(nonblocking)
    اگر(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      جاؤ error;
#اگر_تعریف_شدہ USE_SO_NOSIGPIPE
  اگر(Curl_sock_nosigpipe(socks[1]) < 0)
    جاؤ error;
#اگر_ختم
  sclose(listener);
  واپس 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  واپس -1;
}

#اگر_ختم /* choose implementation */

صحیح Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#اگر_تعریف_شدہ USE_EVENTFD
  واپس wakeup_eventfd(socks, nonblocking);
#وگرنہ_اگر defined(HAVE_PIPE)
  واپس wakeup_pipe(socks, nonblocking);
#وگرنہ_اگر defined(HAVE_SOCKETPAIR)
  واپس wakeup_socketpair(socks, nonblocking);
#ورنہ
  واپس wakeup_inet(socks, nonblocking);
#اگر_ختم
}

#اگر defined(USE_EVENTFD) || defined(HAVE_PIPE)

#تعریف_کرو wakeup_write        اكتب
#تعریف_کرو wakeup_read         اقرأ
#تعریف_کرو wakeup_close        close

#ورنہ /* !USE_EVENTFD && !HAVE_PIPE */

#تعریف_کرو wakeup_write        swrite
#تعریف_کرو wakeup_read         sread
#تعریف_کرو wakeup_close        sclose

#اگر_ختم

صحیح Curl_wakeup_signal(curl_socket_t socks[2])
{
  صحیح sockerr = 0;
#اگر_تعریف_شدہ USE_EVENTFD
  ثابت uint64_t buf[1] = { 1 };
#ورنہ
  ثابت حرف buf[1] = { 1 };
#اگر_ختم

  جب(1) {
    sockerr = 0;
    اگر(wakeup_write(socks[1], buf, حجم(buf)) < 0) {
      sockerr = SOCKERRNO;
#اگر_غیر_تعریف_شدہ USE_WINSOCK
      اگر(sockerr == SOCKEINTR)
        جاری;
#اگر_ختم
      اگر(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    توڑو;
  }
  واپس sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  حرف buf[64];
  ssize_t rc;
  CURLcode نتیجہ = CURLE_OK;

  کرو {
    rc = wakeup_read(socks[0], buf, حجم(buf));
    اگر(!rc)
      توڑو;
    ورنہ اگر(rc < 0) {
      صحیح sockerr = SOCKERRNO;
#اگر_غیر_تعریف_شدہ USE_WINSOCK
      اگر(sockerr == SOCKEINTR)
        جاری;
#اگر_ختم
      اگر(SOCK_EAGAIN(sockerr))
        توڑو;
      نتیجہ = CURLE_READ_ERROR;
      توڑو;
    }
  } جب(all);
  واپس نتیجہ;
}

خالی Curl_wakeup_destroy(curl_socket_t socks[2])
{
#اگر_غیر_تعریف_شدہ USE_EVENTFD
  اگر(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#اگر_ختم
  اگر(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#اگر_ختم /* !CURL_DISABLE_SOCKETPAIR */
