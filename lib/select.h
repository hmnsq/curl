#यदि_अपरिभाषितम् शीर्षकम्_CURL_SELECT_H
#परिभाषय शीर्षकम्_CURL_SELECT_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_POLL_H
#अन्तर्भाव <poll.h>
#नोचेद्यदि defined(HAVE_SYS_POLL_H)
#अन्तर्भाव <sys/poll.h>
#यदि_अन्तः

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#यदि !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#परिभाषय POLLIN      0x01
#परिभाषय POLLPRI     0x02
#परिभाषय POLLOUT     0x04
#परिभाषय POLLERR     0x08
#परिभाषय POLLHUP     0x10
#परिभाषय POLLNVAL    0x20

संरचना pollfd {
  curl_socket_t fd;
  ह्रस्व events;
  ह्रस्व revents;
};

#यदि_अन्तः

#यदि_अपरिभाषितम् POLLRDNORM
#परिभाषय POLLRDNORM POLLIN
#यदि_अन्तः

#यदि_अपरिभाषितम् POLLWRNORM
#परिभाषय POLLWRNORM POLLOUT
#यदि_अन्तः

#यदि_अपरिभाषितम् POLLRDBAND
#परिभाषय POLLRDBAND POLLPRI
#यदि_अन्तः

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#परिभाषय CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

पूर्णाङ्क Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#परिभाषय SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#परिभाषय SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

पूर्णाङ्क Curl_poll(संरचना pollfd ufds[], अचिह्नित पूर्णाङ्क nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#यदि_परिभाषितम् USE_WINSOCK
#परिभाषय VALID_SOCK(s) ((s) < INVALID_SOCKET)
#परिभाषय FDSET_SOCK(x) 1
#परिभाषय VERIFY_SOCK(x)           \
  कुरु {                           \
    यदि(!VALID_SOCK(x)) {         \
      समुच्चयः_SOCKERRNO(SOCKEINVAL); \
      प्रत्यागम -1;                 \
    }                            \
  } यावत्(0)
#अन्यथा
#परिभाषय VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#परिभाषय FDSET_SOCK(s) ((s) < FD_SETSIZE)

#परिभाषय VERIFY_SOCK(x)                     \
  कुरु {                                     \
    यदि(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      समुच्चयः_SOCKERRNO(SOCKEINVAL);           \
      प्रत्यागम -1;                           \
    }                                      \
  } यावत्(0)
#यदि_अन्तः

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#परिभाषय EZ_POLLSET_DEF_गणनम् 2

संरचना easy_pollset {
  curl_socket_t *sockets;
  अचिह्नित वर्ण *actions;
  अचिह्नित पूर्णाङ्क n;
  अचिह्नित पूर्णाङ्क गणनम्;
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क init;
#यदि_अन्तः
  curl_socket_t def_sockets[EZ_POLLSET_DEF_गणनम्];
  अचिह्नित वर्ण def_actions[EZ_POLLSET_DEF_गणनम्];
};

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय CURL_EASY_POLLSET_MAGIC  0x7a657370
#यदि_अन्तः

/* allocate and initialize */
संरचना easy_pollset *Curl_pollset_create(शून्य);

/* Initialize before first use */
शून्य Curl_pollset_init(संरचना easy_pollset *ps);
/* Free any allocated resources */
शून्य Curl_pollset_cleanup(संरचना easy_pollset *ps);
/* Reset to an empty pollset */
शून्य Curl_pollset_reset(संरचना easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
शून्य Curl_pollset_move(संरचना easy_pollset *गन्तव्यम्, संरचना easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps, curl_socket_t sock,
                             पूर्णाङ्क सङ्कलय_flags,
                             पूर्णाङ्क remove_flags) WARN_अप्रयुक्तम्_परिणाम;

CURLcode Curl_pollset_समुच्चयः(संरचना Curl_easy *दत्त,
                          संरचना easy_pollset *ps, curl_socket_t sock,
                          bool do_अन्तर्, bool do_बहिर्गमनम्) WARN_अप्रयुक्तम्_परिणाम;

#परिभाषय Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, CURL_POLL_अन्तर्, 0)
#परिभाषय Curl_pollset_remove_अन्तर्(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, 0, CURL_POLL_अन्तर्)
#परिभाषय Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, CURL_POLL_बहिर्गमनम्, 0)
#परिभाषय Curl_pollset_remove_बहिर्गमनम्(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, 0, CURL_POLL_बहिर्गमनम्)
#परिभाषय Curl_pollset_सङ्कलय_inout(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, CURL_POLL_अन्तर् | CURL_POLL_बहिर्गमनम्, 0)
#परिभाषय Curl_pollset_समुच्चयः_अन्तर्_only(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, CURL_POLL_अन्तर्, CURL_POLL_बहिर्गमनम्)
#परिभाषय Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, ps, sock) \
  Curl_pollset_change(दत्त, ps, sock, CURL_POLL_बहिर्गमनम्, CURL_POLL_अन्तर्)

शून्य Curl_pollset_remove(संरचना easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
पूर्णाङ्क Curl_pollset_poll(संरचना Curl_easy *दत्त,
                      संरचना easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
शून्य Curl_pollset_check(संरचना Curl_easy *दत्त,
                        संरचना easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(संरचना Curl_easy *दत्त,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(संरचना Curl_easy *दत्त,
                            संरचना easy_pollset *ps,
                            curl_socket_t sock);

संरचना curl_pollfds {
  संरचना pollfd *pfds;
  अचिह्नित पूर्णाङ्क n;
  अचिह्नित पूर्णाङ्क गणनम्;
  BIT(allocated_pfds);
};

शून्य Curl_pollfds_init(संरचना curl_pollfds *cpfds,
                       संरचना pollfd *static_pfds,
                       अचिह्नित पूर्णाङ्क static_गणनम्);

शून्य Curl_pollfds_reset(संरचना curl_pollfds *cpfds);

शून्य Curl_pollfds_cleanup(संरचना curl_pollfds *cpfds);

CURLcode Curl_pollfds_सङ्कलय_ps(संरचना curl_pollfds *cpfds,
                             संरचना easy_pollset *ps);

CURLcode Curl_pollfds_सङ्कलय_sock(संरचना curl_pollfds *cpfds,
                               curl_socket_t sock, ह्रस्व events);

संरचना Curl_waitfds {
  संरचना curl_waitfd *wfds;
  अचिह्नित पूर्णाङ्क n;
  अचिह्नित पूर्णाङ्क गणनम्;
};

शून्य Curl_waitfds_init(संरचना Curl_waitfds *cwfds,
                       संरचना curl_waitfd *static_wfds,
                       अचिह्नित पूर्णाङ्क static_गणनम्);

अचिह्नित पूर्णाङ्क Curl_waitfds_सङ्कलय_ps(संरचना Curl_waitfds *cwfds,
                                 संरचना easy_pollset *ps);

#यदि_अन्तः /* HEADER_CURL_SELECT_H */
