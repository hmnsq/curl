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
  ܐܢ((!fds_read || fds_read->fd_count == 0) &&
     (!fds_write || fds_write->fd_count == 0) &&
     (!fds_err || fds_err->fd_count == 0)) {
    /* no sockets, wait */
    ܦܢܐ curlx_wait_ms(timeout_ms);
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
  ܦܢܐ select((صحيح)maxfd + 1,
                fds_read && fds_read->fd_count ? fds_read : عدم,
                fds_write && fds_write->fd_count ? fds_write : عدم,
                fds_err && fds_err->fd_count ? fds_err : عدم, ptimeout);
#ܐܚܪܢܐ
  ܦܢܐ select((صحيح)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
  صحيح num;
  صحيح r;

  ܐܢ((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ܦܢܐ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  num = 0;
  ܐܢ(readfd0 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd0;
    pfd[num].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  ܐܢ(readfd1 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd1;
    pfd[num].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  ܐܢ(writefd != CURL_SOCKET_BAD) {
    pfd[num].fd = writefd;
    pfd[num].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }

  r = Curl_poll(pfd, (unsigned صحيح)num, timeout_ms);
  ܐܢ(r <= 0)
    ܦܢܐ r;

  r = 0;
  num = 0;
  ܐܢ(readfd0 != CURL_SOCKET_BAD) {
    ܐܢ(pfd[num].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    ܐܢ(pfd[num].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    num++;
  }
  ܐܢ(readfd1 != CURL_SOCKET_BAD) {
    ܐܢ(pfd[num].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ܐܢ(pfd[num].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    num++;
  }
  ܐܢ(writefd != CURL_SOCKET_BAD) {
    ܐܢ(pfd[num].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    ܐܢ(pfd[num].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ܦܢܐ r;
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
#ܐܚܪܢܐ
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned صحيح i;
  صحيح r;

  ܐܢ(ufds) {
    لكل(i = 0; i < nfds; i++) {
      ܐܢ(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        اكسر;
      }
    }
  }
  ܐܢ(fds_none) {
    /* no sockets, wait */
    ܦܢܐ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ܐܢ TIMEDIFF_T_MAX > INT_MAX
  ܐܢ(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  ܐܢ(timeout_ms > 0)
    pending_ms = (صحيح)timeout_ms;
  ܐܚܪܢܐ ܐܢ(timeout_ms < 0)
    pending_ms = -1;
  ܐܚܪܢܐ
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ܐܢ(r <= 0) {
    ܐܢ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ܦܢܐ r;
  }

  لكل(i = 0; i < nfds; i++) {
    ܐܢ(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    ܐܢ(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ܐܢ(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#ܐܚܪܢܐ /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  لكل(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ܐܢ(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    VERIFY_SOCK(ufds[i].fd);
    ܐܢ(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ܐܢ(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ܐܢ(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ܐܢ(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ܐܢ(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ܐܢ(r <= 0) {
    ܐܢ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ܦܢܐ r;
  }

  r = 0;
  لكل(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ܐܢ(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    ܐܢ(FD_ISSET(ufds[i].fd, &fds_read)) {
      ܐܢ(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ܐܢ(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ܐܢ(FD_ISSET(ufds[i].fd, &fds_write)) {
      ܐܢ(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ܐܢ(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ܐܢ(FD_ISSET(ufds[i].fd, &fds_err)) {
      ܐܢ(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ܐܢ(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ܐܢ(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  ܦܢܐ r;
}

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       unsigned صحيح static_count)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, حجم(*cpfds));
  ܐܢ(static_pfds && static_count) {
    cpfds->pfds = static_pfds;
    cpfds->count = static_count;
  }
}

فراغ Curl_pollfds_reset(بنية curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

فراغ Curl_pollfds_cleanup(بنية curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ܐܢ(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->count = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ساكن CURLcode cpfds_increase(بنية curl_pollfds *cpfds, unsigned صحيح inc)
{
  بنية pollfd *new_fds;
  unsigned صحيح new_count = cpfds->count + inc;

  new_fds = curlx_calloc(new_count, حجم(بنية pollfd));
  ܐܢ(!new_fds)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->count * حجم(بنية pollfd));
  ܐܢ(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->count = new_count;
  cpfds->allocated_pfds = TRUE;
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cpfds_add_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  صحيح i;

  ܐܢ(fold && cpfds->n <= INT_MAX) {
    لكل(i = (صحيح)cpfds->n - 1; i >= 0; --i) {
      ܐܢ(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ܦܢܐ CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ܐܢ(cpfds->n >= cpfds->count && cpfds_increase(cpfds, 100))
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  ܦܢܐ cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    short events = 0;
    ܐܢ(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    ܐܢ(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    ܐܢ(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      ܦܢܐ CURLE_OUT_OF_MEMORY;
  }
  ܦܢܐ CURLE_OK;
}

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       unsigned صحيح static_count)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_count);
  memset(cwfds, 0, حجم(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->count = static_count;
}

ساكن unsigned صحيح cwfds_add_sock(بنية Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  صحيح i;
  ܐܢ(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->count && !cwfds->n);
    ܦܢܐ 1;
  }
  ܐܢ(cwfds->n <= INT_MAX) {
    لكل(i = (صحيح)cwfds->n - 1; i >= 0; --i) {
      ܐܢ(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ܦܢܐ 0;
      }
    }
  }
  /* not folded, add new entry */
  ܐܢ(cwfds->n < cwfds->count) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ܦܢܐ 1;
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
    ܐܢ(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    ܐܢ(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    ܐܢ(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  ܦܢܐ need;
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
  ps->count = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

بنية easy_pollset *Curl_pollset_create(فراغ)
{
  بنية easy_pollset *ps = curlx_calloc(1, حجم(*ps));
  ܐܢ(ps)
    Curl_pollset_init(ps);
  ܦܢܐ ps;
}

فراغ Curl_pollset_cleanup(بنية easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ܐܢ(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ܐܢ(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->count = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

فراغ Curl_pollset_move(بنية easy_pollset *to, بنية easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  ܐܢ(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->count = from->count;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  ܐܚܪܢܐ {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->count * حجم(to->sockets[0]));
    memcpy(to->actions, from->actions, to->count * حجم(to->actions[0]));
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
  ܐܢ(!VALID_SOCK(sock))
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  لكل(i = 0; i < ps->n; ++i) {
    ܐܢ(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned حرف)(~remove_flags);
      ps->actions[i] |= (unsigned حرف)add_flags;
      /* all gone? remove socket */
      ܐܢ(!ps->actions[i]) {
        ܐܢ((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->actions[0]));
        }
        --ps->n;
      }
      ܦܢܐ CURLE_OK;
    }
  }
  /* not present */
  ܐܢ(add_flags) {
    ܐܢ(i >= ps->count) { /* need to grow */
      unsigned صحيح new_count = CURLMAX(ps->count * 2, 8);
      curl_socket_t *nsockets;
      unsigned حرف *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->count, new_count);
      ܐܢ(new_count <= ps->count)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_count, حجم(nsockets[0]));
      ܐܢ(!nsockets)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_count, حجم(nactions[0]));
      ܐܢ(!nactions) {
        curlx_free(nsockets);
        ܦܢܐ CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->count * حجم(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->count * حجم(ps->actions[0]));
      ܐܢ(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ܐܢ(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->count = new_count;
    }
    DEBUGASSERT(i < ps->count);
    ܐܢ(i < ps->count) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned حرف)add_flags;
      ps->n = i + 1;
    }
  }
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_pollset_set(بنية Curl_easy *data,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  ܦܢܐ Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

فراغ Curl_pollset_remove(بنية easy_pollset *ps, curl_socket_t sock)
{
  unsigned صحيح i;
  لكل(i = 0; i < ps->n; ++i) {
    ܐܢ(ps->sockets[i] == sock) {
      ܐܢ((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * حجم(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * حجم(ps->actions[0]));
      }
      --ps->n;
      ܦܢܐ;
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

  ܐܢ(!ps->n)
    ܦܢܐ curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, حجم(*pfds));
  ܐܢ(!pfds)
    ܦܢܐ -1;

  npfds = 0;
  لكل(i = 0; i < ps->n; ++i) {
    short events = 0;
    ܐܢ(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    ܐܢ(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    ܐܢ(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ܦܢܐ rc;
}

فراغ Curl_pollset_check(بنية Curl_easy *data,
                        بنية easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned صحيح i;

  (فراغ)data;
  DEBUGASSERT(VALID_SOCK(sock));
  لكل(i = 0; i < ps->n; ++i) {
    ܐܢ(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      ܦܢܐ;
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
    ܐܢ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      ܦܢܐ TRUE;
  }
  ܦܢܐ FALSE;
}

bool Curl_pollset_want_send(بنية Curl_easy *data,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned صحيح i;
  (فراغ)data;
  لكل(i = 0; i < ps->n; ++i) {
    ܐܢ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      ܦܢܐ TRUE;
  }
  ܦܢܐ FALSE;
}
