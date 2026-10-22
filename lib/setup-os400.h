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

#অন্তর্ভাব <stdio.h>      /* Be sure it is loaded. */
#undef puts
#define puts(s) (fputs(s, stdout) == EOF ? EOF : putchar('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#অন্তর্ভাব <sys/socket.h>
#অন্তর্ভাব <netdb.h>
#অন্তর্ভাব <gskssl.h>
#অন্তর্ভাব <qsoasync.h>
#অন্তর্ভাব <gssapi.h>

#ifdef BUILDING_LIBCURL

বাহ্যিক পূর্ণসংখ্যা Curl_getaddrinfo_a(ধ্রুবক অক্ষর *nodename,
                              ধ্রুবক অক্ষর *servname,
                              ধ্রুবক কাঠামো addrinfo *hints,
                              কাঠামো addrinfo **res);
#define getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
বাহ্যিক পূর্ণসংখ্যা Curl_getnameinfo_a(ধ্রুবক কাঠামো sockaddr *sa,
                              socklen_t salen,
                              অক্ষর *nodename, socklen_t nodenamelen,
                              অক্ষর *servname, socklen_t servnamelen,
                              পূর্ণসংখ্যা flags);
#define getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

বাহ্যিক OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_status,
                                        gss_বাফার_t in_name,
                                        gss_OID in_name_type,
                                        gss_name_t *out_name);
#define gss_import_name         Curl_gss_import_name_a

বাহ্যিক OM_uint32 Curl_gss_display_status_a(OM_uint32 *minor_status,
                                           OM_uint32 status_মান,
                                           পূর্ণসংখ্যা status_type, gss_OID mech_type,
                                           gss_বার্তা_ctx_t *message_context,
                                           gss_বাফার_t status_string);
#define gss_display_status      Curl_gss_display_status_a

বাহ্যিক OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_status,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t target_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 time_req,
                                             gss_channel_bindings_t
                                             প্রবেশ_chan_bindings,
                                             gss_বাফার_t প্রবেশ_token,
                                             gss_OID *actual_mech_type,
                                             gss_বাফার_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *time_rec);
#define gss_init_sec_context    Curl_gss_init_sec_context_a

বাহ্যিক OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_status,
                                               gss_ctx_id_t *context_handle,
                                               gss_বাফার_t output_token);
#define gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#define Ber_মান                কাঠামো berval

#define ldap_url_parse          ldap_url_parse_utf8
#define ldap_init               Curl_ldap_init_a
#define ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#define ldap_অনুসন্ধান_s           Curl_ldap_অনুসন্ধান_s_a
#define ldap_get_মানসমূহ_len     Curl_ldap_get_মানসমূহ_len_a
#define ldap_err2string         Curl_ldap_err2string_a
#define ldap_get_dn             Curl_ldap_get_dn_a
#define ldap_first_attribute    Curl_ldap_first_attribute_a
#define ldap_পরবর্তী_attribute     Curl_ldap_পরবর্তী_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

বাহ্যিক পূর্ণসংখ্যা Curl_os400_connect(পূর্ণসংখ্যা sd, কাঠামো sockaddr *destaddr, পূর্ণসংখ্যা addrlen);
বাহ্যিক পূর্ণসংখ্যা Curl_os400_bind(পূর্ণসংখ্যা sd, কাঠামো sockaddr *localaddr, পূর্ণসংখ্যা addrlen);
বাহ্যিক পূর্ণসংখ্যা Curl_os400_sendto(পূর্ণসংখ্যা sd, অক্ষর *বাফার, পূর্ণসংখ্যা buflen, পূর্ণসংখ্যা flags,
                             ধ্রুবক কাঠামো sockaddr *dstaddr, পূর্ণসংখ্যা addrlen);
বাহ্যিক পূর্ণসংখ্যা Curl_os400_recvfrom(পূর্ণসংখ্যা sd, অক্ষর *বাফার, পূর্ণসংখ্যা buflen, পূর্ণসংখ্যা flags,
                               কাঠামো sockaddr *fromaddr, পূর্ণসংখ্যা *addrlen);
বাহ্যিক পূর্ণসংখ্যা Curl_os400_getpeername(পূর্ণসংখ্যা sd, কাঠামো sockaddr *addr, পূর্ণসংখ্যা *addrlen);
বাহ্যিক পূর্ণসংখ্যা Curl_os400_getsockname(পূর্ণসংখ্যা sd, কাঠামো sockaddr *addr, পূর্ণসংখ্যা *addrlen);

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
