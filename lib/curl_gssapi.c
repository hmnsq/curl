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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_gssapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਜੇ defined(HAVE_GSSGNU) || !defined(_WIN32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_gss_alloc ਸਮ੍ਰਿਤੀ_ਦਿਓ  /* freed via the GSS API gss_release_buffer() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_gss_free  ਮੁਕਤ    /* pair of the above */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#ਜਾਂਜੇ defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ਅਨ੍ਤਰ੍ਭਾਵ <gssapi/gssapi_alloc.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_gss_alloc gssalloc_malloc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_gss_free  gssalloc_free
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_GSS_STUB
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* DEBUGBUILD */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ALIGN8  __attribute__((aligned(8)))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ALIGN8
#ਜੇ_ਅੰਤ

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
ਗਿਣਤੀ min_err_ਕੋਡ {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_ਸਮ੍ਰਿਤੀ,
  STUB_GSS_INVALID_ਦਲੀਲਾਂ,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_ਆਖਰੀ
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
ਬਣਤਰ stub_gss_ctx_id_t_desc {
  ਗਿਣਤੀ { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } ਭੇਜਿਆ;
  ਪੂਰਨ have_krb5;
  ਪੂਰਨ have_ntlm;
  OM_uint32 flags;
  ਅੱਖਰ creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ਬਣਤਰ stub_gss_cred_id_t_desc {
  ਪੂਰਨ allow_krb5;
  ਪੂਰਨ allow_ntlm;
};

ਸਥਿਰ OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ਬਣਤਰ stub_gss_ctx_id_t_desc **context,
  gss_name_t ਨਿਸ਼ਾਨਾ_name,
  ਅਚਲ gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 ਸਮਾਂ_req,
  ਅਚਲ gss_channel_bindings_t ਨਿਵੇਸ਼_chan_bindings,
  gss_ਬਫਰ_desc *ਨਿਵੇਸ਼_token,
  gss_OID *actual_mech_type,
  gss_ਬਫਰ_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *ਸਮਾਂ_rec)
{
  ਬਣਤਰ stub_gss_ctx_id_t_desc *ctx = ਨਿਰਾ;

  /* The token will be encoded in base64 */
  size_t ਲੰਮਾਈ = ਆਕਾਰ(ctx->creds) * 3 / 4;
  size_t ਵਰਤਿਆ = 0;
  ਅੱਖਰ *token = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *creds = ਨਿਰਾ;

  (ਖਾਲੀ)mech_type;
  (ਖਾਲੀ)ਸਮਾਂ_req;
  (ਖਾਲੀ)ਨਿਵੇਸ਼_chan_bindings;
  (ਖਾਲੀ)actual_mech_type;

  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  *min = 0;

  ਜੇ(!context || !ਨਿਸ਼ਾਨਾ_name || !output_token) {
    *min = STUB_GSS_INVALID_ਦਲੀਲਾਂ;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  ਜੇ(!creds || ਲੜੀ_ਲੰਬਾਈ(creds) >= ਆਕਾਰ(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  ctx = *context;
  ਜੇ(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  output_token->ਲੰਮਾਈ = 0;
  output_token->ਮੁੱਲ = ਨਿਰਾ;

  ਜੇ(ਨਿਵੇਸ਼_token && ਨਿਵੇਸ਼_token->ਲੰਮਾਈ) {
    ਜੇ(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    ਜੇ(((ਅੱਖਰ *)ਨਿਵੇਸ਼_token->ਮੁੱਲ)[0] == 'D') {
      /*ਪੂਰਾ*/
      ਚੋਣ(ctx->ਭੇਜਿਆ) {
      ਹਾਲਤ STUB_GSS_KRB5:
      ਹਾਲਤ STUB_GSS_NTLM3:
        ਜੇ(ret_flags)
          *ret_flags = ctx->flags;
        ਜੇ(ਸਮਾਂ_rec)
          *ਸਮਾਂ_rec = GSS_C_INDEFINITE;
        ਵਾਪਸ GSS_S_COMPLETE;
      ਮੂਲ:
        *min = STUB_GSS_SERVER_ERR;
        ਵਾਪਸ GSS_S_FAILURE;
      }
    }

    ਜੇ(((ਅੱਖਰ *)ਨਿਵੇਸ਼_token->ਮੁੱਲ)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    /* Continue */
    ਚੋਣ(ctx->ਭੇਜਿਆ) {
    ਹਾਲਤ STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      ਜੇ(ctx->have_ntlm) {
        ctx->ਭੇਜਿਆ = STUB_GSS_NTLM1;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ {
        *min = STUB_GSS_SERVER_ERR;
        ਵਾਪਸ GSS_S_FAILURE;
      }
    ਹਾਲਤ STUB_GSS_NTLM1:
      ctx->ਭੇਜਿਆ = STUB_GSS_NTLM3;
      ਰੋਕੋ;
    ਮੂਲ:
      *min = STUB_GSS_SERVER_ERR;
      ਵਾਪਸ GSS_S_FAILURE;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
    ਜੇ(!ctx) {
      *min = STUB_GSS_NO_ਸਮ੍ਰਿਤੀ;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    ਜੇ(ਲੜੀ_ਖੋਜ(creds, "KRB5"))
      ctx->have_krb5 = 1;

    ਜੇ(ਲੜੀ_ਖੋਜ(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    ਜੇ(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ਬਣਤਰ stub_gss_cred_id_t_desc *cred =
        (ਬਣਤਰ stub_gss_cred_id_t_desc *)initiator_cred_handle;
      ਜੇ(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      ਜੇ(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    ਜੇ(ctx->have_krb5)
      ctx->ਭੇਜਿਆ = STUB_GSS_KRB5;
    ਨਹੀਂਤਾਂ ਜੇ(ctx->have_ntlm)
      ctx->ਭੇਜਿਆ = STUB_GSS_NTLM1;
    ਨਹੀਂਤਾਂ {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, ਆਕਾਰ(ctx->creds), creds, ਲੜੀ_ਲੰਬਾਈ(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(ਲੰਮਾਈ);
  ਜੇ(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_ਸਮ੍ਰਿਤੀ;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  {
    gss_ਬਫਰ_desc ਨਿਸ਼ਾਨਾ_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_ਸਥਿਤੀ;
    OM_uint32 major_ਸਥਿਤੀ;
    major_ਸਥਿਤੀ = gss_display_name(&minor_ਸਥਿਤੀ, ਨਿਸ਼ਾਨਾ_name,
                                    &ਨਿਸ਼ਾਨਾ_desc, &name_type);
    ਜੇ(GSS_ERROR(major_ਸਥਿਤੀ)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_ਸਮ੍ਰਿਤੀ;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(creds) + ਨਿਸ਼ਾਨਾ_desc.ਲੰਮਾਈ + 5 >= ਆਕਾਰ(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_ਸਮ੍ਰਿਤੀ;
      ਵਾਪਸ GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    ਵਰਤਿਆ = curl_msnprintf(token, ਲੰਮਾਈ, "%s:%.*s:%d:", creds,
                          (ਪੂਰਨ)ਨਿਸ਼ਾਨਾ_desc.ਲੰਮਾਈ,
                          (ਅਚਲ ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ_desc.ਮੁੱਲ,
                          (ਪੂਰਨ)ctx->ਭੇਜਿਆ);

    gss_release_ਬਫਰ(&minor_ਸਥਿਤੀ, &ਨਿਸ਼ਾਨਾ_desc);
  }

  ਜੇ(ਵਰਤਿਆ >= ਲੰਮਾਈ) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_ਸਮ੍ਰਿਤੀ;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + ਵਰਤਿਆ, 'A', ਲੰਮਾਈ - ਵਰਤਿਆ);

  *context = ctx;

  output_token->ਮੁੱਲ = token;
  output_token->ਲੰਮਾਈ = ਲੰਮਾਈ;

  ਵਾਪਸ GSS_S_CONTINUE_NEEDED;
}

ਸਥਿਰ OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ਬਣਤਰ stub_gss_ctx_id_t_desc **context,
  gss_ਬਫਰ_t output_token)
{
  (ਖਾਲੀ)output_token;

  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  ਜੇ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ਵਾਪਸ GSS_S_FAILURE;
  }
  ਜੇ(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  ਵਾਪਸ GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ਸਥਿਰ gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ਸਥਿਰ OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ਬਣਤਰ stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  ਪੂਰਨ *locally_initiated,
  ਪੂਰਨ *open_context)
{
  (ਖਾਲੀ)src_name;
  (ਖਾਲੀ)targ_name;
  (ਖਾਲੀ)lifetime_rec;
  (ਖਾਲੀ)ctx_flags;
  (ਖਾਲੀ)locally_initiated;
  (ਖਾਲੀ)open_context;

  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  ਜੇ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  *min = 0;
  ਜੇ(mech_type) {
    ਚੋਣ(context->ਭੇਜਿਆ) {
    ਹਾਲਤ STUB_GSS_NTLM1:
    ਹਾਲਤ STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      ਰੋਕੋ;
    ਮੂਲ:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      ਰੋਕੋ;
    }
  }

  ਵਾਪਸ GSS_S_COMPLETE;
}
ਸਥਿਰ OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 ਸਮਾਂ_req,
  gss_OID_ਸੈੱਟ desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_ਸੈੱਟ *actual_mechs,
  OM_uint32 *ਸਮਾਂ_rec)
{
  (ਖਾਲੀ)desired_name;
  (ਖਾਲੀ)ਸਮਾਂ_req;
  (ਖਾਲੀ)desired_mechs;
  (ਖਾਲੀ)cred_usage;
  (ਖਾਲੀ)actual_mechs;
  (ਖਾਲੀ)ਸਮਾਂ_rec;

  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  ਜੇ(output_cred_handle) {
    ਬਣਤਰ stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, ਆਕਾਰ(*cred));
    ਜੇ(!cred) {
      *min = STUB_GSS_NO_ਸਮ੍ਰਿਤੀ;
      ਵਾਪਸ GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  ਵਾਪਸ GSS_S_COMPLETE;
}

ਸਥਿਰ OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_ਸੈੱਟ *mech_ਸੈੱਟ)
{
  ਅਚਲ ਅੱਖਰ *creds;
  OM_uint32 major;

  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  ਜੇ(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    ਵਾਪਸ GSS_S_FAILURE;
  }

  major = gss_create_ਖਾਲੀ_ਥਾਵਾਂ_oid_ਸੈੱਟ(min, mech_ਸੈੱਟ);
  ਜੇ(GSS_ERROR(major))
    ਵਾਪਸ major;

  /* Always include Kerberos */
  gss_ਜੋੜੋ_oid_ਸੈੱਟ_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_ਸੈੱਟ);

  /* Include NTLM if the stub creds contain NTLM */
  ਜੇ(ਲੜੀ_ਖੋਜ(creds, "NTLM"))
    gss_ਜੋੜੋ_oid_ਸੈੱਟ_member(min, &stub_ntlmssp_oid, mech_ਸੈੱਟ);

  ਵਾਪਸ GSS_S_COMPLETE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSS_ਸੈੱਟ_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ਸਥਿਰ OM_uint32 stub_gss_ਸੈੱਟ_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ਅਚਲ gss_OID_ਸੈੱਟ mech_ਸੈੱਟ)
{
  ਬਣਤਰ stub_gss_cred_id_t_desc *cred;
  size_t i;
  ਪੂਰਨ ਮਿਲਿਆ_krb5 = 0;
  ਪੂਰਨ ਮਿਲਿਆ_ntlm = 0;

  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  *min = 0;
  ਜੇ(cred_handle == GSS_C_NO_CREDENTIAL)
    ਵਾਪਸ GSS_S_FAILURE;

  cred = (ਬਣਤਰ stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  ਜੇ(mech_ਸੈੱਟ) {
    ਲਈ(i = 0; i < mech_ਸੈੱਟ->ਗਿਣਤੀ_2; i++) {
      gss_OID oid = &mech_ਸੈੱਟ->elements[i];
      ਜੇ(oid->ਲੰਮਾਈ == Curl_krb5_mech_oid.ਲੰਮਾਈ &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->ਲੰਮਾਈ))
        ਮਿਲਿਆ_krb5 = 1;
      ਜੇ(oid->ਲੰਮਾਈ == stub_ntlmssp_oid.ਲੰਮਾਈ &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->ਲੰਮਾਈ))
        ਮਿਲਿਆ_ntlm = 1;
    }
  }

  cred->allow_krb5 = ਮਿਲਿਆ_krb5;
  cred->allow_ntlm = ਮਿਲਿਆ_ntlm;
  ਵਾਪਸ GSS_S_COMPLETE;
}
#ਜੇ_ਅੰਤ /* HAVE_GSS_SET_NEG_MECHS */

ਸਥਿਰ OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  ਜੇ(!min)
    ਵਾਪਸ GSS_S_FAILURE;

  *min = 0;
  ਜੇ(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  ਵਾਪਸ GSS_S_COMPLETE;
}

#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

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
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  ਜੇ(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#ਜੇ_ਅੰਤ
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_init_sec_context(minor_ਸਥਿਤੀ,
                                     cred_handle,
                                     (ਬਣਤਰ stub_gss_ctx_id_t_desc **)context,
                                     ਨਿਸ਼ਾਨਾ_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     ਨਿਵੇਸ਼_chan_bindings,
                                     ਨਿਵੇਸ਼_token,
                                     ਨਿਰਾ, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ਨਿਰਾ /* time_rec */);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_init_sec_context(minor_ਸਥਿਤੀ,
                              cred_handle,
                              context,
                              ਨਿਸ਼ਾਨਾ_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              ਨਿਵੇਸ਼_chan_bindings,
                              ਨਿਵੇਸ਼_token,
                              ਨਿਰਾ, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ਨਿਰਾ /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_ਬਫਰ_t output_token)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_delete_sec_context(min,
                                     (ਬਣਤਰ stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_ਸਥਿਤੀ,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_inquire_context(minor_ਸਥਿਤੀ,
                             (ਬਣਤਰ stub_gss_ctx_id_t_desc *)context,
                             ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, mech_type,
                             ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_inquire_context(minor_ਸਥਿਤੀ, context,
                             ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, mech_type,
                             ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_ਸਥਿਤੀ,
                                gss_name_t desired_name,
                                OM_uint32 ਸਮਾਂ_req,
                                gss_OID_ਸੈੱਟ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_ਸੈੱਟ *actual_mechs,
                                OM_uint32 *ਸਮਾਂ_rec)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_acquire_cred(minor_ਸਥਿਤੀ, desired_name, ਸਮਾਂ_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, ਸਮਾਂ_rec);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_acquire_cred(minor_ਸਥਿਤੀ, desired_name, ਸਮਾਂ_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, ਸਮਾਂ_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_ਸਥਿਤੀ,
                                  gss_OID_ਸੈੱਟ *mech_ਸੈੱਟ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_indicate_mechs(minor_ਸਥਿਤੀ, mech_ਸੈੱਟ);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_indicate_mechs(minor_ਸਥਿਤੀ, mech_ਸੈੱਟ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSS_ਸੈੱਟ_NEG_MECHS
OM_uint32 Curl_gss_ਸੈੱਟ_neg_mechs(OM_uint32 *minor_ਸਥਿਤੀ,
                                 gss_cred_id_t cred_handle,
                                 ਅਚਲ gss_OID_ਸੈੱਟ mech_ਸੈੱਟ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_ਸੈੱਟ_neg_mechs(minor_ਸਥਿਤੀ, cred_handle, mech_ਸੈੱਟ);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_ਸੈੱਟ_neg_mechs(minor_ਸਥਿਤੀ, cred_handle, mech_ਸੈੱਟ);
}
#ਜੇ_ਅੰਤ /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_ਸਥਿਤੀ,
                                gss_cred_id_t *cred_handle)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GSS_STUB
  ਜੇ(getenv("CURL_STUB_GSS_CREDS"))
    ਵਾਪਸ stub_gss_release_cred(minor_ਸਥਿਤੀ, cred_handle);
#ਜੇ_ਅੰਤ /* CURL_GSS_STUB */

  ਵਾਪਸ gss_release_cred(minor_ਸਥਿਤੀ, cred_handle);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GSS_LOG_ਬਫਰ_LEN 1024
ਸਥਿਰ size_t display_gss_error(OM_uint32 ਸਥਿਤੀ, ਪੂਰਨ type,
                                ਅੱਖਰ *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 ਸੁਨੇਹਾ_ctx = 0;
  gss_ਬਫਰ_desc ਸਥਿਤੀ_string = GSS_C_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;

  ਕਰੋ {
    maj_stat = gss_display_ਸਥਿਤੀ(&min_stat,
                                  ਸਥਿਤੀ,
                                  type,
                                  GSS_C_NO_OID,
                                  &ਸੁਨੇਹਾ_ctx,
                                  &ਸਥਿਤੀ_string);
    ਜੇ(maj_stat == GSS_S_COMPLETE && ਸਥਿਤੀ_string.ਲੰਮਾਈ > 0) {
      ਜੇ(GSS_LOG_ਬਫਰ_LEN > len + ਸਥਿਤੀ_string.ਲੰਮਾਈ + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_ਬਫਰ_LEN - len,
                              "%.*s. ", (ਪੂਰਨ)ਸਥਿਤੀ_string.ਲੰਮਾਈ,
                              (ਅੱਖਰ *)ਸਥਿਤੀ_string.ਮੁੱਲ);
      }
    }
    gss_release_ਬਫਰ(&min_stat, &ਸਥਿਤੀ_string);
  } ਜਦੋਂ(!GSS_ERROR(maj_stat) && ਸੁਨੇਹਾ_ctx);

  ਵਾਪਸ len;
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
ਖਾਲੀ Curl_gss_log_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ,
                        OM_uint32 major, OM_uint32 minor)
{
  ਅੱਖਰ buf[GSS_LOG_ਬਫਰ_LEN] = "";
  size_t len = 0;

  ਜੇ(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_ਕੋਡ, buf, len);

  display_gss_error(minor, GSS_C_MECH_ਕੋਡ, buf, len);

  infof(ਅੰਕੜੇ, "%s%s", ਅਗੇਤਰ, buf);
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HAVE_GSSAPI */
