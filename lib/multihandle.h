#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MULTIHANDLE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MULTIHANDLE_H
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
#ಅನ್ತರ್ಭಾವ "api.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"
#ಅನ್ತರ್ಭಾವ "cshutdn.h"
#ಅನ್ತರ್ಭಾವ "multi_ev.h"
#ಅನ್ತರ್ಭಾವ "multi_ntfy.h"
#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "socketpair.h"
#ಅನ್ತರ್ಭಾವ "splay.h"
#ಅನ್ತರ್ಭಾವ "uint-bset.h"
#ಅನ್ತರ್ಭಾವ "uint-spbset.h"
#ಅನ್ತರ್ಭಾವ "uint-table.h"
#ಅನ್ತರ್ಭಾವ "vdns/dnscache.h"

ರಚನೆ connectdata;
ರಚನೆ Curl_easy;

/* NOTE: if you add a state here, add the name to the statenames[] array
 * in curl_trc.c as well!
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  MSTATE_INIT,         /* 0 - start in this state */
  MSTATE_PENDING,      /* no connections, waiting for one */
  MSTATE_ಸಿದ್ಧತೆ,        /* start a new transfer */
  MSTATE_CONNECT,      /* resolve/connect has been sent off */
  MSTATE_CONNECTING,   /* awaiting the TCP connect to finalize */
  MSTATE_PROTOCONNECT, /* initiate protocol connect procedure */
  MSTATE_PROTOCONNECTING, /* completing the protocol-specific connect phase */
  MSTATE_DO,           /* start send off the request (part 1) */
  MSTATE_DOING,        /* sending off the request (part 1) */
  MSTATE_DOING_ಇನ್ನಷ್ಟು,   /* send off the request (part 2) */
  MSTATE_DID,          /* done sending off request */
  MSTATE_PERFORMING,   /* transfer data */
  MSTATE_RATELIMITING, /* wait because limit-rate exceeded */
  MSTATE_ಮುಗಿದ,         /* post data transfer operation */
  MSTATE_COMPLETED,    /* operation complete */
  MSTATE_MSGSENT,      /* the operation complete message is sent */
  MSTATE_ಕೊನೆಯ          /* not a true state, never use this */
} CURLMstate;

#ವ್ಯಾಖ್ಯಾನಿಸು CURLPIPE_ANY (CURLPIPE_MULTIPLEX)

#ಆದರೆ !defined(CURL_DISABLE_SOCKETPAIR) && !defined(USE_WINSOCK)
#ವ್ಯಾಖ್ಯಾನಿಸು ENABLE_WAKEUP
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_SOCKETPAIR) && \
    defined(USE_RESOLV_THREADED) && \
    !defined(USE_WINSOCK)
#ವ್ಯಾಖ್ಯಾನಿಸು ENABLE_ಆಂತರಿಕ_WAKEUP
#ಆದರೆ_ಅಂತ್ಯ

/* value for MAXIMUM CONCURRENT STREAMS upper limit */
#ವ್ಯಾಖ್ಯಾನಿಸು INITIAL_MAX_CONCURRENT_STREAMS ((1U << 31) - 1)

ರಚನೆ Curl_fixed_buf {
  size_t len;      /* the allocated data length */
  ಅಕ್ಷರ ದತ್ತಾಂಶ[1];   /* the actual buffer */
};

