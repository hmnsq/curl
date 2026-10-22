#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_URLDATA_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_URLDATA_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DEFAULT_USER     "anonymous"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DEFAULT_PASSWORD "ftp@example.com"

#ਜੇ !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PREFER_LF_LINEENDS
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_CONNCACHE_ਮਾਪ 5

/* length of longest IPv6 address string including the null-terminator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_IPADR_LEN ਆਕਾਰ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ 8000000

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IN6_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in6.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "api.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cookie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "creds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "psl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "formdata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_chunks.h" /* for the structs and enum stuff */
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "peer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "splay.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dynhds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "request.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ratelimit.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "netrc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u8_strset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/asyn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_config.h"

/* On error return, the value of `pnwritten` has no meaning */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode (Curl_send)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ਅਚਲ uint8_t *buf,       /* data to write */
                             size_t len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             size_t *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode (Curl_recv)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ਅੱਖਰ *buf,                /* store data here */
                             size_t len,               /* max amount to read */
                             size_t *pnread);          /* how much received */

#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mqtt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftplistparser.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
#  ifdef HAVE_GSSAPPLE
#    include <GSS/gssapi.h>
#    include <GSS/gssapi_oid.h>
#  elif defined(HAVE_GSSGNU)
#    include <gss.h>
#  elif defined(HAVE_GSSAPI_H)
#    include <gssapi.h>
#  ਨਹੀਂਤਾਂ /* MIT Kerberos */
#    include <gssapi/gssapi.h>
#    include <gssapi/gssapi_krb5.h> /* for GSS_C_CHANNEL_BOUND_FLAG in 1.19+ */
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2
#ਅਨ੍ਤਰ੍ਭਾਵ <libssh2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libssh2_sftp.h>
#ਜੇ_ਅੰਤ /* USE_LIBSSH2 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READBUFFER_ਮਾਪ CURL_MAX_WRITE_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READBUFFER_MAX  CURL_MAX_READ_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPLOADBUFFER_DEFAULT 65536
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPLOADBUFFER_MIN     CURL_MAX_WRITE_ਮਾਪ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
ਬਣਤਰ digestdata {
  ਬਣਤਰ Curl_creds *creds;
  ਬਣਤਰ Curl_peer *origin;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  BYTE *ਨਿਵੇਸ਼_token;
  size_t ਨਿਵੇਸ਼_token_len;
  CtxtHandle *http_context;
#ਨਹੀਂਤਾਂ
  ਅੱਖਰ *nonce;
  ਅੱਖਰ *cnonce;
  ਅੱਖਰ *realm;
  ਅੱਖਰ *opaque;
  ਅੱਖਰ *qop;
  ਅੱਖਰ *algorithm;
  ਪੂਰਨ nc; /* nonce count */
  uint8_t algo;
  ਦੋਆਧਾਰੀ_ਅੰਕ(stale); /* set true for re-negotiation */
  ਦੋਆਧਾਰੀ_ਅੰਕ(userhash);
#ਜੇ_ਅੰਤ
};
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_ਆਖਰੀ
} curlntlm;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NEGO_AUTH_NONE   0U
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NEGO_AUTH_RECV   1U
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NEGO_AUTH_ਭੇਜਿਆ   2U
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NEGO_AUTH_ਪੂਰਾ   3U
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
ਬਣਤਰ Connect_ਬਿੱਟ {
  ਦੋਆਧਾਰੀ_ਅੰਕ(connect_only);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਦੋਆਧਾਰੀ_ਅੰਕ(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  ਦੋਆਧਾਰੀ_ਅੰਕ(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#ਜੇ_ਅੰਤ
  /* always modify bits.close with the connclose() and connkeep() macros! */
  ਦੋਆਧਾਰੀ_ਅੰਕ(close); /* if set, we close the connection after this request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reuse); /* if set, this is a reused connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(altused); /* this is an alt-svc "redirect" */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ipv6);    /* we communicate with a site using an IPv6 address */
  ਦੋਆਧਾਰੀ_ਅੰਕ(do_ਹੋਰ); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  ਦੋਆਧਾਰੀ_ਅੰਕ(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  ਦੋਆਧਾਰੀ_ਅੰਕ(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
ਬੇਨਤੀਆਂ*/
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
ਬੇਨਤੀਆਂ*/
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_ਅੰਕੜੇ_ssl); /* Enabled SSL for the data connection */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(bound); /* set true if bind() has already been done on this socket/
                 connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(upgrade_ਵਿੱਚ_ਤਰੱਕੀ); /* protocol upgrade is in progress */
  ਦੋਆਧਾਰੀ_ਅੰਕ(multiplex); /* connection is multiplexed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tcp_fastopen); /* use TCP Fast Open */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tls_enable_alpn); /* TLS ALPN extension? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(sock_accepted); /* TRUE if the SECONDARYSOCKET was created with
                         accept() */
  ਦੋਆਧਾਰੀ_ਅੰਕ(parallel_connect); /* set TRUE when a parallel connect attempt has
                            started (happy eyeballs) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(aborted); /* connection was aborted, e.g. in unclean state */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_reuse); /* connection should not be reused */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown_handler); /* connection shutdown: handler shut down */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown_filters); /* connection shutdown: filters shut down */
  ਦੋਆਧਾਰੀ_ਅੰਕ(dns_resolved); /* DNS records for connection were resolved */
};

