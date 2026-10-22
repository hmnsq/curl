#اگر_غیر_تعریف_شدہ سرخی_CURL_GSSAPI_H
#تعریف_کرو سرخی_CURL_GSSAPI_H
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
#شامل "curl_setup.h"

#شامل "urldata.h"

#اگر_تعریف_شدہ HAVE_GSSAPI
خارجی gss_OID_desc Curl_spnego_mech_oid;
خارجی gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(ساخت Curl_easy *ڈیٹا,
                                    OM_uint32 *minor_حالت,
                                    gss_ctx_id_t *context,
                                    gss_name_t ہدف_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t داخلہ_chan_bindings,
                                    gss_بفر_t داخلہ_token,
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
                                gss_OID_سیٹ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_سیٹ *actual_mechs,
                                OM_uint32 *وقت_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_حالت,
                                  gss_OID_سیٹ *mech_سیٹ);

#اگر_تعریف_شدہ HAVE_GSS_سیٹ_NEG_MECHS
OM_uint32 Curl_gss_سیٹ_neg_mechs(OM_uint32 *minor_حالت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_سیٹ mech_سیٹ);
#اگر_ختم

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_حالت,
                                gss_cred_id_t *cred_handle);

#اگر_تعریف_شدہ CURLVERBOSE
/* Helper to log a GSS-API error status */
خالی Curl_gss_log_error(ساخت Curl_easy *ڈیٹا, ثابت حرف *سابقہ,
                        OM_uint32 major, OM_uint32 minor);
#ورنہ
#تعریف_کرو Curl_gss_log_error(ڈیٹا, سابقہ, major, minor) \
  کرو {                                                 \
    (خالی)(ڈیٹا);                                      \
    (خالی)(سابقہ);                                    \
    (خالی)(major);                                     \
    (خالی)(minor);                                     \
  } جب(0)
#اگر_ختم

/* Define our privacy and integrity protection values */
#تعریف_کرو GSSAUTH_P_NONE      1
#تعریف_کرو GSSAUTH_P_INTEGRITY 2
#تعریف_کرو GSSAUTH_P_PRIVACY   4

#اگر_ختم /* HAVE_GSSAPI */
#اگر_ختم /* HEADER_CURL_GSSAPI_H */
