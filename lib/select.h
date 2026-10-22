#अगर_अघोषित HEADER_CURL_SELECT_H
#घोषणा HEADER_CURL_SELECT_H
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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_POLL_H
#समावेश <poll.h>
#या_अगर defined(HAVE_SYS_POLL_H)
#समावेश <sys/poll.h>
#पूर्ण_अगर

/*
 * Definition of pollfd struct and constants for platforms lacking them.
 */

#अगर !defined(HAVE_SYS_POLL_H) && !defined(HAVE_POLL_H) && !defined(POLLIN)

#घोषणा POLLIN      0x01
#घोषणा POLLPRI     0x02
#घोषणा POLLOUT     0x04
#घोषणा POLLERR     0x08
#घोषणा POLLHUP     0x10
#घोषणा POLLNVAL    0x20

काष्ठा pollfd {
  curl_socket_t fd;
  लघु events;
  लघु revents;
};

#पूर्ण_अगर

#अगर_अघोषित POLLRDNORM
#घोषणा POLLRDNORM POLLIN
#पूर्ण_अगर

#अगर_अघोषित POLLWRNORM
#घोषणा POLLWRNORM POLLOUT
#पूर्ण_अगर

#अगर_अघोषित POLLRDBAND
#घोषणा POLLRDBAND POLLPRI
#पूर्ण_अगर

/* there are three CSELECT defines that are defined in the public header that
   are exposed to users, but this *IN2 bit is only ever used internally and
   therefore defined here */
#घोषणा CURL_CSELECT_IN2 (CURL_CSELECT_ERR << 1)

पूर्णांक Curl_socket_check(curl_socket_t readfd0,
                      curl_socket_t readfd1,
                      curl_socket_t writefd,
                      timediff_t timeout_ms);
#घोषणा SOCKET_READABLE(x, z)                               \
  Curl_socket_check(x, CURL_SOCKET_BAD, CURL_SOCKET_BAD, z)
#घोषणा SOCKET_WRITABLE(x, z)                               \
  Curl_socket_check(CURL_SOCKET_BAD, CURL_SOCKET_BAD, x, z)

पूर्णांक Curl_poll(काष्ठा pollfd ufds[], अचिन्हित पूर्णांक nfds, timediff_t timeout_ms);

/* With Winsock the valid range is [0..INVALID_SOCKET-1] according to
   https://learn.microsoft.com/windows/win32/winsock/socket-data-type-2 */
#अगर_घोषित USE_WINSOCK
#घोषणा VALID_SOCK(s) ((s) < INVALID_SOCKET)
#घोषणा FDSET_SOCK(x) 1
#घोषणा VERIFY_SOCK(x)           \
  करो {                           \
    अगर(!VALID_SOCK(x)) {         \
      SET_SOCKERRNO(SOCKEINVAL); \
      वापस -1;                 \
    }                            \
  } जबतक(0)
#अन्यथा
#घोषणा VALID_SOCK(s) ((s) >= 0)

/* If the socket is small enough to get set or read from an fdset */
#घोषणा FDSET_SOCK(s) ((s) < FD_SETSIZE)

#घोषणा VERIFY_SOCK(x)                     \
  करो {                                     \
    अगर(!VALID_SOCK(x) || !FDSET_SOCK(x)) { \
      SET_SOCKERRNO(SOCKEINVAL);           \
      वापस -1;                           \
    }                                      \
  } जबतक(0)
#पूर्ण_अगर

/* Keep the sockets to poll for an easy handle.
 * `actions` are bitmaps of CURL_POLL_IN and CURL_POLL_OUT.
 * Starts with small capacity, grows on demand.
 */
#घोषणा EZ_POLLSET_DEF_गिनती 2

काष्ठा easy_pollset {
  curl_socket_t *sockets;
  अचिन्हित अक्षर *actions;
  अचिन्हित पूर्णांक n;
  अचिन्हित पूर्णांक गिनती;
#अगर_घोषित DEBUGBUILD
  पूर्णांक init;
#पूर्ण_अगर
  curl_socket_t def_sockets[EZ_POLLSET_DEF_गिनती];
  अचिन्हित अक्षर def_actions[EZ_POLLSET_DEF_गिनती];
};

#अगर_घोषित DEBUGBUILD
#घोषणा CURL_EASY_POLLSET_MAGIC  0x7a657370
#पूर्ण_अगर

/* allocate and initialize */
काष्ठा easy_pollset *Curl_pollset_create(व्योम);