ਬਣਤਰ hostname {
  ਅੱਖਰ *rawalloc; /* allocated "raw" version of the name */
  ਅੱਖਰ *encalloc; /* allocated IDN-encoded version of the name */
  ਅੱਖਰ *name;     /* name to use internally, might be encoded, might be raw */
  ਅਚਲ ਅੱਖਰ *dispname; /* name to display, as 'name' might be encoded */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIRSTSOCKET     0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECONDARYSOCKET 1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRNSPRT_NONE 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRNSPRT_TCP  3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRNSPRT_UDP  4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRNSPRT_QUIC 5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRNSPRT_UNIX 6

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

ਬਣਤਰ ip_quadruple {
  ਅੱਖਰ remote_ip[MAX_IPADR_LEN];
  ਅੱਖਰ local_ip[MAX_IPADR_LEN];
  uint16_t remote_ਪੋਰਟ;
  uint16_t local_ਪੋਰਟ;
  uint8_t transport;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CUR_IP_QUAD_HAS_PORTS(x)      \
  (((x)->transport == TRNSPRT_TCP) || \
   ((x)->transport == TRNSPRT_UDP) || \
   ((x)->transport == TRNSPRT_QUIC))

/*
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
ਬਣਤਰ connectdata {
  curl_off_t connection_id; /* Contains a unique number to make it easier to
                               track the connections in the log output */

  /* A connection cache from a SHARE might be used in several multi handles.
   * We MUST not reuse connections that are running in another multi,
   * for concurrency reasons. That multi might run in another thread.
   * `attached_multi` is set by the first transfer attached and cleared
   * when the last one is detached.
   * NEVER call anything on this multi, check for equality. */
  ਬਣਤਰ Curl_multi *attached_multi;

  /* Who the connection is talking to, ultimately */
  ਬਣਤਰ Curl_peer *origin; /* connection ultimately talks to this */
  ਬਣਤਰ Curl_peer *ਰਾਹੀਂ_peer; /* if set, connection really talks to this */
  ਬਣਤਰ Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  ਬਣਤਰ Curl_peer *ਰਾਹੀਂ_peer2; /* peer of SECONDARYSOCKET */
  ਬਣਤਰ Curl_creds *creds; /* When connection itself is tied to credentials */
  ਬਣਤਰ Curl_peer *creds_origin; /* origin tied credentials are for */
  ਅਚਲ ਬਣਤਰ Curl_scheme *scheme; /* Connection's real protocol handler */

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the connection.
   * Elements need to be added with their own destructor to be invoked when
   * the connection is cleaned up (see Curl_hash_add2()).*/
  ਬਣਤਰ Curl_hash meta_hash;

  ਅੱਖਰ *destination; /* hostname+port, used in conncache */

  ਬਣਤਰ curltime created; /* creation time */
  /* timediff_t are deltas from `created`*/
  timediff_t lastused_ms; /* when returned to the connection pool as idle */
  timediff_t lastchecked_ms; /* when last checked alive status */
  timediff_t lastupkeep_ms; /* when last done conn_upkeep */
  ਬਣਤਰ {
    ਬਣਤਰ curltime at[2]; /* first Curl_conn_connect() for each socket */
    bool ਸੈੱਟ[2];
  } connect_started;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਬਣਤਰ proxy_info socks_proxy;
  ਬਣਤਰ proxy_info http_proxy;
#ਜੇ_ਅੰਤ

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  ਬਣਤਰ Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  ਬਣਤਰ {
    timediff_t ਸ਼ੁਰੂ_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  ਖਾਲੀ *closesocket_client;

  ਬਣਤਰ ssl_filter_config ssl_config;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਬਣਤਰ ssl_filter_config proxy_ssl_config;
#ਜੇ_ਅੰਤ
  ਅੱਖਰ *options; /* options string, allocated */

  /*************** Request - specific items ************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#ਜੇ_ਅੰਤ

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  ਅੱਖਰ *interface_opts;
  ਬਣਤਰ Connect_ਬਿੱਟ ਬਿੱਟ;    /* various state-flags for this connection */
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ਪੂਰਨ socks5_gssapi_enctype;
#ਜੇ_ਅੰਤ

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#ਜੇ_ਅੰਤ
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_ਸੰਸਕਰਣ; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_ਵੇਖਿਆ;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#ਜੇ_ਅੰਤ
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->ਰਾਹੀਂ_peer ? (c)->ਰਾਹੀਂ_peer->user_hostname : \
        (c)->origin->user_hostname)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CONN_HOST_DISPNAME(c) \
  ((c)->ਰਾਹੀਂ_peer ? (c)->ਰਾਹੀਂ_peer->user_hostname : (c)->origin->user_hostname)
#ਜੇ_ਅੰਤ

/* The end of connectdata. */

/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
ਬਣਤਰ ਸ਼ੁੱਧ_Info {
  /* PureInfo primary ip_quadruple is copied over from the connectdata
     struct in order to allow curl_easy_getinfo() to return this information
     even when the session handle is no longer associated with a connection,
     and also allow curl_easy_reset() to clear this information from the
     session handle without disturbing information which is still alive, and
     that might be reused, in the connection pool. */
  ਬਣਤਰ ip_quadruple primary;
  ਬਣਤਰ curl_certinfo certs; /* info about the certs. Asked for with
                                 CURLOPT_CERTINFO / CURLINFO_CERTINFO */
  time_t filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t request_ਮਾਪ; /* the amount of bytes sent in the request(s) */
  curl_off_t numconnects; /* how many new connections libcurl created */
  ਅੱਖਰ *contenttype; /* the content type of the object */
  ਅੱਖਰ *wouldredirect; /* URL this would have been redirected to if asked to */
  ਅੱਖਰ *effective_url; /* when needed */
  curl_off_t retry_after; /* info from Retry-After: header */
  ਅਚਲ ਅੱਖਰ *conn_scheme;
  ਪੂਰਨ httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  ਪੂਰਨ httpproxycode; /* response code from proxy when received separate */
  ਪੂਰਨ httpversion; /* the http version number X.Y = X*10+Y */
  uint32_t conn_protocol;
  uint32_t proxyauthavail; /* what proxy auth types were announced */
  uint32_t httpauthavail;  /* what host auth types were announced */
  uint32_t proxyauthpicked; /* selected proxy auth type */
  uint32_t httpauthpicked;  /* selected host auth type */
  uint32_t ਸਿਰਲੇਖ_ਮਾਪ;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  ਦੋਆਧਾਰੀ_ਅੰਕ(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਰਤਿਆ_proxy); /* the transfer used a proxy */
};

ਬਣਤਰ pgrs_dir {
  curl_off_t ਕੁੱਲ_ਮਾਪ; /* total expected bytes */
  curl_off_t cur_ਮਾਪ; /* transferred bytes so far */
  curl_off_t ਰਫਤਾਰ; /* bytes per second transferred */
  ਬਣਤਰ Curl_rlimit rlimit; /* speed limiting / pausing */
};

ਬਣਤਰ ਤਰੱਕੀ_2 {
  ਬਣਤਰ curltime now; /* current time of processing */
  ਬਣਤਰ curltime ਸ਼ੁਰੂ; /* when transfer was initialized, set once */

  ਬਣਤਰ pgrs_dir ul;
  ਬਣਤਰ pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_ਰਫਤਾਰ; /* uses the currently fastest transfer */
  curl_off_t earlydata_ਭੇਜਿਆ;

  ਬਣਤਰ {
    timediff_t startop_us; /* since start when operations started */
    timediff_t startsingle_us; /* since start when last request started */
    timediff_t startqueue_us; /* since start when last entered queueing */
    timediff_t startredirect_us; /* since start when last redirected */
    timediff_t lastshow_us; /* since start when last progress shown */
  } delta;
  ਬਣਤਰ {
    timediff_t spent_us; /* all time spent since start */
    timediff_t queued_us; /* time spent since startsingle's for queueing */
    timediff_t nslookup_us; /* same for name resolves */
    timediff_t connect_us; /* same for connects */
    timediff_t appconnect_us; /* same for application connects, e.g. TLS */
    timediff_t pretransfer_us; /* same until requests were sent */
    timediff_t starttransfer_us; /* same until responses started */
    timediff_t posttransfer_us; /* same until responses ended */
  } ਕੁੱਲ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t ਰਫਤਾਰ_amount[CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ];
  timediff_t ਰਫਤਾਰ_ਸਮਾਂ[CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ];
  uint32_t speeder_c;
  ਦੋਆਧਾਰੀ_ਅੰਕ(hide);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ul_ਮਾਪ_ਗਿਆਤ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(dl_ਮਾਪ_ਗਿਆਤ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਸਿਰਲੇਖਾਂ_ਬਾਹਰਲਾ); /* when the headers have been written */
  ਦੋਆਧਾਰੀ_ਅੰਕ(callback);  /* set when progress callback is used */
  ਦੋਆਧਾਰੀ_ਅੰਕ(startransfer_added);
};

ਬਣਤਰ auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਾ);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
ਬਣਤਰ Curl_ਅੰਕੜੇ_prio_ਗੰਢ {
  ਬਣਤਰ Curl_ਅੰਕੜੇ_prio_ਗੰਢ *ਅਗਲਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
};
#ਜੇ_ਅੰਤ

/* Timers */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
  EXPIRE_ਆਖਰੀ /* not an actual timer, used as a marker only */
} expire_id;

