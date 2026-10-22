#که_نه_تعریف_شوی سرلیک_CURL_URLDATA_H
#تعریف_کړه سرلیک_CURL_URLDATA_H
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
#ضمن "curl_setup.h"

#تعریف_کړه CURL_DEFAULT_USER     "anonymous"
#تعریف_کړه CURL_DEFAULT_PASSWORD "ftp@example.com"

#إذا !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#تعریف_کړه CURL_PREFER_LF_LINEENDS
#د_که_پای

#تعریف_کړه DEFAULT_CONNCACHE_اندازه 5

/* length of longest IPv6 address string including the null-terminator */
#تعریف_کړه MAX_IPADR_LEN حجم("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#تعریف_کړه CURL_MAX_ننوت_اوږدتیا 8000000

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#د_که_پای

#ضمن "curlx/timeval.h"

#ضمن "api.h"
#ضمن "cookie.h"
#ضمن "creds.h"
#ضمن "psl.h"
#ضمن "formdata.h"
#ضمن "http_chunks.h" /* for the structs and enum stuff */
#ضمن "hash.h"
#ضمن "peer.h"
#ضمن "proxy.h"
#ضمن "splay.h"
#ضمن "curlx/dynbuf.h"
#ضمن "bufref.h"
#ضمن "dynhds.h"
#ضمن "request.h"
#ضمن "ratelimit.h"
#ضمن "netrc.h"
#ضمن "u8_strset.h"
#ضمن "vdns/asyn.h"
#ضمن "vdns/hostip.h"
#ضمن "vtls/vtls_config.h"

