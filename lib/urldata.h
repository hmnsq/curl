#నిర్వచితం_కాకపోతే శీర్షిక_CURL_URLDATA_H
#నిర్వచించు శీర్షిక_CURL_URLDATA_H
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
/* This file is for lib internal stuff */
#అన్తర్భావ "curl_setup.h"

#నిర్వచించు CURL_DEFAULT_USER     "anonymous"
#నిర్వచించు CURL_DEFAULT_PASSWORD "ftp@example.com"

#ఒకవేళ !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#నిర్వచించు CURL_PREFER_LF_LINEENDS
#అయితే_ముగింపు

#నిర్వచించు DEFAULT_CONNCACHE_కొలత 5

/* length of longest IPv6 address string including the null-terminator */
#నిర్వచించు MAX_IPADR_LEN పరిమాణం("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#నిర్వచించు CURL_MAX_ఆదానం_నిడివి 8000000

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IN6_H
#అన్తర్భావ <netinet/in6.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/timeval.h"

#అన్తర్భావ "api.h"
#అన్తర్భావ "cookie.h"
#అన్తర్భావ "creds.h"
#అన్తర్భావ "psl.h"
#అన్తర్భావ "formdata.h"
#అన్తర్భావ "http_chunks.h" /* for the structs and enum stuff */
#అన్తర్భావ "hash.h"
#అన్తర్భావ "peer.h"
#అన్తర్భావ "proxy.h"
#అన్తర్భావ "splay.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "dynhds.h"
#అన్తర్భావ "request.h"
#అన్తర్భావ "ratelimit.h"
#అన్తర్భావ "netrc.h"
#అన్తర్భావ "u8_strset.h"
#అన్తర్భావ "vdns/asyn.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vtls/vtls_config.h"