ਬਣਤਰ expire_timers {
  ਬਣਤਰ Curl_ਰੁੱਖ splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t ਵਿੱਥ_us[EXPIRE_ਆਖਰੀ];
  uint8_t ਅਗਲਾ[EXPIRE_ਆਖਰੀ];
  uint8_t ਪਹਿਲਾ;
};

/* individual pieces of the URL */
ਬਣਤਰ urlpieces {
  ਅੱਖਰ *options;
  ਅੱਖਰ *ਰਸਤਾ;
  ਅੱਖਰ *query;
};

ਬਣਤਰ UrlState {
  /* Origin of the initial (e.g. not followed) request of a transfer.
     Credentials from CURLOPT_* are only valid for this origin.
     Always set once a transfer starts searching for connections. */
  ਬਣਤਰ Curl_peer *initial_origin;
  /* Current origin of the transfer, changes to origin of follow
   * requests. */
  ਬਣਤਰ Curl_peer *origin;

  ਬਣਤਰ curltime keeps_ਰਫਤਾਰ; /* for the progress meter really */
  ਬਣਤਰ dynbuf headerb; /* buffer to store headers in */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  ਬਣਤਰ curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#ਜੇ_ਅੰਤ

  ਪੂਰਨ os_errno;  /* filled in with errno whenever an error occurs */
  ਪੂਰਨ ਬੇਨਤੀਆਂ; /* request counter: redirects + authentication retakes */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  ਖਾਲੀ (*prev_signal)(ਪੂਰਨ sig);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  ਅੱਖਰ *envproxy; /* last proxy string used for proxy-related state */
  ਬਣਤਰ digestdata digest;      /* state data for host Digest auth */
  ਬਣਤਰ digestdata proxydigest; /* state data for proxy Digest auth */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ auth authhost;  /* auth details for host */
  ਬਣਤਰ auth authproxy; /* auth details for proxy */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_ASYNC
  ਬਣਤਰ Curl_resolv_async *async;  /* asynchronous name resolver data */
#ਜੇ_ਅੰਤ

  ਬਣਤਰ expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  ਅੱਖਰ *most_recent_ftp_entrypath;
  ਅੱਖਰ *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  ਖਾਲੀ *ਵਿੱਚ;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  ਬਣਤਰ urlpieces ਉੱਪਰ;
  ਬਣਤਰ bufref url;        /* work URL, initially copied from UserDefined */
  ਬਣਤਰ bufref referer;    /* referer string */
  ਬਣਤਰ curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ Curl_llist httphdrs; /* received headers */
  ਬਣਤਰ curl_ਸਿਰਲੇਖ headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nh_origin;
  ਪੂਰਨ nh_request;
  size_t nh_ਗਿਣਤੀ; /* httphdrs count when the cache was filled */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਬਣਤਰ curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਬਣਤਰ curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
  ਬਣਤਰ store_netrc netrc;
#ਜੇ_ਅੰਤ

  ਬਣਤਰ Curl_creds *creds; /* Credentials for the origin only */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਅੱਖਰ *rangeline; /* allocated */
  ਅੱਖਰ *http_host; /* allocated */
  ਬਣਤਰ http_negotiation http_neg;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  ਬਣਤਰ Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_ਨਕਾਬ[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_ਅਗਲਾ_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_ਅਗਲਾ_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#ਜੇ_ਅੰਤ

  curl_off_t ਆਖਰੀ_conn_id; /* The last assigned connection or -1 */
  uint32_t ਆਖਰੀ_cpid; /* last connection pool id used */

#ਜੇ defined(USE_HTTP2) || defined(USE_HTTP3)
  ਪੂਰਨ ਭਾਰ; /* shallow copy of data->set */
#ਜੇ_ਅੰਤ
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  ਦੋਆਧਾਰੀ_ਅੰਕ(really_alive); /* transfer is really alive in multi, passed INIT */
  ਦੋਆਧਾਰੀ_ਅੰਕ(this_is_a_follow); /* this is a followed Location: request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(refused_ਧਾਰਾ); /* this was refused, try again */
  ਦੋਆਧਾਰੀ_ਅੰਕ(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(allow_ਪੋਰਟ); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  ਦੋਆਧਾਰੀ_ਅੰਕ(wildcardmatch); /* enable wildcard matching */
  ਦੋਆਧਾਰੀ_ਅੰਕ(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  ਦੋਆਧਾਰੀ_ਅੰਕ(use_range);
  ਦੋਆਧਾਰੀ_ਅੰਕ(rangestringalloc); /* the range string is malloc()'ed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਾ); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਦੋਆਧਾਰੀ_ਅੰਕ(cookie_engine);
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(prefer_ascii);   /* ASCII rather than binary */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIST_ONLY_PROTOCOL
  ਦੋਆਧਾਰੀ_ਅੰਕ(list_only);      /* list directory contents */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  ਦੋਆਧਾਰੀ_ਅੰਕ(upload);         /* upload request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਅੰਦਰੂਨੀ); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_ignorecustom); /* ignore custom method from now */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_hd_te); /* Added HTTP header TE: */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_hd_upgrade); /* Added HTTP header Upgrade: */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  ਦੋਆਧਾਰੀ_ਅੰਕ(maybe_folded);
  ਦੋਆਧਾਰੀ_ਅੰਕ(leading_unfold); /* unfold started, this is the leading bytes */
#ਜੇ_ਅੰਤ
};

/*
 * This 'UserDefined' struct must only contain data that is set once to go
 * for many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" MUST be defined within the
 * 'struct UrlState' instead. The only exceptions MUST note the changes in
 * the 'DynamicStatic' struct.
 * Character pointer fields point to dynamic storage, unless otherwise stated.
 */

ਬਣਤਰ Curl_multi;    /* declared in multihandle.c */

ਗਿਣਤੀ dupstring {
  STRING_CERT,            /* client certificate filename */
  STRING_CERT_TYPE,       /* format for certificate (default: PEM)*/
  STRING_ਕੁੰਜੀ,             /* private key filename */
  STRING_ਕੁੰਜੀ_PASSWD,      /* plain text private key password */
  STRING_ਕੁੰਜੀ_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_ਕੁੰਜੀ_PROXY,       /* private key filename */
  STRING_ਕੁੰਜੀ_PASSWD_PROXY, /* plain text private key password */
  STRING_ਕੁੰਜੀ_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#ਜੇ_ਅੰਤ
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_ਮੰਜ਼ਿਲ_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#ਜੇ_ਅੰਤ
  STRING_ਸੈੱਟ_RANGE,       /* range, if used */
  STRING_ਸੈੱਟ_REFERER,     /* custom string for the HTTP referer field */
  STRING_ਸੈੱਟ_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
ਵਰਤਿਆ*/
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_ਧਾਰਾ_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  STRING_SSH_ਨਿੱਜੀ_ਕੁੰਜੀ, /* path to the private key file for auth */
  STRING_SSH_ਜਨਤਕ_ਕੁੰਜੀ,  /* path to the public key file for auth */
  STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#ਜੇ_ਅੰਤ
  STRING_BEARER,                /* <bearer>, if used */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_ਰਸਤਾ,      /* path to Unix socket, if used */
#ਜੇ_ਅੰਤ
  STRING_ਨਿਸ਼ਾਨਾ,                /* CURLOPT_REQUEST_TARGET */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#ਜੇ_ਅੰਤ
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#ਜੇ_ਅੰਤ
  STRING_SSL_EC_CURVES,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_ਕੁੰਜੀ,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_ਸਿਰਲੇਖਾਂ, /* space-separated components to sign */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#ਜੇ_ਅੰਤ
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_ਜਨਤਕ,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_ਆਖਰੀ /* not used, an end-of-list marker */
};

ਗਿਣਤੀ dupblob {
  BLOB_CERT,
  BLOB_ਕੁੰਜੀ,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_ਕੁੰਜੀ_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#ਜੇ_ਅੰਤ
  BLOB_ਆਖਰੀ
};

ਬਣਤਰ UserDefined {
  FILE *err;         /* the stderr user data goes here */
  ਖਾਲੀ *debugdata;   /* the data that will be passed to fdebug */
  ਅੱਖਰ *errorbuffer; /* (Static) store failure messages in here */
  ਖਾਲੀ *ਬਾਹਰਲਾ;         /* CURLOPT_WRITEDATA */
  ਖਾਲੀ *ਵਿੱਚ_ਸੈੱਟ;      /* CURLOPT_READDATA */
  ਖਾਲੀ *writeheader; /* write the header to this if non-NULL */
  uint32_t httpauth;  /* kind of HTTP authentication to use (bitmask) */
  uint32_t proxyauth; /* kind of proxy authentication to use (bitmask) */
  ਖਾਲੀ *postfields;  /* if POST, set the fields' values here */
  ਅੱਖਰ *str_copypostfields; /* CURLOPT_COPYPOSTFIELDS value */
  curl_seek_callback seek_func;      /* function that seeks the input */
  curl_off_t postfieldsize; /* if POST, this might have a size to use instead
                               of strlen(), and then the data *may* be binary
                               (contain NUL bytes) */
  curl_write_callback fwrite_func;   /* function that stores the output */
  curl_write_callback fwrite_ਸਿਰਲੇਖ; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_ਸੈੱਟ; /* function that reads the input */
  curl_ਤਰੱਕੀ_callback fprogress; /* OLD and deprecated progress callback */
  curl_xferinfo_callback fxferinfo; /* progress callback */
  curl_debug_callback fdebug;      /* function that write informational data */
  curl_ioctl_callback ioctl_func;  /* function for I/O control */
  curl_sockopt_callback fsockopt;  /* function for setting socket options */
  ਖਾਲੀ *sockopt_client; /* pointer to pass to the socket options callback */
  curl_opensocket_callback fopensocket; /* function for checking/translating
                                           the address and opening the
                                           socket */
  ਖਾਲੀ *opensocket_client;
  curl_closesocket_callback fclosesocket; /* function for closing the
                                             socket */
  ਖਾਲੀ *closesocket_client;
  curl_prereq_callback fprereq; /* pre-initial request callback */
  ਖਾਲੀ *prereq_userp; /* pre-initial request user data */

  ਖਾਲੀ *seek_client;    /* pointer to pass to the seek callback */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  ਖਾਲੀ *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  ਖਾਲੀ *hsts_write_userp;
#ਜੇ_ਅੰਤ
  ਖਾਲੀ *ਤਰੱਕੀ_client; /* pointer to pass to the progress callback */
  ਖਾਲੀ *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t ਨੀਵਾਂ_ਰਫਤਾਰ_ਹੱਦਬੰਦੀ; /* bytes/second */
  curl_off_t max_send_ਰਫਤਾਰ; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_ਰਫਤਾਰ; /* high speed limit in bytes/second for
                                download */
  curl_off_t ਸੈੱਟ_resume_from;  /* continue [ftp] transfer from here */
  ਬਣਤਰ curl_slist *ਸਿਰਲੇਖਾਂ; /* linked list of extra headers */
  ਬਣਤਰ curl_httppost *httppost;  /* linked list of old POST data */
#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TELNET
  ਬਣਤਰ curl_slist *telnet_options; /* linked list of telnet options */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  ਬਣਤਰ curl_slist *connect_ਮੰਜ਼ਿਲ; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  time_t timevalue;       /* what time to compare with */
  ਬਣਤਰ ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  ਖਾਲੀ *ssl_fsslctxp;        /* parameter for callback */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਬਣਤਰ ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  ਬਣਤਰ curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#ਜੇ_ਅੰਤ
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t ਬਫਰ_ਮਾਪ;        /* size of receive buffer to use */
  uint32_t upload_ਬਫਰ_ਮਾਪ; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  ਖਾਲੀ *ਨਿੱਜੀ_ਅੰਕੜੇ; /* application-private data */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਬਣਤਰ curl_slist *http200aliases; /* linked list of aliases for http200 */
#ਜੇ_ਅੰਤ
  curl_off_t max_filesize; /* Maximum file size to download */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ਬਣਤਰ curl_slist *quote;     /* after connection is established */
  ਬਣਤਰ curl_slist *postquote; /* after the transfer */
  ਬਣਤਰ curl_slist *prequote; /* before the transfer, after type */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  ਖਾਲੀ *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  ਖਾਲੀ *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ u8_strset ਲੜੀਆਂ;
  ਬਣਤਰ curl_blob *blobs[BLOB_ਆਖਰੀ];
  uint32_t new_file_perms;      /* when creating remote files */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#ਜੇ_ਅੰਤ
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਖਾਲੀ *rtp_ਬਾਹਰਲਾ;     /* write RTP to this if non-NULL */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_ਸਿਰਾ_callback chunk_ਸਿਰਾ; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  ਖਾਲੀ *fnmatch_ਅੰਕੜੇ;
  ਖਾਲੀ *wildcardptr;
#ਜੇ_ਅੰਤ

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_ਸ਼ੁਰੂ_callback resolver_ਸ਼ੁਰੂ; /* optional callback called
                                                  before resolver start */
  ਖਾਲੀ *resolver_ਸ਼ੁਰੂ_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਖਾਲੀ *trailer_ਅੰਕੜੇ; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  ਬਣਤਰ curl_slist *mail_rcpt; /* linked list of mail recipients */
#ਜੇ_ਅੰਤ
  ਪੂਰਨ ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  ਪੂਰਨ tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  ਪੂਰਨ tcp_keepintvl;    /* seconds between TCP keepalive probes */
  ਪੂਰਨ tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#ਜੇ defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  ਪੂਰਨ ਭਾਰ;
#ਜੇ_ਅੰਤ
  ਛੋਟਾ maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_ਪੋਰਟ; /* which port to use (when not using default) */
  uint16_t ਨੀਵਾਂ_ਰਫਤਾਰ_ਸਮਾਂ;  /* number of seconds */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#ਜੇ_ਅੰਤ
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t ਵਿਧੀ;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#ਜੇ_ਅੰਤ
  uint8_t http_follow_ਬਹੁਲਕ; /* follow HTTP redirects */
  ਦੋਆਧਾਰੀ_ਅੰਕ(connect_only); /* make connection/request, then let application use the
                        socket */
  ਦੋਆਧਾਰੀ_ਅੰਕ(connect_only_ws); /* special websocket connect-only level */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  ਦੋਆਧਾਰੀ_ਅੰਕ(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
  ਦੋਆਧਾਰੀ_ਅੰਕ(mime_formescape);
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(is_fread_ਸੈੱਟ); /* has read callback been set to non-NULL? */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  ਦੋਆਧਾਰੀ_ਅੰਕ(tftp_no_options); /* do not send TFTP options requests */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(sep_ਸਿਰਲੇਖਾਂ);     /* handle host and proxy headers separately */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਦੋਆਧਾਰੀ_ਅੰਕ(cookiesession);   /* new cookie session? */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(crlf);            /* convert crlf on ftp upload(?) */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਦੋਆਧਾਰੀ_ਅੰਕ(ssh_compression);            /* enable SSH compression */
#ਜੇ_ਅੰਤ

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  ਦੋਆਧਾਰੀ_ਅੰਕ(get_filetime);     /* get the time and get of the remote file */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਦੋਆਧਾਰੀ_ਅੰਕ(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(prefer_ascii);     /* ASCII rather than binary */
  ਦੋਆਧਾਰੀ_ਅੰਕ(remote_append);    /* append, not overwrite, on upload */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIST_ONLY_PROTOCOL
  ਦੋਆਧਾਰੀ_ਅੰਕ(list_only);        /* list directory */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_ਪੋਰਟ);     /* use the FTP PORT command */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  ਦੋਆਧਾਰੀ_ਅੰਕ(wildcard_enabled); /* enable wildcard matching */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  ਦੋਆਧਾਰੀ_ਅੰਕ(allow_auth_ਮੰਜ਼ਿਲ_other_hosts);
  ਦੋਆਧਾਰੀ_ਅੰਕ(include_ਸਿਰਲੇਖ); /* include received protocol headers in data output */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_ਸੈੱਟ_referer); /* is a custom referer used */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_auto_referer); /* set "correct" referer when following
                             location: */
  ਦੋਆਧਾਰੀ_ਅੰਕ(opt_no_body);    /* as set with CURLOPT_NOBODY */
  ਦੋਆਧਾਰੀ_ਅੰਕ(verbose);        /* output verbosity */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reuse_forbid);   /* forbidden to be reused, close after use */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reuse_fresh);    /* do not reuse an existing connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_signal);      /* do not use any signal/alarm handler */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tcp_nodelay);    /* whether to enable TCP_NODELAY or not */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ignorecl);       /* ignore content length */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_te_skip);   /* pass the raw body data to the user, even when
                          transfer-encoded (chunked, compressed) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(http_ce_skip);   /* pass the raw body data to the user, even when
                          content-encoded (chunked, compressed) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(proxy_transfer_ਬਹੁਲਕ); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ਦੋਆਧਾਰੀ_ਅੰਕ(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(socks5_auth_only); /* SOCKS5 must authenticate */
  ਦੋਆਧਾਰੀ_ਅੰਕ(sasl_ir);         /* Enable/disable SASL initial response */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tcp_keepalive);  /* use TCP keepalives */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tcp_fastopen);   /* use TCP Fast Open */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ssl_enable_alpn);/* TLS ALPN extension? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਰਸਤਾ_as_is);     /* allow dotdots? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(suppress_connect_ਸਿਰਲੇਖਾਂ); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  ਦੋਆਧਾਰੀ_ਅੰਕ(dns_shuffle_ਪਤੇ); /* whether to shuffle addresses before use */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਦੋਆਧਾਰੀ_ਅੰਕ(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
ਸਿਰਲੇਖ*/
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਦੋਆਧਾਰੀ_ਅੰਕ(abstract_unix_socket);
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(disallow_username_ਵਿੱਚ_url); /* disallow username in URL */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਦੋਆਧਾਰੀ_ਅੰਕ(doh); /* DNS-over-HTTPS enabled */
  ਦੋਆਧਾਰੀ_ਅੰਕ(doh_verifypeer);     /* DoH certificate peer verification */
  ਦੋਆਧਾਰੀ_ਅੰਕ(doh_verifyhost);     /* DoH certificate hostname verification */
  ਦੋਆਧਾਰੀ_ਅੰਕ(doh_verifystatus);   /* DoH certificate status verification */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(http09_allowed); /* allow HTTP/0.9 responses */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  ਦੋਆਧਾਰੀ_ਅੰਕ(ws_raw_ਬਹੁਲਕ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ws_no_auto_pong);
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(post301); /* keep POSTs as POSTs after a 301 request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(post302); /* keep POSTs as POSTs after a 302 request */
  ਦੋਆਧਾਰੀ_ਅੰਕ(post303); /* keep POSTs as POSTs after a 303 request */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  ਦੋਆਧਾਰੀ_ਅੰਕ(engine_default);
#ਜੇ_ਅੰਤ
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_MIME_POST(a)                                                 \
  ((a)->ਸੈੱਟ.mimepostp && ((a)->ਸੈੱਟ.mimepostp->ਕਿਸਮ != MIMEKIND_NONE))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_MIME_POST(a) FALSE
#ਜੇ_ਅੰਤ

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ multi_sub_xfer_ਪੂਰਾ_cb(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ Curl_easy *master,
                                    CURLcode ਨਤੀਜਾ);

/*
 * The 'connectdata' struct MUST have all the connection oriented stuff as we
 * may have several simultaneous connections and connection structs in memory.
 *
 * The 'struct UserDefined' must only contain data that is set once to go for
 * many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" must be defined within the
 * 'struct UrlState' instead.
 */

ਬਣਤਰ Curl_easy {
  /* First a simple identifier to more easily detect if a user mixes up this
     easy handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  uint32_t magic;
  /* once an easy handle is added to a multi, either explicitly by the
   * libcurl application or implicitly during `curl_easy_perform()`,
   * a unique identifier inside this one multi instance. */
  uint32_t mid;
  CURLMstate mstate;  /* the handle's state */
  CURLcode ਨਤੀਜਾ;   /* previous result */

  ਬਣਤਰ connectdata *conn;
  ਬਣਤਰ Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  ਬਣਤਰ Curl_eapi_ਢੇਰ callstack; /* easy api calls ongoing */

  ਬਣਤਰ Curl_share *share;    /* Share, handles global variable mutexing */

  ਬਣਤਰ Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  ਬਣਤਰ CURLMsg ਸੁਨੇਹਾ; /* A single posted message. */

  /* once an easy handle is tied to a connection pool a non-negative number to
     distinguish this transfer from other using the same pool. For easier
     tracking in log output. This may wrap around after CURL_OFF_T_MAX to 0
     again, so it has no uniqueness guarantee for large processings. Note: it
     has no uniqueness either IFF more than one connection pool is used by the
     libcurl application. */
  curl_off_t id;
  uint32_t master_mid; /* if set, this transfer belongs to a master */
  multi_sub_xfer_ਪੂਰਾ_cb *sub_xfer_ਪੂਰਾ;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  ਬਣਤਰ Curl_hash meta_hash;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  ਬਣਤਰ PslCache *psl;        /* The associated PSL cache. */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ SingleRequest req;    /* Request-specific data */
  ਬਣਤਰ UserDefined ਸੈੱਟ;      /* values set by the libcurl user */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਬਣਤਰ CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  ਬਣਤਰ hsts *hsts;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  ਬਣਤਰ altsvcinfo *asi;      /* the alt-svc cache */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ ਤਰੱਕੀ_2 ਤਰੱਕੀ;    /* for all the progress meter data */
  ਬਣਤਰ UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wildcard; /* wildcard download state info */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ ਸ਼ੁੱਧ_Info info;        /* stats, reports and info data */
  ਬਣਤਰ curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->ਸੈੱਟ.ਲੜੀਆਂ, (uint8_t)(id))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EASY_STR_ਸੈੱਟ(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->ਸੈੱਟ.ਲੜੀਆਂ, (uint8_t)(id), s, slen)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->ਸੈੱਟ.ਲੜੀਆਂ, (uint8_t)(id), s)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->ਸੈੱਟ.ਲੜੀਆਂ, (uint8_t)(id))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->ਸੈੱਟ.ਲੜੀਆਂ, (uint8_t)(id))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIBCURL_NAME "libcurl"

#ਜੇ_ਅੰਤ /* HEADER_CURL_URLDATA_H */
