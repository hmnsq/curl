#اگر_غیر_تعریف_شدہ سرخی_CURL_MULTIHANDLE_H
#تعریف_کرو سرخی_CURL_MULTIHANDLE_H
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
#شامل "api.h"
#شامل "llist.h"
#شامل "hash.h"
#شامل "conncache.h"
#شامل "cshutdn.h"
#شامل "multi_ev.h"
#شامل "multi_ntfy.h"
#شامل "psl.h"
#شامل "socketpair.h"
#شامل "splay.h"
#شامل "uint-bset.h"
#شامل "uint-spbset.h"
#شامل "uint-table.h"
#شامل "vdns/dnscache.h"

ساخت connectdata;
ساخت Curl_easy;

/* NOTE: if you add a state here, add the name to the statenames[] array
 * in curl_trc.c as well!
 */
قسمتعریف شمار {
  MSTATE_INIT,         /* 0 - start in this state */
  MSTATE_PENDING,      /* no connections, waiting for one */
  MSTATE_تیاری,        /* start a new transfer */
  MSTATE_CONNECT,      /* resolve/connect has been sent off */
  MSTATE_CONNECTING,   /* awaiting the TCP connect to finalize */
  MSTATE_PROTOCONNECT, /* initiate protocol connect procedure */
  MSTATE_PROTOCONNECTING, /* completing the protocol-specific connect phase */
  MSTATE_DO,           /* start send off the request (part 1) */
  MSTATE_DOING,        /* sending off the request (part 1) */
  MSTATE_DOING_مزید,   /* send off the request (part 2) */
  MSTATE_DID,          /* done sending off request */
  MSTATE_PERFORMING,   /* transfer data */
  MSTATE_RATELIMITING, /* wait because limit-rate exceeded */
  MSTATE_مکمل,         /* post data transfer operation */
  MSTATE_COMPLETED,    /* operation complete */
  MSTATE_MSGSENT,      /* the operation complete message is sent */
  MSTATE_آخری          /* not a true state, never use this */
} CURLMstate;

#تعریف_کرو CURLPIPE_ANY (CURLPIPE_MULTIPLEX)

#اگر !defined(CURL_DISABLE_SOCKETPAIR) && !defined(USE_WINSOCK)
#تعریف_کرو ENABLE_WAKEUP
#اگر_ختم
#اگر !defined(CURL_DISABLE_SOCKETPAIR) && \
    defined(USE_RESOLV_THREADED) && \
    !defined(USE_WINSOCK)
#تعریف_کرو ENABLE_اندرونی_WAKEUP
#اگر_ختم

/* value for MAXIMUM CONCURRENT STREAMS upper limit */
#تعریف_کرو INITIAL_MAX_CONCURRENT_STREAMS ((1U << 31) - 1)

ساخت Curl_fixed_buf {
  size_t len;      /* the allocated data length */
  حرف ڈیٹا[1];   /* the actual buffer */
};

