#ifndef HEADER_CURL_SELECT_H
#define HEADER_CURL_SELECT_H
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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_POLL_H
#অন্তর্ভাব <poll.h>
#নতুবা_যদি defined(HAVE_SYS_POLL_H)
#অন্তর্ভাব <sys/poll.h>
#endif

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#যদি !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#define POLLIN      0x01
#define POLLPRI     0x02
#define POLLOUT     0x04
#define POLLERR     0x08
#define POLLHUP     0x10
#define POLLNVAL    0x20

কাঠামো pollfd {
  curl_socket_t fd;
  short events;
  short revents;
};

#endif

#ifndef POLLRDNORM
#define POLLRDNORM POLLIN
#endif

#ifndef POLLWRNORM
#define POLLWRNORM POLLOUT
#endif

#ifndef POLLRDBAND
#define POLLRDBAND POLLPRI
#endif

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#define CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

পূর্ণসংখ্যা Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#define SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#define SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

পূর্ণসংখ্যা Curl_poll(কাঠামো pollfd ufds[], unsigned পূর্ণসংখ্যা nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#ifdef USE_WINSOCK
#define VALID_SOCK(s) ((s) < INVALID_SOCKET)
#define FDSET_SOCK(x) 1
#define VERIFY_SOCK(x)           \
  করো {                           \
    যদি(!VALID_SOCK(x)) {         \
      SET_SOCKERRNO(SOCKEINVAL); \
      ফেরত -1;                 \
    }                            \
  } যখন(0)
#অন্যথা
#define VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#define FDSET_SOCK(s) ((s) < FD_SETSIZE)

#define VERIFY_SOCK(x)                     \
  করো {                                     \
    যদি(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      SET_SOCKERRNO(SOCKEINVAL);           \
      ফেরত -1;                           \
    }                                      \
  } যখন(0)
#endif

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#define EZ_POLLSET_DEF_গুনতি 2

কাঠামো easy_pollset {
  curl_socket_t *sockets;
  unsigned অক্ষর *actions;
  unsigned পূর্ণসংখ্যা n;
  unsigned পূর্ণসংখ্যা গুনতি;
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা init;
#endif
  curl_socket_t def_sockets[EZ_POLLSET_DEF_গুনতি];
  unsigned অক্ষর def_actions[EZ_POLLSET_DEF_গুনতি];
};

#ifdef DEBUGBUILD
#define CURL_EASY_POLLSET_MAGIC  0x7a657370
#endif

/* allocate and initialize */
কাঠামো easy_pollset *Curl_pollset_create(শূন্য);

/* Initialize before first use */
শূন্য Curl_pollset_init(কাঠামো easy_pollset *ps);
/* Free any allocated resources */
শূন্য Curl_pollset_cleanup(কাঠামো easy_pollset *ps);
/* Reset to an empty pollset */
শূন্য Curl_pollset_reset(কাঠামো easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
শূন্য Curl_pollset_move(কাঠামো easy_pollset *to, কাঠামো easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps, curl_socket_t sock,
                             পূর্ণসংখ্যা add_flags,
                             পূর্ণসংখ্যা remove_flags) WARN_UNUSED_ফলাফল;

CURLcode Curl_pollset_set(কাঠামো Curl_easy *তথ্য,
                          কাঠামো easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out) WARN_UNUSED_ফলাফল;

#define Curl_pollset_add_in(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, CURL_POLL_IN, 0)
#define Curl_pollset_remove_in(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, 0, CURL_POLL_IN)
#define Curl_pollset_add_out(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, CURL_POLL_OUT, 0)
#define Curl_pollset_remove_out(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, 0, CURL_POLL_OUT)
#define Curl_pollset_add_inout(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, CURL_POLL_IN | CURL_POLL_OUT, 0)
#define Curl_pollset_set_in_only(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, CURL_POLL_IN, CURL_POLL_OUT)
#define Curl_pollset_set_out_only(তথ্য, ps, sock) \
  Curl_pollset_change(তথ্য, ps, sock, CURL_POLL_OUT, CURL_POLL_IN)

শূন্য Curl_pollset_remove(কাঠামো easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
পূর্ণসংখ্যা Curl_pollset_poll(কাঠামো Curl_easy *তথ্য,
                      কাঠামো easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
শূন্য Curl_pollset_check(কাঠামো Curl_easy *তথ্য,
                        কাঠামো easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(কাঠামো Curl_easy *তথ্য,
                            কাঠামো easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(কাঠামো Curl_easy *তথ্য,
                            কাঠামো easy_pollset *ps,
                            curl_socket_t sock);

কাঠামো curl_pollfds {
  কাঠামো pollfd *pfds;
  unsigned পূর্ণসংখ্যা n;
  unsigned পূর্ণসংখ্যা গুনতি;
  BIT(allocated_pfds);
};

শূন্য Curl_pollfds_init(কাঠামো curl_pollfds *cpfds,
                       কাঠামো pollfd *static_pfds,
                       unsigned পূর্ণসংখ্যা static_গুনতি);

শূন্য Curl_pollfds_reset(কাঠামো curl_pollfds *cpfds);

শূন্য Curl_pollfds_cleanup(কাঠামো curl_pollfds *cpfds);

CURLcode Curl_pollfds_add_ps(কাঠামো curl_pollfds *cpfds,
                             কাঠামো easy_pollset *ps);

CURLcode Curl_pollfds_add_sock(কাঠামো curl_pollfds *cpfds,
                               curl_socket_t sock, short events);

কাঠামো Curl_waitfds {
  কাঠামো curl_waitfd *wfds;
  unsigned পূর্ণসংখ্যা n;
  unsigned পূর্ণসংখ্যা গুনতি;
};

শূন্য Curl_waitfds_init(কাঠামো Curl_waitfds *cwfds,
                       কাঠামো curl_waitfd *static_wfds,
                       unsigned পূর্ণসংখ্যা static_গুনতি);

unsigned পূর্ণসংখ্যা Curl_waitfds_add_ps(কাঠামো Curl_waitfds *cwfds,
                                 কাঠামো easy_pollset *ps);

#endif /* HEADER_CURL_SELECT_H */
