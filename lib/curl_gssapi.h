#यदि_अपरिभाषितम् शीर्षकम्_CURL_GSSAPI_H
#परिभाषय शीर्षकम्_CURL_GSSAPI_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"

#यदि_परिभाषितम् HAVE_GSSAPI
बाह्य gss_OID_desc Curl_spnego_mech_oid;
बाह्य gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(संरचना Curl_easy *दत्त,
                                    OM_uint32 *minor_स्थितिः,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t निवेशः_chan_bindings,
                                    gss_बफर_t निवेशः_token,
                                    gss_बफर_t output_token,
                                    ध्रुव bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_बफर_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_स्थितिः,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_स्थितिः,
                                gss_name_t desired_name,
                                OM_uint32 time_req,
                                gss_OID_समुच्चयः desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_समुच्चयः *actual_mechs,
                                OM_uint32 *time_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_स्थितिः,
                                  gss_OID_समुच्चयः *mech_समुच्चयः);

#यदि_परिभाषितम् HAVE_GSS_समुच्चयः_NEG_MECHS
OM_uint32 Curl_gss_समुच्चयः_neg_mechs(OM_uint32 *minor_स्थितिः,
                                 gss_cred_id_t cred_handle,
                                 ध्रुव gss_OID_समुच्चयः mech_समुच्चयः);
#यदि_अन्तः

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_स्थितिः,
                                gss_cred_id_t *cred_handle);

#यदि_परिभाषितम् CURLVERBOSE
/* Helper to log a GSS-API error status */
शून्य Curl_gss_log_error(संरचना Curl_easy *दत्त, ध्रुव वर्ण *उपसर्गः,
                        OM_uint32 major, OM_uint32 minor);
#अन्यथा
#परिभाषय Curl_gss_log_error(दत्त, उपसर्गः, major, minor) \
  कुरु {                                                 \
    (शून्य)(दत्त);                                      \
    (शून्य)(उपसर्गः);                                    \
    (शून्य)(major);                                     \
    (शून्य)(minor);                                     \
  } यावत्(0)
#यदि_अन्तः

/* Define our privacy and integrity protection values */
#परिभाषय GSSAUTH_P_NONE      1
#परिभाषय GSSAUTH_P_INTEGRITY 2
#परिभाषय GSSAUTH_P_PRIVACY   4

#यदि_अन्तः /* HAVE_GSSAPI */
#यदि_अन्तः /* HEADER_CURL_GSSAPI_H */
