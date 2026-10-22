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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_SYS_SELECT_H
#అన్తర్భావ <sys/select.h>
#లేదావేళ defined(HAVE_UNISTD_H)
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/timediff.h"
#అన్తర్భావ "curlx/wait.h"

#నిర్వచితం_కాకపోతే HAVE_POLL  /* use select() */
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
స్థిర పూర్ణాంకం our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  నిర్మాణం timeval pending_tv;
  నిర్మాణం timeval *ptimeout;

#నిర్వచితమైతే USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  ఒకవేళ((!fds_read || fds_read->fd_లెక్క == 0) &&
     (!fds_write || fds_write->fd_లెక్క == 0) &&
     (!fds_err || fds_err->fd_లెక్క == 0)) {
    /* no sockets, wait */
    తిరిగి curlx_wait_ms(timeout_ms);
  }
#అయితే_ముగింపు

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#నిర్వచితమైతే USE_WINSOCK
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
  తిరిగి select((పూర్ణాంకం)maxfd + 1,
                fds_read && fds_read->fd_లెక్క ? fds_read : ఖాళీ,
                fds_write && fds_write->fd_లెక్క ? fds_write : ఖాళీ,
                fds_err && fds_err->fd_లెక్క ? fds_err : ఖాళీ, ptimeout);
#లేకపోతే
  తిరిగి select((పూర్ణాంకం)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#అయితే_ముగింపు
}