/* On error return, the value of `pnwritten` has no meaning */
تعريفنوع CURLcode (Curl_send)(بنية Curl_easy *معلومات,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ثابت uint8_t *buf,       /* data to write */
                             size_t len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             size_t *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
تعريفنوع CURLcode (Curl_recv)(بنية Curl_easy *معلومات,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             حرف *buf,                /* store data here */
                             size_t len,               /* max amount to read */
                             size_t *pnread);          /* how much received */

#ضمن "mime.h"
#ضمن "protocol.h"
#ضمن "ftp.h"
#ضمن "http.h"
#ضمن "mqtt.h"
#ضمن "ftplistparser.h"
#ضمن "multihandle.h"
#ضمن "cf-socket.h"

#که_تعریف_شوی HAVE_GSSAPI
#  ifdef HAVE_GSSAPPLE
#    include <GSS/gssapi.h>
#    include <GSS/gssapi_oid.h>
#  elif defined(HAVE_GSSGNU)
#    include <gss.h>
#  elif defined(HAVE_GSSAPI_H)
#    include <gssapi.h>
#  وإلا /* MIT Kerberos */
#    include <gssapi/gssapi.h>
#    include <gssapi/gssapi_krb5.h> /* for GSS_C_CHANNEL_BOUND_FLAG in 1.19+ */
#  endif
#د_که_پای

#که_تعریف_شوی USE_LIBSSH2
#ضمن <libssh2.h>
#ضمن <libssh2_sftp.h>
#د_که_پای /* USE_LIBSSH2 */

#تعریف_کړه READBUFFER_اندازه CURL_MAX_WRITE_اندازه
#تعریف_کړه READBUFFER_MAX  CURL_MAX_READ_اندازه
#تعریف_کړه READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#تعریف_کړه UPLOADBUFFER_DEFAULT 65536
#تعریف_کړه UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#تعریف_کړه UPLOADBUFFER_MIN     CURL_MAX_WRITE_اندازه

#که_تعریف_شوی USE_WINDOWS_SSPI
#ضمن "curl_sspi.h"
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
بنية digestdata {
  بنية Curl_creds *creds;
  بنية Curl_peer *origin;
#که_تعریف_شوی USE_WINDOWS_SSPI
  BYTE *ننوت_token;
  size_t ننوت_token_len;
  CtxtHandle *http_context;
#وإلا
  حرف *nonce;
  حرف *cnonce;
  حرف *realm;
  حرف *opaque;
  حرف *qop;
  حرف *algorithm;
  صحيح nc; /* nonce count */
  uint8_t algo;
  بټ(stale); /* set true for re-negotiation */
  بټ(userhash);
#د_که_پای
};
#د_که_پای

تعريفنوع تعداد {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_وروستی
} curlntlm;

#تعریف_کړه CURL_NEGO_AUTH_NONE   0U
#تعریف_کړه CURL_NEGO_AUTH_RECV   1U
#تعریف_کړه CURL_NEGO_AUTH_لېږل_شوی   2U
#تعریف_کړه CURL_NEGO_AUTH_بشپړ   3U
#تعریف_کړه CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
بنية Connect_بټونه {
  بټ(connect_only);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بټ(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  بټ(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#د_که_پای
  /* always modify bits.close with the connclose() and connkeep() macros! */
  بټ(close); /* if set, we close the connection after this request */
  بټ(reuse); /* if set, this is a reused connection */
  بټ(altused); /* this is an alt-svc "redirect" */
  بټ(ipv6);    /* we communicate with a site using an IPv6 address */
  بټ(do_نور); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  بټ(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  بټ(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  بټ(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
غوښتنې*/
  بټ(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
غوښتنې*/
  بټ(ftp_use_معلومات_ssl); /* Enabled SSL for the data connection */
#د_که_پای
  بټ(bound); /* set true if bind() has already been done on this socket/
                 connection */
  بټ(upgrade_دننه_پرمختګ); /* protocol upgrade is in progress */
  بټ(multiplex); /* connection is multiplexed */
  بټ(tcp_fastopen); /* use TCP Fast Open */
  بټ(tls_enable_alpn); /* TLS ALPN extension? */
  بټ(sock_accepted); /* TRUE if the SECONDARYSOCKET was created with
                         accept() */
  بټ(parallel_connect); /* set TRUE when a parallel connect attempt has
                            started (happy eyeballs) */
  بټ(aborted); /* connection was aborted, e.g. in unclean state */
  بټ(no_reuse); /* connection should not be reused */
  بټ(shutdown_handler); /* connection shutdown: handler shut down */
  بټ(shutdown_filters); /* connection shutdown: filters shut down */
  بټ(dns_resolved); /* DNS records for connection were resolved */
};

بنية hostname {
  حرف *rawalloc; /* allocated "raw" version of the name */
  حرف *encalloc; /* allocated IDN-encoded version of the name */
  حرف *name;     /* name to use internally, might be encoded, might be raw */
  ثابت حرف *dispname; /* name to display, as 'name' might be encoded */
};

#تعریف_کړه FIRSTSOCKET     0
#تعریف_کړه SECONDARYSOCKET 1

#تعریف_کړه TRNSPRT_NONE 0
#تعریف_کړه TRNSPRT_TCP  3
#تعریف_کړه TRNSPRT_UDP  4
#تعریف_کړه TRNSPRT_QUIC 5
#تعریف_کړه TRNSPRT_UNIX 6

#تعریف_کړه TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

بنية ip_quadruple {
  حرف remote_ip[MAX_IPADR_LEN];
  حرف local_ip[MAX_IPADR_LEN];
  uint16_t remote_پورټ;
  uint16_t local_پورټ;
  uint8_t transport;
};

#تعریف_کړه CUR_IP_QUAD_HAS_PORTS(x)      \
  (((x)->transport == TRNSPRT_TCP) || \
   ((x)->transport == TRNSPRT_UDP) || \
   ((x)->transport == TRNSPRT_QUIC))

/*
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
بنية connectdata {
  curl_off_t connection_id; /* Contains a unique number to make it easier to
                               track the connections in the log output */

  /* A connection cache from a SHARE might be used in several multi handles.
   * We MUST not reuse connections that are running in another multi,
   * for concurrency reasons. That multi might run in another thread.
   * `attached_multi` is set by the first transfer attached and cleared
   * when the last one is detached.
   * NEVER call anything on this multi, check for equality. */
  بنية Curl_multi *attached_multi;

  /* Who the connection is talking to, ultimately */
  بنية Curl_peer *origin; /* connection ultimately talks to this */
  بنية Curl_peer *له_لارې_peer; /* if set, connection really talks to this */
  بنية Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  بنية Curl_peer *له_لارې_peer2; /* peer of SECONDARYSOCKET */
  بنية Curl_creds *creds; /* When connection itself is tied to credentials */
  بنية Curl_peer *creds_origin; /* origin tied credentials are for */
  ثابت بنية Curl_scheme *scheme; /* Connection's real protocol handler */

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the connection.
   * Elements need to be added with their own destructor to be invoked when
   * the connection is cleaned up (see Curl_hash_add2()).*/
  بنية Curl_hash meta_hash;

  حرف *destination; /* hostname+port, used in conncache */

  بنية curltime created; /* creation time */
  /* timediff_t are deltas from `created`*/
  timediff_t lastused_ms; /* when returned to the connection pool as idle */
  timediff_t lastchecked_ms; /* when last checked alive status */
  timediff_t lastupkeep_ms; /* when last done conn_upkeep */
  بنية {
    بنية curltime at[2]; /* first Curl_conn_connect() for each socket */
    bool سېټ[2];
  } connect_started;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بنية proxy_info socks_proxy;
  بنية proxy_info http_proxy;
#د_که_پای

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  بنية Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#تعریف_کړه CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  بنية {
    timediff_t پیل_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  فراغ *closesocket_client;

  بنية ssl_filter_config ssl_config;
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بنية ssl_filter_config proxy_ssl_config;
#د_که_پای
  حرف *options; /* options string, allocated */

  /*************** Request - specific items ************/
#که_تعریف_شوی USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#د_که_پای

#که_تعریف_شوی USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#د_که_پای

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  حرف *interface_opts;
  بنية Connect_بټونه بټونه;    /* various state-flags for this connection */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  صحيح socks5_gssapi_enctype;
#د_که_پای

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#تعریف_کړه CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#که_تعریف_شوی USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#د_که_پای
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_نسخه; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_کتل_شوی;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#که_تعریف_شوی USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#د_که_پای
};

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
#تعریف_کړه CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->له_لارې_peer ? (c)->له_لارې_peer->user_hostname : \
        (c)->origin->user_hostname)
#وإلا
#تعریف_کړه CURL_CONN_HOST_DISPNAME(c) \
  ((c)->له_لارې_peer ? (c)->له_لارې_peer->user_hostname : (c)->origin->user_hostname)
#د_که_پای

/* The end of connectdata. */

/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
بنية خالص_Info {
  /* PureInfo primary ip_quadruple is copied over from the connectdata
     struct in order to allow curl_easy_getinfo() to return this information
     even when the session handle is no longer associated with a connection,
     and also allow curl_easy_reset() to clear this information from the
     session handle without disturbing information which is still alive, and
     that might be reused, in the connection pool. */
  بنية ip_quadruple primary;
  بنية curl_certinfo certs; /* info about the certs. Asked for with
                                 CURLOPT_CERTINFO / CURLINFO_CERTINFO */
  time_t filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t request_اندازه; /* the amount of bytes sent in the request(s) */
  curl_off_t numconnects; /* how many new connections libcurl created */
  حرف *contenttype; /* the content type of the object */
  حرف *wouldredirect; /* URL this would have been redirected to if asked to */
  حرف *effective_url; /* when needed */
  curl_off_t retry_after; /* info from Retry-After: header */
  ثابت حرف *conn_scheme;
  صحيح httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  صحيح httpproxycode; /* response code from proxy when received separate */
  صحيح httpversion; /* the http version number X.Y = X*10+Y */
  uint32_t conn_protocol;
  uint32_t proxyauthavail; /* what proxy auth types were announced */
  uint32_t httpauthavail;  /* what host auth types were announced */
  uint32_t proxyauthpicked; /* selected proxy auth type */
  uint32_t httpauthpicked;  /* selected host auth type */
  uint32_t سرلیک_اندازه;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  بټ(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  بټ(کارول_شوی_proxy); /* the transfer used a proxy */
};

بنية pgrs_dir {
  curl_off_t ټول_اندازه; /* total expected bytes */
  curl_off_t cur_اندازه; /* transferred bytes so far */
  curl_off_t چټکتیا; /* bytes per second transferred */
  بنية Curl_rlimit rlimit; /* speed limiting / pausing */
};

بنية پرمختګ_2 {
  بنية curltime now; /* current time of processing */
  بنية curltime پیل; /* when transfer was initialized, set once */

  بنية pgrs_dir ul;
  بنية pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_چټکتیا; /* uses the currently fastest transfer */
  curl_off_t earlydata_لېږل_شوی;

  بنية {
    timediff_t startop_us; /* since start when operations started */
    timediff_t startsingle_us; /* since start when last request started */
    timediff_t startqueue_us; /* since start when last entered queueing */
    timediff_t startredirect_us; /* since start when last redirected */
    timediff_t lastshow_us; /* since start when last progress shown */
  } delta;
  بنية {
    timediff_t spent_us; /* all time spent since start */
    timediff_t queued_us; /* time spent since startsingle's for queueing */
    timediff_t nslookup_us; /* same for name resolves */
    timediff_t connect_us; /* same for connects */
    timediff_t appconnect_us; /* same for application connects, e.g. TLS */
    timediff_t pretransfer_us; /* same until requests were sent */
    timediff_t starttransfer_us; /* same until responses started */
    timediff_t posttransfer_us; /* same until responses ended */
  } ټول;

#تعریف_کړه CURL_چټکتیا_ریکارډونه (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t چټکتیا_amount[CURL_چټکتیا_ریکارډونه];
  timediff_t چټکتیا_وخت[CURL_چټکتیا_ریکارډونه];
  uint32_t speeder_c;
  بټ(hide);
  بټ(ul_اندازه_معلوم);
  بټ(dl_اندازه_معلوم);
  بټ(سرلیکونه_بهر); /* when the headers have been written */
  بټ(callback);  /* set when progress callback is used */
  بټ(startransfer_added);
};

بنية auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  بټ(بشپړ);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  بټ(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#که_تعریف_شوی USE_NGHTTP2
بنية Curl_معلومات_prio_غوټه {
  بنية Curl_معلومات_prio_غوټه *بل;
  بنية Curl_easy *معلومات;
};
#د_که_پای

/* Timers */
تعريفنوع تعداد {
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
  EXPIRE_وروستی /* not an actual timer, used as a marker only */
} expire_id;

بنية expire_timers {
  بنية Curl_ونه splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t دننه_واټن_us[EXPIRE_وروستی];
  uint8_t بل[EXPIRE_وروستی];
  uint8_t لومړی;
};

/* individual pieces of the URL */
بنية urlpieces {
  حرف *options;
  حرف *لار;
  حرف *query;
};

بنية UrlState {
  /* Origin of the initial (e.g. not followed) request of a transfer.
     Credentials from CURLOPT_* are only valid for this origin.
     Always set once a transfer starts searching for connections. */
  بنية Curl_peer *initial_origin;
  /* Current origin of the transfer, changes to origin of follow
   * requests. */
  بنية Curl_peer *origin;

  بنية curltime keeps_چټکتیا; /* for the progress meter really */
  بنية dynbuf headerb; /* buffer to store headers in */
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  بنية curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#د_که_پای

  صحيح os_errno;  /* filled in with errno whenever an error occurs */
  صحيح غوښتنې; /* request counter: redirects + authentication retakes */
#که_تعریف_شوی HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  فراغ (*prev_signal)(صحيح sig);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_DIGEST_AUTH
  حرف *envproxy; /* last proxy string used for proxy-related state */
  بنية digestdata digest;      /* state data for host Digest auth */
  بنية digestdata proxydigest; /* state data for proxy Digest auth */
#د_که_پای
  بنية auth authhost;  /* auth details for host */
  بنية auth authproxy; /* auth details for proxy */

#که_تعریف_شوی USE_CURL_ASYNC
  بنية Curl_resolv_async *async;  /* asynchronous name resolver data */
#د_که_پای

  بنية expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  حرف *most_recent_ftp_entrypath;
  حرف *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  فراغ *دننه;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  بنية urlpieces پورته;
  بنية bufref url;        /* work URL, initially copied from UserDefined */
  بنية bufref referer;    /* referer string */
  بنية curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#که_نه_تعریف_شوی CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#د_که_پای
  بنية Curl_llist httphdrs; /* received headers */
  بنية curl_سرلیک headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  بینښه صحيح nh_origin;
  صحيح nh_request;
  size_t nh_شمېر; /* httphdrs count when the cache was filled */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  بنية curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#د_که_پای

#که_تعریف_شوی CURLVERBOSE
  بنية curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_NETRC
  بنية store_netrc netrc;
#د_که_پای

  بنية Curl_creds *creds; /* Credentials for the origin only */

#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  حرف *rangeline; /* allocated */
  حرف *http_host; /* allocated */
  بنية http_negotiation http_neg;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  بنية Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_ماسک[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_بل_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_بل_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#د_که_پای

  curl_off_t وروستی_conn_id; /* The last assigned connection or -1 */
  uint32_t وروستی_cpid; /* last connection pool id used */

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  صحيح وزن; /* shallow copy of data->set */
#د_که_پای
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  بټ(really_alive); /* transfer is really alive in multi, passed INIT */
  بټ(this_is_a_follow); /* this is a followed Location: request */
  بټ(refused_بهیر); /* this was refused, try again */
  بټ(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  بټ(allow_پورټ); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  بټ(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  بټ(wildcardmatch); /* enable wildcard matching */
  بټ(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  بټ(use_range);
  بټ(rangestringalloc); /* the range string is malloc()'ed */
  بټ(بشپړ); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  بټ(cookie_engine);
#د_که_پای
  بټ(prefer_ascii);   /* ASCII rather than binary */
#که_تعریف_شوی CURL_LIST_ONLY_PROTOCOL
  بټ(list_only);      /* list directory contents */
#د_که_پای
  بټ(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  بټ(upload);         /* upload request */
  بټ(دننی); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  بټ(http_ignorecustom); /* ignore custom method from now */
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  بټ(http_hd_te); /* Added HTTP header TE: */
  بټ(http_hd_upgrade); /* Added HTTP header Upgrade: */
  بټ(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  بټ(maybe_folded);
  بټ(leading_unfold); /* unfold started, this is the leading bytes */
#د_که_پای
};

/*
 * This 'UserDefined' struct must only contain data that is set once to go
 * for many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" MUST be defined within the
 * 'struct UrlState' instead. The only exceptions MUST note the changes in
 * the 'DynamicStatic' struct.
 * Character pointer fields point to dynamic storage, unless otherwise stated.
 */

بنية Curl_multi;    /* declared in multihandle.c */

تعداد dupstring {
  STRING_CERT,            /* client certificate filename */
  STRING_CERT_TYPE,       /* format for certificate (default: PEM)*/
  STRING_کلی,             /* private key filename */
  STRING_کلی_PASSWD,      /* plain text private key password */
  STRING_کلی_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_کلی_PROXY,       /* private key filename */
  STRING_کلی_PASSWD_PROXY, /* plain text private key password */
  STRING_کلی_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#د_که_پای
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_منزل_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#د_که_پای
  STRING_سېټ_RANGE,       /* range, if used */
  STRING_سېټ_REFERER,     /* custom string for the HTTP referer field */
  STRING_سېټ_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
کارول_شوی*/
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_بهیر_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#د_که_پای
#که_تعریف_شوی USE_SSH
  STRING_SSH_شخصي_کلی, /* path to the private key file for auth */
  STRING_SSH_عامه_کلی,  /* path to the public key file for auth */
  STRING_SSH_HOST_عامه_کلی_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_عامه_کلی_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#د_که_پای
  STRING_BEARER,                /* <bearer>, if used */
#که_تعریف_شوی USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_لار,      /* path to Unix socket, if used */
#د_که_پای
  STRING_موخه,                /* CURLOPT_REQUEST_TARGET */
#که_نه_تعریف_شوی CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#د_که_پای
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#که_تعریف_شوی USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#د_که_پای
  STRING_SSL_EC_CURVES,
#که_نه_تعریف_شوی CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_کلی,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_سرلیکونه, /* space-separated components to sign */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#د_که_پای
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_عامه,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_وروستی /* not used, an end-of-list marker */
};

تعداد dupblob {
  BLOB_CERT,
  BLOB_کلی,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_کلی_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#د_که_پای
  BLOB_وروستی
};

بنية UserDefined {
  FILE *err;         /* the stderr user data goes here */
  فراغ *debugdata;   /* the data that will be passed to fdebug */
  حرف *errorbuffer; /* (Static) store failure messages in here */
  فراغ *بهر;         /* CURLOPT_WRITEDATA */
  فراغ *دننه_سېټ;      /* CURLOPT_READDATA */
  فراغ *writeheader; /* write the header to this if non-NULL */
  uint32_t httpauth;  /* kind of HTTP authentication to use (bitmask) */
  uint32_t proxyauth; /* kind of proxy authentication to use (bitmask) */
  فراغ *postfields;  /* if POST, set the fields' values here */
  حرف *str_copypostfields; /* CURLOPT_COPYPOSTFIELDS value */
  curl_seek_callback seek_func;      /* function that seeks the input */
  curl_off_t postfieldsize; /* if POST, this might have a size to use instead
                               of strlen(), and then the data *may* be binary
                               (contain NUL bytes) */
  curl_write_callback fwrite_func;   /* function that stores the output */
  curl_write_callback fwrite_سرلیک; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_سېټ; /* function that reads the input */
  curl_پرمختګ_callback fprogress; /* OLD and deprecated progress callback */
  curl_xferinfo_callback fxferinfo; /* progress callback */
  curl_debug_callback fdebug;      /* function that write informational data */
  curl_ioctl_callback ioctl_func;  /* function for I/O control */
  curl_sockopt_callback fsockopt;  /* function for setting socket options */
  فراغ *sockopt_client; /* pointer to pass to the socket options callback */
  curl_opensocket_callback fopensocket; /* function for checking/translating
                                           the address and opening the
                                           socket */
  فراغ *opensocket_client;
  curl_closesocket_callback fclosesocket; /* function for closing the
                                             socket */
  فراغ *closesocket_client;
  curl_prereq_callback fprereq; /* pre-initial request callback */
  فراغ *prereq_userp; /* pre-initial request user data */

  فراغ *seek_client;    /* pointer to pass to the seek callback */
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  فراغ *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  فراغ *hsts_write_userp;
#د_که_پای
  فراغ *پرمختګ_client; /* pointer to pass to the progress callback */
  فراغ *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t ټیټ_چټکتیا_حد; /* bytes/second */
  curl_off_t max_send_چټکتیا; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_چټکتیا; /* high speed limit in bytes/second for
                                download */
  curl_off_t سېټ_resume_from;  /* continue [ftp] transfer from here */
  بنية curl_slist *سرلیکونه; /* linked list of extra headers */
  بنية curl_httppost *httppost;  /* linked list of old POST data */
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_TELNET
  بنية curl_slist *telnet_options; /* linked list of telnet options */
#د_که_پای
  بنية curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  بنية curl_slist *connect_منزل; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  time_t timevalue;       /* what time to compare with */
  بنية ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  فراغ *ssl_fsslctxp;        /* parameter for callback */
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بنية ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  بنية curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#د_که_پای
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t بفر_اندازه;        /* size of receive buffer to use */
  uint32_t upload_بفر_اندازه; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  فراغ *شخصي_معلومات; /* application-private data */
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  بنية curl_slist *http200aliases; /* linked list of aliases for http200 */
#د_که_پای
  curl_off_t max_filesize; /* Maximum file size to download */
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#د_که_پای
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  بنية curl_slist *quote;     /* after connection is established */
  بنية curl_slist *postquote; /* after the transfer */
  بنية curl_slist *prequote; /* before the transfer, after type */
#د_که_پای
#که_تعریف_شوی USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  فراغ *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#د_که_پای
#که_تعریف_شوی USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  فراغ *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#د_که_پای
  بنية u8_strset مزي_ګانې;
  بنية curl_blob *blobs[BLOB_وروستی];
  uint32_t new_file_perms;      /* when creating remote files */
#که_تعریف_شوی USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#د_که_پای
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  فراغ *rtp_بهر;     /* write RTP to this if non-NULL */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_پای_ټکی_callback chunk_پای_ټکی; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  فراغ *fnmatch_معلومات;
  فراغ *wildcardptr;
#د_که_پای

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_پیل_callback resolver_پیل; /* optional callback called
                                                  before resolver start */
  فراغ *resolver_پیل_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  فراغ *trailer_معلومات; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  بنية curl_slist *mail_rcpt; /* linked list of mail recipients */
#د_که_پای
  صحيح ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  صحيح tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  صحيح tcp_keepintvl;    /* seconds between TCP keepalive probes */
  صحيح tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  صحيح وزن;
#د_که_پای
  لنډ maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_پورټ; /* which port to use (when not using default) */
  uint16_t ټیټ_چټکتیا_وخت;  /* number of seconds */
#که_نه_تعریف_شوی CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#د_که_پای
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#د_که_پای
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t لاره;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#د_که_پای
  uint8_t http_follow_ډېرپېښ; /* follow HTTP redirects */
  بټ(connect_only); /* make connection/request, then let application use the
                        socket */
  بټ(connect_only_ws); /* special websocket connect-only level */
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  بټ(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_MIME
  بټ(mime_formescape);
#د_که_پای
  بټ(is_fread_سېټ); /* has read callback been set to non-NULL? */
#که_نه_تعریف_شوی CURL_DISABLE_TFTP
  بټ(tftp_no_options); /* do not send TFTP options requests */
#د_که_پای
  بټ(sep_سرلیکونه);     /* handle host and proxy headers separately */
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  بټ(cookiesession);   /* new cookie session? */
#د_که_پای
  بټ(crlf);            /* convert crlf on ftp upload(?) */
#که_تعریف_شوی USE_SSH
  بټ(ssh_compression);            /* enable SSH compression */
#د_که_پای

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  بټ(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  بټ(get_filetime);     /* get the time and get of the remote file */
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بټ(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#د_که_پای
  بټ(prefer_ascii);     /* ASCII rather than binary */
  بټ(remote_append);    /* append, not overwrite, on upload */
#که_تعریف_شوی CURL_LIST_ONLY_PROTOCOL
  بټ(list_only);        /* list directory */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  بټ(ftp_use_پورټ);     /* use the FTP PORT command */
  بټ(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  بټ(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  بټ(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  بټ(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  بټ(wildcard_enabled); /* enable wildcard matching */
#د_که_پای
  بټ(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  بټ(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  بټ(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  بټ(allow_auth_منزل_other_hosts);
  بټ(include_سرلیک); /* include received protocol headers in data output */
  بټ(http_سېټ_referer); /* is a custom referer used */
  بټ(http_auto_referer); /* set "correct" referer when following
                             location: */
  بټ(opt_no_body);    /* as set with CURLOPT_NOBODY */
  بټ(verbose);        /* output verbosity */
  بټ(reuse_forbid);   /* forbidden to be reused, close after use */
  بټ(reuse_fresh);    /* do not reuse an existing connection */
  بټ(no_signal);      /* do not use any signal/alarm handler */
  بټ(tcp_nodelay);    /* whether to enable TCP_NODELAY or not */
  بټ(ignorecl);       /* ignore content length */
  بټ(http_te_skip);   /* pass the raw body data to the user, even when
                          transfer-encoded (chunked, compressed) */
  بټ(http_ce_skip);   /* pass the raw body data to the user, even when
                          content-encoded (chunked, compressed) */
  بټ(proxy_transfer_ډېرپېښ); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  بټ(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#د_که_پای
  بټ(socks5_auth_only); /* SOCKS5 must authenticate */
  بټ(sasl_ir);         /* Enable/disable SASL initial response */
  بټ(tcp_keepalive);  /* use TCP keepalives */
  بټ(tcp_fastopen);   /* use TCP Fast Open */
  بټ(ssl_enable_alpn);/* TLS ALPN extension? */
  بټ(لار_as_is);     /* allow dotdots? */
  بټ(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  بټ(suppress_connect_سرلیکونه); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  بټ(dns_shuffle_پتې); /* whether to shuffle addresses before use */
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بټ(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
سرلیک*/
#د_که_پای
#که_تعریف_شوی USE_UNIX_SOCKETS
  بټ(abstract_unix_socket);
#د_که_پای
  بټ(disallow_username_دننه_url); /* disallow username in URL */
#که_نه_تعریف_شوی CURL_DISABLE_DOH
  بټ(doh); /* DNS-over-HTTPS enabled */
  بټ(doh_verifypeer);     /* DoH certificate peer verification */
  بټ(doh_verifyhost);     /* DoH certificate hostname verification */
  بټ(doh_verifystatus);   /* DoH certificate status verification */
#د_که_پای
  بټ(http09_allowed); /* allow HTTP/0.9 responses */
#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS
  بټ(ws_raw_ډېرپېښ);
  بټ(ws_no_auto_pong);
#د_که_پای
  بټ(post301); /* keep POSTs as POSTs after a 301 request */
  بټ(post302); /* keep POSTs as POSTs after a 302 request */
  بټ(post303); /* keep POSTs as POSTs after a 303 request */
#که_تعریف_شوی USE_OPENSSL
  بټ(engine_default);
#د_که_پای
};

#که_نه_تعریف_شوی CURL_DISABLE_MIME
#تعریف_کړه IS_MIME_POST(a)                                                 \
  ((a)->سېټ.mimepostp && ((a)->سېټ.mimepostp->ډول != MIMEKIND_NONE))
#وإلا
#تعریف_کړه IS_MIME_POST(a) FALSE
#د_که_پای

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
تعريفنوع فراغ multi_sub_xfer_بشپړ_cb(بنية Curl_easy *معلومات,
                                    بنية Curl_easy *master,
                                    CURLcode پایله);

/*
 * The 'connectdata' struct MUST have all the connection oriented stuff as we
 * may have several simultaneous connections and connection structs in memory.
 *
 * The 'struct UserDefined' must only contain data that is set once to go for
 * many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" must be defined within the
 * 'struct UrlState' instead.
 */

بنية Curl_easy {
  /* First a simple identifier to more easily detect if a user mixes up this
     easy handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  uint32_t magic;
  /* once an easy handle is added to a multi, either explicitly by the
   * libcurl application or implicitly during `curl_easy_perform()`,
   * a unique identifier inside this one multi instance. */
  uint32_t mid;
  CURLMstate mstate;  /* the handle's state */
  CURLcode پایله;   /* previous result */

  بنية connectdata *conn;
  بنية Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  بنية Curl_eapi_ډېری callstack; /* easy api calls ongoing */

  بنية Curl_share *share;    /* Share, handles global variable mutexing */

  بنية Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  بنية CURLMsg پیغام; /* A single posted message. */

  /* once an easy handle is tied to a connection pool a non-negative number to
     distinguish this transfer from other using the same pool. For easier
     tracking in log output. This may wrap around after CURL_OFF_T_MAX to 0
     again, so it has no uniqueness guarantee for large processings. Note: it
     has no uniqueness either IFF more than one connection pool is used by the
     libcurl application. */
  curl_off_t id;
  uint32_t master_mid; /* if set, this transfer belongs to a master */
  multi_sub_xfer_بشپړ_cb *sub_xfer_بشپړ;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  بنية Curl_hash meta_hash;

#که_تعریف_شوی USE_LIBPSL
  بنية PslCache *psl;        /* The associated PSL cache. */
#د_که_پای
  بنية SingleRequest req;    /* Request-specific data */
  بنية UserDefined سېټ;      /* values set by the libcurl user */
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  بنية CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  بنية hsts *hsts;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  بنية altsvcinfo *asi;      /* the alt-svc cache */
#د_که_پای
  بنية پرمختګ_2 پرمختګ;    /* for all the progress meter data */
  بنية UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  بنية Wildcard_معلومات *wildcard; /* wildcard download state info */
#د_که_پای
  بنية خالص_Info info;        /* stats, reports and info data */
  بنية curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#تعریف_کړه CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->سېټ.مزي_ګانې, (uint8_t)(id))
#تعریف_کړه CURL_EASY_STR_سېټ(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->سېټ.مزي_ګانې, (uint8_t)(id), s, slen)
#تعریف_کړه CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->سېټ.مزي_ګانې, (uint8_t)(id), s)
#تعریف_کړه CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->سېټ.مزي_ګانې, (uint8_t)(id))
#تعریف_کړه CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->سېټ.مزي_ګانې, (uint8_t)(id))

#تعریف_کړه LIBCURL_NAME "libcurl"

#د_که_پای /* HEADER_CURL_URLDATA_H */
