#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SELECT_H
#وصف_ڪريو سرو_CURL_SELECT_H
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

#جيڪڏهن_وصف_ٿيل HAVE_POLL_H
#ضمن <poll.h>
#وإلاإذا defined(HAVE_SYS_POLL_H)
#ضمن <sys/poll.h>
#جيڪڏهن_پورو

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#إذا !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#وصف_ڪريو POLLIN      0x01
#وصف_ڪريو POLLPRI     0x02
#وصف_ڪريو POLLOUT     0x04
#وصف_ڪريو POLLERR     0x08
#وصف_ڪريو POLLHUP     0x10
#وصف_ڪريو POLLNVAL    0x20

بنية pollfd {
  curl_socket_t fd;
  ننڍو events;
  ننڍو revents;
};

#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل POLLRDNORM
#وصف_ڪريو POLLRDNORM POLLIN
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل POLLWRNORM
#وصف_ڪريو POLLWRNORM POLLOUT
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل POLLRDBAND
#وصف_ڪريو POLLRDBAND POLLPRI
#جيڪڏهن_پورو

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#وصف_ڪريو CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

صحيح Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#وصف_ڪريو SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#وصف_ڪريو SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

صحيح Curl_poll(بنية pollfd ufds[], بنانشان صحيح nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
#وصف_ڪريو VALID_SOCK(s) ((s) < INVALID_SOCKET)
#وصف_ڪريو FDSET_SOCK(x) 1
#وصف_ڪريو VERIFY_SOCK(x)           \
  افعل {                           \
    إذا(!VALID_SOCK(x)) {         \
      سيٽ_SOCKERRNO(SOCKEINVAL); \
      أرجع -1;                 \
    }                            \
  } طالما(0)
#وإلا
#وصف_ڪريو VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#وصف_ڪريو FDSET_SOCK(s) ((s) < FD_SETSIZE)

#وصف_ڪريو VERIFY_SOCK(x)                     \
  افعل {                                     \
    إذا(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      سيٽ_SOCKERRNO(SOCKEINVAL);           \
      أرجع -1;                           \
    }                                      \
  } طالما(0)
#جيڪڏهن_پورو

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#وصف_ڪريو EZ_POLLSET_DEF_ڳڻپ 2

بنية easy_pollset {
  curl_socket_t *sockets;
  بنانشان حرف *actions;
  بنانشان صحيح n;
  بنانشان صحيح ڳڻپ;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  صحيح init;
#جيڪڏهن_پورو
  curl_socket_t def_sockets[EZ_POLLSET_DEF_ڳڻپ];
  بنانشان حرف def_actions[EZ_POLLSET_DEF_ڳڻپ];
};

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو CURL_EASY_POLLSET_MAGIC  0x7a657370
#جيڪڏهن_پورو

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
                             صحيح جوڙيو_flags,
                             صحيح remove_flags) WARN_اڻ_استعمال_ٿيل_نتيجو;

CURLcode Curl_pollset_سيٽ(بنية Curl_easy *معلومات,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_اندريون, bool do_ٻاهريون) WARN_اڻ_استعمال_ٿيل_نتيجو;

#وصف_ڪريو Curl_pollset_جوڙيو_اندريون(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_اندريون, 0)
#وصف_ڪريو Curl_pollset_remove_اندريون(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, 0, CURL_POLL_اندريون)
#وصف_ڪريو Curl_pollset_جوڙيو_ٻاهريون(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_ٻاهريون, 0)
#وصف_ڪريو Curl_pollset_remove_ٻاهريون(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, 0, CURL_POLL_ٻاهريون)
#وصف_ڪريو Curl_pollset_جوڙيو_inout(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_اندريون | CURL_POLL_ٻاهريون, 0)
#وصف_ڪريو Curl_pollset_سيٽ_اندريون_only(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_اندريون, CURL_POLL_ٻاهريون)
#وصف_ڪريو Curl_pollset_سيٽ_ٻاهريون_only(معلومات, ps, sock) \
  Curl_pollset_change(معلومات, ps, sock, CURL_POLL_ٻاهريون, CURL_POLL_اندريون)

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
  بنانشان صحيح n;
  بنانشان صحيح ڳڻپ;
  ٻٽو_انگ(allocated_pfds);
};

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       بنانشان صحيح static_ڳڻپ);

فراغ Curl_pollfds_reset(بنية curl_pollfds *cpfds);

فراغ Curl_pollfds_cleanup(بنية curl_pollfds *cpfds);

CURLcode Curl_pollfds_جوڙيو_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps);

CURLcode Curl_pollfds_جوڙيو_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, ننڍو events);

بنية Curl_waitfds {
  بنية curl_waitfd *wfds;
  بنانشان صحيح n;
  بنانشان صحيح ڳڻپ;
};

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       بنانشان صحيح static_ڳڻپ);

بنانشان صحيح Curl_waitfds_جوڙيو_ps(بنية Curl_waitfds *cwfds,
                                 بنية easy_pollset *ps);

#جيڪڏهن_پورو /* HEADER_CURL_SELECT_H */
