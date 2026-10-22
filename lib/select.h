#اگر_غیر_تعریف_شدہ سرخی_CURL_SELECT_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_POLL_H
#شامل <poll.h>
#وگرنہ_اگر defined(HAVE_SYS_POLL_H)
#شامل <sys/poll.h>
#اگر_ختم

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#اگر !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#تعریف_کرو POLLIN      0x01
#تعریف_کرو POLLPRI     0x02
#تعریف_کرو POLLOUT     0x04
#تعریف_کرو POLLERR     0x08
#تعریف_کرو POLLHUP     0x10
#تعریف_کرو POLLNVAL    0x20

ساخت pollfd {
  curl_socket_t fd;
  مختصر events;
  مختصر revents;
};

#اگر_ختم

#اگر_غیر_تعریف_شدہ POLLRDNORM
#تعریف_کرو POLLRDNORM POLLIN
#اگر_ختم

#اگر_غیر_تعریف_شدہ POLLWRNORM
#تعریف_کرو POLLWRNORM POLLOUT
#اگر_ختم

#اگر_غیر_تعریف_شدہ POLLRDBAND
#تعریف_کرو POLLRDBAND POLLPRI
#اگر_ختم

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#تعریف_کرو CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

صحیح Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#تعریف_کرو SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#تعریف_کرو SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

صحیح Curl_poll(ساخت pollfd ufds[], غیرعلامتی صحیح nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#اگر_تعریف_شدہ USE_WINSOCK
#تعریف_کرو VALID_SOCK(s) ((s) < INVALID_SOCKET)
#تعریف_کرو FDSET_SOCK(x) 1
#تعریف_کرو VERIFY_SOCK(x)           \
  کرو {                           \
    اگر(!VALID_SOCK(x)) {         \
      سیٹ_SOCKERRNO(SOCKEINVAL); \
      واپس -1;                 \
    }                            \
  } جب(0)
#ورنہ
#تعریف_کرو VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#تعریف_کرو FDSET_SOCK(s) ((s) < FD_SETSIZE)

#تعریف_کرو VERIFY_SOCK(x)                     \
  کرو {                                     \
    اگر(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      سیٹ_SOCKERRNO(SOCKEINVAL);           \
      واپس -1;                           \
    }                                      \
  } جب(0)
#اگر_ختم

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#تعریف_کرو EZ_POLLSET_DEF_گنتی 2

ساخت easy_pollset {
  curl_socket_t *sockets;
  غیرعلامتی حرف *actions;
  غیرعلامتی صحیح n;
  غیرعلامتی صحیح گنتی;
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح init;
#اگر_ختم
  curl_socket_t def_sockets[EZ_POLLSET_DEF_گنتی];
  غیرعلامتی حرف def_actions[EZ_POLLSET_DEF_گنتی];
};

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_EASY_POLLSET_MAGIC  0x7a657370
#اگر_ختم

/* allocate and initialize */
ساخت easy_pollset *Curl_pollset_create(خالی);

/* Initialize before first use */
خالی Curl_pollset_init(ساخت easy_pollset *ps);
/* Free any allocated resources */
خالی Curl_pollset_cleanup(ساخت easy_pollset *ps);
/* Reset to an empty pollset */
خالی Curl_pollset_reset(ساخت easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
خالی Curl_pollset_move(ساخت easy_pollset *مقصد, ساخت easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps, curl_socket_t sock,
                             صحیح جوڑو_flags,
                             صحیح remove_flags) WARN_غیر_مستعمل_نتیجہ;

CURLcode Curl_pollset_سیٹ(ساخت Curl_easy *ڈیٹا,
                          ساخت easy_pollset *ps, curl_socket_t sock,
                          bool do_اندر, bool do_برآمد) WARN_غیر_مستعمل_نتیجہ;

#تعریف_کرو Curl_pollset_جوڑو_اندر(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, CURL_POLL_اندر, 0)
#تعریف_کرو Curl_pollset_remove_اندر(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, 0, CURL_POLL_اندر)
#تعریف_کرو Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, CURL_POLL_برآمد, 0)
#تعریف_کرو Curl_pollset_remove_برآمد(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, 0, CURL_POLL_برآمد)
#تعریف_کرو Curl_pollset_جوڑو_inout(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, CURL_POLL_اندر | CURL_POLL_برآمد, 0)
#تعریف_کرو Curl_pollset_سیٹ_اندر_only(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, CURL_POLL_اندر, CURL_POLL_برآمد)
#تعریف_کرو Curl_pollset_سیٹ_برآمد_only(ڈیٹا, ps, sock) \
  Curl_pollset_change(ڈیٹا, ps, sock, CURL_POLL_برآمد, CURL_POLL_اندر)

خالی Curl_pollset_remove(ساخت easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
صحیح Curl_pollset_poll(ساخت Curl_easy *ڈیٹا,
                      ساخت easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
خالی Curl_pollset_check(ساخت Curl_easy *ڈیٹا,
                        ساخت easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(ساخت Curl_easy *ڈیٹا,
                            ساخت easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(ساخت Curl_easy *ڈیٹا,
                            ساخت easy_pollset *ps,
                            curl_socket_t sock);

ساخت curl_pollfds {
  ساخت pollfd *pfds;
  غیرعلامتی صحیح n;
  غیرعلامتی صحیح گنتی;
  ثنائی_ہندسہ(allocated_pfds);
};

خالی Curl_pollfds_init(ساخت curl_pollfds *cpfds,
                       ساخت pollfd *static_pfds,
                       غیرعلامتی صحیح static_گنتی);

خالی Curl_pollfds_reset(ساخت curl_pollfds *cpfds);

خالی Curl_pollfds_cleanup(ساخت curl_pollfds *cpfds);

CURLcode Curl_pollfds_جوڑو_ps(ساخت curl_pollfds *cpfds,
                             ساخت easy_pollset *ps);

CURLcode Curl_pollfds_جوڑو_sock(ساخت curl_pollfds *cpfds,
                               curl_socket_t sock, مختصر events);

ساخت Curl_waitfds {
  ساخت curl_waitfd *wfds;
  غیرعلامتی صحیح n;
  غیرعلامتی صحیح گنتی;
};

خالی Curl_waitfds_init(ساخت Curl_waitfds *cwfds,
                       ساخت curl_waitfd *static_wfds,
                       غیرعلامتی صحیح static_گنتی);

غیرعلامتی صحیح Curl_waitfds_جوڑو_ps(ساخت Curl_waitfds *cwfds,
                                 ساخت easy_pollset *ps);

#اگر_ختم /* HEADER_CURL_SELECT_H */
