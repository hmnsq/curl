#اگر_غیر_تعریف_شدہ سرخی_CURL_تیاری_OS400_H
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

#شامل <stdio.h>      /* Be sure it is loaded. */
#تعریف_منسوخ puts
#تعریف_کرو puts(s) (fputs(s, stdout) == EOF ? EOF : حرف_لکھو('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#شامل <sys/socket.h>
#شامل <netdb.h>
#شامل <gskssl.h>
#شامل <qsoasync.h>
#شامل <gssapi.h>

#اگر_تعریف_شدہ BUILDING_LIBCURL

خارجی صحیح Curl_getaddrinfo_a(ثابت حرف *nodename,
                              ثابت حرف *servname,
                              ثابت ساخت addrinfo *hints,
                              ساخت addrinfo **res);
#تعریف_کرو getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
خارجی صحیح Curl_getnameinfo_a(ثابت ساخت sockaddr *sa,
                              socklen_t salen,
                              حرف *nodename, socklen_t nodenamelen,
                              حرف *servname, socklen_t servnamelen,
                              صحیح flags);
#تعریف_کرو getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

خارجی OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_حالت,
                                        gss_بفر_t اندر_name,
                                        gss_OID اندر_name_type,
                                        gss_name_t *برآمد_name);
#تعریف_کرو gss_import_name         Curl_gss_import_name_a

خارجی OM_uint32 Curl_gss_display_حالت_a(OM_uint32 *minor_حالت,
                                           OM_uint32 حالت_قدر,
                                           صحیح حالت_type, gss_OID mech_type,
                                           gss_پیغام_ctx_t *پیغام_context,
                                           gss_بفر_t حالت_string);
#تعریف_کرو gss_display_حالت      Curl_gss_display_حالت_a

خارجی OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_حالت,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t ہدف_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 وقت_req,
                                             gss_channel_bindings_t
                                             داخلہ_chan_bindings,
                                             gss_بفر_t داخلہ_token,
                                             gss_OID *actual_mech_type,
                                             gss_بفر_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *وقت_rec);
#تعریف_کرو gss_init_sec_context    Curl_gss_init_sec_context_a

خارجی OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_حالت,
                                               gss_ctx_id_t *context_handle,
                                               gss_بفر_t output_token);
#تعریف_کرو gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#تعریف_کرو Ber_قدر                ساخت berval

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

خارجی صحیح Curl_os400_connect(صحیح sd, ساخت sockaddr *destaddr, صحیح addrlen);
خارجی صحیح Curl_os400_bind(صحیح sd, ساخت sockaddr *localaddr, صحیح addrlen);
خارجی صحیح Curl_os400_sendto(صحیح sd, حرف *بفر, صحیح buflen, صحیح flags,
                             ثابت ساخت sockaddr *dstaddr, صحیح addrlen);
خارجی صحیح Curl_os400_recvfrom(صحیح sd, حرف *بفر, صحیح buflen, صحیح flags,
                               ساخت sockaddr *fromaddr, صحیح *addrlen);
خارجی صحیح Curl_os400_getpeername(صحیح sd, ساخت sockaddr *addr, صحیح *addrlen);
خارجی صحیح Curl_os400_getsockname(صحیح sd, ساخت sockaddr *addr, صحیح *addrlen);

#تعریف_کرو connect                 Curl_os400_connect
#تعریف_کرو bind                    Curl_os400_bind
#تعریف_کرو sendto                  Curl_os400_sendto
#تعریف_کرو recvfrom                Curl_os400_recvfrom
#تعریف_کرو getpeername             Curl_os400_getpeername
#تعریف_کرو getsockname             Curl_os400_getsockname

#اگر_تعریف_شدہ HAVE_LIBZ
#تعریف_کرو zlib_نسخہ             Curl_os400_zlib_نسخہ
#تعریف_کرو inflateInit_            Curl_os400_inflateInit_
#تعریف_کرو inflateInit2_           Curl_os400_inflateInit2_
#تعریف_کرو inflate                 Curl_os400_inflate
#تعریف_کرو inflate_انتہا              Curl_os400_inflate_انتہا
#اگر_ختم

#اگر_ختم /* BUILDING_LIBCURL */

#اگر_ختم /* HEADER_CURL_SETUP_OS400_H */
