#यदि_अपरिभाषितम् शीर्षकम्_CURL_URLDATA_H
#परिभाषय शीर्षकम्_CURL_URLDATA_H
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
#अन्तर्भाव "curl_setup.h"

#परिभाषय CURL_DEFAULT_USER     "anonymous"
#परिभाषय CURL_DEFAULT_PASSWORD "ftp@example.com"

#यदि !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#परिभाषय CURL_PREFER_LF_LINEENDS
#यदि_अन्तः

#परिभाषय DEFAULT_CONNCACHE_आकार 5

/* length of longest IPv6 address string including the null-terminator */
#परिभाषय MAX_IPADR_LEN परिमाण("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#परिभाषय CURL_MAX_निवेशः_दीर्घता 8000000

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#यदि_अन्तः

#अन्तर्भाव "curlx/timeval.h"

#अन्तर्भाव "api.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "creds.h"
#अन्तर्भाव "psl.h"
#अन्तर्भाव "formdata.h"
#अन्तर्भाव "http_chunks.h" /* for the structs and enum stuff */
#अन्तर्भाव "hash.h"
#अन्तर्भाव "peer.h"
#अन्तर्भाव "proxy.h"
#अन्तर्भाव "splay.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "request.h"
#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "netrc.h"
#अन्तर्भाव "u8_strset.h"
#अन्तर्भाव "vdns/asyn.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vtls/vtls_config.h"

