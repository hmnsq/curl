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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_aws_sigv4.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMAC_SHA256(k, kl, d, dl, o)                 \
  ਕਰੋ {                                               \
    ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)(k), \
                         kl,                         \
                         (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)(d), \
                         dl, o);                     \
    ਜੇ(ਨਤੀਜਾ) {                                     \
      ਜਾਓ fail;                                     \
    }                                                \
  } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMESTAMP_ਮਾਪ 17

/* hex-encoded with null-terminator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHA256_HEX_ਲੰਮਾਈ ((2 * CURL_SHA256_DIGEST_ਲੰਮਾਈ) + 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_QUERY_COMPONENTS 128

ਬਣਤਰ pair {
  ਬਣਤਰ dynbuf ਕੁੰਜੀ;
  ਬਣਤਰ dynbuf ਮੁੱਲ;
};

ਸਥਿਰ ਖਾਲੀ sha256_ਮੰਜ਼ਿਲ_hex(ਅੱਖਰ *dst, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_ਲੰਮਾਈ,
                 (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)dst, SHA256_HEX_ਲੰਮਾਈ);
}

ਸਥਿਰ ਅੱਖਰ *ਲੱਭੋ_date_hdr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *sig_hdr)
{
  ਅੱਖਰ *tmp = Curl_checkheaders(ਅੰਕੜੇ, sig_hdr, ਲੜੀ_ਲੰਬਾਈ(sig_hdr));

  ਜੇ(tmp)
    ਵਾਪਸ tmp;
  ਵਾਪਸ Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ਸਥਿਰ ਖਾਲੀ trim_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ curl_slist *ਸਿਰ)
{
  ਬਣਤਰ curl_slist *l;
  ਲਈ(l = ਸਿਰ; l; l = l->ਅਗਲਾ) {
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ; /* to read from */
    ਅੱਖਰ *store;
    size_t colon = strcspn(l->ਅੰਕੜੇ, ":");
    Curl_strntolower(l->ਅੰਕੜੇ, l->ਅੰਕੜੇ, colon);

    ਮੁੱਲ = &l->ਅੰਕੜੇ[colon];
    ਜੇ(!*ਮੁੱਲ)
      ਜਾਰੀ;
    ++ਮੁੱਲ;
    store = (ਅੱਖਰ *)CURL_UNCONST(ਮੁੱਲ);

    /* skip leading whitespace */
    curlx_str_passblanks(&ਮੁੱਲ);

    ਜਦੋਂ(*ਮੁੱਲ) {
      ਪੂਰਨ space = 0;
      ਜਦੋਂ(ISBLANK(*ਮੁੱਲ)) {
        ਮੁੱਲ++;
        space++;
      }
      ਜੇ(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        ਜੇ(*ਮੁੱਲ)
          *store++ = ' ';
      }
      ਨਹੀਂਤਾਂ
        *store++ = *ਮੁੱਲ++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ਸਥਿਰ ਖਾਲੀ pair_array_free(ਬਣਤਰ pair *pair_array, size_t ਅੰਕ_elements)
{
  size_t ਸੂਚਕਾਂਕ;

  ਲਈ(ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ != ਅੰਕ_elements; ਸੂਚਕਾਂਕ++) {
    curlx_dyn_free(&pair_array[ਸੂਚਕਾਂਕ].ਕੁੰਜੀ);
    curlx_dyn_free(&pair_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ਸਥਿਰ ਖਾਲੀ dyn_array_free(ਬਣਤਰ dynbuf *db, size_t ਅੰਕ_elements)
{
  size_t ਸੂਚਕਾਂਕ;

  ਲਈ(ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < ਅੰਕ_elements; ਸੂਚਕਾਂਕ++)
    curlx_dyn_free((&db[ਸੂਚਕਾਂਕ]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵੰਡੋ_BY '&'

ਸਥਿਰ CURLcode ਵੰਡੋ_ਮੰਜ਼ਿਲ_dyn_array(ਅਚਲ ਅੱਖਰ *ਸਰੋਤ,
                                   ਬਣਤਰ dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *ਅੰਕ_splits_ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t len = ਲੜੀ_ਲੰਬਾਈ(ਸਰੋਤ);
  size_t pos;         /* Position in result buffer */
  size_t ਸ਼ੁਰੂ = 0;   /* Start of current segment */
  size_t segment_ਲੰਮਾਈ = 0;
  size_t ਸੂਚਕਾਂਕ = 0;
  size_t ਅੰਕ_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  ਲਈ(pos = 0; pos < len; pos++) {
    ਜੇ(ਸਰੋਤ[pos] == ਵੰਡੋ_BY) {
      ਜੇ(segment_ਲੰਮਾਈ) {
        curlx_dyn_init(&db[ਸੂਚਕਾਂਕ], segment_ਲੰਮਾਈ + 1);
        ਨਤੀਜਾ = curlx_dyn_addn(&db[ਸੂਚਕਾਂਕ], &ਸਰੋਤ[ਸ਼ੁਰੂ], segment_ਲੰਮਾਈ);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ fail;

        segment_ਲੰਮਾਈ = 0;
        ਸੂਚਕਾਂਕ++;
        ਜੇ(++ਅੰਕ_splits == MAX_QUERY_COMPONENTS) {
          ਨਤੀਜਾ = CURLE_TOO_LARGE;
          ਜਾਓ fail;
        }
      }
      ਸ਼ੁਰੂ = pos + 1;
    }
    ਨਹੀਂਤਾਂ {
      segment_ਲੰਮਾਈ++;
    }
  }

  ਜੇ(segment_ਲੰਮਾਈ) {
    curlx_dyn_init(&db[ਸੂਚਕਾਂਕ], segment_ਲੰਮਾਈ + 1);
    ਨਤੀਜਾ = curlx_dyn_addn(&db[ਸੂਚਕਾਂਕ], &ਸਰੋਤ[ਸ਼ੁਰੂ], segment_ਲੰਮਾਈ);
    ਜੇ(!ਨਤੀਜਾ && ++ਅੰਕ_splits == MAX_QUERY_COMPONENTS)
      ਨਤੀਜਾ = CURLE_TOO_LARGE;
  }
fail:
  *ਅੰਕ_splits_ਬਾਹਰਲਾ = ਅੰਕ_splits;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool is_reserved_char(ਅਚਲ ਅੱਖਰ c)
{
  ਵਾਪਸ (ISALNUM(c) || ISURLPUNTCS(c));
}

ਸਥਿਰ CURLcode uri_encode_ਰਸਤਾ(ਬਣਤਰ Curl_str *original_ਰਸਤਾ,
                                ਬਣਤਰ dynbuf *new_ਰਸਤਾ)
{
  ਅਚਲ ਅੱਖਰ *p = curlx_str(original_ਰਸਤਾ);
  size_t i;

  ਲਈ(i = 0; i < curlx_strlen(original_ਰਸਤਾ); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode ਨਤੀਜਾ = CURLE_OK;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = p[i];
    ਜੇ(is_reserved_char(c) || c == '/')
      ਨਤੀਜਾ = curlx_dyn_addn(new_ਰਸਤਾ, &c, 1);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = curlx_dyn_addf(new_ਰਸਤਾ, "%%%02X", c);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਵਾਪਸ CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ਸਥਿਰ CURLcode normalize_query(ਅਚਲ ਅੱਖਰ *string, size_t len,
                                ਬਣਤਰ dynbuf *db)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(len && !ਨਤੀਜਾ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਵਿੱਚ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*string;
    ਜੇ(('%' == ਵਿੱਚ) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      ਵਿੱਚ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      ਜੇ(ਵਿੱਚ == '+') {
        /* decodes to plus, so leave this encoded */
        ਨਤੀਜਾ = curlx_dyn_addn(db, "%2B", 3);
        ਜਾਰੀ;
      }
    }
    ਨਹੀਂਤਾਂ {
      string++;
      len--;
    }

    ਜੇ(is_reserved_char(ਵਿੱਚ))
      /* Escape unreserved chars from RFC 3986 */
      ਨਤੀਜਾ = curlx_dyn_addn(db, &ਵਿੱਚ, 1);
    ਨਹੀਂਤਾਂ ਜੇ(ਵਿੱਚ == '+')
      /* Encode '+' as space */
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(db, "%20");
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = curlx_dyn_addf(db, "%%%02X", ਵਿੱਚ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool should_urlencode(ਬਣਤਰ Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  ਜੇ(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

/* maximum length for the aws sivg4 parts */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_SIGV4_LEN    64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DATE_HDR_ਕੁੰਜੀ_LEN (MAX_SIGV4_LEN + ਆਕਾਰ("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DATE_ਭਰਿਆ_HDR_LEN (DATE_HDR_ਕੁੰਜੀ_LEN + TIMESTAMP_ਮਾਪ + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ਸਥਿਰ ਪੂਰਨ compare_ਸਿਰਲੇਖ_ਨਾਂ(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b)
{
  ਅਚਲ ਅੱਖਰ *colon_a;
  ਅਚਲ ਅੱਖਰ *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  ਪੂਰਨ cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : ਲੜੀ_ਲੰਬਾਈ(a);
  len_b = colon_b ? (size_t)(colon_b - b) : ਲੜੀ_ਲੰਬਾਈ(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = ਲੜੀ_ਤੁਲਨਾ(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  ਜੇ(!cmp)
    ਵਾਪਸ (ਪੂਰਨ)(len_a - len_b);

  ਵਾਪਸ cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ਸਥਿਰ CURLcode ਮਿਲਾਓ_duplicate_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ curl_slist *ਸਿਰ)
{
  ਬਣਤਰ curl_slist *curr = ਸਿਰ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(curr) {
    ਬਣਤਰ curl_slist *ਅਗਲਾ = curr->ਅਗਲਾ;
    ਜੇ(!ਅਗਲਾ)
      ਰੋਕੋ;

    ਜੇ(compare_ਸਿਰਲੇਖ_ਨਾਂ(curr->ਅੰਕੜੇ, ਅਗਲਾ->ਅੰਕੜੇ) == 0) {
      ਬਣਤਰ dynbuf buf;
      ਅਚਲ ਅੱਖਰ *colon_ਅਗਲਾ;
      ਅਚਲ ਅੱਖਰ *val_ਅਗਲਾ;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_ਸਿਰਲੇਖ);

      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&buf, curr->ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      colon_ਅਗਲਾ = strchr(ਅਗਲਾ->ਅੰਕੜੇ, ':');
      DEBUGASSERT(colon_ਅਗਲਾ);
      val_ਅਗਲਾ = colon_ਅਗਲਾ + 1;

      ਨਤੀਜਾ = curlx_dyn_addn(&buf, ",", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&buf, val_ਅਗਲਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      curlx_free(curr->ਅੰਕੜੇ);
      curr->ਅੰਕੜੇ = curlx_dyn_ptr(&buf);

      curr->ਅਗਲਾ = ਅਗਲਾ->ਅਗਲਾ;
      curlx_free(ਅਗਲਾ->ਅੰਕੜੇ);
      curlx_free(ਅਗਲਾ);
    }
    ਨਹੀਂਤਾਂ {
      curr = curr->ਅਗਲਾ;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ਸਥਿਰ CURLcode make_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅੱਖਰ *timestamp,
                             ਅਚਲ ਅੱਖਰ *provider1,
                             size_t plen, /* length of provider1 */
                             ਅੱਖਰ **date_ਸਿਰਲੇਖ,
                             ਅੱਖਰ *content_sha256_ਸਿਰਲੇਖ,
                             ਬਣਤਰ dynbuf *canonical_ਸਿਰਲੇਖਾਂ,
                             ਬਣਤਰ dynbuf *signed_ਸਿਰਲੇਖਾਂ)
{
  ਅੱਖਰ date_hdr_ਕੁੰਜੀ[DATE_HDR_ਕੁੰਜੀ_LEN];
  ਅੱਖਰ date_ਭਰਿਆ_hdr[DATE_ਭਰਿਆ_HDR_LEN];
  ਬਣਤਰ curl_slist *ਸਿਰ = ਨਿਰਾ;
  ਬਣਤਰ curl_slist *tmp_ਸਿਰ = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਬਣਤਰ curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_ਕੁੰਜੀ, DATE_HDR_ਕੁੰਜੀ_LEN, "X-%.*s-Date",
                 (ਪੂਰਨ)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_ਕੁੰਜੀ[2], provider1, plen);
  date_hdr_ਕੁੰਜੀ[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_ਭਰਿਆ_hdr, DATE_ਭਰਿਆ_HDR_LEN,
                 "x-%.*s-date:%s", (ਪੂਰਨ)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_ਭਰਿਆ_hdr[2], provider1, plen);

  ਜੇ(!Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Host")) &&
     ਅੰਕੜੇ->state.http_host) {
    /* Host: [host]:[port] */
    ਅੱਖਰ *fullhost = curlx_strdup(ਅੰਕੜੇ->state.http_host);

    ਜੇ(fullhost)
      ਸਿਰ = Curl_slist_append_nodup(ਨਿਰਾ, fullhost);
    ਜੇ(!ਸਿਰ) {
      curlx_free(fullhost);
      ਜਾਓ fail;
    }
  }

  ਜੇ(*content_sha256_ਸਿਰਲੇਖ) {
    tmp_ਸਿਰ = curl_slist_append(ਸਿਰ, content_sha256_ਸਿਰਲੇਖ);
    ਜੇ(!tmp_ਸਿਰ)
      ਜਾਓ fail;
    ਸਿਰ = tmp_ਸਿਰ;
  }

  /* copy user headers to our header list. the logic is based on how http.c
     handles user headers.

     user headers in format 'name:' with no value are used to signal that an
     internal header of that name should be removed. those user headers are not
     added to this list.

     user headers in format 'name;' with no value are used to signal that a
     header of that name with no value should be sent. those user headers are
     added to this list but in the format that they will be sent, ie the
     semi-colon is changed to a colon for format 'name:'.

     user headers with a value of whitespace only, or without a colon or
     semi-colon, are not added to this list.
     */
  ਲਈ(l = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ; l; l = l->ਅਗਲਾ) {
    ਅੱਖਰ *dupdata;
    ਅਚਲ ਅੱਖਰ *ptr;
    ਅਚਲ ਅੱਖਰ *sep = strchr(l->ਅੰਕੜੇ, ':');
    ਜੇ(!sep)
      sep = strchr(l->ਅੰਕੜੇ, ';');
    ਜੇ(!sep || (*sep == ':' && !*(sep + 1)))
      ਜਾਰੀ;
    ਲਈ(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    ਜੇ(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      ਜਾਰੀ;
    dupdata = curlx_strdup(l->ਅੰਕੜੇ);
    ਜੇ(!dupdata)
      ਜਾਓ fail;
    dupdata[sep - l->ਅੰਕੜੇ] = ':';
    tmp_ਸਿਰ = Curl_slist_append_nodup(ਸਿਰ, dupdata);
    ਜੇ(!tmp_ਸਿਰ) {
      curlx_free(dupdata);
      ਜਾਓ fail;
    }
    ਸਿਰ = tmp_ਸਿਰ;
  }

  trim_ਸਿਰਲੇਖਾਂ(ਸਿਰ);

  *date_ਸਿਰਲੇਖ = ਲੱਭੋ_date_hdr(ਅੰਕੜੇ, date_hdr_ਕੁੰਜੀ);
  ਜੇ(!*date_ਸਿਰਲੇਖ) {
    tmp_ਸਿਰ = curl_slist_append(ਸਿਰ, date_ਭਰਿਆ_hdr);
    ਜੇ(!tmp_ਸਿਰ)
      ਜਾਓ fail;
    ਸਿਰ = tmp_ਸਿਰ;
    *date_ਸਿਰਲੇਖ = curl_maprintf("%s: %s\r\n", date_hdr_ਕੁੰਜੀ, timestamp);
    ਜੇ(!*date_ਸਿਰਲੇਖ)
      ਜਾਓ fail;
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;
    ਅਚਲ ਅੱਖਰ *endp;
    ਮੁੱਲ = strchr(*date_ਸਿਰਲੇਖ, ':');
    ਜੇ(!ਮੁੱਲ) {
      *date_ਸਿਰਲੇਖ = ਨਿਰਾ;
      ਜਾਓ fail;
    }
    ++ਮੁੱਲ;
    curlx_str_passblanks(&ਮੁੱਲ);
    endp = ਮੁੱਲ;
    ਜਦੋਂ(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    ਜੇ((endp - ਮੁੱਲ) == TIMESTAMP_ਮਾਪ - 1) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(timestamp, ਮੁੱਲ, TIMESTAMP_ਮਾਪ - 1);
      timestamp[TIMESTAMP_ਮਾਪ - 1] = 0;
    }
    ਨਹੀਂਤਾਂ
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_ਸਿਰਲੇਖ = ਨਿਰਾ;
  }

  /* alpha-sort by header name in a case sensitive manner */
  ਕਰੋ {
    again = FALSE;
    ਲਈ(l = ਸਿਰ; l; l = l->ਅਗਲਾ) {
      ਬਣਤਰ curl_slist *ਅਗਲਾ = l->ਅਗਲਾ;

      ਜੇ(ਅਗਲਾ && compare_ਸਿਰਲੇਖ_ਨਾਂ(l->ਅੰਕੜੇ, ਅਗਲਾ->ਅੰਕੜੇ) > 0) {
        ਅੱਖਰ *tmp = l->ਅੰਕੜੇ;

        l->ਅੰਕੜੇ = ਅਗਲਾ->ਅੰਕੜੇ;
        ਅਗਲਾ->ਅੰਕੜੇ = tmp;
        again = TRUE;
      }
    }
  } ਜਦੋਂ(again);

  ਨਤੀਜਾ = ਮਿਲਾਓ_duplicate_ਸਿਰਲੇਖਾਂ(ਸਿਰ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਲਈ(l = ਸਿਰ; l; l = l->ਅਗਲਾ) {
    ਅੱਖਰ *tmp;

    ਜੇ(curlx_dyn_ਜੋੜੋ(canonical_ਸਿਰਲੇਖਾਂ, l->ਅੰਕੜੇ))
      ਜਾਓ fail;
    ਜੇ(curlx_dyn_ਜੋੜੋ(canonical_ਸਿਰਲੇਖਾਂ, "\n"))
      ਜਾਓ fail;

    tmp = strchr(l->ਅੰਕੜੇ, ':');
    ਜੇ(tmp)
      *tmp = 0;

    ਜੇ(l != ਸਿਰ && curlx_dyn_ਜੋੜੋ(signed_ਸਿਰਲੇਖਾਂ, ";"))
      ਜਾਓ fail;
    ਜੇ(curlx_dyn_ਜੋੜੋ(signed_ਸਿਰਲੇਖਾਂ, l->ਅੰਕੜੇ))
      ਜਾਓ fail;
  }

  ਨਤੀਜਾ = CURLE_OK;
fail:
  curl_slist_free_all(ਸਿਰ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONTENT_SHA256_ਕੁੰਜੀ_LEN (MAX_SIGV4_LEN + ਆਕਾਰ("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_ਕੁੰਜੀ_LEN + 2 + SHA256_HEX_ਲੰਮਾਈ)

/* try to parse a payload hash from the content-sha256 header */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *parse_content_sha_hdr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਅਚਲ ਅੱਖਰ *provider1,
                                         size_t plen,
                                         size_t *ਮੁੱਲ_len)
{
  ਅੱਖਰ ਕੁੰਜੀ[CONTENT_SHA256_ਕੁੰਜੀ_LEN];
  size_t ਕੁੰਜੀ_len;
  ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;
  size_t len;

  ਕੁੰਜੀ_len = curl_msnprintf(ਕੁੰਜੀ, ਆਕਾਰ(ਕੁੰਜੀ), "x-%.*s-content-sha256",
                           (ਪੂਰਨ)plen, provider1);

  ਮੁੱਲ = Curl_checkheaders(ਅੰਕੜੇ, ਕੁੰਜੀ, ਕੁੰਜੀ_len);
  ਜੇ(!ਮੁੱਲ)
    ਵਾਪਸ ਨਿਰਾ;

  ਮੁੱਲ = strchr(ਮੁੱਲ, ':');
  ਜੇ(!ਮੁੱਲ)
    ਵਾਪਸ ਨਿਰਾ;
  ++ਮੁੱਲ;

  curlx_str_passblanks(&ਮੁੱਲ);

  len = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);
  ਜਦੋਂ(len > 0 && ISBLANK(ਮੁੱਲ[len - 1]))
    --len;

  *ਮੁੱਲ_len = len;
  ਵਾਪਸ ਮੁੱਲ;
}

ਸਥਿਰ CURLcode calc_payload_hash(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha_hash, ਅੱਖਰ *sha_hex)
{
  ਅਚਲ ਅੱਖਰ *post_ਅੰਕੜੇ = ਅੰਕੜੇ->ਸੈੱਟ.postfields;
  size_t post_ਅੰਕੜੇ_len = 0;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(post_ਅੰਕੜੇ) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postfieldsize < 0)
      post_ਅੰਕੜੇ_len = ਲੜੀ_ਲੰਬਾਈ(post_ਅੰਕੜੇ);
    ਨਹੀਂਤਾਂ
      post_ਅੰਕੜੇ_len = (size_t)ਅੰਕੜੇ->ਸੈੱਟ.postfieldsize;
  }
  ਨਤੀਜਾ = Curl_sha256it(sha_hash, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)post_ਅੰਕੜੇ,
                         post_ਅੰਕੜੇ_len);
  ਜੇ(!ਨਤੀਜਾ)
    sha256_ਮੰਜ਼ਿਲ_hex(sha_hex, sha_hash);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ਸਥਿਰ CURLcode calc_s3_payload_hash(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     Curl_HttpReq httpreq,
                                     ਅਚਲ ਅੱਖਰ *provider1,
                                     size_t plen,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha_hash,
                                     ਅੱਖਰ *sha_hex, ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  bool ਖਾਲੀ_ਥਾਵਾਂ_ਵਿਧੀ = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_ਸਿਰ);
  /* The request method or filesize indicate no request payload */
  bool ਖਾਲੀ_ਥਾਵਾਂ_payload = (ਖਾਲੀ_ਥਾਵਾਂ_ਵਿਧੀ || ਅੰਕੜੇ->ਸੈੱਟ.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && ਅੰਕੜੇ->ਸੈੱਟ.postfields);
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(ਖਾਲੀ_ਥਾਵਾਂ_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    ਨਤੀਜਾ = calc_payload_hash(ਅੰਕੜੇ, sha_hash, sha_hex);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ fail;
  }
  ਨਹੀਂਤਾਂ {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_ਲੰਮਾਈ); /* 16 < 65 */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(ਸਿਰਲੇਖ, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (ਪੂਰਨ)plen, provider1, sha_hex);

  ਨਤੀਜਾ = CURLE_OK;
fail:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ compare_func(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
  ਅਚਲ ਬਣਤਰ pair *aa = a;
  ਅਚਲ ਬਣਤਰ pair *bb = b;
  ਅਚਲ size_t aa_ਕੁੰਜੀ_len = curlx_dyn_len(&aa->ਕੁੰਜੀ);
  ਅਚਲ size_t bb_ਕੁੰਜੀ_len = curlx_dyn_len(&bb->ਕੁੰਜੀ);
  ਅਚਲ size_t aa_ਮੁੱਲ_len = curlx_dyn_len(&aa->ਮੁੱਲ);
  ਅਚਲ size_t bb_ਮੁੱਲ_len = curlx_dyn_len(&bb->ਮੁੱਲ);
  ਪੂਰਨ compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  ਜੇ((aa_ਕੁੰਜੀ_len == 0) && (bb_ਕੁੰਜੀ_len == 0))
    ਵਾਪਸ 0;
  ਜੇ(aa_ਕੁੰਜੀ_len == 0)
    ਵਾਪਸ -1;
  ਜੇ(bb_ਕੁੰਜੀ_len == 0)
    ਵਾਪਸ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ਕੁੰਜੀ), curlx_dyn_ptr(&bb->ਕੁੰਜੀ));
  ਜੇ(compare) {
    ਵਾਪਸ compare;
  }

  /* Compare values */
  ਜੇ((aa_ਮੁੱਲ_len == 0) && (bb_ਮੁੱਲ_len == 0))
    ਵਾਪਸ 0;
  ਜੇ(aa_ਮੁੱਲ_len == 0)
    ਵਾਪਸ -1;
  ਜੇ(bb_ਮੁੱਲ_len == 0)
    ਵਾਪਸ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ਮੁੱਲ), curlx_dyn_ptr(&bb->ਮੁੱਲ));

  ਵਾਪਸ compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *q, size_t len,
                             ਬਣਤਰ dynbuf *new_ਰਸਤਾ,
                             bool do_uri_encode);
UNITTEST CURLcode canon_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *q, size_t len,
                             ਬਣਤਰ dynbuf *new_ਰਸਤਾ,
                             bool do_uri_encode)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਬਣਤਰ Curl_str original_ਰਸਤਾ;

  curlx_str_ਸੌਂਪੋ(&original_ਰਸਤਾ, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  ਜੇ(do_uri_encode)
    ਨਤੀਜਾ = uri_encode_ਰਸਤਾ(&original_ਰਸਤਾ, new_ਰਸਤਾ);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = curlx_dyn_addn(new_ਰਸਤਾ, q, len);

  ਜੇ(!ਨਤੀਜਾ && curlx_dyn_len(new_ਰਸਤਾ) == 0)
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(new_ਰਸਤਾ, "/");

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ਅਚਲ ਅੱਖਰ *query, ਬਣਤਰ dynbuf *dq);
UNITTEST CURLcode canon_query(ਅਚਲ ਅੱਖਰ *query, ਬਣਤਰ dynbuf *dq)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਬਣਤਰ dynbuf query_array[MAX_QUERY_COMPONENTS];
  ਬਣਤਰ pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t ਅੰਕ_query_components;
  size_t counted_query_components = 0;
  size_t ਸੂਚਕਾਂਕ;

  ਜੇ(!query)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = ਵੰਡੋ_ਮੰਜ਼ਿਲ_dyn_array(query, &query_array[0], &ਅੰਕ_query_components);
  ਜੇ(ਨਤੀਜਾ) {
    ਜਾਓ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  ਲਈ(ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < ਅੰਕ_query_components; ਸੂਚਕਾਂਕ++) {
    ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_ਕੁੰਜੀ;
    size_t ਵਿੱਚ_ਕੁੰਜੀ_len;
    ਅਚਲ ਅੱਖਰ *ਵਿੱਥ;
    size_t query_part_len = curlx_dyn_len(&query_array[ਸੂਚਕਾਂਕ]);
    ਅਚਲ ਅੱਖਰ *query_part = curlx_dyn_ptr(&query_array[ਸੂਚਕਾਂਕ]);

    ਵਿੱਚ_ਕੁੰਜੀ = query_part;

    ਵਿੱਥ = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    ਜੇ(!ਵਿੱਥ) {
      ਵਿੱਚ_ਕੁੰਜੀ_len = ਲੜੀ_ਲੰਬਾਈ(ਵਿੱਚ_ਕੁੰਜੀ);
    }
    ਨਹੀਂਤਾਂ {
      ਵਿੱਚ_ਕੁੰਜੀ_len = ਵਿੱਥ - ਵਿੱਚ_ਕੁੰਜੀ;
    }

    curlx_dyn_init(&encoded_query_array[ਸੂਚਕਾਂਕ].ਕੁੰਜੀ,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    ਨਤੀਜਾ = normalize_query(ਵਿੱਚ_ਕੁੰਜੀ, ਵਿੱਚ_ਕੁੰਜੀ_len,
                             &encoded_query_array[ਸੂਚਕਾਂਕ].ਕੁੰਜੀ);
    ਜੇ(ਨਤੀਜਾ) {
      ਜਾਓ fail;
    }

    /* Decode/encode the value if it exists */
    ਜੇ(ਵਿੱਥ && ਵਿੱਥ != (query_part + query_part_len - 1)) {
      size_t ਵਿੱਚ_ਮੁੱਲ_len;
      ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_ਮੁੱਲ = ਵਿੱਥ + 1;
      ਵਿੱਚ_ਮੁੱਲ_len = query_part + query_part_len - (ਵਿੱਥ + 1);
      ਨਤੀਜਾ = normalize_query(ਵਿੱਚ_ਮੁੱਲ, ਵਿੱਚ_ਮੁੱਲ_len,
                               &encoded_query_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ);
      ਜੇ(ਨਤੀਜਾ) {
        ਜਾਓ fail;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ, 2);
      ਨਤੀਜਾ = curlx_dyn_addn(&encoded_query_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ, "", 1);
    }

    ਜੇ(ਨਤੀਜਾ) {
      ਜਾਓ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, ਅੰਕ_query_components,
        ਆਕਾਰ(ਬਣਤਰ pair), compare_func);

  /* Append the query components together to make a full query string */
  ਲਈ(ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < ਅੰਕ_query_components; ਸੂਚਕਾਂਕ++) {

    ਜੇ(ਸੂਚਕਾਂਕ)
      ਨਤੀਜਾ = curlx_dyn_addn(dq, "&", 1);
    ਜੇ(!ਨਤੀਜਾ) {
      ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_ptr = curlx_dyn_ptr(&encoded_query_array[ਸੂਚਕਾਂਕ].ਕੁੰਜੀ);
      ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_ptr = curlx_dyn_ptr(&encoded_query_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ);
      size_t vlen = curlx_dyn_len(&encoded_query_array[ਸੂਚਕਾਂਕ].ਮੁੱਲ);
      ਜੇ(ਮੁੱਲ_ptr && vlen) {
        ਨਤੀਜਾ = curlx_dyn_addf(dq, "%s=%s", ਕੁੰਜੀ_ptr, ਮੁੱਲ_ptr);
      }
      ਨਹੀਂਤਾਂ {
        /* Empty value is always encoded to key= */
        ਨਤੀਜਾ = curlx_dyn_addf(dq, "%s=", ਕੁੰਜੀ_ptr);
      }
    }
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
  }

fail:
  ਜੇ(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], ਅੰਕ_query_components);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode parse_sigv4_params(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਅੱਖਰ *hostname,
                                   ਬਣਤਰ Curl_str *provider0,
                                   ਬਣਤਰ Curl_str *provider1,
                                   ਬਣਤਰ Curl_str *region,
                                   ਬਣਤਰ Curl_str *service)
{
  ਅਚਲ ਅੱਖਰ *ਸਤਰ = CURL_EASY_STR(ਅੰਕੜੇ, STRING_AWS_SIGV4);
  ਜੇ(!ਸਤਰ || !*ਸਤਰ)
    ਸਤਰ = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  ਜੇ(curlx_str_until(&ਸਤਰ, provider0, MAX_SIGV4_LEN, ':')) {
    failf(ਅੰਕੜੇ, "first aws-sigv4 provider cannot be empty");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਜੇ(curlx_str_single(&ਸਤਰ, ':') ||
     curlx_str_until(&ਸਤਰ, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  ਨਹੀਂਤਾਂ ਜੇ(curlx_str_single(&ਸਤਰ, ':') ||
          curlx_str_until(&ਸਤਰ, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&ਸਤਰ, ':') ||
          curlx_str_until(&ਸਤਰ, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  ਜੇ(!curlx_strlen(service)) {
    ਅਚਲ ਅੱਖਰ *p = hostname;
    ਜੇ(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(ਅੰਕੜੇ, "aws-sigv4: service missing in parameters and hostname");
      ਵਾਪਸ CURLE_URL_MALFORMAT;
    }

    infof(ਅੰਕੜੇ, "aws_sigv4: picked service %.*s from host",
          (ਪੂਰਨ)curlx_strlen(service), curlx_str(service));

    ਜੇ(!curlx_strlen(region)) {
      ਜੇ(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(ਅੰਕੜੇ, "aws-sigv4: region missing in parameters and hostname");
        ਵਾਪਸ CURLE_URL_MALFORMAT;
      }
      infof(ਅੰਕੜੇ, "aws_sigv4: picked region %.*s from host",
            (ਪੂਰਨ)curlx_strlen(region), curlx_str(region));
    }
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode get_payload_hash(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 Curl_HttpReq httpreq,
                                 ਬਣਤਰ Curl_str *provider0,
                                 ਬਣਤਰ Curl_str *provider1,
                                 ਬਣਤਰ Curl_str *service,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha_hash,
                                 ਅੱਖਰ *sha_hex,
                                 ਅੱਖਰ *content_sha256_hdr,
                                 ਅਚਲ ਅੱਖਰ **payload_hash_ਬਾਹਰਲਾ,
                                 size_t *payload_hash_len_ਬਾਹਰਲਾ)
{
  *payload_hash_ਬਾਹਰਲਾ =
    parse_content_sha_hdr(ਅੰਕੜੇ, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_ਬਾਹਰਲਾ);

  ਜੇ(!*payload_hash_ਬਾਹਰਲਾ) {
    CURLcode ਨਤੀਜਾ;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    ਜੇ(sign_as_s3)
      ਨਤੀਜਾ = calc_s3_payload_hash(ਅੰਕੜੇ, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = calc_payload_hash(ਅੰਕੜੇ, sha_hash, sha_hex);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    *payload_hash_ਬਾਹਰਲਾ = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_ਬਾਹਰਲਾ = ਲੜੀ_ਲੰਬਾਈ(sha_hex);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode get_timestamp(ਅੱਖਰ *timestamp, size_t stampsize)
{
  time_t clock;
  ਬਣਤਰ tm tm;
  CURLcode ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅੱਖਰ *force_timestamp = getenv("CURL_FORCETIME");
    ਜੇ(force_timestamp)
      clock = 0;
    ਨਹੀਂਤਾਂ
      clock = ਸਮਾਂ(ਨਿਰਾ);
  }
#ਨਹੀਂਤਾਂ
  clock = ਸਮਾਂ(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
  ਨਤੀਜਾ = curlx_gmtime(clock, &tm);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode make_canonical_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਅੱਖਰ *timestamp,
                                       ਬਣਤਰ Curl_str *provider1,
                                       ਬਣਤਰ Curl_str *service,
                                       ਅਚਲ ਅੱਖਰ *ਵਿਧੀ,
                                       ਅਚਲ ਅੱਖਰ *payload_hash,
                                       size_t payload_hash_len,
                                       ਅੱਖਰ **date_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ,
                                       ਅੱਖਰ *content_sha256_hdr,
                                       ਬਣਤਰ dynbuf *canonical_ਸਿਰਲੇਖਾਂ,
                                       ਬਣਤਰ dynbuf *signed_ਸਿਰਲੇਖਾਂ,
                                       ਅੱਖਰ **canonical_request_ਬਾਹਰਲਾ)
{
  ਬਣਤਰ dynbuf canonical_query;
  ਬਣਤਰ dynbuf canonical_ਰਸਤਾ;
  CURLcode ਨਤੀਜਾ;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  curlx_dyn_init(&canonical_ਰਸਤਾ, CURL_MAX_HTTP_ਸਿਰਲੇਖ);

  ਨਤੀਜਾ = make_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, content_sha256_hdr,
                        canonical_ਸਿਰਲੇਖਾਂ, signed_ਸਿਰਲੇਖਾਂ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਨਤੀਜਾ = canon_query(ਅੰਕੜੇ->state.ਉੱਪਰ.query, &canonical_query);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  ਨਤੀਜਾ = canon_ਰਸਤਾ(ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ, ਲੜੀ_ਲੰਬਾਈ(ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ),
                      &canonical_ਰਸਤਾ,
                      should_urlencode(service));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  *canonical_request_ਬਾਹਰਲਾ =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  ਵਿਧੀ,
                  curlx_dyn_ptr(&canonical_ਰਸਤਾ),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_ਸਿਰਲੇਖਾਂ),
                  curlx_dyn_ptr(signed_ਸਿਰਲੇਖਾਂ),
                  (ਪੂਰਨ)payload_hash_len, payload_hash);
  ਜੇ(!*canonical_request_ਬਾਹਰਲਾ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  ਨਤੀਜਾ = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_ਰਸਤਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode make_string_ਮੰਜ਼ਿਲ_sign(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ Curl_str *provider0,
                                    ਬਣਤਰ Curl_str *region,
                                    ਬਣਤਰ Curl_str *service,
                                    ਅਚਲ ਅੱਖਰ *date,
                                    ਅਚਲ ਅੱਖਰ *timestamp,
                                    ਅਚਲ ਅੱਖਰ *canonical_request,
                                    ਅੱਖਰ **request_type_ਬਾਹਰਲਾ,
                                    ਅੱਖਰ **credential_scope_ਬਾਹਰਲਾ,
                                    ਅੱਖਰ **str_ਮੰਜ਼ਿਲ_sign_ਬਾਹਰਲਾ)
{
  ਅੱਖਰ *request_type;
  ਅੱਖਰ *credential_scope;
  ਅੱਖਰ *str_ਮੰਜ਼ਿਲ_sign;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ sha_hash[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
  ਅੱਖਰ sha_hex[SHA256_HEX_ਲੰਮਾਈ];

  request_type = curl_maprintf("%.*s4_request",
                               (ਪੂਰਨ)curlx_strlen(provider0),
                               curlx_str(provider0));
  ਜੇ(!request_type)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (ਪੂਰਨ)curlx_strlen(region),
                                   curlx_str(region),
                                   (ਪੂਰਨ)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  ਜੇ(!credential_scope) {
    curlx_free(request_type);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(Curl_sha256it(sha_hash, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)canonical_request,
                   ਲੜੀ_ਲੰਬਾਈ(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  sha256_ਮੰਜ਼ਿਲ_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_ਮੰਜ਼ਿਲ_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (ਪੂਰਨ)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  ਜੇ(!str_ਮੰਜ਼ਿਲ_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_ਮੰਜ਼ਿਲ_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(ਅੰਕੜੇ, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_ਮੰਜ਼ਿਲ_sign);

  *request_type_ਬਾਹਰਲਾ = request_type;
  *credential_scope_ਬਾਹਰਲਾ = credential_scope;
  *str_ਮੰਜ਼ਿਲ_sign_ਬਾਹਰਲਾ = str_ਮੰਜ਼ਿਲ_sign;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode sign_and_ਸੈੱਟ_auth_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_str *provider0,
                                          ਬਣਤਰ Curl_str *region,
                                          ਬਣਤਰ Curl_str *service,
                                          ਅਚਲ ਅੱਖਰ *request_type,
                                          ਅਚਲ ਅੱਖਰ *credential_scope,
                                          ਅਚਲ ਅੱਖਰ *date,
                                          ਅਚਲ ਅੱਖਰ *str_ਮੰਜ਼ਿਲ_sign,
                                          ਅਚਲ ਅੱਖਰ *date_ਸਿਰਲੇਖ,
                                          ਅਚਲ ਅੱਖਰ *content_sha256_hdr,
                                          ਬਣਤਰ dynbuf *signed_ਸਿਰਲੇਖਾਂ)
{
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਅਚਲ ਅੱਖਰ *passwd = Curl_creds_passwd(ਅੰਕੜੇ->state.creds);
  ਅੱਖਰ *secret = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ sign0[CURL_SHA256_DIGEST_ਲੰਮਾਈ] = { 0 };
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ sign1[CURL_SHA256_DIGEST_ਲੰਮਾਈ] = { 0 };
  ਅੱਖਰ sha_hex[SHA256_HEX_ਲੰਮਾਈ];
  ਅੱਖਰ *auth_ਸਿਰਲੇਖਾਂ = ਨਿਰਾ;
  ਅੱਖਰ *user = curl_escape(Curl_creds_user(ਅੰਕੜੇ->state.creds), 0);
  ਜੇ(!user)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  secret = curl_maprintf("%.*s4%s", (ਪੂਰਨ)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  ਜੇ(!secret)
    ਜਾਓ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, ਲੜੀ_ਲੰਬਾਈ(secret), date, ਲੜੀ_ਲੰਬਾਈ(date), sign0);
  HMAC_SHA256(sign0, ਆਕਾਰ(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, ਆਕਾਰ(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, ਆਕਾਰ(sign0),
              request_type, ਲੜੀ_ਲੰਬਾਈ(request_type), sign1);
  HMAC_SHA256(sign1, ਆਕਾਰ(sign1),
              str_ਮੰਜ਼ਿਲ_sign, ਲੜੀ_ਲੰਬਾਈ(str_ਮੰਜ਼ਿਲ_sign), sign0);

  sha256_ਮੰਜ਼ਿਲ_hex(sha_hex, sign0);

  infof(ਅੰਕੜੇ, "aws_sigv4: Signature - %s", sha_hex);

  auth_ਸਿਰਲੇਖਾਂ = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (ਪੂਰਨ)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_ਸਿਰਲੇਖਾਂ),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_ਸਿਰਲੇਖ ? date_ਸਿਰਲੇਖ : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  ਜੇ(!auth_ਸਿਰਲੇਖਾਂ)
    ਜਾਓ fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_ਸਿਰਲੇਖਾਂ[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(ਅੰਕੜੇ->req.hd_auth);
  ਅੰਕੜੇ->req.hd_auth = auth_ਸਿਰਲੇਖਾਂ;
  ਅੰਕੜੇ->state.authhost.ਪੂਰਾ = TRUE;
  ਨਤੀਜਾ = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_output_aws_sigv4(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਬਣਤਰ Curl_str provider0 = { ਨਿਰਾ, 0 };
  ਬਣਤਰ Curl_str provider1 = { ਨਿਰਾ, 0 };
  ਬਣਤਰ Curl_str region = { ਨਿਰਾ, 0 };
  ਬਣਤਰ Curl_str service = { ਨਿਰਾ, 0 };
  ਅਚਲ ਅੱਖਰ *hostname = ਅੰਕੜੇ->state.origin->hostname;
  ਅੱਖਰ timestamp[TIMESTAMP_ਮਾਪ];
  ਅੱਖਰ date[9];
  ਬਣਤਰ dynbuf canonical_ਸਿਰਲੇਖਾਂ;
  ਬਣਤਰ dynbuf signed_ਸਿਰਲੇਖਾਂ;
  ਅੱਖਰ *date_ਸਿਰਲੇਖ = ਨਿਰਾ;
  Curl_HttpReq httpreq;
  ਅਚਲ ਅੱਖਰ *ਵਿਧੀ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *payload_hash = ਨਿਰਾ;
  size_t payload_hash_len = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ sha_hash[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
  ਅੱਖਰ sha_hex[SHA256_HEX_ਲੰਮਾਈ];
  ਅੱਖਰ content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  ਅੱਖਰ *canonical_request = ਨਿਰਾ;
  ਅੱਖਰ *request_type = ਨਿਰਾ;
  ਅੱਖਰ *credential_scope = ਨਿਰਾ;
  ਅੱਖਰ *str_ਮੰਜ਼ਿਲ_sign = ਨਿਰਾ;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ਰਸਤਾ_as_is) {
    failf(ਅੰਕੜੇ, "Cannot use sigv4 authentication with path-as-is flag");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਜੇ(Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    ਵਾਪਸ CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_ਸਿਰਲੇਖਾਂ, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  curlx_dyn_init(&signed_ਸਿਰਲੇਖਾਂ, CURL_MAX_HTTP_ਸਿਰਲੇਖ);

  ਨਤੀਜਾ = parse_sigv4_params(ਅੰਕੜੇ, hostname, &provider0, &provider1,
                              &region, &service);
  ਜੇ(!ਨਤੀਜਾ) {
    Curl_http_ਵਿਧੀ(ਅੰਕੜੇ, &ਵਿਧੀ, &httpreq);
    ਨਤੀਜਾ = get_payload_hash(ਅੰਕੜੇ, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = get_timestamp(timestamp, ਆਕਾਰ(timestamp));

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = make_canonical_request(ਅੰਕੜੇ, timestamp,
                                    &provider1, &service,
                                    ਵਿਧੀ, payload_hash, payload_hash_len,
                                    &date_ਸਿਰਲੇਖ, content_sha256_hdr,
                                    &canonical_ਸਿਰਲੇਖਾਂ, &signed_ਸਿਰਲੇਖਾਂ,
                                    &canonical_request);
  ਜੇ(!ਨਤੀਜਾ) {
    /* the timestamp might have been updated in make_canonical_request */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(date, timestamp, ਆਕਾਰ(date) - 1);
    date[ਆਕਾਰ(date) - 1] = 0;

    ਨਤੀਜਾ = make_string_ਮੰਜ਼ਿਲ_sign(ਅੰਕੜੇ, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_ਮੰਜ਼ਿਲ_sign);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = sign_and_ਸੈੱਟ_auth_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_ਮੰਜ਼ਿਲ_sign, date_ਸਿਰਲੇਖ,
                                       content_sha256_hdr, &signed_ਸਿਰਲੇਖਾਂ);

  curlx_dyn_free(&canonical_ਸਿਰਲੇਖਾਂ);
  curlx_dyn_free(&signed_ਸਿਰਲੇਖਾਂ);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_ਮੰਜ਼ਿਲ_sign);
  curlx_free(date_ਸਿਰਲੇਖ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
