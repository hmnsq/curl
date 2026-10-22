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
#شامل "curl_setup.h"

#ifdef HAVE_POLL_H
#شامل <poll.h>
#وگرنهاگر defined(HAVE_SYS_POLL_H)
#شامل <sys/poll.h>
#endif

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#اگر !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#define POLLIN      0x01
#define POLLPRI     0x02
#define POLLOUT     0x04
#define POLLERR     0x08
#define POLLHUP     0x10
#define POLLNVAL    0x20

ساختار pollfd {
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

صحیح Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#define SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#define SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

صحیح Curl_poll(ساختار pollfd ufds[], unsigned صحیح nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#ifdef USE_WINSOCK
#define VALID_SOCK(s) ((s) < INVALID_SOCKET)
#define FDSET_SOCK(x) 1
#define VERIFY_SOCK(x)           \
  انجامبده {                           \
    اگر(!VALID_SOCK(x)) {         \
      SET_SOCKERRNO(SOCKEINVAL); \
      بازگشت -1;                 \
    }                            \
  } وقتی(0)
#وگرنه
#define VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#define FDSET_SOCK(s) ((s) < FD_SETSIZE)

#define VERIFY_SOCK(x)                     \
  انجامبده {                                     \
    اگر(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      SET_SOCKERRNO(SOCKEINVAL);           \
      بازگشت -1;                           \
    }                                      \
  } وقتی(0)
#endif

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#define EZ_POLLSET_DEF_شمار 2

ساختار easy_pollset {
  curl_socket_t *sockets;
  unsigned نویسه *actions;
  unsigned صحیح n;
  unsigned صحیح شمار;
#ifdef DEBUGBUILD
  صحیح init;
#endif
  curl_socket_t def_sockets[EZ_POLLSET_DEF_شمار];
  unsigned نویسه def_actions[EZ_POLLSET_DEF_شمار];
};

#ifdef DEBUGBUILD
#define CURL_EASY_POLLSET_MAGIC  0x7a657370
#endif

/* allocate and initialize */
ساختار easy_pollset *Curl_pollset_create(تهی);

/* Initialize before first use */
تهی Curl_pollset_init(ساختار easy_pollset *ps);
/* Free any allocated resources */
تهی Curl_pollset_cleanup(ساختار easy_pollset *ps);
/* Reset to an empty pollset */
تهی Curl_pollset_reset(ساختار easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
تهی Curl_pollset_move(ساختار easy_pollset *to, ساختار easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps, curl_socket_t sock,
                             صحیح add_flags,
                             صحیح remove_flags) WARN_UNUSED_RESULT;

CURLcode Curl_pollset_set(ساختار Curl_easy *data,
                          ساختار easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out) WARN_UNUSED_RESULT;

#define Curl_pollset_add_in(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0)
#define Curl_pollset_remove_in(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, 0, CURL_POLL_IN)
#define Curl_pollset_add_out(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, CURL_POLL_OUT, 0)
#define Curl_pollset_remove_out(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, 0, CURL_POLL_OUT)
#define Curl_pollset_add_inout(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, CURL_POLL_IN | CURL_POLL_OUT, 0)
#define Curl_pollset_set_in_only(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, CURL_POLL_IN, CURL_POLL_OUT)
#define Curl_pollset_set_out_only(data, ps, sock) \
  Curl_pollset_change(data, ps, sock, CURL_POLL_OUT, CURL_POLL_IN)

تهی Curl_pollset_remove(ساختار easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
صحیح Curl_pollset_poll(ساختار Curl_easy *data,
                      ساختار easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
تهی Curl_pollset_check(ساختار Curl_easy *data,
                        ساختار easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(ساختار Curl_easy *data,
                            ساختار easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(ساختار Curl_easy *data,
                            ساختار easy_pollset *ps,
                            curl_socket_t sock);

ساختار curl_pollfds {
  ساختار pollfd *pfds;
  unsigned صحیح n;
  unsigned صحیح شمار;
  BIT(allocated_pfds);
};

تهی Curl_pollfds_init(ساختار curl_pollfds *cpfds,
                       ساختار pollfd *static_pfds,
                       unsigned صحیح static_شمار);

تهی Curl_pollfds_reset(ساختار curl_pollfds *cpfds);

تهی Curl_pollfds_cleanup(ساختار curl_pollfds *cpfds);

CURLcode Curl_pollfds_add_ps(ساختار curl_pollfds *cpfds,
                             ساختار easy_pollset *ps);

CURLcode Curl_pollfds_add_sock(ساختار curl_pollfds *cpfds,
                               curl_socket_t sock, short events);

ساختار Curl_waitfds {
  ساختار curl_waitfd *wfds;
  unsigned صحیح n;
  unsigned صحیح شمار;
};

تهی Curl_waitfds_init(ساختار Curl_waitfds *cwfds,
                       ساختار curl_waitfd *static_wfds,
                       unsigned صحیح static_شمار);

unsigned صحیح Curl_waitfds_add_ps(ساختار Curl_waitfds *cwfds,
                                 ساختار easy_pollset *ps);

#endif /* HEADER_CURL_SELECT_H */