/* On error return, the value of `pnwritten` has no meaning */
प्रकारनिर्देश CURLcode (Curl_send)(संरचना Curl_easy *दत्त,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ध्रुव uint8_t *buf,       /* data to write */
                             size_t len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             size_t *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
प्रकारनिर्देश CURLcode (Curl_recv)(संरचना Curl_easy *दत्त,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             वर्ण *buf,                /* store data here */
                             size_t len,               /* max amount to read */
                             size_t *pnread);          /* how much received */

#अन्तर्भाव "mime.h"
#अन्तर्भाव "protocol.h"
#अन्तर्भाव "ftp.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "mqtt.h"
#अन्तर्भाव "ftplistparser.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "cf-socket.h"

#यदि_परिभाषितम् HAVE_GSSAPI
#  ifdef HAVE_GSSAPPLE
#    include <GSS/gssapi.h>
#    include <GSS/gssapi_oid.h>
#  elif defined(HAVE_GSSGNU)
#    include <gss.h>
#  elif defined(HAVE_GSSAPI_H)
#    include <gssapi.h>
#  अन्यथा /* MIT Kerberos */
#    include <gssapi/gssapi.h>
#    include <gssapi/gssapi_krb5.h> /* for GSS_C_CHANNEL_BOUND_FLAG in 1.19+ */
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् USE_LIBSSH2
#अन्तर्भाव <libssh2.h>
#अन्तर्भाव <libssh2_sftp.h>
#यदि_अन्तः /* USE_LIBSSH2 */

#परिभाषय READBUFFER_आकार CURL_MAX_WRITE_आकार
#परिभाषय READBUFFER_MAX  CURL_MAX_READ_आकार
#परिभाषय READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#परिभाषय UPLOADBUFFER_DEFAULT 65536
#परिभाषय UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#परिभाषय UPLOADBUFFER_MIN     CURL_MAX_WRITE_आकार

#यदि_परिभाषितम् USE_WINDOWS_SSPI
#अन्तर्भाव "curl_sspi.h"
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
संरचना digestdata {
  संरचना Curl_creds *creds;
  संरचना Curl_peer *origin;
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  BYTE *निवेशः_token;
  size_t निवेशः_token_len;
  CtxtHandle *http_context;
#अन्यथा
  वर्ण *nonce;
  वर्ण *cnonce;
  वर्ण *realm;
  वर्ण *opaque;
  वर्ण *qop;
  वर्ण *algorithm;
  पूर्णाङ्क nc; /* nonce count */
  uint8_t algo;
  BIT(stale); /* set true for re-negotiation */
  BIT(userhash);
#यदि_अन्तः
};
#यदि_अन्तः

प्रकारनिर्देश गणना {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_चरमः
} curlntlm;

#परिभाषय CURL_NEGO_AUTH_NONE   0U
#परिभाषय CURL_NEGO_AUTH_RECV   1U
#परिभाषय CURL_NEGO_AUTH_प्रेषितम्   2U
#परिभाषय CURL_NEGO_AUTH_समाप्तम्   3U
#परिभाषय CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
संरचना Connect_द्वयङ्काः {
  BIT(connect_only);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  BIT(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  BIT(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#यदि_अन्तः
  /* always modify bits.close with the connclose() and connkeep() macros! */
  BIT(close); /* if set, we close the connection after this request */
  BIT(reuse); /* if set, this is a reused connection */
  BIT(altused); /* this is an alt-svc "redirect" */
  BIT(ipv6);    /* we communicate with a site using an IPv6 address */
  BIT(do_अधिकम्); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  BIT(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  BIT(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  BIT(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
अनुरोधाः*/
  BIT(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
अनुरोधाः*/
  BIT(ftp_use_दत्त_ssl); /* Enabled SSL for the data connection */
#यदि_अन्तः
  BIT(bound); /* set true if bind() has already been done on this socket/
                 connection */
  BIT(upgrade_अन्तर्_प्रगतिः); /* protocol upgrade is in progress */
  BIT(multiplex); /* connection is multiplexed */
  BIT(tcp_fastopen); /* use TCP Fast Open */
  BIT(tls_enable_alpn); /* TLS ALPN extension? */
  BIT(sock_accepted); /* TRUE if the SECONDARYSOCKET was created with
                         accept() */
  BIT(parallel_connect); /* set TRUE when a parallel connect attempt has
                            started (happy eyeballs) */
  BIT(aborted); /* connection was aborted, e.g. in unclean state */
  BIT(no_reuse); /* connection should not be reused */
  BIT(shutdown_handler); /* connection shutdown: handler shut down */
  BIT(shutdown_filters); /* connection shutdown: filters shut down */
  BIT(dns_resolved); /* DNS records for connection were resolved */
};

संरचना hostname {
  वर्ण *rawalloc; /* allocated "raw" version of the name */
  वर्ण *encalloc; /* allocated IDN-encoded version of the name */
  वर्ण *name;     /* name to use internally, might be encoded, might be raw */
  ध्रुव वर्ण *dispname; /* name to display, as 'name' might be encoded */
};

#परिभाषय FIRSTSOCKET     0
#परिभाषय SECONDARYSOCKET 1

#परिभाषय TRNSPRT_NONE 0
#परिभाषय TRNSPRT_TCP  3
#परिभाषय TRNSPRT_UDP  4
#परिभाषय TRNSPRT_QUIC 5
#परिभाषय TRNSPRT_UNIX 6

#परिभाषय TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

संरचना ip_quadruple {
  वर्ण remote_ip[MAX_IPADR_LEN];
  वर्ण local_ip[MAX_IPADR_LEN];
  uint16_t remote_पत्तनम्;
  uint16_t local_पत्तनम्;
  uint8_t transport;
};

#परिभाषय CUR_IP_QUAD_HAS_PORTS(x)      \
  (((x)->transport == TRNSPRT_TCP) || \
   ((x)->transport == TRNSPRT_UDP) || \
   ((x)->transport == TRNSPRT_QUIC))

/*
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
संरचना connectdata {
  curl_off_t connection_id; /* Contains a unique number to make it easier to
                               track the connections in the log output */

  /* A connection cache from a SHARE might be used in several multi handles.
   * We MUST not reuse connections that are running in another multi,
   * for concurrency reasons. That multi might run in another thread.
   * `attached_multi` is set by the first transfer attached and cleared
   * when the last one is detached.
   * NEVER call anything on this multi, check for equality. */
  संरचना Curl_multi *attached_multi;

  /* Who the connection is talking to, ultimately */
  संरचना Curl_peer *origin; /* connection ultimately talks to this */
  संरचना Curl_peer *द्वारा_peer; /* if set, connection really talks to this */
  संरचना Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  संरचना Curl_peer *द्वारा_peer2; /* peer of SECONDARYSOCKET */
  संरचना Curl_creds *creds; /* When connection itself is tied to credentials */
  संरचना Curl_peer *creds_origin; /* origin tied credentials are for */
  ध्रुव संरचना Curl_scheme *scheme; /* Connection's real protocol handler */

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the connection.
   * Elements need to be added with their own destructor to be invoked when
   * the connection is cleaned up (see Curl_hash_add2()).*/
  संरचना Curl_hash meta_hash;

  वर्ण *destination; /* hostname+port, used in conncache */

  संरचना curltime created; /* creation time */
  /* timediff_t are deltas from `created`*/
  timediff_t lastused_ms; /* when returned to the connection pool as idle */
  timediff_t lastchecked_ms; /* when last checked alive status */
  timediff_t lastupkeep_ms; /* when last done conn_upkeep */
  संरचना {
    संरचना curltime at[2]; /* first Curl_conn_connect() for each socket */
    bool समुच्चयः[2];
  } connect_started;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  संरचना proxy_info socks_proxy;
  संरचना proxy_info http_proxy;
#यदि_अन्तः

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  संरचना Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#परिभाषय CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  संरचना {
    timediff_t start_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  शून्य *closesocket_client;

  संरचना ssl_filter_config ssl_config;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  संरचना ssl_filter_config proxy_ssl_config;
#यदि_अन्तः
  वर्ण *options; /* options string, allocated */

  /*************** Request - specific items ************/
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#यदि_अन्तः

#यदि_परिभाषितम् USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#यदि_अन्तः

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  वर्ण *interface_opts;
  संरचना Connect_द्वयङ्काः द्वयङ्काः;    /* various state-flags for this connection */
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  पूर्णाङ्क socks5_gssapi_enctype;
#यदि_अन्तः

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#परिभाषय CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#यदि_परिभाषितम् USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#यदि_अन्तः
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_संस्करणम्; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_आलोकितम्;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#यदि_परिभाषितम् USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#यदि_अन्तः
};

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
#परिभाषय CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->द्वारा_peer ? (c)->द्वारा_peer->user_hostname : \
        (c)->origin->user_hostname)
#अन्यथा
#परिभाषय CURL_CONN_HOST_DISPNAME(c) \
  ((c)->द्वारा_peer ? (c)->द्वारा_peer->user_hostname : (c)->origin->user_hostname)
#यदि_अन्तः

/* The end of connectdata. */

/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
संरचना विशुद्धम्_Info {
  /* PureInfo primary ip_quadruple is copied over from the connectdata
     struct in order to allow curl_easy_getinfo() to return this information
     even when the session handle is no longer associated with a connection,
     and also allow curl_easy_reset() to clear this information from the
     session handle without disturbing information which is still alive, and
     that might be reused, in the connection pool. */
  संरचना ip_quadruple primary;
  संरचना curl_certinfo certs; /* info about the certs. Asked for with
                                 CURLOPT_CERTINFO / CURLINFO_CERTINFO */
  time_t filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t request_आकार; /* the amount of bytes sent in the request(s) */
  curl_off_t numconnects; /* how many new connections libcurl created */
  वर्ण *contenttype; /* the content type of the object */
  वर्ण *wouldredirect; /* URL this would have been redirected to if asked to */
  वर्ण *effective_url; /* when needed */
  curl_off_t retry_after; /* info from Retry-After: header */
  ध्रुव वर्ण *conn_scheme;
  पूर्णाङ्क httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  पूर्णाङ्क httpproxycode; /* response code from proxy when received separate */
  पूर्णाङ्क httpversion; /* the http version number X.Y = X*10+Y */
  uint32_t conn_protocol;
  uint32_t proxyauthavail; /* what proxy auth types were announced */
  uint32_t httpauthavail;  /* what host auth types were announced */
  uint32_t proxyauthpicked; /* selected proxy auth type */
  uint32_t httpauthpicked;  /* selected host auth type */
  uint32_t शीर्षकम्_आकार;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  BIT(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  BIT(प्रयुक्तम्_proxy); /* the transfer used a proxy */
};

संरचना pgrs_dir {
  curl_off_t योग_आकार; /* total expected bytes */
  curl_off_t cur_आकार; /* transferred bytes so far */
  curl_off_t जवः; /* bytes per second transferred */
  संरचना Curl_rlimit rlimit; /* speed limiting / pausing */
};

संरचना प्रगतिः_2 {
  संरचना curltime now; /* current time of processing */
  संरचना curltime start; /* when transfer was initialized, set once */

  संरचना pgrs_dir ul;
  संरचना pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_जवः; /* uses the currently fastest transfer */
  curl_off_t earlydata_प्रेषितम्;

  संरचना {
    timediff_t startop_us; /* since start when operations started */
    timediff_t startsingle_us; /* since start when last request started */
    timediff_t startqueue_us; /* since start when last entered queueing */
    timediff_t startredirect_us; /* since start when last redirected */
    timediff_t lastshow_us; /* since start when last progress shown */
  } delta;
  संरचना {
    timediff_t spent_us; /* all time spent since start */
    timediff_t queued_us; /* time spent since startsingle's for queueing */
    timediff_t nslookup_us; /* same for name resolves */
    timediff_t connect_us; /* same for connects */
    timediff_t appconnect_us; /* same for application connects, e.g. TLS */
    timediff_t pretransfer_us; /* same until requests were sent */
    timediff_t starttransfer_us; /* same until responses started */
    timediff_t posttransfer_us; /* same until responses ended */
  } योग;

#परिभाषय CURL_जवः_लेखाः (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t जवः_amount[CURL_जवः_लेखाः];
  timediff_t जवः_time[CURL_जवः_लेखाः];
  uint32_t speeder_c;
  BIT(hide);
  BIT(ul_आकार_ज्ञातम्);
  BIT(dl_आकार_ज्ञातम्);
  BIT(शीर्षकाणि_बहिर्गमनम्); /* when the headers have been written */
  BIT(callback);  /* set when progress callback is used */
  BIT(startransfer_added);
};

संरचना auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  BIT(समाप्तम्);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  BIT(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#यदि_परिभाषितम् USE_NGHTTP2
संरचना Curl_दत्त_prio_ग्रन्थिः {
  संरचना Curl_दत्त_prio_ग्रन्थिः *अग्र;
  संरचना Curl_easy *दत्त;
};
#यदि_अन्तः

/* Timers */
प्रकारनिर्देश गणना {
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
  EXPIRE_चरमः /* not an actual timer, used as a marker only */
} expire_id;

संरचना expire_timers {
  संरचना Curl_वृक्षः splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t विस्थापनम्_us[EXPIRE_चरमः];
  uint8_t अग्र[EXPIRE_चरमः];
  uint8_t first;
};

/* individual pieces of the URL */
संरचना urlpieces {
  वर्ण *options;
  वर्ण *मार्गः;
  वर्ण *query;
};

संरचना UrlState {
  /* Origin of the initial (e.g. not followed) request of a transfer.
     Credentials from CURLOPT_* are only valid for this origin.
     Always set once a transfer starts searching for connections. */
  संरचना Curl_peer *initial_origin;
  /* Current origin of the transfer, changes to origin of follow
   * requests. */
  संरचना Curl_peer *origin;

  संरचना curltime keeps_जवः; /* for the progress meter really */
  संरचना dynbuf headerb; /* buffer to store headers in */
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  संरचना curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#यदि_अन्तः

  पूर्णाङ्क os_errno;  /* filled in with errno whenever an error occurs */
  पूर्णाङ्क अनुरोधाः; /* request counter: redirects + authentication retakes */
#यदि_परिभाषितम् HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  शून्य (*पूर्व_signal)(पूर्णाङ्क sig);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
  वर्ण *envproxy; /* last proxy string used for proxy-related state */
  संरचना digestdata digest;      /* state data for host Digest auth */
  संरचना digestdata proxydigest; /* state data for proxy Digest auth */
#यदि_अन्तः
  संरचना auth authhost;  /* auth details for host */
  संरचना auth authproxy; /* auth details for proxy */

#यदि_परिभाषितम् USE_CURL_ASYNC
  संरचना Curl_resolv_async *async;  /* asynchronous name resolver data */
#यदि_अन्तः

  संरचना expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  वर्ण *most_recent_ftp_entrypath;
  वर्ण *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  शून्य *अन्तर्;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  संरचना urlpieces ऊर्ध्वम्;
  संरचना bufref url;        /* work URL, initially copied from UserDefined */
  संरचना bufref referer;    /* referer string */
  संरचना curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#यदि_अपरिभाषितम् CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#यदि_अन्तः
  संरचना Curl_llist httphdrs; /* received headers */
  संरचना curl_शीर्षकम् headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  अचिह्नित पूर्णाङ्क nh_origin;
  पूर्णाङ्क nh_request;
  size_t nh_गणनम्; /* httphdrs count when the cache was filled */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  संरचना curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#यदि_अन्तः

#यदि_परिभाषितम् CURLVERBOSE
  संरचना curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_NETRC
  संरचना store_netrc netrc;
#यदि_अन्तः

  संरचना Curl_creds *creds; /* Credentials for the origin only */

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  वर्ण *rangeline; /* allocated */
  वर्ण *http_host; /* allocated */
  संरचना http_negotiation http_neg;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  संरचना Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_आवरणम्[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_अग्र_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_अग्र_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#यदि_अन्तः

  curl_off_t चरमः_conn_id; /* The last assigned connection or -1 */
  uint32_t चरमः_cpid; /* last connection pool id used */

#यदि defined(USE_HTTP2) || defined(USE_HTTP3)
  पूर्णाङ्क weight; /* shallow copy of data->set */
#यदि_अन्तः
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  BIT(really_alive); /* transfer is really alive in multi, passed INIT */
  BIT(this_is_a_follow); /* this is a followed Location: request */
  BIT(refused_प्रवाहः); /* this was refused, try again */
  BIT(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  BIT(allow_पत्तनम्); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  BIT(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  BIT(wildcardmatch); /* enable wildcard matching */
  BIT(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  BIT(use_range);
  BIT(rangestringalloc); /* the range string is malloc()'ed */
  BIT(समाप्तम्); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  BIT(cookie_engine);
#यदि_अन्तः
  BIT(prefer_ascii);   /* ASCII rather than binary */
#यदि_परिभाषितम् CURL_LIST_ONLY_PROTOCOL
  BIT(list_only);      /* list directory contents */
#यदि_अन्तः
  BIT(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  BIT(upload);         /* upload request */
  BIT(आन्तरिकम्); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  BIT(http_ignorecustom); /* ignore custom method from now */
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  BIT(http_hd_te); /* Added HTTP header TE: */
  BIT(http_hd_upgrade); /* Added HTTP header Upgrade: */
  BIT(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  BIT(maybe_folded);
  BIT(leading_unfold); /* unfold started, this is the leading bytes */
#यदि_अन्तः
};

/*
 * This 'UserDefined' struct must only contain data that is set once to go
 * for many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" MUST be defined within the
 * 'struct UrlState' instead. The only exceptions MUST note the changes in
 * the 'DynamicStatic' struct.
 * Character pointer fields point to dynamic storage, unless otherwise stated.
 */

संरचना Curl_multi;    /* declared in multihandle.c */

गणना dupstring {
  STRING_CERT,            /* client certificate filename */
  STRING_CERT_TYPE,       /* format for certificate (default: PEM)*/
  STRING_कुञ्चिका,             /* private key filename */
  STRING_कुञ्चिका_PASSWD,      /* plain text private key password */
  STRING_कुञ्चिका_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_कुञ्चिका_PROXY,       /* private key filename */
  STRING_कुञ्चिका_PASSWD_PROXY, /* plain text private key password */
  STRING_कुञ्चिका_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#यदि_अन्तः
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_गन्तव्यम्_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#यदि_अन्तः
  STRING_समुच्चयः_RANGE,       /* range, if used */
  STRING_समुच्चयः_REFERER,     /* custom string for the HTTP referer field */
  STRING_समुच्चयः_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
प्रयुक्तम्*/
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_प्रवाहः_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  STRING_SSH_वैयक्तिकम्_कुञ्चिका, /* path to the private key file for auth */
  STRING_SSH_लौकिकम्_कुञ्चिका,  /* path to the public key file for auth */
  STRING_SSH_HOST_लौकिकम्_कुञ्चिका_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_लौकिकम्_कुञ्चिका_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#यदि_अन्तः
  STRING_BEARER,                /* <bearer>, if used */
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_मार्गः,      /* path to Unix socket, if used */
#यदि_अन्तः
  STRING_TARGET,                /* CURLOPT_REQUEST_TARGET */
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#यदि_अन्तः
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#यदि_परिभाषितम् USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#यदि_अन्तः
  STRING_SSL_EC_CURVES,
#यदि_अपरिभाषितम् CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_कुञ्चिका,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_शीर्षकाणि, /* space-separated components to sign */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#यदि_अन्तः
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_लौकिकम्,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_चरमः /* not used, an end-of-list marker */
};

गणना dupblob {
  BLOB_CERT,
  BLOB_कुञ्चिका,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_कुञ्चिका_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#यदि_अन्तः
  BLOB_चरमः
};

संरचना UserDefined {
  FILE *err;         /* the stderr user data goes here */
  शून्य *debugdata;   /* the data that will be passed to fdebug */
  वर्ण *errorbuffer; /* (Static) store failure messages in here */
  शून्य *बहिर्गमनम्;         /* CURLOPT_WRITEDATA */
  शून्य *अन्तर्_समुच्चयः;      /* CURLOPT_READDATA */
  शून्य *writeheader; /* write the header to this if non-NULL */
  uint32_t httpauth;  /* kind of HTTP authentication to use (bitmask) */
  uint32_t proxyauth; /* kind of proxy authentication to use (bitmask) */
  शून्य *postfields;  /* if POST, set the fields' values here */
  वर्ण *str_copypostfields; /* CURLOPT_COPYPOSTFIELDS value */
  curl_seek_callback seek_func;      /* function that seeks the input */
  curl_off_t postfieldsize; /* if POST, this might have a size to use instead
                               of strlen(), and then the data *may* be binary
                               (contain NUL bytes) */
  curl_write_callback fwrite_func;   /* function that stores the output */
  curl_write_callback fwrite_शीर्षकम्; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_समुच्चयः; /* function that reads the input */
  curl_प्रगतिः_callback fprogress; /* OLD and deprecated progress callback */
  curl_xferinfo_callback fxferinfo; /* progress callback */
  curl_debug_callback fdebug;      /* function that write informational data */
  curl_ioctl_callback ioctl_func;  /* function for I/O control */
  curl_sockopt_callback fsockopt;  /* function for setting socket options */
  शून्य *sockopt_client; /* pointer to pass to the socket options callback */
  curl_opensocket_callback fopensocket; /* function for checking/translating
                                           the address and opening the
                                           socket */
  शून्य *opensocket_client;
  curl_closesocket_callback fclosesocket; /* function for closing the
                                             socket */
  शून्य *closesocket_client;
  curl_prereq_callback fprereq; /* pre-initial request callback */
  शून्य *prereq_userp; /* pre-initial request user data */

  शून्य *seek_client;    /* pointer to pass to the seek callback */
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  शून्य *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  शून्य *hsts_write_userp;
#यदि_अन्तः
  शून्य *प्रगतिः_client; /* pointer to pass to the progress callback */
  शून्य *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t नीचः_जवः_सीमा; /* bytes/second */
  curl_off_t max_send_जवः; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_जवः; /* high speed limit in bytes/second for
                                download */
  curl_off_t समुच्चयः_resume_from;  /* continue [ftp] transfer from here */
  संरचना curl_slist *शीर्षकाणि; /* linked list of extra headers */
  संरचना curl_httppost *httppost;  /* linked list of old POST data */
#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_TELNET
  संरचना curl_slist *telnet_options; /* linked list of telnet options */
#यदि_अन्तः
  संरचना curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  संरचना curl_slist *connect_गन्तव्यम्; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  time_t timevalue;       /* what time to compare with */
  संरचना ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  शून्य *ssl_fsslctxp;        /* parameter for callback */
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  संरचना ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  संरचना curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#यदि_अन्तः
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t बफर_आकार;        /* size of receive buffer to use */
  uint32_t upload_बफर_आकार; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  शून्य *वैयक्तिकम्_दत्त; /* application-private data */
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  संरचना curl_slist *http200aliases; /* linked list of aliases for http200 */
#यदि_अन्तः
  curl_off_t max_filesize; /* Maximum file size to download */
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  संरचना curl_slist *quote;     /* after connection is established */
  संरचना curl_slist *postquote; /* after the transfer */
  संरचना curl_slist *prequote; /* before the transfer, after type */
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  शून्य *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  शून्य *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#यदि_अन्तः
  संरचना u8_strset वाक्यानि;
  संरचना curl_blob *blobs[BLOB_चरमः];
  uint32_t new_file_perms;      /* when creating remote files */
#यदि_परिभाषितम् USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#यदि_अन्तः
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  शून्य *rtp_बहिर्गमनम्;     /* write RTP to this if non-NULL */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_अन्तः_callback chunk_अन्तः; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  शून्य *fnmatch_दत्त;
  शून्य *wildcardptr;
#यदि_अन्तः

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_start_callback resolver_start; /* optional callback called
                                                  before resolver start */
  शून्य *resolver_start_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  शून्य *trailer_दत्त; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
  संरचना curl_slist *mail_rcpt; /* linked list of mail recipients */
#यदि_अन्तः
  पूर्णाङ्क ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  पूर्णाङ्क tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  पूर्णाङ्क tcp_keepintvl;    /* seconds between TCP keepalive probes */
  पूर्णाङ्क tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#यदि defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  पूर्णाङ्क weight;
#यदि_अन्तः
  ह्रस्व maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_पत्तनम्; /* which port to use (when not using default) */
  uint16_t नीचः_जवः_time;  /* number of seconds */
#यदि_अपरिभाषितम् CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#यदि_अन्तः
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t विधानम्;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#यदि_अन्तः
  uint8_t http_follow_बहुलकम्; /* follow HTTP redirects */
  BIT(connect_only); /* make connection/request, then let application use the
                        socket */
  BIT(connect_only_ws); /* special websocket connect-only level */
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
  BIT(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_MIME
  BIT(mime_formescape);
#यदि_अन्तः
  BIT(is_fread_समुच्चयः); /* has read callback been set to non-NULL? */
#यदि_अपरिभाषितम् CURL_DISABLE_TFTP
  BIT(tftp_no_options); /* do not send TFTP options requests */
#यदि_अन्तः
  BIT(sep_शीर्षकाणि);     /* handle host and proxy headers separately */
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  BIT(cookiesession);   /* new cookie session? */
#यदि_अन्तः
  BIT(crlf);            /* convert crlf on ftp upload(?) */
#यदि_परिभाषितम् USE_SSH
  BIT(ssh_compression);            /* enable SSH compression */
#यदि_अन्तः

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  BIT(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  BIT(get_filetime);     /* get the time and get of the remote file */
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  BIT(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#यदि_अन्तः
  BIT(prefer_ascii);     /* ASCII rather than binary */
  BIT(remote_append);    /* append, not overwrite, on upload */
#यदि_परिभाषितम् CURL_LIST_ONLY_PROTOCOL
  BIT(list_only);        /* list directory */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  BIT(ftp_use_पत्तनम्);     /* use the FTP PORT command */
  BIT(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  BIT(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  BIT(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  BIT(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  BIT(wildcard_enabled); /* enable wildcard matching */
#यदि_अन्तः
  BIT(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  BIT(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  BIT(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  BIT(allow_auth_गन्तव्यम्_other_hosts);
  BIT(include_शीर्षकम्); /* include received protocol headers in data output */
  BIT(http_समुच्चयः_referer); /* is a custom referer used */
  BIT(http_auto_referer); /* set "correct" referer when following
                             location: */
  BIT(opt_no_body);    /* as set with CURLOPT_NOBODY */
  BIT(verbose);        /* output verbosity */
  BIT(reuse_forbid);   /* forbidden to be reused, close after use */
  BIT(reuse_fresh);    /* do not reuse an existing connection */
  BIT(no_signal);      /* do not use any signal/alarm handler */
  BIT(tcp_nodelay);    /* whether to enable TCP_NODELAY or not */
  BIT(ignorecl);       /* ignore content length */
  BIT(http_te_skip);   /* pass the raw body data to the user, even when
                          transfer-encoded (chunked, compressed) */
  BIT(http_ce_skip);   /* pass the raw body data to the user, even when
                          content-encoded (chunked, compressed) */
  BIT(proxy_transfer_बहुलकम्); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  BIT(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#यदि_अन्तः
  BIT(socks5_auth_only); /* SOCKS5 must authenticate */
  BIT(sasl_ir);         /* Enable/disable SASL initial response */
  BIT(tcp_keepalive);  /* use TCP keepalives */
  BIT(tcp_fastopen);   /* use TCP Fast Open */
  BIT(ssl_enable_alpn);/* TLS ALPN extension? */
  BIT(मार्गः_as_is);     /* allow dotdots? */
  BIT(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  BIT(suppress_connect_शीर्षकाणि); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  BIT(dns_shuffle_स्थानाङ्काः); /* whether to shuffle addresses before use */
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  BIT(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
शीर्षकम्*/
#यदि_अन्तः
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  BIT(abstract_unix_socket);
#यदि_अन्तः
  BIT(disallow_username_अन्तर्_url); /* disallow username in URL */
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  BIT(doh); /* DNS-over-HTTPS enabled */
  BIT(doh_verifypeer);     /* DoH certificate peer verification */
  BIT(doh_verifyhost);     /* DoH certificate hostname verification */
  BIT(doh_verifystatus);   /* DoH certificate status verification */
#यदि_अन्तः
  BIT(http09_allowed); /* allow HTTP/0.9 responses */
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  BIT(ws_raw_बहुलकम्);
  BIT(ws_no_auto_pong);
#यदि_अन्तः
  BIT(post301); /* keep POSTs as POSTs after a 301 request */
  BIT(post302); /* keep POSTs as POSTs after a 302 request */
  BIT(post303); /* keep POSTs as POSTs after a 303 request */
#यदि_परिभाषितम् USE_OPENSSL
  BIT(engine_default);
#यदि_अन्तः
};

#यदि_अपरिभाषितम् CURL_DISABLE_MIME
#परिभाषय IS_MIME_POST(a)                                                 \
  ((a)->समुच्चयः.mimepostp && ((a)->समुच्चयः.mimepostp->विधा != MIMEKIND_NONE))
#अन्यथा
#परिभाषय IS_MIME_POST(a) FALSE
#यदि_अन्तः

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
प्रकारनिर्देश शून्य multi_sub_xfer_समाप्तम्_cb(संरचना Curl_easy *दत्त,
                                    संरचना Curl_easy *master,
                                    CURLcode परिणाम);

/*
 * The 'connectdata' struct MUST have all the connection oriented stuff as we
 * may have several simultaneous connections and connection structs in memory.
 *
 * The 'struct UserDefined' must only contain data that is set once to go for
 * many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" must be defined within the
 * 'struct UrlState' instead.
 */

संरचना Curl_easy {
  /* First a simple identifier to more easily detect if a user mixes up this
     easy handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  uint32_t magic;
  /* once an easy handle is added to a multi, either explicitly by the
   * libcurl application or implicitly during `curl_easy_perform()`,
   * a unique identifier inside this one multi instance. */
  uint32_t mid;
  CURLMstate mstate;  /* the handle's state */
  CURLcode परिणाम;   /* previous result */

  संरचना connectdata *conn;
  संरचना Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  संरचना Curl_eapi_राशिः callstack; /* easy api calls ongoing */

  संरचना Curl_share *share;    /* Share, handles global variable mutexing */

  संरचना Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  संरचना CURLMsg सन्देशः; /* A single posted message. */

  /* once an easy handle is tied to a connection pool a non-negative number to
     distinguish this transfer from other using the same pool. For easier
     tracking in log output. This may wrap around after CURL_OFF_T_MAX to 0
     again, so it has no uniqueness guarantee for large processings. Note: it
     has no uniqueness either IFF more than one connection pool is used by the
     libcurl application. */
  curl_off_t id;
  uint32_t master_mid; /* if set, this transfer belongs to a master */
  multi_sub_xfer_समाप्तम्_cb *sub_xfer_समाप्तम्;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  संरचना Curl_hash meta_hash;

#यदि_परिभाषितम् USE_LIBPSL
  संरचना PslCache *psl;        /* The associated PSL cache. */
#यदि_अन्तः
  संरचना SingleRequest req;    /* Request-specific data */
  संरचना UserDefined समुच्चयः;      /* values set by the libcurl user */
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  संरचना CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  संरचना hsts *hsts;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  संरचना altsvcinfo *asi;      /* the alt-svc cache */
#यदि_अन्तः
  संरचना प्रगतिः_2 प्रगतिः;    /* for all the progress meter data */
  संरचना UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  संरचना Wildcard_दत्त *wildcard; /* wildcard download state info */
#यदि_अन्तः
  संरचना विशुद्धम्_Info info;        /* stats, reports and info data */
  संरचना curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#परिभाषय CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->समुच्चयः.वाक्यानि, (uint8_t)(id))
#परिभाषय CURL_EASY_STR_समुच्चयः(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->समुच्चयः.वाक्यानि, (uint8_t)(id), s, slen)
#परिभाषय CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->समुच्चयः.वाक्यानि, (uint8_t)(id), s)
#परिभाषय CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->समुच्चयः.वाक्यानि, (uint8_t)(id))
#परिभाषय CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->समुच्चयः.वाक्यानि, (uint8_t)(id))

#परिभाषय LIBCURL_NAME "libcurl"

#यदि_अन्तः /* HEADER_CURL_URLDATA_H */
