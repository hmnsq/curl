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

#اگر_تعریف_شدہ HAVE_GSSAPI

#شامل "curl_gssapi.h"
#شامل "curl_trc.h"
#شامل "curlx/strcopy.h"

#اگر_تعریف_شدہ DEBUGBUILD
#اگر defined(HAVE_GSSGNU) || !defined(_WIN32)
#تعریف_کرو Curl_gss_alloc حافظہ_دو  /* freed via the GSS API gss_release_buffer() */
#تعریف_کرو Curl_gss_free  آزاد    /* pair of the above */
#تعریف_کرو CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#وگرنہ_اگر defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#شامل <gssapi/gssapi_alloc.h>
#تعریف_کرو Curl_gss_alloc gssalloc_malloc
#تعریف_کرو Curl_gss_free  gssalloc_free
#تعریف_کرو CURL_GSS_STUB
#اگر_ختم
#اگر_ختم /* DEBUGBUILD */

#اگر_تعریف_شدہ __GNUC__
#تعریف_کرو CURL_ALIGN8  __attribute__((aligned(8)))
#ورنہ
#تعریف_کرو CURL_ALIGN8
#اگر_ختم

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#اگر_تعریف_شدہ CURL_GSS_STUB
شمار min_err_رمز {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_حافظہ,
  STUB_GSS_INVALID_دلائل,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_آخری
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
ساخت stub_gss_ctx_id_t_desc {
  شمار { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } بھیجا;
  صحیح have_krb5;
  صحیح have_ntlm;
  OM_uint32 flags;
  حرف creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ساخت stub_gss_cred_id_t_desc {
  صحیح allow_krb5;
  صحیح allow_ntlm;
};

ساکن OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ساخت stub_gss_ctx_id_t_desc **context,
  gss_name_t ہدف_name,
  ثابت gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 وقت_req,
  ثابت gss_channel_bindings_t داخلہ_chan_bindings,
  gss_بفر_desc *داخلہ_token,
  gss_OID *actual_mech_type,
  gss_بفر_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *وقت_rec)
{
  ساخت stub_gss_ctx_id_t_desc *ctx = تہی;

  /* The token will be encoded in base64 */
  size_t لمبائی = حجم(ctx->creds) * 3 / 4;
  size_t مستعمل = 0;
  حرف *token = تہی;
  ثابت حرف *creds = تہی;

  (خالی)mech_type;
  (خالی)وقت_req;
  (خالی)داخلہ_chan_bindings;
  (خالی)actual_mech_type;

  اگر(!min)
    واپس GSS_S_FAILURE;

  *min = 0;

  اگر(!context || !ہدف_name || !output_token) {
    *min = STUB_GSS_INVALID_دلائل;
    واپس GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  اگر(!creds || لڑی_لمبائی(creds) >= حجم(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    واپس GSS_S_FAILURE;
  }

  ctx = *context;
  اگر(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    واپس GSS_S_FAILURE;
  }

  output_token->لمبائی = 0;
  output_token->قدر = تہی;

  اگر(داخلہ_token && داخلہ_token->لمبائی) {
    اگر(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      واپس GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    اگر(((حرف *)داخلہ_token->قدر)[0] == 'D') {
      /*مکمل*/
      انتخاب(ctx->بھیجا) {
      حالت STUB_GSS_KRB5:
      حالت STUB_GSS_NTLM3:
        اگر(ret_flags)
          *ret_flags = ctx->flags;
        اگر(وقت_rec)
          *وقت_rec = GSS_C_INDEFINITE;
        واپس GSS_S_COMPLETE;
      طےشدہ:
        *min = STUB_GSS_SERVER_ERR;
        واپس GSS_S_FAILURE;
      }
    }

    اگر(((حرف *)داخلہ_token->قدر)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      واپس GSS_S_FAILURE;
    }

    /* Continue */
    انتخاب(ctx->بھیجا) {
    حالت STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      اگر(ctx->have_ntlm) {
        ctx->بھیجا = STUB_GSS_NTLM1;
        توڑو;
      }
      ورنہ {
        *min = STUB_GSS_SERVER_ERR;
        واپس GSS_S_FAILURE;
      }
    حالت STUB_GSS_NTLM1:
      ctx->بھیجا = STUB_GSS_NTLM3;
      توڑو;
    طےشدہ:
      *min = STUB_GSS_SERVER_ERR;
      واپس GSS_S_FAILURE;
    }
  }
  ورنہ {
    اگر(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      واپس GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, حجم(*ctx));
    اگر(!ctx) {
      *min = STUB_GSS_NO_حافظہ;
      واپس GSS_S_FAILURE;
    }

    اگر(لڑی_تلاش(creds, "KRB5"))
      ctx->have_krb5 = 1;

    اگر(لڑی_تلاش(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    اگر(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ساخت stub_gss_cred_id_t_desc *cred =
        (ساخت stub_gss_cred_id_t_desc *)initiator_cred_handle;
      اگر(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      اگر(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    اگر(ctx->have_krb5)
      ctx->بھیجا = STUB_GSS_KRB5;
    ورنہ اگر(ctx->have_ntlm)
      ctx->بھیجا = STUB_GSS_NTLM1;
    ورنہ {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      واپس GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, حجم(ctx->creds), creds, لڑی_لمبائی(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(لمبائی);
  اگر(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_حافظہ;
    واپس GSS_S_FAILURE;
  }

  {
    gss_بفر_desc ہدف_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_حالت;
    OM_uint32 major_حالت;
    major_حالت = gss_display_name(&minor_حالت, ہدف_name,
                                    &ہدف_desc, &name_type);
    اگر(GSS_ERROR(major_حالت)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_حافظہ;
      واپس GSS_S_FAILURE;
    }

    اگر(لڑی_لمبائی(creds) + ہدف_desc.لمبائی + 5 >= حجم(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_حافظہ;
      واپس GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    مستعمل = curl_msnprintf(token, لمبائی, "%s:%.*s:%d:", creds,
                          (صحیح)ہدف_desc.لمبائی,
                          (ثابت حرف *)ہدف_desc.قدر,
                          (صحیح)ctx->بھیجا);

    gss_release_بفر(&minor_حالت, &ہدف_desc);
  }

  اگر(مستعمل >= لمبائی) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_حافظہ;
    واپس GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + مستعمل, 'A', لمبائی - مستعمل);

  *context = ctx;

  output_token->قدر = token;
  output_token->لمبائی = لمبائی;

  واپس GSS_S_CONTINUE_NEEDED;
}

ساکن OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ساخت stub_gss_ctx_id_t_desc **context,
  gss_بفر_t output_token)
{
  (خالی)output_token;

  اگر(!min)
    واپس GSS_S_FAILURE;

  اگر(!context) {
    *min = STUB_GSS_INVALID_CTX;
    واپس GSS_S_FAILURE;
  }
  اگر(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    واپس GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  واپس GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ساکن gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ساکن OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ساخت stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  صحیح *locally_initiated,
  صحیح *open_context)
{
  (خالی)src_name;
  (خالی)targ_name;
  (خالی)lifetime_rec;
  (خالی)ctx_flags;
  (خالی)locally_initiated;
  (خالی)open_context;

  اگر(!min)
    واپس GSS_S_FAILURE;

  اگر(!context) {
    *min = STUB_GSS_INVALID_CTX;
    واپس GSS_S_FAILURE;
  }

  *min = 0;
  اگر(mech_type) {
    انتخاب(context->بھیجا) {
    حالت STUB_GSS_NTLM1:
    حالت STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      توڑو;
    طےشدہ:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      توڑو;
    }
  }

  واپس GSS_S_COMPLETE;
}
ساکن OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 وقت_req,
  gss_OID_سیٹ desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_سیٹ *actual_mechs,
  OM_uint32 *وقت_rec)
{
  (خالی)desired_name;
  (خالی)وقت_req;
  (خالی)desired_mechs;
  (خالی)cred_usage;
  (خالی)actual_mechs;
  (خالی)وقت_rec;

  اگر(!min)
    واپس GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  اگر(output_cred_handle) {
    ساخت stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, حجم(*cred));
    اگر(!cred) {
      *min = STUB_GSS_NO_حافظہ;
      واپس GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  واپس GSS_S_COMPLETE;
}

ساکن OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_سیٹ *mech_سیٹ)
{
  ثابت حرف *creds;
  OM_uint32 major;

  اگر(!min)
    واپس GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  اگر(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    واپس GSS_S_FAILURE;
  }

  major = gss_create_خالی_خانے_oid_سیٹ(min, mech_سیٹ);
  اگر(GSS_ERROR(major))
    واپس major;

  /* Always include Kerberos */
  gss_جوڑو_oid_سیٹ_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_سیٹ);

  /* Include NTLM if the stub creds contain NTLM */
  اگر(لڑی_تلاش(creds, "NTLM"))
    gss_جوڑو_oid_سیٹ_member(min, &stub_ntlmssp_oid, mech_سیٹ);

  واپس GSS_S_COMPLETE;
}

#اگر_تعریف_شدہ HAVE_GSS_سیٹ_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ساکن OM_uint32 stub_gss_سیٹ_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ثابت gss_OID_سیٹ mech_سیٹ)
{
  ساخت stub_gss_cred_id_t_desc *cred;
  size_t i;
  صحیح ملا_krb5 = 0;
  صحیح ملا_ntlm = 0;

  اگر(!min)
    واپس GSS_S_FAILURE;

  *min = 0;
  اگر(cred_handle == GSS_C_NO_CREDENTIAL)
    واپس GSS_S_FAILURE;

  cred = (ساخت stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  اگر(mech_سیٹ) {
    برائے(i = 0; i < mech_سیٹ->گنتی; i++) {
      gss_OID oid = &mech_سیٹ->elements[i];
      اگر(oid->لمبائی == Curl_krb5_mech_oid.لمبائی &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->لمبائی))
        ملا_krb5 = 1;
      اگر(oid->لمبائی == stub_ntlmssp_oid.لمبائی &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->لمبائی))
        ملا_ntlm = 1;
    }
  }

  cred->allow_krb5 = ملا_krb5;
  cred->allow_ntlm = ملا_ntlm;
  واپس GSS_S_COMPLETE;
}
#اگر_ختم /* HAVE_GSS_SET_NEG_MECHS */

ساکن OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  اگر(!min)
    واپس GSS_S_FAILURE;

  *min = 0;
  اگر(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  واپس GSS_S_COMPLETE;
}

#اگر_ختم /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(ساخت Curl_easy *ڈیٹا,
                                    OM_uint32 *minor_حالت,
                                    gss_ctx_id_t *context,
                                    gss_name_t ہدف_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t داخلہ_chan_bindings,
                                    gss_بفر_t داخلہ_token,
                                    gss_بفر_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  اگر(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  اگر(ڈیٹا->سیٹ.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#اگر_تعریف_شدہ GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#ورنہ
    infof(ڈیٹا, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#اگر_ختم
  }

  اگر(ڈیٹا->سیٹ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_init_sec_context(minor_حالت,
                                     cred_handle,
                                     (ساخت stub_gss_ctx_id_t_desc **)context,
                                     ہدف_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     داخلہ_chan_bindings,
                                     داخلہ_token,
                                     تہی, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     تہی /* time_rec */);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_init_sec_context(minor_حالت,
                              cred_handle,
                              context,
                              ہدف_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              داخلہ_chan_bindings,
                              داخلہ_token,
                              تہی, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              تہی /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_بفر_t output_token)
{
#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_delete_sec_context(min,
                                     (ساخت stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_حالت,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_inquire_context(minor_حالت,
                             (ساخت stub_gss_ctx_id_t_desc *)context,
                             تہی, تہی, تہی, mech_type,
                             تہی, تہی, تہی);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_inquire_context(minor_حالت, context,
                             تہی, تہی, تہی, mech_type,
                             تہی, تہی, تہی);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_حالت,
                                gss_name_t desired_name,
                                OM_uint32 وقت_req,
                                gss_OID_سیٹ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_سیٹ *actual_mechs,
                                OM_uint32 *وقت_rec)
{
#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_acquire_cred(minor_حالت, desired_name, وقت_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, وقت_rec);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_acquire_cred(minor_حالت, desired_name, وقت_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, وقت_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_حالت,
                                  gss_OID_سیٹ *mech_سیٹ)
{
#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_indicate_mechs(minor_حالت, mech_سیٹ);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_indicate_mechs(minor_حالت, mech_سیٹ);
}

#اگر_تعریف_شدہ HAVE_GSS_سیٹ_NEG_MECHS
OM_uint32 Curl_gss_سیٹ_neg_mechs(OM_uint32 *minor_حالت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_سیٹ mech_سیٹ)
{
#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_سیٹ_neg_mechs(minor_حالت, cred_handle, mech_سیٹ);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_سیٹ_neg_mechs(minor_حالت, cred_handle, mech_سیٹ);
}
#اگر_ختم /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_حالت,
                                gss_cred_id_t *cred_handle)
{
#اگر_تعریف_شدہ CURL_GSS_STUB
  اگر(getenv("CURL_STUB_GSS_CREDS"))
    واپس stub_gss_release_cred(minor_حالت, cred_handle);
#اگر_ختم /* CURL_GSS_STUB */

  واپس gss_release_cred(minor_حالت, cred_handle);
}

#اگر_تعریف_شدہ CURLVERBOSE
#تعریف_کرو GSS_LOG_بفر_LEN 1024
ساکن size_t display_gss_error(OM_uint32 حالت_2, صحیح type,
                                حرف *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 پیغام_ctx = 0;
  gss_بفر_desc حالت_string = GSS_C_خالی_خانے_بفر;

  کرو {
    maj_stat = gss_display_حالت(&min_stat,
                                  حالت_2,
                                  type,
                                  GSS_C_NO_OID,
                                  &پیغام_ctx,
                                  &حالت_string);
    اگر(maj_stat == GSS_S_COMPLETE && حالت_string.لمبائی > 0) {
      اگر(GSS_LOG_بفر_LEN > len + حالت_string.لمبائی + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_بفر_LEN - len,
                              "%.*s. ", (صحیح)حالت_string.لمبائی,
                              (حرف *)حالت_string.قدر);
      }
    }
    gss_release_بفر(&min_stat, &حالت_string);
  } جب(!GSS_ERROR(maj_stat) && پیغام_ctx);

  واپس len;
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
خالی Curl_gss_log_error(ساخت Curl_easy *ڈیٹا, ثابت حرف *سابقہ,
                        OM_uint32 major, OM_uint32 minor)
{
  حرف buf[GSS_LOG_بفر_LEN] = "";
  size_t len = 0;

  اگر(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_رمز, buf, len);

  display_gss_error(minor, GSS_C_MECH_رمز, buf, len);

  infof(ڈیٹا, "%s%s", سابقہ, buf);
}
#اگر_ختم /* CURLVERBOSE */

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#ہدایت GCC diagnostic نکالو
#اگر_ختم

#اگر_ختم /* HAVE_GSSAPI */
