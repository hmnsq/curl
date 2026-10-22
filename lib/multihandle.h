#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_MULTIHANDLE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_MULTIHANDLE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "api.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cshutdn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ev.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ntfy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "psl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socketpair.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "splay.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-bset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-spbset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/dnscache.h"

ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_easy;

/* NOTE: if you add a state here, add the name to the statenames[] array
 * in curl_trc.c as well!
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  MSTATE_INIT,         /* 0 - start in this state */
  MSTATE_PENDING,      /* no connections, waiting for one */
  MSTATE_ਤਿਆਰੀ,        /* start a new transfer */
  MSTATE_CONNECT,      /* resolve/connect has been sent off */
  MSTATE_CONNECTING,   /* awaiting the TCP connect to finalize */
  MSTATE_PROTOCONNECT, /* initiate protocol connect procedure */
  MSTATE_PROTOCONNECTING, /* completing the protocol-specific connect phase */
  MSTATE_DO,           /* start send off the request (part 1) */
  MSTATE_DOING,        /* sending off the request (part 1) */
  MSTATE_DOING_ਹੋਰ,   /* send off the request (part 2) */
  MSTATE_DID,          /* done sending off request */
  MSTATE_PERFORMING,   /* transfer data */
  MSTATE_RATELIMITING, /* wait because limit-rate exceeded */
  MSTATE_ਪੂਰਾ,         /* post data transfer operation */
  MSTATE_COMPLETED,    /* operation complete */
  MSTATE_MSGSENT,      /* the operation complete message is sent */
  MSTATE_ਆਖਰੀ          /* not a true state, never use this */
} CURLMstate;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPIPE_ANY (CURLPIPE_MULTIPLEX)

#ਜੇ !defined(CURL_DISABLE_SOCKETPAIR) && !defined(USE_WINSOCK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENABLE_WAKEUP
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_SOCKETPAIR) && \
    defined(USE_RESOLV_THREADED) && \
    !defined(USE_WINSOCK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
#ਜੇ_ਅੰਤ

/* value for MAXIMUM CONCURRENT STREAMS upper limit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INITIAL_MAX_CONCURRENT_STREAMS ((1U << 31) - 1)

ਬਣਤਰ Curl_fixed_buf {
  size_t len;      /* the allocated data length */
  ਅੱਖਰ ਅੰਕੜੇ[1];   /* the actual buffer */
};

