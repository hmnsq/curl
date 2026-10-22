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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_SELECT_H
#ಅನ್ತರ್ಭಾವ <sys/select.h>
#ಅಥವಾಆದರೆ defined(HAVE_UNISTD_H)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_POLL  /* use select() */
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ರಚನೆ timeval pending_tv;
  ರಚನೆ timeval *ptimeout;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  ಆದರೆ((!fds_read || fds_read->fd_ಎಣಿಕೆ == 0) &&
     (!fds_write || fds_write->fd_ಎಣಿಕೆ == 0) &&
     (!fds_err || fds_err->fd_ಎಣಿಕೆ == 0)) {
    /* no sockets, wait */
    ಹಿಂತಿರುಗಿ curlx_wait_ms(timeout_ms);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
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
  ಹಿಂತಿರುಗಿ select((ಪೂರ್ಣಾಂಕ)maxfd + 1,
                fds_read && fds_read->fd_ಎಣಿಕೆ ? fds_read : ಖಾಲಿ,
                fds_write && fds_write->fd_ಎಣಿಕೆ ? fds_write : ಖಾಲಿ,
                fds_err && fds_err->fd_ಎಣಿಕೆ ? fds_err : ಖಾಲಿ, ptimeout);
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ select((ಪೂರ್ಣಾಂಕ)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ

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
ಪೂರ್ಣಾಂಕ Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ರಚನೆ pollfd pfd[3];
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ;
  ಪೂರ್ಣಾಂಕ r;

  ಆದರೆ((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ಹಿಂತಿರುಗಿ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  ಅಂಕೆ = 0;
  ಆದರೆ(readfd0 != CURL_SOCKET_BAD) {
    pfd[ಅಂಕೆ].fd = readfd0;
    pfd[ಅಂಕೆ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ಅಂಕೆ].revents = 0;
    ಅಂಕೆ++;
  }
  ಆದರೆ(readfd1 != CURL_SOCKET_BAD) {
    pfd[ಅಂಕೆ].fd = readfd1;
    pfd[ಅಂಕೆ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ಅಂಕೆ].revents = 0;
    ಅಂಕೆ++;
  }
  ಆದರೆ(writefd != CURL_SOCKET_BAD) {
    pfd[ಅಂಕೆ].fd = writefd;
    pfd[ಅಂಕೆ].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[ಅಂಕೆ].revents = 0;
    ಅಂಕೆ++;
  }

  r = Curl_poll(pfd, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಂಕೆ, timeout_ms);
  ಆದರೆ(r <= 0)
    ಹಿಂತಿರುಗಿ r;

  r = 0;
  ಅಂಕೆ = 0;
  ಆದರೆ(readfd0 != CURL_SOCKET_BAD) {
    ಆದರೆ(pfd[ಅಂಕೆ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_ಒಳ;
    ಆದರೆ(pfd[ಅಂಕೆ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ಅಂಕೆ++;
  }
  ಆದರೆ(readfd1 != CURL_SOCKET_BAD) {
    ಆದರೆ(pfd[ಅಂಕೆ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ಆದರೆ(pfd[ಅಂಕೆ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ಅಂಕೆ++;
  }
  ಆದರೆ(writefd != CURL_SOCKET_BAD) {
    ಆದರೆ(pfd[ಅಂಕೆ].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_ಹೊರಹೋಗು;
    ಆದರೆ(pfd[ಅಂಕೆ].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ಹಿಂತಿರುಗಿ r;
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
ಪೂರ್ಣಾಂಕ Curl_poll(ರಚನೆ pollfd ufds[], ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nfds, timediff_t timeout_ms)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_POLL
  ಪೂರ್ಣಾಂಕ pending_ms;
#ಇಲ್ಲದಿದ್ದರೆ
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#ಆದರೆ_ಅಂತ್ಯ
  bool fds_none = TRUE;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  ಪೂರ್ಣಾಂಕ r;

  ಆದರೆ(ufds) {
    ಗಾಗಿ(i = 0; i < nfds; i++) {
      ಆದರೆ(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಆದರೆ(fds_none) {
    /* no sockets, wait */
    ಹಿಂತಿರುಗಿ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ಆದರೆ TIMEDIFF_T_MAX > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
  ಆದರೆ(timeout_ms > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
    timeout_ms = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(timeout_ms > 0)
    pending_ms = (ಪೂರ್ಣಾಂಕ)timeout_ms;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(timeout_ms < 0)
    pending_ms = -1;
  ಇಲ್ಲದಿದ್ದರೆ
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ಆದರೆ(r <= 0) {
    ಆದರೆ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ಹಿಂತಿರುಗಿ r;
  }

  ಗಾಗಿ(i = 0; i < nfds; i++) {
    ಆದರೆ(ufds[i].fd == CURL_SOCKET_BAD)
      ಮುಂದುವರಿ;
    ಆದರೆ(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ಆದರೆ(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#ಇಲ್ಲದಿದ್ದರೆ /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  ಗಾಗಿ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ಆದರೆ(ufds[i].fd == CURL_SOCKET_BAD)
      ಮುಂದುವರಿ;
    VERIFY_SOCK(ufds[i].fd);
    ಆದರೆ(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ಆದರೆ(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ಆದರೆ(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ಆದರೆ(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ಆದರೆ(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ಆದರೆ(r <= 0) {
    ಆದರೆ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ಹಿಂತಿರುಗಿ r;
  }

  r = 0;
  ಗಾಗಿ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ಆದರೆ(ufds[i].fd == CURL_SOCKET_BAD)
      ಮುಂದುವರಿ;
    ಆದರೆ(FD_ISSET(ufds[i].fd, &fds_read)) {
      ಆದರೆ(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ಆದರೆ(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ಆದರೆ(FD_ISSET(ufds[i].fd, &fds_write)) {
      ಆದರೆ(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ಆದರೆ(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ಆದರೆ(FD_ISSET(ufds[i].fd, &fds_err)) {
      ಆದರೆ(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ಆದರೆ(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ಆದರೆ(ufds[i].revents)
      r++;
  }

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_POLL */

  ಹಿಂತಿರುಗಿ r;
}

ಶೂನ್ಯ Curl_pollfds_init(ರಚನೆ curl_pollfds *cpfds,
                       ರಚನೆ pollfd *static_pfds,
                       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ static_ಎಣಿಕೆ)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, ಗಾತ್ರ(*cpfds));
  ಆದರೆ(static_pfds && static_ಎಣಿಕೆ) {
    cpfds->pfds = static_pfds;
    cpfds->ಎಣಿಕೆ_2 = static_ಎಣಿಕೆ;
  }
}

ಶೂನ್ಯ Curl_pollfds_reset(ರಚನೆ curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

ಶೂನ್ಯ Curl_pollfds_cleanup(ರಚನೆ curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ಆದರೆ(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->ಎಣಿಕೆ_2 = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ಸ್ಥಿರ CURLcode cpfds_increase(ರಚನೆ curl_pollfds *cpfds, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ inc)
{
  ರಚನೆ pollfd *new_fds;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ new_ಎಣಿಕೆ = cpfds->ಎಣಿಕೆ_2 + inc;

  new_fds = curlx_calloc(new_ಎಣಿಕೆ, ಗಾತ್ರ(ರಚನೆ pollfd));
  ಆದರೆ(!new_fds)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಸ್ಮರಣೆ_ನಕಲು(new_fds, cpfds->pfds, cpfds->ಎಣಿಕೆ_2 * ಗಾತ್ರ(ರಚನೆ pollfd));
  ಆದರೆ(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->ಎಣಿಕೆ_2 = new_ಎಣಿಕೆ;
  cpfds->allocated_pfds = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cpfds_ಕೂಡಿಸು_sock(ರಚನೆ curl_pollfds *cpfds,
                               curl_socket_t sock, ಗಿಡ್ಡ events, bool fold)
{
  ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ(fold && cpfds->n <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
    ಗಾಗಿ(i = (ಪೂರ್ಣಾಂಕ)cpfds->n - 1; i >= 0; --i) {
      ಆದರೆ(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ಆದರೆ(cpfds->n >= cpfds->ಎಣಿಕೆ_2 && cpfds_increase(cpfds, 100))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_pollfds_ಕೂಡಿಸು_sock(ರಚನೆ curl_pollfds *cpfds,
                               curl_socket_t sock, ಗಿಡ್ಡ events)
{
  ಹಿಂತಿರುಗಿ cpfds_ಕೂಡಿಸು_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_ಕೂಡಿಸು_ps(ರಚನೆ curl_pollfds *cpfds,
                             ರಚನೆ easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  ಗಾಗಿ(i = 0; i < ps->n; i++) {
    ಗಿಡ್ಡ events = 0;
    ಆದರೆ(ps->actions[i] & CURL_POLL_ಒಳ)
      events |= POLLIN;
    ಆದರೆ(ps->actions[i] & CURL_POLL_ಹೊರಹೋಗು)
      events |= POLLOUT;
    ಆದರೆ(events && cpfds_ಕೂಡಿಸು_sock(cpfds, ps->sockets[i], events, TRUE))
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_waitfds_init(ರಚನೆ Curl_waitfds *cwfds,
                       ರಚನೆ curl_waitfd *static_wfds,
                       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ static_ಎಣಿಕೆ)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_ಎಣಿಕೆ);
  memset(cwfds, 0, ಗಾತ್ರ(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->ಎಣಿಕೆ_2 = static_ಎಣಿಕೆ;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cwfds_ಕೂಡಿಸು_sock(ರಚನೆ Curl_waitfds *cwfds,
                                   curl_socket_t sock, ಗಿಡ್ಡ events)
{
  ಪೂರ್ಣಾಂಕ i;
  ಆದರೆ(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->ಎಣಿಕೆ_2 && !cwfds->n);
    ಹಿಂತಿರುಗಿ 1;
  }
  ಆದರೆ(cwfds->n <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
    ಗಾಗಿ(i = (ಪೂರ್ಣಾಂಕ)cwfds->n - 1; i >= 0; --i) {
      ಆದರೆ(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ಹಿಂತಿರುಗಿ 0;
      }
    }
  }
  /* not folded, add new entry */
  ಆದರೆ(cwfds->n < cwfds->ಎಣಿಕೆ_2) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ಹಿಂತಿರುಗಿ 1;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ Curl_waitfds_ಕೂಡಿಸು_ps(ರಚನೆ Curl_waitfds *cwfds,
                                 ರಚನೆ easy_pollset *ps)
{
  size_t i;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  ಗಾಗಿ(i = 0; i < ps->n; i++) {
    ಗಿಡ್ಡ events = 0;
    ಆದರೆ(ps->actions[i] & CURL_POLL_ಒಳ)
      events |= CURL_WAIT_POLLIN;
    ಆದರೆ(ps->actions[i] & CURL_POLL_ಹೊರಹೋಗು)
      events |= CURL_WAIT_POLLOUT;
    ಆದರೆ(events)
      need += cwfds_ಕೂಡಿಸು_sock(cwfds, ps->sockets[i], events);
  }
  ಹಿಂತಿರುಗಿ need;
}

ಶೂನ್ಯ Curl_pollset_reset(ರಚನೆ easy_pollset *ps)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#ಆದರೆ_ಅಂತ್ಯ
  ps->n = 0;
}

ಶೂನ್ಯ Curl_pollset_init(ರಚನೆ easy_pollset *ps)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->ಎಣಿಕೆ_2 = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ರಚನೆ easy_pollset *Curl_pollset_create(ಶೂನ್ಯ)
{
  ರಚನೆ easy_pollset *ps = curlx_calloc(1, ಗಾತ್ರ(*ps));
  ಆದರೆ(ps)
    Curl_pollset_init(ps);
  ಹಿಂತಿರುಗಿ ps;
}

ಶೂನ್ಯ Curl_pollset_cleanup(ರಚನೆ easy_pollset *ps)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ಆದರೆ(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->ಎಣಿಕೆ_2 = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

ಶೂನ್ಯ Curl_pollset_move(ರಚನೆ easy_pollset *ಗಮ್ಯ, ರಚನೆ easy_pollset *from)
{
  Curl_pollset_cleanup(ಗಮ್ಯ); /* deallocate anything in to */
  ಆದರೆ(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    ಗಮ್ಯ->sockets = from->sockets;
    ಗಮ್ಯ->actions = from->actions;
    ಗಮ್ಯ->ಎಣಿಕೆ_2 = from->ಎಣಿಕೆ_2;
    ಗಮ್ಯ->n = from->n;
    Curl_pollset_init(from);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(ಗಮ್ಯ->sockets == ಗಮ್ಯ->def_sockets);
    DEBUGASSERT(ಗಮ್ಯ->actions == ಗಮ್ಯ->def_actions);
    ಸ್ಮರಣೆ_ನಕಲು(ಗಮ್ಯ->sockets, from->sockets, ಗಮ್ಯ->ಎಣಿಕೆ_2 * ಗಾತ್ರ(ಗಮ್ಯ->sockets[0]));
    ಸ್ಮರಣೆ_ನಕಲು(ಗಮ್ಯ->actions, from->actions, ಗಮ್ಯ->ಎಣಿಕೆ_2 * ಗಾತ್ರ(ಗಮ್ಯ->actions[0]));
    ಗಮ್ಯ->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps, curl_socket_t sock,
                             ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_flags, ಪೂರ್ಣಾಂಕ remove_flags)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#ಆದರೆ_ಅಂತ್ಯ

  DEBUGASSERT(VALID_SOCK(sock));
  ಆದರೆ(!VALID_SOCK(sock))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  DEBUGASSERT(ಕೂಡಿಸು_flags <= (CURL_POLL_ಒಳ | CURL_POLL_ಹೊರಹೋಗು));
  DEBUGASSERT(remove_flags <= (CURL_POLL_ಒಳ | CURL_POLL_ಹೊರಹೋಗು));
  DEBUGASSERT((ಕೂಡಿಸು_flags & remove_flags) == 0); /* no overlap */
  ಗಾಗಿ(i = 0; i < ps->n; ++i) {
    ಆದರೆ(ps->sockets[i] == sock) {
      ps->actions[i] &= (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(~remove_flags);
      ps->actions[i] |= (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಕೂಡಿಸು_flags;
      /* all gone? remove socket */
      ಆದರೆ(!ps->actions[i]) {
        ಆದರೆ((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * ಗಾತ್ರ(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * ಗಾತ್ರ(ps->actions[0]));
        }
        --ps->n;
      }
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }
  /* not present */
  ಆದರೆ(ಕೂಡಿಸು_flags) {
    ಆದರೆ(i >= ps->ಎಣಿಕೆ_2) { /* need to grow */
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ new_ಎಣಿಕೆ = CURLMAX(ps->ಎಣಿಕೆ_2 * 2, 8);
      curl_socket_t *nsockets;
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *nactions;

      CURL_TRC_M(ದತ್ತಾಂಶ, "growing pollset capacity from %u to %u",
                 ps->ಎಣಿಕೆ_2, new_ಎಣಿಕೆ);
      ಆದರೆ(new_ಎಣಿಕೆ <= ps->ಎಣಿಕೆ_2)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      nsockets = curlx_calloc(new_ಎಣಿಕೆ, ಗಾತ್ರ(nsockets[0]));
      ಆದರೆ(!nsockets)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      nactions = curlx_calloc(new_ಎಣಿಕೆ, ಗಾತ್ರ(nactions[0]));
      ಆದರೆ(!nactions) {
        curlx_free(nsockets);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಸ್ಮರಣೆ_ನಕಲು(nsockets, ps->sockets, ps->ಎಣಿಕೆ_2 * ಗಾತ್ರ(ps->sockets[0]));
      ಸ್ಮರಣೆ_ನಕಲು(nactions, ps->actions, ps->ಎಣಿಕೆ_2 * ಗಾತ್ರ(ps->actions[0]));
      ಆದರೆ(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ಆದರೆ(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->ಎಣಿಕೆ_2 = new_ಎಣಿಕೆ;
    }
    DEBUGASSERT(i < ps->ಎಣಿಕೆ_2);
    ಆದರೆ(i < ps->ಎಣಿಕೆ_2) {
      ps->sockets[i] = sock;
      ps->actions[i] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಕೂಡಿಸು_flags;
      ps->n = i + 1;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_pollset_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ easy_pollset *ps, curl_socket_t sock,
                          bool do_ಒಳ, bool do_ಹೊರಹೋಗು)
{
  ಹಿಂತಿರುಗಿ Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock,
                             (do_ಒಳ ? CURL_POLL_ಒಳ : 0) |
                             (do_ಹೊರಹೋಗು ? CURL_POLL_ಹೊರಹೋಗು : 0),
                             (!do_ಒಳ ? CURL_POLL_ಒಳ : 0) |
                             (!do_ಹೊರಹೋಗು ? CURL_POLL_ಹೊರಹೋಗು : 0));
}

ಶೂನ್ಯ Curl_pollset_remove(ರಚನೆ easy_pollset *ps, curl_socket_t sock)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  ಗಾಗಿ(i = 0; i < ps->n; ++i) {
    ಆದರೆ(ps->sockets[i] == sock) {
      ಆದರೆ((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * ಗಾತ್ರ(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * ಗಾತ್ರ(ps->actions[0]));
      }
      --ps->n;
      ಹಿಂತಿರುಗಿ;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ಪೂರ್ಣಾಂಕ Curl_pollset_poll(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ರಚನೆ pollfd *pfds;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i, npfds;
  ಪೂರ್ಣಾಂಕ rc;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);

  ಆದರೆ(!ps->n)
    ಹಿಂತಿರುಗಿ curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, ಗಾತ್ರ(*pfds));
  ಆದರೆ(!pfds)
    ಹಿಂತಿರುಗಿ -1;

  npfds = 0;
  ಗಾಗಿ(i = 0; i < ps->n; ++i) {
    ಗಿಡ್ಡ events = 0;
    ಆದರೆ(ps->actions[i] & CURL_POLL_ಒಳ) {
      events |= POLLIN;
    }
    ಆದರೆ(ps->actions[i] & CURL_POLL_ಹೊರಹೋಗು) {
      events |= POLLOUT;
    }
    ಆದರೆ(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ಹಿಂತಿರುಗಿ rc;
}

ಶೂನ್ಯ Curl_pollset_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(VALID_SOCK(sock));
  ಗಾಗಿ(i = 0; i < ps->n; ++i) {
    ಆದರೆ(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_ಒಳ);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_ಹೊರಹೋಗು);
      ಹಿಂತಿರುಗಿ;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps,
                            curl_socket_t sock)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಗಾಗಿ(i = 0; i < ps->n; ++i) {
    ಆದರೆ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_ಒಳ))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_pollset_want_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps,
                            curl_socket_t sock)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಗಾಗಿ(i = 0; i < ps->n; ++i) {
    ಆದರೆ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_ಹೊರಹೋಗು))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}
