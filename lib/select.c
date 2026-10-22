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

#اگر_تعریف_شده HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#پایان_اگر

#ضمن "urldata.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curl_trc.h"
#ضمن "curlx/timediff.h"
#ضمن "curlx/wait.h"

#اگر_تعریف_نشده HAVE_POLL  /* use select() */
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

#اگر_تعریف_شده USE_WINSOCK
  /* Winsock select() cannot handle zero events. See the comment below. */
  إذا((!fds_read || fds_read->fd_شمار == 0) &&
     (!fds_write || fds_write->fd_شمار == 0) &&
     (!fds_err || fds_err->fd_شمار == 0)) {
    /* no sockets, wait */
    أرجع curlx_wait_ms(timeout_ms);
  }
#پایان_اگر

  ptimeout = curlx_mstotv(&pending_tv, timeout_ms);

#اگر_تعریف_شده USE_WINSOCK
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
                fds_read && fds_read->fd_شمار ? fds_read : عدم,
                fds_write && fds_write->fd_شمار ? fds_write : عدم,
                fds_err && fds_err->fd_شمار ? fds_err : عدم, ptimeout);
#وإلا
  أرجع select((صحيح)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#پایان_اگر
}

#پایان_اگر

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
  صحيح رقم;
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

  رقم = 0;
  إذا(readfd0 != CURL_SOCKET_BAD) {
    pfd[رقم].fd = readfd0;
    pfd[رقم].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[رقم].revents = 0;
    رقم++;
  }
  إذا(readfd1 != CURL_SOCKET_BAD) {
    pfd[رقم].fd = readfd1;
    pfd[رقم].events = POLLRDNORM | POLLIN | POLLRDBAND | POLLPRI;
    pfd[رقم].revents = 0;
    رقم++;
  }
  إذا(writefd != CURL_SOCKET_BAD) {
    pfd[رقم].fd = writefd;
    pfd[رقم].events = POLLWRNORM | POLLOUT | POLLPRI;
    pfd[رقم].revents = 0;
    رقم++;
  }

  r = Curl_poll(pfd, (بیعلامت صحيح)رقم, timeout_ms);
  إذا(r <= 0)
    أرجع r;

  r = 0;
  رقم = 0;
  إذا(readfd0 != CURL_SOCKET_BAD) {
    إذا(pfd[رقم].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_درون;
    إذا(pfd[رقم].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    رقم++;
  }
  إذا(readfd1 != CURL_SOCKET_BAD) {
    إذا(pfd[رقم].revents & (POLLRDNORM | POLLIN | POLLERR | POLLHUP))
      r |= CURL_CSELECT_IN2;
    إذا(pfd[رقم].revents & (POLLPRI | POLLNVAL))
      r |= CURL_CSELECT_ERR;
    رقم++;
  }
  إذا(writefd != CURL_SOCKET_BAD) {
    إذا(pfd[رقم].revents & (POLLWRNORM | POLLOUT))
      r |= CURL_CSELECT_بیرون;
    إذا(pfd[رقم].revents & (POLLERR | POLLHUP | POLLPRI | POLLNVAL))
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
صحيح Curl_poll(بنية pollfd ufds[], بیعلامت صحيح nfds, timediff_t timeout_ms)
{
#اگر_تعریف_شده HAVE_POLL
  صحيح pending_ms;
#وإلا
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  curl_socket_t maxfd;
#پایان_اگر
  bool fds_none = TRUE;
  بیعلامت صحيح i;
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

#اگر_تعریف_شده HAVE_POLL

  /* prevent overflow, timeout_ms is typecast to int. */
#إذا TIMEDIFF_T_MAX > بیشترین_صحیح
  إذا(timeout_ms > بیشترین_صحیح)
    timeout_ms = بیشترین_صحیح;
#پایان_اگر
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

#پایان_اگر /* HAVE_POLL */

  أرجع r;
}

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       بیعلامت صحيح static_شمار)
{
  DEBUGASSERT(cpfds);
  memset(cpfds, 0, حجم(*cpfds));
  إذا(static_pfds && static_شمار) {
    cpfds->pfds = static_pfds;
    cpfds->شمار = static_شمار;
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
    cpfds->شمار = 0;
    cpfds->allocated_pfds = FALSE;
  }
  cpfds->n = 0;
}

ساكن CURLcode cpfds_increase(بنية curl_pollfds *cpfds, بیعلامت صحيح inc)
{
  بنية pollfd *new_fds;
  بیعلامت صحيح new_شمار = cpfds->شمار + inc;

  new_fds = curlx_calloc(new_شمار, حجم(بنية pollfd));
  إذا(!new_fds)
    أرجع CURLE_بیرون_از_حافظه;

  کپی_حافظه(new_fds, cpfds->pfds, cpfds->شمار * حجم(بنية pollfd));
  إذا(cpfds->allocated_pfds)
    curlx_free(cpfds->pfds);
  cpfds->pfds = new_fds;
  cpfds->شمار = new_شمار;
  cpfds->allocated_pfds = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cpfds_جمع_کن_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, کوتاه events, bool fold)
{
  صحيح i;

  إذا(fold && cpfds->n <= بیشترین_صحیح) {
    لكل(i = (صحيح)cpfds->n - 1; i >= 0; --i) {
      إذا(sock == cpfds->pfds[i].fd) {
        cpfds->pfds[i].events |= events;
        أرجع CURLE_OK;
      }
    }
  }
  /* not folded, add new entry */
  إذا(cpfds->n >= cpfds->شمار && cpfds_increase(cpfds, 100))
    أرجع CURLE_بیرون_از_حافظه;
  cpfds->pfds[cpfds->n].fd = sock;
  cpfds->pfds[cpfds->n].events = events;
  ++cpfds->n;
  أرجع CURLE_OK;
}

CURLcode Curl_pollfds_جمع_کن_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, کوتاه events)
{
  أرجع cpfds_جمع_کن_sock(cpfds, sock, events, FALSE);
}

CURLcode Curl_pollfds_جمع_کن_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps)
{
  size_t i;

  DEBUGASSERT(cpfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    کوتاه events = 0;
    إذا(ps->actions[i] & CURL_POLL_درون)
      events |= POLLIN;
    إذا(ps->actions[i] & CURL_POLL_بیرون)
      events |= POLLOUT;
    إذا(events && cpfds_جمع_کن_sock(cpfds, ps->sockets[i], events, TRUE))
      أرجع CURLE_بیرون_از_حافظه;
  }
  أرجع CURLE_OK;
}

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       بیعلامت صحيح static_شمار)
{
  DEBUGASSERT(cwfds);
  DEBUGASSERT(static_wfds || !static_شمار);
  memset(cwfds, 0, حجم(*cwfds));
  cwfds->wfds = static_wfds;
  cwfds->شمار = static_شمار;
}

