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
#شامل "curl_setup.h"

#ifdef HAVE_GSSAPI

#شامل "curl_gssapi.h"
#شامل "curl_trc.h"
#شامل "curlx/strcopy.h"

#ifdef DEBUGBUILD
#اگر defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  آزاد    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#وگرنهاگر defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#شامل <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#وگرنه
#define CURL_ALIGN8
#endif

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
شمارش min_err_code {
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
ساختار stub_gss_ctx_id_t_desc {
  شمارش { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  صحیح have_krb5;
  صحیح have_ntlm;
  OM_uint32 flags;
  نویسه creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ساختار stub_gss_cred_id_t_desc {
  صحیح allow_krb5;
  صحیح allow_ntlm;
};

ایستا OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ساختار stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  ثابت gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  ثابت gss_channel_bindings_t درون_داد_chan_bindings,
  gss_buffer_desc *درون_داد_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  ساختار stub_gss_ctx_id_t_desc *ctx = پوچ;

  /* The token will be encoded in base64 */
  size_t length = اندازه(ctx->creds) * 3 / 4;
  size_t used = 0;
  نویسه *token = پوچ;
  ثابت نویسه *creds = پوچ;

  (تهی)mech_type;
  (تهی)time_req;
  (تهی)درون_داد_chan_bindings;
  (تهی)actual_mech_type;

  اگر(!min)
    بازگشت GSS_S_FAILURE;

  *min = 0;

  اگر(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    بازگشت GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  اگر(!creds || strlen(creds) >= اندازه(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    بازگشت GSS_S_FAILURE;
  }

  ctx = *context;
  اگر(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    بازگشت GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->مقدار = پوچ;

  اگر(درون_داد_token && درون_داد_token->length) {
    اگر(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      بازگشت GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    اگر(((نویسه *)درون_داد_token->مقدار)[0] == 'D') {
      /* Done */
      انتخاب(ctx->sent) {
      حالت STUB_GSS_KRB5:
      حالت STUB_GSS_NTLM3:
        اگر(ret_flags)
          *ret_flags = ctx->flags;
        اگر(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        بازگشت GSS_S_COMPLETE;
      پیشفرض:
        *min = STUB_GSS_SERVER_ERR;
        بازگشت GSS_S_FAILURE;
      }
    }

    اگر(((نویسه *)درون_داد_token->مقدار)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      بازگشت GSS_S_FAILURE;
    }

    /* Continue */
    انتخاب(ctx->sent) {
    حالت STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      اگر(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        بشکن;
      }
      وگرنه {
        *min = STUB_GSS_SERVER_ERR;
        بازگشت GSS_S_FAILURE;
      }
    حالت STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      بشکن;
    پیشفرض:
      *min = STUB_GSS_SERVER_ERR;
      بازگشت GSS_S_FAILURE;
    }
  }
  وگرنه {
    اگر(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      بازگشت GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, اندازه(*ctx));
    اگر(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      بازگشت GSS_S_FAILURE;
    }

    اگر(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    اگر(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    اگر(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ساختار stub_gss_cred_id_t_desc *cred =
        (ساختار stub_gss_cred_id_t_desc *)initiator_cred_handle;
      اگر(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      اگر(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    اگر(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    وگرنه اگر(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    وگرنه {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      بازگشت GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, اندازه(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  اگر(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    بازگشت GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    اگر(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      بازگشت GSS_S_FAILURE;
    }

    اگر(strlen(creds) + target_desc.length + 5 >= اندازه(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      بازگشت GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (صحیح)target_desc.length,
                          (ثابت نویسه *)target_desc.مقدار,
                          (صحیح)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  اگر(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    بازگشت GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->مقدار = token;
  output_token->length = length;

  بازگشت GSS_S_CONTINUE_NEEDED;
}

ایستا OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ساختار stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (تهی)output_token;

  اگر(!min)
    بازگشت GSS_S_FAILURE;

  اگر(!context) {
    *min = STUB_GSS_INVALID_CTX;
    بازگشت GSS_S_FAILURE;
  }
  اگر(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    بازگشت GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  بازگشت GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ایستا gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ایستا OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ساختار stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  صحیح *locally_initiated,
  صحیح *open_context)
{
  (تهی)src_name;
  (تهی)targ_name;
  (تهی)lifetime_rec;
  (تهی)ctx_flags;
  (تهی)locally_initiated;
  (تهی)open_context;

  اگر(!min)
    بازگشت GSS_S_FAILURE;

  اگر(!context) {
    *min = STUB_GSS_INVALID_CTX;
    بازگشت GSS_S_FAILURE;
  }

  *min = 0;
  اگر(mech_type) {
    انتخاب(context->sent) {
    حالت STUB_GSS_NTLM1:
    حالت STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      بشکن;
    پیشفرض:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      بشکن;
    }
  }

  بازگشت GSS_S_COMPLETE;
}
ایستا OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (تهی)desired_name;
  (تهی)time_req;
  (تهی)desired_mechs;
  (تهی)cred_usage;
  (تهی)actual_mechs;
  (تهی)time_rec;

  اگر(!min)
    بازگشت GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  اگر(output_cred_handle) {
    ساختار stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, اندازه(*cred));
    اگر(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      بازگشت GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  بازگشت GSS_S_COMPLETE;
}

ایستا OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  ثابت نویسه *creds;
  OM_uint32 major;

  اگر(!min)
    بازگشت GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  اگر(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    بازگشت GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  اگر(GSS_ERROR(major))
    بازگشت major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  اگر(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  بازگشت GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ایستا OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ثابت gss_OID_set mech_set)
{
  ساختار stub_gss_cred_id_t_desc *cred;
  size_t i;
  صحیح found_krb5 = 0;
  صحیح found_ntlm = 0;

  اگر(!min)
    بازگشت GSS_S_FAILURE;

  *min = 0;
  اگر(cred_handle == GSS_C_NO_CREDENTIAL)
    بازگشت GSS_S_FAILURE;

  cred = (ساختار stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  اگر(mech_set) {
    برای(i = 0; i < mech_set->شمار; i++) {
      gss_OID oid = &mech_set->elements[i];
      اگر(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      اگر(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  بازگشت GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

ایستا OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  اگر(!min)
    بازگشت GSS_S_FAILURE;

  *min = 0;
  اگر(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  بازگشت GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(ساختار Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t درون_داد_chan_bindings,
                                    gss_buffer_t درون_داد_token,
                                    gss_buffer_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  اگر(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  اگر(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#وگرنه
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  اگر(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (ساختار stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     درون_داد_chan_bindings,
                                     درون_داد_token,
                                     پوچ, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     پوچ /* time_rec */);
#endif /* CURL_GSS_STUB */

  بازگشت gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              درون_داد_chan_bindings,
                              درون_داد_token,
                              پوچ, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              پوچ /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_delete_sec_context(min,
                                     (ساختار stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  بازگشت gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_inquire_context(minor_status,
                             (ساختار stub_gss_ctx_id_t_desc *)context,
                             پوچ, پوچ, پوچ, mech_type,
                             پوچ, پوچ, پوچ);
#endif /* CURL_GSS_STUB */

  بازگشت gss_inquire_context(minor_status, context,
                             پوچ, پوچ, پوچ, mech_type,
                             پوچ, پوچ, پوچ);
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
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  بازگشت gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  بازگشت gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  بازگشت gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    بازگشت stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  بازگشت gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
ایستا size_t display_gss_error(OM_uint32 status, صحیح type,
                                نویسه *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 پیام_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  انجامبده {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &پیام_ctx,
                                  &status_string);
    اگر(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      اگر(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (صحیح)status_string.length,
                              (نویسه *)status_string.مقدار);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } وقتی(!GSS_ERROR(maj_stat) && پیام_ctx);

  بازگشت len;
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
تهی Curl_gss_log_error(ساختار Curl_easy *data, ثابت نویسه *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  نویسه buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  اگر(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic برداشت
#endif

#endif /* HAVE_GSSAPI */
