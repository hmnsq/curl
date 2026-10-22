#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_OS400_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_OS400_H
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>      /* Be sure it is loaded. */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ puts
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ puts(s) (fputs(s, stdout) == EOF ? EOF : ਅੱਖਰ_ਲਿਖੋ('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gskssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <qsoasync.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gssapi.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BUILDING_LIBCURL

ਬਾਹਰੀ ਪੂਰਨ Curl_getaddrinfo_a(ਅਚਲ ਅੱਖਰ *nodename,
                              ਅਚਲ ਅੱਖਰ *servname,
                              ਅਚਲ ਬਣਤਰ addrinfo *hints,
                              ਬਣਤਰ addrinfo **res);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
ਬਾਹਰੀ ਪੂਰਨ Curl_getnameinfo_a(ਅਚਲ ਬਣਤਰ sockaddr *sa,
                              socklen_t salen,
                              ਅੱਖਰ *nodename, socklen_t nodenamelen,
                              ਅੱਖਰ *servname, socklen_t servnamelen,
                              ਪੂਰਨ flags);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

ਬਾਹਰੀ OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_ਸਥਿਤੀ,
                                        gss_ਬਫਰ_t ਵਿੱਚ_name,
                                        gss_OID ਵਿੱਚ_name_type,
                                        gss_name_t *ਬਾਹਰਲਾ_name);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gss_import_name         Curl_gss_import_name_a

ਬਾਹਰੀ OM_uint32 Curl_gss_display_ਸਥਿਤੀ_a(OM_uint32 *minor_ਸਥਿਤੀ,
                                           OM_uint32 ਸਥਿਤੀ_ਮੁੱਲ,
                                           ਪੂਰਨ ਸਥਿਤੀ_type, gss_OID mech_type,
                                           gss_ਸੁਨੇਹਾ_ctx_t *ਸੁਨੇਹਾ_context,
                                           gss_ਬਫਰ_t ਸਥਿਤੀ_string);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gss_display_ਸਥਿਤੀ      Curl_gss_display_ਸਥਿਤੀ_a

ਬਾਹਰੀ OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_ਸਥਿਤੀ,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t ਨਿਸ਼ਾਨਾ_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 ਸਮਾਂ_req,
                                             gss_channel_bindings_t
                                             ਨਿਵੇਸ਼_chan_bindings,
                                             gss_ਬਫਰ_t ਨਿਵੇਸ਼_token,
                                             gss_OID *actual_mech_type,
                                             gss_ਬਫਰ_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *ਸਮਾਂ_rec);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gss_init_sec_context    Curl_gss_init_sec_context_a

ਬਾਹਰੀ OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_ਸਥਿਤੀ,
                                               gss_ctx_id_t *context_handle,
                                               gss_ਬਫਰ_t output_token);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ber_ਮੁੱਲ                ਬਣਤਰ berval

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_url_parse          ldap_url_parse_utf8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_init               Curl_ldap_init_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_ਖੋਜ_s           Curl_ldap_ਖੋਜ_s_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_get_ਮੁੱਲਾਂ_len     Curl_ldap_get_ਮੁੱਲਾਂ_len_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_err2string         Curl_ldap_err2string_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_get_dn             Curl_ldap_get_dn_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_ਪਹਿਲਾ_attribute    Curl_ldap_ਪਹਿਲਾ_attribute_a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_ਅਗਲਾ_attribute     Curl_ldap_ਅਗਲਾ_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

ਬਾਹਰੀ ਪੂਰਨ Curl_os400_connect(ਪੂਰਨ sd, ਬਣਤਰ sockaddr *destaddr, ਪੂਰਨ addrlen);
ਬਾਹਰੀ ਪੂਰਨ Curl_os400_bind(ਪੂਰਨ sd, ਬਣਤਰ sockaddr *localaddr, ਪੂਰਨ addrlen);
ਬਾਹਰੀ ਪੂਰਨ Curl_os400_sendto(ਪੂਰਨ sd, ਅੱਖਰ *ਬਫਰ, ਪੂਰਨ buflen, ਪੂਰਨ flags,
                             ਅਚਲ ਬਣਤਰ sockaddr *dstaddr, ਪੂਰਨ addrlen);
ਬਾਹਰੀ ਪੂਰਨ Curl_os400_recvfrom(ਪੂਰਨ sd, ਅੱਖਰ *ਬਫਰ, ਪੂਰਨ buflen, ਪੂਰਨ flags,
                               ਬਣਤਰ sockaddr *fromaddr, ਪੂਰਨ *addrlen);
ਬਾਹਰੀ ਪੂਰਨ Curl_os400_getpeername(ਪੂਰਨ sd, ਬਣਤਰ sockaddr *addr, ਪੂਰਨ *addrlen);
ਬਾਹਰੀ ਪੂਰਨ Curl_os400_getsockname(ਪੂਰਨ sd, ਬਣਤਰ sockaddr *addr, ਪੂਰਨ *addrlen);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ connect                 Curl_os400_connect
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bind                    Curl_os400_bind
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sendto                  Curl_os400_sendto
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ recvfrom                Curl_os400_recvfrom
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getpeername             Curl_os400_getpeername
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getsockname             Curl_os400_getsockname

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ zlib_ਸੰਸਕਰਣ             Curl_os400_zlib_ਸੰਸਕਰਣ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflateInit_            Curl_os400_inflateInit_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflateInit2_           Curl_os400_inflateInit2_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflate                 Curl_os400_inflate
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflate_ਸਿਰਾ              Curl_os400_inflate_ਸਿਰਾ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* BUILDING_LIBCURL */

#ਜੇ_ਅੰਤ /* HEADER_CURL_SETUP_OS400_H */
