#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಸಿದ್ಧತೆ_OS400_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಸಿದ್ಧತೆ_OS400_H
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
#ವ್ಯಾಖ್ಯಾನಿಸು NI_MAXHOST      1025

/* OS/400 netdb.h does not define NI_MAXSERV. */
#ವ್ಯಾಖ್ಯಾನಿಸು NI_MAXSERV      32

/* OS/400 has no idea of a tty! */
#ವ್ಯಾಖ್ಯಾನಿಸು isatty(fd)      0

/* Workaround bug in IBM QADRT runtime library:
 * function puts() does not output the implicit trailing newline.
 */

#ಅನ್ತರ್ಭಾವ <stdio.h>      /* Be sure it is loaded. */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು puts
#ವ್ಯಾಖ್ಯಾನಿಸು puts(s) (fputs(s, stdout) == EOF ? EOF : ಅಕ್ಷರ_ಬರೆ('\n'))

/* System API wrapper prototypes & definitions to support ASCII parameters. */

#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಅನ್ತರ್ಭಾವ <gskssl.h>
#ಅನ್ತರ್ಭಾವ <qsoasync.h>
#ಅನ್ತರ್ಭಾವ <gssapi.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BUILDING_LIBCURL

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_getaddrinfo_a(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nodename,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *servname,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                              ರಚನೆ addrinfo **res);
#ವ್ಯಾಖ್ಯಾನಿಸು getaddrinfo             Curl_getaddrinfo_a

/* Note socklen_t must be used as this is declared before curl_socklen_t */
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_getnameinfo_a(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa,
                              socklen_t salen,
                              ಅಕ್ಷರ *nodename, socklen_t nodenamelen,
                              ಅಕ್ಷರ *servname, socklen_t servnamelen,
                              ಪೂರ್ಣಾಂಕ flags);
#ವ್ಯಾಖ್ಯಾನಿಸು getnameinfo             Curl_getnameinfo_a

/* GSSAPI wrappers. */

ಬಾಹ್ಯ OM_uint32 Curl_gss_import_name_a(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                        gss_ಬಫರ್_t ಒಳ_name,
                                        gss_OID ಒಳ_name_type,
                                        gss_name_t *ಹೊರಹೋಗು_name);
#ವ್ಯಾಖ್ಯಾನಿಸು gss_import_name         Curl_gss_import_name_a

ಬಾಹ್ಯ OM_uint32 Curl_gss_display_ಸ್ಥಿತಿ_a(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                           OM_uint32 ಸ್ಥಿತಿ_ಮೌಲ್ಯ,
                                           ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_type, gss_OID mech_type,
                                           gss_ಸಂದೇಶ_ctx_t *ಸಂದೇಶ_context,
                                           gss_ಬಫರ್_t ಸ್ಥಿತಿ_string);
#ವ್ಯಾಖ್ಯಾನಿಸು gss_display_ಸ್ಥಿತಿ      Curl_gss_display_ಸ್ಥಿತಿ_a

ಬಾಹ್ಯ OM_uint32 Curl_gss_init_sec_context_a(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                             gss_cred_id_t cred_handle,
                                             gss_ctx_id_t *context_handle,
                                             gss_name_t ಗುರಿ_name,
                                             gss_OID mech_type,
                                             gss_flags_t req_flags,
                                             OM_uint32 ಸಮಯ_req,
                                             gss_channel_bindings_t
                                             ಒಳಹರಿವು_chan_bindings,
                                             gss_ಬಫರ್_t ಒಳಹರಿವು_token,
                                             gss_OID *actual_mech_type,
                                             gss_ಬಫರ್_t output_token,
                                             gss_flags_t *ret_flags,
                                             OM_uint32 *ಸಮಯ_rec);
#ವ್ಯಾಖ್ಯಾನಿಸು gss_init_sec_context    Curl_gss_init_sec_context_a

ಬಾಹ್ಯ OM_uint32 Curl_gss_delete_sec_context_a(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                               gss_ctx_id_t *context_handle,
                                               gss_ಬಫರ್_t output_token);
