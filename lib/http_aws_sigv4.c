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

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  انجامبده {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت unsigned نویسه *)(k), \
                         kl,                         \
                         (ثابت unsigned نویسه *)(d), \
                         dl, o);                     \
    اگر(result) {                                     \
      برو fail;                                     \
    }                                                \
  } وقتی(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

ساختار pair {
  ساختار dynbuf کلید;
  ساختار dynbuf مقدار;
};

ایستا تهی sha256_to_hex(نویسه *dst, unsigned نویسه *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned نویسه *)dst, SHA256_HEX_LENGTH);
}

ایستا نویسه *find_date_hdr(ساختار Curl_easy *data, ثابت نویسه *sig_hdr)
{
  نویسه *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  اگر(tmp)
    بازگشت tmp;
  بازگشت Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ایستا تهی trim_headers(ساختار curl_slist *head)
{
  ساختار curl_slist *l;
  برای(l = head; l; l = l->next) {
    ثابت نویسه *مقدار; /* to read from */
    نویسه *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    مقدار = &l->data[colon];
    اگر(!*مقدار)
      ادامه;
    ++مقدار;
    store = (نویسه *)CURL_UNCONST(مقدار);

    /* skip leading whitespace */
    curlx_str_passblanks(&مقدار);

    وقتی(*مقدار) {
      صحیح space = 0;
      وقتی(ISBLANK(*مقدار)) {
        مقدار++;
        space++;
      }
      اگر(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        اگر(*مقدار)
          *store++ = ' ';
      }
      وگرنه
        *store++ = *مقدار++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ایستا تهی pair_array_free(ساختار pair *pair_array, size_t رقم_elements)
{
  size_t index;

  برای(index = 0; index != رقم_elements; index++) {
    curlx_dyn_free(&pair_array[index].کلید);
    curlx_dyn_free(&pair_array[index].مقدار);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ایستا تهی dyn_array_free(ساختار dynbuf *db, size_t رقم_elements)
{
  size_t index;

  برای(index = 0; index < رقم_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

ایستا CURLcode split_to_dyn_array(ثابت نویسه *source,
                                   ساختار dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *رقم_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t رقم_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  برای(pos = 0; pos < len; pos++) {
    اگر(source[pos] == SPLIT_BY) {
      اگر(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        اگر(result)
          برو fail;

        segment_length = 0;
        index++;
        اگر(++رقم_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          برو fail;
        }
      }
      start = pos + 1;
    }
    وگرنه {
      segment_length++;
    }
  }

  اگر(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    اگر(!result && ++رقم_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *رقم_splits_out = رقم_splits;
  بازگشت result;
}

ایستا bool is_reserved_char(ثابت نویسه c)
{
  بازگشت (ISALNUM(c) || ISURLPUNTCS(c));
}

ایستا CURLcode uri_encode_path(ساختار Curl_str *original_path,
                                ساختار dynbuf *new_path)
{
  ثابت نویسه *p = curlx_str(original_path);
  size_t i;

  برای(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned نویسه c = p[i];
    اگر(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    وگرنه
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    اگر(result)
      بازگشت result;
  }

  بازگشت CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ایستا CURLcode normalize_query(ثابت نویسه *string, size_t len,
                                ساختار dynbuf *db)
{
  CURLcode result = CURLE_OK;

  وقتی(len && !result) {
    unsigned نویسه in = (unsigned نویسه)*string;
    اگر(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned نویسه)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      اگر(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        ادامه;
      }
    }
    وگرنه {
      string++;
      len--;
    }

    اگر(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    وگرنه اگر(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    وگرنه
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  بازگشت result;
}

ایستا bool should_urlencode(ساختار Curl_str *service_name)
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
    بازگشت FALSE;
  }
  بازگشت TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_کلید_LEN (MAX_SIGV4_LEN + اندازه("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_کلید_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ایستا صحیح compare_header_names(ثابت نویسه *a, ثابت نویسه *b)
{
  ثابت نویسه *colon_a;
  ثابت نویسه *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  صحیح cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  اگر(!cmp)
    بازگشت (صحیح)(len_a - len_b);

  بازگشت cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ایستا CURLcode merge_duplicate_headers(ساختار curl_slist *head)
{
  ساختار curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  وقتی(curr) {
    ساختار curl_slist *next = curr->next;
    اگر(!next)
      بشکن;

    اگر(compare_header_names(curr->data, next->data) == 0) {
      ساختار dynbuf buf;
      ثابت نویسه *colon_next;
      ثابت نویسه *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      اگر(result)
        بازگشت result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      اگر(result)
        بازگشت result;

      result = curlx_dyn_add(&buf, val_next);
      اگر(result)
        بازگشت result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    وگرنه {
      curr = curr->next;
    }
  }

  بازگشت CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ایستا CURLcode make_headers(ساختار Curl_easy *data,
                             نویسه *timestamp,
                             ثابت نویسه *provider1,
                             size_t plen, /* length of provider1 */
                             نویسه **date_header,
                             نویسه *content_sha256_header,
                             ساختار dynbuf *canonical_headers,
                             ساختار dynbuf *signed_headers)
{
  نویسه date_hdr_کلید[DATE_HDR_کلید_LEN];
  نویسه date_full_hdr[DATE_FULL_HDR_LEN];
  ساختار curl_slist *head = پوچ;
  ساختار curl_slist *tmp_head = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ساختار curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_کلید, DATE_HDR_کلید_LEN, "X-%.*s-Date",
                 (صحیح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_کلید[2], provider1, plen);
  date_hdr_کلید[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (صحیح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  اگر(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    نویسه *fullhost = curlx_strdup(data->state.http_host);

    اگر(fullhost)
      head = Curl_slist_append_nodup(پوچ, fullhost);
    اگر(!head) {
      curlx_free(fullhost);
      برو fail;
    }
  }

  اگر(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    اگر(!tmp_head)
      برو fail;
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
  برای(l = data->set.headers; l; l = l->next) {
    نویسه *dupdata;
    ثابت نویسه *ptr;
    ثابت نویسه *sep = strchr(l->data, ':');
    اگر(!sep)
      sep = strchr(l->data, ';');
    اگر(!sep || (*sep == ':' && !*(sep + 1)))
      ادامه;
    برای(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    اگر(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      ادامه;
    dupdata = curlx_strdup(l->data);
    اگر(!dupdata)
      برو fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    اگر(!tmp_head) {
      curlx_free(dupdata);
      برو fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_کلید);
  اگر(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    اگر(!tmp_head)
      برو fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_کلید, timestamp);
    اگر(!*date_header)
      برو fail;
  }
  وگرنه {
    ثابت نویسه *مقدار;
    ثابت نویسه *endp;
    مقدار = strchr(*date_header, ':');
    اگر(!مقدار) {
      *date_header = پوچ;
      برو fail;
    }
    ++مقدار;
    curlx_str_passblanks(&مقدار);
    endp = مقدار;
    وقتی(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    اگر((endp - مقدار) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, مقدار, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    وگرنه
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = پوچ;
  }

  /* alpha-sort by header name in a case sensitive manner */
  انجامبده {
    again = FALSE;
    برای(l = head; l; l = l->next) {
      ساختار curl_slist *next = l->next;

      اگر(next && compare_header_names(l->data, next->data) > 0) {
        نویسه *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } وقتی(again);

  result = merge_duplicate_headers(head);
  اگر(result)
    برو fail;

  برای(l = head; l; l = l->next) {
    نویسه *tmp;

    اگر(curlx_dyn_add(canonical_headers, l->data))
      برو fail;
    اگر(curlx_dyn_add(canonical_headers, "\n"))
      برو fail;

    tmp = strchr(l->data, ':');
    اگر(tmp)
      *tmp = 0;

    اگر(l != head && curlx_dyn_add(signed_headers, ";"))
      برو fail;
    اگر(curlx_dyn_add(signed_headers, l->data))
      برو fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  بازگشت result;
}

#define CONTENT_SHA256_کلید_LEN (MAX_SIGV4_LEN + اندازه("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_کلید_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
ایستا ثابت نویسه *parse_content_sha_hdr(ساختار Curl_easy *data,
                                         ثابت نویسه *provider1,
                                         size_t plen,
                                         size_t *مقدار_len)
{
  نویسه کلید[CONTENT_SHA256_کلید_LEN];
  size_t کلید_len;
  ثابت نویسه *مقدار;
  size_t len;

  کلید_len = curl_msnprintf(کلید, اندازه(کلید), "x-%.*s-content-sha256",
                           (صحیح)plen, provider1);

  مقدار = Curl_checkheaders(data, کلید, کلید_len);
  اگر(!مقدار)
    بازگشت پوچ;

  مقدار = strchr(مقدار, ':');
  اگر(!مقدار)
    بازگشت پوچ;
  ++مقدار;

  curlx_str_passblanks(&مقدار);

  len = strlen(مقدار);
  وقتی(len > 0 && ISBLANK(مقدار[len - 1]))
    --len;

  *مقدار_len = len;
  بازگشت مقدار;
}

ایستا CURLcode calc_payload_hash(ساختار Curl_easy *data,
                                  unsigned نویسه *sha_hash, نویسه *sha_hex)
{
  ثابت نویسه *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  اگر(post_data) {
    اگر(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    وگرنه
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (ثابت unsigned نویسه *)post_data,
                         post_data_len);
  اگر(!result)
    sha256_to_hex(sha_hex, sha_hash);
  بازگشت result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ایستا CURLcode calc_s3_payload_hash(ساختار Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     ثابت نویسه *provider1,
                                     size_t plen,
                                     unsigned نویسه *sha_hash,
                                     نویسه *sha_hex, نویسه *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  اگر(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    اگر(result)
      برو fail;
  }
  وگرنه {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحیح)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  بازگشت result;
}

ایستا صحیح compare_func(ثابت تهی *a, ثابت تهی *b)
{
  ثابت ساختار pair *aa = a;
  ثابت ساختار pair *bb = b;
  ثابت size_t aa_کلید_len = curlx_dyn_len(&aa->کلید);
  ثابت size_t bb_کلید_len = curlx_dyn_len(&bb->کلید);
  ثابت size_t aa_مقدار_len = curlx_dyn_len(&aa->مقدار);
  ثابت size_t bb_مقدار_len = curlx_dyn_len(&bb->مقدار);
  صحیح compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  اگر((aa_کلید_len == 0) && (bb_کلید_len == 0))
    بازگشت 0;
  اگر(aa_کلید_len == 0)
    بازگشت -1;
  اگر(bb_کلید_len == 0)
    بازگشت 1;
  compare = strcmp(curlx_dyn_ptr(&aa->کلید), curlx_dyn_ptr(&bb->کلید));
  اگر(compare) {
    بازگشت compare;
  }

  /* Compare values */
  اگر((aa_مقدار_len == 0) && (bb_مقدار_len == 0))
    بازگشت 0;
  اگر(aa_مقدار_len == 0)
    بازگشت -1;
  اگر(bb_مقدار_len == 0)
    بازگشت 1;
  compare = strcmp(curlx_dyn_ptr(&aa->مقدار), curlx_dyn_ptr(&bb->مقدار));

  بازگشت compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(ثابت نویسه *q, size_t len,
                             ساختار dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ثابت نویسه *q, size_t len,
                             ساختار dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  ساختار Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  اگر(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  وگرنه
    result = curlx_dyn_addn(new_path, q, len);

  اگر(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  بازگشت result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت نویسه *query, ساختار dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت نویسه *query, ساختار dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  ساختار dynbuf query_array[MAX_QUERY_COMPONENTS];
  ساختار pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t رقم_query_components;
  size_t counted_query_components = 0;
  size_t index;

  اگر(!query)
    بازگشت result;

  result = split_to_dyn_array(query, &query_array[0], &رقم_query_components);
  اگر(result) {
    برو fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  برای(index = 0; index < رقم_query_components; index++) {
    ثابت نویسه *in_کلید;
    size_t in_کلید_len;
    ثابت نویسه *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    ثابت نویسه *query_part = curlx_dyn_ptr(&query_array[index]);

    in_کلید = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    اگر(!offset) {
      in_کلید_len = strlen(in_کلید);
    }
    وگرنه {
      in_کلید_len = offset - in_کلید;
    }

    curlx_dyn_init(&encoded_query_array[index].کلید,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].مقدار,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_کلید, in_کلید_len,
                             &encoded_query_array[index].کلید);
    اگر(result) {
      برو fail;
    }

    /* Decode/encode the value if it exists */
    اگر(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_مقدار_len;
      ثابت نویسه *in_مقدار = offset + 1;
      in_مقدار_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_مقدار, in_مقدار_len,
                               &encoded_query_array[index].مقدار);
      اگر(result) {
        برو fail;
      }
    }
    وگرنه {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].مقدار, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].مقدار, "", 1);
    }

    اگر(result) {
      برو fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, رقم_query_components,
        اندازه(ساختار pair), compare_func);

  /* Append the query components together to make a full query string */
  برای(index = 0; index < رقم_query_components; index++) {

    اگر(index)
      result = curlx_dyn_addn(dq, "&", 1);
    اگر(!result) {
      ثابت نویسه *کلید_ptr = curlx_dyn_ptr(&encoded_query_array[index].کلید);
      ثابت نویسه *مقدار_ptr = curlx_dyn_ptr(&encoded_query_array[index].مقدار);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].مقدار);
      اگر(مقدار_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", کلید_ptr, مقدار_ptr);
      }
      وگرنه {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", کلید_ptr);
      }
    }
    اگر(result)
      بشکن;
  }

fail:
  اگر(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], رقم_query_components);
  بازگشت result;
}

ایستا CURLcode parse_sigv4_params(ساختار Curl_easy *data,
                                   ثابت نویسه *hostname,
                                   ساختار Curl_str *provider0,
                                   ساختار Curl_str *provider1,
                                   ساختار Curl_str *region,
                                   ساختار Curl_str *service)
{
  ثابت نویسه *سطر = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  اگر(!سطر || !*سطر)
    سطر = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  اگر(curlx_str_until(&سطر, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  اگر(curlx_str_single(&سطر, ':') ||
     curlx_str_until(&سطر, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  وگرنه اگر(curlx_str_single(&سطر, ':') ||
          curlx_str_until(&سطر, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&سطر, ':') ||
          curlx_str_until(&سطر, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  اگر(!curlx_strlen(service)) {
    ثابت نویسه *p = hostname;
    اگر(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      بازگشت CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (صحیح)curlx_strlen(service), curlx_str(service));

    اگر(!curlx_strlen(region)) {
      اگر(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        بازگشت CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (صحیح)curlx_strlen(region), curlx_str(region));
    }
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode get_payload_hash(ساختار Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 ساختار Curl_str *provider0,
                                 ساختار Curl_str *provider1,
                                 ساختار Curl_str *service,
                                 unsigned نویسه *sha_hash,
                                 نویسه *sha_hex,
                                 نویسه *content_sha256_hdr,
                                 ثابت نویسه **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  اگر(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    اگر(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    وگرنه
      result = calc_payload_hash(data, sha_hash, sha_hex);
    اگر(result)
      بازگشت result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode get_timestamp(نویسه *timestamp, size_t stampsize)
{
  time_t clock;
  ساختار tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    نویسه *force_timestamp = getenv("CURL_FORCETIME");
    اگر(force_timestamp)
      clock = 0;
    وگرنه
      clock = time(پوچ);
  }
#وگرنه
  clock = time(پوچ);
#endif
  result = curlx_gmtime(clock, &tm);
  اگر(result)
    بازگشت result;

  اگر(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت CURLE_OK;
}

ایستا CURLcode make_canonical_request(ساختار Curl_easy *data,
                                       نویسه *timestamp,
                                       ساختار Curl_str *provider1,
                                       ساختار Curl_str *service,
                                       ثابت نویسه *method,
                                       ثابت نویسه *payload_hash,
                                       size_t payload_hash_len,
                                       نویسه **date_header_out,
                                       نویسه *content_sha256_hdr,
                                       ساختار dynbuf *canonical_headers,
                                       ساختار dynbuf *signed_headers,
                                       نویسه **canonical_request_out)
{
  ساختار dynbuf canonical_query;
  ساختار dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  اگر(result)
    برو fail;

  result = canon_query(data->state.up.query, &canonical_query);
  اگر(result)
    برو fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  اگر(result)
    برو fail;

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
                  (صحیح)payload_hash_len, payload_hash);
  اگر(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    برو fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  بازگشت result;
}

ایستا CURLcode make_string_to_sign(ساختار Curl_easy *data,
                                    ساختار Curl_str *provider0,
                                    ساختار Curl_str *region,
                                    ساختار Curl_str *service,
                                    ثابت نویسه *date,
                                    ثابت نویسه *timestamp,
                                    ثابت نویسه *canonical_request,
                                    نویسه **request_type_out,
                                    نویسه **credential_scope_out,
                                    نویسه **str_to_sign_out)
{
  نویسه *request_type;
  نویسه *credential_scope;
  نویسه *str_to_sign;
  unsigned نویسه sha_hash[CURL_SHA256_DIGEST_LENGTH];
  نویسه sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (صحیح)curlx_strlen(provider0),
                               curlx_str(provider0));
  اگر(!request_type)
    بازگشت CURLE_OUT_OF_MEMORY;

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
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(Curl_sha256it(sha_hash, (ثابت unsigned نویسه *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    بازگشت CURLE_OUT_OF_MEMORY;
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
                              (صحیح)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  اگر(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  بازگشت CURLE_OK;
}

ایستا CURLcode sign_and_set_auth_headers(ساختار Curl_easy *data,
                                          ساختار Curl_str *provider0,
                                          ساختار Curl_str *region,
                                          ساختار Curl_str *service,
                                          ثابت نویسه *request_type,
                                          ثابت نویسه *credential_scope,
                                          ثابت نویسه *date,
                                          ثابت نویسه *str_to_sign,
                                          ثابت نویسه *date_header,
                                          ثابت نویسه *content_sha256_hdr,
                                          ساختار dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ثابت نویسه *passwd = Curl_creds_passwd(data->state.creds);
  نویسه *secret = پوچ;
  unsigned نویسه sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned نویسه sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  نویسه sha_hex[SHA256_HEX_LENGTH];
  نویسه *auth_headers = پوچ;
  نویسه *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  اگر(!user)
    بازگشت CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (صحیح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  اگر(!secret)
    برو fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, اندازه(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, اندازه(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, اندازه(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, اندازه(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحیح)curlx_strlen(provider0),
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
  اگر(!auth_headers)
    برو fail;

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
  بازگشت result;
}

CURLcode Curl_output_aws_sigv4(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ساختار Curl_str provider0 = { پوچ, 0 };
  ساختار Curl_str provider1 = { پوچ, 0 };
  ساختار Curl_str region = { پوچ, 0 };
  ساختار Curl_str service = { پوچ, 0 };
  ثابت نویسه *hostname = data->state.origin->hostname;
  نویسه timestamp[TIMESTAMP_SIZE];
  نویسه date[9];
  ساختار dynbuf canonical_headers;
  ساختار dynbuf signed_headers;
  نویسه *date_header = پوچ;
  Curl_HttpReq httpreq;
  ثابت نویسه *method = پوچ;
  ثابت نویسه *payload_hash = پوچ;
  size_t payload_hash_len = 0;
  unsigned نویسه sha_hash[CURL_SHA256_DIGEST_LENGTH];
  نویسه sha_hex[SHA256_HEX_LENGTH];
  نویسه content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  نویسه *canonical_request = پوچ;
  نویسه *request_type = پوچ;
  نویسه *credential_scope = پوچ;
  نویسه *str_to_sign = پوچ;

  اگر(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  اگر(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    بازگشت CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  اگر(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  اگر(!result)
    result = get_timestamp(timestamp, اندازه(timestamp));

  اگر(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  اگر(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, اندازه(date) - 1);
    date[اندازه(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  اگر(!result)
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
  بازگشت result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
