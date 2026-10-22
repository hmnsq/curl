#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_PROTOCOL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_PROTOCOL_H
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

ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ easy_pollset;

/* Known protocol default port numbers */
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_FTP    21
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_FTPS   990
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_TELNET 23
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_HTTP   80
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_HTTPS  443
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_DICT   2628
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_LDAP   389
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_LDAPS  636
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_TFTP   69
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_SSH    22
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_IMAP   143
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_IMAPS  993
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_POP3   110
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_POP3_S  995
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_SMTP   25
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_SMTPS  465 /* sometimes called SSMTP */
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_RTSP   554
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_GOPHER 70
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_SOCKS  1080
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_MQTT   1883
#ವ್ಯಾಖ್ಯಾನಿಸು ಪೋರ್ಟ್_MQTTS  8883

/* CURLPROTO_GOPHERS (29) is the highest publicly used protocol bit number,
 * the rest are internal information. If we use higher bits we only do this on
 * platforms that have a >= 64-bit type and then we use such a type for the
 * protocol fields in the scheme definition.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_WS     (1L << 30)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_WSS    ((curl_prot_t)1 << 31)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_MQTTS  (1LL << 32)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_SOCKS  (1LL << 33)

#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_64ALL ((uint64_t)0xffffffffffffffff)

/* the default protocols accepting a redirect to */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_REDIR (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | \
                         CURLPROTO_FTPS)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ curl_off_t curl_prot_t;

/* This mask is for all the old protocols that are provided and defined in the
   public header and shall exclude protocols added since which are not exposed
   in the API */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLPROTO_ಮುಖವಾಡ   0x3fffffff

/* Convenience defines for checking protocols or their SSL based version. Each
   protocol scheme should only ever have a single CURLPROTO_ in its protocol
   field. */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTO_FAMILY_HTTP (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_WS | \
                           CURLPROTO_WSS)
#ವ್ಯಾಖ್ಯಾನಿಸು PROTO_FAMILY_FTP  (CURLPROTO_FTP | CURLPROTO_FTPS)
#ವ್ಯಾಖ್ಯಾನಿಸು PROTO_FAMILY_POP3 (CURLPROTO_POP3 | CURLPROTO_POP3S)
#ವ್ಯಾಖ್ಯಾನಿಸು PROTO_FAMILY_SMTP (CURLPROTO_SMTP | CURLPROTO_SMTPS)
#ವ್ಯಾಖ್ಯಾನಿಸು PROTO_FAMILY_SSH  (CURLPROTO_SCP | CURLPROTO_SFTP)

#ಆದರೆ !defined(CURL_DISABLE_FTP) || defined(USE_SSH) || \
  !defined(CURL_DISABLE_POP3)
/* these protocols support CURLOPT_DIRLISTONLY */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LIST_ONLY_PROTOCOL 1
#ಆದರೆ_ಅಂತ್ಯ

/* When redirecting transfers. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  FOLLOW_NONE,  /* not used within the function, a placeholder to allow
                   initing to this */
  FOLLOW_FAKE,  /* only records stuff, not actually following */
  FOLLOW_RETRY, /* set if this is a request retry as opposed to a real
                   redirect following */
  FOLLOW_REDIR /* a full true redirect */
} followtype;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  DOMORE_GOBACK = -1,
  DOMORE_INCOMPLETE = 0,
  DOMORE_ಮುಗಿದ = 1
} domore;

/*
 * Specific protocol handler, an implementation of one or more URI schemes.
 */
