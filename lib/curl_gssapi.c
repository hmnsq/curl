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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_GSSAPI

#অন্তর্ভাব "curl_gssapi.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/strcopy.h"

#ifdef DEBUGBUILD
#যদি defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  মুক্ত    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#নতুবা_যদি defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#অন্তর্ভাব <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#অন্যথা
#define CURL_ALIGN8
#endif

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
গণনা min_err_code {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_MEMORY,
  STUB_GSS_INVALID_ARGS,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_LAST
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
কাঠামো stub_gss_ctx_id_t_desc {
  গণনা { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  পূর্ণসংখ্যা have_krb5;
  পূর্ণসংখ্যা have_ntlm;
  OM_uint32 flags;
  অক্ষর creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
কাঠামো stub_gss_cred_id_t_desc {
  পূর্ণসংখ্যা allow_krb5;
  পূর্ণসংখ্যা allow_ntlm;
};

স্থির OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  কাঠামো stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  ধ্রুবক gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  ধ্রুবক gss_channel_bindings_t প্রবেশ_chan_bindings,
  gss_বাফার_desc *প্রবেশ_token,
  gss_OID *actual_mech_type,
  gss_বাফার_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  কাঠামো stub_gss_ctx_id_t_desc *ctx = ফাঁকা;

  /* The token will be encoded in base64 */
  size_t দৈর্ঘ্য = আকার(ctx->creds) * 3 / 4;
  size_t used = 0;
  অক্ষর *token = ফাঁকা;
  ধ্রুবক অক্ষর *creds = ফাঁকা;

  (শূন্য)mech_type;
  (শূন্য)time_req;
  (শূন্য)প্রবেশ_chan_bindings;
  (শূন্য)actual_mech_type;

  যদি(!min)
    ফেরত GSS_S_FAILURE;

  *min = 0;

  যদি(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    ফেরত GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  যদি(!creds || strlen(creds) >= আকার(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ফেরত GSS_S_FAILURE;
  }

  ctx = *context;
  যদি(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ফেরত GSS_S_FAILURE;
  }

  output_token->দৈর্ঘ্য = 0;
  output_token->মান = ফাঁকা;

  যদি(প্রবেশ_token && প্রবেশ_token->দৈর্ঘ্য) {
    যদি(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ফেরত GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    যদি(((অক্ষর *)প্রবেশ_token->মান)[0] == 'D') {
      /* Done */
      নির্বাচন(ctx->sent) {
      ক্ষেত্র STUB_GSS_KRB5:
      ক্ষেত্র STUB_GSS_NTLM3:
        যদি(ret_flags)
          *ret_flags = ctx->flags;
        যদি(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        ফেরত GSS_S_COMPLETE;
      পূর্বনির্ধারিত:
        *min = STUB_GSS_SERVER_ERR;
        ফেরত GSS_S_FAILURE;
      }
    }

    যদি(((অক্ষর *)প্রবেশ_token->মান)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      ফেরত GSS_S_FAILURE;
    }

    /* Continue */
    নির্বাচন(ctx->sent) {
    ক্ষেত্র STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      যদি(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        বিরতি;
      }
      অন্যথা {
        *min = STUB_GSS_SERVER_ERR;
        ফেরত GSS_S_FAILURE;
      }
    ক্ষেত্র STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      বিরতি;
    পূর্বনির্ধারিত:
      *min = STUB_GSS_SERVER_ERR;
      ফেরত GSS_S_FAILURE;
    }
  }
  অন্যথা {
    যদি(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ফেরত GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, আকার(*ctx));
    যদি(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      ফেরত GSS_S_FAILURE;
    }

    যদি(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    যদি(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    যদি(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      কাঠামো stub_gss_cred_id_t_desc *cred =
        (কাঠামো stub_gss_cred_id_t_desc *)initiator_cred_handle;
      যদি(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      যদি(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    যদি(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    অন্যথা যদি(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    অন্যথা {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      ফেরত GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, আকার(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(দৈর্ঘ্য);
  যদি(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ফেরত GSS_S_FAILURE;
  }

  {
    gss_বাফার_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    যদি(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ফেরত GSS_S_FAILURE;
    }

    যদি(strlen(creds) + target_desc.দৈর্ঘ্য + 5 >= আকার(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ফেরত GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, দৈর্ঘ্য, "%s:%.*s:%d:", creds,
                          (পূর্ণসংখ্যা)target_desc.দৈর্ঘ্য,
                          (ধ্রুবক অক্ষর *)target_desc.মান,
                          (পূর্ণসংখ্যা)ctx->sent);

    gss_release_বাফার(&minor_status, &target_desc);
  }

  যদি(used >= দৈর্ঘ্য) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ফেরত GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', দৈর্ঘ্য - used);

  *context = ctx;

  output_token->মান = token;
  output_token->দৈর্ঘ্য = দৈর্ঘ্য;

  ফেরত GSS_S_CONTINUE_NEEDED;
}

স্থির OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  কাঠামো stub_gss_ctx_id_t_desc **context,
  gss_বাফার_t output_token)
{
  (শূন্য)output_token;

  যদি(!min)
    ফেরত GSS_S_FAILURE;

  যদি(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ফেরত GSS_S_FAILURE;
  }
  যদি(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    ফেরত GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  ফেরত GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
স্থির gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

স্থির OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  কাঠামো stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  পূর্ণসংখ্যা *locally_initiated,
  পূর্ণসংখ্যা *open_context)
{
  (শূন্য)src_name;
  (শূন্য)targ_name;
  (শূন্য)lifetime_rec;
  (শূন্য)ctx_flags;
  (শূন্য)locally_initiated;
  (শূন্য)open_context;

  যদি(!min)
    ফেরত GSS_S_FAILURE;

  যদি(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ফেরত GSS_S_FAILURE;
  }

  *min = 0;
  যদি(mech_type) {
    নির্বাচন(context->sent) {
    ক্ষেত্র STUB_GSS_NTLM1:
    ক্ষেত্র STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      বিরতি;
    পূর্বনির্ধারিত:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      বিরতি;
    }
  }

  ফেরত GSS_S_COMPLETE;
}
স্থির OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (শূন্য)desired_name;
  (শূন্য)time_req;
  (শূন্য)desired_mechs;
  (শূন্য)cred_usage;
  (শূন্য)actual_mechs;
  (শূন্য)time_rec;

  যদি(!min)
    ফেরত GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  যদি(output_cred_handle) {
    কাঠামো stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, আকার(*cred));
    যদি(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      ফেরত GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  ফেরত GSS_S_COMPLETE;
}

স্থির OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  ধ্রুবক অক্ষর *creds;
  OM_uint32 major;

  যদি(!min)
    ফেরত GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  যদি(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    ফেরত GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  যদি(GSS_ERROR(major))
    ফেরত major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  যদি(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  ফেরত GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
স্থির OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ধ্রুবক gss_OID_set mech_set)
{
  কাঠামো stub_gss_cred_id_t_desc *cred;
  size_t i;
  পূর্ণসংখ্যা found_krb5 = 0;
  পূর্ণসংখ্যা found_ntlm = 0;

  যদি(!min)
    ফেরত GSS_S_FAILURE;

  *min = 0;
  যদি(cred_handle == GSS_C_NO_CREDENTIAL)
    ফেরত GSS_S_FAILURE;

  cred = (কাঠামো stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  যদি(mech_set) {
    জন্য(i = 0; i < mech_set->গুনতি; i++) {
      gss_OID oid = &mech_set->elements[i];
      যদি(oid->দৈর্ঘ্য == Curl_krb5_mech_oid.দৈর্ঘ্য &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->দৈর্ঘ্য))
        found_krb5 = 1;
      যদি(oid->দৈর্ঘ্য == stub_ntlmssp_oid.দৈর্ঘ্য &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->দৈর্ঘ্য))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  ফেরত GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

স্থির OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  যদি(!min)
    ফেরত GSS_S_FAILURE;

  *min = 0;
  যদি(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  ফেরত GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(কাঠামো Curl_easy *তথ্য,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t প্রবেশ_chan_bindings,
                                    gss_বাফার_t প্রবেশ_token,
                                    gss_বাফার_t output_token,
                                    ধ্রুবক bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  যদি(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  যদি(তথ্য->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#অন্যথা
    infof(তথ্য, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  যদি(তথ্য->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (কাঠামো stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     প্রবেশ_chan_bindings,
                                     প্রবেশ_token,
                                     ফাঁকা, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ফাঁকা /* time_rec */);
#endif /* CURL_GSS_STUB */

  ফেরত gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              প্রবেশ_chan_bindings,
                              প্রবেশ_token,
                              ফাঁকা, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ফাঁকা /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_বাফার_t output_token)
{
#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_delete_sec_context(min,
                                     (কাঠামো stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  ফেরত gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_inquire_context(minor_status,
                             (কাঠামো stub_gss_ctx_id_t_desc *)context,
                             ফাঁকা, ফাঁকা, ফাঁকা, mech_type,
                             ফাঁকা, ফাঁকা, ফাঁকা);
#endif /* CURL_GSS_STUB */

  ফেরত gss_inquire_context(minor_status, context,
                             ফাঁকা, ফাঁকা, ফাঁকা, mech_type,
                             ফাঁকা, ফাঁকা, ফাঁকা);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_status,
                                gss_name_t desired_name,
                                OM_uint32 time_req,
                                gss_OID_set desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_set *actual_mechs,
                                OM_uint32 *time_rec)
{
#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  ফেরত gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  ফেরত gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 ধ্রুবক gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  ফেরত gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  যদি(getenv("CURL_STUB_GSS_CREDS"))
    ফেরত stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  ফেরত gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_বাফার_LEN 1024
স্থির size_t display_gss_error(OM_uint32 status, পূর্ণসংখ্যা type,
                                অক্ষর *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 বার্তা_ctx = 0;
  gss_বাফার_desc status_string = GSS_C_EMPTY_বাফার;

  করো {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &বার্তা_ctx,
                                  &status_string);
    যদি(maj_stat == GSS_S_COMPLETE && status_string.দৈর্ঘ্য > 0) {
      যদি(GSS_LOG_বাফার_LEN > len + status_string.দৈর্ঘ্য + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_বাফার_LEN - len,
                              "%.*s. ", (পূর্ণসংখ্যা)status_string.দৈর্ঘ্য,
                              (অক্ষর *)status_string.মান);
      }
    }
    gss_release_বাফার(&min_stat, &status_string);
  } যখন(!GSS_ERROR(maj_stat) && বার্তা_ctx);

  ফেরত len;
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
শূন্য Curl_gss_log_error(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  অক্ষর buf[GSS_LOG_বাফার_LEN] = "";
  size_t len = 0;

  যদি(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(তথ্য, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic বের_করো
#endif

#endif /* HAVE_GSSAPI */
