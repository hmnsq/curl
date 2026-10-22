#నిర్వచితం_కాకపోతే శీర్షిక_CURL_PROTOCOL_H
#నిర్వచించు శీర్షిక_CURL_PROTOCOL_H
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

నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం easy_pollset;

/* Known protocol default port numbers */
#నిర్వచించు పోర్టు_FTP    21
#నిర్వచించు పోర్టు_FTPS   990
#నిర్వచించు పోర్టు_TELNET 23
#నిర్వచించు పోర్టు_HTTP   80
#నిర్వచించు పోర్టు_HTTPS  443
#నిర్వచించు పోర్టు_DICT   2628
#నిర్వచించు పోర్టు_LDAP   389
#నిర్వచించు పోర్టు_LDAPS  636
#నిర్వచించు పోర్టు_TFTP   69
#నిర్వచించు పోర్టు_SSH    22
#నిర్వచించు పోర్టు_IMAP   143
#నిర్వచించు పోర్టు_IMAPS  993
#నిర్వచించు పోర్టు_POP3   110
#నిర్వచించు పోర్టు_POP3_S  995
#నిర్వచించు పోర్టు_SMTP   25
#నిర్వచించు పోర్టు_SMTPS  465 /* sometimes called SSMTP */
#నిర్వచించు పోర్టు_RTSP   554
#నిర్వచించు పోర్టు_GOPHER 70
#నిర్వచించు పోర్టు_SOCKS  1080
#నిర్వచించు పోర్టు_MQTT   1883
#నిర్వచించు పోర్టు_MQTTS  8883

/* CURLPROTO_GOPHERS (29) is the highest publicly used protocol bit number,
 * the rest are internal information. If we use higher bits we only do this on
 * platforms that have a >= 64-bit type and then we use such a type for the
 * protocol fields in the scheme definition.
 */
#నిర్వచించు CURLPROTO_WS     (1L << 30)
#నిర్వచించు CURLPROTO_WSS    ((curl_prot_t)1 << 31)
#నిర్వచించు CURLPROTO_MQTTS  (1LL << 32)
#నిర్వచించు CURLPROTO_SOCKS  (1LL << 33)

#నిర్వచించు CURLPROTO_64ALL ((uint64_t)0xffffffffffffffff)

/* the default protocols accepting a redirect to */
#నిర్వచించు CURLPROTO_REDIR (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | \
                         CURLPROTO_FTPS)

రకనిర్వచనం curl_off_t curl_prot_t;

/* This mask is for all the old protocols that are provided and defined in the
   public header and shall exclude protocols added since which are not exposed
   in the API */
#నిర్వచించు CURLPROTO_ముసుగు   0x3fffffff

/* Convenience defines for checking protocols or their SSL based version. Each
   protocol scheme should only ever have a single CURLPROTO_ in its protocol
   field. */
#నిర్వచించు PROTO_FAMILY_HTTP (CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_WS | \
                           CURLPROTO_WSS)
#నిర్వచించు PROTO_FAMILY_FTP  (CURLPROTO_FTP | CURLPROTO_FTPS)
#నిర్వచించు PROTO_FAMILY_POP3 (CURLPROTO_POP3 | CURLPROTO_POP3S)
#నిర్వచించు PROTO_FAMILY_SMTP (CURLPROTO_SMTP | CURLPROTO_SMTPS)
#నిర్వచించు PROTO_FAMILY_SSH  (CURLPROTO_SCP | CURLPROTO_SFTP)

#ఒకవేళ !defined(CURL_DISABLE_FTP) || defined(USE_SSH) || \
  !defined(CURL_DISABLE_POP3)
/* these protocols support CURLOPT_DIRLISTONLY */
#నిర్వచించు CURL_LIST_ONLY_PROTOCOL 1
#అయితే_ముగింపు

/* When redirecting transfers. */
రకనిర్వచనం గణన {
  FOLLOW_NONE,  /* not used within the function, a placeholder to allow
                   initing to this */
  FOLLOW_FAKE,  /* only records stuff, not actually following */
  FOLLOW_RETRY, /* set if this is a request retry as opposed to a real
                   redirect following */
  FOLLOW_REDIR /* a full true redirect */
} followtype;

రకనిర్వచనం గణన {
  DOMORE_GOBACK = -1,
  DOMORE_INCOMPLETE = 0,
  DOMORE_పూర్తి = 1
} domore;

/*
 * Specific protocol handler, an implementation of one or more URI schemes.
 */
