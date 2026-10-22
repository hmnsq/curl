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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#अन्तर्भाव <sys/select.h>
#नोचेद्यदि defined(HAVE_UNISTD_H)
#अन्तर्भाव <unistd.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/timediff.h"
#अन्तर्भाव "curlx/wait.h"

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
स्थिर पूर्णाङ्क our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  संरचना timeval pending_tv;
  संरचना timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  सचे((!fds_read || fds_read->fd_गणनम् == 0) &&
     (!fds_write || fds_write->fd_गणनम् == 0) &&
     (!fds_err || fds_err->fd_गणनम् == 0)) {
    /* no sockets, wait */
    पटिनिवत्त curlx_wait_ms(timeout_ms);
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
  पटिनिवत्त select((पूर्णाङ्क)maxfd + 1,
                fds_read && fds_read->fd_गणनम् ? fds_read : रिक्त,
                fds_write && fds_write->fd_गणनम् ? fds_write : रिक्त,
                fds_err && fds_err->fd_गणनम् ? fds_err : रिक्त, ptimeout);
#अञ्ञथा
  पटिनिवत्त select((पूर्णाङ्क)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
पूर्णाङ्क Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  संरचना pollfd pfd[3];
  पूर्णाङ्क अङ्कः;
  पूर्णाङ्क r;

  सचे((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    पटिनिवत्त curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  अङ्कः = 0;
  सचे(readfd0 != CURL_SOCKET_BAD) {
    pfd[अङ्कः].fd = readfd0;
    pfd[अङ्कः].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अङ्कः].revents = 0;
    अङ्कः++;
  }
  सचे(readfd1 != CURL_SOCKET_BAD) {
    pfd[अङ्कः].fd = readfd1;
    pfd[अङ्कः].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अङ्कः].revents = 0;
    अङ्कः++;
  }
  सचे(writefd != CURL_SOCKET_BAD) {
    pfd[अङ्कः].fd = writefd;
    pfd[अङ्कः].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[अङ्कः].revents = 0;
    अङ्कः++;
  }

  r = Curl_poll(pfd, (unsigned पूर्णाङ्क)अङ्कः, timeout_ms);
  सचे(r <= 0)
    पटिनिवत्त r;

  r = 0;
  अङ्कः = 0;
  सचे(readfd0 != CURL_SOCKET_BAD) {
    सचे(pfd[अङ्कः].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    सचे(pfd[अङ्कः].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अङ्कः++;
  }
  सचे(readfd1 != CURL_SOCKET_BAD) {
    सचे(pfd[अङ्कः].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    सचे(pfd[अङ्कः].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अङ्कः++;
  }
  सचे(writefd != CURL_SOCKET_BAD) {
    सचे(pfd[अङ्कः].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    सचे(pfd[अङ्कः].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  पटिनिवत्त r;
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
पूर्णाङ्क Curl_poll(संरचना pollfd ufds[], unsigned पूर्णाङ्क nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  पूर्णाङ्क pending_ms;
#अञ्ञथा
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned पूर्णाङ्क i;
  पूर्णाङ्क r;

  सचे(ufds) {
    अत्थाय(i = 0; i < nfds; i++) {
      सचे(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        भङ्ग;
      }
    }
  }
  सचे(fds_none) {
    /* no sockets, wait */
    पटिनिवत्त curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#सचे TIMEDIFF_T_MAX > INT_MAX
  सचे(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  सचे(timeout_ms > 0)
    pending_ms = (पूर्णाङ्क)timeout_ms;
  अञ्ञथा सचे(timeout_ms < 0)
    pending_ms = -1;
  अञ्ञथा
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  सचे(r <= 0) {
    सचे((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    पटिनिवत्त r;
  }

  अत्थाय(i = 0; i < nfds; i++) {
    सचे(ufds[i].fd == CURL_SOCKET_BAD)
      अनुवर्त;
    सचे(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    सचे(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#अञ्ञथा /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  अत्थाय(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    सचे(ufds[i].fd == CURL_SOCKET_BAD)
      अनुवर्त;
    VERIFY_SOCK(ufds[i].fd);
    सचे(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      सचे(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      सचे(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      सचे(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      सचे(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  सचे(r <= 0) {
    सचे((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    पटिनिवत्त r;
  }

  r = 0;
  अत्थाय(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    सचे(ufds[i].fd == CURL_SOCKET_BAD)
      अनुवर्त;
    सचे(FD_ISSET(ufds[i].fd, &fds_read)) {
      सचे(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      सचे(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    सचे(FD_ISSET(ufds[i].fd, &fds_write)) {
      सचे(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      सचे(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    सचे(FD_ISSET(ufds[i].fd, &fds_err)) {
      सचे(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      सचे(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    सचे(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  पटिनिवत्त r;
}

शून्य Curl_pollfds_init(संरचना curl_pollfds *cpfds,
                       संरचना pollfd *static_pfds,
                       unsigned पूर्णाङ्क static_गणनम्)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, परिमाण(*cpfds));
  सचे(static_pfds && static_गणनम्) {
    cpfds->pfds = static_pfds;
    cpfds->गणनम् = static_गणनम्;
  }
}

शून्य Curl_pollfds_reset(संरचना curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

शून्य Curl_pollfds_cleanup(संरचना curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  सचे(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->गणनम् = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

स्थिर CURLcode cpfds_increase(संरचना curl_pollfds *cpfds, unsigned पूर्णाङ्क inc)
{
  संरचना pollfd *new_fds;
  unsigned पूर्णाङ्क new_गणनम् = cpfds->गणनम् + inc;

  new_fds = curlx_calloc(new_गणनम्, परिमाण(संरचना pollfd));
  सचे(!new_fds)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->गणनम् * परिमाण(संरचना pollfd));
  सचे(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->गणनम् = new_गणनम्;
  cpfds->allocated_pfds = TRUE;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode cpfds_add_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  पूर्णाङ्क i;

  सचे(fold && cpfds->n <= INT_MAX) {
    अत्थाय(i = (पूर्णाङ्क)cpfds->n - 1; i >= 0; --i) {
      सचे(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        पटिनिवत्त CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  सचे(cpfds->n >= cpfds->गणनम् && cpfds_increase(cpfds, 100))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  पटिनिवत्त cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(संरचना curl_pollfds *cpfds,
                             संरचना easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  अत्थाय(i = 0; i < ps->n; i++) {
    short events = 0;
    सचे(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    सचे(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    सचे(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_waitfds_init(संरचना Curl_waitfds *cwfds,
                       संरचना curl_waitfd *static_wfds,
                       unsigned पूर्णाङ्क static_गणनम्)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_गणनम्);
  memset(cwfds, 0, परिमाण(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->गणनम् = static_गणनम्;
}

स्थिर unsigned पूर्णाङ्क cwfds_add_sock(संरचना Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  पूर्णाङ्क i;
  सचे(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->गणनम् && !cwfds->n);
    पटिनिवत्त 1;
  }
  सचे(cwfds->n <= INT_MAX) {
    अत्थाय(i = (पूर्णाङ्क)cwfds->n - 1; i >= 0; --i) {
      सचे(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        पटिनिवत्त 0;
      }
    }
  }
  /* not folded, add new entry */
  सचे(cwfds->n < cwfds->गणनम्) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  पटिनिवत्त 1;
}

unsigned पूर्णाङ्क Curl_waitfds_add_ps(संरचना Curl_waitfds *cwfds,
                                 संरचना easy_pollset *ps)
{
  size_t i;
  unsigned पूर्णाङ्क need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  अत्थाय(i = 0; i < ps->n; i++) {
    short events = 0;
    सचे(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    सचे(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    सचे(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  पटिनिवत्त need;
}

शून्य Curl_pollset_reset(संरचना easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

शून्य Curl_pollset_init(संरचना easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->गणनम् = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

संरचना easy_pollset *Curl_pollset_create(शून्य)
{
  संरचना easy_pollset *ps = curlx_calloc(1, परिमाण(*ps));
  सचे(ps)
    Curl_pollset_init(ps);
  पटिनिवत्त ps;
}

शून्य Curl_pollset_cleanup(संरचना easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  सचे(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  सचे(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->गणनम् = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

शून्य Curl_pollset_move(संरचना easy_pollset *to, संरचना easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  सचे(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->गणनम् = from->गणनम्;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  अञ्ञथा {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->गणनम् * परिमाण(to->sockets[0]));
    memcpy(to->actions, from->actions, to->गणनम् * परिमाण(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps, curl_socket_t sock,
                             पूर्णाङ्क add_flags, पूर्णाङ्क remove_flags)
{
  unsigned पूर्णाङ्क i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  सचे(!VALID_SOCK(sock))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  अत्थाय(i = 0; i < ps->n; ++i) {
    सचे(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned वर्ण)(~remove_flags);
      ps->actions[i] |= (unsigned वर्ण)add_flags;
      /* all gone? remove socket */
      सचे(!ps->actions[i]) {
        सचे((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * परिमाण(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * परिमाण(ps->actions[0]));
        }
        --ps->n;
      }
      पटिनिवत्त CURLE_OK;
    }
  }
  /* not present */
  सचे(add_flags) {
    सचे(i >= ps->गणनम्) { /* need to grow */
      unsigned पूर्णाङ्क new_गणनम् = CURLMAX(ps->गणनम् * 2, 8);
      curl_socket_t *nsockets;
      unsigned वर्ण *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->गणनम्, new_गणनम्);
      सचे(new_गणनम् <= ps->गणनम्)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_गणनम्, परिमाण(nsockets[0]));
      सचे(!nsockets)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_गणनम्, परिमाण(nactions[0]));
      सचे(!nactions) {
        curlx_free(nsockets);
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->गणनम् * परिमाण(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->गणनम् * परिमाण(ps->actions[0]));
      सचे(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      सचे(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->गणनम् = new_गणनम्;
    }
    DEBUGASSERT(i < ps->गणनम्);
    सचे(i < ps->गणनम्) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned वर्ण)add_flags;
      ps->n = i + 1;
    }
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_pollset_set(संरचना Curl_easy *data,
                          संरचना easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  पटिनिवत्त Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

शून्य Curl_pollset_remove(संरचना easy_pollset *ps, curl_socket_t sock)
{
  unsigned पूर्णाङ्क i;
  अत्थाय(i = 0; i < ps->n; ++i) {
    सचे(ps->sockets[i] == sock) {
      सचे((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * परिमाण(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * परिमाण(ps->actions[0]));
      }
      --ps->n;
      पटिनिवत्त;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
पूर्णाङ्क Curl_pollset_poll(संरचना Curl_easy *data,
                      संरचना easy_pollset *ps,
                      timediff_t timeout_ms)
{
  संरचना pollfd *pfds;
  unsigned पूर्णाङ्क i, npfds;
  पूर्णाङ्क rc;

  (शून्य)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  सचे(!ps->n)
    पटिनिवत्त curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, परिमाण(*pfds));
  सचे(!pfds)
    पटिनिवत्त -1;

  npfds = 0;
  अत्थाय(i = 0; i < ps->n; ++i) {
    short events = 0;
    सचे(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    सचे(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    सचे(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  पटिनिवत्त rc;
}

शून्य Curl_pollset_check(संरचना Curl_easy *data,
                        संरचना easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned पूर्णाङ्क i;

  (शून्य)data;
  DEBUGASSERT(VALID_SOCK(sock));
  अत्थाय(i = 0; i < ps->n; ++i) {
    सचे(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      पटिनिवत्त;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(संरचना Curl_easy *data,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned पूर्णाङ्क i;
  (शून्य)data;
  अत्थाय(i = 0; i < ps->n; ++i) {
    सचे((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}

bool Curl_pollset_want_send(संरचना Curl_easy *data,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned पूर्णाङ्क i;
  (शून्य)data;
  अत्थाय(i = 0; i < ps->n; ++i) {
    सचे((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}
