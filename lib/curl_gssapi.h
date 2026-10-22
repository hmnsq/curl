#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_GSSAPI_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_GSSAPI_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
ਬਾਹਰੀ gss_OID_desc Curl_spnego_mech_oid;
ਬਾਹਰੀ gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    OM_uint32 *minor_ਸਥਿਤੀ,
                                    gss_ctx_id_t *context,
                                    gss_name_t ਨਿਸ਼ਾਨਾ_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ਨਿਵੇਸ਼_chan_bindings,
                                    gss_ਬਫਰ_t ਨਿਵੇਸ਼_token,
                                    gss_ਬਫਰ_t output_token,
                                    ਅਚਲ bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_ਬਫਰ_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_ਸਥਿਤੀ,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_ਸਥਿਤੀ,
                                gss_name_t desired_name,
                                OM_uint32 ਸਮਾਂ_req,
                                gss_OID_ਸੈੱਟ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_ਸੈੱਟ *actual_mechs,
                                OM_uint32 *ਸਮਾਂ_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_ਸਥਿਤੀ,
                                  gss_OID_ਸੈੱਟ *mech_ਸੈੱਟ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSS_ਸੈੱਟ_NEG_MECHS
OM_uint32 Curl_gss_ਸੈੱਟ_neg_mechs(OM_uint32 *minor_ਸਥਿਤੀ,
                                 gss_cred_id_t cred_handle,
                                 ਅਚਲ gss_OID_ਸੈੱਟ mech_ਸੈੱਟ);
#ਜੇ_ਅੰਤ

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_ਸਥਿਤੀ,
                                gss_cred_id_t *cred_handle);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
/* Helper to log a GSS-API error status */
ਖਾਲੀ Curl_gss_log_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ,
                        OM_uint32 major, OM_uint32 minor);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_gss_log_error(ਅੰਕੜੇ, ਅਗੇਤਰ, major, minor) \
  ਕਰੋ {                                                 \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                                      \
    (ਖਾਲੀ)(ਅਗੇਤਰ);                                    \
    (ਖਾਲੀ)(major);                                     \
    (ਖਾਲੀ)(minor);                                     \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/* Define our privacy and integrity protection values */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GSSAUTH_P_NONE      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GSSAUTH_P_INTEGRITY 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GSSAUTH_P_PRIVACY   4

#ਜੇ_ਅੰਤ /* HAVE_GSSAPI */
#ਜੇ_ਅੰਤ /* HEADER_CURL_GSSAPI_H */
