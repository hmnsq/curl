#ifndef HEADER_CURL_MULTIHANDLE_H
#define HEADER_CURL_MULTIHANDLE_H
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
#அந்தர்பாவ "api.h"
#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "conncache.h"
#அந்தர்பாவ "cshutdn.h"
#அந்தர்பாவ "multi_ev.h"
#அந்தர்பாவ "multi_ntfy.h"
#அந்தர்பாவ "psl.h"
#அந்தர்பாவ "socketpair.h"
#அந்தர்பாவ "splay.h"
#அந்தர்பாவ "uint-bset.h"
#அந்தர்பாவ "uint-spbset.h"
#அந்தர்பாவ "uint-table.h"
#அந்தர்பாவ "vdns/dnscache.h"

கட்டமைப்பு connectdata;
கட்டமைப்பு Curl_easy;

/* NOTE: if you add a state here, add the name to the statenames[] array
 * in curl_trc.c as well!
 */
வகைவரை எண்ணிக்கை {
  MSTATE_INIT,         /* 0 - start in this state */
  MSTATE_PENDING,      /* no connections, waiting for one */
  MSTATE_SETUP,        /* start a new transfer */
  MSTATE_CONNECT,      /* resolve/connect has been sent off */
  MSTATE_CONNECTING,   /* awaiting the TCP connect to finalize */
  MSTATE_PROTOCONNECT, /* initiate protocol connect procedure */
  MSTATE_PROTOCONNECTING, /* completing the protocol-specific connect phase */
  MSTATE_DO,           /* start send off the request (part 1) */
  MSTATE_DOING,        /* sending off the request (part 1) */
  MSTATE_DOING_MORE,   /* send off the request (part 2) */
  MSTATE_DID,          /* done sending off request */
  MSTATE_PERFORMING,   /* transfer data */
  MSTATE_RATELIMITING, /* wait because limit-rate exceeded */
  MSTATE_DONE,         /* post data transfer operation */
  MSTATE_COMPLETED,    /* operation complete */
  MSTATE_MSGSENT,      /* the operation complete message is sent */
  MSTATE_LAST          /* not a true state, never use this */
} CURLMstate;

#define CURLPIPE_ANY (CURLPIPE_MULTIPLEX)

#எனில் !defined(CURL_DISABLE_SOCKETPAIR) && !defined(USE_WINSOCK)
#define ENABLE_WAKEUP
#endif
#எனில் !defined(CURL_DISABLE_SOCKETPAIR) && \
    defined(USE_RESOLV_THREADED) && \
    !defined(USE_WINSOCK)
#define ENABLE_INTERNAL_WAKEUP
#endif

/* value for MAXIMUM CONCURRENT STREAMS upper limit */
#define INITIAL_MAX_CONCURRENT_STREAMS ((1U << 31) - 1)

கட்டமைப்பு Curl_fixed_buf {
  size_t len;      /* the allocated data length */
  எழுத்து data[1];   /* the actual buffer */
};

