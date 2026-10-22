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

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  افعل {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ثابت unsigned حرف *)(k), \
                         kl,                         \
                         (ثابت unsigned حرف *)(d), \
                         dl, o);                     \
    إذا(result) {                                     \
      اذهب fail;                                     \
    }                                                \
  } طالما(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

بنية pair {
  بنية dynbuf کلید;
  بنية dynbuf قدر;
};

ساكن فراغ sha256_to_hex(حرف *dst, unsigned حرف *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned حرف *)dst, SHA256_HEX_LENGTH);
}

ساكن حرف *find_date_hdr(بنية Curl_easy *data, ثابت حرف *sig_hdr)
{
  حرف *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  إذا(tmp)
    أرجع tmp;
  أرجع Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ساكن فراغ trim_headers(بنية curl_slist *head)
{
  بنية curl_slist *l;
  لكل(l = head; l; l = l->next) {
    ثابت حرف *قدر; /* to read from */
    حرف *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    قدر = &l->data[colon];
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
  size_t index;

  لكل(index = 0; index != عدد_elements; index++) {
    curlx_dyn_free(&pair_array[index].کلید);
    curlx_dyn_free(&pair_array[index].قدر);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ساكن فراغ dyn_array_free(بنية dynbuf *db, size_t عدد_elements)
{
  size_t index;

  لكل(index = 0; index < عدد_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

ساكن CURLcode split_to_dyn_array(ثابت حرف *source,
                                   بنية dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *عدد_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t عدد_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  لكل(pos = 0; pos < len; pos++) {
    إذا(source[pos] == SPLIT_BY) {
      إذا(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        إذا(result)
          اذهب fail;

        segment_length = 0;
        index++;
        إذا(++عدد_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          اذهب fail;
        }
      }
      start = pos + 1;
    }
    وإلا {
      segment_length++;
    }
  }

  إذا(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    إذا(!result && ++عدد_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *عدد_splits_out = عدد_splits;
  أرجع result;
}

ساكن bool is_reserved_char(ثابت حرف c)
{
  أرجع (ISALNUM(c) || ISURLPUNTCS(c));
}

ساكن CURLcode uri_encode_path(بنية Curl_str *original_path,
                                بنية dynbuf *new_path)
{
  ثابت حرف *p = curlx_str(original_path);
  size_t i;

  لكل(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned حرف c = p[i];
    إذا(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    وإلا
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    إذا(result)
      أرجع result;
  }

  أرجع CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ساكن CURLcode normalize_query(ثابت حرف *string, size_t len,
                                بنية dynbuf *db)
{
  CURLcode result = CURLE_OK;

  طالما(len && !result) {
    unsigned حرف in = (unsigned حرف)*string;
    إذا(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned حرف)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      إذا(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        استمر;
      }
    }
    وإلا {
      string++;
      len--;
    }

    إذا(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    وإلا إذا(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    وإلا
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  أرجع result;
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
#define MAX_SIGV4_LEN    64
#define DATE_HDR_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_کلید_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ساكن صحيح compare_header_names(ثابت حرف *a, ثابت حرف *b)
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

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  إذا(!cmp)
    أرجع (صحيح)(len_a - len_b);

  أرجع cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ساكن CURLcode merge_duplicate_headers(بنية curl_slist *head)
{
  بنية curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  طالما(curr) {
    بنية curl_slist *next = curr->next;
    إذا(!next)
      اكسر;

    إذا(compare_header_names(curr->data, next->data) == 0) {
      بنية dynbuf buf;
      ثابت حرف *colon_next;
      ثابت حرف *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      إذا(result)
        أرجع result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      إذا(result)
        أرجع result;

      result = curlx_dyn_add(&buf, val_next);
      إذا(result)
        أرجع result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    وإلا {
      curr = curr->next;
    }
  }

  أرجع CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ساكن CURLcode make_headers(بنية Curl_easy *data,
                             حرف *timestamp,
                             ثابت حرف *provider1,
                             size_t plen, /* length of provider1 */
                             حرف **date_header,
                             حرف *content_sha256_header,
                             بنية dynbuf *canonical_headers,
                             بنية dynbuf *signed_headers)
{
  حرف date_hdr_کلید[DATE_HDR_کلید_LEN];
  حرف date_full_hdr[DATE_FULL_HDR_LEN];
  بنية curl_slist *head = عدم;
  بنية curl_slist *tmp_head = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  بنية curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_کلید, DATE_HDR_کلید_LEN, "X-%.*s-Date",
                 (صحيح)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_کلید[2], provider1, plen);
  date_hdr_کلید[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (صحيح)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  إذا(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    حرف *fullhost = curlx_strdup(data->state.http_host);

    إذا(fullhost)
      head = Curl_slist_append_nodup(عدم, fullhost);
    إذا(!head) {
      curlx_free(fullhost);
      اذهب fail;
    }
  }

  إذا(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    إذا(!tmp_head)
      اذهب fail;
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
  لكل(l = data->set.headers; l; l = l->next) {
    حرف *dupdata;
    ثابت حرف *ptr;
    ثابت حرف *sep = strchr(l->data, ':');
    إذا(!sep)
      sep = strchr(l->data, ';');
    إذا(!sep || (*sep == ':' && !*(sep + 1)))
      استمر;
    لكل(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    إذا(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      استمر;
    dupdata = curlx_strdup(l->data);
    إذا(!dupdata)
      اذهب fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    إذا(!tmp_head) {
      curlx_free(dupdata);
      اذهب fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_کلید);
  إذا(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    إذا(!tmp_head)
      اذهب fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_کلید, timestamp);
    إذا(!*date_header)
      اذهب fail;
  }
  وإلا {
    ثابت حرف *قدر;
    ثابت حرف *endp;
    قدر = strchr(*date_header, ':');
    إذا(!قدر) {
      *date_header = عدم;
      اذهب fail;
    }
    ++قدر;
    curlx_str_passblanks(&قدر);
    endp = قدر;
    طالما(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    إذا((endp - قدر) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, قدر, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    وإلا
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = عدم;
  }

  /* alpha-sort by header name in a case sensitive manner */
  افعل {
    again = FALSE;
    لكل(l = head; l; l = l->next) {
      بنية curl_slist *next = l->next;

      إذا(next && compare_header_names(l->data, next->data) > 0) {
        حرف *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } طالما(again);

  result = merge_duplicate_headers(head);
  إذا(result)
    اذهب fail;

  لكل(l = head; l; l = l->next) {
    حرف *tmp;

    إذا(curlx_dyn_add(canonical_headers, l->data))
      اذهب fail;
    إذا(curlx_dyn_add(canonical_headers, "\n"))
      اذهب fail;

    tmp = strchr(l->data, ':');
    إذا(tmp)
      *tmp = 0;

    إذا(l != head && curlx_dyn_add(signed_headers, ";"))
      اذهب fail;
    إذا(curlx_dyn_add(signed_headers, l->data))
      اذهب fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  أرجع result;
}

#define CONTENT_SHA256_کلید_LEN (MAX_SIGV4_LEN + حجم("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_کلید_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
ساكن ثابت حرف *parse_content_sha_hdr(بنية Curl_easy *data,
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

  قدر = Curl_checkheaders(data, کلید, کلید_len);
  إذا(!قدر)
    أرجع عدم;

  قدر = strchr(قدر, ':');
  إذا(!قدر)
    أرجع عدم;
  ++قدر;

  curlx_str_passblanks(&قدر);

  len = strlen(قدر);
  طالما(len > 0 && ISBLANK(قدر[len - 1]))
    --len;

  *قدر_len = len;
  أرجع قدر;
}

ساكن CURLcode calc_payload_hash(بنية Curl_easy *data,
                                  unsigned حرف *sha_hash, حرف *sha_hex)
{
  ثابت حرف *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  إذا(post_data) {
    إذا(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    وإلا
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (ثابت unsigned حرف *)post_data,
                         post_data_len);
  إذا(!result)
    sha256_to_hex(sha_hex, sha_hash);
  أرجع result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ساكن CURLcode calc_s3_payload_hash(بنية Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     ثابت حرف *provider1,
                                     size_t plen,
                                     unsigned حرف *sha_hash,
                                     حرف *sha_hex, حرف *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  إذا(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    إذا(result)
      اذهب fail;
  }
  وإلا {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (صحيح)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  أرجع result;
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
UNITTEST CURLcode canon_path(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ثابت حرف *q, size_t len,
                             بنية dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  بنية Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  إذا(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  وإلا
    result = curlx_dyn_addn(new_path, q, len);

  إذا(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  أرجع result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq);
UNITTEST CURLcode canon_query(ثابت حرف *query, بنية dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  بنية dynbuf query_array[MAX_QUERY_COMPONENTS];
  بنية pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t عدد_query_components;
  size_t counted_query_components = 0;
  size_t index;

  إذا(!query)
    أرجع result;

  result = split_to_dyn_array(query, &query_array[0], &عدد_query_components);
  إذا(result) {
    اذهب fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  لكل(index = 0; index < عدد_query_components; index++) {
    ثابت حرف *in_کلید;
    size_t in_کلید_len;
    ثابت حرف *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    ثابت حرف *query_part = curlx_dyn_ptr(&query_array[index]);

    in_کلید = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    إذا(!offset) {
      in_کلید_len = strlen(in_کلید);
    }
    وإلا {
      in_کلید_len = offset - in_کلید;
    }

    curlx_dyn_init(&encoded_query_array[index].کلید,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].قدر,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_کلید, in_کلید_len,
                             &encoded_query_array[index].کلید);
    إذا(result) {
      اذهب fail;
    }

    /* Decode/encode the value if it exists */
    إذا(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_قدر_len;
      ثابت حرف *in_قدر = offset + 1;
      in_قدر_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_قدر, in_قدر_len,
                               &encoded_query_array[index].قدر);
      إذا(result) {
        اذهب fail;
      }
    }
    وإلا {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].قدر, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].قدر, "", 1);
    }

    إذا(result) {
      اذهب fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, عدد_query_components,
        حجم(بنية pair), compare_func);

  /* Append the query components together to make a full query string */
  لكل(index = 0; index < عدد_query_components; index++) {

    إذا(index)
      result = curlx_dyn_addn(dq, "&", 1);
    إذا(!result) {
      ثابت حرف *کلید_ptr = curlx_dyn_ptr(&encoded_query_array[index].کلید);
      ثابت حرف *قدر_ptr = curlx_dyn_ptr(&encoded_query_array[index].قدر);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].قدر);
      إذا(قدر_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", کلید_ptr, قدر_ptr);
      }
      وإلا {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", کلید_ptr);
      }
    }
    إذا(result)
      اكسر;
  }

fail:
  إذا(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], عدد_query_components);
  أرجع result;
}

ساكن CURLcode parse_sigv4_params(بنية Curl_easy *data,
                                   ثابت حرف *hostname,
                                   بنية Curl_str *provider0,
                                   بنية Curl_str *provider1,
                                   بنية Curl_str *region,
                                   بنية Curl_str *service)
{
  ثابت حرف *سطر = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  إذا(!سطر || !*سطر)
    سطر = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  إذا(curlx_str_until(&سطر, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
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
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      أرجع CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (صحيح)curlx_strlen(service), curlx_str(service));

    إذا(!curlx_strlen(region)) {
      إذا(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        أرجع CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (صحيح)curlx_strlen(region), curlx_str(region));
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode get_payload_hash(بنية Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 بنية Curl_str *provider0,
                                 بنية Curl_str *provider1,
                                 بنية Curl_str *service,
                                 unsigned حرف *sha_hash,
                                 حرف *sha_hex,
                                 حرف *content_sha256_hdr,
                                 ثابت حرف **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  إذا(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    إذا(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    وإلا
      result = calc_payload_hash(data, sha_hash, sha_hex);
    إذا(result)
      أرجع result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode get_timestamp(حرف *timestamp, size_t stampsize)
{
  time_t clock;
  بنية tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    حرف *force_timestamp = getenv("CURL_FORCETIME");
    إذا(force_timestamp)
      clock = 0;
    وإلا
      clock = time(عدم);
  }
#وإلا
  clock = time(عدم);
#endif
  result = curlx_gmtime(clock, &tm);
  إذا(result)
    أرجع result;

  إذا(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    أرجع CURLE_OUT_OF_MEMORY;

  أرجع CURLE_OK;
}

ساكن CURLcode make_canonical_request(بنية Curl_easy *data,
                                       حرف *timestamp,
                                       بنية Curl_str *provider1,
                                       بنية Curl_str *service,
                                       ثابت حرف *method,
                                       ثابت حرف *payload_hash,
                                       size_t payload_hash_len,
                                       حرف **date_header_out,
                                       حرف *content_sha256_hdr,
                                       بنية dynbuf *canonical_headers,
                                       بنية dynbuf *signed_headers,
                                       حرف **canonical_request_out)
{
  بنية dynbuf canonical_query;
  بنية dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  إذا(result)
    اذهب fail;

  result = canon_query(data->state.up.query, &canonical_query);
  إذا(result)
    اذهب fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  إذا(result)
    اذهب fail;

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
                  (صحيح)payload_hash_len, payload_hash);
  إذا(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  أرجع result;
}

ساكن CURLcode make_string_to_sign(بنية Curl_easy *data,
                                    بنية Curl_str *provider0,
                                    بنية Curl_str *region,
                                    بنية Curl_str *service,
                                    ثابت حرف *date,
                                    ثابت حرف *timestamp,
                                    ثابت حرف *canonical_request,
                                    حرف **request_type_out,
                                    حرف **credential_scope_out,
                                    حرف **str_to_sign_out)
{
  حرف *request_type;
  حرف *credential_scope;
  حرف *str_to_sign;
  unsigned حرف sha_hash[CURL_SHA256_DIGEST_LENGTH];
  حرف sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (صحيح)curlx_strlen(provider0),
                               curlx_str(provider0));
  إذا(!request_type)
    أرجع CURLE_OUT_OF_MEMORY;

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
    أرجع CURLE_OUT_OF_MEMORY;
  }

  إذا(Curl_sha256it(sha_hash, (ثابت unsigned حرف *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_OUT_OF_MEMORY;
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
                              (صحيح)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  إذا(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    أرجع CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  أرجع CURLE_OK;
}

ساكن CURLcode sign_and_set_auth_headers(بنية Curl_easy *data,
                                          بنية Curl_str *provider0,
                                          بنية Curl_str *region,
                                          بنية Curl_str *service,
                                          ثابت حرف *request_type,
                                          ثابت حرف *credential_scope,
                                          ثابت حرف *date,
                                          ثابت حرف *str_to_sign,
                                          ثابت حرف *date_header,
                                          ثابت حرف *content_sha256_hdr,
                                          بنية dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ثابت حرف *passwd = Curl_creds_passwd(data->state.creds);
  حرف *secret = عدم;
  unsigned حرف sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned حرف sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  حرف sha_hex[SHA256_HEX_LENGTH];
  حرف *auth_headers = عدم;
  حرف *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  إذا(!user)
    أرجع CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (صحيح)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  إذا(!secret)
    اذهب fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, حجم(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, حجم(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (صحيح)curlx_strlen(provider0),
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
  إذا(!auth_headers)
    اذهب fail;

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
  أرجع result;
}

CURLcode Curl_output_aws_sigv4(بنية Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  بنية Curl_str provider0 = { عدم, 0 };
  بنية Curl_str provider1 = { عدم, 0 };
  بنية Curl_str region = { عدم, 0 };
  بنية Curl_str service = { عدم, 0 };
  ثابت حرف *hostname = data->state.origin->hostname;
  حرف timestamp[TIMESTAMP_SIZE];
  حرف date[9];
  بنية dynbuf canonical_headers;
  بنية dynbuf signed_headers;
  حرف *date_header = عدم;
  Curl_HttpReq httpreq;
  ثابت حرف *method = عدم;
  ثابت حرف *payload_hash = عدم;
  size_t payload_hash_len = 0;
  unsigned حرف sha_hash[CURL_SHA256_DIGEST_LENGTH];
  حرف sha_hex[SHA256_HEX_LENGTH];
  حرف content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  حرف *canonical_request = عدم;
  حرف *request_type = عدم;
  حرف *credential_scope = عدم;
  حرف *str_to_sign = عدم;

  إذا(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  إذا(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    أرجع CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  إذا(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  إذا(!result)
    result = get_timestamp(timestamp, حجم(timestamp));

  إذا(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  إذا(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, حجم(date) - 1);
    date[حجم(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  إذا(!result)
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
  أرجع result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
