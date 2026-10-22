#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_PROTOCOL_H
#وصف_ڪريو سرو_CURL_PROTOCOL_H
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

بنية Curl_easy;
بنية connectdata;
بنية easy_pollset;

/* Known protocol default port numbers */
#وصف_ڪريو پورٽ_FTP    21
#وصف_ڪريو پورٽ_FTPS   990
#وصف_ڪريو پورٽ_TELNET 23
#وصف_ڪريو پورٽ_HTTP   80
#وصف_ڪريو پورٽ_HTTPS  443
#وصف_ڪريو پورٽ_DICT   2628
#وصف_ڪريو پورٽ_LDAP   389
#وصف_ڪريو پورٽ_LDAPS  636
#وصف_ڪريو پورٽ_TFTP   69
#وصف_ڪريو پورٽ_SSH    22
#وصف_ڪريو پورٽ_IMAP   143
#وصف_ڪريو پورٽ_IMAPS  993
#وصف_ڪريو پورٽ_POP3   110
#وصف_ڪريو پورٽ_POP3_S  995
#وصف_ڪريو پورٽ_SMTP   25
#وصف_ڪريو پورٽ_SMTPS  465 /* sometimes called SSMTP */
#وصف_ڪريو پورٽ_RTSP   554
#وصف_ڪريو پورٽ_GOPHER 70
#وصف_ڪريو پورٽ_SOCKS  1080
#وصف_ڪريو پورٽ_MQTT   1883
#وصف_ڪريو پورٽ_MQTTS  8883

/* CURLPROTO_GOPHERS (29) is the highest publicly used protocol bit number,
 * the rest are internal information. If we use higher bits we only do this on
 * platforms that have a >= 64-bit type and then we use such a type for the
 * protocol fields in the scheme definition.
 */
#وصف_ڪريو CURLPROTO_WS     (1L << 30)
#وصف_ڪريو CURLPROTO_WSS    ((curl_prot_t)1 << 31)
#وصف_ڪريو CURLPROTO_MQTTS  (1LL << 32)
#وصف_ڪريو CURLPROTO_SOCKS  (1LL << 33)

#وصف_ڪريو CURLPROTO_64ALL ((uint64_t)0xffffffffffffffff)

/* the default protocols accepting a redirect to */
#وصف_ڪريو CURLPROTO_REDIR (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | \
                         CURLPROTO_FTPS)

تعريفنوع curl_off_t curl_prot_t;

/* This mask is for all the old protocols that are provided and defined in the
   public header and shall exclude protocols added since which are not exposed
   in the API */
#وصف_ڪريو CURLPROTO_نقاب   0x3fffffff

/* Convenience defines for checking protocols or their SSL based version. Each
   protocol scheme should only ever have a single CURLPROTO_ in its protocol
   field. */
#وصف_ڪريو PROTO_FAMILY_HTTP (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_WS | \
                           CURLPROTO_WSS)
#وصف_ڪريو PROTO_FAMILY_FTP  (CURLPROTO_FTP | CURLPROTO_FTPS)
#وصف_ڪريو PROTO_FAMILY_POP3 (CURLPROTO_POP3 | CURLPROTO_POP3S)
#وصف_ڪريو PROTO_FAMILY_SMTP (CURLPROTO_SMTP | CURLPROTO_SMTPS)
#وصف_ڪريو PROTO_FAMILY_SSH  (CURLPROTO_SCP | CURLPROTO_SFTP)

#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH) || \
  !defined(CURL_DISABLE_POP3)
/* these protocols support CURLOPT_DIRLISTONLY */
#وصف_ڪريو CURL_LIST_ONLY_PROTOCOL 1
#جيڪڏهن_پورو

/* When redirecting transfers. */
تعريفنوع تعداد {
  FOLLOW_NONE,  /* not used within the function, a placeholder to allow
                   initing to this */
  FOLLOW_FAKE,  /* only records stuff, not actually following */
  FOLLOW_RETRY, /* set if this is a request retry as opposed to a real
                   redirect following */
  FOLLOW_REDIR /* a full true redirect */
} followtype;

تعريفنوع تعداد {
  DOMORE_GOBACK = -1,
  DOMORE_INCOMPLETE = 0,
  DOMORE_پورو = 1
} domore;

/*
 * Specific protocol handler, an implementation of one or more URI schemes.
 */
