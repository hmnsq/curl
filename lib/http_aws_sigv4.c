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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "http_aws_sigv4.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "slist.h"

#ಅನ್ತರ್ಭಾವ <time.h>

#ವ್ಯಾಖ್ಯಾನಿಸು HMAC_SHA256(k, kl, d, dl, o)                 \
  ಮಾಡು {                                               \
    ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)(k), \
                         kl,                         \
                         (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)(d), \
                         dl, o);                     \
    ಆದರೆ(ಫಲಿತಾಂಶ) {                                     \
      ಹೋಗು fail;                                     \
    }                                                \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು TIMESTAMP_ಗಾತ್ರ 17

/* hex-encoded with null-terminator */
#ವ್ಯಾಖ್ಯಾನಿಸು SHA256_HEX_ಉದ್ದಳತೆ ((2 * CURL_SHA256_DIGEST_ಉದ್ದಳತೆ) + 1)

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_QUERY_COMPONENTS 128

ರಚನೆ pair {
  ರಚನೆ dynbuf ಕೀಲಿ;
  ರಚನೆ dynbuf ಮೌಲ್ಯ;
};

ಸ್ಥಿರ ಶೂನ್ಯ sha256_ಗಮ್ಯ_hex(ಅಕ್ಷರ *dst, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_ಉದ್ದಳತೆ,
                 (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)dst, SHA256_HEX_ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ ಅಕ್ಷರ *ಕಂಡುಹಿಡಿ_date_hdr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sig_hdr)
{
  ಅಕ್ಷರ *tmp = Curl_checkheaders(ದತ್ತಾಂಶ, sig_hdr, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sig_hdr));

  ಆದರೆ(tmp)
    ಹಿಂತಿರುಗಿ tmp;
  ಹಿಂತಿರುಗಿ Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ಸ್ಥಿರ ಶೂನ್ಯ trim_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ curl_slist *ತಲೆ)
{
  ರಚನೆ curl_slist *l;
  ಗಾಗಿ(l = ತಲೆ; l; l = l->ಮುಂದಿನ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ; /* to read from */
    ಅಕ್ಷರ *store;
    size_t colon = strcspn(l->ದತ್ತಾಂಶ, ":");
    Curl_strntolower(l->ದತ್ತಾಂಶ, l->ದತ್ತಾಂಶ, colon);

    ಮೌಲ್ಯ = &l->ದತ್ತಾಂಶ[colon];
    ಆದರೆ(!*ಮೌಲ್ಯ)
      ಮುಂದುವರಿ;
    ++ಮೌಲ್ಯ;
    store = (ಅಕ್ಷರ *)CURL_UNCONST(ಮೌಲ್ಯ);

    /* skip leading whitespace */
    curlx_str_passblanks(&ಮೌಲ್ಯ);

    ಆಗ(*ಮೌಲ್ಯ) {
      ಪೂರ್ಣಾಂಕ space = 0;
      ಆಗ(ISBLANK(*ಮೌಲ್ಯ)) {
        ಮೌಲ್ಯ++;
        space++;
      }
      ಆದರೆ(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        ಆದರೆ(*ಮೌಲ್ಯ)
          *store++ = ' ';
      }
      ಇಲ್ಲದಿದ್ದರೆ
        *store++ = *ಮೌಲ್ಯ++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ಸ್ಥಿರ ಶೂನ್ಯ pair_array_free(ರಚನೆ pair *pair_array, size_t ಅಂಕೆ_elements)
{
  size_t ಸೂಚ್ಯಂಕ;

  ಗಾಗಿ(ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ != ಅಂಕೆ_elements; ಸೂಚ್ಯಂಕ++) {
    curlx_dyn_free(&pair_array[ಸೂಚ್ಯಂಕ].ಕೀಲಿ);
    curlx_dyn_free(&pair_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ಸ್ಥಿರ ಶೂನ್ಯ dyn_array_free(ರಚನೆ dynbuf *db, size_t ಅಂಕೆ_elements)
{
  size_t ಸೂಚ್ಯಂಕ;

  ಗಾಗಿ(ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < ಅಂಕೆ_elements; ಸೂಚ್ಯಂಕ++)
    curlx_dyn_free((&db[ಸೂಚ್ಯಂಕ]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#ವ್ಯಾಖ್ಯಾನಿಸು ವಿಭಜಿಸು_BY '&'

ಸ್ಥಿರ CURLcode ವಿಭಜಿಸು_ಗಮ್ಯ_dyn_array(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ,
                                   ರಚನೆ dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *ಅಂಕೆ_splits_ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೂಲ);
  size_t pos;         /* Position in result buffer */
  size_t ಆರಂಭ = 0;   /* Start of current segment */
  size_t segment_ಉದ್ದಳತೆ = 0;
  size_t ಸೂಚ್ಯಂಕ = 0;
  size_t ಅಂಕೆ_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  ಗಾಗಿ(pos = 0; pos < len; pos++) {
    ಆದರೆ(ಮೂಲ[pos] == ವಿಭಜಿಸು_BY) {
      ಆದರೆ(segment_ಉದ್ದಳತೆ) {
        curlx_dyn_init(&db[ಸೂಚ್ಯಂಕ], segment_ಉದ್ದಳತೆ + 1);
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&db[ಸೂಚ್ಯಂಕ], &ಮೂಲ[ಆರಂಭ], segment_ಉದ್ದಳತೆ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು fail;

        segment_ಉದ್ದಳತೆ = 0;
        ಸೂಚ್ಯಂಕ++;
        ಆದರೆ(++ಅಂಕೆ_splits == MAX_QUERY_COMPONENTS) {
          ಫಲಿತಾಂಶ = CURLE_TOO_LARGE;
          ಹೋಗು fail;
        }
      }
      ಆರಂಭ = pos + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      segment_ಉದ್ದಳತೆ++;
    }
  }

  ಆದರೆ(segment_ಉದ್ದಳತೆ) {
    curlx_dyn_init(&db[ಸೂಚ್ಯಂಕ], segment_ಉದ್ದಳತೆ + 1);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&db[ಸೂಚ್ಯಂಕ], &ಮೂಲ[ಆರಂಭ], segment_ಉದ್ದಳತೆ);
    ಆದರೆ(!ಫಲಿತಾಂಶ && ++ಅಂಕೆ_splits == MAX_QUERY_COMPONENTS)
      ಫಲಿತಾಂಶ = CURLE_TOO_LARGE;
  }
fail:
  *ಅಂಕೆ_splits_ಹೊರಹೋಗು = ಅಂಕೆ_splits;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool is_reserved_char(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  ಹಿಂತಿರುಗಿ (ISALNUM(c) || ISURLPUNTCS(c));
}

ಸ್ಥಿರ CURLcode uri_encode_ಮಾರ್ಗ(ರಚನೆ Curl_str *original_ಮಾರ್ಗ,
                                ರಚನೆ dynbuf *new_ಮಾರ್ಗ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = curlx_str(original_ಮಾರ್ಗ);
  size_t i;

  ಗಾಗಿ(i = 0; i < curlx_strlen(original_ಮಾರ್ಗ); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = p[i];
    ಆದರೆ(is_reserved_char(c) || c == '/')
      ಫಲಿತಾಂಶ = curlx_dyn_addn(new_ಮಾರ್ಗ, &c, 1);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = curlx_dyn_addf(new_ಮಾರ್ಗ, "%%%02X", c);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ಸ್ಥಿರ CURLcode normalize_query(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, size_t len,
                                ರಚನೆ dynbuf *db)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(len && !ಫಲಿತಾಂಶ) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಒಳ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*string;
    ಆದರೆ(('%' == ಒಳ) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      ಒಳ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      ಆದರೆ(ಒಳ == '+') {
        /* decodes to plus, so leave this encoded */
        ಫಲಿತಾಂಶ = curlx_dyn_addn(db, "%2B", 3);
        ಮುಂದುವರಿ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      string++;
      len--;
    }

    ಆದರೆ(is_reserved_char(ಒಳ))
      /* Escape unreserved chars from RFC 3986 */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(db, &ಒಳ, 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಒಳ == '+')
      /* Encode '+' as space */
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(db, "%20");
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = curlx_dyn_addf(db, "%%%02X", ಒಳ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool should_urlencode(ರಚನೆ Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  ಆದರೆ(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

/* maximum length for the aws sivg4 parts */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_SIGV4_LEN    64
#ವ್ಯಾಖ್ಯಾನಿಸು DATE_HDR_ಕೀಲಿ_LEN (MAX_SIGV4_LEN + ಗಾತ್ರ("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#ವ್ಯಾಖ್ಯಾನಿಸು DATE_ತುಂಬಿದ_HDR_LEN (DATE_HDR_ಕೀಲಿ_LEN + TIMESTAMP_ಗಾತ್ರ + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compare_ಶೀರ್ಷಿಕೆ_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *colon_a;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  ಪೂರ್ಣಾಂಕ cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(a);
  len_b = colon_b ? (size_t)(colon_b - b) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  ಆದರೆ(!cmp)
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(len_a - len_b);

  ಹಿಂತಿರುಗಿ cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ಸ್ಥಿರ CURLcode ವಿಲೀನ_duplicate_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ curl_slist *ತಲೆ)
{
  ರಚನೆ curl_slist *curr = ತಲೆ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(curr) {
    ರಚನೆ curl_slist *ಮುಂದಿನ = curr->ಮುಂದಿನ;
    ಆದರೆ(!ಮುಂದಿನ)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(compare_ಶೀರ್ಷಿಕೆ_ಹೆಸರುಗಳು(curr->ದತ್ತಾಂಶ, ಮುಂದಿನ->ದತ್ತಾಂಶ) == 0) {
      ರಚನೆ dynbuf buf;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *colon_ಮುಂದಿನ;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val_ಮುಂದಿನ;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);

      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&buf, curr->ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      colon_ಮುಂದಿನ = strchr(ಮುಂದಿನ->ದತ್ತಾಂಶ, ':');
      DEBUGASSERT(colon_ಮುಂದಿನ);
      val_ಮುಂದಿನ = colon_ಮುಂದಿನ + 1;

      ಫಲಿತಾಂಶ = curlx_dyn_addn(&buf, ",", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&buf, val_ಮುಂದಿನ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      curlx_free(curr->ದತ್ತಾಂಶ);
      curr->ದತ್ತಾಂಶ = curlx_dyn_ptr(&buf);

      curr->ಮುಂದಿನ = ಮುಂದಿನ->ಮುಂದಿನ;
      curlx_free(ಮುಂದಿನ->ದತ್ತಾಂಶ);
      curlx_free(ಮುಂದಿನ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      curr = curr->ಮುಂದಿನ;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ಸ್ಥಿರ CURLcode make_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಅಕ್ಷರ *timestamp,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *provider1,
                             size_t plen, /* length of provider1 */
                             ಅಕ್ಷರ **date_ಶೀರ್ಷಿಕೆ,
                             ಅಕ್ಷರ *content_sha256_ಶೀರ್ಷಿಕೆ,
                             ರಚನೆ dynbuf *canonical_ಶೀರ್ಷಿಕೆಗಳು,
                             ರಚನೆ dynbuf *signed_ಶೀರ್ಷಿಕೆಗಳು)
{
  ಅಕ್ಷರ date_hdr_ಕೀಲಿ[DATE_HDR_ಕೀಲಿ_LEN];
  ಅಕ್ಷರ date_ತುಂಬಿದ_hdr[DATE_ತುಂಬಿದ_HDR_LEN];
  ರಚನೆ curl_slist *ತಲೆ = ಖಾಲಿ;
  ರಚನೆ curl_slist *tmp_ತಲೆ = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ರಚನೆ curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_ಕೀಲಿ, DATE_HDR_ಕೀಲಿ_LEN, "X-%.*s-Date",
                 (ಪೂರ್ಣಾಂಕ)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_ಕೀಲಿ[2], provider1, plen);
  date_hdr_ಕೀಲಿ[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_ತುಂಬಿದ_hdr, DATE_ತುಂಬಿದ_HDR_LEN,
                 "x-%.*s-date:%s", (ಪೂರ್ಣಾಂಕ)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_ತುಂಬಿದ_hdr[2], provider1, plen);

  ಆದರೆ(!Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Host")) &&
     ದತ್ತಾಂಶ->state.http_host) {
    /* Host: [host]:[port] */
    ಅಕ್ಷರ *fullhost = curlx_strdup(ದತ್ತಾಂಶ->state.http_host);

    ಆದರೆ(fullhost)
      ತಲೆ = Curl_slist_append_nodup(ಖಾಲಿ, fullhost);
    ಆದರೆ(!ತಲೆ) {
      curlx_free(fullhost);
      ಹೋಗು fail;
    }
  }

  ಆದರೆ(*content_sha256_ಶೀರ್ಷಿಕೆ) {
    tmp_ತಲೆ = curl_slist_append(ತಲೆ, content_sha256_ಶೀರ್ಷಿಕೆ);
    ಆದರೆ(!tmp_ತಲೆ)
      ಹೋಗು fail;
    ತಲೆ = tmp_ತಲೆ;
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
  ಗಾಗಿ(l = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು; l; l = l->ಮುಂದಿನ) {
    ಅಕ್ಷರ *dupdata;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = strchr(l->ದತ್ತಾಂಶ, ':');
    ಆದರೆ(!sep)
      sep = strchr(l->ದತ್ತಾಂಶ, ';');
    ಆದರೆ(!sep || (*sep == ':' && !*(sep + 1)))
      ಮುಂದುವರಿ;
    ಗಾಗಿ(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    ಆದರೆ(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      ಮುಂದುವರಿ;
    dupdata = curlx_strdup(l->ದತ್ತಾಂಶ);
    ಆದರೆ(!dupdata)
      ಹೋಗು fail;
    dupdata[sep - l->ದತ್ತಾಂಶ] = ':';
    tmp_ತಲೆ = Curl_slist_append_nodup(ತಲೆ, dupdata);
    ಆದರೆ(!tmp_ತಲೆ) {
      curlx_free(dupdata);
      ಹೋಗು fail;
    }
    ತಲೆ = tmp_ತಲೆ;
  }

  trim_ಶೀರ್ಷಿಕೆಗಳು(ತಲೆ);

  *date_ಶೀರ್ಷಿಕೆ = ಕಂಡುಹಿಡಿ_date_hdr(ದತ್ತಾಂಶ, date_hdr_ಕೀಲಿ);
  ಆದರೆ(!*date_ಶೀರ್ಷಿಕೆ) {
    tmp_ತಲೆ = curl_slist_append(ತಲೆ, date_ತುಂಬಿದ_hdr);
    ಆದರೆ(!tmp_ತಲೆ)
      ಹೋಗು fail;
    ತಲೆ = tmp_ತಲೆ;
    *date_ಶೀರ್ಷಿಕೆ = curl_maprintf("%s: %s\r\n", date_hdr_ಕೀಲಿ, timestamp);
    ಆದರೆ(!*date_ಶೀರ್ಷಿಕೆ)
      ಹೋಗು fail;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *endp;
    ಮೌಲ್ಯ = strchr(*date_ಶೀರ್ಷಿಕೆ, ':');
    ಆದರೆ(!ಮೌಲ್ಯ) {
      *date_ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
      ಹೋಗು fail;
    }
    ++ಮೌಲ್ಯ;
    curlx_str_passblanks(&ಮೌಲ್ಯ);
    endp = ಮೌಲ್ಯ;
    ಆಗ(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    ಆದರೆ((endp - ಮೌಲ್ಯ) == TIMESTAMP_ಗಾತ್ರ - 1) {
      ಸ್ಮರಣೆ_ನಕಲು(timestamp, ಮೌಲ್ಯ, TIMESTAMP_ಗಾತ್ರ - 1);
      timestamp[TIMESTAMP_ಗಾತ್ರ - 1] = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  }

  /* alpha-sort by header name in a case sensitive manner */
  ಮಾಡು {
    again = FALSE;
    ಗಾಗಿ(l = ತಲೆ; l; l = l->ಮುಂದಿನ) {
      ರಚನೆ curl_slist *ಮುಂದಿನ = l->ಮುಂದಿನ;

      ಆದರೆ(ಮುಂದಿನ && compare_ಶೀರ್ಷಿಕೆ_ಹೆಸರುಗಳು(l->ದತ್ತಾಂಶ, ಮುಂದಿನ->ದತ್ತಾಂಶ) > 0) {
        ಅಕ್ಷರ *tmp = l->ದತ್ತಾಂಶ;

        l->ದತ್ತಾಂಶ = ಮುಂದಿನ->ದತ್ತಾಂಶ;
        ಮುಂದಿನ->ದತ್ತಾಂಶ = tmp;
        again = TRUE;
      }
    }
  } ಆಗ(again);

  ಫಲಿತಾಂಶ = ವಿಲೀನ_duplicate_ಶೀರ್ಷಿಕೆಗಳು(ತಲೆ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಗಾಗಿ(l = ತಲೆ; l; l = l->ಮುಂದಿನ) {
    ಅಕ್ಷರ *tmp;

    ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(canonical_ಶೀರ್ಷಿಕೆಗಳು, l->ದತ್ತಾಂಶ))
      ಹೋಗು fail;
    ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(canonical_ಶೀರ್ಷಿಕೆಗಳು, "\n"))
      ಹೋಗು fail;

    tmp = strchr(l->ದತ್ತಾಂಶ, ':');
    ಆದರೆ(tmp)
      *tmp = 0;

    ಆದರೆ(l != ತಲೆ && curlx_dyn_ಕೂಡಿಸು(signed_ಶೀರ್ಷಿಕೆಗಳು, ";"))
      ಹೋಗು fail;
    ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(signed_ಶೀರ್ಷಿಕೆಗಳು, l->ದತ್ತಾಂಶ))
      ಹೋಗು fail;
  }

  ಫಲಿತಾಂಶ = CURLE_OK;
fail:
  curl_slist_free_all(ತಲೆ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CONTENT_SHA256_ಕೀಲಿ_LEN (MAX_SIGV4_LEN + ಗಾತ್ರ("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#ವ್ಯಾಖ್ಯಾನಿಸು CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_ಕೀಲಿ_LEN + 2 + SHA256_HEX_ಉದ್ದಳತೆ)

/* try to parse a payload hash from the content-sha256 header */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *parse_content_sha_hdr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *provider1,
                                         size_t plen,
                                         size_t *ಮೌಲ್ಯ_len)
{
  ಅಕ್ಷರ ಕೀಲಿ[CONTENT_SHA256_ಕೀಲಿ_LEN];
  size_t ಕೀಲಿ_len;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;
  size_t len;

  ಕೀಲಿ_len = curl_msnprintf(ಕೀಲಿ, ಗಾತ್ರ(ಕೀಲಿ), "x-%.*s-content-sha256",
                           (ಪೂರ್ಣಾಂಕ)plen, provider1);

  ಮೌಲ್ಯ = Curl_checkheaders(ದತ್ತಾಂಶ, ಕೀಲಿ, ಕೀಲಿ_len);
  ಆದರೆ(!ಮೌಲ್ಯ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಮೌಲ್ಯ = strchr(ಮೌಲ್ಯ, ':');
  ಆದರೆ(!ಮೌಲ್ಯ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ++ಮೌಲ್ಯ;

  curlx_str_passblanks(&ಮೌಲ್ಯ);

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);
  ಆಗ(len > 0 && ISBLANK(ಮೌಲ್ಯ[len - 1]))
    --len;

  *ಮೌಲ್ಯ_len = len;
  ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ CURLcode calc_payload_hash(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha_hash, ಅಕ್ಷರ *sha_hex)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *post_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ->ಗಣ.postfields;
  size_t post_ದತ್ತಾಂಶ_len = 0;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(post_ದತ್ತಾಂಶ) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postfieldsize < 0)
      post_ದತ್ತಾಂಶ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(post_ದತ್ತಾಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ
      post_ದತ್ತಾಂಶ_len = (size_t)ದತ್ತಾಂಶ->ಗಣ.postfieldsize;
  }
  ಫಲಿತಾಂಶ = Curl_sha256it(sha_hash, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)post_ದತ್ತಾಂಶ,
                         post_ದತ್ತಾಂಶ_len);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    sha256_ಗಮ್ಯ_hex(sha_hex, sha_hash);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ಸ್ಥಿರ CURLcode calc_s3_payload_hash(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     Curl_HttpReq httpreq,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *provider1,
                                     size_t plen,
                                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha_hash,
                                     ಅಕ್ಷರ *sha_hex, ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  bool ಖಾಲಿಗಳು_ವಿಧಾನ = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_ತಲೆ);
  /* The request method or filesize indicate no request payload */
  bool ಖಾಲಿಗಳು_payload = (ಖಾಲಿಗಳು_ವಿಧಾನ || ದತ್ತಾಂಶ->ಗಣ.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && ದತ್ತಾಂಶ->ಗಣ.postfields);
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(ಖಾಲಿಗಳು_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    ಫಲಿತಾಂಶ = calc_payload_hash(ದತ್ತಾಂಶ, sha_hash, sha_hex);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು fail;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_ಉದ್ದಳತೆ); /* 16 < 65 */
    ಸ್ಮರಣೆ_ನಕಲು(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(ಶೀರ್ಷಿಕೆ, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (ಪೂರ್ಣಾಂಕ)plen, provider1, sha_hex);

  ಫಲಿತಾಂಶ = CURLE_OK;
fail:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compare_func(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ pair *aa = a;
  ಸ್ಥಿರಾಂಕ ರಚನೆ pair *bb = b;
  ಸ್ಥಿರಾಂಕ size_t aa_ಕೀಲಿ_len = curlx_dyn_len(&aa->ಕೀಲಿ);
  ಸ್ಥಿರಾಂಕ size_t bb_ಕೀಲಿ_len = curlx_dyn_len(&bb->ಕೀಲಿ);
  ಸ್ಥಿರಾಂಕ size_t aa_ಮೌಲ್ಯ_len = curlx_dyn_len(&aa->ಮೌಲ್ಯ);
  ಸ್ಥಿರಾಂಕ size_t bb_ಮೌಲ್ಯ_len = curlx_dyn_len(&bb->ಮೌಲ್ಯ);
  ಪೂರ್ಣಾಂಕ compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  ಆದರೆ((aa_ಕೀಲಿ_len == 0) && (bb_ಕೀಲಿ_len == 0))
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(aa_ಕೀಲಿ_len == 0)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(bb_ಕೀಲಿ_len == 0)
    ಹಿಂತಿರುಗಿ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ಕೀಲಿ), curlx_dyn_ptr(&bb->ಕೀಲಿ));
  ಆದರೆ(compare) {
    ಹಿಂತಿರುಗಿ compare;
  }

  /* Compare values */
  ಆದರೆ((aa_ಮೌಲ್ಯ_len == 0) && (bb_ಮೌಲ್ಯ_len == 0))
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(aa_ಮೌಲ್ಯ_len == 0)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(bb_ಮೌಲ್ಯ_len == 0)
    ಹಿಂತಿರುಗಿ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ಮೌಲ್ಯ), curlx_dyn_ptr(&bb->ಮೌಲ್ಯ));

  ಹಿಂತಿರುಗಿ compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q, size_t len,
                             ರಚನೆ dynbuf *new_ಮಾರ್ಗ,
                             bool do_uri_encode);
UNITTEST CURLcode canon_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q, size_t len,
                             ರಚನೆ dynbuf *new_ಮಾರ್ಗ,
                             bool do_uri_encode)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ರಚನೆ Curl_str original_ಮಾರ್ಗ;

  curlx_str_ನಿಯೋಜಿಸು(&original_ಮಾರ್ಗ, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  ಆದರೆ(do_uri_encode)
    ಫಲಿತಾಂಶ = uri_encode_ಮಾರ್ಗ(&original_ಮಾರ್ಗ, new_ಮಾರ್ಗ);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = curlx_dyn_addn(new_ಮಾರ್ಗ, q, len);

  ಆದರೆ(!ಫಲಿತಾಂಶ && curlx_dyn_len(new_ಮಾರ್ಗ) == 0)
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(new_ಮಾರ್ಗ, "/");

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query, ರಚನೆ dynbuf *dq);
UNITTEST CURLcode canon_query(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query, ರಚನೆ dynbuf *dq)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ರಚನೆ dynbuf query_array[MAX_QUERY_COMPONENTS];
  ರಚನೆ pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t ಅಂಕೆ_query_components;
  size_t counted_query_components = 0;
  size_t ಸೂಚ್ಯಂಕ;

  ಆದರೆ(!query)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = ವಿಭಜಿಸು_ಗಮ್ಯ_dyn_array(query, &query_array[0], &ಅಂಕೆ_query_components);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಹೋಗು fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  ಗಾಗಿ(ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < ಅಂಕೆ_query_components; ಸೂಚ್ಯಂಕ++) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_ಕೀಲಿ;
    size_t ಒಳ_ಕೀಲಿ_len;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಾನಾಂತರ;
    size_t query_part_len = curlx_dyn_len(&query_array[ಸೂಚ್ಯಂಕ]);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query_part = curlx_dyn_ptr(&query_array[ಸೂಚ್ಯಂಕ]);

    ಒಳ_ಕೀಲಿ = query_part;

    ಸ್ಥಾನಾಂತರ = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    ಆದರೆ(!ಸ್ಥಾನಾಂತರ) {
      ಒಳ_ಕೀಲಿ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳ_ಕೀಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಒಳ_ಕೀಲಿ_len = ಸ್ಥಾನಾಂತರ - ಒಳ_ಕೀಲಿ;
    }

    curlx_dyn_init(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಕೀಲಿ,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    ಫಲಿತಾಂಶ = normalize_query(ಒಳ_ಕೀಲಿ, ಒಳ_ಕೀಲಿ_len,
                             &encoded_query_array[ಸೂಚ್ಯಂಕ].ಕೀಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಹೋಗು fail;
    }

    /* Decode/encode the value if it exists */
    ಆದರೆ(ಸ್ಥಾನಾಂತರ && ಸ್ಥಾನಾಂತರ != (query_part + query_part_len - 1)) {
      size_t ಒಳ_ಮೌಲ್ಯ_len;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_ಮೌಲ್ಯ = ಸ್ಥಾನಾಂತರ + 1;
      ಒಳ_ಮೌಲ್ಯ_len = query_part + query_part_len - (ಸ್ಥಾನಾಂತರ + 1);
      ಫಲಿತಾಂಶ = normalize_query(ಒಳ_ಮೌಲ್ಯ, ಒಳ_ಮೌಲ್ಯ_len,
                               &encoded_query_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ಹೋಗು fail;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ, 2);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ, "", 1);
    }

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಹೋಗು fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, ಅಂಕೆ_query_components,
        ಗಾತ್ರ(ರಚನೆ pair), compare_func);

  /* Append the query components together to make a full query string */
  ಗಾಗಿ(ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < ಅಂಕೆ_query_components; ಸೂಚ್ಯಂಕ++) {

    ಆದರೆ(ಸೂಚ್ಯಂಕ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(dq, "&", 1);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_ptr = curlx_dyn_ptr(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಕೀಲಿ);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_ptr = curlx_dyn_ptr(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ);
      size_t vlen = curlx_dyn_len(&encoded_query_array[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ);
      ಆದರೆ(ಮೌಲ್ಯ_ptr && vlen) {
        ಫಲಿತಾಂಶ = curlx_dyn_addf(dq, "%s=%s", ಕೀಲಿ_ptr, ಮೌಲ್ಯ_ptr);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Empty value is always encoded to key= */
        ಫಲಿತಾಂಶ = curlx_dyn_addf(dq, "%s=", ಕೀಲಿ_ptr);
      }
    }
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }

fail:
  ಆದರೆ(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], ಅಂಕೆ_query_components);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode parse_sigv4_params(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                                   ರಚನೆ Curl_str *provider0,
                                   ರಚನೆ Curl_str *provider1,
                                   ರಚನೆ Curl_str *region,
                                   ರಚನೆ Curl_str *service)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_AWS_SIGV4);
  ಆದರೆ(!ಸಾಲು || !*ಸಾಲು)
    ಸಾಲು = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  ಆದರೆ(curlx_str_until(&ಸಾಲು, provider0, MAX_SIGV4_LEN, ':')) {
    failf(ದತ್ತಾಂಶ, "first aws-sigv4 provider cannot be empty");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಆದರೆ(curlx_str_single(&ಸಾಲು, ':') ||
     curlx_str_until(&ಸಾಲು, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_single(&ಸಾಲು, ':') ||
          curlx_str_until(&ಸಾಲು, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&ಸಾಲು, ':') ||
          curlx_str_until(&ಸಾಲು, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  ಆದರೆ(!curlx_strlen(service)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = hostname;
    ಆದರೆ(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(ದತ್ತಾಂಶ, "aws-sigv4: service missing in parameters and hostname");
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }

    infof(ದತ್ತಾಂಶ, "aws_sigv4: picked service %.*s from host",
          (ಪೂರ್ಣಾಂಕ)curlx_strlen(service), curlx_str(service));

    ಆದರೆ(!curlx_strlen(region)) {
      ಆದರೆ(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(ದತ್ತಾಂಶ, "aws-sigv4: region missing in parameters and hostname");
        ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
      }
      infof(ದತ್ತಾಂಶ, "aws_sigv4: picked region %.*s from host",
            (ಪೂರ್ಣಾಂಕ)curlx_strlen(region), curlx_str(region));
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode get_payload_hash(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 Curl_HttpReq httpreq,
                                 ರಚನೆ Curl_str *provider0,
                                 ರಚನೆ Curl_str *provider1,
                                 ರಚನೆ Curl_str *service,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha_hash,
                                 ಅಕ್ಷರ *sha_hex,
                                 ಅಕ್ಷರ *content_sha256_hdr,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **payload_hash_ಹೊರಹೋಗು,
                                 size_t *payload_hash_len_ಹೊರಹೋಗು)
{
  *payload_hash_ಹೊರಹೋಗು =
    parse_content_sha_hdr(ದತ್ತಾಂಶ, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_ಹೊರಹೋಗು);

  ಆದರೆ(!*payload_hash_ಹೊರಹೋಗು) {
    CURLcode ಫಲಿತಾಂಶ;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    ಆದರೆ(sign_as_s3)
      ಫಲಿತಾಂಶ = calc_s3_payload_hash(ದತ್ತಾಂಶ, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = calc_payload_hash(ದತ್ತಾಂಶ, sha_hash, sha_hex);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    *payload_hash_ಹೊರಹೋಗು = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_ಹೊರಹೋಗು = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sha_hex);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode get_timestamp(ಅಕ್ಷರ *timestamp, size_t stampsize)
{
  time_t clock;
  ರಚನೆ tm tm;
  CURLcode ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಅಕ್ಷರ *force_timestamp = getenv("CURL_FORCETIME");
    ಆದರೆ(force_timestamp)
      clock = 0;
    ಇಲ್ಲದಿದ್ದರೆ
      clock = ಸಮಯ(ಖಾಲಿ);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  clock = ಸಮಯ(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
  ಫಲಿತಾಂಶ = curlx_gmtime(clock, &tm);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode make_canonical_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ಅಕ್ಷರ *timestamp,
                                       ರಚನೆ Curl_str *provider1,
                                       ರಚನೆ Curl_str *service,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *payload_hash,
                                       size_t payload_hash_len,
                                       ಅಕ್ಷರ **date_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು,
                                       ಅಕ್ಷರ *content_sha256_hdr,
                                       ರಚನೆ dynbuf *canonical_ಶೀರ್ಷಿಕೆಗಳು,
                                       ರಚನೆ dynbuf *signed_ಶೀರ್ಷಿಕೆಗಳು,
                                       ಅಕ್ಷರ **canonical_request_ಹೊರಹೋಗು)
{
  ರಚನೆ dynbuf canonical_query;
  ರಚನೆ dynbuf canonical_ಮಾರ್ಗ;
  CURLcode ಫಲಿತಾಂಶ;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  curlx_dyn_init(&canonical_ಮಾರ್ಗ, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);

  ಫಲಿತಾಂಶ = make_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, content_sha256_hdr,
                        canonical_ಶೀರ್ಷಿಕೆಗಳು, signed_ಶೀರ್ಷಿಕೆಗಳು);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಫಲಿತಾಂಶ = canon_query(ದತ್ತಾಂಶ->state.ಮೇಲೆ.query, &canonical_query);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  ಫಲಿತಾಂಶ = canon_ಮಾರ್ಗ(ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ),
                      &canonical_ಮಾರ್ಗ,
                      should_urlencode(service));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  *canonical_request_ಹೊರಹೋಗು =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  ವಿಧಾನ,
                  curlx_dyn_ptr(&canonical_ಮಾರ್ಗ),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_ಶೀರ್ಷಿಕೆಗಳು),
                  curlx_dyn_ptr(signed_ಶೀರ್ಷಿಕೆಗಳು),
                  (ಪೂರ್ಣಾಂಕ)payload_hash_len, payload_hash);
  ಆದರೆ(!*canonical_request_ಹೊರಹೋಗು) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  ಫಲಿತಾಂಶ = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_ಮಾರ್ಗ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode make_string_ಗಮ್ಯ_sign(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ Curl_str *provider0,
                                    ರಚನೆ Curl_str *region,
                                    ರಚನೆ Curl_str *service,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *timestamp,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *canonical_request,
                                    ಅಕ್ಷರ **request_type_ಹೊರಹೋಗು,
                                    ಅಕ್ಷರ **credential_scope_ಹೊರಹೋಗು,
                                    ಅಕ್ಷರ **str_ಗಮ್ಯ_sign_ಹೊರಹೋಗು)
{
  ಅಕ್ಷರ *request_type;
  ಅಕ್ಷರ *credential_scope;
  ಅಕ್ಷರ *str_ಗಮ್ಯ_sign;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ sha_hash[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
  ಅಕ್ಷರ sha_hex[SHA256_HEX_ಉದ್ದಳತೆ];

  request_type = curl_maprintf("%.*s4_request",
                               (ಪೂರ್ಣಾಂಕ)curlx_strlen(provider0),
                               curlx_str(provider0));
  ಆದರೆ(!request_type)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (ಪೂರ್ಣಾಂಕ)curlx_strlen(region),
                                   curlx_str(region),
                                   (ಪೂರ್ಣಾಂಕ)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  ಆದರೆ(!credential_scope) {
    curlx_free(request_type);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(Curl_sha256it(sha_hash, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)canonical_request,
                   ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  sha256_ಗಮ್ಯ_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_ಗಮ್ಯ_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (ಪೂರ್ಣಾಂಕ)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  ಆದರೆ(!str_ಗಮ್ಯ_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_ಗಮ್ಯ_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(ದತ್ತಾಂಶ, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_ಗಮ್ಯ_sign);

  *request_type_ಹೊರಹೋಗು = request_type;
  *credential_scope_ಹೊರಹೋಗು = credential_scope;
  *str_ಗಮ್ಯ_sign_ಹೊರಹೋಗು = str_ಗಮ್ಯ_sign;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode sign_and_ಗಣ_auth_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_str *provider0,
                                          ರಚನೆ Curl_str *region,
                                          ರಚನೆ Curl_str *service,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request_type,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *credential_scope,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str_ಗಮ್ಯ_sign,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date_ಶೀರ್ಷಿಕೆ,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content_sha256_hdr,
                                          ರಚನೆ dynbuf *signed_ಶೀರ್ಷಿಕೆಗಳು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd = Curl_creds_passwd(ದತ್ತಾಂಶ->state.creds);
  ಅಕ್ಷರ *secret = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ sign0[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ] = { 0 };
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ sign1[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ] = { 0 };
  ಅಕ್ಷರ sha_hex[SHA256_HEX_ಉದ್ದಳತೆ];
  ಅಕ್ಷರ *auth_ಶೀರ್ಷಿಕೆಗಳು = ಖಾಲಿ;
  ಅಕ್ಷರ *user = curl_escape(Curl_creds_user(ದತ್ತಾಂಶ->state.creds), 0);
  ಆದರೆ(!user)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  secret = curl_maprintf("%.*s4%s", (ಪೂರ್ಣಾಂಕ)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  ಆದರೆ(!secret)
    ಹೋಗು fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(secret), date, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(date), sign0);
  HMAC_SHA256(sign0, ಗಾತ್ರ(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, ಗಾತ್ರ(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, ಗಾತ್ರ(sign0),
              request_type, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(request_type), sign1);
  HMAC_SHA256(sign1, ಗಾತ್ರ(sign1),
              str_ಗಮ್ಯ_sign, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str_ಗಮ್ಯ_sign), sign0);

  sha256_ಗಮ್ಯ_hex(sha_hex, sign0);

  infof(ದತ್ತಾಂಶ, "aws_sigv4: Signature - %s", sha_hex);

  auth_ಶೀರ್ಷಿಕೆಗಳು = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (ಪೂರ್ಣಾಂಕ)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_ಶೀರ್ಷಿಕೆಗಳು),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_ಶೀರ್ಷಿಕೆ ? date_ಶೀರ್ಷಿಕೆ : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  ಆದರೆ(!auth_ಶೀರ್ಷಿಕೆಗಳು)
    ಹೋಗು fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_ಶೀರ್ಷಿಕೆಗಳು[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(ದತ್ತಾಂಶ->req.hd_auth);
  ದತ್ತಾಂಶ->req.hd_auth = auth_ಶೀರ್ಷಿಕೆಗಳು;
  ದತ್ತಾಂಶ->state.authhost.ಮುಗಿದ = TRUE;
  ಫಲಿತಾಂಶ = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_output_aws_sigv4(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ರಚನೆ Curl_str provider0 = { ಖಾಲಿ, 0 };
  ರಚನೆ Curl_str provider1 = { ಖಾಲಿ, 0 };
  ರಚನೆ Curl_str region = { ಖಾಲಿ, 0 };
  ರಚನೆ Curl_str service = { ಖಾಲಿ, 0 };
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = ದತ್ತಾಂಶ->state.origin->hostname;
  ಅಕ್ಷರ timestamp[TIMESTAMP_ಗಾತ್ರ];
  ಅಕ್ಷರ date[9];
  ರಚನೆ dynbuf canonical_ಶೀರ್ಷಿಕೆಗಳು;
  ರಚನೆ dynbuf signed_ಶೀರ್ಷಿಕೆಗಳು;
  ಅಕ್ಷರ *date_ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  Curl_HttpReq httpreq;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಧಾನ = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *payload_hash = ಖಾಲಿ;
  size_t payload_hash_len = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ sha_hash[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
  ಅಕ್ಷರ sha_hex[SHA256_HEX_ಉದ್ದಳತೆ];
  ಅಕ್ಷರ content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  ಅಕ್ಷರ *canonical_request = ಖಾಲಿ;
  ಅಕ್ಷರ *request_type = ಖಾಲಿ;
  ಅಕ್ಷರ *credential_scope = ಖಾಲಿ;
  ಅಕ್ಷರ *str_ಗಮ್ಯ_sign = ಖಾಲಿ;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ಮಾರ್ಗ_as_is) {
    failf(ದತ್ತಾಂಶ, "Cannot use sigv4 authentication with path-as-is flag");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಆದರೆ(Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_ಶೀರ್ಷಿಕೆಗಳು, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  curlx_dyn_init(&signed_ಶೀರ್ಷಿಕೆಗಳು, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);

  ಫಲಿತಾಂಶ = parse_sigv4_params(ದತ್ತಾಂಶ, hostname, &provider0, &provider1,
                              &region, &service);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    Curl_http_ವಿಧಾನ(ದತ್ತಾಂಶ, &ವಿಧಾನ, &httpreq);
    ಫಲಿತಾಂಶ = get_payload_hash(ದತ್ತಾಂಶ, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = get_timestamp(timestamp, ಗಾತ್ರ(timestamp));

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = make_canonical_request(ದತ್ತಾಂಶ, timestamp,
                                    &provider1, &service,
                                    ವಿಧಾನ, payload_hash, payload_hash_len,
                                    &date_ಶೀರ್ಷಿಕೆ, content_sha256_hdr,
                                    &canonical_ಶೀರ್ಷಿಕೆಗಳು, &signed_ಶೀರ್ಷಿಕೆಗಳು,
                                    &canonical_request);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    /* the timestamp might have been updated in make_canonical_request */
    ಸ್ಮರಣೆ_ನಕಲು(date, timestamp, ಗಾತ್ರ(date) - 1);
    date[ಗಾತ್ರ(date) - 1] = 0;

    ಫಲಿತಾಂಶ = make_string_ಗಮ್ಯ_sign(ದತ್ತಾಂಶ, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_ಗಮ್ಯ_sign);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = sign_and_ಗಣ_auth_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_ಗಮ್ಯ_sign, date_ಶೀರ್ಷಿಕೆ,
                                       content_sha256_hdr, &signed_ಶೀರ್ಷಿಕೆಗಳು);

  curlx_dyn_free(&canonical_ಶೀರ್ಷಿಕೆಗಳು);
  curlx_dyn_free(&signed_ಶೀರ್ಷಿಕೆಗಳು);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_ಗಮ್ಯ_sign);
  curlx_free(date_ಶೀರ್ಷಿಕೆ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
