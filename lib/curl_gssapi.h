#اگر_تعریف_نشده سرآیند_CURL_GSSAPI_H
#تعریف_کن سرآیند_CURL_GSSAPI_H
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

#اگر_تعریف_شده HAVE_GSSAPI
خارجي gss_OID_desc Curl_spnego_mech_oid;
خارجي gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *داده,
                                    OM_uint32 *minor_وضعیت,
                                    gss_ctx_id_t *context,
                                    gss_name_t هدف_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t درون_داد_chan_bindings,
                                    gss_میانگیر_t درون_داد_token,
                                    gss_میانگیر_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_میانگیر_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_وضعیت,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_وضعیت,
                                gss_name_t desired_name,
                                OM_uint32 زمان_req,
                                gss_OID_مجموعه desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_مجموعه *actual_mechs,
                                OM_uint32 *زمان_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_وضعیت,
                                  gss_OID_مجموعه *mech_مجموعه);

#اگر_تعریف_شده HAVE_GSS_مجموعه_NEG_MECHS
OM_uint32 Curl_gss_مجموعه_neg_mechs(OM_uint32 *minor_وضعیت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_مجموعه mech_مجموعه);
#پایان_اگر

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_وضعیت,
                                gss_cred_id_t *cred_handle);

#اگر_تعریف_شده CURLVERBOSE
/* Helper to log a GSS-API error status */
فراغ Curl_gss_log_error(بنية Curl_easy *داده, ثابت حرف *پیشوند,
                        OM_uint32 major, OM_uint32 minor);
#وإلا
#تعریف_کن Curl_gss_log_error(داده, پیشوند, major, minor) \
  افعل {                                                 \
    (فراغ)(داده);                                      \
    (فراغ)(پیشوند);                                    \
    (فراغ)(major);                                     \
    (فراغ)(minor);                                     \
  } طالما(0)
#پایان_اگر

/* Define our privacy and integrity protection values */
#تعریف_کن GSSAUTH_P_NONE      1
#تعریف_کن GSSAUTH_P_INTEGRITY 2
#تعریف_کن GSSAUTH_P_PRIVACY   4

#پایان_اگر /* HAVE_GSSAPI */
#پایان_اگر /* HEADER_CURL_GSSAPI_H */
