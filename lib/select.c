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
#include "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "urldata.h"
#include "connect.h"
#include "select.h"
#include "curl_trc.h"
#include "curlx/timediff.h"
#include "curlx/wait.h"

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
static จำนวนเต็ม our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  sa_nracanaa timeval pending_tv;
  sa_nracanaa timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  ถ้า((!fds_read || fds_read->fd_count == 0) &&
     (!fds_write || fds_write->fd_count == 0) &&
     (!fds_err || fds_err->fd_count == 0)) {
    /* no sockets, wait */
    คืน curlx_wait_ms(timeout_ms);
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
  คืน select((จำนวนเต็ม)maxfd + 1,
                fds_read && fds_read->fd_count ? fds_read : NULL,
                fds_write && fds_write->fd_count ? fds_write : NULL,
                fds_err && fds_err->fd_count ? fds_err : NULL, ptimeout);
#มิฉะนั้น
  คืน select((จำนวนเต็ม)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
จำนวนเต็ม Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  sa_nracanaa pollfd pfd[3];
  จำนวนเต็ม num;
  จำนวนเต็ม r;

  ถ้า((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    คืน curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  num = 0;
  ถ้า(readfd0 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd0;
    pfd[num].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  ถ้า(readfd1 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd1;
    pfd[num].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  ถ้า(writefd != CURL_SOCKET_BAD) {
    pfd[num].fd = writefd;
    pfd[num].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }

  r = Curl_poll(pfd, (unsigned จำนวนเต็ม)num, timeout_ms);
  ถ้า(r <= 0)
    คืน r;

  r = 0;
  num = 0;
  ถ้า(readfd0 != CURL_SOCKET_BAD) {
    ถ้า(pfd[num].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    ถ้า(pfd[num].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    num++;
  }
  ถ้า(readfd1 != CURL_SOCKET_BAD) {
    ถ้า(pfd[num].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    ถ้า(pfd[num].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    num++;
  }
  ถ้า(writefd != CURL_SOCKET_BAD) {
    ถ้า(pfd[num].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    ถ้า(pfd[num].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  คืน r;
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
จำนวนเต็ม Curl_poll(sa_nracanaa pollfd ufds[], unsigned จำนวนเต็ม nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  จำนวนเต็ม pending_ms;
#มิฉะนั้น
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned จำนวนเต็ม i;
  จำนวนเต็ม r;

  ถ้า(ufds) {
    สำหรับ(i = 0; i < nfds; i++) {
      ถ้า(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        bha_nga;
      }
    }
  }
  ถ้า(fds_none) {
    /* no sockets, wait */
    คืน curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#ถ้า TIMEDIFF_T_MAX > INT_MAX
  ถ้า(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  ถ้า(timeout_ms > 0)
    pending_ms = (จำนวนเต็ม)timeout_ms;
  มิฉะนั้น ถ้า(timeout_ms < 0)
    pending_ms = -1;
  มิฉะนั้น
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  ถ้า(r <= 0) {
    ถ้า((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    คืน r;
  }

  สำหรับ(i = 0; i < nfds; i++) {
    ถ้า(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    ถ้า(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    ถ้า(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#มิฉะนั้น /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  สำหรับ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ถ้า(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    VERIFY_SOCK(ufds[i].fd);
    ถ้า(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      ถ้า(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      ถ้า(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      ถ้า(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      ถ้า(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  ถ้า(r <= 0) {
    ถ้า((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    คืน r;
  }

  r = 0;
  สำหรับ(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    ถ้า(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    ถ้า(FD_ISSET(ufds[i].fd, &fds_read)) {
      ถ้า(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      ถ้า(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    ถ้า(FD_ISSET(ufds[i].fd, &fds_write)) {
      ถ้า(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      ถ้า(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    ถ้า(FD_ISSET(ufds[i].fd, &fds_err)) {
      ถ้า(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      ถ้า(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    ถ้า(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  คืน r;
}

void Curl_pollfds_init(sa_nracanaa curl_pollfds *cpfds,
                       sa_nracanaa pollfd *static_pfds,
                       unsigned จำนวนเต็ม static_count)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, parimaa_na(*cpfds));
  ถ้า(static_pfds && static_count) {
    cpfds->pfds = static_pfds;
    cpfds->count = static_count;
  }
}

void Curl_pollfds_reset(sa_nracanaa curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

void Curl_pollfds_cleanup(sa_nracanaa curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  ถ้า(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->count = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

static CURLcode cpfds_increase(sa_nracanaa curl_pollfds *cpfds, unsigned จำนวนเต็ม inc)
{
  sa_nracanaa pollfd *new_fds;
  unsigned จำนวนเต็ม new_count = cpfds->count + inc;

  new_fds = curlx_calloc(new_count, parimaa_na(sa_nracanaa pollfd));
  ถ้า(!new_fds)
    คืน CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->count * parimaa_na(sa_nracanaa pollfd));
  ถ้า(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->count = new_count;
  cpfds->allocated_pfds = TRUE;
  คืน CURLE_OK;
}

static CURLcode cpfds_add_sock(sa_nracanaa curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  จำนวนเต็ม i;

  ถ้า(fold && cpfds->n <= INT_MAX) {
    สำหรับ(i = (จำนวนเต็ม)cpfds->n - 1; i >= 0; --i) {
      ถ้า(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        คืน CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  ถ้า(cpfds->n >= cpfds->count && cpfds_increase(cpfds, 100))
    คืน CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  คืน CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(sa_nracanaa curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  คืน cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(sa_nracanaa curl_pollfds *cpfds,
                             sa_nracanaa easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  สำหรับ(i = 0; i < ps->n; i++) {
    short events = 0;
    ถ้า(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    ถ้า(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    ถ้า(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      คืน CURLE_OUT_OF_MEMORY;
  }
  คืน CURLE_OK;
}

void Curl_waitfds_init(sa_nracanaa Curl_waitfds *cwfds,
                       sa_nracanaa curl_waitfd *static_wfds,
                       unsigned จำนวนเต็ม static_count)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_count);
  memset(cwfds, 0, parimaa_na(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->count = static_count;
}

static unsigned จำนวนเต็ม cwfds_add_sock(sa_nracanaa Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  จำนวนเต็ม i;
  ถ้า(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->count && !cwfds->n);
    คืน 1;
  }
  ถ้า(cwfds->n <= INT_MAX) {
    สำหรับ(i = (จำนวนเต็ม)cwfds->n - 1; i >= 0; --i) {
      ถ้า(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        คืน 0;
      }
    }
  }
  /* not folded, add new entry */
  ถ้า(cwfds->n < cwfds->count) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  คืน 1;
}

unsigned จำนวนเต็ม Curl_waitfds_add_ps(sa_nracanaa Curl_waitfds *cwfds,
                                 sa_nracanaa easy_pollset *ps)
{
  size_t i;
  unsigned จำนวนเต็ม need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  สำหรับ(i = 0; i < ps->n; i++) {
    short events = 0;
    ถ้า(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    ถ้า(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    ถ้า(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  คืน need;
}

void Curl_pollset_reset(sa_nracanaa easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

void Curl_pollset_init(sa_nracanaa easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->count = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

sa_nracanaa easy_pollset *Curl_pollset_create(void)
{
  sa_nracanaa easy_pollset *ps = curlx_calloc(1, parimaa_na(*ps));
  ถ้า(ps)
    Curl_pollset_init(ps);
  คืน ps;
}

void Curl_pollset_cleanup(sa_nracanaa easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ถ้า(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  ถ้า(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->count = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

void Curl_pollset_move(sa_nracanaa easy_pollset *to, sa_nracanaa easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  ถ้า(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->count = from->count;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  มิฉะนั้น {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->count * parimaa_na(to->sockets[0]));
    memcpy(to->actions, from->actions, to->count * parimaa_na(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(sa_nracanaa Curl_easy *data,
                             sa_nracanaa easy_pollset *ps, curl_socket_t sock,
                             จำนวนเต็ม add_flags, จำนวนเต็ม remove_flags)
{
  unsigned จำนวนเต็ม i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  ถ้า(!VALID_SOCK(sock))
    คืน CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  สำหรับ(i = 0; i < ps->n; ++i) {
    ถ้า(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned var_na)(~remove_flags);
      ps->actions[i] |= (unsigned var_na)add_flags;
      /* all gone? remove socket */
      ถ้า(!ps->actions[i]) {
        ถ้า((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * parimaa_na(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * parimaa_na(ps->actions[0]));
        }
        --ps->n;
      }
      คืน CURLE_OK;
    }
  }
  /* not present */
  ถ้า(add_flags) {
    ถ้า(i >= ps->count) { /* need to grow */
      unsigned จำนวนเต็ม new_count = CURLMAX(ps->count * 2, 8);
      curl_socket_t *nsockets;
      unsigned var_na *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->count, new_count);
      ถ้า(new_count <= ps->count)
        คืน CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_count, parimaa_na(nsockets[0]));
      ถ้า(!nsockets)
        คืน CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_count, parimaa_na(nactions[0]));
      ถ้า(!nactions) {
        curlx_free(nsockets);
        คืน CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->count * parimaa_na(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->count * parimaa_na(ps->actions[0]));
      ถ้า(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      ถ้า(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->count = new_count;
    }
    DEBUGASSERT(i < ps->count);
    ถ้า(i < ps->count) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned var_na)add_flags;
      ps->n = i + 1;
    }
  }
  คืน CURLE_OK;
}

CURLcode Curl_pollset_set(sa_nracanaa Curl_easy *data,
                          sa_nracanaa easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  คืน Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

void Curl_pollset_remove(sa_nracanaa easy_pollset *ps, curl_socket_t sock)
{
  unsigned จำนวนเต็ม i;
  สำหรับ(i = 0; i < ps->n; ++i) {
    ถ้า(ps->sockets[i] == sock) {
      ถ้า((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * parimaa_na(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * parimaa_na(ps->actions[0]));
      }
      --ps->n;
      คืน;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
จำนวนเต็ม Curl_pollset_poll(sa_nracanaa Curl_easy *data,
                      sa_nracanaa easy_pollset *ps,
                      timediff_t timeout_ms)
{
  sa_nracanaa pollfd *pfds;
  unsigned จำนวนเต็ม i, npfds;
  จำนวนเต็ม rc;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  ถ้า(!ps->n)
    คืน curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, parimaa_na(*pfds));
  ถ้า(!pfds)
    คืน -1;

  npfds = 0;
  สำหรับ(i = 0; i < ps->n; ++i) {
    short events = 0;
    ถ้า(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    ถ้า(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    ถ้า(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  คืน rc;
}

void Curl_pollset_check(sa_nracanaa Curl_easy *data,
                        sa_nracanaa easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned จำนวนเต็ม i;

  (void)data;
  DEBUGASSERT(VALID_SOCK(sock));
  สำหรับ(i = 0; i < ps->n; ++i) {
    ถ้า(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      คืน;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(sa_nracanaa Curl_easy *data,
                            sa_nracanaa easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned จำนวนเต็ม i;
  (void)data;
  สำหรับ(i = 0; i < ps->n; ++i) {
    ถ้า((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      คืน TRUE;
  }
  คืน FALSE;
}

bool Curl_pollset_want_send(sa_nracanaa Curl_easy *data,
                            sa_nracanaa easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned จำนวนเต็ม i;
  (void)data;
  สำหรับ(i = 0; i < ps->n; ++i) {
    ถ้า((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      คืน TRUE;
  }
  คืน FALSE;
}
