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

#यदि_परिभाषितम् HAVE_SYS_SELECT_H
#अन्तर्भाव <sys/select.h>
#नोचेद्यदि defined(HAVE_UNISTD_H)
#अन्तर्भाव <unistd.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/timediff.h"
#अन्तर्भाव "curlx/wait.h"

#यदि_अपरिभाषितम् HAVE_POLL  /* use select() */
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

#यदि_परिभाषितम् USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  यदि((!fds_read || fds_read->fd_गणनम् == 0) &&
     (!fds_write || fds_write->fd_गणनम् == 0) &&
     (!fds_err || fds_err->fd_गणनम् == 0)) {
    /* no sockets, wait */
    प्रत्यागम curlx_wait_ms(timeout_ms);
  }
#यदि_अन्तः

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#यदि_परिभाषितम् USE_WINSOCK
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
                fds_read && fds_read->fd_गणनम् ? fds_read : रिक्त,
                fds_write && fds_write->fd_गणनम् ? fds_write : रिक्त,
                fds_err && fds_err->fd_गणनम् ? fds_err : रिक्त, ptimeout);
#अन्यथा
  प्रत्यागम select((पूर्णाङ्क)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#यदि_अन्तः
}

#यदि_अन्तः

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

  यदि((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    प्रत्यागम curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  अङ्कः = 0;
  यदि(readfd0 != CURL_SOCKET_BAD) {
    pfd[अङ्कः].fd = readfd0;
    pfd[अङ्कः].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अङ्कः].revents = 0;
    अङ्कः++;
  }
  यदि(readfd1 != CURL_SOCKET_BAD) {
    pfd[अङ्कः].fd = readfd1;
    pfd[अङ्कः].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[अङ्कः].revents = 0;
    अङ्कः++;
  }
  यदि(writefd != CURL_SOCKET_BAD) {
    pfd[अङ्कः].fd = writefd;
    pfd[अङ्कः].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[अङ्कः].revents = 0;
    अङ्कः++;
  }

  r = Curl_poll(pfd, (अचिह्नित पूर्णाङ्क)अङ्कः, timeout_ms);
  यदि(r <= 0)
    प्रत्यागम r;

  r = 0;
  अङ्कः = 0;
  यदि(readfd0 != CURL_SOCKET_BAD) {
    यदि(pfd[अङ्कः].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_अन्तर्;
    यदि(pfd[अङ्कः].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अङ्कः++;
  }
  यदि(readfd1 != CURL_SOCKET_BAD) {
    यदि(pfd[अङ्कः].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    यदि(pfd[अङ्कः].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    अङ्कः++;
  }
  यदि(writefd != CURL_SOCKET_BAD) {
    यदि(pfd[अङ्कः].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_बहिर्गमनम्;
    यदि(pfd[अङ्कः].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
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
पूर्णाङ्क Curl_poll(संरचना pollfd ufds[], अचिह्नित पूर्णाङ्क nfds, timediff_t timeout_ms)
{
#यदि_परिभाषितम् HAVE_POLL
  पूर्णाङ्क pending_ms;
#अन्यथा
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#यदि_अन्तः
  bool fds_none = TRUE;
  अचिह्नित पूर्णाङ्क i;
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

#यदि_परिभाषितम् HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#यदि TIMEDIFF_T_MAX > अधिकतम_पूर्णाङ्कः
  यदि(timeout_ms > अधिकतम_पूर्णाङ्कः)
    timeout_ms = अधिकतम_पूर्णाङ्कः;
#यदि_अन्तः
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

#यदि_अन्तः /* HAVE_POLL */

  प्रत्यागम r;
}

शून्य Curl_pollfds_init(संरचना curl_pollfds *cpfds,
                       संरचना pollfd *static_pfds,
                       अचिह्नित पूर्णाङ्क static_गणनम्)
{
  DEBUGASSERT(cpfds);
  स्मृतिं_पूरय(cpfds, 0, परिमाण(*cpfds));
  यदि(static_pfds && static_गणनम्) {
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
  यदि(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->गणनम् = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

स्थिर CURLcode cpfds_increase(संरचना curl_pollfds *cpfds, अचिह्नित पूर्णाङ्क inc)
{
  संरचना pollfd *new_fds;
  अचिह्नित पूर्णाङ्क new_गणनम् = cpfds->गणनम् + inc;

  new_fds = curlx_calloc(new_गणनम्, परिमाण(संरचना pollfd));
  यदि(!new_fds)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  स्मृतिं_प्रतिलिख(new_fds, cpfds->pfds, cpfds->गणनम् * परिमाण(संरचना pollfd));
  यदि(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->गणनम् = new_गणनम्;
  cpfds->allocated_pfds = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cpfds_सङ्कलय_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, ह्रस्व events, bool fold)
{
  पूर्णाङ्क i;

  यदि(fold && cpfds->n <= अधिकतम_पूर्णाङ्कः) {
    प्रति(i = (पूर्णाङ्क)cpfds->n - 1; i >= 0; --i) {
      यदि(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        प्रत्यागम CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  यदि(cpfds->n >= cpfds->गणनम् && cpfds_increase(cpfds, 100))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_pollfds_सङ्कलय_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, ह्रस्व events)
{
  प्रत्यागम cpfds_सङ्कलय_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_सङ्कलय_ps(संरचना curl_pollfds *cpfds,
                             संरचना easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  प्रति(i = 0; i < ps->n; i++) {
    ह्रस्व events = 0;
    यदि(ps->actions[i] & CURL_POLL_अन्तर्)
      events |= POLLIN;
    यदि(ps->actions[i] & CURL_POLL_बहिर्गमनम्)
      events |= POLLOUT;
    यदि(events && cpfds_सङ्कलय_sock(cpfds, ps->sockets[i], events, TRUE))
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_waitfds_init(संरचना Curl_waitfds *cwfds,
                       संरचना curl_waitfd *static_wfds,
                       अचिह्नित पूर्णाङ्क static_गणनम्)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_गणनम्);
  स्मृतिं_पूरय(cwfds, 0, परिमाण(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->गणनम् = static_गणनम्;
}

स्थिर अचिह्नित पूर्णाङ्क cwfds_सङ्कलय_sock(संरचना Curl_waitfds *cwfds,
                                   curl_socket_t sock, ह्रस्व events)
{
  पूर्णाङ्क i;
  यदि(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->गणनम् && !cwfds->n);
    प्रत्यागम 1;
  }
  यदि(cwfds->n <= अधिकतम_पूर्णाङ्कः) {
    प्रति(i = (पूर्णाङ्क)cwfds->n - 1; i >= 0; --i) {
      यदि(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        प्रत्यागम 0;
      }
    }
  }
  /* not folded, add new entry */
  यदि(cwfds->n < cwfds->गणनम्) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  प्रत्यागम 1;
}

अचिह्नित पूर्णाङ्क Curl_waitfds_सङ्कलय_ps(संरचना Curl_waitfds *cwfds,
                                 संरचना easy_pollset *ps)
{
  size_t i;
  अचिह्नित पूर्णाङ्क need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  प्रति(i = 0; i < ps->n; i++) {
    ह्रस्व events = 0;
    यदि(ps->actions[i] & CURL_POLL_अन्तर्)
      events |= CURL_WAIT_POLLIN;
    यदि(ps->actions[i] & CURL_POLL_बहिर्गमनम्)
      events |= CURL_WAIT_POLLOUT;
    यदि(events)
      need += cwfds_सङ्कलय_sock(cwfds, ps->sockets[i], events);
  }
  प्रत्यागम need;
}

शून्य Curl_pollset_reset(संरचना easy_pollset *ps)
{
#यदि_परिभाषितम् DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#यदि_अन्तः
  ps->n = 0;
}

शून्य Curl_pollset_init(संरचना easy_pollset *ps)
{
#यदि_परिभाषितम् DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#यदि_अन्तः
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->गणनम् = CURL_ARRAYSIZE(ps->def_sockets);
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
#यदि_परिभाषितम् DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#यदि_अन्तः
  यदि(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  यदि(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->गणनम् = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

शून्य Curl_pollset_move(संरचना easy_pollset *गन्तव्यम्, संरचना easy_pollset *from)
{
  Curl_pollset_cleanup(गन्तव्यम्); /* deallocate anything in to */
  यदि(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    गन्तव्यम्->sockets = from->sockets;
    गन्तव्यम्->actions = from->actions;
    गन्तव्यम्->गणनम् = from->गणनम्;
    गन्तव्यम्->n = from->n;
    Curl_pollset_init(from);
  }
  अन्यथा {
    DEBUGASSERT(गन्तव्यम्->sockets == गन्तव्यम्->def_sockets);
    DEBUGASSERT(गन्तव्यम्->actions == गन्तव्यम्->def_actions);
    स्मृतिं_प्रतिलिख(गन्तव्यम्->sockets, from->sockets, गन्तव्यम्->गणनम् * परिमाण(गन्तव्यम्->sockets[0]));
    स्मृतिं_प्रतिलिख(गन्तव्यम्->actions, from->actions, गन्तव्यम्->गणनम् * परिमाण(गन्तव्यम्->actions[0]));
    गन्तव्यम्->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps, curl_socket_t sock,
                             पूर्णाङ्क सङ्कलय_flags, पूर्णाङ्क remove_flags)
{
  अचिह्नित पूर्णाङ्क i;

#यदि_परिभाषितम् DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#यदि_अन्तः

  DEBUGASSERT(VALID_SOCK(sock));
  यदि(!VALID_SOCK(sock))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  DEBUGASSERT(सङ्कलय_flags <= (CURL_POLL_अन्तर् | CURL_POLL_बहिर्गमनम्));
  DEBUGASSERT(remove_flags <= (CURL_POLL_अन्तर् | CURL_POLL_बहिर्गमनम्));
  DEBUGASSERT((सङ्कलय_flags & remove_flags) == 0); /* no overlap */
  प्रति(i = 0; i < ps->n; ++i) {
    यदि(ps->sockets[i] == sock) {
      ps->actions[i] &= (अचिह्नित वर्ण)(~remove_flags);
      ps->actions[i] |= (अचिह्नित वर्ण)सङ्कलय_flags;
      /* all gone? remove socket */
      यदि(!ps->actions[i]) {
        यदि((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * परिमाण(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * परिमाण(ps->actions[0]));
        }
        --ps->n;
      }
      प्रत्यागम CURLE_OK;
    }
  }
  /* not present */
  यदि(सङ्कलय_flags) {
    यदि(i >= ps->गणनम्) { /* need to grow */
      अचिह्नित पूर्णाङ्क new_गणनम् = CURLMAX(ps->गणनम् * 2, 8);
      curl_socket_t *nsockets;
      अचिह्नित वर्ण *nactions;

      CURL_TRC_M(दत्त, "growing pollset capacity from %u to %u",
                 ps->गणनम्, new_गणनम्);
      यदि(new_गणनम् <= ps->गणनम्)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      nsockets = curlx_calloc(new_गणनम्, परिमाण(nsockets[0]));
      यदि(!nsockets)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      nactions = curlx_calloc(new_गणनम्, परिमाण(nactions[0]));
      यदि(!nactions) {
        curlx_free(nsockets);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      स्मृतिं_प्रतिलिख(nsockets, ps->sockets, ps->गणनम् * परिमाण(ps->sockets[0]));
      स्मृतिं_प्रतिलिख(nactions, ps->actions, ps->गणनम् * परिमाण(ps->actions[0]));
      यदि(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      यदि(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->गणनम् = new_गणनम्;
    }
    DEBUGASSERT(i < ps->गणनम्);
    यदि(i < ps->गणनम्) {
      ps->sockets[i] = sock;
      ps->actions[i] = (अचिह्नित वर्ण)सङ्कलय_flags;
      ps->n = i + 1;
    }
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_pollset_समुच्चयः(संरचना Curl_easy *दत्त,
                          संरचना easy_pollset *ps, curl_socket_t sock,
                          bool do_अन्तर्, bool do_बहिर्गमनम्)
{
  प्रत्यागम Curl_pollset_change(दत्त, ps, sock,
                             (do_अन्तर् ? CURL_POLL_अन्तर् : 0) |
                             (do_बहिर्गमनम् ? CURL_POLL_बहिर्गमनम् : 0),
                             (!do_अन्तर् ? CURL_POLL_अन्तर् : 0) |
                             (!do_बहिर्गमनम् ? CURL_POLL_बहिर्गमनम् : 0));
}

शून्य Curl_pollset_remove(संरचना easy_pollset *ps, curl_socket_t sock)
{
  अचिह्नित पूर्णाङ्क i;
  प्रति(i = 0; i < ps->n; ++i) {
    यदि(ps->sockets[i] == sock) {
      यदि((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * परिमाण(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
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
पूर्णाङ्क Curl_pollset_poll(संरचना Curl_easy *दत्त,
                      संरचना easy_pollset *ps,
                      timediff_t timeout_ms)
{
  संरचना pollfd *pfds;
  अचिह्नित पूर्णाङ्क i, npfds;
  पूर्णाङ्क rc;

  (शून्य)दत्त;
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);

  यदि(!ps->n)
    प्रत्यागम curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, परिमाण(*pfds));
  यदि(!pfds)
    प्रत्यागम -1;

  npfds = 0;
  प्रति(i = 0; i < ps->n; ++i) {
    ह्रस्व events = 0;
    यदि(ps->actions[i] & CURL_POLL_अन्तर्) {
      events |= POLLIN;
    }
    यदि(ps->actions[i] & CURL_POLL_बहिर्गमनम्) {
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

शून्य Curl_pollset_check(संरचना Curl_easy *दत्त,
                        संरचना easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  अचिह्नित पूर्णाङ्क i;

  (शून्य)दत्त;
  DEBUGASSERT(VALID_SOCK(sock));
  प्रति(i = 0; i < ps->n; ++i) {
    यदि(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_अन्तर्);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_बहिर्गमनम्);
      प्रत्यागम;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(संरचना Curl_easy *दत्त,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock)
{
  अचिह्नित पूर्णाङ्क i;
  (शून्य)दत्त;
  प्रति(i = 0; i < ps->n; ++i) {
    यदि((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_अन्तर्))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

bool Curl_pollset_want_send(संरचना Curl_easy *दत्त,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock)
{
  अचिह्नित पूर्णाङ्क i;
  (शून्य)दत्त;
  प्रति(i = 0; i < ps->n; ++i) {
    यदि((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_बहिर्गमनम्))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