/* On error return, the value of `pnwritten` has no meaning */
రకనిర్వచనం CURLcode (Curl_send)(నిర్మాణం Curl_easy *దత్తాంశం,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             స్థిరరాశి uint8_t *buf,       /* data to write */
                             size_t len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             size_t *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
రకనిర్వచనం CURLcode (Curl_recv)(నిర్మాణం Curl_easy *దత్తాంశం,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             అక్షరం *buf,                /* store data here */
                             size_t len,               /* max amount to read */
                             size_t *pnread);          /* how much received */

#అన్తర్భావ "mime.h"
#అన్తర్భావ "protocol.h"
#అన్తర్భావ "ftp.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "mqtt.h"
#అన్తర్భావ "ftplistparser.h"
#అన్తర్భావ "multihandle.h"
#అన్తర్భావ "cf-socket.h"

#నిర్వచితమైతే HAVE_GSSAPI
#  ifdef HAVE_GSSAPPLE
#    include <GSS/gssapi.h>
#    include <GSS/gssapi_oid.h>
#  elif defined(HAVE_GSSGNU)
#    include <gss.h>
#  elif defined(HAVE_GSSAPI_H)
#    include <gssapi.h>
#  లేకపోతే /* MIT Kerberos */
#    include <gssapi/gssapi.h>
#    include <gssapi/gssapi_krb5.h> /* for GSS_C_CHANNEL_BOUND_FLAG in 1.19+ */
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIBSSH2
#అన్తర్భావ <libssh2.h>
#అన్తర్భావ <libssh2_sftp.h>
#అయితే_ముగింపు /* USE_LIBSSH2 */

#నిర్వచించు READBUFFER_కొలత CURL_MAX_WRITE_కొలత
#నిర్వచించు READBUFFER_MAX  CURL_MAX_READ_కొలత
#నిర్వచించు READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#నిర్వచించు UPLOADBUFFER_DEFAULT 65536
#నిర్వచించు UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#నిర్వచించు UPLOADBUFFER_MIN     CURL_MAX_WRITE_కొలత

#నిర్వచితమైతే USE_WINDOWS_SSPI
#అన్తర్భావ "curl_sspi.h"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
నిర్మాణం digestdata {
  నిర్మాణం Curl_creds *creds;
  నిర్మాణం Curl_peer *origin;
#నిర్వచితమైతే USE_WINDOWS_SSPI
  BYTE *ఆదానం_token;
  size_t ఆదానం_token_len;
  CtxtHandle *http_context;
#లేకపోతే
  అక్షరం *nonce;
  అక్షరం *cnonce;
  అక్షరం *realm;
  అక్షరం *opaque;
  అక్షరం *qop;
  అక్షరం *algorithm;
  పూర్ణాంకం nc; /* nonce count */
  uint8_t algo;
  బిట్టు(stale); /* set true for re-negotiation */
  బిట్టు(userhash);
#అయితే_ముగింపు
};
#అయితే_ముగింపు

రకనిర్వచనం గణన {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_చివరి
} curlntlm;

#నిర్వచించు CURL_NEGO_AUTH_NONE   0U
#నిర్వచించు CURL_NEGO_AUTH_RECV   1U
#నిర్వచించు CURL_NEGO_AUTH_పంపిన   2U
#నిర్వచించు CURL_NEGO_AUTH_పూర్తి   3U
#నిర్వచించు CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
నిర్మాణం Connect_బిట్లు {
  బిట్టు(connect_only);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  బిట్టు(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  బిట్టు(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#అయితే_ముగింపు
  /* always modify bits.close with the connclose() and connkeep() macros! */
  బిట్టు(close); /* if set, we close the connection after this request */
  బిట్టు(reuse); /* if set, this is a reused connection */
  బిట్టు(altused); /* this is an alt-svc "redirect" */
  బిట్టు(ipv6);    /* we communicate with a site using an IPv6 address */
  బిట్టు(do_మరిన్ని); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  బిట్టు(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  బిట్టు(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  బిట్టు(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
అభ్యర్థనలు*/
  బిట్టు(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
అభ్యర్థనలు*/
  బిట్టు(ftp_use_దత్తాంశం_ssl); /* Enabled SSL for the data connection */
#అయితే_ముగింపు
  బిట్టు(bound); /* set true if bind() has already been done on this socket/
                 connection */
  బిట్టు(upgrade_లోపల_పురోగతి); /* protocol upgrade is in progress */
  బిట్టు(multiplex); /* connection is multiplexed */
  బిట్టు(tcp_fastopen); /* use TCP Fast Open */
  బిట్టు(tls_enable_alpn); /* TLS ALPN extension? */
  బిట్టు(sock_accepted); /* TRUE if the SECONDARYSOCKET was created with
                         accept() */
  బిట్టు(parallel_connect); /* set TRUE when a parallel connect attempt has
                            started (happy eyeballs) */
  బిట్టు(aborted); /* connection was aborted, e.g. in unclean state */
  బిట్టు(no_reuse); /* connection should not be reused */
  బిట్టు(shutdown_handler); /* connection shutdown: handler shut down */
  బిట్టు(shutdown_filters); /* connection shutdown: filters shut down */
  బిట్టు(dns_resolved); /* DNS records for connection were resolved */
};

నిర్మాణం hostname {
  అక్షరం *rawalloc; /* allocated "raw" version of the name */
  అక్షరం *encalloc; /* allocated IDN-encoded version of the name */
  అక్షరం *name;     /* name to use internally, might be encoded, might be raw */
  స్థిరరాశి అక్షరం *dispname; /* name to display, as 'name' might be encoded */
};

#నిర్వచించు FIRSTSOCKET     0
#నిర్వచించు SECONDARYSOCKET 1

#నిర్వచించు TRNSPRT_NONE 0
#నిర్వచించు TRNSPRT_TCP  3
#నిర్వచించు TRNSPRT_UDP  4
#నిర్వచించు TRNSPRT_QUIC 5
#నిర్వచించు TRNSPRT_UNIX 6

#నిర్వచించు TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

నిర్మాణం ip_quadruple {
  అక్షరం remote_ip[MAX_IPADR_LEN];
  అక్షరం local_ip[MAX_IPADR_LEN];
  uint16_t remote_పోర్టు;
  uint16_t local_పోర్టు;
  uint8_t transport;
};

#నిర్వచించు CUR_IP_QUAD_HAS_PORTS(x)      \
  (((x)->transport == TRNSPRT_TCP) || \
   ((x)->transport == TRNSPRT_UDP) || \
   ((x)->transport == TRNSPRT_QUIC))

/*
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
నిర్మాణం connectdata {
  curl_off_t connection_id; /* Contains a unique number to make it easier to
                               track the connections in the log output */

  /* A connection cache from a SHARE might be used in several multi handles.
   * We MUST not reuse connections that are running in another multi,
   * for concurrency reasons. That multi might run in another thread.
   * `attached_multi` is set by the first transfer attached and cleared
   * when the last one is detached.
   * NEVER call anything on this multi, check for equality. */
  నిర్మాణం Curl_multi *attached_multi;

  /* Who the connection is talking to, ultimately */
  నిర్మాణం Curl_peer *origin; /* connection ultimately talks to this */
  నిర్మాణం Curl_peer *ద్వారా_peer; /* if set, connection really talks to this */
  నిర్మాణం Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  నిర్మాణం Curl_peer *ద్వారా_peer2; /* peer of SECONDARYSOCKET */
  నిర్మాణం Curl_creds *creds; /* When connection itself is tied to credentials */
  నిర్మాణం Curl_peer *creds_origin; /* origin tied credentials are for */
  స్థిరరాశి నిర్మాణం Curl_scheme *scheme; /* Connection's real protocol handler */

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the connection.
   * Elements need to be added with their own destructor to be invoked when
   * the connection is cleaned up (see Curl_hash_add2()).*/
  నిర్మాణం Curl_hash meta_hash;

  అక్షరం *destination; /* hostname+port, used in conncache */

  నిర్మాణం curltime created; /* creation time */
  /* timediff_t are deltas from `created`*/
  timediff_t lastused_ms; /* when returned to the connection pool as idle */
  timediff_t lastchecked_ms; /* when last checked alive status */
  timediff_t lastupkeep_ms; /* when last done conn_upkeep */
  నిర్మాణం {
    నిర్మాణం curltime at[2]; /* first Curl_conn_connect() for each socket */
    bool సమితి[2];
  } connect_started;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  నిర్మాణం proxy_info socks_proxy;
  నిర్మాణం proxy_info http_proxy;
#అయితే_ముగింపు

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  నిర్మాణం Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#నిర్వచించు CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  నిర్మాణం {
    timediff_t ప్రారంభం_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  శూన్య *closesocket_client;

  నిర్మాణం ssl_filter_config ssl_config;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  నిర్మాణం ssl_filter_config proxy_ssl_config;
#అయితే_ముగింపు
  అక్షరం *options; /* options string, allocated */

  /*************** Request - specific items ************/
#నిర్వచితమైతే USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#అయితే_ముగింపు

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  అక్షరం *interface_opts;
  నిర్మాణం Connect_బిట్లు బిట్లు;    /* various state-flags for this connection */
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  పూర్ణాంకం socks5_gssapi_enctype;
#అయితే_ముగింపు

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#నిర్వచించు CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#నిర్వచితమైతే USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#అయితే_ముగింపు
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_సంస్కరణ; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_చూసిన;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#నిర్వచితమైతే USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#అయితే_ముగింపు
};

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
#నిర్వచించు CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->ద్వారా_peer ? (c)->ద్వారా_peer->user_hostname : \
        (c)->origin->user_hostname)
#లేకపోతే
#నిర్వచించు CURL_CONN_HOST_DISPNAME(c) \
  ((c)->ద్వారా_peer ? (c)->ద్వారా_peer->user_hostname : (c)->origin->user_hostname)
#అయితే_ముగింపు

/* The end of connectdata. */

/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
నిర్మాణం శుద్ధ_Info {
  /* PureInfo primary ip_quadruple is copied over from the connectdata
     struct in order to allow curl_easy_getinfo() to return this information
     even when the session handle is no longer associated with a connection,
     and also allow curl_easy_reset() to clear this information from the
     session handle without disturbing information which is still alive, and
     that might be reused, in the connection pool. */
  నిర్మాణం ip_quadruple primary;
  నిర్మాణం curl_certinfo certs; /* info about the certs. Asked for with
                                 CURLOPT_CERTINFO / CURLINFO_CERTINFO */
  time_t filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t request_కొలత; /* the amount of bytes sent in the request(s) */
  curl_off_t numconnects; /* how many new connections libcurl created */
  అక్షరం *contenttype; /* the content type of the object */
  అక్షరం *wouldredirect; /* URL this would have been redirected to if asked to */
  అక్షరం *effective_url; /* when needed */
  curl_off_t retry_after; /* info from Retry-After: header */
  స్థిరరాశి అక్షరం *conn_scheme;
  పూర్ణాంకం httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  పూర్ణాంకం httpproxycode; /* response code from proxy when received separate */
  పూర్ణాంకం httpversion; /* the http version number X.Y = X*10+Y */
  uint32_t conn_protocol;
  uint32_t proxyauthavail; /* what proxy auth types were announced */
  uint32_t httpauthavail;  /* what host auth types were announced */
  uint32_t proxyauthpicked; /* selected proxy auth type */
  uint32_t httpauthpicked;  /* selected host auth type */
  uint32_t శీర్షిక_కొలత;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  బిట్టు(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  బిట్టు(వాడిన_proxy); /* the transfer used a proxy */
};

నిర్మాణం pgrs_dir {
  curl_off_t మొత్తం_కొలత; /* total expected bytes */
  curl_off_t cur_కొలత; /* transferred bytes so far */
  curl_off_t వేగం; /* bytes per second transferred */
  నిర్మాణం Curl_rlimit rlimit; /* speed limiting / pausing */
};

నిర్మాణం పురోగతి_2 {
  నిర్మాణం curltime now; /* current time of processing */
  నిర్మాణం curltime ప్రారంభం; /* when transfer was initialized, set once */

  నిర్మాణం pgrs_dir ul;
  నిర్మాణం pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_వేగం; /* uses the currently fastest transfer */
  curl_off_t earlydata_పంపిన;

  నిర్మాణం {
    timediff_t startop_us; /* since start when operations started */
    timediff_t startsingle_us; /* since start when last request started */
    timediff_t startqueue_us; /* since start when last entered queueing */
    timediff_t startredirect_us; /* since start when last redirected */
    timediff_t lastshow_us; /* since start when last progress shown */
  } delta;
  నిర్మాణం {
    timediff_t spent_us; /* all time spent since start */
    timediff_t queued_us; /* time spent since startsingle's for queueing */
    timediff_t nslookup_us; /* same for name resolves */
    timediff_t connect_us; /* same for connects */
    timediff_t appconnect_us; /* same for application connects, e.g. TLS */
    timediff_t pretransfer_us; /* same until requests were sent */
    timediff_t starttransfer_us; /* same until responses started */
    timediff_t posttransfer_us; /* same until responses ended */
  } మొత్తం;

#నిర్వచించు CURL_వేగం_రికార్డులు (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t వేగం_amount[CURL_వేగం_రికార్డులు];
  timediff_t వేగం_సమయం[CURL_వేగం_రికార్డులు];
  uint32_t speeder_c;
  బిట్టు(hide);
  బిట్టు(ul_కొలత_తెలిసిన);
  బిట్టు(dl_కొలత_తెలిసిన);
  బిట్టు(శీర్షికలు_బహిర్గతం); /* when the headers have been written */
  బిట్టు(callback);  /* set when progress callback is used */
  బిట్టు(startransfer_added);
};

నిర్మాణం auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  బిట్టు(పూర్తి);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  బిట్టు(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#నిర్వచితమైతే USE_NGHTTP2
నిర్మాణం Curl_దత్తాంశం_prio_కణుపు {
  నిర్మాణం Curl_దత్తాంశం_prio_కణుపు *తదుపరి;
  నిర్మాణం Curl_easy *దత్తాంశం;
};
#అయితే_ముగింపు

/* Timers */
రకనిర్వచనం గణన {
  EXPIRE_100_TIMEOUT,
  EXPIRE_ASYNC_NAME,
  EXPIRE_CONNECTTIMEOUT,
  EXPIRE_HAPPY_EYEBALLS,
  EXPIRE_MULTI_PENDING,
  EXPIRE_SPEEDCHECK,
  EXPIRE_TIMEOUT,
  EXPIRE_TOOFAST,
  EXPIRE_QUIC,
  EXPIRE_FTP_ACCEPT,
  EXPIRE_ALPN_EYEBALLS,
  EXPIRE_SHUTDOWN,
  EXPIRE_SERVER_RESPONSE,
  EXPIRE_చివరి /* not an actual timer, used as a marker only */
} expire_id;

నిర్మాణం expire_timers {
  నిర్మాణం Curl_వృక్షం splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t స్థానభ్రంశం_us[EXPIRE_చివరి];
  uint8_t తదుపరి[EXPIRE_చివరి];
  uint8_t మొదటి;
};

/* individual pieces of the URL */
నిర్మాణం urlpieces {
  అక్షరం *options;
  అక్షరం *మార్గం;
  అక్షరం *query;
};

నిర్మాణం UrlState {
  /* Origin of the initial (e.g. not followed) request of a transfer.
     Credentials from CURLOPT_* are only valid for this origin.
     Always set once a transfer starts searching for connections. */
  నిర్మాణం Curl_peer *initial_origin;
  /* Current origin of the transfer, changes to origin of follow
   * requests. */
  నిర్మాణం Curl_peer *origin;

  నిర్మాణం curltime keeps_వేగం; /* for the progress meter really */
  నిర్మాణం dynbuf headerb; /* buffer to store headers in */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  నిర్మాణం curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#అయితే_ముగింపు

  పూర్ణాంకం os_errno;  /* filled in with errno whenever an error occurs */
  పూర్ణాంకం అభ్యర్థనలు; /* request counter: redirects + authentication retakes */
#నిర్వచితమైతే HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  శూన్య (*prev_signal)(పూర్ణాంకం sig);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
  అక్షరం *envproxy; /* last proxy string used for proxy-related state */
  నిర్మాణం digestdata digest;      /* state data for host Digest auth */
  నిర్మాణం digestdata proxydigest; /* state data for proxy Digest auth */
#అయితే_ముగింపు
  నిర్మాణం auth authhost;  /* auth details for host */
  నిర్మాణం auth authproxy; /* auth details for proxy */

#నిర్వచితమైతే USE_CURL_ASYNC
  నిర్మాణం Curl_resolv_async *async;  /* asynchronous name resolver data */
#అయితే_ముగింపు

  నిర్మాణం expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  అక్షరం *most_recent_ftp_entrypath;
  అక్షరం *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  శూన్య *లోపల;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  నిర్మాణం urlpieces పైకి;
  నిర్మాణం bufref url;        /* work URL, initially copied from UserDefined */
  నిర్మాణం bufref referer;    /* referer string */
  నిర్మాణం curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#నిర్వచితం_కాకపోతే CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#అయితే_ముగింపు
  నిర్మాణం Curl_llist httphdrs; /* received headers */
  నిర్మాణం curl_శీర్షిక headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  గుర్తులేని పూర్ణాంకం nh_origin;
  పూర్ణాంకం nh_request;
  size_t nh_లెక్క; /* httphdrs count when the cache was filled */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  నిర్మాణం curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#అయితే_ముగింపు

#నిర్వచితమైతే CURLVERBOSE
  నిర్మాణం curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
  నిర్మాణం store_netrc netrc;
#అయితే_ముగింపు

  నిర్మాణం Curl_creds *creds; /* Credentials for the origin only */

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  అక్షరం *rangeline; /* allocated */
  అక్షరం *http_host; /* allocated */
  నిర్మాణం http_negotiation http_neg;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  నిర్మాణం Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_ముసుగు[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_తదుపరి_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_తదుపరి_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#అయితే_ముగింపు

  curl_off_t చివరి_conn_id; /* The last assigned connection or -1 */
  uint32_t చివరి_cpid; /* last connection pool id used */

#ఒకవేళ defined(USE_HTTP2) || defined(USE_HTTP3)
  పూర్ణాంకం బరువు; /* shallow copy of data->set */
#అయితే_ముగింపు
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  బిట్టు(really_alive); /* transfer is really alive in multi, passed INIT */
  బిట్టు(this_is_a_follow); /* this is a followed Location: request */
  బిట్టు(refused_ప్రవాహం); /* this was refused, try again */
  బిట్టు(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  బిట్టు(allow_పోర్టు); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  బిట్టు(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  బిట్టు(wildcardmatch); /* enable wildcard matching */
  బిట్టు(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  బిట్టు(use_range);
  బిట్టు(rangestringalloc); /* the range string is malloc()'ed */
  బిట్టు(పూర్తి); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  బిట్టు(cookie_engine);
#అయితే_ముగింపు
  బిట్టు(prefer_ascii);   /* ASCII rather than binary */
#నిర్వచితమైతే CURL_LIST_ONLY_PROTOCOL
  బిట్టు(list_only);      /* list directory contents */
#అయితే_ముగింపు
  బిట్టు(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  బిట్టు(upload);         /* upload request */
  బిట్టు(అంతర్గత); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  బిట్టు(http_ignorecustom); /* ignore custom method from now */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  బిట్టు(http_hd_te); /* Added HTTP header TE: */
  బిట్టు(http_hd_upgrade); /* Added HTTP header Upgrade: */
  బిట్టు(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  బిట్టు(maybe_folded);
  బిట్టు(leading_unfold); /* unfold started, this is the leading bytes */
#అయితే_ముగింపు
};

/*
 * This 'UserDefined' struct must only contain data that is set once to go
 * for many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" MUST be defined within the
 * 'struct UrlState' instead. The only exceptions MUST note the changes in
 * the 'DynamicStatic' struct.
 * Character pointer fields point to dynamic storage, unless otherwise stated.
 */

నిర్మాణం Curl_multi;    /* declared in multihandle.c */

గణన dupstring {
  STRING_CERT,            /* client certificate filename */
  STRING_CERT_TYPE,       /* format for certificate (default: PEM)*/
  STRING_కీలకం,             /* private key filename */
  STRING_కీలకం_PASSWD,      /* plain text private key password */
  STRING_కీలకం_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_కీలకం_PROXY,       /* private key filename */
  STRING_కీలకం_PASSWD_PROXY, /* plain text private key password */
  STRING_కీలకం_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#అయితే_ముగింపు
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_గమ్యం_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#అయితే_ముగింపు
  STRING_సమితి_RANGE,       /* range, if used */
  STRING_సమితి_REFERER,     /* custom string for the HTTP referer field */
  STRING_సమితి_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
వాడిన*/
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_ప్రవాహం_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  STRING_SSH_వ్యక్తిగత_కీలకం, /* path to the private key file for auth */
  STRING_SSH_ప్రజా_కీలకం,  /* path to the public key file for auth */
  STRING_SSH_HOST_ప్రజా_కీలకం_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_ప్రజా_కీలకం_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#అయితే_ముగింపు
  STRING_BEARER,                /* <bearer>, if used */
#నిర్వచితమైతే USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_మార్గం,      /* path to Unix socket, if used */
#అయితే_ముగింపు
  STRING_లక్ష్యం,                /* CURLOPT_REQUEST_TARGET */
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#అయితే_ముగింపు
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#నిర్వచితమైతే USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#అయితే_ముగింపు
  STRING_SSL_EC_CURVES,
#నిర్వచితం_కాకపోతే CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_కీలకం,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_శీర్షికలు, /* space-separated components to sign */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#అయితే_ముగింపు
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_ప్రజా,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_చివరి /* not used, an end-of-list marker */
};

గణన dupblob {
  BLOB_CERT,
  BLOB_కీలకం,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_కీలకం_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#అయితే_ముగింపు
  BLOB_చివరి
};

నిర్మాణం UserDefined {
  FILE *err;         /* the stderr user data goes here */
  శూన్య *debugdata;   /* the data that will be passed to fdebug */
  అక్షరం *errorbuffer; /* (Static) store failure messages in here */
  శూన్య *బహిర్గతం;         /* CURLOPT_WRITEDATA */
  శూన్య *లోపల_సమితి;      /* CURLOPT_READDATA */
  శూన్య *writeheader; /* write the header to this if non-NULL */
  uint32_t httpauth;  /* kind of HTTP authentication to use (bitmask) */
  uint32_t proxyauth; /* kind of proxy authentication to use (bitmask) */
  శూన్య *postfields;  /* if POST, set the fields' values here */
  అక్షరం *str_copypostfields; /* CURLOPT_COPYPOSTFIELDS value */
  curl_seek_callback seek_func;      /* function that seeks the input */
  curl_off_t postfieldsize; /* if POST, this might have a size to use instead
                               of strlen(), and then the data *may* be binary
                               (contain NUL bytes) */
  curl_write_callback fwrite_func;   /* function that stores the output */
  curl_write_callback fwrite_శీర్షిక; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_సమితి; /* function that reads the input */
  curl_పురోగతి_callback fprogress; /* OLD and deprecated progress callback */
  curl_xferinfo_callback fxferinfo; /* progress callback */
  curl_debug_callback fdebug;      /* function that write informational data */
  curl_ioctl_callback ioctl_func;  /* function for I/O control */
  curl_sockopt_callback fsockopt;  /* function for setting socket options */
  శూన్య *sockopt_client; /* pointer to pass to the socket options callback */
  curl_opensocket_callback fopensocket; /* function for checking/translating
                                           the address and opening the
                                           socket */
  శూన్య *opensocket_client;
  curl_closesocket_callback fclosesocket; /* function for closing the
                                             socket */
  శూన్య *closesocket_client;
  curl_prereq_callback fprereq; /* pre-initial request callback */
  శూన్య *prereq_userp; /* pre-initial request user data */

  శూన్య *seek_client;    /* pointer to pass to the seek callback */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  శూన్య *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  శూన్య *hsts_write_userp;
#అయితే_ముగింపు
  శూన్య *పురోగతి_client; /* pointer to pass to the progress callback */
  శూన్య *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t దిగువ_వేగం_హద్దు; /* bytes/second */
  curl_off_t max_send_వేగం; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_వేగం; /* high speed limit in bytes/second for
                                download */
  curl_off_t సమితి_resume_from;  /* continue [ftp] transfer from here */
  నిర్మాణం curl_slist *శీర్షికలు; /* linked list of extra headers */
  నిర్మాణం curl_httppost *httppost;  /* linked list of old POST data */
#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_TELNET
  నిర్మాణం curl_slist *telnet_options; /* linked list of telnet options */
#అయితే_ముగింపు
  నిర్మాణం curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  నిర్మాణం curl_slist *connect_గమ్యం; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  time_t timevalue;       /* what time to compare with */
  నిర్మాణం ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  శూన్య *ssl_fsslctxp;        /* parameter for callback */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  నిర్మాణం ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  నిర్మాణం curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#అయితే_ముగింపు
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t బఫరు_కొలత;        /* size of receive buffer to use */
  uint32_t upload_బఫరు_కొలత; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  శూన్య *వ్యక్తిగత_దత్తాంశం; /* application-private data */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  నిర్మాణం curl_slist *http200aliases; /* linked list of aliases for http200 */
#అయితే_ముగింపు
  curl_off_t max_filesize; /* Maximum file size to download */
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  నిర్మాణం curl_slist *quote;     /* after connection is established */
  నిర్మాణం curl_slist *postquote; /* after the transfer */
  నిర్మాణం curl_slist *prequote; /* before the transfer, after type */
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  శూన్య *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  శూన్య *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#అయితే_ముగింపు
  నిర్మాణం u8_strset అక్షరశ్రేణులు;
  నిర్మాణం curl_blob *blobs[BLOB_చివరి];
  uint32_t new_file_perms;      /* when creating remote files */
#నిర్వచితమైతే USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#అయితే_ముగింపు
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  శూన్య *rtp_బహిర్గతం;     /* write RTP to this if non-NULL */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_చివర_callback chunk_చివర; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  శూన్య *fnmatch_దత్తాంశం;
  శూన్య *wildcardptr;
#అయితే_ముగింపు

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_ప్రారంభం_callback resolver_ప్రారంభం; /* optional callback called
                                                  before resolver start */
  శూన్య *resolver_ప్రారంభం_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  శూన్య *trailer_దత్తాంశం; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  నిర్మాణం curl_slist *mail_rcpt; /* linked list of mail recipients */
#అయితే_ముగింపు
  పూర్ణాంకం ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  పూర్ణాంకం tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  పూర్ణాంకం tcp_keepintvl;    /* seconds between TCP keepalive probes */
  పూర్ణాంకం tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#ఒకవేళ defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  పూర్ణాంకం బరువు;
#అయితే_ముగింపు
  పొట్టి maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_పోర్టు; /* which port to use (when not using default) */
  uint16_t దిగువ_వేగం_సమయం;  /* number of seconds */
#నిర్వచితం_కాకపోతే CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#అయితే_ముగింపు
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t పద్ధతి;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#అయితే_ముగింపు
  uint8_t http_follow_బహుళకం; /* follow HTTP redirects */
  బిట్టు(connect_only); /* make connection/request, then let application use the
                        socket */
  బిట్టు(connect_only_ws); /* special websocket connect-only level */
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  బిట్టు(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
  బిట్టు(mime_formescape);
#అయితే_ముగింపు
  బిట్టు(is_fread_సమితి); /* has read callback been set to non-NULL? */
#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP
  బిట్టు(tftp_no_options); /* do not send TFTP options requests */
#అయితే_ముగింపు
  బిట్టు(sep_శీర్షికలు);     /* handle host and proxy headers separately */
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  బిట్టు(cookiesession);   /* new cookie session? */
#అయితే_ముగింపు
  బిట్టు(crlf);            /* convert crlf on ftp upload(?) */
#నిర్వచితమైతే USE_SSH
  బిట్టు(ssh_compression);            /* enable SSH compression */
#అయితే_ముగింపు

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  బిట్టు(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  బిట్టు(get_filetime);     /* get the time and get of the remote file */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  బిట్టు(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#అయితే_ముగింపు
  బిట్టు(prefer_ascii);     /* ASCII rather than binary */
  బిట్టు(remote_append);    /* append, not overwrite, on upload */
#నిర్వచితమైతే CURL_LIST_ONLY_PROTOCOL
  బిట్టు(list_only);        /* list directory */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  బిట్టు(ftp_use_పోర్టు);     /* use the FTP PORT command */
  బిట్టు(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  బిట్టు(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  బిట్టు(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  బిట్టు(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  బిట్టు(wildcard_enabled); /* enable wildcard matching */
#అయితే_ముగింపు
  బిట్టు(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  బిట్టు(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  బిట్టు(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  బిట్టు(allow_auth_గమ్యం_other_hosts);
  బిట్టు(include_శీర్షిక); /* include received protocol headers in data output */
  బిట్టు(http_సమితి_referer); /* is a custom referer used */
  బిట్టు(http_auto_referer); /* set "correct" referer when following
                             location: */
  బిట్టు(opt_no_body);    /* as set with CURLOPT_NOBODY */
  బిట్టు(verbose);        /* output verbosity */
  బిట్టు(reuse_forbid);   /* forbidden to be reused, close after use */
  బిట్టు(reuse_fresh);    /* do not reuse an existing connection */
  బిట్టు(no_signal);      /* do not use any signal/alarm handler */
  బిట్టు(tcp_nodelay);    /* whether to enable TCP_NODELAY or not */
  బిట్టు(ignorecl);       /* ignore content length */
  బిట్టు(http_te_skip);   /* pass the raw body data to the user, even when
                          transfer-encoded (chunked, compressed) */
  బిట్టు(http_ce_skip);   /* pass the raw body data to the user, even when
                          content-encoded (chunked, compressed) */
  బిట్టు(proxy_transfer_బహుళకం); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  బిట్టు(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#అయితే_ముగింపు
  బిట్టు(socks5_auth_only); /* SOCKS5 must authenticate */
  బిట్టు(sasl_ir);         /* Enable/disable SASL initial response */
  బిట్టు(tcp_keepalive);  /* use TCP keepalives */
  బిట్టు(tcp_fastopen);   /* use TCP Fast Open */
  బిట్టు(ssl_enable_alpn);/* TLS ALPN extension? */
  బిట్టు(మార్గం_as_is);     /* allow dotdots? */
  బిట్టు(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  బిట్టు(suppress_connect_శీర్షికలు); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  బిట్టు(dns_shuffle_చిరునామాలు); /* whether to shuffle addresses before use */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  బిట్టు(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
శీర్షిక*/
#అయితే_ముగింపు
#నిర్వచితమైతే USE_UNIX_SOCKETS
  బిట్టు(abstract_unix_socket);
#అయితే_ముగింపు
  బిట్టు(disallow_username_లోపల_url); /* disallow username in URL */
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  బిట్టు(doh); /* DNS-over-HTTPS enabled */
  బిట్టు(doh_verifypeer);     /* DoH certificate peer verification */
  బిట్టు(doh_verifyhost);     /* DoH certificate hostname verification */
  బిట్టు(doh_verifystatus);   /* DoH certificate status verification */
#అయితే_ముగింపు
  బిట్టు(http09_allowed); /* allow HTTP/0.9 responses */
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  బిట్టు(ws_raw_బహుళకం);
  బిట్టు(ws_no_auto_pong);
#అయితే_ముగింపు
  బిట్టు(post301); /* keep POSTs as POSTs after a 301 request */
  బిట్టు(post302); /* keep POSTs as POSTs after a 302 request */
  బిట్టు(post303); /* keep POSTs as POSTs after a 303 request */
#నిర్వచితమైతే USE_OPENSSL
  బిట్టు(engine_default);
#అయితే_ముగింపు
};

#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
#నిర్వచించు IS_MIME_POST(a)                                                 \
  ((a)->సమితి.mimepostp && ((a)->సమితి.mimepostp->రకం != MIMEKIND_NONE))
#లేకపోతే
#నిర్వచించు IS_MIME_POST(a) FALSE
#అయితే_ముగింపు

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
రకనిర్వచనం శూన్య multi_sub_xfer_పూర్తి_cb(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం Curl_easy *master,
                                    CURLcode ఫలితం);

/*
 * The 'connectdata' struct MUST have all the connection oriented stuff as we
 * may have several simultaneous connections and connection structs in memory.
 *
 * The 'struct UserDefined' must only contain data that is set once to go for
 * many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" must be defined within the
 * 'struct UrlState' instead.
 */

నిర్మాణం Curl_easy {
  /* First a simple identifier to more easily detect if a user mixes up this
     easy handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  uint32_t magic;
  /* once an easy handle is added to a multi, either explicitly by the
   * libcurl application or implicitly during `curl_easy_perform()`,
   * a unique identifier inside this one multi instance. */
  uint32_t mid;
  CURLMstate mstate;  /* the handle's state */
  CURLcode ఫలితం;   /* previous result */

  నిర్మాణం connectdata *conn;
  నిర్మాణం Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  నిర్మాణం Curl_eapi_దొంతర callstack; /* easy api calls ongoing */

  నిర్మాణం Curl_share *share;    /* Share, handles global variable mutexing */

  నిర్మాణం Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  నిర్మాణం CURLMsg సందేశం; /* A single posted message. */

  /* once an easy handle is tied to a connection pool a non-negative number to
     distinguish this transfer from other using the same pool. For easier
     tracking in log output. This may wrap around after CURL_OFF_T_MAX to 0
     again, so it has no uniqueness guarantee for large processings. Note: it
     has no uniqueness either IFF more than one connection pool is used by the
     libcurl application. */
  curl_off_t id;
  uint32_t master_mid; /* if set, this transfer belongs to a master */
  multi_sub_xfer_పూర్తి_cb *sub_xfer_పూర్తి;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  నిర్మాణం Curl_hash meta_hash;

#నిర్వచితమైతే USE_LIBPSL
  నిర్మాణం PslCache *psl;        /* The associated PSL cache. */
#అయితే_ముగింపు
  నిర్మాణం SingleRequest req;    /* Request-specific data */
  నిర్మాణం UserDefined సమితి;      /* values set by the libcurl user */
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  నిర్మాణం CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  నిర్మాణం hsts *hsts;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  నిర్మాణం altsvcinfo *asi;      /* the alt-svc cache */
#అయితే_ముగింపు
  నిర్మాణం పురోగతి_2 పురోగతి;    /* for all the progress meter data */
  నిర్మాణం UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  నిర్మాణం Wildcard_దత్తాంశం *wildcard; /* wildcard download state info */
#అయితే_ముగింపు
  నిర్మాణం శుద్ధ_Info info;        /* stats, reports and info data */
  నిర్మాణం curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#నిర్వచించు CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->సమితి.అక్షరశ్రేణులు, (uint8_t)(id))
#నిర్వచించు CURL_EASY_STR_సమితి(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->సమితి.అక్షరశ్రేణులు, (uint8_t)(id), s, slen)
#నిర్వచించు CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->సమితి.అక్షరశ్రేణులు, (uint8_t)(id), s)
#నిర్వచించు CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->సమితి.అక్షరశ్రేణులు, (uint8_t)(id))
#నిర్వచించు CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->సమితి.అక్షరశ్రేణులు, (uint8_t)(id))

#నిర్వచించు LIBCURL_NAME "libcurl"

#అయితే_ముగింపు /* HEADER_CURL_URLDATA_H */
