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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "http_aws_sigv4.h"
#அந்தர்பாவ "curl_sha256.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "slist.h"

#அந்தர்பாவ <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  செய் {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (மாறிலி unsigned எழுத்து *)(k), \
                         kl,                         \
                         (மாறிலி unsigned எழுத்து *)(d), \
                         dl, o);                     \
    எனில்(result) {                                     \
      செல் fail;                                     \
    }                                                \
  } போது(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

கட்டமைப்பு pair {
  கட்டமைப்பு dynbuf சாவி;
  கட்டமைப்பு dynbuf மதிப்பு;
};

நிலையான வெறுமை sha256_to_hex(எழுத்து *dst, unsigned எழுத்து *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned எழுத்து *)dst, SHA256_HEX_LENGTH);
}

நிலையான எழுத்து *find_date_hdr(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *sig_hdr)
{
  எழுத்து *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  எனில்(tmp)
    திரும்பு tmp;
  திரும்பு Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
நிலையான வெறுமை trim_headers(கட்டமைப்பு curl_slist *head)
{
  கட்டமைப்பு curl_slist *l;
  க்கு(l = head; l; l = l->next) {
    மாறிலி எழுத்து *மதிப்பு; /* to read from */
    எழுத்து *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    மதிப்பு = &l->data[colon];
    எனில்(!*மதிப்பு)
      தொடர்;
    ++மதிப்பு;
    store = (எழுத்து *)CURL_UNCONST(மதிப்பு);

    /* skip leading whitespace */
    curlx_str_passblanks(&மதிப்பு);

    போது(*மதிப்பு) {
      முழு space = 0;
      போது(ISBLANK(*மதிப்பு)) {
        மதிப்பு++;
        space++;
      }
      எனில்(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        எனில்(*மதிப்பு)
          *store++ = ' ';
      }
      இல்லையெனில்
        *store++ = *மதிப்பு++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
நிலையான வெறுமை pair_array_free(கட்டமைப்பு pair *pair_array, size_t இலக்கம்_elements)
{
  size_t index;

  க்கு(index = 0; index != இலக்கம்_elements; index++) {
    curlx_dyn_free(&pair_array[index].சாவி);
    curlx_dyn_free(&pair_array[index].மதிப்பு);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
நிலையான வெறுமை dyn_array_free(கட்டமைப்பு dynbuf *db, size_t இலக்கம்_elements)
{
  size_t index;

  க்கு(index = 0; index < இலக்கம்_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

நிலையான CURLcode split_to_dyn_array(மாறிலி எழுத்து *source,
                                   கட்டமைப்பு dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *இலக்கம்_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t இலக்கம்_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  க்கு(pos = 0; pos < len; pos++) {
    எனில்(source[pos] == SPLIT_BY) {
      எனில்(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        எனில்(result)
          செல் fail;

        segment_length = 0;
        index++;
        எனில்(++இலக்கம்_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          செல் fail;
        }
      }
      start = pos + 1;
    }
    இல்லையெனில் {
      segment_length++;
    }
  }

  எனில்(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    எனில்(!result && ++இலக்கம்_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *இலக்கம்_splits_out = இலக்கம்_splits;
  திரும்பு result;
}

நிலையான bool is_reserved_char(மாறிலி எழுத்து c)
{
  திரும்பு (ISALNUM(c) || ISURLPUNTCS(c));
}

நிலையான CURLcode uri_encode_path(கட்டமைப்பு Curl_str *original_path,
                                கட்டமைப்பு dynbuf *new_path)
{
  மாறிலி எழுத்து *p = curlx_str(original_path);
  size_t i;

  க்கு(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned எழுத்து c = p[i];
    எனில்(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    இல்லையெனில்
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    எனில்(result)
      திரும்பு result;
  }

  திரும்பு CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
நிலையான CURLcode normalize_query(மாறிலி எழுத்து *string, size_t len,
                                கட்டமைப்பு dynbuf *db)
{
  CURLcode result = CURLE_OK;

  போது(len && !result) {
    unsigned எழுத்து in = (unsigned எழுத்து)*string;
    எனில்(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned எழுத்து)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      எனில்(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        தொடர்;
      }
    }
    இல்லையெனில் {
      string++;
      len--;
    }

    எனில்(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    இல்லையெனில் எனில்(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    இல்லையெனில்
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  திரும்பு result;
}

நிலையான bool should_urlencode(கட்டமைப்பு Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  எனில்(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_சாவி_LEN (MAX_SIGV4_LEN + அளவு("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_சாவி_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
நிலையான முழு compare_header_names(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b)
{
  மாறிலி எழுத்து *colon_a;
  மாறிலி எழுத்து *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  முழு cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  எனில்(!cmp)
    திரும்பு (முழு)(len_a - len_b);

  திரும்பு cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
நிலையான CURLcode merge_duplicate_headers(கட்டமைப்பு curl_slist *head)
{
  கட்டமைப்பு curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  போது(curr) {
    கட்டமைப்பு curl_slist *next = curr->next;
    எனில்(!next)
      நிறுத்து;

    எனில்(compare_header_names(curr->data, next->data) == 0) {
      கட்டமைப்பு dynbuf buf;
      மாறிலி எழுத்து *colon_next;
      மாறிலி எழுத்து *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      எனில்(result)
        திரும்பு result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      எனில்(result)
        திரும்பு result;

      result = curlx_dyn_add(&buf, val_next);
      எனில்(result)
        திரும்பு result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    இல்லையெனில் {
      curr = curr->next;
    }
  }

  திரும்பு CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
நிலையான CURLcode make_headers(கட்டமைப்பு Curl_easy *data,
                             எழுத்து *timestamp,
                             மாறிலி எழுத்து *provider1,
                             size_t plen, /* length of provider1 */
                             எழுத்து **date_header,
                             எழுத்து *content_sha256_header,
                             கட்டமைப்பு dynbuf *canonical_headers,
                             கட்டமைப்பு dynbuf *signed_headers)
{
  எழுத்து date_hdr_சாவி[DATE_HDR_சாவி_LEN];
  எழுத்து date_full_hdr[DATE_FULL_HDR_LEN];
  கட்டமைப்பு curl_slist *head = வெற்று;
  கட்டமைப்பு curl_slist *tmp_head = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  கட்டமைப்பு curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_சாவி, DATE_HDR_சாவி_LEN, "X-%.*s-Date",
                 (முழு)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_சாவி[2], provider1, plen);
  date_hdr_சாவி[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (முழு)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  எனில்(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    எழுத்து *fullhost = curlx_strdup(data->state.http_host);

    எனில்(fullhost)
      head = Curl_slist_append_nodup(வெற்று, fullhost);
    எனில்(!head) {
      curlx_free(fullhost);
      செல் fail;
    }
  }

  எனில்(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    எனில்(!tmp_head)
      செல் fail;
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
  க்கு(l = data->set.headers; l; l = l->next) {
    எழுத்து *dupdata;
    மாறிலி எழுத்து *ptr;
    மாறிலி எழுத்து *sep = strchr(l->data, ':');
    எனில்(!sep)
      sep = strchr(l->data, ';');
    எனில்(!sep || (*sep == ':' && !*(sep + 1)))
      தொடர்;
    க்கு(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    எனில்(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      தொடர்;
    dupdata = curlx_strdup(l->data);
    எனில்(!dupdata)
      செல் fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    எனில்(!tmp_head) {
      curlx_free(dupdata);
      செல் fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_சாவி);
  எனில்(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    எனில்(!tmp_head)
      செல் fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_சாவி, timestamp);
    எனில்(!*date_header)
      செல் fail;
  }
  இல்லையெனில் {
    மாறிலி எழுத்து *மதிப்பு;
    மாறிலி எழுத்து *endp;
    மதிப்பு = strchr(*date_header, ':');
    எனில்(!மதிப்பு) {
      *date_header = வெற்று;
      செல் fail;
    }
    ++மதிப்பு;
    curlx_str_passblanks(&மதிப்பு);
    endp = மதிப்பு;
    போது(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    எனில்((endp - மதிப்பு) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, மதிப்பு, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    இல்லையெனில்
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = வெற்று;
  }

  /* alpha-sort by header name in a case sensitive manner */
  செய் {
    again = FALSE;
    க்கு(l = head; l; l = l->next) {
      கட்டமைப்பு curl_slist *next = l->next;

      எனில்(next && compare_header_names(l->data, next->data) > 0) {
        எழுத்து *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } போது(again);

  result = merge_duplicate_headers(head);
  எனில்(result)
    செல் fail;

  க்கு(l = head; l; l = l->next) {
    எழுத்து *tmp;

    எனில்(curlx_dyn_add(canonical_headers, l->data))
      செல் fail;
    எனில்(curlx_dyn_add(canonical_headers, "\n"))
      செல் fail;

    tmp = strchr(l->data, ':');
    எனில்(tmp)
      *tmp = 0;

    எனில்(l != head && curlx_dyn_add(signed_headers, ";"))
      செல் fail;
    எனில்(curlx_dyn_add(signed_headers, l->data))
      செல் fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  திரும்பு result;
}

#define CONTENT_SHA256_சாவி_LEN (MAX_SIGV4_LEN + அளவு("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_சாவி_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
நிலையான மாறிலி எழுத்து *parse_content_sha_hdr(கட்டமைப்பு Curl_easy *data,
                                         மாறிலி எழுத்து *provider1,
                                         size_t plen,
                                         size_t *மதிப்பு_len)
{
  எழுத்து சாவி[CONTENT_SHA256_சாவி_LEN];
  size_t சாவி_len;
  மாறிலி எழுத்து *மதிப்பு;
  size_t len;

  சாவி_len = curl_msnprintf(சாவி, அளவு(சாவி), "x-%.*s-content-sha256",
                           (முழு)plen, provider1);

  மதிப்பு = Curl_checkheaders(data, சாவி, சாவி_len);
  எனில்(!மதிப்பு)
    திரும்பு வெற்று;

  மதிப்பு = strchr(மதிப்பு, ':');
  எனில்(!மதிப்பு)
    திரும்பு வெற்று;
  ++மதிப்பு;

  curlx_str_passblanks(&மதிப்பு);

  len = strlen(மதிப்பு);
  போது(len > 0 && ISBLANK(மதிப்பு[len - 1]))
    --len;

  *மதிப்பு_len = len;
  திரும்பு மதிப்பு;
}

நிலையான CURLcode calc_payload_hash(கட்டமைப்பு Curl_easy *data,
                                  unsigned எழுத்து *sha_hash, எழுத்து *sha_hex)
{
  மாறிலி எழுத்து *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  எனில்(post_data) {
    எனில்(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    இல்லையெனில்
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (மாறிலி unsigned எழுத்து *)post_data,
                         post_data_len);
  எனில்(!result)
    sha256_to_hex(sha_hex, sha_hash);
  திரும்பு result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

நிலையான CURLcode calc_s3_payload_hash(கட்டமைப்பு Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     மாறிலி எழுத்து *provider1,
                                     size_t plen,
                                     unsigned எழுத்து *sha_hash,
                                     எழுத்து *sha_hex, எழுத்து *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  எனில்(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    எனில்(result)
      செல் fail;
  }
  இல்லையெனில் {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (முழு)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  திரும்பு result;
}

நிலையான முழு compare_func(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
  மாறிலி கட்டமைப்பு pair *aa = a;
  மாறிலி கட்டமைப்பு pair *bb = b;
  மாறிலி size_t aa_சாவி_len = curlx_dyn_len(&aa->சாவி);
  மாறிலி size_t bb_சாவி_len = curlx_dyn_len(&bb->சாவி);
  மாறிலி size_t aa_மதிப்பு_len = curlx_dyn_len(&aa->மதிப்பு);
  மாறிலி size_t bb_மதிப்பு_len = curlx_dyn_len(&bb->மதிப்பு);
  முழு compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  எனில்((aa_சாவி_len == 0) && (bb_சாவி_len == 0))
    திரும்பு 0;
  எனில்(aa_சாவி_len == 0)
    திரும்பு -1;
  எனில்(bb_சாவி_len == 0)
    திரும்பு 1;
  compare = strcmp(curlx_dyn_ptr(&aa->சாவி), curlx_dyn_ptr(&bb->சாவி));
  எனில்(compare) {
    திரும்பு compare;
  }

  /* Compare values */
  எனில்((aa_மதிப்பு_len == 0) && (bb_மதிப்பு_len == 0))
    திரும்பு 0;
  எனில்(aa_மதிப்பு_len == 0)
    திரும்பு -1;
  எனில்(bb_மதிப்பு_len == 0)
    திரும்பு 1;
  compare = strcmp(curlx_dyn_ptr(&aa->மதிப்பு), curlx_dyn_ptr(&bb->மதிப்பு));

  திரும்பு compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(மாறிலி எழுத்து *q, size_t len,
                             கட்டமைப்பு dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(மாறிலி எழுத்து *q, size_t len,
                             கட்டமைப்பு dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  கட்டமைப்பு Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  எனில்(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  இல்லையெனில்
    result = curlx_dyn_addn(new_path, q, len);

  எனில்(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  திரும்பு result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(மாறிலி எழுத்து *query, கட்டமைப்பு dynbuf *dq);
UNITTEST CURLcode canon_query(மாறிலி எழுத்து *query, கட்டமைப்பு dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  கட்டமைப்பு dynbuf query_array[MAX_QUERY_COMPONENTS];
  கட்டமைப்பு pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t இலக்கம்_query_components;
  size_t counted_query_components = 0;
  size_t index;

  எனில்(!query)
    திரும்பு result;

  result = split_to_dyn_array(query, &query_array[0], &இலக்கம்_query_components);
  எனில்(result) {
    செல் fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  க்கு(index = 0; index < இலக்கம்_query_components; index++) {
    மாறிலி எழுத்து *in_சாவி;
    size_t in_சாவி_len;
    மாறிலி எழுத்து *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    மாறிலி எழுத்து *query_part = curlx_dyn_ptr(&query_array[index]);

    in_சாவி = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    எனில்(!offset) {
      in_சாவி_len = strlen(in_சாவி);
    }
    இல்லையெனில் {
      in_சாவி_len = offset - in_சாவி;
    }

    curlx_dyn_init(&encoded_query_array[index].சாவி,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].மதிப்பு,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_சாவி, in_சாவி_len,
                             &encoded_query_array[index].சாவி);
    எனில்(result) {
      செல் fail;
    }

    /* Decode/encode the value if it exists */
    எனில்(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_மதிப்பு_len;
      மாறிலி எழுத்து *in_மதிப்பு = offset + 1;
      in_மதிப்பு_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_மதிப்பு, in_மதிப்பு_len,
                               &encoded_query_array[index].மதிப்பு);
      எனில்(result) {
        செல் fail;
      }
    }
    இல்லையெனில் {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].மதிப்பு, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].மதிப்பு, "", 1);
    }

    எனில்(result) {
      செல் fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, இலக்கம்_query_components,
        அளவு(கட்டமைப்பு pair), compare_func);

  /* Append the query components together to make a full query string */
  க்கு(index = 0; index < இலக்கம்_query_components; index++) {

    எனில்(index)
      result = curlx_dyn_addn(dq, "&", 1);
    எனில்(!result) {
      மாறிலி எழுத்து *சாவி_ptr = curlx_dyn_ptr(&encoded_query_array[index].சாவி);
      மாறிலி எழுத்து *மதிப்பு_ptr = curlx_dyn_ptr(&encoded_query_array[index].மதிப்பு);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].மதிப்பு);
      எனில்(மதிப்பு_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", சாவி_ptr, மதிப்பு_ptr);
      }
      இல்லையெனில் {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", சாவி_ptr);
      }
    }
    எனில்(result)
      நிறுத்து;
  }

fail:
  எனில்(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], இலக்கம்_query_components);
  திரும்பு result;
}

நிலையான CURLcode parse_sigv4_params(கட்டமைப்பு Curl_easy *data,
                                   மாறிலி எழுத்து *hostname,
                                   கட்டமைப்பு Curl_str *provider0,
                                   கட்டமைப்பு Curl_str *provider1,
                                   கட்டமைப்பு Curl_str *region,
                                   கட்டமைப்பு Curl_str *service)
{
  மாறிலி எழுத்து *வரி = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  எனில்(!வரி || !*வரி)
    வரி = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  எனில்(curlx_str_until(&வரி, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  எனில்(curlx_str_single(&வரி, ':') ||
     curlx_str_until(&வரி, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  இல்லையெனில் எனில்(curlx_str_single(&வரி, ':') ||
          curlx_str_until(&வரி, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&வரி, ':') ||
          curlx_str_until(&வரி, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  எனில்(!curlx_strlen(service)) {
    மாறிலி எழுத்து *p = hostname;
    எனில்(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      திரும்பு CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (முழு)curlx_strlen(service), curlx_str(service));

    எனில்(!curlx_strlen(region)) {
      எனில்(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        திரும்பு CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (முழு)curlx_strlen(region), curlx_str(region));
    }
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode get_payload_hash(கட்டமைப்பு Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 கட்டமைப்பு Curl_str *provider0,
                                 கட்டமைப்பு Curl_str *provider1,
                                 கட்டமைப்பு Curl_str *service,
                                 unsigned எழுத்து *sha_hash,
                                 எழுத்து *sha_hex,
                                 எழுத்து *content_sha256_hdr,
                                 மாறிலி எழுத்து **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  எனில்(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    எனில்(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    இல்லையெனில்
      result = calc_payload_hash(data, sha_hash, sha_hex);
    எனில்(result)
      திரும்பு result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode get_timestamp(எழுத்து *timestamp, size_t stampsize)
{
  time_t clock;
  கட்டமைப்பு tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    எழுத்து *force_timestamp = getenv("CURL_FORCETIME");
    எனில்(force_timestamp)
      clock = 0;
    இல்லையெனில்
      clock = time(வெற்று);
  }
#இல்லையெனில்
  clock = time(வெற்று);
#endif
  result = curlx_gmtime(clock, &tm);
  எனில்(result)
    திரும்பு result;

  எனில்(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode make_canonical_request(கட்டமைப்பு Curl_easy *data,
                                       எழுத்து *timestamp,
                                       கட்டமைப்பு Curl_str *provider1,
                                       கட்டமைப்பு Curl_str *service,
                                       மாறிலி எழுத்து *method,
                                       மாறிலி எழுத்து *payload_hash,
                                       size_t payload_hash_len,
                                       எழுத்து **date_header_out,
                                       எழுத்து *content_sha256_hdr,
                                       கட்டமைப்பு dynbuf *canonical_headers,
                                       கட்டமைப்பு dynbuf *signed_headers,
                                       எழுத்து **canonical_request_out)
{
  கட்டமைப்பு dynbuf canonical_query;
  கட்டமைப்பு dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  எனில்(result)
    செல் fail;

  result = canon_query(data->state.up.query, &canonical_query);
  எனில்(result)
    செல் fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  எனில்(result)
    செல் fail;

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
                  (முழு)payload_hash_len, payload_hash);
  எனில்(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    செல் fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  திரும்பு result;
}

நிலையான CURLcode make_string_to_sign(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு Curl_str *provider0,
                                    கட்டமைப்பு Curl_str *region,
                                    கட்டமைப்பு Curl_str *service,
                                    மாறிலி எழுத்து *date,
                                    மாறிலி எழுத்து *timestamp,
                                    மாறிலி எழுத்து *canonical_request,
                                    எழுத்து **request_type_out,
                                    எழுத்து **credential_scope_out,
                                    எழுத்து **str_to_sign_out)
{
  எழுத்து *request_type;
  எழுத்து *credential_scope;
  எழுத்து *str_to_sign;
  unsigned எழுத்து sha_hash[CURL_SHA256_DIGEST_LENGTH];
  எழுத்து sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (முழு)curlx_strlen(provider0),
                               curlx_str(provider0));
  எனில்(!request_type)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (முழு)curlx_strlen(region),
                                   curlx_str(region),
                                   (முழு)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  எனில்(!credential_scope) {
    curlx_free(request_type);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(Curl_sha256it(sha_hash, (மாறிலி unsigned எழுத்து *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    திரும்பு CURLE_OUT_OF_MEMORY;
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
                              (முழு)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  எனில்(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode sign_and_set_auth_headers(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_str *provider0,
                                          கட்டமைப்பு Curl_str *region,
                                          கட்டமைப்பு Curl_str *service,
                                          மாறிலி எழுத்து *request_type,
                                          மாறிலி எழுத்து *credential_scope,
                                          மாறிலி எழுத்து *date,
                                          மாறிலி எழுத்து *str_to_sign,
                                          மாறிலி எழுத்து *date_header,
                                          மாறிலி எழுத்து *content_sha256_hdr,
                                          கட்டமைப்பு dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  மாறிலி எழுத்து *passwd = Curl_creds_passwd(data->state.creds);
  எழுத்து *secret = வெற்று;
  unsigned எழுத்து sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned எழுத்து sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  எழுத்து sha_hex[SHA256_HEX_LENGTH];
  எழுத்து *auth_headers = வெற்று;
  எழுத்து *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  எனில்(!user)
    திரும்பு CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (முழு)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  எனில்(!secret)
    செல் fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, அளவு(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, அளவு(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, அளவு(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, அளவு(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (முழு)curlx_strlen(provider0),
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
  எனில்(!auth_headers)
    செல் fail;

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
  திரும்பு result;
}

CURLcode Curl_output_aws_sigv4(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  கட்டமைப்பு Curl_str provider0 = { வெற்று, 0 };
  கட்டமைப்பு Curl_str provider1 = { வெற்று, 0 };
  கட்டமைப்பு Curl_str region = { வெற்று, 0 };
  கட்டமைப்பு Curl_str service = { வெற்று, 0 };
  மாறிலி எழுத்து *hostname = data->state.origin->hostname;
  எழுத்து timestamp[TIMESTAMP_SIZE];
  எழுத்து date[9];
  கட்டமைப்பு dynbuf canonical_headers;
  கட்டமைப்பு dynbuf signed_headers;
  எழுத்து *date_header = வெற்று;
  Curl_HttpReq httpreq;
  மாறிலி எழுத்து *method = வெற்று;
  மாறிலி எழுத்து *payload_hash = வெற்று;
  size_t payload_hash_len = 0;
  unsigned எழுத்து sha_hash[CURL_SHA256_DIGEST_LENGTH];
  எழுத்து sha_hex[SHA256_HEX_LENGTH];
  எழுத்து content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  எழுத்து *canonical_request = வெற்று;
  எழுத்து *request_type = வெற்று;
  எழுத்து *credential_scope = வெற்று;
  எழுத்து *str_to_sign = வெற்று;

  எனில்(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  எனில்(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    திரும்பு CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  எனில்(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  எனில்(!result)
    result = get_timestamp(timestamp, அளவு(timestamp));

  எனில்(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  எனில்(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, அளவு(date) - 1);
    date[அளவு(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  எனில்(!result)
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
  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
