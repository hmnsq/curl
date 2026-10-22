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

#ifdef HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#endif

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curl_trc.h"
#ضمن "curlx/timediff.h"
#ضمن "curlx/wait.h"

#ifndef HAVE_POLL  /* use select() */
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
ساكن صحيح our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  بنية timeval pending_tv;
  بنية timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  إذا((!fds_read || fds_read->fd_گنتی == 0) &&
     (!fds_write || fds_write->fd_گنتی == 0) &&
     (!fds_err || fds_err->fd_گنتی == 0)) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }
#endif

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#ifdef USE_WINSOCK
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
  أرجع select((صحيح)maxfd + 1,
                fds_read && fds_read->fd_گنتی ? fds_read : عدم,
                fds_write && fds_write->fd_گنتی ? fds_write : عدم,
                fds_err && fds_err->fd_گنتی ? fds_err : عدم, ptimeout);
#وإلا
  أرجع select((صحيح)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#endif
}

#endif

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
صحيح Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  بنية pollfd pfd[3];
  صحيح عدد;
  صحيح r;

  إذا((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  عدد = 0;
  إذا(readfd0 != CURL_SOCKET_BAD) {
    pfd[عدد].fd = readfd0;
    pfd[عدد].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[عدد].revents = 0;
    عدد++;
  }
  إذا(readfd1 != CURL_SOCKET_BAD) {
    pfd[عدد].fd = readfd1;
    pfd[عدد].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[عدد].revents = 0;
    عدد++;
  }
  إذا(writefd != CURL_SOCKET_BAD) {
    pfd[عدد].fd = writefd;
    pfd[عدد].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[عدد].revents = 0;
    عدد++;
  }

  r = Curl_poll(pfd, (unsigned صحيح)عدد, timeout_ms);
  إذا(r <= 0)
    أرجع r;

  r = 0;
  عدد = 0;
  إذا(readfd0 != CURL_SOCKET_BAD) {
    إذا(pfd[عدد].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    إذا(pfd[عدد].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    عدد++;
  }
  إذا(readfd1 != CURL_SOCKET_BAD) {
    إذا(pfd[عدد].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    إذا(pfd[عدد].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    عدد++;
  }
  إذا(writefd != CURL_SOCKET_BAD) {
    إذا(pfd[عدد].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    إذا(pfd[عدد].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  أرجع r;
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
صحيح Curl_poll(بنية pollfd ufds[], unsigned صحيح nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  صحيح pending_ms;
#وإلا
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned صحيح i;
  صحيح r;

  إذا(ufds) {
    لكل(i = 0; i < nfds; i++) {
      إذا(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        اكسر;
      }
    }
  }
  إذا(fds_none) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#إذا TIMEDIFF_T_MAX > INT_MAX
  إذا(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  إذا(timeout_ms > 0)
    pending_ms = (صحيح)timeout_ms;
  وإلا إذا(timeout_ms < 0)
    pending_ms = -1;
  وإلا
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  إذا(r <= 0) {
    إذا((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    أرجع r;
  }

  لكل(i = 0; i < nfds; i++) {
    إذا(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    إذا(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    إذا(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#وإلا /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  لكل(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    إذا(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    VERIFY_SOCK(ufds[i].fd);
    إذا(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      إذا(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      إذا(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      إذا(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      إذا(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  إذا(r <= 0) {
    إذا((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    أرجع r;
  }

  r = 0;
  لكل(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    إذا(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    إذا(FD_ISSET(ufds[i].fd, &fds_read)) {
      إذا(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      إذا(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    إذا(FD_ISSET(ufds[i].fd, &fds_write)) {
      إذا(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      إذا(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    إذا(FD_ISSET(ufds[i].fd, &fds_err)) {
      إذا(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      إذا(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    إذا(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  أرجع r;
}

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       unsigned صحيح static_گنتی)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, حجم(*cpfds));
  إذا(static_pfds && static_گنتی) {
    cpfds->pfds = static_pfds;
    cpfds->گنتی = static_گنتی;
  }
}

فراغ Curl_pollfds_reset(بنية curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

فراغ Curl_pollfds_cleanup(بنية curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  إذا(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->گنتی = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ساكن CURLcode cpfds_increase(بنية curl_pollfds *cpfds, unsigned صحيح inc)
{
  بنية pollfd *new_fds;
  unsigned صحيح new_گنتی = cpfds->گنتی + inc;

  new_fds = curlx_calloc(new_گنتی, حجم(بنية pollfd));
  إذا(!new_fds)
    أرجع CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->گنتی * حجم(بنية pollfd));
  إذا(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->گنتی = new_گنتی;
  cpfds->allocated_pfds = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cpfds_add_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  صحيح i;

  إذا(fold && cpfds->n <= INT_MAX) {
    لكل(i = (صحيح)cpfds->n - 1; i >= 0; --i) {
      إذا(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        أرجع CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  إذا(cpfds->n >= cpfds->گنتی && cpfds_increase(cpfds, 100))
    أرجع CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  أرجع CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  أرجع cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    short events = 0;
    إذا(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    إذا(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    إذا(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      أرجع CURLE_OUT_OF_MEMORY;
  }
  أرجع CURLE_OK;
}

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       unsigned صحيح static_گنتی)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_گنتی);
  memset(cwfds, 0, حجم(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->گنتی = static_گنتی;
}

ساكن unsigned صحيح cwfds_add_sock(بنية Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  صحيح i;
  إذا(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->گنتی && !cwfds->n);
    أرجع 1;
  }
  إذا(cwfds->n <= INT_MAX) {
    لكل(i = (صحيح)cwfds->n - 1; i >= 0; --i) {
      إذا(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        أرجع 0;
      }
    }
  }
  /* not folded, add new entry */
  إذا(cwfds->n < cwfds->گنتی) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  أرجع 1;
}

unsigned صحيح Curl_waitfds_add_ps(بنية Curl_waitfds *cwfds,
                                 بنية easy_pollset *ps)
{
  size_t i;
  unsigned صحيح need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    short events = 0;
    إذا(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    إذا(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    إذا(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  أرجع need;
}

فراغ Curl_pollset_reset(بنية easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

فراغ Curl_pollset_init(بنية easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->گنتی = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

بنية easy_pollset *Curl_pollset_create(فراغ)
{
  بنية easy_pollset *ps = curlx_calloc(1, حجم(*ps));
  إذا(ps)
    Curl_pollset_init(ps);
  أرجع ps;
}

فراغ Curl_pollset_cleanup(بنية easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  إذا(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  إذا(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->گنتی = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

فراغ Curl_pollset_move(بنية easy_pollset *to, بنية easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  إذا(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->گنتی = from->گنتی;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  وإلا {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->گنتی * حجم(to->sockets[0]));
    memcpy(to->actions, from->actions, to->گنتی * حجم(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(بنية Curl_easy *data,
                             بنية easy_pollset *ps, curl_socket_t sock,
                             صحيح add_flags, صحيح remove_flags)
{
  unsigned صحيح i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  إذا(!VALID_SOCK(sock))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned حرف)(~remove_flags);
      ps->actions[i] |= (unsigned حرف)add_flags;
      /* all gone? remove socket */
      إذا(!ps->actions[i]) {
        إذا((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->actions[0]));
        }
        --ps->n;
      }
      أرجع CURLE_OK;
    }
  }
  /* not present */
  إذا(add_flags) {
    إذا(i >= ps->گنتی) { /* need to grow */
      unsigned صحيح new_گنتی = CURLMAX(ps->گنتی * 2, 8);
      curl_socket_t *nsockets;
      unsigned حرف *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->گنتی, new_گنتی);
      إذا(new_گنتی <= ps->گنتی)
        أرجع CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_گنتی, حجم(nsockets[0]));
      إذا(!nsockets)
        أرجع CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_گنتی, حجم(nactions[0]));
      إذا(!nactions) {
        curlx_free(nsockets);
        أرجع CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->گنتی * حجم(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->گنتی * حجم(ps->actions[0]));
      إذا(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      إذا(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->گنتی = new_گنتی;
    }
    DEBUGASSERT(i < ps->گنتی);
    إذا(i < ps->گنتی) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned حرف)add_flags;
      ps->n = i + 1;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_pollset_set(بنية Curl_easy *data,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  أرجع Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

فراغ Curl_pollset_remove(بنية easy_pollset *ps, curl_socket_t sock)
{
  unsigned صحيح i;
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      إذا((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * حجم(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * حجم(ps->actions[0]));
      }
      --ps->n;
      أرجع;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحيح Curl_pollset_poll(بنية Curl_easy *data,
                      بنية easy_pollset *ps,
                      timediff_t timeout_ms)
{
  بنية pollfd *pfds;
  unsigned صحيح i, npfds;
  صحيح rc;

  (فراغ)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  إذا(!ps->n)
    أرجع curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, حجم(*pfds));
  إذا(!pfds)
    أرجع -1;

  npfds = 0;
  لكل(i = 0; i < ps->n; ++i) {
    short events = 0;
    إذا(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    إذا(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    إذا(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  أرجع rc;
}

فراغ Curl_pollset_check(بنية Curl_easy *data,
                        بنية easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned صحيح i;

  (فراغ)data;
  DEBUGASSERT(VALID_SOCK(sock));
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      أرجع;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(بنية Curl_easy *data,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned صحيح i;
  (فراغ)data;
  لكل(i = 0; i < ps->n; ++i) {
    إذا((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      أرجع TRUE;
  }
  أرجع FALSE;
}

bool Curl_pollset_want_send(بنية Curl_easy *data,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned صحيح i;
  (فراغ)data;
  لكل(i = 0; i < ps->n; ++i) {
    إذا((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      أرجع TRUE;
  }
  أرجع FALSE;
}
