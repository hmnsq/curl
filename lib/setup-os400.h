#यदि_अपरिभाषितम् शीर्षकम्_CURL_सज्जता_OS400_H
#परिभाषय शीर्षकम्_CURL_सज्जता_OS400_H
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
#परिभाषय NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#परिभाषय NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#परिभाषय isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#अन्तर्भाव <stdio.h>      /* Be sure it is loaded. */
#अपरिभाषय पङ्क्तिं_लिख
#परिभाषय पङ्क्तिं_लिख(s) (fputs(s, stdout) == EOF ? EOF : अक्षरं_लिख('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#अन्तर्भाव <sys/socket.h>
#अन्तर्भाव <netdb.h>
#अन्तर्भाव <gskssl.h>
#अन्तर्भाव <qsoasync.h>
#अन्तर्भाव <gssapi.h>

#यदि_परिभाषितम् BUILDING_LIBCURL

बाह्य पूर्णाङ्क Curl_getaddrinfo_a(ध्रुव वर्ण *nodename,
                              ध्रुव वर्ण *servname,
                              ध्रुव संरचना addrinfo *hints,
                              संरचना addrinfo **res);
#परिभाषय getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
बाह्य पूर्णाङ्क Curl_getnameinfo_a(ध्रुव संरचना sockaddr *sa,
                              socklen_t salen,
                              वर्ण *nodename, socklen_t nodenamelen,
                              वर्ण *servname, socklen_t servnamelen,
                              पूर्णाङ्क flags);
#परिभाषय getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

बाह्य OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_स्थितिः,
                                        gss_बफर_t अन्तर्_name,
                                        gss_OID अन्तर्_name_type,
                                        gss_name_t *बहिर्गमनम्_name);
#परिभाषय gss_import_name         Curl_gss_import_name_a

बाह्य OM_uint32 Curl_gss_display_स्थितिः_a(OM_uint32 *minor_स्थितिः,
                                           OM_uint32 स्थितिः_मानम्,
                                           पूर्णाङ्क स्थितिः_type, gss_OID mech_type,
                                           gss_सन्देशः_ctx_t *सन्देशः_context,
                                           gss_बफर_t स्थितिः_string);
#परिभाषय gss_display_स्थितिः      Curl_gss_display_स्थितिः_a

बाह्य OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_स्थितिः,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t target_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 time_req,
                                             gss_channel_bindings_t
                                             निवेशः_chan_bindings,
                                             gss_बफर_t निवेशः_token,
                                             gss_OID *actual_mech_type,
                                             gss_बफर_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *time_rec);
#परिभाषय gss_init_sec_context    Curl_gss_init_sec_context_a

बाह्य OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_स्थितिः,
                                               gss_ctx_id_t *context_handle,
                                               gss_बफर_t output_token);
#परिभाषय gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#परिभाषय Ber_मानम्                संरचना berval

#परिभाषय ldap_url_parse          ldap_url_parse_utf8
#परिभाषय ldap_init               Curl_ldap_init_a
#परिभाषय ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#परिभाषय ldap_अन्वेषणम्_s           Curl_ldap_अन्वेषणम्_s_a
#परिभाषय ldap_get_मानानि_len     Curl_ldap_get_मानानि_len_a
#परिभाषय ldap_err2string         Curl_ldap_err2string_a
#परिभाषय ldap_get_dn             Curl_ldap_get_dn_a
#परिभाषय ldap_first_attribute    Curl_ldap_first_attribute_a
#परिभाषय ldap_अग्र_attribute     Curl_ldap_अग्र_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

बाह्य पूर्णाङ्क Curl_os400_connect(पूर्णाङ्क sd, संरचना sockaddr *destaddr, पूर्णाङ्क addrlen);
बाह्य पूर्णाङ्क Curl_os400_bind(पूर्णाङ्क sd, संरचना sockaddr *localaddr, पूर्णाङ्क addrlen);
बाह्य पूर्णाङ्क Curl_os400_sendto(पूर्णाङ्क sd, वर्ण *बफर, पूर्णाङ्क buflen, पूर्णाङ्क flags,
                             ध्रुव संरचना sockaddr *dstaddr, पूर्णाङ्क addrlen);
बाह्य पूर्णाङ्क Curl_os400_recvfrom(पूर्णाङ्क sd, वर्ण *बफर, पूर्णाङ्क buflen, पूर्णाङ्क flags,
                               संरचना sockaddr *fromaddr, पूर्णाङ्क *addrlen);
बाह्य पूर्णाङ्क Curl_os400_getpeername(पूर्णाङ्क sd, संरचना sockaddr *addr, पूर्णाङ्क *addrlen);
बाह्य पूर्णाङ्क Curl_os400_getsockname(पूर्णाङ्क sd, संरचना sockaddr *addr, पूर्णाङ्क *addrlen);

#परिभाषय connect                 Curl_os400_connect
#परिभाषय bind                    Curl_os400_bind
#परिभाषय sendto                  Curl_os400_sendto
#परिभाषय recvfrom                Curl_os400_recvfrom
#परिभाषय getpeername             Curl_os400_getpeername
#परिभाषय getsockname             Curl_os400_getsockname

#यदि_परिभाषितम् HAVE_LIBZ
#परिभाषय zlib_संस्करणम्             Curl_os400_zlib_संस्करणम्
#परिभाषय inflateInit_            Curl_os400_inflateInit_
#परिभाषय inflateInit2_           Curl_os400_inflateInit2_
#परिभाषय inflate                 Curl_os400_inflate
#परिभाषय inflate_अन्तः              Curl_os400_inflate_अन्तः
#यदि_अन्तः

#यदि_अन्तः /* BUILDING_LIBCURL */

#यदि_अन्तः /* HEADER_CURL_SETUP_OS400_H */
