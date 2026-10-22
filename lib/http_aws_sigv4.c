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
#include "curl_setup.h"

#නම් !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#include "urldata.h"
#include "strcase.h"
#include "curlx/strdup.h"
#include "http_aws_sigv4.h"
#include "curl_sha256.h"
#include "transfer.h"
#include "curl_trc.h"
#include "escape.h"
#include "curlx/strparse.h"
#include "slist.h"

#include <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  do {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (const unsigned var_na *)(k), \
                         kl,                         \
                         (const unsigned var_na *)(d), \
                         dl, o);                     \
    නම්(result) {                                     \
      goto fail;                                     \
    }                                                \
  } අතරතුර(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

sa_nracanaa pair {
  sa_nracanaa dynbuf key;
  sa_nracanaa dynbuf value;
};

static void sha256_to_hex(var_na *dst, unsigned var_na *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned var_na *)dst, SHA256_HEX_LENGTH);
}

static var_na *find_date_hdr(sa_nracanaa Curl_easy *data, const var_na *sig_hdr)
{
  var_na *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  නම්(tmp)
    ආපසු tmp;
  ආපසු Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
static void trim_headers(sa_nracanaa curl_slist *head)
{
  sa_nracanaa curl_slist *l;
  සඳහා(l = head; l; l = l->next) {
    const var_na *value; /* to read from */
    var_na *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    value = &l->data[colon];
    නම්(!*value)
      continue;
    ++value;
    store = (var_na *)CURL_UNCONST(value);

    /* skip leading whitespace */
    curlx_str_passblanks(&value);

    අතරතුර(*value) {
      පූර්ණ_සංඛ්‍යා space = 0;
      අතරතුර(ISBLANK(*value)) {
        value++;
        space++;
      }
      නම්(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        නම්(*value)
          *store++ = ' ';
      }
      නැතහොත්
        *store++ = *value++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
static void pair_array_free(sa_nracanaa pair *pair_array, size_t num_elements)
{
  size_t index;

  සඳහා(index = 0; index != num_elements; index++) {
    curlx_dyn_free(&pair_array[index].key);
    curlx_dyn_free(&pair_array[index].value);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
static void dyn_array_free(sa_nracanaa dynbuf *db, size_t num_elements)
{
  size_t index;

  සඳහා(index = 0; index < num_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

static CURLcode split_to_dyn_array(const var_na *source,
                                   sa_nracanaa dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *num_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t num_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  සඳහා(pos = 0; pos < len; pos++) {
    නම්(source[pos] == SPLIT_BY) {
      නම්(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        නම්(result)
          goto fail;

        segment_length = 0;
        index++;
        නම්(++num_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          goto fail;
        }
      }
      start = pos + 1;
    }
    නැතහොත් {
      segment_length++;
    }
  }

  නම්(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    නම්(!result && ++num_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *num_splits_out = num_splits;
  ආපසු result;
}

static bool is_reserved_char(const var_na c)
{
  ආපසු (ISALNUM(c) || ISURLPUNTCS(c));
}

static CURLcode uri_encode_path(sa_nracanaa Curl_str *original_path,
                                sa_nracanaa dynbuf *new_path)
{
  const var_na *p = curlx_str(original_path);
  size_t i;

  සඳහා(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned var_na c = p[i];
    නම්(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    නැතහොත්
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    නම්(result)
      ආපසු result;
  }

  ආපසු CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
static CURLcode normalize_query(const var_na *string, size_t len,
                                sa_nracanaa dynbuf *db)
{
  CURLcode result = CURLE_OK;

  අතරතුර(len && !result) {
    unsigned var_na in = (unsigned var_na)*string;
    නම්(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned var_na)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      නම්(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        continue;
      }
    }
    නැතහොත් {
      string++;
      len--;
    }

    නම්(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    නැතහොත් නම්(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    නැතහොත්
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  ආපසු result;
}

static bool should_urlencode(sa_nracanaa Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  නම්(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    ආපසු FALSE;
  }
  ආපසු TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_KEY_LEN (MAX_SIGV4_LEN + parimaa_na("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_KEY_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
static පූර්ණ_සංඛ්‍යා compare_header_names(const var_na *a, const var_na *b)
{
  const var_na *colon_a;
  const var_na *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  පූර්ණ_සංඛ්‍යා cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  නම්(!cmp)
    ආපසු (පූර්ණ_සංඛ්‍යා)(len_a - len_b);

  ආපසු cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
static CURLcode merge_duplicate_headers(sa_nracanaa curl_slist *head)
{
  sa_nracanaa curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  අතරතුර(curr) {
    sa_nracanaa curl_slist *next = curr->next;
    නම්(!next)
      bha_nga;

    නම්(compare_header_names(curr->data, next->data) == 0) {
      sa_nracanaa dynbuf buf;
      const var_na *colon_next;
      const var_na *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      නම්(result)
        ආපසු result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      නම්(result)
        ආපසු result;

      result = curlx_dyn_add(&buf, val_next);
      නම්(result)
        ආපසු result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    නැතහොත් {
      curr = curr->next;
    }
  }

  ආපසු CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
static CURLcode make_headers(sa_nracanaa Curl_easy *data,
                             var_na *timestamp,
                             const var_na *provider1,
                             size_t plen, /* length of provider1 */
                             var_na **date_header,
                             var_na *content_sha256_header,
                             sa_nracanaa dynbuf *canonical_headers,
                             sa_nracanaa dynbuf *signed_headers)
{
  var_na date_hdr_key[DATE_HDR_KEY_LEN];
  var_na date_full_hdr[DATE_FULL_HDR_LEN];
  sa_nracanaa curl_slist *head = NULL;
  sa_nracanaa curl_slist *tmp_head = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  sa_nracanaa curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_key, DATE_HDR_KEY_LEN, "X-%.*s-Date",
                 (පූර්ණ_සංඛ්‍යා)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_key[2], provider1, plen);
  date_hdr_key[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (පූර්ණ_සංඛ්‍යා)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  නම්(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    var_na *fullhost = curlx_strdup(data->state.http_host);

    නම්(fullhost)
      head = Curl_slist_append_nodup(NULL, fullhost);
    නම්(!head) {
      curlx_free(fullhost);
      goto fail;
    }
  }

  නම්(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    නම්(!tmp_head)
      goto fail;
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
  සඳහා(l = data->set.headers; l; l = l->next) {
    var_na *dupdata;
    const var_na *ptr;
    const var_na *sep = strchr(l->data, ':');
    නම්(!sep)
      sep = strchr(l->data, ';');
    නම්(!sep || (*sep == ':' && !*(sep + 1)))
      continue;
    සඳහා(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    නම්(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      continue;
    dupdata = curlx_strdup(l->data);
    නම්(!dupdata)
      goto fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    නම්(!tmp_head) {
      curlx_free(dupdata);
      goto fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_key);
  නම්(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    නම්(!tmp_head)
      goto fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_key, timestamp);
    නම්(!*date_header)
      goto fail;
  }
  නැතහොත් {
    const var_na *value;
    const var_na *endp;
    value = strchr(*date_header, ':');
    නම්(!value) {
      *date_header = NULL;
      goto fail;
    }
    ++value;
    curlx_str_passblanks(&value);
    endp = value;
    අතරතුර(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    නම්((endp - value) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, value, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    නැතහොත්
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = NULL;
  }

  /* alpha-sort by header name in a case sensitive manner */
  do {
    again = FALSE;
    සඳහා(l = head; l; l = l->next) {
      sa_nracanaa curl_slist *next = l->next;

      නම්(next && compare_header_names(l->data, next->data) > 0) {
        var_na *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } අතරතුර(again);

  result = merge_duplicate_headers(head);
  නම්(result)
    goto fail;

  සඳහා(l = head; l; l = l->next) {
    var_na *tmp;

    නම්(curlx_dyn_add(canonical_headers, l->data))
      goto fail;
    නම්(curlx_dyn_add(canonical_headers, "\n"))
      goto fail;

    tmp = strchr(l->data, ':');
    නම්(tmp)
      *tmp = 0;

    නම්(l != head && curlx_dyn_add(signed_headers, ";"))
      goto fail;
    නම්(curlx_dyn_add(signed_headers, l->data))
      goto fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  ආපසු result;
}

#define CONTENT_SHA256_KEY_LEN (MAX_SIGV4_LEN + parimaa_na("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_KEY_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
static const var_na *parse_content_sha_hdr(sa_nracanaa Curl_easy *data,
                                         const var_na *provider1,
                                         size_t plen,
                                         size_t *value_len)
{
  var_na key[CONTENT_SHA256_KEY_LEN];
  size_t key_len;
  const var_na *value;
  size_t len;

  key_len = curl_msnprintf(key, parimaa_na(key), "x-%.*s-content-sha256",
                           (පූර්ණ_සංඛ්‍යා)plen, provider1);

  value = Curl_checkheaders(data, key, key_len);
  නම්(!value)
    ආපසු NULL;

  value = strchr(value, ':');
  නම්(!value)
    ආපසු NULL;
  ++value;

  curlx_str_passblanks(&value);

  len = strlen(value);
  අතරතුර(len > 0 && ISBLANK(value[len - 1]))
    --len;

  *value_len = len;
  ආපසු value;
}

static CURLcode calc_payload_hash(sa_nracanaa Curl_easy *data,
                                  unsigned var_na *sha_hash, var_na *sha_hex)
{
  const var_na *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  නම්(post_data) {
    නම්(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    නැතහොත්
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (const unsigned var_na *)post_data,
                         post_data_len);
  නම්(!result)
    sha256_to_hex(sha_hex, sha_hash);
  ආපසු result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

static CURLcode calc_s3_payload_hash(sa_nracanaa Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     const var_na *provider1,
                                     size_t plen,
                                     unsigned var_na *sha_hash,
                                     var_na *sha_hex, var_na *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  නම්(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    නම්(result)
      goto fail;
  }
  නැතහොත් {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (පූර්ණ_සංඛ්‍යා)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  ආපසු result;
}

static පූර්ණ_සංඛ්‍යා compare_func(const void *a, const void *b)
{
  const sa_nracanaa pair *aa = a;
  const sa_nracanaa pair *bb = b;
  const size_t aa_key_len = curlx_dyn_len(&aa->key);
  const size_t bb_key_len = curlx_dyn_len(&bb->key);
  const size_t aa_value_len = curlx_dyn_len(&aa->value);
  const size_t bb_value_len = curlx_dyn_len(&bb->value);
  පූර්ණ_සංඛ්‍යා compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  නම්((aa_key_len == 0) && (bb_key_len == 0))
    ආපසු 0;
  නම්(aa_key_len == 0)
    ආපසු -1;
  නම්(bb_key_len == 0)
    ආපසු 1;
  compare = strcmp(curlx_dyn_ptr(&aa->key), curlx_dyn_ptr(&bb->key));
  නම්(compare) {
    ආපසු compare;
  }

  /* Compare values */
  නම්((aa_value_len == 0) && (bb_value_len == 0))
    ආපසු 0;
  නම්(aa_value_len == 0)
    ආපසු -1;
  නම්(bb_value_len == 0)
    ආපසු 1;
  compare = strcmp(curlx_dyn_ptr(&aa->value), curlx_dyn_ptr(&bb->value));

  ආපසු compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(const var_na *q, size_t len,
                             sa_nracanaa dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(const var_na *q, size_t len,
                             sa_nracanaa dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  sa_nracanaa Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  නම්(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  නැතහොත්
    result = curlx_dyn_addn(new_path, q, len);

  නම්(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  ආපසු result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(const var_na *query, sa_nracanaa dynbuf *dq);
UNITTEST CURLcode canon_query(const var_na *query, sa_nracanaa dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  sa_nracanaa dynbuf query_array[MAX_QUERY_COMPONENTS];
  sa_nracanaa pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t num_query_components;
  size_t counted_query_components = 0;
  size_t index;

  නම්(!query)
    ආපසු result;

  result = split_to_dyn_array(query, &query_array[0], &num_query_components);
  නම්(result) {
    goto fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  සඳහා(index = 0; index < num_query_components; index++) {
    const var_na *in_key;
    size_t in_key_len;
    const var_na *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    const var_na *query_part = curlx_dyn_ptr(&query_array[index]);

    in_key = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    නම්(!offset) {
      in_key_len = strlen(in_key);
    }
    නැතහොත් {
      in_key_len = offset - in_key;
    }

    curlx_dyn_init(&encoded_query_array[index].key,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].value,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_key, in_key_len,
                             &encoded_query_array[index].key);
    නම්(result) {
      goto fail;
    }

    /* Decode/encode the value if it exists */
    නම්(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_value_len;
      const var_na *in_value = offset + 1;
      in_value_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_value, in_value_len,
                               &encoded_query_array[index].value);
      නම්(result) {
        goto fail;
      }
    }
    නැතහොත් {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].value, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].value, "", 1);
    }

    නම්(result) {
      goto fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, num_query_components,
        parimaa_na(sa_nracanaa pair), compare_func);

  /* Append the query components together to make a full query string */
  සඳහා(index = 0; index < num_query_components; index++) {

    නම්(index)
      result = curlx_dyn_addn(dq, "&", 1);
    නම්(!result) {
      const var_na *key_ptr = curlx_dyn_ptr(&encoded_query_array[index].key);
      const var_na *value_ptr = curlx_dyn_ptr(&encoded_query_array[index].value);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].value);
      නම්(value_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", key_ptr, value_ptr);
      }
      නැතහොත් {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", key_ptr);
      }
    }
    නම්(result)
      bha_nga;
  }

fail:
  නම්(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], num_query_components);
  ආපසු result;
}

static CURLcode parse_sigv4_params(sa_nracanaa Curl_easy *data,
                                   const var_na *hostname,
                                   sa_nracanaa Curl_str *provider0,
                                   sa_nracanaa Curl_str *provider1,
                                   sa_nracanaa Curl_str *region,
                                   sa_nracanaa Curl_str *service)
{
  const var_na *line = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  නම්(!line || !*line)
    line = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  නම්(curlx_str_until(&line, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
  }
  නම්(curlx_str_single(&line, ':') ||
     curlx_str_until(&line, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  නැතහොත් නම්(curlx_str_single(&line, ':') ||
          curlx_str_until(&line, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&line, ':') ||
          curlx_str_until(&line, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  නම්(!curlx_strlen(service)) {
    const var_na *p = hostname;
    නම්(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      ආපසු CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (පූර්ණ_සංඛ්‍යා)curlx_strlen(service), curlx_str(service));

    නම්(!curlx_strlen(region)) {
      නම්(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        ආපසු CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (පූර්ණ_සංඛ්‍යා)curlx_strlen(region), curlx_str(region));
    }
  }

  ආපසු CURLE_OK;
}

static CURLcode get_payload_hash(sa_nracanaa Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 sa_nracanaa Curl_str *provider0,
                                 sa_nracanaa Curl_str *provider1,
                                 sa_nracanaa Curl_str *service,
                                 unsigned var_na *sha_hash,
                                 var_na *sha_hex,
                                 var_na *content_sha256_hdr,
                                 const var_na **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  නම්(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    නම්(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    නැතහොත්
      result = calc_payload_hash(data, sha_hash, sha_hex);
    නම්(result)
      ආපසු result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  ආපසු CURLE_OK;
}

static CURLcode get_timestamp(var_na *timestamp, size_t stampsize)
{
  time_t clock;
  sa_nracanaa tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    var_na *force_timestamp = getenv("CURL_FORCETIME");
    නම්(force_timestamp)
      clock = 0;
    නැතහොත්
      clock = time(NULL);
  }
#නැතහොත්
  clock = time(NULL);
#endif
  result = curlx_gmtime(clock, &tm);
  නම්(result)
    ආපසු result;

  නම්(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    ආපසු CURLE_OUT_OF_MEMORY;

  ආපසු CURLE_OK;
}

static CURLcode make_canonical_request(sa_nracanaa Curl_easy *data,
                                       var_na *timestamp,
                                       sa_nracanaa Curl_str *provider1,
                                       sa_nracanaa Curl_str *service,
                                       const var_na *method,
                                       const var_na *payload_hash,
                                       size_t payload_hash_len,
                                       var_na **date_header_out,
                                       var_na *content_sha256_hdr,
                                       sa_nracanaa dynbuf *canonical_headers,
                                       sa_nracanaa dynbuf *signed_headers,
                                       var_na **canonical_request_out)
{
  sa_nracanaa dynbuf canonical_query;
  sa_nracanaa dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  නම්(result)
    goto fail;

  result = canon_query(data->state.up.query, &canonical_query);
  නම්(result)
    goto fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  නම්(result)
    goto fail;

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
                  (පූර්ණ_සංඛ්‍යා)payload_hash_len, payload_hash);
  නම්(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  ආපසු result;
}

static CURLcode make_string_to_sign(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa Curl_str *provider0,
                                    sa_nracanaa Curl_str *region,
                                    sa_nracanaa Curl_str *service,
                                    const var_na *date,
                                    const var_na *timestamp,
                                    const var_na *canonical_request,
                                    var_na **request_type_out,
                                    var_na **credential_scope_out,
                                    var_na **str_to_sign_out)
{
  var_na *request_type;
  var_na *credential_scope;
  var_na *str_to_sign;
  unsigned var_na sha_hash[CURL_SHA256_DIGEST_LENGTH];
  var_na sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (පූර්ණ_සංඛ්‍යා)curlx_strlen(provider0),
                               curlx_str(provider0));
  නම්(!request_type)
    ආපසු CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (පූර්ණ_සංඛ්‍යා)curlx_strlen(region),
                                   curlx_str(region),
                                   (පූර්ණ_සංඛ්‍යා)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  නම්(!credential_scope) {
    curlx_free(request_type);
    ආපසු CURLE_OUT_OF_MEMORY;
  }

  නම්(Curl_sha256it(sha_hash, (const unsigned var_na *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ආපසු CURLE_OUT_OF_MEMORY;
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
                              (පූර්ණ_සංඛ්‍යා)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  නම්(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ආපසු CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  ආපසු CURLE_OK;
}

static CURLcode sign_and_set_auth_headers(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa Curl_str *provider0,
                                          sa_nracanaa Curl_str *region,
                                          sa_nracanaa Curl_str *service,
                                          const var_na *request_type,
                                          const var_na *credential_scope,
                                          const var_na *date,
                                          const var_na *str_to_sign,
                                          const var_na *date_header,
                                          const var_na *content_sha256_hdr,
                                          sa_nracanaa dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  const var_na *passwd = Curl_creds_passwd(data->state.creds);
  var_na *secret = NULL;
  unsigned var_na sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned var_na sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  var_na sha_hex[SHA256_HEX_LENGTH];
  var_na *auth_headers = NULL;
  var_na *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  නම්(!user)
    ආපසු CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (පූර්ණ_සංඛ්‍යා)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  නම්(!secret)
    goto fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, parimaa_na(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, parimaa_na(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, parimaa_na(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, parimaa_na(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (පූර්ණ_සංඛ්‍යා)curlx_strlen(provider0),
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
  නම්(!auth_headers)
    goto fail;

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
  ආපසු result;
}

CURLcode Curl_output_aws_sigv4(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  sa_nracanaa Curl_str provider0 = { NULL, 0 };
  sa_nracanaa Curl_str provider1 = { NULL, 0 };
  sa_nracanaa Curl_str region = { NULL, 0 };
  sa_nracanaa Curl_str service = { NULL, 0 };
  const var_na *hostname = data->state.origin->hostname;
  var_na timestamp[TIMESTAMP_SIZE];
  var_na date[9];
  sa_nracanaa dynbuf canonical_headers;
  sa_nracanaa dynbuf signed_headers;
  var_na *date_header = NULL;
  Curl_HttpReq httpreq;
  const var_na *method = NULL;
  const var_na *payload_hash = NULL;
  size_t payload_hash_len = 0;
  unsigned var_na sha_hash[CURL_SHA256_DIGEST_LENGTH];
  var_na sha_hex[SHA256_HEX_LENGTH];
  var_na content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  var_na *canonical_request = NULL;
  var_na *request_type = NULL;
  var_na *credential_scope = NULL;
  var_na *str_to_sign = NULL;

  නම්(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
  }

  නම්(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    ආපසු CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  නම්(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  නම්(!result)
    result = get_timestamp(timestamp, parimaa_na(timestamp));

  නම්(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  නම්(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, parimaa_na(date) - 1);
    date[parimaa_na(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  නම්(!result)
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
  ආපසු result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
