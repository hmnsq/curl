#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_GSSAPI_H
#وصف_ڪريو سرو_CURL_GSSAPI_H
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
#ضمن "curl_setup.h"

#ضمن "urldata.h"

#جيڪڏهن_وصف_ٿيل HAVE_GSSAPI
خارجي gss_OID_desc Curl_spnego_mech_oid;
خارجي gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *معلومات,
                                    OM_uint32 *minor_حالت,
                                    gss_ctx_id_t *context,
                                    gss_name_t هدف_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t داخلا_chan_bindings,
                                    gss_بفر_t داخلا_token,
                                    gss_بفر_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_بفر_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_حالت,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_حالت,
                                gss_name_t desired_name,
                                OM_uint32 وقت_req,
                                gss_OID_سيٽ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_سيٽ *actual_mechs,
                                OM_uint32 *وقت_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_حالت,
                                  gss_OID_سيٽ *mech_سيٽ);

#جيڪڏهن_وصف_ٿيل HAVE_GSS_سيٽ_NEG_MECHS
OM_uint32 Curl_gss_سيٽ_neg_mechs(OM_uint32 *minor_حالت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_سيٽ mech_سيٽ);
#جيڪڏهن_پورو

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_حالت,
                                gss_cred_id_t *cred_handle);

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
/* Helper to log a GSS-API error status */
فراغ Curl_gss_log_error(بنية Curl_easy *معلومات, ثابت حرف *اڳياڙي,
                        OM_uint32 major, OM_uint32 minor);
#وإلا
#وصف_ڪريو Curl_gss_log_error(معلومات, اڳياڙي, major, minor) \
  افعل {                                                 \
    (فراغ)(معلومات);                                      \
    (فراغ)(اڳياڙي);                                    \
    (فراغ)(major);                                     \
    (فراغ)(minor);                                     \
  } طالما(0)
#جيڪڏهن_پورو

/* Define our privacy and integrity protection values */
#وصف_ڪريو GSSAUTH_P_NONE      1
#وصف_ڪريو GSSAUTH_P_INTEGRITY 2
#وصف_ڪريو GSSAUTH_P_PRIVACY   4

#جيڪڏهن_پورو /* HAVE_GSSAPI */
#جيڪڏهن_پورو /* HEADER_CURL_GSSAPI_H */
