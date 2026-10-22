#अगर_अघोषित HEADER_CURL_URLDATA_H
#घोषणा HEADER_CURL_URLDATA_H
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
#समावेश "curl_setup.h"

#घोषणा CURL_DEFAULT_USER     "anonymous"
#घोषणा CURL_DEFAULT_PASSWORD "ftp@example.com"

#अगर !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#घोषणा CURL_PREFER_LF_LINEENDS
#पूर्ण_अगर

#घोषणा DEFAULT_CONNCACHE_आकार 5

/* length of longest IPv6 address string including the null-terminator */
#घोषणा MAX_IPADR_LEN माप("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#घोषणा CURL_MAX_निवेश_लंबाई 8000000

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN6_H
#समावेश <netinet/in6.h>
#पूर्ण_अगर

#समावेश "curlx/timeval.h"

#समावेश "api.h"
#समावेश "cookie.h"
#समावेश "creds.h"
#समावेश "psl.h"
#समावेश "formdata.h"
#समावेश "http_chunks.h" /* for the structs and enum stuff */
#समावेश "hash.h"
#समावेश "peer.h"
#समावेश "proxy.h"
#समावेश "splay.h"
#समावेश "curlx/dynbuf.h"
#समावेश "bufref.h"
#समावेश "dynhds.h"
#समावेश "request.h"
#समावेश "ratelimit.h"
#समावेश "netrc.h"
#समावेश "u8_strset.h"
#समावेश "vdns/asyn.h"
#समावेश "vdns/hostip.h"
#समावेश "vtls/vtls_config.h"

