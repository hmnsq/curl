#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_URLDATA_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_URLDATA_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DEFAULT_USER     "anonymous"
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DEFAULT_PASSWORD "ftp@example.com"

#ಆದರೆ !defined(_WIN32) && !defined(MSDOS)
/* do FTP line-end CRLF => LF conversions on platforms that prefer LF-only. It
   also means: keep CRLF line endings on the CRLF platforms */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PREFER_LF_LINEENDS
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_CONNCACHE_ಗಾತ್ರ 5

/* length of longest IPv6 address string including the null-terminator */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_IPADR_LEN ಗಾತ್ರ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Max string input length is a precaution against abuse and to detect junk
   input easier and better. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ 8000000

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

#ಅನ್ತರ್ಭಾವ "api.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "creds.h"
#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "formdata.h"
#ಅನ್ತರ್ಭಾವ "http_chunks.h" /* for the structs and enum stuff */
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "peer.h"
#ಅನ್ತರ್ಭಾವ "proxy.h"
#ಅನ್ತರ್ಭಾವ "splay.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "dynhds.h"
#ಅನ್ತರ್ಭಾವ "request.h"
#ಅನ್ತರ್ಭಾವ "ratelimit.h"
#ಅನ್ತರ್ಭಾವ "netrc.h"
#ಅನ್ತರ್ಭಾವ "u8_strset.h"
#ಅನ್ತರ್ಭಾವ "vdns/asyn.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_config.h"

/* On error return, the value of `pnwritten` has no meaning */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode (Curl_send)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ಸ್ಥಿರಾಂಕ uint8_t *buf,       /* data to write */
                             size_t len,               /* amount to send */
                             bool eos,                 /* last chunk */
                             size_t *pnwritten);       /* how much sent */

/* On error return, the value of `pnread` has no meaning */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode (Curl_recv)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,   /* transfer */
                             int8_t sockindex,            /* socketindex */
                             ಅಕ್ಷರ *buf,                /* store data here */
                             size_t len,               /* max amount to read */
                             size_t *pnread);          /* how much received */

#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "protocol.h"
#ಅನ್ತರ್ಭಾವ "ftp.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "mqtt.h"
#ಅನ್ತರ್ಭಾವ "ftplistparser.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
#  ifdef HAVE_GSSAPPLE
#    include <GSS/gssapi.h>
#    include <GSS/gssapi_oid.h>
#  elif defined(HAVE_GSSGNU)
#    include <gss.h>
#  elif defined(HAVE_GSSAPI_H)
#    include <gssapi.h>
#  ಇಲ್ಲದಿದ್ದರೆ /* MIT Kerberos */
#    include <gssapi/gssapi.h>
#    include <gssapi/gssapi_krb5.h> /* for GSS_C_CHANNEL_BOUND_FLAG in 1.19+ */
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2
#ಅನ್ತರ್ಭಾವ <libssh2.h>
#ಅನ್ತರ್ಭಾವ <libssh2_sftp.h>
#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBSSH2 */

#ವ್ಯಾಖ್ಯಾನಿಸು READBUFFER_ಗಾತ್ರ CURL_MAX_WRITE_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು READBUFFER_MAX  CURL_MAX_READ_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು READBUFFER_MIN  1024

/* The default upload buffer size, should not be smaller than
   CURL_MAX_WRITE_SIZE, as it needs to hold a full buffer as could be sent in
   a write callback.

   The size was 16KB for many years but was bumped to 64KB because it makes
   libcurl able to do significantly faster uploads in some circumstances. Even
   larger buffers can help further, but this is deemed a fair memory/speed
   compromise. */
#ವ್ಯಾಖ್ಯಾನಿಸು UPLOADBUFFER_DEFAULT 65536
#ವ್ಯಾಖ್ಯಾನಿಸು UPLOADBUFFER_MAX     (2 * 1024 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು UPLOADBUFFER_MIN     CURL_MAX_WRITE_ಗಾತ್ರ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
/* Struct used for Digest challenge-response authentication */
ರಚನೆ digestdata {
  ರಚನೆ Curl_creds *creds;
  ರಚನೆ Curl_peer *origin;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  BYTE *ಒಳಹರಿವು_token;
  size_t ಒಳಹರಿವು_token_len;
  CtxtHandle *http_context;
#ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ *nonce;
  ಅಕ್ಷರ *cnonce;
  ಅಕ್ಷರ *realm;
  ಅಕ್ಷರ *opaque;
  ಅಕ್ಷರ *qop;
  ಅಕ್ಷರ *algorithm;
  ಪೂರ್ಣಾಂಕ nc; /* nonce count */
  uint8_t algo;
  ಬಿಟ್(stale); /* set true for re-negotiation */
  ಬಿಟ್(userhash);
#ಆದರೆ_ಅಂತ್ಯ
};
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  NTLMSTATE_NONE,
  NTLMSTATE_TYPE1,
  NTLMSTATE_TYPE2,
  NTLMSTATE_TYPE3,
  NTLMSTATE_ಕೊನೆಯ
} curlntlm;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NEGO_AUTH_NONE   0U
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NEGO_AUTH_RECV   1U
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NEGO_AUTH_ಕಳುಹಿಸಿದ   2U
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NEGO_AUTH_ಮುಗಿದ   3U
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NEGO_AUTH_SUCC   4U

/*
 * Boolean values that concerns this connection.
 */
ರಚನೆ Connect_ಬಿಟ್ಟುಗಳು {
  ಬಿಟ್(connect_only);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಬಿಟ್(origin_is_proxy);  /* if set, the connection's origin is a proxy */
  ಬಿಟ್(socks5_authenticated); /* SOCKS5 authenticated on FIRSTSOCKET */
#ಆದರೆ_ಅಂತ್ಯ
  /* always modify bits.close with the connclose() and connkeep() macros! */
  ಬಿಟ್(close); /* if set, we close the connection after this request */
  ಬಿಟ್(reuse); /* if set, this is a reused connection */
  ಬಿಟ್(altused); /* this is an alt-svc "redirect" */
  ಬಿಟ್(ipv6);    /* we communicate with a site using an IPv6 address */
  ಬಿಟ್(do_ಇನ್ನಷ್ಟು); /* this is set TRUE if the ->curl_do_more() function is
                   supposed to be called, after ->curl_do() */
  ಬಿಟ್(protoconnstart);/* the protocol layer has STARTED its operation after
                         the TCP layer connect */
  ಬಿಟ್(retry);         /* this connection is about to get closed and then
                         re-attempted at another connection. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಬಿಟ್(ftp_use_epsv);  /* As set with CURLOPT_FTP_USE_EPSV, but if we find out
                         EPSV does not work we disable it for the forthcoming
ವಿನಂತಿಗಳು*/
  ಬಿಟ್(ftp_use_eprt);  /* As set with CURLOPT_FTP_USE_EPRT, but if we find out
                         EPRT does not work we disable it for the forthcoming
ವಿನಂತಿಗಳು*/
  ಬಿಟ್(ftp_use_ದತ್ತಾಂಶ_ssl); /* Enabled SSL for the data connection */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(bound); /* set true if bind() has already been done on this socket/
                 connection */
  ಬಿಟ್(upgrade_ಒಳ_ಪ್ರಗತಿ); /* protocol upgrade is in progress */
  ಬಿಟ್(multiplex); /* connection is multiplexed */
  ಬಿಟ್(tcp_fastopen); /* use TCP Fast Open */
  ಬಿಟ್(tls_enable_alpn); /* TLS ALPN extension? */
  ಬಿಟ್(sock_accepted); /* TRUE if the SECONDARYSOCKET was created with
                         accept() */
  ಬಿಟ್(parallel_connect); /* set TRUE when a parallel connect attempt has
                            started (happy eyeballs) */
  ಬಿಟ್(aborted); /* connection was aborted, e.g. in unclean state */
  ಬಿಟ್(no_reuse); /* connection should not be reused */
  ಬಿಟ್(shutdown_handler); /* connection shutdown: handler shut down */
  ಬಿಟ್(shutdown_filters); /* connection shutdown: filters shut down */
  ಬಿಟ್(dns_resolved); /* DNS records for connection were resolved */
};

