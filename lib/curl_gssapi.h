#अगर_अघोषित HEADER_CURL_GSSAPI_H
#घोषणा HEADER_CURL_GSSAPI_H
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
#समावेश "curl_setup.h"

#समावेश "urldata.h"

#अगर_घोषित HAVE_GSSAPI
बाह्य gss_OID_desc Curl_spnego_mech_oid;
बाह्य gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(काष्ठा Curl_easy *तथ्य,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t निवेश_chan_bindings,
                                    gss_बफर_t निवेश_token,
                                    gss_बफर_t output_token,
                                    स्थिर bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_बफर_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_status,
                                gss_name_t desired_name,
                                OM_uint32 time_req,
                                gss_OID_set desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_set *actual_mechs,
                                OM_uint32 *time_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set);

#अगर_घोषित HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 स्थिर gss_OID_set mech_set);
#पूर्ण_अगर

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle);

#अगर_घोषित CURLVERBOSE
/* Helper to log a GSS-API error status */
व्योम Curl_gss_log_error(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *prefix,
                        OM_uint32 major, OM_uint32 minor);
#अन्यथा
#घोषणा Curl_gss_log_error(तथ्य, prefix, major, minor) \
  करो {                                                 \
    (व्योम)(तथ्य);                                      \
    (व्योम)(prefix);                                    \
    (व्योम)(major);                                     \
    (व्योम)(minor);                                     \
  } जबतक(0)
#पूर्ण_अगर

/* Define our privacy and integrity protection values */
#घोषणा GSSAUTH_P_NONE      1
#घोषणा GSSAUTH_P_INTEGRITY 2
#घोषणा GSSAUTH_P_PRIVACY   4

#पूर्ण_अगर /* HAVE_GSSAPI */
#पूर्ण_अगर /* HEADER_CURL_GSSAPI_H */