/* On error return, the value of `pnwritten` has no meaning */
प्रकार CURLcode (Curl_send)(काष्ठा Curl_easy *तथ्य,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             स्थिर uint8_t *buf,       /* data to write */
                             माप_प्रकार len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             माप_प्रकार *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
प्रकार CURLcode (Curl_recv)(काष्ठा Curl_easy *तथ्य,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             अक्षर *buf,                /* store data here */
                             माप_प्रकार len,               /* max amount to read */
                             माप_प्रकार *pnread);          /* how much received */

#समावेश "mime.h"
#समावेश "protocol.h"
#समावेश "ftp.h"
#समावेश "http.h"
#समावेश "mqtt.h"
#समावेश "ftplistparser.h"
#समावेश "multihandle.h"
#समावेश "cf-socket.h"

#अगर_घोषित HAVE_GSSAPI
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
#पूर्ण_अगर

#अगर_घोषित USE_LIBSSH2
#समावेश <libssh2.h>
#समावेश <libssh2_sftp.h>
#पूर्ण_अगर /* USE_LIBSSH2 */

#घोषणा READBUFFER_आकार CURL_MAX_WRITE_आकार
#घोषणा READBUFFER_MAX  CURL_MAX_READ_आकार
#घोषणा READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#घोषणा UPLOADBUFFER_DEFAULT 65536
#घोषणा UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#घोषणा UPLOADBUFFER_MIN     CURL_MAX_WRITE_आकार

#अगर_घोषित USE_WINDOWS_SSPI
#समावेश "curl_sspi.h"
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
काष्ठा digestdata {
  काष्ठा Curl_creds *creds;
  काष्ठा Curl_peer *origin;
#अगर_घोषित USE_WINDOWS_SSPI
  BYTE *निवेश_token;
  माप_प्रकार निवेश_token_len;
  CtxtHandle *http_context;
#अन्यथा
  अक्षर *nonce;
  अक्षर *cnonce;
  अक्षर *realm;
  अक्षर *opaque;
  अक्षर *qop;
  अक्षर *algorithm;
  पूर्णांक nc; /* nonce count */
  uint8_t algo;
  BIT(stale); /* set true for re-negotiation */
  BIT(userhash);
#पूर्ण_अगर
};
#पूर्ण_अगर

प्रकार क्रमागत {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_LAST
} curlntlm;

#घोषणा CURL_NEGO_AUTH_NONE   0U
#घोषणा CURL_NEGO_AUTH_RECV   1U
#घोषणा CURL_NEGO_AUTH_SENT   2U
#घोषणा CURL_NEGO_AUTH_DONE   3U
#घोषणा CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
काष्ठा ConnectBits {
  BIT(connect_only);
#अगर_अघोषित CURL_DISABLE_PROXY
  BIT(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  BIT(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#पूर्ण_अगर
  /* always modify bits.close with the connclose() and connkeep() macros! */
  BIT(बंद); /* if set, we close the connection after this request */
  BIT(reuse); /* if set, this is a reused connection */
  BIT(altused); /* this is an alt-svc "redirect" */
  BIT(ipv6);    /* we communicate with a site using an IPv6 address */
  BIT(do_more); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  BIT(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  BIT(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#अगर_अघोषित CURL_DISABLE_FTP
  BIT(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
                         requests */
  BIT(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
                         requests */
  BIT(ftp_use_तथ्य_ssl); /* Enabled SSL for the data connection */
#पूर्ण_अगर
  BIT(bound); /* set true if bind() has already been done on this socket/
                 connection */
  BIT(upgrade_in_progress); /* protocol upgrade is in progress */
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

काष्ठा hostname {
  अक्षर *rawalloc; /* allocated "raw" version of the name */
  अक्षर *encalloc; /* allocated IDN-encoded version of the name */
  अक्षर *name;     /* name to use internally, might be encoded, might be raw */
  स्थिर अक्षर *dispname; /* name to display, as 'name' might be encoded */
};

#घोषणा FIRSTSOCKET     0
#घोषणा SECONDARYSOCKET 1

#घोषणा TRNSPRT_NONE 0
#घोषणा TRNSPRT_TCP  3
#घोषणा TRNSPRT_UDP  4
#घोषणा TRNSPRT_QUIC 5
#घोषणा TRNSPRT_UNIX 6

#घोषणा TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

काष्ठा ip_quadruple {
  अक्षर remote_ip[MAX_IPADR_LEN];
  अक्षर local_ip[MAX_IPADR_LEN];
  uint16_t remote_port;
  uint16_t local_port;
  uint8_t transport;
};

#घोषणा CUR_IP_QUAD_HAS_PORTS(x)      \
  (((x)->transport == TRNSPRT_TCP) || \
   ((x)->transport == TRNSPRT_UDP) || \
   ((x)->transport == TRNSPRT_QUIC))

/*
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
काष्ठा connectdata {
  curl_off_t connection_id; /* Contains a unique number to make it easier to
                               track the connections in the log output */

  /* A connection cache from a SHARE might be used in several multi handles.
   * We MUST not reuse connections that are running in another multi,
   * for concurrency reasons. That multi might run in another thread.
   * `attached_multi` is set by the first transfer attached and cleared
   * when the last one is detached.
   * NEVER call anything on this multi, check for equality. */
  काष्ठा Curl_multi *attached_multi;

  /* Who the connection is talking to, ultimately */
  काष्ठा Curl_peer *origin; /* connection ultimately talks to this */
  काष्ठा Curl_peer *via_peer; /* if set, connection really talks to this */
  काष्ठा Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  काष्ठा Curl_peer *via_peer2; /* peer of SECONDARYSOCKET */
  काष्ठा Curl_creds *creds; /* When connection itself is tied to credentials */
  काष्ठा Curl_peer *creds_origin; /* origin tied credentials are for */
  स्थिर काष्ठा Curl_scheme *scheme; /* Connection's real protocol handler */

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the connection.
   * Elements need to be added with their own destructor to be invoked when
   * the connection is cleaned up (see Curl_hash_add2()).*/
  काष्ठा Curl_hash meta_hash;

  अक्षर *destination; /* hostname+port, used in conncache */

  काष्ठा curltime created; /* creation time */
  /* timediff_t are deltas from `created`*/
  timediff_t lastused_ms; /* when returned to the connection pool as idle */
  timediff_t lastchecked_ms; /* when last checked alive status */
  timediff_t lastupkeep_ms; /* when last done conn_upkeep */
  काष्ठा {
    काष्ठा curltime at[2]; /* first Curl_conn_connect() for each socket */
    bool set[2];
  } connect_started;

#अगर_अघोषित CURL_DISABLE_PROXY
  काष्ठा proxy_info socks_proxy;
  काष्ठा proxy_info http_proxy;
#पूर्ण_अगर

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  काष्ठा Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#घोषणा CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  काष्ठा {
    timediff_t start_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  व्योम *closesocket_client;

  काष्ठा ssl_filter_config ssl_config;
#अगर_अघोषित CURL_DISABLE_PROXY
  काष्ठा ssl_filter_config proxy_ssl_config;
#पूर्ण_अगर
  अक्षर *options; /* options string, allocated */

  /*************** Request - specific items ************/
#अगर_घोषित USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#पूर्ण_अगर

#अगर_घोषित USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#पूर्ण_अगर

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  अक्षर *interface_opts;
  काष्ठा ConnectBits bits;    /* various state-flags for this connection */
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  पूर्णांक socks5_gssapi_enctype;
#पूर्ण_अगर

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#घोषणा CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#अगर_घोषित USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#पूर्ण_अगर
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_version; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_seen;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#अगर_घोषित USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#पूर्ण_अगर
};

#अगर_अघोषित CURL_DISABLE_PROXY
#घोषणा CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->via_peer ? (c)->via_peer->user_hostname : \
        (c)->origin->user_hostname)
#अन्यथा
#घोषणा CURL_CONN_HOST_DISPNAME(c) \
  ((c)->via_peer ? (c)->via_peer->user_hostname : (c)->origin->user_hostname)
#पूर्ण_अगर

/* The end of connectdata. */

/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
काष्ठा PureInfo {
  /* PureInfo primary ip_quadruple is copied over from the connectdata
     struct in order to allow curl_easy_getinfo() to return this information
     even when the session handle is no longer associated with a connection,
     and also allow curl_easy_reset() to clear this information from the
     session handle without disturbing information which is still alive, and
     that might be reused, in the connection pool. */
  काष्ठा ip_quadruple primary;
  काष्ठा curl_certinfo certs; /* info about the certs. Asked for with
                                 CURLOPT_CERTINFO / CURLINFO_CERTINFO */
  समय_प्रकार filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t request_आकार; /* the amount of bytes sent in the request(s) */
  curl_off_t numconnects; /* how many new connections libcurl created */
  अक्षर *contenttype; /* the content type of the object */
  अक्षर *wouldredirect; /* URL this would have been redirected to if asked to */
  अक्षर *effective_url; /* when needed */
  curl_off_t retry_after; /* info from Retry-After: header */
  स्थिर अक्षर *conn_scheme;
  पूर्णांक httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  पूर्णांक httpproxycode; /* response code from proxy when received separate */
  पूर्णांक httpversion; /* the http version number X.Y = X*10+Y */
  uint32_t conn_protocol;
  uint32_t proxyauthavail; /* what proxy auth types were announced */
  uint32_t httpauthavail;  /* what host auth types were announced */
  uint32_t proxyauthpicked; /* selected proxy auth type */
  uint32_t httpauthpicked;  /* selected host auth type */
  uint32_t header_आकार;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  BIT(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  BIT(used_proxy); /* the transfer used a proxy */
};

काष्ठा pgrs_dir {
  curl_off_t योग_आकार; /* total expected bytes */
  curl_off_t cur_आकार; /* transferred bytes so far */
  curl_off_t speed; /* bytes per second transferred */
  काष्ठा Curl_rlimit rlimit; /* speed limiting / pausing */
};

काष्ठा Progress {
  काष्ठा curltime now; /* current time of processing */
  काष्ठा curltime start; /* when transfer was initialized, set once */

  काष्ठा pgrs_dir ul;
  काष्ठा pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_speed; /* uses the currently fastest transfer */
  curl_off_t earlydata_sent;

  काष्ठा {
    timediff_t startop_us; /* since start when operations started */
    timediff_t startsingle_us; /* since start when last request started */
    timediff_t startqueue_us; /* since start when last entered queueing */
    timediff_t startredirect_us; /* since start when last redirected */
    timediff_t lastshow_us; /* since start when last progress shown */
  } delta;
  काष्ठा {
    timediff_t spent_us; /* all time spent since start */
    timediff_t queued_us; /* time spent since startsingle's for queueing */
    timediff_t nslookup_us; /* same for name resolves */
    timediff_t connect_us; /* same for connects */
    timediff_t appconnect_us; /* same for application connects, e.g. TLS */
    timediff_t pretransfer_us; /* same until requests were sent */
    timediff_t starttransfer_us; /* same until responses started */
    timediff_t posttransfer_us; /* same until responses ended */
  } योग;

#घोषणा CURL_SPEED_RECORDS (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t speed_amount[CURL_SPEED_RECORDS];
  timediff_t speed_time[CURL_SPEED_RECORDS];
  uint32_t speeder_c;
  BIT(hide);
  BIT(ul_आकार_known);
  BIT(dl_आकार_known);
  BIT(headers_out); /* when the headers have been written */
  BIT(callback);  /* set when progress callback is used */
  BIT(startransfer_added);
};

काष्ठा auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  BIT(done);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  BIT(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#अगर_घोषित USE_NGHTTP2
काष्ठा Curl_तथ्य_prio_गाँठ {
  काष्ठा Curl_तथ्य_prio_गाँठ *अग्र;
  काष्ठा Curl_easy *तथ्य;
};
#पूर्ण_अगर

/* Timers */
प्रकार क्रमागत {
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
  EXPIRE_LAST /* not an actual timer, used as a marker only */
} expire_id;

काष्ठा expire_timers {
  काष्ठा Curl_वृक्ष splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t offset_us[EXPIRE_LAST];
  uint8_t अग्र[EXPIRE_LAST];
  uint8_t first;
};

/* individual pieces of the URL */
काष्ठा urlpieces {
  अक्षर *options;
  अक्षर *path;
  अक्षर *query;
};

काष्ठा UrlState {
  /* Origin of the initial (e.g. not followed) request of a transfer.
     Credentials from CURLOPT_* are only valid for this origin.
     Always set once a transfer starts searching for connections. */
  काष्ठा Curl_peer *initial_origin;
  /* Current origin of the transfer, changes to origin of follow
   * requests. */
  काष्ठा Curl_peer *origin;

  काष्ठा curltime keeps_speed; /* for the progress meter really */
  काष्ठा dynbuf headerb; /* buffer to store headers in */
#अगर_अघोषित CURL_DISABLE_HSTS
  काष्ठा curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#पूर्ण_अगर

  पूर्णांक os_errno;  /* filled in with errno whenever an error occurs */
  पूर्णांक requests; /* request counter: redirects + authentication retakes */
#अगर_घोषित HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  व्योम (*पूर्व_signal)(पूर्णांक sig);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
  अक्षर *envproxy; /* last proxy string used for proxy-related state */
  काष्ठा digestdata digest;      /* state data for host Digest auth */
  काष्ठा digestdata proxydigest; /* state data for proxy Digest auth */
#पूर्ण_अगर
  काष्ठा auth authhost;  /* auth details for host */
  काष्ठा auth authproxy; /* auth details for proxy */

#अगर_घोषित USE_CURL_ASYNC
  काष्ठा Curl_resolv_async *async;  /* asynchronous name resolver data */
#पूर्ण_अगर

  काष्ठा expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  अक्षर *most_recent_ftp_entrypath;
  अक्षर *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  व्योम *in;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  काष्ठा urlpieces up;
  काष्ठा bufref url;        /* work URL, initially copied from UserDefined */
  काष्ठा bufref referer;    /* referer string */
  काष्ठा curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#अगर_अघोषित CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#अगर_अघोषित CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#पूर्ण_अगर
  काष्ठा Curl_llist httphdrs; /* received headers */
  काष्ठा curl_header headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  अचिन्हित पूर्णांक nh_origin;
  पूर्णांक nh_request;
  माप_प्रकार nh_गिनती; /* httphdrs count when the cache was filled */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_COOKIES
  काष्ठा curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#पूर्ण_अगर

#अगर_घोषित CURLVERBOSE
  काष्ठा curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_NETRC
  काष्ठा store_netrc netrc;
#पूर्ण_अगर

  काष्ठा Curl_creds *creds; /* Credentials for the origin only */

#अगर_अघोषित CURL_DISABLE_HTTP
  अक्षर *rangeline; /* allocated */
  अक्षर *http_host; /* allocated */
  काष्ठा http_negotiation http_neg;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  काष्ठा Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_mask[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_अग्र_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_अग्र_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#पूर्ण_अगर

  curl_off_t last_conn_id; /* The last assigned connection or -1 */
  uint32_t last_cpid; /* last connection pool id used */

#अगर defined(USE_HTTP2) || defined(USE_HTTP3)
  पूर्णांक weight; /* shallow copy of data->set */
#पूर्ण_अगर
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  BIT(really_alive); /* transfer is really alive in multi, passed INIT */
  BIT(this_is_a_follow); /* this is a followed Location: request */
  BIT(refused_stream); /* this was refused, try again */
  BIT(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  BIT(allow_port); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  BIT(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  BIT(wildcardmatch); /* enable wildcard matching */
  BIT(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  BIT(use_range);
  BIT(rangestringalloc); /* the range string is malloc()'ed */
  BIT(done); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#अगर_अघोषित CURL_DISABLE_COOKIES
  BIT(cookie_engine);
#पूर्ण_अगर
  BIT(prefer_ascii);   /* ASCII rather than binary */
#अगर_घोषित CURL_LIST_ONLY_PROTOCOL
  BIT(list_only);      /* list directory contents */
#पूर्ण_अगर
  BIT(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  BIT(upload);         /* upload request */
  BIT(internal); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  BIT(http_ignorecustom); /* ignore custom method from now */
#अगर_अघोषित CURL_DISABLE_HTTP
  BIT(http_hd_te); /* Added HTTP header TE: */
  BIT(http_hd_upgrade); /* Added HTTP header Upgrade: */
  BIT(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  BIT(maybe_folded);
  BIT(leading_unfold); /* unfold started, this is the leading bytes */
#पूर्ण_अगर
};

/*
 * This 'UserDefined' struct must only contain data that is set once to go
 * for many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" MUST be defined within the
 * 'struct UrlState' instead. The only exceptions MUST note the changes in
 * the 'DynamicStatic' struct.
 * Character pointer fields point to dynamic storage, unless otherwise stated.
 */

काष्ठा Curl_multi;    /* declared in multihandle.c */

क्रमागत dupstring {
  STRING_CERT,            /* client certificate filename */
  STRING_CERT_TYPE,       /* format for certificate (default: PEM)*/
  STRING_कुंजी,             /* private key filename */
  STRING_कुंजी_PASSWD,      /* plain text private key password */
  STRING_कुंजी_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#अगर_अघोषित CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_कुंजी_PROXY,       /* private key filename */
  STRING_कुंजी_PASSWD_PROXY, /* plain text private key password */
  STRING_कुंजी_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#पूर्ण_अगर
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#अगर_अघोषित CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_TO_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#पूर्ण_अगर
  STRING_SET_RANGE,       /* range, if used */
  STRING_SET_REFERER,     /* custom string for the HTTP referer field */
  STRING_SET_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#अगर_अघोषित CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
                             used */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_STREAM_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  STRING_SSH_PRIVATE_कुंजी, /* path to the private key file for auth */
  STRING_SSH_PUBLIC_कुंजी,  /* path to the public key file for auth */
  STRING_SSH_HOST_PUBLIC_कुंजी_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_PUBLIC_कुंजी_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#पूर्ण_अगर
  STRING_BEARER,                /* <bearer>, if used */
#अगर_घोषित USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_PATH,      /* path to Unix socket, if used */
#पूर्ण_अगर
  STRING_TARGET,                /* CURLOPT_REQUEST_TARGET */
#अगर_अघोषित CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#पूर्ण_अगर
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#अगर_घोषित USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#पूर्ण_अगर
  STRING_SSL_EC_CURVES,
#अगर_अघोषित CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_कुंजी,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_HEADERS, /* space-separated components to sign */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#पूर्ण_अगर
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_PUBLIC,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_LAST /* not used, an end-of-list marker */
};

क्रमागत dupblob {
  BLOB_CERT,
  BLOB_कुंजी,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#अगर_अघोषित CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_कुंजी_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#पूर्ण_अगर
  BLOB_LAST
};

काष्ठा UserDefined {
  खाता *err;         /* the stderr user data goes here */
  व्योम *debugdata;   /* the data that will be passed to fdebug */
  अक्षर *errorbuffer; /* (Static) store failure messages in here */
  व्योम *out;         /* CURLOPT_WRITEDATA */
  व्योम *in_set;      /* CURLOPT_READDATA */
  व्योम *writeheader; /* write the header to this if non-NULL */
  uint32_t httpauth;  /* kind of HTTP authentication to use (bitmask) */
  uint32_t proxyauth; /* kind of proxy authentication to use (bitmask) */
  व्योम *postfields;  /* if POST, set the fields' values here */
  अक्षर *str_copypostfields; /* CURLOPT_COPYPOSTFIELDS value */
  curl_seek_callback seek_func;      /* function that seeks the input */
  curl_off_t postfieldsize; /* if POST, this might have a size to use instead
                               of strlen(), and then the data *may* be binary
                               (contain NUL bytes) */
  curl_write_callback fwrite_func;   /* function that stores the output */
  curl_write_callback fwrite_header; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_set; /* function that reads the input */
  curl_progress_callback fprogress; /* OLD and deprecated progress callback */
  curl_xferinfo_callback fxferinfo; /* progress callback */
  curl_debug_callback fdebug;      /* function that write informational data */
  curl_ioctl_callback ioctl_func;  /* function for I/O control */
  curl_sockopt_callback fsockopt;  /* function for setting socket options */
  व्योम *sockopt_client; /* pointer to pass to the socket options callback */
  curl_opensocket_callback fopensocket; /* function for checking/translating
                                           the address and opening the
                                           socket */
  व्योम *opensocket_client;
  curl_closesocket_callback fclosesocket; /* function for closing the
                                             socket */
  व्योम *closesocket_client;
  curl_prereq_callback fprereq; /* pre-initial request callback */
  व्योम *prereq_userp; /* pre-initial request user data */

  व्योम *seek_client;    /* pointer to pass to the seek callback */
#अगर_अघोषित CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  व्योम *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  व्योम *hsts_write_userp;
#पूर्ण_अगर
  व्योम *progress_client; /* pointer to pass to the progress callback */
  व्योम *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t निचला_speed_limit; /* bytes/second */
  curl_off_t max_send_speed; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_speed; /* high speed limit in bytes/second for
                                download */
  curl_off_t set_resume_from;  /* continue [ftp] transfer from here */
  काष्ठा curl_slist *headers; /* linked list of extra headers */
  काष्ठा curl_httppost *httppost;  /* linked list of old POST data */
#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_TELNET
  काष्ठा curl_slist *telnet_options; /* linked list of telnet options */
#पूर्ण_अगर
  काष्ठा curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  काष्ठा curl_slist *connect_to; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  समय_प्रकार timevalue;       /* what time to compare with */
  काष्ठा ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  व्योम *ssl_fsslctxp;        /* parameter for callback */
#अगर_अघोषित CURL_DISABLE_PROXY
  काष्ठा ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  काष्ठा curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#पूर्ण_अगर
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t बफर_आकार;        /* size of receive buffer to use */
  uint32_t upload_बफर_आकार; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  व्योम *private_तथ्य; /* application-private data */
#अगर_अघोषित CURL_DISABLE_HTTP
  काष्ठा curl_slist *http200aliases; /* linked list of aliases for http200 */
#पूर्ण_अगर
  curl_off_t max_filesize; /* Maximum file size to download */
#अगर_अघोषित CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  काष्ठा curl_slist *quote;     /* after connection is established */
  काष्ठा curl_slist *postquote; /* after the transfer */
  काष्ठा curl_slist *prequote; /* before the transfer, after type */
#पूर्ण_अगर
#अगर_घोषित USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  व्योम *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  व्योम *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#पूर्ण_अगर
  काष्ठा u8_strset strings;
  काष्ठा curl_blob *blobs[BLOB_LAST];
  uint32_t new_file_perms;      /* when creating remote files */
#अगर_घोषित USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#पूर्ण_अगर
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#अगर_अघोषित CURL_DISABLE_RTSP
  व्योम *rtp_out;     /* write RTP to this if non-NULL */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_end_callback chunk_end; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  व्योम *fnmatch_तथ्य;
  व्योम *wildcardptr;
#पूर्ण_अगर

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_start_callback resolver_start; /* optional callback called
                                                  before resolver start */
  व्योम *resolver_start_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#अगर_अघोषित CURL_DISABLE_HTTP
  व्योम *trailer_तथ्य; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
  काष्ठा curl_slist *mail_rcpt; /* linked list of mail recipients */
#पूर्ण_अगर
  पूर्णांक ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  पूर्णांक tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  पूर्णांक tcp_keepintvl;    /* seconds between TCP keepalive probes */
  पूर्णांक tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#अगर defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  पूर्णांक weight;
#पूर्ण_अगर
  लघु maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_port; /* which port to use (when not using default) */
  uint16_t निचला_speed_time;  /* number of seconds */
#अगर_अघोषित CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#पूर्ण_अगर
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t method;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#पूर्ण_अगर
  uint8_t http_follow_mode; /* follow HTTP redirects */
  BIT(connect_only); /* make connection/request, then let application use the
                        socket */
  BIT(connect_only_ws); /* special websocket connect-only level */
#अगर_अघोषित CURL_DISABLE_SMTP
  BIT(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_MIME
  BIT(mime_formescape);
#पूर्ण_अगर
  BIT(is_fread_set); /* has read callback been set to non-NULL? */
#अगर_अघोषित CURL_DISABLE_TFTP
  BIT(tftp_no_options); /* do not send TFTP options requests */
#पूर्ण_अगर
  BIT(sep_headers);     /* handle host and proxy headers separately */
#अगर_अघोषित CURL_DISABLE_COOKIES
  BIT(cookiesession);   /* new cookie session? */
#पूर्ण_अगर
  BIT(crlf);            /* convert crlf on ftp upload(?) */
#अगर_घोषित USE_SSH
  BIT(ssh_compression);            /* enable SSH compression */
#पूर्ण_अगर

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  BIT(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  BIT(get_filetime);     /* get the time and get of the remote file */
#अगर_अघोषित CURL_DISABLE_PROXY
  BIT(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#पूर्ण_अगर
  BIT(prefer_ascii);     /* ASCII rather than binary */
  BIT(remote_append);    /* append, not overwrite, on upload */
#अगर_घोषित CURL_LIST_ONLY_PROTOCOL
  BIT(list_only);        /* list directory */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  BIT(ftp_use_port);     /* use the FTP PORT command */
  BIT(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  BIT(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  BIT(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  BIT(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  BIT(wildcard_enabled); /* enable wildcard matching */
#पूर्ण_अगर
  BIT(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  BIT(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  BIT(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  BIT(allow_auth_to_other_hosts);
  BIT(include_header); /* include received protocol headers in data output */
  BIT(http_set_referer); /* is a custom referer used */
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
  BIT(proxy_transfer_mode); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  BIT(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#पूर्ण_अगर
  BIT(socks5_auth_only); /* SOCKS5 must authenticate */
  BIT(sasl_ir);         /* Enable/disable SASL initial response */
  BIT(tcp_keepalive);  /* use TCP keepalives */
  BIT(tcp_fastopen);   /* use TCP Fast Open */
  BIT(ssl_enable_alpn);/* TLS ALPN extension? */
  BIT(path_as_is);     /* allow dotdots? */
  BIT(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  BIT(suppress_connect_headers); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  BIT(dns_shuffle_addresses); /* whether to shuffle addresses before use */
#अगर_अघोषित CURL_DISABLE_PROXY
  BIT(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
                           header */
#पूर्ण_अगर
#अगर_घोषित USE_UNIX_SOCKETS
  BIT(abstract_unix_socket);
#पूर्ण_अगर
  BIT(disallow_username_in_url); /* disallow username in URL */
#अगर_अघोषित CURL_DISABLE_DOH
  BIT(doh); /* DNS-over-HTTPS enabled */
  BIT(doh_verifypeer);     /* DoH certificate peer verification */
  BIT(doh_verifyhost);     /* DoH certificate hostname verification */
  BIT(doh_verifystatus);   /* DoH certificate status verification */
#पूर्ण_अगर
  BIT(http09_allowed); /* allow HTTP/0.9 responses */
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  BIT(ws_raw_mode);
  BIT(ws_no_auto_pong);
#पूर्ण_अगर
  BIT(post301); /* keep POSTs as POSTs after a 301 request */
  BIT(post302); /* keep POSTs as POSTs after a 302 request */
  BIT(post303); /* keep POSTs as POSTs after a 303 request */
#अगर_घोषित USE_OPENSSL
  BIT(engine_default);
#पूर्ण_अगर
};

#अगर_अघोषित CURL_DISABLE_MIME
#घोषणा IS_MIME_POST(a)                                                 \
  ((a)->set.mimepostp && ((a)->set.mimepostp->kind != MIMEKIND_NONE))
#अन्यथा
#घोषणा IS_MIME_POST(a) FALSE
#पूर्ण_अगर

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
प्रकार व्योम multi_sub_xfer_done_cb(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा Curl_easy *master,
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

काष्ठा Curl_easy {
  /* First a simple identifier to more easily detect if a user mixes up this
     easy handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  uint32_t magic;
  /* once an easy handle is added to a multi, either explicitly by the
   * libcurl application or implicitly during `curl_easy_perform()`,
   * a unique identifier inside this one multi instance. */
  uint32_t mid;
  CURLMstate mstate;  /* the handle's state */
  CURLcode परिणाम;   /* previous result */

  काष्ठा connectdata *conn;
  काष्ठा Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  काष्ठा Curl_eapi_ढेर callstack; /* easy api calls ongoing */

  काष्ठा Curl_share *share;    /* Share, handles global variable mutexing */

  काष्ठा Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  काष्ठा CURLMsg संदेश; /* A single posted message. */

  /* once an easy handle is tied to a connection pool a non-negative number to
     distinguish this transfer from other using the same pool. For easier
     tracking in log output. This may wrap around after CURL_OFF_T_MAX to 0
     again, so it has no uniqueness guarantee for large processings. Note: it
     has no uniqueness either IFF more than one connection pool is used by the
     libcurl application. */
  curl_off_t id;
  uint32_t master_mid; /* if set, this transfer belongs to a master */
  multi_sub_xfer_done_cb *sub_xfer_done;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  काष्ठा Curl_hash meta_hash;

#अगर_घोषित USE_LIBPSL
  काष्ठा PslCache *psl;        /* The associated PSL cache. */
#पूर्ण_अगर
  काष्ठा SingleRequest req;    /* Request-specific data */
  काष्ठा UserDefined set;      /* values set by the libcurl user */
#अगर_अघोषित CURL_DISABLE_COOKIES
  काष्ठा CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  काष्ठा hsts *hsts;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_ALTSVC
  काष्ठा altsvcinfo *asi;      /* the alt-svc cache */
#पूर्ण_अगर
  काष्ठा Progress progress;    /* for all the progress meter data */
  काष्ठा UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#अगर_अघोषित CURL_DISABLE_FTP
  काष्ठा Wildcard_तथ्य *wildcard; /* wildcard download state info */
#पूर्ण_अगर
  काष्ठा PureInfo info;        /* stats, reports and info data */
  काष्ठा curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#घोषणा CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->set.strings, (uint8_t)(id))
#घोषणा CURL_EASY_STR_SET(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->set.strings, (uint8_t)(id), s, slen)
#घोषणा CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->set.strings, (uint8_t)(id), s)
#घोषणा CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->set.strings, (uint8_t)(id))
#घोषणा CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->set.strings, (uint8_t)(id))

#घोषणा LIBCURL_NAME "libcurl"

#पूर्ण_अगर /* HEADER_CURL_URLDATA_H */
