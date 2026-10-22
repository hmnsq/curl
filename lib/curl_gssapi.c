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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_GSSAPI

#అన్తర్భావ "curl_gssapi.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/strcopy.h"

#నిర్వచితమైతే DEBUGBUILD
#ఒకవేళ defined(HAVE_GSSGNU) || !defined(_WIN32)
#నిర్వచించు Curl_gss_alloc జ్ఞాపకం_ఇవ్వు  /* freed via the GSS API gss_release_buffer() */
#నిర్వచించు Curl_gss_free  విడుదల    /* pair of the above */
#నిర్వచించు CURL_GSS_STUB
/* For correctness this would be required for all platforms, not only Windows,
   but, as of v1.22.1, MIT Kerberos uses a special allocator only for Windows,
   and the availability of 'gssapi/gssapi_alloc.h' is difficult to detect,
   because GSS headers are not versioned, and there is also no other macro to
   indicate 1.18+ vs. previous versions. On Windows we can use 'GSS_S_BAD_MIC'.
 */
#లేదావేళ defined(_WIN32) && defined(GSS_S_BAD_MIC) /* MIT Kerberos 1.15+ */
/* MIT Kerberos 1.10+ (Windows), 1.18+ (all platforms), missing from GNU GSS */
#అన్తర్భావ <gssapi/gssapi_alloc.h>
#నిర్వచించు Curl_gss_alloc gssalloc_malloc
#నిర్వచించు Curl_gss_free  gssalloc_free
#నిర్వచించు CURL_GSS_STUB
#అయితే_ముగింపు
#అయితే_ముగింపు /* DEBUGBUILD */

#నిర్వచితమైతే __GNUC__
#నిర్వచించు CURL_ALIGN8  __attribute__((aligned(8)))
#లేకపోతే
#నిర్వచించు CURL_ALIGN8
#అయితే_ముగింపు

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు

gss_OID_desc Curl_spnego_mech_oid CURL_ALIGN8 = {
  6, CURL_UNCONST("\x2b\x06\x01\x05\x05\x02")
};
gss_OID_desc Curl_krb5_mech_oid CURL_ALIGN8 = {
  9, CURL_UNCONST("\x2a\x86\x48\x86\xf7\x12\x01\x02\x02")
};

#నిర్వచితమైతే CURL_GSS_STUB
గణన min_err_సంకేతం {
  STUB_GSS_OK = 0,
  STUB_GSS_NO_జ్ఞాపకం,
  STUB_GSS_INVALID_వాదనలు,
  STUB_GSS_INVALID_CREDS,
  STUB_GSS_INVALID_CTX,
  STUB_GSS_SERVER_ERR,
  STUB_GSS_NO_MECH,
  STUB_GSS_చివరి
};

/* libcurl is also passing this struct to these functions, which are not yet
 * stubbed:
 *   gss_unwrap()
 *   gss_wrap()
 */
నిర్మాణం stub_gss_ctx_id_t_desc {
  గణన { STUB_GSS_NONE, STUB_GSS_KRB5, STUB_GSS_NTLM1, STUB_GSS_NTLM3 } పంపిన;
  పూర్ణాంకం have_krb5;
  పూర్ణాంకం have_ntlm;
  OM_uint32 flags;
  అక్షరం creds[250];
};

/* Stub credential: tracks which mechanisms are allowed */
నిర్మాణం stub_gss_cred_id_t_desc {
  పూర్ణాంకం allow_krb5;
  పూర్ణాంకం allow_ntlm;
};

