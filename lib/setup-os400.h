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

#شامل <stdio.h>      /* Be sure it is loaded. */
#undef puts
#define puts(s) (fputs(s, stdout) == EOF ? EOF : putchar('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#شامل <sys/socket.h>
#شامل <netdb.h>
#شامل <gskssl.h>
#شامل <qsoasync.h>
#شامل <gssapi.h>

#ifdef BUILDING_LIBCURL

بیرونی صحیح Curl_getaddrinfo_a(ثابت نویسه *nodename,
                              ثابت نویسه *servname,
                              ثابت ساختار addrinfo *hints,
                              ساختار addrinfo **res);
#define getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
بیرونی صحیح Curl_getnameinfo_a(ثابت ساختار sockaddr *sa,
                              socklen_t salen,
                              نویسه *nodename, socklen_t nodenamelen,
                              نویسه *servname, socklen_t servnamelen,
                              صحیح flags);
#define getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

بیرونی OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_status,
                                        gss_buffer_t in_name,
                                        gss_OID in_name_type,
                                        gss_name_t *out_name);
#define gss_import_name         Curl_gss_import_name_a

بیرونی OM_uint32 Curl_gss_display_status_a(OM_uint32 *minor_status,
                                           OM_uint32 status_مقدار,
                                           صحیح status_type, gss_OID mech_type,
                                           gss_پیام_ctx_t *message_context,
                                           gss_buffer_t status_string);
#define gss_display_status      Curl_gss_display_status_a

بیرونی OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_status,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t target_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 time_req,
                                             gss_channel_bindings_t
                                             درون_داد_chan_bindings,
                                             gss_buffer_t درون_داد_token,
                                             gss_OID *actual_mech_type,
                                             gss_buffer_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *time_rec);
#define gss_init_sec_context    Curl_gss_init_sec_context_a

بیرونی OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_status,
                                               gss_ctx_id_t *context_handle,
                                               gss_buffer_t output_token);
#define gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#define Ber_مقدار                ساختار berval

#define ldap_url_parse          ldap_url_parse_utf8
#define ldap_init               Curl_ldap_init_a
#define ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#define ldap_جستجو_s           Curl_ldap_جستجو_s_a
#define ldap_get_مقادیر_len     Curl_ldap_get_مقادیر_len_a
#define ldap_err2string         Curl_ldap_err2string_a
#define ldap_get_dn             Curl_ldap_get_dn_a
#define ldap_first_attribute    Curl_ldap_first_attribute_a
#define ldap_next_attribute     Curl_ldap_next_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

بیرونی صحیح Curl_os400_connect(صحیح sd, ساختار sockaddr *destaddr, صحیح addrlen);
بیرونی صحیح Curl_os400_bind(صحیح sd, ساختار sockaddr *localaddr, صحیح addrlen);
بیرونی صحیح Curl_os400_sendto(صحیح sd, نویسه *buffer, صحیح buflen, صحیح flags,
                             ثابت ساختار sockaddr *dstaddr, صحیح addrlen);
بیرونی صحیح Curl_os400_recvfrom(صحیح sd, نویسه *buffer, صحیح buflen, صحیح flags,
                               ساختار sockaddr *fromaddr, صحیح *addrlen);
بیرونی صحیح Curl_os400_getpeername(صحیح sd, ساختار sockaddr *addr, صحیح *addrlen);
بیرونی صحیح Curl_os400_getsockname(صحیح sd, ساختار sockaddr *addr, صحیح *addrlen);

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
