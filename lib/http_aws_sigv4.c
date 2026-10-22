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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#ضمن "urldata.h"
#ضمن "strcase.h"
#ضمن "curlx/strdup.h"
#ضمن "http_aws_sigv4.h"
#ضمن "curl_sha256.h"
#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "escape.h"
#ضمن "curlx/strparse.h"
#ضمن "slist.h"

#ضمن <time.h>

#تعریف_کرو HMAC_SHA256(k, kl, d, dl, o)                 \
  افعل {                                               \
    نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت بنانشانی حرف *)(k), \
                         kl,                         \
                         (ثابت بنانشانی حرف *)(d), \
                         dl, o);                     \
    إذا(نتیجہ) {                                     \
      اذهب fail;                                     \
    }                                                \
  } طالما(0)

#تعریف_کرو TIMESTAMP_ناپ 17

/* hex-encoded with null-terminator */
#تعریف_کرو SHA256_HEX_لمائی ((2 * CURL_SHA256_DIGEST_لمائی) + 1)

#تعریف_کرو MAX_QUERY_COMPONENTS 128

بنية pair {
  بنية dynbuf کلید;
  بنية dynbuf قدر;
};

ساكن فراغ sha256_منزل_hex(حرف *dst, بنانشانی حرف *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_لمائی,
                 (بنانشانی حرف *)dst, SHA256_HEX_لمائی);
}