ساكن بیعلامت صحيح cwfds_جمع_کن_sock(بنية Curl_waitfds *cwfds,
                                   curl_socket_t sock, کوتاه events)
{
  صحيح i;
  إذا(!cwfds->wfds) {
    DEBUGASSERT(!cwfds->شمار && !cwfds->n);
    أرجع 1;
  }
  إذا(cwfds->n <= بیشترین_صحیح) {
    لكل(i = (صحيح)cwfds->n - 1; i >= 0; --i) {
      إذا(sock == cwfds->wfds[i].fd) {
        cwfds->wfds[i].events |= events;
        أرجع 0;
      }
    }
  }
  /* not folded, add new entry */
  إذا(cwfds->n < cwfds->شمار) {
    cwfds->wfds[cwfds->n].fd = sock;
    cwfds->wfds[cwfds->n].events = events;
    ++cwfds->n;
  }
  أرجع 1;
}

بیعلامت صحيح Curl_waitfds_جمع_کن_ps(بنية Curl_waitfds *cwfds,
                                 بنية easy_pollset *ps)
{
  size_t i;
  بیعلامت صحيح need = 0;

  DEBUGASSERT(cwfds);
  DEBUGASSERT(ps);
  لكل(i = 0; i < ps->n; i++) {
    کوتاه events = 0;
    إذا(ps->actions[i] & CURL_POLL_درون)
      events |= CURL_WAIT_POLLIN;
    إذا(ps->actions[i] & CURL_POLL_بیرون)
      events |= CURL_WAIT_POLLOUT;
    إذا(events)
      need += cwfds_جمع_کن_sock(cwfds, ps->sockets[i], events);
  }
  أرجع need;
}