స్థిర OM_uint32 stub_gss_init_sec_context(
  OM_uint32 *min,
  gss_cred_id_t initiator_cred_handle,
  నిర్మాణం stub_gss_ctx_id_t_desc **context,
  gss_name_t లక్ష్యం_name,
  స్థిరరాశి gss_OID mech_type,
  OM_uint32 req_flags,
  OM_uint32 సమయం_req,
  స్థిరరాశి gss_channel_bindings_t ఆదానం_chan_bindings,
  gss_బఫరు_desc *ఆదానం_token,
  gss_OID *actual_mech_type,
  gss_బఫరు_desc *output_token,
  OM_uint32 *ret_flags,
  OM_uint32 *సమయం_rec)
{
  నిర్మాణం stub_gss_ctx_id_t_desc *ctx = ఖాళీ;

  /* The token will be encoded in base64 */
  size_t నిడివి = పరిమాణం(ctx->creds) * 3 / 4;
  size_t వాడిన = 0;
  అక్షరం *token = ఖాళీ;
  స్థిరరాశి అక్షరం *creds = ఖాళీ;

  (శూన్య)mech_type;
  (శూన్య)సమయం_req;
  (శూన్య)ఆదానం_chan_bindings;
  (శూన్య)actual_mech_type;

  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  *min = 0;

  ఒకవేళ(!context || !లక్ష్యం_name || !output_token) {
    *min = STUB_GSS_INVALID_వాదనలు;
    తిరిగి GSS_S_FAILURE;
  }

  creds = getenv("CURL_STUB_GSS_CREDS");
  ఒకవేళ(!creds || అక్షరశ్రేణి_పొడవు(creds) >= పరిమాణం(ctx->creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    తిరిగి GSS_S_FAILURE;
  }

  ctx = *context;
  ఒకవేళ(ctx && strcmp(ctx->creds, creds)) {
    *min = STUB_GSS_INVALID_CREDS;
    తిరిగి GSS_S_FAILURE;
  }

  output_token->నిడివి = 0;
  output_token->విలువ = ఖాళీ;

  ఒకవేళ(ఆదానం_token && ఆదానం_token->నిడివి) {
    ఒకవేళ(!ctx) {
      *min = STUB_GSS_INVALID_CTX;
      తిరిగి GSS_S_FAILURE;
    }

    /* Server response, either D (RA==) or C (Qw==) */
    ఒకవేళ(((అక్షరం *)ఆదానం_token->విలువ)[0] == 'D') {
      /*పూర్తి*/
      ఎంపిక(ctx->పంపిన) {
      స్థితి STUB_GSS_KRB5:
      స్థితి STUB_GSS_NTLM3:
        ఒకవేళ(ret_flags)
          *ret_flags = ctx->flags;
        ఒకవేళ(సమయం_rec)
          *సమయం_rec = GSS_C_INDEFINITE;
        తిరిగి GSS_S_COMPLETE;
      అప్రమేయ:
        *min = STUB_GSS_SERVER_ERR;
        తిరిగి GSS_S_FAILURE;
      }
    }

    ఒకవేళ(((అక్షరం *)ఆదానం_token->విలువ)[0] != 'C') {
      /* We only support Done or Continue */
      *min = STUB_GSS_SERVER_ERR;
      తిరిగి GSS_S_FAILURE;
    }

    /* Continue */
    ఎంపిక(ctx->పంపిన) {
    స్థితి STUB_GSS_KRB5:
      /* We sent KRB5 and it failed, let's try NTLM */
      ఒకవేళ(ctx->have_ntlm) {
        ctx->పంపిన = STUB_GSS_NTLM1;
        ఆపు;
      }
      లేకపోతే {
        *min = STUB_GSS_SERVER_ERR;
        తిరిగి GSS_S_FAILURE;
      }
    స్థితి STUB_GSS_NTLM1:
      ctx->పంపిన = STUB_GSS_NTLM3;
      ఆపు;
    అప్రమేయ:
      *min = STUB_GSS_SERVER_ERR;
      తిరిగి GSS_S_FAILURE;
    }
  }
  లేకపోతే {
    ఒకవేళ(ctx) {
      *min = STUB_GSS_INVALID_CTX;
      తిరిగి GSS_S_FAILURE;
    }

    ctx = curlx_calloc(1, పరిమాణం(*ctx));
    ఒకవేళ(!ctx) {
      *min = STUB_GSS_NO_జ్ఞాపకం;
      తిరిగి GSS_S_FAILURE;
    }

    ఒకవేళ(అక్షరశ్రేణి_శోధన(creds, "KRB5"))
      ctx->have_krb5 = 1;

    ఒకవేళ(అక్షరశ్రేణి_శోధన(creds, "NTLM"))
      ctx->have_ntlm = 1;

    /* If a credential restricts allowed mechs, honour it */
    ఒకవేళ(initiator_cred_handle != GSS_C_NO_CREDENTIAL) {
      నిర్మాణం stub_gss_cred_id_t_desc *cred =
        (నిర్మాణం stub_gss_cred_id_t_desc *)initiator_cred_handle;
      ఒకవేళ(!cred->allow_krb5)
        ctx->have_krb5 = 0;
      ఒకవేళ(!cred->allow_ntlm)
        ctx->have_ntlm = 0;
    }

    ఒకవేళ(ctx->have_krb5)
      ctx->పంపిన = STUB_GSS_KRB5;
    లేకపోతే ఒకవేళ(ctx->have_ntlm)
      ctx->పంపిన = STUB_GSS_NTLM1;
    లేకపోతే {
      curlx_free(ctx);
      *min = STUB_GSS_NO_MECH;
      తిరిగి GSS_S_FAILURE;
    }

    curlx_strcopy(ctx->creds, పరిమాణం(ctx->creds), creds, అక్షరశ్రేణి_పొడవు(creds));
    ctx->flags = req_flags;
  }

  token = Curl_gss_alloc(నిడివి);
  ఒకవేళ(!token) {
    curlx_free(ctx);
    *min = STUB_GSS_NO_జ్ఞాపకం;
    తిరిగి GSS_S_FAILURE;
  }

  {
    gss_బఫరు_desc లక్ష్యం_desc;
    gss_OID name_type = GSS_C_NO_OID;
    OM_uint32 minor_స్థితి;
    OM_uint32 major_స్థితి;
    major_స్థితి = gss_display_name(&minor_స్థితి, లక్ష్యం_name,
                                    &లక్ష్యం_desc, &name_type);
    ఒకవేళ(GSS_ERROR(major_స్థితి)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_జ్ఞాపకం;
      తిరిగి GSS_S_FAILURE;
    }

    ఒకవేళ(అక్షరశ్రేణి_పొడవు(creds) + లక్ష్యం_desc.నిడివి + 5 >= పరిమాణం(ctx->creds)) {
      Curl_gss_free(token);
      curlx_free(ctx);
      *min = STUB_GSS_NO_జ్ఞాపకం;
      తిరిగి GSS_S_FAILURE;
    }

    /* Token format: creds:target:type:padding */
    వాడిన = curl_msnprintf(token, నిడివి, "%s:%.*s:%d:", creds,
                          (పూర్ణాంకం)లక్ష్యం_desc.నిడివి,
                          (స్థిరరాశి అక్షరం *)లక్ష్యం_desc.విలువ,
                          (పూర్ణాంకం)ctx->పంపిన);

    gss_release_బఫరు(&minor_స్థితి, &లక్ష్యం_desc);
  }

  ఒకవేళ(వాడిన >= నిడివి) {
    Curl_gss_free(token);
    curlx_free(ctx);
    *min = STUB_GSS_NO_జ్ఞాపకం;
    తిరిగి GSS_S_FAILURE;
  }

  /* Overwrite null-terminator */
  memset(token + వాడిన, 'A', నిడివి - వాడిన);

  *context = ctx;

  output_token->విలువ = token;
  output_token->నిడివి = నిడివి;

  తిరిగి GSS_S_CONTINUE_NEEDED;
}

స్థిర OM_uint32 stub_gss_delete_sec_context(
  OM_uint32 *min,
  నిర్మాణం stub_gss_ctx_id_t_desc **context,
  gss_బఫరు_t output_token)
{
  (శూన్య)output_token;

  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  ఒకవేళ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    తిరిగి GSS_S_FAILURE;
  }
  ఒకవేళ(!*context) {
    *min = STUB_GSS_INVALID_CTX;
    తిరిగి GSS_S_FAILURE;
  }

  curlx_safefree(*context);
  *min = 0;

  తిరిగి GSS_S_COMPLETE;
}

