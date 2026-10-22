#ifndef HEADER_CURL_SETUP_OS400_H
#define HEADER_CURL_SETUP_OS400_H
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
#define NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#define NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#define isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#अन्तर्भाव <stdio.h>      /* Be sure it is loaded. */
#undef puts
#define puts(s) (fputs(s, stdout) == EOF ? EOF : putchar('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#अन्तर्भाव <sys/socket.h>
#अन्तर्भाव <netdb.h>
#अन्तर्भाव <gskssl.h>
#अन्तर्भाव <qsoasync.h>
#अन्तर्भाव <gssapi.h>

#ifdef BUILDING_LIBCURL

बाह्य पूर्णाङ्क Curl_getaddrinfo_a(ध्रुव वर्ण *nodename,
                              ध्रुव वर्ण *servname,
                              ध्रुव संरचना addrinfo *hints,
                              संरचना addrinfo **res);
#define getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
बाह्य पूर्णाङ्क Curl_getnameinfo_a(ध्रुव संरचना sockaddr *sa,
                              socklen_t salen,
                              वर्ण *nodename, socklen_t nodenamelen,
                              वर्ण *servname, socklen_t servnamelen,
                              पूर्णाङ्क flags);
#define getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

बाह्य OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_status,
                                        gss_buffer_t in_name,
                                        gss_OID in_name_type,
                                        gss_name_t *out_name);
#define gss_import_name         Curl_gss_import_name_a

बाह्य OM_uint32 Curl_gss_display_status_a(OM_uint32 *minor_status,
                                           OM_uint32 status_मूल्य,
                                           पूर्णाङ्क status_type, gss_OID mech_type,
                                           gss_संदेश_ctx_t *message_context,
                                           gss_buffer_t status_string);
#define gss_display_status      Curl_gss_display_status_a

बाह्य OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_status,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t target_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 time_req,
                                             gss_channel_bindings_t
                                             आदान_chan_bindings,
                                             gss_buffer_t आदान_token,
                                             gss_OID *actual_mech_type,
                                             gss_buffer_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *time_rec);
#define gss_init_sec_context    Curl_gss_init_sec_context_a

बाह्य OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_status,
                                               gss_ctx_id_t *context_handle,
                                               gss_buffer_t output_token);
#define gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#define Ber_मूल्य                संरचना berval

#define ldap_url_parse          ldap_url_parse_utf8
#define ldap_init               Curl_ldap_init_a
#define ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#define ldap_शोध_s           Curl_ldap_शोध_s_a
#define ldap_get_मूल्ये_len     Curl_ldap_get_मूल्ये_len_a
#define ldap_err2string         Curl_ldap_err2string_a
#define ldap_get_dn             Curl_ldap_get_dn_a
#define ldap_first_attribute    Curl_ldap_first_attribute_a
#define ldap_next_attribute     Curl_ldap_next_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

बाह्य पूर्णाङ्क Curl_os400_connect(पूर्णाङ्क sd, संरचना sockaddr *destaddr, पूर्णाङ्क addrlen);
बाह्य पूर्णाङ्क Curl_os400_bind(पूर्णाङ्क sd, संरचना sockaddr *localaddr, पूर्णाङ्क addrlen);
बाह्य पूर्णाङ्क Curl_os400_sendto(पूर्णाङ्क sd, वर्ण *buffer, पूर्णाङ्क buflen, पूर्णाङ्क flags,
                             ध्रुव संरचना sockaddr *dstaddr, पूर्णाङ्क addrlen);
बाह्य पूर्णाङ्क Curl_os400_recvfrom(पूर्णाङ्क sd, वर्ण *buffer, पूर्णाङ्क buflen, पूर्णाङ्क flags,
                               संरचना sockaddr *fromaddr, पूर्णाङ्क *addrlen);
बाह्य पूर्णाङ्क Curl_os400_getpeername(पूर्णाङ्क sd, संरचना sockaddr *addr, पूर्णाङ्क *addrlen);
बाह्य पूर्णाङ्क Curl_os400_getsockname(पूर्णाङ्क sd, संरचना sockaddr *addr, पूर्णाङ्क *addrlen);

#define connect                 Curl_os400_connect
#define bind                    Curl_os400_bind
#define sendto                  Curl_os400_sendto
#define recvfrom                Curl_os400_recvfrom
#define getpeername             Curl_os400_getpeername
#define getsockname             Curl_os400_getsockname

#ifdef HAVE_LIBZ
#define zlibVersion             Curl_os400_zlibVersion
#define inflateInit_            Curl_os400_inflateInit_
#define inflateInit2_           Curl_os400_inflateInit2_
#define inflate                 Curl_os400_inflate
#define inflateEnd              Curl_os400_inflateEnd
#endif

#endif /* BUILDING_LIBCURL */

#endif /* HEADER_CURL_SETUP_OS400_H */
