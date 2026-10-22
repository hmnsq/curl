#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_PROTOCOL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_PROTOCOL_H
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

ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;
ਬਣਤਰ easy_pollset;

/* Known protocol default port numbers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_FTP    21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_FTPS   990
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_TELNET 23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_HTTP   80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_HTTPS  443
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_DICT   2628
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_LDAP   389
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_LDAPS  636
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_TFTP   69
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_SSH    22
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_IMAP   143
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_IMAPS  993
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_POP3   110
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_POP3_S  995
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_SMTP   25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_SMTPS  465 /* sometimes called SSMTP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_RTSP   554
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_GOPHER 70
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_SOCKS  1080
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_MQTT   1883
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੋਰਟ_MQTTS  8883

/* CURLPROTO_GOPHERS (29) is the highest publicly used protocol bit number,
 * the rest are internal information. If we use higher bits we only do this on
 * platforms that have a >= 64-bit type and then we use such a type for the
 * protocol fields in the scheme definition.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_WS     (1L << 30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_WSS    ((curl_prot_t)1 << 31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_MQTTS  (1LL << 32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_SOCKS  (1LL << 33)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_64ALL ((uint64_t)0xffffffffffffffff)

/* the default protocols accepting a redirect to */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_REDIR (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | \
                         CURLPROTO_FTPS)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ curl_off_t curl_prot_t;

/* This mask is for all the old protocols that are provided and defined in the
   public header and shall exclude protocols added since which are not exposed
   in the API */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLPROTO_ਨਕਾਬ   0x3fffffff

/* Convenience defines for checking protocols or their SSL based version. Each
   protocol scheme should only ever have a single CURLPROTO_ in its protocol
   field. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTO_FAMILY_HTTP (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_WS | \
                           CURLPROTO_WSS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTO_FAMILY_FTP  (CURLPROTO_FTP | CURLPROTO_FTPS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTO_FAMILY_POP3 (CURLPROTO_POP3 | CURLPROTO_POP3S)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTO_FAMILY_SMTP (CURLPROTO_SMTP | CURLPROTO_SMTPS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTO_FAMILY_SSH  (CURLPROTO_SCP | CURLPROTO_SFTP)

#ਜੇ !defined(CURL_DISABLE_FTP) || defined(USE_SSH) || \
  !defined(CURL_DISABLE_POP3)
/* these protocols support CURLOPT_DIRLISTONLY */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LIST_ONLY_PROTOCOL 1
#ਜੇ_ਅੰਤ

/* When redirecting transfers. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  FOLLOW_NONE,  /* not used within the function, a placeholder to allow
                   initing to this */
  FOLLOW_FAKE,  /* only records stuff, not actually following */
  FOLLOW_RETRY, /* set if this is a request retry as opposed to a real
                   redirect following */
  FOLLOW_REDIR /* a full true redirect */
} followtype;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  DOMORE_GOBACK = -1,
  DOMORE_INCOMPLETE = 0,
  DOMORE_ਪੂਰਾ = 1
} domore;

/*
 * Specific protocol handler, an implementation of one or more URI schemes.
 */
