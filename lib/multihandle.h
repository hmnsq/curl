#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MULTIHANDLE_H
#నిర్వచించు శీర్షిక_CURL_MULTIHANDLE_H
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
#అన్తర్భావ "api.h"
#అన్తర్భావ "llist.h"
#అన్తర్భావ "hash.h"
#అన్తర్భావ "conncache.h"
#అన్తర్భావ "cshutdn.h"
#అన్తర్భావ "multi_ev.h"
#అన్తర్భావ "multi_ntfy.h"
#అన్తర్భావ "psl.h"
#అన్తర్భావ "socketpair.h"
#అన్తర్భావ "splay.h"
#అన్తర్భావ "uint-bset.h"
#అన్తర్భావ "uint-spbset.h"
#అన్తర్భావ "uint-table.h"
#అన్తర్భావ "vdns/dnscache.h"

నిర్మాణం connectdata;
నిర్మాణం Curl_easy;

/* NOTE: if you add a state here, add the name to the statenames[] array
 * in curl_trc.c as well!
 */
రకనిర్వచనం గణన {
  MSTATE_INIT,         /* 0 - start in this state */
  MSTATE_PENDING,      /* no connections, waiting for one */
  MSTATE_అమరిక,        /* start a new transfer */
  MSTATE_CONNECT,      /* resolve/connect has been sent off */
  MSTATE_CONNECTING,   /* awaiting the TCP connect to finalize */
  MSTATE_PROTOCONNECT, /* initiate protocol connect procedure */
  MSTATE_PROTOCONNECTING, /* completing the protocol-specific connect phase */
  MSTATE_DO,           /* start send off the request (part 1) */
  MSTATE_DOING,        /* sending off the request (part 1) */
  MSTATE_DOING_మరిన్ని,   /* send off the request (part 2) */
  MSTATE_DID,          /* done sending off request */
  MSTATE_PERFORMING,   /* transfer data */
  MSTATE_RATELIMITING, /* wait because limit-rate exceeded */
  MSTATE_పూర్తి,         /* post data transfer operation */
  MSTATE_COMPLETED,    /* operation complete */
  MSTATE_MSGSENT,      /* the operation complete message is sent */
  MSTATE_చివరి          /* not a true state, never use this */
} CURLMstate;

#నిర్వచించు CURLPIPE_ANY (CURLPIPE_MULTIPLEX)

#ఒకవేళ !defined(CURL_DISABLE_SOCKETPAIR) && !defined(USE_WINSOCK)
#నిర్వచించు ENABLE_WAKEUP
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_SOCKETPAIR) && \
    defined(USE_RESOLV_THREADED) && \
    !defined(USE_WINSOCK)
#నిర్వచించు ENABLE_అంతర్గత_WAKEUP
#అయితే_ముగింపు

/* value for MAXIMUM CONCURRENT STREAMS upper limit */
#నిర్వచించు INITIAL_MAX_CONCURRENT_STREAMS ((1U << 31) - 1)

నిర్మాణం Curl_fixed_buf {
  size_t len;      /* the allocated data length */
  అక్షరం దత్తాంశం[1];   /* the actual buffer */
};

