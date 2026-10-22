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

#ifdef HAVE_GSSAPI

#ضمن "curl_gssapi.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strcopy.h"

#ifdef DEBUGBUILD
#𐡄𐡍 defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  حرر    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#وإلاإذا defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ضمن <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#وإلا
#define CURL_ALIGN8
#endif

#𐡄𐡍 defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
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
تعداد min_err_code {
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
بنية stub_gss_ctx_id_t_desc {
  تعداد { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  صحيح have_krb5;
  صحيح have_ntlm;
  OM_uint32 flags;
  حرف creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
بنية stub_gss_cred_id_t_desc {
  صحيح allow_krb5;
  صحيح allow_ntlm;
};

ساكن OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  بنية stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  ثابت gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  ثابت gss_channel_bindings_t input_chan_bindings,
  gss_buffer_desc *input_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  بنية stub_gss_ctx_id_t_desc *ctx = عدم;

  /* The token will be encoded in base64 */
  size_t length = حجم(ctx->creds) * 3 / 4;
  size_t used = 0;
  حرف *token = عدم;
  ثابت حرف *creds = عدم;

  (فراغ)mech_type;
  (فراغ)time_req;
  (فراغ)input_chan_bindings;
  (فراغ)actual_mech_type;

  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;

  𐡄𐡍(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    أرجع GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  𐡄𐡍(!creds || strlen(creds) >= حجم(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  ctx = *context;
  𐡄𐡍(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->value = عدم;

  𐡄𐡍(input_token && input_token->length) {
    𐡄𐡍(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      أرجع GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    𐡄𐡍(((حرف *)input_token->value)[0] == 'D') {
      /* Done */
      بدل(ctx->sent) {
      حالة STUB_GSS_KRB5:
      حالة STUB_GSS_NTLM3:
        𐡄𐡍(ret_flags)
          *ret_flags = ctx->flags;
        𐡄𐡍(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        أرجع GSS_S_COMPLETE;
      افتراضي:
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    }

    𐡄𐡍(((حرف *)input_token->value)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      أرجع GSS_S_FAILURE;
    }

    /* Continue */
    بدل(ctx->sent) {
    حالة STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      𐡄𐡍(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        اكسر;
      }
      وإلا {
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    حالة STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      اكسر;
    افتراضي:
      *min = STUB_GSS_SERVER_ERR;
      أرجع GSS_S_FAILURE;
    }
  }
  وإلا {
    𐡄𐡍(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      أرجع GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, حجم(*ctx));
    𐡄𐡍(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      أرجع GSS_S_FAILURE;
    }

    𐡄𐡍(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    𐡄𐡍(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    𐡄𐡍(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      بنية stub_gss_cred_id_t_desc *cred =
        (بنية stub_gss_cred_id_t_desc *)initiator_cred_handle;
      𐡄𐡍(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      𐡄𐡍(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    𐡄𐡍(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    وإلا 𐡄𐡍(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    وإلا {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      أرجع GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, حجم(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  𐡄𐡍(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    أرجع GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    𐡄𐡍(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      أرجع GSS_S_FAILURE;
    }

    𐡄𐡍(strlen(creds) + target_desc.length + 5 >= حجم(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      أرجع GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (صحيح)target_desc.length,
                          (ثابت حرف *)target_desc.value,
                          (صحيح)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  𐡄𐡍(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    أرجع GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->value = token;
  output_token->length = length;

  أرجع GSS_S_CONTINUE_NEEDED;
}

ساكن OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  بنية stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (فراغ)output_token;

  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  𐡄𐡍(!context) {
    *min = STUB_GSS_INVALID_CTX;
    أرجع GSS_S_FAILURE;
  }
  𐡄𐡍(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    أرجع GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  أرجع GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ساكن gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ساكن OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  بنية stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  صحيح *locally_initiated,
  صحيح *open_context)
{
  (فراغ)src_name;
  (فراغ)targ_name;
  (فراغ)lifetime_rec;
  (فراغ)ctx_flags;
  (فراغ)locally_initiated;
  (فراغ)open_context;

  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  𐡄𐡍(!context) {
    *min = STUB_GSS_INVALID_CTX;
    أرجع GSS_S_FAILURE;
  }

  *min = 0;
  𐡄𐡍(mech_type) {
    بدل(context->sent) {
    حالة STUB_GSS_NTLM1:
    حالة STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      اكسر;
    افتراضي:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      اكسر;
    }
  }

  أرجع GSS_S_COMPLETE;
}
ساكن OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (فراغ)desired_name;
  (فراغ)time_req;
  (فراغ)desired_mechs;
  (فراغ)cred_usage;
  (فراغ)actual_mechs;
  (فراغ)time_rec;

  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  𐡄𐡍(output_cred_handle) {
    بنية stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, حجم(*cred));
    𐡄𐡍(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      أرجع GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  أرجع GSS_S_COMPLETE;
}

ساكن OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  ثابت حرف *creds;
  OM_uint32 major;

  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  𐡄𐡍(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  𐡄𐡍(GSS_ERROR(major))
    أرجع major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  𐡄𐡍(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  أرجع GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ساكن OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ثابت gss_OID_set mech_set)
{
  بنية stub_gss_cred_id_t_desc *cred;
  size_t i;
  صحيح found_krb5 = 0;
  صحيح found_ntlm = 0;

  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  𐡄𐡍(cred_handle == GSS_C_NO_CREDENTIAL)
    أرجع GSS_S_FAILURE;

  cred = (بنية stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  𐡄𐡍(mech_set) {
    لكل(i = 0; i < mech_set->count; i++) {
      gss_OID oid = &mech_set->elements[i];
      𐡄𐡍(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      𐡄𐡍(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  أرجع GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

ساكن OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  𐡄𐡍(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  𐡄𐡍(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  أرجع GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t input_chan_bindings,
                                    gss_buffer_t input_token,
                                    gss_buffer_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  𐡄𐡍(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  𐡄𐡍(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#وإلا
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  𐡄𐡍(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     input_chan_bindings,
                                     input_token,
                                     عدم, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     عدم /* time_rec */);
#endif /* CURL_GSS_STUB */

  أرجع gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              input_chan_bindings,
                              input_token,
                              عدم, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              عدم /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_delete_sec_context(min,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  أرجع gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_inquire_context(minor_status,
                             (بنية stub_gss_ctx_id_t_desc *)context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
#endif /* CURL_GSS_STUB */

  أرجع gss_inquire_context(minor_status, context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
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
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  أرجع gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  أرجع gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  أرجع gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  𐡄𐡍(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  أرجع gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
ساكن size_t display_gss_error(OM_uint32 status, صحيح type,
                                حرف *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 msg_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  𐡏𐡁𐡃 {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &msg_ctx,
                                  &status_string);
    𐡄𐡍(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      𐡄𐡍(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (صحيح)status_string.length,
                              (حرف *)status_string.value);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } طالما(!GSS_ERROR(maj_stat) && msg_ctx);

  أرجع len;
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
فراغ Curl_gss_log_error(بنية Curl_easy *data, ثابت حرف *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  حرف buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  𐡄𐡍(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#𐡄𐡍 defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic pop
#endif

#endif /* HAVE_GSSAPI */
