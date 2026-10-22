#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_GSSAPI_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_GSSAPI_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
ಬಾಹ್ಯ gss_OID_desc Curl_spnego_mech_oid;
ಬಾಹ್ಯ gss_OID_desc Curl_krb5_mech_oid;

/* Common method for using GSS-API */
OM_uint32 Curl_gss_init_sec_context(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    OM_uint32 *minor_ಸ್ಥಿತಿ,
                                    gss_ctx_id_t *context,
                                    gss_name_t ಗುರಿ_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ಒಳಹರಿವು_chan_bindings,
                                    gss_ಬಫರ್_t ಒಳಹರಿವು_token,
                                    gss_ಬಫರ್_t output_token,
                                    ಸ್ಥಿರಾಂಕ bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle);

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_ಬಫರ್_t output_token);

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                    gss_ctx_id_t context,
                                    gss_OID *mech_type);

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                gss_name_t desired_name,
                                OM_uint32 ಸಮಯ_req,
                                gss_OID_ಗಣ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_ಗಣ *actual_mechs,
                                OM_uint32 *ಸಮಯ_rec);

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                  gss_OID_ಗಣ *mech_ಗಣ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSS_ಗಣ_NEG_MECHS
OM_uint32 Curl_gss_ಗಣ_neg_mechs(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                 gss_cred_id_t cred_handle,
                                 ಸ್ಥಿರಾಂಕ gss_OID_ಗಣ mech_ಗಣ);
#ಆದರೆ_ಅಂತ್ಯ

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                gss_cred_id_t *cred_handle);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
/* Helper to log a GSS-API error status */
ಶೂನ್ಯ Curl_gss_log_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                        OM_uint32 major, OM_uint32 minor);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_gss_log_error(ದತ್ತಾಂಶ, ಪೂರ್ವಪ್ರತ್ಯಯ, major, minor) \
  ಮಾಡು {                                                 \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                                      \
    (ಶೂನ್ಯ)(ಪೂರ್ವಪ್ರತ್ಯಯ);                                    \
    (ಶೂನ್ಯ)(major);                                     \
    (ಶೂನ್ಯ)(minor);                                     \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/* Define our privacy and integrity protection values */
#ವ್ಯಾಖ್ಯಾನಿಸು GSSAUTH_P_NONE      1
#ವ್ಯಾಖ್ಯಾನಿಸು GSSAUTH_P_INTEGRITY 2
#ವ್ಯಾಖ್ಯಾನಿಸು GSSAUTH_P_PRIVACY   4

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSSAPI */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_GSSAPI_H */