/* NTLMSSP OID: 1.3.6.1.4.1.311.2.2.10 */
స్థిర gss_OID_desc stub_ntlmssp_oid = {
  10, CURL_UNCONST("\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a")
};

స్థిర OM_uint32 stub_gss_inquire_context(
  OM_uint32 *min,
  నిర్మాణం stub_gss_ctx_id_t_desc *context,
  gss_name_t *src_name,
  gss_name_t *targ_name,
  OM_uint32 *lifetime_rec,
  gss_OID *mech_type,
  OM_uint32 *ctx_flags,
  పూర్ణాంకం *locally_initiated,
  పూర్ణాంకం *open_context)
{
  (శూన్య)src_name;
  (శూన్య)targ_name;
  (శూన్య)lifetime_rec;
  (శూన్య)ctx_flags;
  (శూన్య)locally_initiated;
  (శూన్య)open_context;

  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  ఒకవేళ(!context) {
    *min = STUB_GSS_INVALID_CTX;
    తిరిగి GSS_S_FAILURE;
  }

  *min = 0;
  ఒకవేళ(mech_type) {
    ఎంపిక(context->పంపిన) {
    స్థితి STUB_GSS_NTLM1:
    స్థితి STUB_GSS_NTLM3:
      *mech_type = &stub_ntlmssp_oid;
      ఆపు;
    అప్రమేయ:
      *mech_type = (gss_OID)&Curl_krb5_mech_oid;
      ఆపు;
    }
  }

  తిరిగి GSS_S_COMPLETE;
}
స్థిర OM_uint32 stub_gss_acquire_cred(
  OM_uint32 *min,
  gss_name_t desired_name,
  OM_uint32 సమయం_req,
  gss_OID_సమితి desired_mechs,
  gss_cred_usage_t cred_usage,
  gss_cred_id_t *output_cred_handle,
  gss_OID_సమితి *actual_mechs,
  OM_uint32 *సమయం_rec)
{
  (శూన్య)desired_name;
  (శూన్య)సమయం_req;
  (శూన్య)desired_mechs;
  (శూన్య)cred_usage;
  (శూన్య)actual_mechs;
  (శూన్య)సమయం_rec;

  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  *min = 0;
  /* Allocate a stub credential that initially allows all mechanisms */
  ఒకవేళ(output_cred_handle) {
    నిర్మాణం stub_gss_cred_id_t_desc *cred =
      curlx_calloc(1, పరిమాణం(*cred));
    ఒకవేళ(!cred) {
      *min = STUB_GSS_NO_జ్ఞాపకం;
      తిరిగి GSS_S_FAILURE;
    }
    cred->allow_krb5 = 1;
    cred->allow_ntlm = 1;
    *output_cred_handle = (gss_cred_id_t)cred;
  }
  తిరిగి GSS_S_COMPLETE;
}

