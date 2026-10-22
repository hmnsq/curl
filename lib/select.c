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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_SELECT_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/select.h>
#ਜਾਂਜੇ defined(HAVE_UNISTD_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timediff.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/wait.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_POLL  /* use select() */
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
ਸਥਿਰ ਪੂਰਨ our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ਬਣਤਰ timeval pending_tv;
  ਬਣਤਰ timeval *ptimeout;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  ਜੇ((!fds_read || fds_read->fd_ਗਿਣਤੀ == 0) &&
     (!fds_write || fds_write->fd_ਗਿਣਤੀ == 0) &&
     (!fds_err || fds_err->fd_ਗਿਣਤੀ == 0)) {
    /* no sockets, wait */
    ਵਾਪਸ curlx_wait_ms(timeout_ms);
  }
#ਜੇ_ਅੰਤ

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
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
  ਵਾਪਸ select((ਪੂਰਨ)maxfd + 1,
                fds_read && fds_read->fd_ਗਿਣਤੀ ? fds_read : ਨਿਰਾ,
                fds_write && fds_write->fd_ਗਿਣਤੀ ? fds_write : ਨਿਰਾ,
                fds_err && fds_err->fd_ਗਿਣਤੀ ? fds_err : ਨਿਰਾ, ptimeout);
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ select((ਪੂਰਨ)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ

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
ਪੂਰਨ Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ਬਣਤਰ pollfd pfd[3];
  ਪੂਰਨ ਅੰਕ;
  ਪੂਰਨ r;

  ਜੇ((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ਵਾਪਸ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  ਅੰਕ = 0;
  ਜੇ(readfd0 != CURL_SOCKET_BAD) {
    pfd[ਅੰਕ].fd = readfd0;
    pfd[ਅੰਕ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ਅੰਕ].revents = 0;
    ਅੰਕ++;
  }
  ਜੇ(readfd1 != CURL_SOCKET_BAD) {
    pfd[ਅੰਕ].fd = readfd1;
    pfd[ਅੰਕ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ਅੰਕ].revents = 0;
    ਅੰਕ++;
  }
  ਜੇ(writefd != CURL_SOCKET_BAD) {
    pfd[ਅੰਕ].fd = writefd;
    pfd[ਅੰਕ].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[ਅੰਕ].revents = 0;
    ਅੰਕ++;
  }

  r = Curl_poll(pfd, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਅੰਕ, timeout_ms);
  ਜੇ(r <= 0)
    ਵਾਪਸ r;

  r = 0;
  ਅੰਕ = 0;
  ਜੇ(readfd0 != CURL_SOCKET_BAD) {
    ਜੇ(pfd[ਅੰਕ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_ਵਿੱਚ;
    ਜੇ(pfd[ਅੰਕ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ਅੰਕ++;
  }
  ਜੇ(readfd1 != CURL_SOCKET_BAD) {
    ਜੇ(pfd[ਅੰਕ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ਜੇ(pfd[ਅੰਕ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ਅੰਕ++;
  }
  ਜੇ(writefd != CURL_SOCKET_BAD) {
    ਜੇ(pfd[ਅੰਕ].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_ਬਾਹਰਲਾ;
    ਜੇ(pfd[ਅੰਕ].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ਵਾਪਸ r;
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
ਪੂਰਨ Curl_poll(ਬਣਤਰ pollfd ufds[], ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nfds, timediff_t timeout_ms)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_POLL
  ਪੂਰਨ pending_ms;
#ਨਹੀਂਤਾਂ
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#ਜੇ_ਅੰਤ
  bool fds_none = TRUE;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  ਪੂਰਨ r;

  ਜੇ(ufds) {
    ਲਈ(i = 0; i < nfds; i++) {
      ਜੇ(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        ਰੋਕੋ;
      }
    }
  }
  ਜੇ(fds_none) {
    /* no sockets, wait */
    ਵਾਪਸ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ਜੇ TIMEDIFF_T_MAX > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
  ਜੇ(timeout_ms > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
    timeout_ms = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
#ਜੇ_ਅੰਤ
  ਜੇ(timeout_ms > 0)
    pending_ms = (ਪੂਰਨ)timeout_ms;
  ਨਹੀਂਤਾਂ ਜੇ(timeout_ms < 0)
    pending_ms = -1;
  ਨਹੀਂਤਾਂ
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ਜੇ(r <= 0) {
    ਜੇ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ਵਾਪਸ r;
  }

  ਲਈ(i = 0; i < nfds; i++) {
    ਜੇ(ufds[i].fd == CURL_SOCKET_BAD)
      ਜਾਰੀ;
    ਜੇ(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ਜੇ(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#ਨਹੀਂਤਾਂ /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  ਲਈ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ਜੇ(ufds[i].fd == CURL_SOCKET_BAD)
      ਜਾਰੀ;
    VERIFY_SOCK(ufds[i].fd);
    ਜੇ(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ਜੇ(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ਜੇ(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ਜੇ(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ਜੇ(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ਜੇ(r <= 0) {
    ਜੇ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ਵਾਪਸ r;
  }

  r = 0;
  ਲਈ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ਜੇ(ufds[i].fd == CURL_SOCKET_BAD)
      ਜਾਰੀ;
    ਜੇ(FD_ISSET(ufds[i].fd, &fds_read)) {
      ਜੇ(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ਜੇ(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ਜੇ(FD_ISSET(ufds[i].fd, &fds_write)) {
      ਜੇ(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ਜੇ(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ਜੇ(FD_ISSET(ufds[i].fd, &fds_err)) {
      ਜੇ(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ਜੇ(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ਜੇ(ufds[i].revents)
      r++;
  }

#ਜੇ_ਅੰਤ /* HAVE_POLL */

  ਵਾਪਸ r;
}

ਖਾਲੀ Curl_pollfds_init(ਬਣਤਰ curl_pollfds *cpfds,
                       ਬਣਤਰ pollfd *static_pfds,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ static_ਗਿਣਤੀ)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, ਆਕਾਰ(*cpfds));
  ਜੇ(static_pfds && static_ਗਿਣਤੀ) {
    cpfds->pfds = static_pfds;
    cpfds->ਗਿਣਤੀ_2 = static_ਗਿਣਤੀ;
  }
}

ਖਾਲੀ Curl_pollfds_reset(ਬਣਤਰ curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

ਖਾਲੀ Curl_pollfds_cleanup(ਬਣਤਰ curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ਜੇ(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->ਗਿਣਤੀ_2 = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ਸਥਿਰ CURLcode cpfds_increase(ਬਣਤਰ curl_pollfds *cpfds, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ inc)
{
  ਬਣਤਰ pollfd *new_fds;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ new_ਗਿਣਤੀ = cpfds->ਗਿਣਤੀ_2 + inc;

  new_fds = curlx_calloc(new_ਗਿਣਤੀ, ਆਕਾਰ(ਬਣਤਰ pollfd));
  ਜੇ(!new_fds)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_fds, cpfds->pfds, cpfds->ਗਿਣਤੀ_2 * ਆਕਾਰ(ਬਣਤਰ pollfd));
  ਜੇ(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->ਗਿਣਤੀ_2 = new_ਗਿਣਤੀ;
  cpfds->allocated_pfds = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cpfds_ਜੋੜੋ_sock(ਬਣਤਰ curl_pollfds *cpfds,
                               curl_socket_t sock, ਛੋਟਾ events, bool fold)
{
  ਪੂਰਨ i;

  ਜੇ(fold && cpfds->n <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
    ਲਈ(i = (ਪੂਰਨ)cpfds->n - 1; i >= 0; --i) {
      ਜੇ(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ਵਾਪਸ CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ਜੇ(cpfds->n >= cpfds->ਗਿਣਤੀ_2 && cpfds_increase(cpfds, 100))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_pollfds_ਜੋੜੋ_sock(ਬਣਤਰ curl_pollfds *cpfds,
                               curl_socket_t sock, ਛੋਟਾ events)
{
  ਵਾਪਸ cpfds_ਜੋੜੋ_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_ਜੋੜੋ_ps(ਬਣਤਰ curl_pollfds *cpfds,
                             ਬਣਤਰ easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  ਲਈ(i = 0; i < ps->n; i++) {
    ਛੋਟਾ events = 0;
    ਜੇ(ps->actions[i] & CURL_POLL_ਵਿੱਚ)
      events |= POLLIN;
    ਜੇ(ps->actions[i] & CURL_POLL_ਬਾਹਰਲਾ)
      events |= POLLOUT;
    ਜੇ(events && cpfds_ਜੋੜੋ_sock(cpfds, ps->sockets[i], events, TRUE))
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_waitfds_init(ਬਣਤਰ Curl_waitfds *cwfds,
                       ਬਣਤਰ curl_waitfd *static_wfds,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ static_ਗਿਣਤੀ)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_ਗਿਣਤੀ);
  memset(cwfds, 0, ਆਕਾਰ(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->ਗਿਣਤੀ_2 = static_ਗਿਣਤੀ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cwfds_ਜੋੜੋ_sock(ਬਣਤਰ Curl_waitfds *cwfds,
                                   curl_socket_t sock, ਛੋਟਾ events)
{
  ਪੂਰਨ i;
  ਜੇ(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->ਗਿਣਤੀ_2 && !cwfds->n);
    ਵਾਪਸ 1;
  }
  ਜੇ(cwfds->n <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
    ਲਈ(i = (ਪੂਰਨ)cwfds->n - 1; i >= 0; --i) {
      ਜੇ(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ਵਾਪਸ 0;
      }
    }
  }
  /* not folded, add new entry */
  ਜੇ(cwfds->n < cwfds->ਗਿਣਤੀ_2) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ਵਾਪਸ 1;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ Curl_waitfds_ਜੋੜੋ_ps(ਬਣਤਰ Curl_waitfds *cwfds,
                                 ਬਣਤਰ easy_pollset *ps)
{
  size_t i;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  ਲਈ(i = 0; i < ps->n; i++) {
    ਛੋਟਾ events = 0;
    ਜੇ(ps->actions[i] & CURL_POLL_ਵਿੱਚ)
      events |= CURL_WAIT_POLLIN;
    ਜੇ(ps->actions[i] & CURL_POLL_ਬਾਹਰਲਾ)
      events |= CURL_WAIT_POLLOUT;
    ਜੇ(events)
      need += cwfds_ਜੋੜੋ_sock(cwfds, ps->sockets[i], events);
  }
  ਵਾਪਸ need;
}

ਖਾਲੀ Curl_pollset_reset(ਬਣਤਰ easy_pollset *ps)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#ਜੇ_ਅੰਤ
  ps->n = 0;
}

ਖਾਲੀ Curl_pollset_init(ਬਣਤਰ easy_pollset *ps)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#ਜੇ_ਅੰਤ
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->ਗਿਣਤੀ_2 = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ਬਣਤਰ easy_pollset *Curl_pollset_create(ਖਾਲੀ)
{
  ਬਣਤਰ easy_pollset *ps = curlx_calloc(1, ਆਕਾਰ(*ps));
  ਜੇ(ps)
    Curl_pollset_init(ps);
  ਵਾਪਸ ps;
}

ਖਾਲੀ Curl_pollset_cleanup(ਬਣਤਰ easy_pollset *ps)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#ਜੇ_ਅੰਤ
  ਜੇ(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ਜੇ(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->ਗਿਣਤੀ_2 = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

ਖਾਲੀ Curl_pollset_move(ਬਣਤਰ easy_pollset *ਮੰਜ਼ਿਲ, ਬਣਤਰ easy_pollset *from)
{
  Curl_pollset_cleanup(ਮੰਜ਼ਿਲ); /* deallocate anything in to */
  ਜੇ(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    ਮੰਜ਼ਿਲ->sockets = from->sockets;
    ਮੰਜ਼ਿਲ->actions = from->actions;
    ਮੰਜ਼ਿਲ->ਗਿਣਤੀ_2 = from->ਗਿਣਤੀ_2;
    ਮੰਜ਼ਿਲ->n = from->n;
    Curl_pollset_init(from);
  }
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(ਮੰਜ਼ਿਲ->sockets == ਮੰਜ਼ਿਲ->def_sockets);
    DEBUGASSERT(ਮੰਜ਼ਿਲ->actions == ਮੰਜ਼ਿਲ->def_actions);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੰਜ਼ਿਲ->sockets, from->sockets, ਮੰਜ਼ਿਲ->ਗਿਣਤੀ_2 * ਆਕਾਰ(ਮੰਜ਼ਿਲ->sockets[0]));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੰਜ਼ਿਲ->actions, from->actions, ਮੰਜ਼ਿਲ->ਗਿਣਤੀ_2 * ਆਕਾਰ(ਮੰਜ਼ਿਲ->actions[0]));
    ਮੰਜ਼ਿਲ->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps, curl_socket_t sock,
                             ਪੂਰਨ ਜੋੜੋ_flags, ਪੂਰਨ remove_flags)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#ਜੇ_ਅੰਤ

  DEBUGASSERT(VALID_SOCK(sock));
  ਜੇ(!VALID_SOCK(sock))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  DEBUGASSERT(ਜੋੜੋ_flags <= (CURL_POLL_ਵਿੱਚ | CURL_POLL_ਬਾਹਰਲਾ));
  DEBUGASSERT(remove_flags <= (CURL_POLL_ਵਿੱਚ | CURL_POLL_ਬਾਹਰਲਾ));
  DEBUGASSERT((ਜੋੜੋ_flags & remove_flags) == 0); /* no overlap */
  ਲਈ(i = 0; i < ps->n; ++i) {
    ਜੇ(ps->sockets[i] == sock) {
      ps->actions[i] &= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(~remove_flags);
      ps->actions[i] |= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਜੋੜੋ_flags;
      /* all gone? remove socket */
      ਜੇ(!ps->actions[i]) {
        ਜੇ((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * ਆਕਾਰ(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * ਆਕਾਰ(ps->actions[0]));
        }
        --ps->n;
      }
      ਵਾਪਸ CURLE_OK;
    }
  }
  /* not present */
  ਜੇ(ਜੋੜੋ_flags) {
    ਜੇ(i >= ps->ਗਿਣਤੀ_2) { /* need to grow */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ new_ਗਿਣਤੀ = CURLMAX(ps->ਗਿਣਤੀ_2 * 2, 8);
      curl_socket_t *nsockets;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *nactions;

      CURL_TRC_M(ਅੰਕੜੇ, "growing pollset capacity from %u to %u",
                 ps->ਗਿਣਤੀ_2, new_ਗਿਣਤੀ);
      ਜੇ(new_ਗਿਣਤੀ <= ps->ਗਿਣਤੀ_2)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      nsockets = curlx_calloc(new_ਗਿਣਤੀ, ਆਕਾਰ(nsockets[0]));
      ਜੇ(!nsockets)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      nactions = curlx_calloc(new_ਗਿਣਤੀ, ਆਕਾਰ(nactions[0]));
      ਜੇ(!nactions) {
        curlx_free(nsockets);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(nsockets, ps->sockets, ps->ਗਿਣਤੀ_2 * ਆਕਾਰ(ps->sockets[0]));
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(nactions, ps->actions, ps->ਗਿਣਤੀ_2 * ਆਕਾਰ(ps->actions[0]));
      ਜੇ(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ਜੇ(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->ਗਿਣਤੀ_2 = new_ਗਿਣਤੀ;
    }
    DEBUGASSERT(i < ps->ਗਿਣਤੀ_2);
    ਜੇ(i < ps->ਗਿਣਤੀ_2) {
      ps->sockets[i] = sock;
      ps->actions[i] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਜੋੜੋ_flags;
      ps->n = i + 1;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_pollset_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ easy_pollset *ps, curl_socket_t sock,
                          bool do_ਵਿੱਚ, bool do_ਬਾਹਰਲਾ)
{
  ਵਾਪਸ Curl_pollset_change(ਅੰਕੜੇ, ps, sock,
                             (do_ਵਿੱਚ ? CURL_POLL_ਵਿੱਚ : 0) |
                             (do_ਬਾਹਰਲਾ ? CURL_POLL_ਬਾਹਰਲਾ : 0),
                             (!do_ਵਿੱਚ ? CURL_POLL_ਵਿੱਚ : 0) |
                             (!do_ਬਾਹਰਲਾ ? CURL_POLL_ਬਾਹਰਲਾ : 0));
}

ਖਾਲੀ Curl_pollset_remove(ਬਣਤਰ easy_pollset *ps, curl_socket_t sock)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  ਲਈ(i = 0; i < ps->n; ++i) {
    ਜੇ(ps->sockets[i] == sock) {
      ਜੇ((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * ਆਕਾਰ(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * ਆਕਾਰ(ps->actions[0]));
      }
      --ps->n;
      ਵਾਪਸ;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ਪੂਰਨ Curl_pollset_poll(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ਬਣਤਰ pollfd *pfds;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i, npfds;
  ਪੂਰਨ rc;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);

  ਜੇ(!ps->n)
    ਵਾਪਸ curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, ਆਕਾਰ(*pfds));
  ਜੇ(!pfds)
    ਵਾਪਸ -1;

  npfds = 0;
  ਲਈ(i = 0; i < ps->n; ++i) {
    ਛੋਟਾ events = 0;
    ਜੇ(ps->actions[i] & CURL_POLL_ਵਿੱਚ) {
      events |= POLLIN;
    }
    ਜੇ(ps->actions[i] & CURL_POLL_ਬਾਹਰਲਾ) {
      events |= POLLOUT;
    }
    ਜੇ(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ਵਾਪਸ rc;
}

ਖਾਲੀ Curl_pollset_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(VALID_SOCK(sock));
  ਲਈ(i = 0; i < ps->n; ++i) {
    ਜੇ(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_ਵਿੱਚ);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_ਬਾਹਰਲਾ);
      ਵਾਪਸ;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps,
                            curl_socket_t sock)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਲਈ(i = 0; i < ps->n; ++i) {
    ਜੇ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_ਵਿੱਚ))
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

bool Curl_pollset_want_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps,
                            curl_socket_t sock)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਲਈ(i = 0; i < ps->n; ++i) {
    ਜੇ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_ਬਾਹਰਲਾ))
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}
