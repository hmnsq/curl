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

#که_تعریف_شوی HAVE_GSSAPI

#ضمن "curl_gssapi.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strcopy.h"

#که_تعریف_شوی DEBUGBUILD
#إذا defined(HAVE_GSSGNU) || !defined(_WIN32)
#تعریف_کړه Curl_gss_alloc حافظه_ورکړه  /* freed via the GSS API gss_release_buffer() */
#تعریف_کړه Curl_gss_free  حرر    /* pair of the above */
#تعریف_کړه CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#وإلاإذا defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ضمن <gssapi/gssapi_alloc.h>
#تعریف_کړه Curl_gss_alloc gssalloc_malloc
#تعریف_کړه Curl_gss_free  gssalloc_free
#تعریف_کړه CURL_GSS_STUB
#د_که_پای
#د_که_پای /* DEBUGBUILD */

#که_تعریف_شوی __GNUC__
#تعریف_کړه CURL_ALIGN8  __attribute__((aligned(8)))
#وإلا
#تعریف_کړه CURL_ALIGN8
#د_که_پای

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wdeprecated-declarations"
#د_که_پای

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#که_تعریف_شوی CURL_GSS_STUB
تعداد min_err_کوډ {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_حافظه,
  STUB_GSS_INVALID_دلیلونه,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_وروستی
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
بنية stub_gss_ctx_id_t_desc {
  تعداد { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } لېږل_شوی;
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
  gss_name_t موخه_name,
  ثابت gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 وخت_req,
  ثابت gss_channel_bindings_t ننوت_chan_bindings,
  gss_بفر_desc *ننوت_token,
  gss_OID *actual_mech_type,
  gss_بفر_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *وخت_rec)
{
  بنية stub_gss_ctx_id_t_desc *ctx = عدم;

  /* The token will be encoded in base64 */
  size_t اوږدتیا = حجم(ctx->creds) * 3 / 4;
  size_t کارول_شوی = 0;
  حرف *token = عدم;
  ثابت حرف *creds = عدم;

  (فراغ)mech_type;
  (فراغ)وخت_req;
  (فراغ)ننوت_chan_bindings;
  (فراغ)actual_mech_type;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;

  إذا(!context || !موخه_name || !output_token) {
    *min = STUB_GSS_INVALID_دلیلونه;
    أرجع GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  إذا(!creds || مزي_اوږدوالی(creds) >= حجم(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  ctx = *context;
  إذا(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  output_token->اوږدتیا = 0;
  output_token->ارزښت = عدم;

  إذا(ننوت_token && ننوت_token->اوږدتیا) {
    إذا(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      أرجع GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    إذا(((حرف *)ننوت_token->ارزښت)[0] == 'D') {
      /*بشپړ*/
      بدل(ctx->لېږل_شوی) {
      حالة STUB_GSS_KRB5:
      حالة STUB_GSS_NTLM3:
        إذا(ret_flags)
          *ret_flags = ctx->flags;
        إذا(وخت_rec)
          *وخت_rec = GSS_C_INDEFINITE;
        أرجع GSS_S_COMPLETE;
      افتراضي:
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    }

    إذا(((حرف *)ننوت_token->ارزښت)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      أرجع GSS_S_FAILURE;
    }

    /* Continue */
    بدل(ctx->لېږل_شوی) {
    حالة STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      إذا(ctx->have_ntlm) {
        ctx->لېږل_شوی = STUB_GSS_NTLM1;
        اكسر;
      }
      وإلا {
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    حالة STUB_GSS_NTLM1:
      ctx->لېږل_شوی = STUB_GSS_NTLM3;
      اكسر;
    افتراضي:
      *min = STUB_GSS_SERVER_ERR;
      أرجع GSS_S_FAILURE;
    }
  }
  وإلا {
    إذا(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      أرجع GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, حجم(*ctx));
    إذا(!ctx) {
      *min = STUB_GSS_NO_حافظه;
      أرجع GSS_S_FAILURE;
    }

    إذا(مزي_پلټنه(creds, "KRB5"))
      ctx->have_krb5 = 1;

    إذا(مزي_پلټنه(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    إذا(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      بنية stub_gss_cred_id_t_desc *cred =
        (بنية stub_gss_cred_id_t_desc *)initiator_cred_handle;
      إذا(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      إذا(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    إذا(ctx->have_krb5)
      ctx->لېږل_شوی = STUB_GSS_KRB5;
    وإلا إذا(ctx->have_ntlm)
      ctx->لېږل_شوی = STUB_GSS_NTLM1;
    وإلا {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      أرجع GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, حجم(ctx->creds), creds, مزي_اوږدوالی(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(اوږدتیا);
  إذا(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_حافظه;
    أرجع GSS_S_FAILURE;
  }

  {
    gss_بفر_desc موخه_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_حالت;
    OM_uint32 major_حالت;
    major_حالت = gss_display_name(&minor_حالت, موخه_name,
                                    &موخه_desc, &name_type);
    إذا(GSS_ERROR(major_حالت)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_حافظه;
      أرجع GSS_S_FAILURE;
    }

    إذا(مزي_اوږدوالی(creds) + موخه_desc.اوږدتیا + 5 >= حجم(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_حافظه;
      أرجع GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    کارول_شوی = curl_msnprintf(token, اوږدتیا, "%s:%.*s:%d:", creds,
                          (صحيح)موخه_desc.اوږدتیا,
                          (ثابت حرف *)موخه_desc.ارزښت,
                          (صحيح)ctx->لېږل_شوی);

    gss_release_بفر(&minor_حالت, &موخه_desc);
  }

  إذا(کارول_شوی >= اوږدتیا) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_حافظه;
    أرجع GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + کارول_شوی, 'A', اوږدتیا - کارول_شوی);

  *context = ctx;

  output_token->ارزښت = token;
  output_token->اوږدتیا = اوږدتیا;

  أرجع GSS_S_CONTINUE_NEEDED;
}

ساكن OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  بنية stub_gss_ctx_id_t_desc **context,
  gss_بفر_t output_token)
{
  (فراغ)output_token;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  إذا(!context) {
    *min = STUB_GSS_INVALID_CTX;
    أرجع GSS_S_FAILURE;
  }
  إذا(!*context) {
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

  إذا(!min)
    أرجع GSS_S_FAILURE;

  إذا(!context) {
    *min = STUB_GSS_INVALID_CTX;
    أرجع GSS_S_FAILURE;
  }

  *min = 0;
  إذا(mech_type) {
    بدل(context->لېږل_شوی) {
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
  OM_uint32 وخت_req,
  gss_OID_سېټ desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_سېټ *actual_mechs,
  OM_uint32 *وخت_rec)
{
  (فراغ)desired_name;
  (فراغ)وخت_req;
  (فراغ)desired_mechs;
  (فراغ)cred_usage;
  (فراغ)actual_mechs;
  (فراغ)وخت_rec;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  إذا(output_cred_handle) {
    بنية stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, حجم(*cred));
    إذا(!cred) {
      *min = STUB_GSS_NO_حافظه;
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
  gss_OID_سېټ *mech_سېټ)
{
  ثابت حرف *creds;
  OM_uint32 major;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  إذا(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  major = gss_create_تشه_oid_سېټ(min, mech_سېټ);
  إذا(GSS_ERROR(major))
    أرجع major;

  /* Always include Kerberos */
  gss_جمع_کړه_oid_سېټ_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_سېټ);

  /* Include NTLM if the stub creds contain NTLM */
  إذا(مزي_پلټنه(creds, "NTLM"))
    gss_جمع_کړه_oid_سېټ_member(min, &stub_ntlmssp_oid, mech_سېټ);

  أرجع GSS_S_COMPLETE;
}

#که_تعریف_شوی HAVE_GSS_سېټ_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ساكن OM_uint32 stub_gss_سېټ_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ثابت gss_OID_سېټ mech_سېټ)
{
  بنية stub_gss_cred_id_t_desc *cred;
  size_t i;
  صحيح وموندل_krb5 = 0;
  صحيح وموندل_ntlm = 0;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  إذا(cred_handle == GSS_C_NO_CREDENTIAL)
    أرجع GSS_S_FAILURE;

  cred = (بنية stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  إذا(mech_سېټ) {
    لكل(i = 0; i < mech_سېټ->شمېر; i++) {
      gss_OID oid = &mech_سېټ->elements[i];
      إذا(oid->اوږدتیا == Curl_krb5_mech_oid.اوږدتیا &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->اوږدتیا))
        وموندل_krb5 = 1;
      إذا(oid->اوږدتیا == stub_ntlmssp_oid.اوږدتیا &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->اوږدتیا))
        وموندل_ntlm = 1;
    }
  }

  cred->allow_krb5 = وموندل_krb5;
  cred->allow_ntlm = وموندل_ntlm;
  أرجع GSS_S_COMPLETE;
}
#د_که_پای /* HAVE_GSS_SET_NEG_MECHS */

ساكن OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  إذا(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  أرجع GSS_S_COMPLETE;
}

#د_که_پای /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *معلومات,
                                    OM_uint32 *minor_حالت,
                                    gss_ctx_id_t *context,
                                    gss_name_t موخه_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ننوت_chan_bindings,
                                    gss_بفر_t ننوت_token,
                                    gss_بفر_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  إذا(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  إذا(معلومات->سېټ.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#که_تعریف_شوی GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#وإلا
    infof(معلومات, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#د_که_پای
  }

  إذا(معلومات->سېټ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_init_sec_context(minor_حالت,
                                     cred_handle,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     موخه_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     ننوت_chan_bindings,
                                     ننوت_token,
                                     عدم, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     عدم /* time_rec */);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_init_sec_context(minor_حالت,
                              cred_handle,
                              context,
                              موخه_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              ننوت_chan_bindings,
                              ننوت_token,
                              عدم, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              عدم /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_بفر_t output_token)
{
#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_delete_sec_context(min,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_حالت,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_inquire_context(minor_حالت,
                             (بنية stub_gss_ctx_id_t_desc *)context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_inquire_context(minor_حالت, context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_حالت,
                                gss_name_t desired_name,
                                OM_uint32 وخت_req,
                                gss_OID_سېټ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_سېټ *actual_mechs,
                                OM_uint32 *وخت_rec)
{
#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_acquire_cred(minor_حالت, desired_name, وخت_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, وخت_rec);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_acquire_cred(minor_حالت, desired_name, وخت_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, وخت_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_حالت,
                                  gss_OID_سېټ *mech_سېټ)
{
#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_indicate_mechs(minor_حالت, mech_سېټ);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_indicate_mechs(minor_حالت, mech_سېټ);
}

#که_تعریف_شوی HAVE_GSS_سېټ_NEG_MECHS
OM_uint32 Curl_gss_سېټ_neg_mechs(OM_uint32 *minor_حالت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_سېټ mech_سېټ)
{
#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_سېټ_neg_mechs(minor_حالت, cred_handle, mech_سېټ);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_سېټ_neg_mechs(minor_حالت, cred_handle, mech_سېټ);
}
#د_که_پای /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_حالت,
                                gss_cred_id_t *cred_handle)
{
#که_تعریف_شوی CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_release_cred(minor_حالت, cred_handle);
#د_که_پای /* CURL_GSS_STUB */

  أرجع gss_release_cred(minor_حالت, cred_handle);
}

#که_تعریف_شوی CURLVERBOSE
#تعریف_کړه GSS_LOG_بفر_LEN 1024
ساكن size_t display_gss_error(OM_uint32 حالت, صحيح type,
                                حرف *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 پیغام_ctx = 0;
  gss_بفر_desc حالت_string = GSS_C_تشه_بفر;

  افعل {
    maj_stat = gss_display_حالت(&min_stat,
                                  حالت,
                                  type,
                                  GSS_C_NO_OID,
                                  &پیغام_ctx,
                                  &حالت_string);
    إذا(maj_stat == GSS_S_COMPLETE && حالت_string.اوږدتیا > 0) {
      إذا(GSS_LOG_بفر_LEN > len + حالت_string.اوږدتیا + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_بفر_LEN - len,
                              "%.*s. ", (صحيح)حالت_string.اوږدتیا,
                              (حرف *)حالت_string.ارزښت);
      }
    }
    gss_release_بفر(&min_stat, &حالت_string);
  } طالما(!GSS_ERROR(maj_stat) && پیغام_ctx);

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
فراغ Curl_gss_log_error(بنية Curl_easy *معلومات, ثابت حرف *مختاړی,
                        OM_uint32 major, OM_uint32 minor)
{
  حرف buf[GSS_LOG_بفر_LEN] = "";
  size_t len = 0;

  إذا(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_کوډ, buf, len);

  display_gss_error(minor, GSS_C_MECH_کوډ, buf, len);

  infof(معلومات, "%s%s", مختاړی, buf);
}
#د_که_پای /* CURLVERBOSE */

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

#د_که_پای /* HAVE_GSSAPI */
