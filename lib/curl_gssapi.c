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

#جيڪڏهن_وصف_ٿيل HAVE_GSSAPI

#ضمن "curl_gssapi.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strcopy.h"

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#إذا defined(HAVE_GSSGNU) || !defined(_WIN32)
#وصف_ڪريو Curl_gss_alloc يادگيري_ڏيو  /* freed via the GSS API gss_release_buffer() */
#وصف_ڪريو Curl_gss_free  حرر    /* pair of the above */
#وصف_ڪريو CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#وإلاإذا defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ضمن <gssapi/gssapi_alloc.h>
#وصف_ڪريو Curl_gss_alloc gssalloc_malloc
#وصف_ڪريو Curl_gss_free  gssalloc_free
#وصف_ڪريو CURL_GSS_STUB
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* DEBUGBUILD */

#جيڪڏهن_وصف_ٿيل __GNUC__
#وصف_ڪريو CURL_ALIGN8  __attribute__((aligned(8)))
#وإلا
#وصف_ڪريو CURL_ALIGN8
#جيڪڏهن_پورو

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wdeprecated-declarations"
#جيڪڏهن_پورو

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
تعداد min_err_ڪوڊ {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_يادگيري,
  STUB_GSS_INVALID_دليلون,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_آخري
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
بنية stub_gss_ctx_id_t_desc {
  تعداد { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } موڪليل;
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
  gss_name_t هدف_name,
  ثابت gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 وقت_req,
  ثابت gss_channel_bindings_t داخلا_chan_bindings,
  gss_بفر_desc *داخلا_token,
  gss_OID *actual_mech_type,
  gss_بفر_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *وقت_rec)
{
  بنية stub_gss_ctx_id_t_desc *ctx = عدم;

  /* The token will be encoded in base64 */
  size_t ڊيگهائي = حجم(ctx->creds) * 3 / 4;
  size_t استعمال_ٿيل = 0;
  حرف *token = عدم;
  ثابت حرف *creds = عدم;

  (فراغ)mech_type;
  (فراغ)وقت_req;
  (فراغ)داخلا_chan_bindings;
  (فراغ)actual_mech_type;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;

  إذا(!context || !هدف_name || !output_token) {
    *min = STUB_GSS_INVALID_دليلون;
    أرجع GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  إذا(!creds || لڙي_ڊيگهه(creds) >= حجم(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  ctx = *context;
  إذا(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  output_token->ڊيگهائي = 0;
  output_token->قدر = عدم;

  إذا(داخلا_token && داخلا_token->ڊيگهائي) {
    إذا(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      أرجع GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    إذا(((حرف *)داخلا_token->قدر)[0] == 'D') {
      /*پورو*/
      بدل(ctx->موڪليل) {
      حالة STUB_GSS_KRB5:
      حالة STUB_GSS_NTLM3:
        إذا(ret_flags)
          *ret_flags = ctx->flags;
        إذا(وقت_rec)
          *وقت_rec = GSS_C_INDEFINITE;
        أرجع GSS_S_COMPLETE;
      افتراضي:
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    }

    إذا(((حرف *)داخلا_token->قدر)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      أرجع GSS_S_FAILURE;
    }

    /* Continue */
    بدل(ctx->موڪليل) {
    حالة STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      إذا(ctx->have_ntlm) {
        ctx->موڪليل = STUB_GSS_NTLM1;
        اكسر;
      }
      وإلا {
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    حالة STUB_GSS_NTLM1:
      ctx->موڪليل = STUB_GSS_NTLM3;
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
      *min = STUB_GSS_NO_يادگيري;
      أرجع GSS_S_FAILURE;
    }

    إذا(لڙي_ڳولا(creds, "KRB5"))
      ctx->have_krb5 = 1;

    إذا(لڙي_ڳولا(creds, "NTLM"))
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
      ctx->موڪليل = STUB_GSS_KRB5;
    وإلا إذا(ctx->have_ntlm)
      ctx->موڪليل = STUB_GSS_NTLM1;
    وإلا {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      أرجع GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, حجم(ctx->creds), creds, لڙي_ڊيگهه(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(ڊيگهائي);
  إذا(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_يادگيري;
    أرجع GSS_S_FAILURE;
  }

  {
    gss_بفر_desc هدف_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_حالت;
    OM_uint32 major_حالت;
    major_حالت = gss_display_name(&minor_حالت, هدف_name,
                                    &هدف_desc, &name_type);
    إذا(GSS_ERROR(major_حالت)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_يادگيري;
      أرجع GSS_S_FAILURE;
    }

    إذا(لڙي_ڊيگهه(creds) + هدف_desc.ڊيگهائي + 5 >= حجم(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_يادگيري;
      أرجع GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    استعمال_ٿيل = curl_msnprintf(token, ڊيگهائي, "%s:%.*s:%d:", creds,
                          (صحيح)هدف_desc.ڊيگهائي,
                          (ثابت حرف *)هدف_desc.قدر,
                          (صحيح)ctx->موڪليل);

    gss_release_بفر(&minor_حالت, &هدف_desc);
  }

  إذا(استعمال_ٿيل >= ڊيگهائي) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_يادگيري;
    أرجع GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + استعمال_ٿيل, 'A', ڊيگهائي - استعمال_ٿيل);

  *context = ctx;

  output_token->قدر = token;
  output_token->ڊيگهائي = ڊيگهائي;

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
    بدل(context->موڪليل) {
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
  OM_uint32 وقت_req,
  gss_OID_سيٽ desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_سيٽ *actual_mechs,
  OM_uint32 *وقت_rec)
{
  (فراغ)desired_name;
  (فراغ)وقت_req;
  (فراغ)desired_mechs;
  (فراغ)cred_usage;
  (فراغ)actual_mechs;
  (فراغ)وقت_rec;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  إذا(output_cred_handle) {
    بنية stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, حجم(*cred));
    إذا(!cred) {
      *min = STUB_GSS_NO_يادگيري;
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
  gss_OID_سيٽ *mech_سيٽ)
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

  major = gss_create_خالي_جايون_oid_سيٽ(min, mech_سيٽ);
  إذا(GSS_ERROR(major))
    أرجع major;

  /* Always include Kerberos */
  gss_جوڙيو_oid_سيٽ_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_سيٽ);

  /* Include NTLM if the stub creds contain NTLM */
  إذا(لڙي_ڳولا(creds, "NTLM"))
    gss_جوڙيو_oid_سيٽ_member(min, &stub_ntlmssp_oid, mech_سيٽ);

  أرجع GSS_S_COMPLETE;
}

#جيڪڏهن_وصف_ٿيل HAVE_GSS_سيٽ_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ساكن OM_uint32 stub_gss_سيٽ_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ثابت gss_OID_سيٽ mech_سيٽ)
{
  بنية stub_gss_cred_id_t_desc *cred;
  size_t i;
  صحيح مليو_krb5 = 0;
  صحيح مليو_ntlm = 0;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  إذا(cred_handle == GSS_C_NO_CREDENTIAL)
    أرجع GSS_S_FAILURE;

  cred = (بنية stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  إذا(mech_سيٽ) {
    لكل(i = 0; i < mech_سيٽ->ڳڻپ; i++) {
      gss_OID oid = &mech_سيٽ->elements[i];
      إذا(oid->ڊيگهائي == Curl_krb5_mech_oid.ڊيگهائي &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->ڊيگهائي))
        مليو_krb5 = 1;
      إذا(oid->ڊيگهائي == stub_ntlmssp_oid.ڊيگهائي &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->ڊيگهائي))
        مليو_ntlm = 1;
    }
  }

  cred->allow_krb5 = مليو_krb5;
  cred->allow_ntlm = مليو_ntlm;
  أرجع GSS_S_COMPLETE;
}
#جيڪڏهن_پورو /* HAVE_GSS_SET_NEG_MECHS */

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

#جيڪڏهن_پورو /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *معلومات,
                                    OM_uint32 *minor_حالت,
                                    gss_ctx_id_t *context,
                                    gss_name_t هدف_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t داخلا_chan_bindings,
                                    gss_بفر_t داخلا_token,
                                    gss_بفر_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  إذا(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  إذا(معلومات->سيٽ.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#جيڪڏهن_وصف_ٿيل GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#وإلا
    infof(معلومات, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#جيڪڏهن_پورو
  }

  إذا(معلومات->سيٽ.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_init_sec_context(minor_حالت,
                                     cred_handle,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     هدف_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     داخلا_chan_bindings,
                                     داخلا_token,
                                     عدم, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     عدم /* time_rec */);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_init_sec_context(minor_حالت,
                              cred_handle,
                              context,
                              هدف_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              داخلا_chan_bindings,
                              داخلا_token,
                              عدم, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              عدم /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_بفر_t output_token)
{
#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_delete_sec_context(min,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_حالت,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_inquire_context(minor_حالت,
                             (بنية stub_gss_ctx_id_t_desc *)context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_inquire_context(minor_حالت, context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_حالت,
                                gss_name_t desired_name,
                                OM_uint32 وقت_req,
                                gss_OID_سيٽ desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_سيٽ *actual_mechs,
                                OM_uint32 *وقت_rec)
{
#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_acquire_cred(minor_حالت, desired_name, وقت_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, وقت_rec);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_acquire_cred(minor_حالت, desired_name, وقت_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, وقت_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_حالت,
                                  gss_OID_سيٽ *mech_سيٽ)
{
#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_indicate_mechs(minor_حالت, mech_سيٽ);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_indicate_mechs(minor_حالت, mech_سيٽ);
}

#جيڪڏهن_وصف_ٿيل HAVE_GSS_سيٽ_NEG_MECHS
OM_uint32 Curl_gss_سيٽ_neg_mechs(OM_uint32 *minor_حالت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_سيٽ mech_سيٽ)
{
#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_سيٽ_neg_mechs(minor_حالت, cred_handle, mech_سيٽ);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_سيٽ_neg_mechs(minor_حالت, cred_handle, mech_سيٽ);
}
#جيڪڏهن_پورو /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_حالت,
                                gss_cred_id_t *cred_handle)
{
#جيڪڏهن_وصف_ٿيل CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_release_cred(minor_حالت, cred_handle);
#جيڪڏهن_پورو /* CURL_GSS_STUB */

  أرجع gss_release_cred(minor_حالت, cred_handle);
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#وصف_ڪريو GSS_LOG_بفر_LEN 1024
ساكن size_t display_gss_error(OM_uint32 حالت, صحيح type,
                                حرف *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 نياپو_ctx = 0;
  gss_بفر_desc حالت_string = GSS_C_خالي_جايون_بفر;

  افعل {
    maj_stat = gss_display_حالت(&min_stat,
                                  حالت,
                                  type,
                                  GSS_C_NO_OID,
                                  &نياپو_ctx,
                                  &حالت_string);
    إذا(maj_stat == GSS_S_COMPLETE && حالت_string.ڊيگهائي > 0) {
      إذا(GSS_LOG_بفر_LEN > len + حالت_string.ڊيگهائي + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_بفر_LEN - len,
                              "%.*s. ", (صحيح)حالت_string.ڊيگهائي,
                              (حرف *)حالت_string.قدر);
      }
    }
    gss_release_بفر(&min_stat, &حالت_string);
  } طالما(!GSS_ERROR(maj_stat) && نياپو_ctx);

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
فراغ Curl_gss_log_error(بنية Curl_easy *معلومات, ثابت حرف *اڳياڙي,
                        OM_uint32 major, OM_uint32 minor)
{
  حرف buf[GSS_LOG_بفر_LEN] = "";
  size_t len = 0;

  إذا(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_ڪوڊ, buf, len);

  display_gss_error(minor, GSS_C_MECH_ڪوڊ, buf, len);

  infof(معلومات, "%s%s", اڳياڙي, buf);
}
#جيڪڏهن_پورو /* CURLVERBOSE */

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HAVE_GSSAPI */
