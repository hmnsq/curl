#اگر_تعریف_نشده سرآیند_CURL_MULTIHANDLE_H
#تعریف_کن سرآیند_CURL_MULTIHANDLE_H
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
#ضمن "api.h"
#ضمن "llist.h"
#ضمن "hash.h"
#ضمن "conncache.h"
#ضمن "cshutdn.h"
#ضمن "multi_ev.h"
#ضمن "multi_ntfy.h"
#ضمن "psl.h"
#ضمن "socketpair.h"
#ضمن "splay.h"
#ضمن "uint-bset.h"
#ضمن "uint-spbset.h"
#ضمن "uint-table.h"
#ضمن "vdns/dnscache.h"

بنية connectdata;
بنية Curl_easy;

/* NOTE: if you add a state here, add the name to the statenames[] array
 * in curl_trc.c as well!
 */
تعريفنوع تعداد {
  MSTATE_INIT,         /* 0 - start in this state */
  MSTATE_PENDING,      /* no connections, waiting for one */
  MSTATE_راهاندازی,        /* start a new transfer */
  MSTATE_CONNECT,      /* resolve/connect has been sent off */
  MSTATE_CONNECTING,   /* awaiting the TCP connect to finalize */
  MSTATE_PROTOCONNECT, /* initiate protocol connect procedure */
  MSTATE_PROTOCONNECTING, /* completing the protocol-specific connect phase */
  MSTATE_DO,           /* start send off the request (part 1) */
  MSTATE_DOING,        /* sending off the request (part 1) */
  MSTATE_DOING_بیشتر,   /* send off the request (part 2) */
  MSTATE_DID,          /* done sending off request */
  MSTATE_PERFORMING,   /* transfer data */
  MSTATE_RATELIMITING, /* wait because limit-rate exceeded */
  MSTATE_انجامشده,         /* post data transfer operation */
  MSTATE_COMPLETED,    /* operation complete */
  MSTATE_MSGSENT,      /* the operation complete message is sent */
  MSTATE_آخرین          /* not a true state, never use this */
} CURLMstate;

#تعریف_کن CURLPIPE_ANY (CURLPIPE_MULTIPLEX)

#إذا !defined(CURL_DISABLE_SOCKETPAIR) && !defined(USE_WINSOCK)
#تعریف_کن ENABLE_WAKEUP
#پایان_اگر
#إذا !defined(CURL_DISABLE_SOCKETPAIR) && \
    defined(USE_RESOLV_THREADED) && \
    !defined(USE_WINSOCK)
#تعریف_کن ENABLE_داخلی_WAKEUP
#پایان_اگر

/* value for MAXIMUM CONCURRENT STREAMS upper limit */
#تعریف_کن INITIAL_MAX_CONCURRENT_STREAMS ((1U << 31) - 1)

بنية Curl_fixed_buf {
  size_t len;      /* the allocated data length */
  حرف داده[1];   /* the actual buffer */
};