فراغ Curl_pollset_reset(بنية easy_pollset *ps)
{
#اگر_تعریف_شده DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#پایان_اگر
  ps->n = 0;
}

فراغ Curl_pollset_init(بنية easy_pollset *ps)
{
#اگر_تعریف_شده DEBUGBUILD
  ps->init = CURL_EASY_POLLSET_MAGIC;
#پایان_اگر
  ps->sockets = ps->def_sockets;
  ps->actions = ps->def_actions;
  ps->شمار = CURL_ARRAYSIZE(ps->def_sockets);
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
#اگر_تعریف_شده DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#پایان_اگر
  إذا(ps->sockets != ps->def_sockets) {
    curlx_free(ps->sockets);
    ps->sockets = ps->def_sockets;
  }
  إذا(ps->actions != ps->def_actions) {
    curlx_free(ps->actions);
    ps->actions = ps->def_actions;
  }
  ps->شمار = CURL_ARRAYSIZE(ps->def_sockets);
  Curl_pollset_reset(ps);
}

فراغ Curl_pollset_move(بنية easy_pollset *مقصد, بنية easy_pollset *from)
{
  Curl_pollset_cleanup(مقصد); /* deallocate anything in to */
  إذا(from->sockets != from->def_sockets) {
    DEBUGASSERT(from->actions != from->def_actions);
    مقصد->sockets = from->sockets;
    مقصد->actions = from->actions;
    مقصد->شمار = from->شمار;
    مقصد->n = from->n;
    Curl_pollset_init(from);
  }
  وإلا {
    DEBUGASSERT(مقصد->sockets == مقصد->def_sockets);
    DEBUGASSERT(مقصد->actions == مقصد->def_actions);
    کپی_حافظه(مقصد->sockets, from->sockets, مقصد->شمار * حجم(مقصد->sockets[0]));
    کپی_حافظه(مقصد->actions, from->actions, مقصد->شمار * حجم(مقصد->actions[0]));
    مقصد->n = from->n;
    Curl_pollset_init(from);
  }
}

/**
 *
 */
