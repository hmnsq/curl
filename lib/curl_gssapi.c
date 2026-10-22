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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_GSSAPI

#അന്തര്ഭാവ "curl_gssapi.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

#ifdef DEBUGBUILD
#എങ്കിൽ defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  സ്വതന്ത്രം    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#അന്തര്ഭാവ <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#അല്ലെങ്കിൽ
#define CURL_ALIGN8
#endif

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
എണ്ണൽ min_err_code {
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
ഘടന stub_gss_ctx_id_t_desc {
  എണ്ണൽ { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  പൂർണ്ണസംഖ്യ have_krb5;
  പൂർണ്ണസംഖ്യ have_ntlm;
  OM_uint32 flags;
  അക്ഷരം creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ഘടന stub_gss_cred_id_t_desc {
  പൂർണ്ണസംഖ്യ allow_krb5;
  പൂർണ്ണസംഖ്യ allow_ntlm;
};

സ്ഥിതം OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ഘടന stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  സ്ഥിരം gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  സ്ഥിരം gss_channel_bindings_t നിവേശം_chan_bindings,
  gss_buffer_desc *നിവേശം_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  ഘടന stub_gss_ctx_id_t_desc *ctx = ശൂന്യ;

  /* The token will be encoded in base64 */
  size_t length = വലുപ്പം(ctx->creds) * 3 / 4;
  size_t used = 0;
  അക്ഷരം *token = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *creds = ശൂന്യ;

  (ശൂന്യം)mech_type;
  (ശൂന്യം)time_req;
  (ശൂന്യം)നിവേശം_chan_bindings;
  (ശൂന്യം)actual_mech_type;

  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  *min = 0;

  എങ്കിൽ(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    തിരികെ GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  എങ്കിൽ(!creds || strlen(creds) >= വലുപ്പം(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    തിരികെ GSS_S_FAILURE;
  }

  ctx = *context;
  എങ്കിൽ(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    തിരികെ GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->മൂല്യം = ശൂന്യ;

  എങ്കിൽ(നിവേശം_token && നിവേശം_token->length) {
    എങ്കിൽ(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      തിരികെ GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    എങ്കിൽ(((അക്ഷരം *)നിവേശം_token->മൂല്യം)[0] == 'D') {
      /* Done */
      തിരഞ്ഞെടുപ്പ്(ctx->sent) {
      അവസ്ഥ STUB_GSS_KRB5:
      അവസ്ഥ STUB_GSS_NTLM3:
        എങ്കിൽ(ret_flags)
          *ret_flags = ctx->flags;
        എങ്കിൽ(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        തിരികെ GSS_S_COMPLETE;
      സ്ഥിരസ്ഥിതി:
        *min = STUB_GSS_SERVER_ERR;
        തിരികെ GSS_S_FAILURE;
      }
    }

    എങ്കിൽ(((അക്ഷരം *)നിവേശം_token->മൂല്യം)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      തിരികെ GSS_S_FAILURE;
    }

    /* Continue */
    തിരഞ്ഞെടുപ്പ്(ctx->sent) {
    അവസ്ഥ STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      എങ്കിൽ(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        നിർത്തുക;
      }
      അല്ലെങ്കിൽ {
        *min = STUB_GSS_SERVER_ERR;
        തിരികെ GSS_S_FAILURE;
      }
    അവസ്ഥ STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      *min = STUB_GSS_SERVER_ERR;
      തിരികെ GSS_S_FAILURE;
    }
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      തിരികെ GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, വലുപ്പം(*ctx));
    എങ്കിൽ(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      തിരികെ GSS_S_FAILURE;
    }

    എങ്കിൽ(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    എങ്കിൽ(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    എങ്കിൽ(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ഘടന stub_gss_cred_id_t_desc *cred =
        (ഘടന stub_gss_cred_id_t_desc *)initiator_cred_handle;
      എങ്കിൽ(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      എങ്കിൽ(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    എങ്കിൽ(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    അല്ലെങ്കിൽ എങ്കിൽ(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    അല്ലെങ്കിൽ {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      തിരികെ GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, വലുപ്പം(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  എങ്കിൽ(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    തിരികെ GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    എങ്കിൽ(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      തിരികെ GSS_S_FAILURE;
    }

    എങ്കിൽ(strlen(creds) + target_desc.length + 5 >= വലുപ്പം(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      തിരികെ GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (പൂർണ്ണസംഖ്യ)target_desc.length,
                          (സ്ഥിരം അക്ഷരം *)target_desc.മൂല്യം,
                          (പൂർണ്ണസംഖ്യ)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  എങ്കിൽ(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    തിരികെ GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->മൂല്യം = token;
  output_token->length = length;

  തിരികെ GSS_S_CONTINUE_NEEDED;
}

സ്ഥിതം OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ഘടന stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (ശൂന്യം)output_token;

  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  എങ്കിൽ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    തിരികെ GSS_S_FAILURE;
  }
  എങ്കിൽ(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    തിരികെ GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  തിരികെ GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
സ്ഥിതം gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

സ്ഥിതം OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ഘടന stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  പൂർണ്ണസംഖ്യ *locally_initiated,
  പൂർണ്ണസംഖ്യ *open_context)
{
  (ശൂന്യം)src_name;
  (ശൂന്യം)targ_name;
  (ശൂന്യം)lifetime_rec;
  (ശൂന്യം)ctx_flags;
  (ശൂന്യം)locally_initiated;
  (ശൂന്യം)open_context;

  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  എങ്കിൽ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    തിരികെ GSS_S_FAILURE;
  }

  *min = 0;
  എങ്കിൽ(mech_type) {
    തിരഞ്ഞെടുപ്പ്(context->sent) {
    അവസ്ഥ STUB_GSS_NTLM1:
    അവസ്ഥ STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      നിർത്തുക;
    }
  }

  തിരികെ GSS_S_COMPLETE;
}
സ്ഥിതം OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (ശൂന്യം)desired_name;
  (ശൂന്യം)time_req;
  (ശൂന്യം)desired_mechs;
  (ശൂന്യം)cred_usage;
  (ശൂന്യം)actual_mechs;
  (ശൂന്യം)time_rec;

  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  എങ്കിൽ(output_cred_handle) {
    ഘടന stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, വലുപ്പം(*cred));
    എങ്കിൽ(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      തിരികെ GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  തിരികെ GSS_S_COMPLETE;
}

സ്ഥിതം OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  സ്ഥിരം അക്ഷരം *creds;
  OM_uint32 major;

  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  എങ്കിൽ(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    തിരികെ GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  എങ്കിൽ(GSS_ERROR(major))
    തിരികെ major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  എങ്കിൽ(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  തിരികെ GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
സ്ഥിതം OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        സ്ഥിരം gss_OID_set mech_set)
{
  ഘടന stub_gss_cred_id_t_desc *cred;
  size_t i;
  പൂർണ്ണസംഖ്യ found_krb5 = 0;
  പൂർണ്ണസംഖ്യ found_ntlm = 0;

  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  *min = 0;
  എങ്കിൽ(cred_handle == GSS_C_NO_CREDENTIAL)
    തിരികെ GSS_S_FAILURE;

  cred = (ഘടന stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  എങ്കിൽ(mech_set) {
    വേണ്ടി(i = 0; i < mech_set->എണ്ണം; i++) {
      gss_OID oid = &mech_set->elements[i];
      എങ്കിൽ(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      എങ്കിൽ(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  തിരികെ GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

സ്ഥിതം OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  എങ്കിൽ(!min)
    തിരികെ GSS_S_FAILURE;

  *min = 0;
  എങ്കിൽ(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  തിരികെ GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(ഘടന Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t നിവേശം_chan_bindings,
                                    gss_buffer_t നിവേശം_token,
                                    gss_buffer_t output_token,
                                    സ്ഥിരം bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  എങ്കിൽ(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  എങ്കിൽ(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#അല്ലെങ്കിൽ
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  എങ്കിൽ(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (ഘടന stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     നിവേശം_chan_bindings,
                                     നിവേശം_token,
                                     ശൂന്യ, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ശൂന്യ /* time_rec */);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              നിവേശം_chan_bindings,
                              നിവേശം_token,
                              ശൂന്യ, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ശൂന്യ /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_delete_sec_context(min,
                                     (ഘടന stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_inquire_context(minor_status,
                             (ഘടന stub_gss_ctx_id_t_desc *)context,
                             ശൂന്യ, ശൂന്യ, ശൂന്യ, mech_type,
                             ശൂന്യ, ശൂന്യ, ശൂന്യ);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_inquire_context(minor_status, context,
                             ശൂന്യ, ശൂന്യ, ശൂന്യ, mech_type,
                             ശൂന്യ, ശൂന്യ, ശൂന്യ);
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
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 സ്ഥിരം gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  എങ്കിൽ(getenv("CURL_STUB_GSS_CREDS"))
    തിരികെ stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  തിരികെ gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
സ്ഥിതം size_t display_gss_error(OM_uint32 status, പൂർണ്ണസംഖ്യ type,
                                അക്ഷരം *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 സന്ദേശം_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  ചെയ്യുക {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &സന്ദേശം_ctx,
                                  &status_string);
    എങ്കിൽ(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      എങ്കിൽ(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (പൂർണ്ണസംഖ്യ)status_string.length,
                              (അക്ഷരം *)status_string.മൂല്യം);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } ആകുമ്പോൾ(!GSS_ERROR(maj_stat) && സന്ദേശം_ctx);

  തിരികെ len;
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
ശൂന്യം Curl_gss_log_error(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  അക്ഷരം buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  എങ്കിൽ(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic എടുക്കുക
#endif

#endif /* HAVE_GSSAPI */