ರಚನೆ Curl_protocol {
  /* Complement to setup_connection_internals(). This is done before the
     transfer "owns" the connection. */
  CURLcode (*ಸಿದ್ಧತೆ_connection)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn);

  /* These two functions MUST be set to be protocol dependent */
  CURLcode (*do_it)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);
  CURLcode (*ಮುಗಿದ)(ರಚನೆ Curl_easy *, CURLcode, bool);

  /* If the curl_do() function is better made in two halves, this
   * curl_do_more() function will be called afterwards, if set. For example
   * for doing the FTP stuff after the PASV/PORT command. The second
   * argument is an output parameter that MUST be set to one of the
   * DOMORE_* values: DOMORE_INCOMPLETE if more do_more work remains,
   * DOMORE_DONE when the second phase is complete, or DOMORE_GOBACK
   * to return to the regular DO/DOING handling.
   */
  CURLcode (*do_ಇನ್ನಷ್ಟು)(ರಚನೆ Curl_easy *, domore *);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * after the connect() and everything is done, as a step in the connection.
   * The 'done' pointer points to a bool that should be set to TRUE if the
   * function completes before return. If it does not complete, the caller
   * should call the ->connecting() function until it is.
   */
  CURLcode (*connect_it)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);

  /* See above. */
  CURLcode (*connecting)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);
  CURLcode (*doing)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);

  /* Called from the multi interface during the PROTOCONNECT phase, and it
     should then return a proper fd set */
  CURLcode (*proto_pollset)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps);

  /* Called from the multi interface during the DOING phase, and it should
     then return a proper fd set */
  CURLcode (*doing_pollset)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps);

  /* Called from the multi interface during the DO_MORE phase, and it should
     then return a proper fd set */
  CURLcode (*domore_pollset)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ easy_pollset *ps);

  /* Called from the multi interface during the DO_DONE, PERFORM and
     WAITPERFORM phases, and it should then return a proper fd set. Not setting
     this will make libcurl use the generic default one. */
  CURLcode (*perform_pollset)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ easy_pollset *ps);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * by the curl_disconnect(), as a step in the disconnection. If the handler
   * is called because the connection has been considered dead,
   * dead_connection is set to TRUE. The connection is (again) associated with
   * the transfer here.
   */
  CURLcode (*disconnect)(ರಚನೆ Curl_easy *, ರಚನೆ connectdata *,
                         bool dead_connection);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing response to
     the client. */
  CURLcode (*write_resp)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen,
                         bool is_eos);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing a single response
     header line to the client. */
  CURLcode (*write_resp_hd)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hd, size_t hdlen, bool is_eos);

  /* If used, this function checks for a connection managed by this
    protocol and currently not in use, if it should be considered dead. */
  bool (*connection_is_dead)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn);

  /* attach() attaches this transfer to this connection */
  ಶೂನ್ಯ (*attach)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn);

  /* return CURLE_OK if a redirect to `newurl` should be followed,
     CURLE_TOO_MANY_REDIRECTS otherwise. May alter `data` to change
     the way the follow request is performed. */
  CURLcode (*follow)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newurl,
                     followtype type);
};

#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_NONE 0             /* nothing extra */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_SSL (1 << 0)       /* uses SSL */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_DUAL (1 << 1)      /* this protocol uses two connections */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_CLOSEACTION (1 << 2) /* need action before socket close */
/* some protocols will have to call the underlying functions without regard to
   what exact state the socket signals. IE even if the socket says "readable",
   the send function might need to be called while uploading, or vice versa. */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_DIRLOCK (1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_NONETWORK (1 << 4) /* protocol does not use the network! */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_NEEDSPWD (1 << 5)  /* needs a password, and if none is set it
                                      gets a default */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_NOURLQUERY (1 << 6)  /* protocol cannot handle
                                        URL query strings (?foo=bar) ! */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_CREDSPERREQUEST (1 << 7) /* requires login credentials per
                                            request instead of per
                                            connection */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_ALPN (1 << 8) /* set ALPN for this */
/* (1 << 9) was PROTOPT_STREAM, now free */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_URLOPTIONS (1 << 10) /* allow options part in the userinfo
                                        field of the URL */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_PROXY_AS_HTTP (1 << 11) /* allow this non-HTTP scheme over a
                                           HTTP proxy as HTTP proxies may know
                                           this protocol and act as
                                           a gateway */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_WILDCARD (1 << 12)  /* protocol supports wildcard matching */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_USERPWDCTRL (1 << 13) /* Allow "control bytes" (< 32 ASCII) in
                                         username and password */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_NOTCPPROXY (1 << 14)  /* this protocol cannot proxy over TCP */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_SSL_REUSE (1 << 15)   /* this protocol may reuse an existing
                                         SSL connection in the same family
                                         without having PROTOPT_SSL. */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_CONN_REUSE (1 << 16)  /* this protocol can reuse connections */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_NO_TRANSFER (1 << 17) /* this protocol is not for transfers */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTOPT_HTTP_PROXY_TUNNEL (1 << 18) /* Using this protocol with a
                                             * HTTP proxy requires tunneling */

/* Everything about a URI scheme. */
ರಚನೆ Curl_scheme {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;       /* URL scheme name in lowercase */
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol *ಸತತ; /* implementation, optional */
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
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *Curl_get_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme);
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *Curl_getn_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme, size_t len);

/* For direct access to a URI scheme */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_dict;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_file;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ftp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ftps;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_gopher;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_gophers;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_http;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_https;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_imap;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_imaps;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ldap;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ldaps;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_mqtt;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_mqtts;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_pop3;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_pop3s;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_rtsp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_scp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_sftp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_smtp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_smtps;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks4;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks4a;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks5;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks5h;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_telnet;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_tftp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ws;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_wss;

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_PROTOCOL_H */