నిర్మాణం Curl_protocol {
  /* Complement to setup_connection_internals(). This is done before the
     transfer "owns" the connection. */
  CURLcode (*అమరిక_connection)(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn);

  /* These two functions MUST be set to be protocol dependent */
  CURLcode (*do_it)(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);
  CURLcode (*పూర్తి)(నిర్మాణం Curl_easy *, CURLcode, bool);

  /* If the curl_do() function is better made in two halves, this
   * curl_do_more() function will be called afterwards, if set. For example
   * for doing the FTP stuff after the PASV/PORT command. The second
   * argument is an output parameter that MUST be set to one of the
   * DOMORE_* values: DOMORE_INCOMPLETE if more do_more work remains,
   * DOMORE_DONE when the second phase is complete, or DOMORE_GOBACK
   * to return to the regular DO/DOING handling.
   */
  CURLcode (*do_మరిన్ని)(నిర్మాణం Curl_easy *, domore *);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * after the connect() and everything is done, as a step in the connection.
   * The 'done' pointer points to a bool that should be set to TRUE if the
   * function completes before return. If it does not complete, the caller
   * should call the ->connecting() function until it is.
   */
  CURLcode (*connect_it)(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);

  /* See above. */
  CURLcode (*connecting)(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);
  CURLcode (*doing)(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);

  /* Called from the multi interface during the PROTOCONNECT phase, and it
     should then return a proper fd set */
  CURLcode (*proto_pollset)(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps);

  /* Called from the multi interface during the DOING phase, and it should
     then return a proper fd set */
  CURLcode (*doing_pollset)(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps);

  /* Called from the multi interface during the DO_MORE phase, and it should
     then return a proper fd set */
  CURLcode (*domore_pollset)(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం easy_pollset *ps);

  /* Called from the multi interface during the DO_DONE, PERFORM and
     WAITPERFORM phases, and it should then return a proper fd set. Not setting
     this will make libcurl use the generic default one. */
  CURLcode (*perform_pollset)(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం easy_pollset *ps);

  /* This function *MAY* be set to a protocol-dependent function that is run
   * by the curl_disconnect(), as a step in the disconnection. If the handler
   * is called because the connection has been considered dead,
   * dead_connection is set to TRUE. The connection is (again) associated with
   * the transfer here.
   */
  CURLcode (*disconnect)(నిర్మాణం Curl_easy *, నిర్మాణం connectdata *,
                         bool dead_connection);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing response to
     the client. */
  CURLcode (*write_resp)(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *buf, size_t blen,
                         bool is_eos);

  /* If used, this function gets called from transfer.c to
     allow the protocol to do extra handling in writing a single response
     header line to the client. */
  CURLcode (*write_resp_hd)(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *hd, size_t hdlen, bool is_eos);

  /* If used, this function checks for a connection managed by this
    protocol and currently not in use, if it should be considered dead. */
  bool (*connection_is_dead)(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn);

  /* attach() attaches this transfer to this connection */
  శూన్య (*attach)(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn);

  /* return CURLE_OK if a redirect to `newurl` should be followed,
     CURLE_TOO_MANY_REDIRECTS otherwise. May alter `data` to change
     the way the follow request is performed. */
  CURLcode (*follow)(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *newurl,
                     followtype type);
};

#నిర్వచించు PROTOPT_NONE 0             /* nothing extra */
#నిర్వచించు PROTOPT_SSL (1 << 0)       /* uses SSL */
#నిర్వచించు PROTOPT_DUAL (1 << 1)      /* this protocol uses two connections */
#నిర్వచించు PROTOPT_CLOSEACTION (1 << 2) /* need action before socket close */
/* some protocols will have to call the underlying functions without regard to
   what exact state the socket signals. IE even if the socket says "readable",
   the send function might need to be called while uploading, or vice versa. */
#నిర్వచించు PROTOPT_DIRLOCK (1 << 3)
#నిర్వచించు PROTOPT_NONETWORK (1 << 4) /* protocol does not use the network! */
#నిర్వచించు PROTOPT_NEEDSPWD (1 << 5)  /* needs a password, and if none is set it
                                      gets a default */
#నిర్వచించు PROTOPT_NOURLQUERY (1 << 6)  /* protocol cannot handle
                                        URL query strings (?foo=bar) ! */
#నిర్వచించు PROTOPT_CREDSPERREQUEST (1 << 7) /* requires login credentials per
                                            request instead of per
                                            connection */
#నిర్వచించు PROTOPT_ALPN (1 << 8) /* set ALPN for this */
/* (1 << 9) was PROTOPT_STREAM, now free */
#నిర్వచించు PROTOPT_URLOPTIONS (1 << 10) /* allow options part in the userinfo
                                        field of the URL */
#నిర్వచించు PROTOPT_PROXY_AS_HTTP (1 << 11) /* allow this non-HTTP scheme over a
                                           HTTP proxy as HTTP proxies may know
                                           this protocol and act as
                                           a gateway */
#నిర్వచించు PROTOPT_WILDCARD (1 << 12)  /* protocol supports wildcard matching */
#నిర్వచించు PROTOPT_USERPWDCTRL (1 << 13) /* Allow "control bytes" (< 32 ASCII) in
                                         username and password */
#నిర్వచించు PROTOPT_NOTCPPROXY (1 << 14)  /* this protocol cannot proxy over TCP */
#నిర్వచించు PROTOPT_SSL_REUSE (1 << 15)   /* this protocol may reuse an existing
                                         SSL connection in the same family
                                         without having PROTOPT_SSL. */
#నిర్వచించు PROTOPT_CONN_REUSE (1 << 16)  /* this protocol can reuse connections */
#నిర్వచించు PROTOPT_NO_TRANSFER (1 << 17) /* this protocol is not for transfers */
#నిర్వచించు PROTOPT_HTTP_PROXY_TUNNEL (1 << 18) /* Using this protocol with a
                                             * HTTP proxy requires tunneling */

/* Everything about a URI scheme. */
నిర్మాణం Curl_scheme {
  స్థిరరాశి అక్షరం *name;       /* URL scheme name in lowercase */
  స్థిరరాశి నిర్మాణం Curl_protocol *నిరంతరం; /* implementation, optional */
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
స్థిరరాశి నిర్మాణం Curl_scheme *Curl_get_scheme(స్థిరరాశి అక్షరం *scheme);
స్థిరరాశి నిర్మాణం Curl_scheme *Curl_getn_scheme(స్థిరరాశి అక్షరం *scheme, size_t len);

/* For direct access to a URI scheme */
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_dict;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_file;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ftp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ftps;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_gopher;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_gophers;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_http;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_https;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_imap;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_imaps;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ldap;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ldaps;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_mqtt;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_mqtts;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_pop3;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_pop3s;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_rtsp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_scp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_sftp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_smtp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_smtps;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks4;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks4a;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks5;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks5h;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_telnet;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_tftp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ws;
బాహ్య స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_wss;

#అయితే_ముగింపు /* HEADER_CURL_PROTOCOL_H */