بنية Curl_protocol {
  /* Complement to setup_connection_internals(). This is done before the
     transfer "owns" the connection. */
  CURLcode (*تياري_connection)(بنية Curl_easy *معلومات,
                               بنية connectdata *conn);

  /* These two functions MUST be set to be protocol dependent */
  CURLcode (*do_it)(بنية Curl_easy *معلومات, bool *پورو);
  CURLcode (*پورو)(بنية Curl_easy *, CURLcode, bool);

  /* If the curl_do() function is better made in two halves, this
   * curl_do_more() function will be called afterwards, if set. For example
   * for doing the FTP stuff after the PASV/PORT command. The second
   * argument is an output parameter that MUST be set to one of the
   * DOMORE_* values: DOMORE_INCOMPLETE if more do_more work remains,
   * DOMORE_DONE when the second phase is complete, or DOMORE_GOBACK
   * to return to the regular DO/DOING handling.
   */
  CURLcode (*do_وڌيڪ)(بنية Curl_easy *, domore *);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * after the connect() and everything is done, as a step in the connection.
   * The 'done' pointer points to a bool that should be set to TRUE if the
   * function completes before return. If it does not complete, the caller
   * should call the ->connecting() function until it is.
   */
  CURLcode (*connect_it)(بنية Curl_easy *معلومات, bool *پورو);

  /* See above. */
  CURLcode (*connecting)(بنية Curl_easy *معلومات, bool *پورو);
  CURLcode (*doing)(بنية Curl_easy *معلومات, bool *پورو);

  /* Called from the multi interface during the PROTOCONNECT phase, and it
     should then return a proper fd set */
  CURLcode (*proto_pollset)(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps);

  /* Called from the multi interface during the DOING phase, and it should
     then return a proper fd set */
  CURLcode (*doing_pollset)(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps);

  /* Called from the multi interface during the DO_MORE phase, and it should
     then return a proper fd set */
  CURLcode (*domore_pollset)(بنية Curl_easy *معلومات,
                            بنية easy_pollset *ps);

  /* Called from the multi interface during the DO_DONE, PERFORM and
     WAITPERFORM phases, and it should then return a proper fd set. Not setting
     this will make libcurl use the generic default one. */
  CURLcode (*perform_pollset)(بنية Curl_easy *معلومات,
                              بنية easy_pollset *ps);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * by the curl_disconnect(), as a step in the disconnection. If the handler
   * is called because the connection has been considered dead,
   * dead_connection is set to TRUE. The connection is (again) associated with
   * the transfer here.
   */
  CURLcode (*disconnect)(بنية Curl_easy *, بنية connectdata *,
                         bool dead_connection);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing response to
     the client. */
  CURLcode (*write_resp)(بنية Curl_easy *معلومات, ثابت حرف *buf, size_t blen,
                         bool is_eos);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing a single response
     header line to the client. */
  CURLcode (*write_resp_hd)(بنية Curl_easy *معلومات,
                            ثابت حرف *hd, size_t hdlen, bool is_eos);

  /* If used, this function checks for a connection managed by this
    protocol and currently not in use, if it should be considered dead. */
  bool (*connection_is_dead)(بنية Curl_easy *معلومات,
                             بنية connectdata *conn);

  /* attach() attaches this transfer to this connection */
  فراغ (*attach)(بنية Curl_easy *معلومات, بنية connectdata *conn);

  /* return CURLE_OK if a redirect to `newurl` should be followed,
     CURLE_TOO_MANY_REDIRECTS otherwise. May alter `data` to change
     the way the follow request is performed. */
  CURLcode (*follow)(بنية Curl_easy *معلومات, ثابت حرف *newurl,
                     followtype type);
};

#وصف_ڪريو PROTOPT_NONE 0             /* nothing extra */
#وصف_ڪريو PROTOPT_SSL (1 << 0)       /* uses SSL */
#وصف_ڪريو PROTOPT_DUAL (1 << 1)      /* this protocol uses two connections */
#وصف_ڪريو PROTOPT_CLOSEACTION (1 << 2) /* need action before socket close */
/* some protocols will have to call the underlying functions without regard to
   what exact state the socket signals. IE even if the socket says "readable",
   the send function might need to be called while uploading, or vice versa. */
#وصف_ڪريو PROTOPT_DIRLOCK (1 << 3)
#وصف_ڪريو PROTOPT_NONETWORK (1 << 4) /* protocol does not use the network! */
#وصف_ڪريو PROTOPT_NEEDSPWD (1 << 5)  /* needs a password, and if none is set it
                                      gets a default */
#وصف_ڪريو PROTOPT_NOURLQUERY (1 << 6)  /* protocol cannot handle
                                        URL query strings (?foo=bar) ! */