స్థిర OM_uint32 stub_gss_indicate_mechs(
  OM_uint32 *min,
  gss_OID_సమితి *mech_సమితి)
{
  స్థిరరాశి అక్షరం *creds;
  OM_uint32 major;

  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  *min = 0;
  creds = getenv("CURL_STUB_GSS_CREDS");
  ఒకవేళ(!creds) {
    *min = STUB_GSS_INVALID_CREDS;
    తిరిగి GSS_S_FAILURE;
  }

  major = gss_create_ఖాళీలు_oid_సమితి(min, mech_సమితి);
  ఒకవేళ(GSS_ERROR(major))
    తిరిగి major;

  /* Always include Kerberos */
  gss_కూడు_oid_సమితి_member(min, (gss_OID)&Curl_krb5_mech_oid, mech_సమితి);

  /* Include NTLM if the stub creds contain NTLM */
  ఒకవేళ(అక్షరశ్రేణి_శోధన(creds, "NTLM"))
    gss_కూడు_oid_సమితి_member(min, &stub_ntlmssp_oid, mech_సమితి);

  తిరిగి GSS_S_COMPLETE;
}

#నిర్వచితమైతే HAVE_GSS_సమితి_NEG_MECHS  /* MIT Kerberos 1.8+ (2010-03-02),
                                  missing from Apple GSS, GNU GSS */
స్థిర OM_uint32 stub_gss_సమితి_neg_mechs(OM_uint32 *min,
                                        gss_cred_id_t cred_handle,
                                        స్థిరరాశి gss_OID_సమితి mech_సమితి)
{
  నిర్మాణం stub_gss_cred_id_t_desc *cred;
  size_t i;
  పూర్ణాంకం దొరికింది_krb5 = 0;
  పూర్ణాంకం దొరికింది_ntlm = 0;

  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  *min = 0;
  ఒకవేళ(cred_handle == GSS_C_NO_CREDENTIAL)
    తిరిగి GSS_S_FAILURE;

  cred = (నిర్మాణం stub_gss_cred_id_t_desc *)cred_handle;

  /* Determine which mechs are in the allowed set */
  ఒకవేళ(mech_సమితి) {
    కొరకు(i = 0; i < mech_సమితి->లెక్క; i++) {
      gss_OID oid = &mech_సమితి->elements[i];
      ఒకవేళ(oid->నిడివి == Curl_krb5_mech_oid.నిడివి &&
         !memcmp(oid->elements, Curl_krb5_mech_oid.elements, oid->నిడివి))
        దొరికింది_krb5 = 1;
      ఒకవేళ(oid->నిడివి == stub_ntlmssp_oid.నిడివి &&
         !memcmp(oid->elements, stub_ntlmssp_oid.elements, oid->నిడివి))
        దొరికింది_ntlm = 1;
    }
  }

  cred->allow_krb5 = దొరికింది_krb5;
  cred->allow_ntlm = దొరికింది_ntlm;
  తిరిగి GSS_S_COMPLETE;
}
#అయితే_ముగింపు /* HAVE_GSS_SET_NEG_MECHS */

