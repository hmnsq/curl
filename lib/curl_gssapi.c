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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_GSSAPI

#அந்தர்பாவ "curl_gssapi.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "curlx/strcopy.h"

#ifdef DEBUGBUILD
#எனில் defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  விடுவி    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#அல்லதெனில் defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#அந்தர்பாவ <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#இல்லையெனில்
#define CURL_ALIGN8
#endif

#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
எண்ணிக்கை min_err_code {
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
கட்டமைப்பு stub_gss_ctx_id_t_desc {
  எண்ணிக்கை { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  முழு have_krb5;
  முழு have_ntlm;
  OM_uint32 flags;
  எழுத்து creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
கட்டமைப்பு stub_gss_cred_id_t_desc {
  முழு allow_krb5;
  முழு allow_ntlm;
};

நிலையான OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  கட்டமைப்பு stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  மாறிலி gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  மாறிலி gss_channel_bindings_t உள்ளீடு_chan_bindings,
  gss_buffer_desc *உள்ளீடு_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  கட்டமைப்பு stub_gss_ctx_id_t_desc *ctx = வெற்று;

  /* The token will be encoded in base64 */
  size_t length = அளவு(ctx->creds) * 3 / 4;
  size_t used = 0;
  எழுத்து *token = வெற்று;
  மாறிலி எழுத்து *creds = வெற்று;

  (வெறுமை)mech_type;
  (வெறுமை)time_req;
  (வெறுமை)உள்ளீடு_chan_bindings;
  (வெறுமை)actual_mech_type;

  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  *min = 0;

  எனில்(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    திரும்பு GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  எனில்(!creds || strlen(creds) >= அளவு(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    திரும்பு GSS_S_FAILURE;
  }

  ctx = *context;
  எனில்(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    திரும்பு GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->மதிப்பு = வெற்று;

  எனில்(உள்ளீடு_token && உள்ளீடு_token->length) {
    எனில்(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      திரும்பு GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    எனில்(((எழுத்து *)உள்ளீடு_token->மதிப்பு)[0] == 'D') {
      /* Done */
      தேர்வு(ctx->sent) {
      நிலை STUB_GSS_KRB5:
      நிலை STUB_GSS_NTLM3:
        எனில்(ret_flags)
          *ret_flags = ctx->flags;
        எனில்(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        திரும்பு GSS_S_COMPLETE;
      இயல்பு:
        *min = STUB_GSS_SERVER_ERR;
        திரும்பு GSS_S_FAILURE;
      }
    }

    எனில்(((எழுத்து *)உள்ளீடு_token->மதிப்பு)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      திரும்பு GSS_S_FAILURE;
    }

    /* Continue */
    தேர்வு(ctx->sent) {
    நிலை STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      எனில்(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        நிறுத்து;
      }
      இல்லையெனில் {
        *min = STUB_GSS_SERVER_ERR;
        திரும்பு GSS_S_FAILURE;
      }
    நிலை STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      நிறுத்து;
    இயல்பு:
      *min = STUB_GSS_SERVER_ERR;
      திரும்பு GSS_S_FAILURE;
    }
  }
  இல்லையெனில் {
    எனில்(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      திரும்பு GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, அளவு(*ctx));
    எனில்(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      திரும்பு GSS_S_FAILURE;
    }

    எனில்(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    எனில்(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    எனில்(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      கட்டமைப்பு stub_gss_cred_id_t_desc *cred =
        (கட்டமைப்பு stub_gss_cred_id_t_desc *)initiator_cred_handle;
      எனில்(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      எனில்(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    எனில்(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    இல்லையெனில் எனில்(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    இல்லையெனில் {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      திரும்பு GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, அளவு(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  எனில்(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    திரும்பு GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    எனில்(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      திரும்பு GSS_S_FAILURE;
    }

    எனில்(strlen(creds) + target_desc.length + 5 >= அளவு(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      திரும்பு GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (முழு)target_desc.length,
                          (மாறிலி எழுத்து *)target_desc.மதிப்பு,
                          (முழு)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  எனில்(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    திரும்பு GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->மதிப்பு = token;
  output_token->length = length;

  திரும்பு GSS_S_CONTINUE_NEEDED;
}

நிலையான OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  கட்டமைப்பு stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (வெறுமை)output_token;

  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  எனில்(!context) {
    *min = STUB_GSS_INVALID_CTX;
    திரும்பு GSS_S_FAILURE;
  }
  எனில்(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    திரும்பு GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  திரும்பு GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
நிலையான gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

நிலையான OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  கட்டமைப்பு stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  முழு *locally_initiated,
  முழு *open_context)
{
  (வெறுமை)src_name;
  (வெறுமை)targ_name;
  (வெறுமை)lifetime_rec;
  (வெறுமை)ctx_flags;
  (வெறுமை)locally_initiated;
  (வெறுமை)open_context;

  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  எனில்(!context) {
    *min = STUB_GSS_INVALID_CTX;
    திரும்பு GSS_S_FAILURE;
  }

  *min = 0;
  எனில்(mech_type) {
    தேர்வு(context->sent) {
    நிலை STUB_GSS_NTLM1:
    நிலை STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      நிறுத்து;
    இயல்பு:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      நிறுத்து;
    }
  }

  திரும்பு GSS_S_COMPLETE;
}
நிலையான OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (வெறுமை)desired_name;
  (வெறுமை)time_req;
  (வெறுமை)desired_mechs;
  (வெறுமை)cred_usage;
  (வெறுமை)actual_mechs;
  (வெறுமை)time_rec;

  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  எனில்(output_cred_handle) {
    கட்டமைப்பு stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, அளவு(*cred));
    எனில்(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      திரும்பு GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  திரும்பு GSS_S_COMPLETE;
}

நிலையான OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  மாறிலி எழுத்து *creds;
  OM_uint32 major;

  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  எனில்(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    திரும்பு GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  எனில்(GSS_ERROR(major))
    திரும்பு major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  எனில்(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  திரும்பு GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
நிலையான OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        மாறிலி gss_OID_set mech_set)
{
  கட்டமைப்பு stub_gss_cred_id_t_desc *cred;
  size_t i;
  முழு found_krb5 = 0;
  முழு found_ntlm = 0;

  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  *min = 0;
  எனில்(cred_handle == GSS_C_NO_CREDENTIAL)
    திரும்பு GSS_S_FAILURE;

  cred = (கட்டமைப்பு stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  எனில்(mech_set) {
    க்கு(i = 0; i < mech_set->மொத்தம்; i++) {
      gss_OID oid = &mech_set->elements[i];
      எனில்(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      எனில்(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  திரும்பு GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

நிலையான OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  எனில்(!min)
    திரும்பு GSS_S_FAILURE;

  *min = 0;
  எனில்(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  திரும்பு GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(கட்டமைப்பு Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t உள்ளீடு_chan_bindings,
                                    gss_buffer_t உள்ளீடு_token,
                                    gss_buffer_t output_token,
                                    மாறிலி bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  எனில்(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  எனில்(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#இல்லையெனில்
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  எனில்(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (கட்டமைப்பு stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     உள்ளீடு_chan_bindings,
                                     உள்ளீடு_token,
                                     வெற்று, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     வெற்று /* time_rec */);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              உள்ளீடு_chan_bindings,
                              உள்ளீடு_token,
                              வெற்று, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              வெற்று /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_delete_sec_context(min,
                                     (கட்டமைப்பு stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_inquire_context(minor_status,
                             (கட்டமைப்பு stub_gss_ctx_id_t_desc *)context,
                             வெற்று, வெற்று, வெற்று, mech_type,
                             வெற்று, வெற்று, வெற்று);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_inquire_context(minor_status, context,
                             வெற்று, வெற்று, வெற்று, mech_type,
                             வெற்று, வெற்று, வெற்று);
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
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 மாறிலி gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  எனில்(getenv("CURL_STUB_GSS_CREDS"))
    திரும்பு stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  திரும்பு gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
நிலையான size_t display_gss_error(OM_uint32 status, முழு type,
                                எழுத்து *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 செய்தி_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  செய் {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &செய்தி_ctx,
                                  &status_string);
    எனில்(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      எனில்(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (முழு)status_string.length,
                              (எழுத்து *)status_string.மதிப்பு);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } போது(!GSS_ERROR(maj_stat) && செய்தி_ctx);

  திரும்பு len;
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
வெறுமை Curl_gss_log_error(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  எழுத்து buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  எனில்(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic எடு
#endif

#endif /* HAVE_GSSAPI */
