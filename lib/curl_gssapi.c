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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_GSSAPI

#समावेश "curl_gssapi.h"
#समावेश "curl_trc.h"
#समावेश "curlx/strcopy.h"

#अगर_घोषित DEBUGBUILD
#अगर defined(HAVE_GSSGNU) || !defined(_WIN32)
#घोषणा Curl_gss_alloc दो_स्मृति  /* freed via the GSS API gss_release_buffer() */
#घोषणा Curl_gss_free  मुक्त    /* pair of the above */
#घोषणा CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#या_अगर defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#समावेश <gssapi/gssapi_alloc.h>
#घोषणा Curl_gss_alloc gssalloc_malloc
#घोषणा Curl_gss_free  gssalloc_free
#घोषणा CURL_GSS_STUB
#पूर्ण_अगर
#पूर्ण_अगर /* DEBUGBUILD */

#अगर_घोषित __GNUC__
#घोषणा CURL_ALIGN8  __attribute__((aligned(8)))
#अन्यथा
#घोषणा CURL_ALIGN8
#पूर्ण_अगर

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
#पूर्ण_अगर

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#अगर_घोषित CURL_GSS_STUB
क्रमागत min_err_code {
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
काष्ठा stub_gss_ctx_id_t_desc {
  क्रमागत { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  पूर्णांक have_krb5;
  पूर्णांक have_ntlm;
  OM_uint32 flags;
  अक्षर creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
काष्ठा stub_gss_cred_id_t_desc {
  पूर्णांक allow_krb5;
  पूर्णांक allow_ntlm;
};

अटल OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  काष्ठा stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  स्थिर gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  स्थिर gss_channel_bindings_t निवेश_chan_bindings,
  gss_बफर_desc *निवेश_token,
  gss_OID *actual_mech_type,
  gss_बफर_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  काष्ठा stub_gss_ctx_id_t_desc *ctx = शून्य;

  /* The token will be encoded in base64 */
  माप_प्रकार लंबाई = माप(ctx->creds) * 3 / 4;
  माप_प्रकार used = 0;
  अक्षर *token = शून्य;
  स्थिर अक्षर *creds = शून्य;

  (व्योम)mech_type;
  (व्योम)time_req;
  (व्योम)निवेश_chan_bindings;
  (व्योम)actual_mech_type;

  अगर(!min)
    वापस GSS_S_FAILURE;

  *min = 0;

  अगर(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    वापस GSS_S_FAILURE;
  }

  creds = दो_पर्या("CURL_STUB_GSS_CREDS");
  अगर(!creds || म_माप(creds) >= माप(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    वापस GSS_S_FAILURE;
  }

  ctx = *context;
  अगर(ctx && म_भेद(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    वापस GSS_S_FAILURE;
  }

  output_token->लंबाई = 0;
  output_token->मूल्य = शून्य;

  अगर(निवेश_token && निवेश_token->लंबाई) {
    अगर(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      वापस GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    अगर(((अक्षर *)निवेश_token->मूल्य)[0] == 'D') {
      /* Done */
      चयन(ctx->sent) {
      हाल STUB_GSS_KRB5:
      हाल STUB_GSS_NTLM3:
        अगर(ret_flags)
          *ret_flags = ctx->flags;
        अगर(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        वापस GSS_S_COMPLETE;
      शेष:
        *min = STUB_GSS_SERVER_ERR;
        वापस GSS_S_FAILURE;
      }
    }

    अगर(((अक्षर *)निवेश_token->मूल्य)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      वापस GSS_S_FAILURE;
    }

    /* Continue */
    चयन(ctx->sent) {
    हाल STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      अगर(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        अवरोध;
      }
      अन्यथा {
        *min = STUB_GSS_SERVER_ERR;
        वापस GSS_S_FAILURE;
      }
    हाल STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      अवरोध;
    शेष:
      *min = STUB_GSS_SERVER_ERR;
      वापस GSS_S_FAILURE;
    }
  }
  अन्यथा {
    अगर(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      वापस GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, माप(*ctx));
    अगर(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      वापस GSS_S_FAILURE;
    }

    अगर(म_माला(creds, "KRB5"))
      ctx->have_krb5 = 1;

    अगर(म_माला(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    अगर(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      काष्ठा stub_gss_cred_id_t_desc *cred =
        (काष्ठा stub_gss_cred_id_t_desc *)initiator_cred_handle;
      अगर(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      अगर(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    अगर(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    अन्यथा अगर(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    अन्यथा {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      वापस GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, माप(ctx->creds), creds, म_माप(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(लंबाई);
  अगर(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    वापस GSS_S_FAILURE;
  }

  {
    gss_बफर_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    अगर(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      वापस GSS_S_FAILURE;
    }

    अगर(म_माप(creds) + target_desc.लंबाई + 5 >= माप(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      वापस GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, लंबाई, "%s:%.*s:%d:", creds,
                          (पूर्णांक)target_desc.लंबाई,
                          (स्थिर अक्षर *)target_desc.मूल्य,
                          (पूर्णांक)ctx->sent);

    gss_release_बफर(&minor_status, &target_desc);
  }

  अगर(used >= लंबाई) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    वापस GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  स_रखो(token + used, 'A', लंबाई - used);

  *context = ctx;

  output_token->मूल्य = token;
  output_token->लंबाई = लंबाई;

  वापस GSS_S_CONTINUE_NEEDED;
}

अटल OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  काष्ठा stub_gss_ctx_id_t_desc **context,
  gss_बफर_t output_token)
{
  (व्योम)output_token;

  अगर(!min)
    वापस GSS_S_FAILURE;

  अगर(!context) {
    *min = STUB_GSS_INVALID_CTX;
    वापस GSS_S_FAILURE;
  }
  अगर(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    वापस GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  वापस GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
अटल gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

अटल OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  काष्ठा stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  पूर्णांक *locally_initiated,
  पूर्णांक *open_context)
{
  (व्योम)src_name;
  (व्योम)targ_name;
  (व्योम)lifetime_rec;
  (व्योम)ctx_flags;
  (व्योम)locally_initiated;
  (व्योम)open_context;

  अगर(!min)
    वापस GSS_S_FAILURE;

  अगर(!context) {
    *min = STUB_GSS_INVALID_CTX;
    वापस GSS_S_FAILURE;
  }

  *min = 0;
  अगर(mech_type) {
    चयन(context->sent) {
    हाल STUB_GSS_NTLM1:
    हाल STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      अवरोध;
    शेष:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      अवरोध;
    }
  }

  वापस GSS_S_COMPLETE;
}
अटल OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (व्योम)desired_name;
  (व्योम)time_req;
  (व्योम)desired_mechs;
  (व्योम)cred_usage;
  (व्योम)actual_mechs;
  (व्योम)time_rec;

  अगर(!min)
    वापस GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  अगर(output_cred_handle) {
    काष्ठा stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, माप(*cred));
    अगर(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      वापस GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  वापस GSS_S_COMPLETE;
}

अटल OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  स्थिर अक्षर *creds;
  OM_uint32 major;

  अगर(!min)
    वापस GSS_S_FAILURE;

  *min = 0;
  creds = दो_पर्या("CURL_STUB_GSS_CREDS");
  अगर(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    वापस GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  अगर(GSS_ERROR(major))
    वापस major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  अगर(म_माला(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  वापस GSS_S_COMPLETE;
}

#अगर_घोषित HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
अटल OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        स्थिर gss_OID_set mech_set)
{
  काष्ठा stub_gss_cred_id_t_desc *cred;
  माप_प्रकार i;
  पूर्णांक found_krb5 = 0;
  पूर्णांक found_ntlm = 0;

  अगर(!min)
    वापस GSS_S_FAILURE;

  *min = 0;
  अगर(cred_handle == GSS_C_NO_CREDENTIAL)
    वापस GSS_S_FAILURE;

  cred = (काष्ठा stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  अगर(mech_set) {
    क्रम(i = 0; i < mech_set->गिनती; i++) {
      gss_OID oid = &mech_set->elements[i];
      अगर(oid->लंबाई == Curl_krb5_mech_oid.लंबाई &&
         !स_भेद(oid->elements, Curl_krb5_mech_oid.elements, oid->लंबाई))
        found_krb5 = 1;
      अगर(oid->लंबाई == stub_ntlmssp_oid.लंबाई &&
         !स_भेद(oid->elements, stub_ntlmssp_oid.elements, oid->लंबाई))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  वापस GSS_S_COMPLETE;
}
#पूर्ण_अगर /* HAVE_GSS_SET_NEG_MECHS */

अटल OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  अगर(!min)
    वापस GSS_S_FAILURE;

  *min = 0;
  अगर(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  वापस GSS_S_COMPLETE;
}

#पूर्ण_अगर /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(काष्ठा Curl_easy *तथ्य,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t निवेश_chan_bindings,
                                    gss_बफर_t निवेश_token,
                                    gss_बफर_t output_token,
                                    स्थिर bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  अगर(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  अगर(तथ्य->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#अगर_घोषित GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#अन्यथा
    infof(तथ्य, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#पूर्ण_अगर
  }

  अगर(तथ्य->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (काष्ठा stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     निवेश_chan_bindings,
                                     निवेश_token,
                                     शून्य, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     शून्य /* time_rec */);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              निवेश_chan_bindings,
                              निवेश_token,
                              शून्य, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              शून्य /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_बफर_t output_token)
{
#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_delete_sec_context(min,
                                     (काष्ठा stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_inquire_context(minor_status,
                             (काष्ठा stub_gss_ctx_id_t_desc *)context,
                             शून्य, शून्य, शून्य, mech_type,
                             शून्य, शून्य, शून्य);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_inquire_context(minor_status, context,
                             शून्य, शून्य, शून्य, mech_type,
                             शून्य, शून्य, शून्य);
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
#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_indicate_mechs(minor_status, mech_set);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_indicate_mechs(minor_status, mech_set);
}

#अगर_घोषित HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 स्थिर gss_OID_set mech_set)
{
#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#पूर्ण_अगर /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#अगर_घोषित CURL_GSS_STUB
  अगर(दो_पर्या("CURL_STUB_GSS_CREDS"))
    वापस stub_gss_release_cred(minor_status, cred_handle);
#पूर्ण_अगर /* CURL_GSS_STUB */

  वापस gss_release_cred(minor_status, cred_handle);
}

#अगर_घोषित CURLVERBOSE
#घोषणा GSS_LOG_बफर_LEN 1024
अटल माप_प्रकार display_gss_error(OM_uint32 status, पूर्णांक type,
                                अक्षर *buf, माप_प्रकार len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 संदेश_ctx = 0;
  gss_बफर_desc status_string = GSS_C_EMPTY_बफर;

  करो {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &संदेश_ctx,
                                  &status_string);
    अगर(maj_stat == GSS_S_COMPLETE && status_string.लंबाई > 0) {
      अगर(GSS_LOG_बफर_LEN > len + status_string.लंबाई + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_बफर_LEN - len,
                              "%.*s. ", (पूर्णांक)status_string.लंबाई,
                              (अक्षर *)status_string.मूल्य);
      }
    }
    gss_release_बफर(&min_stat, &status_string);
  } जबतक(!GSS_ERROR(maj_stat) && संदेश_ctx);

  वापस len;
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
व्योम Curl_gss_log_error(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  अक्षर buf[GSS_LOG_बफर_LEN] = "";
  माप_प्रकार len = 0;

  अगर(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(तथ्य, "%s%s", prefix, buf);
}
#पूर्ण_अगर /* CURLVERBOSE */

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

#पूर्ण_अगर /* HAVE_GSSAPI */
