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
  यदि((!fds_read || fds_read->fd_गिनती == 0) &&
     (!fds_write || fds_write->fd_गिनती == 0) &&
     (!fds_err || fds_err->fd_गिनती == 0)) {
    /* no sockets, wait */
    प्रत्यागम curlx_wait_ms(timeout_ms);
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
  प्रत्यागम select((पूर्णाङ्क)maxfd + 1,
                fds_read && fds_read->fd_गिनती ? fds_read : रिक्त,
                fds_write && fds_write->fd_गिनती ? fds_write : रिक्त,
                fds_err && fds_err->fd_गिनती ? fds_err : रिक्त, ptimeout);
#अन्यथा
  प्रत्यागम select((पूर्णाङ्क)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
  पूर्णाङ्क अंक;
  पूर्णाङ्क r;

  यदि((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    प्रत्यागम curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  अंक = 0;
  यदि(readfd0 != CURL_SOCKET_BAD) {
    pfd[अंक].fd = readfd0;
    pfd[अंक].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अंक].revents = 0;
    अंक++;
  }
  यदि(readfd1 != CURL_SOCKET_BAD) {
    pfd[अंक].fd = readfd1;
    pfd[अंक].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अंक].revents = 0;
    अंक++;
  }
  यदि(writefd != CURL_SOCKET_BAD) {
    pfd[अंक].fd = writefd;
    pfd[अंक].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[अंक].revents = 0;
    अंक++;
  }

  r = Curl_poll(pfd, (अचिन्हित पूर्णाङ्क)अंक, timeout_ms);
  यदि(r <= 0)
    प्रत्यागम r;

  r = 0;
  अंक = 0;
  यदि(readfd0 != CURL_SOCKET_BAD) {
    यदि(pfd[अंक].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    यदि(pfd[अंक].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अंक++;
  }
  यदि(readfd1 != CURL_SOCKET_BAD) {
    यदि(pfd[अंक].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    यदि(pfd[अंक].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अंक++;
  }
  यदि(writefd != CURL_SOCKET_BAD) {
    यदि(pfd[अंक].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    यदि(pfd[अंक].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  प्रत्यागम r;
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
पूर्णाङ्क Curl_poll(संरचना pollfd ufds[], अचिन्हित पूर्णाङ्क nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  पूर्णाङ्क pending_ms;
#अन्यथा
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  अचिन्हित पूर्णाङ्क i;
  पूर्णाङ्क r;

  यदि(ufds) {
    प्रति(i = 0; i < nfds; i++) {
      यदि(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        भङ्ग;
      }
    }
  }
  यदि(fds_none) {
    /* no sockets, wait */
    प्रत्यागम curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#यदि TIMEDIFF_T_MAX > पूर्णांक_उच्च
  यदि(timeout_ms > पूर्णांक_उच्च)
    timeout_ms = पूर्णांक_उच्च;
#endif
  यदि(timeout_ms > 0)
    pending_ms = (पूर्णाङ्क)timeout_ms;
  अन्यथा यदि(timeout_ms < 0)
    pending_ms = -1;
  अन्यथा
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  यदि(r <= 0) {
    यदि((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    प्रत्यागम r;
  }

  प्रति(i = 0; i < nfds; i++) {
    यदि(ufds[i].fd == CURL_SOCKET_BAD)
      अनुवर्त;
    यदि(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    यदि(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#अन्यथा /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  प्रति(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    यदि(ufds[i].fd == CURL_SOCKET_BAD)
      अनुवर्त;
    VERIFY_SOCK(ufds[i].fd);
    यदि(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      यदि(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      यदि(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      यदि(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      यदि(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  यदि(r <= 0) {
    यदि((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    प्रत्यागम r;
  }

  r = 0;
  प्रति(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    यदि(ufds[i].fd == CURL_SOCKET_BAD)
      अनुवर्त;
    यदि(FD_ISSET(ufds[i].fd, &fds_read)) {
      यदि(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      यदि(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    यदि(FD_ISSET(ufds[i].fd, &fds_write)) {
      यदि(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      यदि(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    यदि(FD_ISSET(ufds[i].fd, &fds_err)) {
      यदि(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      यदि(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    यदि(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  प्रत्यागम r;
}

शून्य Curl_pollfds_init(संरचना curl_pollfds *cpfds,
                       संरचना pollfd *static_pfds,
                       अचिन्हित पूर्णाङ्क static_गिनती)
{
  DEBUGASSERT(cpfds);
  स_रखो(cpfds, 0, परिमाण(*cpfds));
  यदि(static_pfds && static_गिनती) {
    cpfds->pfds = static_pfds;
    cpfds->गिनती = static_गिनती;
  }
}

शून्य Curl_pollfds_reset(संरचना curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

शून्य Curl_pollfds_cleanup(संरचना curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  यदि(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->गिनती = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

स्थिर CURLcode cpfds_increase(संरचना curl_pollfds *cpfds, अचिन्हित पूर्णाङ्क inc)
{
  संरचना pollfd *new_fds;
  अचिन्हित पूर्णाङ्क new_गिनती = cpfds->गिनती + inc;

  new_fds = curlx_calloc(new_गिनती, परिमाण(संरचना pollfd));
  यदि(!new_fds)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  स_नकल(new_fds, cpfds->pfds, cpfds->गिनती * परिमाण(संरचना pollfd));
  यदि(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->गिनती = new_गिनती;
  cpfds->allocated_pfds = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cpfds_add_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, लघु events, bool fold)
{
  पूर्णाङ्क i;

  यदि(fold && cpfds->n <= पूर्णांक_उच्च) {
    प्रति(i = (पूर्णाङ्क)cpfds->n - 1; i >= 0; --i) {
      यदि(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        प्रत्यागम CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  यदि(cpfds->n >= cpfds->गिनती && cpfds_increase(cpfds, 100))
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, लघु events)
{
  प्रत्यागम cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(संरचना curl_pollfds *cpfds,
                             संरचना easy_pollset *ps)
{
  माप_प्रकार i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  प्रति(i = 0; i < ps->n; i++) {
    लघु events = 0;
    यदि(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    यदि(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    यदि(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_waitfds_init(संरचना Curl_waitfds *cwfds,
                       संरचना curl_waitfd *static_wfds,
                       अचिन्हित पूर्णाङ्क static_गिनती)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_गिनती);
  स_रखो(cwfds, 0, परिमाण(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->गिनती = static_गिनती;
}

स्थिर अचिन्हित पूर्णाङ्क cwfds_add_sock(संरचना Curl_waitfds *cwfds,
                                   curl_socket_t sock, लघु events)
{
  पूर्णाङ्क i;
  यदि(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->गिनती && !cwfds->n);
    प्रत्यागम 1;
  }
  यदि(cwfds->n <= पूर्णांक_उच्च) {
    प्रति(i = (पूर्णाङ्क)cwfds->n - 1; i >= 0; --i) {
      यदि(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        प्रत्यागम 0;
      }
    }
  }
  /* not folded, add new entry */
  यदि(cwfds->n < cwfds->गिनती) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  प्रत्यागम 1;
}

अचिन्हित पूर्णाङ्क Curl_waitfds_add_ps(संरचना Curl_waitfds *cwfds,
                                 संरचना easy_pollset *ps)
{
  माप_प्रकार i;
  अचिन्हित पूर्णाङ्क need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  प्रति(i = 0; i < ps->n; i++) {
    लघु events = 0;
    यदि(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    यदि(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    यदि(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  प्रत्यागम need;
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
  ps->गिनती = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

संरचना easy_pollset *Curl_pollset_create(शून्य)
{
  संरचना easy_pollset *ps = curlx_calloc(1, परिमाण(*ps));
  यदि(ps)
    Curl_pollset_init(ps);
  प्रत्यागम ps;
}

शून्य Curl_pollset_cleanup(संरचना easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  यदि(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  यदि(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->गिनती = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

शून्य Curl_pollset_move(संरचना easy_pollset *to, संरचना easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  यदि(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->गिनती = from->गिनती;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  अन्यथा {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    स_नकल(to->sockets, from->sockets, to->गिनती * परिमाण(to->sockets[0]));
    स_नकल(to->actions, from->actions, to->गिनती * परिमाण(to->actions[0]));
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
  अचिन्हित पूर्णाङ्क i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  यदि(!VALID_SOCK(sock))
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  प्रति(i = 0; i < ps->n; ++i) {
    यदि(ps->sockets[i] == sock) {
      ps->actions[i] &= (अचिन्हित वर्ण)(~remove_flags);
      ps->actions[i] |= (अचिन्हित वर्ण)add_flags;
      /* all gone? remove socket */
      यदि(!ps->actions[i]) {
        यदि((i + 1) < ps->n) {
          स_हटाओ(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * परिमाण(ps->sockets[0]));
          स_हटाओ(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * परिमाण(ps->actions[0]));
        }
        --ps->n;
      }
      प्रत्यागम CURLE_OK;
    }
  }
  /* not present */
  यदि(add_flags) {
    यदि(i >= ps->गिनती) { /* need to grow */
      अचिन्हित पूर्णाङ्क new_गिनती = CURLMAX(ps->गिनती * 2, 8);
      curl_socket_t *nsockets;
      अचिन्हित वर्ण *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->गिनती, new_गिनती);
      यदि(new_गिनती <= ps->गिनती)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_गिनती, परिमाण(nsockets[0]));
      यदि(!nsockets)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_गिनती, परिमाण(nactions[0]));
      यदि(!nactions) {
        curlx_free(nsockets);
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
      स_नकल(nsockets, ps->sockets, ps->गिनती * परिमाण(ps->sockets[0]));
      स_नकल(nactions, ps->actions, ps->गिनती * परिमाण(ps->actions[0]));
      यदि(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      यदि(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->गिनती = new_गिनती;
    }
    DEBUGASSERT(i < ps->गिनती);
    यदि(i < ps->गिनती) {
      ps->sockets[i] = sock;
      ps->actions[i] = (अचिन्हित वर्ण)add_flags;
      ps->n = i + 1;
    }
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_pollset_set(संरचना Curl_easy *data,
                          संरचना easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  प्रत्यागम Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

शून्य Curl_pollset_remove(संरचना easy_pollset *ps, curl_socket_t sock)
{
  अचिन्हित पूर्णाङ्क i;
  प्रति(i = 0; i < ps->n; ++i) {
    यदि(ps->sockets[i] == sock) {
      यदि((i + 1) < ps->n) {
        स_हटाओ(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * परिमाण(ps->sockets[0]));
        स_हटाओ(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * परिमाण(ps->actions[0]));
      }
      --ps->n;
      प्रत्यागम;
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
  अचिन्हित पूर्णाङ्क i, npfds;
  पूर्णाङ्क rc;

  (शून्य)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  यदि(!ps->n)
    प्रत्यागम curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, परिमाण(*pfds));
  यदि(!pfds)
    प्रत्यागम -1;

  npfds = 0;
  प्रति(i = 0; i < ps->n; ++i) {
    लघु events = 0;
    यदि(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    यदि(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    यदि(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  प्रत्यागम rc;
}

शून्य Curl_pollset_check(संरचना Curl_easy *data,
                        संरचना easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  अचिन्हित पूर्णाङ्क i;

  (शून्य)data;
  DEBUGASSERT(VALID_SOCK(sock));
  प्रति(i = 0; i < ps->n; ++i) {
    यदि(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      प्रत्यागम;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(संरचना Curl_easy *data,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock)
{
  अचिन्हित पूर्णाङ्क i;
  (शून्य)data;
  प्रति(i = 0; i < ps->n; ++i) {
    यदि((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

bool Curl_pollset_want_send(संरचना Curl_easy *data,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock)
{
  अचिन्हित पूर्णाङ्क i;
  (शून्य)data;
  प्रति(i = 0; i < ps->n; ++i) {
    यदि((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