ರಚನೆ hostname {
  ಅಕ್ಷರ *rawalloc; /* allocated "raw" version of the name */
  ಅಕ್ಷರ *encalloc; /* allocated IDN-encoded version of the name */
  ಅಕ್ಷರ *name;     /* name to use internally, might be encoded, might be raw */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dispname; /* name to display, as 'name' might be encoded */
};

#ವ್ಯಾಖ್ಯಾನಿಸು FIRSTSOCKET     0
#ವ್ಯಾಖ್ಯಾನಿಸು SECONDARYSOCKET 1

#ವ್ಯಾಖ್ಯಾನಿಸು TRNSPRT_NONE 0
#ವ್ಯಾಖ್ಯಾನಿಸು TRNSPRT_TCP  3
#ವ್ಯಾಖ್ಯಾನಿಸು TRNSPRT_UDP  4
#ವ್ಯಾಖ್ಯಾನಿಸು TRNSPRT_QUIC 5
#ವ್ಯಾಖ್ಯಾನಿಸು TRNSPRT_UNIX 6

#ವ್ಯಾಖ್ಯಾನಿಸು TRNSPRT_IS_DGRAM(x)   (((x) == TRNSPRT_UDP) || ((x) == TRNSPRT_QUIC))

ರಚನೆ ip_quadruple {
  ಅಕ್ಷರ remote_ip[MAX_IPADR_LEN];
  ಅಕ್ಷರ local_ip[MAX_IPADR_LEN];
  uint16_t remote_ಪೋರ್ಟ್;
  uint16_t local_ಪೋರ್ಟ್;
  uint8_t transport;
};

#ವ್ಯಾಖ್ಯಾನಿಸು CUR_IP_QUAD_HAS_PORTS(x)      \
  (((x)->transport == TRNSPRT_TCP) || \
   ((x)->transport == TRNSPRT_UDP) || \
   ((x)->transport == TRNSPRT_QUIC))

/*
 * The connectdata struct contains all fields and variables that should be
 * unique for an entire connection.
 */
