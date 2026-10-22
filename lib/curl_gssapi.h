#جے_بنا_تعریف سرخی_CURL_GSSAPI_H
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
#ضمن "curl_setup.h"

#ضمن "urldata.h"

#جے_تعریف_شدہ HAVE_GSSAPI
خارجي gss_OID_desc Curl_spnego_mech_oid;
خارجي gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *انکڑے,
                                    OM_uint32 *minor_حالت,
                                    gss_ctx_id_t *context,
                                    gss_name_t نشانہ_name,
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
                                OM_uint32 ویلا_req,
                                gss_OID_سیٹ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_سیٹ *actual_mechs,
                                OM_uint32 *ویلا_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_حالت,
                                  gss_OID_سیٹ *mech_سیٹ);

#جے_تعریف_شدہ HAVE_GSS_سیٹ_NEG_MECHS
OM_uint32 Curl_gss_سیٹ_neg_mechs(OM_uint32 *minor_حالت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_سیٹ mech_سیٹ);
#جے_انت

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_حالت,
                                gss_cred_id_t *cred_handle);

#جے_تعریف_شدہ CURLVERBOSE
/* Helper to log a GSS-API error status */
فراغ Curl_gss_log_error(بنية Curl_easy *انکڑے, ثابت حرف *اگیتر,
                        OM_uint32 major, OM_uint32 minor);
#وإلا
#تعریف_کرو Curl_gss_log_error(انکڑے, اگیتر, major, minor) \
  افعل {                                                 \
    (فراغ)(انکڑے);                                      \
    (فراغ)(اگیتر);                                    \
    (فراغ)(major);                                     \
    (فراغ)(minor);                                     \
  } طالما(0)
#جے_انت

/* Define our privacy and integrity protection values */
#تعریف_کرو GSSAUTH_P_NONE      1
#تعریف_کرو GSSAUTH_P_INTEGRITY 2
#تعریف_کرو GSSAUTH_P_PRIVACY   4

#جے_انت /* HAVE_GSSAPI */
#جے_انت /* HEADER_CURL_GSSAPI_H */
