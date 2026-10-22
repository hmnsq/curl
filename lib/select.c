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
static ကိန်းပြည့် our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  sa_nracanaa timeval pending_tv;
  sa_nracanaa timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  အကယ်၍((!fds_read || fds_read->fd_count == 0) &&
     (!fds_write || fds_write->fd_count == 0) &&
     (!fds_err || fds_err->fd_count == 0)) {
    /* no sockets, wait */
    ပြန်ပေး curlx_wait_ms(timeout_ms);
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
  ပြန်ပေး select((ကိန်းပြည့်)maxfd + 1,
                fds_read && fds_read->fd_count ? fds_read : NULL,
                fds_write && fds_write->fd_count ? fds_write : NULL,
                fds_err && fds_err->fd_count ? fds_err : NULL, ptimeout);
#မဟုတ်ပါက
  ပြန်ပေး select((ကိန်းပြည့်)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
ကိန်းပြည့် Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  sa_nracanaa pollfd pfd[3];
  ကိန်းပြည့် num;
  ကိန်းပြည့် r;

  အကယ်၍((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    ပြန်ပေး curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  num = 0;
  အကယ်၍(readfd0 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd0;
    pfd[num].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  အကယ်၍(readfd1 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd1;
    pfd[num].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  အကယ်၍(writefd != CURL_SOCKET_BAD) {
    pfd[num].fd = writefd;
    pfd[num].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[num].revents = 0;
    num++;
  }

  r = Curl_poll(pfd, (unsigned ကိန်းပြည့်)num, timeout_ms);
  အကယ်၍(r <= 0)
    ပြန်ပေး r;

  r = 0;
  num = 0;
  အကယ်၍(readfd0 != CURL_SOCKET_BAD) {
    အကယ်၍(pfd[num].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    အကယ်၍(pfd[num].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    num++;
  }
  အကယ်၍(readfd1 != CURL_SOCKET_BAD) {
    အကယ်၍(pfd[num].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    အကယ်၍(pfd[num].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    num++;
  }
  အကယ်၍(writefd != CURL_SOCKET_BAD) {
    အကယ်၍(pfd[num].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    အကယ်၍(pfd[num].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  ပြန်ပေး r;
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
ကိန်းပြည့် Curl_poll(sa_nracanaa pollfd ufds[], unsigned ကိန်းပြည့် nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  ကိန်းပြည့် pending_ms;
#မဟုတ်ပါက
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned ကိန်းပြည့် i;
  ကိန်းပြည့် r;

  အကယ်၍(ufds) {
    အတွက်(i = 0; i < nfds; i++) {
      အကယ်၍(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        bha_nga;
      }
    }
  }
  အကယ်၍(fds_none) {
    /* no sockets, wait */
    ပြန်ပေး curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#အကယ်၍ TIMEDIFF_T_MAX > INT_MAX
  အကယ်၍(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  အကယ်၍(timeout_ms > 0)
    pending_ms = (ကိန်းပြည့်)timeout_ms;
  မဟုတ်ပါက အကယ်၍(timeout_ms < 0)
    pending_ms = -1;
  မဟုတ်ပါက
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  အကယ်၍(r <= 0) {
    အကယ်၍((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ပြန်ပေး r;
  }

  အတွက်(i = 0; i < nfds; i++) {
    အကယ်၍(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    အကယ်၍(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    အကယ်၍(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#မဟုတ်ပါက /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  အတွက်(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    အကယ်၍(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    VERIFY_SOCK(ufds[i].fd);
    အကယ်၍(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      အကယ်၍(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      အကယ်၍(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      အကယ်၍(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      အကယ်၍(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  အကယ်၍(r <= 0) {
    အကယ်၍((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ပြန်ပေး r;
  }

  r = 0;
  အတွက်(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    အကယ်၍(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    အကယ်၍(FD_ISSET(ufds[i].fd, &fds_read)) {
      အကယ်၍(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      အကယ်၍(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    အကယ်၍(FD_ISSET(ufds[i].fd, &fds_write)) {
      အကယ်၍(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      အကယ်၍(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    အကယ်၍(FD_ISSET(ufds[i].fd, &fds_err)) {
      အကယ်၍(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      အကယ်၍(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    အကယ်၍(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  ပြန်ပေး r;
}

void Curl_pollfds_init(sa_nracanaa curl_pollfds *cpfds,
                       sa_nracanaa pollfd *static_pfds,
                       unsigned ကိန်းပြည့် static_count)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, parimaa_na(*cpfds));
  အကယ်၍(static_pfds && static_count) {
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
  အကယ်၍(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->count = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

static CURLcode cpfds_increase(sa_nracanaa curl_pollfds *cpfds, unsigned ကိန်းပြည့် inc)
{
  sa_nracanaa pollfd *new_fds;
  unsigned ကိန်းပြည့် new_count = cpfds->count + inc;

  new_fds = curlx_calloc(new_count, parimaa_na(sa_nracanaa pollfd));
  အကယ်၍(!new_fds)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->count * parimaa_na(sa_nracanaa pollfd));
  အကယ်၍(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->count = new_count;
  cpfds->allocated_pfds = TRUE;
  ပြန်ပေး CURLE_OK;
}

static CURLcode cpfds_add_sock(sa_nracanaa curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  ကိန်းပြည့် i;

  အကယ်၍(fold && cpfds->n <= INT_MAX) {
    အတွက်(i = (ကိန်းပြည့်)cpfds->n - 1; i >= 0; --i) {
      အကယ်၍(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        ပြန်ပေး CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  အကယ်၍(cpfds->n >= cpfds->count && cpfds_increase(cpfds, 100))
    ပြန်ပေး CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  ပြန်ပေး CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(sa_nracanaa curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  ပြန်ပေး cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(sa_nracanaa curl_pollfds *cpfds,
                             sa_nracanaa easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  အတွက်(i = 0; i < ps->n; i++) {
    short events = 0;
    အကယ်၍(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    အကယ်၍(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    အကယ်၍(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }
  ပြန်ပေး CURLE_OK;
}

void Curl_waitfds_init(sa_nracanaa Curl_waitfds *cwfds,
                       sa_nracanaa curl_waitfd *static_wfds,
                       unsigned ကိန်းပြည့် static_count)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_count);
  memset(cwfds, 0, parimaa_na(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->count = static_count;
}

static unsigned ကိန်းပြည့် cwfds_add_sock(sa_nracanaa Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  ကိန်းပြည့် i;
  အကယ်၍(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->count && !cwfds->n);
    ပြန်ပေး 1;
  }
  အကယ်၍(cwfds->n <= INT_MAX) {
    အတွက်(i = (ကိန်းပြည့်)cwfds->n - 1; i >= 0; --i) {
      အကယ်၍(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        ပြန်ပေး 0;
      }
    }
  }
  /* not folded, add new entry */
  အကယ်၍(cwfds->n < cwfds->count) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  ပြန်ပေး 1;
}

unsigned ကိန်းပြည့် Curl_waitfds_add_ps(sa_nracanaa Curl_waitfds *cwfds,
                                 sa_nracanaa easy_pollset *ps)
{
  size_t i;
  unsigned ကိန်းပြည့် need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  အတွက်(i = 0; i < ps->n; i++) {
    short events = 0;
    အကယ်၍(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    အကယ်၍(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    အကယ်၍(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  ပြန်ပေး need;
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
  အကယ်၍(ps)
    Curl_pollset_init(ps);
  ပြန်ပေး ps;
}

void Curl_pollset_cleanup(sa_nracanaa easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  အကယ်၍(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  အကယ်၍(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->count = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

void Curl_pollset_move(sa_nracanaa easy_pollset *to, sa_nracanaa easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  အကယ်၍(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->count = from->count;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  မဟုတ်ပါက {
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
                             ကိန်းပြည့် add_flags, ကိန်းပြည့် remove_flags)
{
  unsigned ကိန်းပြည့် i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  အကယ်၍(!VALID_SOCK(sock))
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  အတွက်(i = 0; i < ps->n; ++i) {
    အကယ်၍(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned var_na)(~remove_flags);
      ps->actions[i] |= (unsigned var_na)add_flags;
      /* all gone? remove socket */
      အကယ်၍(!ps->actions[i]) {
        အကယ်၍((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * parimaa_na(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * parimaa_na(ps->actions[0]));
        }
        --ps->n;
      }
      ပြန်ပေး CURLE_OK;
    }
  }
  /* not present */
  အကယ်၍(add_flags) {
    အကယ်၍(i >= ps->count) { /* need to grow */
      unsigned ကိန်းပြည့် new_count = CURLMAX(ps->count * 2, 8);
      curl_socket_t *nsockets;
      unsigned var_na *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->count, new_count);
      အကယ်၍(new_count <= ps->count)
        ပြန်ပေး CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_count, parimaa_na(nsockets[0]));
      အကယ်၍(!nsockets)
        ပြန်ပေး CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_count, parimaa_na(nactions[0]));
      အကယ်၍(!nactions) {
        curlx_free(nsockets);
        ပြန်ပေး CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->count * parimaa_na(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->count * parimaa_na(ps->actions[0]));
      အကယ်၍(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      အကယ်၍(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->count = new_count;
    }
    DEBUGASSERT(i < ps->count);
    အကယ်၍(i < ps->count) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned var_na)add_flags;
      ps->n = i + 1;
    }
  }
  ပြန်ပေး CURLE_OK;
}

CURLcode Curl_pollset_set(sa_nracanaa Curl_easy *data,
                          sa_nracanaa easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  ပြန်ပေး Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

void Curl_pollset_remove(sa_nracanaa easy_pollset *ps, curl_socket_t sock)
{
  unsigned ကိန်းပြည့် i;
  အတွက်(i = 0; i < ps->n; ++i) {
    အကယ်၍(ps->sockets[i] == sock) {
      အကယ်၍((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * parimaa_na(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * parimaa_na(ps->actions[0]));
      }
      --ps->n;
      ပြန်ပေး;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ကိန်းပြည့် Curl_pollset_poll(sa_nracanaa Curl_easy *data,
                      sa_nracanaa easy_pollset *ps,
                      timediff_t timeout_ms)
{
  sa_nracanaa pollfd *pfds;
  unsigned ကိန်းပြည့် i, npfds;
  ကိန်းပြည့် rc;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  အကယ်၍(!ps->n)
    ပြန်ပေး curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, parimaa_na(*pfds));
  အကယ်၍(!pfds)
    ပြန်ပေး -1;

  npfds = 0;
  အတွက်(i = 0; i < ps->n; ++i) {
    short events = 0;
    အကယ်၍(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    အကယ်၍(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    အကယ်၍(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  ပြန်ပေး rc;
}

void Curl_pollset_check(sa_nracanaa Curl_easy *data,
                        sa_nracanaa easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned ကိန်းပြည့် i;

  (void)data;
  DEBUGASSERT(VALID_SOCK(sock));
  အတွက်(i = 0; i < ps->n; ++i) {
    အကယ်၍(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      ပြန်ပေး;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(sa_nracanaa Curl_easy *data,
                            sa_nracanaa easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned ကိန်းပြည့် i;
  (void)data;
  အတွက်(i = 0; i < ps->n; ++i) {
    အကယ်၍((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      ပြန်ပေး TRUE;
  }
  ပြန်ပေး FALSE;
}

bool Curl_pollset_want_send(sa_nracanaa Curl_easy *data,
                            sa_nracanaa easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned ကိန်းပြည့် i;
  (void)data;
  အတွက်(i = 0; i < ps->n; ++i) {
    အကယ်၍((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      ပြန်ပေး TRUE;
  }
  ပြန်ပေး FALSE;
}
