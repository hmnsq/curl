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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "http_httpsig.h"
#అన్తర్భావ "curl_ed25519.h"
#అన్తర్భావ "curl_hmac.h"
#అన్తర్భావ "curl_sha256.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "slist.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "strcase.h"

#అన్తర్భావ <time.h>

#నిర్వచించు HTTPSIG_MAX_SIG_పునాది   CURL_MAX_HTTP_శీర్షిక
#నిర్వచించు HTTPSIG_MAX_COMPONENTS 16
#నిర్వచించు HTTPSIG_MAX_RAW_SIG    CURL_ED25519_SIGLEN
#నిర్వచించు HTTPSIG_DEFAULT_LABEL  "sig1"

గణన httpsig_alg {
  HTTPSIG_ALG_ED25519,
  HTTPSIG_ALG_HMAC_SHA256,
  HTTPSIG_ALG_UNKNOWN
};

స్థిర స్థిరరాశి అక్షరం *alg_గమ్యం_str(గణన httpsig_alg alg)
{
  ఎంపిక(alg) {
  స్థితి HTTPSIG_ALG_ED25519:
    తిరిగి "ed25519";
  స్థితి HTTPSIG_ALG_HMAC_SHA256:
    తిరిగి "hmac-sha256";
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఖాళీ;
}

స్థిర గణన httpsig_alg id_గమ్యం_alg(uint8_t val)
{
  ఎంపిక(val) {
  స్థితి CURLHTTPSIG_ED25519:
    తిరిగి HTTPSIG_ALG_ED25519;
  స్థితి CURLHTTPSIG_HMAC_SHA256:
    తిరిగి HTTPSIG_ALG_HMAC_SHA256;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి HTTPSIG_ALG_UNKNOWN;
}

స్థిర CURLcode decode_hex_కీలకం(నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి అక్షరం *hexstr,
                               గుర్తులేని అక్షరం **keyout,
                               size_t *keylen)
{
  size_t len, i;
  గుర్తులేని అక్షరం *keybuf;

  *keyout = ఖాళీ;
  *keylen = 0;

  len = అక్షరశ్రేణి_పొడవు(hexstr);
  అయితే(len > 0 && ISNEWLINE(hexstr[len - 1]))
    len--;

  ఒకవేళ(len == 0 || (len & 1) != 0) {
    failf(దత్తాంశం, "httpsig: invalid hex key (length %zu)", len);
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  ఒకవేళ(len > CURL_MAX_ఆదానం_నిడివి) {
    failf(దత్తాంశం, "httpsig: hex key too long");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  keybuf = curlx_malloc(len / 2);
  ఒకవేళ(!keybuf)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  కొరకు(i = 0; i < len; i += 2) {
    ఒకవేళ(!ISXDIGIT(hexstr[i]) || !ISXDIGIT(hexstr[i + 1])) {
      failf(దత్తాంశం, "httpsig: invalid hex at position %zu ('%c%c')",
            i, hexstr[i], hexstr[i + 1]);
      curlx_free(keybuf);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    keybuf[i / 2] = (గుర్తులేని అక్షరం)((curlx_hexval(hexstr[i]) << 4) |
                                     curlx_hexval(hexstr[i + 1]));
  }

  *keyout = keybuf;
  *keylen = len / 2;
  తిరిగి CURLE_OK;
}

/* @authority matches the Host header field-value when available (RFC 9421).
   data->state.http_host is produced by http_set_aptr_host() before auth. */
స్థిర CURLcode httpsig_authority(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  నిర్మాణం dynbuf *authority_buf)
{
  స్థిరరాశి అక్షరం *h = దత్తాంశం->state.http_host;

  ఒకవేళ(h && curl_strnequal(h, "host:", 5)) {
    స్థిరరాశి అక్షరం *విలువ = h + 5;
    స్థిరరాశి అక్షరం *చివర;

    అయితే(ISBLANK(*విలువ))
      విలువ++;
    ఒకవేళ(*విలువ) {
      CURLcode ఫలితం;

      చివర = విలువ;
      అయితే(*చివర && !ISNEWLINE(*చివర))
        చివర++;
      అయితే(చివర > విలువ && ISBLANK(చివర[-1]))
        చివర--;
      ఫలితం = curlx_dyn_addn(authority_buf, విలువ, (size_t)(చివర - విలువ));
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      తిరిగి CURLE_OK;
    }
  }

  {
    స్థిరరాశి అక్షరం *hostname = conn->origin->hostname;
    uint16_t పోర్టు = conn->origin->పోర్టు;

    ఒకవేళ((conn->origin->scheme->defport != పోర్టు) && పోర్టు)
      తిరిగి curlx_dyn_addf(authority_buf, "%s:%u", hostname, పోర్టు);
    తిరిగి curlx_dyn_కూడు(authority_buf, hostname);
  }
}

స్థిర CURLcode sf_append_quoted(నిర్మాణం dynbuf *buf, స్థిరరాశి అక్షరం *str)
{
  CURLcode ఫలితం = curlx_dyn_addn(buf, "\"", 1);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  అయితే(*str) {
    ఒకవేళ(ISCNTRL(*str))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(*str == '\\' || *str == '"') {
      ఫలితం = curlx_dyn_addn(buf, "\\", 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    ఫలితం = curlx_dyn_addn(buf, str, 1);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    str++;
  }
  తిరిగి curlx_dyn_addn(buf, "\"", 1);
}

/* base64-encode raw bytes into an RFC 8941 byte sequence (:base64:) */
స్థిర CURLcode sf_encode_byte_seq(స్థిరరాశి గుర్తులేని అక్షరం *raw, size_t rawlen,
                                   నిర్మాణం dynbuf *బహిర్గతం)
{
  CURLcode ఫలితం;
  size_t b64len;
  అక్షరం *b64;

  ఫలితం = curlx_base64_encode(raw, rawlen, &b64, &b64len);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = curlx_dyn_addn(బహిర్గతం, ":", 1);
  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_addn(బహిర్గతం, b64, b64len);
  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_addn(బహిర్గతం, ":", 1);

  curlx_free(b64);
  తిరిగి ఫలితం;
}

స్థిర CURLcode build_sig_params(నిర్మాణం dynbuf *params,
                                 స్థిరరాశి అక్షరం **components, size_t లెక్క,
                                 time_t created, స్థిరరాశి అక్షరం *keyid,
                                 గణన httpsig_alg alg)
{
  CURLcode ఫలితం;
  size_t i;

  ఫలితం = curlx_dyn_addn(params, "(", 1);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  కొరకు(i = 0; i < లెక్క; i++) {
    ఒకవేళ(i > 0) {
      ఫలితం = curlx_dyn_addn(params, " ", 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    ఫలితం = sf_append_quoted(params, components[i]);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఫలితం = curlx_dyn_addn(params, ")", 1);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = curlx_dyn_addf(params, ";created=%lld", (దీర్ఘ దీర్ఘ)created);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(keyid && *keyid) {
    ఫలితం = curlx_dyn_కూడు(params, ";keyid=");
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఫలితం = sf_append_quoted(params, keyid);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఫలితం = curlx_dyn_addf(params, ";alg=\"%s\"", alg_గమ్యం_str(alg));
  తిరిగి ఫలితం;
}

/* strings defined by RFC 9421 */
#నిర్వచించు SIG_పద్ధతి    "@method"
#నిర్వచించు SIG_AUTHORITY "@authority"
#నిర్వచించు SIG_మార్గం      "@path"
#నిర్వచించు SIG_QUERY     "@query"

/* Resolve a component identifier to its value.
 * For headers, we walk the full user-supplied header list to combine
 * duplicate field values with ", " per RFC 9421 Section 2.1. Each
 * individual value is trimmed of leading/trailing OWS and the trailing
 * \r\n. The combined result is written into the caller-provided buffer. */
స్థిర CURLcode resolve_component(స్థిరరాశి అక్షరం *name,
                                  స్థిరరాశి అక్షరం *పద్ధతి,
                                  స్థిరరాశి అక్షరం *authority,
                                  స్థిరరాశి అక్షరం *మార్గం,
                                  స్థిరరాశి అక్షరం *query,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం dynbuf *valbuf,
                                  స్థిరరాశి అక్షరం **బహిర్గతం)
{
  *బహిర్గతం = ఖాళీ;

  ఒకవేళ(name[0] == '@') {
    ఒకవేళ(curl_strequal(name, SIG_పద్ధతి))
      *బహిర్గతం = పద్ధతి;
    లేకపోతే ఒకవేళ(curl_strequal(name, SIG_AUTHORITY))
      *బహిర్గతం = authority;
    లేకపోతే ఒకవేళ(curl_strequal(name, SIG_మార్గం))
      *బహిర్గతం = మార్గం;
    లేకపోతే ఒకవేళ(curl_strequal(name, SIG_QUERY))
      *బహిర్గతం = query;
    లేకపోతే {
      failf(దత్తాంశం, "httpsig: unsupported derived component '%s'", name);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    ఒకవేళ(!*బహిర్గతం) {
      failf(దత్తాంశం, "httpsig: derived component '%s' has no value", name);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    తిరిగి CURLE_OK;
  }
  లేకపోతే {
    /* RFC 9421 Section 2.1: walk all user-supplied headers and combine
       duplicate field values with ", " per HTTP field combination rules. */
    నిర్మాణం curl_slist *తల;
    size_t namelen = అక్షరశ్రేణి_పొడవు(name);
    bool దొరికింది = FALSE;

    curlx_dyn_reset(valbuf);

    కొరకు(తల = దత్తాంశం->సమితి.శీర్షికలు; తల; తల = తల->తదుపరి) {
      ఒకవేళ(curl_strnequal(తల->దత్తాంశం, name, namelen) &&
         Curl_headersep(తల->దత్తాంశం[namelen])) {
        స్థిరరాశి అక్షరం *p = strchr(తల->దత్తాంశం, ':');
        ఒకవేళ(p) {
          CURLcode ఫలితం;
          నిర్మాణం Curl_str content;
          curlx_str_కేటాయించు(&content, p + 1, అక్షరశ్రేణి_పొడవు(p + 1));
          curlx_str_trimblanks(&content);
          ఒకవేళ(దొరికింది) {
            ఫలితం = curlx_dyn_addn(valbuf, ", ", 2);
            ఒకవేళ(ఫలితం)
              తిరిగి ఫలితం;
          }
          ఫలితం = curlx_dyn_addn(valbuf, curlx_str(&content),
                                  curlx_strlen(&content));
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
          దొరికింది = TRUE;
        }
      }
    }

    ఒకవేళ(దొరికింది) {
      *బహిర్గతం = curlx_dyn_ptr(valbuf);
      తిరిగి CURLE_OK;
    }
    failf(దత్తాంశం, "httpsig: header '%s' not found in request", name);
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
}

స్థిర CURLcode build_sig_పునాది(నిర్మాణం dynbuf *పునాది,
                               స్థిరరాశి అక్షరం **components, size_t లెక్క,
                               స్థిరరాశి అక్షరం *పద్ధతి,
                               స్థిరరాశి అక్షరం *authority,
                               స్థిరరాశి అక్షరం *మార్గం,
                               స్థిరరాశి అక్షరం *query,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి అక్షరం *sig_params)
{
  CURLcode ఫలితం;
  size_t i;
  నిర్మాణం dynbuf hdrvalbuf;

  curlx_dyn_init(&hdrvalbuf, CURL_MAX_HTTP_శీర్షిక);

  కొరకు(i = 0; i < లెక్క; i++) {
    స్థిరరాశి అక్షరం *val = ఖాళీ;
    ఫలితం = resolve_component(components[i], పద్ధతి,
                               authority, మార్గం, query, దత్తాంశం,
                               &hdrvalbuf, &val);
    ఒకవేళ(ఫలితం || !val) {
      failf(దత్తాంశం, "httpsig: cannot resolve component '%s'", components[i]);
      curlx_dyn_free(&hdrvalbuf);
      తిరిగి ఫలితం ? ఫలితం : CURLE_BAD_FUNCTION_వాదం;
    }

    ఫలితం = curlx_dyn_addf(పునాది, "\"%s\": %s\n", components[i], val);
    ఒకవేళ(ఫలితం) {
      curlx_dyn_free(&hdrvalbuf);
      తిరిగి ఫలితం;
    }
  }

  curlx_dyn_free(&hdrvalbuf);
  ఫలితం = curlx_dyn_addf(పునాది, "\"@signature-params\": %s", sig_params);
  తిరిగి ఫలితం;
}

స్థిర CURLcode parse_components(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *query,
                                 స్థిరరాశి అక్షరం **components,
                                 size_t *ncomp_బహిర్గతం,
                                 అక్షరం **hdrs_copy_బహిర్గతం)
{
  స్థిరరాశి అక్షరం *hdrs = CURL_EASY_STR(దత్తాంశం, STRING_HTTPSIG_శీర్షికలు);
  size_t ncomp = 0;

  *hdrs_copy_బహిర్గతం = ఖాళీ;
  ఒకవేళ(hdrs && *hdrs) {
    అక్షరం *p;
    అక్షరం *hdrs_copy = curlx_strdup(hdrs);
    ఒకవేళ(!hdrs_copy)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    *hdrs_copy_బహిర్గతం = hdrs_copy;
    p = hdrs_copy;
    అయితే(*p && ncomp < HTTPSIG_MAX_COMPONENTS) {
      అక్షరం *ప్రారంభం;
      size_t tlen = 0;
      స్థిరరాశి అక్షరం *p2 = p;

      curlx_str_passblanks(&p2);
      ఒకవేళ(!*p2)
        ఆపు;
      p = ప్రారంభం = CURL_UNCONST(p2);
      అయితే(*p && !ISBLANK(*p)) {
        ఒకవేళ((*p == '\"') || (*p == '\\'))
          తిరిగి CURLE_BAD_FUNCTION_వాదం;
        p++;
        tlen++;
      }
      ఒకవేళ(*p)
        *p++ = '\0';

      ఒకవేళ(tlen && ప్రారంభం[tlen - 1] == ':') {
        /* Header field: drop the trailing ':' marker. RFC 9421 field
           names are canonically lowercase (Section 2.1). */
        ప్రారంభం[--tlen] = '\0';
        ఒకవేళ(!tlen) {
          failf(దత్తాంశం, "httpsig: empty header component name");
          తిరిగి CURLE_BAD_FUNCTION_వాదం;
        }
        Curl_strntolower(ప్రారంభం, ప్రారంభం, tlen);
        components[ncomp++] = ప్రారంభం;
      }
      లేకపోతే {
        /* Derived component: map the bare name to its canonical RFC 9421
           '@'-prefixed identifier (Section 2.2). */
        Curl_strntolower(ప్రారంభం, ప్రారంభం, tlen);
        ఒకవేళ(!strcmp(ప్రారంభం, "method"))
          components[ncomp++] = SIG_పద్ధతి;
        లేకపోతే ఒకవేళ(!strcmp(ప్రారంభం, "authority"))
          components[ncomp++] = SIG_AUTHORITY;
        లేకపోతే ఒకవేళ(!strcmp(ప్రారంభం, "path"))
          components[ncomp++] = SIG_మార్గం;
        లేకపోతే ఒకవేళ(!strcmp(ప్రారంభం, "query"))
          components[ncomp++] = SIG_QUERY;
        లేకపోతే {
          failf(దత్తాంశం, "httpsig: unknown component '%s'", ప్రారంభం);
          తిరిగి CURLE_BAD_FUNCTION_వాదం;
        }
      }
    }
    ఒకవేళ(!ncomp) {
      failf(దత్తాంశం, "httpsig: no signature components specified");
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    ఒకవేళ(*p) {
      failf(దత్తాంశం, "httpsig: too many signature components (max %u)",
            (గుర్తులేని పూర్ణాంకం)HTTPSIG_MAX_COMPONENTS);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }

    /* RFC 9421 Section 2: each covered component MUST occur only once */
    {
      size_t i, j;

      కొరకు(i = 0; i < ncomp; i++) {
        కొరకు(j = i + 1; j < ncomp; j++) {
          ఒకవేళ(!strcmp(components[i], components[j])) {
            failf(దత్తాంశం, "httpsig: duplicate signature component '%s'",
                  components[i]);
            తిరిగి CURLE_BAD_FUNCTION_వాదం;
          }
        }
      }
    }
  }
  లేకపోతే {
    components[ncomp++] = SIG_పద్ధతి;
    components[ncomp++] = SIG_AUTHORITY;
    components[ncomp++] = SIG_మార్గం;
    ఒకవేళ(query)
      components[ncomp++] = SIG_QUERY;
  }

  *ncomp_బహిర్గతం = ncomp;
  తిరిగి CURLE_OK;
}

స్థిర time_t httpsig_get_created(శూన్య)
{
#నిర్వచితమైతే DEBUGBUILD
  అక్షరం *force = getenv("CURL_FORCETIME");
  ఒకవేళ(force && *force) {
    అక్షరం *sigts = getenv("CURL_HTTPSIG_CREATED");
    ఒకవేళ(sigts && *sigts) {
      స్థిరరాశి అక్షరం *p = sigts;
      curl_off_t అంకె;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &అంకె, CURL_OFF_T_MAX))
        తిరిగి (time_t)అంకె;
    }
    తిరిగి 0;
  }
#అయితే_ముగింపు
  తిరిగి సమయం(ఖాళీ);
}

స్థిర CURLcode httpsig_sign_పునాది(నిర్మాణం Curl_easy *దత్తాంశం,
                                  గణన httpsig_alg alg,
                                  స్థిరరాశి గుర్తులేని అక్షరం *keybuf,
                                  size_t keylen,
                                  స్థిరరాశి నిర్మాణం dynbuf *sig_పునాది,
                                  గుర్తులేని అక్షరం *raw_sig,
                                  size_t *raw_sig_len)
{
  CURLcode ఫలితం;

  ఎంపిక(alg) {
  స్థితి HTTPSIG_ALG_ED25519:
    ఫలితం = Curl_ed25519_sign(
      keybuf, keylen,
      (స్థిరరాశి గుర్తులేని అక్షరం *)curlx_dyn_ptr(sig_పునాది),
      curlx_dyn_len(sig_పునాది),
      raw_sig, raw_sig_len);
    ఆపు;
  స్థితి HTTPSIG_ALG_HMAC_SHA256:
    ఫలితం = Curl_hmacit(&Curl_HMAC_SHA256, keybuf, keylen,
                         (స్థిరరాశి గుర్తులేని అక్షరం *)curlx_dyn_ptr(sig_పునాది),
                         curlx_dyn_len(sig_పునాది), raw_sig);
    ఒకవేళ(!ఫలితం)
      *raw_sig_len = CURL_SHA256_DIGEST_నిడివి;
    ఆపు;
  అప్రమేయ:
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    ఆపు;
  }

  ఒకవేళ(ఫలితం && ఫలితం == CURLE_NOT_BUILT_లోపల) {
    failf(దత్తాంశం, "httpsig: algorithm '%s' not supported by TLS backend",
          alg_గమ్యం_str(alg));
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_output_httpsig(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *మార్గం;
  స్థిరరాశి అక్షరం *query;
  Curl_HttpReq httpreq;
  స్థిరరాశి అక్షరం *పద్ధతి = ఖాళీ;
  స్థిరరాశి అక్షరం *hexkey = CURL_EASY_STR(దత్తాంశం, STRING_HTTPSIG_కీలకం);
  స్థిరరాశి అక్షరం *keyid = CURL_EASY_STR(దత్తాంశం, STRING_HTTPSIG_KEYID);
  గణన httpsig_alg alg;
  time_t created;
  నిర్మాణం dynbuf sig_params;
  నిర్మాణం dynbuf sig_పునాది;
  నిర్మాణం dynbuf sig_hdr;
  నిర్మాణం dynbuf ఆదానం_hdr;
  నిర్మాణం dynbuf authority_buf;
  స్థిరరాశి అక్షరం *authority;
  స్థిరరాశి అక్షరం *components[HTTPSIG_MAX_COMPONENTS];
  size_t ncomp = 0;
  గుర్తులేని అక్షరం *keybuf = ఖాళీ;
  size_t keylen = 0;
  గుర్తులేని అక్షరం raw_sig[HTTPSIG_MAX_RAW_SIG];
  size_t raw_sig_len = 0;
  అక్షరం *auth_శీర్షికలు = ఖాళీ;
  అక్షరం *hdrs_copy = ఖాళీ;
  నిర్మాణం dynbuf query_dyn;

  alg = id_గమ్యం_alg(దత్తాంశం->సమితి.httpsig_algorithm);
  ఒకవేళ(alg == HTTPSIG_ALG_UNKNOWN) {
    failf(దత్తాంశం, "httpsig: CURLOPT_HTTPSIG_ALGORITHM is required");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  ఒకవేళ(!hexkey || !*hexkey) {
    failf(దత్తాంశం, "httpsig: CURLOPT_HTTPSIG_KEY is required");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  ఒకవేళ(!keyid || !*keyid) {
    failf(దత్తాంశం, "httpsig: CURLOPT_HTTPSIG_KEYID is required");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  curlx_dyn_init(&sig_params, CURL_MAX_HTTP_శీర్షిక);
  curlx_dyn_init(&sig_పునాది, HTTPSIG_MAX_SIG_పునాది);
  curlx_dyn_init(&sig_hdr, CURL_MAX_HTTP_శీర్షిక);
  curlx_dyn_init(&ఆదానం_hdr, CURL_MAX_HTTP_శీర్షిక);
  curlx_dyn_init(&authority_buf, CURL_MAX_HTTP_శీర్షిక);
  curlx_dyn_init(&query_dyn, CURL_MAX_HTTP_శీర్షిక);

  ఒకవేళ(Curl_checkheaders(దత్తాంశం, STRCONST("Signature")) ||
     Curl_checkheaders(దత్తాంశం, STRCONST("Signature-Input"))) {
    /* user provides their own Signature / Signature-Input headers, consider
       this done */
    వెళ్ళు పూర్తి;
  }

  ఫలితం = decode_hex_కీలకం(దత్తాంశం, hexkey, &keybuf, &keylen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఒకవేళ(alg == HTTPSIG_ALG_ED25519 && keylen != 32) {
    failf(దత్తాంశం, "httpsig: ed25519 requires a 32-byte key (got %zu)", keylen);
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు fail;
  }

  Curl_http_పద్ధతి(దత్తాంశం, &పద్ధతి, &httpreq);

  మార్గం = దత్తాంశం->state.పైకి.మార్గం;
  ఒకవేళ(!మార్గం || !*మార్గం)
    మార్గం = "/";

  query = దత్తాంశం->state.పైకి.query;

  ఫలితం = httpsig_authority(దత్తాంశం, conn, &authority_buf);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;
  authority = curlx_dyn_ptr(&authority_buf);

  /* Build @query value: RFC 9421 Section 2.2.7 - always starts with "?" */
  ఒకవేళ(query && *query)
    ఫలితం = curlx_dyn_addf(&query_dyn, "?%s", query);
  లేకపోతే
    ఫలితం = curlx_dyn_కూడు(&query_dyn, "?");
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఫలితం = parse_components(దత్తాంశం, query, components, &ncomp, &hdrs_copy);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  created = httpsig_get_created();

  ఫలితం = build_sig_params(&sig_params, components, ncomp,
                            created, keyid, alg);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  infof(దత్తాంశం, "httpsig: Signature-Input params: %s",
        curlx_dyn_ptr(&sig_params));

  ఫలితం = build_sig_పునాది(&sig_పునాది, components, ncomp,
                          పద్ధతి, authority, మార్గం,
                          curlx_dyn_ptr(&query_dyn),
                          దత్తాంశం, curlx_dyn_ptr(&sig_params));
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  infof(దత్తాంశం, "httpsig: Signature base: [%s]",
        curlx_dyn_ptr(&sig_పునాది));

  ఫలితం = httpsig_sign_పునాది(దత్తాంశం, alg, keybuf, keylen, &sig_పునాది,
                             raw_sig, &raw_sig_len);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఫలితం = curlx_dyn_కూడు(&sig_hdr, HTTPSIG_DEFAULT_LABEL "=");
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;
  ఫలితం = sf_encode_byte_seq(raw_sig, raw_sig_len, &sig_hdr);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఫలితం = curlx_dyn_addf(&ఆదానం_hdr, "%s=%s", HTTPSIG_DEFAULT_LABEL,
                          curlx_dyn_ptr(&sig_params));
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  auth_శీర్షికలు = curl_maprintf("Signature-Input: %s\r\n"
                               "Signature: %s\r\n",
                               curlx_dyn_ptr(&ఆదానం_hdr),
                               curlx_dyn_ptr(&sig_hdr));
  ఒకవేళ(!auth_శీర్షికలు)
    వెళ్ళు fail;

  infof(దత్తాంశం, "httpsig: Signature-Input: %s", curlx_dyn_ptr(&ఆదానం_hdr));
  infof(దత్తాంశం, "httpsig: Signature: %s", curlx_dyn_ptr(&sig_hdr));
పూర్తి:
  curlx_free(దత్తాంశం->req.hd_auth);
  దత్తాంశం->req.hd_auth = auth_శీర్షికలు;
  దత్తాంశం->state.authhost.పూర్తి = TRUE;
  ఫలితం = CURLE_OK;

fail:
  ఒకవేళ(keybuf) {
    memset(keybuf, 0, keylen);
    curlx_free(keybuf);
  }
  memset(raw_sig, 0, పరిమాణం(raw_sig));
  curlx_free(hdrs_copy);
  curlx_dyn_free(&sig_params);
  curlx_dyn_free(&sig_పునాది);
  curlx_dyn_free(&sig_hdr);
  curlx_dyn_free(&ఆదానం_hdr);
  curlx_dyn_free(&authority_buf);
  curlx_dyn_free(&query_dyn);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