/* This is the struct known as CURLM on the outside */
கட்டமைப்பு Curl_multi {
  /* First a simple identifier to more easily detect if a user mixes up
     this multi handle with an easy handle.
     Set this to CURLMULTI_MAGIC_NUMBER. */
  uint32_t magic;
  uint32_t xfers_alive; /* amount of added transfers that have
                           not yet reached COMPLETE state */
  uint32_t xfers_really_alive; /* amount of added transfers that have
                                  passed INIT state but are not COMPLETE yet */
  uint32_t max_concurrent_streams;
  curl_off_t xfers_total_ever; /* total of added transfers, ever. */

  கட்டமைப்பு uint32_tbl xfers; /* transfers added to this multi */
  /* A transfer's mid may be present in at most one of
   * process/pending/msgsent. The dirty set may overlap with process. */
  கட்டமைப்பு uint32_bset process; /* transfer being processed */
  கட்டமைப்பு uint32_bset dirty; /* transfer to be run NOW, e.g. ASAP. */
  கட்டமைப்பு uint32_bset pending; /* transfers in waiting (conn limit etc.) */
  கட்டமைப்பு uint32_bset msgsent; /* transfers with unread application messages */

  கட்டமைப்பு Curl_mapi_அடுக்கு callstack; /* multi api calls ongoing */

  /* expiration times for all attached easy handles */
  கட்டமைப்பு Curl_timeouts timeouts;

  கட்டமைப்பு Curl_easy *admin; /* internal easy handle for admin operations.
                              gets assigned `mid` 0 on multi init */

  /* callback function and user data pointer for the *socket() API */
  curl_socket_callback socket_cb;
  வெறுமை *socket_userp;

  /* callback function and user data pointer for server push */
  curl_தள்ளு_callback தள்ளு_cb;
  வெறுமை *தள்ளு_userp;

  கட்டமைப்பு Curl_dnscache dnscache; /* DNS cache */
  கட்டமைப்பு Curl_ssl_scache *ssl_scache; /* TLS session pool */
#ifdef USE_RESOLV_THREADED
  கட்டமைப்பு curl_thrdq *resolv_thrdq;
#endif

#ifdef USE_LIBPSL
  /* PSL cache. */
  கட்டமைப்பு PslCache psl;
#endif

  கட்டமைப்பு Curl_fixed_buf *xfer_buf; /* transfer downloads, lazy */
  கட்டமைப்பு Curl_fixed_buf *xfer_ulbuf; /* transfer uploads, lazy */
  கட்டமைப்பு Curl_fixed_buf *xfer_sockbuf; /* transfer socket I/O, lazy */

  /* multi event related things */
  கட்டமைப்பு curl_multi_ev ev;
  /* multi notification related things */
  கட்டமைப்பு curl_multi_ntfy ntfy;

  /* `proto_hash` is a general key-value store for protocol implementations
   * with the lifetime of the multi handle. The number of elements kept here
   * should be in the order of supported protocols (and sub-protocols like
   * TLS), *not* in the order of connections or current transfers!
   * Elements need to be added with their own destructor to be invoked when
   * the multi handle is cleaned up (see Curl_hash_add2()).*/
  கட்டமைப்பு Curl_hash proto_hash;

  கட்டமைப்பு cshutdn cshutdn; /* connection shutdown handling */
  கட்டமைப்பு cpool cpool;     /* connection pool (bundles) */

  timediff_t last_expire_offset_us; /* times offset of last expiry */

  /* timer callback and user data pointer for the *socket() API */
  curl_multi_timer_callback timer_cb;
  வெறுமை *timer_userp;

#ifdef USE_WINSOCK
  WSAEVENT wsa_event; /* Winsock event used for waits */
#endif
#ifdef ENABLE_WAKEUP
  curl_socket_t wakeup_pair[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used by curl_multi_wakeup() */
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  curl_socket_t wakeup_internal[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used for internal wakeups,
                                   e.g. threaded resolver. */
#endif
  uint32_t max_host_connections; /* if >0, a fixed limit of the maximum number
                                  of connections per host */
  uint32_t max_total_connections; /* if >0, a fixed limit of the maximum number
                                   of connections in total */
  uint32_t maxconnects; /* if >0, a fixed limit of the maximum number of
                               entries we are allowed to grow the connection
                               cache to */
  uint32_t last_pending_mid; /* mid of last pending transfer rescheduled */
  uint32_t last_resolv_id; /* id of the last DNS resolve operation */
  BIT(ipv6_works);
  BIT(multiplexing);           /* multiplexing wanted */
  BIT(recheckstate);           /* see Curl_multi_connchanged */
#ifdef USE_OPENSSL
  BIT(ssl_seeded);
#endif
  BIT(dead); /* a callback returned error, everything needs to crash and
                burn */
  BIT(last_timeout_set);      /* last timer callback value was nonnegative */
  BIT(xfer_buf_borrowed);      /* xfer_buf is currently being borrowed */
  BIT(xfer_ulbuf_borrowed);    /* xfer_ulbuf is currently being borrowed */
  BIT(xfer_sockbuf_borrowed);  /* xfer_sockbuf is currently being borrowed */
  BIT(quick_exit);             /* do not join threads on cleanup */
#ifdef DEBUGBUILD
  BIT(warned);                 /* true after user warned of DEBUGBUILD */
#endif
};

#endif /* HEADER_CURL_MULTIHANDLE_H */
