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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#ଅନ୍ତର୍ଭାଵ <sys/select.h>
#ନଚେତଯଦି defined(HAVE_UNISTD_H)
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timediff.h"
#ଅନ୍ତର୍ଭାଵ "curlx/wait.h"

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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ସଂରଚନା timeval pending_tv;
  ସଂରଚନା timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  ଯଦି((!fds_read || fds_read->fd_ଗଣନା == 0) &&
     (!fds_write || fds_write->fd_ଗଣନା == 0) &&
     (!fds_err || fds_err->fd_ଗଣନା == 0)) {
    /* no sockets, wait */
    ଫେରନ୍ତୁ curlx_wait_ms(timeout_ms);
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
  ଫେରନ୍ତୁ select((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)maxfd + 1,
                fds_read && fds_read->fd_ଗଣନା ? fds_read : ଖାଲି,
                fds_write && fds_write->fd_ଗଣନା ? fds_write : ଖାଲି,
                fds_err && fds_err->fd_ଗଣନା ? fds_err : ଖାଲି, ptimeout);
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ select((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ସଂରଚନା pollfd pfd[3];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

  ଯଦି((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ଫେରନ୍ତୁ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  ଅଙ୍କ = 0;
  ଯଦି(readfd0 != CURL_SOCKET_BAD) {
    pfd[ଅଙ୍କ].fd = readfd0;
    pfd[ଅଙ୍କ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ଅଙ୍କ].revents = 0;
    ଅଙ୍କ++;
  }
  ଯଦି(readfd1 != CURL_SOCKET_BAD) {
    pfd[ଅଙ୍କ].fd = readfd1;
    pfd[ଅଙ୍କ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[ଅଙ୍କ].revents = 0;
    ଅଙ୍କ++;
  }
  ଯଦି(writefd != CURL_SOCKET_BAD) {
    pfd[ଅଙ୍କ].fd = writefd;
    pfd[ଅଙ୍କ].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[ଅଙ୍କ].revents = 0;
    ଅଙ୍କ++;
  }

  r = Curl_poll(pfd, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ, timeout_ms);
  ଯଦି(r <= 0)
    ଫେରନ୍ତୁ r;

  r = 0;
  ଅଙ୍କ = 0;
  ଯଦି(readfd0 != CURL_SOCKET_BAD) {
    ଯଦି(pfd[ଅଙ୍କ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    ଯଦି(pfd[ଅଙ୍କ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ଅଙ୍କ++;
  }
  ଯଦି(readfd1 != CURL_SOCKET_BAD) {
    ଯଦି(pfd[ଅଙ୍କ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ଯଦି(pfd[ଅଙ୍କ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    ଅଙ୍କ++;
  }
  ଯଦି(writefd != CURL_SOCKET_BAD) {
    ଯଦି(pfd[ଅଙ୍କ].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    ଯଦି(pfd[ଅଙ୍କ].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ଫେରନ୍ତୁ r;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_poll(ସଂରଚନା pollfd ufds[], unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pending_ms;
#ଅନ୍ୟଥା
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

  ଯଦି(ufds) {
    ପାଇଁ(i = 0; i < nfds; i++) {
      ଯଦି(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        ଭାଙ୍ଗ;
      }
    }
  }
  ଯଦି(fds_none) {
    /* no sockets, wait */
    ଫେରନ୍ତୁ curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ଯଦି TIMEDIFF_T_MAX > INT_MAX
  ଯଦି(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  ଯଦି(timeout_ms > 0)
    pending_ms = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)timeout_ms;
  ଅନ୍ୟଥା ଯଦି(timeout_ms < 0)
    pending_ms = -1;
  ଅନ୍ୟଥା
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ଯଦି(r <= 0) {
    ଯଦି((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ଫେରନ୍ତୁ r;
  }

  ପାଇଁ(i = 0; i < nfds; i++) {
    ଯଦି(ufds[i].fd == CURL_SOCKET_BAD)
      ଜାରି;
    ଯଦି(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ଯଦି(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#ଅନ୍ୟଥା /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  ପାଇଁ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ଯଦି(ufds[i].fd == CURL_SOCKET_BAD)
      ଜାରି;
    VERIFY_SOCK(ufds[i].fd);
    ଯଦି(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ଯଦି(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ଯଦି(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ଯଦି(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ଯଦି(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ଯଦି(r <= 0) {
    ଯଦି((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ଫେରନ୍ତୁ r;
  }

  r = 0;
  ପାଇଁ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ଯଦି(ufds[i].fd == CURL_SOCKET_BAD)
      ଜାରି;
    ଯଦି(FD_ISSET(ufds[i].fd, &fds_read)) {
      ଯଦି(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ଯଦି(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ଯଦି(FD_ISSET(ufds[i].fd, &fds_write)) {
      ଯଦି(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ଯଦି(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ଯଦି(FD_ISSET(ufds[i].fd, &fds_err)) {
      ଯଦି(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ଯଦି(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ଯଦି(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  ଫେରନ୍ତୁ r;
}

ଶୂନ୍ୟ Curl_pollfds_init(ସଂରଚନା curl_pollfds *cpfds,
                       ସଂରଚନା pollfd *static_pfds,
                       unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା static_ଗଣନା)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, ଆକାର(*cpfds));
  ଯଦି(static_pfds && static_ଗଣନା) {
    cpfds->pfds = static_pfds;
    cpfds->ଗଣନା_2 = static_ଗଣନା;
  }
}

ଶୂନ୍ୟ Curl_pollfds_reset(ସଂରଚନା curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

ଶୂନ୍ୟ Curl_pollfds_cleanup(ସଂରଚନା curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ଯଦି(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->ଗଣନା_2 = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ସ୍ଥିର CURLcode cpfds_increase(ସଂରଚନା curl_pollfds *cpfds, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା inc)
{
  ସଂରଚନା pollfd *new_fds;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ଗଣନା = cpfds->ଗଣନା_2 + inc;

  new_fds = curlx_calloc(new_ଗଣନା, ଆକାର(ସଂରଚନା pollfd));
  ଯଦି(!new_fds)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->ଗଣନା_2 * ଆକାର(ସଂରଚନା pollfd));
  ଯଦି(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->ଗଣନା_2 = new_ଗଣନା;
  cpfds->allocated_pfds = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cpfds_add_sock(ସଂରଚନା curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି(fold && cpfds->n <= INT_MAX) {
    ପାଇଁ(i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cpfds->n - 1; i >= 0; --i) {
      ଯଦି(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ଫେରନ୍ତୁ CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ଯଦି(cpfds->n >= cpfds->ଗଣନା_2 && cpfds_increase(cpfds, 100))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(ସଂରଚନା curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  ଫେରନ୍ତୁ cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(ସଂରଚନା curl_pollfds *cpfds,
                             ସଂରଚନା easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  ପାଇଁ(i = 0; i < ps->n; i++) {
    short events = 0;
    ଯଦି(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    ଯଦି(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    ଯଦି(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_waitfds_init(ସଂରଚନା Curl_waitfds *cwfds,
                       ସଂରଚନା curl_waitfd *static_wfds,
                       unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା static_ଗଣନା)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_ଗଣନା);
  memset(cwfds, 0, ଆକାର(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->ଗଣନା_2 = static_ଗଣନା;
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cwfds_add_sock(ସଂରଚନା Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଯଦି(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->ଗଣନା_2 && !cwfds->n);
    ଫେରନ୍ତୁ 1;
  }
  ଯଦି(cwfds->n <= INT_MAX) {
    ପାଇଁ(i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cwfds->n - 1; i >= 0; --i) {
      ଯଦି(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ଫେରନ୍ତୁ 0;
      }
    }
  }
  /* not folded, add new entry */
  ଯଦି(cwfds->n < cwfds->ଗଣନା_2) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ଫେରନ୍ତୁ 1;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_waitfds_add_ps(ସଂରଚନା Curl_waitfds *cwfds,
                                 ସଂରଚନା easy_pollset *ps)
{
  size_t i;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  ପାଇଁ(i = 0; i < ps->n; i++) {
    short events = 0;
    ଯଦି(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    ଯଦି(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    ଯଦି(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  ଫେରନ୍ତୁ need;
}

ଶୂନ୍ୟ Curl_pollset_reset(ସଂରଚନା easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

ଶୂନ୍ୟ Curl_pollset_init(ସଂରଚନା easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->ଗଣନା_2 = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ସଂରଚନା easy_pollset *Curl_pollset_create(ଶୂନ୍ୟ)
{
  ସଂରଚନା easy_pollset *ps = curlx_calloc(1, ଆକାର(*ps));
  ଯଦି(ps)
    Curl_pollset_init(ps);
  ଫେରନ୍ତୁ ps;
}

ଶୂନ୍ୟ Curl_pollset_cleanup(ସଂରଚନା easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ଯଦି(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ଯଦି(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->ଗଣନା_2 = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

ଶୂନ୍ୟ Curl_pollset_move(ସଂରଚନା easy_pollset *to, ସଂରଚନା easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  ଯଦି(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->ଗଣନା_2 = from->ଗଣନା_2;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  ଅନ୍ୟଥା {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->ଗଣନା_2 * ଆକାର(to->sockets[0]));
    memcpy(to->actions, from->actions, to->ଗଣନା_2 * ଆକାର(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା easy_pollset *ps, curl_socket_t sock,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remove_flags)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  ଯଦି(!VALID_SOCK(sock))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  ପାଇଁ(i = 0; i < ps->n; ++i) {
    ଯଦି(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned ଅକ୍ଷର)(~remove_flags);
      ps->actions[i] |= (unsigned ଅକ୍ଷର)add_flags;
      /* all gone? remove socket */
      ଯଦି(!ps->actions[i]) {
        ଯଦି((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * ଆକାର(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * ଆକାର(ps->actions[0]));
        }
        --ps->n;
      }
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }
  /* not present */
  ଯଦି(add_flags) {
    ଯଦି(i >= ps->ଗଣନା_2) { /* need to grow */
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ଗଣନା = CURLMAX(ps->ଗଣନା_2 * 2, 8);
      curl_socket_t *nsockets;
      unsigned ଅକ୍ଷର *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->ଗଣନା_2, new_ଗଣନା);
      ଯଦି(new_ଗଣନା <= ps->ଗଣନା_2)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_ଗଣନା, ଆକାର(nsockets[0]));
      ଯଦି(!nsockets)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_ଗଣନା, ଆକାର(nactions[0]));
      ଯଦି(!nactions) {
        curlx_free(nsockets);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->ଗଣନା_2 * ଆକାର(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->ଗଣନା_2 * ଆକାର(ps->actions[0]));
      ଯଦି(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ଯଦି(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->ଗଣନା_2 = new_ଗଣନା;
    }
    DEBUGASSERT(i < ps->ଗଣନା_2);
    ଯଦି(i < ps->ଗଣନା_2) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned ଅକ୍ଷର)add_flags;
      ps->n = i + 1;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_pollset_set(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  ଫେରନ୍ତୁ Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

ଶୂନ୍ୟ Curl_pollset_remove(ସଂରଚନା easy_pollset *ps, curl_socket_t sock)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପାଇଁ(i = 0; i < ps->n; ++i) {
    ଯଦି(ps->sockets[i] == sock) {
      ଯଦି((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * ଆକାର(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * ଆକାର(ps->actions[0]));
      }
      --ps->n;
      ଫେରନ୍ତୁ;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_pollset_poll(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ସଂରଚନା pollfd *pfds;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, npfds;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  ଯଦି(!ps->n)
    ଫେରନ୍ତୁ curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, ଆକାର(*pfds));
  ଯଦି(!pfds)
    ଫେରନ୍ତୁ -1;

  npfds = 0;
  ପାଇଁ(i = 0; i < ps->n; ++i) {
    short events = 0;
    ଯଦି(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    ଯଦି(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    ଯଦି(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ଫେରନ୍ତୁ rc;
}

ଶୂନ୍ୟ Curl_pollset_check(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(VALID_SOCK(sock));
  ପାଇଁ(i = 0; i < ps->n; ++i) {
    ଯଦି(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      ଫେରନ୍ତୁ;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  (ଶୂନ୍ୟ)data;
  ପାଇଁ(i = 0; i < ps->n; ++i) {
    ଯଦି((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_pollset_want_send(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  (ଶୂନ୍ୟ)data;
  ପାଇଁ(i = 0; i < ps->n; ++i) {
    ଯଦି((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}
