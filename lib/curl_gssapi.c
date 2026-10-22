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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI

#ಅನ್ತರ್ಭಾವ "curl_gssapi.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ಆದರೆ defined(HAVE_GSSGNU) || !defined(_WIN32)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_gss_alloc ಸ್ಮರಣೆ_ಕೊಡು  /* freed via the GSS API gss_release_buffer() */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_gss_free  ಬಿಡುಗಡೆ    /* pair of the above */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#ಅಥವಾಆದರೆ defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ಅನ್ತರ್ಭಾವ <gssapi/gssapi_alloc.h>
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_gss_alloc gssalloc_malloc
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_gss_free  gssalloc_free
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_GSS_STUB
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* DEBUGBUILD */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ALIGN8  __attribute__((aligned(8)))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ALIGN8
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
ಎಣಿಕೆ min_err_ಸಂಕೇತ {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_ಸ್ಮರಣೆ,
  STUB_GSS_INVALID_ವಾದಗಳು,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_ಕೊನೆಯ
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
ರಚನೆ stub_gss_ctx_id_t_desc {
  ಎಣಿಕೆ { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } ಕಳುಹಿಸಿದ;
  ಪೂರ್ಣಾಂಕ have_krb5;
  ಪೂರ್ಣಾಂಕ have_ntlm;
  OM_uint32 flags;
  ಅಕ್ಷರ creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ರಚನೆ stub_gss_cred_id_t_desc {
  ಪೂರ್ಣಾಂಕ allow_krb5;
  ಪೂರ್ಣಾಂಕ allow_ntlm;
};

ಸ್ಥಿರ OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ರಚನೆ stub_gss_ctx_id_t_desc **context,
  gss_name_t ಗುರಿ_name,
  ಸ್ಥಿರಾಂಕ gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 ಸಮಯ_req,
  ಸ್ಥಿರಾಂಕ gss_channel_bindings_t ಒಳಹರಿವು_chan_bindings,
  gss_ಬಫರ್_desc *ಒಳಹರಿವು_token,
  gss_OID *actual_mech_type,
  gss_ಬಫರ್_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *ಸಮಯ_rec)
{
  ರಚನೆ stub_gss_ctx_id_t_desc *ctx = ಖಾಲಿ;

  /* The token will be encoded in base64 */
  size_t ಉದ್ದಳತೆ = ಗಾತ್ರ(ctx->creds) * 3 / 4;
  size_t ಬಳಸಿದ = 0;
  ಅಕ್ಷರ *token = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *creds = ಖಾಲಿ;

  (ಶೂನ್ಯ)mech_type;
  (ಶೂನ್ಯ)ಸಮಯ_req;
  (ಶೂನ್ಯ)ಒಳಹರಿವು_chan_bindings;
  (ಶೂನ್ಯ)actual_mech_type;

  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  *min = 0;

  ಆದರೆ(!context || !ಗುರಿ_name || !output_token) {
    *min = STUB_GSS_INVALID_ವಾದಗಳು;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  ಆದರೆ(!creds || ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(creds) >= ಗಾತ್ರ(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  ctx = *context;
  ಆದರೆ(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  output_token->ಉದ್ದಳತೆ = 0;
  output_token->ಮೌಲ್ಯ = ಖಾಲಿ;

  ಆದರೆ(ಒಳಹರಿವು_token && ಒಳಹರಿವು_token->ಉದ್ದಳತೆ) {
    ಆದರೆ(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    ಆದರೆ(((ಅಕ್ಷರ *)ಒಳಹರಿವು_token->ಮೌಲ್ಯ)[0] == 'D') {
      /*ಮುಗಿದ*/
      ಆಯ್ಕೆ(ctx->ಕಳುಹಿಸಿದ) {
      ಸ್ಥಿತಿ STUB_GSS_KRB5:
      ಸ್ಥಿತಿ STUB_GSS_NTLM3:
        ಆದರೆ(ret_flags)
          *ret_flags = ctx->flags;
        ಆದರೆ(ಸಮಯ_rec)
          *ಸಮಯ_rec = GSS_C_INDEFINITE;
        ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
      ಪೂರ್ವನಿಯೋಜಿತ:
        *min = STUB_GSS_SERVER_ERR;
        ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
      }
    }

    ಆದರೆ(((ಅಕ್ಷರ *)ಒಳಹರಿವು_token->ಮೌಲ್ಯ)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    /* Continue */
    ಆಯ್ಕೆ(ctx->ಕಳುಹಿಸಿದ) {
    ಸ್ಥಿತಿ STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      ಆದರೆ(ctx->have_ntlm) {
        ctx->ಕಳುಹಿಸಿದ = STUB_GSS_NTLM1;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        *min = STUB_GSS_SERVER_ERR;
        ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
      }
    ಸ್ಥಿತಿ STUB_GSS_NTLM1:
      ctx->ಕಳುಹಿಸಿದ = STUB_GSS_NTLM3;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      *min = STUB_GSS_SERVER_ERR;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
    ಆದರೆ(!ctx) {
      *min = STUB_GSS_NO_ಸ್ಮರಣೆ;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(creds, "KRB5"))
      ctx->have_krb5 = 1;

    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    ಆದರೆ(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ರಚನೆ stub_gss_cred_id_t_desc *cred =
        (ರಚನೆ stub_gss_cred_id_t_desc *)initiator_cred_handle;
      ಆದರೆ(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      ಆದರೆ(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    ಆದರೆ(ctx->have_krb5)
      ctx->ಕಳುಹಿಸಿದ = STUB_GSS_KRB5;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->have_ntlm)
      ctx->ಕಳುಹಿಸಿದ = STUB_GSS_NTLM1;
    ಇಲ್ಲದಿದ್ದರೆ {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, ಗಾತ್ರ(ctx->creds), creds, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(ಉದ್ದಳತೆ);
  ಆದರೆ(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_ಸ್ಮರಣೆ;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  {
    gss_ಬಫರ್_desc ಗುರಿ_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_ಸ್ಥಿತಿ;
    OM_uint32 major_ಸ್ಥಿತಿ;
    major_ಸ್ಥಿತಿ = gss_display_name(&minor_ಸ್ಥಿತಿ, ಗುರಿ_name,
                                    &ಗುರಿ_desc, &name_type);
    ಆದರೆ(GSS_ERROR(major_ಸ್ಥಿತಿ)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_ಸ್ಮರಣೆ;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(creds) + ಗುರಿ_desc.ಉದ್ದಳತೆ + 5 >= ಗಾತ್ರ(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_ಸ್ಮರಣೆ;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    ಬಳಸಿದ = curl_msnprintf(token, ಉದ್ದಳತೆ, "%s:%.*s:%d:", creds,
                          (ಪೂರ್ಣಾಂಕ)ಗುರಿ_desc.ಉದ್ದಳತೆ,
                          (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಗುರಿ_desc.ಮೌಲ್ಯ,
                          (ಪೂರ್ಣಾಂಕ)ctx->ಕಳುಹಿಸಿದ);

    gss_release_ಬಫರ್(&minor_ಸ್ಥಿತಿ, &ಗುರಿ_desc);
  }

  ಆದರೆ(ಬಳಸಿದ >= ಉದ್ದಳತೆ) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_ಸ್ಮರಣೆ;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + ಬಳಸಿದ, 'A', ಉದ್ದಳತೆ - ಬಳಸಿದ);

  *context = ctx;

  output_token->ಮೌಲ್ಯ = token;
  output_token->ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;

  ಹಿಂತಿರುಗಿ GSS_S_CONTINUE_NEEDED;
}

ಸ್ಥಿರ OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ರಚನೆ stub_gss_ctx_id_t_desc **context,
  gss_ಬಫರ್_t output_token)
{
  (ಶೂನ್ಯ)output_token;

  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  ಆದರೆ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }
  ಆದರೆ(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ಸ್ಥಿರ gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ಸ್ಥಿರ OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ರಚನೆ stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  ಪೂರ್ಣಾಂಕ *locally_initiated,
  ಪೂರ್ಣಾಂಕ *open_context)
{
  (ಶೂನ್ಯ)src_name;
  (ಶೂನ್ಯ)targ_name;
  (ಶೂನ್ಯ)lifetime_rec;
  (ಶೂನ್ಯ)ctx_flags;
  (ಶೂನ್ಯ)locally_initiated;
  (ಶೂನ್ಯ)open_context;

  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  ಆದರೆ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  *min = 0;
  ಆದರೆ(mech_type) {
    ಆಯ್ಕೆ(context->ಕಳುಹಿಸಿದ) {
    ಸ್ಥಿತಿ STUB_GSS_NTLM1:
    ಸ್ಥಿತಿ STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
}
ಸ್ಥಿರ OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 ಸಮಯ_req,
  gss_OID_ಗಣ desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_ಗಣ *actual_mechs,
  OM_uint32 *ಸಮಯ_rec)
{
  (ಶೂನ್ಯ)desired_name;
  (ಶೂನ್ಯ)ಸಮಯ_req;
  (ಶೂನ್ಯ)desired_mechs;
  (ಶೂನ್ಯ)cred_usage;
  (ಶೂನ್ಯ)actual_mechs;
  (ಶೂನ್ಯ)ಸಮಯ_rec;

  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  ಆದರೆ(output_cred_handle) {
    ರಚನೆ stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, ಗಾತ್ರ(*cred));
    ಆದರೆ(!cred) {
      *min = STUB_GSS_NO_ಸ್ಮರಣೆ;
      ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
}

ಸ್ಥಿರ OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_ಗಣ *mech_ಗಣ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *creds;
  OM_uint32 major;

  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  ಆದರೆ(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;
  }

  major = gss_create_ಖಾಲಿಗಳು_oid_ಗಣ(min, mech_ಗಣ);
  ಆದರೆ(GSS_ERROR(major))
    ಹಿಂತಿರುಗಿ major;

  /* Always include Kerberos */
  gss_ಕೂಡಿಸು_oid_ಗಣ_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_ಗಣ);

  /* Include NTLM if the stub creds contain NTLM */
  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(creds, "NTLM"))
    gss_ಕೂಡಿಸು_oid_ಗಣ_member(min, &stub_ntlmssp_oid, mech_ಗಣ);

  ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSS_ಗಣ_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ಸ್ಥಿರ OM_uint32 stub_gss_ಗಣ_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ಸ್ಥಿರಾಂಕ gss_OID_ಗಣ mech_ಗಣ)
{
  ರಚನೆ stub_gss_cred_id_t_desc *cred;
  size_t i;
  ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_krb5 = 0;
  ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_ntlm = 0;

  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  *min = 0;
  ಆದರೆ(cred_handle == GSS_C_NO_CREDENTIAL)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  cred = (ರಚನೆ stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  ಆದರೆ(mech_ಗಣ) {
    ಗಾಗಿ(i = 0; i < mech_ಗಣ->ಎಣಿಕೆ_2; i++) {
      gss_OID oid = &mech_ಗಣ->elements[i];
      ಆದರೆ(oid->ಉದ್ದಳತೆ == Curl_krb5_mech_oid.ಉದ್ದಳತೆ &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->ಉದ್ದಳತೆ))
        ಸಿಕ್ಕಿತು_krb5 = 1;
      ಆದರೆ(oid->ಉದ್ದಳತೆ == stub_ntlmssp_oid.ಉದ್ದಳತೆ &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->ಉದ್ದಳತೆ))
        ಸಿಕ್ಕಿತು_ntlm = 1;
    }
  }

  cred->allow_krb5 = ಸಿಕ್ಕಿತು_krb5;
  cred->allow_ntlm = ಸಿಕ್ಕಿತು_ntlm;
  ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
}
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSS_SET_NEG_MECHS */

ಸ್ಥಿರ OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  ಆದರೆ(!min)
    ಹಿಂತಿರುಗಿ GSS_S_FAILURE;

  *min = 0;
  ಆದರೆ(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  ಹಿಂತಿರುಗಿ GSS_S_COMPLETE;
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

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
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  ಆದರೆ(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_init_sec_context(minor_ಸ್ಥಿತಿ,
                                     cred_handle,
                                     (ರಚನೆ stub_gss_ctx_id_t_desc **)context,
                                     ಗುರಿ_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     ಒಳಹರಿವು_chan_bindings,
                                     ಒಳಹರಿವು_token,
                                     ಖಾಲಿ, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ಖಾಲಿ /* time_rec */);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_init_sec_context(minor_ಸ್ಥಿತಿ,
                              cred_handle,
                              context,
                              ಗುರಿ_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              ಒಳಹರಿವು_chan_bindings,
                              ಒಳಹರಿವು_token,
                              ಖಾಲಿ, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ಖಾಲಿ /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_ಬಫರ್_t output_token)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_delete_sec_context(min,
                                     (ರಚನೆ stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_inquire_context(minor_ಸ್ಥಿತಿ,
                             (ರಚನೆ stub_gss_ctx_id_t_desc *)context,
                             ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, mech_type,
                             ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_inquire_context(minor_ಸ್ಥಿತಿ, context,
                             ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, mech_type,
                             ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                gss_name_t desired_name,
                                OM_uint32 ಸಮಯ_req,
                                gss_OID_ಗಣ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_ಗಣ *actual_mechs,
                                OM_uint32 *ಸಮಯ_rec)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_acquire_cred(minor_ಸ್ಥಿತಿ, desired_name, ಸಮಯ_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, ಸಮಯ_rec);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_acquire_cred(minor_ಸ್ಥಿತಿ, desired_name, ಸಮಯ_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, ಸಮಯ_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                  gss_OID_ಗಣ *mech_ಗಣ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_indicate_mechs(minor_ಸ್ಥಿತಿ, mech_ಗಣ);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_indicate_mechs(minor_ಸ್ಥಿತಿ, mech_ಗಣ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSS_ಗಣ_NEG_MECHS
OM_uint32 Curl_gss_ಗಣ_neg_mechs(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                 gss_cred_id_t cred_handle,
                                 ಸ್ಥಿರಾಂಕ gss_OID_ಗಣ mech_ಗಣ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_ಗಣ_neg_mechs(minor_ಸ್ಥಿತಿ, cred_handle, mech_ಗಣ);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_ಗಣ_neg_mechs(minor_ಸ್ಥಿತಿ, cred_handle, mech_ಗಣ);
}
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_ಸ್ಥಿತಿ,
                                gss_cred_id_t *cred_handle)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GSS_STUB
  ಆದರೆ(getenv("CURL_STUB_GSS_CREDS"))
    ಹಿಂತಿರುಗಿ stub_gss_release_cred(minor_ಸ್ಥಿತಿ, cred_handle);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_GSS_STUB */

  ಹಿಂತಿರುಗಿ gss_release_cred(minor_ಸ್ಥಿತಿ, cred_handle);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು GSS_LOG_ಬಫರ್_LEN 1024
ಸ್ಥಿರ size_t display_gss_error(OM_uint32 ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ type,
                                ಅಕ್ಷರ *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 ಸಂದೇಶ_ctx = 0;
  gss_ಬಫರ್_desc ಸ್ಥಿತಿ_string = GSS_C_ಖಾಲಿಗಳು_ಬಫರ್;

  ಮಾಡು {
    maj_stat = gss_display_ಸ್ಥಿತಿ(&min_stat,
                                  ಸ್ಥಿತಿ_2,
                                  type,
                                  GSS_C_NO_OID,
                                  &ಸಂದೇಶ_ctx,
                                  &ಸ್ಥಿತಿ_string);
    ಆದರೆ(maj_stat == GSS_S_COMPLETE && ಸ್ಥಿತಿ_string.ಉದ್ದಳತೆ > 0) {
      ಆದರೆ(GSS_LOG_ಬಫರ್_LEN > len + ಸ್ಥಿತಿ_string.ಉದ್ದಳತೆ + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_ಬಫರ್_LEN - len,
                              "%.*s. ", (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_string.ಉದ್ದಳತೆ,
                              (ಅಕ್ಷರ *)ಸ್ಥಿತಿ_string.ಮೌಲ್ಯ);
      }
    }
    gss_release_ಬಫರ್(&min_stat, &ಸ್ಥಿತಿ_string);
  } ಆಗ(!GSS_ERROR(maj_stat) && ಸಂದೇಶ_ctx);

  ಹಿಂತಿರುಗಿ len;
}

/*
 * Curl_gss_log_error()
 *
 * This is used to log a GSS-API error status.
 *
 * Parameters:
 *
 * data    [in] - The session handle.
 * prefix  [in] - The prefix of the log message.
 * major   [in] - The major status code.
 * minor   [in] - The minor status code.
 */
ಶೂನ್ಯ Curl_gss_log_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                        OM_uint32 major, OM_uint32 minor)
{
  ಅಕ್ಷರ buf[GSS_LOG_ಬಫರ್_LEN] = "";
  size_t len = 0;

  ಆದರೆ(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_ಸಂಕೇತ, buf, len);

  display_gss_error(minor, GSS_C_MECH_ಸಂಕೇತ, buf, len);

  infof(ದತ್ತಾಂಶ, "%s%s", ಪೂರ್ವಪ್ರತ್ಯಯ, buf);
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSSAPI */
