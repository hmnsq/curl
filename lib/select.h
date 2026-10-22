#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SELECT_H
#నిర్వచించు శీర్షిక_CURL_SELECT_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_POLL_H
#అన్తర్భావ <poll.h>
#లేదావేళ defined(HAVE_SYS_POLL_H)
#అన్తర్భావ <sys/poll.h>
#అయితే_ముగింపు

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#ఒకవేళ !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#నిర్వచించు POLLIN      0x01
#నిర్వచించు POLLPRI     0x02
#నిర్వచించు POLLOUT     0x04
#నిర్వచించు POLLERR     0x08
#నిర్వచించు POLLHUP     0x10
#నిర్వచించు POLLNVAL    0x20

నిర్మాణం pollfd {
  curl_socket_t fd;
  పొట్టి events;
  పొట్టి revents;
};

#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే POLLRDNORM
#నిర్వచించు POLLRDNORM POLLIN
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే POLLWRNORM
#నిర్వచించు POLLWRNORM POLLOUT
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే POLLRDBAND
#నిర్వచించు POLLRDBAND POLLPRI
#అయితే_ముగింపు

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#నిర్వచించు CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

పూర్ణాంకం Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#నిర్వచించు SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#నిర్వచించు SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

పూర్ణాంకం Curl_poll(నిర్మాణం pollfd ufds[], గుర్తులేని పూర్ణాంకం nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#నిర్వచితమైతే USE_WINSOCK
#నిర్వచించు VALID_SOCK(s) ((s) < INVALID_SOCKET)
#నిర్వచించు FDSET_SOCK(x) 1
#నిర్వచించు VERIFY_SOCK(x)           \
  చేయి {                           \
    ఒకవేళ(!VALID_SOCK(x)) {         \
      సమితి_SOCKERRNO(SOCKEINVAL); \
      తిరిగి -1;                 \
    }                            \
  } అయితే(0)
#లేకపోతే
#నిర్వచించు VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#నిర్వచించు FDSET_SOCK(s) ((s) < FD_SETSIZE)

#నిర్వచించు VERIFY_SOCK(x)                     \
  చేయి {                                     \
    ఒకవేళ(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      సమితి_SOCKERRNO(SOCKEINVAL);           \
      తిరిగి -1;                           \
    }                                      \
  } అయితే(0)
#అయితే_ముగింపు

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#నిర్వచించు EZ_POLLSET_DEF_లెక్క 2

నిర్మాణం easy_pollset {
  curl_socket_t *sockets;
  గుర్తులేని అక్షరం *actions;
  గుర్తులేని పూర్ణాంకం n;
  గుర్తులేని పూర్ణాంకం లెక్క;
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం init;
#అయితే_ముగింపు
  curl_socket_t def_sockets[EZ_POLLSET_DEF_లెక్క];
  గుర్తులేని అక్షరం def_actions[EZ_POLLSET_DEF_లెక్క];
};

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_EASY_POLLSET_MAGIC  0x7a657370
#అయితే_ముగింపు

/* allocate and initialize */
నిర్మాణం easy_pollset *Curl_pollset_create(శూన్య);

/* Initialize before first use */
శూన్య Curl_pollset_init(నిర్మాణం easy_pollset *ps);
/* Free any allocated resources */
శూన్య Curl_pollset_cleanup(నిర్మాణం easy_pollset *ps);
/* Reset to an empty pollset */
శూన్య Curl_pollset_reset(నిర్మాణం easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
శూన్య Curl_pollset_move(నిర్మాణం easy_pollset *గమ్యం, నిర్మాణం easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps, curl_socket_t sock,
                             పూర్ణాంకం కూడు_flags,
                             పూర్ణాంకం remove_flags) WARN_వాడని_ఫలితం;

CURLcode Curl_pollset_సమితి(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం easy_pollset *ps, curl_socket_t sock,
                          bool do_లోపల, bool do_బహిర్గతం) WARN_వాడని_ఫలితం;

#నిర్వచించు Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, CURL_POLL_లోపల, 0)
#నిర్వచించు Curl_pollset_remove_లోపల(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, 0, CURL_POLL_లోపల)
#నిర్వచించు Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, CURL_POLL_బహిర్గతం, 0)
#నిర్వచించు Curl_pollset_remove_బహిర్గతం(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, 0, CURL_POLL_బహిర్గతం)
#నిర్వచించు Curl_pollset_కూడు_inout(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, CURL_POLL_లోపల | CURL_POLL_బహిర్గతం, 0)
#నిర్వచించు Curl_pollset_సమితి_లోపల_only(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, CURL_POLL_లోపల, CURL_POLL_బహిర్గతం)
#నిర్వచించు Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, ps, sock) \
  Curl_pollset_change(దత్తాంశం, ps, sock, CURL_POLL_బహిర్గతం, CURL_POLL_లోపల)

శూన్య Curl_pollset_remove(నిర్మాణం easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
పూర్ణాంకం Curl_pollset_poll(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
శూన్య Curl_pollset_check(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps,
                            curl_socket_t sock);

నిర్మాణం curl_pollfds {
  నిర్మాణం pollfd *pfds;
  గుర్తులేని పూర్ణాంకం n;
  గుర్తులేని పూర్ణాంకం లెక్క;
  బిట్టు(allocated_pfds);
};

శూన్య Curl_pollfds_init(నిర్మాణం curl_pollfds *cpfds,
                       నిర్మాణం pollfd *static_pfds,
                       గుర్తులేని పూర్ణాంకం static_లెక్క);

శూన్య Curl_pollfds_reset(నిర్మాణం curl_pollfds *cpfds);

శూన్య Curl_pollfds_cleanup(నిర్మాణం curl_pollfds *cpfds);

CURLcode Curl_pollfds_కూడు_ps(నిర్మాణం curl_pollfds *cpfds,
                             నిర్మాణం easy_pollset *ps);

CURLcode Curl_pollfds_కూడు_sock(నిర్మాణం curl_pollfds *cpfds,
                               curl_socket_t sock, పొట్టి events);

నిర్మాణం Curl_waitfds {
  నిర్మాణం curl_waitfd *wfds;
  గుర్తులేని పూర్ణాంకం n;
  గుర్తులేని పూర్ణాంకం లెక్క;
};

శూన్య Curl_waitfds_init(నిర్మాణం Curl_waitfds *cwfds,
                       నిర్మాణం curl_waitfd *static_wfds,
                       గుర్తులేని పూర్ణాంకం static_లెక్క);

గుర్తులేని పూర్ణాంకం Curl_waitfds_కూడు_ps(నిర్మాణం Curl_waitfds *cwfds,
                                 నిర్మాణం easy_pollset *ps);

#అయితే_ముగింపు /* HEADER_CURL_SELECT_H */
