#جے_بنا_تعریف سرخی_CURL_تیاری_OS400_H
#تعریف_کرو سرخی_CURL_تیاری_OS400_H
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
#تعریف_کرو NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#تعریف_کرو NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#تعریف_کرو isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#ضمن <stdio.h>      /* Be sure it is loaded. */
#تعریف_رد puts
#تعریف_کرو puts(s) (fputs(s, stdout) == EOF ? EOF : حرف_لکھو('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#ضمن <sys/socket.h>
#ضمن <netdb.h>
#ضمن <gskssl.h>
#ضمن <qsoasync.h>
#ضمن <gssapi.h>

#جے_تعریف_شدہ BUILDING_LIBCURL

خارجي صحيح Curl_getaddrinfo_a(ثابت حرف *nodename,
                              ثابت حرف *servname,
                              ثابت بنية addrinfo *hints,
                              بنية addrinfo **res);
#تعریف_کرو getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
خارجي صحيح Curl_getnameinfo_a(ثابت بنية sockaddr *sa,
                              socklen_t salen,
                              حرف *nodename, socklen_t nodenamelen,
                              حرف *servname, socklen_t servnamelen,
                              صحيح flags);
#تعریف_کرو getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

خارجي OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_حالت,
                                        gss_بفر_t وچ_name,
                                        gss_OID وچ_name_type,
                                        gss_name_t *باہرلا_name);
#تعریف_کرو gss_import_name         Curl_gss_import_name_a

خارجي OM_uint32 Curl_gss_display_حالت_a(OM_uint32 *minor_حالت,
                                           OM_uint32 حالت_قدر,
                                           صحيح حالت_type, gss_OID mech_type,
                                           gss_پیغام_ctx_t *سنیہا_context,
                                           gss_بفر_t حالت_string);
#تعریف_کرو gss_display_حالت      Curl_gss_display_حالت_a

خارجي OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_حالت,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t نشانہ_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 ویلا_req,
                                             gss_channel_bindings_t
                                             داخلہ_chan_bindings,
                                             gss_بفر_t داخلہ_token,
                                             gss_OID *actual_mech_type,
                                             gss_بفر_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *ویلا_rec);
#تعریف_کرو gss_init_sec_context    Curl_gss_init_sec_context_a

خارجي OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_حالت,
                                               gss_ctx_id_t *context_handle,
                                               gss_بفر_t output_token);
#تعریف_کرو gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#تعریف_کرو Ber_قدر                بنية berval

#تعریف_کرو ldap_url_parse          ldap_url_parse_utf8
#تعریف_کرو ldap_init               Curl_ldap_init_a
#تعریف_کرو ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#تعریف_کرو ldap_تلاش_s           Curl_ldap_تلاش_s_a
#تعریف_کرو ldap_get_اقدار_len     Curl_ldap_get_اقدار_len_a
#تعریف_کرو ldap_err2string         Curl_ldap_err2string_a
#تعریف_کرو ldap_get_dn             Curl_ldap_get_dn_a
#تعریف_کرو ldap_پہلا_attribute    Curl_ldap_پہلا_attribute_a
#تعریف_کرو ldap_اگلا_attribute     Curl_ldap_اگلا_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

خارجي صحيح Curl_os400_connect(صحيح sd, بنية sockaddr *destaddr, صحيح addrlen);
خارجي صحيح Curl_os400_bind(صحيح sd, بنية sockaddr *localaddr, صحيح addrlen);
خارجي صحيح Curl_os400_sendto(صحيح sd, حرف *بفر, صحيح buflen, صحيح flags,
                             ثابت بنية sockaddr *dstaddr, صحيح addrlen);
خارجي صحيح Curl_os400_recvfrom(صحيح sd, حرف *بفر, صحيح buflen, صحيح flags,
                               بنية sockaddr *fromaddr, صحيح *addrlen);
خارجي صحيح Curl_os400_getpeername(صحيح sd, بنية sockaddr *addr, صحيح *addrlen);
خارجي صحيح Curl_os400_getsockname(صحيح sd, بنية sockaddr *addr, صحيح *addrlen);

#تعریف_کرو connect                 Curl_os400_connect
#تعریف_کرو bind                    Curl_os400_bind
#تعریف_کرو sendto                  Curl_os400_sendto
#تعریف_کرو recvfrom                Curl_os400_recvfrom
#تعریف_کرو getpeername             Curl_os400_getpeername
#تعریف_کرو getsockname             Curl_os400_getsockname

#جے_تعریف_شدہ HAVE_LIBZ
#تعریف_کرو zlib_نسخہ             Curl_os400_zlib_نسخہ
#تعریف_کرو inflateInit_            Curl_os400_inflateInit_
#تعریف_کرو inflateInit2_           Curl_os400_inflateInit2_
#تعریف_کرو inflate                 Curl_os400_inflate
#تعریف_کرو inflate_سرا              Curl_os400_inflate_سرا
#جے_انت

#جے_انت /* BUILDING_LIBCURL */

#جے_انت /* HEADER_CURL_SETUP_OS400_H */