#అయితే_ముగింపు

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
పూర్ణాంకం Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  నిర్మాణం pollfd pfd[3];
  పూర్ణాంకం అంకె;
  పూర్ణాంకం r;

  ఒకవేళ((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    తిరిగి curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  అంకె = 0;
  ఒకవేళ(readfd0 != CURL_SOCKET_BAD) {
    pfd[అంకె].fd = readfd0;
    pfd[అంకె].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[అంకె].revents = 0;
    అంకె++;
  }
  ఒకవేళ(readfd1 != CURL_SOCKET_BAD) {
    pfd[అంకె].fd = readfd1;
    pfd[అంకె].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[అంకె].revents = 0;
    అంకె++;
  }
  ఒకవేళ(writefd != CURL_SOCKET_BAD) {
    pfd[అంకె].fd = writefd;
    pfd[అంకె].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[అంకె].revents = 0;
    అంకె++;
  }

  r = Curl_poll(pfd, (గుర్తులేని పూర్ణాంకం)అంకె, timeout_ms);
  ఒకవేళ(r <= 0)
    తిరిగి r;

  r = 0;
  అంకె = 0;
  ఒకవేళ(readfd0 != CURL_SOCKET_BAD) {
    ఒకవేళ(pfd[అంకె].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_లోపల;
    ఒకవేళ(pfd[అంకె].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    అంకె++;
  }
  ఒకవేళ(readfd1 != CURL_SOCKET_BAD) {
    ఒకవేళ(pfd[అంకె].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ఒకవేళ(pfd[అంకె].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    అంకె++;
  }
  ఒకవేళ(writefd != CURL_SOCKET_BAD) {
    ఒకవేళ(pfd[అంకె].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_బహిర్గతం;
    ఒకవేళ(pfd[అంకె].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  తిరిగి r;
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
పూర్ణాంకం Curl_poll(నిర్మాణం pollfd ufds[], గుర్తులేని పూర్ణాంకం nfds, timediff_t timeout_ms)
{
#నిర్వచితమైతే HAVE_POLL
  పూర్ణాంకం pending_ms;
#లేకపోతే
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#అయితే_ముగింపు
  bool fds_none = TRUE;
  గుర్తులేని పూర్ణాంకం i;
  పూర్ణాంకం r;

  ఒకవేళ(ufds) {
    కొరకు(i = 0; i < nfds; i++) {
      ఒకవేళ(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        ఆపు;
      }
    }
  }
  ఒకవేళ(fds_none) {
    /* no sockets, wait */
    తిరిగి curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#నిర్వచితమైతే HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ఒకవేళ TIMEDIFF_T_MAX > గరిష్ఠ_పూర్ణాంకం
  ఒకవేళ(timeout_ms > గరిష్ఠ_పూర్ణాంకం)
    timeout_ms = గరిష్ఠ_పూర్ణాంకం;
#అయితే_ముగింపు
  ఒకవేళ(timeout_ms > 0)
    pending_ms = (పూర్ణాంకం)timeout_ms;
  లేకపోతే ఒకవేళ(timeout_ms < 0)
    pending_ms = -1;
  లేకపోతే
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ఒకవేళ(r <= 0) {
    ఒకవేళ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    తిరిగి r;
  }

  కొరకు(i = 0; i < nfds; i++) {
    ఒకవేళ(ufds[i].fd == CURL_SOCKET_BAD)
      కొనసాగు;
    ఒకవేళ(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ఒకవేళ(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#లేకపోతే /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  కొరకు(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ఒకవేళ(ufds[i].fd == CURL_SOCKET_BAD)
      కొనసాగు;
    VERIFY_SOCK(ufds[i].fd);
    ఒకవేళ(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ఒకవేళ(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ఒకవేళ(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ఒకవేళ(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ఒకవేళ(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ఒకవేళ(r <= 0) {
    ఒకవేళ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    తిరిగి r;
  }

  r = 0;
  కొరకు(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ఒకవేళ(ufds[i].fd == CURL_SOCKET_BAD)
      కొనసాగు;
    ఒకవేళ(FD_ISSET(ufds[i].fd, &fds_read)) {
      ఒకవేళ(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ఒకవేళ(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ఒకవేళ(FD_ISSET(ufds[i].fd, &fds_write)) {
      ఒకవేళ(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ఒకవేళ(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ఒకవేళ(FD_ISSET(ufds[i].fd, &fds_err)) {
      ఒకవేళ(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ఒకవేళ(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ఒకవేళ(ufds[i].revents)
      r++;
  }

#అయితే_ముగింపు /* HAVE_POLL */

  తిరిగి r;
}

శూన్య Curl_pollfds_init(నిర్మాణం curl_pollfds *cpfds,
                       నిర్మాణం pollfd *static_pfds,
                       గుర్తులేని పూర్ణాంకం static_లెక్క)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, పరిమాణం(*cpfds));
  ఒకవేళ(static_pfds && static_లెక్క) {
    cpfds->pfds = static_pfds;
    cpfds->లెక్క = static_లెక్క;
  }
}

శూన్య Curl_pollfds_reset(నిర్మాణం curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

శూన్య Curl_pollfds_cleanup(నిర్మాణం curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ఒకవేళ(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->లెక్క = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

స్థిర CURLcode cpfds_increase(నిర్మాణం curl_pollfds *cpfds, గుర్తులేని పూర్ణాంకం inc)
{
  నిర్మాణం pollfd *new_fds;
  గుర్తులేని పూర్ణాంకం new_లెక్క = cpfds->లెక్క + inc;

  new_fds = curlx_calloc(new_లెక్క, పరిమాణం(నిర్మాణం pollfd));
  ఒకవేళ(!new_fds)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  జ్ఞాపకం_నకలు(new_fds, cpfds->pfds, cpfds->లెక్క * పరిమాణం(నిర్మాణం pollfd));
  ఒకవేళ(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->లెక్క = new_లెక్క;
  cpfds->allocated_pfds = TRUE;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cpfds_కూడు_sock(నిర్మాణం curl_pollfds *cpfds,
                               curl_socket_t sock, పొట్టి events, bool fold)
{
  పూర్ణాంకం i;

  ఒకవేళ(fold && cpfds->n <= గరిష్ఠ_పూర్ణాంకం) {
    కొరకు(i = (పూర్ణాంకం)cpfds->n - 1; i >= 0; --i) {
      ఒకవేళ(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        తిరిగి CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ఒకవేళ(cpfds->n >= cpfds->లెక్క && cpfds_increase(cpfds, 100))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  తిరిగి CURLE_OK;
}

CURLcode Curl_pollfds_కూడు_sock(నిర్మాణం curl_pollfds *cpfds,
                               curl_socket_t sock, పొట్టి events)
{
  తిరిగి cpfds_కూడు_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_కూడు_ps(నిర్మాణం curl_pollfds *cpfds,
                             నిర్మాణం easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  కొరకు(i = 0; i < ps->n; i++) {
    పొట్టి events = 0;
    ఒకవేళ(ps->actions[i] & CURL_POLL_లోపల)
      events |= POLLIN;
    ఒకవేళ(ps->actions[i] & CURL_POLL_బహిర్గతం)
      events |= POLLOUT;
    ఒకవేళ(events && cpfds_కూడు_sock(cpfds, ps->sockets[i], events, TRUE))
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLE_OK;
}

శూన్య Curl_waitfds_init(నిర్మాణం Curl_waitfds *cwfds,
                       నిర్మాణం curl_waitfd *static_wfds,
                       గుర్తులేని పూర్ణాంకం static_లెక్క)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_లెక్క);
  memset(cwfds, 0, పరిమాణం(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->లెక్క = static_లెక్క;
}

స్థిర గుర్తులేని పూర్ణాంకం cwfds_కూడు_sock(నిర్మాణం Curl_waitfds *cwfds,
                                   curl_socket_t sock, పొట్టి events)
{
  పూర్ణాంకం i;
  ఒకవేళ(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->లెక్క && !cwfds->n);
    తిరిగి 1;
  }
  ఒకవేళ(cwfds->n <= గరిష్ఠ_పూర్ణాంకం) {
    కొరకు(i = (పూర్ణాంకం)cwfds->n - 1; i >= 0; --i) {
      ఒకవేళ(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        తిరిగి 0;
      }
    }
  }
  /* not folded, add new entry */
  ఒకవేళ(cwfds->n < cwfds->లెక్క) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  తిరిగి 1;
}

గుర్తులేని పూర్ణాంకం Curl_waitfds_కూడు_ps(నిర్మాణం Curl_waitfds *cwfds,
                                 నిర్మాణం easy_pollset *ps)
{
  size_t i;
  గుర్తులేని పూర్ణాంకం need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  కొరకు(i = 0; i < ps->n; i++) {
    పొట్టి events = 0;
    ఒకవేళ(ps->actions[i] & CURL_POLL_లోపల)
      events |= CURL_WAIT_POLLIN;
    ఒకవేళ(ps->actions[i] & CURL_POLL_బహిర్గతం)
      events |= CURL_WAIT_POLLOUT;
    ఒకవేళ(events)
      need += cwfds_కూడు_sock(cwfds, ps->sockets[i], events);
  }
  తిరిగి need;
}

శూన్య Curl_pollset_reset(నిర్మాణం easy_pollset *ps)
{
#నిర్వచితమైతే DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#అయితే_ముగింపు
  ps->n = 0;
}

శూన్య Curl_pollset_init(నిర్మాణం easy_pollset *ps)
{
#నిర్వచితమైతే DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#అయితే_ముగింపు
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->లెక్క = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

నిర్మాణం easy_pollset *Curl_pollset_create(శూన్య)
{
  నిర్మాణం easy_pollset *ps = curlx_calloc(1, పరిమాణం(*ps));
  ఒకవేళ(ps)
    Curl_pollset_init(ps);
  తిరిగి ps;
}

శూన్య Curl_pollset_cleanup(నిర్మాణం easy_pollset *ps)
{
#నిర్వచితమైతే DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#అయితే_ముగింపు
  ఒకవేళ(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ఒకవేళ(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->లెక్క = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

శూన్య Curl_pollset_move(నిర్మాణం easy_pollset *గమ్యం, నిర్మాణం easy_pollset *from)
{
  Curl_pollset_cleanup(గమ్యం); /* deallocate anything in to */
  ఒకవేళ(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    గమ్యం->sockets = from->sockets;
    గమ్యం->actions = from->actions;
    గమ్యం->లెక్క = from->లెక్క;
    గమ్యం->n = from->n;
    Curl_pollset_init(from);
  }
  లేకపోతే {
    DEBUGASSERT(గమ్యం->sockets == గమ్యం->def_sockets);
    DEBUGASSERT(గమ్యం->actions == గమ్యం->def_actions);
    జ్ఞాపకం_నకలు(గమ్యం->sockets, from->sockets, గమ్యం->లెక్క * పరిమాణం(గమ్యం->sockets[0]));
    జ్ఞాపకం_నకలు(గమ్యం->actions, from->actions, గమ్యం->లెక్క * పరిమాణం(గమ్యం->actions[0]));
    గమ్యం->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps, curl_socket_t sock,
                             పూర్ణాంకం కూడు_flags, పూర్ణాంకం remove_flags)
{
  గుర్తులేని పూర్ణాంకం i;

#నిర్వచితమైతే DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#అయితే_ముగింపు

  DEBUGASSERT(VALID_SOCK(sock));
  ఒకవేళ(!VALID_SOCK(sock))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  DEBUGASSERT(కూడు_flags <= (CURL_POLL_లోపల | CURL_POLL_బహిర్గతం));
  DEBUGASSERT(remove_flags <= (CURL_POLL_లోపల | CURL_POLL_బహిర్గతం));
  DEBUGASSERT((కూడు_flags & remove_flags) == 0); /* no overlap */
  కొరకు(i = 0; i < ps->n; ++i) {
    ఒకవేళ(ps->sockets[i] == sock) {
      ps->actions[i] &= (గుర్తులేని అక్షరం)(~remove_flags);
      ps->actions[i] |= (గుర్తులేని అక్షరం)కూడు_flags;
      /* all gone? remove socket */
      ఒకవేళ(!ps->actions[i]) {
        ఒకవేళ((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * పరిమాణం(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * పరిమాణం(ps->actions[0]));
        }
        --ps->n;
      }
      తిరిగి CURLE_OK;
    }
  }
  /* not present */
  ఒకవేళ(కూడు_flags) {
    ఒకవేళ(i >= ps->లెక్క) { /* need to grow */
      గుర్తులేని పూర్ణాంకం new_లెక్క = CURLMAX(ps->లెక్క * 2, 8);
      curl_socket_t *nsockets;
      గుర్తులేని అక్షరం *nactions;

      CURL_TRC_M(దత్తాంశం, "growing pollset capacity from %u to %u",
                 ps->లెక్క, new_లెక్క);
      ఒకవేళ(new_లెక్క <= ps->లెక్క)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      nsockets = curlx_calloc(new_లెక్క, పరిమాణం(nsockets[0]));
      ఒకవేళ(!nsockets)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      nactions = curlx_calloc(new_లెక్క, పరిమాణం(nactions[0]));
      ఒకవేళ(!nactions) {
        curlx_free(nsockets);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      జ్ఞాపకం_నకలు(nsockets, ps->sockets, ps->లెక్క * పరిమాణం(ps->sockets[0]));
      జ్ఞాపకం_నకలు(nactions, ps->actions, ps->లెక్క * పరిమాణం(ps->actions[0]));
      ఒకవేళ(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ఒకవేళ(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->లెక్క = new_లెక్క;
    }
    DEBUGASSERT(i < ps->లెక్క);
    ఒకవేళ(i < ps->లెక్క) {
      ps->sockets[i] = sock;
      ps->actions[i] = (గుర్తులేని అక్షరం)కూడు_flags;
      ps->n = i + 1;
    }
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_pollset_సమితి(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం easy_pollset *ps, curl_socket_t sock,
                          bool do_లోపల, bool do_బహిర్గతం)
{
  తిరిగి Curl_pollset_change(దత్తాంశం, ps, sock,
                             (do_లోపల ? CURL_POLL_లోపల : 0) |
                             (do_బహిర్గతం ? CURL_POLL_బహిర్గతం : 0),
                             (!do_లోపల ? CURL_POLL_లోపల : 0) |
                             (!do_బహిర్గతం ? CURL_POLL_బహిర్గతం : 0));
}

శూన్య Curl_pollset_remove(నిర్మాణం easy_pollset *ps, curl_socket_t sock)
{
  గుర్తులేని పూర్ణాంకం i;
  కొరకు(i = 0; i < ps->n; ++i) {
    ఒకవేళ(ps->sockets[i] == sock) {
      ఒకవేళ((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * పరిమాణం(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * పరిమాణం(ps->actions[0]));
      }
      --ps->n;
      తిరిగి;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
పూర్ణాంకం Curl_pollset_poll(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం easy_pollset *ps,
                      timediff_t timeout_ms)
{
  నిర్మాణం pollfd *pfds;
  గుర్తులేని పూర్ణాంకం i, npfds;
  పూర్ణాంకం rc;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);

  ఒకవేళ(!ps->n)
    తిరిగి curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, పరిమాణం(*pfds));
  ఒకవేళ(!pfds)
    తిరిగి -1;

  npfds = 0;
  కొరకు(i = 0; i < ps->n; ++i) {
    పొట్టి events = 0;
    ఒకవేళ(ps->actions[i] & CURL_POLL_లోపల) {
      events |= POLLIN;
    }
    ఒకవేళ(ps->actions[i] & CURL_POLL_బహిర్గతం) {
      events |= POLLOUT;
    }
    ఒకవేళ(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  తిరిగి rc;
}

శూన్య Curl_pollset_check(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  గుర్తులేని పూర్ణాంకం i;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(VALID_SOCK(sock));
  కొరకు(i = 0; i < ps->n; ++i) {
    ఒకవేళ(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_లోపల);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_బహిర్గతం);
      తిరిగి;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps,
                            curl_socket_t sock)
{
  గుర్తులేని పూర్ణాంకం i;
  (శూన్య)దత్తాంశం;
  కొరకు(i = 0; i < ps->n; ++i) {
    ఒకవేళ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_లోపల))
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

bool Curl_pollset_want_send(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps,
                            curl_socket_t sock)
{
  గుర్తులేని పూర్ణాంకం i;
  (శూన్య)దత్తాంశం;
  కొరకు(i = 0; i < ps->n; ++i) {
    ఒకవేళ((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_బహిర్గతం))
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}
