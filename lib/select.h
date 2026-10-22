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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_POLL_H
#ಅನ್ತರ್ಭಾವ <poll.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_SYS_POLL_H)
#ಅನ್ತರ್ಭಾವ <sys/poll.h>
#endif

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#ಯದಿ !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#define POLLIN      0x01
#define POLLPRI     0x02
#define POLLOUT     0x04
#define POLLERR     0x08
#define POLLHUP     0x10
#define POLLNVAL    0x20

ಸಂರಚನಾ pollfd {
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

ಪೂರ್ಣಾಙ್ಕ Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#define SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#define SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

ಪೂರ್ಣಾಙ್ಕ Curl_poll(ಸಂರಚನಾ pollfd ufds[], unsigned ಪೂರ್ಣಾಙ್ಕ nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#ifdef USE_WINSOCK
#define VALID_SOCK(s) ((s) < INVALID_SOCKET)
#define FDSET_SOCK(x) 1
#define VERIFY_SOCK(x)           \
  ಕುರು {                           \
    ಯದಿ(!VALID_SOCK(x)) {         \
      SET_SOCKERRNO(SOCKEINVAL); \
      ಪ್ರತ್ಯಾಗಮ -1;                 \
    }                            \
  } ಯಾವತ್(0)
#ಅನ್ಯಥಾ
#define VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#define FDSET_SOCK(s) ((s) < FD_SETSIZE)

#define VERIFY_SOCK(x)                     \
  ಕುರು {                                     \
    ಯದಿ(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      SET_SOCKERRNO(SOCKEINVAL);           \
      ಪ್ರತ್ಯಾಗಮ -1;                           \
    }                                      \
  } ಯಾವತ್(0)
#endif

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#define EZ_POLLSET_DEF_ಎಣಿಕೆ 2

ಸಂರಚನಾ easy_pollset {
  curl_socket_t *sockets;
  unsigned ವರ್ಣ *actions;
  unsigned ಪೂರ್ಣಾಙ್ಕ n;
  unsigned ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ init;
#endif
  curl_socket_t def_sockets[EZ_POLLSET_DEF_ಎಣಿಕೆ];
  unsigned ವರ್ಣ def_actions[EZ_POLLSET_DEF_ಎಣಿಕೆ];
};

#ifdef DEBUGBUILD
#define CURL_EASY_POLLSET_MAGIC  0x7a657370
#endif

/* allocate and initialize */
ಸಂರಚನಾ easy_pollset *Curl_pollset_create(ಶೂನ್ಯ);

/* Initialize before first use */
ಶೂನ್ಯ Curl_pollset_init(ಸಂರಚನಾ easy_pollset *ps);
/* Free any allocated resources */
ಶೂನ್ಯ Curl_pollset_cleanup(ಸಂರಚನಾ easy_pollset *ps);
/* Reset to an empty pollset */
ಶೂನ್ಯ Curl_pollset_reset(ಸಂರಚನಾ easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
ಶೂನ್ಯ Curl_pollset_move(ಸಂರಚನಾ easy_pollset *to, ಸಂರಚನಾ easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock,
                             ಪೂರ್ಣಾಙ್ಕ add_flags,
                             ಪೂರ್ಣಾಙ್ಕ remove_flags) WARN_UNUSED_RESULT;

CURLcode Curl_pollset_set(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock,
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

ಶೂನ್ಯ Curl_pollset_remove(ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
ಪೂರ್ಣಾಙ್ಕ Curl_pollset_poll(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
ಶೂನ್ಯ Curl_pollset_check(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ easy_pollset *ps,
                            curl_socket_t sock);

ಸಂರಚನಾ curl_pollfds {
  ಸಂರಚನಾ pollfd *pfds;
  unsigned ಪೂರ್ಣಾಙ್ಕ n;
  unsigned ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
  BIT(allocated_pfds);
};

ಶೂನ್ಯ Curl_pollfds_init(ಸಂರಚನಾ curl_pollfds *cpfds,
                       ಸಂರಚನಾ pollfd *static_pfds,
                       unsigned ಪೂರ್ಣಾಙ್ಕ static_ಎಣಿಕೆ);

ಶೂನ್ಯ Curl_pollfds_reset(ಸಂರಚನಾ curl_pollfds *cpfds);

ಶೂನ್ಯ Curl_pollfds_cleanup(ಸಂರಚನಾ curl_pollfds *cpfds);

CURLcode Curl_pollfds_add_ps(ಸಂರಚನಾ curl_pollfds *cpfds,
                             ಸಂರಚನಾ easy_pollset *ps);

CURLcode Curl_pollfds_add_sock(ಸಂರಚನಾ curl_pollfds *cpfds,
                               curl_socket_t sock, short events);

ಸಂರಚನಾ Curl_waitfds {
  ಸಂರಚನಾ curl_waitfd *wfds;
  unsigned ಪೂರ್ಣಾಙ್ಕ n;
  unsigned ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
};

ಶೂನ್ಯ Curl_waitfds_init(ಸಂರಚನಾ Curl_waitfds *cwfds,
                       ಸಂರಚನಾ curl_waitfd *static_wfds,
                       unsigned ಪೂರ್ಣಾಙ್ಕ static_ಎಣಿಕೆ);

unsigned ಪೂರ್ಣಾಙ್ಕ Curl_waitfds_add_ps(ಸಂರಚನಾ Curl_waitfds *cwfds,
                                 ಸಂರಚನಾ easy_pollset *ps);

#endif /* HEADER_CURL_SELECT_H */
