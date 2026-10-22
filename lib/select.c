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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#அந்தர்பாவ <sys/select.h>
#அல்லதெனில் defined(HAVE_UNISTD_H)
#அந்தர்பாவ <unistd.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "curlx/timediff.h"
#அந்தர்பாவ "curlx/wait.h"

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
நிலையான முழு our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  கட்டமைப்பு timeval pending_tv;
  கட்டமைப்பு timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  எனில்((!fds_read || fds_read->fd_மொத்தம் == 0) &&
     (!fds_write || fds_write->fd_மொத்தம் == 0) &&
     (!fds_err || fds_err->fd_மொத்தம் == 0)) {
    /* no sockets, wait */
    திரும்பு curlx_wait_ms(timeout_ms);
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
  திரும்பு select((முழு)maxfd + 1,
                fds_read && fds_read->fd_மொத்தம் ? fds_read : வெற்று,
                fds_write && fds_write->fd_மொத்தம் ? fds_write : வெற்று,
                fds_err && fds_err->fd_மொத்தம் ? fds_err : வெற்று, ptimeout);
#இல்லையெனில்
  திரும்பு select((முழு)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
முழு Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  கட்டமைப்பு pollfd pfd[3];
  முழு இலக்கம்;
  முழு r;

  எனில்((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    திரும்பு curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  இலக்கம் = 0;
  எனில்(readfd0 != CURL_SOCKET_BAD) {
    pfd[இலக்கம்].fd = readfd0;
    pfd[இலக்கம்].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[இலக்கம்].revents = 0;
    இலக்கம்++;
  }
  எனில்(readfd1 != CURL_SOCKET_BAD) {
    pfd[இலக்கம்].fd = readfd1;
    pfd[இலக்கம்].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[இலக்கம்].revents = 0;
    இலக்கம்++;
  }
  எனில்(writefd != CURL_SOCKET_BAD) {
    pfd[இலக்கம்].fd = writefd;
    pfd[இலக்கம்].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[இலக்கம்].revents = 0;
    இலக்கம்++;
  }

  r = Curl_poll(pfd, (unsigned முழு)இலக்கம், timeout_ms);
  எனில்(r <= 0)
    திரும்பு r;

  r = 0;
  இலக்கம் = 0;
  எனில்(readfd0 != CURL_SOCKET_BAD) {
    எனில்(pfd[இலக்கம்].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    எனில்(pfd[இலக்கம்].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    இலக்கம்++;
  }
  எனில்(readfd1 != CURL_SOCKET_BAD) {
    எனில்(pfd[இலக்கம்].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    எனில்(pfd[இலக்கம்].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    இலக்கம்++;
  }
  எனில்(writefd != CURL_SOCKET_BAD) {
    எனில்(pfd[இலக்கம்].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    எனில்(pfd[இலக்கம்].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  திரும்பு r;
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
முழு Curl_poll(கட்டமைப்பு pollfd ufds[], unsigned முழு nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  முழு pending_ms;
#இல்லையெனில்
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned முழு i;
  முழு r;

  எனில்(ufds) {
    க்கு(i = 0; i < nfds; i++) {
      எனில்(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        நிறுத்து;
      }
    }
  }
  எனில்(fds_none) {
    /* no sockets, wait */
    திரும்பு curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#எனில் TIMEDIFF_T_MAX > INT_MAX
  எனில்(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  எனில்(timeout_ms > 0)
    pending_ms = (முழு)timeout_ms;
  இல்லையெனில் எனில்(timeout_ms < 0)
    pending_ms = -1;
  இல்லையெனில்
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  எனில்(r <= 0) {
    எனில்((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    திரும்பு r;
  }

  க்கு(i = 0; i < nfds; i++) {
    எனில்(ufds[i].fd == CURL_SOCKET_BAD)
      தொடர்;
    எனில்(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    எனில்(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#இல்லையெனில் /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  க்கு(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    எனில்(ufds[i].fd == CURL_SOCKET_BAD)
      தொடர்;
    VERIFY_SOCK(ufds[i].fd);
    எனில்(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      எனில்(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      எனில்(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      எனில்(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      எனில்(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  எனில்(r <= 0) {
    எனில்((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    திரும்பு r;
  }

  r = 0;
  க்கு(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    எனில்(ufds[i].fd == CURL_SOCKET_BAD)
      தொடர்;
    எனில்(FD_ISSET(ufds[i].fd, &fds_read)) {
      எனில்(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      எனில்(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    எனில்(FD_ISSET(ufds[i].fd, &fds_write)) {
      எனில்(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      எனில்(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    எனில்(FD_ISSET(ufds[i].fd, &fds_err)) {
      எனில்(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      எனில்(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    எனில்(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  திரும்பு r;
}

வெறுமை Curl_pollfds_init(கட்டமைப்பு curl_pollfds *cpfds,
                       கட்டமைப்பு pollfd *static_pfds,
                       unsigned முழு static_மொத்தம்)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, அளவு(*cpfds));
  எனில்(static_pfds && static_மொத்தம்) {
    cpfds->pfds = static_pfds;
    cpfds->மொத்தம் = static_மொத்தம்;
  }
}

வெறுமை Curl_pollfds_reset(கட்டமைப்பு curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

வெறுமை Curl_pollfds_cleanup(கட்டமைப்பு curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  எனில்(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->மொத்தம் = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

நிலையான CURLcode cpfds_increase(கட்டமைப்பு curl_pollfds *cpfds, unsigned முழு inc)
{
  கட்டமைப்பு pollfd *new_fds;
  unsigned முழு new_மொத்தம் = cpfds->மொத்தம் + inc;

  new_fds = curlx_calloc(new_மொத்தம், அளவு(கட்டமைப்பு pollfd));
  எனில்(!new_fds)
    திரும்பு CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->மொத்தம் * அளவு(கட்டமைப்பு pollfd));
  எனில்(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->மொத்தம் = new_மொத்தம்;
  cpfds->allocated_pfds = TRUE;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cpfds_add_sock(கட்டமைப்பு curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  முழு i;

  எனில்(fold && cpfds->n <= INT_MAX) {
    க்கு(i = (முழு)cpfds->n - 1; i >= 0; --i) {
      எனில்(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        திரும்பு CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  எனில்(cpfds->n >= cpfds->மொத்தம் && cpfds_increase(cpfds, 100))
    திரும்பு CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  திரும்பு CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(கட்டமைப்பு curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  திரும்பு cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(கட்டமைப்பு curl_pollfds *cpfds,
                             கட்டமைப்பு easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  க்கு(i = 0; i < ps->n; i++) {
    short events = 0;
    எனில்(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    எனில்(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    எனில்(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  திரும்பு CURLE_OK;
}

வெறுமை Curl_waitfds_init(கட்டமைப்பு Curl_waitfds *cwfds,
                       கட்டமைப்பு curl_waitfd *static_wfds,
                       unsigned முழு static_மொத்தம்)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_மொத்தம்);
  memset(cwfds, 0, அளவு(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->மொத்தம் = static_மொத்தம்;
}

நிலையான unsigned முழு cwfds_add_sock(கட்டமைப்பு Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  முழு i;
  எனில்(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->மொத்தம் && !cwfds->n);
    திரும்பு 1;
  }
  எனில்(cwfds->n <= INT_MAX) {
    க்கு(i = (முழு)cwfds->n - 1; i >= 0; --i) {
      எனில்(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        திரும்பு 0;
      }
    }
  }
  /* not folded, add new entry */
  எனில்(cwfds->n < cwfds->மொத்தம்) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  திரும்பு 1;
}

unsigned முழு Curl_waitfds_add_ps(கட்டமைப்பு Curl_waitfds *cwfds,
                                 கட்டமைப்பு easy_pollset *ps)
{
  size_t i;
  unsigned முழு need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  க்கு(i = 0; i < ps->n; i++) {
    short events = 0;
    எனில்(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    எனில்(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    எனில்(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  திரும்பு need;
}

வெறுமை Curl_pollset_reset(கட்டமைப்பு easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

வெறுமை Curl_pollset_init(கட்டமைப்பு easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->மொத்தம் = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

கட்டமைப்பு easy_pollset *Curl_pollset_create(வெறுமை)
{
  கட்டமைப்பு easy_pollset *ps = curlx_calloc(1, அளவு(*ps));
  எனில்(ps)
    Curl_pollset_init(ps);
  திரும்பு ps;
}

வெறுமை Curl_pollset_cleanup(கட்டமைப்பு easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  எனில்(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  எனில்(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->மொத்தம் = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

வெறுமை Curl_pollset_move(கட்டமைப்பு easy_pollset *to, கட்டமைப்பு easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  எனில்(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->மொத்தம் = from->மொத்தம்;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  இல்லையெனில் {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->மொத்தம் * அளவு(to->sockets[0]));
    memcpy(to->actions, from->actions, to->மொத்தம் * அளவு(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps, curl_socket_t sock,
                             முழு add_flags, முழு remove_flags)
{
  unsigned முழு i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  எனில்(!VALID_SOCK(sock))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  க்கு(i = 0; i < ps->n; ++i) {
    எனில்(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned எழுத்து)(~remove_flags);
      ps->actions[i] |= (unsigned எழுத்து)add_flags;
      /* all gone? remove socket */
      எனில்(!ps->actions[i]) {
        எனில்((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * அளவு(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * அளவு(ps->actions[0]));
        }
        --ps->n;
      }
      திரும்பு CURLE_OK;
    }
  }
  /* not present */
  எனில்(add_flags) {
    எனில்(i >= ps->மொத்தம்) { /* need to grow */
      unsigned முழு new_மொத்தம் = CURLMAX(ps->மொத்தம் * 2, 8);
      curl_socket_t *nsockets;
      unsigned எழுத்து *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->மொத்தம், new_மொத்தம்);
      எனில்(new_மொத்தம் <= ps->மொத்தம்)
        திரும்பு CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_மொத்தம், அளவு(nsockets[0]));
      எனில்(!nsockets)
        திரும்பு CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_மொத்தம், அளவு(nactions[0]));
      எனில்(!nactions) {
        curlx_free(nsockets);
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->மொத்தம் * அளவு(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->மொத்தம் * அளவு(ps->actions[0]));
      எனில்(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      எனில்(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->மொத்தம் = new_மொத்தம்;
    }
    DEBUGASSERT(i < ps->மொத்தம்);
    எனில்(i < ps->மொத்தம்) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned எழுத்து)add_flags;
      ps->n = i + 1;
    }
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_pollset_set(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  திரும்பு Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

வெறுமை Curl_pollset_remove(கட்டமைப்பு easy_pollset *ps, curl_socket_t sock)
{
  unsigned முழு i;
  க்கு(i = 0; i < ps->n; ++i) {
    எனில்(ps->sockets[i] == sock) {
      எனில்((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * அளவு(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * அளவு(ps->actions[0]));
      }
      --ps->n;
      திரும்பு;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
முழு Curl_pollset_poll(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு easy_pollset *ps,
                      timediff_t timeout_ms)
{
  கட்டமைப்பு pollfd *pfds;
  unsigned முழு i, npfds;
  முழு rc;

  (வெறுமை)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  எனில்(!ps->n)
    திரும்பு curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, அளவு(*pfds));
  எனில்(!pfds)
    திரும்பு -1;

  npfds = 0;
  க்கு(i = 0; i < ps->n; ++i) {
    short events = 0;
    எனில்(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    எனில்(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    எனில்(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  திரும்பு rc;
}

வெறுமை Curl_pollset_check(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned முழு i;

  (வெறுமை)data;
  DEBUGASSERT(VALID_SOCK(sock));
  க்கு(i = 0; i < ps->n; ++i) {
    எனில்(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      திரும்பு;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned முழு i;
  (வெறுமை)data;
  க்கு(i = 0; i < ps->n; ++i) {
    எனில்((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

bool Curl_pollset_want_send(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned முழு i;
  (வெறுமை)data;
  க்கு(i = 0; i < ps->n; ++i) {
    எனில்((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
