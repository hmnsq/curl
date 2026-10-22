#اگر_تعریف_نشده سرآیند_CURL_راهاندازی_OS400_H
#تعریف_کن سرآیند_CURL_راهاندازی_OS400_H
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
#تعریف_کن NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#تعریف_کن NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#تعریف_کن isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#ضمن <stdio.h>      /* Be sure it is loaded. */
#لغو_تعریف puts
#تعریف_کن puts(s) (fputs(s, stdout) == EOF ? EOF : حرف_چاپ_کن('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#ضمن <sys/socket.h>
#ضمن <netdb.h>
#ضمن <gskssl.h>
#ضمن <qsoasync.h>
#ضمن <gssapi.h>

#اگر_تعریف_شده BUILDING_LIBCURL

خارجي صحيح Curl_getaddrinfo_a(ثابت حرف *nodename,
                              ثابت حرف *servname,
                              ثابت بنية addrinfo *hints,
                              بنية addrinfo **res);
#تعریف_کن getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
خارجي صحيح Curl_getnameinfo_a(ثابت بنية sockaddr *sa,
                              socklen_t salen,
                              حرف *nodename, socklen_t nodenamelen,
                              حرف *servname, socklen_t servnamelen,
                              صحيح flags);
#تعریف_کن getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

خارجي OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_وضعیت,
                                        gss_میانگیر_t درون_name,
                                        gss_OID درون_name_type,
                                        gss_name_t *بیرون_name);
#تعریف_کن gss_import_name         Curl_gss_import_name_a

خارجي OM_uint32 Curl_gss_display_وضعیت_a(OM_uint32 *minor_وضعیت,
                                           OM_uint32 وضعیت_مقدار,
                                           صحيح وضعیت_type, gss_OID mech_type,
                                           gss_پیام_ctx_t *پیام_context,
                                           gss_میانگیر_t وضعیت_string);
#تعریف_کن gss_display_وضعیت      Curl_gss_display_وضعیت_a

خارجي OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_وضعیت,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t هدف_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 زمان_req,
                                             gss_channel_bindings_t
                                             درون_داد_chan_bindings,
                                             gss_میانگیر_t درون_داد_token,
                                             gss_OID *actual_mech_type,
                                             gss_میانگیر_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *زمان_rec);
#تعریف_کن gss_init_sec_context    Curl_gss_init_sec_context_a

خارجي OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_وضعیت,
                                               gss_ctx_id_t *context_handle,
                                               gss_میانگیر_t output_token);
#تعریف_کن gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#تعریف_کن Ber_مقدار                بنية berval

#تعریف_کن ldap_url_parse          ldap_url_parse_utf8
#تعریف_کن ldap_init               Curl_ldap_init_a
#تعریف_کن ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#تعریف_کن ldap_جستجو_s           Curl_ldap_جستجو_s_a
#تعریف_کن ldap_get_مقادیر_len     Curl_ldap_get_مقادیر_len_a
#تعریف_کن ldap_err2string         Curl_ldap_err2string_a
#تعریف_کن ldap_get_dn             Curl_ldap_get_dn_a
#تعریف_کن ldap_اولی_attribute    Curl_ldap_اولی_attribute_a
#تعریف_کن ldap_بعدی_attribute     Curl_ldap_بعدی_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

خارجي صحيح Curl_os400_connect(صحيح sd, بنية sockaddr *destaddr, صحيح addrlen);
خارجي صحيح Curl_os400_bind(صحيح sd, بنية sockaddr *localaddr, صحيح addrlen);
خارجي صحيح Curl_os400_sendto(صحيح sd, حرف *میانگیر, صحيح buflen, صحيح flags,
                             ثابت بنية sockaddr *dstaddr, صحيح addrlen);
خارجي صحيح Curl_os400_recvfrom(صحيح sd, حرف *میانگیر, صحيح buflen, صحيح flags,
                               بنية sockaddr *fromaddr, صحيح *addrlen);
خارجي صحيح Curl_os400_getpeername(صحيح sd, بنية sockaddr *addr, صحيح *addrlen);
خارجي صحيح Curl_os400_getsockname(صحيح sd, بنية sockaddr *addr, صحيح *addrlen);

#تعریف_کن connect                 Curl_os400_connect
#تعریف_کن bind                    Curl_os400_bind
#تعریف_کن sendto                  Curl_os400_sendto
#تعریف_کن recvfrom                Curl_os400_recvfrom
#تعریف_کن getpeername             Curl_os400_getpeername
#تعریف_کن getsockname             Curl_os400_getsockname

#اگر_تعریف_شده HAVE_LIBZ
#تعریف_کن zlib_نسخه             Curl_os400_zlib_نسخه
#تعریف_کن inflateInit_            Curl_os400_inflateInit_
#تعریف_کن inflateInit2_           Curl_os400_inflateInit2_
#تعریف_کن inflate                 Curl_os400_inflate
#تعریف_کن inflate_انتها              Curl_os400_inflate_انتها
#پایان_اگر

#پایان_اگر /* BUILDING_LIBCURL */

#پایان_اگر /* HEADER_CURL_SETUP_OS400_H */