CURLcode Curl_pollset_change(بنية Curl_easy *داده,
                             بنية easy_pollset *ps, curl_socket_t sock,
                             صحيح جمع_کن_flags, صحيح remove_flags)
{
  بیعلامت صحيح i;

#اگر_تعریف_شده DEBUGBUILD
  DEBUGASSERT(ps->init == CURL_EASY_POLLSET_MAGIC);
#پایان_اگر

  DEBUGASSERT(VALID_SOCK(sock));
  إذا(!VALID_SOCK(sock))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  DEBUGASSERT(جمع_کن_flags <= (CURL_POLL_درون | CURL_POLL_بیرون));
  DEBUGASSERT(remove_flags <= (CURL_POLL_درون | CURL_POLL_بیرون));
  DEBUGASSERT((جمع_کن_flags & remove_flags) == 0); /* no overlap */
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      ps->actions[i] &= (بیعلامت حرف)(~remove_flags);
      ps->actions[i] |= (بیعلامت حرف)جمع_کن_flags;
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
  إذا(جمع_کن_flags) {
    إذا(i >= ps->شمار) { /* need to grow */
      بیعلامت صحيح new_شمار = CURLMAX(ps->شمار * 2, 8);
      curl_socket_t *nsockets;
      بیعلامت حرف *nactions;

      CURL_TRC_M(داده, "growing pollset capacity from %u to %u",
                 ps->شمار, new_شمار);
      إذا(new_شمار <= ps->شمار)
        أرجع CURLE_بیرون_از_حافظه;
      nsockets = curlx_calloc(new_شمار, حجم(nsockets[0]));
      إذا(!nsockets)
        أرجع CURLE_بیرون_از_حافظه;
      nactions = curlx_calloc(new_شمار, حجم(nactions[0]));
      إذا(!nactions) {
        curlx_free(nsockets);
        أرجع CURLE_بیرون_از_حافظه;
      }
      کپی_حافظه(nsockets, ps->sockets, ps->شمار * حجم(ps->sockets[0]));
      کپی_حافظه(nactions, ps->actions, ps->شمار * حجم(ps->actions[0]));
      إذا(ps->sockets != ps->def_sockets)
        curlx_free(ps->sockets);
      ps->sockets = nsockets;
      إذا(ps->actions != ps->def_actions)
        curlx_free(ps->actions);
      ps->actions = nactions;
      ps->شمار = new_شمار;
    }
    DEBUGASSERT(i < ps->شمار);
    إذا(i < ps->شمار) {
      ps->sockets[i] = sock;
      ps->actions[i] = (بیعلامت حرف)جمع_کن_flags;
      ps->n = i + 1;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_pollset_مجموعه(بنية Curl_easy *داده,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_درون, bool do_بیرون)
{
  أرجع Curl_pollset_change(داده, ps, sock,
                             (do_درون ? CURL_POLL_درون : 0) |
                             (do_بیرون ? CURL_POLL_بیرون : 0),
                             (!do_درون ? CURL_POLL_درون : 0) |
                             (!do_بیرون ? CURL_POLL_بیرون : 0));
}

فراغ Curl_pollset_remove(بنية easy_pollset *ps, curl_socket_t sock)
{
  بیعلامت صحيح i;
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
صحيح Curl_pollset_poll(بنية Curl_easy *داده,
                      بنية easy_pollset *ps,
                      timediff_t timeout_ms)
{
  بنية pollfd *pfds;
  بیعلامت صحيح i, npfds;
  صحيح rc;

  (فراغ)داده;
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);

  إذا(!ps->n)
    أرجع curlx_wait_ms(timeout_ms);

  pfds = curlx_calloc(ps->n, حجم(*pfds));
  إذا(!pfds)
    أرجع -1;

  npfds = 0;
  لكل(i = 0; i < ps->n; ++i) {
    کوتاه events = 0;
    إذا(ps->actions[i] & CURL_POLL_درون) {
      events |= POLLIN;
    }
    إذا(ps->actions[i] & CURL_POLL_بیرون) {
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

فراغ Curl_pollset_check(بنية Curl_easy *داده,
                        بنية easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write)
{
  بیعلامت صحيح i;

  (فراغ)داده;
  DEBUGASSERT(VALID_SOCK(sock));
  لكل(i = 0; i < ps->n; ++i) {
    إذا(ps->sockets[i] == sock) {
      *pwant_read = !!(ps->actions[i] & CURL_POLL_درون);
      *pwant_write = !!(ps->actions[i] & CURL_POLL_بیرون);
      أرجع;
    }
  }
  *pwant_read = *pwant_write = FALSE;
}

bool Curl_pollset_want_recv(بنية Curl_easy *داده,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  بیعلامت صحيح i;
  (فراغ)داده;
  لكل(i = 0; i < ps->n; ++i) {
    إذا((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_درون))
      أرجع TRUE;
  }
  أرجع FALSE;
}

bool Curl_pollset_want_send(بنية Curl_easy *داده,
                            بنية easy_pollset *ps,
                            curl_socket_t sock)
{
  بیعلامت صحيح i;
  (فراغ)داده;
  لكل(i = 0; i < ps->n; ++i) {
    إذا((ps->sockets[i] == sock) && (ps->actions[i] & CURL_POLL_بیرون))
      أرجع TRUE;
  }
  أرجع FALSE;
}
