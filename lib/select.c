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

#ifdef HAVE_SYS_SELECT_H
#شامل <sys/select.h>
#وگرنهاگر defined(HAVE_UNISTD_H)
#شامل <unistd.h>
#endif

#شامل "urldata.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "curl_trc.h"
#شامل "curlx/timediff.h"
#شامل "curlx/wait.h"

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
ایستا صحیح our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ساختار timeval pending_tv;
  ساختار timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  اگر((!fds_read || fds_read->fd_شمار == 0) &&
     (!fds_write || fds_write->fd_شمار == 0) &&
     (!fds_err || fds_err->fd_شمار == 0)) {
    /* no sockets, wait */
    بازگشت curlx_wait_ms(timeout_ms);
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
  بازگشت select((صحیح)maxfd + 1,
                fds_read && fds_read->fd_شمار ? fds_read : پوچ,
                fds_write && fds_write->fd_شمار ? fds_write : پوچ,
                fds_err && fds_err->fd_شمار ? fds_err : پوچ, ptimeout);
#وگرنه
  بازگشت select((صحیح)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
صحیح Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  ساختار pollfd pfd[3];
  صحیح رقم;
  صحیح r;

  اگر((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    بازگشت curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  رقم = 0;
  اگر(readfd0 != CURL_SOCKET_BAD) {
    pfd[رقم].fd = readfd0;
    pfd[رقم].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[رقم].revents = 0;
    رقم++;
  }
  اگر(readfd1 != CURL_SOCKET_BAD) {
    pfd[رقم].fd = readfd1;
    pfd[رقم].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[رقم].revents = 0;
    رقم++;
  }
  اگر(writefd != CURL_SOCKET_BAD) {
    pfd[رقم].fd = writefd;
    pfd[رقم].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[رقم].revents = 0;
    رقم++;
  }

  r = Curl_poll(pfd, (unsigned صحیح)رقم, timeout_ms);
  اگر(r <= 0)
    بازگشت r;

  r = 0;
  رقم = 0;
  اگر(readfd0 != CURL_SOCKET_BAD) {
    اگر(pfd[رقم].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    اگر(pfd[رقم].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    رقم++;
  }
  اگر(readfd1 != CURL_SOCKET_BAD) {
    اگر(pfd[رقم].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    اگر(pfd[رقم].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    رقم++;
  }
  اگر(writefd != CURL_SOCKET_BAD) {
    اگر(pfd[رقم].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    اگر(pfd[رقم].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  بازگشت r;
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
صحیح Curl_poll(ساختار pollfd ufds[], unsigned صحیح nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  صحیح pending_ms;
#وگرنه
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned صحیح i;
  صحیح r;

  اگر(ufds) {
    برای(i = 0; i < nfds; i++) {
      اگر(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        بشکن;
      }
    }
  }
  اگر(fds_none) {
    /* no sockets, wait */
    بازگشت curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#اگر TIMEDIFF_T_MAX > INT_MAX
  اگر(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  اگر(timeout_ms > 0)
    pending_ms = (صحیح)timeout_ms;
  وگرنه اگر(timeout_ms < 0)
    pending_ms = -1;
  وگرنه
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  اگر(r <= 0) {
    اگر((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    بازگشت r;
  }

  برای(i = 0; i < nfds; i++) {
    اگر(ufds[i].fd == CURL_SOCKET_BAD)
      ادامه;
    اگر(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    اگر(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#وگرنه /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  برای(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    اگر(ufds[i].fd == CURL_SOCKET_BAD)
      ادامه;
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
    بازگشت r;
  }

  r = 0;
  برای(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    اگر(ufds[i].fd == CURL_SOCKET_BAD)
      ادامه;
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

#endif /* HAVE_POLL */

  بازگشت r;
}

تهی Curl_pollfds_init(ساختار curl_pollfds *cpfds,
                       ساختار pollfd *static_pfds,
                       unsigned صحیح static_شمار)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, اندازه(*cpfds));
  اگر(static_pfds && static_شمار) {
    cpfds->pfds = static_pfds;
    cpfds->شمار = static_شمار;
  }
}

تهی Curl_pollfds_reset(ساختار curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

تهی Curl_pollfds_cleanup(ساختار curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  اگر(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->شمار = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ایستا CURLcode cpfds_increase(ساختار curl_pollfds *cpfds, unsigned صحیح inc)
{
  ساختار pollfd *new_fds;
  unsigned صحیح new_شمار = cpfds->شمار + inc;

  new_fds = curlx_calloc(new_شمار, اندازه(ساختار pollfd));
  اگر(!new_fds)
    بازگشت CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->شمار * اندازه(ساختار pollfd));
  اگر(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->شمار = new_شمار;
  cpfds->allocated_pfds = TRUE;
  بازگشت CURLE_OK;
}

ایستا CURLcode cpfds_add_sock(ساختار curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  صحیح i;

  اگر(fold && cpfds->n <= INT_MAX) {
    برای(i = (صحیح)cpfds->n - 1; i >= 0; --i) {
      اگر(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        بازگشت CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  اگر(cpfds->n >= cpfds->شمار && cpfds_increase(cpfds, 100))
    بازگشت CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  بازگشت CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(ساختار curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  بازگشت cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(ساختار curl_pollfds *cpfds,
                             ساختار easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  برای(i = 0; i < ps->n; i++) {
    short events = 0;
    اگر(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    اگر(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    اگر(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  بازگشت CURLE_OK;
}

تهی Curl_waitfds_init(ساختار Curl_waitfds *cwfds,
                       ساختار curl_waitfd *static_wfds,
                       unsigned صحیح static_شمار)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_شمار);
  memset(cwfds, 0, اندازه(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->شمار = static_شمار;
}

ایستا unsigned صحیح cwfds_add_sock(ساختار Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  صحیح i;
  اگر(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->شمار && !cwfds->n);
    بازگشت 1;
  }
  اگر(cwfds->n <= INT_MAX) {
    برای(i = (صحیح)cwfds->n - 1; i >= 0; --i) {
      اگر(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        بازگشت 0;
      }
    }
  }
  /* not folded, add new entry */
  اگر(cwfds->n < cwfds->شمار) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  بازگشت 1;
}

unsigned صحیح Curl_waitfds_add_ps(ساختار Curl_waitfds *cwfds,
                                 ساختار easy_pollset *ps)
{
  size_t i;
  unsigned صحیح need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  برای(i = 0; i < ps->n; i++) {
    short events = 0;
    اگر(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    اگر(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    اگر(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  بازگشت need;
}

تهی Curl_pollset_reset(ساختار easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

تهی Curl_pollset_init(ساختار easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->شمار = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

ساختار easy_pollset *Curl_pollset_create(تهی)
{
  ساختار easy_pollset *ps = curlx_calloc(1, اندازه(*ps));
  اگر(ps)
    Curl_pollset_init(ps);
  بازگشت ps;
}

تهی Curl_pollset_cleanup(ساختار easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  اگر(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  اگر(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->شمار = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

تهی Curl_pollset_move(ساختار easy_pollset *to, ساختار easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  اگر(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->شمار = from->شمار;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  وگرنه {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->شمار * اندازه(to->sockets[0]));
    memcpy(to->actions, from->actions, to->شمار * اندازه(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps, curl_socket_t sock,
                             صحیح add_flags, صحیح remove_flags)
{
  unsigned صحیح i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  اگر(!VALID_SOCK(sock))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  برای(i = 0; i < ps->n; ++i) {
    اگر(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned نویسه)(~remove_flags);
      ps->actions[i] |= (unsigned نویسه)add_flags;
      /* all gone? remove socket */
      اگر(!ps->actions[i]) {
        اگر((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * اندازه(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * اندازه(ps->actions[0]));
        }
        --ps->n;
      }
      بازگشت CURLE_OK;
    }
  }
  /* not present */
  اگر(add_flags) {
    اگر(i >= ps->شمار) { /* need to grow */
      unsigned صحیح new_شمار = CURLMAX(ps->شمار * 2, 8);
      curl_socket_t *nsockets;
      unsigned نویسه *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->شمار, new_شمار);
      اگر(new_شمار <= ps->شمار)
        بازگشت CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_شمار, اندازه(nsockets[0]));
      اگر(!nsockets)
        بازگشت CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_شمار, اندازه(nactions[0]));
      اگر(!nactions) {
        curlx_free(nsockets);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->شمار * اندازه(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->شمار * اندازه(ps->actions[0]));
      اگر(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      اگر(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->شمار = new_شمار;
    }
    DEBUGASSERT(i < ps->شمار);
    اگر(i < ps->شمار) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned نویسه)add_flags;
      ps->n = i + 1;
    }
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_pollset_set(ساختار Curl_easy *data,
                          ساختار easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  بازگشت Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

تهی Curl_pollset_remove(ساختار easy_pollset *ps, curl_socket_t sock)
{
  unsigned صحیح i;
  برای(i = 0; i < ps->n; ++i) {
    اگر(ps->sockets[i] == sock) {
      اگر((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * اندازه(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * اندازه(ps->actions[0]));
      }
      --ps->n;
      بازگشت;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحیح Curl_pollset_poll(ساختار Curl_easy *data,
                      ساختار easy_pollset *ps,
                      timediff_t timeout_ms)
{
  ساختار pollfd *pfds;
  unsigned صحیح i, npfds;
  صحیح rc;

  (تهی)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  اگر(!ps->n)
    بازگشت curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, اندازه(*pfds));
  اگر(!pfds)
    بازگشت -1;

  npfds = 0;
  برای(i = 0; i < ps->n; ++i) {
    short events = 0;
    اگر(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    اگر(ps->actions[i] & CURL_POLL_OUT) {
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
  بازگشت rc;
}

تهی Curl_pollset_check(ساختار Curl_easy *data,
                        ساختار easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned صحیح i;

  (تهی)data;
  DEBUGASSERT(VALID_SOCK(sock));
  برای(i = 0; i < ps->n; ++i) {
    اگر(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      بازگشت;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(ساختار Curl_easy *data,
                            ساختار easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned صحیح i;
  (تهی)data;
  برای(i = 0; i < ps->n; ++i) {
    اگر((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      بازگشت TRUE;
  }
  بازگشت FALSE;
}

bool Curl_pollset_want_send(ساختار Curl_easy *data,
                            ساختار easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned صحیح i;
  (تهی)data;
  برای(i = 0; i < ps->n; ++i) {
    اگر((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      بازگشت TRUE;
  }
  بازگشت FALSE;
}
