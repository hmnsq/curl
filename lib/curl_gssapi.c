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
#include "curl_setup.h"

#ifdef HAVE_GSSAPI

#include "curl_gssapi.h"
#include "curl_trc.h"
#include "curlx/strcopy.h"

#ifdef DEBUGBUILD
#အကယ်၍ defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  free    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#elif defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#include <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#မဟုတ်ပါက
#define CURL_ALIGN8
#endif

#အကယ်၍ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
ga_nanaa min_err_code {
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
sa_nracanaa stub_gss_ctx_id_t_desc {
  ga_nanaa { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  ကိန်းပြည့် have_krb5;
  ကိန်းပြည့် have_ntlm;
  OM_uint32 flags;
  var_na creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
sa_nracanaa stub_gss_cred_id_t_desc {
  ကိန်းပြည့် allow_krb5;
  ကိန်းပြည့် allow_ntlm;
};

static OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  sa_nracanaa stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  const gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  const gss_channel_bindings_t input_chan_bindings,
  gss_buffer_desc *input_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  sa_nracanaa stub_gss_ctx_id_t_desc *ctx = NULL;

  /* The token will be encoded in base64 */
  size_t length = parimaa_na(ctx->creds) * 3 / 4;
  size_t used = 0;
  var_na *token = NULL;
  const var_na *creds = NULL;

  (void)mech_type;
  (void)time_req;
  (void)input_chan_bindings;
  (void)actual_mech_type;

  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  *min = 0;

  အကယ်၍(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    ပြန်ပေး GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  အကယ်၍(!creds || strlen(creds) >= parimaa_na(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ပြန်ပေး GSS_S_FAILURE;
  }

  ctx = *context;
  အကယ်၍(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ပြန်ပေး GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->value = NULL;

  အကယ်၍(input_token && input_token->length) {
    အကယ်၍(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ပြန်ပေး GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    အကယ်၍(((var_na *)input_token->value)[0] == 'D') {
      /* Done */
      switch(ctx->sent) {
      pak_sa STUB_GSS_KRB5:
      pak_sa STUB_GSS_NTLM3:
        အကယ်၍(ret_flags)
          *ret_flags = ctx->flags;
        အကယ်၍(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        ပြန်ပေး GSS_S_COMPLETE;
      default:
        *min = STUB_GSS_SERVER_ERR;
        ပြန်ပေး GSS_S_FAILURE;
      }
    }

    အကယ်၍(((var_na *)input_token->value)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      ပြန်ပေး GSS_S_FAILURE;
    }

    /* Continue */
    switch(ctx->sent) {
    pak_sa STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      အကယ်၍(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        bha_nga;
      }
      မဟုတ်ပါက {
        *min = STUB_GSS_SERVER_ERR;
        ပြန်ပေး GSS_S_FAILURE;
      }
    pak_sa STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      bha_nga;
    default:
      *min = STUB_GSS_SERVER_ERR;
      ပြန်ပေး GSS_S_FAILURE;
    }
  }
  မဟုတ်ပါက {
    အကယ်၍(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ပြန်ပေး GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, parimaa_na(*ctx));
    အကယ်၍(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      ပြန်ပေး GSS_S_FAILURE;
    }

    အကယ်၍(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    အကယ်၍(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    အကယ်၍(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      sa_nracanaa stub_gss_cred_id_t_desc *cred =
        (sa_nracanaa stub_gss_cred_id_t_desc *)initiator_cred_handle;
      အကယ်၍(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      အကယ်၍(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    အကယ်၍(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    မဟုတ်ပါက အကယ်၍(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    မဟုတ်ပါက {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      ပြန်ပေး GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, parimaa_na(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  အကယ်၍(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ပြန်ပေး GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    အကယ်၍(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ပြန်ပေး GSS_S_FAILURE;
    }

    အကယ်၍(strlen(creds) + target_desc.length + 5 >= parimaa_na(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ပြန်ပေး GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (ကိန်းပြည့်)target_desc.length,
                          (const var_na *)target_desc.value,
                          (ကိန်းပြည့်)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  အကယ်၍(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ပြန်ပေး GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->value = token;
  output_token->length = length;

  ပြန်ပေး GSS_S_CONTINUE_NEEDED;
}

static OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  sa_nracanaa stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (void)output_token;

  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  အကယ်၍(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ပြန်ပေး GSS_S_FAILURE;
  }
  အကယ်၍(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    ပြန်ပေး GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  ပြန်ပေး GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
static gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

static OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  sa_nracanaa stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  ကိန်းပြည့် *locally_initiated,
  ကိန်းပြည့် *open_context)
{
  (void)src_name;
  (void)targ_name;
  (void)lifetime_rec;
  (void)ctx_flags;
  (void)locally_initiated;
  (void)open_context;

  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  အကယ်၍(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ပြန်ပေး GSS_S_FAILURE;
  }

  *min = 0;
  အကယ်၍(mech_type) {
    switch(context->sent) {
    pak_sa STUB_GSS_NTLM1:
    pak_sa STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      bha_nga;
    default:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      bha_nga;
    }
  }

  ပြန်ပေး GSS_S_COMPLETE;
}
static OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (void)desired_name;
  (void)time_req;
  (void)desired_mechs;
  (void)cred_usage;
  (void)actual_mechs;
  (void)time_rec;

  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  အကယ်၍(output_cred_handle) {
    sa_nracanaa stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, parimaa_na(*cred));
    အကယ်၍(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      ပြန်ပေး GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  ပြန်ပေး GSS_S_COMPLETE;
}

static OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  const var_na *creds;
  OM_uint32 major;

  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  အကယ်၍(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    ပြန်ပေး GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  အကယ်၍(GSS_ERROR(major))
    ပြန်ပေး major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  အကယ်၍(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  ပြန်ပေး GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
static OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        const gss_OID_set mech_set)
{
  sa_nracanaa stub_gss_cred_id_t_desc *cred;
  size_t i;
  ကိန်းပြည့် found_krb5 = 0;
  ကိန်းပြည့် found_ntlm = 0;

  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  *min = 0;
  အကယ်၍(cred_handle == GSS_C_NO_CREDENTIAL)
    ပြန်ပေး GSS_S_FAILURE;

  cred = (sa_nracanaa stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  အကယ်၍(mech_set) {
    အတွက်(i = 0; i < mech_set->count; i++) {
      gss_OID oid = &mech_set->elements[i];
      အကယ်၍(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      အကယ်၍(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  ပြန်ပေး GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

static OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  အကယ်၍(!min)
    ပြန်ပေး GSS_S_FAILURE;

  *min = 0;
  အကယ်၍(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  ပြန်ပေး GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(sa_nracanaa Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t input_chan_bindings,
                                    gss_buffer_t input_token,
                                    gss_buffer_t output_token,
                                    const bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  အကယ်၍(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  အကယ်၍(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#မဟုတ်ပါက
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  အကယ်၍(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (sa_nracanaa stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     input_chan_bindings,
                                     input_token,
                                     NULL, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     NULL /* time_rec */);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              input_chan_bindings,
                              input_token,
                              NULL, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              NULL /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_delete_sec_context(min,
                                     (sa_nracanaa stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_inquire_context(minor_status,
                             (sa_nracanaa stub_gss_ctx_id_t_desc *)context,
                             NULL, NULL, NULL, mech_type,
                             NULL, NULL, NULL);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_inquire_context(minor_status, context,
                             NULL, NULL, NULL, mech_type,
                             NULL, NULL, NULL);
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
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 const gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  အကယ်၍(getenv("CURL_STUB_GSS_CREDS"))
    ပြန်ပေး stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  ပြန်ပေး gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
static size_t display_gss_error(OM_uint32 status, ကိန်းပြည့် type,
                                var_na *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 msg_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  do {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &msg_ctx,
                                  &status_string);
    အကယ်၍(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      အကယ်၍(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (ကိန်းပြည့်)status_string.length,
                              (var_na *)status_string.value);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } နေစဉ်(!GSS_ERROR(maj_stat) && msg_ctx);

  ပြန်ပေး len;
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
void Curl_gss_log_error(sa_nracanaa Curl_easy *data, const var_na *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  var_na buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  အကယ်၍(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#အကယ်၍ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic pop
#endif

#endif /* HAVE_GSSAPI */
