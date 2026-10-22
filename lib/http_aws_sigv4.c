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

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

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
  करा {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (स्थिरांक अचिन्हित अक्षर *)(k), \
                         kl,                         \
                         (स्थिरांक अचिन्हित अक्षर *)(d), \
                         dl, o);                     \
    जर(result) {                                     \
      जा fail;                                     \
    }                                                \
  } पर्यंत(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

संरचना pair {
  संरचना dynbuf किल्ली;
  संरचना dynbuf मूल्य;
};

स्थिर शून्य sha256_to_hex(अक्षर *dst, अचिन्हित अक्षर *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (अचिन्हित अक्षर *)dst, SHA256_HEX_LENGTH);
}

स्थिर अक्षर *find_date_hdr(संरचना Curl_easy *data, स्थिरांक अक्षर *sig_hdr)
{
  अक्षर *tmp = Curl_checkheaders(data, sig_hdr, म_माप(sig_hdr));

  जर(tmp)
    परत tmp;
  परत Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
स्थिर शून्य trim_headers(संरचना curl_slist *head)
{
  संरचना curl_slist *l;
  साठी(l = head; l; l = l->next) {
    स्थिरांक अक्षर *मूल्य; /* to read from */
    अक्षर *store;
    माप_प्रकार colon = म_खोज(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    मूल्य = &l->data[colon];
    जर(!*मूल्य)
      सुरूठेवा;
    ++मूल्य;
    store = (अक्षर *)CURL_UNCONST(मूल्य);

    /* skip leading whitespace */
    curlx_str_passblanks(&मूल्य);

    पर्यंत(*मूल्य) {
      पूर्णांक space = 0;
      पर्यंत(ISBLANK(*मूल्य)) {
        मूल्य++;
        space++;
      }
      जर(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        जर(*मूल्य)
          *store++ = ' ';
      }
      अन्यथा
        *store++ = *मूल्य++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
स्थिर शून्य pair_array_free(संरचना pair *pair_array, माप_प्रकार अंक_elements)
{
  माप_प्रकार index;

  साठी(index = 0; index != अंक_elements; index++) {
    curlx_dyn_free(&pair_array[index].किल्ली);
    curlx_dyn_free(&pair_array[index].मूल्य);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
स्थिर शून्य dyn_array_free(संरचना dynbuf *db, माप_प्रकार अंक_elements)
{
  माप_प्रकार index;

  साठी(index = 0; index < अंक_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

स्थिर CURLcode split_to_dyn_array(स्थिरांक अक्षर *source,
                                   संरचना dynbuf db[MAX_QUERY_COMPONENTS],
                                   माप_प्रकार *अंक_splits_out)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार len = म_माप(source);
  माप_प्रकार pos;         /* Position in result buffer */
  माप_प्रकार start = 0;   /* Start of current segment */
  माप_प्रकार segment_length = 0;
  माप_प्रकार index = 0;
  माप_प्रकार अंक_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  साठी(pos = 0; pos < len; pos++) {
    जर(source[pos] == SPLIT_BY) {
      जर(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        जर(result)
          जा fail;

        segment_length = 0;
        index++;
        जर(++अंक_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          जा fail;
        }
      }
      start = pos + 1;
    }
    अन्यथा {
      segment_length++;
    }
  }

  जर(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    जर(!result && ++अंक_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *अंक_splits_out = अंक_splits;
  परत result;
}

स्थिर bool is_reserved_char(स्थिरांक अक्षर c)
{
  परत (ISALNUM(c) || ISURLPUNTCS(c));
}

स्थिर CURLcode uri_encode_path(संरचना Curl_str *original_path,
                                संरचना dynbuf *new_path)
{
  स्थिरांक अक्षर *p = curlx_str(original_path);
  माप_प्रकार i;

  साठी(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    अचिन्हित अक्षर c = p[i];
    जर(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    अन्यथा
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    जर(result)
      परत result;
  }

  परत CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
स्थिर CURLcode normalize_query(स्थिरांक अक्षर *string, माप_प्रकार len,
                                संरचना dynbuf *db)
{
  CURLcode result = CURLE_OK;

  पर्यंत(len && !result) {
    अचिन्हित अक्षर in = (अचिन्हित अक्षर)*string;
    जर(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (अचिन्हित अक्षर)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      जर(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        सुरूठेवा;
      }
    }
    अन्यथा {
      string++;
      len--;
    }

    जर(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    अन्यथा जर(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    अन्यथा
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  परत result;
}

स्थिर bool should_urlencode(संरचना Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  जर(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    परत FALSE;
  }
  परत TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_किल्ली_LEN (MAX_SIGV4_LEN + आकार("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_किल्ली_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
स्थिर पूर्णांक compare_header_names(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b)
{
  स्थिरांक अक्षर *colon_a;
  स्थिरांक अक्षर *colon_b;
  माप_प्रकार len_a;
  माप_प्रकार len_b;
  माप_प्रकार min_len;
  पूर्णांक cmp;

  colon_a = म_अक्षर(a, ':');
  colon_b = म_अक्षर(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (माप_प्रकार)(colon_a - a) : म_माप(a);
  len_b = colon_b ? (माप_प्रकार)(colon_b - b) : म_माप(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = म_भेदन(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  जर(!cmp)
    परत (पूर्णांक)(len_a - len_b);

  परत cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
स्थिर CURLcode merge_duplicate_headers(संरचना curl_slist *head)
{
  संरचना curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  पर्यंत(curr) {
    संरचना curl_slist *next = curr->next;
    जर(!next)
      थांबा;

    जर(compare_header_names(curr->data, next->data) == 0) {
      संरचना dynbuf buf;
      स्थिरांक अक्षर *colon_next;
      स्थिरांक अक्षर *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      जर(result)
        परत result;

      colon_next = म_अक्षर(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      जर(result)
        परत result;

      result = curlx_dyn_add(&buf, val_next);
      जर(result)
        परत result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    अन्यथा {
      curr = curr->next;
    }
  }

  परत CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
स्थिर CURLcode make_headers(संरचना Curl_easy *data,
                             अक्षर *timestamp,
                             स्थिरांक अक्षर *provider1,
                             माप_प्रकार plen, /* length of provider1 */
                             अक्षर **date_header,
                             अक्षर *content_sha256_header,
                             संरचना dynbuf *canonical_headers,
                             संरचना dynbuf *signed_headers)
{
  अक्षर date_hdr_किल्ली[DATE_HDR_किल्ली_LEN];
  अक्षर date_full_hdr[DATE_FULL_HDR_LEN];
  संरचना curl_slist *head = रिक्त;
  संरचना curl_slist *tmp_head = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  संरचना curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_किल्ली, DATE_HDR_किल्ली_LEN, "X-%.*s-Date",
                 (पूर्णांक)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_किल्ली[2], provider1, plen);
  date_hdr_किल्ली[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (पूर्णांक)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  जर(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    अक्षर *fullhost = curlx_strdup(data->state.http_host);

    जर(fullhost)
      head = Curl_slist_append_nodup(रिक्त, fullhost);
    जर(!head) {
      curlx_free(fullhost);
      जा fail;
    }
  }

  जर(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    जर(!tmp_head)
      जा fail;
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
  साठी(l = data->set.headers; l; l = l->next) {
    अक्षर *dupdata;
    स्थिरांक अक्षर *ptr;
    स्थिरांक अक्षर *sep = म_अक्षर(l->data, ':');
    जर(!sep)
      sep = म_अक्षर(l->data, ';');
    जर(!sep || (*sep == ':' && !*(sep + 1)))
      सुरूठेवा;
    साठी(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    जर(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      सुरूठेवा;
    dupdata = curlx_strdup(l->data);
    जर(!dupdata)
      जा fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    जर(!tmp_head) {
      curlx_free(dupdata);
      जा fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_किल्ली);
  जर(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    जर(!tmp_head)
      जा fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_किल्ली, timestamp);
    जर(!*date_header)
      जा fail;
  }
  अन्यथा {
    स्थिरांक अक्षर *मूल्य;
    स्थिरांक अक्षर *endp;
    मूल्य = म_अक्षर(*date_header, ':');
    जर(!मूल्य) {
      *date_header = रिक्त;
      जा fail;
    }
    ++मूल्य;
    curlx_str_passblanks(&मूल्य);
    endp = मूल्य;
    पर्यंत(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    जर((endp - मूल्य) == TIMESTAMP_SIZE - 1) {
      स_नकल(timestamp, मूल्य, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    अन्यथा
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = रिक्त;
  }

  /* alpha-sort by header name in a case sensitive manner */
  करा {
    again = FALSE;
    साठी(l = head; l; l = l->next) {
      संरचना curl_slist *next = l->next;

      जर(next && compare_header_names(l->data, next->data) > 0) {
        अक्षर *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } पर्यंत(again);

  result = merge_duplicate_headers(head);
  जर(result)
    जा fail;

  साठी(l = head; l; l = l->next) {
    अक्षर *tmp;

    जर(curlx_dyn_add(canonical_headers, l->data))
      जा fail;
    जर(curlx_dyn_add(canonical_headers, "\n"))
      जा fail;

    tmp = म_अक्षर(l->data, ':');
    जर(tmp)
      *tmp = 0;

    जर(l != head && curlx_dyn_add(signed_headers, ";"))
      जा fail;
    जर(curlx_dyn_add(signed_headers, l->data))
      जा fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  परत result;
}

#define CONTENT_SHA256_किल्ली_LEN (MAX_SIGV4_LEN + आकार("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_किल्ली_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
स्थिर स्थिरांक अक्षर *parse_content_sha_hdr(संरचना Curl_easy *data,
                                         स्थिरांक अक्षर *provider1,
                                         माप_प्रकार plen,
                                         माप_प्रकार *मूल्य_len)
{
  अक्षर किल्ली[CONTENT_SHA256_किल्ली_LEN];
  माप_प्रकार किल्ली_len;
  स्थिरांक अक्षर *मूल्य;
  माप_प्रकार len;

  किल्ली_len = curl_msnprintf(किल्ली, आकार(किल्ली), "x-%.*s-content-sha256",
                           (पूर्णांक)plen, provider1);

  मूल्य = Curl_checkheaders(data, किल्ली, किल्ली_len);
  जर(!मूल्य)
    परत रिक्त;

  मूल्य = म_अक्षर(मूल्य, ':');
  जर(!मूल्य)
    परत रिक्त;
  ++मूल्य;

  curlx_str_passblanks(&मूल्य);

  len = म_माप(मूल्य);
  पर्यंत(len > 0 && ISBLANK(मूल्य[len - 1]))
    --len;

  *मूल्य_len = len;
  परत मूल्य;
}

स्थिर CURLcode calc_payload_hash(संरचना Curl_easy *data,
                                  अचिन्हित अक्षर *sha_hash, अक्षर *sha_hex)
{
  स्थिरांक अक्षर *post_data = data->set.postfields;
  माप_प्रकार post_data_len = 0;
  CURLcode result;

  जर(post_data) {
    जर(data->set.postfieldsize < 0)
      post_data_len = म_माप(post_data);
    अन्यथा
      post_data_len = (माप_प्रकार)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (स्थिरांक अचिन्हित अक्षर *)post_data,
                         post_data_len);
  जर(!result)
    sha256_to_hex(sha_hex, sha_hash);
  परत result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

स्थिर CURLcode calc_s3_payload_hash(संरचना Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     स्थिरांक अक्षर *provider1,
                                     माप_प्रकार plen,
                                     अचिन्हित अक्षर *sha_hash,
                                     अक्षर *sha_hex, अक्षर *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  जर(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    जर(result)
      जा fail;
  }
  अन्यथा {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    माप_प्रकार len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    स_नकल(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (पूर्णांक)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  परत result;
}

स्थिर पूर्णांक compare_func(स्थिरांक शून्य *a, स्थिरांक शून्य *b)
{
  स्थिरांक संरचना pair *aa = a;
  स्थिरांक संरचना pair *bb = b;
  स्थिरांक माप_प्रकार aa_किल्ली_len = curlx_dyn_len(&aa->किल्ली);
  स्थिरांक माप_प्रकार bb_किल्ली_len = curlx_dyn_len(&bb->किल्ली);
  स्थिरांक माप_प्रकार aa_मूल्य_len = curlx_dyn_len(&aa->मूल्य);
  स्थिरांक माप_प्रकार bb_मूल्य_len = curlx_dyn_len(&bb->मूल्य);
  पूर्णांक compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  जर((aa_किल्ली_len == 0) && (bb_किल्ली_len == 0))
    परत 0;
  जर(aa_किल्ली_len == 0)
    परत -1;
  जर(bb_किल्ली_len == 0)
    परत 1;
  compare = म_भेद(curlx_dyn_ptr(&aa->किल्ली), curlx_dyn_ptr(&bb->किल्ली));
  जर(compare) {
    परत compare;
  }

  /* Compare values */
  जर((aa_मूल्य_len == 0) && (bb_मूल्य_len == 0))
    परत 0;
  जर(aa_मूल्य_len == 0)
    परत -1;
  जर(bb_मूल्य_len == 0)
    परत 1;
  compare = म_भेद(curlx_dyn_ptr(&aa->मूल्य), curlx_dyn_ptr(&bb->मूल्य));

  परत compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(स्थिरांक अक्षर *q, माप_प्रकार len,
                             संरचना dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(स्थिरांक अक्षर *q, माप_प्रकार len,
                             संरचना dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  संरचना Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  जर(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  अन्यथा
    result = curlx_dyn_addn(new_path, q, len);

  जर(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  परत result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(स्थिरांक अक्षर *query, संरचना dynbuf *dq);
UNITTEST CURLcode canon_query(स्थिरांक अक्षर *query, संरचना dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  संरचना dynbuf query_array[MAX_QUERY_COMPONENTS];
  संरचना pair encoded_query_array[MAX_QUERY_COMPONENTS];
  माप_प्रकार अंक_query_components;
  माप_प्रकार counted_query_components = 0;
  माप_प्रकार index;

  जर(!query)
    परत result;

  result = split_to_dyn_array(query, &query_array[0], &अंक_query_components);
  जर(result) {
    जा fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  साठी(index = 0; index < अंक_query_components; index++) {
    स्थिरांक अक्षर *in_किल्ली;
    माप_प्रकार in_किल्ली_len;
    स्थिरांक अक्षर *offset;
    माप_प्रकार query_part_len = curlx_dyn_len(&query_array[index]);
    स्थिरांक अक्षर *query_part = curlx_dyn_ptr(&query_array[index]);

    in_किल्ली = query_part;

    offset = म_अक्षर(query_part, '=');
    /* If there is no equals, this key has no value */
    जर(!offset) {
      in_किल्ली_len = म_माप(in_किल्ली);
    }
    अन्यथा {
      in_किल्ली_len = offset - in_किल्ली;
    }

    curlx_dyn_init(&encoded_query_array[index].किल्ली,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].मूल्य,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_किल्ली, in_किल्ली_len,
                             &encoded_query_array[index].किल्ली);
    जर(result) {
      जा fail;
    }

    /* Decode/encode the value if it exists */
    जर(offset && offset != (query_part + query_part_len - 1)) {
      माप_प्रकार in_मूल्य_len;
      स्थिरांक अक्षर *in_मूल्य = offset + 1;
      in_मूल्य_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_मूल्य, in_मूल्य_len,
                               &encoded_query_array[index].मूल्य);
      जर(result) {
        जा fail;
      }
    }
    अन्यथा {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].मूल्य, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].मूल्य, "", 1);
    }

    जर(result) {
      जा fail;
    }
  }

  /* Sort the encoded query components by key and value */
  क्विक(&encoded_query_array, अंक_query_components,
        आकार(संरचना pair), compare_func);

  /* Append the query components together to make a full query string */
  साठी(index = 0; index < अंक_query_components; index++) {

    जर(index)
      result = curlx_dyn_addn(dq, "&", 1);
    जर(!result) {
      स्थिरांक अक्षर *किल्ली_ptr = curlx_dyn_ptr(&encoded_query_array[index].किल्ली);
      स्थिरांक अक्षर *मूल्य_ptr = curlx_dyn_ptr(&encoded_query_array[index].मूल्य);
      माप_प्रकार vlen = curlx_dyn_len(&encoded_query_array[index].मूल्य);
      जर(मूल्य_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", किल्ली_ptr, मूल्य_ptr);
      }
      अन्यथा {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", किल्ली_ptr);
      }
    }
    जर(result)
      थांबा;
  }

fail:
  जर(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], अंक_query_components);
  परत result;
}

स्थिर CURLcode parse_sigv4_params(संरचना Curl_easy *data,
                                   स्थिरांक अक्षर *hostname,
                                   संरचना Curl_str *provider0,
                                   संरचना Curl_str *provider1,
                                   संरचना Curl_str *region,
                                   संरचना Curl_str *service)
{
  स्थिरांक अक्षर *ओळ = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  जर(!ओळ || !*ओळ)
    ओळ = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  जर(curlx_str_until(&ओळ, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  जर(curlx_str_single(&ओळ, ':') ||
     curlx_str_until(&ओळ, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  अन्यथा जर(curlx_str_single(&ओळ, ':') ||
          curlx_str_until(&ओळ, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&ओळ, ':') ||
          curlx_str_until(&ओळ, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  जर(!curlx_strlen(service)) {
    स्थिरांक अक्षर *p = hostname;
    जर(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      परत CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (पूर्णांक)curlx_strlen(service), curlx_str(service));

    जर(!curlx_strlen(region)) {
      जर(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        परत CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (पूर्णांक)curlx_strlen(region), curlx_str(region));
    }
  }

  परत CURLE_OK;
}

स्थिर CURLcode get_payload_hash(संरचना Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 संरचना Curl_str *provider0,
                                 संरचना Curl_str *provider1,
                                 संरचना Curl_str *service,
                                 अचिन्हित अक्षर *sha_hash,
                                 अक्षर *sha_hex,
                                 अक्षर *content_sha256_hdr,
                                 स्थिरांक अक्षर **payload_hash_out,
                                 माप_प्रकार *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  जर(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    जर(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    अन्यथा
      result = calc_payload_hash(data, sha_hash, sha_hex);
    जर(result)
      परत result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = म_माप(sha_hex);
  }
  परत CURLE_OK;
}

स्थिर CURLcode get_timestamp(अक्षर *timestamp, माप_प्रकार stampsize)
{
  समय_प्रकार घड़ी;
  संरचना पंचांग पंचांग;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    अक्षर *force_timestamp = दो_पर्या("CURL_FORCETIME");
    जर(force_timestamp)
      घड़ी = 0;
    अन्यथा
      घड़ी = समय(रिक्त);
  }
#अन्यथा
  घड़ी = समय(रिक्त);
#endif
  result = curlx_gmtime(घड़ी, &पंचांग);
  जर(result)
    परत result;

  जर(!स_माला(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &पंचांग))
    परत CURLE_OUT_OF_MEMORY;

  परत CURLE_OK;
}

स्थिर CURLcode make_canonical_request(संरचना Curl_easy *data,
                                       अक्षर *timestamp,
                                       संरचना Curl_str *provider1,
                                       संरचना Curl_str *service,
                                       स्थिरांक अक्षर *method,
                                       स्थिरांक अक्षर *payload_hash,
                                       माप_प्रकार payload_hash_len,
                                       अक्षर **date_header_out,
                                       अक्षर *content_sha256_hdr,
                                       संरचना dynbuf *canonical_headers,
                                       संरचना dynbuf *signed_headers,
                                       अक्षर **canonical_request_out)
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
  जर(result)
    जा fail;

  result = canon_query(data->state.up.query, &canonical_query);
  जर(result)
    जा fail;

  result = canon_path(data->state.up.path, म_माप(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  जर(result)
    जा fail;

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
                  (पूर्णांक)payload_hash_len, payload_hash);
  जर(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    जा fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  परत result;
}

स्थिर CURLcode make_string_to_sign(संरचना Curl_easy *data,
                                    संरचना Curl_str *provider0,
                                    संरचना Curl_str *region,
                                    संरचना Curl_str *service,
                                    स्थिरांक अक्षर *date,
                                    स्थिरांक अक्षर *timestamp,
                                    स्थिरांक अक्षर *canonical_request,
                                    अक्षर **request_type_out,
                                    अक्षर **credential_scope_out,
                                    अक्षर **str_to_sign_out)
{
  अक्षर *request_type;
  अक्षर *credential_scope;
  अक्षर *str_to_sign;
  अचिन्हित अक्षर sha_hash[CURL_SHA256_DIGEST_LENGTH];
  अक्षर sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (पूर्णांक)curlx_strlen(provider0),
                               curlx_str(provider0));
  जर(!request_type)
    परत CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (पूर्णांक)curlx_strlen(region),
                                   curlx_str(region),
                                   (पूर्णांक)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  जर(!credential_scope) {
    curlx_free(request_type);
    परत CURLE_OUT_OF_MEMORY;
  }

  जर(Curl_sha256it(sha_hash, (स्थिरांक अचिन्हित अक्षर *)canonical_request,
                   म_माप(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    परत CURLE_OUT_OF_MEMORY;
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
                              (पूर्णांक)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  जर(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    परत CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  परत CURLE_OK;
}

स्थिर CURLcode sign_and_set_auth_headers(संरचना Curl_easy *data,
                                          संरचना Curl_str *provider0,
                                          संरचना Curl_str *region,
                                          संरचना Curl_str *service,
                                          स्थिरांक अक्षर *request_type,
                                          स्थिरांक अक्षर *credential_scope,
                                          स्थिरांक अक्षर *date,
                                          स्थिरांक अक्षर *str_to_sign,
                                          स्थिरांक अक्षर *date_header,
                                          स्थिरांक अक्षर *content_sha256_hdr,
                                          संरचना dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  स्थिरांक अक्षर *passwd = Curl_creds_passwd(data->state.creds);
  अक्षर *secret = रिक्त;
  अचिन्हित अक्षर sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  अचिन्हित अक्षर sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  अक्षर sha_hex[SHA256_HEX_LENGTH];
  अक्षर *auth_headers = रिक्त;
  अक्षर *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  जर(!user)
    परत CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (पूर्णांक)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  जर(!secret)
    जा fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, म_माप(secret), date, म_माप(date), sign0);
  HMAC_SHA256(sign0, आकार(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, आकार(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, आकार(sign0),
              request_type, म_माप(request_type), sign1);
  HMAC_SHA256(sign1, आकार(sign1),
              str_to_sign, म_माप(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (पूर्णांक)curlx_strlen(provider0),
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
  जर(!auth_headers)
    जा fail;

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
  परत result;
}

CURLcode Curl_output_aws_sigv4(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  संरचना Curl_str provider0 = { रिक्त, 0 };
  संरचना Curl_str provider1 = { रिक्त, 0 };
  संरचना Curl_str region = { रिक्त, 0 };
  संरचना Curl_str service = { रिक्त, 0 };
  स्थिरांक अक्षर *hostname = data->state.origin->hostname;
  अक्षर timestamp[TIMESTAMP_SIZE];
  अक्षर date[9];
  संरचना dynbuf canonical_headers;
  संरचना dynbuf signed_headers;
  अक्षर *date_header = रिक्त;
  Curl_HttpReq httpreq;
  स्थिरांक अक्षर *method = रिक्त;
  स्थिरांक अक्षर *payload_hash = रिक्त;
  माप_प्रकार payload_hash_len = 0;
  अचिन्हित अक्षर sha_hash[CURL_SHA256_DIGEST_LENGTH];
  अक्षर sha_hex[SHA256_HEX_LENGTH];
  अक्षर content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  अक्षर *canonical_request = रिक्त;
  अक्षर *request_type = रिक्त;
  अक्षर *credential_scope = रिक्त;
  अक्षर *str_to_sign = रिक्त;

  जर(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }

  जर(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    परत CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  जर(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  जर(!result)
    result = get_timestamp(timestamp, आकार(timestamp));

  जर(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  जर(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    स_नकल(date, timestamp, आकार(date) - 1);
    date[आकार(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  जर(!result)
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
  परत result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
