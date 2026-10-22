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

#اگر_تعریف_شده HAVE_GSSAPI

#ضمن "curl_gssapi.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strcopy.h"

#اگر_تعریف_شده DEBUGBUILD
#إذا defined(HAVE_GSSGNU) || !defined(_WIN32)
#تعریف_کن Curl_gss_alloc حافظه_بده  /* freed via the GSS API gss_release_buffer() */
#تعریف_کن Curl_gss_free  حرر    /* pair of the above */
#تعریف_کن CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#وإلاإذا defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ضمن <gssapi/gssapi_alloc.h>
#تعریف_کن Curl_gss_alloc gssalloc_malloc
#تعریف_کن Curl_gss_free  gssalloc_free
#تعریف_کن CURL_GSS_STUB
#پایان_اگر
#پایان_اگر /* DEBUGBUILD */

#اگر_تعریف_شده __GNUC__
#تعریف_کن CURL_ALIGN8  __attribute__((aligned(8)))
#وإلا
#تعریف_کن CURL_ALIGN8
#پایان_اگر

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wdeprecated-declarations"
#پایان_اگر

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#اگر_تعریف_شده CURL_GSS_STUB
تعداد min_err_کد {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_حافظه,
  STUB_GSS_INVALID_آرگومانها,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_آخرین
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
بنية stub_gss_ctx_id_t_desc {
  تعداد { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } فرستاده;
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
  OM_uint32 زمان_req,
  ثابت gss_channel_bindings_t درون_داد_chan_bindings,
  gss_میانگیر_desc *درون_داد_token,
  gss_OID *actual_mech_type,
  gss_میانگیر_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *زمان_rec)
{
  بنية stub_gss_ctx_id_t_desc *ctx = عدم;

  /* The token will be encoded in base64 */
  size_t درازی = حجم(ctx->creds) * 3 / 4;
  size_t استفاده_شده = 0;
  حرف *token = عدم;
  ثابت حرف *creds = عدم;

  (فراغ)mech_type;
  (فراغ)زمان_req;
  (فراغ)درون_داد_chan_bindings;
  (فراغ)actual_mech_type;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;

  إذا(!context || !هدف_name || !output_token) {
    *min = STUB_GSS_INVALID_آرگومانها;
    أرجع GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  إذا(!creds || طول_رشته(creds) >= حجم(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  ctx = *context;
  إذا(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    أرجع GSS_S_FAILURE;
  }

  output_token->درازی = 0;
  output_token->مقدار = عدم;

  إذا(درون_داد_token && درون_داد_token->درازی) {
    إذا(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      أرجع GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    إذا(((حرف *)درون_داد_token->مقدار)[0] == 'D') {
      /*انجامشده*/
      بدل(ctx->فرستاده) {
      حالة STUB_GSS_KRB5:
      حالة STUB_GSS_NTLM3:
        إذا(ret_flags)
          *ret_flags = ctx->flags;
        إذا(زمان_rec)
          *زمان_rec = GSS_C_INDEFINITE;
        أرجع GSS_S_COMPLETE;
      افتراضي:
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    }

    إذا(((حرف *)درون_داد_token->مقدار)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      أرجع GSS_S_FAILURE;
    }

    /* Continue */
    بدل(ctx->فرستاده) {
    حالة STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      إذا(ctx->have_ntlm) {
        ctx->فرستاده = STUB_GSS_NTLM1;
        اكسر;
      }
      وإلا {
        *min = STUB_GSS_SERVER_ERR;
        أرجع GSS_S_FAILURE;
      }
    حالة STUB_GSS_NTLM1:
      ctx->فرستاده = STUB_GSS_NTLM3;
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

    إذا(جستجوی_رشته(creds, "KRB5"))
      ctx->have_krb5 = 1;

    إذا(جستجوی_رشته(creds, "NTLM"))
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
      ctx->فرستاده = STUB_GSS_KRB5;
    وإلا إذا(ctx->have_ntlm)
      ctx->فرستاده = STUB_GSS_NTLM1;
    وإلا {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      أرجع GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, حجم(ctx->creds), creds, طول_رشته(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(درازی);
  إذا(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_حافظه;
    أرجع GSS_S_FAILURE;
  }

  {
    gss_میانگیر_desc هدف_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_وضعیت;
    OM_uint32 major_وضعیت;
    major_وضعیت = gss_display_name(&minor_وضعیت, هدف_name,
                                    &هدف_desc, &name_type);
    إذا(GSS_ERROR(major_وضعیت)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_حافظه;
      أرجع GSS_S_FAILURE;
    }

    إذا(طول_رشته(creds) + هدف_desc.درازی + 5 >= حجم(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_حافظه;
      أرجع GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    استفاده_شده = curl_msnprintf(token, درازی, "%s:%.*s:%d:", creds,
                          (صحيح)هدف_desc.درازی,
                          (ثابت حرف *)هدف_desc.مقدار,
                          (صحيح)ctx->فرستاده);

    gss_release_میانگیر(&minor_وضعیت, &هدف_desc);
  }

  إذا(استفاده_شده >= درازی) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_حافظه;
    أرجع GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + استفاده_شده, 'A', درازی - استفاده_شده);

  *context = ctx;

  output_token->مقدار = token;
  output_token->درازی = درازی;

  أرجع GSS_S_CONTINUE_NEEDED;
}

ساكن OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  بنية stub_gss_ctx_id_t_desc **context,
  gss_میانگیر_t output_token)
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
    بدل(context->فرستاده) {
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
  OM_uint32 زمان_req,
  gss_OID_مجموعه desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_مجموعه *actual_mechs,
  OM_uint32 *زمان_rec)
{
  (فراغ)desired_name;
  (فراغ)زمان_req;
  (فراغ)desired_mechs;
  (فراغ)cred_usage;
  (فراغ)actual_mechs;
  (فراغ)زمان_rec;

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
  gss_OID_مجموعه *mech_مجموعه)
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

  major = gss_create_خالی_oid_مجموعه(min, mech_مجموعه);
  إذا(GSS_ERROR(major))
    أرجع major;

  /* Always include Kerberos */
  gss_جمع_کن_oid_مجموعه_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_مجموعه);

  /* Include NTLM if the stub creds contain NTLM */
  إذا(جستجوی_رشته(creds, "NTLM"))
    gss_جمع_کن_oid_مجموعه_member(min, &stub_ntlmssp_oid, mech_مجموعه);

  أرجع GSS_S_COMPLETE;
}

#اگر_تعریف_شده HAVE_GSS_مجموعه_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ساكن OM_uint32 stub_gss_مجموعه_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ثابت gss_OID_مجموعه mech_مجموعه)
{
  بنية stub_gss_cred_id_t_desc *cred;
  size_t i;
  صحيح یافت_krb5 = 0;
  صحيح یافت_ntlm = 0;

  إذا(!min)
    أرجع GSS_S_FAILURE;

  *min = 0;
  إذا(cred_handle == GSS_C_NO_CREDENTIAL)
    أرجع GSS_S_FAILURE;

  cred = (بنية stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  إذا(mech_مجموعه) {
    لكل(i = 0; i < mech_مجموعه->شمار; i++) {
      gss_OID oid = &mech_مجموعه->elements[i];
      إذا(oid->درازی == Curl_krb5_mech_oid.درازی &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->درازی))
        یافت_krb5 = 1;
      إذا(oid->درازی == stub_ntlmssp_oid.درازی &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->درازی))
        یافت_ntlm = 1;
    }
  }

  cred->allow_krb5 = یافت_krb5;
  cred->allow_ntlm = یافت_ntlm;
  أرجع GSS_S_COMPLETE;
}
#پایان_اگر /* HAVE_GSS_SET_NEG_MECHS */

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

#پایان_اگر /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(بنية Curl_easy *داده,
                                    OM_uint32 *minor_وضعیت,
                                    gss_ctx_id_t *context,
                                    gss_name_t هدف_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t درون_داد_chan_bindings,
                                    gss_میانگیر_t درون_داد_token,
                                    gss_میانگیر_t output_token,
                                    ثابت bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  إذا(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  إذا(داده->مجموعه.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#اگر_تعریف_شده GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#وإلا
    infof(داده, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#پایان_اگر
  }

  إذا(داده->مجموعه.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_init_sec_context(minor_وضعیت,
                                     cred_handle,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     هدف_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     درون_داد_chan_bindings,
                                     درون_داد_token,
                                     عدم, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     عدم /* time_rec */);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_init_sec_context(minor_وضعیت,
                              cred_handle,
                              context,
                              هدف_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              درون_داد_chan_bindings,
                              درون_داد_token,
                              عدم, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              عدم /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_میانگیر_t output_token)
{
#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_delete_sec_context(min,
                                     (بنية stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_وضعیت,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_inquire_context(minor_وضعیت,
                             (بنية stub_gss_ctx_id_t_desc *)context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_inquire_context(minor_وضعیت, context,
                             عدم, عدم, عدم, mech_type,
                             عدم, عدم, عدم);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_وضعیت,
                                gss_name_t desired_name,
                                OM_uint32 زمان_req,
                                gss_OID_مجموعه desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_مجموعه *actual_mechs,
                                OM_uint32 *زمان_rec)
{
#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_acquire_cred(minor_وضعیت, desired_name, زمان_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, زمان_rec);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_acquire_cred(minor_وضعیت, desired_name, زمان_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, زمان_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_وضعیت,
                                  gss_OID_مجموعه *mech_مجموعه)
{
#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_indicate_mechs(minor_وضعیت, mech_مجموعه);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_indicate_mechs(minor_وضعیت, mech_مجموعه);
}

#اگر_تعریف_شده HAVE_GSS_مجموعه_NEG_MECHS
OM_uint32 Curl_gss_مجموعه_neg_mechs(OM_uint32 *minor_وضعیت,
                                 gss_cred_id_t cred_handle,
                                 ثابت gss_OID_مجموعه mech_مجموعه)
{
#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_مجموعه_neg_mechs(minor_وضعیت, cred_handle, mech_مجموعه);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_مجموعه_neg_mechs(minor_وضعیت, cred_handle, mech_مجموعه);
}
#پایان_اگر /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_وضعیت,
                                gss_cred_id_t *cred_handle)
{
#اگر_تعریف_شده CURL_GSS_STUB
  إذا(getenv("CURL_STUB_GSS_CREDS"))
    أرجع stub_gss_release_cred(minor_وضعیت, cred_handle);
#پایان_اگر /* CURL_GSS_STUB */

  أرجع gss_release_cred(minor_وضعیت, cred_handle);
}

#اگر_تعریف_شده CURLVERBOSE
#تعریف_کن GSS_LOG_میانگیر_LEN 1024
ساكن size_t display_gss_error(OM_uint32 وضعیت, صحيح type,
                                حرف *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 پیام_ctx = 0;
  gss_میانگیر_desc وضعیت_string = GSS_C_خالی_میانگیر;

  افعل {
    maj_stat = gss_display_وضعیت(&min_stat,
                                  وضعیت,
                                  type,
                                  GSS_C_NO_OID,
                                  &پیام_ctx,
                                  &وضعیت_string);
    إذا(maj_stat == GSS_S_COMPLETE && وضعیت_string.درازی > 0) {
      إذا(GSS_LOG_میانگیر_LEN > len + وضعیت_string.درازی + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_میانگیر_LEN - len,
                              "%.*s. ", (صحيح)وضعیت_string.درازی,
                              (حرف *)وضعیت_string.مقدار);
      }
    }
    gss_release_میانگیر(&min_stat, &وضعیت_string);
  } طالما(!GSS_ERROR(maj_stat) && پیام_ctx);

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
فراغ Curl_gss_log_error(بنية Curl_easy *داده, ثابت حرف *پیشوند,
                        OM_uint32 major, OM_uint32 minor)
{
  حرف buf[GSS_LOG_میانگیر_LEN] = "";
  size_t len = 0;

  إذا(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_کد, buf, len);

  display_gss_error(minor, GSS_C_MECH_کد, buf, len);

  infof(داده, "%s%s", پیشوند, buf);
}
#پایان_اگر /* CURLVERBOSE */

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#رهنمود GCC diagnostic برداشت
#پایان_اگر

#پایان_اگر /* HAVE_GSSAPI */