/* This is the struct known as CURLM on the outside */
ਬਣਤਰ Curl_multi {
  /* First a simple identifier to more easily detect if a user mixes up
     this multi handle with an easy handle.
     Set this to CURLMULTI_MAGIC_NUMBER. */
  uint32_t magic;
  uint32_t xfers_alive; /* amount of added transfers that have
                           not yet reached COMPLETE state */
  uint32_t xfers_really_alive; /* amount of added transfers that have
                                  passed INIT state but are not COMPLETE yet */
  uint32_t max_concurrent_streams;
  curl_off_t xfers_ਕੁੱਲ_ever; /* total of added transfers, ever. */

  ਬਣਤਰ uint32_tbl xfers; /* transfers added to this multi */
  /* A transfer's mid may be present in at most one of
   * process/pending/msgsent. The dirty set may overlap with process. */
  ਬਣਤਰ uint32_bset process; /* transfer being processed */
  ਬਣਤਰ uint32_bset dirty; /* transfer to be run NOW, e.g. ASAP. */
  ਬਣਤਰ uint32_bset pending; /* transfers in waiting (conn limit etc.) */
  ਬਣਤਰ uint32_bset msgsent; /* transfers with unread application messages */

  ਬਣਤਰ Curl_mapi_ਢੇਰ callstack; /* multi api calls ongoing */

  /* expiration times for all attached easy handles */
  ਬਣਤਰ Curl_timeouts timeouts;

  ਬਣਤਰ Curl_easy *admin; /* internal easy handle for admin operations.
                              gets assigned `mid` 0 on multi init */

  /* callback function and user data pointer for the *socket() API */
  curl_socket_callback socket_cb;
  ਖਾਲੀ *socket_userp;

  /* callback function and user data pointer for server push */
  curl_ਧੱਕੋ_callback ਧੱਕੋ_cb;
  ਖਾਲੀ *ਧੱਕੋ_userp;

  ਬਣਤਰ Curl_dnscache dnscache; /* DNS cache */
  ਬਣਤਰ Curl_ssl_scache *ssl_scache; /* TLS session pool */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
  ਬਣਤਰ curl_thrdq *resolv_thrdq;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  /* PSL cache. */
  ਬਣਤਰ PslCache psl;
#ਜੇ_ਅੰਤ

  ਬਣਤਰ Curl_fixed_buf *xfer_buf; /* transfer downloads, lazy */
  ਬਣਤਰ Curl_fixed_buf *xfer_ulbuf; /* transfer uploads, lazy */
  ਬਣਤਰ Curl_fixed_buf *xfer_sockbuf; /* transfer socket I/O, lazy */

  /* multi event related things */
  ਬਣਤਰ curl_multi_ev ev;
  /* multi notification related things */
  ਬਣਤਰ curl_multi_ntfy ntfy;

  /* `proto_hash` is a general key-value store for protocol implementations
   * with the lifetime of the multi handle. The number of elements kept here
   * should be in the order of supported protocols (and sub-protocols like
   * TLS), *not* in the order of connections or current transfers!
   * Elements need to be added with their own destructor to be invoked when
   * the multi handle is cleaned up (see Curl_hash_add2()).*/
  ਬਣਤਰ Curl_hash proto_hash;

  ਬਣਤਰ cshutdn cshutdn; /* connection shutdown handling */
  ਬਣਤਰ cpool cpool;     /* connection pool (bundles) */

  timediff_t ਆਖਰੀ_expire_ਵਿੱਥ_us; /* times offset of last expiry */

  /* timer callback and user data pointer for the *socket() API */
  curl_multi_timer_callback timer_cb;
  ਖਾਲੀ *timer_userp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  WSAEVENT wsa_event; /* Winsock event used for waits */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  curl_socket_t wakeup_pair[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used by curl_multi_wakeup() */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  curl_socket_t wakeup_ਅੰਦਰੂਨੀ[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used for internal wakeups,
                                   e.g. threaded resolver. */
#ਜੇ_ਅੰਤ
  uint32_t max_host_connections; /* if >0, a fixed limit of the maximum number
                                  of connections per host */
  uint32_t max_ਕੁੱਲ_connections; /* if >0, a fixed limit of the maximum number
                                   of connections in total */
  uint32_t maxconnects; /* if >0, a fixed limit of the maximum number of
                               entries we are allowed to grow the connection
                               cache to */
  uint32_t ਆਖਰੀ_pending_mid; /* mid of last pending transfer rescheduled */
  uint32_t ਆਖਰੀ_resolv_id; /* id of the last DNS resolve operation */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ipv6_ਚੱਲਦਾ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(multiplexing);           /* multiplexing wanted */
  ਦੋਆਧਾਰੀ_ਅੰਕ(recheckstate);           /* see Curl_multi_connchanged */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  ਦੋਆਧਾਰੀ_ਅੰਕ(ssl_seeded);
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(dead); /* a callback returned error, everything needs to crash and
                burn */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਆਖਰੀ_timeout_ਸੈੱਟ);      /* last timer callback value was nonnegative */
  ਦੋਆਧਾਰੀ_ਅੰਕ(xfer_buf_borrowed);      /* xfer_buf is currently being borrowed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(xfer_ulbuf_borrowed);    /* xfer_ulbuf is currently being borrowed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(xfer_sockbuf_borrowed);  /* xfer_sockbuf is currently being borrowed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(quick_exit);             /* do not join threads on cleanup */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਦੋਆਧਾਰੀ_ਅੰਕ(warned);                 /* true after user warned of DEBUGBUILD */
#ਜੇ_ਅੰਤ
};

#ਜੇ_ਅੰਤ /* HEADER_CURL_MULTIHANDLE_H */
