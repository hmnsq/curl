#جے_بنا_تعریف سرخی_CURL_URLDATA_H
#تعریف_کرو سرخی_CURL_URLDATA_H
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

#تعریف_کرو CURL_DEFAULT_USER     "anonymous"
#تعریف_کرو CURL_DEFAULT_PASSWORD "ftp@example.com"

#إذا !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#تعریف_کرو CURL_PREFER_LF_LINEENDS
#جے_انت

#تعریف_کرو DEFAULT_CONNCACHE_ناپ 5

/* length of longest IPv6 address string including the null-terminator */
#تعریف_کرو MAX_IPADR_LEN حجم("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#تعریف_کرو CURL_MAX_داخلہ_لمائی 8000000

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#جے_انت

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
تعريفنوع CURLcode (Curl_send)(بنية Curl_easy *انکڑے,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ثابت uint8_t *buf,       /* data to write */
                             size_t len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             size_t *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
تعريفنوع CURLcode (Curl_recv)(بنية Curl_easy *انکڑے,   /* transfer */
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

#جے_تعریف_شدہ HAVE_GSSAPI
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
#جے_انت

#جے_تعریف_شدہ USE_LIBSSH2
#ضمن <libssh2.h>
#ضمن <libssh2_sftp.h>
#جے_انت /* USE_LIBSSH2 */

#تعریف_کرو READBUFFER_ناپ CURL_MAX_WRITE_ناپ
#تعریف_کرو READBUFFER_MAX  CURL_MAX_READ_ناپ
#تعریف_کرو READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#تعریف_کرو UPLOADBUFFER_DEFAULT 65536
#تعریف_کرو UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#تعریف_کرو UPLOADBUFFER_MIN     CURL_MAX_WRITE_ناپ

#جے_تعریف_شدہ USE_WINDOWS_SSPI
#ضمن "curl_sspi.h"
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
بنية digestdata {
  بنية Curl_creds *creds;
  بنية Curl_peer *origin;
#جے_تعریف_شدہ USE_WINDOWS_SSPI
  BYTE *داخلہ_token;
  size_t داخلہ_token_len;
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
  ثنائی_ہندسہ(stale); /* set true for re-negotiation */
  ثنائی_ہندسہ(userhash);
#جے_انت
};
#جے_انت

تعريفنوع تعداد {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_آخری
} curlntlm;

#تعریف_کرو CURL_NEGO_AUTH_NONE   0U
#تعریف_کرو CURL_NEGO_AUTH_RECV   1U
#تعریف_کرو CURL_NEGO_AUTH_گھلیا   2U
#تعریف_کرو CURL_NEGO_AUTH_پورا   3U
#تعریف_کرو CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
بنية Connect_بٹ {
  ثنائی_ہندسہ(connect_only);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  ثنائی_ہندسہ(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  ثنائی_ہندسہ(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#جے_انت
  /* always modify bits.close with the connclose() and connkeep() macros! */
  ثنائی_ہندسہ(close); /* if set, we close the connection after this request */
  ثنائی_ہندسہ(reuse); /* if set, this is a reused connection */
  ثنائی_ہندسہ(altused); /* this is an alt-svc "redirect" */
  ثنائی_ہندسہ(ipv6);    /* we communicate with a site using an IPv6 address */
  ثنائی_ہندسہ(do_ہور); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  ثنائی_ہندسہ(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  ثنائی_ہندسہ(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#جے_بنا_تعریف CURL_DISABLE_FTP
  ثنائی_ہندسہ(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
بینتیاں*/
  ثنائی_ہندسہ(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
بینتیاں*/
  ثنائی_ہندسہ(ftp_use_انکڑے_ssl); /* Enabled SSL for the data connection */
#جے_انت
  ثنائی_ہندسہ(bound); /* set true if bind() has already been done on this socket/
                 connection */
  ثنائی_ہندسہ(upgrade_وچ_ترقی); /* protocol upgrade is in progress */
  ثنائی_ہندسہ(multiplex); /* connection is multiplexed */
  ثنائی_ہندسہ(tcp_fastopen); /* use TCP Fast Open */
  ثنائی_ہندسہ(tls_enable_alpn); /* TLS ALPN extension? */
  ثنائی_ہندسہ(sock_accepted); /* TRUE if the SECONDARYSOCKET was created with
                         accept() */
  ثنائی_ہندسہ(parallel_connect); /* set TRUE when a parallel connect attempt has
                            started (happy eyeballs) */
  ثنائی_ہندسہ(aborted); /* connection was aborted, e.g. in unclean state */
  ثنائی_ہندسہ(no_reuse); /* connection should not be reused */
  ثنائی_ہندسہ(shutdown_handler); /* connection shutdown: handler shut down */
  ثنائی_ہندسہ(shutdown_filters); /* connection shutdown: filters shut down */
  ثنائی_ہندسہ(dns_resolved); /* DNS records for connection were resolved */
};

بنية hostname {
  حرف *rawalloc; /* allocated "raw" version of the name */
  حرف *encalloc; /* allocated IDN-encoded version of the name */
  حرف *name;     /* name to use internally, might be encoded, might be raw */
  ثابت حرف *dispname; /* name to display, as 'name' might be encoded */
};

#تعریف_کرو FIRSTSOCKET     0
#تعریف_کرو SECONDARYSOCKET 1

#تعریف_کرو TRNSPRT_NONE 0
#تعریف_کرو TRNSPRT_TCP  3
#تعریف_کرو TRNSPRT_UDP  4
#تعریف_کرو TRNSPRT_QUIC 5
#تعریف_کرو TRNSPRT_UNIX 6

#تعریف_کرو TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

بنية ip_quadruple {
  حرف remote_ip[MAX_IPADR_LEN];
  حرف local_ip[MAX_IPADR_LEN];
  uint16_t remote_پورٹ;
  uint16_t local_پورٹ;
  uint8_t transport;
};

#تعریف_کرو CUR_IP_QUAD_HAS_PORTS(x)      \
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
  بنية Curl_peer *راہیں_peer; /* if set, connection really talks to this */
  بنية Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  بنية Curl_peer *راہیں_peer2; /* peer of SECONDARYSOCKET */
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
    bool سیٹ[2];
  } connect_started;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  بنية proxy_info socks_proxy;
  بنية proxy_info http_proxy;
#جے_انت

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  بنية Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#تعریف_کرو CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  بنية {
    timediff_t شروع_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  فراغ *closesocket_client;

  بنية ssl_filter_config ssl_config;
#جے_بنا_تعریف CURL_DISABLE_PROXY
  بنية ssl_filter_config proxy_ssl_config;
#جے_انت
  حرف *options; /* options string, allocated */

  /*************** Request - specific items ************/
#جے_تعریف_شدہ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#جے_انت

#جے_تعریف_شدہ USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#جے_انت

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  حرف *interface_opts;
  بنية Connect_بٹ بٹ;    /* various state-flags for this connection */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  صحيح socks5_gssapi_enctype;
#جے_انت

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#تعریف_کرو CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#جے_تعریف_شدہ USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#جے_انت
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_نسخہ; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_تکیا;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#جے_تعریف_شدہ USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#جے_انت
};

#جے_بنا_تعریف CURL_DISABLE_PROXY
#تعریف_کرو CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->راہیں_peer ? (c)->راہیں_peer->user_hostname : \
        (c)->origin->user_hostname)
#وإلا
#تعریف_کرو CURL_CONN_HOST_DISPNAME(c) \
  ((c)->راہیں_peer ? (c)->راہیں_peer->user_hostname : (c)->origin->user_hostname)
#جے_انت

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
  curl_off_t request_ناپ; /* the amount of bytes sent in the request(s) */
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
  uint32_t سرخی_ناپ;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  ثنائی_ہندسہ(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  ثنائی_ہندسہ(ورتیا_proxy); /* the transfer used a proxy */
};

بنية pgrs_dir {
  curl_off_t کل_ناپ; /* total expected bytes */
  curl_off_t cur_ناپ; /* transferred bytes so far */
  curl_off_t رفتار; /* bytes per second transferred */
  بنية Curl_rlimit rlimit; /* speed limiting / pausing */
};

بنية ترقی_2 {
  بنية curltime now; /* current time of processing */
  بنية curltime شروع; /* when transfer was initialized, set once */

  بنية pgrs_dir ul;
  بنية pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_رفتار; /* uses the currently fastest transfer */
  curl_off_t earlydata_گھلیا;

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
  } کل;

#تعریف_کرو CURL_رفتار_ریکارڈاں (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t رفتار_amount[CURL_رفتار_ریکارڈاں];
  timediff_t رفتار_ویلا[CURL_رفتار_ریکارڈاں];
  uint32_t speeder_c;
  ثنائی_ہندسہ(hide);
  ثنائی_ہندسہ(ul_ناپ_معلوم);
  ثنائی_ہندسہ(dl_ناپ_معلوم);
  ثنائی_ہندسہ(سرخیاں_باہرلا); /* when the headers have been written */
  ثنائی_ہندسہ(callback);  /* set when progress callback is used */
  ثنائی_ہندسہ(startransfer_added);
};

بنية auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  ثنائی_ہندسہ(پورا);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  ثنائی_ہندسہ(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#جے_تعریف_شدہ USE_NGHTTP2
بنية Curl_انکڑے_prio_گرہ {
  بنية Curl_انکڑے_prio_گرہ *اگلا;
  بنية Curl_easy *انکڑے;
};
#جے_انت

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
  EXPIRE_آخری /* not an actual timer, used as a marker only */
} expire_id;

بنية expire_timers {
  بنية Curl_درخت splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t وتھ_us[EXPIRE_آخری];
  uint8_t اگلا[EXPIRE_آخری];
  uint8_t پہلا;
};

/* individual pieces of the URL */
بنية urlpieces {
  حرف *options;
  حرف *رستہ;
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

  بنية curltime keeps_رفتار; /* for the progress meter really */
  بنية dynbuf headerb; /* buffer to store headers in */
#جے_بنا_تعریف CURL_DISABLE_HSTS
  بنية curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#جے_انت

  صحيح os_errno;  /* filled in with errno whenever an error occurs */
  صحيح بینتیاں; /* request counter: redirects + authentication retakes */
#جے_تعریف_شدہ HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  فراغ (*prev_signal)(صحيح sig);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
  حرف *envproxy; /* last proxy string used for proxy-related state */
  بنية digestdata digest;      /* state data for host Digest auth */
  بنية digestdata proxydigest; /* state data for proxy Digest auth */
#جے_انت
  بنية auth authhost;  /* auth details for host */
  بنية auth authproxy; /* auth details for proxy */

#جے_تعریف_شدہ USE_CURL_ASYNC
  بنية Curl_resolv_async *async;  /* asynchronous name resolver data */
#جے_انت

  بنية expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  حرف *most_recent_ftp_entrypath;
  حرف *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  فراغ *وچ;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  بنية urlpieces اتے;
  بنية bufref url;        /* work URL, initially copied from UserDefined */
  بنية bufref referer;    /* referer string */
  بنية curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#جے_بنا_تعریف CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#جے_بنا_تعریف CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#جے_انت
  بنية Curl_llist httphdrs; /* received headers */
  بنية curl_سرخی headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  بنانشانی صحيح nh_origin;
  صحيح nh_request;
  size_t nh_گنتی; /* httphdrs count when the cache was filled */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  بنية curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#جے_انت

#جے_تعریف_شدہ CURLVERBOSE
  بنية curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_NETRC
  بنية store_netrc netrc;
#جے_انت

  بنية Curl_creds *creds; /* Credentials for the origin only */

#جے_بنا_تعریف CURL_DISABLE_HTTP
  حرف *rangeline; /* allocated */
  حرف *http_host; /* allocated */
  بنية http_negotiation http_neg;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  بنية Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_نقاب[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_اگلا_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_اگلا_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#جے_انت

  curl_off_t آخری_conn_id; /* The last assigned connection or -1 */
  uint32_t آخری_cpid; /* last connection pool id used */

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  صحيح بھار; /* shallow copy of data->set */
#جے_انت
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  ثنائی_ہندسہ(really_alive); /* transfer is really alive in multi, passed INIT */
  ثنائی_ہندسہ(this_is_a_follow); /* this is a followed Location: request */
  ثنائی_ہندسہ(refused_دھارا); /* this was refused, try again */
  ثنائی_ہندسہ(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  ثنائی_ہندسہ(allow_پورٹ); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  ثنائی_ہندسہ(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  ثنائی_ہندسہ(wildcardmatch); /* enable wildcard matching */
  ثنائی_ہندسہ(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  ثنائی_ہندسہ(use_range);
  ثنائی_ہندسہ(rangestringalloc); /* the range string is malloc()'ed */
  ثنائی_ہندسہ(پورا); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  ثنائی_ہندسہ(cookie_engine);
#جے_انت
  ثنائی_ہندسہ(prefer_ascii);   /* ASCII rather than binary */
#جے_تعریف_شدہ CURL_LIST_ONLY_PROTOCOL
  ثنائی_ہندسہ(list_only);      /* list directory contents */
#جے_انت
  ثنائی_ہندسہ(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  ثنائی_ہندسہ(upload);         /* upload request */
  ثنائی_ہندسہ(اندرونی); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  ثنائی_ہندسہ(http_ignorecustom); /* ignore custom method from now */
#جے_بنا_تعریف CURL_DISABLE_HTTP
  ثنائی_ہندسہ(http_hd_te); /* Added HTTP header TE: */
  ثنائی_ہندسہ(http_hd_upgrade); /* Added HTTP header Upgrade: */
  ثنائی_ہندسہ(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  ثنائی_ہندسہ(maybe_folded);
  ثنائی_ہندسہ(leading_unfold); /* unfold started, this is the leading bytes */
#جے_انت
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
  STRING_کلید,             /* private key filename */
  STRING_کلید_PASSWD,      /* plain text private key password */
  STRING_کلید_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#جے_بنا_تعریف CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_کلید_PROXY,       /* private key filename */
  STRING_کلید_PASSWD_PROXY, /* plain text private key password */
  STRING_کلید_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#جے_انت
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#جے_بنا_تعریف CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_منزل_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#جے_انت
  STRING_سیٹ_RANGE,       /* range, if used */
  STRING_سیٹ_REFERER,     /* custom string for the HTTP referer field */
  STRING_سیٹ_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#جے_بنا_تعریف CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
ورتیا*/
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_دھارا_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#جے_انت
#جے_تعریف_شدہ USE_SSH
  STRING_SSH_نجی_کلید, /* path to the private key file for auth */
  STRING_SSH_عوامی_کلید,  /* path to the public key file for auth */
  STRING_SSH_HOST_عوامی_کلید_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_عوامی_کلید_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#جے_انت
  STRING_BEARER,                /* <bearer>, if used */
#جے_تعریف_شدہ USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_رستہ,      /* path to Unix socket, if used */
#جے_انت
  STRING_نشانہ,                /* CURLOPT_REQUEST_TARGET */
#جے_بنا_تعریف CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#جے_انت
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#جے_تعریف_شدہ USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#جے_انت
  STRING_SSL_EC_CURVES,
#جے_بنا_تعریف CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_کلید,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_سرخیاں, /* space-separated components to sign */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#جے_انت
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_عوامی,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_آخری /* not used, an end-of-list marker */
};

تعداد dupblob {
  BLOB_CERT,
  BLOB_کلید,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#جے_بنا_تعریف CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_کلید_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#جے_انت
  BLOB_آخری
};

بنية UserDefined {
  FILE *err;         /* the stderr user data goes here */
  فراغ *debugdata;   /* the data that will be passed to fdebug */
  حرف *errorbuffer; /* (Static) store failure messages in here */
  فراغ *باہرلا;         /* CURLOPT_WRITEDATA */
  فراغ *وچ_سیٹ;      /* CURLOPT_READDATA */
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
  curl_write_callback fwrite_سرخی; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_سیٹ; /* function that reads the input */
  curl_ترقی_callback fprogress; /* OLD and deprecated progress callback */
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
#جے_بنا_تعریف CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  فراغ *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  فراغ *hsts_write_userp;
#جے_انت
  فراغ *ترقی_client; /* pointer to pass to the progress callback */
  فراغ *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t کم_رفتار_حد_بندی; /* bytes/second */
  curl_off_t max_send_رفتار; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_رفتار; /* high speed limit in bytes/second for
                                download */
  curl_off_t سیٹ_resume_from;  /* continue [ftp] transfer from here */
  بنية curl_slist *سرخیاں; /* linked list of extra headers */
  بنية curl_httppost *httppost;  /* linked list of old POST data */
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_TELNET
  بنية curl_slist *telnet_options; /* linked list of telnet options */
#جے_انت
  بنية curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  بنية curl_slist *connect_منزل; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  time_t timevalue;       /* what time to compare with */
  بنية ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  فراغ *ssl_fsslctxp;        /* parameter for callback */
#جے_بنا_تعریف CURL_DISABLE_PROXY
  بنية ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  بنية curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#جے_انت
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t بفر_ناپ;        /* size of receive buffer to use */
  uint32_t upload_بفر_ناپ; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  فراغ *نجی_انکڑے; /* application-private data */
#جے_بنا_تعریف CURL_DISABLE_HTTP
  بنية curl_slist *http200aliases; /* linked list of aliases for http200 */
#جے_انت
  curl_off_t max_filesize; /* Maximum file size to download */
#جے_بنا_تعریف CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#جے_انت
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  بنية curl_slist *quote;     /* after connection is established */
  بنية curl_slist *postquote; /* after the transfer */
  بنية curl_slist *prequote; /* before the transfer, after type */
#جے_انت
#جے_تعریف_شدہ USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  فراغ *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#جے_انت
#جے_تعریف_شدہ USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  فراغ *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#جے_انت
  بنية u8_strset لڑیاں;
  بنية curl_blob *blobs[BLOB_آخری];
  uint32_t new_file_perms;      /* when creating remote files */
#جے_تعریف_شدہ USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#جے_انت
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#جے_بنا_تعریف CURL_DISABLE_RTSP
  فراغ *rtp_باہرلا;     /* write RTP to this if non-NULL */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_سرا_callback chunk_سرا; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  فراغ *fnmatch_انکڑے;
  فراغ *wildcardptr;
#جے_انت

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_شروع_callback resolver_شروع; /* optional callback called
                                                  before resolver start */
  فراغ *resolver_شروع_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#جے_بنا_تعریف CURL_DISABLE_HTTP
  فراغ *trailer_انکڑے; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_SMTP
  بنية curl_slist *mail_rcpt; /* linked list of mail recipients */
#جے_انت
  صحيح ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  صحيح tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  صحيح tcp_keepintvl;    /* seconds between TCP keepalive probes */
  صحيح tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  صحيح بھار;
#جے_انت
  چھوٹا maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_پورٹ; /* which port to use (when not using default) */
  uint16_t کم_رفتار_ویلا;  /* number of seconds */
#جے_بنا_تعریف CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#جے_انت
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#جے_انت
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t طریقہ;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#جے_انت
  uint8_t http_follow_منوال; /* follow HTTP redirects */
  ثنائی_ہندسہ(connect_only); /* make connection/request, then let application use the
                        socket */
  ثنائی_ہندسہ(connect_only_ws); /* special websocket connect-only level */
#جے_بنا_تعریف CURL_DISABLE_SMTP
  ثنائی_ہندسہ(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_MIME
  ثنائی_ہندسہ(mime_formescape);
#جے_انت
  ثنائی_ہندسہ(is_fread_سیٹ); /* has read callback been set to non-NULL? */
#جے_بنا_تعریف CURL_DISABLE_TFTP
  ثنائی_ہندسہ(tftp_no_options); /* do not send TFTP options requests */
#جے_انت
  ثنائی_ہندسہ(sep_سرخیاں);     /* handle host and proxy headers separately */
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  ثنائی_ہندسہ(cookiesession);   /* new cookie session? */
#جے_انت
  ثنائی_ہندسہ(crlf);            /* convert crlf on ftp upload(?) */
#جے_تعریف_شدہ USE_SSH
  ثنائی_ہندسہ(ssh_compression);            /* enable SSH compression */
#جے_انت

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  ثنائی_ہندسہ(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  ثنائی_ہندسہ(get_filetime);     /* get the time and get of the remote file */
#جے_بنا_تعریف CURL_DISABLE_PROXY
  ثنائی_ہندسہ(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#جے_انت
  ثنائی_ہندسہ(prefer_ascii);     /* ASCII rather than binary */
  ثنائی_ہندسہ(remote_append);    /* append, not overwrite, on upload */
#جے_تعریف_شدہ CURL_LIST_ONLY_PROTOCOL
  ثنائی_ہندسہ(list_only);        /* list directory */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  ثنائی_ہندسہ(ftp_use_پورٹ);     /* use the FTP PORT command */
  ثنائی_ہندسہ(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  ثنائی_ہندسہ(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  ثنائی_ہندسہ(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  ثنائی_ہندسہ(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  ثنائی_ہندسہ(wildcard_enabled); /* enable wildcard matching */
#جے_انت
  ثنائی_ہندسہ(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  ثنائی_ہندسہ(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  ثنائی_ہندسہ(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  ثنائی_ہندسہ(allow_auth_منزل_other_hosts);
  ثنائی_ہندسہ(include_سرخی); /* include received protocol headers in data output */
  ثنائی_ہندسہ(http_سیٹ_referer); /* is a custom referer used */
  ثنائی_ہندسہ(http_auto_referer); /* set "correct" referer when following
                             location: */
  ثنائی_ہندسہ(opt_no_body);    /* as set with CURLOPT_NOBODY */
  ثنائی_ہندسہ(verbose);        /* output verbosity */
  ثنائی_ہندسہ(reuse_forbid);   /* forbidden to be reused, close after use */
  ثنائی_ہندسہ(reuse_fresh);    /* do not reuse an existing connection */
  ثنائی_ہندسہ(no_signal);      /* do not use any signal/alarm handler */
  ثنائی_ہندسہ(tcp_nodelay);    /* whether to enable TCP_NODELAY or not */
  ثنائی_ہندسہ(ignorecl);       /* ignore content length */
  ثنائی_ہندسہ(http_te_skip);   /* pass the raw body data to the user, even when
                          transfer-encoded (chunked, compressed) */
  ثنائی_ہندسہ(http_ce_skip);   /* pass the raw body data to the user, even when
                          content-encoded (chunked, compressed) */
  ثنائی_ہندسہ(proxy_transfer_منوال); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ثنائی_ہندسہ(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#جے_انت
  ثنائی_ہندسہ(socks5_auth_only); /* SOCKS5 must authenticate */
  ثنائی_ہندسہ(sasl_ir);         /* Enable/disable SASL initial response */
  ثنائی_ہندسہ(tcp_keepalive);  /* use TCP keepalives */
  ثنائی_ہندسہ(tcp_fastopen);   /* use TCP Fast Open */
  ثنائی_ہندسہ(ssl_enable_alpn);/* TLS ALPN extension? */
  ثنائی_ہندسہ(رستہ_as_is);     /* allow dotdots? */
  ثنائی_ہندسہ(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  ثنائی_ہندسہ(suppress_connect_سرخیاں); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  ثنائی_ہندسہ(dns_shuffle_پتے); /* whether to shuffle addresses before use */
#جے_بنا_تعریف CURL_DISABLE_PROXY
  ثنائی_ہندسہ(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
سرخی*/
#جے_انت
#جے_تعریف_شدہ USE_UNIX_SOCKETS
  ثنائی_ہندسہ(abstract_unix_socket);
#جے_انت
  ثنائی_ہندسہ(disallow_username_وچ_url); /* disallow username in URL */
#جے_بنا_تعریف CURL_DISABLE_DOH
  ثنائی_ہندسہ(doh); /* DNS-over-HTTPS enabled */
  ثنائی_ہندسہ(doh_verifypeer);     /* DoH certificate peer verification */
  ثنائی_ہندسہ(doh_verifyhost);     /* DoH certificate hostname verification */
  ثنائی_ہندسہ(doh_verifystatus);   /* DoH certificate status verification */
#جے_انت
  ثنائی_ہندسہ(http09_allowed); /* allow HTTP/0.9 responses */
#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
  ثنائی_ہندسہ(ws_raw_منوال);
  ثنائی_ہندسہ(ws_no_auto_pong);
#جے_انت
  ثنائی_ہندسہ(post301); /* keep POSTs as POSTs after a 301 request */
  ثنائی_ہندسہ(post302); /* keep POSTs as POSTs after a 302 request */
  ثنائی_ہندسہ(post303); /* keep POSTs as POSTs after a 303 request */
#جے_تعریف_شدہ USE_OPENSSL
  ثنائی_ہندسہ(engine_default);
#جے_انت
};

#جے_بنا_تعریف CURL_DISABLE_MIME
#تعریف_کرو IS_MIME_POST(a)                                                 \
  ((a)->سیٹ.mimepostp && ((a)->سیٹ.mimepostp->قسم != MIMEKIND_NONE))
#وإلا
#تعریف_کرو IS_MIME_POST(a) FALSE
#جے_انت

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
تعريفنوع فراغ multi_sub_xfer_پورا_cb(بنية Curl_easy *انکڑے,
                                    بنية Curl_easy *master,
                                    CURLcode نتیجہ);

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
  CURLcode نتیجہ;   /* previous result */

  بنية connectdata *conn;
  بنية Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  بنية Curl_eapi_ڈھیر callstack; /* easy api calls ongoing */

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
  multi_sub_xfer_پورا_cb *sub_xfer_پورا;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  بنية Curl_hash meta_hash;

#جے_تعریف_شدہ USE_LIBPSL
  بنية PslCache *psl;        /* The associated PSL cache. */
#جے_انت
  بنية SingleRequest req;    /* Request-specific data */
  بنية UserDefined سیٹ;      /* values set by the libcurl user */
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  بنية CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HSTS
  بنية hsts *hsts;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  بنية altsvcinfo *asi;      /* the alt-svc cache */
#جے_انت
  بنية ترقی_2 ترقی;    /* for all the progress meter data */
  بنية UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#جے_بنا_تعریف CURL_DISABLE_FTP
  بنية Wildcard_انکڑے *wildcard; /* wildcard download state info */
#جے_انت
  بنية خالص_Info info;        /* stats, reports and info data */
  بنية curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#تعریف_کرو CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->سیٹ.لڑیاں, (uint8_t)(id))
#تعریف_کرو CURL_EASY_STR_سیٹ(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->سیٹ.لڑیاں, (uint8_t)(id), s, slen)
#تعریف_کرو CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->سیٹ.لڑیاں, (uint8_t)(id), s)
#تعریف_کرو CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->سیٹ.لڑیاں, (uint8_t)(id))
#تعریف_کرو CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->سیٹ.لڑیاں, (uint8_t)(id))

#تعریف_کرو LIBCURL_NAME "libcurl"

#جے_انت /* HEADER_CURL_URLDATA_H */
