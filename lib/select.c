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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#അന്തര്ഭാവ <sys/select.h>
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_UNISTD_H)
#അന്തര്ഭാവ <unistd.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "curlx/timediff.h"
#അന്തര്ഭാവ "curlx/wait.h"

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
സ്ഥിതം പൂർണ്ണസംഖ്യ our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ഘടന timeval pending_tv;
  ഘടന timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  എങ്കിൽ((!fds_read || fds_read->fd_എണ്ണം == 0) &&
     (!fds_write || fds_write->fd_എണ്ണം == 0) &&
     (!fds_err || fds_err->fd_എണ്ണം == 0)) {
    /* no sockets, wait */
    തിരികെ curlx_wait_ms(timeout_ms);
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
  തിരികെ select((പൂർണ്ണസംഖ്യ)maxfd + 1,
                fds_read && fds_read->fd_എണ്ണം ? fds_read : ശൂന്യ,
                fds_write && fds_write->fd_എണ്ണം ? fds_write : ശൂന്യ,
                fds_err && fds_err->fd_എണ്ണം ? fds_err : ശൂന്യ, ptimeout);
#അല്ലെങ്കിൽ
  തിരികെ select((പൂർണ്ണസംഖ്യ)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
പൂർണ്ണസംഖ്യ Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ഘടന pollfd pfd[3];
  പൂർണ്ണസംഖ്യ അക്കം;
  പൂർണ്ണസംഖ്യ r;

  എങ്കിൽ((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    തിരികെ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  അക്കം = 0;
  എങ്കിൽ(readfd0 != CURL_SOCKET_BAD) {
    pfd[അക്കം].fd = readfd0;
    pfd[അക്കം].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[അക്കം].revents = 0;
    അക്കം++;
  }
  എങ്കിൽ(readfd1 != CURL_SOCKET_BAD) {
    pfd[അക്കം].fd = readfd1;
    pfd[അക്കം].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[അക്കം].revents = 0;
    അക്കം++;
  }
  എങ്കിൽ(writefd != CURL_SOCKET_BAD) {
    pfd[അക്കം].fd = writefd;
    pfd[അക്കം].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[അക്കം].revents = 0;
    അക്കം++;
  }

  r = Curl_poll(pfd, (unsigned പൂർണ്ണസംഖ്യ)അക്കം, timeout_ms);
  എങ്കിൽ(r <= 0)
    തിരികെ r;

  r = 0;
  അക്കം = 0;
  എങ്കിൽ(readfd0 != CURL_SOCKET_BAD) {
    എങ്കിൽ(pfd[അക്കം].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    എങ്കിൽ(pfd[അക്കം].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    അക്കം++;
  }
  എങ്കിൽ(readfd1 != CURL_SOCKET_BAD) {
    എങ്കിൽ(pfd[അക്കം].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    എങ്കിൽ(pfd[അക്കം].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    അക്കം++;
  }
  എങ്കിൽ(writefd != CURL_SOCKET_BAD) {
    എങ്കിൽ(pfd[അക്കം].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    എങ്കിൽ(pfd[അക്കം].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  തിരികെ r;
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
പൂർണ്ണസംഖ്യ Curl_poll(ഘടന pollfd ufds[], unsigned പൂർണ്ണസംഖ്യ nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  പൂർണ്ണസംഖ്യ pending_ms;
#അല്ലെങ്കിൽ
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned പൂർണ്ണസംഖ്യ i;
  പൂർണ്ണസംഖ്യ r;

  എങ്കിൽ(ufds) {
    വേണ്ടി(i = 0; i < nfds; i++) {
      എങ്കിൽ(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        നിർത്തുക;
      }
    }
  }
  എങ്കിൽ(fds_none) {
    /* no sockets, wait */
    തിരികെ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#എങ്കിൽ TIMEDIFF_T_MAX > INT_MAX
  എങ്കിൽ(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  എങ്കിൽ(timeout_ms > 0)
    pending_ms = (പൂർണ്ണസംഖ്യ)timeout_ms;
  അല്ലെങ്കിൽ എങ്കിൽ(timeout_ms < 0)
    pending_ms = -1;
  അല്ലെങ്കിൽ
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  എങ്കിൽ(r <= 0) {
    എങ്കിൽ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    തിരികെ r;
  }

  വേണ്ടി(i = 0; i < nfds; i++) {
    എങ്കിൽ(ufds[i].fd == CURL_SOCKET_BAD)
      തുടരുക;
    എങ്കിൽ(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    എങ്കിൽ(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#അല്ലെങ്കിൽ /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  വേണ്ടി(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    എങ്കിൽ(ufds[i].fd == CURL_SOCKET_BAD)
      തുടരുക;
    VERIFY_SOCK(ufds[i].fd);
    എങ്കിൽ(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      എങ്കിൽ(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      എങ്കിൽ(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      എങ്കിൽ(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      എങ്കിൽ(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  എങ്കിൽ(r <= 0) {
    എങ്കിൽ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    തിരികെ r;
  }

  r = 0;
  വേണ്ടി(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    എങ്കിൽ(ufds[i].fd == CURL_SOCKET_BAD)
      തുടരുക;
    എങ്കിൽ(FD_ISSET(ufds[i].fd, &fds_read)) {
      എങ്കിൽ(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      എങ്കിൽ(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    എങ്കിൽ(FD_ISSET(ufds[i].fd, &fds_write)) {
      എങ്കിൽ(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      എങ്കിൽ(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    എങ്കിൽ(FD_ISSET(ufds[i].fd, &fds_err)) {
      എങ്കിൽ(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      എങ്കിൽ(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    എങ്കിൽ(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  തിരികെ r;
}

ശൂന്യം Curl_pollfds_init(ഘടന curl_pollfds *cpfds,
                       ഘടന pollfd *static_pfds,
                       unsigned പൂർണ്ണസംഖ്യ static_എണ്ണം)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, വലുപ്പം(*cpfds));
  എങ്കിൽ(static_pfds && static_എണ്ണം) {
    cpfds->pfds = static_pfds;
    cpfds->എണ്ണം = static_എണ്ണം;
  }
}

ശൂന്യം Curl_pollfds_reset(ഘടന curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

ശൂന്യം Curl_pollfds_cleanup(ഘടന curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  എങ്കിൽ(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->എണ്ണം = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

സ്ഥിതം CURLcode cpfds_increase(ഘടന curl_pollfds *cpfds, unsigned പൂർണ്ണസംഖ്യ inc)
{
  ഘടന pollfd *new_fds;
  unsigned പൂർണ്ണസംഖ്യ new_എണ്ണം = cpfds->എണ്ണം + inc;

  new_fds = curlx_calloc(new_എണ്ണം, വലുപ്പം(ഘടന pollfd));
  എങ്കിൽ(!new_fds)
    തിരികെ CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->എണ്ണം * വലുപ്പം(ഘടന pollfd));
  എങ്കിൽ(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->എണ്ണം = new_എണ്ണം;
  cpfds->allocated_pfds = TRUE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cpfds_add_sock(ഘടന curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ(fold && cpfds->n <= INT_MAX) {
    വേണ്ടി(i = (പൂർണ്ണസംഖ്യ)cpfds->n - 1; i >= 0; --i) {
      എങ്കിൽ(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        തിരികെ CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  എങ്കിൽ(cpfds->n >= cpfds->എണ്ണം && cpfds_increase(cpfds, 100))
    തിരികെ CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  തിരികെ CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(ഘടന curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  തിരികെ cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(ഘടന curl_pollfds *cpfds,
                             ഘടന easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  വേണ്ടി(i = 0; i < ps->n; i++) {
    short events = 0;
    എങ്കിൽ(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    എങ്കിൽ(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    എങ്കിൽ(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      തിരികെ CURLE_OUT_OF_MEMORY;
  }
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_waitfds_init(ഘടന Curl_waitfds *cwfds,
                       ഘടന curl_waitfd *static_wfds,
                       unsigned പൂർണ്ണസംഖ്യ static_എണ്ണം)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_എണ്ണം);
  memset(cwfds, 0, വലുപ്പം(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->എണ്ണം = static_എണ്ണം;
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ cwfds_add_sock(ഘടന Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  പൂർണ്ണസംഖ്യ i;
  എങ്കിൽ(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->എണ്ണം && !cwfds->n);
    തിരികെ 1;
  }
  എങ്കിൽ(cwfds->n <= INT_MAX) {
    വേണ്ടി(i = (പൂർണ്ണസംഖ്യ)cwfds->n - 1; i >= 0; --i) {
      എങ്കിൽ(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        തിരികെ 0;
      }
    }
  }
  /* not folded, add new entry */
  എങ്കിൽ(cwfds->n < cwfds->എണ്ണം) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  തിരികെ 1;
}

unsigned പൂർണ്ണസംഖ്യ Curl_waitfds_add_ps(ഘടന Curl_waitfds *cwfds,
                                 ഘടന easy_pollset *ps)
{
  size_t i;
  unsigned പൂർണ്ണസംഖ്യ need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  വേണ്ടി(i = 0; i < ps->n; i++) {
    short events = 0;
    എങ്കിൽ(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    എങ്കിൽ(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    എങ്കിൽ(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  തിരികെ need;
}

ശൂന്യം Curl_pollset_reset(ഘടന easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

ശൂന്യം Curl_pollset_init(ഘടന easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->എണ്ണം = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ഘടന easy_pollset *Curl_pollset_create(ശൂന്യം)
{
  ഘടന easy_pollset *ps = curlx_calloc(1, വലുപ്പം(*ps));
  എങ്കിൽ(ps)
    Curl_pollset_init(ps);
  തിരികെ ps;
}

ശൂന്യം Curl_pollset_cleanup(ഘടന easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  എങ്കിൽ(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  എങ്കിൽ(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->എണ്ണം = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

ശൂന്യം Curl_pollset_move(ഘടന easy_pollset *to, ഘടന easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  എങ്കിൽ(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->എണ്ണം = from->എണ്ണം;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  അല്ലെങ്കിൽ {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->എണ്ണം * വലുപ്പം(to->sockets[0]));
    memcpy(to->actions, from->actions, to->എണ്ണം * വലുപ്പം(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps, curl_socket_t sock,
                             പൂർണ്ണസംഖ്യ add_flags, പൂർണ്ണസംഖ്യ remove_flags)
{
  unsigned പൂർണ്ണസംഖ്യ i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  എങ്കിൽ(!VALID_SOCK(sock))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  വേണ്ടി(i = 0; i < ps->n; ++i) {
    എങ്കിൽ(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned അക്ഷരം)(~remove_flags);
      ps->actions[i] |= (unsigned അക്ഷരം)add_flags;
      /* all gone? remove socket */
      എങ്കിൽ(!ps->actions[i]) {
        എങ്കിൽ((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * വലുപ്പം(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * വലുപ്പം(ps->actions[0]));
        }
        --ps->n;
      }
      തിരികെ CURLE_OK;
    }
  }
  /* not present */
  എങ്കിൽ(add_flags) {
    എങ്കിൽ(i >= ps->എണ്ണം) { /* need to grow */
      unsigned പൂർണ്ണസംഖ്യ new_എണ്ണം = CURLMAX(ps->എണ്ണം * 2, 8);
      curl_socket_t *nsockets;
      unsigned അക്ഷരം *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->എണ്ണം, new_എണ്ണം);
      എങ്കിൽ(new_എണ്ണം <= ps->എണ്ണം)
        തിരികെ CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_എണ്ണം, വലുപ്പം(nsockets[0]));
      എങ്കിൽ(!nsockets)
        തിരികെ CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_എണ്ണം, വലുപ്പം(nactions[0]));
      എങ്കിൽ(!nactions) {
        curlx_free(nsockets);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->എണ്ണം * വലുപ്പം(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->എണ്ണം * വലുപ്പം(ps->actions[0]));
      എങ്കിൽ(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      എങ്കിൽ(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->എണ്ണം = new_എണ്ണം;
    }
    DEBUGASSERT(i < ps->എണ്ണം);
    എങ്കിൽ(i < ps->എണ്ണം) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned അക്ഷരം)add_flags;
      ps->n = i + 1;
    }
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_pollset_set(ഘടന Curl_easy *data,
                          ഘടന easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  തിരികെ Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

ശൂന്യം Curl_pollset_remove(ഘടന easy_pollset *ps, curl_socket_t sock)
{
  unsigned പൂർണ്ണസംഖ്യ i;
  വേണ്ടി(i = 0; i < ps->n; ++i) {
    എങ്കിൽ(ps->sockets[i] == sock) {
      എങ്കിൽ((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * വലുപ്പം(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * വലുപ്പം(ps->actions[0]));
      }
      --ps->n;
      തിരികെ;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
പൂർണ്ണസംഖ്യ Curl_pollset_poll(ഘടന Curl_easy *data,
                      ഘടന easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ഘടന pollfd *pfds;
  unsigned പൂർണ്ണസംഖ്യ i, npfds;
  പൂർണ്ണസംഖ്യ rc;

  (ശൂന്യം)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  എങ്കിൽ(!ps->n)
    തിരികെ curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, വലുപ്പം(*pfds));
  എങ്കിൽ(!pfds)
    തിരികെ -1;

  npfds = 0;
  വേണ്ടി(i = 0; i < ps->n; ++i) {
    short events = 0;
    എങ്കിൽ(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    എങ്കിൽ(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    എങ്കിൽ(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  തിരികെ rc;
}

ശൂന്യം Curl_pollset_check(ഘടന Curl_easy *data,
                        ഘടന easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned പൂർണ്ണസംഖ്യ i;

  (ശൂന്യം)data;
  DEBUGASSERT(VALID_SOCK(sock));
  വേണ്ടി(i = 0; i < ps->n; ++i) {
    എങ്കിൽ(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      തിരികെ;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ഘടന Curl_easy *data,
                            ഘടന easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned പൂർണ്ണസംഖ്യ i;
  (ശൂന്യം)data;
  വേണ്ടി(i = 0; i < ps->n; ++i) {
    എങ്കിൽ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

bool Curl_pollset_want_send(ഘടന Curl_easy *data,
                            ഘടന easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned പൂർണ്ണസംഖ്യ i;
  (ശൂന്യം)data;
  വേണ്ടി(i = 0; i < ps->n; ++i) {
    എങ്കിൽ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}