/* This is the struct known as CURLM on the outside */
بنية Curl_multi {
  /* First a simple identifier to more easily detect if a user mixes up
     this multi handle with an easy handle.
     Set this to CURLMULTI_MAGIC_NUMBER. */
  uint32_t magic;
  uint32_t xfers_alive; /* amount of added transfers that have
                           not yet reached COMPLETE state */
  uint32_t xfers_really_alive; /* amount of added transfers that have
                                  passed INIT state but are not COMPLETE yet */
  uint32_t max_concurrent_streams;
  curl_off_t xfers_مجموع_ever; /* total of added transfers, ever. */

  بنية uint32_tbl xfers; /* transfers added to this multi */
  /* A transfer's mid may be present in at most one of
   * process/pending/msgsent. The dirty set may overlap with process. */
  بنية uint32_bset process; /* transfer being processed */
  بنية uint32_bset dirty; /* transfer to be run NOW, e.g. ASAP. */
  بنية uint32_bset pending; /* transfers in waiting (conn limit etc.) */
  بنية uint32_bset msgsent; /* transfers with unread application messages */

  بنية Curl_mapi_پشته callstack; /* multi api calls ongoing */

  /* expiration times for all attached easy handles */
  بنية Curl_timeouts timeouts;

  بنية Curl_easy *admin; /* internal easy handle for admin operations.
                              gets assigned `mid` 0 on multi init */

  /* callback function and user data pointer for the *socket() API */
  curl_socket_callback socket_cb;
  فراغ *socket_userp;

  /* callback function and user data pointer for server push */
  curl_افزودن_callback افزودن_cb;
  فراغ *افزودن_userp;

  بنية Curl_dnscache dnscache; /* DNS cache */
  بنية Curl_ssl_scache *ssl_scache; /* TLS session pool */
#اگر_تعریف_شده USE_RESOLV_THREADED
  بنية curl_thrdq *resolv_thrdq;
#پایان_اگر

#اگر_تعریف_شده USE_LIBPSL
  /* PSL cache. */
  بنية PslCache psl;
#پایان_اگر

  بنية Curl_fixed_buf *xfer_buf; /* transfer downloads, lazy */
  بنية Curl_fixed_buf *xfer_ulbuf; /* transfer uploads, lazy */
  بنية Curl_fixed_buf *xfer_sockbuf; /* transfer socket I/O, lazy */

  /* multi event related things */
  بنية curl_multi_ev ev;
  /* multi notification related things */
  بنية curl_multi_ntfy ntfy;

  /* `proto_hash` is a general key-value store for protocol implementations
   * with the lifetime of the multi handle. The number of elements kept here
   * should be in the order of supported protocols (and sub-protocols like
   * TLS), *not* in the order of connections or current transfers!
   * Elements need to be added with their own destructor to be invoked when
   * the multi handle is cleaned up (see Curl_hash_add2()).*/
  بنية Curl_hash proto_hash;

  بنية cshutdn cshutdn; /* connection shutdown handling */
  بنية cpool cpool;     /* connection pool (bundles) */

  timediff_t آخرین_expire_جابه_جایی_us; /* times offset of last expiry */

  /* timer callback and user data pointer for the *socket() API */
  curl_multi_timer_callback timer_cb;
  فراغ *timer_userp;

#اگر_تعریف_شده USE_WINSOCK
  WSAEVENT wsa_event; /* Winsock event used for waits */
#پایان_اگر
#اگر_تعریف_شده ENABLE_WAKEUP
  curl_socket_t wakeup_pair[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used by curl_multi_wakeup() */
#پایان_اگر
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  curl_socket_t wakeup_داخلی[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used for internal wakeups,
                                   e.g. threaded resolver. */
#پایان_اگر
  uint32_t max_host_connections; /* if >0, a fixed limit of the maximum number
                                  of connections per host */
  uint32_t max_مجموع_connections; /* if >0, a fixed limit of the maximum number
                                   of connections in total */
  uint32_t maxconnects; /* if >0, a fixed limit of the maximum number of
                               entries we are allowed to grow the connection
                               cache to */
  uint32_t آخرین_pending_mid; /* mid of last pending transfer rescheduled */
  uint32_t آخرین_resolv_id; /* id of the last DNS resolve operation */
  بیت(ipv6_کار_میکند);
  بیت(multiplexing);           /* multiplexing wanted */
  بیت(recheckstate);           /* see Curl_multi_connchanged */
#اگر_تعریف_شده USE_OPENSSL
  بیت(ssl_seeded);
#پایان_اگر
  بیت(dead); /* a callback returned error, everything needs to crash and
                burn */
  بیت(آخرین_timeout_مجموعه);      /* last timer callback value was nonnegative */
  بیت(xfer_buf_borrowed);      /* xfer_buf is currently being borrowed */
  بیت(xfer_ulbuf_borrowed);    /* xfer_ulbuf is currently being borrowed */
  بیت(xfer_sockbuf_borrowed);  /* xfer_sockbuf is currently being borrowed */
  بیت(quick_exit);             /* do not join threads on cleanup */
#اگر_تعریف_شده DEBUGBUILD
  بیت(warned);                 /* true after user warned of DEBUGBUILD */
#پایان_اگر
};

#پایان_اگر /* HEADER_CURL_MULTIHANDLE_H */
