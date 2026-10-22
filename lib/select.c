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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#جيڪڏهن_پورو

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curl_trc.h"
#ضمن "curlx/timediff.h"
#ضمن "curlx/wait.h"

#جيڪڏهن_وصف_نه_ٿيل HAVE_POLL  /* use select() */
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
ساكن صحيح our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  بنية timeval pending_tv;
  بنية timeval *ptimeout;

#جيڪڏهن_وصف_ٿيل USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  إذا((!fds_read || fds_read->fd_ڳڻپ == 0) &&
     (!fds_write || fds_write->fd_ڳڻپ == 0) &&
     (!fds_err || fds_err->fd_ڳڻپ == 0)) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }
#جيڪڏهن_پورو

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#جيڪڏهن_وصف_ٿيل USE_WINSOCK
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
  أرجع select((صحيح)maxfd + 1,
                fds_read && fds_read->fd_ڳڻپ ? fds_read : عدم,
                fds_write && fds_write->fd_ڳڻپ ? fds_write : عدم,
                fds_err && fds_err->fd_ڳڻپ ? fds_err : عدم, ptimeout);
#وإلا
  أرجع select((صحيح)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#جيڪڏهن_پورو
}

#جيڪڏهن_پورو

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
صحيح Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  بنية pollfd pfd[3];
  صحيح انگ;
  صحيح r;

  إذا((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  انگ = 0;
  إذا(readfd0 != CURL_SOCKET_BAD) {
    pfd[انگ].fd = readfd0;
    pfd[انگ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[انگ].revents = 0;
    انگ++;
  }
  إذا(readfd1 != CURL_SOCKET_BAD) {
    pfd[انگ].fd = readfd1;
    pfd[انگ].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[انگ].revents = 0;
    انگ++;
  }
  إذا(writefd != CURL_SOCKET_BAD) {
    pfd[انگ].fd = writefd;
    pfd[انگ].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[انگ].revents = 0;
    انگ++;
  }

  r = Curl_poll(pfd, (بنانشان صحيح)انگ, timeout_ms);
  إذا(r <= 0)
    أرجع r;

  r = 0;
  انگ = 0;
  إذا(readfd0 != CURL_SOCKET_BAD) {
    إذا(pfd[انگ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_اندريون;
    إذا(pfd[انگ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    انگ++;
  }
  إذا(readfd1 != CURL_SOCKET_BAD) {
    إذا(pfd[انگ].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    إذا(pfd[انگ].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    انگ++;
  }
  إذا(writefd != CURL_SOCKET_BAD) {
    إذا(pfd[انگ].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_ٻاهريون;
    إذا(pfd[انگ].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  أرجع r;
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
صحيح Curl_poll(بنية pollfd ufds[], بنانشان صحيح nfds, timediff_t timeout_ms)
{
#جيڪڏهن_وصف_ٿيل HAVE_POLL
  صحيح pending_ms;
#وإلا
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#جيڪڏهن_پورو
  bool fds_none = TRUE;
  بنانشان صحيح i;
  صحيح r;

  إذا(ufds) {
    لكل(i = 0; i < nfds; i++) {
      إذا(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        اكسر;
      }
    }
  }
  إذا(fds_none) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#جيڪڏهن_وصف_ٿيل HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#إذا TIMEDIFF_T_MAX > وڌ_کان_وڌ_عدد
  إذا(timeout_ms > وڌ_کان_وڌ_عدد)
    timeout_ms = وڌ_کان_وڌ_عدد;
#جيڪڏهن_پورو
  إذا(timeout_ms > 0)
    pending_ms = (صحيح)timeout_ms;
  وإلا إذا(timeout_ms < 0)
    pending_ms = -1;
  وإلا
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  إذا(r <= 0) {
    إذا((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    أرجع r;
  }

  لكل(i = 0; i < nfds; i++) {
    إذا(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    إذا(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    إذا(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#وإلا /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  لكل(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    إذا(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    VERIFY_SOCK(ufds[i].fd);
    إذا(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      إذا(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      إذا(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      إذا(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      إذا(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  إذا(r <= 0) {
    إذا((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    أرجع r;
  }

  r = 0;
  لكل(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    إذا(ufds[i].fd == CURL_SOCKET_BAD)
      استمر;
    إذا(FD_ISSET(ufds[i].fd, &fds_read)) {
      إذا(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      إذا(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    إذا(FD_ISSET(ufds[i].fd, &fds_write)) {
      إذا(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      إذا(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    إذا(FD_ISSET(ufds[i].fd, &fds_err)) {
      إذا(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      إذا(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    إذا(ufds[i].revents)
      r++;
  }

#جيڪڏهن_پورو /* HAVE_POLL */

  أرجع r;
}

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       بنانشان صحيح static_ڳڻپ)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, حجم(*cpfds));
  إذا(static_pfds && static_ڳڻپ) {
    cpfds->pfds = static_pfds;
    cpfds->ڳڻپ = static_ڳڻپ;
  }
}

فراغ Curl_pollfds_reset(بنية curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

فراغ Curl_pollfds_cleanup(بنية curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  إذا(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->ڳڻپ = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ساكن CURLcode cpfds_increase(بنية curl_pollfds *cpfds, بنانشان صحيح inc)
{
  بنية pollfd *new_fds;
  بنانشان صحيح new_ڳڻپ = cpfds->ڳڻپ + inc;

  new_fds = curlx_calloc(new_ڳڻپ, حجم(بنية pollfd));
  إذا(!new_fds)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  يادگيري_نقل(new_fds, cpfds->pfds, cpfds->ڳڻپ * حجم(بنية pollfd));
  إذا(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->ڳڻپ = new_ڳڻپ;
  cpfds->allocated_pfds = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cpfds_جوڙيو_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, ننڍو events, bool fold)
{
  صحيح i;

  إذا(fold && cpfds->n <= وڌ_کان_وڌ_عدد) {
    لكل(i = (صحيح)cpfds->n - 1; i >= 0; --i) {
      إذا(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        أرجع CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  إذا(cpfds->n >= cpfds->ڳڻپ && cpfds_increase(cpfds, 100))
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  أرجع CURLE_OK;
}

CURLcode Curl_pollfds_جوڙيو_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, ننڍو events)
{
  أرجع cpfds_جوڙيو_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_جوڙيو_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    ننڍو events = 0;
    إذا(ps->actions[i] & CURL_POLL_اندريون)
      events |= POLLIN;
    إذا(ps->actions[i] & CURL_POLL_ٻاهريون)
      events |= POLLOUT;
    إذا(events && cpfds_جوڙيو_sock(cpfds, ps->sockets[i], events, TRUE))
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  أرجع CURLE_OK;
}

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       بنانشان صحيح static_ڳڻپ)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_ڳڻپ);
  memset(cwfds, 0, حجم(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->ڳڻپ = static_ڳڻپ;
}

ساكن بنانشان صحيح cwfds_جوڙيو_sock(بنية Curl_waitfds *cwfds,
                                   curl_socket_t sock, ننڍو events)
{
  صحيح i;
  إذا(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->ڳڻپ && !cwfds->n);
    أرجع 1;
  }
  إذا(cwfds->n <= وڌ_کان_وڌ_عدد) {
    لكل(i = (صحيح)cwfds->n - 1; i >= 0; --i) {
      إذا(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        أرجع 0;
      }
    }
  }
  /* not folded, add new entry */
  إذا(cwfds->n < cwfds->ڳڻپ) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  أرجع 1;
}

بنانشان صحيح Curl_waitfds_جوڙيو_ps(بنية Curl_waitfds *cwfds,
                                 بنية easy_pollset *ps)
{
  size_t i;
  بنانشان صحيح need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    ننڍو events = 0;
    إذا(ps->actions[i] & CURL_POLL_اندريون)
      events |= CURL_WAIT_POLLIN;
    إذا(ps->actions[i] & CURL_POLL_ٻاهريون)
      events |= CURL_WAIT_POLLOUT;
    إذا(events)
      need += cwfds_جوڙيو_sock(cwfds, ps->sockets[i], events);
  }
  أرجع need;
}

فراغ Curl_pollset_reset(بنية easy_pollset *ps)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#جيڪڏهن_پورو
  ps->n = 0;
}

فراغ Curl_pollset_init(بنية easy_pollset *ps)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#جيڪڏهن_پورو
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->ڳڻپ = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

بنية easy_pollset *Curl_pollset_create(فراغ)
{
  بنية easy_pollset *ps = curlx_calloc(1, حجم(*ps));
  إذا(ps)
    Curl_pollset_init(ps);
  أرجع ps;
}

فراغ Curl_pollset_cleanup(بنية easy_pollset *ps)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#جيڪڏهن_پورو
  إذا(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  إذا(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->ڳڻپ = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

فراغ Curl_pollset_move(بنية easy_pollset *منزل, بنية easy_pollset *from)
{
  Curl_pollset_cleanup(منزل); /* deallocate anything in to */
  إذا(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    منزل->sockets = from->sockets;
    منزل->actions = from->actions;
    منزل->ڳڻپ = from->ڳڻپ;
    منزل->n = from->n;
    Curl_pollset_init(from);
  }
  وإلا {
    DEBUGASSERT(منزل->sockets == منزل->def_sockets);
    DEBUGASSERT(منزل->actions == منزل->def_actions);
    يادگيري_نقل(منزل->sockets, from->sockets, منزل->ڳڻپ * حجم(منزل->sockets[0]));
    يادگيري_نقل(منزل->actions, from->actions, منزل->ڳڻپ * حجم(منزل->actions[0]));
    منزل->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps, curl_socket_t sock,
                             صحيح جوڙيو_flags, صحيح remove_flags)
{
  بنانشان صحيح i;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#جيڪڏهن_پورو

  DEBUGASSERT(VALID_SOCK(sock));
  إذا(!VALID_SOCK(sock))
    أرجع CURLE_BAD_FUNCTION_دليل;

  DEBUGASSERT(جوڙيو_flags <= (CURL_POLL_اندريون | CURL_POLL_ٻاهريون));
  DEBUGASSERT(remove_flags <= (CURL_POLL_اندريون | CURL_POLL_ٻاهريون));
  DEBUGASSERT((جوڙيو_flags & remove_flags) == 0); /* no overlap */
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      ps->actions[i] &= (بنانشان حرف)(~remove_flags);
      ps->actions[i] |= (بنانشان حرف)جوڙيو_flags;
      /* all gone? remove socket */
      إذا(!ps->actions[i]) {
        إذا((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->actions[0]));
        }
        --ps->n;
      }
      أرجع CURLE_OK;
    }
  }
  /* not present */
  إذا(جوڙيو_flags) {
    إذا(i >= ps->ڳڻپ) { /* need to grow */
      بنانشان صحيح new_ڳڻپ = CURLMAX(ps->ڳڻپ * 2, 8);
      curl_socket_t *nsockets;
      بنانشان حرف *nactions;

      CURL_TRC_M(معلومات, "growing pollset capacity from %u to %u",
                 ps->ڳڻپ, new_ڳڻپ);
      إذا(new_ڳڻپ <= ps->ڳڻپ)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      nsockets = curlx_calloc(new_ڳڻپ, حجم(nsockets[0]));
      إذا(!nsockets)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      nactions = curlx_calloc(new_ڳڻپ, حجم(nactions[0]));
      إذا(!nactions) {
        curlx_free(nsockets);
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
      يادگيري_نقل(nsockets, ps->sockets, ps->ڳڻپ * حجم(ps->sockets[0]));
      يادگيري_نقل(nactions, ps->actions, ps->ڳڻپ * حجم(ps->actions[0]));
      إذا(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      إذا(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->ڳڻپ = new_ڳڻپ;
    }
    DEBUGASSERT(i < ps->ڳڻپ);
    إذا(i < ps->ڳڻپ) {
      ps->sockets[i] = sock;
      ps->actions[i] = (بنانشان حرف)جوڙيو_flags;
      ps->n = i + 1;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_pollset_سيٽ(بنية Curl_easy *معلومات,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_اندريون, bool do_ٻاهريون)
{
  أرجع Curl_pollset_change(معلومات, ps, sock,
                             (do_اندريون ? CURL_POLL_اندريون : 0) |
                             (do_ٻاهريون ? CURL_POLL_ٻاهريون : 0),
                             (!do_اندريون ? CURL_POLL_اندريون : 0) |
                             (!do_ٻاهريون ? CURL_POLL_ٻاهريون : 0));
}

فراغ Curl_pollset_remove(بنية easy_pollset *ps, curl_socket_t sock)
{
  بنانشان صحيح i;
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      إذا((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * حجم(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * حجم(ps->actions[0]));
      }
      --ps->n;
      أرجع;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحيح Curl_pollset_poll(بنية Curl_easy *معلومات,
                      بنية easy_pollset *ps,
                      timediff_t timeout_ms)
{
  بنية pollfd *pfds;
  بنانشان صحيح i, npfds;
  صحيح rc;

  (فراغ)معلومات;
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);

  إذا(!ps->n)
    أرجع curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, حجم(*pfds));
  إذا(!pfds)
    أرجع -1;

  npfds = 0;
  لكل(i = 0; i < ps->n; ++i) {
    ننڍو events = 0;
    إذا(ps->actions[i] & CURL_POLL_اندريون) {
      events |= POLLIN;
    }
    إذا(ps->actions[i] & CURL_POLL_ٻاهريون) {
      events |= POLLOUT;
    }
    إذا(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  أرجع rc;
}

فراغ Curl_pollset_check(بنية Curl_easy *معلومات,
                        بنية easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  بنانشان صحيح i;

  (فراغ)معلومات;
  DEBUGASSERT(VALID_SOCK(sock));
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_اندريون);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_ٻاهريون);
      أرجع;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  بنانشان صحيح i;
  (فراغ)معلومات;
  لكل(i = 0; i < ps->n; ++i) {
    إذا((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_اندريون))
      أرجع TRUE;
  }
  أرجع FALSE;
}

bool Curl_pollset_want_send(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  بنانشان صحيح i;
  (فراغ)معلومات;
  لكل(i = 0; i < ps->n; ++i) {
    إذا((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_ٻاهريون))
      أرجع TRUE;
  }
  أرجع FALSE;
}
