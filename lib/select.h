#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SELECT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SELECT_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_POLL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਜਾਂਜੇ defined(HAVE_SYS_POLL_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/poll.h>
#ਜੇ_ਅੰਤ

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#ਜੇ !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLIN      0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLPRI     0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLOUT     0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLERR     0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLHUP     0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLNVAL    0x20

ਬਣਤਰ pollfd {
  curl_socket_t fd;
  ਛੋਟਾ events;
  ਛੋਟਾ revents;
};

#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ POLLRDNORM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLRDNORM POLLIN
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ POLLWRNORM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLWRNORM POLLOUT
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ POLLRDBAND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POLLRDBAND POLLPRI
#ਜੇ_ਅੰਤ

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

ਪੂਰਨ Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

ਪੂਰਨ Curl_poll(ਬਣਤਰ pollfd ufds[], ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_SOCK(s) ((s) < INVALID_SOCKET)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDSET_SOCK(x) 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERIFY_SOCK(x)           \
  ਕਰੋ {                           \
    ਜੇ(!VALID_SOCK(x)) {         \
      ਸੈੱਟ_SOCKERRNO(SOCKEINVAL); \
      ਵਾਪਸ -1;                 \
    }                            \
  } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDSET_SOCK(s) ((s) < FD_SETSIZE)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VERIFY_SOCK(x)                     \
  ਕਰੋ {                                     \
    ਜੇ(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      ਸੈੱਟ_SOCKERRNO(SOCKEINVAL);           \
      ਵਾਪਸ -1;                           \
    }                                      \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EZ_POLLSET_DEF_ਗਿਣਤੀ 2

ਬਣਤਰ easy_pollset {
  curl_socket_t *sockets;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *actions;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਗਿਣਤੀ_2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ init;
#ਜੇ_ਅੰਤ
  curl_socket_t def_sockets[EZ_POLLSET_DEF_ਗਿਣਤੀ];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ def_actions[EZ_POLLSET_DEF_ਗਿਣਤੀ];
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EASY_POLLSET_MAGIC  0x7a657370
#ਜੇ_ਅੰਤ

/* allocate and initialize */
ਬਣਤਰ easy_pollset *Curl_pollset_create(ਖਾਲੀ);

/* Initialize before first use */
ਖਾਲੀ Curl_pollset_init(ਬਣਤਰ easy_pollset *ps);
/* Free any allocated resources */
ਖਾਲੀ Curl_pollset_cleanup(ਬਣਤਰ easy_pollset *ps);
/* Reset to an empty pollset */
ਖਾਲੀ Curl_pollset_reset(ਬਣਤਰ easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
ਖਾਲੀ Curl_pollset_move(ਬਣਤਰ easy_pollset *ਮੰਜ਼ਿਲ, ਬਣਤਰ easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps, curl_socket_t sock,
                             ਪੂਰਨ ਜੋੜੋ_flags,
                             ਪੂਰਨ remove_flags) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

CURLcode Curl_pollset_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ easy_pollset *ps, curl_socket_t sock,
                          bool do_ਵਿੱਚ, bool do_ਬਾਹਰਲਾ) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, CURL_POLL_ਵਿੱਚ, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_remove_ਵਿੱਚ(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, 0, CURL_POLL_ਵਿੱਚ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, CURL_POLL_ਬਾਹਰਲਾ, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_remove_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, 0, CURL_POLL_ਬਾਹਰਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_ਜੋੜੋ_inout(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, CURL_POLL_ਵਿੱਚ | CURL_POLL_ਬਾਹਰਲਾ, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_ਸੈੱਟ_ਵਿੱਚ_only(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, CURL_POLL_ਵਿੱਚ, CURL_POLL_ਬਾਹਰਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, ps, sock) \
  Curl_pollset_change(ਅੰਕੜੇ, ps, sock, CURL_POLL_ਬਾਹਰਲਾ, CURL_POLL_ਵਿੱਚ)

ਖਾਲੀ Curl_pollset_remove(ਬਣਤਰ easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
ਪੂਰਨ Curl_pollset_poll(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
ਖਾਲੀ Curl_pollset_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps,
                            curl_socket_t sock);

ਬਣਤਰ curl_pollfds {
  ਬਣਤਰ pollfd *pfds;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਗਿਣਤੀ_2;
  ਦੋਆਧਾਰੀ_ਅੰਕ(allocated_pfds);
};

ਖਾਲੀ Curl_pollfds_init(ਬਣਤਰ curl_pollfds *cpfds,
                       ਬਣਤਰ pollfd *static_pfds,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ static_ਗਿਣਤੀ);

ਖਾਲੀ Curl_pollfds_reset(ਬਣਤਰ curl_pollfds *cpfds);

ਖਾਲੀ Curl_pollfds_cleanup(ਬਣਤਰ curl_pollfds *cpfds);

CURLcode Curl_pollfds_ਜੋੜੋ_ps(ਬਣਤਰ curl_pollfds *cpfds,
                             ਬਣਤਰ easy_pollset *ps);

CURLcode Curl_pollfds_ਜੋੜੋ_sock(ਬਣਤਰ curl_pollfds *cpfds,
                               curl_socket_t sock, ਛੋਟਾ events);

ਬਣਤਰ Curl_waitfds {
  ਬਣਤਰ curl_waitfd *wfds;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਗਿਣਤੀ_2;
};

ਖਾਲੀ Curl_waitfds_init(ਬਣਤਰ Curl_waitfds *cwfds,
                       ਬਣਤਰ curl_waitfd *static_wfds,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ static_ਗਿਣਤੀ);

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ Curl_waitfds_ਜੋੜੋ_ps(ਬਣਤਰ Curl_waitfds *cwfds,
                                 ਬਣਤਰ easy_pollset *ps);

#ਜੇ_ਅੰਤ /* HEADER_CURL_SELECT_H */