/* This is the struct known as CURLM on the outside */
ساخت Curl_multi {
  /* First a simple identifier to more easily detect if a user mixes up
     this multi handle with an easy handle.
     Set this to CURLMULTI_MAGIC_NUMBER. */
  uint32_t magic;
  uint32_t xfers_alive; /* amount of added transfers that have
                           not yet reached COMPLETE state */
  uint32_t xfers_really_alive; /* amount of added transfers that have
                                  passed INIT state but are not COMPLETE yet */
  uint32_t max_concurrent_streams;
  curl_off_t xfers_کل_ever; /* total of added transfers, ever. */

  ساخت uint32_tbl xfers; /* transfers added to this multi */
  /* A transfer's mid may be present in at most one of
   * process/pending/msgsent. The dirty set may overlap with process. */
  ساخت uint32_bset process; /* transfer being processed */
  ساخت uint32_bset dirty; /* transfer to be run NOW, e.g. ASAP. */
  ساخت uint32_bset pending; /* transfers in waiting (conn limit etc.) */
  ساخت uint32_bset msgsent; /* transfers with unread application messages */

  ساخت Curl_mapi_ڈھیر callstack; /* multi api calls ongoing */

  /* expiration times for all attached easy handles */
  ساخت Curl_timeouts timeouts;

  ساخت Curl_easy *admin; /* internal easy handle for admin operations.
                              gets assigned `mid` 0 on multi init */

  /* callback function and user data pointer for the *socket() API */
  curl_socket_callback socket_cb;
  خالی *socket_userp;

  /* callback function and user data pointer for server push */
  curl_ڈالو_callback ڈالو_cb;
  خالی *ڈالو_userp;

  ساخت Curl_dnscache dnscache; /* DNS cache */
  ساخت Curl_ssl_scache *ssl_scache; /* TLS session pool */
#اگر_تعریف_شدہ USE_RESOLV_THREADED
  ساخت curl_thrdq *resolv_thrdq;
#اگر_ختم

#اگر_تعریف_شدہ USE_LIBPSL
  /* PSL cache. */
  ساخت PslCache psl;
#اگر_ختم

  ساخت Curl_fixed_buf *xfer_buf; /* transfer downloads, lazy */
  ساخت Curl_fixed_buf *xfer_ulbuf; /* transfer uploads, lazy */
  ساخت Curl_fixed_buf *xfer_sockbuf; /* transfer socket I/O, lazy */

  /* multi event related things */
  ساخت curl_multi_ev ev;
  /* multi notification related things */
  ساخت curl_multi_ntfy ntfy;

  /* `proto_hash` is a general key-value store for protocol implementations
   * with the lifetime of the multi handle. The number of elements kept here
   * should be in the order of supported protocols (and sub-protocols like
   * TLS), *not* in the order of connections or current transfers!
   * Elements need to be added with their own destructor to be invoked when
   * the multi handle is cleaned up (see Curl_hash_add2()).*/
  ساخت Curl_hash proto_hash;

  ساخت cshutdn cshutdn; /* connection shutdown handling */
  ساخت cpool cpool;     /* connection pool (bundles) */

  timediff_t آخری_expire_انحراف_us; /* times offset of last expiry */

  /* timer callback and user data pointer for the *socket() API */
  curl_multi_timer_callback timer_cb;
  خالی *timer_userp;

#اگر_تعریف_شدہ USE_WINSOCK
  WSAEVENT wsa_event; /* Winsock event used for waits */
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_WAKEUP
  curl_socket_t wakeup_pair[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used by curl_multi_wakeup() */
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  curl_socket_t wakeup_اندرونی[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used for internal wakeups,
                                   e.g. threaded resolver. */
#اگر_ختم
  uint32_t max_host_connections; /* if >0, a fixed limit of the maximum number
                                  of connections per host */
  uint32_t max_کل_connections; /* if >0, a fixed limit of the maximum number
                                   of connections in total */
  uint32_t maxconnects; /* if >0, a fixed limit of the maximum number of
                               entries we are allowed to grow the connection
                               cache to */
  uint32_t آخری_pending_mid; /* mid of last pending transfer rescheduled */
  uint32_t آخری_resolv_id; /* id of the last DNS resolve operation */
  ثنائی_ہندسہ(ipv6_چلتا);
  ثنائی_ہندسہ(multiplexing);           /* multiplexing wanted */
  ثنائی_ہندسہ(recheckstate);           /* see Curl_multi_connchanged */
#اگر_تعریف_شدہ USE_OPENSSL
  ثنائی_ہندسہ(ssl_seeded);
#اگر_ختم
  ثنائی_ہندسہ(dead); /* a callback returned error, everything needs to crash and
                burn */
  ثنائی_ہندسہ(آخری_timeout_سیٹ);      /* last timer callback value was nonnegative */
  ثنائی_ہندسہ(xfer_buf_borrowed);      /* xfer_buf is currently being borrowed */
  ثنائی_ہندسہ(xfer_ulbuf_borrowed);    /* xfer_ulbuf is currently being borrowed */
  ثنائی_ہندسہ(xfer_sockbuf_borrowed);  /* xfer_sockbuf is currently being borrowed */
  ثنائی_ہندسہ(quick_exit);             /* do not join threads on cleanup */
#اگر_تعریف_شدہ DEBUGBUILD
  ثنائی_ہندسہ(warned);                 /* true after user warned of DEBUGBUILD */
#اگر_ختم
};

#اگر_ختم /* HEADER_CURL_MULTIHANDLE_H */
