#که_نه_تعریف_شوی سرلیک_CURL_SELECT_H
#تعریف_کړه سرلیک_CURL_SELECT_H
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

#که_تعریف_شوی HAVE_POLL_H
#ضمن <poll.h>
#وإلاإذا defined(HAVE_SYS_POLL_H)
#ضمن <sys/poll.h>
#د_که_پای

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#إذا !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#تعریف_کړه POLLIN      0x01
#تعریف_کړه POLLPRI     0x02
#تعریف_کړه POLLOUT     0x04
#تعریف_کړه POLLERR     0x08
#تعریف_کړه POLLHUP     0x10
#تعریف_کړه POLLNVAL    0x20

بنية pollfd {
  curl_socket_t fd;
  لنډ events;
  لنډ revents;
};

#د_که_پای

#که_نه_تعریف_شوی POLLRDNORM
#تعریف_کړه POLLRDNORM POLLIN
#د_که_پای

#که_نه_تعریف_شوی POLLWRNORM
#تعریف_کړه POLLWRNORM POLLOUT
#د_که_پای

#که_نه_تعریف_شوی POLLRDBAND
#تعریف_کړه POLLRDBAND POLLPRI
#د_که_پای

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#تعریف_کړه CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

صحيح Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#تعریف_کړه SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#تعریف_کړه SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

صحيح Curl_poll(بنية pollfd ufds[], بینښه صحيح nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#که_تعریف_شوی USE_WINSOCK
#تعریف_کړه VALID_SOCK(s) ((s) < INVALID_SOCKET)
#تعریف_کړه FDSET_SOCK(x) 1
#تعریف_کړه VERIFY_SOCK(x)           \
  افعل {                           \
    إذا(!VALID_SOCK(x)) {         \
      سېټ_SOCKERRNO(SOCKEINVAL); \
      أرجع -1;                 \
    }                            \
  } طالما(0)
#وإلا
#تعریف_کړه VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#تعریف_کړه FDSET_SOCK(s) ((s) < FD_SETSIZE)

#تعریف_کړه VERIFY_SOCK(x)                     \
  افعل {                                     \
    إذا(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      سېټ_SOCKERRNO(SOCKEINVAL);           \
      أرجع -1;                           \
    }                                      \
  } طالما(0)
#د_که_پای

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#تعریف_کړه EZ_POLLSET_DEF_شمېر 2

بنية easy_pollset {
  curl_socket_t *sockets;
  بینښه حرف *actions;
  بینښه صحيح n;
  بینښه صحيح شمېر;
#که_تعریف_شوی DEBUGBUILD
  صحيح init;
#د_که_پای
  curl_socket_t def_sockets[EZ_POLLSET_DEF_شمېر];
  بینښه حرف def_actions[EZ_POLLSET_DEF_شمېر];
};

#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه CURL_EASY_POLLSET_MAGIC  0x7a657370
#د_که_پای

/* allocate and initialize */
بنية easy_pollset *Curl_pollset_create(فراغ);

/* Initialize before first use */
فراغ Curl_pollset_init(بنية easy_pollset *ps);
/* Free any allocated resources */
فراغ Curl_pollset_cleanup(بنية easy_pollset *ps);
/* Reset to an empty pollset */
فراغ Curl_pollset_reset(بنية easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
فراغ Curl_pollset_move(بنية easy_pollset *منزل, بنية easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps, curl_socket_t sock,
                             صحيح جمع_کړه_flags,
                             صحيح remove_flags) WARN_ناکارول_شوی_پایله;

CURLcode Curl_pollset_سېټ(بنية Curl_easy *معلومات,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_دننه, bool do_بهر) WARN_ناکارول_شوی_پایله;

#تعریف_کړه Curl_pollset_جمع_کړه_دننه(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_دننه, 0)
#تعریف_کړه Curl_pollset_remove_دننه(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, 0, CURL_POLL_دننه)
#تعریف_کړه Curl_pollset_جمع_کړه_بهر(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_بهر, 0)
#تعریف_کړه Curl_pollset_remove_بهر(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, 0, CURL_POLL_بهر)
#تعریف_کړه Curl_pollset_جمع_کړه_inout(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_دننه | CURL_POLL_بهر, 0)
#تعریف_کړه Curl_pollset_سېټ_دننه_only(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_دننه, CURL_POLL_بهر)
#تعریف_کړه Curl_pollset_سېټ_بهر_only(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_بهر, CURL_POLL_دننه)

فراغ Curl_pollset_remove(بنية easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
صحيح Curl_pollset_poll(بنية Curl_easy *معلومات,
                      بنية easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
فراغ Curl_pollset_check(بنية Curl_easy *معلومات,
                        بنية easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps,
                            curl_socket_t sock);

بنية curl_pollfds {
  بنية pollfd *pfds;
  بینښه صحيح n;
  بینښه صحيح شمېر;
  بټ(allocated_pfds);
};

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       بینښه صحيح static_شمېر);

فراغ Curl_pollfds_reset(بنية curl_pollfds *cpfds);

فراغ Curl_pollfds_cleanup(بنية curl_pollfds *cpfds);

CURLcode Curl_pollfds_جمع_کړه_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps);

CURLcode Curl_pollfds_جمع_کړه_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, لنډ events);

بنية Curl_waitfds {
  بنية curl_waitfd *wfds;
  بینښه صحيح n;
  بینښه صحيح شمېر;
};

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       بینښه صحيح static_شمېر);

بینښه صحيح Curl_waitfds_جمع_کړه_ps(بنية Curl_waitfds *cwfds,
                                 بنية easy_pollset *ps);

#د_که_پای /* HEADER_CURL_SELECT_H */
