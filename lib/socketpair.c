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
#समावेश "curl_setup.h"

#समावेश "socketpair.h"
#समावेश "urldata.h"
#समावेश "rand.h"
#समावेश "curlx/nonblock.h"

#अगर_अघोषित CURL_DISABLE_SOCKETPAIR

/* choose implementation */
#अगर_घोषित USE_EVENTFD

#समावेश <sys/eventfd.h>

अटल पूर्णांक wakeup_eventfd(curl_socket_t socks[2], bool nonblocking)
{
  पूर्णांक efd = eventfd(0, nonblocking ? EFD_CLOEXEC | EFD_NONBLOCK : EFD_CLOEXEC);
  अगर(efd == -1) {
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    वापस -1;
  }
  socks[0] = socks[1] = efd;
  वापस 0;
}

#या_अगर defined(HAVE_PIPE)

#अगर_घोषित HAVE_FCNTL
#समावेश <fcntl.h>
#पूर्ण_अगर

अटल पूर्णांक wakeup_pipe(curl_socket_t socks[2], bool nonblocking)
{
#अगर_घोषित HAVE_PIPE2
  वापस pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
#अन्यथा
#अगर defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
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
  अगर(__builtin_available(macOS 27, iOS 27, tvOS 27, watchOS 27, visionOS 27,
                         macCatalyst 27, *))
    वापस pipe2(socks, (nonblocking ? O_NONBLOCK : 0) | O_CLOEXEC) ? -1 : 0;
  अन्यथा
#पूर्ण_अगर
  अगर(pipe(socks))
    वापस -1;
#अगर_घोषित HAVE_FCNTL
  अगर(fcntl(socks[0], F_SETFD, FD_CLOEXEC) ||
     fcntl(socks[1], F_SETFD, FD_CLOEXEC)) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    वापस -1;
  }
#पूर्ण_अगर
  अगर(nonblocking) {
    अगर(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      वापस -1;
    }
  }
  वापस 0;
#पूर्ण_अगर /* HAVE_PIPE2 */
}

#या_अगर defined(HAVE_SOCKETPAIR)  /* !USE_EVENTFD && !HAVE_PIPE */

#अगर_अघोषित USE_UNIX_SOCKETS
#त्रुटि "unsupported Unix domain and socketpair build combo"
#पूर्ण_अगर

अटल पूर्णांक wakeup_socketpair(curl_socket_t socks[2], bool nonblocking)
{
  पूर्णांक type = SOCK_STREAM;
#अगर_घोषित SOCK_CLOEXEC
  type |= SOCK_CLOEXEC;
#पूर्ण_अगर
#अगर_घोषित CURL_USE_SOCK_NONBLOCK
  अगर(nonblocking)
    type |= SOCK_NONBLOCK;
#पूर्ण_अगर

  अगर(CURL_SOCKETPAIR(AF_UNIX, type, 0, socks))
    वापस -1;
#अगर_अघोषित CURL_USE_SOCK_NONBLOCK
  अगर(nonblocking) {
    अगर(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0) {
      sclose(socks[0]);
      sclose(socks[1]);
      socks[0] = socks[1] = CURL_SOCKET_BAD;
      वापस -1;
    }
  }
#पूर्ण_अगर
#अगर_घोषित USE_SO_NOSIGPIPE
  अगर(Curl_sock_nosigpipe(socks[1]) < 0) {
    sclose(socks[0]);
    sclose(socks[1]);
    socks[0] = socks[1] = CURL_SOCKET_BAD;
    वापस -1;
  }
#पूर्ण_अगर /* USE_SO_NOSIGPIPE */

  वापस 0;
}

#अन्यथा /* !USE_EVENTFD && !HAVE_PIPE && !HAVE_SOCKETPAIR */

#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h> /* for IPPROTO_TCP */
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#अगर_अघोषित INADDR_LOOPBACK
#घोषणा INADDR_LOOPBACK 0x7f000001
#पूर्ण_अगर