/* This is the struct known as CURLM on the outside */
నిర్మాణం Curl_multi {
  /* First a simple identifier to more easily detect if a user mixes up
     this multi handle with an easy handle.
     Set this to CURLMULTI_MAGIC_NUMBER. */
  uint32_t magic;
  uint32_t xfers_alive; /* amount of added transfers that have
                           not yet reached COMPLETE state */
  uint32_t xfers_really_alive; /* amount of added transfers that have
                                  passed INIT state but are not COMPLETE yet */
  uint32_t max_concurrent_streams;
  curl_off_t xfers_మొత్తం_ever; /* total of added transfers, ever. */

  నిర్మాణం uint32_tbl xfers; /* transfers added to this multi */
  /* A transfer's mid may be present in at most one of
   * process/pending/msgsent. The dirty set may overlap with process. */
  నిర్మాణం uint32_bset process; /* transfer being processed */
  నిర్మాణం uint32_bset dirty; /* transfer to be run NOW, e.g. ASAP. */
  నిర్మాణం uint32_bset pending; /* transfers in waiting (conn limit etc.) */
  నిర్మాణం uint32_bset msgsent; /* transfers with unread application messages */

  నిర్మాణం Curl_mapi_దొంతర callstack; /* multi api calls ongoing */

  /* expiration times for all attached easy handles */
  నిర్మాణం Curl_timeouts timeouts;

  నిర్మాణం Curl_easy *admin; /* internal easy handle for admin operations.
                              gets assigned `mid` 0 on multi init */

  /* callback function and user data pointer for the *socket() API */
  curl_socket_callback socket_cb;
  శూన్య *socket_userp;

  /* callback function and user data pointer for server push */
  curl_నెట్టు_callback నెట్టు_cb;
  శూన్య *నెట్టు_userp;

  నిర్మాణం Curl_dnscache dnscache; /* DNS cache */
  నిర్మాణం Curl_ssl_scache *ssl_scache; /* TLS session pool */
#నిర్వచితమైతే USE_RESOLV_THREADED
  నిర్మాణం curl_thrdq *resolv_thrdq;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIBPSL
  /* PSL cache. */
  నిర్మాణం PslCache psl;
#అయితే_ముగింపు

  నిర్మాణం Curl_fixed_buf *xfer_buf; /* transfer downloads, lazy */
  నిర్మాణం Curl_fixed_buf *xfer_ulbuf; /* transfer uploads, lazy */
  నిర్మాణం Curl_fixed_buf *xfer_sockbuf; /* transfer socket I/O, lazy */

  /* multi event related things */
  నిర్మాణం curl_multi_ev ev;
  /* multi notification related things */
  నిర్మాణం curl_multi_ntfy ntfy;

  /* `proto_hash` is a general key-value store for protocol implementations
   * with the lifetime of the multi handle. The number of elements kept here
   * should be in the order of supported protocols (and sub-protocols like
   * TLS), *not* in the order of connections or current transfers!
   * Elements need to be added with their own destructor to be invoked when
   * the multi handle is cleaned up (see Curl_hash_add2()).*/
  నిర్మాణం Curl_hash proto_hash;

  నిర్మాణం cshutdn cshutdn; /* connection shutdown handling */
  నిర్మాణం cpool cpool;     /* connection pool (bundles) */

  timediff_t చివరి_expire_స్థానభ్రంశం_us; /* times offset of last expiry */

  /* timer callback and user data pointer for the *socket() API */
  curl_multi_timer_callback timer_cb;
  శూన్య *timer_userp;

#నిర్వచితమైతే USE_WINSOCK
  WSAEVENT wsa_event; /* Winsock event used for waits */
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_WAKEUP
  curl_socket_t wakeup_pair[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used by curl_multi_wakeup() */
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  curl_socket_t wakeup_అంతర్గత[2]; /* eventfd()/pipe()/socketpair() used for
                                   wakeup 0 is used for read, 1 is used
                                   for write. Used for internal wakeups,
                                   e.g. threaded resolver. */
#అయితే_ముగింపు
  uint32_t max_host_connections; /* if >0, a fixed limit of the maximum number
                                  of connections per host */
  uint32_t max_మొత్తం_connections; /* if >0, a fixed limit of the maximum number
                                   of connections in total */
  uint32_t maxconnects; /* if >0, a fixed limit of the maximum number of
                               entries we are allowed to grow the connection
                               cache to */
  uint32_t చివరి_pending_mid; /* mid of last pending transfer rescheduled */
  uint32_t చివరి_resolv_id; /* id of the last DNS resolve operation */
  బిట్టు(ipv6_పనిచేస్తుంది);
  బిట్టు(multiplexing);           /* multiplexing wanted */
  బిట్టు(recheckstate);           /* see Curl_multi_connchanged */
#నిర్వచితమైతే USE_OPENSSL
  బిట్టు(ssl_seeded);
#అయితే_ముగింపు
  బిట్టు(dead); /* a callback returned error, everything needs to crash and
                burn */
  బిట్టు(చివరి_timeout_సమితి);      /* last timer callback value was nonnegative */
  బిట్టు(xfer_buf_borrowed);      /* xfer_buf is currently being borrowed */
  బిట్టు(xfer_ulbuf_borrowed);    /* xfer_ulbuf is currently being borrowed */
  బిట్టు(xfer_sockbuf_borrowed);  /* xfer_sockbuf is currently being borrowed */
  బిట్టు(quick_exit);             /* do not join threads on cleanup */
#నిర్వచితమైతే DEBUGBUILD
  బిట్టు(warned);                 /* true after user warned of DEBUGBUILD */
#అయితే_ముగింపు
};

#అయితే_ముగింపు /* HEADER_CURL_MULTIHANDLE_H */
