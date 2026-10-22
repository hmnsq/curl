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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_SYS_SELECT_H
#شامل <sys/select.h>
#وگرنہ_اگر defined(HAVE_UNISTD_H)
#شامل <unistd.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "curl_trc.h"
#شامل "curlx/timediff.h"
#شامل "curlx/wait.h"

#اگر_غیر_تعریف_شدہ HAVE_POLL  /* use select() */
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
ساکن صحیح our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ساخت timeval pending_tv;
  ساخت timeval *ptimeout;

#اگر_تعریف_شدہ USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  اگر((!fds_read || fds_read->fd_گنتی == 0) &&
     (!fds_write || fds_write->fd_گنتی == 0) &&
     (!fds_err || fds_err->fd_گنتی == 0)) {
    /* no sockets, wait */
    واپس curlx_wait_ms(timeout_ms);
  }
#اگر_ختم

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#اگر_تعریف_شدہ USE_WINSOCK
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
  واپس select((صحیح)maxfd + 1,
                fds_read && fds_read->fd_گنتی ? fds_read : تہی,
                fds_write && fds_write->fd_گنتی ? fds_write : تہی,
                fds_err && fds_err->fd_گنتی ? fds_err : تہی, ptimeout);
#ورنہ
  واپس select((صحیح)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#اگر_ختم
}