ರಚನೆ connectdata {
  curl_off_t connection_id; /* Contains a unique number to make it easier to
                               track the connections in the log output */

  /* A connection cache from a SHARE might be used in several multi handles.
   * We MUST not reuse connections that are running in another multi,
   * for concurrency reasons. That multi might run in another thread.
   * `attached_multi` is set by the first transfer attached and cleared
   * when the last one is detached.
   * NEVER call anything on this multi, check for equality. */
  ರಚನೆ Curl_multi *attached_multi;

  /* Who the connection is talking to, ultimately */
  ರಚನೆ Curl_peer *origin; /* connection ultimately talks to this */
  ರಚನೆ Curl_peer *ಮೂಲಕ_peer; /* if set, connection really talks to this */
  ರಚನೆ Curl_peer *origin2; /* origin of SECONDARYSOCKET */
  ರಚನೆ Curl_peer *ಮೂಲಕ_peer2; /* peer of SECONDARYSOCKET */
  ರಚನೆ Curl_creds *creds; /* When connection itself is tied to credentials */
  ರಚನೆ Curl_peer *creds_origin; /* origin tied credentials are for */
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme; /* Connection's real protocol handler */

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the connection.
   * Elements need to be added with their own destructor to be invoked when
   * the connection is cleaned up (see Curl_hash_add2()).*/
  ರಚನೆ Curl_hash meta_hash;

  ಅಕ್ಷರ *destination; /* hostname+port, used in conncache */

  ರಚನೆ curltime created; /* creation time */
  /* timediff_t are deltas from `created`*/
  timediff_t lastused_ms; /* when returned to the connection pool as idle */
  timediff_t lastchecked_ms; /* when last checked alive status */
  timediff_t lastupkeep_ms; /* when last done conn_upkeep */
  ರಚನೆ {
    ರಚನೆ curltime at[2]; /* first Curl_conn_connect() for each socket */
    bool ಗಣ[2];
  } connect_started;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ರಚನೆ proxy_info socks_proxy;
  ರಚನೆ proxy_info http_proxy;
#ಆದರೆ_ಅಂತ್ಯ

  /* A connection can have one or two sockets and connection filters.
   * The protocol using the 2nd one is FTP for CONTROL+DATA sockets */
  ರಚನೆ Curl_cfilter *cfilter[2]; /* connection filters */
  Curl_recv *recv[2];
  Curl_send *send[2];
  curl_socket_t sock[2];

#ವ್ಯಾಖ್ಯಾನಿಸು CONN_SOCK_IDX_VALID(i)    (((i) >= 0) && ((i) < 2))

  ರಚನೆ {
    timediff_t ಆರಂಭ_ms[2]; /* when filter shutdown started */
    timediff_t timeout_ms; /* 0 means no timeout */
  } shutdown;

  curl_closesocket_callback fclosesocket; /* function closing the socket(s) */
  ಶೂನ್ಯ *closesocket_client;

  ರಚನೆ ssl_filter_config ssl_config;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ರಚನೆ ssl_filter_config proxy_ssl_config;
#ಆದರೆ_ಅಂತ್ಯ
  ಅಕ್ಷರ *options; /* options string, allocated */

  /*************** Request - specific items ************/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  CtxtHandle *sslContext;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  curlntlm http_ntlm_state;
  curlntlm proxy_ntlm_state;
#ಆದರೆ_ಅಂತ್ಯ

  /* When this connection is created, store the conditions for the local end
     bind. This is stored before the actual bind and before any connection is
     made and will serve the purpose of being used for comparison reasons so
     that subsequent bound-requested connections are not accidentally reusing
     wrong connections. */
  ಅಕ್ಷರ *interface_opts;
  ರಚನೆ Connect_ಬಿಟ್ಟುಗಳು ಬಿಟ್ಟುಗಳು;    /* various state-flags for this connection */
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಪೂರ್ಣಾಂಕ socks5_gssapi_enctype;
#ಆದರೆ_ಅಂತ್ಯ

  /* This is used by the connection pool logic. If this returns TRUE, this
     handle is still used by one or more easy handles and can only used by any
     other easy handle without careful consideration (== only for
     multiplexing) and it cannot be used by another multi handle! */
#ವ್ಯಾಖ್ಯಾನಿಸು CONN_INUSE(c) (!!(c)->attached_xfers)
  uint32_t attached_xfers; /* # of attached easy handles */

  uint32_t cpid; /* connection pool id */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#ಆದರೆ_ಅಂತ್ಯ
  uint16_t localportrange;
  uint16_t localport;
  int8_t recv_idx;  /* on which socket index to receive, default 0 */
  int8_t send_idx;  /* on which socket index to send, default 0 */
  uint8_t transport_wanted; /* one of the TRNSPRT_* defines. Not necessarily
   the transport the connection ends using due to Alt-Svc and happy
   eyeballing. Use Curl_conn_get_transport() for actual value once the
   connection is set up. */
  uint8_t ip_ಆವೃತ್ತಿ; /* copied from the Curl_easy at creation time */
  /* HTTP version last responded with by the server or negotiated via ALPN.
   * 0 at start, then one of 09, 10, 11, etc. */
  uint8_t httpversion_ನೋಡಿದ;
  uint8_t gssapi_delegation; /* inherited from set.gssapi_delegation */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
  uint8_t http_negotiate_state;
  uint8_t proxy_negotiate_state;
#ಆದರೆ_ಅಂತ್ಯ
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CONN_HOST_DISPNAME(c) \
  ((c)->socks_proxy.peer ? (c)->socks_proxy.peer->user_hostname : \
    (c)->http_proxy.peer ? (c)->http_proxy.peer->user_hostname : \
      (c)->ಮೂಲಕ_peer ? (c)->ಮೂಲಕ_peer->user_hostname : \
        (c)->origin->user_hostname)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CONN_HOST_DISPNAME(c) \
  ((c)->ಮೂಲಕ_peer ? (c)->ಮೂಲಕ_peer->user_hostname : (c)->origin->user_hostname)
#ಆದರೆ_ಅಂತ್ಯ

/* The end of connectdata. */

/*
 * Struct to keep statistical and informational data.
 * All variables in this struct must be initialized/reset in Curl_initinfo().
 */
ರಚನೆ ಶುದ್ಧ_Info {
  /* PureInfo primary ip_quadruple is copied over from the connectdata
     struct in order to allow curl_easy_getinfo() to return this information
     even when the session handle is no longer associated with a connection,
     and also allow curl_easy_reset() to clear this information from the
     session handle without disturbing information which is still alive, and
     that might be reused, in the connection pool. */
  ರಚನೆ ip_quadruple primary;
  ರಚನೆ curl_certinfo certs; /* info about the certs. Asked for with
                                 CURLOPT_CERTINFO / CURLINFO_CERTINFO */
  time_t filetime; /* If requested, this is might get set. Set to -1 if the
                      time was unretrievable. */
  curl_off_t request_ಗಾತ್ರ; /* the amount of bytes sent in the request(s) */
  curl_off_t numconnects; /* how many new connections libcurl created */
  ಅಕ್ಷರ *contenttype; /* the content type of the object */
  ಅಕ್ಷರ *wouldredirect; /* URL this would have been redirected to if asked to */
  ಅಕ್ಷರ *effective_url; /* when needed */
  curl_off_t retry_after; /* info from Retry-After: header */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *conn_scheme;
  ಪೂರ್ಣಾಂಕ httpcode;  /* Recent HTTP, FTP, RTSP or SMTP response code */
  ಪೂರ್ಣಾಂಕ httpproxycode; /* response code from proxy when received separate */
  ಪೂರ್ಣಾಂಕ httpversion; /* the http version number X.Y = X*10+Y */
  uint32_t conn_protocol;
  uint32_t proxyauthavail; /* what proxy auth types were announced */
  uint32_t httpauthavail;  /* what host auth types were announced */
  uint32_t proxyauthpicked; /* selected proxy auth type */
  uint32_t httpauthpicked;  /* selected host auth type */
  uint32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;  /* size of read header(s) in bytes */
  uint8_t pxcode; /* holds a CURLproxycode */
  ಬಿಟ್(timecond);  /* set to TRUE if the time condition did not match, which
                     thus made the document NOT get fetched */
  ಬಿಟ್(ಬಳಸಿದ_proxy); /* the transfer used a proxy */
};

ರಚನೆ pgrs_dir {
  curl_off_t ಒಟ್ಟು_ಗಾತ್ರ; /* total expected bytes */
  curl_off_t cur_ಗಾತ್ರ; /* transferred bytes so far */
  curl_off_t ವೇಗ; /* bytes per second transferred */
  ರಚನೆ Curl_rlimit rlimit; /* speed limiting / pausing */
};

ರಚನೆ ಪ್ರಗತಿ_2 {
  ರಚನೆ curltime now; /* current time of processing */
  ರಚನೆ curltime ಆರಂಭ; /* when transfer was initialized, set once */

  ರಚನೆ pgrs_dir ul;
  ರಚನೆ pgrs_dir dl;
  curl_off_t deliver; /* amount of data delivered to application */
  curl_off_t current_ವೇಗ; /* uses the currently fastest transfer */
  curl_off_t earlydata_ಕಳುಹಿಸಿದ;

  ರಚನೆ {
    timediff_t startop_us; /* since start when operations started */
    timediff_t startsingle_us; /* since start when last request started */
    timediff_t startqueue_us; /* since start when last entered queueing */
    timediff_t startredirect_us; /* since start when last redirected */
    timediff_t lastshow_us; /* since start when last progress shown */
  } delta;
  ರಚನೆ {
    timediff_t spent_us; /* all time spent since start */
    timediff_t queued_us; /* time spent since startsingle's for queueing */
    timediff_t nslookup_us; /* same for name resolves */
    timediff_t connect_us; /* same for connects */
    timediff_t appconnect_us; /* same for application connects, e.g. TLS */
    timediff_t pretransfer_us; /* same until requests were sent */
    timediff_t starttransfer_us; /* same until responses started */
    timediff_t posttransfer_us; /* same until responses ended */
  } ಒಟ್ಟು;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ವೇಗ_ದಾಖಲೆಗಳು (5 + 1) /* 6 entries for 5 seconds */

  curl_off_t ವೇಗ_amount[CURL_ವೇಗ_ದಾಖಲೆಗಳು];
  timediff_t ವೇಗ_ಸಮಯ[CURL_ವೇಗ_ದಾಖಲೆಗಳು];
  uint32_t speeder_c;
  ಬಿಟ್(hide);
  ಬಿಟ್(ul_ಗಾತ್ರ_ತಿಳಿದ);
  ಬಿಟ್(dl_ಗಾತ್ರ_ತಿಳಿದ);
  ಬಿಟ್(ಶೀರ್ಷಿಕೆಗಳು_ಹೊರಹೋಗು); /* when the headers have been written */
  ಬಿಟ್(callback);  /* set when progress callback is used */
  ಬಿಟ್(startransfer_added);
};

ರಚನೆ auth {
  uint32_t want;  /* Bitmask set to the authentication methods wanted by app
                     (with CURLOPT_HTTPAUTH or CURLOPT_PROXYAUTH). */
  uint32_t picked;
  uint32_t avail; /* Bitmask for what the server reports to support for this
                     resource */
  ಬಿಟ್(ಮುಗಿದ);  /* TRUE when the auth phase is done and ready to do the
                 actual request */
  ಬಿಟ್(multipass); /* TRUE if this is not yet authenticated but within the
                     auth multipass negotiation */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
ರಚನೆ Curl_ದತ್ತಾಂಶ_prio_ಗಂಟು {
  ರಚನೆ Curl_ದತ್ತಾಂಶ_prio_ಗಂಟು *ಮುಂದಿನ;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
};
#ಆದರೆ_ಅಂತ್ಯ

/* Timers */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  EXPIRE_ಕೊನೆಯ /* not an actual timer, used as a marker only */
} expire_id;

ರಚನೆ expire_timers {
  ರಚನೆ Curl_ವೃಕ್ಷ splaynode; /* for the splay stuff */
  /* microsecond offset from Curl_timeouts base timestamp */
  timediff_t ಸ್ಥಾನಾಂತರ_us[EXPIRE_ಕೊನೆಯ];
  uint8_t ಮುಂದಿನ[EXPIRE_ಕೊನೆಯ];
  uint8_t ಮೊದಲ;
};

/* individual pieces of the URL */
ರಚನೆ urlpieces {
  ಅಕ್ಷರ *options;
  ಅಕ್ಷರ *ಮಾರ್ಗ;
  ಅಕ್ಷರ *query;
};

ರಚನೆ UrlState {
  /* Origin of the initial (e.g. not followed) request of a transfer.
     Credentials from CURLOPT_* are only valid for this origin.
     Always set once a transfer starts searching for connections. */
  ರಚನೆ Curl_peer *initial_origin;
  /* Current origin of the transfer, changes to origin of follow
   * requests. */
  ರಚನೆ Curl_peer *origin;

  ರಚನೆ curltime keeps_ವೇಗ; /* for the progress meter really */
  ರಚನೆ dynbuf headerb; /* buffer to store headers in */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  ರಚನೆ curl_slist *hstslist; /* list of HSTS files set by
                                  curl_easy_setopt(HSTS) calls */
#ಆದರೆ_ಅಂತ್ಯ

  ಪೂರ್ಣಾಂಕ os_errno;  /* filled in with errno whenever an error occurs */
  ಪೂರ್ಣಾಂಕ ವಿನಂತಿಗಳು; /* request counter: redirects + authentication retakes */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGNAL
  /* storage for the previous bag^H^H^HSIGPIPE signal handler :-) */
  ಶೂನ್ಯ (*prev_signal)(ಪೂರ್ಣಾಂಕ sig);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
  ಅಕ್ಷರ *envproxy; /* last proxy string used for proxy-related state */
  ರಚನೆ digestdata digest;      /* state data for host Digest auth */
  ರಚನೆ digestdata proxydigest; /* state data for proxy Digest auth */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ auth authhost;  /* auth details for host */
  ರಚನೆ auth authproxy; /* auth details for proxy */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_ASYNC
  ರಚನೆ Curl_resolv_async *async;  /* asynchronous name resolver data */
#ಆದರೆ_ಅಂತ್ಯ

  ರಚನೆ expire_timers timeouts; /* expire timeouts */

  /* a place to store the most recently set (S)FTP entrypath */
  ಅಕ್ಷರ *most_recent_ftp_entrypath;
  ಅಕ್ಷರ *range; /* range, if used. See README for detailed specification on
                  this syntax. */
  curl_off_t resume_from; /* continue [ftp] transfer from here */
  curl_off_t infilesize; /* size of file to upload, -1 means unknown.
                            Copied from set.filesize at start of operation */
  curl_read_callback fread_func; /* read callback/function */
  ಶೂನ್ಯ *ಒಳ;                      /* CURLOPT_READDATA */
  CURLU *uh; /* URL handle for the current parsed URL */
  ರಚನೆ urlpieces ಮೇಲೆ;
  ರಚನೆ bufref url;        /* work URL, initially copied from UserDefined */
  ರಚನೆ bufref referer;    /* referer string */
  ರಚನೆ curl_slist *resolve; /* set to point to the set.resolve list when
                                 this should be dealt with in pretransfer */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  curl_mimepart *mimepost;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FORM_API
  curl_mimepart *formp; /* storage for old API form-posting, allocated on
                           demand */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ Curl_llist httphdrs; /* received headers */
  ರಚನೆ curl_ಶೀರ್ಷಿಕೆ headerout[2]; /* for external purposes */
  /* curl_easy_nextheader() cache parameters */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nh_origin;
  ಪೂರ್ಣಾಂಕ nh_request;
  size_t nh_ಎಣಿಕೆ; /* httphdrs count when the cache was filled */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  ರಚನೆ curl_slist *cookielist; /* list of cookie files set by
                                    curl_easy_setopt(COOKIEFILE) calls */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ರಚನೆ curl_trc_feat *feat; /* opt. trace feature transfer is part of */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
  ರಚನೆ store_netrc netrc;
#ಆದರೆ_ಅಂತ್ಯ

  ರಚನೆ Curl_creds *creds; /* Credentials for the origin only */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಅಕ್ಷರ *rangeline; /* allocated */
  ಅಕ್ಷರ *http_host; /* allocated */
  ರಚನೆ http_negotiation http_neg;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  /* This RTSP state information survives requests and connections */
  ರಚನೆ Curl_peer *rtsp_session_origin; /* origin that issued the session id,
                                            NULL when set by the application */
  uint8_t rtp_channel_ಮುಖವಾಡ[32]; /* for the correctness checking of the
                                         interleaved data */
  uint32_t rtsp_ಮುಂದಿನ_client_CSeq; /* the session's next client CSeq */
  uint32_t rtsp_ಮುಂದಿನ_server_CSeq; /* the session's next server CSeq */
  uint32_t rtsp_CSeq_recv; /* most recent CSeq received */
#ಆದರೆ_ಅಂತ್ಯ

  curl_off_t ಕೊನೆಯ_conn_id; /* The last assigned connection or -1 */
  uint32_t ಕೊನೆಯ_cpid; /* last connection pool id used */

#ಆದರೆ defined(USE_HTTP2) || defined(USE_HTTP3)
  ಪೂರ್ಣಾಂಕ ತೂಕ; /* shallow copy of data->set */
#ಆದರೆ_ಅಂತ್ಯ
  uint16_t followlocation; /* redirect counter */
  uint8_t retrycount; /* number of retries on a new connection, up to
                               CONN_MAX_RETRIES */
  uint8_t httpreq; /* Curl_HttpReq; what kind of HTTP request (if any)
                            is this */

  ಬಿಟ್(really_alive); /* transfer is really alive in multi, passed INIT */
  ಬಿಟ್(this_is_a_follow); /* this is a followed Location: request */
  ಬಿಟ್(refused_ಹರಿವು); /* this was refused, try again */
  ಬಿಟ್(errorbuf); /* Set to TRUE if the error buffer is already filled in.
                    This must be set to FALSE every time _easy_perform() is
                    called. */
  ಬಿಟ್(allow_ಪೋರ್ಟ್); /* Is set.use_port allowed to take effect or not. This
                      is always set TRUE when curl_easy_perform() is called. */
  ಬಿಟ್(authproblem); /* TRUE if there is some problem authenticating */
  /* set after initial USER failure, to prevent an authentication loop */
  ಬಿಟ್(wildcardmatch); /* enable wildcard matching */
  ಬಿಟ್(disableexpect);    /* TRUE if Expect: is disabled due to a previous
                            417 response */
  ಬಿಟ್(use_range);
  ಬಿಟ್(rangestringalloc); /* the range string is malloc()'ed */
  ಬಿಟ್(ಮುಗಿದ); /* set to FALSE when Curl_init_transfer() is called and set to
                TRUE when multi_done() is called, to prevent multi_done() from
                being invoked twice. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  ಬಿಟ್(cookie_engine);
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(prefer_ascii);   /* ASCII rather than binary */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIST_ONLY_PROTOCOL
  ಬಿಟ್(list_only);      /* list directory contents */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(wildcard_resolve); /* Set to true if any resolve change is a wildcard */
  ಬಿಟ್(upload);         /* upload request */
  ಬಿಟ್(ಆಂತರಿಕ); /* internal: true if this easy handle was created for
                    internal use and the user does not have ownership of the
                    handle. */
  ಬಿಟ್(http_ignorecustom); /* ignore custom method from now */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಬಿಟ್(http_hd_te); /* Added HTTP header TE: */
  ಬಿಟ್(http_hd_upgrade); /* Added HTTP header Upgrade: */
  ಬಿಟ್(http_hd_h2_settings); /* Added HTTP header H2Settings: */
  ಬಿಟ್(maybe_folded);
  ಬಿಟ್(leading_unfold); /* unfold started, this is the leading bytes */
#ಆದರೆ_ಅಂತ್ಯ
};

/*
 * This 'UserDefined' struct must only contain data that is set once to go
 * for many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" MUST be defined within the
 * 'struct UrlState' instead. The only exceptions MUST note the changes in
 * the 'DynamicStatic' struct.
 * Character pointer fields point to dynamic storage, unless otherwise stated.
 */

ರಚನೆ Curl_multi;    /* declared in multihandle.c */

ಎಣಿಕೆ dupstring {
  STRING_CERT,            /* client certificate filename */
  STRING_CERT_TYPE,       /* format for certificate (default: PEM)*/
  STRING_ಕೀಲಿ,             /* private key filename */
  STRING_ಕೀಲಿ_PASSWD,      /* plain text private key password */
  STRING_ಕೀಲಿ_TYPE,        /* format for private key (default: PEM) */
  STRING_SSL_CAPATH,      /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE,      /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY, /* public key file to verify peer against */
  STRING_SSL_CIPHER_LIST, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE,     /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT,  /* issuer cert file to check certificate */
  STRING_SERVICE_NAME,    /* Service name */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  STRING_CERT_PROXY,      /* client certificate filename */
  STRING_CERT_TYPE_PROXY, /* format for certificate (default: PEM)*/
  STRING_ಕೀಲಿ_PROXY,       /* private key filename */
  STRING_ಕೀಲಿ_PASSWD_PROXY, /* plain text private key password */
  STRING_ಕೀಲಿ_TYPE_PROXY,  /* format for private key (default: PEM) */
  STRING_SSL_CAPATH_PROXY, /* CA directory name (does not work on Windows) */
  STRING_SSL_CAFILE_PROXY, /* certificate file to verify peer against */
  STRING_SSL_PINNEDPUBLICKEY_PROXY, /* public key file to verify proxy */
  STRING_SSL_CIPHER_LIST_PROXY, /* list of ciphers to use */
  STRING_SSL_CIPHER13_LIST_PROXY, /* list of TLS 1.3 ciphers to use */
  STRING_SSL_CRLFILE_PROXY, /* CRL file to check certificate */
  STRING_SSL_ISSUERCERT_PROXY, /* issuer cert file to check certificate */
  STRING_PROXY_SERVICE_NAME, /* Proxy service name */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  STRING_COOKIE,          /* HTTP cookie string to send */
  STRING_COOKIEJAR,       /* dump all cookies to this file */
#ಆದರೆ_ಅಂತ್ಯ
  STRING_CUSTOMREQUEST,   /* HTTP/FTP/RTSP request/method to use */
  STRING_DEFAULT_PROTOCOL, /* Protocol to use when the URL does not specify */
  STRING_INTERFACE_OPT,   /* CURLOPT_INTERFACE config */
  STRING_ENCODING,        /* Accept-Encoding string */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  STRING_FTP_ACCOUNT,     /* ftp account data */
  STRING_FTP_ALTERNATIVE_ಗಮ್ಯ_USER, /* command to send if USER/PASS fails */
  STRING_FTPPORT,         /* port to send with the FTP PORT command */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
  STRING_NETRC_FILE,      /* if not NULL, use this instead of trying to find
                             $HOME/.netrc */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  STRING_PROXY,           /* proxy to use */
  STRING_PRE_PROXY,       /* pre socks proxy to use */
#ಆದರೆ_ಅಂತ್ಯ
  STRING_ಗಣ_RANGE,       /* range, if used */
  STRING_ಗಣ_REFERER,     /* custom string for the HTTP referer field */
  STRING_ಗಣ_URL,         /* what original URL to work on */
  STRING_USERAGENT,       /* User-Agent string */
  STRING_SSL_ENGINE,      /* name of SSL engine */
  STRING_USERNAME,        /* <username>, if used */
  STRING_PASSWORD,        /* <password>, if used */
  STRING_OPTIONS,         /* <options>, if used */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  STRING_PROXYUSERNAME,   /* Proxy <username>, if used */
  STRING_PROXYPASSWORD,   /* Proxy <password>, if used */
  STRING_NOPROXY,         /* List of hosts which should not use the proxy, if
ಬಳಸಿದ*/
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  STRING_RTSP_SESSION_ID, /* Session ID to use */
  STRING_RTSP_ಹರಿವು_URI, /* Stream URI for this request */
  STRING_RTSP_TRANSPORT,  /* Transport for this session */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  STRING_SSH_ಖಾಸಗಿ_ಕೀಲಿ, /* path to the private key file for auth */
  STRING_SSH_ಬಹಿರಂಗ_ಕೀಲಿ,  /* path to the public key file for auth */
  STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5, /* md5 of host public key in ASCII hex */
  STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256, /* sha256 of host public key in base64 */
  STRING_SSH_KNOWNHOSTS,  /* filename of knownhosts file */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  STRING_MAIL_FROM,
  STRING_MAIL_AUTH,
#ಆದರೆ_ಅಂತ್ಯ
  STRING_BEARER,                /* <bearer>, if used */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  STRING_UNIX_SOCKET_ಮಾರ್ಗ,      /* path to Unix socket, if used */
#ಆದರೆ_ಅಂತ್ಯ
  STRING_ಗುರಿ,                /* CURLOPT_REQUEST_TARGET */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  STRING_DOH,                   /* CURLOPT_DOH_URL */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  STRING_ALTSVC,                /* CURLOPT_ALTSVC */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  STRING_HSTS,                  /* CURLOPT_HSTS */
#ಆದರೆ_ಅಂತ್ಯ
  STRING_SASL_AUTHZID,          /* CURLOPT_SASL_AUTHZID */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
  STRING_DNS_SERVERS,
  STRING_DNS_INTERFACE,
  STRING_DNS_LOCAL_IP4,
  STRING_DNS_LOCAL_IP6,
#ಆದರೆ_ಅಂತ್ಯ
  STRING_SSL_EC_CURVES,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_AWS
  STRING_AWS_SIGV4, /* Parameters for V4 signature */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
  STRING_HTTPSIG_ಕೀಲಿ,    /* hex-encoded key data */
  STRING_HTTPSIG_KEYID,  /* key identifier */
  STRING_HTTPSIG_ಶೀರ್ಷಿಕೆಗಳು, /* space-separated components to sign */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  STRING_HAPROXY_CLIENT_IP,     /* CURLOPT_HAPROXY_CLIENT_IP */
#ಆದರೆ_ಅಂತ್ಯ
  STRING_ECH_CONFIG,            /* CURLOPT_ECH_CONFIG */
  STRING_ECH_ಬಹಿರಂಗ,            /* CURLOPT_ECH_PUBLIC */
  STRING_SSL_SIGNATURE_ALGORITHMS, /* CURLOPT_SSL_SIGNATURE_ALGORITHMS */

  STRING_ಕೊನೆಯ /* not used, an end-of-list marker */
};

ಎಣಿಕೆ dupblob {
  BLOB_CERT,
  BLOB_ಕೀಲಿ,
  BLOB_SSL_ISSUERCERT,
  BLOB_CAINFO,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  BLOB_CERT_PROXY,
  BLOB_ಕೀಲಿ_PROXY,
  BLOB_SSL_ISSUERCERT_PROXY,
  BLOB_CAINFO_PROXY,
#ಆದರೆ_ಅಂತ್ಯ
  BLOB_ಕೊನೆಯ
};

ರಚನೆ UserDefined {
  FILE *err;         /* the stderr user data goes here */
  ಶೂನ್ಯ *debugdata;   /* the data that will be passed to fdebug */
  ಅಕ್ಷರ *errorbuffer; /* (Static) store failure messages in here */
  ಶೂನ್ಯ *ಹೊರಹೋಗು;         /* CURLOPT_WRITEDATA */
  ಶೂನ್ಯ *ಒಳ_ಗಣ;      /* CURLOPT_READDATA */
  ಶೂನ್ಯ *writeheader; /* write the header to this if non-NULL */
  uint32_t httpauth;  /* kind of HTTP authentication to use (bitmask) */
  uint32_t proxyauth; /* kind of proxy authentication to use (bitmask) */
  ಶೂನ್ಯ *postfields;  /* if POST, set the fields' values here */
  ಅಕ್ಷರ *str_copypostfields; /* CURLOPT_COPYPOSTFIELDS value */
  curl_seek_callback seek_func;      /* function that seeks the input */
  curl_off_t postfieldsize; /* if POST, this might have a size to use instead
                               of strlen(), and then the data *may* be binary
                               (contain NUL bytes) */
  curl_write_callback fwrite_func;   /* function that stores the output */
  curl_write_callback fwrite_ಶೀರ್ಷಿಕೆ; /* function that stores headers */
  curl_write_callback fwrite_rtp;    /* function that stores interleaved RTP */
  curl_read_callback fread_func_ಗಣ; /* function that reads the input */
  curl_ಪ್ರಗತಿ_callback fprogress; /* OLD and deprecated progress callback */
  curl_xferinfo_callback fxferinfo; /* progress callback */
  curl_debug_callback fdebug;      /* function that write informational data */
  curl_ioctl_callback ioctl_func;  /* function for I/O control */
  curl_sockopt_callback fsockopt;  /* function for setting socket options */
  ಶೂನ್ಯ *sockopt_client; /* pointer to pass to the socket options callback */
  curl_opensocket_callback fopensocket; /* function for checking/translating
                                           the address and opening the
                                           socket */
  ಶೂನ್ಯ *opensocket_client;
  curl_closesocket_callback fclosesocket; /* function for closing the
                                             socket */
  ಶೂನ್ಯ *closesocket_client;
  curl_prereq_callback fprereq; /* pre-initial request callback */
  ಶೂನ್ಯ *prereq_userp; /* pre-initial request user data */

  ಶೂನ್ಯ *seek_client;    /* pointer to pass to the seek callback */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  curl_hstsread_callback hsts_read;
  ಶೂನ್ಯ *hsts_read_userp;
  curl_hstswrite_callback hsts_write;
  ಶೂನ್ಯ *hsts_write_userp;
#ಆದರೆ_ಅಂತ್ಯ
  ಶೂನ್ಯ *ಪ್ರಗತಿ_client; /* pointer to pass to the progress callback */
  ಶೂನ್ಯ *ioctl_client;   /* pointer to pass to the ioctl callback */
  timediff_t conn_max_idle_ms; /* max idle time to allow a connection that
                                  is to be reused */
  timediff_t conn_max_age_ms; /* max time since creation to allow a
                                 connection that is to be reused */
  curl_off_t filesize;  /* size of file to upload, -1 means unknown */
  curl_off_t ಕೆಳಗಿನ_ವೇಗ_ಎಲ್ಲೆ; /* bytes/second */
  curl_off_t max_send_ವೇಗ; /* high speed limit in bytes/second for upload */
  curl_off_t max_recv_ವೇಗ; /* high speed limit in bytes/second for
                                download */
  curl_off_t ಗಣ_resume_from;  /* continue [ftp] transfer from here */
  ರಚನೆ curl_slist *ಶೀರ್ಷಿಕೆಗಳು; /* linked list of extra headers */
  ರಚನೆ curl_httppost *httppost;  /* linked list of old POST data */
#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  curl_mimepart *mimepostp;  /* MIME/POST data. */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TELNET
  ರಚನೆ curl_slist *telnet_options; /* linked list of telnet options */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ curl_slist *resolve;     /* list of names to add/remove from
                                     DNS cache */
  ರಚನೆ curl_slist *connect_ಗಮ್ಯ; /* list of host:port mappings to override
                                    the hostname and port to connect to */
  time_t timevalue;       /* what time to compare with */
  ರಚನೆ ssl_easy_config ssl;  /* user defined SSL stuff */
  curl_ssl_ctx_callback ssl_fsslctx; /* function to initialize SSL ctx */
  ಶೂನ್ಯ *ssl_fsslctxp;        /* parameter for callback */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ರಚನೆ ssl_easy_config proxy_ssl;  /* user defined SSL stuff for proxy */
  ರಚನೆ curl_slist *proxyheaders; /* linked list of extra CONNECT headers */
  uint16_t proxyport;       /* If non-zero, use this port number by
                               default. If the proxy string features a
                               ":[port]" that one will override this. */
  uint8_t proxytype; /* what kind of proxy */
  uint8_t socks5auth;/* kind of SOCKS5 authentication to use (bitmask) */
#ಆದರೆ_ಅಂತ್ಯ
  timediff_t dns_cache_timeout_ms; /* DNS cache timeout (milliseconds) */
  uint32_t ಬಫರ್_ಗಾತ್ರ;        /* size of receive buffer to use */
  uint32_t upload_ಬಫರ್_ಗಾತ್ರ; /* size of upload buffer to use, keep it >=
                                  CURL_MAX_WRITE_SIZE */
  ಶೂನ್ಯ *ಖಾಸಗಿ_ದತ್ತಾಂಶ; /* application-private data */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ರಚನೆ curl_slist *http200aliases; /* linked list of aliases for http200 */
#ಆದರೆ_ಅಂತ್ಯ
  curl_off_t max_filesize; /* Maximum file size to download */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  timediff_t accepttimeout;   /* in milliseconds, 0 means no timeout */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ರಚನೆ curl_slist *quote;     /* after connection is established */
  ರಚನೆ curl_slist *postquote; /* after the transfer */
  ರಚನೆ curl_slist *prequote; /* before the transfer, after type */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2
  curl_sshhostkeycallback ssh_hostkeyfunc; /* hostkey check callback */
  ಶೂನ್ಯ *ssh_hostkeyfunc_userp;         /* custom pointer to callback */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  curl_sshkeycallback ssh_keyfunc; /* key matching callback */
  ಶೂನ್ಯ *ssh_keyfunc_userp;         /* custom pointer to callback */
  uint32_t ssh_auth_types;   /* allowed SSH auth types */
  uint32_t new_directory_perms; /* when creating remote dirs */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ u8_strset ಅಕ್ಷರಸರಣಿಗಳು;
  ರಚನೆ curl_blob *blobs[BLOB_ಕೊನೆಯ];
  uint32_t new_file_perms;      /* when creating remote files */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  uint32_t scope_id;  /* Scope id for IPv6 */
#ಆದರೆ_ಅಂತ್ಯ
  curl_prot_t allowed_protocols;
  curl_prot_t redir_protocols;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಶೂನ್ಯ *rtp_ಹೊರಹೋಗು;     /* write RTP to this if non-NULL */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  curl_chunk_bgn_callback chunk_bgn; /* called before part of transfer
                                        starts */
  curl_chunk_ಕೊನೆ_callback chunk_ಕೊನೆ; /* called after part transferring
                                        stopped */
  curl_fnmatch_callback fnmatch; /* callback to decide which file corresponds
                                    to pattern (e.g. if WILDCARDMATCH is on) */
  ಶೂನ್ಯ *fnmatch_ದತ್ತಾಂಶ;
  ಶೂನ್ಯ *wildcardptr;
#ಆದರೆ_ಅಂತ್ಯ

  timediff_t timeout;   /* ms, 0 means no timeout */
  timediff_t connecttimeout; /* ms, 0 means default timeout */
  timediff_t happy_eyeballs_timeout; /* ms, 0 is a valid value */
  timediff_t server_response_timeout; /* ms, 0 means no timeout */
  timediff_t shutdowntimeout; /* ms, 0 means default timeout */
  curl_resolver_ಆರಂಭ_callback resolver_ಆರಂಭ; /* optional callback called
                                                  before resolver start */
  ಶೂನ್ಯ *resolver_ಆರಂಭ_client; /* pointer to pass to resolver start callback */
  timediff_t upkeep_interval_ms; /* Time between calls for connection
                                    upkeep. */
  CURLU *uh; /* URL handle for the current parsed URL */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಶೂನ್ಯ *trailer_ದತ್ತಾಂಶ; /* pointer to pass to trailer data callback */
  curl_trailer_callback trailer_callback; /* trailing data callback */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  ರಚನೆ curl_slist *mail_rcpt; /* linked list of mail recipients */
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ ssl_ca_cache_timeout;  /* Certificate store cache timeout (seconds) */

  ಪೂರ್ಣಾಂಕ tcp_keepidle;     /* seconds in idle before sending keepalive probe */
  ಪೂರ್ಣಾಂಕ tcp_keepintvl;    /* seconds between TCP keepalive probes */
  ಪೂರ್ಣಾಂಕ tcp_keepcnt;      /* maximum number of keepalive probes */

  uint32_t maxconnects; /* Max idle connections in the connection cache */
#ಆದರೆ defined(USE_HTTP2) || defined(USE_HTTP3)
  /* Priority information for an easy handle in relation to others on the same
     connection. */
  ಪೂರ್ಣಾಂಕ ತೂಕ;
#ಆದರೆ_ಅಂತ್ಯ
  ಗಿಡ್ಡ maxredirs;    /* maximum no. of http(s) redirects to follow,
                         set to -1 for infinity */
  uint16_t expect_100_timeout; /* in milliseconds */
  uint16_t use_ಪೋರ್ಟ್; /* which port to use (when not using default) */
  uint16_t ಕೆಳಗಿನ_ವೇಗ_ಸಮಯ;  /* number of seconds */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BINDLOCAL
  uint16_t localport; /* local port number to bind to */
  uint16_t localportrange; /* number of additional port numbers to test
                                    in case the 'localport' one cannot be
                                    bind()ed */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP
  uint16_t tftp_blksize;    /* in bytes, 0 means use default */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  uint8_t rtspreq; /* RTSP request type */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
  uint8_t use_netrc;        /* enum CURL_NETRC_OPTION values */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  /* Despite the name, ftp_create_missing_dirs is for FTP(S) and SFTP
     1 - create directories that do not exist
     2 - the same but also allow MKD to fail once
   */
  uint8_t ftp_create_missing_dirs;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  uint8_t ftp_filemethod; /* how to get to a file: curl_ftpfile */
  uint8_t ftpsslauth; /* what AUTH XXX to try: curl_ftpauth */
  uint8_t ftp_ccc;   /* FTP CCC options: curl_ftpccc */
#ಆದರೆ_ಅಂತ್ಯ
  uint8_t httpsig_algorithm; /* CURLHTTPSIG_* algorithm for RFC 9421 */
  uint8_t use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or IMAP or
                        POP3 or others! (type: curl_usessl)*/
  uint8_t timecondition; /* kind of time comparison: curl_TimeCond */
  uint8_t ವಿಧಾನ;   /* what kind of HTTP request: Curl_HttpReq */
  uint8_t httpwant; /* when non-zero, a specific HTTP version requested
                             to be used in the library's request(s) */
  uint8_t ipver; /* the CURL_IPRESOLVE_* defines in the public header
                          file 0 - whatever, 1 - v2, 2 - v6 */
  uint8_t upload_flags; /* flags set by CURLOPT_UPLOAD_FLAGS */
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS-API/SSPI credential delegation, see CURLOPT_GSSAPI_DELEGATION */
  uint8_t gssapi_delegation;
#ಆದರೆ_ಅಂತ್ಯ
  uint8_t http_follow_ಬಹುಲಕ; /* follow HTTP redirects */
  ಬಿಟ್(connect_only); /* make connection/request, then let application use the
                        socket */
  ಬಿಟ್(connect_only_ws); /* special websocket connect-only level */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  ಬಿಟ್(mail_rcpt_allowfails); /* allow RCPT TO command to fail for some
                                recipients */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
  ಬಿಟ್(mime_formescape);
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(is_fread_ಗಣ); /* has read callback been set to non-NULL? */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP
  ಬಿಟ್(tftp_no_options); /* do not send TFTP options requests */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(sep_ಶೀರ್ಷಿಕೆಗಳು);     /* handle host and proxy headers separately */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  ಬಿಟ್(cookiesession);   /* new cookie session? */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(crlf);            /* convert crlf on ftp upload(?) */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  ಬಿಟ್(ssh_compression);            /* enable SSH compression */
#ಆದರೆ_ಅಂತ್ಯ

/* Here follows boolean settings that define how to behave during
   this session. They are STATIC, set by libcurl users or at least initially
   and they do not change during operations. */
  ಬಿಟ್(quick_exit);       /* set 1L when it is okay to leak things (like
                            threads), as we are about to exit() anyway and
                            do not want lengthy cleanups to delay termination,
                            e.g. after a DNS timeout */
  ಬಿಟ್(get_filetime);     /* get the time and get of the remote file */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಬಿಟ್(tunnel_thru_httpproxy); /* use CONNECT through an HTTP proxy */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(prefer_ascii);     /* ASCII rather than binary */
  ಬಿಟ್(remote_append);    /* append, not overwrite, on upload */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIST_ONLY_PROTOCOL
  ಬಿಟ್(list_only);        /* list directory */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಬಿಟ್(ftp_use_ಪೋರ್ಟ್);     /* use the FTP PORT command */
  ಬಿಟ್(ftp_use_epsv);     /* if EPSV is to be attempted or not */
  ಬಿಟ್(ftp_use_eprt);     /* if EPRT is to be attempted or not */
  ಬಿಟ್(ftp_use_pret);     /* if PRET is to be used before PASV or not */
  ಬಿಟ್(ftp_skip_ip);      /* skip the IP address the FTP server passes on to
                            us */
  ಬಿಟ್(wildcard_enabled); /* enable wildcard matching */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(http_fail_on_error);  /* fail on HTTP error codes >= 400 */
  ಬಿಟ್(http_keep_sending_on_error); /* for HTTP status codes >= 300 */
  ಬಿಟ್(http_transfer_encoding); /* request compressed HTTP transfer-encoding */
  ಬಿಟ್(allow_auth_ಗಮ್ಯ_other_hosts);
  ಬಿಟ್(include_ಶೀರ್ಷಿಕೆ); /* include received protocol headers in data output */
  ಬಿಟ್(http_ಗಣ_referer); /* is a custom referer used */
  ಬಿಟ್(http_auto_referer); /* set "correct" referer when following
                             location: */
  ಬಿಟ್(opt_no_body);    /* as set with CURLOPT_NOBODY */
  ಬಿಟ್(verbose);        /* output verbosity */
  ಬಿಟ್(reuse_forbid);   /* forbidden to be reused, close after use */
  ಬಿಟ್(reuse_fresh);    /* do not reuse an existing connection */
  ಬಿಟ್(no_signal);      /* do not use any signal/alarm handler */
  ಬಿಟ್(tcp_nodelay);    /* whether to enable TCP_NODELAY or not */
  ಬಿಟ್(ignorecl);       /* ignore content length */
  ಬಿಟ್(http_te_skip);   /* pass the raw body data to the user, even when
                          transfer-encoded (chunked, compressed) */
  ಬಿಟ್(http_ce_skip);   /* pass the raw body data to the user, even when
                          content-encoded (chunked, compressed) */
  ಬಿಟ್(proxy_transfer_ಬಹುಲಕ); /* set transfer mode (;type=<a|i>) when doing
                               FTP via an HTTP proxy */
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಬಿಟ್(socks5_gssapi_nec); /* Flag to support NEC SOCKS5 server */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(socks5_auth_only); /* SOCKS5 must authenticate */
  ಬಿಟ್(sasl_ir);         /* Enable/disable SASL initial response */
  ಬಿಟ್(tcp_keepalive);  /* use TCP keepalives */
  ಬಿಟ್(tcp_fastopen);   /* use TCP Fast Open */
  ಬಿಟ್(ssl_enable_alpn);/* TLS ALPN extension? */
  ಬಿಟ್(ಮಾರ್ಗ_as_is);     /* allow dotdots? */
  ಬಿಟ್(pipewait);       /* wait for multiplex status before starting a new
                          connection */
  ಬಿಟ್(suppress_connect_ಶೀರ್ಷಿಕೆಗಳು); /* suppress proxy CONNECT response headers
                                    from user callbacks */
  ಬಿಟ್(dns_shuffle_ವಿಳಾಸಗಳು); /* whether to shuffle addresses before use */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಬಿಟ್(haproxyprotocol); /* whether to send HAProxy PROXY protocol v1
ಶೀರ್ಷಿಕೆ*/
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ಬಿಟ್(abstract_unix_socket);
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(disallow_username_ಒಳ_url); /* disallow username in URL */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ಬಿಟ್(doh); /* DNS-over-HTTPS enabled */
  ಬಿಟ್(doh_verifypeer);     /* DoH certificate peer verification */
  ಬಿಟ್(doh_verifyhost);     /* DoH certificate hostname verification */
  ಬಿಟ್(doh_verifystatus);   /* DoH certificate status verification */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(http09_allowed); /* allow HTTP/0.9 responses */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  ಬಿಟ್(ws_raw_ಬಹುಲಕ);
  ಬಿಟ್(ws_no_auto_pong);
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(post301); /* keep POSTs as POSTs after a 301 request */
  ಬಿಟ್(post302); /* keep POSTs as POSTs after a 302 request */
  ಬಿಟ್(post303); /* keep POSTs as POSTs after a 303 request */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  ಬಿಟ್(engine_default);
#ಆದರೆ_ಅಂತ್ಯ
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
#ವ್ಯಾಖ್ಯಾನಿಸು IS_MIME_POST(a)                                                 \
  ((a)->ಗಣ.mimepostp && ((a)->ಗಣ.mimepostp->ಬಗೆ != MIMEKIND_NONE))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು IS_MIME_POST(a) FALSE
#ಆದರೆ_ಅಂತ್ಯ

/* callback that gets called when the transfer `data` is done and
 * `data->master_mid` is set to an existing easy handle. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ multi_sub_xfer_ಮುಗಿದ_cb(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ Curl_easy *master,
                                    CURLcode ಫಲಿತಾಂಶ);

/*
 * The 'connectdata' struct MUST have all the connection oriented stuff as we
 * may have several simultaneous connections and connection structs in memory.
 *
 * The 'struct UserDefined' must only contain data that is set once to go for
 * many (perhaps) independent connections. Values that are generated or
 * calculated internally for the "session handle" must be defined within the
 * 'struct UrlState' instead.
 */

ರಚನೆ Curl_easy {
  /* First a simple identifier to more easily detect if a user mixes up this
     easy handle with a multi handle. Set this to CURLEASY_MAGIC_NUMBER */
  uint32_t magic;
  /* once an easy handle is added to a multi, either explicitly by the
   * libcurl application or implicitly during `curl_easy_perform()`,
   * a unique identifier inside this one multi instance. */
  uint32_t mid;
  CURLMstate mstate;  /* the handle's state */
  CURLcode ಫಲಿತಾಂಶ;   /* previous result */

  ರಚನೆ connectdata *conn;
  ರಚನೆ Curl_multi *multi;    /* if non-NULL, points to the multi handle
                                  struct to which this "belongs" when used by
                                  the multi interface */
  ರಚನೆ Curl_eapi_ರಾಶಿ callstack; /* easy api calls ongoing */

  ರಚನೆ Curl_share *share;    /* Share, handles global variable mutexing */

  ರಚನೆ Curl_multi *multi_easy; /* if non-NULL, points to the multi handle
                                    struct to which this "belongs" when used
                                    by the easy interface */
  ರಚನೆ CURLMsg ಸಂದೇಶ; /* A single posted message. */

  /* once an easy handle is tied to a connection pool a non-negative number to
     distinguish this transfer from other using the same pool. For easier
     tracking in log output. This may wrap around after CURL_OFF_T_MAX to 0
     again, so it has no uniqueness guarantee for large processings. Note: it
     has no uniqueness either IFF more than one connection pool is used by the
     libcurl application. */
  curl_off_t id;
  uint32_t master_mid; /* if set, this transfer belongs to a master */
  multi_sub_xfer_ಮುಗಿದ_cb *sub_xfer_ಮುಗಿದ;

  /* `meta_hash` is a general key-value store for implementations
   * with the lifetime of the easy handle.
   * Elements need to be added with their own destructor to be invoked when
   * the easy handle is cleaned up (see Curl_hash_add2()).*/
  ರಚನೆ Curl_hash meta_hash;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  ರಚನೆ PslCache *psl;        /* The associated PSL cache. */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ SingleRequest req;    /* Request-specific data */
  ರಚನೆ UserDefined ಗಣ;      /* values set by the libcurl user */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  ರಚನೆ CookieInfo *cookies;  /* the cookies, read from files and servers.
                                  NOTE that the 'cookie' field in the
                                  UserDefined struct defines if the "engine"
                                  is to be used or not. */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  ರಚನೆ hsts *hsts;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  ರಚನೆ altsvcinfo *asi;      /* the alt-svc cache */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ ಪ್ರಗತಿ_2 ಪ್ರಗತಿ;    /* for all the progress meter data */
  ರಚನೆ UrlState state;       /* struct for fields used for state info and
                                  other dynamic purposes */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wildcard; /* wildcard download state info */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ ಶುದ್ಧ_Info info;        /* stats, reports and info data */
  ರಚನೆ curl_tlssessioninfo tsi; /* Information about the TLS session, only
                                     valid after a client has asked for it */
};

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EASY_STR(d, id) \
  Curl_u8_strset_get(&(d)->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು, (uint8_t)(id))
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EASY_STR_ಗಣ(d, id, s, slen) \
  Curl_u8_strset_setx(&(d)->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು, (uint8_t)(id), s, slen)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EASY_STR_SETN(d, id, s) \
  Curl_u8_strset_setn(&(d)->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು, (uint8_t)(id), s)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EASY_STR_CLEAR(d, id) \
  Curl_u8_strset_unset(&(d)->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು, (uint8_t)(id))
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EASY_STR_CLEAR0(d, id) \
  Curl_u8_strset_unset0(&(d)->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು, (uint8_t)(id))

#ವ್ಯಾಖ್ಯಾನಿಸು LIBCURL_NAME "libcurl"

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_URLDATA_H */
