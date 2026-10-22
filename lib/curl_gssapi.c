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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_GSSAPI

#ಅನ್ತರ್ಭಾವ "curl_gssapi.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ifdef DEBUGBUILD
#ಯದಿ defined(HAVE_GSSGNU) || !defined(_WIN32)
#define Curl_gss_alloc malloc  /* freed via the GSS API gss_release_buffer() */
#define Curl_gss_free  ಮುಕ್ತ    /* pair of the above */
#define CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#ನೋಚೇದ್ಯದಿ defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#ಅನ್ತರ್ಭಾವ <gssapi/gssapi_alloc.h>
#define Curl_gss_alloc gssalloc_malloc
#define Curl_gss_free  gssalloc_free
#define CURL_GSS_STUB
#endif
#endif /* DEBUGBUILD */

#ifdef __GNUC__
#define CURL_ALIGN8  __attribute__((aligned(8)))
#ಅನ್ಯಥಾ
#define CURL_ALIGN8
#endif

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#ifdef CURL_GSS_STUB
ಗಣನಾ min_err_code {
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
ಸಂರಚನಾ stub_gss_ctx_id_t_desc {
  ಗಣನಾ { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } sent;
  ಪೂರ್ಣಾಙ್ಕ have_krb5;
  ಪೂರ್ಣಾಙ್ಕ have_ntlm;
  OM_uint32 flags;
  ವರ್ಣ creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
ಸಂರಚನಾ stub_gss_cred_id_t_desc {
  ಪೂರ್ಣಾಙ್ಕ allow_krb5;
  ಪೂರ್ಣಾಙ್ಕ allow_ntlm;
};

ಸ್ಥಿರ OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  ಸಂರಚನಾ stub_gss_ctx_id_t_desc **context,
  gss_name_t target_name,
  ಧ್ರುವ gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 time_req,
  ಧ್ರುವ gss_channel_bindings_t ಒಳಹರಿವು_chan_bindings,
  gss_buffer_desc *ಒಳಹರಿವು_token,
  gss_OID *actual_mech_type,
  gss_buffer_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *time_rec)
{
  ಸಂರಚನಾ stub_gss_ctx_id_t_desc *ctx = ರಿಕ್ತ;

  /* The token will be encoded in base64 */
  size_t length = ಪರಿಮಾಣ(ctx->creds) * 3 / 4;
  size_t used = 0;
  ವರ್ಣ *token = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *creds = ರಿಕ್ತ;

  (ಶೂನ್ಯ)mech_type;
  (ಶೂನ್ಯ)time_req;
  (ಶೂನ್ಯ)ಒಳಹರಿವು_chan_bindings;
  (ಶೂನ್ಯ)actual_mech_type;

  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  *min = 0;

  ಯದಿ(!context || !target_name || !output_token) {
    *min = STUB_GSS_INVALID_ARGS;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  ಯದಿ(!creds || strlen(creds) >= ಪರಿಮಾಣ(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  ctx = *context;
  ಯದಿ(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  output_token->length = 0;
  output_token->ಮೌಲ್ಯ = ರಿಕ್ತ;

  ಯದಿ(ಒಳಹರಿವು_token && ಒಳಹರಿವು_token->length) {
    ಯದಿ(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    ಯದಿ(((ವರ್ಣ *)ಒಳಹರಿವು_token->ಮೌಲ್ಯ)[0] == 'D') {
      /* Done */
      ವಿಕಲ್ಪ(ctx->sent) {
      ಪಕ್ಷ STUB_GSS_KRB5:
      ಪಕ್ಷ STUB_GSS_NTLM3:
        ಯದಿ(ret_flags)
          *ret_flags = ctx->flags;
        ಯದಿ(time_rec)
          *time_rec = GSS_C_INDEFINITE;
        ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
      ಸ್ವಭಾವ:
        *min = STUB_GSS_SERVER_ERR;
        ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
      }
    }

    ಯದಿ(((ವರ್ಣ *)ಒಳಹರಿವು_token->ಮೌಲ್ಯ)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    /* Continue */
    ವಿಕಲ್ಪ(ctx->sent) {
    ಪಕ್ಷ STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      ಯದಿ(ctx->have_ntlm) {
        ctx->sent = STUB_GSS_NTLM1;
        ಭಙ್ಗ;
      }
      ಅನ್ಯಥಾ {
        *min = STUB_GSS_SERVER_ERR;
        ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
      }
    ಪಕ್ಷ STUB_GSS_NTLM1:
      ctx->sent = STUB_GSS_NTLM3;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      *min = STUB_GSS_SERVER_ERR;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
    ಯದಿ(!ctx) {
      *min = STUB_GSS_NO_MEMORY;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    ಯದಿ(strstr(creds, "KRB5"))
      ctx->have_krb5 = 1;

    ಯದಿ(strstr(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    ಯದಿ(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      ಸಂರಚನಾ stub_gss_cred_id_t_desc *cred =
        (ಸಂರಚನಾ stub_gss_cred_id_t_desc *)initiator_cred_handle;
      ಯದಿ(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      ಯದಿ(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    ಯದಿ(ctx->have_krb5)
      ctx->sent = STUB_GSS_KRB5;
    ಅನ್ಯಥಾ ಯದಿ(ctx->have_ntlm)
      ctx->sent = STUB_GSS_NTLM1;
    ಅನ್ಯಥಾ {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, ಪರಿಮಾಣ(ctx->creds), creds, strlen(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(length);
  ಯದಿ(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  {
    gss_buffer_desc target_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_status;
    OM_uint32 major_status;
    major_status = gss_display_name(&minor_status, target_name,
                                    &target_desc, &name_type);
    ಯದಿ(GSS_ERROR(major_status)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    ಯದಿ(strlen(creds) + target_desc.length + 5 >= ಪರಿಮಾಣ(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_MEMORY;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    used = curl_msnprintf(token, length, "%s:%.*s:%d:", creds,
                          (ಪೂರ್ಣಾಙ್ಕ)target_desc.length,
                          (ಧ್ರುವ ವರ್ಣ *)target_desc.ಮೌಲ್ಯ,
                          (ಪೂರ್ಣಾಙ್ಕ)ctx->sent);

    gss_release_buffer(&minor_status, &target_desc);
  }

  ಯದಿ(used >= length) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_MEMORY;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + used, 'A', length - used);

  *context = ctx;

  output_token->ಮೌಲ್ಯ = token;
  output_token->length = length;

  ಪ್ರತ್ಯಾಗಮ GSS_S_CONTINUE_NEEDED;
}

ಸ್ಥಿರ OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  ಸಂರಚನಾ stub_gss_ctx_id_t_desc **context,
  gss_buffer_t output_token)
{
  (ಶೂನ್ಯ)output_token;

  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  ಯದಿ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }
  ಯದಿ(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
ಸ್ಥಿರ gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

ಸ್ಥಿರ OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  ಸಂರಚನಾ stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  ಪೂರ್ಣಾಙ್ಕ *locally_initiated,
  ಪೂರ್ಣಾಙ್ಕ *open_context)
{
  (ಶೂನ್ಯ)src_name;
  (ಶೂನ್ಯ)targ_name;
  (ಶೂನ್ಯ)lifetime_rec;
  (ಶೂನ್ಯ)ctx_flags;
  (ಶೂನ್ಯ)locally_initiated;
  (ಶೂನ್ಯ)open_context;

  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  ಯದಿ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  *min = 0;
  ಯದಿ(mech_type) {
    ವಿಕಲ್ಪ(context->sent) {
    ಪಕ್ಷ STUB_GSS_NTLM1:
    ಪಕ್ಷ STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      ಭಙ್ಗ;
    }
  }

  ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
}
ಸ್ಥಿರ OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 time_req,
  gss_OID_set desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_set *actual_mechs,
  OM_uint32 *time_rec)
{
  (ಶೂನ್ಯ)desired_name;
  (ಶೂನ್ಯ)time_req;
  (ಶೂನ್ಯ)desired_mechs;
  (ಶೂನ್ಯ)cred_usage;
  (ಶೂನ್ಯ)actual_mechs;
  (ಶೂನ್ಯ)time_rec;

  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  ಯದಿ(output_cred_handle) {
    ಸಂರಚನಾ stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, ಪರಿಮಾಣ(*cred));
    ಯದಿ(!cred) {
      *min = STUB_GSS_NO_MEMORY;
      ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
}

ಸ್ಥಿರ OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_set *mech_set)
{
  ಧ್ರುವ ವರ್ಣ *creds;
  OM_uint32 major;

  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  ಯದಿ(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;
  }

  major = gss_create_empty_oid_set(min, mech_set);
  ಯದಿ(GSS_ERROR(major))
    ಪ್ರತ್ಯಾಗಮ major;

  /* Always include Kerberos */
  gss_add_oid_set_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_set);

  /* Include NTLM if the stub creds contain NTLM */
  ಯದಿ(strstr(creds, "NTLM"))
    gss_add_oid_set_member(min, &stub_ntlmssp_oid, mech_set);

  ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
}

#ifdef HAVE_GSS_SET_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
ಸ್ಥಿರ OM_uint32 stub_gss_set_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        ಧ್ರುವ gss_OID_set mech_set)
{
  ಸಂರಚನಾ stub_gss_cred_id_t_desc *cred;
  size_t i;
  ಪೂರ್ಣಾಙ್ಕ found_krb5 = 0;
  ಪೂರ್ಣಾಙ್ಕ found_ntlm = 0;

  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  *min = 0;
  ಯದಿ(cred_handle == GSS_C_NO_CREDENTIAL)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  cred = (ಸಂರಚನಾ stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  ಯದಿ(mech_set) {
    ಪ್ರತಿ(i = 0; i < mech_set->ಎಣಿಕೆ; i++) {
      gss_OID oid = &mech_set->elements[i];
      ಯದಿ(oid->length == Curl_krb5_mech_oid.length &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->length))
        found_krb5 = 1;
      ಯದಿ(oid->length == stub_ntlmssp_oid.length &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->length))
        found_ntlm = 1;
    }
  }

  cred->allow_krb5 = found_krb5;
  cred->allow_ntlm = found_ntlm;
  ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

ಸ್ಥಿರ OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  ಯದಿ(!min)
    ಪ್ರತ್ಯಾಗಮ GSS_S_FAILURE;

  *min = 0;
  ಯದಿ(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  ಪ್ರತ್ಯಾಗಮ GSS_S_COMPLETE;
}

#endif /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(ಸಂರಚನಾ Curl_easy *data,
                                    OM_uint32 *minor_status,
                                    gss_ctx_id_t *context,
                                    gss_name_t target_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ಒಳಹರಿವು_chan_bindings,
                                    gss_buffer_t ಒಳಹರಿವು_token,
                                    gss_buffer_t output_token,
                                    ಧ್ರುವ bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  ಯದಿ(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  ಯದಿ(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#ifdef GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#ಅನ್ಯಥಾ
    infof(data, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#endif
  }

  ಯದಿ(data->set.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#ifdef CURL_GSS_STUB
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_init_sec_context(minor_status,
                                     cred_handle,
                                     (ಸಂರಚನಾ stub_gss_ctx_id_t_desc **)context,
                                     target_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     ಒಳಹರಿವು_chan_bindings,
                                     ಒಳಹರಿವು_token,
                                     ರಿಕ್ತ, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ರಿಕ್ತ /* time_rec */);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_init_sec_context(minor_status,
                              cred_handle,
                              context,
                              target_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              ಒಳಹರಿವು_chan_bindings,
                              ಒಳಹರಿವು_token,
                              ರಿಕ್ತ, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ರಿಕ್ತ /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_buffer_t output_token)
{
#ifdef CURL_GSS_STUB
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_delete_sec_context(min,
                                     (ಸಂರಚನಾ stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_status,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#ifdef CURL_GSS_STUB
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_inquire_context(minor_status,
                             (ಸಂರಚನಾ stub_gss_ctx_id_t_desc *)context,
                             ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, mech_type,
                             ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_inquire_context(minor_status, context,
                             ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, mech_type,
                             ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
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
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_acquire_cred(minor_status, desired_name, time_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, time_rec);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_acquire_cred(minor_status, desired_name, time_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, time_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_status,
                                  gss_OID_set *mech_set)
{
#ifdef CURL_GSS_STUB
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_indicate_mechs(minor_status, mech_set);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_indicate_mechs(minor_status, mech_set);
}

#ifdef HAVE_GSS_SET_NEG_MECHS
OM_uint32 Curl_gss_set_neg_mechs(OM_uint32 *minor_status,
                                 gss_cred_id_t cred_handle,
                                 ಧ್ರುವ gss_OID_set mech_set)
{
#ifdef CURL_GSS_STUB
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_set_neg_mechs(minor_status, cred_handle, mech_set);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_set_neg_mechs(minor_status, cred_handle, mech_set);
}
#endif /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_status,
                                gss_cred_id_t *cred_handle)
{
#ifdef CURL_GSS_STUB
  ಯದಿ(getenv("CURL_STUB_GSS_CREDS"))
    ಪ್ರತ್ಯಾಗಮ stub_gss_release_cred(minor_status, cred_handle);
#endif /* CURL_GSS_STUB */

  ಪ್ರತ್ಯಾಗಮ gss_release_cred(minor_status, cred_handle);
}

#ifdef CURLVERBOSE
#define GSS_LOG_BUFFER_LEN 1024
ಸ್ಥಿರ size_t display_gss_error(OM_uint32 status, ಪೂರ್ಣಾಙ್ಕ type,
                                ವರ್ಣ *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 ಸಂದೇಶ_ctx = 0;
  gss_buffer_desc status_string = GSS_C_EMPTY_BUFFER;

  ಕುರು {
    maj_stat = gss_display_status(&min_stat,
                                  status,
                                  type,
                                  GSS_C_NO_OID,
                                  &ಸಂದೇಶ_ctx,
                                  &status_string);
    ಯದಿ(maj_stat == GSS_S_COMPLETE && status_string.length > 0) {
      ಯದಿ(GSS_LOG_BUFFER_LEN > len + status_string.length + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_BUFFER_LEN - len,
                              "%.*s. ", (ಪೂರ್ಣಾಙ್ಕ)status_string.length,
                              (ವರ್ಣ *)status_string.ಮೌಲ್ಯ);
      }
    }
    gss_release_buffer(&min_stat, &status_string);
  } ಯಾವತ್(!GSS_ERROR(maj_stat) && ಸಂದೇಶ_ctx);

  ಪ್ರತ್ಯಾಗಮ len;
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
ಶೂನ್ಯ Curl_gss_log_error(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *prefix,
                        OM_uint32 major, OM_uint32 minor)
{
  ವರ್ಣ buf[GSS_LOG_BUFFER_LEN] = "";
  size_t len = 0;

  ಯದಿ(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_CODE, buf, len);

  display_gss_error(minor, GSS_C_MECH_CODE, buf, len);

  infof(data, "%s%s", prefix, buf);
}
#endif /* CURLVERBOSE */

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#pragma GCC diagnostic ತೆಗೆ
#endif

#endif /* HAVE_GSSAPI */
