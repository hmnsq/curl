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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#ಅನ್ತರ್ಭಾವ <sys/select.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_UNISTD_H)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"

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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ಸಂರಚನಾ timeval pending_tv;
  ಸಂರಚನಾ timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  ಯದಿ((!fds_read || fds_read->fd_ಎಣಿಕೆ == 0) &&
     (!fds_write || fds_write->fd_ಎಣಿಕೆ == 0) &&
     (!fds_err || fds_err->fd_ಎಣಿಕೆ == 0)) {
    /* no sockets, wait */
    ಪ್ರತ್ಯಾಗಮ curlx_wait_ms(timeout_ms);
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
  ಪ್ರತ್ಯಾಗಮ select((ಪೂರ್ಣಾಙ್ಕ)maxfd + 1,
                fds_read && fds_read->fd_ಎಣಿಕೆ ? fds_read : ರಿಕ್ತ,
                fds_write && fds_write->fd_ಎಣಿಕೆ ? fds_write : ರಿಕ್ತ,
                fds_err && fds_err->fd_ಎಣಿಕೆ ? fds_err : ರಿಕ್ತ, ptimeout);
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ select((ಪೂರ್ಣಾಙ್ಕ)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
ಪೂರ್ಣಾಙ್ಕ Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ಸಂರಚನಾ pollfd pfd[3];
  ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ;
  ಪೂರ್ಣಾಙ್ಕ r;

  ಯದಿ((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ಪ್ರತ್ಯಾಗಮ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  ಅಂಕೆ = 0;
  ಯದಿ(readfd0 != CURL_SOCKET_BAD) {
    pfd[ಅಂಕೆ].fd = readfd0;
    pfd[ಅಂಕೆ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ಅಂಕೆ].revents = 0;
    ಅಂಕೆ++;
  }
  ಯದಿ(readfd1 != CURL_SOCKET_BAD) {
    pfd[ಅಂಕೆ].fd = readfd1;
    pfd[ಅಂಕೆ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ಅಂಕೆ].revents = 0;
    ಅಂಕೆ++;
  }
  ಯದಿ(writefd != CURL_SOCKET_BAD) {
    pfd[ಅಂಕೆ].fd = writefd;
    pfd[ಅಂಕೆ].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[ಅಂಕೆ].revents = 0;
    ಅಂಕೆ++;
  }

  r = Curl_poll(pfd, (unsigned ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ, timeout_ms);
  ಯದಿ(r <= 0)
    ಪ್ರತ್ಯಾಗಮ r;

  r = 0;
  ಅಂಕೆ = 0;
  ಯದಿ(readfd0 != CURL_SOCKET_BAD) {
    ಯದಿ(pfd[ಅಂಕೆ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    ಯದಿ(pfd[ಅಂಕೆ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ಅಂಕೆ++;
  }
  ಯದಿ(readfd1 != CURL_SOCKET_BAD) {
    ಯದಿ(pfd[ಅಂಕೆ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ಯದಿ(pfd[ಅಂಕೆ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ಅಂಕೆ++;
  }
  ಯದಿ(writefd != CURL_SOCKET_BAD) {
    ಯದಿ(pfd[ಅಂಕೆ].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    ಯದಿ(pfd[ಅಂಕೆ].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ಪ್ರತ್ಯಾಗಮ r;
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
ಪೂರ್ಣಾಙ್ಕ Curl_poll(ಸಂರಚನಾ pollfd ufds[], unsigned ಪೂರ್ಣಾಙ್ಕ nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  ಪೂರ್ಣಾಙ್ಕ pending_ms;
#ಅನ್ಯಥಾ
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  ಪೂರ್ಣಾಙ್ಕ r;

  ಯದಿ(ufds) {
    ಪ್ರತಿ(i = 0; i < nfds; i++) {
      ಯದಿ(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        ಭಙ್ಗ;
      }
    }
  }
  ಯದಿ(fds_none) {
    /* no sockets, wait */
    ಪ್ರತ್ಯಾಗಮ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ಯದಿ TIMEDIFF_T_MAX > INT_MAX
  ಯದಿ(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  ಯದಿ(timeout_ms > 0)
    pending_ms = (ಪೂರ್ಣಾಙ್ಕ)timeout_ms;
  ಅನ್ಯಥಾ ಯದಿ(timeout_ms < 0)
    pending_ms = -1;
  ಅನ್ಯಥಾ
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ಯದಿ(r <= 0) {
    ಯದಿ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ಪ್ರತ್ಯಾಗಮ r;
  }

  ಪ್ರತಿ(i = 0; i < nfds; i++) {
    ಯದಿ(ufds[i].fd == CURL_SOCKET_BAD)
      ಅನುವರ್ತ;
    ಯದಿ(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ಯದಿ(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#ಅನ್ಯಥಾ /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  ಪ್ರತಿ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ಯದಿ(ufds[i].fd == CURL_SOCKET_BAD)
      ಅನುವರ್ತ;
    VERIFY_SOCK(ufds[i].fd);
    ಯದಿ(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ಯದಿ(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ಯದಿ(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ಯದಿ(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ಯದಿ(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ಯದಿ(r <= 0) {
    ಯದಿ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ಪ್ರತ್ಯಾಗಮ r;
  }

  r = 0;
  ಪ್ರತಿ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ಯದಿ(ufds[i].fd == CURL_SOCKET_BAD)
      ಅನುವರ್ತ;
    ಯದಿ(FD_ISSET(ufds[i].fd, &fds_read)) {
      ಯದಿ(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ಯದಿ(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ಯದಿ(FD_ISSET(ufds[i].fd, &fds_write)) {
      ಯದಿ(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ಯದಿ(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ಯದಿ(FD_ISSET(ufds[i].fd, &fds_err)) {
      ಯದಿ(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ಯದಿ(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ಯದಿ(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  ಪ್ರತ್ಯಾಗಮ r;
}

ಶೂನ್ಯ Curl_pollfds_init(ಸಂರಚನಾ curl_pollfds *cpfds,
                       ಸಂರಚನಾ pollfd *static_pfds,
                       unsigned ಪೂರ್ಣಾಙ್ಕ static_ಎಣಿಕೆ)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, ಪರಿಮಾಣ(*cpfds));
  ಯದಿ(static_pfds && static_ಎಣಿಕೆ) {
    cpfds->pfds = static_pfds;
    cpfds->ಎಣಿಕೆ = static_ಎಣಿಕೆ;
  }
}

ಶೂನ್ಯ Curl_pollfds_reset(ಸಂರಚನಾ curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

ಶೂನ್ಯ Curl_pollfds_cleanup(ಸಂರಚನಾ curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ಯದಿ(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->ಎಣಿಕೆ = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ಸ್ಥಿರ CURLcode cpfds_increase(ಸಂರಚನಾ curl_pollfds *cpfds, unsigned ಪೂರ್ಣಾಙ್ಕ inc)
{
  ಸಂರಚನಾ pollfd *new_fds;
  unsigned ಪೂರ್ಣಾಙ್ಕ new_ಎಣಿಕೆ = cpfds->ಎಣಿಕೆ + inc;

  new_fds = curlx_calloc(new_ಎಣಿಕೆ, ಪರಿಮಾಣ(ಸಂರಚನಾ pollfd));
  ಯದಿ(!new_fds)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->ಎಣಿಕೆ * ಪರಿಮಾಣ(ಸಂರಚನಾ pollfd));
  ಯದಿ(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->ಎಣಿಕೆ = new_ಎಣಿಕೆ;
  cpfds->allocated_pfds = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cpfds_add_sock(ಸಂರಚನಾ curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಯದಿ(fold && cpfds->n <= INT_MAX) {
    ಪ್ರತಿ(i = (ಪೂರ್ಣಾಙ್ಕ)cpfds->n - 1; i >= 0; --i) {
      ಯದಿ(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ಯದಿ(cpfds->n >= cpfds->ಎಣಿಕೆ && cpfds_increase(cpfds, 100))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(ಸಂರಚನಾ curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  ಪ್ರತ್ಯಾಗಮ cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(ಸಂರಚನಾ curl_pollfds *cpfds,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  ಪ್ರತಿ(i = 0; i < ps->n; i++) {
    short events = 0;
    ಯದಿ(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    ಯದಿ(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    ಯದಿ(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_waitfds_init(ಸಂರಚನಾ Curl_waitfds *cwfds,
                       ಸಂರಚನಾ curl_waitfd *static_wfds,
                       unsigned ಪೂರ್ಣಾಙ್ಕ static_ಎಣಿಕೆ)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_ಎಣಿಕೆ);
  memset(cwfds, 0, ಪರಿಮಾಣ(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->ಎಣಿಕೆ = static_ಎಣಿಕೆ;
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ cwfds_add_sock(ಸಂರಚನಾ Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಯದಿ(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->ಎಣಿಕೆ && !cwfds->n);
    ಪ್ರತ್ಯಾಗಮ 1;
  }
  ಯದಿ(cwfds->n <= INT_MAX) {
    ಪ್ರತಿ(i = (ಪೂರ್ಣಾಙ್ಕ)cwfds->n - 1; i >= 0; --i) {
      ಯದಿ(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ಪ್ರತ್ಯಾಗಮ 0;
      }
    }
  }
  /* not folded, add new entry */
  ಯದಿ(cwfds->n < cwfds->ಎಣಿಕೆ) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
}

unsigned ಪೂರ್ಣಾಙ್ಕ Curl_waitfds_add_ps(ಸಂರಚನಾ Curl_waitfds *cwfds,
                                 ಸಂರಚನಾ easy_pollset *ps)
{
  size_t i;
  unsigned ಪೂರ್ಣಾಙ್ಕ need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  ಪ್ರತಿ(i = 0; i < ps->n; i++) {
    short events = 0;
    ಯದಿ(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    ಯದಿ(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    ಯದಿ(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  ಪ್ರತ್ಯಾಗಮ need;
}

ಶೂನ್ಯ Curl_pollset_reset(ಸಂರಚನಾ easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

ಶೂನ್ಯ Curl_pollset_init(ಸಂರಚನಾ easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->ಎಣಿಕೆ = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ಸಂರಚನಾ easy_pollset *Curl_pollset_create(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ easy_pollset *ps = curlx_calloc(1, ಪರಿಮಾಣ(*ps));
  ಯದಿ(ps)
    Curl_pollset_init(ps);
  ಪ್ರತ್ಯಾಗಮ ps;
}

ಶೂನ್ಯ Curl_pollset_cleanup(ಸಂರಚನಾ easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ಯದಿ(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ಯದಿ(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->ಎಣಿಕೆ = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

ಶೂನ್ಯ Curl_pollset_move(ಸಂರಚನಾ easy_pollset *to, ಸಂರಚನಾ easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  ಯದಿ(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->ಎಣಿಕೆ = from->ಎಣಿಕೆ;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  ಅನ್ಯಥಾ {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->ಎಣಿಕೆ * ಪರಿಮಾಣ(to->sockets[0]));
    memcpy(to->actions, from->actions, to->ಎಣಿಕೆ * ಪರಿಮಾಣ(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock,
                             ಪೂರ್ಣಾಙ್ಕ add_flags, ಪೂರ್ಣಾಙ್ಕ remove_flags)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  ಯದಿ(!VALID_SOCK(sock))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  ಪ್ರತಿ(i = 0; i < ps->n; ++i) {
    ಯದಿ(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned ವರ್ಣ)(~remove_flags);
      ps->actions[i] |= (unsigned ವರ್ಣ)add_flags;
      /* all gone? remove socket */
      ಯದಿ(!ps->actions[i]) {
        ಯದಿ((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * ಪರಿಮಾಣ(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * ಪರಿಮಾಣ(ps->actions[0]));
        }
        --ps->n;
      }
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }
  /* not present */
  ಯದಿ(add_flags) {
    ಯದಿ(i >= ps->ಎಣಿಕೆ) { /* need to grow */
      unsigned ಪೂರ್ಣಾಙ್ಕ new_ಎಣಿಕೆ = CURLMAX(ps->ಎಣಿಕೆ * 2, 8);
      curl_socket_t *nsockets;
      unsigned ವರ್ಣ *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->ಎಣಿಕೆ, new_ಎಣಿಕೆ);
      ಯದಿ(new_ಎಣಿಕೆ <= ps->ಎಣಿಕೆ)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_ಎಣಿಕೆ, ಪರಿಮಾಣ(nsockets[0]));
      ಯದಿ(!nsockets)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_ಎಣಿಕೆ, ಪರಿಮಾಣ(nactions[0]));
      ಯದಿ(!nactions) {
        curlx_free(nsockets);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->ಎಣಿಕೆ * ಪರಿಮಾಣ(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->ಎಣಿಕೆ * ಪರಿಮಾಣ(ps->actions[0]));
      ಯದಿ(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ಯದಿ(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->ಎಣಿಕೆ = new_ಎಣಿಕೆ;
    }
    DEBUGASSERT(i < ps->ಎಣಿಕೆ);
    ಯದಿ(i < ps->ಎಣಿಕೆ) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned ವರ್ಣ)add_flags;
      ps->n = i + 1;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_pollset_set(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  ಪ್ರತ್ಯಾಗಮ Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

ಶೂನ್ಯ Curl_pollset_remove(ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  ಪ್ರತಿ(i = 0; i < ps->n; ++i) {
    ಯದಿ(ps->sockets[i] == sock) {
      ಯದಿ((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * ಪರಿಮಾಣ(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * ಪರಿಮಾಣ(ps->actions[0]));
      }
      --ps->n;
      ಪ್ರತ್ಯಾಗಮ;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ಪೂರ್ಣಾಙ್ಕ Curl_pollset_poll(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ಸಂರಚನಾ pollfd *pfds;
  unsigned ಪೂರ್ಣಾಙ್ಕ i, npfds;
  ಪೂರ್ಣಾಙ್ಕ rc;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  ಯದಿ(!ps->n)
    ಪ್ರತ್ಯಾಗಮ curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, ಪರಿಮಾಣ(*pfds));
  ಯದಿ(!pfds)
    ಪ್ರತ್ಯಾಗಮ -1;

  npfds = 0;
  ಪ್ರತಿ(i = 0; i < ps->n; ++i) {
    short events = 0;
    ಯದಿ(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    ಯದಿ(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    ಯದಿ(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಶೂನ್ಯ Curl_pollset_check(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(VALID_SOCK(sock));
  ಪ್ರತಿ(i = 0; i < ps->n; ++i) {
    ಯದಿ(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      ಪ್ರತ್ಯಾಗಮ;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  (ಶೂನ್ಯ)data;
  ಪ್ರತಿ(i = 0; i < ps->n; ++i) {
    ಯದಿ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_pollset_want_send(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  (ಶೂನ್ಯ)data;
  ಪ್ರತಿ(i = 0; i < ps->n; ++i) {
    ಯದಿ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}