/* This is the struct known as CURLM on the outside */
ರಚನೆ Curl_multi {
  /* First a simple identifier to more easily detect if a user mixes up
     this multi handle with an easy handle.
     Set this to CURLMULTI_MAGIC_NUMBER. */
  uint32_t magic;
  uint32_t xfers_alive; /* amount of added transfers that have
                           not yet reached COMPLETE state */
  uint32_t xfers_really_alive; /* amount of added transfers that have
                                  passed INIT state but are not COMPLETE yet */
  uint32_t max_concurrent_streams;
  curl_off_t xfers_ಒಟ್ಟು_ever; /* total of added transfers, ever. */

  ರಚನೆ uint32_tbl xfers; /* transfers added to this multi */
  /* A transfer's mid may be present in at most one of
   * process/pending/msgsent. The dirty set may overlap with process. */
  ರಚನೆ uint32_bset process; /* transfer being processed */
  ರಚನೆ uint32_bset dirty; /* transfer to be run NOW, e.g. ASAP. */
  ರಚನೆ uint32_bset pending; /* transfers in waiting (conn limit etc.) */
  ರಚನೆ uint32_bset msgsent; /* transfers with unread application messages */

  ರಚನೆ Curl_mapi_ರಾಶಿ callstack; /* multi api calls ongoing */

  /* expiration times for all attached easy handles */
  ರಚನೆ Curl_timeouts timeouts;

  ರಚನೆ Curl_easy *admin; /* internal easy handle for admin operations.
                              gets assigned `mid` 0 on multi init */

  /* callback function and user data pointer for the *socket() API */
  curl_socket_callback socket_cb;
  ಶೂನ್ಯ *socket_userp;

  /* callback function and user data pointer for server push */
  curl_ತಳ್ಳು_callback ತಳ್ಳು_cb;
  ಶೂನ್ಯ *ತಳ್ಳು_userp;

  ರಚನೆ Curl_dnscache dnscache; /* DNS cache */
  ರಚನೆ Curl_ssl_scache *ssl_scache; /* TLS session pool */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
  ರಚನೆ curl_thrdq *resolv_thrdq;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  /* PSL cache. */
  ರಚನೆ PslCache psl;
#ಆದರೆ_ಅಂತ್ಯ

  ರಚನೆ Curl_fixed_buf *xfer_buf; /* transfer downloads, lazy */
  ರಚನೆ Curl_fixed_buf *xfer_ulbuf; /* transfer uploads, lazy */
  ರಚನೆ Curl_fixed_buf *xfer_sockbuf; /* transfer socket I/O, lazy */

  /* multi event related things */
  ರಚನೆ curl_multi_ev ev;
  /* multi notification related things */
  ರಚನೆ curl_multi_ntfy ntfy;

  /* `proto_hash` is a general key-value store for protocol implementations
   * with the lifetime of the multi handle. The number of elements kept here
   * should be in the order of supported protocols (and sub-protocols like
   * TLS), *not* in the order of connections or current transfers!
   * Elements need to be added with their own destructor to be invoked when
   * the multi handle is cleaned up (see Curl_hash_add2()).*/
  ರಚನೆ Curl_hash proto_hash;

  ರಚನೆ cshutdn cshutdn; /* connection shutdown handling */
  ರಚನೆ cpool cpool;     /* connection pool (bundles) */

  timediff_t ಕೊನೆಯ_expire_ಸ್ಥಾನಾಂತರ_us; /* times offset of last expiry */

  /* timer callback and user data pointer for the *socket() API */
  curl_multi_timer_callback timer_cb;
  ಶೂನ್ಯ *timer_userp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  WSAEVENT wsa_event; /* Winsock event used for waits */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  curl_socket_t wakeup_pair[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used by curl_multi_wakeup() */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  curl_socket_t wakeup_ಆಂತರಿಕ[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used for internal wakeups,
                                   e.g. threaded resolver. */
#ಆದರೆ_ಅಂತ್ಯ
  uint32_t max_host_connections; /* if >0, a fixed limit of the maximum number
                                  of connections per host */
  uint32_t max_ಒಟ್ಟು_connections; /* if >0, a fixed limit of the maximum number
                                   of connections in total */
  uint32_t maxconnects; /* if >0, a fixed limit of the maximum number of
                               entries we are allowed to grow the connection
                               cache to */
  uint32_t ಕೊನೆಯ_pending_mid; /* mid of last pending transfer rescheduled */
  uint32_t ಕೊನೆಯ_resolv_id; /* id of the last DNS resolve operation */
  ಬಿಟ್(ipv6_ಕೆಲಸಮಾಡುತ್ತದೆ);
  ಬಿಟ್(multiplexing);           /* multiplexing wanted */
  ಬಿಟ್(recheckstate);           /* see Curl_multi_connchanged */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  ಬಿಟ್(ssl_seeded);
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(dead); /* a callback returned error, everything needs to crash and
                burn */
  ಬಿಟ್(ಕೊನೆಯ_timeout_ಗಣ);      /* last timer callback value was nonnegative */
  ಬಿಟ್(xfer_buf_borrowed);      /* xfer_buf is currently being borrowed */
  ಬಿಟ್(xfer_ulbuf_borrowed);    /* xfer_ulbuf is currently being borrowed */
  ಬಿಟ್(xfer_sockbuf_borrowed);  /* xfer_sockbuf is currently being borrowed */
  ಬಿಟ್(quick_exit);             /* do not join threads on cleanup */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಬಿಟ್(warned);                 /* true after user warned of DEBUGBUILD */
#ಆದರೆ_ಅಂತ್ಯ
};

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MULTIHANDLE_H */
