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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "http_aws_sigv4.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "slist.h"

#ಅನ್ತರ್ಭಾವ <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  ಕುರು {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ಧ್ರುವ unsigned ವರ್ಣ *)(k), \
                         kl,                         \
                         (ಧ್ರುವ unsigned ವರ್ಣ *)(d), \
                         dl, o);                     \
    ಯದಿ(result) {                                     \
      ಗಚ್ಛ fail;                                     \
    }                                                \
  } ಯಾವತ್(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

ಸಂರಚನಾ pair {
  ಸಂರಚನಾ dynbuf ಕೀಲಿ;
  ಸಂರಚನಾ dynbuf ಮೌಲ್ಯ;
};

ಸ್ಥಿರ ಶೂನ್ಯ sha256_to_hex(ವರ್ಣ *dst, unsigned ವರ್ಣ *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned ವರ್ಣ *)dst, SHA256_HEX_LENGTH);
}

ಸ್ಥಿರ ವರ್ಣ *find_date_hdr(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *sig_hdr)
{
  ವರ್ಣ *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  ಯದಿ(tmp)
    ಪ್ರತ್ಯಾಗಮ tmp;
  ಪ್ರತ್ಯಾಗಮ Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ಸ್ಥಿರ ಶೂನ್ಯ trim_headers(ಸಂರಚನಾ curl_slist *head)
{
  ಸಂರಚನಾ curl_slist *l;
  ಪ್ರತಿ(l = head; l; l = l->next) {
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ; /* to read from */
    ವರ್ಣ *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    ಮೌಲ್ಯ = &l->data[colon];
    ಯದಿ(!*ಮೌಲ್ಯ)
      ಅನುವರ್ತ;
    ++ಮೌಲ್ಯ;
    store = (ವರ್ಣ *)CURL_UNCONST(ಮೌಲ್ಯ);

    /* skip leading whitespace */
    curlx_str_passblanks(&ಮೌಲ್ಯ);

    ಯಾವತ್(*ಮೌಲ್ಯ) {
      ಪೂರ್ಣಾಙ್ಕ space = 0;
      ಯಾವತ್(ISBLANK(*ಮೌಲ್ಯ)) {
        ಮೌಲ್ಯ++;
        space++;
      }
      ಯದಿ(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        ಯದಿ(*ಮೌಲ್ಯ)
          *store++ = ' ';
      }
      ಅನ್ಯಥಾ
        *store++ = *ಮೌಲ್ಯ++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ಸ್ಥಿರ ಶೂನ್ಯ pair_array_free(ಸಂರಚನಾ pair *pair_array, size_t ಅಂಕೆ_elements)
{
  size_t index;

  ಪ್ರತಿ(index = 0; index != ಅಂಕೆ_elements; index++) {
    curlx_dyn_free(&pair_array[index].ಕೀಲಿ);
    curlx_dyn_free(&pair_array[index].ಮೌಲ್ಯ);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ಸ್ಥಿರ ಶೂನ್ಯ dyn_array_free(ಸಂರಚನಾ dynbuf *db, size_t ಅಂಕೆ_elements)
{
  size_t index;

  ಪ್ರತಿ(index = 0; index < ಅಂಕೆ_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

ಸ್ಥಿರ CURLcode split_to_dyn_array(ಧ್ರುವ ವರ್ಣ *source,
                                   ಸಂರಚನಾ dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *ಅಂಕೆ_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t ಅಂಕೆ_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  ಪ್ರತಿ(pos = 0; pos < len; pos++) {
    ಯದಿ(source[pos] == SPLIT_BY) {
      ಯದಿ(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        ಯದಿ(result)
          ಗಚ್ಛ fail;

        segment_length = 0;
        index++;
        ಯದಿ(++ಅಂಕೆ_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          ಗಚ್ಛ fail;
        }
      }
      start = pos + 1;
    }
    ಅನ್ಯಥಾ {
      segment_length++;
    }
  }

  ಯದಿ(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    ಯದಿ(!result && ++ಅಂಕೆ_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *ಅಂಕೆ_splits_out = ಅಂಕೆ_splits;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool is_reserved_char(ಧ್ರುವ ವರ್ಣ c)
{
  ಪ್ರತ್ಯಾಗಮ (ISALNUM(c) || ISURLPUNTCS(c));
}

ಸ್ಥಿರ CURLcode uri_encode_path(ಸಂರಚನಾ Curl_str *original_path,
                                ಸಂರಚನಾ dynbuf *new_path)
{
  ಧ್ರುವ ವರ್ಣ *p = curlx_str(original_path);
  size_t i;

  ಪ್ರತಿ(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned ವರ್ಣ c = p[i];
    ಯದಿ(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    ಅನ್ಯಥಾ
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ಸ್ಥಿರ CURLcode normalize_query(ಧ್ರುವ ವರ್ಣ *string, size_t len,
                                ಸಂರಚನಾ dynbuf *db)
{
  CURLcode result = CURLE_OK;

  ಯಾವತ್(len && !result) {
    unsigned ವರ್ಣ in = (unsigned ವರ್ಣ)*string;
    ಯದಿ(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned ವರ್ಣ)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      ಯದಿ(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        ಅನುವರ್ತ;
      }
    }
    ಅನ್ಯಥಾ {
      string++;
      len--;
    }

    ಯದಿ(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    ಅನ್ಯಥಾ ಯದಿ(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    ಅನ್ಯಥಾ
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool should_urlencode(ಸಂರಚನಾ Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  ಯದಿ(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_ಕೀಲಿ_LEN (MAX_SIGV4_LEN + ಪರಿಮಾಣ("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_ಕೀಲಿ_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compare_header_names(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b)
{
  ಧ್ರುವ ವರ್ಣ *colon_a;
  ಧ್ರುವ ವರ್ಣ *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  ಪೂರ್ಣಾಙ್ಕ cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  ಯದಿ(!cmp)
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(len_a - len_b);

  ಪ್ರತ್ಯಾಗಮ cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ಸ್ಥಿರ CURLcode merge_duplicate_headers(ಸಂರಚನಾ curl_slist *head)
{
  ಸಂರಚನಾ curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  ಯಾವತ್(curr) {
    ಸಂರಚನಾ curl_slist *next = curr->next;
    ಯದಿ(!next)
      ಭಙ್ಗ;

    ಯದಿ(compare_header_names(curr->data, next->data) == 0) {
      ಸಂರಚನಾ dynbuf buf;
      ಧ್ರುವ ವರ್ಣ *colon_next;
      ಧ್ರುವ ವರ್ಣ *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      result = curlx_dyn_add(&buf, val_next);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    ಅನ್ಯಥಾ {
      curr = curr->next;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ಸ್ಥಿರ CURLcode make_headers(ಸಂರಚನಾ Curl_easy *data,
                             ವರ್ಣ *timestamp,
                             ಧ್ರುವ ವರ್ಣ *provider1,
                             size_t plen, /* length of provider1 */
                             ವರ್ಣ **date_header,
                             ವರ್ಣ *content_sha256_header,
                             ಸಂರಚನಾ dynbuf *canonical_headers,
                             ಸಂರಚನಾ dynbuf *signed_headers)
{
  ವರ್ಣ date_hdr_ಕೀಲಿ[DATE_HDR_ಕೀಲಿ_LEN];
  ವರ್ಣ date_full_hdr[DATE_FULL_HDR_LEN];
  ಸಂರಚನಾ curl_slist *head = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_slist *tmp_head = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಸಂರಚನಾ curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_ಕೀಲಿ, DATE_HDR_ಕೀಲಿ_LEN, "X-%.*s-Date",
                 (ಪೂರ್ಣಾಙ್ಕ)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_ಕೀಲಿ[2], provider1, plen);
  date_hdr_ಕೀಲಿ[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (ಪೂರ್ಣಾಙ್ಕ)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  ಯದಿ(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    ವರ್ಣ *fullhost = curlx_strdup(data->state.http_host);

    ಯದಿ(fullhost)
      head = Curl_slist_append_nodup(ರಿಕ್ತ, fullhost);
    ಯದಿ(!head) {
      curlx_free(fullhost);
      ಗಚ್ಛ fail;
    }
  }

  ಯದಿ(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    ಯದಿ(!tmp_head)
      ಗಚ್ಛ fail;
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
  ಪ್ರತಿ(l = data->set.headers; l; l = l->next) {
    ವರ್ಣ *dupdata;
    ಧ್ರುವ ವರ್ಣ *ptr;
    ಧ್ರುವ ವರ್ಣ *sep = strchr(l->data, ':');
    ಯದಿ(!sep)
      sep = strchr(l->data, ';');
    ಯದಿ(!sep || (*sep == ':' && !*(sep + 1)))
      ಅನುವರ್ತ;
    ಪ್ರತಿ(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    ಯದಿ(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      ಅನುವರ್ತ;
    dupdata = curlx_strdup(l->data);
    ಯದಿ(!dupdata)
      ಗಚ್ಛ fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    ಯದಿ(!tmp_head) {
      curlx_free(dupdata);
      ಗಚ್ಛ fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_ಕೀಲಿ);
  ಯದಿ(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    ಯದಿ(!tmp_head)
      ಗಚ್ಛ fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_ಕೀಲಿ, timestamp);
    ಯದಿ(!*date_header)
      ಗಚ್ಛ fail;
  }
  ಅನ್ಯಥಾ {
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;
    ಧ್ರುವ ವರ್ಣ *endp;
    ಮೌಲ್ಯ = strchr(*date_header, ':');
    ಯದಿ(!ಮೌಲ್ಯ) {
      *date_header = ರಿಕ್ತ;
      ಗಚ್ಛ fail;
    }
    ++ಮೌಲ್ಯ;
    curlx_str_passblanks(&ಮೌಲ್ಯ);
    endp = ಮೌಲ್ಯ;
    ಯಾವತ್(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    ಯದಿ((endp - ಮೌಲ್ಯ) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, ಮೌಲ್ಯ, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    ಅನ್ಯಥಾ
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = ರಿಕ್ತ;
  }

  /* alpha-sort by header name in a case sensitive manner */
  ಕುರು {
    again = FALSE;
    ಪ್ರತಿ(l = head; l; l = l->next) {
      ಸಂರಚನಾ curl_slist *next = l->next;

      ಯದಿ(next && compare_header_names(l->data, next->data) > 0) {
        ವರ್ಣ *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } ಯಾವತ್(again);

  result = merge_duplicate_headers(head);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  ಪ್ರತಿ(l = head; l; l = l->next) {
    ವರ್ಣ *tmp;

    ಯದಿ(curlx_dyn_add(canonical_headers, l->data))
      ಗಚ್ಛ fail;
    ಯದಿ(curlx_dyn_add(canonical_headers, "\n"))
      ಗಚ್ಛ fail;

    tmp = strchr(l->data, ':');
    ಯದಿ(tmp)
      *tmp = 0;

    ಯದಿ(l != head && curlx_dyn_add(signed_headers, ";"))
      ಗಚ್ಛ fail;
    ಯದಿ(curlx_dyn_add(signed_headers, l->data))
      ಗಚ್ಛ fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  ಪ್ರತ್ಯಾಗಮ result;
}

#define CONTENT_SHA256_ಕೀಲಿ_LEN (MAX_SIGV4_LEN + ಪರಿಮಾಣ("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_ಕೀಲಿ_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *parse_content_sha_hdr(ಸಂರಚನಾ Curl_easy *data,
                                         ಧ್ರುವ ವರ್ಣ *provider1,
                                         size_t plen,
                                         size_t *ಮೌಲ್ಯ_len)
{
  ವರ್ಣ ಕೀಲಿ[CONTENT_SHA256_ಕೀಲಿ_LEN];
  size_t ಕೀಲಿ_len;
  ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;
  size_t len;

  ಕೀಲಿ_len = curl_msnprintf(ಕೀಲಿ, ಪರಿಮಾಣ(ಕೀಲಿ), "x-%.*s-content-sha256",
                           (ಪೂರ್ಣಾಙ್ಕ)plen, provider1);

  ಮೌಲ್ಯ = Curl_checkheaders(data, ಕೀಲಿ, ಕೀಲಿ_len);
  ಯದಿ(!ಮೌಲ್ಯ)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಮೌಲ್ಯ = strchr(ಮೌಲ್ಯ, ':');
  ಯದಿ(!ಮೌಲ್ಯ)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ++ಮೌಲ್ಯ;

  curlx_str_passblanks(&ಮೌಲ್ಯ);

  len = strlen(ಮೌಲ್ಯ);
  ಯಾವತ್(len > 0 && ISBLANK(ಮೌಲ್ಯ[len - 1]))
    --len;

  *ಮೌಲ್ಯ_len = len;
  ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ CURLcode calc_payload_hash(ಸಂರಚನಾ Curl_easy *data,
                                  unsigned ವರ್ಣ *sha_hash, ವರ್ಣ *sha_hex)
{
  ಧ್ರುವ ವರ್ಣ *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  ಯದಿ(post_data) {
    ಯದಿ(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    ಅನ್ಯಥಾ
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (ಧ್ರುವ unsigned ವರ್ಣ *)post_data,
                         post_data_len);
  ಯದಿ(!result)
    sha256_to_hex(sha_hex, sha_hash);
  ಪ್ರತ್ಯಾಗಮ result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ಸ್ಥಿರ CURLcode calc_s3_payload_hash(ಸಂರಚನಾ Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     ಧ್ರುವ ವರ್ಣ *provider1,
                                     size_t plen,
                                     unsigned ವರ್ಣ *sha_hash,
                                     ವರ್ಣ *sha_hex, ವರ್ಣ *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ಯದಿ(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    ಯದಿ(result)
      ಗಚ್ಛ fail;
  }
  ಅನ್ಯಥಾ {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (ಪೂರ್ಣಾಙ್ಕ)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compare_func(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
  ಧ್ರುವ ಸಂರಚನಾ pair *aa = a;
  ಧ್ರುವ ಸಂರಚನಾ pair *bb = b;
  ಧ್ರುವ size_t aa_ಕೀಲಿ_len = curlx_dyn_len(&aa->ಕೀಲಿ);
  ಧ್ರುವ size_t bb_ಕೀಲಿ_len = curlx_dyn_len(&bb->ಕೀಲಿ);
  ಧ್ರುವ size_t aa_ಮೌಲ್ಯ_len = curlx_dyn_len(&aa->ಮೌಲ್ಯ);
  ಧ್ರುವ size_t bb_ಮೌಲ್ಯ_len = curlx_dyn_len(&bb->ಮೌಲ್ಯ);
  ಪೂರ್ಣಾಙ್ಕ compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  ಯದಿ((aa_ಕೀಲಿ_len == 0) && (bb_ಕೀಲಿ_len == 0))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(aa_ಕೀಲಿ_len == 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(bb_ಕೀಲಿ_len == 0)
    ಪ್ರತ್ಯಾಗಮ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ಕೀಲಿ), curlx_dyn_ptr(&bb->ಕೀಲಿ));
  ಯದಿ(compare) {
    ಪ್ರತ್ಯಾಗಮ compare;
  }

  /* Compare values */
  ಯದಿ((aa_ಮೌಲ್ಯ_len == 0) && (bb_ಮೌಲ್ಯ_len == 0))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(aa_ಮೌಲ್ಯ_len == 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(bb_ಮೌಲ್ಯ_len == 0)
    ಪ್ರತ್ಯಾಗಮ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ಮೌಲ್ಯ), curlx_dyn_ptr(&bb->ಮೌಲ್ಯ));

  ಪ್ರತ್ಯಾಗಮ compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(ಧ್ರುವ ವರ್ಣ *q, size_t len,
                             ಸಂರಚನಾ dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ಧ್ರುವ ವರ್ಣ *q, size_t len,
                             ಸಂರಚನಾ dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  ಸಂರಚನಾ Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  ಯದಿ(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  ಅನ್ಯಥಾ
    result = curlx_dyn_addn(new_path, q, len);

  ಯದಿ(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  ಪ್ರತ್ಯಾಗಮ result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ಧ್ರುವ ವರ್ಣ *query, ಸಂರಚನಾ dynbuf *dq);
UNITTEST CURLcode canon_query(ಧ್ರುವ ವರ್ಣ *query, ಸಂರಚನಾ dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  ಸಂರಚನಾ dynbuf query_array[MAX_QUERY_COMPONENTS];
  ಸಂರಚನಾ pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t ಅಂಕೆ_query_components;
  size_t counted_query_components = 0;
  size_t index;

  ಯದಿ(!query)
    ಪ್ರತ್ಯಾಗಮ result;

  result = split_to_dyn_array(query, &query_array[0], &ಅಂಕೆ_query_components);
  ಯದಿ(result) {
    ಗಚ್ಛ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  ಪ್ರತಿ(index = 0; index < ಅಂಕೆ_query_components; index++) {
    ಧ್ರುವ ವರ್ಣ *in_ಕೀಲಿ;
    size_t in_ಕೀಲಿ_len;
    ಧ್ರುವ ವರ್ಣ *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    ಧ್ರುವ ವರ್ಣ *query_part = curlx_dyn_ptr(&query_array[index]);

    in_ಕೀಲಿ = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    ಯದಿ(!offset) {
      in_ಕೀಲಿ_len = strlen(in_ಕೀಲಿ);
    }
    ಅನ್ಯಥಾ {
      in_ಕೀಲಿ_len = offset - in_ಕೀಲಿ;
    }

    curlx_dyn_init(&encoded_query_array[index].ಕೀಲಿ,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].ಮೌಲ್ಯ,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_ಕೀಲಿ, in_ಕೀಲಿ_len,
                             &encoded_query_array[index].ಕೀಲಿ);
    ಯದಿ(result) {
      ಗಚ್ಛ fail;
    }

    /* Decode/encode the value if it exists */
    ಯದಿ(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_ಮೌಲ್ಯ_len;
      ಧ್ರುವ ವರ್ಣ *in_ಮೌಲ್ಯ = offset + 1;
      in_ಮೌಲ್ಯ_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_ಮೌಲ್ಯ, in_ಮೌಲ್ಯ_len,
                               &encoded_query_array[index].ಮೌಲ್ಯ);
      ಯದಿ(result) {
        ಗಚ್ಛ fail;
      }
    }
    ಅನ್ಯಥಾ {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].ಮೌಲ್ಯ, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].ಮೌಲ್ಯ, "", 1);
    }

    ಯದಿ(result) {
      ಗಚ್ಛ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, ಅಂಕೆ_query_components,
        ಪರಿಮಾಣ(ಸಂರಚನಾ pair), compare_func);

  /* Append the query components together to make a full query string */
  ಪ್ರತಿ(index = 0; index < ಅಂಕೆ_query_components; index++) {

    ಯದಿ(index)
      result = curlx_dyn_addn(dq, "&", 1);
    ಯದಿ(!result) {
      ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_ptr = curlx_dyn_ptr(&encoded_query_array[index].ಕೀಲಿ);
      ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ_ptr = curlx_dyn_ptr(&encoded_query_array[index].ಮೌಲ್ಯ);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].ಮೌಲ್ಯ);
      ಯದಿ(ಮೌಲ್ಯ_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", ಕೀಲಿ_ptr, ಮೌಲ್ಯ_ptr);
      }
      ಅನ್ಯಥಾ {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", ಕೀಲಿ_ptr);
      }
    }
    ಯದಿ(result)
      ಭಙ್ಗ;
  }

fail:
  ಯದಿ(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], ಅಂಕೆ_query_components);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode parse_sigv4_params(ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ವರ್ಣ *hostname,
                                   ಸಂರಚನಾ Curl_str *provider0,
                                   ಸಂರಚನಾ Curl_str *provider1,
                                   ಸಂರಚನಾ Curl_str *region,
                                   ಸಂರಚನಾ Curl_str *service)
{
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  ಯದಿ(!ಸಾಲು || !*ಸಾಲು)
    ಸಾಲು = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  ಯದಿ(curlx_str_until(&ಸಾಲು, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಯದಿ(curlx_str_single(&ಸಾಲು, ':') ||
     curlx_str_until(&ಸಾಲು, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  ಅನ್ಯಥಾ ಯದಿ(curlx_str_single(&ಸಾಲು, ':') ||
          curlx_str_until(&ಸಾಲು, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&ಸಾಲು, ':') ||
          curlx_str_until(&ಸಾಲು, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  ಯದಿ(!curlx_strlen(service)) {
    ಧ್ರುವ ವರ್ಣ *p = hostname;
    ಯದಿ(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(service), curlx_str(service));

    ಯದಿ(!curlx_strlen(region)) {
      ಯದಿ(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(region), curlx_str(region));
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode get_payload_hash(ಸಂರಚನಾ Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 ಸಂರಚನಾ Curl_str *provider0,
                                 ಸಂರಚನಾ Curl_str *provider1,
                                 ಸಂರಚನಾ Curl_str *service,
                                 unsigned ವರ್ಣ *sha_hash,
                                 ವರ್ಣ *sha_hex,
                                 ವರ್ಣ *content_sha256_hdr,
                                 ಧ್ರುವ ವರ್ಣ **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  ಯದಿ(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    ಯದಿ(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    ಅನ್ಯಥಾ
      result = calc_payload_hash(data, sha_hash, sha_hex);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode get_timestamp(ವರ್ಣ *timestamp, size_t stampsize)
{
  time_t clock;
  ಸಂರಚನಾ tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    ವರ್ಣ *force_timestamp = getenv("CURL_FORCETIME");
    ಯದಿ(force_timestamp)
      clock = 0;
    ಅನ್ಯಥಾ
      clock = time(ರಿಕ್ತ);
  }
#ಅನ್ಯಥಾ
  clock = time(ರಿಕ್ತ);
#endif
  result = curlx_gmtime(clock, &tm);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode make_canonical_request(ಸಂರಚನಾ Curl_easy *data,
                                       ವರ್ಣ *timestamp,
                                       ಸಂರಚನಾ Curl_str *provider1,
                                       ಸಂರಚನಾ Curl_str *service,
                                       ಧ್ರುವ ವರ್ಣ *method,
                                       ಧ್ರುವ ವರ್ಣ *payload_hash,
                                       size_t payload_hash_len,
                                       ವರ್ಣ **date_header_out,
                                       ವರ್ಣ *content_sha256_hdr,
                                       ಸಂರಚನಾ dynbuf *canonical_headers,
                                       ಸಂರಚನಾ dynbuf *signed_headers,
                                       ವರ್ಣ **canonical_request_out)
{
  ಸಂರಚನಾ dynbuf canonical_query;
  ಸಂರಚನಾ dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  result = canon_query(data->state.up.query, &canonical_query);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  ಯದಿ(result)
    ಗಚ್ಛ fail;

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
                  (ಪೂರ್ಣಾಙ್ಕ)payload_hash_len, payload_hash);
  ಯದಿ(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode make_string_to_sign(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ Curl_str *provider0,
                                    ಸಂರಚನಾ Curl_str *region,
                                    ಸಂರಚನಾ Curl_str *service,
                                    ಧ್ರುವ ವರ್ಣ *date,
                                    ಧ್ರುವ ವರ್ಣ *timestamp,
                                    ಧ್ರುವ ವರ್ಣ *canonical_request,
                                    ವರ್ಣ **request_type_out,
                                    ವರ್ಣ **credential_scope_out,
                                    ವರ್ಣ **str_to_sign_out)
{
  ವರ್ಣ *request_type;
  ವರ್ಣ *credential_scope;
  ವರ್ಣ *str_to_sign;
  unsigned ವರ್ಣ sha_hash[CURL_SHA256_DIGEST_LENGTH];
  ವರ್ಣ sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(provider0),
                               curlx_str(provider0));
  ಯದಿ(!request_type)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(region),
                                   curlx_str(region),
                                   (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  ಯದಿ(!credential_scope) {
    curlx_free(request_type);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(Curl_sha256it(sha_hash, (ಧ್ರುವ unsigned ವರ್ಣ *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
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
                              (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  ಯದಿ(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode sign_and_set_auth_headers(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ Curl_str *provider0,
                                          ಸಂರಚನಾ Curl_str *region,
                                          ಸಂರಚನಾ Curl_str *service,
                                          ಧ್ರುವ ವರ್ಣ *request_type,
                                          ಧ್ರುವ ವರ್ಣ *credential_scope,
                                          ಧ್ರುವ ವರ್ಣ *date,
                                          ಧ್ರುವ ವರ್ಣ *str_to_sign,
                                          ಧ್ರುವ ವರ್ಣ *date_header,
                                          ಧ್ರುವ ವರ್ಣ *content_sha256_hdr,
                                          ಸಂರಚನಾ dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಧ್ರುವ ವರ್ಣ *passwd = Curl_creds_passwd(data->state.creds);
  ವರ್ಣ *secret = ರಿಕ್ತ;
  unsigned ವರ್ಣ sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned ವರ್ಣ sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  ವರ್ಣ sha_hex[SHA256_HEX_LENGTH];
  ವರ್ಣ *auth_headers = ರಿಕ್ತ;
  ವರ್ಣ *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  ಯದಿ(!user)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  ಯದಿ(!secret)
    ಗಚ್ಛ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, ಪರಿಮಾಣ(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, ಪರಿಮಾಣ(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, ಪರಿಮಾಣ(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, ಪರಿಮಾಣ(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(provider0),
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
  ಯದಿ(!auth_headers)
    ಗಚ್ಛ fail;

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
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_output_aws_sigv4(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಸಂರಚನಾ Curl_str provider0 = { ರಿಕ್ತ, 0 };
  ಸಂರಚನಾ Curl_str provider1 = { ರಿಕ್ತ, 0 };
  ಸಂರಚನಾ Curl_str region = { ರಿಕ್ತ, 0 };
  ಸಂರಚನಾ Curl_str service = { ರಿಕ್ತ, 0 };
  ಧ್ರುವ ವರ್ಣ *hostname = data->state.origin->hostname;
  ವರ್ಣ timestamp[TIMESTAMP_SIZE];
  ವರ್ಣ date[9];
  ಸಂರಚನಾ dynbuf canonical_headers;
  ಸಂರಚನಾ dynbuf signed_headers;
  ವರ್ಣ *date_header = ರಿಕ್ತ;
  Curl_HttpReq httpreq;
  ಧ್ರುವ ವರ್ಣ *method = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *payload_hash = ರಿಕ್ತ;
  size_t payload_hash_len = 0;
  unsigned ವರ್ಣ sha_hash[CURL_SHA256_DIGEST_LENGTH];
  ವರ್ಣ sha_hex[SHA256_HEX_LENGTH];
  ವರ್ಣ content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  ವರ್ಣ *canonical_request = ರಿಕ್ತ;
  ವರ್ಣ *request_type = ರಿಕ್ತ;
  ವರ್ಣ *credential_scope = ರಿಕ್ತ;
  ವರ್ಣ *str_to_sign = ರಿಕ್ತ;

  ಯದಿ(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ಯದಿ(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  ಯದಿ(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  ಯದಿ(!result)
    result = get_timestamp(timestamp, ಪರಿಮಾಣ(timestamp));

  ಯದಿ(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  ಯದಿ(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, ಪರಿಮಾಣ(date) - 1);
    date[ಪರಿಮಾಣ(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  ಯದಿ(!result)
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
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
