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
  কুরু {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ধ্রুব unsigned বর্ণ *)(k), \
                         kl,                         \
                         (ধ্রুব unsigned বর্ণ *)(d), \
                         dl, o);                     \
    যদি(result) {                                     \
      গচ্ছ fail;                                     \
    }                                                \
  } যাবত্(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

সংরচনা pair {
  সংরচনা dynbuf চাবি;
  সংরচনা dynbuf মান;
};

স্থির শূন্য sha256_to_hex(বর্ণ *dst, unsigned বর্ণ *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned বর্ণ *)dst, SHA256_HEX_LENGTH);
}

স্থির বর্ণ *find_date_hdr(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *sig_hdr)
{
  বর্ণ *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  যদি(tmp)
    প্রত্যাগম tmp;
  প্রত্যাগম Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
স্থির শূন্য trim_headers(সংরচনা curl_slist *head)
{
  সংরচনা curl_slist *l;
  প্রতি(l = head; l; l = l->next) {
    ধ্রুব বর্ণ *মান; /* to read from */
    বর্ণ *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    মান = &l->data[colon];
    যদি(!*মান)
      অনুবর্ত;
    ++মান;
    store = (বর্ণ *)CURL_UNCONST(মান);

    /* skip leading whitespace */
    curlx_str_passblanks(&মান);

    যাবত্(*মান) {
      পূর্ণাঙ্ক space = 0;
      যাবত্(ISBLANK(*মান)) {
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
স্থির শূন্য pair_array_free(সংরচনা pair *pair_array, size_t অঙ্ক_elements)
{
  size_t index;

  প্রতি(index = 0; index != অঙ্ক_elements; index++) {
    curlx_dyn_free(&pair_array[index].চাবি);
    curlx_dyn_free(&pair_array[index].মান);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
স্থির শূন্য dyn_array_free(সংরচনা dynbuf *db, size_t অঙ্ক_elements)
{
  size_t index;

  প্রতি(index = 0; index < অঙ্ক_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

স্থির CURLcode split_to_dyn_array(ধ্রুব বর্ণ *source,
                                   সংরচনা dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *অঙ্ক_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t অঙ্ক_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  প্রতি(pos = 0; pos < len; pos++) {
    যদি(source[pos] == SPLIT_BY) {
      যদি(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        যদি(result)
          গচ্ছ fail;

        segment_length = 0;
        index++;
        যদি(++অঙ্ক_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          গচ্ছ fail;
        }
      }
      start = pos + 1;
    }
    অন্যথা {
      segment_length++;
    }
  }

  যদি(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    যদি(!result && ++অঙ্ক_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *অঙ্ক_splits_out = অঙ্ক_splits;
  প্রত্যাগম result;
}

স্থির bool is_reserved_char(ধ্রুব বর্ণ c)
{
  প্রত্যাগম (ISALNUM(c) || ISURLPUNTCS(c));
}

স্থির CURLcode uri_encode_path(সংরচনা Curl_str *original_path,
                                সংরচনা dynbuf *new_path)
{
  ধ্রুব বর্ণ *p = curlx_str(original_path);
  size_t i;

  প্রতি(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned বর্ণ c = p[i];
    যদি(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    অন্যথা
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    যদি(result)
      প্রত্যাগম result;
  }

  প্রত্যাগম CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
স্থির CURLcode normalize_query(ধ্রুব বর্ণ *string, size_t len,
                                সংরচনা dynbuf *db)
{
  CURLcode result = CURLE_OK;

  যাবত্(len && !result) {
    unsigned বর্ণ in = (unsigned বর্ণ)*string;
    যদি(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned বর্ণ)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      যদি(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        অনুবর্ত;
      }
    }
    অন্যথা {
      string++;
      len--;
    }

    যদি(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    অন্যথা যদি(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    অন্যথা
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  প্রত্যাগম result;
}

স্থির bool should_urlencode(সংরচনা Curl_str *service_name)
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
    প্রত্যাগম FALSE;
  }
  প্রত্যাগম TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_চাবি_LEN (MAX_SIGV4_LEN + পরিমাণ("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_চাবি_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
স্থির পূর্ণাঙ্ক compare_header_names(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b)
{
  ধ্রুব বর্ণ *colon_a;
  ধ্রুব বর্ণ *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  পূর্ণাঙ্ক cmp;

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
    প্রত্যাগম (পূর্ণাঙ্ক)(len_a - len_b);

  প্রত্যাগম cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
স্থির CURLcode merge_duplicate_headers(সংরচনা curl_slist *head)
{
  সংরচনা curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  যাবত্(curr) {
    সংরচনা curl_slist *next = curr->next;
    যদি(!next)
      ভঙ্গ;

    যদি(compare_header_names(curr->data, next->data) == 0) {
      সংরচনা dynbuf buf;
      ধ্রুব বর্ণ *colon_next;
      ধ্রুব বর্ণ *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      যদি(result)
        প্রত্যাগম result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      যদি(result)
        প্রত্যাগম result;

      result = curlx_dyn_add(&buf, val_next);
      যদি(result)
        প্রত্যাগম result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    অন্যথা {
      curr = curr->next;
    }
  }

  প্রত্যাগম CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
স্থির CURLcode make_headers(সংরচনা Curl_easy *data,
                             বর্ণ *timestamp,
                             ধ্রুব বর্ণ *provider1,
                             size_t plen, /* length of provider1 */
                             বর্ণ **date_header,
                             বর্ণ *content_sha256_header,
                             সংরচনা dynbuf *canonical_headers,
                             সংরচনা dynbuf *signed_headers)
{
  বর্ণ date_hdr_চাবি[DATE_HDR_চাবি_LEN];
  বর্ণ date_full_hdr[DATE_FULL_HDR_LEN];
  সংরচনা curl_slist *head = রিক্ত;
  সংরচনা curl_slist *tmp_head = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  সংরচনা curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_চাবি, DATE_HDR_চাবি_LEN, "X-%.*s-Date",
                 (পূর্ণাঙ্ক)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_চাবি[2], provider1, plen);
  date_hdr_চাবি[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (পূর্ণাঙ্ক)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  যদি(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    বর্ণ *fullhost = curlx_strdup(data->state.http_host);

    যদি(fullhost)
      head = Curl_slist_append_nodup(রিক্ত, fullhost);
    যদি(!head) {
      curlx_free(fullhost);
      গচ্ছ fail;
    }
  }

  যদি(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    যদি(!tmp_head)
      গচ্ছ fail;
    head = tmp_head;
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
  প্রতি(l = data->set.headers; l; l = l->next) {
    বর্ণ *dupdata;
    ধ্রুব বর্ণ *ptr;
    ধ্রুব বর্ণ *sep = strchr(l->data, ':');
    যদি(!sep)
      sep = strchr(l->data, ';');
    যদি(!sep || (*sep == ':' && !*(sep + 1)))
      অনুবর্ত;
    প্রতি(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    যদি(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      অনুবর্ত;
    dupdata = curlx_strdup(l->data);
    যদি(!dupdata)
      গচ্ছ fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    যদি(!tmp_head) {
      curlx_free(dupdata);
      গচ্ছ fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_চাবি);
  যদি(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    যদি(!tmp_head)
      গচ্ছ fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_চাবি, timestamp);
    যদি(!*date_header)
      গচ্ছ fail;
  }
  অন্যথা {
    ধ্রুব বর্ণ *মান;
    ধ্রুব বর্ণ *endp;
    মান = strchr(*date_header, ':');
    যদি(!মান) {
      *date_header = রিক্ত;
      গচ্ছ fail;
    }
    ++মান;
    curlx_str_passblanks(&মান);
    endp = মান;
    যাবত্(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    যদি((endp - মান) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, মান, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    অন্যথা
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = রিক্ত;
  }

  /* alpha-sort by header name in a case sensitive manner */
  কুরু {
    again = FALSE;
    প্রতি(l = head; l; l = l->next) {
      সংরচনা curl_slist *next = l->next;

      যদি(next && compare_header_names(l->data, next->data) > 0) {
        বর্ণ *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } যাবত্(again);

  result = merge_duplicate_headers(head);
  যদি(result)
    গচ্ছ fail;

  প্রতি(l = head; l; l = l->next) {
    বর্ণ *tmp;

    যদি(curlx_dyn_add(canonical_headers, l->data))
      গচ্ছ fail;
    যদি(curlx_dyn_add(canonical_headers, "\n"))
      গচ্ছ fail;

    tmp = strchr(l->data, ':');
    যদি(tmp)
      *tmp = 0;

    যদি(l != head && curlx_dyn_add(signed_headers, ";"))
      গচ্ছ fail;
    যদি(curlx_dyn_add(signed_headers, l->data))
      গচ্ছ fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  প্রত্যাগম result;
}

#define CONTENT_SHA256_চাবি_LEN (MAX_SIGV4_LEN + পরিমাণ("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_চাবি_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
স্থির ধ্রুব বর্ণ *parse_content_sha_hdr(সংরচনা Curl_easy *data,
                                         ধ্রুব বর্ণ *provider1,
                                         size_t plen,
                                         size_t *মান_len)
{
  বর্ণ চাবি[CONTENT_SHA256_চাবি_LEN];
  size_t চাবি_len;
  ধ্রুব বর্ণ *মান;
  size_t len;

  চাবি_len = curl_msnprintf(চাবি, পরিমাণ(চাবি), "x-%.*s-content-sha256",
                           (পূর্ণাঙ্ক)plen, provider1);

  মান = Curl_checkheaders(data, চাবি, চাবি_len);
  যদি(!মান)
    প্রত্যাগম রিক্ত;

  মান = strchr(মান, ':');
  যদি(!মান)
    প্রত্যাগম রিক্ত;
  ++মান;

  curlx_str_passblanks(&মান);

  len = strlen(মান);
  যাবত্(len > 0 && ISBLANK(মান[len - 1]))
    --len;

  *মান_len = len;
  প্রত্যাগম মান;
}

স্থির CURLcode calc_payload_hash(সংরচনা Curl_easy *data,
                                  unsigned বর্ণ *sha_hash, বর্ণ *sha_hex)
{
  ধ্রুব বর্ণ *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  যদি(post_data) {
    যদি(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    অন্যথা
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (ধ্রুব unsigned বর্ণ *)post_data,
                         post_data_len);
  যদি(!result)
    sha256_to_hex(sha_hex, sha_hash);
  প্রত্যাগম result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

স্থির CURLcode calc_s3_payload_hash(সংরচনা Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     ধ্রুব বর্ণ *provider1,
                                     size_t plen,
                                     unsigned বর্ণ *sha_hash,
                                     বর্ণ *sha_hex, বর্ণ *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  যদি(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    যদি(result)
      গচ্ছ fail;
  }
  অন্যথা {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (পূর্ণাঙ্ক)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক compare_func(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
{
  ধ্রুব সংরচনা pair *aa = a;
  ধ্রুব সংরচনা pair *bb = b;
  ধ্রুব size_t aa_চাবি_len = curlx_dyn_len(&aa->চাবি);
  ধ্রুব size_t bb_চাবি_len = curlx_dyn_len(&bb->চাবি);
  ধ্রুব size_t aa_মান_len = curlx_dyn_len(&aa->মান);
  ধ্রুব size_t bb_মান_len = curlx_dyn_len(&bb->মান);
  পূর্ণাঙ্ক compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  যদি((aa_চাবি_len == 0) && (bb_চাবি_len == 0))
    প্রত্যাগম 0;
  যদি(aa_চাবি_len == 0)
    প্রত্যাগম -1;
  যদি(bb_চাবি_len == 0)
    প্রত্যাগম 1;
  compare = strcmp(curlx_dyn_ptr(&aa->চাবি), curlx_dyn_ptr(&bb->চাবি));
  যদি(compare) {
    প্রত্যাগম compare;
  }

  /* Compare values */
  যদি((aa_মান_len == 0) && (bb_মান_len == 0))
    প্রত্যাগম 0;
  যদি(aa_মান_len == 0)
    প্রত্যাগম -1;
  যদি(bb_মান_len == 0)
    প্রত্যাগম 1;
  compare = strcmp(curlx_dyn_ptr(&aa->মান), curlx_dyn_ptr(&bb->মান));

  প্রত্যাগম compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(ধ্রুব বর্ণ *q, size_t len,
                             সংরচনা dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ধ্রুব বর্ণ *q, size_t len,
                             সংরচনা dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  সংরচনা Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  যদি(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  অন্যথা
    result = curlx_dyn_addn(new_path, q, len);

  যদি(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  প্রত্যাগম result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ধ্রুব বর্ণ *query, সংরচনা dynbuf *dq);
UNITTEST CURLcode canon_query(ধ্রুব বর্ণ *query, সংরচনা dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  সংরচনা dynbuf query_array[MAX_QUERY_COMPONENTS];
  সংরচনা pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t অঙ্ক_query_components;
  size_t counted_query_components = 0;
  size_t index;

  যদি(!query)
    প্রত্যাগম result;

  result = split_to_dyn_array(query, &query_array[0], &অঙ্ক_query_components);
  যদি(result) {
    গচ্ছ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  প্রতি(index = 0; index < অঙ্ক_query_components; index++) {
    ধ্রুব বর্ণ *in_চাবি;
    size_t in_চাবি_len;
    ধ্রুব বর্ণ *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    ধ্রুব বর্ণ *query_part = curlx_dyn_ptr(&query_array[index]);

    in_চাবি = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    যদি(!offset) {
      in_চাবি_len = strlen(in_চাবি);
    }
    অন্যথা {
      in_চাবি_len = offset - in_চাবি;
    }

    curlx_dyn_init(&encoded_query_array[index].চাবি,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].মান,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_চাবি, in_চাবি_len,
                             &encoded_query_array[index].চাবি);
    যদি(result) {
      গচ্ছ fail;
    }

    /* Decode/encode the value if it exists */
    যদি(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_মান_len;
      ধ্রুব বর্ণ *in_মান = offset + 1;
      in_মান_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_মান, in_মান_len,
                               &encoded_query_array[index].মান);
      যদি(result) {
        গচ্ছ fail;
      }
    }
    অন্যথা {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].মান, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].মান, "", 1);
    }

    যদি(result) {
      গচ্ছ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, অঙ্ক_query_components,
        পরিমাণ(সংরচনা pair), compare_func);

  /* Append the query components together to make a full query string */
  প্রতি(index = 0; index < অঙ্ক_query_components; index++) {

    যদি(index)
      result = curlx_dyn_addn(dq, "&", 1);
    যদি(!result) {
      ধ্রুব বর্ণ *চাবি_ptr = curlx_dyn_ptr(&encoded_query_array[index].চাবি);
      ধ্রুব বর্ণ *মান_ptr = curlx_dyn_ptr(&encoded_query_array[index].মান);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].মান);
      যদি(মান_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", চাবি_ptr, মান_ptr);
      }
      অন্যথা {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", চাবি_ptr);
      }
    }
    যদি(result)
      ভঙ্গ;
  }

fail:
  যদি(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], অঙ্ক_query_components);
  প্রত্যাগম result;
}

স্থির CURLcode parse_sigv4_params(সংরচনা Curl_easy *data,
                                   ধ্রুব বর্ণ *hostname,
                                   সংরচনা Curl_str *provider0,
                                   সংরচনা Curl_str *provider1,
                                   সংরচনা Curl_str *region,
                                   সংরচনা Curl_str *service)
{
  ধ্রুব বর্ণ *সারি = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  যদি(!সারি || !*সারি)
    সারি = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  যদি(curlx_str_until(&সারি, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
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
    ধ্রুব বর্ণ *p = hostname;
    যদি(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      প্রত্যাগম CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (পূর্ণাঙ্ক)curlx_strlen(service), curlx_str(service));

    যদি(!curlx_strlen(region)) {
      যদি(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        প্রত্যাগম CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (পূর্ণাঙ্ক)curlx_strlen(region), curlx_str(region));
    }
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode get_payload_hash(সংরচনা Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 সংরচনা Curl_str *provider0,
                                 সংরচনা Curl_str *provider1,
                                 সংরচনা Curl_str *service,
                                 unsigned বর্ণ *sha_hash,
                                 বর্ণ *sha_hex,
                                 বর্ণ *content_sha256_hdr,
                                 ধ্রুব বর্ণ **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  যদি(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    যদি(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    অন্যথা
      result = calc_payload_hash(data, sha_hash, sha_hex);
    যদি(result)
      প্রত্যাগম result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode get_timestamp(বর্ণ *timestamp, size_t stampsize)
{
  time_t clock;
  সংরচনা tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    বর্ণ *force_timestamp = getenv("CURL_FORCETIME");
    যদি(force_timestamp)
      clock = 0;
    অন্যথা
      clock = time(রিক্ত);
  }
#অন্যথা
  clock = time(রিক্ত);
#endif
  result = curlx_gmtime(clock, &tm);
  যদি(result)
    প্রত্যাগম result;

  যদি(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode make_canonical_request(সংরচনা Curl_easy *data,
                                       বর্ণ *timestamp,
                                       সংরচনা Curl_str *provider1,
                                       সংরচনা Curl_str *service,
                                       ধ্রুব বর্ণ *method,
                                       ধ্রুব বর্ণ *payload_hash,
                                       size_t payload_hash_len,
                                       বর্ণ **date_header_out,
                                       বর্ণ *content_sha256_hdr,
                                       সংরচনা dynbuf *canonical_headers,
                                       সংরচনা dynbuf *signed_headers,
                                       বর্ণ **canonical_request_out)
{
  সংরচনা dynbuf canonical_query;
  সংরচনা dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  যদি(result)
    গচ্ছ fail;

  result = canon_query(data->state.up.query, &canonical_query);
  যদি(result)
    গচ্ছ fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  যদি(result)
    গচ্ছ fail;

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
                  (পূর্ণাঙ্ক)payload_hash_len, payload_hash);
  যদি(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  প্রত্যাগম result;
}

স্থির CURLcode make_string_to_sign(সংরচনা Curl_easy *data,
                                    সংরচনা Curl_str *provider0,
                                    সংরচনা Curl_str *region,
                                    সংরচনা Curl_str *service,
                                    ধ্রুব বর্ণ *date,
                                    ধ্রুব বর্ণ *timestamp,
                                    ধ্রুব বর্ণ *canonical_request,
                                    বর্ণ **request_type_out,
                                    বর্ণ **credential_scope_out,
                                    বর্ণ **str_to_sign_out)
{
  বর্ণ *request_type;
  বর্ণ *credential_scope;
  বর্ণ *str_to_sign;
  unsigned বর্ণ sha_hash[CURL_SHA256_DIGEST_LENGTH];
  বর্ণ sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (পূর্ণাঙ্ক)curlx_strlen(provider0),
                               curlx_str(provider0));
  যদি(!request_type)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (পূর্ণাঙ্ক)curlx_strlen(region),
                                   curlx_str(region),
                                   (পূর্ণাঙ্ক)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  যদি(!credential_scope) {
    curlx_free(request_type);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_sha256it(sha_hash, (ধ্রুব unsigned বর্ণ *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
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
                              (পূর্ণাঙ্ক)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  যদি(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode sign_and_set_auth_headers(সংরচনা Curl_easy *data,
                                          সংরচনা Curl_str *provider0,
                                          সংরচনা Curl_str *region,
                                          সংরচনা Curl_str *service,
                                          ধ্রুব বর্ণ *request_type,
                                          ধ্রুব বর্ণ *credential_scope,
                                          ধ্রুব বর্ণ *date,
                                          ধ্রুব বর্ণ *str_to_sign,
                                          ধ্রুব বর্ণ *date_header,
                                          ধ্রুব বর্ণ *content_sha256_hdr,
                                          সংরচনা dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ধ্রুব বর্ণ *passwd = Curl_creds_passwd(data->state.creds);
  বর্ণ *secret = রিক্ত;
  unsigned বর্ণ sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned বর্ণ sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  বর্ণ sha_hex[SHA256_HEX_LENGTH];
  বর্ণ *auth_headers = রিক্ত;
  বর্ণ *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  যদি(!user)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (পূর্ণাঙ্ক)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  যদি(!secret)
    গচ্ছ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, পরিমাণ(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, পরিমাণ(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, পরিমাণ(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, পরিমাণ(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (পূর্ণাঙ্ক)curlx_strlen(provider0),
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
    গচ্ছ fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_headers[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(data->req.hd_auth);
  data->req.hd_auth = auth_headers;
  data->state.authhost.done = TRUE;
  result = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  প্রত্যাগম result;
}

CURLcode Curl_output_aws_sigv4(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  সংরচনা Curl_str provider0 = { রিক্ত, 0 };
  সংরচনা Curl_str provider1 = { রিক্ত, 0 };
  সংরচনা Curl_str region = { রিক্ত, 0 };
  সংরচনা Curl_str service = { রিক্ত, 0 };
  ধ্রুব বর্ণ *hostname = data->state.origin->hostname;
  বর্ণ timestamp[TIMESTAMP_SIZE];
  বর্ণ date[9];
  সংরচনা dynbuf canonical_headers;
  সংরচনা dynbuf signed_headers;
  বর্ণ *date_header = রিক্ত;
  Curl_HttpReq httpreq;
  ধ্রুব বর্ণ *method = রিক্ত;
  ধ্রুব বর্ণ *payload_hash = রিক্ত;
  size_t payload_hash_len = 0;
  unsigned বর্ণ sha_hash[CURL_SHA256_DIGEST_LENGTH];
  বর্ণ sha_hex[SHA256_HEX_LENGTH];
  বর্ণ content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  বর্ণ *canonical_request = রিক্ত;
  বর্ণ *request_type = রিক্ত;
  বর্ণ *credential_scope = রিক্ত;
  বর্ণ *str_to_sign = রিক্ত;

  যদি(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  যদি(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    প্রত্যাগম CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  যদি(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  যদি(!result)
    result = get_timestamp(timestamp, পরিমাণ(timestamp));

  যদি(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  যদি(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, পরিমাণ(date) - 1);
    date[পরিমাণ(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  যদি(!result)
    result = sign_and_set_auth_headers(data, &provider0, &region, &service,
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
  প্রত্যাগম result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
