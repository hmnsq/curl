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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "http_aws_sigv4.h"
#অন্তর্ভাব "curl_sha256.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "slist.h"

#অন্তর্ভাব <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  করো {                                               \
    ফলাফল = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ধ্রুবক unsigned অক্ষর *)(k), \
                         kl,                         \
                         (ধ্রুবক unsigned অক্ষর *)(d), \
                         dl, o);                     \
    যদি(ফলাফল) {                                     \
      যাও fail;                                     \
    }                                                \
  } যখন(0)

#define TIMESTAMP_আকার 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_দৈর্ঘ্য ((2 * CURL_SHA256_DIGEST_দৈর্ঘ্য) + 1)

#define MAX_QUERY_COMPONENTS 128

কাঠামো pair {
  কাঠামো dynbuf চাবি;
  কাঠামো dynbuf মান;
};

স্থির শূন্য sha256_to_hex(অক্ষর *dst, unsigned অক্ষর *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_দৈর্ঘ্য,
                 (unsigned অক্ষর *)dst, SHA256_HEX_দৈর্ঘ্য);
}

স্থির অক্ষর *find_date_hdr(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *sig_hdr)
{
  অক্ষর *tmp = Curl_checkheaders(তথ্য, sig_hdr, strlen(sig_hdr));

  যদি(tmp)
    ফেরত tmp;
  ফেরত Curl_checkheaders(তথ্য, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
স্থির শূন্য trim_headers(কাঠামো curl_slist *শীর্ষ)
{
  কাঠামো curl_slist *l;
  জন্য(l = শীর্ষ; l; l = l->পরবর্তী) {
    ধ্রুবক অক্ষর *মান; /* to read from */
    অক্ষর *store;
    size_t colon = strcspn(l->তথ্য, ":");
    Curl_strntolower(l->তথ্য, l->তথ্য, colon);

    মান = &l->তথ্য[colon];
    যদি(!*মান)
      চালু;
    ++মান;
    store = (অক্ষর *)CURL_UNCONST(মান);

    /* skip leading whitespace */
    curlx_str_passblanks(&মান);

    যখন(*মান) {
      পূর্ণসংখ্যা space = 0;
      যখন(ISBLANK(*মান)) {
        মান++;
        space++;
      }
      যদি(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        যদি(*মান)
          *store++ = ' ';
      }
      অন্যথা
        *store++ = *মান++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
স্থির শূন্য pair_array_free(কাঠামো pair *pair_array, size_t অঙ্ক_elements)
{
  size_t সূচক;

  জন্য(সূচক = 0; সূচক != অঙ্ক_elements; সূচক++) {
    curlx_dyn_free(&pair_array[সূচক].চাবি);
    curlx_dyn_free(&pair_array[সূচক].মান);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
স্থির শূন্য dyn_array_free(কাঠামো dynbuf *db, size_t অঙ্ক_elements)
{
  size_t সূচক;

  জন্য(সূচক = 0; সূচক < অঙ্ক_elements; সূচক++)
    curlx_dyn_free((&db[সূচক]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

স্থির CURLcode split_to_dyn_array(ধ্রুবক অক্ষর *source,
                                   কাঠামো dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *অঙ্ক_splits_out)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_দৈর্ঘ্য = 0;
  size_t সূচক = 0;
  size_t অঙ্ক_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  জন্য(pos = 0; pos < len; pos++) {
    যদি(source[pos] == SPLIT_BY) {
      যদি(segment_দৈর্ঘ্য) {
        curlx_dyn_init(&db[সূচক], segment_দৈর্ঘ্য + 1);
        ফলাফল = curlx_dyn_addn(&db[সূচক], &source[start], segment_দৈর্ঘ্য);
        যদি(ফলাফল)
          যাও fail;

        segment_দৈর্ঘ্য = 0;
        সূচক++;
        যদি(++অঙ্ক_splits == MAX_QUERY_COMPONENTS) {
          ফলাফল = CURLE_TOO_LARGE;
          যাও fail;
        }
      }
      start = pos + 1;
    }
    অন্যথা {
      segment_দৈর্ঘ্য++;
    }
  }

  যদি(segment_দৈর্ঘ্য) {
    curlx_dyn_init(&db[সূচক], segment_দৈর্ঘ্য + 1);
    ফলাফল = curlx_dyn_addn(&db[সূচক], &source[start], segment_দৈর্ঘ্য);
    যদি(!ফলাফল && ++অঙ্ক_splits == MAX_QUERY_COMPONENTS)
      ফলাফল = CURLE_TOO_LARGE;
  }
fail:
  *অঙ্ক_splits_out = অঙ্ক_splits;
  ফেরত ফলাফল;
}

স্থির bool is_reserved_char(ধ্রুবক অক্ষর c)
{
  ফেরত (ISALNUM(c) || ISURLPUNTCS(c));
}

স্থির CURLcode uri_encode_path(কাঠামো Curl_str *original_path,
                                কাঠামো dynbuf *new_path)
{
  ধ্রুবক অক্ষর *p = curlx_str(original_path);
  size_t i;

  জন্য(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode ফলাফল = CURLE_OK;
    unsigned অক্ষর c = p[i];
    যদি(is_reserved_char(c) || c == '/')
      ফলাফল = curlx_dyn_addn(new_path, &c, 1);
    অন্যথা
      ফলাফল = curlx_dyn_addf(new_path, "%%%02X", c);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফেরত CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
স্থির CURLcode normalize_query(ধ্রুবক অক্ষর *string, size_t len,
                                কাঠামো dynbuf *db)
{
  CURLcode ফলাফল = CURLE_OK;

  যখন(len && !ফলাফল) {
    unsigned অক্ষর in = (unsigned অক্ষর)*string;
    যদি(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned অক্ষর)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      যদি(in == '+') {
        /* decodes to plus, so leave this encoded */
        ফলাফল = curlx_dyn_addn(db, "%2B", 3);
        চালু;
      }
    }
    অন্যথা {
      string++;
      len--;
    }

    যদি(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      ফলাফল = curlx_dyn_addn(db, &in, 1);
    অন্যথা যদি(in == '+')
      /* Encode '+' as space */
      ফলাফল = curlx_dyn_add(db, "%20");
    অন্যথা
      ফলাফল = curlx_dyn_addf(db, "%%%02X", in);
  }

  ফেরত ফলাফল;
}

স্থির bool should_urlencode(কাঠামো Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  যদি(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_চাবি_LEN (MAX_SIGV4_LEN + আকার("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_চাবি_LEN + TIMESTAMP_আকার + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
স্থির পূর্ণসংখ্যা compare_header_names(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b)
{
  ধ্রুবক অক্ষর *colon_a;
  ধ্রুবক অক্ষর *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  পূর্ণসংখ্যা cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  যদি(!cmp)
    ফেরত (পূর্ণসংখ্যা)(len_a - len_b);

  ফেরত cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
স্থির CURLcode merge_duplicate_headers(কাঠামো curl_slist *শীর্ষ)
{
  কাঠামো curl_slist *curr = শীর্ষ;
  CURLcode ফলাফল = CURLE_OK;

  যখন(curr) {
    কাঠামো curl_slist *পরবর্তী = curr->পরবর্তী;
    যদি(!পরবর্তী)
      বিরতি;

    যদি(compare_header_names(curr->তথ্য, পরবর্তী->তথ্য) == 0) {
      কাঠামো dynbuf buf;
      ধ্রুবক অক্ষর *colon_পরবর্তী;
      ধ্রুবক অক্ষর *val_পরবর্তী;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      ফলাফল = curlx_dyn_add(&buf, curr->তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      colon_পরবর্তী = strchr(পরবর্তী->তথ্য, ':');
      DEBUGASSERT(colon_পরবর্তী);
      val_পরবর্তী = colon_পরবর্তী + 1;

      ফলাফল = curlx_dyn_addn(&buf, ",", 1);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      ফলাফল = curlx_dyn_add(&buf, val_পরবর্তী);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      curlx_free(curr->তথ্য);
      curr->তথ্য = curlx_dyn_ptr(&buf);

      curr->পরবর্তী = পরবর্তী->পরবর্তী;
      curlx_free(পরবর্তী->তথ্য);
      curlx_free(পরবর্তী);
    }
    অন্যথা {
      curr = curr->পরবর্তী;
    }
  }

  ফেরত CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
স্থির CURLcode make_headers(কাঠামো Curl_easy *তথ্য,
                             অক্ষর *timestamp,
                             ধ্রুবক অক্ষর *provider1,
                             size_t plen, /* length of provider1 */
                             অক্ষর **date_header,
                             অক্ষর *content_sha256_header,
                             কাঠামো dynbuf *canonical_headers,
                             কাঠামো dynbuf *signed_headers)
{
  অক্ষর date_hdr_চাবি[DATE_HDR_চাবি_LEN];
  অক্ষর date_full_hdr[DATE_FULL_HDR_LEN];
  কাঠামো curl_slist *শীর্ষ = ফাঁকা;
  কাঠামো curl_slist *tmp_শীর্ষ = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  কাঠামো curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_চাবি, DATE_HDR_চাবি_LEN, "X-%.*s-Date",
                 (পূর্ণসংখ্যা)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_চাবি[2], provider1, plen);
  date_hdr_চাবি[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (পূর্ণসংখ্যা)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  যদি(!Curl_checkheaders(তথ্য, STRCONST("Host")) &&
     তথ্য->state.http_host) {
    /* Host: [host]:[port] */
    অক্ষর *fullhost = curlx_strdup(তথ্য->state.http_host);

    যদি(fullhost)
      শীর্ষ = Curl_slist_append_nodup(ফাঁকা, fullhost);
    যদি(!শীর্ষ) {
      curlx_free(fullhost);
      যাও fail;
    }
  }

  যদি(*content_sha256_header) {
    tmp_শীর্ষ = curl_slist_append(শীর্ষ, content_sha256_header);
    যদি(!tmp_শীর্ষ)
      যাও fail;
    শীর্ষ = tmp_শীর্ষ;
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
  জন্য(l = তথ্য->set.headers; l; l = l->পরবর্তী) {
    অক্ষর *dupdata;
    ধ্রুবক অক্ষর *ptr;
    ধ্রুবক অক্ষর *sep = strchr(l->তথ্য, ':');
    যদি(!sep)
      sep = strchr(l->তথ্য, ';');
    যদি(!sep || (*sep == ':' && !*(sep + 1)))
      চালু;
    জন্য(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    যদি(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      চালু;
    dupdata = curlx_strdup(l->তথ্য);
    যদি(!dupdata)
      যাও fail;
    dupdata[sep - l->তথ্য] = ':';
    tmp_শীর্ষ = Curl_slist_append_nodup(শীর্ষ, dupdata);
    যদি(!tmp_শীর্ষ) {
      curlx_free(dupdata);
      যাও fail;
    }
    শীর্ষ = tmp_শীর্ষ;
  }

  trim_headers(শীর্ষ);

  *date_header = find_date_hdr(তথ্য, date_hdr_চাবি);
  যদি(!*date_header) {
    tmp_শীর্ষ = curl_slist_append(শীর্ষ, date_full_hdr);
    যদি(!tmp_শীর্ষ)
      যাও fail;
    শীর্ষ = tmp_শীর্ষ;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_চাবি, timestamp);
    যদি(!*date_header)
      যাও fail;
  }
  অন্যথা {
    ধ্রুবক অক্ষর *মান;
    ধ্রুবক অক্ষর *endp;
    মান = strchr(*date_header, ':');
    যদি(!মান) {
      *date_header = ফাঁকা;
      যাও fail;
    }
    ++মান;
    curlx_str_passblanks(&মান);
    endp = মান;
    যখন(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    যদি((endp - মান) == TIMESTAMP_আকার - 1) {
      memcpy(timestamp, মান, TIMESTAMP_আকার - 1);
      timestamp[TIMESTAMP_আকার - 1] = 0;
    }
    অন্যথা
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = ফাঁকা;
  }

  /* alpha-sort by header name in a case sensitive manner */
  করো {
    again = FALSE;
    জন্য(l = শীর্ষ; l; l = l->পরবর্তী) {
      কাঠামো curl_slist *পরবর্তী = l->পরবর্তী;

      যদি(পরবর্তী && compare_header_names(l->তথ্য, পরবর্তী->তথ্য) > 0) {
        অক্ষর *tmp = l->তথ্য;

        l->তথ্য = পরবর্তী->তথ্য;
        পরবর্তী->তথ্য = tmp;
        again = TRUE;
      }
    }
  } যখন(again);

  ফলাফল = merge_duplicate_headers(শীর্ষ);
  যদি(ফলাফল)
    যাও fail;

  জন্য(l = শীর্ষ; l; l = l->পরবর্তী) {
    অক্ষর *tmp;

    যদি(curlx_dyn_add(canonical_headers, l->তথ্য))
      যাও fail;
    যদি(curlx_dyn_add(canonical_headers, "\n"))
      যাও fail;

    tmp = strchr(l->তথ্য, ':');
    যদি(tmp)
      *tmp = 0;

    যদি(l != শীর্ষ && curlx_dyn_add(signed_headers, ";"))
      যাও fail;
    যদি(curlx_dyn_add(signed_headers, l->তথ্য))
      যাও fail;
  }

  ফলাফল = CURLE_OK;
fail:
  curl_slist_free_all(শীর্ষ);

  ফেরত ফলাফল;
}

#define CONTENT_SHA256_চাবি_LEN (MAX_SIGV4_LEN + আকার("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_চাবি_LEN + 2 + SHA256_HEX_দৈর্ঘ্য)

/* try to parse a payload hash from the content-sha256 header */
স্থির ধ্রুবক অক্ষর *parse_content_sha_hdr(কাঠামো Curl_easy *তথ্য,
                                         ধ্রুবক অক্ষর *provider1,
                                         size_t plen,
                                         size_t *মান_len)
{
  অক্ষর চাবি[CONTENT_SHA256_চাবি_LEN];
  size_t চাবি_len;
  ধ্রুবক অক্ষর *মান;
  size_t len;

  চাবি_len = curl_msnprintf(চাবি, আকার(চাবি), "x-%.*s-content-sha256",
                           (পূর্ণসংখ্যা)plen, provider1);

  মান = Curl_checkheaders(তথ্য, চাবি, চাবি_len);
  যদি(!মান)
    ফেরত ফাঁকা;

  মান = strchr(মান, ':');
  যদি(!মান)
    ফেরত ফাঁকা;
  ++মান;

  curlx_str_passblanks(&মান);

  len = strlen(মান);
  যখন(len > 0 && ISBLANK(মান[len - 1]))
    --len;

  *মান_len = len;
  ফেরত মান;
}

স্থির CURLcode calc_payload_hash(কাঠামো Curl_easy *তথ্য,
                                  unsigned অক্ষর *sha_hash, অক্ষর *sha_hex)
{
  ধ্রুবক অক্ষর *post_তথ্য = তথ্য->set.postfields;
  size_t post_তথ্য_len = 0;
  CURLcode ফলাফল;

  যদি(post_তথ্য) {
    যদি(তথ্য->set.postfieldsize < 0)
      post_তথ্য_len = strlen(post_তথ্য);
    অন্যথা
      post_তথ্য_len = (size_t)তথ্য->set.postfieldsize;
  }
  ফলাফল = Curl_sha256it(sha_hash, (ধ্রুবক unsigned অক্ষর *)post_তথ্য,
                         post_তথ্য_len);
  যদি(!ফলাফল)
    sha256_to_hex(sha_hex, sha_hash);
  ফেরত ফলাফল;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

স্থির CURLcode calc_s3_payload_hash(কাঠামো Curl_easy *তথ্য,
                                     Curl_HttpReq httpreq,
                                     ধ্রুবক অক্ষর *provider1,
                                     size_t plen,
                                     unsigned অক্ষর *sha_hash,
                                     অক্ষর *sha_hex, অক্ষর *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_শীর্ষ);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || তথ্য->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && তথ্য->set.postfields);
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  যদি(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    ফলাফল = calc_payload_hash(তথ্য, sha_hash, sha_hex);
    যদি(ফলাফল)
      যাও fail;
  }
  অন্যথা {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_দৈর্ঘ্য); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (পূর্ণসংখ্যা)plen, provider1, sha_hex);

  ফলাফল = CURLE_OK;
fail:
  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা compare_func(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
{
  ধ্রুবক কাঠামো pair *aa = a;
  ধ্রুবক কাঠামো pair *bb = b;
  ধ্রুবক size_t aa_চাবি_len = curlx_dyn_len(&aa->চাবি);
  ধ্রুবক size_t bb_চাবি_len = curlx_dyn_len(&bb->চাবি);
  ধ্রুবক size_t aa_মান_len = curlx_dyn_len(&aa->মান);
  ধ্রুবক size_t bb_মান_len = curlx_dyn_len(&bb->মান);
  পূর্ণসংখ্যা compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  যদি((aa_চাবি_len == 0) && (bb_চাবি_len == 0))
    ফেরত 0;
  যদি(aa_চাবি_len == 0)
    ফেরত -1;
  যদি(bb_চাবি_len == 0)
    ফেরত 1;
  compare = strcmp(curlx_dyn_ptr(&aa->চাবি), curlx_dyn_ptr(&bb->চাবি));
  যদি(compare) {
    ফেরত compare;
  }

  /* Compare values */
  যদি((aa_মান_len == 0) && (bb_মান_len == 0))
    ফেরত 0;
  যদি(aa_মান_len == 0)
    ফেরত -1;
  যদি(bb_মান_len == 0)
    ফেরত 1;
  compare = strcmp(curlx_dyn_ptr(&aa->মান), curlx_dyn_ptr(&bb->মান));

  ফেরত compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(ধ্রুবক অক্ষর *q, size_t len,
                             কাঠামো dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ধ্রুবক অক্ষর *q, size_t len,
                             কাঠামো dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode ফলাফল = CURLE_OK;

  কাঠামো Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  যদি(do_uri_encode)
    ফলাফল = uri_encode_path(&original_path, new_path);
  অন্যথা
    ফলাফল = curlx_dyn_addn(new_path, q, len);

  যদি(!ফলাফল && curlx_dyn_len(new_path) == 0)
    ফলাফল = curlx_dyn_add(new_path, "/");

  ফেরত ফলাফল;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ধ্রুবক অক্ষর *query, কাঠামো dynbuf *dq);
UNITTEST CURLcode canon_query(ধ্রুবক অক্ষর *query, কাঠামো dynbuf *dq)
{
  CURLcode ফলাফল = CURLE_OK;

  কাঠামো dynbuf query_array[MAX_QUERY_COMPONENTS];
  কাঠামো pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t অঙ্ক_query_components;
  size_t counted_query_components = 0;
  size_t সূচক;

  যদি(!query)
    ফেরত ফলাফল;

  ফলাফল = split_to_dyn_array(query, &query_array[0], &অঙ্ক_query_components);
  যদি(ফলাফল) {
    যাও fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  জন্য(সূচক = 0; সূচক < অঙ্ক_query_components; সূচক++) {
    ধ্রুবক অক্ষর *in_চাবি;
    size_t in_চাবি_len;
    ধ্রুবক অক্ষর *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[সূচক]);
    ধ্রুবক অক্ষর *query_part = curlx_dyn_ptr(&query_array[সূচক]);

    in_চাবি = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    যদি(!offset) {
      in_চাবি_len = strlen(in_চাবি);
    }
    অন্যথা {
      in_চাবি_len = offset - in_চাবি;
    }

    curlx_dyn_init(&encoded_query_array[সূচক].চাবি,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[সূচক].মান,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    ফলাফল = normalize_query(in_চাবি, in_চাবি_len,
                             &encoded_query_array[সূচক].চাবি);
    যদি(ফলাফল) {
      যাও fail;
    }

    /* Decode/encode the value if it exists */
    যদি(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_মান_len;
      ধ্রুবক অক্ষর *in_মান = offset + 1;
      in_মান_len = query_part + query_part_len - (offset + 1);
      ফলাফল = normalize_query(in_মান, in_মান_len,
                               &encoded_query_array[সূচক].মান);
      যদি(ফলাফল) {
        যাও fail;
      }
    }
    অন্যথা {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[সূচক].মান, 2);
      ফলাফল = curlx_dyn_addn(&encoded_query_array[সূচক].মান, "", 1);
    }

    যদি(ফলাফল) {
      যাও fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, অঙ্ক_query_components,
        আকার(কাঠামো pair), compare_func);

  /* Append the query components together to make a full query string */
  জন্য(সূচক = 0; সূচক < অঙ্ক_query_components; সূচক++) {

    যদি(সূচক)
      ফলাফল = curlx_dyn_addn(dq, "&", 1);
    যদি(!ফলাফল) {
      ধ্রুবক অক্ষর *চাবি_ptr = curlx_dyn_ptr(&encoded_query_array[সূচক].চাবি);
      ধ্রুবক অক্ষর *মান_ptr = curlx_dyn_ptr(&encoded_query_array[সূচক].মান);
      size_t vlen = curlx_dyn_len(&encoded_query_array[সূচক].মান);
      যদি(মান_ptr && vlen) {
        ফলাফল = curlx_dyn_addf(dq, "%s=%s", চাবি_ptr, মান_ptr);
      }
      অন্যথা {
        /* Empty value is always encoded to key= */
        ফলাফল = curlx_dyn_addf(dq, "%s=", চাবি_ptr);
      }
    }
    যদি(ফলাফল)
      বিরতি;
  }

fail:
  যদি(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], অঙ্ক_query_components);
  ফেরত ফলাফল;
}

স্থির CURLcode parse_sigv4_params(কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক অক্ষর *hostname,
                                   কাঠামো Curl_str *provider0,
                                   কাঠামো Curl_str *provider1,
                                   কাঠামো Curl_str *region,
                                   কাঠামো Curl_str *service)
{
  ধ্রুবক অক্ষর *সারি = CURL_EASY_STR(তথ্য, STRING_AWS_SIGV4);
  যদি(!সারি || !*সারি)
    সারি = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  যদি(curlx_str_until(&সারি, provider0, MAX_SIGV4_LEN, ':')) {
    failf(তথ্য, "first aws-sigv4 provider cannot be empty");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  যদি(curlx_str_single(&সারি, ':') ||
     curlx_str_until(&সারি, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  অন্যথা যদি(curlx_str_single(&সারি, ':') ||
          curlx_str_until(&সারি, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&সারি, ':') ||
          curlx_str_until(&সারি, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  যদি(!curlx_strlen(service)) {
    ধ্রুবক অক্ষর *p = hostname;
    যদি(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(তথ্য, "aws-sigv4: service missing in parameters and hostname");
      ফেরত CURLE_URL_MALFORMAT;
    }

    infof(তথ্য, "aws_sigv4: picked service %.*s from host",
          (পূর্ণসংখ্যা)curlx_strlen(service), curlx_str(service));

    যদি(!curlx_strlen(region)) {
      যদি(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(তথ্য, "aws-sigv4: region missing in parameters and hostname");
        ফেরত CURLE_URL_MALFORMAT;
      }
      infof(তথ্য, "aws_sigv4: picked region %.*s from host",
            (পূর্ণসংখ্যা)curlx_strlen(region), curlx_str(region));
    }
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode get_payload_hash(কাঠামো Curl_easy *তথ্য,
                                 Curl_HttpReq httpreq,
                                 কাঠামো Curl_str *provider0,
                                 কাঠামো Curl_str *provider1,
                                 কাঠামো Curl_str *service,
                                 unsigned অক্ষর *sha_hash,
                                 অক্ষর *sha_hex,
                                 অক্ষর *content_sha256_hdr,
                                 ধ্রুবক অক্ষর **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(তথ্য, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  যদি(!*payload_hash_out) {
    CURLcode ফলাফল;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    যদি(sign_as_s3)
      ফলাফল = calc_s3_payload_hash(তথ্য, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    অন্যথা
      ফলাফল = calc_payload_hash(তথ্য, sha_hash, sha_hex);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode get_timestamp(অক্ষর *timestamp, size_t stampsize)
{
  time_t clock;
  কাঠামো tm tm;
  CURLcode ফলাফল;

#ifdef DEBUGBUILD
  {
    অক্ষর *force_timestamp = getenv("CURL_FORCETIME");
    যদি(force_timestamp)
      clock = 0;
    অন্যথা
      clock = time(ফাঁকা);
  }
#অন্যথা
  clock = time(ফাঁকা);
#endif
  ফলাফল = curlx_gmtime(clock, &tm);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

স্থির CURLcode make_canonical_request(কাঠামো Curl_easy *তথ্য,
                                       অক্ষর *timestamp,
                                       কাঠামো Curl_str *provider1,
                                       কাঠামো Curl_str *service,
                                       ধ্রুবক অক্ষর *method,
                                       ধ্রুবক অক্ষর *payload_hash,
                                       size_t payload_hash_len,
                                       অক্ষর **date_header_out,
                                       অক্ষর *content_sha256_hdr,
                                       কাঠামো dynbuf *canonical_headers,
                                       কাঠামো dynbuf *signed_headers,
                                       অক্ষর **canonical_request_out)
{
  কাঠামো dynbuf canonical_query;
  কাঠামো dynbuf canonical_path;
  CURLcode ফলাফল;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  ফলাফল = make_headers(তথ্য, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  যদি(ফলাফল)
    যাও fail;

  ফলাফল = canon_query(তথ্য->state.up.query, &canonical_query);
  যদি(ফলাফল)
    যাও fail;

  ফলাফল = canon_path(তথ্য->state.up.path, strlen(তথ্য->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  যদি(ফলাফল)
    যাও fail;

  *canonical_request_out =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  method,
                  curlx_dyn_ptr(&canonical_path),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_headers),
                  curlx_dyn_ptr(signed_headers),
                  (পূর্ণসংখ্যা)payload_hash_len, payload_hash);
  যদি(!*canonical_request_out) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail;
  }

  ফলাফল = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  ফেরত ফলাফল;
}

স্থির CURLcode make_string_to_sign(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো Curl_str *provider0,
                                    কাঠামো Curl_str *region,
                                    কাঠামো Curl_str *service,
                                    ধ্রুবক অক্ষর *date,
                                    ধ্রুবক অক্ষর *timestamp,
                                    ধ্রুবক অক্ষর *canonical_request,
                                    অক্ষর **request_type_out,
                                    অক্ষর **credential_scope_out,
                                    অক্ষর **str_to_sign_out)
{
  অক্ষর *request_type;
  অক্ষর *credential_scope;
  অক্ষর *str_to_sign;
  unsigned অক্ষর sha_hash[CURL_SHA256_DIGEST_দৈর্ঘ্য];
  অক্ষর sha_hex[SHA256_HEX_দৈর্ঘ্য];

  request_type = curl_maprintf("%.*s4_request",
                               (পূর্ণসংখ্যা)curlx_strlen(provider0),
                               curlx_str(provider0));
  যদি(!request_type)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (পূর্ণসংখ্যা)curlx_strlen(region),
                                   curlx_str(region),
                                   (পূর্ণসংখ্যা)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  যদি(!credential_scope) {
    curlx_free(request_type);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_sha256it(sha_hash, (ধ্রুবক unsigned অক্ষর *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  sha256_to_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_to_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (পূর্ণসংখ্যা)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  যদি(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(তথ্য, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  ফেরত CURLE_OK;
}

স্থির CURLcode sign_and_set_auth_headers(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_str *provider0,
                                          কাঠামো Curl_str *region,
                                          কাঠামো Curl_str *service,
                                          ধ্রুবক অক্ষর *request_type,
                                          ধ্রুবক অক্ষর *credential_scope,
                                          ধ্রুবক অক্ষর *date,
                                          ধ্রুবক অক্ষর *str_to_sign,
                                          ধ্রুবক অক্ষর *date_header,
                                          ধ্রুবক অক্ষর *content_sha256_hdr,
                                          কাঠামো dynbuf *signed_headers)
{
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  ধ্রুবক অক্ষর *passwd = Curl_creds_passwd(তথ্য->state.creds);
  অক্ষর *secret = ফাঁকা;
  unsigned অক্ষর sign0[CURL_SHA256_DIGEST_দৈর্ঘ্য] = { 0 };
  unsigned অক্ষর sign1[CURL_SHA256_DIGEST_দৈর্ঘ্য] = { 0 };
  অক্ষর sha_hex[SHA256_HEX_দৈর্ঘ্য];
  অক্ষর *auth_headers = ফাঁকা;
  অক্ষর *user = curl_escape(Curl_creds_user(তথ্য->state.creds), 0);
  যদি(!user)
    ফেরত CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (পূর্ণসংখ্যা)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  যদি(!secret)
    যাও fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, আকার(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, আকার(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, আকার(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, আকার(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(তথ্য, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (পূর্ণসংখ্যা)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_headers),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_header ? date_header : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  যদি(!auth_headers)
    যাও fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_headers[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(তথ্য->req.hd_auth);
  তথ্য->req.hd_auth = auth_headers;
  তথ্য->state.authhost.done = TRUE;
  ফলাফল = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  ফেরত ফলাফল;
}

CURLcode Curl_output_aws_sigv4(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  কাঠামো Curl_str provider0 = { ফাঁকা, 0 };
  কাঠামো Curl_str provider1 = { ফাঁকা, 0 };
  কাঠামো Curl_str region = { ফাঁকা, 0 };
  কাঠামো Curl_str service = { ফাঁকা, 0 };
  ধ্রুবক অক্ষর *hostname = তথ্য->state.origin->hostname;
  অক্ষর timestamp[TIMESTAMP_আকার];
  অক্ষর date[9];
  কাঠামো dynbuf canonical_headers;
  কাঠামো dynbuf signed_headers;
  অক্ষর *date_header = ফাঁকা;
  Curl_HttpReq httpreq;
  ধ্রুবক অক্ষর *method = ফাঁকা;
  ধ্রুবক অক্ষর *payload_hash = ফাঁকা;
  size_t payload_hash_len = 0;
  unsigned অক্ষর sha_hash[CURL_SHA256_DIGEST_দৈর্ঘ্য];
  অক্ষর sha_hex[SHA256_HEX_দৈর্ঘ্য];
  অক্ষর content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  অক্ষর *canonical_request = ফাঁকা;
  অক্ষর *request_type = ফাঁকা;
  অক্ষর *credential_scope = ফাঁকা;
  অক্ষর *str_to_sign = ফাঁকা;

  যদি(তথ্য->set.path_as_is) {
    failf(তথ্য, "Cannot use sigv4 authentication with path-as-is flag");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(Curl_checkheaders(তথ্য, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    ফেরত CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  ফলাফল = parse_sigv4_params(তথ্য, hostname, &provider0, &provider1,
                              &region, &service);
  যদি(!ফলাফল) {
    Curl_http_method(তথ্য, &method, &httpreq);
    ফলাফল = get_payload_hash(তথ্য, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  যদি(!ফলাফল)
    ফলাফল = get_timestamp(timestamp, আকার(timestamp));

  যদি(!ফলাফল)
    ফলাফল = make_canonical_request(তথ্য, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  যদি(!ফলাফল) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, আকার(date) - 1);
    date[আকার(date) - 1] = 0;

    ফলাফল = make_string_to_sign(তথ্য, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  যদি(!ফলাফল)
    ফলাফল = sign_and_set_auth_headers(তথ্য, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_to_sign, date_header,
                                       content_sha256_hdr, &signed_headers);

  curlx_dyn_free(&canonical_headers);
  curlx_dyn_free(&signed_headers);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_to_sign);
  curlx_free(date_header);
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
