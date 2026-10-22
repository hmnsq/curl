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
#ضمن "curl_setup.h"

#ضمن "socketpair.h"
#ضمن "urldata.h"
#ضمن "rand.h"
#ضمن "curlx/nonblock.h"

#که_نه_تعریف_شوی CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#که_تعریف_شوی USE_EVENTFD

#ضمن <sys/eventfd.h>

ساكن صحيح wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  صحيح efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  إذا(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    أرجع -1;
  }
  socks[0] = socks[1] = efd;
  أرجع 0;
}

#وإلاإذا defined(HAVE_PIPE)

#که_تعریف_شوی HAVE_FCNTL
#ضمن <fcntl.h>
#د_که_پای

ساكن صحيح wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#که_تعریف_شوی HAVE_PIPE2
  أرجع pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#وإلا
#إذا defined(__APPLE__) && defined(HAVE_BUILTIN_شته) && \
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
  إذا(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    أرجع pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  وإلا
#د_که_پای
  إذا(pipe(socks))
    أرجع -1;
#که_تعریف_شوی HAVE_FCNTL
  إذا(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    أرجع -1;
  }
#د_که_پای
  إذا(nonblocking) {
    إذا(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      أرجع -1;
    }
  }
  أرجع 0;
#د_که_پای /* HAVE_PIPE2 */
}

#وإلاإذا defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#که_نه_تعریف_شوی USE_UNIX_SOCKETS
#تېروتنه "unsupported Unix domain and socketpair build combo"
#د_که_پای

ساكن صحيح wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  صحيح type = SOCK_STREAM;
#که_تعریف_شوی SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#د_که_پای
#که_تعریف_شوی CURL_USE_SOCK_NONBLOCK
  إذا(nonblocking)
    type |= SOCK_NONBLOCK;
#د_که_پای

  إذا(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    أرجع -1;
#که_نه_تعریف_شوی CURL_USE_SOCK_NONBLOCK
  إذا(nonblocking) {
    إذا(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      أرجع -1;
    }
  }
#د_که_پای
#که_تعریف_شوی USE_SO_NOSIGPIPE
  إذا(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    أرجع -1;
  }
#د_که_پای /* USE_SO_NOSIGPIPE */

  أرجع 0;
}

#وإلا /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h> /* for IPPROTO_TCP */
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای

#که_نه_تعریف_شوی INADDR_LOOPBACK
#تعریف_کړه INADDR_LOOPBACK 0x7f000001
#د_که_پای

#ضمن "select.h"   /* for Curl_poll */

ساكن صحيح wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  اتحاد {
    بنية sockaddr_in inaddr;
    بنية sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = حجم(a.inaddr);
  صحيح reuse = 1;
  بنية pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  إذا(listener == CURL_SOCKET_BAD)
    أرجع -1;

  memset(&a, 0, حجم(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#إذا defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (فراغ)reuse;
#که_تعریف_شوی SO_EXCLUSIVEADDRUSE
  {
    صحيح exclusive = 1;
    إذا(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (حرف *)&exclusive, (curl_socklen_t)حجم(exclusive)) == -1)
      اذهب error;
  }
#د_که_پای
#وإلا
  إذا(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (حرف *)&reuse, (curl_socklen_t)حجم(reuse)) == -1)
    اذهب error;
#د_که_پای
  إذا(bind(listener, &a.addr, حجم(a.inaddr)) == -1)
    اذهب error;
  إذا(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (صحيح)حجم(a.inaddr))
    اذهب error;
  إذا(listen(listener, 1) == -1)
    اذهب error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  إذا(socks[0] == CURL_SOCKET_BAD)
    اذهب error;
  إذا(connect(socks[0], &a.addr, حجم(a.inaddr)) == -1)
    اذهب error;

  /* use non-blocking accept to make sure we do not block forever */
  إذا(curlx_nonblock(listener, TRUE) < 0)
    اذهب error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (فراغ)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, عدم, عدم);
  إذا(socks[1] == CURL_SOCKET_BAD)
    اذهب error;
  وإلا {
    بنية curltime پیل = curlx_now();
    حرف rnd[9];
    حرف check[حجم(rnd)];
    حرف *p = &check[0];
    size_t s = حجم(check);

    إذا(Curl_rand(عدم, (بینښه حرف *)rnd, حجم(rnd)))
      اذهب error;

    /* write data to the socket */
    swrite(socks[0], rnd, حجم(rnd));
    /* verify that we read the correct data */
    افعل {
      ssize_t nread;

      /* Do not block forever */
      إذا(curlx_timediff_ms(curlx_now(), پیل) > (60 * 1000))
        اذهب error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (فراغ)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      إذا(!nread) { /* unexpected close before getting complete `check` */
        اذهب error;
      }
      وإلا إذا(nread == -1) { /* error or close */
        صحيح sockerr = SOCKERRNO;
        إذا(SOCK_EAGAIN(sockerr)
#که_نه_تعریف_شوی USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#د_که_پای
          ) {
          استمر;
        }
        اذهب error;
      }
      s -= nread;
      إذا(s) {
        p += nread;
        استمر;
      }
      إذا(memcmp(rnd, check, حجم(check)))
        اذهب error;
      اكسر;
    } طالما(1);
  }

  إذا(nonblocking)
    إذا(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      اذهب error;
#که_تعریف_شوی USE_SO_NOSIGPIPE
  إذا(Curl_sock_nosigpipe(socks[1]) < 0)
    اذهب error;
#د_که_پای
  sclose(listener);
  أرجع 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  أرجع -1;
}

