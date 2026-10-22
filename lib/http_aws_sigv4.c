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
#अन्तर्भाव "curl_setup.h"

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "http_aws_sigv4.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "slist.h"

#अन्तर्भाव <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  करेहि {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ध्रुव unsigned वर्ण *)(k), \
                         kl,                         \
                         (ध्रुव unsigned वर्ण *)(d), \
                         dl, o);                     \
    जइ(result) {                                     \
      गच्छ fail;                                     \
    }                                                \
  } जाव(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

संरचना pair {
  संरचना dynbuf कुञ्चिका;
  संरचना dynbuf मानम्;
};

स्थिर शून्य sha256_to_hex(वर्ण *dst, unsigned वर्ण *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned वर्ण *)dst, SHA256_HEX_LENGTH);
}

स्थिर वर्ण *find_date_hdr(संरचना Curl_easy *data, ध्रुव वर्ण *sig_hdr)
{
  वर्ण *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  जइ(tmp)
    प्रत्यागम tmp;
  प्रत्यागम Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
स्थिर शून्य trim_headers(संरचना curl_slist *head)
{
  संरचना curl_slist *l;
  प्रति(l = head; l; l = l->next) {
    ध्रुव वर्ण *मानम्; /* to read from */
    वर्ण *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    मानम् = &l->data[colon];
    जइ(!*मानम्)
      अनुवर्त;
    ++मानम्;
    store = (वर्ण *)CURL_UNCONST(मानम्);

    /* skip leading whitespace */
    curlx_str_passblanks(&मानम्);

    जाव(*मानम्) {
      पूर्णाङ्क space = 0;
      जाव(ISBLANK(*मानम्)) {
        मानम्++;
        space++;
      }
      जइ(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        जइ(*मानम्)
          *store++ = ' ';
      }
      अण्णहा
        *store++ = *मानम्++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
स्थिर शून्य pair_array_free(संरचना pair *pair_array, size_t अङ्कः_elements)
{
  size_t index;

  प्रति(index = 0; index != अङ्कः_elements; index++) {
    curlx_dyn_free(&pair_array[index].कुञ्चिका);
    curlx_dyn_free(&pair_array[index].मानम्);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
स्थिर शून्य dyn_array_free(संरचना dynbuf *db, size_t अङ्कः_elements)
{
  size_t index;

  प्रति(index = 0; index < अङ्कः_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

स्थिर CURLcode split_to_dyn_array(ध्रुव वर्ण *source,
                                   संरचना dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *अङ्कः_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t अङ्कः_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  प्रति(pos = 0; pos < len; pos++) {
    जइ(source[pos] == SPLIT_BY) {
      जइ(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        जइ(result)
          गच्छ fail;

        segment_length = 0;
        index++;
        जइ(++अङ्कः_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          गच्छ fail;
        }
      }
      start = pos + 1;
    }
    अण्णहा {
      segment_length++;
    }
  }

  जइ(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    जइ(!result && ++अङ्कः_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *अङ्कः_splits_out = अङ्कः_splits;
  प्रत्यागम result;
}

स्थिर bool is_reserved_char(ध्रुव वर्ण c)
{
  प्रत्यागम (ISALNUM(c) || ISURLPUNTCS(c));
}

स्थिर CURLcode uri_encode_path(संरचना Curl_str *original_path,
                                संरचना dynbuf *new_path)
{
  ध्रुव वर्ण *p = curlx_str(original_path);
  size_t i;

  प्रति(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned वर्ण c = p[i];
    जइ(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    अण्णहा
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    जइ(result)
      प्रत्यागम result;
  }

  प्रत्यागम CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
स्थिर CURLcode normalize_query(ध्रुव वर्ण *string, size_t len,
                                संरचना dynbuf *db)
{
  CURLcode result = CURLE_OK;

  जाव(len && !result) {
    unsigned वर्ण in = (unsigned वर्ण)*string;
    जइ(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned वर्ण)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      जइ(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        अनुवर्त;
      }
    }
    अण्णहा {
      string++;
      len--;
    }

    जइ(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    अण्णहा जइ(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    अण्णहा
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  प्रत्यागम result;
}

स्थिर bool should_urlencode(संरचना Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  जइ(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_कुञ्चिका_LEN (MAX_SIGV4_LEN + परिमाण("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_कुञ्चिका_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
स्थिर पूर्णाङ्क compare_header_names(ध्रुव वर्ण *a, ध्रुव वर्ण *b)
{
  ध्रुव वर्ण *colon_a;
  ध्रुव वर्ण *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  पूर्णाङ्क cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  जइ(!cmp)
    प्रत्यागम (पूर्णाङ्क)(len_a - len_b);

  प्रत्यागम cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
स्थिर CURLcode merge_duplicate_headers(संरचना curl_slist *head)
{
  संरचना curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  जाव(curr) {
    संरचना curl_slist *next = curr->next;
    जइ(!next)
      भङ्ग;

    जइ(compare_header_names(curr->data, next->data) == 0) {
      संरचना dynbuf buf;
      ध्रुव वर्ण *colon_next;
      ध्रुव वर्ण *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      जइ(result)
        प्रत्यागम result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      जइ(result)
        प्रत्यागम result;

      result = curlx_dyn_add(&buf, val_next);
      जइ(result)
        प्रत्यागम result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    अण्णहा {
      curr = curr->next;
    }
  }

  प्रत्यागम CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
स्थिर CURLcode make_headers(संरचना Curl_easy *data,
                             वर्ण *timestamp,
                             ध्रुव वर्ण *provider1,
                             size_t plen, /* length of provider1 */
                             वर्ण **date_header,
                             वर्ण *content_sha256_header,
                             संरचना dynbuf *canonical_headers,
                             संरचना dynbuf *signed_headers)
{
  वर्ण date_hdr_कुञ्चिका[DATE_HDR_कुञ्चिका_LEN];
  वर्ण date_full_hdr[DATE_FULL_HDR_LEN];
  संरचना curl_slist *head = रिक्त;
  संरचना curl_slist *tmp_head = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  संरचना curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_कुञ्चिका, DATE_HDR_कुञ्चिका_LEN, "X-%.*s-Date",
                 (पूर्णाङ्क)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_कुञ्चिका[2], provider1, plen);
  date_hdr_कुञ्चिका[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (पूर्णाङ्क)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  जइ(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    वर्ण *fullhost = curlx_strdup(data->state.http_host);

    जइ(fullhost)
      head = Curl_slist_append_nodup(रिक्त, fullhost);
    जइ(!head) {
      curlx_free(fullhost);
      गच्छ fail;
    }
  }

  जइ(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    जइ(!tmp_head)
      गच्छ fail;
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
  प्रति(l = data->set.headers; l; l = l->next) {
    वर्ण *dupdata;
    ध्रुव वर्ण *ptr;
    ध्रुव वर्ण *sep = strchr(l->data, ':');
    जइ(!sep)
      sep = strchr(l->data, ';');
    जइ(!sep || (*sep == ':' && !*(sep + 1)))
      अनुवर्त;
    प्रति(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    जइ(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      अनुवर्त;
    dupdata = curlx_strdup(l->data);
    जइ(!dupdata)
      गच्छ fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    जइ(!tmp_head) {
      curlx_free(dupdata);
      गच्छ fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_कुञ्चिका);
  जइ(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    जइ(!tmp_head)
      गच्छ fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_कुञ्चिका, timestamp);
    जइ(!*date_header)
      गच्छ fail;
  }
  अण्णहा {
    ध्रुव वर्ण *मानम्;
    ध्रुव वर्ण *endp;
    मानम् = strchr(*date_header, ':');
    जइ(!मानम्) {
      *date_header = रिक्त;
      गच्छ fail;
    }
    ++मानम्;
    curlx_str_passblanks(&मानम्);
    endp = मानम्;
    जाव(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    जइ((endp - मानम्) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, मानम्, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    अण्णहा
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = रिक्त;
  }

  /* alpha-sort by header name in a case sensitive manner */
  करेहि {
    again = FALSE;
    प्रति(l = head; l; l = l->next) {
      संरचना curl_slist *next = l->next;

      जइ(next && compare_header_names(l->data, next->data) > 0) {
        वर्ण *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } जाव(again);

  result = merge_duplicate_headers(head);
  जइ(result)
    गच्छ fail;

  प्रति(l = head; l; l = l->next) {
    वर्ण *tmp;

    जइ(curlx_dyn_add(canonical_headers, l->data))
      गच्छ fail;
    जइ(curlx_dyn_add(canonical_headers, "\n"))
      गच्छ fail;

    tmp = strchr(l->data, ':');
    जइ(tmp)
      *tmp = 0;

    जइ(l != head && curlx_dyn_add(signed_headers, ";"))
      गच्छ fail;
    जइ(curlx_dyn_add(signed_headers, l->data))
      गच्छ fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  प्रत्यागम result;
}

#define CONTENT_SHA256_कुञ्चिका_LEN (MAX_SIGV4_LEN + परिमाण("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_कुञ्चिका_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
स्थिर ध्रुव वर्ण *parse_content_sha_hdr(संरचना Curl_easy *data,
                                         ध्रुव वर्ण *provider1,
                                         size_t plen,
                                         size_t *मानम्_len)
{
  वर्ण कुञ्चिका[CONTENT_SHA256_कुञ्चिका_LEN];
  size_t कुञ्चिका_len;
  ध्रुव वर्ण *मानम्;
  size_t len;

  कुञ्चिका_len = curl_msnprintf(कुञ्चिका, परिमाण(कुञ्चिका), "x-%.*s-content-sha256",
                           (पूर्णाङ्क)plen, provider1);

  मानम् = Curl_checkheaders(data, कुञ्चिका, कुञ्चिका_len);
  जइ(!मानम्)
    प्रत्यागम रिक्त;

  मानम् = strchr(मानम्, ':');
  जइ(!मानम्)
    प्रत्यागम रिक्त;
  ++मानम्;

  curlx_str_passblanks(&मानम्);

  len = strlen(मानम्);
  जाव(len > 0 && ISBLANK(मानम्[len - 1]))
    --len;

  *मानम्_len = len;
  प्रत्यागम मानम्;
}

स्थिर CURLcode calc_payload_hash(संरचना Curl_easy *data,
                                  unsigned वर्ण *sha_hash, वर्ण *sha_hex)
{
  ध्रुव वर्ण *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  जइ(post_data) {
    जइ(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    अण्णहा
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (ध्रुव unsigned वर्ण *)post_data,
                         post_data_len);
  जइ(!result)
    sha256_to_hex(sha_hex, sha_hash);
  प्रत्यागम result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

स्थिर CURLcode calc_s3_payload_hash(संरचना Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     ध्रुव वर्ण *provider1,
                                     size_t plen,
                                     unsigned वर्ण *sha_hash,
                                     वर्ण *sha_hex, वर्ण *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  जइ(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    जइ(result)
      गच्छ fail;
  }
  अण्णहा {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (पूर्णाङ्क)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  प्रत्यागम result;
}

स्थिर पूर्णाङ्क compare_func(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
  ध्रुव संरचना pair *aa = a;
  ध्रुव संरचना pair *bb = b;
  ध्रुव size_t aa_कुञ्चिका_len = curlx_dyn_len(&aa->कुञ्चिका);
  ध्रुव size_t bb_कुञ्चिका_len = curlx_dyn_len(&bb->कुञ्चिका);
  ध्रुव size_t aa_मानम्_len = curlx_dyn_len(&aa->मानम्);
  ध्रुव size_t bb_मानम्_len = curlx_dyn_len(&bb->मानम्);
  पूर्णाङ्क compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  जइ((aa_कुञ्चिका_len == 0) && (bb_कुञ्चिका_len == 0))
    प्रत्यागम 0;
  जइ(aa_कुञ्चिका_len == 0)
    प्रत्यागम -1;
  जइ(bb_कुञ्चिका_len == 0)
    प्रत्यागम 1;
  compare = strcmp(curlx_dyn_ptr(&aa->कुञ्चिका), curlx_dyn_ptr(&bb->कुञ्चिका));
  जइ(compare) {
    प्रत्यागम compare;
  }

  /* Compare values */
  जइ((aa_मानम्_len == 0) && (bb_मानम्_len == 0))
    प्रत्यागम 0;
  जइ(aa_मानम्_len == 0)
    प्रत्यागम -1;
  जइ(bb_मानम्_len == 0)
    प्रत्यागम 1;
  compare = strcmp(curlx_dyn_ptr(&aa->मानम्), curlx_dyn_ptr(&bb->मानम्));

  प्रत्यागम compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(ध्रुव वर्ण *q, size_t len,
                             संरचना dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ध्रुव वर्ण *q, size_t len,
                             संरचना dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  संरचना Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  जइ(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  अण्णहा
    result = curlx_dyn_addn(new_path, q, len);

  जइ(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  प्रत्यागम result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ध्रुव वर्ण *query, संरचना dynbuf *dq);
UNITTEST CURLcode canon_query(ध्रुव वर्ण *query, संरचना dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  संरचना dynbuf query_array[MAX_QUERY_COMPONENTS];
  संरचना pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t अङ्कः_query_components;
  size_t counted_query_components = 0;
  size_t index;

  जइ(!query)
    प्रत्यागम result;

  result = split_to_dyn_array(query, &query_array[0], &अङ्कः_query_components);
  जइ(result) {
    गच्छ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  प्रति(index = 0; index < अङ्कः_query_components; index++) {
    ध्रुव वर्ण *in_कुञ्चिका;
    size_t in_कुञ्चिका_len;
    ध्रुव वर्ण *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    ध्रुव वर्ण *query_part = curlx_dyn_ptr(&query_array[index]);

    in_कुञ्चिका = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    जइ(!offset) {
      in_कुञ्चिका_len = strlen(in_कुञ्चिका);
    }
    अण्णहा {
      in_कुञ्चिका_len = offset - in_कुञ्चिका;
    }

    curlx_dyn_init(&encoded_query_array[index].कुञ्चिका,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].मानम्,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_कुञ्चिका, in_कुञ्चिका_len,
                             &encoded_query_array[index].कुञ्चिका);
    जइ(result) {
      गच्छ fail;
    }

    /* Decode/encode the value if it exists */
    जइ(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_मानम्_len;
      ध्रुव वर्ण *in_मानम् = offset + 1;
      in_मानम्_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_मानम्, in_मानम्_len,
                               &encoded_query_array[index].मानम्);
      जइ(result) {
        गच्छ fail;
      }
    }
    अण्णहा {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].मानम्, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].मानम्, "", 1);
    }

    जइ(result) {
      गच्छ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, अङ्कः_query_components,
        परिमाण(संरचना pair), compare_func);

  /* Append the query components together to make a full query string */
  प्रति(index = 0; index < अङ्कः_query_components; index++) {

    जइ(index)
      result = curlx_dyn_addn(dq, "&", 1);
    जइ(!result) {
      ध्रुव वर्ण *कुञ्चिका_ptr = curlx_dyn_ptr(&encoded_query_array[index].कुञ्चिका);
      ध्रुव वर्ण *मानम्_ptr = curlx_dyn_ptr(&encoded_query_array[index].मानम्);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].मानम्);
      जइ(मानम्_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", कुञ्चिका_ptr, मानम्_ptr);
      }
      अण्णहा {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", कुञ्चिका_ptr);
      }
    }
    जइ(result)
      भङ्ग;
  }

fail:
  जइ(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], अङ्कः_query_components);
  प्रत्यागम result;
}

स्थिर CURLcode parse_sigv4_params(संरचना Curl_easy *data,
                                   ध्रुव वर्ण *hostname,
                                   संरचना Curl_str *provider0,
                                   संरचना Curl_str *provider1,
                                   संरचना Curl_str *region,
                                   संरचना Curl_str *service)
{
  ध्रुव वर्ण *पङ्क्तिः = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  जइ(!पङ्क्तिः || !*पङ्क्तिः)
    पङ्क्तिः = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  जइ(curlx_str_until(&पङ्क्तिः, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }
  जइ(curlx_str_single(&पङ्क्तिः, ':') ||
     curlx_str_until(&पङ्क्तिः, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  अण्णहा जइ(curlx_str_single(&पङ्क्तिः, ':') ||
          curlx_str_until(&पङ्क्तिः, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&पङ्क्तिः, ':') ||
          curlx_str_until(&पङ्क्तिः, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  जइ(!curlx_strlen(service)) {
    ध्रुव वर्ण *p = hostname;
    जइ(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      प्रत्यागम CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (पूर्णाङ्क)curlx_strlen(service), curlx_str(service));

    जइ(!curlx_strlen(region)) {
      जइ(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        प्रत्यागम CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (पूर्णाङ्क)curlx_strlen(region), curlx_str(region));
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode get_payload_hash(संरचना Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 संरचना Curl_str *provider0,
                                 संरचना Curl_str *provider1,
                                 संरचना Curl_str *service,
                                 unsigned वर्ण *sha_hash,
                                 वर्ण *sha_hex,
                                 वर्ण *content_sha256_hdr,
                                 ध्रुव वर्ण **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  जइ(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    जइ(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    अण्णहा
      result = calc_payload_hash(data, sha_hash, sha_hex);
    जइ(result)
      प्रत्यागम result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode get_timestamp(वर्ण *timestamp, size_t stampsize)
{
  time_t clock;
  संरचना tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    वर्ण *force_timestamp = getenv("CURL_FORCETIME");
    जइ(force_timestamp)
      clock = 0;
    अण्णहा
      clock = time(रिक्त);
  }
#अण्णहा
  clock = time(रिक्त);
#endif
  result = curlx_gmtime(clock, &tm);
  जइ(result)
    प्रत्यागम result;

  जइ(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode make_canonical_request(संरचना Curl_easy *data,
                                       वर्ण *timestamp,
                                       संरचना Curl_str *provider1,
                                       संरचना Curl_str *service,
                                       ध्रुव वर्ण *method,
                                       ध्रुव वर्ण *payload_hash,
                                       size_t payload_hash_len,
                                       वर्ण **date_header_out,
                                       वर्ण *content_sha256_hdr,
                                       संरचना dynbuf *canonical_headers,
                                       संरचना dynbuf *signed_headers,
                                       वर्ण **canonical_request_out)
{
  संरचना dynbuf canonical_query;
  संरचना dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  जइ(result)
    गच्छ fail;

  result = canon_query(data->state.up.query, &canonical_query);
  जइ(result)
    गच्छ fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  जइ(result)
    गच्छ fail;

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
                  (पूर्णाङ्क)payload_hash_len, payload_hash);
  जइ(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  प्रत्यागम result;
}

स्थिर CURLcode make_string_to_sign(संरचना Curl_easy *data,
                                    संरचना Curl_str *provider0,
                                    संरचना Curl_str *region,
                                    संरचना Curl_str *service,
                                    ध्रुव वर्ण *date,
                                    ध्रुव वर्ण *timestamp,
                                    ध्रुव वर्ण *canonical_request,
                                    वर्ण **request_type_out,
                                    वर्ण **credential_scope_out,
                                    वर्ण **str_to_sign_out)
{
  वर्ण *request_type;
  वर्ण *credential_scope;
  वर्ण *str_to_sign;
  unsigned वर्ण sha_hash[CURL_SHA256_DIGEST_LENGTH];
  वर्ण sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (पूर्णाङ्क)curlx_strlen(provider0),
                               curlx_str(provider0));
  जइ(!request_type)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (पूर्णाङ्क)curlx_strlen(region),
                                   curlx_str(region),
                                   (पूर्णाङ्क)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  जइ(!credential_scope) {
    curlx_free(request_type);
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  जइ(Curl_sha256it(sha_hash, (ध्रुव unsigned वर्ण *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    प्रत्यागम CURLE_OUT_OF_MEMORY;
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
                              (पूर्णाङ्क)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  जइ(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode sign_and_set_auth_headers(संरचना Curl_easy *data,
                                          संरचना Curl_str *provider0,
                                          संरचना Curl_str *region,
                                          संरचना Curl_str *service,
                                          ध्रुव वर्ण *request_type,
                                          ध्रुव वर्ण *credential_scope,
                                          ध्रुव वर्ण *date,
                                          ध्रुव वर्ण *str_to_sign,
                                          ध्रुव वर्ण *date_header,
                                          ध्रुव वर्ण *content_sha256_hdr,
                                          संरचना dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ध्रुव वर्ण *passwd = Curl_creds_passwd(data->state.creds);
  वर्ण *secret = रिक्त;
  unsigned वर्ण sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned वर्ण sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  वर्ण sha_hex[SHA256_HEX_LENGTH];
  वर्ण *auth_headers = रिक्त;
  वर्ण *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  जइ(!user)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (पूर्णाङ्क)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  जइ(!secret)
    गच्छ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, परिमाण(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, परिमाण(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, परिमाण(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, परिमाण(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (पूर्णाङ्क)curlx_strlen(provider0),
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
  जइ(!auth_headers)
    गच्छ fail;

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
  प्रत्यागम result;
}

CURLcode Curl_output_aws_sigv4(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  संरचना Curl_str provider0 = { रिक्त, 0 };
  संरचना Curl_str provider1 = { रिक्त, 0 };
  संरचना Curl_str region = { रिक्त, 0 };
  संरचना Curl_str service = { रिक्त, 0 };
  ध्रुव वर्ण *hostname = data->state.origin->hostname;
  वर्ण timestamp[TIMESTAMP_SIZE];
  वर्ण date[9];
  संरचना dynbuf canonical_headers;
  संरचना dynbuf signed_headers;
  वर्ण *date_header = रिक्त;
  Curl_HttpReq httpreq;
  ध्रुव वर्ण *method = रिक्त;
  ध्रुव वर्ण *payload_hash = रिक्त;
  size_t payload_hash_len = 0;
  unsigned वर्ण sha_hash[CURL_SHA256_DIGEST_LENGTH];
  वर्ण sha_hex[SHA256_HEX_LENGTH];
  वर्ण content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  वर्ण *canonical_request = रिक्त;
  वर्ण *request_type = रिक्त;
  वर्ण *credential_scope = रिक्त;
  वर्ण *str_to_sign = रिक्त;

  जइ(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }

  जइ(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    प्रत्यागम CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  जइ(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  जइ(!result)
    result = get_timestamp(timestamp, परिमाण(timestamp));

  जइ(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  जइ(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, परिमाण(date) - 1);
    date[परिमाण(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  जइ(!result)
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
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
