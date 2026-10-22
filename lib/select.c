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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#અન્તર્ભાવ <sys/select.h>
#અથવાજો defined(HAVE_UNISTD_H)
#અન્તર્ભાવ <unistd.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "curlx/timediff.h"
#અન્તર્ભાવ "curlx/wait.h"

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
સ્થિર પૂર્ણાંક our_select(curl_socket_t maxfd,   /* highest socket number */
                      fd_set *fds_read,      /* sockets ready for reading */
                      fd_set *fds_write,     /* sockets ready for writing */
                      fd_set *fds_err,       /* sockets with errors */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  સંરચના timeval pending_tv;
  સંરચના timeval *ptimeout;

#ifdef USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  જો((!fds_read || fds_read->fd_ગણતરી == 0) &&
     (!fds_write || fds_write->fd_ગણતરી == 0) &&
     (!fds_err || fds_err->fd_ગણતરી == 0)) {
    /* no sockets, wait */
    પાછા curlx_wait_ms(timeout_ms);
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
  પાછા select((પૂર્ણાંક)maxfd + 1,
                fds_read && fds_read->fd_ગણતરી ? fds_read : ખાલી,
                fds_write && fds_write->fd_ગણતરી ? fds_write : ખાલી,
                fds_err && fds_err->fd_ગણતરી ? fds_err : ખાલી, ptimeout);
#નહીંતર
  પાછા select((પૂર્ણાંક)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
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
પૂર્ણાંક Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
  સંરચના pollfd pfd[3];
  પૂર્ણાંક અંક;
  પૂર્ણાંક r;

  જો((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, wait */
    પાછા curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  અંક = 0;
  જો(readfd0 != CURL_SOCKET_BAD) {
    pfd[અંક].fd = readfd0;
    pfd[અંક].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[અંક].revents = 0;
    અંક++;
  }
  જો(readfd1 != CURL_SOCKET_BAD) {
    pfd[અંક].fd = readfd1;
    pfd[અંક].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[અંક].revents = 0;
    અંક++;
  }
  જો(writefd != CURL_SOCKET_BAD) {
    pfd[અંક].fd = writefd;
    pfd[અંક].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[અંક].revents = 0;
    અંક++;
  }

  r = Curl_poll(pfd, (unsigned પૂર્ણાંક)અંક, timeout_ms);
  જો(r <= 0)
    પાછા r;

  r = 0;
  અંક = 0;
  જો(readfd0 != CURL_SOCKET_BAD) {
    જો(pfd[અંક].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN;
    જો(pfd[અંક].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    અંક++;
  }
  જો(readfd1 != CURL_SOCKET_BAD) {
    જો(pfd[અંક].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    જો(pfd[અંક].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    અંક++;
  }
  જો(writefd != CURL_SOCKET_BAD) {
    જો(pfd[અંક].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_OUT;
    જો(pfd[અંક].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
  }

  પાછા r;
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
પૂર્ણાંક Curl_poll(સંરચના pollfd ufds[], unsigned પૂર્ણાંક nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL
  પૂર્ણાંક pending_ms;
#નહીંતર
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#endif
  bool fds_none = TRUE;
  unsigned પૂર્ણાંક i;
  પૂર્ણાંક r;

  જો(ufds) {
    માટે(i = 0; i < nfds; i++) {
      જો(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        તોડો;
      }
    }
  }
  જો(fds_none) {
    /* no sockets, wait */
    પાછા curlx_wait_ms(timeout_ms);
  }

  /* Avoid initial timestamp, avoid curlx_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#જો TIMEDIFF_T_MAX > INT_MAX
  જો(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  જો(timeout_ms > 0)
    pending_ms = (પૂર્ણાંક)timeout_ms;
  નહીંતર જો(timeout_ms < 0)
    pending_ms = -1;
  નહીંતર
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);
  જો(r <= 0) {
    જો((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    પાછા r;
  }

  માટે(i = 0; i < nfds; i++) {
    જો(ufds[i].fd == CURL_SOCKET_BAD)
      ચાલુ;
    જો(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    જો(ufds[i].revents & POLLERR)
      ufds[i].revents |= POLLIN | POLLOUT;
  }

#નહીંતર /* !HAVE_POLL */

  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  માટે(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    જો(ufds[i].fd == CURL_SOCKET_BAD)
      ચાલુ;
    VERIFY_SOCK(ufds[i].fd);
    જો(ufds[i].events & (POLLIN | POLLOUT | POLLPRI |
                         POLLRDNORM | POLLWRNORM | POLLRDBAND)) {
      જો(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      જો(ufds[i].events & (POLLRDNORM | POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      જો(ufds[i].events & (POLLWRNORM | POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      જો(ufds[i].events & (POLLRDBAND | POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  /* Note also that Winsock ignores the first argument, so we do not worry
     about the fact that maxfd is computed incorrectly with Winsock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value). */
  r = our_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);
  જો(r <= 0) {
    જો((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    પાછા r;
  }

  r = 0;
  માટે(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    જો(ufds[i].fd == CURL_SOCKET_BAD)
      ચાલુ;
    જો(FD_ISSET(ufds[i].fd, &fds_read)) {
      જો(ufds[i].events & POLLRDNORM)
        ufds[i].revents |= POLLRDNORM;
      જો(ufds[i].events & POLLIN)
        ufds[i].revents |= POLLIN;
    }
    જો(FD_ISSET(ufds[i].fd, &fds_write)) {
      જો(ufds[i].events & POLLWRNORM)
        ufds[i].revents |= POLLWRNORM;
      જો(ufds[i].events & POLLOUT)
        ufds[i].revents |= POLLOUT;
    }
    જો(FD_ISSET(ufds[i].fd, &fds_err)) {
      જો(ufds[i].events & POLLRDBAND)
        ufds[i].revents |= POLLRDBAND;
      જો(ufds[i].events & POLLPRI)
        ufds[i].revents |= POLLPRI;
    }
    જો(ufds[i].revents)
      r++;
  }

#endif /* HAVE_POLL */

  પાછા r;
}

શૂન્ય Curl_pollfds_init(સંરચના curl_pollfds *cpfds,
                       સંરચના pollfd *static_pfds,
                       unsigned પૂર્ણાંક static_ગણતરી)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, કદ(*cpfds));
  જો(static_pfds && static_ગણતરી) {
    cpfds->pfds = static_pfds;
    cpfds->ગણતરી = static_ગણતરી;
  }
}

શૂન્ય Curl_pollfds_reset(સંરચના curl_pollfds *cpfds)
{
  cpfds->n = 0;
}

શૂન્ય Curl_pollfds_cleanup(સંરચના curl_pollfds *cpfds)
{
  DEBUGASSERT(cpfds);
  જો(cpfds->allocated_pfds) {
    curlx_safefree(cpfds->pfds);
    cpfds->ગણતરી = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

સ્થિર CURLcode cpfds_increase(સંરચના curl_pollfds *cpfds, unsigned પૂર્ણાંક inc)
{
  સંરચના pollfd *new_fds;
  unsigned પૂર્ણાંક new_ગણતરી = cpfds->ગણતરી + inc;

  new_fds = curlx_calloc(new_ગણતરી, કદ(સંરચના pollfd));
  જો(!new_fds)
    પાછા CURLE_OUT_OF_MEMORY;

  memcpy(new_fds, cpfds->pfds, cpfds->ગણતરી * કદ(સંરચના pollfd));
  જો(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->ગણતરી = new_ગણતરી;
  cpfds->allocated_pfds = TRUE;
  પાછા CURLE_OK;
}

સ્થિર CURLcode cpfds_add_sock(સંરચના curl_pollfds *cpfds,
                               curl_socket_t sock, short events, bool fold)
{
  પૂર્ણાંક i;

  જો(fold && cpfds->n <= INT_MAX) {
    માટે(i = (પૂર્ણાંક)cpfds->n - 1; i >= 0; --i) {
      જો(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        પાછા CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  જો(cpfds->n >= cpfds->ગણતરી && cpfds_increase(cpfds, 100))
    પાછા CURLE_OUT_OF_MEMORY;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  પાછા CURLE_OK;
}

CURLcode Curl_pollfds_add_sock(સંરચના curl_pollfds *cpfds,
                               curl_socket_t sock, short events)
{
  પાછા cpfds_add_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_add_ps(સંરચના curl_pollfds *cpfds,
                             સંરચના easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  માટે(i = 0; i < ps->n; i++) {
    short events = 0;
    જો(ps->actions[i] & CURL_POLL_IN)
      events |= POLLIN;
    જો(ps->actions[i] & CURL_POLL_OUT)
      events |= POLLOUT;
    જો(events && cpfds_add_sock(cpfds, ps->sockets[i], events, TRUE))
      પાછા CURLE_OUT_OF_MEMORY;
  }
  પાછા CURLE_OK;
}

શૂન્ય Curl_waitfds_init(સંરચના Curl_waitfds *cwfds,
                       સંરચના curl_waitfd *static_wfds,
                       unsigned પૂર્ણાંક static_ગણતરી)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_ગણતરી);
  memset(cwfds, 0, કદ(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->ગણતરી = static_ગણતરી;
}

સ્થિર unsigned પૂર્ણાંક cwfds_add_sock(સંરચના Curl_waitfds *cwfds,
                                   curl_socket_t sock, short events)
{
  પૂર્ણાંક i;
  જો(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->ગણતરી && !cwfds->n);
    પાછા 1;
  }
  જો(cwfds->n <= INT_MAX) {
    માટે(i = (પૂર્ણાંક)cwfds->n - 1; i >= 0; --i) {
      જો(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        પાછા 0;
      }
    }
  }
  /* not folded, add new entry */
  જો(cwfds->n < cwfds->ગણતરી) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  પાછા 1;
}

unsigned પૂર્ણાંક Curl_waitfds_add_ps(સંરચના Curl_waitfds *cwfds,
                                 સંરચના easy_pollset *ps)
{
  size_t i;
  unsigned પૂર્ણાંક need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  માટે(i = 0; i < ps->n; i++) {
    short events = 0;
    જો(ps->actions[i] & CURL_POLL_IN)
      events |= CURL_WAIT_POLLIN;
    જો(ps->actions[i] & CURL_POLL_OUT)
      events |= CURL_WAIT_POLLOUT;
    જો(events)
      need += cwfds_add_sock(cwfds, ps->sockets[i], events);
  }
  પાછા need;
}

શૂન્ય Curl_pollset_reset(સંરચના easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  ps->n = 0;
}

શૂન્ય Curl_pollset_init(સંરચના easy_pollset *ps)
{
#ifdef DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#endif
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->ગણતરી = CURL_ARRAYSIZE(ps->def_sockets);
  ps->n = 0;
  Curl_pollset_reset(ps);
}

સંરચના easy_pollset *Curl_pollset_create(શૂન્ય)
{
  સંરચના easy_pollset *ps = curlx_calloc(1, કદ(*ps));
  જો(ps)
    Curl_pollset_init(ps);
  પાછા ps;
}

શૂન્ય Curl_pollset_cleanup(સંરચના easy_pollset *ps)
{
#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif
  જો(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  જો(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->ગણતરી = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

શૂન્ય Curl_pollset_move(સંરચના easy_pollset *to, સંરચના easy_pollset *from)
{
  Curl_pollset_cleanup(to); /* deallocate anything in to */
  જો(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    to->sockets = from->sockets;
    to->actions = from->actions;
    to->ગણતરી = from->ગણતરી;
    to->n = from->n;
    Curl_pollset_init(from);
  }
  નહીંતર {
    DEBUGASSERT(to->sockets == to->def_sockets);
    DEBUGASSERT(to->actions == to->def_actions);
    memcpy(to->sockets, from->sockets, to->ગણતરી * કદ(to->sockets[0]));
    memcpy(to->actions, from->actions, to->ગણતરી * કદ(to->actions[0]));
    to->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps, curl_socket_t sock,
                             પૂર્ણાંક add_flags, પૂર્ણાંક remove_flags)
{
  unsigned પૂર્ણાંક i;

#ifdef DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#endif

  DEBUGASSERT(VALID_SOCK(sock));
  જો(!VALID_SOCK(sock))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  DEBUGASSERT(add_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT(remove_flags <= (CURL_POLL_IN | CURL_POLL_OUT));
  DEBUGASSERT((add_flags & remove_flags) == 0); /* no overlap */
  માટે(i = 0; i < ps->n; ++i) {
    જો(ps->sockets[i] == sock) {
      ps->actions[i] &= (unsigned અક્ષર)(~remove_flags);
      ps->actions[i] |= (unsigned અક્ષર)add_flags;
      /* all gone? remove socket */
      જો(!ps->actions[i]) {
        જો((i + 1) < ps->n) {
          memmove(&ps->sockets[i], &ps->sockets[i + 1],
                  (ps->n - (i + 1)) * કદ(ps->sockets[0]));
          memmove(&ps->actions[i], &ps->actions[i + 1],
                  (ps->n - (i + 1)) * કદ(ps->actions[0]));
        }
        --ps->n;
      }
      પાછા CURLE_OK;
    }
  }
  /* not present */
  જો(add_flags) {
    જો(i >= ps->ગણતરી) { /* need to grow */
      unsigned પૂર્ણાંક new_ગણતરી = CURLMAX(ps->ગણતરી * 2, 8);
      curl_socket_t *nsockets;
      unsigned અક્ષર *nactions;

      CURL_TRC_M(data, "growing pollset capacity from %u to %u",
                 ps->ગણતરી, new_ગણતરી);
      જો(new_ગણતરી <= ps->ગણતરી)
        પાછા CURLE_OUT_OF_MEMORY;
      nsockets = curlx_calloc(new_ગણતરી, કદ(nsockets[0]));
      જો(!nsockets)
        પાછા CURLE_OUT_OF_MEMORY;
      nactions = curlx_calloc(new_ગણતરી, કદ(nactions[0]));
      જો(!nactions) {
        curlx_free(nsockets);
        પાછા CURLE_OUT_OF_MEMORY;
      }
      memcpy(nsockets, ps->sockets, ps->ગણતરી * કદ(ps->sockets[0]));
      memcpy(nactions, ps->actions, ps->ગણતરી * કદ(ps->actions[0]));
      જો(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      જો(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->ગણતરી = new_ગણતરી;
    }
    DEBUGASSERT(i < ps->ગણતરી);
    જો(i < ps->ગણતરી) {
      ps->sockets[i] = sock;
      ps->actions[i] = (unsigned અક્ષર)add_flags;
      ps->n = i + 1;
    }
  }
  પાછા CURLE_OK;
}

CURLcode Curl_pollset_set(સંરચના Curl_easy *data,
                          સંરચના easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out)
{
  પાછા Curl_pollset_change(data, ps, sock,
                             (do_in ? CURL_POLL_IN : 0) |
                             (do_out ? CURL_POLL_OUT : 0),
                             (!do_in ? CURL_POLL_IN : 0) |
                             (!do_out ? CURL_POLL_OUT : 0));
}

શૂન્ય Curl_pollset_remove(સંરચના easy_pollset *ps, curl_socket_t sock)
{
  unsigned પૂર્ણાંક i;
  માટે(i = 0; i < ps->n; ++i) {
    જો(ps->sockets[i] == sock) {
      જો((i + 1) < ps->n) {
        memmove(&ps->sockets[i], &ps->sockets[i + 1],
                (ps->n - (i + 1)) * કદ(ps->sockets[0]));
        memmove(&ps->actions[i], &ps->actions[i + 1],
                (ps->n - (i + 1)) * કદ(ps->actions[0]));
      }
      --ps->n;
      પાછા;
    }
  }
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
પૂર્ણાંક Curl_pollset_poll(સંરચના Curl_easy *data,
                      સંરચના easy_pollset *ps,
                      timediff_t timeout_ms)
{
  સંરચના pollfd *pfds;
  unsigned પૂર્ણાંક i, npfds;
  પૂર્ણાંક rc;

  (શૂન્ય)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  જો(!ps->n)
    પાછા curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, કદ(*pfds));
  જો(!pfds)
    પાછા -1;

  npfds = 0;
  માટે(i = 0; i < ps->n; ++i) {
    short events = 0;
    જો(ps->actions[i] & CURL_POLL_IN) {
      events |= POLLIN;
    }
    જો(ps->actions[i] & CURL_POLL_OUT) {
      events |= POLLOUT;
    }
    જો(events) {
      pfds[npfds].fd = ps->sockets[i];
      pfds[npfds].events = events;
      ++npfds;
    }
  }

  rc = Curl_poll(pfds, npfds, timeout_ms);
  curlx_free(pfds);
  પાછા rc;
}

શૂન્ય Curl_pollset_check(સંરચના Curl_easy *data,
                        સંરચના easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  unsigned પૂર્ણાંક i;

  (શૂન્ય)data;
  DEBUGASSERT(VALID_SOCK(sock));
  માટે(i = 0; i < ps->n; ++i) {
    જો(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_IN);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_OUT);
      પાછા;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(સંરચના Curl_easy *data,
                            સંરચના easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned પૂર્ણાંક i;
  (શૂન્ય)data;
  માટે(i = 0; i < ps->n; ++i) {
    જો((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_IN))
      પાછા TRUE;
  }
  પાછા FALSE;
}

bool Curl_pollset_want_send(સંરચના Curl_easy *data,
                            સંરચના easy_pollset *ps,
                            curl_socket_t sock)
{
  unsigned પૂર્ણાંક i;
  (શૂન્ય)data;
  માટે(i = 0; i < ps->n; ++i) {
    જો((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_OUT))
      પાછા TRUE;
  }
  પાછા FALSE;
}
