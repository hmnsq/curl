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

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#شامل "urldata.h"
#شامل "strcase.h"
#شامل "curlx/strdup.h"
#شامل "http_aws_sigv4.h"
#شامل "curl_sha256.h"
#شامل "transfer.h"
#شامل "curl_trc.h"
#شامل "escape.h"
#شامل "curlx/strparse.h"
#شامل "slist.h"

#شامل <time.h>

#تعریف_کرو HMAC_SHA256(k, kl, d, dl, o)                 \
  کرو {                                               \
    نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت غیرعلامتی حرف *)(k), \
                         kl,                         \
                         (ثابت غیرعلامتی حرف *)(d), \
                         dl, o);                     \
    اگر(نتیجہ) {                                     \
      جاؤ fail;                                     \
    }                                                \
  } جب(0)

#تعریف_کرو TIMESTAMP_سائز 17

/* hex-encoded with null-terminator */
#تعریف_کرو SHA256_HEX_لمبائی ((2 * CURL_SHA256_DIGEST_لمبائی) + 1)

#تعریف_کرو MAX_QUERY_COMPONENTS 128

ساخت pair {
  ساخت dynbuf کلید;
  ساخت dynbuf قدر;
};

ساکن خالی sha256_مقصد_hex(حرف *dst, غیرعلامتی حرف *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_لمبائی,
                 (غیرعلامتی حرف *)dst, SHA256_HEX_لمبائی);
}