#اگر_ختم

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
صحیح Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ساخت pollfd pfd[3];
  صحیح عدد;
  صحیح r;

  اگر((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    واپس curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  عدد = 0;
  اگر(readfd0 != CURL_SOCKET_BAD) {
    pfd[عدد].fd = readfd0;
    pfd[عدد].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[عدد].revents = 0;
    عدد++;
  }
  اگر(readfd1 != CURL_SOCKET_BAD) {
    pfd[عدد].fd = readfd1;
    pfd[عدد].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[عدد].revents = 0;
    عدد++;
  }
  اگر(writefd != CURL_SOCKET_BAD) {
    pfd[عدد].fd = writefd;
    pfd[عدد].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[عدد].revents = 0;
    عدد++;
  }

  r = Curl_poll(pfd, (غیرعلامتی صحیح)عدد, timeout_ms);
  اگر(r <= 0)
    واپس r;

  r = 0;
  عدد = 0;
  اگر(readfd0 != CURL_SOCKET_BAD) {
    اگر(pfd[عدد].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_اندر;
    اگر(pfd[عدد].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    عدد++;
  }
  اگر(readfd1 != CURL_SOCKET_BAD) {
    اگر(pfd[عدد].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    اگر(pfd[عدد].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    عدد++;
  }
  اگر(writefd != CURL_SOCKET_BAD) {
    اگر(pfd[عدد].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_برآمد;
    اگر(pfd[عدد].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  واپس r;
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
صحیح Curl_poll(ساخت pollfd ufds[], غیرعلامتی صحیح nfds, timediff_t timeout_ms)
{
#اگر_تعریف_شدہ HAVE_POLL
  صحیح pending_ms;
#ورنہ
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#اگر_ختم
  bool fds_none = TRUE;
  غیرعلامتی صحیح i;
  صحیح r;

  اگر(ufds) {
    برائے(i = 0; i < nfds; i++) {
      اگر(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        توڑو;
      }
    }
  }
  اگر(fds_none) {
    /* no sockets, wait */
    واپس curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#اگر_تعریف_شدہ HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#اگر TIMEDIFF_T_MAX > زیادہ_سے_زیادہ_عدد
  اگر(timeout_ms > زیادہ_سے_زیادہ_عدد)
    timeout_ms = زیادہ_سے_زیادہ_عدد;
#اگر_ختم
  اگر(timeout_ms > 0)
    pending_ms = (صحیح)timeout_ms;
  ورنہ اگر(timeout_ms < 0)
    pending_ms = -1;
  ورنہ
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  اگر(r <= 0) {
    اگر((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    واپس r;
  }

  برائے(i = 0; i < nfds; i++) {
    اگر(ufds[i].fd == CURL_SOCKET_BAD)
      جاری;
    اگر(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    اگر(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#ورنہ /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  برائے(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    اگر(ufds[i].fd == CURL_SOCKET_BAD)
      جاری;
    VERIFY_SOCK(ufds[i].fd);
    اگر(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      اگر(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      اگر(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      اگر(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      اگر(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  اگر(r <= 0) {
    اگر((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    واپس r;
  }

  r = 0;
  برائے(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    اگر(ufds[i].fd == CURL_SOCKET_BAD)
      جاری;
    اگر(FD_ISSET(ufds[i].fd, &fds_read)) {
      اگر(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      اگر(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    اگر(FD_ISSET(ufds[i].fd, &fds_write)) {
      اگر(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      اگر(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    اگر(FD_ISSET(ufds[i].fd, &fds_err)) {
      اگر(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      اگر(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    اگر(ufds[i].revents)
      r++;
  }

#اگر_ختم /* HAVE_POLL */

  واپس r;
}

خالی Curl_pollfds_init(ساخت curl_pollfds *cpfds,
                       ساخت pollfd *static_pfds,
                       غیرعلامتی صحیح static_گنتی)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, حجم(*cpfds));
  اگر(static_pfds && static_گنتی) {
    cpfds->pfds = static_pfds;
    cpfds->گنتی = static_گنتی;
  }
}

خالی Curl_pollfds_reset(ساخت curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

خالی Curl_pollfds_cleanup(ساخت curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  اگر(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->گنتی = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ساکن CURLcode cpfds_increase(ساخت curl_pollfds *cpfds, غیرعلامتی صحیح inc)
{
  ساخت pollfd *new_fds;
  غیرعلامتی صحیح new_گنتی = cpfds->گنتی + inc;

  new_fds = curlx_calloc(new_گنتی, حجم(ساخت pollfd));
  اگر(!new_fds)
    واپس CURLE_برآمد_کا_حافظہ;

  حافظہ_نقل(new_fds, cpfds->pfds, cpfds->گنتی * حجم(ساخت pollfd));
  اگر(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->گنتی = new_گنتی;
  cpfds->allocated_pfds = TRUE;
  واپس CURLE_OK;
}

ساکن CURLcode cpfds_جوڑو_sock(ساخت curl_pollfds *cpfds,
                               curl_socket_t sock, مختصر events, bool fold)
{
  صحیح i;

  اگر(fold && cpfds->n <= زیادہ_سے_زیادہ_عدد) {
    برائے(i = (صحیح)cpfds->n - 1; i >= 0; --i) {
      اگر(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        واپس CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  اگر(cpfds->n >= cpfds->گنتی && cpfds_increase(cpfds, 100))
    واپس CURLE_برآمد_کا_حافظہ;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  واپس CURLE_OK;
}

CURLcode Curl_pollfds_جوڑو_sock(ساخت curl_pollfds *cpfds,
                               curl_socket_t sock, مختصر events)
{
  واپس cpfds_جوڑو_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_جوڑو_ps(ساخت curl_pollfds *cpfds,
                             ساخت easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  برائے(i = 0; i < ps->n; i++) {
    مختصر events = 0;
    اگر(ps->actions[i] & CURL_POLL_اندر)
      events |= POLLIN;
    اگر(ps->actions[i] & CURL_POLL_برآمد)
      events |= POLLOUT;
    اگر(events && cpfds_جوڑو_sock(cpfds, ps->sockets[i], events, TRUE))
      واپس CURLE_برآمد_کا_حافظہ;
  }
  واپس CURLE_OK;
}

خالی Curl_waitfds_init(ساخت Curl_waitfds *cwfds,
                       ساخت curl_waitfd *static_wfds,
                       غیرعلامتی صحیح static_گنتی)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_گنتی);
  memset(cwfds, 0, حجم(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->گنتی = static_گنتی;
}

ساکن غیرعلامتی صحیح cwfds_جوڑو_sock(ساخت Curl_waitfds *cwfds,
                                   curl_socket_t sock, مختصر events)
{
  صحیح i;
  اگر(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->گنتی && !cwfds->n);
    واپس 1;
  }
  اگر(cwfds->n <= زیادہ_سے_زیادہ_عدد) {
    برائے(i = (صحیح)cwfds->n - 1; i >= 0; --i) {
      اگر(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        واپس 0;
      }
    }
  }
  /* not folded, add new entry */
  اگر(cwfds->n < cwfds->گنتی) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  واپس 1;
}

غیرعلامتی صحیح Curl_waitfds_جوڑو_ps(ساخت Curl_waitfds *cwfds,
                                 ساخت easy_pollset *ps)
{
  size_t i;
  غیرعلامتی صحیح need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  برائے(i = 0; i < ps->n; i++) {
    مختصر events = 0;
    اگر(ps->actions[i] & CURL_POLL_اندر)
      events |= CURL_WAIT_POLLIN;
    اگر(ps->actions[i] & CURL_POLL_برآمد)
      events |= CURL_WAIT_POLLOUT;
    اگر(events)
      need += cwfds_جوڑو_sock(cwfds, ps->sockets[i], events);
  }
  واپس need;
}

خالی Curl_pollset_reset(ساخت easy_pollset *ps)
{
#اگر_تعریف_شدہ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#اگر_ختم
  ps->n = 0;
}

خالی Curl_pollset_init(ساخت easy_pollset *ps)
{
#اگر_تعریف_شدہ DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#اگر_ختم
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->گنتی = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ساخت easy_pollset *Curl_pollset_create(خالی)
{
  ساخت easy_pollset *ps = curlx_calloc(1, حجم(*ps));
  اگر(ps)
    Curl_pollset_init(ps);
  واپس ps;
}

خالی Curl_pollset_cleanup(ساخت easy_pollset *ps)
{
#اگر_تعریف_شدہ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#اگر_ختم
  اگر(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  اگر(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->گنتی = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

خالی Curl_pollset_move(ساخت easy_pollset *مقصد, ساخت easy_pollset *from)
{
  Curl_pollset_cleanup(مقصد); /* deallocate anything in to */
  اگر(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    مقصد->sockets = from->sockets;
    مقصد->actions = from->actions;
    مقصد->گنتی = from->گنتی;
    مقصد->n = from->n;
    Curl_pollset_init(from);
  }
  ورنہ {
    DEBUGASSERT(مقصد->sockets == مقصد->def_sockets);
    DEBUGASSERT(مقصد->actions == مقصد->def_actions);
    حافظہ_نقل(مقصد->sockets, from->sockets, مقصد->گنتی * حجم(مقصد->sockets[0]));
    حافظہ_نقل(مقصد->actions, from->actions, مقصد->گنتی * حجم(مقصد->actions[0]));
    مقصد->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps, curl_socket_t sock,
                             صحیح جوڑو_flags, صحیح remove_flags)
{
  غیرعلامتی صحیح i;

#اگر_تعریف_شدہ DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#اگر_ختم

  DEBUGASSERT(VALID_SOCK(sock));
  اگر(!VALID_SOCK(sock))
    واپس CURLE_BAD_FUNCTION_دلیل;

  DEBUGASSERT(جوڑو_flags <= (CURL_POLL_اندر | CURL_POLL_برآمد));
  DEBUGASSERT(remove_flags <= (CURL_POLL_اندر | CURL_POLL_برآمد));
  DEBUGASSERT((جوڑو_flags & remove_flags) == 0); /* no overlap */
  برائے(i = 0; i < ps->n; ++i) {
    اگر(ps->sockets[i] == sock) {
      ps->actions[i] &= (غیرعلامتی حرف)(~remove_flags);
      ps->actions[i] |= (غیرعلامتی حرف)جوڑو_flags;
      /* all gone? remove socket */
      اگر(!ps->actions[i]) {
        اگر((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * حجم(ps->actions[0]));
        }
        --ps->n;
      }
      واپس CURLE_OK;
    }
  }
  /* not present */
  اگر(جوڑو_flags) {
    اگر(i >= ps->گنتی) { /* need to grow */
      غیرعلامتی صحیح new_گنتی = CURLMAX(ps->گنتی * 2, 8);
      curl_socket_t *nsockets;
      غیرعلامتی حرف *nactions;

      CURL_TRC_M(ڈیٹا, "growing pollset capacity from %u to %u",
                 ps->گنتی, new_گنتی);
      اگر(new_گنتی <= ps->گنتی)
        واپس CURLE_برآمد_کا_حافظہ;
      nsockets = curlx_calloc(new_گنتی, حجم(nsockets[0]));
      اگر(!nsockets)
        واپس CURLE_برآمد_کا_حافظہ;
      nactions = curlx_calloc(new_گنتی, حجم(nactions[0]));
      اگر(!nactions) {
        curlx_free(nsockets);
        واپس CURLE_برآمد_کا_حافظہ;
      }
      حافظہ_نقل(nsockets, ps->sockets, ps->گنتی * حجم(ps->sockets[0]));
      حافظہ_نقل(nactions, ps->actions, ps->گنتی * حجم(ps->actions[0]));
      اگر(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      اگر(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->گنتی = new_گنتی;
    }
    DEBUGASSERT(i < ps->گنتی);
    اگر(i < ps->گنتی) {
      ps->sockets[i] = sock;
      ps->actions[i] = (غیرعلامتی حرف)جوڑو_flags;
      ps->n = i + 1;
    }
  }
  واپس CURLE_OK;
}

CURLcode Curl_pollset_سیٹ(ساخت Curl_easy *ڈیٹا,
                          ساخت easy_pollset *ps, curl_socket_t sock,
                          bool do_اندر, bool do_برآمد)
{
  واپس Curl_pollset_change(ڈیٹا, ps, sock,
                             (do_اندر ? CURL_POLL_اندر : 0) |
                             (do_برآمد ? CURL_POLL_برآمد : 0),
                             (!do_اندر ? CURL_POLL_اندر : 0) |
                             (!do_برآمد ? CURL_POLL_برآمد : 0));
}

خالی Curl_pollset_remove(ساخت easy_pollset *ps, curl_socket_t sock)
{
  غیرعلامتی صحیح i;
  برائے(i = 0; i < ps->n; ++i) {
    اگر(ps->sockets[i] == sock) {
      اگر((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * حجم(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * حجم(ps->actions[0]));
      }
      --ps->n;
      واپس;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحیح Curl_pollset_poll(ساخت Curl_easy *ڈیٹا,
                      ساخت easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ساخت pollfd *pfds;
  غیرعلامتی صحیح i, npfds;
  صحیح rc;

  (خالی)ڈیٹا;
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);

  اگر(!ps->n)
    واپس curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, حجم(*pfds));
  اگر(!pfds)
    واپس -1;

  npfds = 0;
  برائے(i = 0; i < ps->n; ++i) {
    مختصر events = 0;
    اگر(ps->actions[i] & CURL_POLL_اندر) {
      events |= POLLIN;
    }
    اگر(ps->actions[i] & CURL_POLL_برآمد) {
      events |= POLLOUT;
    }
    اگر(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  واپس rc;
}

خالی Curl_pollset_check(ساخت Curl_easy *ڈیٹا,
                        ساخت easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  غیرعلامتی صحیح i;

  (خالی)ڈیٹا;
  DEBUGASSERT(VALID_SOCK(sock));
  برائے(i = 0; i < ps->n; ++i) {
    اگر(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_اندر);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_برآمد);
      واپس;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ساخت Curl_easy *ڈیٹا,
                            ساخت easy_pollset *ps,
                            curl_socket_t sock)
{
  غیرعلامتی صحیح i;
  (خالی)ڈیٹا;
  برائے(i = 0; i < ps->n; ++i) {
    اگر((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_اندر))
      واپس TRUE;
  }
  واپس FALSE;
}

bool Curl_pollset_want_send(ساخت Curl_easy *ڈیٹا,
                            ساخت easy_pollset *ps,
                            curl_socket_t sock)
{
  غیرعلامتی صحیح i;
  (خالی)ڈیٹا;
  برائے(i = 0; i < ps->n; ++i) {
    اگر((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_برآمد))
      واپس TRUE;
  }
  واپس FALSE;
}