#ವ್ಯಾಖ್ಯಾನಿಸು gss_delete_sec_context  Curl_gss_delete_sec_context_a

/* LDAP wrappers. */

#ವ್ಯಾಖ್ಯಾನಿಸು Ber_ಮೌಲ್ಯ                ರಚನೆ berval

#ವ್ಯಾಖ್ಯಾನಿಸು ldap_url_parse          ldap_url_parse_utf8
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_init               Curl_ldap_init_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_simple_bind_s      Curl_ldap_simple_bind_s_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_ಹುಡುಕು_s           Curl_ldap_ಹುಡುಕು_s_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_get_ಮೌಲ್ಯಗಳು_len     Curl_ldap_get_ಮೌಲ್ಯಗಳು_len_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_err2string         Curl_ldap_err2string_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_get_dn             Curl_ldap_get_dn_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_ಮೊದಲ_attribute    Curl_ldap_ಮೊದಲ_attribute_a
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_ಮುಂದಿನ_attribute     Curl_ldap_ಮುಂದಿನ_attribute_a

/* Some socket functions must be wrapped to process textual addresses
   like AF_UNIX. */

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_os400_connect(ಪೂರ್ಣಾಂಕ sd, ರಚನೆ sockaddr *destaddr, ಪೂರ್ಣಾಂಕ addrlen);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_os400_bind(ಪೂರ್ಣಾಂಕ sd, ರಚನೆ sockaddr *localaddr, ಪೂರ್ಣಾಂಕ addrlen);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_os400_sendto(ಪೂರ್ಣಾಂಕ sd, ಅಕ್ಷರ *ಬಫರ್, ಪೂರ್ಣಾಂಕ buflen, ಪೂರ್ಣಾಂಕ flags,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *dstaddr, ಪೂರ್ಣಾಂಕ addrlen);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_os400_recvfrom(ಪೂರ್ಣಾಂಕ sd, ಅಕ್ಷರ *ಬಫರ್, ಪೂರ್ಣಾಂಕ buflen, ಪೂರ್ಣಾಂಕ flags,
                               ರಚನೆ sockaddr *fromaddr, ಪೂರ್ಣಾಂಕ *addrlen);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_os400_getpeername(ಪೂರ್ಣಾಂಕ sd, ರಚನೆ sockaddr *addr, ಪೂರ್ಣಾಂಕ *addrlen);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ Curl_os400_getsockname(ಪೂರ್ಣಾಂಕ sd, ರಚನೆ sockaddr *addr, ಪೂರ್ಣಾಂಕ *addrlen);

#ವ್ಯಾಖ್ಯಾನಿಸು connect                 Curl_os400_connect
#ವ್ಯಾಖ್ಯಾನಿಸು bind                    Curl_os400_bind
#ವ್ಯಾಖ್ಯಾನಿಸು sendto                  Curl_os400_sendto
#ವ್ಯಾಖ್ಯಾನಿಸು recvfrom                Curl_os400_recvfrom
#ವ್ಯಾಖ್ಯಾನಿಸು getpeername             Curl_os400_getpeername
#ವ್ಯಾಖ್ಯಾನಿಸು getsockname             Curl_os400_getsockname

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
#ವ್ಯಾಖ್ಯಾನಿಸು zlib_ಆವೃತ್ತಿ             Curl_os400_zlib_ಆವೃತ್ತಿ
#ವ್ಯಾಖ್ಯಾನಿಸು inflateInit_            Curl_os400_inflateInit_
#ವ್ಯಾಖ್ಯಾನಿಸು inflateInit2_           Curl_os400_inflateInit2_
#ವ್ಯಾಖ್ಯಾನಿಸು inflate                 Curl_os400_inflate
#ವ್ಯಾಖ್ಯಾನಿಸು inflate_ಕೊನೆ              Curl_os400_inflate_ಕೊನೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* BUILDING_LIBCURL */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SETUP_OS400_H */
