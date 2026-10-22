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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "http_httpsig.h"
#अन्तर्भाव "curl_ed25519.h"
#अन्तर्भाव "curl_hmac.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "strcase.h"

#अन्तर्भाव <time.h>

#परिभाषय HTTPSIG_MAX_SIG_आधारः   CURL_MAX_HTTP_शीर्षकम्
#परिभाषय HTTPSIG_MAX_COMPONENTS 16
#परिभाषय HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#परिभाषय HTTPSIG_DEFAULT_LABEL  "sig1"

गणना httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

स्थिर ध्रुव वर्ण *alg_गन्तव्यम्_str(गणना httpsig_alg alg)
{
  विकल्प(alg) {
  पक्ष HTTPSIG_ALG_ED25519:
    प्रत्यागम "ed25519";
  पक्ष HTTPSIG_ALG_HMAC_SHA256:
    प्रत्यागम "hmac-sha256";
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम रिक्त;
}

स्थिर गणना httpsig_alg id_गन्तव्यम्_alg(uint8_t val)
{
  विकल्प(val) {
  पक्ष CURLHTTPSIG_ED25519:
    प्रत्यागम HTTPSIG_ALG_ED25519;
  पक्ष CURLHTTPSIG_HMAC_SHA256:
    प्रत्यागम HTTPSIG_ALG_HMAC_SHA256;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम HTTPSIG_ALG_UNKNOWN;
}

स्थिर CURLcode decode_hex_कुञ्चिका(संरचना Curl_easy *दत्त,
                               ध्रुव वर्ण *hexstr,
                               अचिह्नित वर्ण **keyout,
                               size_t *keylen)
{
  size_t len, i;
  अचिह्नित वर्ण *keybuf;

  *keyout = रिक्त;
  *keylen = 0;

  len = वाक्यदैर्घ्यम्(hexstr);
  यावत्(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  यदि(len == 0 || (len & 1) != 0) {
    failf(दत्त, "httpsig: invalid hex key (length %zu)", len);
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  यदि(len > CURL_MAX_निवेशः_दीर्घता) {
    failf(दत्त, "httpsig: hex key too long");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  keybuf = curlx_malloc(len / 2);
  यदि(!keybuf)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रति(i = 0; i < len; i += 2) {
    यदि(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(दत्त, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    keybuf[i / 2] = (अचिह्नित वर्ण)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  प्रत्यागम CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
स्थिर CURLcode httpsig_authority(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  संरचना dynbuf *authority_buf)
{
  ध्रुव वर्ण *h = दत्त->state.http_host;

  यदि(h && curl_strnequal(h, "host:", 5)) {
    ध्रुव वर्ण *मानम् = h + 5;
    ध्रुव वर्ण *अन्तः;

    यावत्(ISBLANK(*मानम्))
      मानम्++;
    यदि(*मानम्) {
      CURLcode परिणाम;

      अन्तः = मानम्;
      यावत्(*अन्तः && !ISNEWLINE(*अन्तः))
        अन्तः++;
      यावत्(अन्तः > मानम् && ISBLANK(अन्तः[-1]))
        अन्तः--;
      परिणाम = curlx_dyn_addn(authority_buf, मानम्, (size_t)(अन्तः - मानम्));
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      प्रत्यागम CURLE_OK;
    }
  }

  {
    ध्रुव वर्ण *hostname = conn->origin->hostname;
    uint16_t पत्तनम् = conn->origin->पत्तनम्;

    यदि((conn->origin->scheme->defport != पत्तनम्) && पत्तनम्)
      प्रत्यागम curlx_dyn_addf(authority_buf, "%s:%u", hostname, पत्तनम्);
    प्रत्यागम curlx_dyn_सङ्कलय(authority_buf, hostname);
  }
}

स्थिर CURLcode sf_append_quoted(संरचना dynbuf *buf, ध्रुव वर्ण *str)
{
  CURLcode परिणाम = curlx_dyn_addn(buf, "\"", 1);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यावत्(*str) {
    यदि(ISCNTRL(*str))
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    यदि(*str == '\\' || *str == '"') {
      परिणाम = curlx_dyn_addn(buf, "\\", 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    परिणाम = curlx_dyn_addn(buf, str, 1);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    str++;
  }
  प्रत्यागम curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
स्थिर CURLcode sf_encode_byte_seq(ध्रुव अचिह्नित वर्ण *raw, size_t rawlen,
                                   संरचना dynbuf *बहिर्गमनम्)
{
  CURLcode परिणाम;
  size_t b64len;
  वर्ण *b64;

  परिणाम = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = curlx_dyn_addn(बहिर्गमनम्, ":", 1);
  यदि(!परिणाम)
    परिणाम = curlx_dyn_addn(बहिर्गमनम्, b64, b64len);
  यदि(!परिणाम)
    परिणाम = curlx_dyn_addn(बहिर्गमनम्, ":", 1);

  curlx_free(b64);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode build_sig_params(संरचना dynbuf *params,
                                 ध्रुव वर्ण **components, size_t गणनम्,
                                 time_t created, ध्रुव वर्ण *keyid,
                                 गणना httpsig_alg alg)
{
  CURLcode परिणाम;
  size_t i;

  परिणाम = curlx_dyn_addn(params, "(", 1);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  प्रति(i = 0; i < गणनम्; i++) {
    यदि(i > 0) {
      परिणाम = curlx_dyn_addn(params, " ", 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    परिणाम = sf_append_quoted(params, components[i]);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  परिणाम = curlx_dyn_addn(params, ")", 1);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = curlx_dyn_addf(params, ";created=%lld", (दीर्घ दीर्घ)created);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(keyid && *keyid) {
    परिणाम = curlx_dyn_सङ्कलय(params, ";keyid=");
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    परिणाम = sf_append_quoted(params, keyid);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  परिणाम = curlx_dyn_addf(params, ";alg=\"%s\"", alg_गन्तव्यम्_str(alg));
  प्रत्यागम परिणाम;
}

/* strings defined by RFC 9421 */
#परिभाषय SIG_विधानम्    "@method"
#परिभाषय SIG_AUTHORITY "@authority"
#परिभाषय SIG_मार्गः      "@path"
#परिभाषय SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
स्थिर CURLcode resolve_component(ध्रुव वर्ण *name,
                                  ध्रुव वर्ण *विधानम्,
                                  ध्रुव वर्ण *authority,
                                  ध्रुव वर्ण *मार्गः,
                                  ध्रुव वर्ण *query,
                                  संरचना Curl_easy *दत्त,
                                  संरचना dynbuf *valbuf,
                                  ध्रुव वर्ण **बहिर्गमनम्)
{
  *बहिर्गमनम् = रिक्त;

  यदि(name[0] == '@') {
    यदि(curl_strequal(name, SIG_विधानम्))
      *बहिर्गमनम् = विधानम्;
    अन्यथा यदि(curl_strequal(name, SIG_AUTHORITY))
      *बहिर्गमनम् = authority;
    अन्यथा यदि(curl_strequal(name, SIG_मार्गः))
      *बहिर्गमनम् = मार्गः;
    अन्यथा यदि(curl_strequal(name, SIG_QUERY))
      *बहिर्गमनम् = query;
    अन्यथा {
      failf(दत्त, "httpsig: unsupported derived component '%s'", name);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    यदि(!*बहिर्गमनम्) {
      failf(दत्त, "httpsig: derived component '%s' has no value", name);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    प्रत्यागम CURLE_OK;
  }
  अन्यथा {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    संरचना curl_slist *शीर्ष;
    size_t namelen = वाक्यदैर्घ्यम्(name);
    bool प्राप्तम् = FALSE;

    curlx_dyn_reset(valbuf);

    प्रति(शीर्ष = दत्त->समुच्चयः.शीर्षकाणि; शीर्ष; शीर्ष = शीर्ष->अग्र) {
      यदि(curl_strnequal(शीर्ष->दत्त, name, namelen) &&
         Curl_headersep(शीर्ष->दत्त[namelen])) {
        ध्रुव वर्ण *p = strchr(शीर्ष->दत्त, ':');
        यदि(p) {
          CURLcode परिणाम;
          संरचना Curl_str content;
          curlx_str_नियोजय(&content, p + 1, वाक्यदैर्घ्यम्(p + 1));
          curlx_str_trimblanks(&content);
          यदि(प्राप्तम्) {
            परिणाम = curlx_dyn_addn(valbuf, ", ", 2);
            यदि(परिणाम)
              प्रत्यागम परिणाम;
          }
          परिणाम = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          यदि(परिणाम)
            प्रत्यागम परिणाम;
          प्राप्तम् = TRUE;
        }
      }
    }

    यदि(प्राप्तम्) {
      *बहिर्गमनम् = curlx_dyn_ptr(valbuf);
      प्रत्यागम CURLE_OK;
    }
    failf(दत्त, "httpsig: header '%s' not found in request", name);
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
}

स्थिर CURLcode build_sig_आधारः(संरचना dynbuf *आधारः,
                               ध्रुव वर्ण **components, size_t गणनम्,
                               ध्रुव वर्ण *विधानम्,
                               ध्रुव वर्ण *authority,
                               ध्रुव वर्ण *मार्गः,
                               ध्रुव वर्ण *query,
                               संरचना Curl_easy *दत्त,
                               ध्रुव वर्ण *sig_params)
{
  CURLcode परिणाम;
  size_t i;
  संरचना dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_शीर्षकम्);

  प्रति(i = 0; i < गणनम्; i++) {
    ध्रुव वर्ण *val = रिक्त;
    परिणाम = resolve_component(components[i], विधानम्,
                               authority, मार्गः, query, दत्त,
                               &hdrvalbuf, &val);
    यदि(परिणाम || !val) {
      failf(दत्त, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      प्रत्यागम परिणाम ? परिणाम : CURLE_BAD_FUNCTION_तर्कः;
    }

    परिणाम = curlx_dyn_addf(आधारः, "\"%s\": %s\n", components[i], val);
    यदि(परिणाम) {
      curlx_dyn_free(&hdrvalbuf);
      प्रत्यागम परिणाम;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  परिणाम = curlx_dyn_addf(आधारः, "\"@signature-params\": %s", sig_params);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode parse_components(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *query,
                                 ध्रुव वर्ण **components,
                                 size_t *ncomp_बहिर्गमनम्,
                                 वर्ण **hdrs_copy_बहिर्गमनम्)
{
  ध्रुव वर्ण *hdrs = CURL_EASY_STR(दत्त, STRING_HTTPSIG_शीर्षकाणि);
  size_t ncomp = 0;

  *hdrs_copy_बहिर्गमनम् = रिक्त;
  यदि(hdrs && *hdrs) {
    वर्ण *p;
    वर्ण *hdrs_copy = curlx_strdup(hdrs);
    यदि(!hdrs_copy)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    *hdrs_copy_बहिर्गमनम् = hdrs_copy;
    p = hdrs_copy;
    यावत्(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      वर्ण *start;
      size_t tlen = 0;
      ध्रुव वर्ण *p2 = p;

      curlx_str_passblanks(&p2);
      यदि(!*p2)
        भङ्ग;
      p = start = CURL_UNCONST(p2);
      यावत्(*p && !ISBLANK(*p)) {
        यदि((*p == '\"') || (*p == '\\'))
          प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
        p++;
        tlen++;
      }
      यदि(*p)
        *p++ = '\0';

      यदि(tlen && start[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        start[--tlen] = '\0';
        यदि(!tlen) {
          failf(दत्त, "httpsig: empty header component name");
          प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
        }
        Curl_strntolower(start, start, tlen);
        components[ncomp++] = start;
      }
      अन्यथा {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(start, start, tlen);
        यदि(!वाक्यं_तुलय(start, "method"))
          components[ncomp++] = SIG_विधानम्;
        अन्यथा यदि(!वाक्यं_तुलय(start, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        अन्यथा यदि(!वाक्यं_तुलय(start, "path"))
          components[ncomp++] = SIG_मार्गः;
        अन्यथा यदि(!वाक्यं_तुलय(start, "query"))
          components[ncomp++] = SIG_QUERY;
        अन्यथा {
          failf(दत्त, "httpsig: unknown component '%s'", start);
          प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
        }
      }
    }
    यदि(!ncomp) {
      failf(दत्त, "httpsig: no signature components specified");
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    यदि(*p) {
      failf(दत्त, "httpsig: too many signature components (max %u)",
            (अचिह्नित पूर्णाङ्क)HTTPSIG_MAX_COMPONENTS);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      प्रति(i = 0; i < ncomp; i++) {
        प्रति(j = i + 1; j < ncomp; j++) {
          यदि(!वाक्यं_तुलय(components[i], components[j])) {
            failf(दत्त, "httpsig: duplicate signature component '%s'",
                  components[i]);
            प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
          }
        }
      }
    }
  }
  अन्यथा {
    components[ncomp++] = SIG_विधानम्;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_मार्गः;
    यदि(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_बहिर्गमनम् = ncomp;
  प्रत्यागम CURLE_OK;
}

स्थिर time_t httpsig_get_created(शून्य)
{
#यदि_परिभाषितम् DEBUGBUILD
  वर्ण *force = getenv("CURL_FORCETIME");
  यदि(force && *force) {
    वर्ण *sigts = getenv("CURL_HTTPSIG_CREATED");
    यदि(sigts && *sigts) {
      ध्रुव वर्ण *p = sigts;
      curl_off_t अङ्कः;
      यदि(!curlx_str_सङ्ख्या(&p, &अङ्कः, CURL_OFF_T_MAX))
        प्रत्यागम (time_t)अङ्कः;
    }
    प्रत्यागम 0;
  }
#यदि_अन्तः
  प्रत्यागम time(रिक्त);
}

स्थिर CURLcode httpsig_sign_आधारः(संरचना Curl_easy *दत्त,
                                  गणना httpsig_alg alg,
                                  ध्रुव अचिह्नित वर्ण *keybuf,
                                  size_t keylen,
                                  ध्रुव संरचना dynbuf *sig_आधारः,
                                  अचिह्नित वर्ण *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode परिणाम;

  विकल्प(alg) {
  पक्ष HTTPSIG_ALG_ED25519:
    परिणाम = Curl_ed25519_sign(
      keybuf, keylen,
      (ध्रुव अचिह्नित वर्ण *)curlx_dyn_ptr(sig_आधारः),
      curlx_dyn_len(sig_आधारः),
      raw_sig, raw_sig_len);
    भङ्ग;
  पक्ष HTTPSIG_ALG_HMAC_SHA256:
    परिणाम = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (ध्रुव अचिह्नित वर्ण *)curlx_dyn_ptr(sig_आधारः),
                         curlx_dyn_len(sig_आधारः), raw_sig);
    यदि(!परिणाम)
      *raw_sig_len = CURL_SHA256_DIGEST_दीर्घता;
    भङ्ग;
  स्वभाव:
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    भङ्ग;
  }

  यदि(परिणाम && परिणाम == CURLE_NOT_BUILT_अन्तर्) {
    failf(दत्त, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_गन्तव्यम्_str(alg));
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_output_httpsig(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *मार्गः;
  ध्रुव वर्ण *query;
  Curl_HttpReq httpreq;
  ध्रुव वर्ण *विधानम् = रिक्त;
  ध्रुव वर्ण *hexkey = CURL_EASY_STR(दत्त, STRING_HTTPSIG_कुञ्चिका);
  ध्रुव वर्ण *keyid = CURL_EASY_STR(दत्त, STRING_HTTPSIG_KEYID);
  गणना httpsig_alg alg;
  time_t created;
  संरचना dynbuf sig_params;
  संरचना dynbuf sig_आधारः;
  संरचना dynbuf sig_hdr;
  संरचना dynbuf निवेशः_hdr;
  संरचना dynbuf authority_buf;
  ध्रुव वर्ण *authority;
  ध्रुव वर्ण *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  अचिह्नित वर्ण *keybuf = रिक्त;
  size_t keylen = 0;
  अचिह्नित वर्ण raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  वर्ण *auth_शीर्षकाणि = रिक्त;
  वर्ण *hdrs_copy = रिक्त;
  संरचना dynbuf query_dyn;

  alg = id_गन्तव्यम्_alg(दत्त->समुच्चयः.httpsig_algorithm);
  यदि(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(दत्त, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  यदि(!hexkey || !*hexkey) {
    failf(दत्त, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  यदि(!keyid || !*keyid) {
    failf(दत्त, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_शीर्षकम्);
  curlx_dyn_init(&sig_आधारः, HTTPSIG_MAX_SIG_आधारः);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_शीर्षकम्);
  curlx_dyn_init(&निवेशः_hdr, CURL_MAX_HTTP_शीर्षकम्);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_शीर्षकम्);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_शीर्षकम्);

  यदि(Curl_checkheaders(दत्त, STRCONST("Signature")) ||
     Curl_checkheaders(दत्त, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    गच्छ समाप्तम्;
  }

  परिणाम = decode_hex_कुञ्चिका(दत्त, hexkey, &keybuf, &keylen);
  यदि(परिणाम)
    गच्छ fail;

  यदि(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(दत्त, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ fail;
  }

  Curl_http_विधानम्(दत्त, &विधानम्, &httpreq);

  मार्गः = दत्त->state.ऊर्ध्वम्.मार्गः;
  यदि(!मार्गः || !*मार्गः)
    मार्गः = "/";

  query = दत्त->state.ऊर्ध्वम्.query;

  परिणाम = httpsig_authority(दत्त, conn, &authority_buf);
  यदि(परिणाम)
    गच्छ fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  यदि(query && *query)
    परिणाम = curlx_dyn_addf(&query_dyn, "?%s", query);
  अन्यथा
    परिणाम = curlx_dyn_सङ्कलय(&query_dyn, "?");
  यदि(परिणाम)
    गच्छ fail;

  परिणाम = parse_components(दत्त, query, components, &ncomp, &hdrs_copy);
  यदि(परिणाम)
    गच्छ fail;

  created = httpsig_get_created();

  परिणाम = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  यदि(परिणाम)
    गच्छ fail;

  infof(दत्त, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  परिणाम = build_sig_आधारः(&sig_आधारः, components, ncomp,
                          विधानम्, authority, मार्गः,
                          curlx_dyn_ptr(&query_dyn),
                          दत्त, curlx_dyn_ptr(&sig_params));
  यदि(परिणाम)
    गच्छ fail;

  infof(दत्त, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_आधारः));

  परिणाम = httpsig_sign_आधारः(दत्त, alg, keybuf, keylen, &sig_आधारः,
                             raw_sig, &raw_sig_len);
  यदि(परिणाम)
    गच्छ fail;

  परिणाम = curlx_dyn_सङ्कलय(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  यदि(परिणाम)
    गच्छ fail;
  परिणाम = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  यदि(परिणाम)
    गच्छ fail;

  परिणाम = curlx_dyn_addf(&निवेशः_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  यदि(परिणाम)
    गच्छ fail;

  auth_शीर्षकाणि = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&निवेशः_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  यदि(!auth_शीर्षकाणि)
    गच्छ fail;

  infof(दत्त, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&निवेशः_hdr));
  infof(दत्त, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
समाप्तम्:
  curlx_free(दत्त->req.hd_auth);
  दत्त->req.hd_auth = auth_शीर्षकाणि;
  दत्त->state.authhost.समाप्तम् = TRUE;
  परिणाम = CURLE_OK;

fail:
  यदि(keybuf) {
    स्मृतिं_पूरय(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  स्मृतिं_पूरय(raw_sig, 0, परिमाण(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_आधारः);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&निवेशः_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