స్థిర OM_uint32 stub_gss_release_cred(
  OM_uint32 *min,
  gss_cred_id_t *cred_handle)
{
  ఒకవేళ(!min)
    తిరిగి GSS_S_FAILURE;

  *min = 0;
  ఒకవేళ(cred_handle && *cred_handle != GSS_C_NO_CREDENTIAL) {
    curlx_free(*cred_handle);
    *cred_handle = GSS_C_NO_CREDENTIAL;
  }
  తిరిగి GSS_S_COMPLETE;
}

#అయితే_ముగింపు /* CURL_GSS_STUB */

OM_uint32 Curl_gss_init_sec_context(నిర్మాణం Curl_easy *దత్తాంశం,
                                    OM_uint32 *minor_స్థితి,
                                    gss_ctx_id_t *context,
                                    gss_name_t లక్ష్యం_name,
                                    gss_OID mech_type,
                                    gss_channel_bindings_t ఆదానం_chan_bindings,
                                    gss_బఫరు_t ఆదానం_token,
                                    gss_బఫరు_t output_token,
                                    స్థిరరాశి bool mutual_auth,
                                    OM_uint32 *ret_flags,
                                    gss_cred_id_t cred_handle)
{
  OM_uint32 req_flags = GSS_C_REPLAY_FLAG;

  ఒకవేళ(mutual_auth)
    req_flags |= GSS_C_MUTUAL_FLAG;

  ఒకవేళ(దత్తాంశం->సమితి.gssapi_delegation & CURLGSSAPI_DELEGATION_POLICY_FLAG) {
#నిర్వచితమైతే GSS_C_DELEG_POLICY_FLAG  /* MIT Kerberos 1.7+ (2009-06-02), Apple GSS,
                                   missing from GNU GSS */
    req_flags |= GSS_C_DELEG_POLICY_FLAG;
#లేకపోతే
    infof(దత్తాంశం, "WARNING: support for CURLGSSAPI_DELEGATION_POLICY_FLAG not "
          "compiled in");
#అయితే_ముగింపు
  }

  ఒకవేళ(దత్తాంశం->సమితి.gssapi_delegation & CURLGSSAPI_DELEGATION_FLAG)
    req_flags |= GSS_C_DELEG_FLAG;

#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_init_sec_context(minor_స్థితి,
                                     cred_handle,
                                     (నిర్మాణం stub_gss_ctx_id_t_desc **)context,
                                     లక్ష్యం_name,
                                     mech_type,
                                     req_flags,
                                     0, /* time_req */
                                     ఆదానం_chan_bindings,
                                     ఆదానం_token,
                                     ఖాళీ, /* actual_mech_type */
                                     output_token,
                                     ret_flags,
                                     ఖాళీ /* time_rec */);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_init_sec_context(minor_స్థితి,
                              cred_handle,
                              context,
                              లక్ష్యం_name,
                              mech_type,
                              req_flags,
                              0, /* time_req */
                              ఆదానం_chan_bindings,
                              ఆదానం_token,
                              ఖాళీ, /* actual_mech_type */
                              output_token,
                              ret_flags,
                              ఖాళీ /* time_rec */);
}

OM_uint32 Curl_gss_delete_sec_context(OM_uint32 *min,
                                      gss_ctx_id_t *context,
                                      gss_బఫరు_t output_token)
{
#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_delete_sec_context(min,
                                     (నిర్మాణం stub_gss_ctx_id_t_desc **)context,
                                     output_token);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_delete_sec_context(min, context, output_token);
}

OM_uint32 Curl_gss_inquire_context(OM_uint32 *minor_స్థితి,
                                   gss_ctx_id_t context,
                                   gss_OID *mech_type)
{
#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_inquire_context(minor_స్థితి,
                             (నిర్మాణం stub_gss_ctx_id_t_desc *)context,
                             ఖాళీ, ఖాళీ, ఖాళీ, mech_type,
                             ఖాళీ, ఖాళీ, ఖాళీ);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_inquire_context(minor_స్థితి, context,
                             ఖాళీ, ఖాళీ, ఖాళీ, mech_type,
                             ఖాళీ, ఖాళీ, ఖాళీ);
}

