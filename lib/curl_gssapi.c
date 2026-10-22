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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_GSSAPI

#अन्तर्भाव "curl_gssapi.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/strcopy.h"

#ifdef DEBUGBUILD
#यदि defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  मुक्त    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#नोचेद्यदि defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#अन्तर्भाव <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#अन्यथा
#define CURL_ALIGN8
#endif

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic धकेलो
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
गणना min_err_code {
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
संरचना stub_gss_ctx_id_t_desc {
  गणना { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  पूर्णाङ्क have_krb5;
  पूर्णाङ्क have_ntlm;
  OM_uint32 flags;
  वर्ण creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
संरचना stub_gss_cred_id_t_desc {
  पूर्णाङ्क allow_krb5;
  पूर्णाङ्क allow_ntlm;
};

स्थिर OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  संरचना stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  ध्रुव gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  ध्रुव gss_channel_bindings_t निवेश_chan_bindings,
  gss_buffer_desc *निवेश_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  संरचना stub_gss_ctx_id_t_desc *ctx = रिक्त;

  /* The token will be encoded in base64 */
  size_t length = परिमाण(ctx->creds) * 3 / 4;
  size_t used = 0;
  वर्ण *token = रिक्त;
  ध्रुव वर्ण *creds = रिक्त;

  (शून्य)mech_type;
  (शून्य)time_req;
  (शून्य)निवेश_chan_bindings;
  (शून्य)actual_mech_type;

  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  *min = 0;

  यदि(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    प्रत्यागम GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  यदि(!creds || strlen(creds) >= परिमाण(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    प्रत्यागम GSS_S_FAILURE;
  }

  ctx = *context;
  यदि(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    प्रत्यागम GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->मूल्य = रिक्त;

  यदि(निवेश_token && निवेश_token->length) {
    यदि(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      प्रत्यागम GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    यदि(((वर्ण *)निवेश_token->मूल्य)[0] == 'D') {
      /* Done */
      विकल्प(ctx->sent) {
      पक्ष STUB_GSS_KRB5:
      पक्ष STUB_GSS_NTLM3:
        यदि(ret_flags)
          *ret_flags = ctx->flags;
        यदि(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        प्रत्यागम GSS_S_COMPLETE;
      स्वभाव:
        *min = STUB_GSS_SERVER_ERR;
        प्रत्यागम GSS_S_FAILURE;
      }
    }

    यदि(((वर्ण *)निवेश_token->मूल्य)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      प्रत्यागम GSS_S_FAILURE;
    }

    /* Continue */
    विकल्प(ctx->sent) {
    पक्ष STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      यदि(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        भङ्ग;
      }
      अन्यथा {
        *min = STUB_GSS_SERVER_ERR;
        प्रत्यागम GSS_S_FAILURE;
      }
    पक्ष STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      भङ्ग;
    स्वभाव:
      *min = STUB_GSS_SERVER_ERR;
      प्रत्यागम GSS_S_FAILURE;
    }
  }
  अन्यथा {
    यदि(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      प्रत्यागम GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, परिमाण(*ctx));
    यदि(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      प्रत्यागम GSS_S_FAILURE;
    }

    यदि(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    यदि(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    यदि(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      संरचना stub_gss_cred_id_t_desc *cred =
        (संरचना stub_gss_cred_id_t_desc *)initiator_cred_handle;
      यदि(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      यदि(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    यदि(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    अन्यथा यदि(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    अन्यथा {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      प्रत्यागम GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, परिमाण(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  यदि(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    प्रत्यागम GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    यदि(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      प्रत्यागम GSS_S_FAILURE;
    }

    यदि(strlen(creds) + target_desc.length + 5 >= परिमाण(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      प्रत्यागम GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (पूर्णाङ्क)target_desc.length,
                          (ध्रुव वर्ण *)target_desc.मूल्य,
                          (पूर्णाङ्क)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  यदि(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    प्रत्यागम GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->मूल्य = token;
  output_token->length = length;

  प्रत्यागम GSS_S_CONTINUE_NEEDED;
}

स्थिर OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  संरचना stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (शून्य)output_token;

  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  यदि(!context) {
    *min = STUB_GSS_INVALID_CTX;
    प्रत्यागम GSS_S_FAILURE;
  }
  यदि(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    प्रत्यागम GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  प्रत्यागम GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
स्थिर gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

स्थिर OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  संरचना stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  पूर्णाङ्क *locally_initiated,
  पूर्णाङ्क *open_context)
{
  (शून्य)src_name;
  (शून्य)targ_name;
  (शून्य)lifetime_rec;
  (शून्य)ctx_flags;
  (शून्य)locally_initiated;
  (शून्य)open_context;

  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  यदि(!context) {
    *min = STUB_GSS_INVALID_CTX;
    प्रत्यागम GSS_S_FAILURE;
  }

  *min = 0;
  यदि(mech_type) {
    विकल्प(context->sent) {
    पक्ष STUB_GSS_NTLM1:
    पक्ष STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      भङ्ग;
    स्वभाव:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      भङ्ग;
    }
  }

  प्रत्यागम GSS_S_COMPLETE;
}
स्थिर OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (शून्य)desired_name;
  (शून्य)time_req;
  (शून्य)desired_mechs;
  (शून्य)cred_usage;
  (शून्य)actual_mechs;
  (शून्य)time_rec;

  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  यदि(output_cred_handle) {
    संरचना stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, परिमाण(*cred));
    यदि(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      प्रत्यागम GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  प्रत्यागम GSS_S_COMPLETE;
}

स्थिर OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  ध्रुव वर्ण *creds;
  OM_uint32 major;

  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  यदि(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    प्रत्यागम GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  यदि(GSS_ERROR(major))
    प्रत्यागम major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  यदि(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  प्रत्यागम GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
स्थिर OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ध्रुव gss_OID_set mech_set)
{
  संरचना stub_gss_cred_id_t_desc *cred;
  size_t i;
  पूर्णाङ्क found_krb5 = 0;
  पूर्णाङ्क found_ntlm = 0;

  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  *min = 0;
  यदि(cred_handle == GSS_C_NO_CREDENTIAL)
    प्रत्यागम GSS_S_FAILURE;

  cred = (संरचना stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  यदि(mech_set) {
    प्रति(i = 0; i < mech_set->गिनती; i++) {
      gss_OID oid = &mech_set->elements[i];
      यदि(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      यदि(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  प्रत्यागम GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

स्थिर OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  यदि(!min)
    प्रत्यागम GSS_S_FAILURE;

  *min = 0;
  यदि(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  प्रत्यागम GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(संरचना Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t निवेश_chan_bindings,
                                    gss_buffer_t निवेश_token,
                                    gss_buffer_t output_token,
                                    ध्रुव bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  यदि(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  यदि(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#अन्यथा
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  यदि(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (संरचना stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     निवेश_chan_bindings,
                                     निवेश_token,
                                     रिक्त, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     रिक्त /* time_rec */);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              निवेश_chan_bindings,
                              निवेश_token,
                              रिक्त, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              रिक्त /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_delete_sec_context(min,
                                     (संरचना stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_inquire_context(minor_status,
                             (संरचना stub_gss_ctx_id_t_desc *)context,
                             रिक्त, रिक्त, रिक्त, mech_type,
                             रिक्त, रिक्त, रिक्त);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_inquire_context(minor_status, context,
                             रिक्त, रिक्त, रिक्त, mech_type,
                             रिक्त, रिक्त, रिक्त);
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
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 ध्रुव gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  यदि(getenv("CURL_STUB_GSS_CREDS"))
    प्रत्यागम stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  प्रत्यागम gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
स्थिर size_t display_gss_error(OM_uint32 status, पूर्णाङ्क type,
                                वर्ण *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 संदेश_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  कुरु {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &संदेश_ctx,
                                  &status_string);
    यदि(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      यदि(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (पूर्णाङ्क)status_string.length,
                              (वर्ण *)status_string.मूल्य);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } यावत्(!GSS_ERROR(maj_stat) && संदेश_ctx);

  प्रत्यागम len;
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
शून्य Curl_gss_log_error(संरचना Curl_easy *data, ध्रुव वर्ण *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  वर्ण buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  यदि(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic निकालो
#endif

#endif /* HAVE_GSSAPI */
