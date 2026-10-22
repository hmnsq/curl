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

#अगर_घोषित HAVE_SYS_SELECT_H
#समावेश <sys/select.h>
#या_अगर defined(HAVE_UNISTD_H)
#समावेश <unistd.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "curl_trc.h"
#समावेश "curlx/timediff.h"
#समावेश "curlx/wait.h"

#अगर_अघोषित HAVE_POLL  /* use select() */
/*
 * This is a wrapper around select() to aid in Windows compatibility. A
 * negative timeout value makes this function wait indefinitely, unless no
 * valid file descriptor is given, when this happens the negative timeout is
 * ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    N = number of signalled file descriptors
 */
अटल पूर्णांक our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  काष्ठा timeval pending_tv;
  काष्ठा timeval *ptimeout;

#अगर_घोषित USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  अगर((!fds_read || fds_read->fd_गिनती == 0) &&
     (!fds_write || fds_write->fd_गिनती == 0) &&
     (!fds_err || fds_err->fd_गिनती == 0)) {
    /* no sockets, wait */
    वापस curlx_wait_ms(timeout_ms);
  }
#पूर्ण_अगर

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#अगर_घोषित USE_WINSOCK
  /* Winsock select() must not be called with an fd_set that contains zero
     fd flags, or it will return WSAEINVAL. It also cannot be called with
     no fd_sets at all!  From the documentation:

     Any two of the parameters, readfds, writefds, or exceptfds, can be
     given as null. At least one must be non-null, and any non-null
     descriptor set must contain at least one handle to a socket.

     It is unclear why Winsock does not handle this for us instead of
     calling this an error. Luckily, with Winsock, we can _also_ ask how
     many bits are set on an fd_set. Therefore, let's check it beforehand.
   */
  वापस select((पूर्णांक)maxfd + 1,
                fds_read && fds_read->fd_गिनती ? fds_read : शून्य,
                fds_write && fds_write->fd_गिनती ? fds_write : शून्य,
                fds_err && fds_err->fd_गिनती ? fds_err : शून्य, ptimeout);
#अन्यथा
  वापस select((पूर्णांक)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#पूर्ण_अगर
}

#पूर्ण_अगर

/*
 * Wait for read or write events on a set of file descriptors. It uses poll()
 * when poll() is available, in order to avoid limits with FD_SETSIZE,
 * otherwise select() is used. An error is returned if select() is being used
 * and a file descriptor is too large for FD_SETSIZE.
 *
 * A negative timeout value makes this function wait indefinitely, unless no
 * valid file descriptor is given, when this happens the negative timeout is
 * ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    [bitmask] = action as described below
 *
 * CURL_CSELECT_IN - first socket is readable
 * CURL_CSELECT_IN2 - second socket is readable
 * CURL_CSELECT_OUT - write socket is writable
 * CURL_CSELECT_ERR - an error condition occurred
 */