#समावेश "select.h"   /* for Curl_poll */

अटल पूर्णांक wakeup_inet(curl_socket_t socks[2], bool nonblocking)
{
  जोड़ {
    काष्ठा sockaddr_in inaddr;
    काष्ठा sockaddr addr;
  } a;
  curl_socket_t listener;
  curl_socklen_t addrlen = माप(a.inaddr);
  पूर्णांक reuse = 1;
  काष्ठा pollfd pfd[1];

  listener = CURL_SOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  अगर(listener == CURL_SOCKET_BAD)
    वापस -1;

  स_रखो(&a, 0, माप(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0;

  socks[0] = socks[1] = CURL_SOCKET_BAD;

#अगर defined(_WIN32) || defined(__CYGWIN__)
  /* do not set SO_REUSEADDR on Windows */
  (व्योम)reuse;
#अगर_घोषित SO_EXCLUSIVEADDRUSE
  {
    पूर्णांक exclusive = 1;
    अगर(setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                  (अक्षर *)&exclusive, (curl_socklen_t)माप(exclusive)) == -1)
      जाओ error;
  }
#पूर्ण_अगर
#अन्यथा
  अगर(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                (अक्षर *)&reuse, (curl_socklen_t)माप(reuse)) == -1)
    जाओ error;
#पूर्ण_अगर
  अगर(bind(listener, &a.addr, माप(a.inaddr)) == -1)
    जाओ error;
  अगर(getsockname(listener, &a.addr, &addrlen) == -1 ||
     addrlen < (पूर्णांक)माप(a.inaddr))
    जाओ error;
  अगर(listen(listener, 1) == -1)
    जाओ error;
  socks[0] = CURL_SOCKET(AF_INET, SOCK_STREAM, 0);
  अगर(socks[0] == CURL_SOCKET_BAD)
    जाओ error;
  अगर(connect(socks[0], &a.addr, माप(a.inaddr)) == -1)
    जाओ error;

  /* use non-blocking accept to make sure we do not block forever */
  अगर(curlx_nonblock(listener, TRUE) < 0)
    जाओ error;
  pfd[0].fd = listener;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;
  (व्योम)Curl_poll(pfd, 1, 1000); /* one second */
  socks[1] = CURL_ACCEPT(listener, शून्य, शून्य);
  अगर(socks[1] == CURL_SOCKET_BAD)
    जाओ error;
  अन्यथा {
    काष्ठा curltime start = curlx_now();
    अक्षर rnd[9];
    अक्षर check[माप(rnd)];
    अक्षर *p = &check[0];
    माप_प्रकार s = माप(check);

    अगर(Curl_rand(शून्य, (अचिन्हित अक्षर *)rnd, माप(rnd)))
      जाओ error;

    /* write data to the socket */
    swrite(socks[0], rnd, माप(rnd));
    /* verify that we read the correct data */
    करो {
      ssize_t nread;

      /* Do not block forever */
      अगर(curlx_timediff_ms(curlx_now(), start) > (60 * 1000))
        जाओ error;

      pfd[0].fd = socks[1];
      pfd[0].events = POLLIN;
      pfd[0].revents = 0;
      (व्योम)Curl_poll(pfd, 1, 1000); /* one second */

      nread = sread(socks[1], p, s);
      अगर(!nread) { /* unexpected close before getting complete `check` */
        जाओ error;
      }
      अन्यथा अगर(nread == -1) { /* error or close */
        पूर्णांक sockerr = SOCKERRNO;
        अगर(SOCK_EAGAIN(sockerr)
#अगर_अघोषित USE_WINSOCK
           || (sockerr == SOCKEINTR) || (sockerr == SOCKEINPROGRESS)
#पूर्ण_अगर
          ) {
          जारी;
        }
        जाओ error;
      }
      s -= nread;
      अगर(s) {
        p += nread;
        जारी;
      }
      अगर(स_भेद(rnd, check, माप(check)))
        जाओ error;
      अवरोध;
    } जबतक(1);
  }

  अगर(nonblocking)
    अगर(curlx_nonblock(socks[0], TRUE) < 0 ||
       curlx_nonblock(socks[1], TRUE) < 0)
      जाओ error;
#अगर_घोषित USE_SO_NOSIGPIPE
  अगर(Curl_sock_nosigpipe(socks[1]) < 0)
    जाओ error;
#पूर्ण_अगर
  sclose(listener);
  वापस 0;

error:
  sclose(listener);
  sclose(socks[0]);
  sclose(socks[1]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
  वापस -1;
}

#पूर्ण_अगर /* choose implementation */

पूर्णांक Curl_wakeup_init(curl_socket_t socks[2], bool nonblocking)
{
#अगर_घोषित USE_EVENTFD
  वापस wakeup_eventfd(socks, nonblocking);
#या_अगर defined(HAVE_PIPE)
  वापस wakeup_pipe(socks, nonblocking);
#या_अगर defined(HAVE_SOCKETPAIR)
  वापस wakeup_socketpair(socks, nonblocking);
#अन्यथा
  वापस wakeup_inet(socks, nonblocking);
#पूर्ण_अगर
}

#अगर defined(USE_EVENTFD) || defined(HAVE_PIPE)

#घोषणा wakeup_write        ग_लिखो
#घोषणा wakeup_read         पढ़ो
#घोषणा wakeup_close        बंद

#अन्यथा /* !USE_EVENTFD && !HAVE_PIPE */

#घोषणा wakeup_write        swrite
#घोषणा wakeup_read         sread
#घोषणा wakeup_close        sclose

#पूर्ण_अगर

पूर्णांक Curl_wakeup_signal(curl_socket_t socks[2])
{
  पूर्णांक sockerr = 0;
#अगर_घोषित USE_EVENTFD
  स्थिर uint64_t buf[1] = { 1 };
#अन्यथा
  स्थिर अक्षर buf[1] = { 1 };
#पूर्ण_अगर

  जबतक(1) {
    sockerr = 0;
    अगर(wakeup_write(socks[1], buf, माप(buf)) < 0) {
      sockerr = SOCKERRNO;
#अगर_अघोषित USE_WINSOCK
      अगर(sockerr == SOCKEINTR)
        जारी;
#पूर्ण_अगर
      अगर(SOCK_EAGAIN(sockerr))
        sockerr = 0; /* wakeup is already ongoing */
    }
    अवरोध;
  }
  वापस sockerr;
}

CURLcode Curl_wakeup_consume(curl_socket_t socks[2], bool all)
{
  अक्षर buf[64];
  ssize_t rc;
  CURLcode परिणाम = CURLE_OK;

  करो {
    rc = wakeup_read(socks[0], buf, माप(buf));
    अगर(!rc)
      अवरोध;
    अन्यथा अगर(rc < 0) {
      पूर्णांक sockerr = SOCKERRNO;
#अगर_अघोषित USE_WINSOCK
      अगर(sockerr == SOCKEINTR)
        जारी;
#पूर्ण_अगर
      अगर(SOCK_EAGAIN(sockerr))
        अवरोध;
      परिणाम = CURLE_READ_ERROR;
      अवरोध;
    }
  } जबतक(all);
  वापस परिणाम;
}

व्योम Curl_wakeup_destroy(curl_socket_t socks[2])
{
#अगर_अघोषित USE_EVENTFD
  अगर(socks[1] != CURL_SOCKET_BAD)
    wakeup_close(socks[1]);
#पूर्ण_अगर
  अगर(socks[0] != CURL_SOCKET_BAD)
    wakeup_close(socks[0]);
  socks[0] = socks[1] = CURL_SOCKET_BAD;
}

#पूर्ण_अगर /* !CURL_DISABLE_SOCKETPAIR */