/* Initialize before first use */
व्योम Curl_pollset_init(काष्ठा easy_pollset *ps);
/* Free any allocated resources */
व्योम Curl_pollset_cleanup(काष्ठा easy_pollset *ps);
/* Reset to an empty pollset */
व्योम Curl_pollset_reset(काष्ठा easy_pollset *ps);
/* Move pollset from to pollset to, replacing all in to,
 * leaving from empty. */
व्योम Curl_pollset_move(काष्ठा easy_pollset *to, काष्ठा easy_pollset *from);

/* Change the poll flags (CURL_POLL_IN/CURL_POLL_OUT) to the poll set for
 * socket `sock`. If the socket is not already part of the poll set, it
 * will be added.
 * If the socket is present and all poll flags are cleared, it will be removed.
 */
CURLcode Curl_pollset_change(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps, curl_socket_t sock,
                             पूर्णांक add_flags,
                             पूर्णांक remove_flags) WARN_UNUSED_परिणाम;

CURLcode Curl_pollset_set(काष्ठा Curl_easy *तथ्य,
                          काष्ठा easy_pollset *ps, curl_socket_t sock,
                          bool do_in, bool do_out) WARN_UNUSED_परिणाम;

#घोषणा Curl_pollset_add_in(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, CURL_POLL_IN, 0)
#घोषणा Curl_pollset_remove_in(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, 0, CURL_POLL_IN)
#घोषणा Curl_pollset_add_out(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, CURL_POLL_OUT, 0)
#घोषणा Curl_pollset_remove_out(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, 0, CURL_POLL_OUT)
#घोषणा Curl_pollset_add_inout(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, CURL_POLL_IN | CURL_POLL_OUT, 0)
#घोषणा Curl_pollset_set_in_only(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, CURL_POLL_IN, CURL_POLL_OUT)
#घोषणा Curl_pollset_set_out_only(तथ्य, ps, sock) \
  Curl_pollset_change(तथ्य, ps, sock, CURL_POLL_OUT, CURL_POLL_IN)

व्योम Curl_pollset_remove(काष्ठा easy_pollset *ps, curl_socket_t sock);

/* return < = on error, 0 on timeout or how many sockets are ready */
पूर्णांक Curl_pollset_poll(काष्ठा Curl_easy *तथ्य,
                      काष्ठा easy_pollset *ps,
                      timediff_t timeout_ms);

/**
 * Check if the pollset, as is, wants to read and/or write regarding
 * the given socket.
 */
व्योम Curl_pollset_check(काष्ठा Curl_easy *तथ्य,
                        काष्ठा easy_pollset *ps, curl_socket_t sock,
                        bool *pwant_read, bool *pwant_write);

/* TRUE if the pollset contains socket with CURL_POLL_IN. */
bool Curl_pollset_want_recv(काष्ठा Curl_easy *तथ्य,
                            काष्ठा easy_pollset *ps,
                            curl_socket_t sock);
/* TRUE if the pollset contains socket with CURL_POLL_OUT. */
bool Curl_pollset_want_send(काष्ठा Curl_easy *तथ्य,
                            काष्ठा easy_pollset *ps,
                            curl_socket_t sock);

काष्ठा curl_pollfds {
  काष्ठा pollfd *pfds;
  अचिन्हित पूर्णांक n;
  अचिन्हित पूर्णांक गिनती;
  BIT(allocated_pfds);
};

व्योम Curl_pollfds_init(काष्ठा curl_pollfds *cpfds,
                       काष्ठा pollfd *static_pfds,
                       अचिन्हित पूर्णांक static_गिनती);

व्योम Curl_pollfds_reset(काष्ठा curl_pollfds *cpfds);

व्योम Curl_pollfds_cleanup(काष्ठा curl_pollfds *cpfds);

CURLcode Curl_pollfds_add_ps(काष्ठा curl_pollfds *cpfds,
                             काष्ठा easy_pollset *ps);

CURLcode Curl_pollfds_add_sock(काष्ठा curl_pollfds *cpfds,
                               curl_socket_t sock, लघु events);

काष्ठा Curl_waitfds {
  काष्ठा curl_waitfd *wfds;
  अचिन्हित पूर्णांक n;
  अचिन्हित पूर्णांक गिनती;
};

व्योम Curl_waitfds_init(काष्ठा Curl_waitfds *cwfds,
                       काष्ठा curl_waitfd *static_wfds,
                       अचिन्हित पूर्णांक static_गिनती);

अचिन्हित पूर्णांक Curl_waitfds_add_ps(काष्ठा Curl_waitfds *cwfds,
                                 काष्ठा easy_pollset *ps);

#पूर्ण_अगर /* HEADER_CURL_SELECT_H */