पूर्णांक Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  काष्ठा pollfd pfd[3];
  पूर्णांक अंक;
  पूर्णांक r;

  अगर((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    वापस curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  अंक = 0;
  अगर(readfd0 != CURL_SOCKET_BAD) {
    pfd[अंक].fd = readfd0;
    pfd[अंक].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अंक].revents = 0;
    अंक++;
  }
  अगर(readfd1 != CURL_SOCKET_BAD) {
    pfd[अंक].fd = readfd1;
    pfd[अंक].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अंक].revents = 0;
    अंक++;
  }
  अगर(writefd != CURL_SOCKET_BAD) {
    pfd[अंक].fd = writefd;
    pfd[अंक].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[अंक].revents = 0;
    अंक++;
  }

  r = Curl_poll(pfd, (अचिन्हित पूर्णांक)अंक, timeout_ms);
  अगर(r <= 0)
    वापस r;

  r = 0;
  अंक = 0;
  अगर(readfd0 != CURL_SOCKET_BAD) {
    अगर(pfd[अंक].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    अगर(pfd[अंक].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अंक++;
  }
  अगर(readfd1 != CURL_SOCKET_BAD) {
    अगर(pfd[अंक].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    अगर(pfd[अंक].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अंक++;
  }
  अगर(writefd != CURL_SOCKET_BAD) {
    अगर(pfd[अंक].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    अगर(pfd[अंक].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  वापस r;
}

/*
 * This is a wrapper around poll(). If poll() does not exist, then
 * select() is used instead. An error is returned if select() is
 * being used and a file descriptor is too large for FD_SETSIZE.
 * A negative timeout value makes this function wait indefinitely,
 * unless no valid file descriptor is given, when this happens the
 * negative timeout is ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णांक Curl_poll(काष्ठा pollfd ufds[], अचिन्हित पूर्णांक nfds, timediff_t timeout_ms)
{
#अगर_घोषित HAVE_POLL
  पूर्णांक pending_ms;
#अन्यथा
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#पूर्ण_अगर
  bool fds_none = TRUE;
  अचिन्हित पूर्णांक i;
  पूर्णांक r;

  अगर(ufds) {
    क्रम(i = 0; i < nfds; i++) {
      अगर(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        अवरोध;
      }
    }
  }
  अगर(fds_none) {
    /* no sockets, wait */
    वापस curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#अगर_घोषित HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#अगर TIMEDIFF_T_MAX > पूर्णांक_उच्च
  अगर(timeout_ms > पूर्णांक_उच्च)
    timeout_ms = पूर्णांक_उच्च;
#पूर्ण_अगर
  अगर(timeout_ms > 0)
    pending_ms = (पूर्णांक)timeout_ms;
  अन्यथा अगर(timeout_ms < 0)
    pending_ms = -1;
  अन्यथा
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  अगर(r <= 0) {
    अगर((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    वापस r;
  }

  क्रम(i = 0; i < nfds; i++) {
    अगर(ufds[i].fd == CURL_SOCKET_BAD)
      जारी;
    अगर(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    अगर(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#अन्यथा /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  क्रम(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    अगर(ufds[i].fd == CURL_SOCKET_BAD)
      जारी;
    VERIFY_SOCK(ufds[i].fd);
    अगर(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      अगर(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      अगर(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      अगर(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      अगर(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  अगर(r <= 0) {
    अगर((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    वापस r;
  }

  r = 0;
  क्रम(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    अगर(ufds[i].fd == CURL_SOCKET_BAD)
      जारी;
    अगर(FD_ISSET(ufds[i].fd, &fds_read)) {
      अगर(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      अगर(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    अगर(FD_ISSET(ufds[i].fd, &fds_write)) {
      अगर(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      अगर(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    अगर(FD_ISSET(ufds[i].fd, &fds_err)) {
      अगर(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      अगर(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    अगर(ufds[i].revents)
      r++;
  }

#पूर्ण_अगर /* HAVE_POLL */

  वापस r;
}

व्योम Curl_pollfds_init(काष्ठा curl_pollfds *cpfds,
                       काष्ठा pollfd *static_pfds,
                       अचिन्हित पूर्णांक static_गिनती)
{
  DEBUGASSERT(cpfds);
  स_रखो(cpfds, 0, माप(*cpfds));
  अगर(static_pfds && static_गिनती) {
    cpfds->pfds = static_pfds;
    cpfds->गिनती = static_गिनती;
  }
}

व्योम Curl_pollfds_reset(काष्ठा curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

व्योम Curl_pollfds_cleanup(काष्ठा curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  अगर(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->गिनती = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

अटल CURLcode cpfds_increase(काष्ठा curl_pollfds *cpfds, अचिन्हित पूर्णांक inc)
{
  काष्ठा pollfd *new_fds;
  अचिन्हित पूर्णांक new_गिनती = cpfds->गिनती + inc;

  new_fds = curlx_calloc(new_गिनती, माप(काष्ठा pollfd));
  अगर(!new_fds)
    वापस CURLE_OUT_OF_MEMORY;

  स_नकल(new_fds, cpfds->pfds, cpfds->गिनती * माप(काष्ठा pollfd));
  अगर(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->गिनती = new_गिनती;
  cpfds->allocated_pfds = TRUE;
  वापस CURLE_OK;
}

अटल CURLcode cpfds_add_sock(काष्ठा curl_pollfds *cpfds,
                               curl_socket_t sock, लघु events, bool fold)
{
  पूर्णांक i;

  अगर(fold && cpfds->n <= पूर्णांक_उच्च) {
    क्रम(i = (पूर्णांक)cpfds->n - 1; i >= 0; --i) {
      अगर(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        वापस CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  अगर(cpfds->n >= cpfds->गिनती && cpfds_increase(cpfds, 100))
    वापस CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  वापस CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(काष्ठा curl_pollfds *cpfds,
                               curl_socket_t sock, लघु events)
{
  वापस cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(काष्ठा curl_pollfds *cpfds,
                             काष्ठा easy_pollset *ps)
{
  माप_प्रकार i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  क्रम(i = 0; i < ps->n; i++) {
    लघु events = 0;
    अगर(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    अगर(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    अगर(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      वापस CURLE_OUT_OF_MEMORY;
  }
  वापस CURLE_OK;
}

व्योम Curl_waitfds_init(काष्ठा Curl_waitfds *cwfds,
                       काष्ठा curl_waitfd *static_wfds,
                       अचिन्हित पूर्णांक static_गिनती)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_गिनती);
  स_रखो(cwfds, 0, माप(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->गिनती = static_गिनती;
}

अटल अचिन्हित पूर्णांक cwfds_add_sock(काष्ठा Curl_waitfds *cwfds,
                                   curl_socket_t sock, लघु events)
{
  पूर्णांक i;
  अगर(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->गिनती && !cwfds->n);
    वापस 1;
  }
  अगर(cwfds->n <= पूर्णांक_उच्च) {
    क्रम(i = (पूर्णांक)cwfds->n - 1; i >= 0; --i) {
      अगर(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        वापस 0;
      }
    }
  }
  /* not folded, add new entry */
  अगर(cwfds->n < cwfds->गिनती) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  वापस 1;
}

अचिन्हित पूर्णांक Curl_waitfds_add_ps(काष्ठा Curl_waitfds *cwfds,
                                 काष्ठा easy_pollset *ps)
{
  माप_प्रकार i;
  अचिन्हित पूर्णांक need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  क्रम(i = 0; i < ps->n; i++) {
    लघु events = 0;
    अगर(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    अगर(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    अगर(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  वापस need;
}

व्योम Curl_pollset_reset(काष्ठा easy_pollset *ps)
{
#अगर_घोषित DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#पूर्ण_अगर
  ps->n = 0;
}

व्योम Curl_pollset_init(काष्ठा easy_pollset *ps)
{
#अगर_घोषित DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#पूर्ण_अगर
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->गिनती = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

काष्ठा easy_pollset *Curl_pollset_create(व्योम)
{
  काष्ठा easy_pollset *ps = curlx_calloc(1, माप(*ps));
  अगर(ps)
    Curl_pollset_init(ps);
  वापस ps;
}

व्योम Curl_pollset_cleanup(काष्ठा easy_pollset *ps)
{
#अगर_घोषित DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#पूर्ण_अगर
  अगर(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  अगर(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->गिनती = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

व्योम Curl_pollset_move(काष्ठा easy_pollset *to, काष्ठा easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  अगर(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->गिनती = from->गिनती;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  अन्यथा {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    स_नकल(to->sockets, from->sockets, to->गिनती * माप(to->sockets[0]));
    स_नकल(to->actions, from->actions, to->गिनती * माप(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps, curl_socket_t sock,
                             पूर्णांक add_flags, पूर्णांक remove_flags)
{
  अचिन्हित पूर्णांक i;

#अगर_घोषित DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#पूर्ण_अगर

  DEBUGASSERT(VALID_SOCK(sock));
  अगर(!VALID_SOCK(sock))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  क्रम(i = 0; i < ps->n; ++i) {
    अगर(ps->sockets[i] == sock) {
      ps->actions[i] &= (अचिन्हित अक्षर)(~remove_flags);
      ps->actions[i] |= (अचिन्हित अक्षर)add_flags;
      /* all gone? remove socket */
      अगर(!ps->actions[i]) {
        अगर((i + 1) < ps->n) {
          स_हटाओ(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * माप(ps->sockets[0]));
          स_हटाओ(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * माप(ps->actions[0]));
        }
        --ps->n;
      }
      वापस CURLE_OK;
    }
  }
  /* not present */
  अगर(add_flags) {
    अगर(i >= ps->गिनती) { /* need to grow */
      अचिन्हित पूर्णांक new_गिनती = CURLMAX(ps->गिनती * 2, 8);
      curl_socket_t *nsockets;
      अचिन्हित अक्षर *nactions;

      CURL_TRC_M(तथ्य, "growing pollset capacity from %u to %u",
                 ps->गिनती, new_गिनती);
      अगर(new_गिनती <= ps->गिनती)
        वापस CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_गिनती, माप(nsockets[0]));
      अगर(!nsockets)
        वापस CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_गिनती, माप(nactions[0]));
      अगर(!nactions) {
        curlx_free(nsockets);
        वापस CURLE_OUT_OF_MEMORY;
      }
      स_नकल(nsockets, ps->sockets, ps->गिनती * माप(ps->sockets[0]));
      स_नकल(nactions, ps->actions, ps->गिनती * माप(ps->actions[0]));
      अगर(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      अगर(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->गिनती = new_गिनती;
    }
    DEBUGASSERT(i < ps->गिनती);
    अगर(i < ps->गिनती) {
      ps->sockets[i] = sock;
      ps->actions[i] = (अचिन्हित अक्षर)add_flags;
      ps->n = i + 1;
    }
  }
  वापस CURLE_OK;
}

CURLcode Curl_pollset_set(काष्ठा Curl_easy *तथ्य,
                          काष्ठा easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  वापस Curl_pollset_change(तथ्य, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

व्योम Curl_pollset_remove(काष्ठा easy_pollset *ps, curl_socket_t sock)
{
  अचिन्हित पूर्णांक i;
  क्रम(i = 0; i < ps->n; ++i) {
    अगर(ps->sockets[i] == sock) {
      अगर((i + 1) < ps->n) {
        स_हटाओ(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * माप(ps->sockets[0]));
        स_हटाओ(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * माप(ps->actions[0]));
      }
      --ps->n;
      वापस;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णांक Curl_pollset_poll(काष्ठा Curl_easy *तथ्य,
                      काष्ठा easy_pollset *ps,
                      timediff_t timeout_ms)
{
  काष्ठा pollfd *pfds;
  अचिन्हित पूर्णांक i, npfds;
  पूर्णांक rc;

  (व्योम)तथ्य;
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);

  अगर(!ps->n)
    वापस curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, माप(*pfds));
  अगर(!pfds)
    वापस -1;

  npfds = 0;
  क्रम(i = 0; i < ps->n; ++i) {
    लघु events = 0;
    अगर(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    अगर(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    अगर(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  वापस rc;
}

व्योम Curl_pollset_check(काष्ठा Curl_easy *तथ्य,
                        काष्ठा easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  अचिन्हित पूर्णांक i;

  (व्योम)तथ्य;
  DEBUGASSERT(VALID_SOCK(sock));
  क्रम(i = 0; i < ps->n; ++i) {
    अगर(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      वापस;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(काष्ठा Curl_easy *तथ्य,
                            काष्ठा easy_pollset *ps,
                            curl_socket_t sock)
{
  अचिन्हित पूर्णांक i;
  (व्योम)तथ्य;
  क्रम(i = 0; i < ps->n; ++i) {
    अगर((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      वापस TRUE;
  }
  वापस FALSE;
}

bool Curl_pollset_want_send(काष्ठा Curl_easy *तथ्य,
                            काष्ठा easy_pollset *ps,
                            curl_socket_t sock)
{
  अचिन्हित पूर्णांक i;
  (व्योम)तथ्य;
  क्रम(i = 0; i < ps->n; ++i) {
    अगर((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      वापस TRUE;
  }
  वापस FALSE;
}
