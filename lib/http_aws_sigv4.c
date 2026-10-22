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

#تعریف_کن HMAC_SHA256(k, kl, d, dl, o)                 \
  افعل {                                               \
    نتیجه = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت بیعلامت حرف *)(k), \
                         kl,                         \
                         (ثابت بیعلامت حرف *)(d), \
                         dl, o);                     \
    إذا(نتیجه) {                                     \
      اذهب fail;                                     \
    }                                                \
  } طالما(0)

#تعریف_کن TIMESTAMP_اندازه 17

/* hex-encoded with null-terminator */
#تعریف_کن SHA256_HEX_درازی ((2 * CURL_SHA256_DIGEST_درازی) + 1)

#تعریف_کن MAX_QUERY_COMPONENTS 128

بنية pair {
  بنية dynbuf کلید;
  بنية dynbuf مقدار;
};

ساكن فراغ sha256_مقصد_hex(حرف *dst, بیعلامت حرف *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_درازی,
                 (بیعلامت حرف *)dst, SHA256_HEX_درازی);
}

ساكن حرف *پیدا_کن_date_hdr(بنية Curl_easy *داده, ثابت حرف *sig_hdr)
{
  حرف *tmp = Curl_checkheaders(داده, sig_hdr, طول_رشته(sig_hdr));

  إذا(tmp)
    أرجع tmp;
  أرجع Curl_checkheaders(داده, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ساكن فراغ trim_سرآیندها(بنية curl_slist *سر)
{
  بنية curl_slist *l;
  لكل(l = سر; l; l = l->بعدی) {
    ثابت حرف *مقدار; /* to read from */
    حرف *store;
    size_t colon = strcspn(l->داده, ":");
    Curl_strntolower(l->داده, l->داده, colon);

    مقدار = &l->داده[colon];
    إذا(!*مقدار)
      استمر;
    ++مقدار;
    store = (حرف *)CURL_UNCONST(مقدار);

    /* skip leading whitespace */
    curlx_str_passblanks(&مقدار);

    طالما(*مقدار) {
      صحيح space = 0;
      طالما(ISBLANK(*مقدار)) {
        مقدار++;
        space++;
      }
      إذا(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        إذا(*مقدار)
          *store++ = ' ';
      }
      وإلا
        *store++ = *مقدار++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ساكن فراغ pair_array_free(بنية pair *pair_array, size_t رقم_elements)
{
  size_t شاخص;

  لكل(شاخص = 0; شاخص != رقم_elements; شاخص++) {
    curlx_dyn_free(&pair_array[شاخص].کلید);
    curlx_dyn_free(&pair_array[شاخص].مقدار);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ساكن فراغ dyn_array_free(بنية dynbuf *db, size_t رقم_elements)
{
  size_t شاخص;

  لكل(شاخص = 0; شاخص < رقم_elements; شاخص++)
    curlx_dyn_free((&db[شاخص]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#تعریف_کن تقسیم_BY '&'

ساكن CURLcode تقسیم_مقصد_dyn_array(ثابت حرف *منبع,
                                   بنية dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *رقم_splits_بیرون)
{
  CURLcode نتیجه = CURLE_OK;
  size_t len = طول_رشته(منبع);
  size_t pos;         /* Position in result buffer */
  size_t شروع = 0;   /* Start of current segment */
  size_t segment_درازی = 0;
  size_t شاخص = 0;
  size_t رقم_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  لكل(pos = 0; pos < len; pos++) {
    إذا(منبع[pos] == تقسیم_BY) {
      إذا(segment_درازی) {
        curlx_dyn_init(&db[شاخص], segment_درازی + 1);
        نتیجه = curlx_dyn_addn(&db[شاخص], &منبع[شروع], segment_درازی);
        إذا(نتیجه)
          اذهب fail;

        segment_درازی = 0;
        شاخص++;
        إذا(++رقم_splits == MAX_QUERY_COMPONENTS) {
          نتیجه = CURLE_TOO_LARGE;
          اذهب fail;
        }
      }
      شروع = pos + 1;
    }
    وإلا {
      segment_درازی++;
    }
  }

  إذا(segment_درازی) {
    curlx_dyn_init(&db[شاخص], segment_درازی + 1);
    نتیجه = curlx_dyn_addn(&db[شاخص], &منبع[شروع], segment_درازی);
    إذا(!نتیجه && ++رقم_splits == MAX_QUERY_COMPONENTS)
      نتیجه = CURLE_TOO_LARGE;
  }
fail:
  *رقم_splits_بیرون = رقم_splits;
  أرجع نتیجه;
}

ساكن bool is_reserved_char(ثابت حرف c)
{
  أرجع (ISALNUM(c) || ISURLPUNTCS(c));
}

ساكن CURLcode uri_encode_مسیر(بنية Curl_str *original_مسیر,
                                بنية dynbuf *new_مسیر)
{
  ثابت حرف *p = curlx_str(original_مسیر);
  size_t i;

  لكل(i = 0; i < curlx_strlen(original_مسیر); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode نتیجه = CURLE_OK;
    بیعلامت حرف c = p[i];
    إذا(is_reserved_char(c) || c == '/')
      نتیجه = curlx_dyn_addn(new_مسیر, &c, 1);
    وإلا
      نتیجه = curlx_dyn_addf(new_مسیر, "%%%02X", c);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  أرجع CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ساكن CURLcode normalize_query(ثابت حرف *string, size_t len,
                                بنية dynbuf *db)
{
  CURLcode نتیجه = CURLE_OK;

  طالما(len && !نتیجه) {
    بیعلامت حرف درون = (بیعلامت حرف)*string;
    إذا(('%' == درون) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      درون = (بیعلامت حرف)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      إذا(درون == '+') {
        /* decodes to plus, so leave this encoded */
        نتیجه = curlx_dyn_addn(db, "%2B", 3);
        استمر;
      }
    }
    وإلا {
      string++;
      len--;
    }

    إذا(is_reserved_char(درون))
      /* Escape unreserved chars from RFC 3986 */
      نتیجه = curlx_dyn_addn(db, &درون, 1);
    وإلا إذا(درون == '+')
      /* Encode '+' as space */
      نتیجه = curlx_dyn_جمع_کن(db, "%20");
    وإلا
      نتیجه = curlx_dyn_addf(db, "%%%02X", درون);
  }

  أرجع نتیجه;
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
#تعریف_کن MAX_SIGV4_LEN    64
#تعریف_کن DATE_HDR_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#تعریف_کن DATE_پر_HDR_LEN (DATE_HDR_کلید_LEN + TIMESTAMP_اندازه + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ساكن صحيح compare_سرآیند_نامها(ثابت حرف *a, ثابت حرف *b)
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

  len_a = colon_a ? (size_t)(colon_a - a) : طول_رشته(a);
  len_b = colon_b ? (size_t)(colon_b - b) : طول_رشته(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = مقایسه_رشته(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  إذا(!cmp)
    أرجع (صحيح)(len_a - len_b);

  أرجع cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ساكن CURLcode ادغام_duplicate_سرآیندها(بنية curl_slist *سر)
{
  بنية curl_slist *curr = سر;
  CURLcode نتیجه = CURLE_OK;

  طالما(curr) {
    بنية curl_slist *بعدی = curr->بعدی;
    إذا(!بعدی)
      اكسر;

    إذا(compare_سرآیند_نامها(curr->داده, بعدی->داده) == 0) {
      بنية dynbuf buf;
      ثابت حرف *colon_بعدی;
      ثابت حرف *val_بعدی;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_سرآیند);

      نتیجه = curlx_dyn_جمع_کن(&buf, curr->داده);
      إذا(نتیجه)
        أرجع نتیجه;

      colon_بعدی = strchr(بعدی->داده, ':');
      DEBUGASSERT(colon_بعدی);
      val_بعدی = colon_بعدی + 1;

      نتیجه = curlx_dyn_addn(&buf, ",", 1);
      إذا(نتیجه)
        أرجع نتیجه;

      نتیجه = curlx_dyn_جمع_کن(&buf, val_بعدی);
      إذا(نتیجه)
        أرجع نتیجه;

      curlx_free(curr->داده);
      curr->داده = curlx_dyn_ptr(&buf);

      curr->بعدی = بعدی->بعدی;
      curlx_free(بعدی->داده);
      curlx_free(بعدی);
    }
    وإلا {
      curr = curr->بعدی;
    }
  }

  أرجع CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ساكن CURLcode make_سرآیندها(بنية Curl_easy *داده,
                             حرف *timestamp,
                             ثابت حرف *provider1,
                             size_t plen, /* length of provider1 */
                             حرف **date_سرآیند,
                             حرف *content_sha256_سرآیند,
                             بنية dynbuf *canonical_سرآیندها,
                             بنية dynbuf *signed_سرآیندها)
{
  حرف date_hdr_کلید[DATE_HDR_کلید_LEN];
  حرف date_پر_hdr[DATE_پر_HDR_LEN];
  بنية curl_slist *سر = عدم;
  بنية curl_slist *tmp_سر = عدم;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  بنية curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_کلید, DATE_HDR_کلید_LEN, "X-%.*s-Date",
                 (صحيح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_کلید[2], provider1, plen);
  date_hdr_کلید[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_پر_hdr, DATE_پر_HDR_LEN,
                 "x-%.*s-date:%s", (صحيح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_پر_hdr[2], provider1, plen);

  إذا(!Curl_checkheaders(داده, STRCONST("Host")) &&
     داده->state.http_host) {
    /* Host: [host]:[port] */
    حرف *fullhost = curlx_strdup(داده->state.http_host);

    إذا(fullhost)
      سر = Curl_slist_append_nodup(عدم, fullhost);
    إذا(!سر) {
      curlx_free(fullhost);
      اذهب fail;
    }
  }

  إذا(*content_sha256_سرآیند) {
    tmp_سر = curl_slist_append(سر, content_sha256_سرآیند);
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
  لكل(l = داده->مجموعه.سرآیندها; l; l = l->بعدی) {
    حرف *dupdata;
    ثابت حرف *ptr;
    ثابت حرف *sep = strchr(l->داده, ':');
    إذا(!sep)
      sep = strchr(l->داده, ';');
    إذا(!sep || (*sep == ':' && !*(sep + 1)))
      استمر;
    لكل(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    إذا(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      استمر;
    dupdata = curlx_strdup(l->داده);
    إذا(!dupdata)
      اذهب fail;
    dupdata[sep - l->داده] = ':';
    tmp_سر = Curl_slist_append_nodup(سر, dupdata);
    إذا(!tmp_سر) {
      curlx_free(dupdata);
      اذهب fail;
    }
    سر = tmp_سر;
  }

  trim_سرآیندها(سر);

  *date_سرآیند = پیدا_کن_date_hdr(داده, date_hdr_کلید);
  إذا(!*date_سرآیند) {
    tmp_سر = curl_slist_append(سر, date_پر_hdr);
    إذا(!tmp_سر)
      اذهب fail;
    سر = tmp_سر;
    *date_سرآیند = curl_maprintf("%s: %s\r\n", date_hdr_کلید, timestamp);
    إذا(!*date_سرآیند)
      اذهب fail;
  }
  وإلا {
    ثابت حرف *مقدار;
    ثابت حرف *endp;
    مقدار = strchr(*date_سرآیند, ':');
    إذا(!مقدار) {
      *date_سرآیند = عدم;
      اذهب fail;
    }
    ++مقدار;
    curlx_str_passblanks(&مقدار);
    endp = مقدار;
    طالما(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    إذا((endp - مقدار) == TIMESTAMP_اندازه - 1) {
      کپی_حافظه(timestamp, مقدار, TIMESTAMP_اندازه - 1);
      timestamp[TIMESTAMP_اندازه - 1] = 0;
    }
    وإلا
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_سرآیند = عدم;
  }

  /* alpha-sort by header name in a case sensitive manner */
  افعل {
    again = FALSE;
    لكل(l = سر; l; l = l->بعدی) {
      بنية curl_slist *بعدی = l->بعدی;

      إذا(بعدی && compare_سرآیند_نامها(l->داده, بعدی->داده) > 0) {
        حرف *tmp = l->داده;

        l->داده = بعدی->داده;
        بعدی->داده = tmp;
        again = TRUE;
      }
    }
  } طالما(again);

  نتیجه = ادغام_duplicate_سرآیندها(سر);
  إذا(نتیجه)
    اذهب fail;

  لكل(l = سر; l; l = l->بعدی) {
    حرف *tmp;

    إذا(curlx_dyn_جمع_کن(canonical_سرآیندها, l->داده))
      اذهب fail;
    إذا(curlx_dyn_جمع_کن(canonical_سرآیندها, "\n"))
      اذهب fail;

    tmp = strchr(l->داده, ':');
    إذا(tmp)
      *tmp = 0;

    إذا(l != سر && curlx_dyn_جمع_کن(signed_سرآیندها, ";"))
      اذهب fail;
    إذا(curlx_dyn_جمع_کن(signed_سرآیندها, l->داده))
      اذهب fail;
  }

  نتیجه = CURLE_OK;
fail:
  curl_slist_free_all(سر);

  أرجع نتیجه;
}

#تعریف_کن CONTENT_SHA256_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#تعریف_کن CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_کلید_LEN + 2 + SHA256_HEX_درازی)

/* try to parse a payload hash from the content-sha256 header */
ساكن ثابت حرف *parse_content_sha_hdr(بنية Curl_easy *داده,
                                         ثابت حرف *provider1,
                                         size_t plen,
                                         size_t *مقدار_len)
{
  حرف کلید[CONTENT_SHA256_کلید_LEN];
  size_t کلید_len;
  ثابت حرف *مقدار;
  size_t len;

  کلید_len = curl_msnprintf(کلید, حجم(کلید), "x-%.*s-content-sha256",
                           (صحيح)plen, provider1);

  مقدار = Curl_checkheaders(داده, کلید, کلید_len);
  إذا(!مقدار)
    أرجع عدم;

  مقدار = strchr(مقدار, ':');
  إذا(!مقدار)
    أرجع عدم;
  ++مقدار;

  curlx_str_passblanks(&مقدار);

  len = طول_رشته(مقدار);
  طالما(len > 0 && ISBLANK(مقدار[len - 1]))
    --len;

  *مقدار_len = len;
  أرجع مقدار;
}

ساكن CURLcode calc_payload_hash(بنية Curl_easy *داده,
                                  بیعلامت حرف *sha_hash, حرف *sha_hex)
{
  ثابت حرف *post_داده = داده->مجموعه.postfields;
  size_t post_داده_len = 0;
  CURLcode نتیجه;

  إذا(post_داده) {
    إذا(داده->مجموعه.postfieldsize < 0)
      post_داده_len = طول_رشته(post_داده);
    وإلا
      post_داده_len = (size_t)داده->مجموعه.postfieldsize;
  }
  نتیجه = Curl_sha256it(sha_hash, (ثابت بیعلامت حرف *)post_داده,
                         post_داده_len);
  إذا(!نتیجه)
    sha256_مقصد_hex(sha_hex, sha_hash);
  أرجع نتیجه;
}

#تعریف_کن S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ساكن CURLcode calc_s3_payload_hash(بنية Curl_easy *داده,
                                     Curl_HttpReq httpreq,
                                     ثابت حرف *provider1,
                                     size_t plen,
                                     بیعلامت حرف *sha_hash,
                                     حرف *sha_hex, حرف *سرآیند)
{
  bool خالی_روش = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_سر);
  /* The request method or filesize indicate no request payload */
  bool خالی_payload = (خالی_روش || داده->مجموعه.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && داده->مجموعه.postfields);
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  إذا(خالی_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    نتیجه = calc_payload_hash(داده, sha_hash, sha_hex);
    إذا(نتیجه)
      اذهب fail;
  }
  وإلا {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_درازی); /* 16 < 65 */
    کپی_حافظه(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(سرآیند, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحيح)plen, provider1, sha_hex);

  نتیجه = CURLE_OK;
fail:
  أرجع نتیجه;
}

ساكن صحيح compare_func(ثابت فراغ *a, ثابت فراغ *b)
{
  ثابت بنية pair *aa = a;
  ثابت بنية pair *bb = b;
  ثابت size_t aa_کلید_len = curlx_dyn_len(&aa->کلید);
  ثابت size_t bb_کلید_len = curlx_dyn_len(&bb->کلید);
  ثابت size_t aa_مقدار_len = curlx_dyn_len(&aa->مقدار);
  ثابت size_t bb_مقدار_len = curlx_dyn_len(&bb->مقدار);
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
  إذا((aa_مقدار_len == 0) && (bb_مقدار_len == 0))
    أرجع 0;
  إذا(aa_مقدار_len == 0)
    أرجع -1;
  إذا(bb_مقدار_len == 0)
    أرجع 1;
  compare = strcmp(curlx_dyn_ptr(&aa->مقدار), curlx_dyn_ptr(&bb->مقدار));

  أرجع compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_مسیر(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_مسیر,
                             bool do_uri_encode);
UNITTEST CURLcode canon_مسیر(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_مسیر,
                             bool do_uri_encode)
{
  CURLcode نتیجه = CURLE_OK;

  بنية Curl_str original_مسیر;

  curlx_str_تخصیص_بده(&original_مسیر, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  إذا(do_uri_encode)
    نتیجه = uri_encode_مسیر(&original_مسیر, new_مسیر);
  وإلا
    نتیجه = curlx_dyn_addn(new_مسیر, q, len);

  إذا(!نتیجه && curlx_dyn_len(new_مسیر) == 0)
    نتیجه = curlx_dyn_جمع_کن(new_مسیر, "/");

  أرجع نتیجه;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq)
{
  CURLcode نتیجه = CURLE_OK;

  بنية dynbuf query_array[MAX_QUERY_COMPONENTS];
  بنية pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t رقم_query_components;
  size_t counted_query_components = 0;
  size_t شاخص;

  إذا(!query)
    أرجع نتیجه;

  نتیجه = تقسیم_مقصد_dyn_array(query, &query_array[0], &رقم_query_components);
  إذا(نتیجه) {
    اذهب fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  لكل(شاخص = 0; شاخص < رقم_query_components; شاخص++) {
    ثابت حرف *درون_کلید;
    size_t درون_کلید_len;
    ثابت حرف *جابه_جایی;
    size_t query_part_len = curlx_dyn_len(&query_array[شاخص]);
    ثابت حرف *query_part = curlx_dyn_ptr(&query_array[شاخص]);

    درون_کلید = query_part;

    جابه_جایی = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    إذا(!جابه_جایی) {
      درون_کلید_len = طول_رشته(درون_کلید);
    }
    وإلا {
      درون_کلید_len = جابه_جایی - درون_کلید;
    }

    curlx_dyn_init(&encoded_query_array[شاخص].کلید,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[شاخص].مقدار,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    نتیجه = normalize_query(درون_کلید, درون_کلید_len,
                             &encoded_query_array[شاخص].کلید);
    إذا(نتیجه) {
      اذهب fail;
    }

    /* Decode/encode the value if it exists */
    إذا(جابه_جایی && جابه_جایی != (query_part + query_part_len - 1)) {
      size_t درون_مقدار_len;
      ثابت حرف *درون_مقدار = جابه_جایی + 1;
      درون_مقدار_len = query_part + query_part_len - (جابه_جایی + 1);
      نتیجه = normalize_query(درون_مقدار, درون_مقدار_len,
                               &encoded_query_array[شاخص].مقدار);
      إذا(نتیجه) {
        اذهب fail;
      }
    }
    وإلا {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[شاخص].مقدار, 2);
      نتیجه = curlx_dyn_addn(&encoded_query_array[شاخص].مقدار, "", 1);
    }

    إذا(نتیجه) {
      اذهب fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, رقم_query_components,
        حجم(بنية pair), compare_func);

  /* Append the query components together to make a full query string */
  لكل(شاخص = 0; شاخص < رقم_query_components; شاخص++) {

    إذا(شاخص)
      نتیجه = curlx_dyn_addn(dq, "&", 1);
    إذا(!نتیجه) {
      ثابت حرف *کلید_ptr = curlx_dyn_ptr(&encoded_query_array[شاخص].کلید);
      ثابت حرف *مقدار_ptr = curlx_dyn_ptr(&encoded_query_array[شاخص].مقدار);
      size_t vlen = curlx_dyn_len(&encoded_query_array[شاخص].مقدار);
      إذا(مقدار_ptr && vlen) {
        نتیجه = curlx_dyn_addf(dq, "%s=%s", کلید_ptr, مقدار_ptr);
      }
      وإلا {
        /* Empty value is always encoded to key= */
        نتیجه = curlx_dyn_addf(dq, "%s=", کلید_ptr);
      }
    }
    إذا(نتیجه)
      اكسر;
  }

fail:
  إذا(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], رقم_query_components);
  أرجع نتیجه;
}

ساكن CURLcode parse_sigv4_params(بنية Curl_easy *داده,
                                   ثابت حرف *hostname,
                                   بنية Curl_str *provider0,
                                   بنية Curl_str *provider1,
                                   بنية Curl_str *region,
                                   بنية Curl_str *service)
{
  ثابت حرف *سطر = CURL_EASY_STR(داده, STRING_AWS_SIGV4);
  إذا(!سطر || !*سطر)
    سطر = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  إذا(curlx_str_until(&سطر, provider0, MAX_SIGV4_LEN, ':')) {
    failf(داده, "first aws-sigv4 provider cannot be empty");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
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
      failf(داده, "aws-sigv4: service missing in parameters and hostname");
      أرجع CURLE_URL_MALFORMAT;
    }

    infof(داده, "aws_sigv4: picked service %.*s from host",
          (صحيح)curlx_strlen(service), curlx_str(service));

    إذا(!curlx_strlen(region)) {
      إذا(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(داده, "aws-sigv4: region missing in parameters and hostname");
        أرجع CURLE_URL_MALFORMAT;
      }
      infof(داده, "aws_sigv4: picked region %.*s from host",
            (صحيح)curlx_strlen(region), curlx_str(region));
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode get_payload_hash(بنية Curl_easy *داده,
                                 Curl_HttpReq httpreq,
                                 بنية Curl_str *provider0,
                                 بنية Curl_str *provider1,
                                 بنية Curl_str *service,
                                 بیعلامت حرف *sha_hash,
                                 حرف *sha_hex,
                                 حرف *content_sha256_hdr,
                                 ثابت حرف **payload_hash_بیرون,
                                 size_t *payload_hash_len_بیرون)
{
  *payload_hash_بیرون =
    parse_content_sha_hdr(داده, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_بیرون);

  إذا(!*payload_hash_بیرون) {
    CURLcode نتیجه;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    إذا(sign_as_s3)
      نتیجه = calc_s3_payload_hash(داده, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    وإلا
      نتیجه = calc_payload_hash(داده, sha_hash, sha_hex);
    إذا(نتیجه)
      أرجع نتیجه;

    *payload_hash_بیرون = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_بیرون = طول_رشته(sha_hex);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode get_timestamp(حرف *timestamp, size_t stampsize)
{
  time_t clock;
  بنية tm tm;
  CURLcode نتیجه;

#اگر_تعریف_شده DEBUGBUILD
  {
    حرف *force_timestamp = getenv("CURL_FORCETIME");
    إذا(force_timestamp)
      clock = 0;
    وإلا
      clock = زمان(عدم);
  }
#وإلا
  clock = زمان(عدم);
#پایان_اگر
  نتیجه = curlx_gmtime(clock, &tm);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

ساكن CURLcode make_canonical_request(بنية Curl_easy *داده,
                                       حرف *timestamp,
                                       بنية Curl_str *provider1,
                                       بنية Curl_str *service,
                                       ثابت حرف *روش,
                                       ثابت حرف *payload_hash,
                                       size_t payload_hash_len,
                                       حرف **date_سرآیند_بیرون,
                                       حرف *content_sha256_hdr,
                                       بنية dynbuf *canonical_سرآیندها,
                                       بنية dynbuf *signed_سرآیندها,
                                       حرف **canonical_request_بیرون)
{
  بنية dynbuf canonical_query;
  بنية dynbuf canonical_مسیر;
  CURLcode نتیجه;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_سرآیند);
  curlx_dyn_init(&canonical_مسیر, CURL_MAX_HTTP_سرآیند);

  نتیجه = make_سرآیندها(داده, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_سرآیند_بیرون, content_sha256_hdr,
                        canonical_سرآیندها, signed_سرآیندها);
  إذا(نتیجه)
    اذهب fail;

  نتیجه = canon_query(داده->state.به_بالا.query, &canonical_query);
  إذا(نتیجه)
    اذهب fail;

  نتیجه = canon_مسیر(داده->state.به_بالا.مسیر, طول_رشته(داده->state.به_بالا.مسیر),
                      &canonical_مسیر,
                      should_urlencode(service));
  إذا(نتیجه)
    اذهب fail;

  *canonical_request_بیرون =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  روش,
                  curlx_dyn_ptr(&canonical_مسیر),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_سرآیندها),
                  curlx_dyn_ptr(signed_سرآیندها),
                  (صحيح)payload_hash_len, payload_hash);
  إذا(!*canonical_request_بیرون) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

  نتیجه = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_مسیر);
  أرجع نتیجه;
}

ساكن CURLcode make_string_مقصد_sign(بنية Curl_easy *داده,
                                    بنية Curl_str *provider0,
                                    بنية Curl_str *region,
                                    بنية Curl_str *service,
                                    ثابت حرف *date,
                                    ثابت حرف *timestamp,
                                    ثابت حرف *canonical_request,
                                    حرف **request_type_بیرون,
                                    حرف **credential_scope_بیرون,
                                    حرف **str_مقصد_sign_بیرون)
{
  حرف *request_type;
  حرف *credential_scope;
  حرف *str_مقصد_sign;
  بیعلامت حرف sha_hash[CURL_SHA256_DIGEST_درازی];
  حرف sha_hex[SHA256_HEX_درازی];

  request_type = curl_maprintf("%.*s4_request",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0));
  إذا(!request_type)
    أرجع CURLE_بیرون_از_حافظه;

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
    أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(Curl_sha256it(sha_hash, (ثابت بیعلامت حرف *)canonical_request,
                   طول_رشته(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_بیرون_از_حافظه;
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
                              (صحيح)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  إذا(!str_مقصد_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_بیرون_از_حافظه;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_مقصد_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(داده, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_مقصد_sign);

  *request_type_بیرون = request_type;
  *credential_scope_بیرون = credential_scope;
  *str_مقصد_sign_بیرون = str_مقصد_sign;
  أرجع CURLE_OK;
}

ساكن CURLcode sign_and_مجموعه_auth_سرآیندها(بنية Curl_easy *داده,
                                          بنية Curl_str *provider0,
                                          بنية Curl_str *region,
                                          بنية Curl_str *service,
                                          ثابت حرف *request_type,
                                          ثابت حرف *credential_scope,
                                          ثابت حرف *date,
                                          ثابت حرف *str_مقصد_sign,
                                          ثابت حرف *date_سرآیند,
                                          ثابت حرف *content_sha256_hdr,
                                          بنية dynbuf *signed_سرآیندها)
{
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  ثابت حرف *passwd = Curl_creds_passwd(داده->state.creds);
  حرف *secret = عدم;
  بیعلامت حرف sign0[CURL_SHA256_DIGEST_درازی] = { 0 };
  بیعلامت حرف sign1[CURL_SHA256_DIGEST_درازی] = { 0 };
  حرف sha_hex[SHA256_HEX_درازی];
  حرف *auth_سرآیندها = عدم;
  حرف *user = curl_escape(Curl_creds_user(داده->state.creds), 0);
  إذا(!user)
    أرجع CURLE_بیرون_از_حافظه;

  secret = curl_maprintf("%.*s4%s", (صحيح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  إذا(!secret)
    اذهب fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, طول_رشته(secret), date, طول_رشته(date), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              request_type, طول_رشته(request_type), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              str_مقصد_sign, طول_رشته(str_مقصد_sign), sign0);

  sha256_مقصد_hex(sha_hex, sign0);

  infof(داده, "aws_sigv4: Signature - %s", sha_hex);

  auth_سرآیندها = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_سرآیندها),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_سرآیند ? date_سرآیند : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  إذا(!auth_سرآیندها)
    اذهب fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_سرآیندها[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(داده->req.hd_auth);
  داده->req.hd_auth = auth_سرآیندها;
  داده->state.authhost.انجامشده = TRUE;
  نتیجه = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  أرجع نتیجه;
}

CURLcode Curl_output_aws_sigv4(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  بنية Curl_str provider0 = { عدم, 0 };
  بنية Curl_str provider1 = { عدم, 0 };
  بنية Curl_str region = { عدم, 0 };
  بنية Curl_str service = { عدم, 0 };
  ثابت حرف *hostname = داده->state.origin->hostname;
  حرف timestamp[TIMESTAMP_اندازه];
  حرف date[9];
  بنية dynbuf canonical_سرآیندها;
  بنية dynbuf signed_سرآیندها;
  حرف *date_سرآیند = عدم;
  Curl_HttpReq httpreq;
  ثابت حرف *روش = عدم;
  ثابت حرف *payload_hash = عدم;
  size_t payload_hash_len = 0;
  بیعلامت حرف sha_hash[CURL_SHA256_DIGEST_درازی];
  حرف sha_hex[SHA256_HEX_درازی];
  حرف content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  حرف *canonical_request = عدم;
  حرف *request_type = عدم;
  حرف *credential_scope = عدم;
  حرف *str_مقصد_sign = عدم;

  إذا(داده->مجموعه.مسیر_as_is) {
    failf(داده, "Cannot use sigv4 authentication with path-as-is flag");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  إذا(Curl_checkheaders(داده, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    أرجع CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_سرآیندها, CURL_MAX_HTTP_سرآیند);
  curlx_dyn_init(&signed_سرآیندها, CURL_MAX_HTTP_سرآیند);

  نتیجه = parse_sigv4_params(داده, hostname, &provider0, &provider1,
                              &region, &service);
  إذا(!نتیجه) {
    Curl_http_روش(داده, &روش, &httpreq);
    نتیجه = get_payload_hash(داده, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  إذا(!نتیجه)
    نتیجه = get_timestamp(timestamp, حجم(timestamp));

  إذا(!نتیجه)
    نتیجه = make_canonical_request(داده, timestamp,
                                    &provider1, &service,
                                    روش, payload_hash, payload_hash_len,
                                    &date_سرآیند, content_sha256_hdr,
                                    &canonical_سرآیندها, &signed_سرآیندها,
                                    &canonical_request);
  إذا(!نتیجه) {
    /* the timestamp might have been updated in make_canonical_request */
    کپی_حافظه(date, timestamp, حجم(date) - 1);
    date[حجم(date) - 1] = 0;

    نتیجه = make_string_مقصد_sign(داده, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_مقصد_sign);
  }
  إذا(!نتیجه)
    نتیجه = sign_and_مجموعه_auth_سرآیندها(داده, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_مقصد_sign, date_سرآیند,
                                       content_sha256_hdr, &signed_سرآیندها);

  curlx_dyn_free(&canonical_سرآیندها);
  curlx_dyn_free(&signed_سرآیندها);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_مقصد_sign);
  curlx_free(date_سرآیند);
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
