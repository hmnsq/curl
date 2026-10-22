#నిర్వచితం_కాకపోతే శీర్షిక_CURL_అమరిక_OS400_H
#నిర్వచించు శీర్షిక_CURL_అమరిక_OS400_H
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

/* OS/400 netdb.h does not define NI_MAXHOST. */
#నిర్వచించు NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#నిర్వచించు NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#నిర్వచించు isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#అన్తర్భావ <stdio.h>      /* Be sure it is loaded. */
#నిర్వచనం_రద్దు puts
#నిర్వచించు puts(s) (fputs(s, stdout) == EOF ? EOF : అక్షరం_రాయి('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#అన్తర్భావ <sys/socket.h>
#అన్తర్భావ <netdb.h>
#అన్తర్భావ <gskssl.h>
#అన్తర్భావ <qsoasync.h>
#అన్తర్భావ <gssapi.h>

#నిర్వచితమైతే BUILDING_LIBCURL

బాహ్య పూర్ణాంకం Curl_getaddrinfo_a(స్థిరరాశి అక్షరం *nodename,
                              స్థిరరాశి అక్షరం *servname,
                              స్థిరరాశి నిర్మాణం addrinfo *hints,
                              నిర్మాణం addrinfo **res);
#నిర్వచించు getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
బాహ్య పూర్ణాంకం Curl_getnameinfo_a(స్థిరరాశి నిర్మాణం sockaddr *sa,
                              socklen_t salen,
                              అక్షరం *nodename, socklen_t nodenamelen,
                              అక్షరం *servname, socklen_t servnamelen,
                              పూర్ణాంకం flags);
#నిర్వచించు getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

బాహ్య OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_స్థితి,
                                        gss_బఫరు_t లోపల_name,
                                        gss_OID లోపల_name_type,
                                        gss_name_t *బహిర్గతం_name);
#నిర్వచించు gss_import_name         Curl_gss_import_name_a

బాహ్య OM_uint32 Curl_gss_display_స్థితి_a(OM_uint32 *minor_స్థితి,
                                           OM_uint32 స్థితి_విలువ,
                                           పూర్ణాంకం స్థితి_type, gss_OID mech_type,
                                           gss_సందేశం_ctx_t *సందేశం_context,
                                           gss_బఫరు_t స్థితి_string);
#నిర్వచించు gss_display_స్థితి      Curl_gss_display_స్థితి_a

బాహ్య OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_స్థితి,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t లక్ష్యం_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 సమయం_req,
                                             gss_channel_bindings_t
                                             ఆదానం_chan_bindings,
                                             gss_బఫరు_t ఆదానం_token,
                                             gss_OID *actual_mech_type,
                                             gss_బఫరు_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *సమయం_rec);
#నిర్వచించు gss_init_sec_context    Curl_gss_init_sec_context_a

బాహ్య OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_స్థితి,
                                               gss_ctx_id_t *context_handle,
                                               gss_బఫరు_t output_token);
#నిర్వచించు gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#నిర్వచించు Ber_విలువ                నిర్మాణం berval

#నిర్వచించు ldap_url_parse          ldap_url_parse_utf8
#నిర్వచించు ldap_init               Curl_ldap_init_a
#నిర్వచించు ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#నిర్వచించు ldap_శోధన_s           Curl_ldap_శోధన_s_a
#నిర్వచించు ldap_get_విలువలు_len     Curl_ldap_get_విలువలు_len_a
#నిర్వచించు ldap_err2string         Curl_ldap_err2string_a
#నిర్వచించు ldap_get_dn             Curl_ldap_get_dn_a
#నిర్వచించు ldap_మొదటి_attribute    Curl_ldap_మొదటి_attribute_a
#నిర్వచించు ldap_తదుపరి_attribute     Curl_ldap_తదుపరి_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

బాహ్య పూర్ణాంకం Curl_os400_connect(పూర్ణాంకం sd, నిర్మాణం sockaddr *destaddr, పూర్ణాంకం addrlen);
బాహ్య పూర్ణాంకం Curl_os400_bind(పూర్ణాంకం sd, నిర్మాణం sockaddr *localaddr, పూర్ణాంకం addrlen);
బాహ్య పూర్ణాంకం Curl_os400_sendto(పూర్ణాంకం sd, అక్షరం *బఫరు, పూర్ణాంకం buflen, పూర్ణాంకం flags,
                             స్థిరరాశి నిర్మాణం sockaddr *dstaddr, పూర్ణాంకం addrlen);
బాహ్య పూర్ణాంకం Curl_os400_recvfrom(పూర్ణాంకం sd, అక్షరం *బఫరు, పూర్ణాంకం buflen, పూర్ణాంకం flags,
                               నిర్మాణం sockaddr *fromaddr, పూర్ణాంకం *addrlen);
బాహ్య పూర్ణాంకం Curl_os400_getpeername(పూర్ణాంకం sd, నిర్మాణం sockaddr *addr, పూర్ణాంకం *addrlen);
బాహ్య పూర్ణాంకం Curl_os400_getsockname(పూర్ణాంకం sd, నిర్మాణం sockaddr *addr, పూర్ణాంకం *addrlen);

#నిర్వచించు connect                 Curl_os400_connect
#నిర్వచించు bind                    Curl_os400_bind
#నిర్వచించు sendto                  Curl_os400_sendto
#నిర్వచించు recvfrom                Curl_os400_recvfrom
#నిర్వచించు getpeername             Curl_os400_getpeername
#నిర్వచించు getsockname             Curl_os400_getsockname

#నిర్వచితమైతే HAVE_LIBZ
#నిర్వచించు zlib_సంస్కరణ             Curl_os400_zlib_సంస్కరణ
#నిర్వచించు inflateInit_            Curl_os400_inflateInit_
#నిర్వచించు inflateInit2_           Curl_os400_inflateInit2_
#నిర్వచించు inflate                 Curl_os400_inflate
#నిర్వచించు inflate_చివర              Curl_os400_inflate_చివర
#అయితే_ముగింపు

#అయితే_ముగింపు /* BUILDING_LIBCURL */

#అయితే_ముగింపు /* HEADER_CURL_SETUP_OS400_H */
