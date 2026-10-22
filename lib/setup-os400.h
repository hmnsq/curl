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

#അന്തര്ഭാവ <stdio.h>      /* Be sure it is loaded. */
#undef puts
#define puts(s) (fputs(s, stdout) == EOF ? EOF : putchar('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#അന്തര്ഭാവ <sys/socket.h>
#അന്തര്ഭാവ <netdb.h>
#അന്തര്ഭാവ <gskssl.h>
#അന്തര്ഭാവ <qsoasync.h>
#അന്തര്ഭാവ <gssapi.h>

#ifdef BUILDING_LIBCURL

ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_getaddrinfo_a(സ്ഥിരം അക്ഷരം *nodename,
                              സ്ഥിരം അക്ഷരം *servname,
                              സ്ഥിരം ഘടന addrinfo *hints,
                              ഘടന addrinfo **res);
#define getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_getnameinfo_a(സ്ഥിരം ഘടന sockaddr *sa,
                              socklen_t salen,
                              അക്ഷരം *nodename, socklen_t nodenamelen,
                              അക്ഷരം *servname, socklen_t servnamelen,
                              പൂർണ്ണസംഖ്യ flags);
#define getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

ബാഹ്യ OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_status,
                                        gss_buffer_t in_name,
                                        gss_OID in_name_type,
                                        gss_name_t *out_name);
#define gss_import_name         Curl_gss_import_name_a

ബാഹ്യ OM_uint32 Curl_gss_display_status_a(OM_uint32 *minor_status,
                                           OM_uint32 status_മൂല്യം,
                                           പൂർണ്ണസംഖ്യ status_type, gss_OID mech_type,
                                           gss_സന്ദേശം_ctx_t *message_context,
                                           gss_buffer_t status_string);
#define gss_display_status      Curl_gss_display_status_a

ബാഹ്യ OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_status,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t target_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 time_req,
                                             gss_channel_bindings_t
                                             നിവേശം_chan_bindings,
                                             gss_buffer_t നിവേശം_token,
                                             gss_OID *actual_mech_type,
                                             gss_buffer_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *time_rec);
#define gss_init_sec_context    Curl_gss_init_sec_context_a

ബാഹ്യ OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_status,
                                               gss_ctx_id_t *context_handle,
                                               gss_buffer_t output_token);
#define gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#define Ber_മൂല്യം                ഘടന berval

#define ldap_url_parse          ldap_url_parse_utf8
#define ldap_init               Curl_ldap_init_a
#define ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#define ldap_തിരയൽ_s           Curl_ldap_തിരയൽ_s_a
#define ldap_get_മൂല്യങ്ങൾ_len     Curl_ldap_get_മൂല്യങ്ങൾ_len_a
#define ldap_err2string         Curl_ldap_err2string_a
#define ldap_get_dn             Curl_ldap_get_dn_a
#define ldap_first_attribute    Curl_ldap_first_attribute_a
#define ldap_next_attribute     Curl_ldap_next_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_os400_connect(പൂർണ്ണസംഖ്യ sd, ഘടന sockaddr *destaddr, പൂർണ്ണസംഖ്യ addrlen);
ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_os400_bind(പൂർണ്ണസംഖ്യ sd, ഘടന sockaddr *localaddr, പൂർണ്ണസംഖ്യ addrlen);
ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_os400_sendto(പൂർണ്ണസംഖ്യ sd, അക്ഷരം *buffer, പൂർണ്ണസംഖ്യ buflen, പൂർണ്ണസംഖ്യ flags,
                             സ്ഥിരം ഘടന sockaddr *dstaddr, പൂർണ്ണസംഖ്യ addrlen);
ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_os400_recvfrom(പൂർണ്ണസംഖ്യ sd, അക്ഷരം *buffer, പൂർണ്ണസംഖ്യ buflen, പൂർണ്ണസംഖ്യ flags,
                               ഘടന sockaddr *fromaddr, പൂർണ്ണസംഖ്യ *addrlen);
ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_os400_getpeername(പൂർണ്ണസംഖ്യ sd, ഘടന sockaddr *addr, പൂർണ്ണസംഖ്യ *addrlen);
ബാഹ്യ പൂർണ്ണസംഖ്യ Curl_os400_getsockname(പൂർണ്ണസംഖ്യ sd, ഘടന sockaddr *addr, പൂർണ്ണസംഖ്യ *addrlen);

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
