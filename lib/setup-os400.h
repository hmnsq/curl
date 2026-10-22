#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_تياري_OS400_H
#وصف_ڪريو سرو_CURL_تياري_OS400_H
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
#وصف_ڪريو NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#وصف_ڪريو NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#وصف_ڪريو isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#ضمن <stdio.h>      /* Be sure it is loaded. */
#وصف_رد puts
#وصف_ڪريو puts(s) (fputs(s, stdout) == EOF ? EOF : اکر_لکو('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#ضمن <sys/socket.h>
#ضمن <netdb.h>
#ضمن <gskssl.h>
#ضمن <qsoasync.h>
#ضمن <gssapi.h>

#جيڪڏهن_وصف_ٿيل BUILDING_LIBCURL

خارجي صحيح Curl_getaddrinfo_a(ثابت حرف *nodename,
                              ثابت حرف *servname,
                              ثابت بنية addrinfo *hints,
                              بنية addrinfo **res);
#وصف_ڪريو getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
خارجي صحيح Curl_getnameinfo_a(ثابت بنية sockaddr *sa,
                              socklen_t salen,
                              حرف *nodename, socklen_t nodenamelen,
                              حرف *servname, socklen_t servnamelen,
                              صحيح flags);
#وصف_ڪريو getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

خارجي OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_حالت,
                                        gss_بفر_t اندريون_name,
                                        gss_OID اندريون_name_type,
                                        gss_name_t *ٻاهريون_name);
#وصف_ڪريو gss_import_name         Curl_gss_import_name_a

خارجي OM_uint32 Curl_gss_display_حالت_a(OM_uint32 *minor_حالت,
                                           OM_uint32 حالت_قدر,
                                           صحيح حالت_type, gss_OID mech_type,
                                           gss_نياپو_ctx_t *پيغام_context,
                                           gss_بفر_t حالت_string);
#وصف_ڪريو gss_display_حالت      Curl_gss_display_حالت_a

خارجي OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_حالت,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t هدف_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 وقت_req,
                                             gss_channel_bindings_t
                                             داخلا_chan_bindings,
                                             gss_بفر_t داخلا_token,
                                             gss_OID *actual_mech_type,
                                             gss_بفر_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *وقت_rec);
#وصف_ڪريو gss_init_sec_context    Curl_gss_init_sec_context_a

خارجي OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_حالت,
                                               gss_ctx_id_t *context_handle,
                                               gss_بفر_t output_token);
#وصف_ڪريو gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#وصف_ڪريو Ber_قدر                بنية berval

#وصف_ڪريو ldap_url_parse          ldap_url_parse_utf8
#وصف_ڪريو ldap_init               Curl_ldap_init_a
#وصف_ڪريو ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#وصف_ڪريو ldap_ڳولا_s           Curl_ldap_ڳولا_s_a
#وصف_ڪريو ldap_get_قدرون_len     Curl_ldap_get_قدرون_len_a
#وصف_ڪريو ldap_err2string         Curl_ldap_err2string_a
#وصف_ڪريو ldap_get_dn             Curl_ldap_get_dn_a
#وصف_ڪريو ldap_پهريون_attribute    Curl_ldap_پهريون_attribute_a
#وصف_ڪريو ldap_ايندڙ_attribute     Curl_ldap_ايندڙ_attribute_a

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

#وصف_ڪريو connect                 Curl_os400_connect
#وصف_ڪريو bind                    Curl_os400_bind
#وصف_ڪريو sendto                  Curl_os400_sendto
#وصف_ڪريو recvfrom                Curl_os400_recvfrom
#وصف_ڪريو getpeername             Curl_os400_getpeername
#وصف_ڪريو getsockname             Curl_os400_getsockname

#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
#وصف_ڪريو zlib_نسخو             Curl_os400_zlib_نسخو
#وصف_ڪريو inflateInit_            Curl_os400_inflateInit_
#وصف_ڪريو inflateInit2_           Curl_os400_inflateInit2_
#وصف_ڪريو inflate                 Curl_os400_inflate
#وصف_ڪريو inflate_پڇاڙي              Curl_os400_inflate_پڇاڙي
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* BUILDING_LIBCURL */

#جيڪڏهن_پورو /* HEADER_CURL_SETUP_OS400_H */
