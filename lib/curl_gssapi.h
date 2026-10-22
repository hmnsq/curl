#నిర్వచితం_కాకపోతే శీర్షిక_CURL_GSSAPI_H
#నిర్వచించు శీర్షిక_CURL_GSSAPI_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"

#నిర్వచితమైతే HAVE_GSSAPI
బాహ్య gss_OID_desc Curl_spnego_mech_oid;
బాహ్య gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(నిర్మాణం Curl_easy *దత్తాంశం,
                                    OM_uint32 *minor_స్థితి,
                                    gss_ctx_id_t *context,
                                    gss_name_t లక్ష్యం_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ఆదానం_chan_bindings,
                                    gss_బఫరు_t ఆదానం_token,
                                    gss_బఫరు_t output_token,
                                    స్థిరరాశి bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_బఫరు_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_స్థితి,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_స్థితి,
                                gss_name_t desired_name,
                                OM_uint32 సమయం_req,
                                gss_OID_సమితి desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_సమితి *actual_mechs,
                                OM_uint32 *సమయం_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_స్థితి,
                                  gss_OID_సమితి *mech_సమితి);

#నిర్వచితమైతే HAVE_GSS_సమితి_NEG_MECHS
OM_uint32 Curl_gss_సమితి_neg_mechs(OM_uint32 *minor_స్థితి,
                                 gss_cred_id_t cred_handle,
                                 స్థిరరాశి gss_OID_సమితి mech_సమితి);
#అయితే_ముగింపు

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_స్థితి,
                                gss_cred_id_t *cred_handle);

#నిర్వచితమైతే CURLVERBOSE
/* Helper to log a GSS-API error status */
శూన్య Curl_gss_log_error(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *ఉపసర్గ,
                        OM_uint32 major, OM_uint32 minor);
#లేకపోతే
#నిర్వచించు Curl_gss_log_error(దత్తాంశం, ఉపసర్గ, major, minor) \
  చేయి {                                                 \
    (శూన్య)(దత్తాంశం);                                      \
    (శూన్య)(ఉపసర్గ);                                    \
    (శూన్య)(major);                                     \
    (శూన్య)(minor);                                     \
  } అయితే(0)
#అయితే_ముగింపు

/* Define our privacy and integrity protection values */
#నిర్వచించు GSSAUTH_P_NONE      1
#నిర్వచించు GSSAUTH_P_INTEGRITY 2
#నిర్వచించు GSSAUTH_P_PRIVACY   4

#అయితే_ముగింపు /* HAVE_GSSAPI */
#అయితే_ముగింపు /* HEADER_CURL_GSSAPI_H */