OM_uint32 Curl_gss_acquire_cred(OM_uint32 *minor_స్థితి,
                                gss_name_t desired_name,
                                OM_uint32 సమయం_req,
                                gss_OID_సమితి desired_mechs,
                                gss_cred_usage_t cred_usage,
                                gss_cred_id_t *output_cred_handle,
                                gss_OID_సమితి *actual_mechs,
                                OM_uint32 *సమయం_rec)
{
#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_acquire_cred(minor_స్థితి, desired_name, సమయం_req,
                                 desired_mechs, cred_usage,
                                 output_cred_handle, actual_mechs, సమయం_rec);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_acquire_cred(minor_స్థితి, desired_name, సమయం_req,
                          desired_mechs, cred_usage,
                          output_cred_handle, actual_mechs, సమయం_rec);
}

OM_uint32 Curl_gss_indicate_mechs(OM_uint32 *minor_స్థితి,
                                  gss_OID_సమితి *mech_సమితి)
{
#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_indicate_mechs(minor_స్థితి, mech_సమితి);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_indicate_mechs(minor_స్థితి, mech_సమితి);
}

#నిర్వచితమైతే HAVE_GSS_సమితి_NEG_MECHS
OM_uint32 Curl_gss_సమితి_neg_mechs(OM_uint32 *minor_స్థితి,
                                 gss_cred_id_t cred_handle,
                                 స్థిరరాశి gss_OID_సమితి mech_సమితి)
{
#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_సమితి_neg_mechs(minor_స్థితి, cred_handle, mech_సమితి);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_సమితి_neg_mechs(minor_స్థితి, cred_handle, mech_సమితి);
}
#అయితే_ముగింపు /* HAVE_GSS_SET_NEG_MECHS */

OM_uint32 Curl_gss_release_cred(OM_uint32 *minor_స్థితి,
                                gss_cred_id_t *cred_handle)
{
#నిర్వచితమైతే CURL_GSS_STUB
  ఒకవేళ(getenv("CURL_STUB_GSS_CREDS"))
    తిరిగి stub_gss_release_cred(minor_స్థితి, cred_handle);
#అయితే_ముగింపు /* CURL_GSS_STUB */

  తిరిగి gss_release_cred(minor_స్థితి, cred_handle);
}

#నిర్వచితమైతే CURLVERBOSE
#నిర్వచించు GSS_LOG_బఫరు_LEN 1024
స్థిర size_t display_gss_error(OM_uint32 స్థితి_2, పూర్ణాంకం type,
                                అక్షరం *buf, size_t len)
{
  OM_uint32 maj_stat;
  OM_uint32 min_stat;
  OM_uint32 సందేశం_ctx = 0;
  gss_బఫరు_desc స్థితి_string = GSS_C_ఖాళీలు_బఫరు;

  చేయి {
    maj_stat = gss_display_స్థితి(&min_stat,
                                  స్థితి_2,
                                  type,
                                  GSS_C_NO_OID,
                                  &సందేశం_ctx,
                                  &స్థితి_string);
    ఒకవేళ(maj_stat == GSS_S_COMPLETE && స్థితి_string.నిడివి > 0) {
      ఒకవేళ(GSS_LOG_బఫరు_LEN > len + స్థితి_string.నిడివి + 3) {
        len += curl_msnprintf(buf + len, GSS_LOG_బఫరు_LEN - len,
                              "%.*s. ", (పూర్ణాంకం)స్థితి_string.నిడివి,
                              (అక్షరం *)స్థితి_string.విలువ);
      }
    }
    gss_release_బఫరు(&min_stat, &స్థితి_string);
  } అయితే(!GSS_ERROR(maj_stat) && సందేశం_ctx);

  తిరిగి len;
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
శూన్య Curl_gss_log_error(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *ఉపసర్గ,
                        OM_uint32 major, OM_uint32 minor)
{
  అక్షరం buf[GSS_LOG_బఫరు_LEN] = "";
  size_t len = 0;

  ఒకవేళ(major != GSS_S_FAILURE)
    len = display_gss_error(major, GSS_C_GSS_సంకేతం, buf, len);

  display_gss_error(minor, GSS_C_MECH_సంకేతం, buf, len);

  infof(దత్తాంశం, "%s%s", ఉపసర్గ, buf);
}
#అయితే_ముగింపు /* CURLVERBOSE */

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__) && !defined(HAVE_GSSAPPLE)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

#అయితే_ముగింపు /* HAVE_GSSAPI */
