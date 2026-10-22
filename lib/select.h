#جے_بنا_تعریف سرخی_CURL_SELECT_H
#تعریف_کرو سرخی_CURL_SELECT_H
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

#جے_تعریف_شدہ HAVE_POLL_H
#ضمن <poll.h>
#وإلاإذا defined(HAVE_SYS_POLL_H)
#ضمن <sys/poll.h>
#جے_انت

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#إذا !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#تعریف_کرو POLLIN      0x01
#تعریف_کرو POLLPRI     0x02
#تعریف_کرو POLLOUT     0x04
#تعریف_کرو POLLERR     0x08
#تعریف_کرو POLLHUP     0x10
#تعریف_کرو POLLNVAL    0x20

بنية pollfd {
  curl_socket_t fd;
  چھوٹا events;
  چھوٹا revents;
};

#جے_انت

#جے_بنا_تعریف POLLRDNORM
#تعریف_کرو POLLRDNORM POLLIN
#جے_انت

#جے_بنا_تعریف POLLWRNORM
#تعریف_کرو POLLWRNORM POLLOUT
#جے_انت

#جے_بنا_تعریف POLLRDBAND
#تعریف_کرو POLLRDBAND POLLPRI
#جے_انت

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#تعریف_کرو CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

صحيح Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#تعریف_کرو SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#تعریف_کرو SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

صحيح Curl_poll(بنية pollfd ufds[], بنانشانی صحيح nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#جے_تعریف_شدہ USE_WINSOCK
#تعریف_کرو VALID_SOCK(s) ((s) < INVALID_SOCKET)
#تعریف_کرو FDSET_SOCK(x) 1
#تعریف_کرو VERIFY_SOCK(x)           \
  افعل {                           \
    إذا(!VALID_SOCK(x)) {         \
      سیٹ_SOCKERRNO(SOCKEINVAL); \
      أرجع -1;                 \
    }                            \
  } طالما(0)
#وإلا
#تعریف_کرو VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#تعریف_کرو FDSET_SOCK(s) ((s) < FD_SETSIZE)

#تعریف_کرو VERIFY_SOCK(x)                     \
  افعل {                                     \
    إذا(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      سیٹ_SOCKERRNO(SOCKEINVAL);           \
      أرجع -1;                           \
    }                                      \
  } طالما(0)
#جے_انت

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#تعریف_کرو EZ_POLLSET_DEF_گنتی 2

بنية easy_pollset {
  curl_socket_t *sockets;
  بنانشانی حرف *actions;
  بنانشانی صحيح n;
  بنانشانی صحيح گنتی;
#جے_تعریف_شدہ DEBUGBUILD
  صحيح init;
#جے_انت
  curl_socket_t def_sockets[EZ_POLLSET_DEF_گنتی];
  بنانشانی حرف def_actions[EZ_POLLSET_DEF_گنتی];
};

#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_EASY_POLLSET_MAGIC  0x7a657370
#جے_انت

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
CURLcode Curl_pollset_change(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps, curl_socket_t sock,
                             صحيح جوڑو_flags,
                             صحيح remove_flags) WARN_بن_ورتیا_نتیجہ;

CURLcode Curl_pollset_سیٹ(بنية Curl_easy *انکڑے,
                          بنية easy_pollset *ps, curl_socket_t sock,
                          bool do_وچ, bool do_باہرلا) WARN_بن_ورتیا_نتیجہ;

#تعریف_کرو Curl_pollset_جوڑو_وچ(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, CURL_POLL_وچ, 0)
#تعریف_کرو Curl_pollset_remove_وچ(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, 0, CURL_POLL_وچ)
#تعریف_کرو Curl_pollset_جوڑو_باہرلا(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, CURL_POLL_باہرلا, 0)
#تعریف_کرو Curl_pollset_remove_باہرلا(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, 0, CURL_POLL_باہرلا)
#تعریف_کرو Curl_pollset_جوڑو_inout(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, CURL_POLL_وچ | CURL_POLL_باہرلا, 0)
#تعریف_کرو Curl_pollset_سیٹ_وچ_only(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, CURL_POLL_وچ, CURL_POLL_باہرلا)
#تعریف_کرو Curl_pollset_سیٹ_باہرلا_only(انکڑے, ps, sock) \
  Curl_pollset_change(انکڑے, ps, sock, CURL_POLL_باہرلا, CURL_POLL_وچ)

فراغ Curl_pollset_remove(بنية easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
صحيح Curl_pollset_poll(بنية Curl_easy *انکڑے,
                      بنية easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
فراغ Curl_pollset_check(بنية Curl_easy *انکڑے,
                        بنية easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(بنية Curl_easy *انکڑے,
                            بنية easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(بنية Curl_easy *انکڑے,
                            بنية easy_pollset *ps,
                            curl_socket_t sock);

بنية curl_pollfds {
  بنية pollfd *pfds;
  بنانشانی صحيح n;
  بنانشانی صحيح گنتی;
  ثنائی_ہندسہ(allocated_pfds);
};

فراغ Curl_pollfds_init(بنية curl_pollfds *cpfds,
                       بنية pollfd *static_pfds,
                       بنانشانی صحيح static_گنتی);

فراغ Curl_pollfds_reset(بنية curl_pollfds *cpfds);

فراغ Curl_pollfds_cleanup(بنية curl_pollfds *cpfds);

CURLcode Curl_pollfds_جوڑو_ps(بنية curl_pollfds *cpfds,
                             بنية easy_pollset *ps);

CURLcode Curl_pollfds_جوڑو_sock(بنية curl_pollfds *cpfds,
                               curl_socket_t sock, چھوٹا events);

بنية Curl_waitfds {
  بنية curl_waitfd *wfds;
  بنانشانی صحيح n;
  بنانشانی صحيح گنتی;
};

فراغ Curl_waitfds_init(بنية Curl_waitfds *cwfds,
                       بنية curl_waitfd *static_wfds,
                       بنانشانی صحيح static_گنتی);

بنانشانی صحيح Curl_waitfds_جوڑو_ps(بنية Curl_waitfds *cwfds,
                                 بنية easy_pollset *ps);

#جے_انت /* HEADER_CURL_SELECT_H */