ਬਣਤਰ Curl_protocol {
  /* Complement to setup_connection_internals(). This is done before the
     transfer "owns" the connection. */
  CURLcode (*ਤਿਆਰੀ_connection)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn);

  /* These two functions MUST be set to be protocol dependent */
  CURLcode (*do_it)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);
  CURLcode (*ਪੂਰਾ)(ਬਣਤਰ Curl_easy *, CURLcode, bool);

  /* If the curl_do() function is better made in two halves, this
   * curl_do_more() function will be called afterwards, if set. For example
   * for doing the FTP stuff after the PASV/PORT command. The second
   * argument is an output parameter that MUST be set to one of the
   * DOMORE_* values: DOMORE_INCOMPLETE if more do_more work remains,
   * DOMORE_DONE when the second phase is complete, or DOMORE_GOBACK
   * to return to the regular DO/DOING handling.
   */
  CURLcode (*do_ਹੋਰ)(ਬਣਤਰ Curl_easy *, domore *);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * after the connect() and everything is done, as a step in the connection.
   * The 'done' pointer points to a bool that should be set to TRUE if the
   * function completes before return. If it does not complete, the caller
   * should call the ->connecting() function until it is.
   */
  CURLcode (*connect_it)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);

  /* See above. */
  CURLcode (*connecting)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);
  CURLcode (*doing)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);

  /* Called from the multi interface during the PROTOCONNECT phase, and it
     should then return a proper fd set */
  CURLcode (*proto_pollset)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps);

  /* Called from the multi interface during the DOING phase, and it should
     then return a proper fd set */
  CURLcode (*doing_pollset)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps);

  /* Called from the multi interface during the DO_MORE phase, and it should
     then return a proper fd set */
  CURLcode (*domore_pollset)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ easy_pollset *ps);

  /* Called from the multi interface during the DO_DONE, PERFORM and
     WAITPERFORM phases, and it should then return a proper fd set. Not setting
     this will make libcurl use the generic default one. */
  CURLcode (*perform_pollset)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ easy_pollset *ps);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * by the curl_disconnect(), as a step in the disconnection. If the handler
   * is called because the connection has been considered dead,
   * dead_connection is set to TRUE. The connection is (again) associated with
   * the transfer here.
   */
  CURLcode (*disconnect)(ਬਣਤਰ Curl_easy *, ਬਣਤਰ connectdata *,
                         bool dead_connection);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing response to
     the client. */
  CURLcode (*write_resp)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                         bool is_eos);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing a single response
     header line to the client. */
  CURLcode (*write_resp_hd)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *hd, size_t hdlen, bool is_eos);

  /* If used, this function checks for a connection managed by this
    protocol and currently not in use, if it should be considered dead. */
  bool (*connection_is_dead)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn);

  /* attach() attaches this transfer to this connection */
  ਖਾਲੀ (*attach)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn);

  /* return CURLE_OK if a redirect to `newurl` should be followed,
     CURLE_TOO_MANY_REDIRECTS otherwise. May alter `data` to change
     the way the follow request is performed. */
  CURLcode (*follow)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *newurl,
                     followtype type);
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_NONE 0             /* nothing extra */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_SSL (1 << 0)       /* uses SSL */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_DUAL (1 << 1)      /* this protocol uses two connections */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_CLOSEACTION (1 << 2) /* need action before socket close */
/* some protocols will have to call the underlying functions without regard to
   what exact state the socket signals. IE even if the socket says "readable",
   the send function might need to be called while uploading, or vice versa. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_DIRLOCK (1 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_NONETWORK (1 << 4) /* protocol does not use the network! */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_NEEDSPWD (1 << 5)  /* needs a password, and if none is set it
                                      gets a default */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_NOURLQUERY (1 << 6)  /* protocol cannot handle
                                        URL query strings (?foo=bar) ! */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_CREDSPERREQUEST (1 << 7) /* requires login credentials per
                                            request instead of per
                                            connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_ALPN (1 << 8) /* set ALPN for this */
/* (1 << 9) was PROTOPT_STREAM, now free */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_URLOPTIONS (1 << 10) /* allow options part in the userinfo
                                        field of the URL */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_PROXY_AS_HTTP (1 << 11) /* allow this non-HTTP scheme over a
                                           HTTP proxy as HTTP proxies may know
                                           this protocol and act as
                                           a gateway */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_WILDCARD (1 << 12)  /* protocol supports wildcard matching */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_USERPWDCTRL (1 << 13) /* Allow "control bytes" (< 32 ASCII) in
                                         username and password */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_NOTCPPROXY (1 << 14)  /* this protocol cannot proxy over TCP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_SSL_REUSE (1 << 15)   /* this protocol may reuse an existing
                                         SSL connection in the same family
                                         without having PROTOPT_SSL. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_CONN_REUSE (1 << 16)  /* this protocol can reuse connections */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_NO_TRANSFER (1 << 17) /* this protocol is not for transfers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTOPT_HTTP_PROXY_TUNNEL (1 << 18) /* Using this protocol with a
                                             * HTTP proxy requires tunneling */

/* Everything about a URI scheme. */
ਬਣਤਰ Curl_scheme {
  ਅਚਲ ਅੱਖਰ *name;       /* URL scheme name in lowercase */
  ਅਚਲ ਬਣਤਰ Curl_protocol *ਲੜੀਵਾਰ; /* implementation, optional */
  curl_prot_t protocol;   /* See CURLPROTO_* - this needs to be the single
                             specific protocol bit */
  curl_prot_t family;     /* single bit for protocol family; the non-TLS name
                             of the protocol this is */
  uint32_t flags;         /* Extra particular characteristics, see PROTOPT_* */
  uint16_t defport;       /* Default port. */
};

/* Get scheme definition for a URI scheme name
 * @param scheme URI scheme name, case-insensitive
 * @return NULL if scheme is not known
 */
ਅਚਲ ਬਣਤਰ Curl_scheme *Curl_get_scheme(ਅਚਲ ਅੱਖਰ *scheme);
ਅਚਲ ਬਣਤਰ Curl_scheme *Curl_getn_scheme(ਅਚਲ ਅੱਖਰ *scheme, size_t len);

/* For direct access to a URI scheme */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_dict;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_file;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ftp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ftps;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_gopher;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_gophers;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_http;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_https;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_imap;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_imaps;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ldap;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ldaps;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_mqtt;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_mqtts;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_pop3;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_pop3s;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_rtsp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_scp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_sftp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_smtp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_smtps;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks4;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks4a;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks5;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks5h;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_telnet;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_tftp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ws;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_wss;

#ਜੇ_ਅੰਤ /* HEADER_CURL_PROTOCOL_H */
