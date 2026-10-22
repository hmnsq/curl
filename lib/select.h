#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SELECT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SELECT_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_POLL_H
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅಥವಾಆದರೆ defined(HAVE_SYS_POLL_H)
#ಅನ್ತರ್ಭಾವ <sys/poll.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#ಆದರೆ !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#ವ್ಯಾಖ್ಯಾನಿಸು POLLIN      0x01
#ವ್ಯಾಖ್ಯಾನಿಸು POLLPRI     0x02
#ವ್ಯಾಖ್ಯಾನಿಸು POLLOUT     0x04
#ವ್ಯಾಖ್ಯಾನಿಸು POLLERR     0x08
#ವ್ಯಾಖ್ಯಾನಿಸು POLLHUP     0x10
#ವ್ಯಾಖ್ಯಾನಿಸು POLLNVAL    0x20

ರಚನೆ pollfd {
  curl_socket_t fd;
  ಗಿಡ್ಡ events;
  ಗಿಡ್ಡ revents;
};

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ POLLRDNORM
#ವ್ಯಾಖ್ಯಾನಿಸು POLLRDNORM POLLIN
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ POLLWRNORM
#ವ್ಯಾಖ್ಯಾನಿಸು POLLWRNORM POLLOUT
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ POLLRDBAND
#ವ್ಯಾಖ್ಯಾನಿಸು POLLRDBAND POLLPRI
#ಆದರೆ_ಅಂತ್ಯ

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

ಪೂರ್ಣಾಂಕ Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#ವ್ಯಾಖ್ಯಾನಿಸು SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

ಪೂರ್ಣಾಂಕ Curl_poll(ರಚನೆ pollfd ufds[], ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_SOCK(s) ((s) < INVALID_SOCKET)
#ವ್ಯಾಖ್ಯಾನಿಸು FDSET_SOCK(x) 1
#ವ್ಯಾಖ್ಯಾನಿಸು VERIFY_SOCK(x)           \
  ಮಾಡು {                           \
    ಆದರೆ(!VALID_SOCK(x)) {         \
      ಗಣ_SOCKERRNO(SOCKEINVAL); \
      ಹಿಂತಿರುಗಿ -1;                 \
    }                            \
  } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#ವ್ಯಾಖ್ಯಾನಿಸು FDSET_SOCK(s) ((s) < FD_SETSIZE)

#ವ್ಯಾಖ್ಯಾನಿಸು VERIFY_SOCK(x)                     \
  ಮಾಡು {                                     \
    ಆದರೆ(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      ಗಣ_SOCKERRNO(SOCKEINVAL);           \
      ಹಿಂತಿರುಗಿ -1;                           \
    }                                      \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು EZ_POLLSET_DEF_ಎಣಿಕೆ 2

ರಚನೆ easy_pollset {
  curl_socket_t *sockets;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *actions;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ init;
#ಆದರೆ_ಅಂತ್ಯ
  curl_socket_t def_sockets[EZ_POLLSET_DEF_ಎಣಿಕೆ];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ def_actions[EZ_POLLSET_DEF_ಎಣಿಕೆ];
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EASY_POLLSET_MAGIC  0x7a657370
#ಆದರೆ_ಅಂತ್ಯ

/* allocate and initialize */
ರಚನೆ easy_pollset *Curl_pollset_create(ಶೂನ್ಯ);

/* Initialize before first use */
ಶೂನ್ಯ Curl_pollset_init(ರಚನೆ easy_pollset *ps);
/* Free any allocated resources */
ಶೂನ್ಯ Curl_pollset_cleanup(ರಚನೆ easy_pollset *ps);
/* Reset to an empty pollset */
ಶೂನ್ಯ Curl_pollset_reset(ರಚನೆ easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
ಶೂನ್ಯ Curl_pollset_move(ರಚನೆ easy_pollset *ಗಮ್ಯ, ರಚನೆ easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps, curl_socket_t sock,
                             ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_flags,
                             ಪೂರ್ಣಾಂಕ remove_flags) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

CURLcode Curl_pollset_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ easy_pollset *ps, curl_socket_t sock,
                          bool do_ಒಳ, bool do_ಹೊರಹೋಗು) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, CURL_POLL_ಒಳ, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_remove_ಒಳ(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, 0, CURL_POLL_ಒಳ)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, CURL_POLL_ಹೊರಹೋಗು, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_remove_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, 0, CURL_POLL_ಹೊರಹೋಗು)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_ಕೂಡಿಸು_inout(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, CURL_POLL_ಒಳ | CURL_POLL_ಹೊರಹೋಗು, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_ಗಣ_ಒಳ_only(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, CURL_POLL_ಒಳ, CURL_POLL_ಹೊರಹೋಗು)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, ps, sock) \
  Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, CURL_POLL_ಹೊರಹೋಗು, CURL_POLL_ಒಳ)

ಶೂನ್ಯ Curl_pollset_remove(ರಚನೆ easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
ಪೂರ್ಣಾಂಕ Curl_pollset_poll(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
ಶೂನ್ಯ Curl_pollset_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps,
                            curl_socket_t sock);

ರಚನೆ curl_pollfds {
  ರಚನೆ pollfd *pfds;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
  ಬಿಟ್(allocated_pfds);
};

ಶೂನ್ಯ Curl_pollfds_init(ರಚನೆ curl_pollfds *cpfds,
                       ರಚನೆ pollfd *static_pfds,
                       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ static_ಎಣಿಕೆ);

ಶೂನ್ಯ Curl_pollfds_reset(ರಚನೆ curl_pollfds *cpfds);

ಶೂನ್ಯ Curl_pollfds_cleanup(ರಚನೆ curl_pollfds *cpfds);

CURLcode Curl_pollfds_ಕೂಡಿಸು_ps(ರಚನೆ curl_pollfds *cpfds,
                             ರಚನೆ easy_pollset *ps);

CURLcode Curl_pollfds_ಕೂಡಿಸು_sock(ರಚನೆ curl_pollfds *cpfds,
                               curl_socket_t sock, ಗಿಡ್ಡ events);

ರಚನೆ Curl_waitfds {
  ರಚನೆ curl_waitfd *wfds;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
};

ಶೂನ್ಯ Curl_waitfds_init(ರಚನೆ Curl_waitfds *cwfds,
                       ರಚನೆ curl_waitfd *static_wfds,
                       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ static_ಎಣಿಕೆ);

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ Curl_waitfds_ಕೂಡಿಸು_ps(ರಚನೆ Curl_waitfds *cwfds,
                                 ರಚನೆ easy_pollset *ps);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SELECT_H */