#د_که_پای /* choose implementation */

صحيح Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#که_تعریف_شوی USE_EVENTFD
  أرجع wakeup_eventfd(socks, nonblocking);
#وإلاإذا defined(HAVE_PIPE)
  أرجع wakeup_pipe(socks, nonblocking);
#وإلاإذا defined(HAVE_SOCKETPAIR)
  أرجع wakeup_socketpair(socks, nonblocking);
#وإلا
  أرجع wakeup_inet(socks, nonblocking);
#د_که_پای
}

#إذا defined(USE_EVENTFD) || defined(HAVE_PIPE)

#تعریف_کړه wakeup_write        اكتب
#تعریف_کړه wakeup_read         اقرأ
#تعریف_کړه wakeup_close        close

#وإلا /* !USE_EVENTFD && !HAVE_PIPE */

#تعریف_کړه wakeup_write        swrite
#تعریف_کړه wakeup_read         sread
#تعریف_کړه wakeup_close        sclose

#د_که_پای

صحيح Curl_wakeup_signal(curl_socket_t socks[2])
{
  صحيح sockerr = 0;
#که_تعریف_شوی USE_EVENTFD
  ثابت uint64_t buf[1] = { 1 };
#وإلا
  ثابت حرف buf[1] = { 1 };
#د_که_پای

  طالما(1) {
    sockerr = 0;
    إذا(wakeup_write(socks[1], buf, حجم(buf)) < 0) {
      sockerr = SOCKERRNO;
#که_نه_تعریف_شوی USE_WINSOCK
      إذا(sockerr == SOCKEINTR)
        استمر;
#د_که_پای
      إذا(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    اكسر;
  }
  أرجع sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  حرف buf[64];
  ssize_t rc;
  CURLcode پایله = CURLE_OK;

  افعل {
    rc = wakeup_read(socks[0], buf, حجم(buf));
    إذا(!rc)
      اكسر;
    وإلا إذا(rc < 0) {
      صحيح sockerr = SOCKERRNO;
#که_نه_تعریف_شوی USE_WINSOCK
      إذا(sockerr == SOCKEINTR)
        استمر;
#د_که_پای
      إذا(SOCK_EAGAIN(sockerr))
        اكسر;
      پایله = CURLE_READ_ERROR;
      اكسر;
    }
  } طالما(all);
  أرجع پایله;
}

فراغ Curl_wakeup_destroy(curl_socket_t socks[2])
{
#که_نه_تعریف_شوی USE_EVENTFD
  إذا(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#د_که_پای
  إذا(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#د_که_پای /* !CURL_DISABLE_SOCKETPAIR */