ساكن حرف *لبھو_date_hdr(بنية Curl_easy *انکڑے, ثابت حرف *sig_hdr)
{
  حرف *tmp = Curl_checkheaders(انکڑے, sig_hdr, لڑی_لمبائی(sig_hdr));

  إذا(tmp)
    أرجع tmp;
  أرجع Curl_checkheaders(انکڑے, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ساكن فراغ trim_سرخیاں(بنية curl_slist *سر)
{
  بنية curl_slist *l;
  لكل(l = سر; l; l = l->اگلا) {
    ثابت حرف *قدر; /* to read from */
    حرف *store;
    size_t colon = strcspn(l->انکڑے, ":");
    Curl_strntolower(l->انکڑے, l->انکڑے, colon);

    قدر = &l->انکڑے[colon];
    إذا(!*قدر)
      استمر;
    ++قدر;
    store = (حرف *)CURL_UNCONST(قدر);

    /* skip leading whitespace */
    curlx_str_passblanks(&قدر);

    طالما(*قدر) {
      صحيح space = 0;
      طالما(ISBLANK(*قدر)) {
        قدر++;
        space++;
      }
      إذا(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        إذا(*قدر)
          *store++ = ' ';
      }
      وإلا
        *store++ = *قدر++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ساكن فراغ pair_array_free(بنية pair *pair_array, size_t عدد_elements)
{
  size_t اشاریہ;

  لكل(اشاریہ = 0; اشاریہ != عدد_elements; اشاریہ++) {
    curlx_dyn_free(&pair_array[اشاریہ].کلید);
    curlx_dyn_free(&pair_array[اشاریہ].قدر);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ساكن فراغ dyn_array_free(بنية dynbuf *db, size_t عدد_elements)
{
  size_t اشاریہ;

  لكل(اشاریہ = 0; اشاریہ < عدد_elements; اشاریہ++)
    curlx_dyn_free((&db[اشاریہ]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#تعریف_کرو ونڈو_BY '&'

ساكن CURLcode ونڈو_منزل_dyn_array(ثابت حرف *سروت,
                                   بنية dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *عدد_splits_باہرلا)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t len = لڑی_لمبائی(سروت);
  size_t pos;         /* Position in result buffer */
  size_t شروع = 0;   /* Start of current segment */
  size_t segment_لمائی = 0;
  size_t اشاریہ = 0;
  size_t عدد_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  لكل(pos = 0; pos < len; pos++) {
    إذا(سروت[pos] == ونڈو_BY) {
      إذا(segment_لمائی) {
        curlx_dyn_init(&db[اشاریہ], segment_لمائی + 1);
        نتیجہ = curlx_dyn_addn(&db[اشاریہ], &سروت[شروع], segment_لمائی);
        إذا(نتیجہ)
          اذهب fail;

        segment_لمائی = 0;
        اشاریہ++;
        إذا(++عدد_splits == MAX_QUERY_COMPONENTS) {
          نتیجہ = CURLE_TOO_LARGE;
          اذهب fail;
        }
      }
      شروع = pos + 1;
    }
    وإلا {
      segment_لمائی++;
    }
  }

  إذا(segment_لمائی) {
    curlx_dyn_init(&db[اشاریہ], segment_لمائی + 1);
    نتیجہ = curlx_dyn_addn(&db[اشاریہ], &سروت[شروع], segment_لمائی);
    إذا(!نتیجہ && ++عدد_splits == MAX_QUERY_COMPONENTS)
      نتیجہ = CURLE_TOO_LARGE;
  }
fail:
  *عدد_splits_باہرلا = عدد_splits;
  أرجع نتیجہ;
}

ساكن bool is_reserved_char(ثابت حرف c)
{
  أرجع (ISALNUM(c) || ISURLPUNTCS(c));
}

ساكن CURLcode uri_encode_رستہ(بنية Curl_str *original_رستہ,
                                بنية dynbuf *new_رستہ)
{
  ثابت حرف *p = curlx_str(original_رستہ);
  size_t i;

  لكل(i = 0; i < curlx_strlen(original_رستہ); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode نتیجہ = CURLE_OK;
    بنانشانی حرف c = p[i];
    إذا(is_reserved_char(c) || c == '/')
      نتیجہ = curlx_dyn_addn(new_رستہ, &c, 1);
    وإلا
      نتیجہ = curlx_dyn_addf(new_رستہ, "%%%02X", c);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  أرجع CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ساكن CURLcode normalize_query(ثابت حرف *string, size_t len,
                                بنية dynbuf *db)
{
  CURLcode نتیجہ = CURLE_OK;

  طالما(len && !نتیجہ) {
    بنانشانی حرف وچ = (بنانشانی حرف)*string;
    إذا(('%' == وچ) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      وچ = (بنانشانی حرف)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      إذا(وچ == '+') {
        /* decodes to plus, so leave this encoded */
        نتیجہ = curlx_dyn_addn(db, "%2B", 3);
        استمر;
      }
    }
    وإلا {
      string++;
      len--;
    }

    إذا(is_reserved_char(وچ))
      /* Escape unreserved chars from RFC 3986 */
      نتیجہ = curlx_dyn_addn(db, &وچ, 1);
    وإلا إذا(وچ == '+')
      /* Encode '+' as space */
      نتیجہ = curlx_dyn_جوڑو(db, "%20");
    وإلا
      نتیجہ = curlx_dyn_addf(db, "%%%02X", وچ);
  }

  أرجع نتیجہ;
}

ساكن bool should_urlencode(بنية Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  إذا(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    أرجع FALSE;
  }
  أرجع TRUE;
}

/* maximum length for the aws sivg4 parts */
#تعریف_کرو MAX_SIGV4_LEN    64
#تعریف_کرو DATE_HDR_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#تعریف_کرو DATE_بھریا_HDR_LEN (DATE_HDR_کلید_LEN + TIMESTAMP_ناپ + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ساكن صحيح compare_سرخی_ناں(ثابت حرف *a, ثابت حرف *b)
{
  ثابت حرف *colon_a;
  ثابت حرف *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  صحيح cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : لڑی_لمبائی(a);
  len_b = colon_b ? (size_t)(colon_b - b) : لڑی_لمبائی(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = لڑی_مقابلہ(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  إذا(!cmp)
    أرجع (صحيح)(len_a - len_b);

  أرجع cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ساكن CURLcode ملاؤ_duplicate_سرخیاں(بنية curl_slist *سر)
{
  بنية curl_slist *curr = سر;
  CURLcode نتیجہ = CURLE_OK;

  طالما(curr) {
    بنية curl_slist *اگلا = curr->اگلا;
    إذا(!اگلا)
      اكسر;

    إذا(compare_سرخی_ناں(curr->انکڑے, اگلا->انکڑے) == 0) {
      بنية dynbuf buf;
      ثابت حرف *colon_اگلا;
      ثابت حرف *val_اگلا;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_سرخی);

      نتیجہ = curlx_dyn_جوڑو(&buf, curr->انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;

      colon_اگلا = strchr(اگلا->انکڑے, ':');
      DEBUGASSERT(colon_اگلا);
      val_اگلا = colon_اگلا + 1;

      نتیجہ = curlx_dyn_addn(&buf, ",", 1);
      إذا(نتیجہ)
        أرجع نتیجہ;

      نتیجہ = curlx_dyn_جوڑو(&buf, val_اگلا);
      إذا(نتیجہ)
        أرجع نتیجہ;

      curlx_free(curr->انکڑے);
      curr->انکڑے = curlx_dyn_ptr(&buf);

      curr->اگلا = اگلا->اگلا;
      curlx_free(اگلا->انکڑے);
      curlx_free(اگلا);
    }
    وإلا {
      curr = curr->اگلا;
    }
  }

  أرجع CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ساكن CURLcode make_سرخیاں(بنية Curl_easy *انکڑے,
                             حرف *timestamp,
                             ثابت حرف *provider1,
                             size_t plen, /* length of provider1 */
                             حرف **date_سرخی,
                             حرف *content_sha256_سرخی,
                             بنية dynbuf *canonical_سرخیاں,
                             بنية dynbuf *signed_سرخیاں)
{
  حرف date_hdr_کلید[DATE_HDR_کلید_LEN];
  حرف date_بھریا_hdr[DATE_بھریا_HDR_LEN];
  بنية curl_slist *سر = عدم;
  بنية curl_slist *tmp_سر = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  بنية curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_کلید, DATE_HDR_کلید_LEN, "X-%.*s-Date",
                 (صحيح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_کلید[2], provider1, plen);
  date_hdr_کلید[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_بھریا_hdr, DATE_بھریا_HDR_LEN,
                 "x-%.*s-date:%s", (صحيح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_بھریا_hdr[2], provider1, plen);

  إذا(!Curl_checkheaders(انکڑے, STRCONST("Host")) &&
     انکڑے->state.http_host) {
    /* Host: [host]:[port] */
    حرف *fullhost = curlx_strdup(انکڑے->state.http_host);

    إذا(fullhost)
      سر = Curl_slist_append_nodup(عدم, fullhost);
    إذا(!سر) {
      curlx_free(fullhost);
      اذهب fail;
    }
  }

  إذا(*content_sha256_سرخی) {
    tmp_سر = curl_slist_append(سر, content_sha256_سرخی);
    إذا(!tmp_سر)
      اذهب fail;
    سر = tmp_سر;
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
  لكل(l = انکڑے->سیٹ.سرخیاں; l; l = l->اگلا) {
    حرف *dupdata;
    ثابت حرف *ptr;
    ثابت حرف *sep = strchr(l->انکڑے, ':');
    إذا(!sep)
      sep = strchr(l->انکڑے, ';');
    إذا(!sep || (*sep == ':' && !*(sep + 1)))
      استمر;
    لكل(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    إذا(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      استمر;
    dupdata = curlx_strdup(l->انکڑے);
    إذا(!dupdata)
      اذهب fail;
    dupdata[sep - l->انکڑے] = ':';
    tmp_سر = Curl_slist_append_nodup(سر, dupdata);
    إذا(!tmp_سر) {
      curlx_free(dupdata);
      اذهب fail;
    }
    سر = tmp_سر;
  }

  trim_سرخیاں(سر);

  *date_سرخی = لبھو_date_hdr(انکڑے, date_hdr_کلید);
  إذا(!*date_سرخی) {
    tmp_سر = curl_slist_append(سر, date_بھریا_hdr);
    إذا(!tmp_سر)
      اذهب fail;
    سر = tmp_سر;
    *date_سرخی = curl_maprintf("%s: %s\r\n", date_hdr_کلید, timestamp);
    إذا(!*date_سرخی)
      اذهب fail;
  }
  وإلا {
    ثابت حرف *قدر;
    ثابت حرف *endp;
    قدر = strchr(*date_سرخی, ':');
    إذا(!قدر) {
      *date_سرخی = عدم;
      اذهب fail;
    }
    ++قدر;
    curlx_str_passblanks(&قدر);
    endp = قدر;
    طالما(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    إذا((endp - قدر) == TIMESTAMP_ناپ - 1) {
      حافظہ_نقل(timestamp, قدر, TIMESTAMP_ناپ - 1);
      timestamp[TIMESTAMP_ناپ - 1] = 0;
    }
    وإلا
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_سرخی = عدم;
  }

  /* alpha-sort by header name in a case sensitive manner */
  افعل {
    again = FALSE;
    لكل(l = سر; l; l = l->اگلا) {
      بنية curl_slist *اگلا = l->اگلا;

      إذا(اگلا && compare_سرخی_ناں(l->انکڑے, اگلا->انکڑے) > 0) {
        حرف *tmp = l->انکڑے;

        l->انکڑے = اگلا->انکڑے;
        اگلا->انکڑے = tmp;
        again = TRUE;
      }
    }
  } طالما(again);

  نتیجہ = ملاؤ_duplicate_سرخیاں(سر);
  إذا(نتیجہ)
    اذهب fail;

  لكل(l = سر; l; l = l->اگلا) {
    حرف *tmp;

    إذا(curlx_dyn_جوڑو(canonical_سرخیاں, l->انکڑے))
      اذهب fail;
    إذا(curlx_dyn_جوڑو(canonical_سرخیاں, "\n"))
      اذهب fail;

    tmp = strchr(l->انکڑے, ':');
    إذا(tmp)
      *tmp = 0;

    إذا(l != سر && curlx_dyn_جوڑو(signed_سرخیاں, ";"))
      اذهب fail;
    إذا(curlx_dyn_جوڑو(signed_سرخیاں, l->انکڑے))
      اذهب fail;
  }

  نتیجہ = CURLE_OK;
fail:
  curl_slist_free_all(سر);

  أرجع نتیجہ;
}

#تعریف_کرو CONTENT_SHA256_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#تعریف_کرو CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_کلید_LEN + 2 + SHA256_HEX_لمائی)

/* try to parse a payload hash from the content-sha256 header */
ساكن ثابت حرف *parse_content_sha_hdr(بنية Curl_easy *انکڑے,
                                         ثابت حرف *provider1,
                                         size_t plen,
                                         size_t *قدر_len)
{
  حرف کلید[CONTENT_SHA256_کلید_LEN];
  size_t کلید_len;
  ثابت حرف *قدر;
  size_t len;

  کلید_len = curl_msnprintf(کلید, حجم(کلید), "x-%.*s-content-sha256",
                           (صحيح)plen, provider1);

  قدر = Curl_checkheaders(انکڑے, کلید, کلید_len);
  إذا(!قدر)
    أرجع عدم;

  قدر = strchr(قدر, ':');
  إذا(!قدر)
    أرجع عدم;
  ++قدر;

  curlx_str_passblanks(&قدر);

  len = لڑی_لمبائی(قدر);
  طالما(len > 0 && ISBLANK(قدر[len - 1]))
    --len;

  *قدر_len = len;
  أرجع قدر;
}

ساكن CURLcode calc_payload_hash(بنية Curl_easy *انکڑے,
                                  بنانشانی حرف *sha_hash, حرف *sha_hex)
{
  ثابت حرف *post_انکڑے = انکڑے->سیٹ.postfields;
  size_t post_انکڑے_len = 0;
  CURLcode نتیجہ;

  إذا(post_انکڑے) {
    إذا(انکڑے->سیٹ.postfieldsize < 0)
      post_انکڑے_len = لڑی_لمبائی(post_انکڑے);
    وإلا
      post_انکڑے_len = (size_t)انکڑے->سیٹ.postfieldsize;
  }
  نتیجہ = Curl_sha256it(sha_hash, (ثابت بنانشانی حرف *)post_انکڑے,
                         post_انکڑے_len);
  إذا(!نتیجہ)
    sha256_منزل_hex(sha_hex, sha_hash);
  أرجع نتیجہ;
}

#تعریف_کرو S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ساكن CURLcode calc_s3_payload_hash(بنية Curl_easy *انکڑے,
                                     Curl_HttpReq httpreq,
                                     ثابت حرف *provider1,
                                     size_t plen,
                                     بنانشانی حرف *sha_hash,
                                     حرف *sha_hex, حرف *سرخی)
{
  bool خالی_تھاواں_طریقہ = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_سر);
  /* The request method or filesize indicate no request payload */
  bool خالی_تھاواں_payload = (خالی_تھاواں_طریقہ || انکڑے->سیٹ.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && انکڑے->سیٹ.postfields);
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  إذا(خالی_تھاواں_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    نتیجہ = calc_payload_hash(انکڑے, sha_hash, sha_hex);
    إذا(نتیجہ)
      اذهب fail;
  }
  وإلا {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_لمائی); /* 16 < 65 */
    حافظہ_نقل(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(سرخی, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحيح)plen, provider1, sha_hex);

  نتیجہ = CURLE_OK;
fail:
  أرجع نتیجہ;
}

ساكن صحيح compare_func(ثابت فراغ *a, ثابت فراغ *b)
{
  ثابت بنية pair *aa = a;
  ثابت بنية pair *bb = b;
  ثابت size_t aa_کلید_len = curlx_dyn_len(&aa->کلید);
  ثابت size_t bb_کلید_len = curlx_dyn_len(&bb->کلید);
  ثابت size_t aa_قدر_len = curlx_dyn_len(&aa->قدر);
  ثابت size_t bb_قدر_len = curlx_dyn_len(&bb->قدر);
  صحيح compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  إذا((aa_کلید_len == 0) && (bb_کلید_len == 0))
    أرجع 0;
  إذا(aa_کلید_len == 0)
    أرجع -1;
  إذا(bb_کلید_len == 0)
    أرجع 1;
  compare = strcmp(curlx_dyn_ptr(&aa->کلید), curlx_dyn_ptr(&bb->کلید));
  إذا(compare) {
    أرجع compare;
  }

  /* Compare values */
  إذا((aa_قدر_len == 0) && (bb_قدر_len == 0))
    أرجع 0;
  إذا(aa_قدر_len == 0)
    أرجع -1;
  إذا(bb_قدر_len == 0)
    أرجع 1;
  compare = strcmp(curlx_dyn_ptr(&aa->قدر), curlx_dyn_ptr(&bb->قدر));

  أرجع compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_رستہ(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_رستہ,
                             bool do_uri_encode);
UNITTEST CURLcode canon_رستہ(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_رستہ,
                             bool do_uri_encode)
{
  CURLcode نتیجہ = CURLE_OK;

  بنية Curl_str original_رستہ;

  curlx_str_سونپو(&original_رستہ, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  إذا(do_uri_encode)
    نتیجہ = uri_encode_رستہ(&original_رستہ, new_رستہ);
  وإلا
    نتیجہ = curlx_dyn_addn(new_رستہ, q, len);

  إذا(!نتیجہ && curlx_dyn_len(new_رستہ) == 0)
    نتیجہ = curlx_dyn_جوڑو(new_رستہ, "/");

  أرجع نتیجہ;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq)
{
  CURLcode نتیجہ = CURLE_OK;

  بنية dynbuf query_array[MAX_QUERY_COMPONENTS];
  بنية pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t عدد_query_components;
  size_t counted_query_components = 0;
  size_t اشاریہ;

  إذا(!query)
    أرجع نتیجہ;

  نتیجہ = ونڈو_منزل_dyn_array(query, &query_array[0], &عدد_query_components);
  إذا(نتیجہ) {
    اذهب fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  لكل(اشاریہ = 0; اشاریہ < عدد_query_components; اشاریہ++) {
    ثابت حرف *وچ_کلید;
    size_t وچ_کلید_len;
    ثابت حرف *وتھ;
    size_t query_part_len = curlx_dyn_len(&query_array[اشاریہ]);
    ثابت حرف *query_part = curlx_dyn_ptr(&query_array[اشاریہ]);

    وچ_کلید = query_part;

    وتھ = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    إذا(!وتھ) {
      وچ_کلید_len = لڑی_لمبائی(وچ_کلید);
    }
    وإلا {
      وچ_کلید_len = وتھ - وچ_کلید;
    }

    curlx_dyn_init(&encoded_query_array[اشاریہ].کلید,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[اشاریہ].قدر,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    نتیجہ = normalize_query(وچ_کلید, وچ_کلید_len,
                             &encoded_query_array[اشاریہ].کلید);
    إذا(نتیجہ) {
      اذهب fail;
    }

    /* Decode/encode the value if it exists */
    إذا(وتھ && وتھ != (query_part + query_part_len - 1)) {
      size_t وچ_قدر_len;
      ثابت حرف *وچ_قدر = وتھ + 1;
      وچ_قدر_len = query_part + query_part_len - (وتھ + 1);
      نتیجہ = normalize_query(وچ_قدر, وچ_قدر_len,
                               &encoded_query_array[اشاریہ].قدر);
      إذا(نتیجہ) {
        اذهب fail;
      }
    }
    وإلا {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[اشاریہ].قدر, 2);
      نتیجہ = curlx_dyn_addn(&encoded_query_array[اشاریہ].قدر, "", 1);
    }

    إذا(نتیجہ) {
      اذهب fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, عدد_query_components,
        حجم(بنية pair), compare_func);

  /* Append the query components together to make a full query string */
  لكل(اشاریہ = 0; اشاریہ < عدد_query_components; اشاریہ++) {

    إذا(اشاریہ)
      نتیجہ = curlx_dyn_addn(dq, "&", 1);
    إذا(!نتیجہ) {
      ثابت حرف *کلید_ptr = curlx_dyn_ptr(&encoded_query_array[اشاریہ].کلید);
      ثابت حرف *قدر_ptr = curlx_dyn_ptr(&encoded_query_array[اشاریہ].قدر);
      size_t vlen = curlx_dyn_len(&encoded_query_array[اشاریہ].قدر);
      إذا(قدر_ptr && vlen) {
        نتیجہ = curlx_dyn_addf(dq, "%s=%s", کلید_ptr, قدر_ptr);
      }
      وإلا {
        /* Empty value is always encoded to key= */
        نتیجہ = curlx_dyn_addf(dq, "%s=", کلید_ptr);
      }
    }
    إذا(نتیجہ)
      اكسر;
  }

fail:
  إذا(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], عدد_query_components);
  أرجع نتیجہ;
}

ساكن CURLcode parse_sigv4_params(بنية Curl_easy *انکڑے,
                                   ثابت حرف *hostname,
                                   بنية Curl_str *provider0,
                                   بنية Curl_str *provider1,
                                   بنية Curl_str *region,
                                   بنية Curl_str *service)
{
  ثابت حرف *سطر = CURL_EASY_STR(انکڑے, STRING_AWS_SIGV4);
  إذا(!سطر || !*سطر)
    سطر = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  إذا(curlx_str_until(&سطر, provider0, MAX_SIGV4_LEN, ':')) {
    failf(انکڑے, "first aws-sigv4 provider cannot be empty");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  إذا(curlx_str_single(&سطر, ':') ||
     curlx_str_until(&سطر, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  وإلا إذا(curlx_str_single(&سطر, ':') ||
          curlx_str_until(&سطر, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&سطر, ':') ||
          curlx_str_until(&سطر, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  إذا(!curlx_strlen(service)) {
    ثابت حرف *p = hostname;
    إذا(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(انکڑے, "aws-sigv4: service missing in parameters and hostname");
      أرجع CURLE_URL_MALFORMAT;
    }

    infof(انکڑے, "aws_sigv4: picked service %.*s from host",
          (صحيح)curlx_strlen(service), curlx_str(service));

    إذا(!curlx_strlen(region)) {
      إذا(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(انکڑے, "aws-sigv4: region missing in parameters and hostname");
        أرجع CURLE_URL_MALFORMAT;
      }
      infof(انکڑے, "aws_sigv4: picked region %.*s from host",
            (صحيح)curlx_strlen(region), curlx_str(region));
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode get_payload_hash(بنية Curl_easy *انکڑے,
                                 Curl_HttpReq httpreq,
                                 بنية Curl_str *provider0,
                                 بنية Curl_str *provider1,
                                 بنية Curl_str *service,
                                 بنانشانی حرف *sha_hash,
                                 حرف *sha_hex,
                                 حرف *content_sha256_hdr,
                                 ثابت حرف **payload_hash_باہرلا,
                                 size_t *payload_hash_len_باہرلا)
{
  *payload_hash_باہرلا =
    parse_content_sha_hdr(انکڑے, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_باہرلا);

  إذا(!*payload_hash_باہرلا) {
    CURLcode نتیجہ;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    إذا(sign_as_s3)
      نتیجہ = calc_s3_payload_hash(انکڑے, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    وإلا
      نتیجہ = calc_payload_hash(انکڑے, sha_hash, sha_hex);
    إذا(نتیجہ)
      أرجع نتیجہ;

    *payload_hash_باہرلا = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_باہرلا = لڑی_لمبائی(sha_hex);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode get_timestamp(حرف *timestamp, size_t stampsize)
{
  time_t clock;
  بنية tm tm;
  CURLcode نتیجہ;

#جے_تعریف_شدہ DEBUGBUILD
  {
    حرف *force_timestamp = getenv("CURL_FORCETIME");
    إذا(force_timestamp)
      clock = 0;
    وإلا
      clock = ویلا(عدم);
  }
#وإلا
  clock = ویلا(عدم);
#جے_انت
  نتیجہ = curlx_gmtime(clock, &tm);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع CURLE_OK;
}

ساكن CURLcode make_canonical_request(بنية Curl_easy *انکڑے,
                                       حرف *timestamp,
                                       بنية Curl_str *provider1,
                                       بنية Curl_str *service,
                                       ثابت حرف *طریقہ,
                                       ثابت حرف *payload_hash,
                                       size_t payload_hash_len,
                                       حرف **date_سرخی_باہرلا,
                                       حرف *content_sha256_hdr,
                                       بنية dynbuf *canonical_سرخیاں,
                                       بنية dynbuf *signed_سرخیاں,
                                       حرف **canonical_request_باہرلا)
{
  بنية dynbuf canonical_query;
  بنية dynbuf canonical_رستہ;
  CURLcode نتیجہ;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&canonical_رستہ, CURL_MAX_HTTP_سرخی);

  نتیجہ = make_سرخیاں(انکڑے, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_سرخی_باہرلا, content_sha256_hdr,
                        canonical_سرخیاں, signed_سرخیاں);
  إذا(نتیجہ)
    اذهب fail;

  نتیجہ = canon_query(انکڑے->state.اتے.query, &canonical_query);
  إذا(نتیجہ)
    اذهب fail;

  نتیجہ = canon_رستہ(انکڑے->state.اتے.رستہ, لڑی_لمبائی(انکڑے->state.اتے.رستہ),
                      &canonical_رستہ,
                      should_urlencode(service));
  إذا(نتیجہ)
    اذهب fail;

  *canonical_request_باہرلا =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  طریقہ,
                  curlx_dyn_ptr(&canonical_رستہ),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_سرخیاں),
                  curlx_dyn_ptr(signed_سرخیاں),
                  (صحيح)payload_hash_len, payload_hash);
  إذا(!*canonical_request_باہرلا) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

  نتیجہ = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_رستہ);
  أرجع نتیجہ;
}

ساكن CURLcode make_string_منزل_sign(بنية Curl_easy *انکڑے,
                                    بنية Curl_str *provider0,
                                    بنية Curl_str *region,
                                    بنية Curl_str *service,
                                    ثابت حرف *date,
                                    ثابت حرف *timestamp,
                                    ثابت حرف *canonical_request,
                                    حرف **request_type_باہرلا,
                                    حرف **credential_scope_باہرلا,
                                    حرف **str_منزل_sign_باہرلا)
{
  حرف *request_type;
  حرف *credential_scope;
  حرف *str_منزل_sign;
  بنانشانی حرف sha_hash[CURL_SHA256_DIGEST_لمائی];
  حرف sha_hex[SHA256_HEX_لمائی];

  request_type = curl_maprintf("%.*s4_request",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0));
  إذا(!request_type)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (صحيح)curlx_strlen(region),
                                   curlx_str(region),
                                   (صحيح)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  إذا(!credential_scope) {
    curlx_free(request_type);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  إذا(Curl_sha256it(sha_hash, (ثابت بنانشانی حرف *)canonical_request,
                   لڑی_لمبائی(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  sha256_منزل_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_منزل_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (صحيح)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  إذا(!str_منزل_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_منزل_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(انکڑے, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_منزل_sign);

  *request_type_باہرلا = request_type;
  *credential_scope_باہرلا = credential_scope;
  *str_منزل_sign_باہرلا = str_منزل_sign;
  أرجع CURLE_OK;
}

ساكن CURLcode sign_and_سیٹ_auth_سرخیاں(بنية Curl_easy *انکڑے,
                                          بنية Curl_str *provider0,
                                          بنية Curl_str *region,
                                          بنية Curl_str *service,
                                          ثابت حرف *request_type,
                                          ثابت حرف *credential_scope,
                                          ثابت حرف *date,
                                          ثابت حرف *str_منزل_sign,
                                          ثابت حرف *date_سرخی,
                                          ثابت حرف *content_sha256_hdr,
                                          بنية dynbuf *signed_سرخیاں)
{
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  ثابت حرف *passwd = Curl_creds_passwd(انکڑے->state.creds);
  حرف *secret = عدم;
  بنانشانی حرف sign0[CURL_SHA256_DIGEST_لمائی] = { 0 };
  بنانشانی حرف sign1[CURL_SHA256_DIGEST_لمائی] = { 0 };
  حرف sha_hex[SHA256_HEX_لمائی];
  حرف *auth_سرخیاں = عدم;
  حرف *user = curl_escape(Curl_creds_user(انکڑے->state.creds), 0);
  إذا(!user)
    أرجع CURLE_باہرلا_دا_حافظہ;

  secret = curl_maprintf("%.*s4%s", (صحيح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  إذا(!secret)
    اذهب fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, لڑی_لمبائی(secret), date, لڑی_لمبائی(date), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              request_type, لڑی_لمبائی(request_type), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              str_منزل_sign, لڑی_لمبائی(str_منزل_sign), sign0);

  sha256_منزل_hex(sha_hex, sign0);

  infof(انکڑے, "aws_sigv4: Signature - %s", sha_hex);

  auth_سرخیاں = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_سرخیاں),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_سرخی ? date_سرخی : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  إذا(!auth_سرخیاں)
    اذهب fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_سرخیاں[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(انکڑے->req.hd_auth);
  انکڑے->req.hd_auth = auth_سرخیاں;
  انکڑے->state.authhost.پورا = TRUE;
  نتیجہ = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  أرجع نتیجہ;
}

CURLcode Curl_output_aws_sigv4(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  بنية Curl_str provider0 = { عدم, 0 };
  بنية Curl_str provider1 = { عدم, 0 };
  بنية Curl_str region = { عدم, 0 };
  بنية Curl_str service = { عدم, 0 };
  ثابت حرف *hostname = انکڑے->state.origin->hostname;
  حرف timestamp[TIMESTAMP_ناپ];
  حرف date[9];
  بنية dynbuf canonical_سرخیاں;
  بنية dynbuf signed_سرخیاں;
  حرف *date_سرخی = عدم;
  Curl_HttpReq httpreq;
  ثابت حرف *طریقہ = عدم;
  ثابت حرف *payload_hash = عدم;
  size_t payload_hash_len = 0;
  بنانشانی حرف sha_hash[CURL_SHA256_DIGEST_لمائی];
  حرف sha_hex[SHA256_HEX_لمائی];
  حرف content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  حرف *canonical_request = عدم;
  حرف *request_type = عدم;
  حرف *credential_scope = عدم;
  حرف *str_منزل_sign = عدم;

  إذا(انکڑے->سیٹ.رستہ_as_is) {
    failf(انکڑے, "Cannot use sigv4 authentication with path-as-is flag");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(Curl_checkheaders(انکڑے, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    أرجع CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_سرخیاں, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&signed_سرخیاں, CURL_MAX_HTTP_سرخی);

  نتیجہ = parse_sigv4_params(انکڑے, hostname, &provider0, &provider1,
                              &region, &service);
  إذا(!نتیجہ) {
    Curl_http_طریقہ(انکڑے, &طریقہ, &httpreq);
    نتیجہ = get_payload_hash(انکڑے, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  إذا(!نتیجہ)
    نتیجہ = get_timestamp(timestamp, حجم(timestamp));

  إذا(!نتیجہ)
    نتیجہ = make_canonical_request(انکڑے, timestamp,
                                    &provider1, &service,
                                    طریقہ, payload_hash, payload_hash_len,
                                    &date_سرخی, content_sha256_hdr,
                                    &canonical_سرخیاں, &signed_سرخیاں,
                                    &canonical_request);
  إذا(!نتیجہ) {
    /* the timestamp might have been updated in make_canonical_request */
    حافظہ_نقل(date, timestamp, حجم(date) - 1);
    date[حجم(date) - 1] = 0;

    نتیجہ = make_string_منزل_sign(انکڑے, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_منزل_sign);
  }
  إذا(!نتیجہ)
    نتیجہ = sign_and_سیٹ_auth_سرخیاں(انکڑے, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_منزل_sign, date_سرخی,
                                       content_sha256_hdr, &signed_سرخیاں);

  curlx_dyn_free(&canonical_سرخیاں);
  curlx_dyn_free(&signed_سرخیاں);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_منزل_sign);
  curlx_free(date_سرخی);
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