#وصف_ڪريو PROTOPT_CREDSPERREQUEST (1 << 7) /* requires login credentials per
                                            request instead of per
                                            connection */
#وصف_ڪريو PROTOPT_ALPN (1 << 8) /* set ALPN for this */
/* (1 << 9) was PROTOPT_STREAM, now free */
#وصف_ڪريو PROTOPT_URLOPTIONS (1 << 10) /* allow options part in the userinfo
                                        field of the URL */
#وصف_ڪريو PROTOPT_PROXY_AS_HTTP (1 << 11) /* allow this non-HTTP scheme over a
                                           HTTP proxy as HTTP proxies may know
                                           this protocol and act as
                                           a gateway */
#وصف_ڪريو PROTOPT_WILDCARD (1 << 12)  /* protocol supports wildcard matching */
#وصف_ڪريو PROTOPT_USERPWDCTRL (1 << 13) /* Allow "control bytes" (< 32 ASCII) in
                                         username and password */
#وصف_ڪريو PROTOPT_NOTCPPROXY (1 << 14)  /* this protocol cannot proxy over TCP */
#وصف_ڪريو PROTOPT_SSL_REUSE (1 << 15)   /* this protocol may reuse an existing
                                         SSL connection in the same family
                                         without having PROTOPT_SSL. */
#وصف_ڪريو PROTOPT_CONN_REUSE (1 << 16)  /* this protocol can reuse connections */
#وصف_ڪريو PROTOPT_NO_TRANSFER (1 << 17) /* this protocol is not for transfers */
#وصف_ڪريو PROTOPT_HTTP_PROXY_TUNNEL (1 << 18) /* Using this protocol with a
                                             * HTTP proxy requires tunneling */

/* Everything about a URI scheme. */
بنية Curl_scheme {
  ثابت حرف *name;       /* URL scheme name in lowercase */
  ثابت بنية Curl_protocol *لاڳيتو; /* implementation, optional */
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
ثابت بنية Curl_scheme *Curl_get_scheme(ثابت حرف *scheme);
ثابت بنية Curl_scheme *Curl_getn_scheme(ثابت حرف *scheme, size_t len);

/* For direct access to a URI scheme */
خارجي ثابت بنية Curl_scheme Curl_scheme_dict;
خارجي ثابت بنية Curl_scheme Curl_scheme_file;
خارجي ثابت بنية Curl_scheme Curl_scheme_ftp;
خارجي ثابت بنية Curl_scheme Curl_scheme_ftps;
خارجي ثابت بنية Curl_scheme Curl_scheme_gopher;
خارجي ثابت بنية Curl_scheme Curl_scheme_gophers;
خارجي ثابت بنية Curl_scheme Curl_scheme_http;
خارجي ثابت بنية Curl_scheme Curl_scheme_https;
خارجي ثابت بنية Curl_scheme Curl_scheme_imap;
خارجي ثابت بنية Curl_scheme Curl_scheme_imaps;
خارجي ثابت بنية Curl_scheme Curl_scheme_ldap;
خارجي ثابت بنية Curl_scheme Curl_scheme_ldaps;
خارجي ثابت بنية Curl_scheme Curl_scheme_mqtt;
خارجي ثابت بنية Curl_scheme Curl_scheme_mqtts;
خارجي ثابت بنية Curl_scheme Curl_scheme_pop3;
خارجي ثابت بنية Curl_scheme Curl_scheme_pop3s;
خارجي ثابت بنية Curl_scheme Curl_scheme_rtsp;
خارجي ثابت بنية Curl_scheme Curl_scheme_scp;
خارجي ثابت بنية Curl_scheme Curl_scheme_sftp;
خارجي ثابت بنية Curl_scheme Curl_scheme_smtp;
خارجي ثابت بنية Curl_scheme Curl_scheme_smtps;
خارجي ثابت بنية Curl_scheme Curl_scheme_socks;
خارجي ثابت بنية Curl_scheme Curl_scheme_socks4;
خارجي ثابت بنية Curl_scheme Curl_scheme_socks4a;
خارجي ثابت بنية Curl_scheme Curl_scheme_socks5;
خارجي ثابت بنية Curl_scheme Curl_scheme_socks5h;
خارجي ثابت بنية Curl_scheme Curl_scheme_telnet;
خارجي ثابت بنية Curl_scheme Curl_scheme_tftp;
خارجي ثابت بنية Curl_scheme Curl_scheme_ws;
خارجي ثابت بنية Curl_scheme Curl_scheme_wss;

#جيڪڏهن_پورو /* HEADER_CURL_PROTOCOL_H */
