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

#تعریف_کړه HMAC_SHA256(k, kl, d, dl, o)                 \
  افعل {                                               \
    پایله = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت بینښه حرف *)(k), \
                         kl,                         \
                         (ثابت بینښه حرف *)(d), \
                         dl, o);                     \
    إذا(پایله) {                                     \
      اذهب fail;                                     \
    }                                                \
  } طالما(0)

#تعریف_کړه TIMESTAMP_اندازه 17

/* hex-encoded with null-terminator */
#تعریف_کړه SHA256_HEX_اوږدتیا ((2 * CURL_SHA256_DIGEST_اوږدتیا) + 1)

#تعریف_کړه MAX_QUERY_COMPONENTS 128

بنية pair {
  بنية dynbuf کلی;
  بنية dynbuf ارزښت;
};

ساكن فراغ sha256_منزل_hex(حرف *dst, بینښه حرف *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_اوږدتیا,
                 (بینښه حرف *)dst, SHA256_HEX_اوږدتیا);
}

ساكن حرف *پیدا_کړه_date_hdr(بنية Curl_easy *معلومات, ثابت حرف *sig_hdr)
{
  حرف *tmp = Curl_checkheaders(معلومات, sig_hdr, مزي_اوږدوالی(sig_hdr));

  إذا(tmp)
    أرجع tmp;
  أرجع Curl_checkheaders(معلومات, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ساكن فراغ trim_سرلیکونه(بنية curl_slist *سر)
{
  بنية curl_slist *l;
  لكل(l = سر; l; l = l->بل) {
    ثابت حرف *ارزښت; /* to read from */
    حرف *store;
    size_t colon = strcspn(l->معلومات, ":");
    Curl_strntolower(l->معلومات, l->معلومات, colon);

    ارزښت = &l->معلومات[colon];
    إذا(!*ارزښت)
      استمر;
    ++ارزښت;
    store = (حرف *)CURL_UNCONST(ارزښت);

    /* skip leading whitespace */
    curlx_str_passblanks(&ارزښت);

    طالما(*ارزښت) {
      صحيح space = 0;
      طالما(ISBLANK(*ارزښت)) {
        ارزښت++;
        space++;
      }
      إذا(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        إذا(*ارزښت)
          *store++ = ' ';
      }
      وإلا
        *store++ = *ارزښت++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ساكن فراغ pair_array_free(بنية pair *pair_array, size_t عدد_elements)
{
  size_t شاخص;

  لكل(شاخص = 0; شاخص != عدد_elements; شاخص++) {
    curlx_dyn_free(&pair_array[شاخص].کلی);
    curlx_dyn_free(&pair_array[شاخص].ارزښت);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ساكن فراغ dyn_array_free(بنية dynbuf *db, size_t عدد_elements)
{
  size_t شاخص;

  لكل(شاخص = 0; شاخص < عدد_elements; شاخص++)
    curlx_dyn_free((&db[شاخص]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#تعریف_کړه وېشل_BY '&'

ساكن CURLcode وېشل_منزل_dyn_array(ثابت حرف *سرچینه,
                                   بنية dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *عدد_splits_بهر)
{
  CURLcode پایله = CURLE_OK;
  size_t len = مزي_اوږدوالی(سرچینه);
  size_t pos;         /* Position in result buffer */
  size_t پیل = 0;   /* Start of current segment */
  size_t segment_اوږدتیا = 0;
  size_t شاخص = 0;
  size_t عدد_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  لكل(pos = 0; pos < len; pos++) {
    إذا(سرچینه[pos] == وېشل_BY) {
      إذا(segment_اوږدتیا) {
        curlx_dyn_init(&db[شاخص], segment_اوږدتیا + 1);
        پایله = curlx_dyn_addn(&db[شاخص], &سرچینه[پیل], segment_اوږدتیا);
        إذا(پایله)
          اذهب fail;

        segment_اوږدتیا = 0;
        شاخص++;
        إذا(++عدد_splits == MAX_QUERY_COMPONENTS) {
          پایله = CURLE_TOO_LARGE;
          اذهب fail;
        }
      }
      پیل = pos + 1;
    }
    وإلا {
      segment_اوږدتیا++;
    }
  }

  إذا(segment_اوږدتیا) {
    curlx_dyn_init(&db[شاخص], segment_اوږدتیا + 1);
    پایله = curlx_dyn_addn(&db[شاخص], &سرچینه[پیل], segment_اوږدتیا);
    إذا(!پایله && ++عدد_splits == MAX_QUERY_COMPONENTS)
      پایله = CURLE_TOO_LARGE;
  }
fail:
  *عدد_splits_بهر = عدد_splits;
  أرجع پایله;
}

ساكن bool is_reserved_char(ثابت حرف c)
{
  أرجع (ISALNUM(c) || ISURLPUNTCS(c));
}

ساكن CURLcode uri_encode_لار(بنية Curl_str *original_لار,
                                بنية dynbuf *new_لار)
{
  ثابت حرف *p = curlx_str(original_لار);
  size_t i;

  لكل(i = 0; i < curlx_strlen(original_لار); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode پایله = CURLE_OK;
    بینښه حرف c = p[i];
    إذا(is_reserved_char(c) || c == '/')
      پایله = curlx_dyn_addn(new_لار, &c, 1);
    وإلا
      پایله = curlx_dyn_addf(new_لار, "%%%02X", c);
    إذا(پایله)
      أرجع پایله;
  }

  أرجع CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ساكن CURLcode normalize_query(ثابت حرف *string, size_t len,
                                بنية dynbuf *db)
{
  CURLcode پایله = CURLE_OK;

  طالما(len && !پایله) {
    بینښه حرف دننه = (بینښه حرف)*string;
    إذا(('%' == دننه) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      دننه = (بینښه حرف)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      إذا(دننه == '+') {
        /* decodes to plus, so leave this encoded */
        پایله = curlx_dyn_addn(db, "%2B", 3);
        استمر;
      }
    }
    وإلا {
      string++;
      len--;
    }

    إذا(is_reserved_char(دننه))
      /* Escape unreserved chars from RFC 3986 */
      پایله = curlx_dyn_addn(db, &دننه, 1);
    وإلا إذا(دننه == '+')
      /* Encode '+' as space */
      پایله = curlx_dyn_جمع_کړه(db, "%20");
    وإلا
      پایله = curlx_dyn_addf(db, "%%%02X", دننه);
  }

  أرجع پایله;
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
#تعریف_کړه MAX_SIGV4_LEN    64
#تعریف_کړه DATE_HDR_کلی_LEN (MAX_SIGV4_LEN + حجم("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#تعریف_کړه DATE_ډک_HDR_LEN (DATE_HDR_کلی_LEN + TIMESTAMP_اندازه + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ساكن صحيح compare_سرلیک_نومونه(ثابت حرف *a, ثابت حرف *b)
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

  len_a = colon_a ? (size_t)(colon_a - a) : مزي_اوږدوالی(a);
  len_b = colon_b ? (size_t)(colon_b - b) : مزي_اوږدوالی(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = مزي_پرتله(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  إذا(!cmp)
    أرجع (صحيح)(len_a - len_b);

  أرجع cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ساكن CURLcode یوځای_duplicate_سرلیکونه(بنية curl_slist *سر)
{
  بنية curl_slist *curr = سر;
  CURLcode پایله = CURLE_OK;

  طالما(curr) {
    بنية curl_slist *بل = curr->بل;
    إذا(!بل)
      اكسر;

    إذا(compare_سرلیک_نومونه(curr->معلومات, بل->معلومات) == 0) {
      بنية dynbuf buf;
      ثابت حرف *colon_بل;
      ثابت حرف *val_بل;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_سرلیک);

      پایله = curlx_dyn_جمع_کړه(&buf, curr->معلومات);
      إذا(پایله)
        أرجع پایله;

      colon_بل = strchr(بل->معلومات, ':');
      DEBUGASSERT(colon_بل);
      val_بل = colon_بل + 1;

      پایله = curlx_dyn_addn(&buf, ",", 1);
      إذا(پایله)
        أرجع پایله;

      پایله = curlx_dyn_جمع_کړه(&buf, val_بل);
      إذا(پایله)
        أرجع پایله;

      curlx_free(curr->معلومات);
      curr->معلومات = curlx_dyn_ptr(&buf);

      curr->بل = بل->بل;
      curlx_free(بل->معلومات);
      curlx_free(بل);
    }
    وإلا {
      curr = curr->بل;
    }
  }

  أرجع CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ساكن CURLcode make_سرلیکونه(بنية Curl_easy *معلومات,
                             حرف *timestamp,
                             ثابت حرف *provider1,
                             size_t plen, /* length of provider1 */
                             حرف **date_سرلیک,
                             حرف *content_sha256_سرلیک,
                             بنية dynbuf *canonical_سرلیکونه,
                             بنية dynbuf *signed_سرلیکونه)
{
  حرف date_hdr_کلی[DATE_HDR_کلی_LEN];
  حرف date_ډک_hdr[DATE_ډک_HDR_LEN];
  بنية curl_slist *سر = عدم;
  بنية curl_slist *tmp_سر = عدم;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  بنية curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_کلی, DATE_HDR_کلی_LEN, "X-%.*s-Date",
                 (صحيح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_کلی[2], provider1, plen);
  date_hdr_کلی[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_ډک_hdr, DATE_ډک_HDR_LEN,
                 "x-%.*s-date:%s", (صحيح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_ډک_hdr[2], provider1, plen);

  إذا(!Curl_checkheaders(معلومات, STRCONST("Host")) &&
     معلومات->state.http_host) {
    /* Host: [host]:[port] */
    حرف *fullhost = curlx_strdup(معلومات->state.http_host);

    إذا(fullhost)
      سر = Curl_slist_append_nodup(عدم, fullhost);
    إذا(!سر) {
      curlx_free(fullhost);
      اذهب fail;
    }
  }

  إذا(*content_sha256_سرلیک) {
    tmp_سر = curl_slist_append(سر, content_sha256_سرلیک);
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
  لكل(l = معلومات->سېټ.سرلیکونه; l; l = l->بل) {
    حرف *dupdata;
    ثابت حرف *ptr;
    ثابت حرف *sep = strchr(l->معلومات, ':');
    إذا(!sep)
      sep = strchr(l->معلومات, ';');
    إذا(!sep || (*sep == ':' && !*(sep + 1)))
      استمر;
    لكل(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    إذا(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      استمر;
    dupdata = curlx_strdup(l->معلومات);
    إذا(!dupdata)
      اذهب fail;
    dupdata[sep - l->معلومات] = ':';
    tmp_سر = Curl_slist_append_nodup(سر, dupdata);
    إذا(!tmp_سر) {
      curlx_free(dupdata);
      اذهب fail;
    }
    سر = tmp_سر;
  }

  trim_سرلیکونه(سر);

  *date_سرلیک = پیدا_کړه_date_hdr(معلومات, date_hdr_کلی);
  إذا(!*date_سرلیک) {
    tmp_سر = curl_slist_append(سر, date_ډک_hdr);
    إذا(!tmp_سر)
      اذهب fail;
    سر = tmp_سر;
    *date_سرلیک = curl_maprintf("%s: %s\r\n", date_hdr_کلی, timestamp);
    إذا(!*date_سرلیک)
      اذهب fail;
  }
  وإلا {
    ثابت حرف *ارزښت;
    ثابت حرف *endp;
    ارزښت = strchr(*date_سرلیک, ':');
    إذا(!ارزښت) {
      *date_سرلیک = عدم;
      اذهب fail;
    }
    ++ارزښت;
    curlx_str_passblanks(&ارزښت);
    endp = ارزښت;
    طالما(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    إذا((endp - ارزښت) == TIMESTAMP_اندازه - 1) {
      حافظه_نقل(timestamp, ارزښت, TIMESTAMP_اندازه - 1);
      timestamp[TIMESTAMP_اندازه - 1] = 0;
    }
    وإلا
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_سرلیک = عدم;
  }

  /* alpha-sort by header name in a case sensitive manner */
  افعل {
    again = FALSE;
    لكل(l = سر; l; l = l->بل) {
      بنية curl_slist *بل = l->بل;

      إذا(بل && compare_سرلیک_نومونه(l->معلومات, بل->معلومات) > 0) {
        حرف *tmp = l->معلومات;

        l->معلومات = بل->معلومات;
        بل->معلومات = tmp;
        again = TRUE;
      }
    }
  } طالما(again);

  پایله = یوځای_duplicate_سرلیکونه(سر);
  إذا(پایله)
    اذهب fail;

  لكل(l = سر; l; l = l->بل) {
    حرف *tmp;

    إذا(curlx_dyn_جمع_کړه(canonical_سرلیکونه, l->معلومات))
      اذهب fail;
    إذا(curlx_dyn_جمع_کړه(canonical_سرلیکونه, "\n"))
      اذهب fail;

    tmp = strchr(l->معلومات, ':');
    إذا(tmp)
      *tmp = 0;

    إذا(l != سر && curlx_dyn_جمع_کړه(signed_سرلیکونه, ";"))
      اذهب fail;
    إذا(curlx_dyn_جمع_کړه(signed_سرلیکونه, l->معلومات))
      اذهب fail;
  }

  پایله = CURLE_OK;
fail:
  curl_slist_free_all(سر);

  أرجع پایله;
}

#تعریف_کړه CONTENT_SHA256_کلی_LEN (MAX_SIGV4_LEN + حجم("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#تعریف_کړه CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_کلی_LEN + 2 + SHA256_HEX_اوږدتیا)

/* try to parse a payload hash from the content-sha256 header */
ساكن ثابت حرف *parse_content_sha_hdr(بنية Curl_easy *معلومات,
                                         ثابت حرف *provider1,
                                         size_t plen,
                                         size_t *ارزښت_len)
{
  حرف کلی[CONTENT_SHA256_کلی_LEN];
  size_t کلی_len;
  ثابت حرف *ارزښت;
  size_t len;

  کلی_len = curl_msnprintf(کلی, حجم(کلی), "x-%.*s-content-sha256",
                           (صحيح)plen, provider1);

  ارزښت = Curl_checkheaders(معلومات, کلی, کلی_len);
  إذا(!ارزښت)
    أرجع عدم;

  ارزښت = strchr(ارزښت, ':');
  إذا(!ارزښت)
    أرجع عدم;
  ++ارزښت;

  curlx_str_passblanks(&ارزښت);

  len = مزي_اوږدوالی(ارزښت);
  طالما(len > 0 && ISBLANK(ارزښت[len - 1]))
    --len;

  *ارزښت_len = len;
  أرجع ارزښت;
}

ساكن CURLcode calc_payload_hash(بنية Curl_easy *معلومات,
                                  بینښه حرف *sha_hash, حرف *sha_hex)
{
  ثابت حرف *post_معلومات = معلومات->سېټ.postfields;
  size_t post_معلومات_len = 0;
  CURLcode پایله;

  إذا(post_معلومات) {
    إذا(معلومات->سېټ.postfieldsize < 0)
      post_معلومات_len = مزي_اوږدوالی(post_معلومات);
    وإلا
      post_معلومات_len = (size_t)معلومات->سېټ.postfieldsize;
  }
  پایله = Curl_sha256it(sha_hash, (ثابت بینښه حرف *)post_معلومات,
                         post_معلومات_len);
  إذا(!پایله)
    sha256_منزل_hex(sha_hex, sha_hash);
  أرجع پایله;
}

#تعریف_کړه S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ساكن CURLcode calc_s3_payload_hash(بنية Curl_easy *معلومات,
                                     Curl_HttpReq httpreq,
                                     ثابت حرف *provider1,
                                     size_t plen,
                                     بینښه حرف *sha_hash,
                                     حرف *sha_hex, حرف *سرلیک)
{
  bool تشه_لاره = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_سر);
  /* The request method or filesize indicate no request payload */
  bool تشه_payload = (تشه_لاره || معلومات->سېټ.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && معلومات->سېټ.postfields);
  CURLcode پایله = CURLE_بهر_د_حافظه;

  إذا(تشه_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    پایله = calc_payload_hash(معلومات, sha_hash, sha_hex);
    إذا(پایله)
      اذهب fail;
  }
  وإلا {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_اوږدتیا); /* 16 < 65 */
    حافظه_نقل(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(سرلیک, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحيح)plen, provider1, sha_hex);

  پایله = CURLE_OK;
fail:
  أرجع پایله;
}

ساكن صحيح compare_func(ثابت فراغ *a, ثابت فراغ *b)
{
  ثابت بنية pair *aa = a;
  ثابت بنية pair *bb = b;
  ثابت size_t aa_کلی_len = curlx_dyn_len(&aa->کلی);
  ثابت size_t bb_کلی_len = curlx_dyn_len(&bb->کلی);
  ثابت size_t aa_ارزښت_len = curlx_dyn_len(&aa->ارزښت);
  ثابت size_t bb_ارزښت_len = curlx_dyn_len(&bb->ارزښت);
  صحيح compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  إذا((aa_کلی_len == 0) && (bb_کلی_len == 0))
    أرجع 0;
  إذا(aa_کلی_len == 0)
    أرجع -1;
  إذا(bb_کلی_len == 0)
    أرجع 1;
  compare = strcmp(curlx_dyn_ptr(&aa->کلی), curlx_dyn_ptr(&bb->کلی));
  إذا(compare) {
    أرجع compare;
  }

  /* Compare values */
  إذا((aa_ارزښت_len == 0) && (bb_ارزښت_len == 0))
    أرجع 0;
  إذا(aa_ارزښت_len == 0)
    أرجع -1;
  إذا(bb_ارزښت_len == 0)
    أرجع 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ارزښت), curlx_dyn_ptr(&bb->ارزښت));

  أرجع compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_لار(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_لار,
                             bool do_uri_encode);
UNITTEST CURLcode canon_لار(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_لار,
                             bool do_uri_encode)
{
  CURLcode پایله = CURLE_OK;

  بنية Curl_str original_لار;

  curlx_str_ټاکل(&original_لار, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  إذا(do_uri_encode)
    پایله = uri_encode_لار(&original_لار, new_لار);
  وإلا
    پایله = curlx_dyn_addn(new_لار, q, len);

  إذا(!پایله && curlx_dyn_len(new_لار) == 0)
    پایله = curlx_dyn_جمع_کړه(new_لار, "/");

  أرجع پایله;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq)
{
  CURLcode پایله = CURLE_OK;

  بنية dynbuf query_array[MAX_QUERY_COMPONENTS];
  بنية pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t عدد_query_components;
  size_t counted_query_components = 0;
  size_t شاخص;

  إذا(!query)
    أرجع پایله;

  پایله = وېشل_منزل_dyn_array(query, &query_array[0], &عدد_query_components);
  إذا(پایله) {
    اذهب fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  لكل(شاخص = 0; شاخص < عدد_query_components; شاخص++) {
    ثابت حرف *دننه_کلی;
    size_t دننه_کلی_len;
    ثابت حرف *دننه_واټن;
    size_t query_part_len = curlx_dyn_len(&query_array[شاخص]);
    ثابت حرف *query_part = curlx_dyn_ptr(&query_array[شاخص]);

    دننه_کلی = query_part;

    دننه_واټن = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    إذا(!دننه_واټن) {
      دننه_کلی_len = مزي_اوږدوالی(دننه_کلی);
    }
    وإلا {
      دننه_کلی_len = دننه_واټن - دننه_کلی;
    }

    curlx_dyn_init(&encoded_query_array[شاخص].کلی,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[شاخص].ارزښت,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    پایله = normalize_query(دننه_کلی, دننه_کلی_len,
                             &encoded_query_array[شاخص].کلی);
    إذا(پایله) {
      اذهب fail;
    }

    /* Decode/encode the value if it exists */
    إذا(دننه_واټن && دننه_واټن != (query_part + query_part_len - 1)) {
      size_t دننه_ارزښت_len;
      ثابت حرف *دننه_ارزښت = دننه_واټن + 1;
      دننه_ارزښت_len = query_part + query_part_len - (دننه_واټن + 1);
      پایله = normalize_query(دننه_ارزښت, دننه_ارزښت_len,
                               &encoded_query_array[شاخص].ارزښت);
      إذا(پایله) {
        اذهب fail;
      }
    }
    وإلا {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[شاخص].ارزښت, 2);
      پایله = curlx_dyn_addn(&encoded_query_array[شاخص].ارزښت, "", 1);
    }

    إذا(پایله) {
      اذهب fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, عدد_query_components,
        حجم(بنية pair), compare_func);

  /* Append the query components together to make a full query string */
  لكل(شاخص = 0; شاخص < عدد_query_components; شاخص++) {

    إذا(شاخص)
      پایله = curlx_dyn_addn(dq, "&", 1);
    إذا(!پایله) {
      ثابت حرف *کلی_ptr = curlx_dyn_ptr(&encoded_query_array[شاخص].کلی);
      ثابت حرف *ارزښت_ptr = curlx_dyn_ptr(&encoded_query_array[شاخص].ارزښت);
      size_t vlen = curlx_dyn_len(&encoded_query_array[شاخص].ارزښت);
      إذا(ارزښت_ptr && vlen) {
        پایله = curlx_dyn_addf(dq, "%s=%s", کلی_ptr, ارزښت_ptr);
      }
      وإلا {
        /* Empty value is always encoded to key= */
        پایله = curlx_dyn_addf(dq, "%s=", کلی_ptr);
      }
    }
    إذا(پایله)
      اكسر;
  }

fail:
  إذا(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], عدد_query_components);
  أرجع پایله;
}

ساكن CURLcode parse_sigv4_params(بنية Curl_easy *معلومات,
                                   ثابت حرف *hostname,
                                   بنية Curl_str *provider0,
                                   بنية Curl_str *provider1,
                                   بنية Curl_str *region,
                                   بنية Curl_str *service)
{
  ثابت حرف *کرښه = CURL_EASY_STR(معلومات, STRING_AWS_SIGV4);
  إذا(!کرښه || !*کرښه)
    کرښه = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  إذا(curlx_str_until(&کرښه, provider0, MAX_SIGV4_LEN, ':')) {
    failf(معلومات, "first aws-sigv4 provider cannot be empty");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  إذا(curlx_str_single(&کرښه, ':') ||
     curlx_str_until(&کرښه, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  وإلا إذا(curlx_str_single(&کرښه, ':') ||
          curlx_str_until(&کرښه, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&کرښه, ':') ||
          curlx_str_until(&کرښه, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  إذا(!curlx_strlen(service)) {
    ثابت حرف *p = hostname;
    إذا(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(معلومات, "aws-sigv4: service missing in parameters and hostname");
      أرجع CURLE_URL_MALFORMAT;
    }

    infof(معلومات, "aws_sigv4: picked service %.*s from host",
          (صحيح)curlx_strlen(service), curlx_str(service));

    إذا(!curlx_strlen(region)) {
      إذا(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(معلومات, "aws-sigv4: region missing in parameters and hostname");
        أرجع CURLE_URL_MALFORMAT;
      }
      infof(معلومات, "aws_sigv4: picked region %.*s from host",
            (صحيح)curlx_strlen(region), curlx_str(region));
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode get_payload_hash(بنية Curl_easy *معلومات,
                                 Curl_HttpReq httpreq,
                                 بنية Curl_str *provider0,
                                 بنية Curl_str *provider1,
                                 بنية Curl_str *service,
                                 بینښه حرف *sha_hash,
                                 حرف *sha_hex,
                                 حرف *content_sha256_hdr,
                                 ثابت حرف **payload_hash_بهر,
                                 size_t *payload_hash_len_بهر)
{
  *payload_hash_بهر =
    parse_content_sha_hdr(معلومات, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_بهر);

  إذا(!*payload_hash_بهر) {
    CURLcode پایله;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    إذا(sign_as_s3)
      پایله = calc_s3_payload_hash(معلومات, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    وإلا
      پایله = calc_payload_hash(معلومات, sha_hash, sha_hex);
    إذا(پایله)
      أرجع پایله;

    *payload_hash_بهر = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_بهر = مزي_اوږدوالی(sha_hex);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode get_timestamp(حرف *timestamp, size_t stampsize)
{
  time_t clock;
  بنية tm tm;
  CURLcode پایله;

#که_تعریف_شوی DEBUGBUILD
  {
    حرف *force_timestamp = getenv("CURL_FORCETIME");
    إذا(force_timestamp)
      clock = 0;
    وإلا
      clock = وخت(عدم);
  }
#وإلا
  clock = وخت(عدم);
#د_که_پای
  پایله = curlx_gmtime(clock, &tm);
  إذا(پایله)
    أرجع پایله;

  إذا(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    أرجع CURLE_بهر_د_حافظه;

  أرجع CURLE_OK;
}

ساكن CURLcode make_canonical_request(بنية Curl_easy *معلومات,
                                       حرف *timestamp,
                                       بنية Curl_str *provider1,
                                       بنية Curl_str *service,
                                       ثابت حرف *لاره,
                                       ثابت حرف *payload_hash,
                                       size_t payload_hash_len,
                                       حرف **date_سرلیک_بهر,
                                       حرف *content_sha256_hdr,
                                       بنية dynbuf *canonical_سرلیکونه,
                                       بنية dynbuf *signed_سرلیکونه,
                                       حرف **canonical_request_بهر)
{
  بنية dynbuf canonical_query;
  بنية dynbuf canonical_لار;
  CURLcode پایله;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_سرلیک);
  curlx_dyn_init(&canonical_لار, CURL_MAX_HTTP_سرلیک);

  پایله = make_سرلیکونه(معلومات, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_سرلیک_بهر, content_sha256_hdr,
                        canonical_سرلیکونه, signed_سرلیکونه);
  إذا(پایله)
    اذهب fail;

  پایله = canon_query(معلومات->state.پورته.query, &canonical_query);
  إذا(پایله)
    اذهب fail;

  پایله = canon_لار(معلومات->state.پورته.لار, مزي_اوږدوالی(معلومات->state.پورته.لار),
                      &canonical_لار,
                      should_urlencode(service));
  إذا(پایله)
    اذهب fail;

  *canonical_request_بهر =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  لاره,
                  curlx_dyn_ptr(&canonical_لار),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_سرلیکونه),
                  curlx_dyn_ptr(signed_سرلیکونه),
                  (صحيح)payload_hash_len, payload_hash);
  إذا(!*canonical_request_بهر) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب fail;
  }

  پایله = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_لار);
  أرجع پایله;
}

ساكن CURLcode make_string_منزل_sign(بنية Curl_easy *معلومات,
                                    بنية Curl_str *provider0,
                                    بنية Curl_str *region,
                                    بنية Curl_str *service,
                                    ثابت حرف *date,
                                    ثابت حرف *timestamp,
                                    ثابت حرف *canonical_request,
                                    حرف **request_type_بهر,
                                    حرف **credential_scope_بهر,
                                    حرف **str_منزل_sign_بهر)
{
  حرف *request_type;
  حرف *credential_scope;
  حرف *str_منزل_sign;
  بینښه حرف sha_hash[CURL_SHA256_DIGEST_اوږدتیا];
  حرف sha_hex[SHA256_HEX_اوږدتیا];

  request_type = curl_maprintf("%.*s4_request",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0));
  إذا(!request_type)
    أرجع CURLE_بهر_د_حافظه;

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
    أرجع CURLE_بهر_د_حافظه;
  }

  إذا(Curl_sha256it(sha_hash, (ثابت بینښه حرف *)canonical_request,
                   مزي_اوږدوالی(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_بهر_د_حافظه;
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
    أرجع CURLE_بهر_د_حافظه;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_منزل_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(معلومات, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_منزل_sign);

  *request_type_بهر = request_type;
  *credential_scope_بهر = credential_scope;
  *str_منزل_sign_بهر = str_منزل_sign;
  أرجع CURLE_OK;
}

ساكن CURLcode sign_and_سېټ_auth_سرلیکونه(بنية Curl_easy *معلومات,
                                          بنية Curl_str *provider0,
                                          بنية Curl_str *region,
                                          بنية Curl_str *service,
                                          ثابت حرف *request_type,
                                          ثابت حرف *credential_scope,
                                          ثابت حرف *date,
                                          ثابت حرف *str_منزل_sign,
                                          ثابت حرف *date_سرلیک,
                                          ثابت حرف *content_sha256_hdr,
                                          بنية dynbuf *signed_سرلیکونه)
{
  CURLcode پایله = CURLE_بهر_د_حافظه;
  ثابت حرف *passwd = Curl_creds_passwd(معلومات->state.creds);
  حرف *secret = عدم;
  بینښه حرف sign0[CURL_SHA256_DIGEST_اوږدتیا] = { 0 };
  بینښه حرف sign1[CURL_SHA256_DIGEST_اوږدتیا] = { 0 };
  حرف sha_hex[SHA256_HEX_اوږدتیا];
  حرف *auth_سرلیکونه = عدم;
  حرف *user = curl_escape(Curl_creds_user(معلومات->state.creds), 0);
  إذا(!user)
    أرجع CURLE_بهر_د_حافظه;

  secret = curl_maprintf("%.*s4%s", (صحيح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  إذا(!secret)
    اذهب fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, مزي_اوږدوالی(secret), date, مزي_اوږدوالی(date), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              request_type, مزي_اوږدوالی(request_type), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              str_منزل_sign, مزي_اوږدوالی(str_منزل_sign), sign0);

  sha256_منزل_hex(sha_hex, sign0);

  infof(معلومات, "aws_sigv4: Signature - %s", sha_hex);

  auth_سرلیکونه = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_سرلیکونه),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_سرلیک ? date_سرلیک : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  إذا(!auth_سرلیکونه)
    اذهب fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_سرلیکونه[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(معلومات->req.hd_auth);
  معلومات->req.hd_auth = auth_سرلیکونه;
  معلومات->state.authhost.بشپړ = TRUE;
  پایله = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  أرجع پایله;
}

CURLcode Curl_output_aws_sigv4(بنية Curl_easy *معلومات)
{
  CURLcode پایله = CURLE_بهر_د_حافظه;
  بنية Curl_str provider0 = { عدم, 0 };
  بنية Curl_str provider1 = { عدم, 0 };
  بنية Curl_str region = { عدم, 0 };
  بنية Curl_str service = { عدم, 0 };
  ثابت حرف *hostname = معلومات->state.origin->hostname;
  حرف timestamp[TIMESTAMP_اندازه];
  حرف date[9];
  بنية dynbuf canonical_سرلیکونه;
  بنية dynbuf signed_سرلیکونه;
  حرف *date_سرلیک = عدم;
  Curl_HttpReq httpreq;
  ثابت حرف *لاره = عدم;
  ثابت حرف *payload_hash = عدم;
  size_t payload_hash_len = 0;
  بینښه حرف sha_hash[CURL_SHA256_DIGEST_اوږدتیا];
  حرف sha_hex[SHA256_HEX_اوږدتیا];
  حرف content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  حرف *canonical_request = عدم;
  حرف *request_type = عدم;
  حرف *credential_scope = عدم;
  حرف *str_منزل_sign = عدم;

  إذا(معلومات->سېټ.لار_as_is) {
    failf(معلومات, "Cannot use sigv4 authentication with path-as-is flag");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  إذا(Curl_checkheaders(معلومات, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    أرجع CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_سرلیکونه, CURL_MAX_HTTP_سرلیک);
  curlx_dyn_init(&signed_سرلیکونه, CURL_MAX_HTTP_سرلیک);

  پایله = parse_sigv4_params(معلومات, hostname, &provider0, &provider1,
                              &region, &service);
  إذا(!پایله) {
    Curl_http_لاره(معلومات, &لاره, &httpreq);
    پایله = get_payload_hash(معلومات, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  إذا(!پایله)
    پایله = get_timestamp(timestamp, حجم(timestamp));

  إذا(!پایله)
    پایله = make_canonical_request(معلومات, timestamp,
                                    &provider1, &service,
                                    لاره, payload_hash, payload_hash_len,
                                    &date_سرلیک, content_sha256_hdr,
                                    &canonical_سرلیکونه, &signed_سرلیکونه,
                                    &canonical_request);
  إذا(!پایله) {
    /* the timestamp might have been updated in make_canonical_request */
    حافظه_نقل(date, timestamp, حجم(date) - 1);
    date[حجم(date) - 1] = 0;

    پایله = make_string_منزل_sign(معلومات, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_منزل_sign);
  }
  إذا(!پایله)
    پایله = sign_and_سېټ_auth_سرلیکونه(معلومات, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_منزل_sign, date_سرلیک,
                                       content_sha256_hdr, &signed_سرلیکونه);

  curlx_dyn_free(&canonical_سرلیکونه);
  curlx_dyn_free(&signed_سرلیکونه);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_منزل_sign);
  curlx_free(date_سرلیک);
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
