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

#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

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
                         (const unsigned char *)(k), \
                         kl,                         \
                         (const unsigned char *)(d), \
                         dl, o);                     \
    만약(result) {                                     \
      goto fail;                                     \
    }                                                \
  } 동안(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

struct pair {
  struct dynbuf key;
  struct dynbuf value;
};

static void sha256_to_hex(char *dst, unsigned char *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned char *)dst, SHA256_HEX_LENGTH);
}

static char *find_date_hdr(struct Curl_easy *data, const char *sig_hdr)
{
  char *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  만약(tmp)
    반환 tmp;
  반환 Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
static void trim_headers(struct curl_slist *head)
{
  struct curl_slist *l;
  위해(l = head; l; l = l->next) {
    const char *value; /* to read from */
    char *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    value = &l->data[colon];
    만약(!*value)
      continue;
    ++value;
    store = (char *)CURL_UNCONST(value);

    /* skip leading whitespace */
    curlx_str_passblanks(&value);

    동안(*value) {
      정수 space = 0;
      동안(ISBLANK(*value)) {
        value++;
        space++;
      }
      만약(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        만약(*value)
          *store++ = ' ';
      }
      아니면
        *store++ = *value++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
static void pair_array_free(struct pair *pair_array, size_t num_elements)
{
  size_t index;

  위해(index = 0; index != num_elements; index++) {
    curlx_dyn_free(&pair_array[index].key);
    curlx_dyn_free(&pair_array[index].value);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
static void dyn_array_free(struct dynbuf *db, size_t num_elements)
{
  size_t index;

  위해(index = 0; index < num_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

static CURLcode split_to_dyn_array(const char *source,
                                   struct dynbuf db[MAX_QUERY_COMPONENTS],
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
  위해(pos = 0; pos < len; pos++) {
    만약(source[pos] == SPLIT_BY) {
      만약(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        만약(result)
          goto fail;

        segment_length = 0;
        index++;
        만약(++num_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          goto fail;
        }
      }
      start = pos + 1;
    }
    아니면 {
      segment_length++;
    }
  }

  만약(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    만약(!result && ++num_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *num_splits_out = num_splits;
  반환 result;
}

static bool is_reserved_char(const char c)
{
  반환 (ISALNUM(c) || ISURLPUNTCS(c));
}

static CURLcode uri_encode_path(struct Curl_str *original_path,
                                struct dynbuf *new_path)
{
  const char *p = curlx_str(original_path);
  size_t i;

  위해(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned char c = p[i];
    만약(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    아니면
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    만약(result)
      반환 result;
  }

  반환 CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
static CURLcode normalize_query(const char *string, size_t len,
                                struct dynbuf *db)
{
  CURLcode result = CURLE_OK;

  동안(len && !result) {
    unsigned char in = (unsigned char)*string;
    만약(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned char)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      만약(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        continue;
      }
    }
    아니면 {
      string++;
      len--;
    }

    만약(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    아니면 만약(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    아니면
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  반환 result;
}

static bool should_urlencode(struct Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  만약(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    반환 FALSE;
  }
  반환 TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_KEY_LEN (MAX_SIGV4_LEN + sizeof("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_KEY_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
static 정수 compare_header_names(const char *a, const char *b)
{
  const char *colon_a;
  const char *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  정수 cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  만약(!cmp)
    반환 (정수)(len_a - len_b);

  반환 cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
static CURLcode merge_duplicate_headers(struct curl_slist *head)
{
  struct curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  동안(curr) {
    struct curl_slist *next = curr->next;
    만약(!next)
      break;

    만약(compare_header_names(curr->data, next->data) == 0) {
      struct dynbuf buf;
      const char *colon_next;
      const char *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      만약(result)
        반환 result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      만약(result)
        반환 result;

      result = curlx_dyn_add(&buf, val_next);
      만약(result)
        반환 result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    아니면 {
      curr = curr->next;
    }
  }

  반환 CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
static CURLcode make_headers(struct Curl_easy *data,
                             char *timestamp,
                             const char *provider1,
                             size_t plen, /* length of provider1 */
                             char **date_header,
                             char *content_sha256_header,
                             struct dynbuf *canonical_headers,
                             struct dynbuf *signed_headers)
{
  char date_hdr_key[DATE_HDR_KEY_LEN];
  char date_full_hdr[DATE_FULL_HDR_LEN];
  struct curl_slist *head = NULL;
  struct curl_slist *tmp_head = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  struct curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_key, DATE_HDR_KEY_LEN, "X-%.*s-Date",
                 (정수)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_key[2], provider1, plen);
  date_hdr_key[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (정수)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  만약(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    char *fullhost = curlx_strdup(data->state.http_host);

    만약(fullhost)
      head = Curl_slist_append_nodup(NULL, fullhost);
    만약(!head) {
      curlx_free(fullhost);
      goto fail;
    }
  }

  만약(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    만약(!tmp_head)
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
  위해(l = data->set.headers; l; l = l->next) {
    char *dupdata;
    const char *ptr;
    const char *sep = strchr(l->data, ':');
    만약(!sep)
      sep = strchr(l->data, ';');
    만약(!sep || (*sep == ':' && !*(sep + 1)))
      continue;
    위해(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    만약(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      continue;
    dupdata = curlx_strdup(l->data);
    만약(!dupdata)
      goto fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    만약(!tmp_head) {
      curlx_free(dupdata);
      goto fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_key);
  만약(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    만약(!tmp_head)
      goto fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_key, timestamp);
    만약(!*date_header)
      goto fail;
  }
  아니면 {
    const char *value;
    const char *endp;
    value = strchr(*date_header, ':');
    만약(!value) {
      *date_header = NULL;
      goto fail;
    }
    ++value;
    curlx_str_passblanks(&value);
    endp = value;
    동안(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    만약((endp - value) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, value, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    아니면
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = NULL;
  }

  /* alpha-sort by header name in a case sensitive manner */
  do {
    again = FALSE;
    위해(l = head; l; l = l->next) {
      struct curl_slist *next = l->next;

      만약(next && compare_header_names(l->data, next->data) > 0) {
        char *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } 동안(again);

  result = merge_duplicate_headers(head);
  만약(result)
    goto fail;

  위해(l = head; l; l = l->next) {
    char *tmp;

    만약(curlx_dyn_add(canonical_headers, l->data))
      goto fail;
    만약(curlx_dyn_add(canonical_headers, "\n"))
      goto fail;

    tmp = strchr(l->data, ':');
    만약(tmp)
      *tmp = 0;

    만약(l != head && curlx_dyn_add(signed_headers, ";"))
      goto fail;
    만약(curlx_dyn_add(signed_headers, l->data))
      goto fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  반환 result;
}

#define CONTENT_SHA256_KEY_LEN (MAX_SIGV4_LEN + sizeof("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_KEY_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
static const char *parse_content_sha_hdr(struct Curl_easy *data,
                                         const char *provider1,
                                         size_t plen,
                                         size_t *value_len)
{
  char key[CONTENT_SHA256_KEY_LEN];
  size_t key_len;
  const char *value;
  size_t len;

  key_len = curl_msnprintf(key, sizeof(key), "x-%.*s-content-sha256",
                           (정수)plen, provider1);

  value = Curl_checkheaders(data, key, key_len);
  만약(!value)
    반환 NULL;

  value = strchr(value, ':');
  만약(!value)
    반환 NULL;
  ++value;

  curlx_str_passblanks(&value);

  len = strlen(value);
  동안(len > 0 && ISBLANK(value[len - 1]))
    --len;

  *value_len = len;
  반환 value;
}

static CURLcode calc_payload_hash(struct Curl_easy *data,
                                  unsigned char *sha_hash, char *sha_hex)
{
  const char *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  만약(post_data) {
    만약(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    아니면
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (const unsigned char *)post_data,
                         post_data_len);
  만약(!result)
    sha256_to_hex(sha_hex, sha_hash);
  반환 result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

static CURLcode calc_s3_payload_hash(struct Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     const char *provider1,
                                     size_t plen,
                                     unsigned char *sha_hash,
                                     char *sha_hex, char *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  만약(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    만약(result)
      goto fail;
  }
  아니면 {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (정수)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  반환 result;
}

static 정수 compare_func(const void *a, const void *b)
{
  const struct pair *aa = a;
  const struct pair *bb = b;
  const size_t aa_key_len = curlx_dyn_len(&aa->key);
  const size_t bb_key_len = curlx_dyn_len(&bb->key);
  const size_t aa_value_len = curlx_dyn_len(&aa->value);
  const size_t bb_value_len = curlx_dyn_len(&bb->value);
  정수 compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  만약((aa_key_len == 0) && (bb_key_len == 0))
    반환 0;
  만약(aa_key_len == 0)
    반환 -1;
  만약(bb_key_len == 0)
    반환 1;
  compare = strcmp(curlx_dyn_ptr(&aa->key), curlx_dyn_ptr(&bb->key));
  만약(compare) {
    반환 compare;
  }

  /* Compare values */
  만약((aa_value_len == 0) && (bb_value_len == 0))
    반환 0;
  만약(aa_value_len == 0)
    반환 -1;
  만약(bb_value_len == 0)
    반환 1;
  compare = strcmp(curlx_dyn_ptr(&aa->value), curlx_dyn_ptr(&bb->value));

  반환 compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(const char *q, size_t len,
                             struct dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(const char *q, size_t len,
                             struct dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  struct Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  만약(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  아니면
    result = curlx_dyn_addn(new_path, q, len);

  만약(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  반환 result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(const char *query, struct dynbuf *dq);
UNITTEST CURLcode canon_query(const char *query, struct dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  struct dynbuf query_array[MAX_QUERY_COMPONENTS];
  struct pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t num_query_components;
  size_t counted_query_components = 0;
  size_t index;

  만약(!query)
    반환 result;

  result = split_to_dyn_array(query, &query_array[0], &num_query_components);
  만약(result) {
    goto fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  위해(index = 0; index < num_query_components; index++) {
    const char *in_key;
    size_t in_key_len;
    const char *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    const char *query_part = curlx_dyn_ptr(&query_array[index]);

    in_key = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    만약(!offset) {
      in_key_len = strlen(in_key);
    }
    아니면 {
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
    만약(result) {
      goto fail;
    }

    /* Decode/encode the value if it exists */
    만약(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_value_len;
      const char *in_value = offset + 1;
      in_value_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_value, in_value_len,
                               &encoded_query_array[index].value);
      만약(result) {
        goto fail;
      }
    }
    아니면 {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].value, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].value, "", 1);
    }

    만약(result) {
      goto fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, num_query_components,
        sizeof(struct pair), compare_func);

  /* Append the query components together to make a full query string */
  위해(index = 0; index < num_query_components; index++) {

    만약(index)
      result = curlx_dyn_addn(dq, "&", 1);
    만약(!result) {
      const char *key_ptr = curlx_dyn_ptr(&encoded_query_array[index].key);
      const char *value_ptr = curlx_dyn_ptr(&encoded_query_array[index].value);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].value);
      만약(value_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", key_ptr, value_ptr);
      }
      아니면 {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", key_ptr);
      }
    }
    만약(result)
      break;
  }

fail:
  만약(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], num_query_components);
  반환 result;
}

static CURLcode parse_sigv4_params(struct Curl_easy *data,
                                   const char *hostname,
                                   struct Curl_str *provider0,
                                   struct Curl_str *provider1,
                                   struct Curl_str *region,
                                   struct Curl_str *service)
{
  const char *line = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  만약(!line || !*line)
    line = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  만약(curlx_str_until(&line, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  }
  만약(curlx_str_single(&line, ':') ||
     curlx_str_until(&line, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  아니면 만약(curlx_str_single(&line, ':') ||
          curlx_str_until(&line, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&line, ':') ||
          curlx_str_until(&line, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  만약(!curlx_strlen(service)) {
    const char *p = hostname;
    만약(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      반환 CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (정수)curlx_strlen(service), curlx_str(service));

    만약(!curlx_strlen(region)) {
      만약(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        반환 CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (정수)curlx_strlen(region), curlx_str(region));
    }
  }

  반환 CURLE_OK;
}

static CURLcode get_payload_hash(struct Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 struct Curl_str *provider0,
                                 struct Curl_str *provider1,
                                 struct Curl_str *service,
                                 unsigned char *sha_hash,
                                 char *sha_hex,
                                 char *content_sha256_hdr,
                                 const char **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  만약(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    만약(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    아니면
      result = calc_payload_hash(data, sha_hash, sha_hex);
    만약(result)
      반환 result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  반환 CURLE_OK;
}

static CURLcode get_timestamp(char *timestamp, size_t stampsize)
{
  time_t clock;
  struct tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    char *force_timestamp = getenv("CURL_FORCETIME");
    만약(force_timestamp)
      clock = 0;
    아니면
      clock = time(NULL);
  }
#아니면
  clock = time(NULL);
#endif
  result = curlx_gmtime(clock, &tm);
  만약(result)
    반환 result;

  만약(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    반환 CURLE_OUT_OF_MEMORY;

  반환 CURLE_OK;
}

static CURLcode make_canonical_request(struct Curl_easy *data,
                                       char *timestamp,
                                       struct Curl_str *provider1,
                                       struct Curl_str *service,
                                       const char *method,
                                       const char *payload_hash,
                                       size_t payload_hash_len,
                                       char **date_header_out,
                                       char *content_sha256_hdr,
                                       struct dynbuf *canonical_headers,
                                       struct dynbuf *signed_headers,
                                       char **canonical_request_out)
{
  struct dynbuf canonical_query;
  struct dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  만약(result)
    goto fail;

  result = canon_query(data->state.up.query, &canonical_query);
  만약(result)
    goto fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  만약(result)
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
                  (정수)payload_hash_len, payload_hash);
  만약(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  반환 result;
}

static CURLcode make_string_to_sign(struct Curl_easy *data,
                                    struct Curl_str *provider0,
                                    struct Curl_str *region,
                                    struct Curl_str *service,
                                    const char *date,
                                    const char *timestamp,
                                    const char *canonical_request,
                                    char **request_type_out,
                                    char **credential_scope_out,
                                    char **str_to_sign_out)
{
  char *request_type;
  char *credential_scope;
  char *str_to_sign;
  unsigned char sha_hash[CURL_SHA256_DIGEST_LENGTH];
  char sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (정수)curlx_strlen(provider0),
                               curlx_str(provider0));
  만약(!request_type)
    반환 CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (정수)curlx_strlen(region),
                                   curlx_str(region),
                                   (정수)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  만약(!credential_scope) {
    curlx_free(request_type);
    반환 CURLE_OUT_OF_MEMORY;
  }

  만약(Curl_sha256it(sha_hash, (const unsigned char *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    반환 CURLE_OUT_OF_MEMORY;
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
                              (정수)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  만약(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    반환 CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  반환 CURLE_OK;
}

static CURLcode sign_and_set_auth_headers(struct Curl_easy *data,
                                          struct Curl_str *provider0,
                                          struct Curl_str *region,
                                          struct Curl_str *service,
                                          const char *request_type,
                                          const char *credential_scope,
                                          const char *date,
                                          const char *str_to_sign,
                                          const char *date_header,
                                          const char *content_sha256_hdr,
                                          struct dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  const char *passwd = Curl_creds_passwd(data->state.creds);
  char *secret = NULL;
  unsigned char sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned char sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  char sha_hex[SHA256_HEX_LENGTH];
  char *auth_headers = NULL;
  char *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  만약(!user)
    반환 CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (정수)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  만약(!secret)
    goto fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, sizeof(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, sizeof(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, sizeof(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, sizeof(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (정수)curlx_strlen(provider0),
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
  만약(!auth_headers)
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
  반환 result;
}

CURLcode Curl_output_aws_sigv4(struct Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  struct Curl_str provider0 = { NULL, 0 };
  struct Curl_str provider1 = { NULL, 0 };
  struct Curl_str region = { NULL, 0 };
  struct Curl_str service = { NULL, 0 };
  const char *hostname = data->state.origin->hostname;
  char timestamp[TIMESTAMP_SIZE];
  char date[9];
  struct dynbuf canonical_headers;
  struct dynbuf signed_headers;
  char *date_header = NULL;
  Curl_HttpReq httpreq;
  const char *method = NULL;
  const char *payload_hash = NULL;
  size_t payload_hash_len = 0;
  unsigned char sha_hash[CURL_SHA256_DIGEST_LENGTH];
  char sha_hex[SHA256_HEX_LENGTH];
  char content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  char *canonical_request = NULL;
  char *request_type = NULL;
  char *credential_scope = NULL;
  char *str_to_sign = NULL;

  만약(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  }

  만약(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    반환 CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  만약(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  만약(!result)
    result = get_timestamp(timestamp, sizeof(timestamp));

  만약(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  만약(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, sizeof(date) - 1);
    date[sizeof(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  만약(!result)
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
  반환 result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
