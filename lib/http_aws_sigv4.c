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

#وصف_ڪريو HMAC_SHA256(k, kl, d, dl, o)                 \
  افعل {                                               \
    نتيجو = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت بنانشان حرف *)(k), \
                         kl,                         \
                         (ثابت بنانشان حرف *)(d), \
                         dl, o);                     \
    إذا(نتيجو) {                                     \
      اذهب fail;                                     \
    }                                                \
  } طالما(0)

#وصف_ڪريو TIMESTAMP_سائيز 17

/* hex-encoded with null-terminator */
#وصف_ڪريو SHA256_HEX_ڊيگهائي ((2 * CURL_SHA256_DIGEST_ڊيگهائي) + 1)

#وصف_ڪريو MAX_QUERY_COMPONENTS 128

بنية pair {
  بنية dynbuf ڪنجي;
  بنية dynbuf قدر;
};

ساكن فراغ sha256_منزل_hex(حرف *dst, بنانشان حرف *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_ڊيگهائي,
                 (بنانشان حرف *)dst, SHA256_HEX_ڊيگهائي);
}

ساكن حرف *ڳوليو_date_hdr(بنية Curl_easy *معلومات, ثابت حرف *sig_hdr)
{
  حرف *tmp = Curl_checkheaders(معلومات, sig_hdr, لڙي_ڊيگهه(sig_hdr));

  إذا(tmp)
    أرجع tmp;
  أرجع Curl_checkheaders(معلومات, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ساكن فراغ trim_سرا(بنية curl_slist *سر)
{
  بنية curl_slist *l;
  لكل(l = سر; l; l = l->ايندڙ) {
    ثابت حرف *قدر; /* to read from */
    حرف *store;
    size_t colon = strcspn(l->معلومات, ":");
    Curl_strntolower(l->معلومات, l->معلومات, colon);

    قدر = &l->معلومات[colon];
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
ساكن فراغ pair_array_free(بنية pair *pair_array, size_t انگ_elements)
{
  size_t اشاريو;

  لكل(اشاريو = 0; اشاريو != انگ_elements; اشاريو++) {
    curlx_dyn_free(&pair_array[اشاريو].ڪنجي);
    curlx_dyn_free(&pair_array[اشاريو].قدر);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ساكن فراغ dyn_array_free(بنية dynbuf *db, size_t انگ_elements)
{
  size_t اشاريو;

  لكل(اشاريو = 0; اشاريو < انگ_elements; اشاريو++)
    curlx_dyn_free((&db[اشاريو]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#وصف_ڪريو ورهايو_BY '&'

ساكن CURLcode ورهايو_منزل_dyn_array(ثابت حرف *ذريعو,
                                   بنية dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *انگ_splits_ٻاهريون)
{
  CURLcode نتيجو = CURLE_OK;
  size_t len = لڙي_ڊيگهه(ذريعو);
  size_t pos;         /* Position in result buffer */
  size_t شروعات = 0;   /* Start of current segment */
  size_t segment_ڊيگهائي = 0;
  size_t اشاريو = 0;
  size_t انگ_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  لكل(pos = 0; pos < len; pos++) {
    إذا(ذريعو[pos] == ورهايو_BY) {
      إذا(segment_ڊيگهائي) {
        curlx_dyn_init(&db[اشاريو], segment_ڊيگهائي + 1);
        نتيجو = curlx_dyn_addn(&db[اشاريو], &ذريعو[شروعات], segment_ڊيگهائي);
        إذا(نتيجو)
          اذهب fail;

        segment_ڊيگهائي = 0;
        اشاريو++;
        إذا(++انگ_splits == MAX_QUERY_COMPONENTS) {
          نتيجو = CURLE_TOO_LARGE;
          اذهب fail;
        }
      }
      شروعات = pos + 1;
    }
    وإلا {
      segment_ڊيگهائي++;
    }
  }

  إذا(segment_ڊيگهائي) {
    curlx_dyn_init(&db[اشاريو], segment_ڊيگهائي + 1);
    نتيجو = curlx_dyn_addn(&db[اشاريو], &ذريعو[شروعات], segment_ڊيگهائي);
    إذا(!نتيجو && ++انگ_splits == MAX_QUERY_COMPONENTS)
      نتيجو = CURLE_TOO_LARGE;
  }
fail:
  *انگ_splits_ٻاهريون = انگ_splits;
  أرجع نتيجو;
}

ساكن bool is_reserved_char(ثابت حرف c)
{
  أرجع (ISALNUM(c) || ISURLPUNTCS(c));
}

ساكن CURLcode uri_encode_رستو(بنية Curl_str *original_رستو,
                                بنية dynbuf *new_رستو)
{
  ثابت حرف *p = curlx_str(original_رستو);
  size_t i;

  لكل(i = 0; i < curlx_strlen(original_رستو); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode نتيجو = CURLE_OK;
    بنانشان حرف c = p[i];
    إذا(is_reserved_char(c) || c == '/')
      نتيجو = curlx_dyn_addn(new_رستو, &c, 1);
    وإلا
      نتيجو = curlx_dyn_addf(new_رستو, "%%%02X", c);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  أرجع CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ساكن CURLcode normalize_query(ثابت حرف *string, size_t len,
                                بنية dynbuf *db)
{
  CURLcode نتيجو = CURLE_OK;

  طالما(len && !نتيجو) {
    بنانشان حرف اندريون = (بنانشان حرف)*string;
    إذا(('%' == اندريون) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      اندريون = (بنانشان حرف)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      إذا(اندريون == '+') {
        /* decodes to plus, so leave this encoded */
        نتيجو = curlx_dyn_addn(db, "%2B", 3);
        استمر;
      }
    }
    وإلا {
      string++;
      len--;
    }

    إذا(is_reserved_char(اندريون))
      /* Escape unreserved chars from RFC 3986 */
      نتيجو = curlx_dyn_addn(db, &اندريون, 1);
    وإلا إذا(اندريون == '+')
      /* Encode '+' as space */
      نتيجو = curlx_dyn_جوڙيو(db, "%20");
    وإلا
      نتيجو = curlx_dyn_addf(db, "%%%02X", اندريون);
  }

  أرجع نتيجو;
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
#وصف_ڪريو MAX_SIGV4_LEN    64
#وصف_ڪريو DATE_HDR_ڪنجي_LEN (MAX_SIGV4_LEN + حجم("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#وصف_ڪريو DATE_ڀريل_HDR_LEN (DATE_HDR_ڪنجي_LEN + TIMESTAMP_سائيز + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ساكن صحيح compare_سرو_نالا(ثابت حرف *a, ثابت حرف *b)
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

  len_a = colon_a ? (size_t)(colon_a - a) : لڙي_ڊيگهه(a);
  len_b = colon_b ? (size_t)(colon_b - b) : لڙي_ڊيگهه(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = لڙي_ڀيٽ(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  إذا(!cmp)
    أرجع (صحيح)(len_a - len_b);

  أرجع cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ساكن CURLcode ملايو_duplicate_سرا(بنية curl_slist *سر)
{
  بنية curl_slist *curr = سر;
  CURLcode نتيجو = CURLE_OK;

  طالما(curr) {
    بنية curl_slist *ايندڙ = curr->ايندڙ;
    إذا(!ايندڙ)
      اكسر;

    إذا(compare_سرو_نالا(curr->معلومات, ايندڙ->معلومات) == 0) {
      بنية dynbuf buf;
      ثابت حرف *colon_ايندڙ;
      ثابت حرف *val_ايندڙ;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_سرو);

      نتيجو = curlx_dyn_جوڙيو(&buf, curr->معلومات);
      إذا(نتيجو)
        أرجع نتيجو;

      colon_ايندڙ = strchr(ايندڙ->معلومات, ':');
      DEBUGASSERT(colon_ايندڙ);
      val_ايندڙ = colon_ايندڙ + 1;

      نتيجو = curlx_dyn_addn(&buf, ",", 1);
      إذا(نتيجو)
        أرجع نتيجو;

      نتيجو = curlx_dyn_جوڙيو(&buf, val_ايندڙ);
      إذا(نتيجو)
        أرجع نتيجو;

      curlx_free(curr->معلومات);
      curr->معلومات = curlx_dyn_ptr(&buf);

      curr->ايندڙ = ايندڙ->ايندڙ;
      curlx_free(ايندڙ->معلومات);
      curlx_free(ايندڙ);
    }
    وإلا {
      curr = curr->ايندڙ;
    }
  }

  أرجع CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ساكن CURLcode make_سرا(بنية Curl_easy *معلومات,
                             حرف *timestamp,
                             ثابت حرف *provider1,
                             size_t plen, /* length of provider1 */
                             حرف **date_سرو,
                             حرف *content_sha256_سرو,
                             بنية dynbuf *canonical_سرا,
                             بنية dynbuf *signed_سرا)
{
  حرف date_hdr_ڪنجي[DATE_HDR_ڪنجي_LEN];
  حرف date_ڀريل_hdr[DATE_ڀريل_HDR_LEN];
  بنية curl_slist *سر = عدم;
  بنية curl_slist *tmp_سر = عدم;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  بنية curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_ڪنجي, DATE_HDR_ڪنجي_LEN, "X-%.*s-Date",
                 (صحيح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_ڪنجي[2], provider1, plen);
  date_hdr_ڪنجي[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_ڀريل_hdr, DATE_ڀريل_HDR_LEN,
                 "x-%.*s-date:%s", (صحيح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_ڀريل_hdr[2], provider1, plen);

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

  إذا(*content_sha256_سرو) {
    tmp_سر = curl_slist_append(سر, content_sha256_سرو);
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
  لكل(l = معلومات->سيٽ.سرا; l; l = l->ايندڙ) {
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

  trim_سرا(سر);

  *date_سرو = ڳوليو_date_hdr(معلومات, date_hdr_ڪنجي);
  إذا(!*date_سرو) {
    tmp_سر = curl_slist_append(سر, date_ڀريل_hdr);
    إذا(!tmp_سر)
      اذهب fail;
    سر = tmp_سر;
    *date_سرو = curl_maprintf("%s: %s\r\n", date_hdr_ڪنجي, timestamp);
    إذا(!*date_سرو)
      اذهب fail;
  }
  وإلا {
    ثابت حرف *قدر;
    ثابت حرف *endp;
    قدر = strchr(*date_سرو, ':');
    إذا(!قدر) {
      *date_سرو = عدم;
      اذهب fail;
    }
    ++قدر;
    curlx_str_passblanks(&قدر);
    endp = قدر;
    طالما(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    إذا((endp - قدر) == TIMESTAMP_سائيز - 1) {
      يادگيري_نقل(timestamp, قدر, TIMESTAMP_سائيز - 1);
      timestamp[TIMESTAMP_سائيز - 1] = 0;
    }
    وإلا
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_سرو = عدم;
  }

  /* alpha-sort by header name in a case sensitive manner */
  افعل {
    again = FALSE;
    لكل(l = سر; l; l = l->ايندڙ) {
      بنية curl_slist *ايندڙ = l->ايندڙ;

      إذا(ايندڙ && compare_سرو_نالا(l->معلومات, ايندڙ->معلومات) > 0) {
        حرف *tmp = l->معلومات;

        l->معلومات = ايندڙ->معلومات;
        ايندڙ->معلومات = tmp;
        again = TRUE;
      }
    }
  } طالما(again);

  نتيجو = ملايو_duplicate_سرا(سر);
  إذا(نتيجو)
    اذهب fail;

  لكل(l = سر; l; l = l->ايندڙ) {
    حرف *tmp;

    إذا(curlx_dyn_جوڙيو(canonical_سرا, l->معلومات))
      اذهب fail;
    إذا(curlx_dyn_جوڙيو(canonical_سرا, "\n"))
      اذهب fail;

    tmp = strchr(l->معلومات, ':');
    إذا(tmp)
      *tmp = 0;

    إذا(l != سر && curlx_dyn_جوڙيو(signed_سرا, ";"))
      اذهب fail;
    إذا(curlx_dyn_جوڙيو(signed_سرا, l->معلومات))
      اذهب fail;
  }

  نتيجو = CURLE_OK;
fail:
  curl_slist_free_all(سر);

  أرجع نتيجو;
}

#وصف_ڪريو CONTENT_SHA256_ڪنجي_LEN (MAX_SIGV4_LEN + حجم("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#وصف_ڪريو CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_ڪنجي_LEN + 2 + SHA256_HEX_ڊيگهائي)

/* try to parse a payload hash from the content-sha256 header */
ساكن ثابت حرف *parse_content_sha_hdr(بنية Curl_easy *معلومات,
                                         ثابت حرف *provider1,
                                         size_t plen,
                                         size_t *قدر_len)
{
  حرف ڪنجي[CONTENT_SHA256_ڪنجي_LEN];
  size_t ڪنجي_len;
  ثابت حرف *قدر;
  size_t len;

  ڪنجي_len = curl_msnprintf(ڪنجي, حجم(ڪنجي), "x-%.*s-content-sha256",
                           (صحيح)plen, provider1);

  قدر = Curl_checkheaders(معلومات, ڪنجي, ڪنجي_len);
  إذا(!قدر)
    أرجع عدم;

  قدر = strchr(قدر, ':');
  إذا(!قدر)
    أرجع عدم;
  ++قدر;

  curlx_str_passblanks(&قدر);

  len = لڙي_ڊيگهه(قدر);
  طالما(len > 0 && ISBLANK(قدر[len - 1]))
    --len;

  *قدر_len = len;
  أرجع قدر;
}

ساكن CURLcode calc_payload_hash(بنية Curl_easy *معلومات,
                                  بنانشان حرف *sha_hash, حرف *sha_hex)
{
  ثابت حرف *post_معلومات = معلومات->سيٽ.postfields;
  size_t post_معلومات_len = 0;
  CURLcode نتيجو;

  إذا(post_معلومات) {
    إذا(معلومات->سيٽ.postfieldsize < 0)
      post_معلومات_len = لڙي_ڊيگهه(post_معلومات);
    وإلا
      post_معلومات_len = (size_t)معلومات->سيٽ.postfieldsize;
  }
  نتيجو = Curl_sha256it(sha_hash, (ثابت بنانشان حرف *)post_معلومات,
                         post_معلومات_len);
  إذا(!نتيجو)
    sha256_منزل_hex(sha_hex, sha_hash);
  أرجع نتيجو;
}

#وصف_ڪريو S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ساكن CURLcode calc_s3_payload_hash(بنية Curl_easy *معلومات,
                                     Curl_HttpReq httpreq,
                                     ثابت حرف *provider1,
                                     size_t plen,
                                     بنانشان حرف *sha_hash,
                                     حرف *sha_hex, حرف *سرو)
{
  bool خالي_جايون_طريقو = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_سر);
  /* The request method or filesize indicate no request payload */
  bool خالي_جايون_payload = (خالي_جايون_طريقو || معلومات->سيٽ.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && معلومات->سيٽ.postfields);
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  إذا(خالي_جايون_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    نتيجو = calc_payload_hash(معلومات, sha_hash, sha_hex);
    إذا(نتيجو)
      اذهب fail;
  }
  وإلا {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_ڊيگهائي); /* 16 < 65 */
    يادگيري_نقل(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(سرو, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحيح)plen, provider1, sha_hex);

  نتيجو = CURLE_OK;
fail:
  أرجع نتيجو;
}

ساكن صحيح compare_func(ثابت فراغ *a, ثابت فراغ *b)
{
  ثابت بنية pair *aa = a;
  ثابت بنية pair *bb = b;
  ثابت size_t aa_ڪنجي_len = curlx_dyn_len(&aa->ڪنجي);
  ثابت size_t bb_ڪنجي_len = curlx_dyn_len(&bb->ڪنجي);
  ثابت size_t aa_قدر_len = curlx_dyn_len(&aa->قدر);
  ثابت size_t bb_قدر_len = curlx_dyn_len(&bb->قدر);
  صحيح compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  إذا((aa_ڪنجي_len == 0) && (bb_ڪنجي_len == 0))
    أرجع 0;
  إذا(aa_ڪنجي_len == 0)
    أرجع -1;
  إذا(bb_ڪنجي_len == 0)
    أرجع 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ڪنجي), curlx_dyn_ptr(&bb->ڪنجي));
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
UNITTEST CURLcode canon_رستو(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_رستو,
                             bool do_uri_encode);
UNITTEST CURLcode canon_رستو(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_رستو,
                             bool do_uri_encode)
{
  CURLcode نتيجو = CURLE_OK;

  بنية Curl_str original_رستو;

  curlx_str_سونپيو(&original_رستو, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  إذا(do_uri_encode)
    نتيجو = uri_encode_رستو(&original_رستو, new_رستو);
  وإلا
    نتيجو = curlx_dyn_addn(new_رستو, q, len);

  إذا(!نتيجو && curlx_dyn_len(new_رستو) == 0)
    نتيجو = curlx_dyn_جوڙيو(new_رستو, "/");

  أرجع نتيجو;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq)
{
  CURLcode نتيجو = CURLE_OK;

  بنية dynbuf query_array[MAX_QUERY_COMPONENTS];
  بنية pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t انگ_query_components;
  size_t counted_query_components = 0;
  size_t اشاريو;

  إذا(!query)
    أرجع نتيجو;

  نتيجو = ورهايو_منزل_dyn_array(query, &query_array[0], &انگ_query_components);
  إذا(نتيجو) {
    اذهب fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  لكل(اشاريو = 0; اشاريو < انگ_query_components; اشاريو++) {
    ثابت حرف *اندريون_ڪنجي;
    size_t اندريون_ڪنجي_len;
    ثابت حرف *وٿي;
    size_t query_part_len = curlx_dyn_len(&query_array[اشاريو]);
    ثابت حرف *query_part = curlx_dyn_ptr(&query_array[اشاريو]);

    اندريون_ڪنجي = query_part;

    وٿي = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    إذا(!وٿي) {
      اندريون_ڪنجي_len = لڙي_ڊيگهه(اندريون_ڪنجي);
    }
    وإلا {
      اندريون_ڪنجي_len = وٿي - اندريون_ڪنجي;
    }

    curlx_dyn_init(&encoded_query_array[اشاريو].ڪنجي,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[اشاريو].قدر,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    نتيجو = normalize_query(اندريون_ڪنجي, اندريون_ڪنجي_len,
                             &encoded_query_array[اشاريو].ڪنجي);
    إذا(نتيجو) {
      اذهب fail;
    }

    /* Decode/encode the value if it exists */
    إذا(وٿي && وٿي != (query_part + query_part_len - 1)) {
      size_t اندريون_قدر_len;
      ثابت حرف *اندريون_قدر = وٿي + 1;
      اندريون_قدر_len = query_part + query_part_len - (وٿي + 1);
      نتيجو = normalize_query(اندريون_قدر, اندريون_قدر_len,
                               &encoded_query_array[اشاريو].قدر);
      إذا(نتيجو) {
        اذهب fail;
      }
    }
    وإلا {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[اشاريو].قدر, 2);
      نتيجو = curlx_dyn_addn(&encoded_query_array[اشاريو].قدر, "", 1);
    }

    إذا(نتيجو) {
      اذهب fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, انگ_query_components,
        حجم(بنية pair), compare_func);

  /* Append the query components together to make a full query string */
  لكل(اشاريو = 0; اشاريو < انگ_query_components; اشاريو++) {

    إذا(اشاريو)
      نتيجو = curlx_dyn_addn(dq, "&", 1);
    إذا(!نتيجو) {
      ثابت حرف *ڪنجي_ptr = curlx_dyn_ptr(&encoded_query_array[اشاريو].ڪنجي);
      ثابت حرف *قدر_ptr = curlx_dyn_ptr(&encoded_query_array[اشاريو].قدر);
      size_t vlen = curlx_dyn_len(&encoded_query_array[اشاريو].قدر);
      إذا(قدر_ptr && vlen) {
        نتيجو = curlx_dyn_addf(dq, "%s=%s", ڪنجي_ptr, قدر_ptr);
      }
      وإلا {
        /* Empty value is always encoded to key= */
        نتيجو = curlx_dyn_addf(dq, "%s=", ڪنجي_ptr);
      }
    }
    إذا(نتيجو)
      اكسر;
  }

fail:
  إذا(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], انگ_query_components);
  أرجع نتيجو;
}

ساكن CURLcode parse_sigv4_params(بنية Curl_easy *معلومات,
                                   ثابت حرف *hostname,
                                   بنية Curl_str *provider0,
                                   بنية Curl_str *provider1,
                                   بنية Curl_str *region,
                                   بنية Curl_str *service)
{
  ثابت حرف *سٽ = CURL_EASY_STR(معلومات, STRING_AWS_SIGV4);
  إذا(!سٽ || !*سٽ)
    سٽ = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  إذا(curlx_str_until(&سٽ, provider0, MAX_SIGV4_LEN, ':')) {
    failf(معلومات, "first aws-sigv4 provider cannot be empty");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
  إذا(curlx_str_single(&سٽ, ':') ||
     curlx_str_until(&سٽ, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  وإلا إذا(curlx_str_single(&سٽ, ':') ||
          curlx_str_until(&سٽ, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&سٽ, ':') ||
          curlx_str_until(&سٽ, service, MAX_SIGV4_LEN, ':')) {
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
                                 بنانشان حرف *sha_hash,
                                 حرف *sha_hex,
                                 حرف *content_sha256_hdr,
                                 ثابت حرف **payload_hash_ٻاهريون,
                                 size_t *payload_hash_len_ٻاهريون)
{
  *payload_hash_ٻاهريون =
    parse_content_sha_hdr(معلومات, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_ٻاهريون);

  إذا(!*payload_hash_ٻاهريون) {
    CURLcode نتيجو;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    إذا(sign_as_s3)
      نتيجو = calc_s3_payload_hash(معلومات, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    وإلا
      نتيجو = calc_payload_hash(معلومات, sha_hash, sha_hex);
    إذا(نتيجو)
      أرجع نتيجو;

    *payload_hash_ٻاهريون = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_ٻاهريون = لڙي_ڊيگهه(sha_hex);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode get_timestamp(حرف *timestamp, size_t stampsize)
{
  time_t clock;
  بنية tm tm;
  CURLcode نتيجو;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  {
    حرف *force_timestamp = getenv("CURL_FORCETIME");
    إذا(force_timestamp)
      clock = 0;
    وإلا
      clock = وقت(عدم);
  }
#وإلا
  clock = وقت(عدم);
#جيڪڏهن_پورو
  نتيجو = curlx_gmtime(clock, &tm);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  أرجع CURLE_OK;
}

ساكن CURLcode make_canonical_request(بنية Curl_easy *معلومات,
                                       حرف *timestamp,
                                       بنية Curl_str *provider1,
                                       بنية Curl_str *service,
                                       ثابت حرف *طريقو,
                                       ثابت حرف *payload_hash,
                                       size_t payload_hash_len,
                                       حرف **date_سرو_ٻاهريون,
                                       حرف *content_sha256_hdr,
                                       بنية dynbuf *canonical_سرا,
                                       بنية dynbuf *signed_سرا,
                                       حرف **canonical_request_ٻاهريون)
{
  بنية dynbuf canonical_query;
  بنية dynbuf canonical_رستو;
  CURLcode نتيجو;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_سرو);
  curlx_dyn_init(&canonical_رستو, CURL_MAX_HTTP_سرو);

  نتيجو = make_سرا(معلومات, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_سرو_ٻاهريون, content_sha256_hdr,
                        canonical_سرا, signed_سرا);
  إذا(نتيجو)
    اذهب fail;

  نتيجو = canon_query(معلومات->state.مٿي.query, &canonical_query);
  إذا(نتيجو)
    اذهب fail;

  نتيجو = canon_رستو(معلومات->state.مٿي.رستو, لڙي_ڊيگهه(معلومات->state.مٿي.رستو),
                      &canonical_رستو,
                      should_urlencode(service));
  إذا(نتيجو)
    اذهب fail;

  *canonical_request_ٻاهريون =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  طريقو,
                  curlx_dyn_ptr(&canonical_رستو),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_سرا),
                  curlx_dyn_ptr(signed_سرا),
                  (صحيح)payload_hash_len, payload_hash);
  إذا(!*canonical_request_ٻاهريون) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب fail;
  }

  نتيجو = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_رستو);
  أرجع نتيجو;
}

ساكن CURLcode make_string_منزل_sign(بنية Curl_easy *معلومات,
                                    بنية Curl_str *provider0,
                                    بنية Curl_str *region,
                                    بنية Curl_str *service,
                                    ثابت حرف *date,
                                    ثابت حرف *timestamp,
                                    ثابت حرف *canonical_request,
                                    حرف **request_type_ٻاهريون,
                                    حرف **credential_scope_ٻاهريون,
                                    حرف **str_منزل_sign_ٻاهريون)
{
  حرف *request_type;
  حرف *credential_scope;
  حرف *str_منزل_sign;
  بنانشان حرف sha_hash[CURL_SHA256_DIGEST_ڊيگهائي];
  حرف sha_hex[SHA256_HEX_ڊيگهائي];

  request_type = curl_maprintf("%.*s4_request",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0));
  إذا(!request_type)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

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
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  إذا(Curl_sha256it(sha_hash, (ثابت بنانشان حرف *)canonical_request,
                   لڙي_ڊيگهه(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
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
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_منزل_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(معلومات, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_منزل_sign);

  *request_type_ٻاهريون = request_type;
  *credential_scope_ٻاهريون = credential_scope;
  *str_منزل_sign_ٻاهريون = str_منزل_sign;
  أرجع CURLE_OK;
}

ساكن CURLcode sign_and_سيٽ_auth_سرا(بنية Curl_easy *معلومات,
                                          بنية Curl_str *provider0,
                                          بنية Curl_str *region,
                                          بنية Curl_str *service,
                                          ثابت حرف *request_type,
                                          ثابت حرف *credential_scope,
                                          ثابت حرف *date,
                                          ثابت حرف *str_منزل_sign,
                                          ثابت حرف *date_سرو,
                                          ثابت حرف *content_sha256_hdr,
                                          بنية dynbuf *signed_سرا)
{
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  ثابت حرف *passwd = Curl_creds_passwd(معلومات->state.creds);
  حرف *secret = عدم;
  بنانشان حرف sign0[CURL_SHA256_DIGEST_ڊيگهائي] = { 0 };
  بنانشان حرف sign1[CURL_SHA256_DIGEST_ڊيگهائي] = { 0 };
  حرف sha_hex[SHA256_HEX_ڊيگهائي];
  حرف *auth_سرا = عدم;
  حرف *user = curl_escape(Curl_creds_user(معلومات->state.creds), 0);
  إذا(!user)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  secret = curl_maprintf("%.*s4%s", (صحيح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  إذا(!secret)
    اذهب fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, لڙي_ڊيگهه(secret), date, لڙي_ڊيگهه(date), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              request_type, لڙي_ڊيگهه(request_type), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              str_منزل_sign, لڙي_ڊيگهه(str_منزل_sign), sign0);

  sha256_منزل_hex(sha_hex, sign0);

  infof(معلومات, "aws_sigv4: Signature - %s", sha_hex);

  auth_سرا = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_سرا),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_سرو ? date_سرو : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  إذا(!auth_سرا)
    اذهب fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_سرا[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(معلومات->req.hd_auth);
  معلومات->req.hd_auth = auth_سرا;
  معلومات->state.authhost.پورو = TRUE;
  نتيجو = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  أرجع نتيجو;
}

CURLcode Curl_output_aws_sigv4(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  بنية Curl_str provider0 = { عدم, 0 };
  بنية Curl_str provider1 = { عدم, 0 };
  بنية Curl_str region = { عدم, 0 };
  بنية Curl_str service = { عدم, 0 };
  ثابت حرف *hostname = معلومات->state.origin->hostname;
  حرف timestamp[TIMESTAMP_سائيز];
  حرف date[9];
  بنية dynbuf canonical_سرا;
  بنية dynbuf signed_سرا;
  حرف *date_سرو = عدم;
  Curl_HttpReq httpreq;
  ثابت حرف *طريقو = عدم;
  ثابت حرف *payload_hash = عدم;
  size_t payload_hash_len = 0;
  بنانشان حرف sha_hash[CURL_SHA256_DIGEST_ڊيگهائي];
  حرف sha_hex[SHA256_HEX_ڊيگهائي];
  حرف content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  حرف *canonical_request = عدم;
  حرف *request_type = عدم;
  حرف *credential_scope = عدم;
  حرف *str_منزل_sign = عدم;

  إذا(معلومات->سيٽ.رستو_as_is) {
    failf(معلومات, "Cannot use sigv4 authentication with path-as-is flag");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  إذا(Curl_checkheaders(معلومات, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    أرجع CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_سرا, CURL_MAX_HTTP_سرو);
  curlx_dyn_init(&signed_سرا, CURL_MAX_HTTP_سرو);

  نتيجو = parse_sigv4_params(معلومات, hostname, &provider0, &provider1,
                              &region, &service);
  إذا(!نتيجو) {
    Curl_http_طريقو(معلومات, &طريقو, &httpreq);
    نتيجو = get_payload_hash(معلومات, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  إذا(!نتيجو)
    نتيجو = get_timestamp(timestamp, حجم(timestamp));

  إذا(!نتيجو)
    نتيجو = make_canonical_request(معلومات, timestamp,
                                    &provider1, &service,
                                    طريقو, payload_hash, payload_hash_len,
                                    &date_سرو, content_sha256_hdr,
                                    &canonical_سرا, &signed_سرا,
                                    &canonical_request);
  إذا(!نتيجو) {
    /* the timestamp might have been updated in make_canonical_request */
    يادگيري_نقل(date, timestamp, حجم(date) - 1);
    date[حجم(date) - 1] = 0;

    نتيجو = make_string_منزل_sign(معلومات, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_منزل_sign);
  }
  إذا(!نتيجو)
    نتيجو = sign_and_سيٽ_auth_سرا(معلومات, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_منزل_sign, date_سرو,
                                       content_sha256_hdr, &signed_سرا);

  curlx_dyn_free(&canonical_سرا);
  curlx_dyn_free(&signed_سرا);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_منزل_sign);
  curlx_free(date_سرو);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
