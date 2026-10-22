#अगर_अघोषित HEADER_CURL_SETUP_OS400_H
#घोषणा HEADER_CURL_SETUP_OS400_H
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
#घोषणा NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#घोषणा NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#घोषणा isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#समावेश <stdio.h>      /* Be sure it is loaded. */
#अघोषित माला_दो
#घोषणा माला_दो(s) (ख_माला_दो(s, मानक_निकास) == खातापूर्ण ? खातापूर्ण : अक्षर_दो('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#समावेश <sys/socket.h>
#समावेश <netdb.h>
#समावेश <gskssl.h>
#समावेश <qsoasync.h>
#समावेश <gssapi.h>

#अगर_घोषित BUILDING_LIBCURL

बाह्य पूर्णांक Curl_getaddrinfo_a(स्थिर अक्षर *nodename,
                              स्थिर अक्षर *servname,
                              स्थिर काष्ठा addrinfo *hints,
                              काष्ठा addrinfo **res);
#घोषणा getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
बाह्य पूर्णांक Curl_getnameinfo_a(स्थिर काष्ठा sockaddr *sa,
                              socklen_t salen,
                              अक्षर *nodename, socklen_t nodenamelen,
                              अक्षर *servname, socklen_t servnamelen,
                              पूर्णांक flags);
#घोषणा getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

बाह्य OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_status,
                                        gss_बफर_t in_name,
                                        gss_OID in_name_type,
                                        gss_name_t *out_name);
#घोषणा gss_import_name         Curl_gss_import_name_a

बाह्य OM_uint32 Curl_gss_display_status_a(OM_uint32 *minor_status,
                                           OM_uint32 status_मूल्य,
                                           पूर्णांक status_type, gss_OID mech_type,
                                           gss_संदेश_ctx_t *message_context,
                                           gss_बफर_t status_string);
#घोषणा gss_display_status      Curl_gss_display_status_a

बाह्य OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_status,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t target_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 time_req,
                                             gss_channel_bindings_t
                                             निवेश_chan_bindings,
                                             gss_बफर_t निवेश_token,
                                             gss_OID *actual_mech_type,
                                             gss_बफर_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *time_rec);
#घोषणा gss_init_sec_context    Curl_gss_init_sec_context_a

बाह्य OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_status,
                                               gss_ctx_id_t *context_handle,
                                               gss_बफर_t output_token);
#घोषणा gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#घोषणा Ber_मूल्य                काष्ठा berval

#घोषणा ldap_url_parse          ldap_url_parse_utf8
#घोषणा ldap_init               Curl_ldap_init_a
#घोषणा ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#घोषणा ldap_खोज_s           Curl_ldap_खोज_s_a
#घोषणा ldap_get_मूल्यावली_len     Curl_ldap_get_मूल्यावली_len_a
#घोषणा ldap_err2string         Curl_ldap_err2string_a
#घोषणा ldap_get_dn             Curl_ldap_get_dn_a
#घोषणा ldap_first_attribute    Curl_ldap_first_attribute_a
#घोषणा ldap_अग्र_attribute     Curl_ldap_अग्र_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

बाह्य पूर्णांक Curl_os400_connect(पूर्णांक sd, काष्ठा sockaddr *destaddr, पूर्णांक addrlen);
बाह्य पूर्णांक Curl_os400_bind(पूर्णांक sd, काष्ठा sockaddr *localaddr, पूर्णांक addrlen);
बाह्य पूर्णांक Curl_os400_sendto(पूर्णांक sd, अक्षर *बफर, पूर्णांक buflen, पूर्णांक flags,
                             स्थिर काष्ठा sockaddr *dstaddr, पूर्णांक addrlen);
बाह्य पूर्णांक Curl_os400_recvfrom(पूर्णांक sd, अक्षर *बफर, पूर्णांक buflen, पूर्णांक flags,
                               काष्ठा sockaddr *fromaddr, पूर्णांक *addrlen);
बाह्य पूर्णांक Curl_os400_getpeername(पूर्णांक sd, काष्ठा sockaddr *addr, पूर्णांक *addrlen);
बाह्य पूर्णांक Curl_os400_getsockname(पूर्णांक sd, काष्ठा sockaddr *addr, पूर्णांक *addrlen);

#घोषणा connect                 Curl_os400_connect
#घोषणा bind                    Curl_os400_bind
#घोषणा sendto                  Curl_os400_sendto
#घोषणा recvfrom                Curl_os400_recvfrom
#घोषणा getpeername             Curl_os400_getpeername
#घोषणा getsockname             Curl_os400_getsockname

#अगर_घोषित HAVE_LIBZ
#घोषणा zlibVersion             Curl_os400_zlibVersion
#घोषणा inflateInit_            Curl_os400_inflateInit_
#घोषणा inflateInit2_           Curl_os400_inflateInit2_
#घोषणा inflate                 Curl_os400_inflate
#घोषणा inflateEnd              Curl_os400_inflateEnd
#पूर्ण_अगर

#पूर्ण_अगर /* BUILDING_LIBCURL */

#पूर्ण_अगर /* HEADER_CURL_SETUP_OS400_H */