ساکن حرف *ڈھونڈو_date_hdr(ساخت Curl_easy *ڈیٹا, ثابت حرف *sig_hdr)
{
  حرف *tmp = Curl_checkheaders(ڈیٹا, sig_hdr, لڑی_لمبائی(sig_hdr));

  اگر(tmp)
    واپس tmp;
  واپس Curl_checkheaders(ڈیٹا, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ساکن خالی trim_سرخیاں(ساخت curl_slist *سر)
{
  ساخت curl_slist *l;
  برائے(l = سر; l; l = l->اگلا) {
    ثابت حرف *قدر; /* to read from */
    حرف *store;
    size_t colon = strcspn(l->ڈیٹا, ":");
    Curl_strntolower(l->ڈیٹا, l->ڈیٹا, colon);

    قدر = &l->ڈیٹا[colon];
    اگر(!*قدر)
      جاری;
    ++قدر;
    store = (حرف *)CURL_UNCONST(قدر);

    /* skip leading whitespace */
    curlx_str_passblanks(&قدر);

    جب(*قدر) {
      صحیح space = 0;
      جب(ISBLANK(*قدر)) {
        قدر++;
        space++;
      }
      اگر(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        اگر(*قدر)
          *store++ = ' ';
      }
      ورنہ
        *store++ = *قدر++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ساکن خالی pair_array_free(ساخت pair *pair_array, size_t عدد_elements)
{
  size_t اشاریہ;

  برائے(اشاریہ = 0; اشاریہ != عدد_elements; اشاریہ++) {
    curlx_dyn_free(&pair_array[اشاریہ].کلید);
    curlx_dyn_free(&pair_array[اشاریہ].قدر);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ساکن خالی dyn_array_free(ساخت dynbuf *db, size_t عدد_elements)
{
  size_t اشاریہ;

  برائے(اشاریہ = 0; اشاریہ < عدد_elements; اشاریہ++)
    curlx_dyn_free((&db[اشاریہ]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#تعریف_کرو بانٹو_BY '&'

ساکن CURLcode بانٹو_مقصد_dyn_array(ثابت حرف *ماخذ,
                                   ساخت dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *عدد_splits_برآمد)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t len = لڑی_لمبائی(ماخذ);
  size_t pos;         /* Position in result buffer */
  size_t آغاز = 0;   /* Start of current segment */
  size_t segment_لمبائی = 0;
  size_t اشاریہ = 0;
  size_t عدد_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  برائے(pos = 0; pos < len; pos++) {
    اگر(ماخذ[pos] == بانٹو_BY) {
      اگر(segment_لمبائی) {
        curlx_dyn_init(&db[اشاریہ], segment_لمبائی + 1);
        نتیجہ = curlx_dyn_addn(&db[اشاریہ], &ماخذ[آغاز], segment_لمبائی);
        اگر(نتیجہ)
          جاؤ fail;

        segment_لمبائی = 0;
        اشاریہ++;
        اگر(++عدد_splits == MAX_QUERY_COMPONENTS) {
          نتیجہ = CURLE_TOO_LARGE;
          جاؤ fail;
        }
      }
      آغاز = pos + 1;
    }
    ورنہ {
      segment_لمبائی++;
    }
  }

  اگر(segment_لمبائی) {
    curlx_dyn_init(&db[اشاریہ], segment_لمبائی + 1);
    نتیجہ = curlx_dyn_addn(&db[اشاریہ], &ماخذ[آغاز], segment_لمبائی);
    اگر(!نتیجہ && ++عدد_splits == MAX_QUERY_COMPONENTS)
      نتیجہ = CURLE_TOO_LARGE;
  }
fail:
  *عدد_splits_برآمد = عدد_splits;
  واپس نتیجہ;
}

ساکن bool is_reserved_char(ثابت حرف c)
{
  واپس (ISALNUM(c) || ISURLPUNTCS(c));
}

ساکن CURLcode uri_encode_راستہ(ساخت Curl_str *original_راستہ,
                                ساخت dynbuf *new_راستہ)
{
  ثابت حرف *p = curlx_str(original_راستہ);
  size_t i;

  برائے(i = 0; i < curlx_strlen(original_راستہ); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode نتیجہ = CURLE_OK;
    غیرعلامتی حرف c = p[i];
    اگر(is_reserved_char(c) || c == '/')
      نتیجہ = curlx_dyn_addn(new_راستہ, &c, 1);
    ورنہ
      نتیجہ = curlx_dyn_addf(new_راستہ, "%%%02X", c);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  واپس CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ساکن CURLcode normalize_query(ثابت حرف *string, size_t len,
                                ساخت dynbuf *db)
{
  CURLcode نتیجہ = CURLE_OK;

  جب(len && !نتیجہ) {
    غیرعلامتی حرف اندر = (غیرعلامتی حرف)*string;
    اگر(('%' == اندر) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      اندر = (غیرعلامتی حرف)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      اگر(اندر == '+') {
        /* decodes to plus, so leave this encoded */
        نتیجہ = curlx_dyn_addn(db, "%2B", 3);
        جاری;
      }
    }
    ورنہ {
      string++;
      len--;
    }

    اگر(is_reserved_char(اندر))
      /* Escape unreserved chars from RFC 3986 */
      نتیجہ = curlx_dyn_addn(db, &اندر, 1);
    ورنہ اگر(اندر == '+')
      /* Encode '+' as space */
      نتیجہ = curlx_dyn_جوڑو(db, "%20");
    ورنہ
      نتیجہ = curlx_dyn_addf(db, "%%%02X", اندر);
  }

  واپس نتیجہ;
}

ساکن bool should_urlencode(ساخت Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  اگر(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    واپس FALSE;
  }
  واپس TRUE;
}

/* maximum length for the aws sivg4 parts */
#تعریف_کرو MAX_SIGV4_LEN    64
#تعریف_کرو DATE_HDR_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#تعریف_کرو DATE_بھرا_HDR_LEN (DATE_HDR_کلید_LEN + TIMESTAMP_سائز + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ساکن صحیح compare_سرخی_نام(ثابت حرف *a, ثابت حرف *b)
{
  ثابت حرف *colon_a;
  ثابت حرف *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  صحیح cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : لڑی_لمبائی(a);
  len_b = colon_b ? (size_t)(colon_b - b) : لڑی_لمبائی(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = لڑی_موازنہ(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  اگر(!cmp)
    واپس (صحیح)(len_a - len_b);

  واپس cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ساکن CURLcode ملاؤ_duplicate_سرخیاں(ساخت curl_slist *سر)
{
  ساخت curl_slist *curr = سر;
  CURLcode نتیجہ = CURLE_OK;

  جب(curr) {
    ساخت curl_slist *اگلا = curr->اگلا;
    اگر(!اگلا)
      توڑو;

    اگر(compare_سرخی_نام(curr->ڈیٹا, اگلا->ڈیٹا) == 0) {
      ساخت dynbuf buf;
      ثابت حرف *colon_اگلا;
      ثابت حرف *val_اگلا;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_سرخی);

      نتیجہ = curlx_dyn_جوڑو(&buf, curr->ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;

      colon_اگلا = strchr(اگلا->ڈیٹا, ':');
      DEBUGASSERT(colon_اگلا);
      val_اگلا = colon_اگلا + 1;

      نتیجہ = curlx_dyn_addn(&buf, ",", 1);
      اگر(نتیجہ)
        واپس نتیجہ;

      نتیجہ = curlx_dyn_جوڑو(&buf, val_اگلا);
      اگر(نتیجہ)
        واپس نتیجہ;

      curlx_free(curr->ڈیٹا);
      curr->ڈیٹا = curlx_dyn_ptr(&buf);

      curr->اگلا = اگلا->اگلا;
      curlx_free(اگلا->ڈیٹا);
      curlx_free(اگلا);
    }
    ورنہ {
      curr = curr->اگلا;
    }
  }

  واپس CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ساکن CURLcode make_سرخیاں(ساخت Curl_easy *ڈیٹا,
                             حرف *timestamp,
                             ثابت حرف *provider1,
                             size_t plen, /* length of provider1 */
                             حرف **date_سرخی,
                             حرف *content_sha256_سرخی,
                             ساخت dynbuf *canonical_سرخیاں,
                             ساخت dynbuf *signed_سرخیاں)
{
  حرف date_hdr_کلید[DATE_HDR_کلید_LEN];
  حرف date_بھرا_hdr[DATE_بھرا_HDR_LEN];
  ساخت curl_slist *سر = تہی;
  ساخت curl_slist *tmp_سر = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  ساخت curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_کلید, DATE_HDR_کلید_LEN, "X-%.*s-Date",
                 (صحیح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_کلید[2], provider1, plen);
  date_hdr_کلید[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_بھرا_hdr, DATE_بھرا_HDR_LEN,
                 "x-%.*s-date:%s", (صحیح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_بھرا_hdr[2], provider1, plen);

  اگر(!Curl_checkheaders(ڈیٹا, STRCONST("Host")) &&
     ڈیٹا->state.http_host) {
    /* Host: [host]:[port] */
    حرف *fullhost = curlx_strdup(ڈیٹا->state.http_host);

    اگر(fullhost)
      سر = Curl_slist_append_nodup(تہی, fullhost);
    اگر(!سر) {
      curlx_free(fullhost);
      جاؤ fail;
    }
  }

  اگر(*content_sha256_سرخی) {
    tmp_سر = curl_slist_append(سر, content_sha256_سرخی);
    اگر(!tmp_سر)
      جاؤ fail;
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
  برائے(l = ڈیٹا->سیٹ.سرخیاں; l; l = l->اگلا) {
    حرف *dupdata;
    ثابت حرف *ptr;
    ثابت حرف *sep = strchr(l->ڈیٹا, ':');
    اگر(!sep)
      sep = strchr(l->ڈیٹا, ';');
    اگر(!sep || (*sep == ':' && !*(sep + 1)))
      جاری;
    برائے(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    اگر(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      جاری;
    dupdata = curlx_strdup(l->ڈیٹا);
    اگر(!dupdata)
      جاؤ fail;
    dupdata[sep - l->ڈیٹا] = ':';
    tmp_سر = Curl_slist_append_nodup(سر, dupdata);
    اگر(!tmp_سر) {
      curlx_free(dupdata);
      جاؤ fail;
    }
    سر = tmp_سر;
  }

  trim_سرخیاں(سر);

  *date_سرخی = ڈھونڈو_date_hdr(ڈیٹا, date_hdr_کلید);
  اگر(!*date_سرخی) {
    tmp_سر = curl_slist_append(سر, date_بھرا_hdr);
    اگر(!tmp_سر)
      جاؤ fail;
    سر = tmp_سر;
    *date_سرخی = curl_maprintf("%s: %s\r\n", date_hdr_کلید, timestamp);
    اگر(!*date_سرخی)
      جاؤ fail;
  }
  ورنہ {
    ثابت حرف *قدر;
    ثابت حرف *endp;
    قدر = strchr(*date_سرخی, ':');
    اگر(!قدر) {
      *date_سرخی = تہی;
      جاؤ fail;
    }
    ++قدر;
    curlx_str_passblanks(&قدر);
    endp = قدر;
    جب(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    اگر((endp - قدر) == TIMESTAMP_سائز - 1) {
      حافظہ_نقل(timestamp, قدر, TIMESTAMP_سائز - 1);
      timestamp[TIMESTAMP_سائز - 1] = 0;
    }
    ورنہ
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_سرخی = تہی;
  }

  /* alpha-sort by header name in a case sensitive manner */
  کرو {
    again = FALSE;
    برائے(l = سر; l; l = l->اگلا) {
      ساخت curl_slist *اگلا = l->اگلا;

      اگر(اگلا && compare_سرخی_نام(l->ڈیٹا, اگلا->ڈیٹا) > 0) {
        حرف *tmp = l->ڈیٹا;

        l->ڈیٹا = اگلا->ڈیٹا;
        اگلا->ڈیٹا = tmp;
        again = TRUE;
      }
    }
  } جب(again);

  نتیجہ = ملاؤ_duplicate_سرخیاں(سر);
  اگر(نتیجہ)
    جاؤ fail;

  برائے(l = سر; l; l = l->اگلا) {
    حرف *tmp;

    اگر(curlx_dyn_جوڑو(canonical_سرخیاں, l->ڈیٹا))
      جاؤ fail;
    اگر(curlx_dyn_جوڑو(canonical_سرخیاں, "\n"))
      جاؤ fail;

    tmp = strchr(l->ڈیٹا, ':');
    اگر(tmp)
      *tmp = 0;

    اگر(l != سر && curlx_dyn_جوڑو(signed_سرخیاں, ";"))
      جاؤ fail;
    اگر(curlx_dyn_جوڑو(signed_سرخیاں, l->ڈیٹا))
      جاؤ fail;
  }

  نتیجہ = CURLE_OK;
fail:
  curl_slist_free_all(سر);

  واپس نتیجہ;
}

#تعریف_کرو CONTENT_SHA256_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#تعریف_کرو CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_کلید_LEN + 2 + SHA256_HEX_لمبائی)

/* try to parse a payload hash from the content-sha256 header */
ساکن ثابت حرف *parse_content_sha_hdr(ساخت Curl_easy *ڈیٹا,
                                         ثابت حرف *provider1,
                                         size_t plen,
                                         size_t *قدر_len)
{
  حرف کلید[CONTENT_SHA256_کلید_LEN];
  size_t کلید_len;
  ثابت حرف *قدر;
  size_t len;

  کلید_len = curl_msnprintf(کلید, حجم(کلید), "x-%.*s-content-sha256",
                           (صحیح)plen, provider1);

  قدر = Curl_checkheaders(ڈیٹا, کلید, کلید_len);
  اگر(!قدر)
    واپس تہی;

  قدر = strchr(قدر, ':');
  اگر(!قدر)
    واپس تہی;
  ++قدر;

  curlx_str_passblanks(&قدر);

  len = لڑی_لمبائی(قدر);
  جب(len > 0 && ISBLANK(قدر[len - 1]))
    --len;

  *قدر_len = len;
  واپس قدر;
}

ساکن CURLcode calc_payload_hash(ساخت Curl_easy *ڈیٹا,
                                  غیرعلامتی حرف *sha_hash, حرف *sha_hex)
{
  ثابت حرف *post_ڈیٹا = ڈیٹا->سیٹ.postfields;
  size_t post_ڈیٹا_len = 0;
  CURLcode نتیجہ;

  اگر(post_ڈیٹا) {
    اگر(ڈیٹا->سیٹ.postfieldsize < 0)
      post_ڈیٹا_len = لڑی_لمبائی(post_ڈیٹا);
    ورنہ
      post_ڈیٹا_len = (size_t)ڈیٹا->سیٹ.postfieldsize;
  }
  نتیجہ = Curl_sha256it(sha_hash, (ثابت غیرعلامتی حرف *)post_ڈیٹا,
                         post_ڈیٹا_len);
  اگر(!نتیجہ)
    sha256_مقصد_hex(sha_hex, sha_hash);
  واپس نتیجہ;
}

#تعریف_کرو S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ساکن CURLcode calc_s3_payload_hash(ساخت Curl_easy *ڈیٹا,
                                     Curl_HttpReq httpreq,
                                     ثابت حرف *provider1,
                                     size_t plen,
                                     غیرعلامتی حرف *sha_hash,
                                     حرف *sha_hex, حرف *سرخی)
{
  bool خالی_خانے_طریقہ = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_سر);
  /* The request method or filesize indicate no request payload */
  bool خالی_خانے_payload = (خالی_خانے_طریقہ || ڈیٹا->سیٹ.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && ڈیٹا->سیٹ.postfields);
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  اگر(خالی_خانے_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    نتیجہ = calc_payload_hash(ڈیٹا, sha_hash, sha_hex);
    اگر(نتیجہ)
      جاؤ fail;
  }
  ورنہ {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_لمبائی); /* 16 < 65 */
    حافظہ_نقل(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(سرخی, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحیح)plen, provider1, sha_hex);

  نتیجہ = CURLE_OK;
fail:
  واپس نتیجہ;
}

ساکن صحیح compare_func(ثابت خالی *a, ثابت خالی *b)
{
  ثابت ساخت pair *aa = a;
  ثابت ساخت pair *bb = b;
  ثابت size_t aa_کلید_len = curlx_dyn_len(&aa->کلید);
  ثابت size_t bb_کلید_len = curlx_dyn_len(&bb->کلید);
  ثابت size_t aa_قدر_len = curlx_dyn_len(&aa->قدر);
  ثابت size_t bb_قدر_len = curlx_dyn_len(&bb->قدر);
  صحیح compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  اگر((aa_کلید_len == 0) && (bb_کلید_len == 0))
    واپس 0;
  اگر(aa_کلید_len == 0)
    واپس -1;
  اگر(bb_کلید_len == 0)
    واپس 1;
  compare = strcmp(curlx_dyn_ptr(&aa->کلید), curlx_dyn_ptr(&bb->کلید));
  اگر(compare) {
    واپس compare;
  }

  /* Compare values */
  اگر((aa_قدر_len == 0) && (bb_قدر_len == 0))
    واپس 0;
  اگر(aa_قدر_len == 0)
    واپس -1;
  اگر(bb_قدر_len == 0)
    واپس 1;
  compare = strcmp(curlx_dyn_ptr(&aa->قدر), curlx_dyn_ptr(&bb->قدر));

  واپس compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_راستہ(ثابت حرف *q, size_t len,
                             ساخت dynbuf *new_راستہ,
                             bool do_uri_encode);
UNITTEST CURLcode canon_راستہ(ثابت حرف *q, size_t len,
                             ساخت dynbuf *new_راستہ,
                             bool do_uri_encode)
{
  CURLcode نتیجہ = CURLE_OK;

  ساخت Curl_str original_راستہ;

  curlx_str_تفویض(&original_راستہ, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  اگر(do_uri_encode)
    نتیجہ = uri_encode_راستہ(&original_راستہ, new_راستہ);
  ورنہ
    نتیجہ = curlx_dyn_addn(new_راستہ, q, len);

  اگر(!نتیجہ && curlx_dyn_len(new_راستہ) == 0)
    نتیجہ = curlx_dyn_جوڑو(new_راستہ, "/");

  واپس نتیجہ;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت حرف *query, ساخت dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت حرف *query, ساخت dynbuf *dq)
{
  CURLcode نتیجہ = CURLE_OK;

  ساخت dynbuf query_array[MAX_QUERY_COMPONENTS];
  ساخت pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t عدد_query_components;
  size_t counted_query_components = 0;
  size_t اشاریہ;

  اگر(!query)
    واپس نتیجہ;

  نتیجہ = بانٹو_مقصد_dyn_array(query, &query_array[0], &عدد_query_components);
  اگر(نتیجہ) {
    جاؤ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  برائے(اشاریہ = 0; اشاریہ < عدد_query_components; اشاریہ++) {
    ثابت حرف *اندر_کلید;
    size_t اندر_کلید_len;
    ثابت حرف *انحراف;
    size_t query_part_len = curlx_dyn_len(&query_array[اشاریہ]);
    ثابت حرف *query_part = curlx_dyn_ptr(&query_array[اشاریہ]);

    اندر_کلید = query_part;

    انحراف = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    اگر(!انحراف) {
      اندر_کلید_len = لڑی_لمبائی(اندر_کلید);
    }
    ورنہ {
      اندر_کلید_len = انحراف - اندر_کلید;
    }

    curlx_dyn_init(&encoded_query_array[اشاریہ].کلید,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[اشاریہ].قدر,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    نتیجہ = normalize_query(اندر_کلید, اندر_کلید_len,
                             &encoded_query_array[اشاریہ].کلید);
    اگر(نتیجہ) {
      جاؤ fail;
    }

    /* Decode/encode the value if it exists */
    اگر(انحراف && انحراف != (query_part + query_part_len - 1)) {
      size_t اندر_قدر_len;
      ثابت حرف *اندر_قدر = انحراف + 1;
      اندر_قدر_len = query_part + query_part_len - (انحراف + 1);
      نتیجہ = normalize_query(اندر_قدر, اندر_قدر_len,
                               &encoded_query_array[اشاریہ].قدر);
      اگر(نتیجہ) {
        جاؤ fail;
      }
    }
    ورنہ {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[اشاریہ].قدر, 2);
      نتیجہ = curlx_dyn_addn(&encoded_query_array[اشاریہ].قدر, "", 1);
    }

    اگر(نتیجہ) {
      جاؤ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, عدد_query_components,
        حجم(ساخت pair), compare_func);

  /* Append the query components together to make a full query string */
  برائے(اشاریہ = 0; اشاریہ < عدد_query_components; اشاریہ++) {

    اگر(اشاریہ)
      نتیجہ = curlx_dyn_addn(dq, "&", 1);
    اگر(!نتیجہ) {
      ثابت حرف *کلید_ptr = curlx_dyn_ptr(&encoded_query_array[اشاریہ].کلید);
      ثابت حرف *قدر_ptr = curlx_dyn_ptr(&encoded_query_array[اشاریہ].قدر);
      size_t vlen = curlx_dyn_len(&encoded_query_array[اشاریہ].قدر);
      اگر(قدر_ptr && vlen) {
        نتیجہ = curlx_dyn_addf(dq, "%s=%s", کلید_ptr, قدر_ptr);
      }
      ورنہ {
        /* Empty value is always encoded to key= */
        نتیجہ = curlx_dyn_addf(dq, "%s=", کلید_ptr);
      }
    }
    اگر(نتیجہ)
      توڑو;
  }

fail:
  اگر(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], عدد_query_components);
  واپس نتیجہ;
}

ساکن CURLcode parse_sigv4_params(ساخت Curl_easy *ڈیٹا,
                                   ثابت حرف *hostname,
                                   ساخت Curl_str *provider0,
                                   ساخت Curl_str *provider1,
                                   ساخت Curl_str *region,
                                   ساخت Curl_str *service)
{
  ثابت حرف *سطر = CURL_EASY_STR(ڈیٹا, STRING_AWS_SIGV4);
  اگر(!سطر || !*سطر)
    سطر = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  اگر(curlx_str_until(&سطر, provider0, MAX_SIGV4_LEN, ':')) {
    failf(ڈیٹا, "first aws-sigv4 provider cannot be empty");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  اگر(curlx_str_single(&سطر, ':') ||
     curlx_str_until(&سطر, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  ورنہ اگر(curlx_str_single(&سطر, ':') ||
          curlx_str_until(&سطر, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&سطر, ':') ||
          curlx_str_until(&سطر, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  اگر(!curlx_strlen(service)) {
    ثابت حرف *p = hostname;
    اگر(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(ڈیٹا, "aws-sigv4: service missing in parameters and hostname");
      واپس CURLE_URL_MALFORMAT;
    }

    infof(ڈیٹا, "aws_sigv4: picked service %.*s from host",
          (صحیح)curlx_strlen(service), curlx_str(service));

    اگر(!curlx_strlen(region)) {
      اگر(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(ڈیٹا, "aws-sigv4: region missing in parameters and hostname");
        واپس CURLE_URL_MALFORMAT;
      }
      infof(ڈیٹا, "aws_sigv4: picked region %.*s from host",
            (صحیح)curlx_strlen(region), curlx_str(region));
    }
  }

  واپس CURLE_OK;
}

ساکن CURLcode get_payload_hash(ساخت Curl_easy *ڈیٹا,
                                 Curl_HttpReq httpreq,
                                 ساخت Curl_str *provider0,
                                 ساخت Curl_str *provider1,
                                 ساخت Curl_str *service,
                                 غیرعلامتی حرف *sha_hash,
                                 حرف *sha_hex,
                                 حرف *content_sha256_hdr,
                                 ثابت حرف **payload_hash_برآمد,
                                 size_t *payload_hash_len_برآمد)
{
  *payload_hash_برآمد =
    parse_content_sha_hdr(ڈیٹا, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_برآمد);

  اگر(!*payload_hash_برآمد) {
    CURLcode نتیجہ;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    اگر(sign_as_s3)
      نتیجہ = calc_s3_payload_hash(ڈیٹا, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    ورنہ
      نتیجہ = calc_payload_hash(ڈیٹا, sha_hash, sha_hex);
    اگر(نتیجہ)
      واپس نتیجہ;

    *payload_hash_برآمد = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_برآمد = لڑی_لمبائی(sha_hex);
  }
  واپس CURLE_OK;
}

ساکن CURLcode get_timestamp(حرف *timestamp, size_t stampsize)
{
  time_t clock;
  ساخت tm tm;
  CURLcode نتیجہ;

#اگر_تعریف_شدہ DEBUGBUILD
  {
    حرف *force_timestamp = getenv("CURL_FORCETIME");
    اگر(force_timestamp)
      clock = 0;
    ورنہ
      clock = وقت(تہی);
  }
#ورنہ
  clock = وقت(تہی);
#اگر_ختم
  نتیجہ = curlx_gmtime(clock, &tm);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

ساکن CURLcode make_canonical_request(ساخت Curl_easy *ڈیٹا,
                                       حرف *timestamp,
                                       ساخت Curl_str *provider1,
                                       ساخت Curl_str *service,
                                       ثابت حرف *طریقہ,
                                       ثابت حرف *payload_hash,
                                       size_t payload_hash_len,
                                       حرف **date_سرخی_برآمد,
                                       حرف *content_sha256_hdr,
                                       ساخت dynbuf *canonical_سرخیاں,
                                       ساخت dynbuf *signed_سرخیاں,
                                       حرف **canonical_request_برآمد)
{
  ساخت dynbuf canonical_query;
  ساخت dynbuf canonical_راستہ;
  CURLcode نتیجہ;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&canonical_راستہ, CURL_MAX_HTTP_سرخی);

  نتیجہ = make_سرخیاں(ڈیٹا, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_سرخی_برآمد, content_sha256_hdr,
                        canonical_سرخیاں, signed_سرخیاں);
  اگر(نتیجہ)
    جاؤ fail;

  نتیجہ = canon_query(ڈیٹا->state.اوپر.query, &canonical_query);
  اگر(نتیجہ)
    جاؤ fail;

  نتیجہ = canon_راستہ(ڈیٹا->state.اوپر.راستہ, لڑی_لمبائی(ڈیٹا->state.اوپر.راستہ),
                      &canonical_راستہ,
                      should_urlencode(service));
  اگر(نتیجہ)
    جاؤ fail;

  *canonical_request_برآمد =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  طریقہ,
                  curlx_dyn_ptr(&canonical_راستہ),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_سرخیاں),
                  curlx_dyn_ptr(signed_سرخیاں),
                  (صحیح)payload_hash_len, payload_hash);
  اگر(!*canonical_request_برآمد) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  نتیجہ = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_راستہ);
  واپس نتیجہ;
}

ساکن CURLcode make_string_مقصد_sign(ساخت Curl_easy *ڈیٹا,
                                    ساخت Curl_str *provider0,
                                    ساخت Curl_str *region,
                                    ساخت Curl_str *service,
                                    ثابت حرف *date,
                                    ثابت حرف *timestamp,
                                    ثابت حرف *canonical_request,
                                    حرف **request_type_برآمد,
                                    حرف **credential_scope_برآمد,
                                    حرف **str_مقصد_sign_برآمد)
{
  حرف *request_type;
  حرف *credential_scope;
  حرف *str_مقصد_sign;
  غیرعلامتی حرف sha_hash[CURL_SHA256_DIGEST_لمبائی];
  حرف sha_hex[SHA256_HEX_لمبائی];

  request_type = curl_maprintf("%.*s4_request",
                               (صحیح)curlx_strlen(provider0),
                               curlx_str(provider0));
  اگر(!request_type)
    واپس CURLE_برآمد_کا_حافظہ;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (صحیح)curlx_strlen(region),
                                   curlx_str(region),
                                   (صحیح)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  اگر(!credential_scope) {
    curlx_free(request_type);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(Curl_sha256it(sha_hash, (ثابت غیرعلامتی حرف *)canonical_request,
                   لڑی_لمبائی(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  sha256_مقصد_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_مقصد_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (صحیح)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  اگر(!str_مقصد_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_مقصد_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(ڈیٹا, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_مقصد_sign);

  *request_type_برآمد = request_type;
  *credential_scope_برآمد = credential_scope;
  *str_مقصد_sign_برآمد = str_مقصد_sign;
  واپس CURLE_OK;
}

ساکن CURLcode sign_and_سیٹ_auth_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_str *provider0,
                                          ساخت Curl_str *region,
                                          ساخت Curl_str *service,
                                          ثابت حرف *request_type,
                                          ثابت حرف *credential_scope,
                                          ثابت حرف *date,
                                          ثابت حرف *str_مقصد_sign,
                                          ثابت حرف *date_سرخی,
                                          ثابت حرف *content_sha256_hdr,
                                          ساخت dynbuf *signed_سرخیاں)
{
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  ثابت حرف *passwd = Curl_creds_passwd(ڈیٹا->state.creds);
  حرف *secret = تہی;
  غیرعلامتی حرف sign0[CURL_SHA256_DIGEST_لمبائی] = { 0 };
  غیرعلامتی حرف sign1[CURL_SHA256_DIGEST_لمبائی] = { 0 };
  حرف sha_hex[SHA256_HEX_لمبائی];
  حرف *auth_سرخیاں = تہی;
  حرف *user = curl_escape(Curl_creds_user(ڈیٹا->state.creds), 0);
  اگر(!user)
    واپس CURLE_برآمد_کا_حافظہ;

  secret = curl_maprintf("%.*s4%s", (صحیح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  اگر(!secret)
    جاؤ fail;
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
              str_مقصد_sign, لڑی_لمبائی(str_مقصد_sign), sign0);

  sha256_مقصد_hex(sha_hex, sign0);

  infof(ڈیٹا, "aws_sigv4: Signature - %s", sha_hex);

  auth_سرخیاں = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحیح)curlx_strlen(provider0),
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
  اگر(!auth_سرخیاں)
    جاؤ fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_سرخیاں[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(ڈیٹا->req.hd_auth);
  ڈیٹا->req.hd_auth = auth_سرخیاں;
  ڈیٹا->state.authhost.مکمل = TRUE;
  نتیجہ = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  واپس نتیجہ;
}

CURLcode Curl_output_aws_sigv4(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  ساخت Curl_str provider0 = { تہی, 0 };
  ساخت Curl_str provider1 = { تہی, 0 };
  ساخت Curl_str region = { تہی, 0 };
  ساخت Curl_str service = { تہی, 0 };
  ثابت حرف *hostname = ڈیٹا->state.origin->hostname;
  حرف timestamp[TIMESTAMP_سائز];
  حرف date[9];
  ساخت dynbuf canonical_سرخیاں;
  ساخت dynbuf signed_سرخیاں;
  حرف *date_سرخی = تہی;
  Curl_HttpReq httpreq;
  ثابت حرف *طریقہ = تہی;
  ثابت حرف *payload_hash = تہی;
  size_t payload_hash_len = 0;
  غیرعلامتی حرف sha_hash[CURL_SHA256_DIGEST_لمبائی];
  حرف sha_hex[SHA256_HEX_لمبائی];
  حرف content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  حرف *canonical_request = تہی;
  حرف *request_type = تہی;
  حرف *credential_scope = تہی;
  حرف *str_مقصد_sign = تہی;

  اگر(ڈیٹا->سیٹ.راستہ_as_is) {
    failf(ڈیٹا, "Cannot use sigv4 authentication with path-as-is flag");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  اگر(Curl_checkheaders(ڈیٹا, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    واپس CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_سرخیاں, CURL_MAX_HTTP_سرخی);
  curlx_dyn_init(&signed_سرخیاں, CURL_MAX_HTTP_سرخی);

  نتیجہ = parse_sigv4_params(ڈیٹا, hostname, &provider0, &provider1,
                              &region, &service);
  اگر(!نتیجہ) {
    Curl_http_طریقہ(ڈیٹا, &طریقہ, &httpreq);
    نتیجہ = get_payload_hash(ڈیٹا, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  اگر(!نتیجہ)
    نتیجہ = get_timestamp(timestamp, حجم(timestamp));

  اگر(!نتیجہ)
    نتیجہ = make_canonical_request(ڈیٹا, timestamp,
                                    &provider1, &service,
                                    طریقہ, payload_hash, payload_hash_len,
                                    &date_سرخی, content_sha256_hdr,
                                    &canonical_سرخیاں, &signed_سرخیاں,
                                    &canonical_request);
  اگر(!نتیجہ) {
    /* the timestamp might have been updated in make_canonical_request */
    حافظہ_نقل(date, timestamp, حجم(date) - 1);
    date[حجم(date) - 1] = 0;

    نتیجہ = make_string_مقصد_sign(ڈیٹا, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_مقصد_sign);
  }
  اگر(!نتیجہ)
    نتیجہ = sign_and_سیٹ_auth_سرخیاں(ڈیٹا, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_مقصد_sign, date_سرخی,
                                       content_sha256_hdr, &signed_سرخیاں);

  curlx_dyn_free(&canonical_سرخیاں);
  curlx_dyn_free(&signed_سرخیاں);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_مقصد_sign);
  curlx_free(date_سرخی);
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
