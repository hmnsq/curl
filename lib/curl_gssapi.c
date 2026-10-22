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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_GSSAPI

#ଅନ୍ତର୍ଭାଵ "curl_gssapi.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

#ifdef DEBUGBUILD
#ଯଦି defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  ମୁକ୍ତ    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#ନଚେତଯଦି defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ଅନ୍ତର୍ଭାଵ <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#ଅନ୍ୟଥା
#define CURL_ALIGN8
#endif

#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
ଗଣନା min_err_code {
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
ସଂରଚନା stub_gss_ctx_id_t_desc {
  ଗଣନା { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_krb5;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_ntlm;
  OM_uint32 flags;
  ଅକ୍ଷର creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ସଂରଚନା stub_gss_cred_id_t_desc {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_krb5;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_ntlm;
};

ସ୍ଥିର OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ସଂରଚନା stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  ଅଚଳ gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  ଅଚଳ gss_channel_bindings_t ନିବେଶ_chan_bindings,
  gss_buffer_desc *ନିବେଶ_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  ସଂରଚନା stub_gss_ctx_id_t_desc *ctx = ଖାଲି;

  /* The token will be encoded in base64 */
  size_t length = ଆକାର(ctx->creds) * 3 / 4;
  size_t used = 0;
  ଅକ୍ଷର *token = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *creds = ଖାଲି;

  (ଶୂନ୍ୟ)mech_type;
  (ଶୂନ୍ୟ)time_req;
  (ଶୂନ୍ୟ)ନିବେଶ_chan_bindings;
  (ଶୂନ୍ୟ)actual_mech_type;

  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  *min = 0;

  ଯଦି(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  ଯଦି(!creds || strlen(creds) >= ଆକାର(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  ctx = *context;
  ଯଦି(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->ମୂଲ୍ୟ = ଖାଲି;

  ଯଦି(ନିବେଶ_token && ନିବେଶ_token->length) {
    ଯଦି(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    ଯଦି(((ଅକ୍ଷର *)ନିବେଶ_token->ମୂଲ୍ୟ)[0] == 'D') {
      /* Done */
      ବାଛ(ctx->sent) {
      ସ୍ଥିତି STUB_GSS_KRB5:
      ସ୍ଥିତି STUB_GSS_NTLM3:
        ଯଦି(ret_flags)
          *ret_flags = ctx->flags;
        ଯଦି(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        ଫେରନ୍ତୁ GSS_S_COMPLETE;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *min = STUB_GSS_SERVER_ERR;
        ଫେରନ୍ତୁ GSS_S_FAILURE;
      }
    }

    ଯଦି(((ଅକ୍ଷର *)ନିବେଶ_token->ମୂଲ୍ୟ)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    /* Continue */
    ବାଛ(ctx->sent) {
    ସ୍ଥିତି STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      ଯଦି(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        ଭାଙ୍ଗ;
      }
      ଅନ୍ୟଥା {
        *min = STUB_GSS_SERVER_ERR;
        ଫେରନ୍ତୁ GSS_S_FAILURE;
      }
    ସ୍ଥିତି STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      *min = STUB_GSS_SERVER_ERR;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }
  }
  ଅନ୍ୟଥା {
    ଯଦି(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, ଆକାର(*ctx));
    ଯଦି(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    ଯଦି(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    ଯଦି(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    ଯଦି(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ସଂରଚନା stub_gss_cred_id_t_desc *cred =
        (ସଂରଚନା stub_gss_cred_id_t_desc *)initiator_cred_handle;
      ଯଦି(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      ଯଦି(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    ଯଦି(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    ଅନ୍ୟଥା ଯଦି(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    ଅନ୍ୟଥା {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, ଆକାର(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  ଯଦି(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    ଯଦି(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    ଯଦି(strlen(creds) + target_desc.length + 5 >= ଆକାର(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)target_desc.length,
                          (ଅଚଳ ଅକ୍ଷର *)target_desc.ମୂଲ୍ୟ,
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  ଯଦି(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->ମୂଲ୍ୟ = token;
  output_token->length = length;

  ଫେରନ୍ତୁ GSS_S_CONTINUE_NEEDED;
}

ସ୍ଥିର OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ସଂରଚନା stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (ଶୂନ୍ୟ)output_token;

  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  ଯଦି(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }
  ଯଦି(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  ଫେରନ୍ତୁ GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ସ୍ଥିର gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ସ୍ଥିର OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ସଂରଚନା stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା *locally_initiated,
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା *open_context)
{
  (ଶୂନ୍ୟ)src_name;
  (ଶୂନ୍ୟ)targ_name;
  (ଶୂନ୍ୟ)lifetime_rec;
  (ଶୂନ୍ୟ)ctx_flags;
  (ଶୂନ୍ୟ)locally_initiated;
  (ଶୂନ୍ୟ)open_context;

  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  ଯଦି(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  *min = 0;
  ଯଦି(mech_type) {
    ବାଛ(context->sent) {
    ସ୍ଥିତି STUB_GSS_NTLM1:
    ସ୍ଥିତି STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      ଭାଙ୍ଗ;
    }
  }

  ଫେରନ୍ତୁ GSS_S_COMPLETE;
}
ସ୍ଥିର OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (ଶୂନ୍ୟ)desired_name;
  (ଶୂନ୍ୟ)time_req;
  (ଶୂନ୍ୟ)desired_mechs;
  (ଶୂନ୍ୟ)cred_usage;
  (ଶୂନ୍ୟ)actual_mechs;
  (ଶୂନ୍ୟ)time_rec;

  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  ଯଦି(output_cred_handle) {
    ସଂରଚନା stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, ଆକାର(*cred));
    ଯଦି(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      ଫେରନ୍ତୁ GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  ଫେରନ୍ତୁ GSS_S_COMPLETE;
}

ସ୍ଥିର OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  ଅଚଳ ଅକ୍ଷର *creds;
  OM_uint32 major;

  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  ଯଦି(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    ଫେରନ୍ତୁ GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  ଯଦି(GSS_ERROR(major))
    ଫେରନ୍ତୁ major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  ଯଦି(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  ଫେରନ୍ତୁ GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ସ୍ଥିର OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ଅଚଳ gss_OID_set mech_set)
{
  ସଂରଚନା stub_gss_cred_id_t_desc *cred;
  size_t i;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା found_krb5 = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା found_ntlm = 0;

  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  *min = 0;
  ଯଦି(cred_handle == GSS_C_NO_CREDENTIAL)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  cred = (ସଂରଚନା stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  ଯଦି(mech_set) {
    ପାଇଁ(i = 0; i < mech_set->ଗଣନା_2; i++) {
      gss_OID oid = &mech_set->elements[i];
      ଯଦି(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      ଯଦି(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  ଫେରନ୍ତୁ GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

ସ୍ଥିର OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  ଯଦି(!min)
    ଫେରନ୍ତୁ GSS_S_FAILURE;

  *min = 0;
  ଯଦି(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  ଫେରନ୍ତୁ GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(ସଂରଚନା Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ନିବେଶ_chan_bindings,
                                    gss_buffer_t ନିବେଶ_token,
                                    gss_buffer_t output_token,
                                    ଅଚଳ bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  ଯଦି(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  ଯଦି(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#ଅନ୍ୟଥା
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  ଯଦି(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (ସଂରଚନା stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     ନିବେଶ_chan_bindings,
                                     ନିବେଶ_token,
                                     ଖାଲି, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ଖାଲି /* time_rec */);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              ନିବେଶ_chan_bindings,
                              ନିବେଶ_token,
                              ଖାଲି, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ଖାଲି /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_delete_sec_context(min,
                                     (ସଂରଚନା stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_inquire_context(minor_status,
                             (ସଂରଚନା stub_gss_ctx_id_t_desc *)context,
                             ଖାଲି, ଖାଲି, ଖାଲି, mech_type,
                             ଖାଲି, ଖାଲି, ଖାଲି);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_inquire_context(minor_status, context,
                             ଖାଲି, ଖାଲି, ଖାଲି, mech_type,
                             ଖାଲି, ଖାଲି, ଖାଲି);
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
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 ଅଚଳ gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  ଯଦି(getenv("CURL_STUB_GSS_CREDS"))
    ଫେରନ୍ତୁ stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  ଫେରନ୍ତୁ gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
ସ୍ଥିର size_t display_gss_error(OM_uint32 status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                ଅକ୍ଷର *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 ସନ୍ଦେଶ_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  କର {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &ସନ୍ଦେଶ_ctx,
                                  &status_string);
    ଯଦି(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      ଯଦି(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status_string.length,
                              (ଅକ୍ଷର *)status_string.ମୂଲ୍ୟ);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } ଯେତେବେଳେ(!GSS_ERROR(maj_stat) && ସନ୍ଦେଶ_ctx);

  ଫେରନ୍ତୁ len;
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
ଶୂନ୍ୟ Curl_gss_log_error(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  ଅକ୍ଷର buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  ଯଦି(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic କାଢ଼
#endif

#endif /* HAVE_GSSAPI */
