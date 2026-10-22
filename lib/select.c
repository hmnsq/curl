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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#অন্তর্ভাব <sys/select.h>
#নতুবা_যদি defined(HAVE_UNISTD_H)
#অন্তর্ভাব <unistd.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/timediff.h"
#অন্তর্ভাব "curlx/wait.h"

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
স্থির পূর্ণসংখ্যা our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  কাঠামো timeval pending_tv;
  কাঠামো timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  যদি((!fds_read || fds_read->fd_গুনতি == 0) &&
     (!fds_write || fds_write->fd_গুনতি == 0) &&
     (!fds_err || fds_err->fd_গুনতি == 0)) {
    /* no sockets, wait */
    ফেরত curlx_wait_ms(timeout_ms);
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
  ফেরত select((পূর্ণসংখ্যা)maxfd + 1,
                fds_read && fds_read->fd_গুনতি ? fds_read : ফাঁকা,
                fds_write && fds_write->fd_গুনতি ? fds_write : ফাঁকা,
                fds_err && fds_err->fd_গুনতি ? fds_err : ফাঁকা, ptimeout);
#অন্যথা
  ফেরত select((পূর্ণসংখ্যা)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
পূর্ণসংখ্যা Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  কাঠামো pollfd pfd[3];
  পূর্ণসংখ্যা অঙ্ক;
  পূর্ণসংখ্যা r;

  যদি((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ফেরত curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  অঙ্ক = 0;
  যদি(readfd0 != CURL_SOCKET_BAD) {
    pfd[অঙ্ক].fd = readfd0;
    pfd[অঙ্ক].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[অঙ্ক].revents = 0;
    অঙ্ক++;
  }
  যদি(readfd1 != CURL_SOCKET_BAD) {
    pfd[অঙ্ক].fd = readfd1;
    pfd[অঙ্ক].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[অঙ্ক].revents = 0;
    অঙ্ক++;
  }
  যদি(writefd != CURL_SOCKET_BAD) {
    pfd[অঙ্ক].fd = writefd;
    pfd[অঙ্ক].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[অঙ্ক].revents = 0;
    অঙ্ক++;
  }

  r = Curl_poll(pfd, (unsigned পূর্ণসংখ্যা)অঙ্ক, timeout_ms);
  যদি(r <= 0)
    ফেরত r;

  r = 0;
  অঙ্ক = 0;
  যদি(readfd0 != CURL_SOCKET_BAD) {
    যদি(pfd[অঙ্ক].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    যদি(pfd[অঙ্ক].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    অঙ্ক++;
  }
  যদি(readfd1 != CURL_SOCKET_BAD) {
    যদি(pfd[অঙ্ক].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    যদি(pfd[অঙ্ক].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    অঙ্ক++;
  }
  যদি(writefd != CURL_SOCKET_BAD) {
    যদি(pfd[অঙ্ক].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    যদি(pfd[অঙ্ক].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ফেরত r;
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
পূর্ণসংখ্যা Curl_poll(কাঠামো pollfd ufds[], unsigned পূর্ণসংখ্যা nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  পূর্ণসংখ্যা pending_ms;
#অন্যথা
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned পূর্ণসংখ্যা i;
  পূর্ণসংখ্যা r;

  যদি(ufds) {
    জন্য(i = 0; i < nfds; i++) {
      যদি(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        বিরতি;
      }
    }
  }
  যদি(fds_none) {
    /* no sockets, wait */
    ফেরত curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#যদি TIMEDIFF_T_MAX > INT_MAX
  যদি(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  যদি(timeout_ms > 0)
    pending_ms = (পূর্ণসংখ্যা)timeout_ms;
  অন্যথা যদি(timeout_ms < 0)
    pending_ms = -1;
  অন্যথা
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  যদি(r <= 0) {
    যদি((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ফেরত r;
  }

  জন্য(i = 0; i < nfds; i++) {
    যদি(ufds[i].fd == CURL_SOCKET_BAD)
      চালু;
    যদি(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    যদি(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#অন্যথা /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  জন্য(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    যদি(ufds[i].fd == CURL_SOCKET_BAD)
      চালু;
    VERIFY_SOCK(ufds[i].fd);
    যদি(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      যদি(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      যদি(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      যদি(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      যদি(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  যদি(r <= 0) {
    যদি((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ফেরত r;
  }

  r = 0;
  জন্য(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    যদি(ufds[i].fd == CURL_SOCKET_BAD)
      চালু;
    যদি(FD_ISSET(ufds[i].fd, &fds_read)) {
      যদি(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      যদি(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    যদি(FD_ISSET(ufds[i].fd, &fds_write)) {
      যদি(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      যদি(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    যদি(FD_ISSET(ufds[i].fd, &fds_err)) {
      যদি(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      যদি(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    যদি(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  ফেরত r;
}

শূন্য Curl_pollfds_init(কাঠামো curl_pollfds *cpfds,
                       কাঠামো pollfd *static_pfds,
                       unsigned পূর্ণসংখ্যা static_গুনতি)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, আকার(*cpfds));
  যদি(static_pfds && static_গুনতি) {
    cpfds->pfds = static_pfds;
    cpfds->গুনতি = static_গুনতি;
  }
}

শূন্য Curl_pollfds_reset(কাঠামো curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

শূন্য Curl_pollfds_cleanup(কাঠামো curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  যদি(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->গুনতি = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

স্থির CURLcode cpfds_increase(কাঠামো curl_pollfds *cpfds, unsigned পূর্ণসংখ্যা inc)
{
  কাঠামো pollfd *new_fds;
  unsigned পূর্ণসংখ্যা new_গুনতি = cpfds->গুনতি + inc;

  new_fds = curlx_calloc(new_গুনতি, আকার(কাঠামো pollfd));
  যদি(!new_fds)
    ফেরত CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->গুনতি * আকার(কাঠামো pollfd));
  যদি(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->গুনতি = new_গুনতি;
  cpfds->allocated_pfds = TRUE;
  ফেরত CURLE_OK;
}

স্থির CURLcode cpfds_add_sock(কাঠামো curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  পূর্ণসংখ্যা i;

  যদি(fold && cpfds->n <= INT_MAX) {
    জন্য(i = (পূর্ণসংখ্যা)cpfds->n - 1; i >= 0; --i) {
      যদি(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ফেরত CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  যদি(cpfds->n >= cpfds->গুনতি && cpfds_increase(cpfds, 100))
    ফেরত CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ফেরত CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(কাঠামো curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  ফেরত cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(কাঠামো curl_pollfds *cpfds,
                             কাঠামো easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  জন্য(i = 0; i < ps->n; i++) {
    short events = 0;
    যদি(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    যদি(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    যদি(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      ফেরত CURLE_OUT_OF_MEMORY;
  }
  ফেরত CURLE_OK;
}

শূন্য Curl_waitfds_init(কাঠামো Curl_waitfds *cwfds,
                       কাঠামো curl_waitfd *static_wfds,
                       unsigned পূর্ণসংখ্যা static_গুনতি)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_গুনতি);
  memset(cwfds, 0, আকার(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->গুনতি = static_গুনতি;
}

স্থির unsigned পূর্ণসংখ্যা cwfds_add_sock(কাঠামো Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  পূর্ণসংখ্যা i;
  যদি(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->গুনতি && !cwfds->n);
    ফেরত 1;
  }
  যদি(cwfds->n <= INT_MAX) {
    জন্য(i = (পূর্ণসংখ্যা)cwfds->n - 1; i >= 0; --i) {
      যদি(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ফেরত 0;
      }
    }
  }
  /* not folded, add new entry */
  যদি(cwfds->n < cwfds->গুনতি) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ফেরত 1;
}

unsigned পূর্ণসংখ্যা Curl_waitfds_add_ps(কাঠামো Curl_waitfds *cwfds,
                                 কাঠামো easy_pollset *ps)
{
  size_t i;
  unsigned পূর্ণসংখ্যা need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  জন্য(i = 0; i < ps->n; i++) {
    short events = 0;
    যদি(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    যদি(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    যদি(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  ফেরত need;
}

শূন্য Curl_pollset_reset(কাঠামো easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

শূন্য Curl_pollset_init(কাঠামো easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->গুনতি = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

কাঠামো easy_pollset *Curl_pollset_create(শূন্য)
{
  কাঠামো easy_pollset *ps = curlx_calloc(1, আকার(*ps));
  যদি(ps)
    Curl_pollset_init(ps);
  ফেরত ps;
}

শূন্য Curl_pollset_cleanup(কাঠামো easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  যদি(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  যদি(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->গুনতি = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

শূন্য Curl_pollset_move(কাঠামো easy_pollset *to, কাঠামো easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  যদি(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->গুনতি = from->গুনতি;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  অন্যথা {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->গুনতি * আকার(to->sockets[0]));
    memcpy(to->actions, from->actions, to->গুনতি * আকার(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps, curl_socket_t sock,
                             পূর্ণসংখ্যা add_flags, পূর্ণসংখ্যা remove_flags)
{
  unsigned পূর্ণসংখ্যা i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  যদি(!VALID_SOCK(sock))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  জন্য(i = 0; i < ps->n; ++i) {
    যদি(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned অক্ষর)(~remove_flags);
      ps->actions[i] |= (unsigned অক্ষর)add_flags;
      /* all gone? remove socket */
      যদি(!ps->actions[i]) {
        যদি((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * আকার(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * আকার(ps->actions[0]));
        }
        --ps->n;
      }
      ফেরত CURLE_OK;
    }
  }
  /* not present */
  যদি(add_flags) {
    যদি(i >= ps->গুনতি) { /* need to grow */
      unsigned পূর্ণসংখ্যা new_গুনতি = CURLMAX(ps->গুনতি * 2, 8);
      curl_socket_t *nsockets;
      unsigned অক্ষর *nactions;

      CURL_TRC_M(তথ্য, "growing pollset capacity from %u to %u",
                 ps->গুনতি, new_গুনতি);
      যদি(new_গুনতি <= ps->গুনতি)
        ফেরত CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_গুনতি, আকার(nsockets[0]));
      যদি(!nsockets)
        ফেরত CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_গুনতি, আকার(nactions[0]));
      যদি(!nactions) {
        curlx_free(nsockets);
        ফেরত CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->গুনতি * আকার(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->গুনতি * আকার(ps->actions[0]));
      যদি(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      যদি(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->গুনতি = new_গুনতি;
    }
    DEBUGASSERT(i < ps->গুনতি);
    যদি(i < ps->গুনতি) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned অক্ষর)add_flags;
      ps->n = i + 1;
    }
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_pollset_set(কাঠামো Curl_easy *তথ্য,
                          কাঠামো easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  ফেরত Curl_pollset_change(তথ্য, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

শূন্য Curl_pollset_remove(কাঠামো easy_pollset *ps, curl_socket_t sock)
{
  unsigned পূর্ণসংখ্যা i;
  জন্য(i = 0; i < ps->n; ++i) {
    যদি(ps->sockets[i] == sock) {
      যদি((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * আকার(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * আকার(ps->actions[0]));
      }
      --ps->n;
      ফেরত;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
পূর্ণসংখ্যা Curl_pollset_poll(কাঠামো Curl_easy *তথ্য,
                      কাঠামো easy_pollset *ps,
                      timediff_t timeout_ms)
{
  কাঠামো pollfd *pfds;
  unsigned পূর্ণসংখ্যা i, npfds;
  পূর্ণসংখ্যা rc;

  (শূন্য)তথ্য;
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);

  যদি(!ps->n)
    ফেরত curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, আকার(*pfds));
  যদি(!pfds)
    ফেরত -1;

  npfds = 0;
  জন্য(i = 0; i < ps->n; ++i) {
    short events = 0;
    যদি(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    যদি(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    যদি(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ফেরত rc;
}

শূন্য Curl_pollset_check(কাঠামো Curl_easy *তথ্য,
                        কাঠামো easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned পূর্ণসংখ্যা i;

  (শূন্য)তথ্য;
  DEBUGASSERT(VALID_SOCK(sock));
  জন্য(i = 0; i < ps->n; ++i) {
    যদি(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      ফেরত;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(কাঠামো Curl_easy *তথ্য,
                            কাঠামো easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned পূর্ণসংখ্যা i;
  (শূন্য)তথ্য;
  জন্য(i = 0; i < ps->n; ++i) {
    যদি((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      ফেরত TRUE;
  }
  ফেরত FALSE;
}

bool Curl_pollset_want_send(কাঠামো Curl_easy *তথ্য,
                            কাঠামো easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned পূর্ণসংখ্যা i;
  (শূন্য)তথ্য;
  জন্য(i = 0; i < ps->n; ++i) {
    যদি((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      ফেরত TRUE;
  }
  ফেরত FALSE;
}
