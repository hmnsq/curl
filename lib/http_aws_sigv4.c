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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "http_aws_sigv4.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "slist.h"

#ଅନ୍ତର୍ଭାଵ <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  କର {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ଅଚଳ unsigned ଅକ୍ଷର *)(k), \
                         kl,                         \
                         (ଅଚଳ unsigned ଅକ୍ଷର *)(d), \
                         dl, o);                     \
    ଯଦି(result) {                                     \
      ଯାଅ fail;                                     \
    }                                                \
  } ଯେତେବେଳେ(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

ସଂରଚନା pair {
  ସଂରଚନା dynbuf ଚାବି;
  ସଂରଚନା dynbuf ମୂଲ୍ୟ;
};

ସ୍ଥିର ଶୂନ୍ୟ sha256_to_hex(ଅକ୍ଷର *dst, unsigned ଅକ୍ଷର *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned ଅକ୍ଷର *)dst, SHA256_HEX_LENGTH);
}

ସ୍ଥିର ଅକ୍ଷର *find_date_hdr(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *sig_hdr)
{
  ଅକ୍ଷର *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  ଯଦି(tmp)
    ଫେରନ୍ତୁ tmp;
  ଫେରନ୍ତୁ Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
ସ୍ଥିର ଶୂନ୍ୟ trim_headers(ସଂରଚନା curl_slist *head)
{
  ସଂରଚନା curl_slist *l;
  ପାଇଁ(l = head; l; l = l->next) {
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ; /* to read from */
    ଅକ୍ଷର *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    ମୂଲ୍ୟ = &l->data[colon];
    ଯଦି(!*ମୂଲ୍ୟ)
      ଜାରି;
    ++ମୂଲ୍ୟ;
    store = (ଅକ୍ଷର *)CURL_UNCONST(ମୂଲ୍ୟ);

    /* skip leading whitespace */
    curlx_str_passblanks(&ମୂଲ୍ୟ);

    ଯେତେବେଳେ(*ମୂଲ୍ୟ) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା space = 0;
      ଯେତେବେଳେ(ISBLANK(*ମୂଲ୍ୟ)) {
        ମୂଲ୍ୟ++;
        space++;
      }
      ଯଦି(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        ଯଦି(*ମୂଲ୍ୟ)
          *store++ = ' ';
      }
      ଅନ୍ୟଥା
        *store++ = *ମୂଲ୍ୟ++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
ସ୍ଥିର ଶୂନ୍ୟ pair_array_free(ସଂରଚନା pair *pair_array, size_t ଅଙ୍କ_elements)
{
  size_t index;

  ପାଇଁ(index = 0; index != ଅଙ୍କ_elements; index++) {
    curlx_dyn_free(&pair_array[index].ଚାବି);
    curlx_dyn_free(&pair_array[index].ମୂଲ୍ୟ);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
ସ୍ଥିର ଶୂନ୍ୟ dyn_array_free(ସଂରଚନା dynbuf *db, size_t ଅଙ୍କ_elements)
{
  size_t index;

  ପାଇଁ(index = 0; index < ଅଙ୍କ_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

ସ୍ଥିର CURLcode split_to_dyn_array(ଅଚଳ ଅକ୍ଷର *source,
                                   ସଂରଚନା dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *ଅଙ୍କ_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t ଅଙ୍କ_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  ପାଇଁ(pos = 0; pos < len; pos++) {
    ଯଦି(source[pos] == SPLIT_BY) {
      ଯଦି(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        ଯଦି(result)
          ଯାଅ fail;

        segment_length = 0;
        index++;
        ଯଦି(++ଅଙ୍କ_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          ଯାଅ fail;
        }
      }
      start = pos + 1;
    }
    ଅନ୍ୟଥା {
      segment_length++;
    }
  }

  ଯଦି(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    ଯଦି(!result && ++ଅଙ୍କ_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *ଅଙ୍କ_splits_out = ଅଙ୍କ_splits;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool is_reserved_char(ଅଚଳ ଅକ୍ଷର c)
{
  ଫେରନ୍ତୁ (ISALNUM(c) || ISURLPUNTCS(c));
}

ସ୍ଥିର CURLcode uri_encode_path(ସଂରଚନା Curl_str *original_path,
                                ସଂରଚନା dynbuf *new_path)
{
  ଅଚଳ ଅକ୍ଷର *p = curlx_str(original_path);
  size_t i;

  ପାଇଁ(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned ଅକ୍ଷର c = p[i];
    ଯଦି(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    ଅନ୍ୟଥା
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
ସ୍ଥିର CURLcode normalize_query(ଅଚଳ ଅକ୍ଷର *string, size_t len,
                                ସଂରଚନା dynbuf *db)
{
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(len && !result) {
    unsigned ଅକ୍ଷର in = (unsigned ଅକ୍ଷର)*string;
    ଯଦି(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned ଅକ୍ଷର)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      ଯଦି(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        ଜାରି;
      }
    }
    ଅନ୍ୟଥା {
      string++;
      len--;
    }

    ଯଦି(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    ଅନ୍ୟଥା ଯଦି(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    ଅନ୍ୟଥା
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool should_urlencode(ସଂରଚନା Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  ଯଦି(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_ଚାବି_LEN (MAX_SIGV4_LEN + ଆକାର("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_ଚାବି_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compare_header_names(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b)
{
  ଅଚଳ ଅକ୍ଷର *colon_a;
  ଅଚଳ ଅକ୍ଷର *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  ଯଦି(!cmp)
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(len_a - len_b);

  ଫେରନ୍ତୁ cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
ସ୍ଥିର CURLcode merge_duplicate_headers(ସଂରଚନା curl_slist *head)
{
  ସଂରଚନା curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(curr) {
    ସଂରଚନା curl_slist *next = curr->next;
    ଯଦି(!next)
      ଭାଙ୍ଗ;

    ଯଦି(compare_header_names(curr->data, next->data) == 0) {
      ସଂରଚନା dynbuf buf;
      ଅଚଳ ଅକ୍ଷର *colon_next;
      ଅଚଳ ଅକ୍ଷର *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      result = curlx_dyn_add(&buf, val_next);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    ଅନ୍ୟଥା {
      curr = curr->next;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
ସ୍ଥିର CURLcode make_headers(ସଂରଚନା Curl_easy *data,
                             ଅକ୍ଷର *timestamp,
                             ଅଚଳ ଅକ୍ଷର *provider1,
                             size_t plen, /* length of provider1 */
                             ଅକ୍ଷର **date_header,
                             ଅକ୍ଷର *content_sha256_header,
                             ସଂରଚନା dynbuf *canonical_headers,
                             ସଂରଚନା dynbuf *signed_headers)
{
  ଅକ୍ଷର date_hdr_ଚାବି[DATE_HDR_ଚାବି_LEN];
  ଅକ୍ଷର date_full_hdr[DATE_FULL_HDR_LEN];
  ସଂରଚନା curl_slist *head = ଖାଲି;
  ସଂରଚନା curl_slist *tmp_head = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ସଂରଚନା curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_ଚାବି, DATE_HDR_ଚାବି_LEN, "X-%.*s-Date",
                 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_ଚାବି[2], provider1, plen);
  date_hdr_ଚାବି[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  ଯଦି(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    ଅକ୍ଷର *fullhost = curlx_strdup(data->state.http_host);

    ଯଦି(fullhost)
      head = Curl_slist_append_nodup(ଖାଲି, fullhost);
    ଯଦି(!head) {
      curlx_free(fullhost);
      ଯାଅ fail;
    }
  }

  ଯଦି(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    ଯଦି(!tmp_head)
      ଯାଅ fail;
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
  ପାଇଁ(l = data->set.headers; l; l = l->next) {
    ଅକ୍ଷର *dupdata;
    ଅଚଳ ଅକ୍ଷର *ptr;
    ଅଚଳ ଅକ୍ଷର *sep = strchr(l->data, ':');
    ଯଦି(!sep)
      sep = strchr(l->data, ';');
    ଯଦି(!sep || (*sep == ':' && !*(sep + 1)))
      ଜାରି;
    ପାଇଁ(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    ଯଦି(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      ଜାରି;
    dupdata = curlx_strdup(l->data);
    ଯଦି(!dupdata)
      ଯାଅ fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    ଯଦି(!tmp_head) {
      curlx_free(dupdata);
      ଯାଅ fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_ଚାବି);
  ଯଦି(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    ଯଦି(!tmp_head)
      ଯାଅ fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_ଚାବି, timestamp);
    ଯଦି(!*date_header)
      ଯାଅ fail;
  }
  ଅନ୍ୟଥା {
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;
    ଅଚଳ ଅକ୍ଷର *endp;
    ମୂଲ୍ୟ = strchr(*date_header, ':');
    ଯଦି(!ମୂଲ୍ୟ) {
      *date_header = ଖାଲି;
      ଯାଅ fail;
    }
    ++ମୂଲ୍ୟ;
    curlx_str_passblanks(&ମୂଲ୍ୟ);
    endp = ମୂଲ୍ୟ;
    ଯେତେବେଳେ(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    ଯଦି((endp - ମୂଲ୍ୟ) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, ମୂଲ୍ୟ, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    ଅନ୍ୟଥା
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = ଖାଲି;
  }

  /* alpha-sort by header name in a case sensitive manner */
  କର {
    again = FALSE;
    ପାଇଁ(l = head; l; l = l->next) {
      ସଂରଚନା curl_slist *next = l->next;

      ଯଦି(next && compare_header_names(l->data, next->data) > 0) {
        ଅକ୍ଷର *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } ଯେତେବେଳେ(again);

  result = merge_duplicate_headers(head);
  ଯଦି(result)
    ଯାଅ fail;

  ପାଇଁ(l = head; l; l = l->next) {
    ଅକ୍ଷର *tmp;

    ଯଦି(curlx_dyn_add(canonical_headers, l->data))
      ଯାଅ fail;
    ଯଦି(curlx_dyn_add(canonical_headers, "\n"))
      ଯାଅ fail;

    tmp = strchr(l->data, ':');
    ଯଦି(tmp)
      *tmp = 0;

    ଯଦି(l != head && curlx_dyn_add(signed_headers, ";"))
      ଯାଅ fail;
    ଯଦି(curlx_dyn_add(signed_headers, l->data))
      ଯାଅ fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  ଫେରନ୍ତୁ result;
}

#define CONTENT_SHA256_ଚାବି_LEN (MAX_SIGV4_LEN + ଆକାର("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_ଚାବି_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *parse_content_sha_hdr(ସଂରଚନା Curl_easy *data,
                                         ଅଚଳ ଅକ୍ଷର *provider1,
                                         size_t plen,
                                         size_t *ମୂଲ୍ୟ_len)
{
  ଅକ୍ଷର ଚାବି[CONTENT_SHA256_ଚାବି_LEN];
  size_t ଚାବି_len;
  ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;
  size_t len;

  ଚାବି_len = curl_msnprintf(ଚାବି, ଆକାର(ଚାବି), "x-%.*s-content-sha256",
                           (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen, provider1);

  ମୂଲ୍ୟ = Curl_checkheaders(data, ଚାବି, ଚାବି_len);
  ଯଦି(!ମୂଲ୍ୟ)
    ଫେରନ୍ତୁ ଖାଲି;

  ମୂଲ୍ୟ = strchr(ମୂଲ୍ୟ, ':');
  ଯଦି(!ମୂଲ୍ୟ)
    ଫେରନ୍ତୁ ଖାଲି;
  ++ମୂଲ୍ୟ;

  curlx_str_passblanks(&ମୂଲ୍ୟ);

  len = strlen(ମୂଲ୍ୟ);
  ଯେତେବେଳେ(len > 0 && ISBLANK(ମୂଲ୍ୟ[len - 1]))
    --len;

  *ମୂଲ୍ୟ_len = len;
  ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

ସ୍ଥିର CURLcode calc_payload_hash(ସଂରଚନା Curl_easy *data,
                                  unsigned ଅକ୍ଷର *sha_hash, ଅକ୍ଷର *sha_hex)
{
  ଅଚଳ ଅକ୍ଷର *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  ଯଦି(post_data) {
    ଯଦି(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    ଅନ୍ୟଥା
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (ଅଚଳ unsigned ଅକ୍ଷର *)post_data,
                         post_data_len);
  ଯଦି(!result)
    sha256_to_hex(sha_hex, sha_hash);
  ଫେରନ୍ତୁ result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

ସ୍ଥିର CURLcode calc_s3_payload_hash(ସଂରଚନା Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     ଅଚଳ ଅକ୍ଷର *provider1,
                                     size_t plen,
                                     unsigned ଅକ୍ଷର *sha_hash,
                                     ଅକ୍ଷର *sha_hex, ଅକ୍ଷର *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ଯଦି(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    ଯଦି(result)
      ଯାଅ fail;
  }
  ଅନ୍ୟଥା {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compare_func(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
  ଅଚଳ ସଂରଚନା pair *aa = a;
  ଅଚଳ ସଂରଚନା pair *bb = b;
  ଅଚଳ size_t aa_ଚାବି_len = curlx_dyn_len(&aa->ଚାବି);
  ଅଚଳ size_t bb_ଚାବି_len = curlx_dyn_len(&bb->ଚାବି);
  ଅଚଳ size_t aa_ମୂଲ୍ୟ_len = curlx_dyn_len(&aa->ମୂଲ୍ୟ);
  ଅଚଳ size_t bb_ମୂଲ୍ୟ_len = curlx_dyn_len(&bb->ମୂଲ୍ୟ);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  ଯଦି((aa_ଚାବି_len == 0) && (bb_ଚାବି_len == 0))
    ଫେରନ୍ତୁ 0;
  ଯଦି(aa_ଚାବି_len == 0)
    ଫେରନ୍ତୁ -1;
  ଯଦି(bb_ଚାବି_len == 0)
    ଫେରନ୍ତୁ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ଚାବି), curlx_dyn_ptr(&bb->ଚାବି));
  ଯଦି(compare) {
    ଫେରନ୍ତୁ compare;
  }

  /* Compare values */
  ଯଦି((aa_ମୂଲ୍ୟ_len == 0) && (bb_ମୂଲ୍ୟ_len == 0))
    ଫେରନ୍ତୁ 0;
  ଯଦି(aa_ମୂଲ୍ୟ_len == 0)
    ଫେରନ୍ତୁ -1;
  ଯଦି(bb_ମୂଲ୍ୟ_len == 0)
    ଫେରନ୍ତୁ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ମୂଲ୍ୟ), curlx_dyn_ptr(&bb->ମୂଲ୍ୟ));

  ଫେରନ୍ତୁ compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(ଅଚଳ ଅକ୍ଷର *q, size_t len,
                             ସଂରଚନା dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(ଅଚଳ ଅକ୍ଷର *q, size_t len,
                             ସଂରଚନା dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  ସଂରଚନା Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  ଯଦି(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  ଅନ୍ୟଥା
    result = curlx_dyn_addn(new_path, q, len);

  ଯଦି(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  ଫେରନ୍ତୁ result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ଅଚଳ ଅକ୍ଷର *query, ସଂରଚନା dynbuf *dq);
UNITTEST CURLcode canon_query(ଅଚଳ ଅକ୍ଷର *query, ସଂରଚନା dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  ସଂରଚନା dynbuf query_array[MAX_QUERY_COMPONENTS];
  ସଂରଚନା pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t ଅଙ୍କ_query_components;
  size_t counted_query_components = 0;
  size_t index;

  ଯଦି(!query)
    ଫେରନ୍ତୁ result;

  result = split_to_dyn_array(query, &query_array[0], &ଅଙ୍କ_query_components);
  ଯଦି(result) {
    ଯାଅ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  ପାଇଁ(index = 0; index < ଅଙ୍କ_query_components; index++) {
    ଅଚଳ ଅକ୍ଷର *in_ଚାବି;
    size_t in_ଚାବି_len;
    ଅଚଳ ଅକ୍ଷର *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    ଅଚଳ ଅକ୍ଷର *query_part = curlx_dyn_ptr(&query_array[index]);

    in_ଚାବି = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    ଯଦି(!offset) {
      in_ଚାବି_len = strlen(in_ଚାବି);
    }
    ଅନ୍ୟଥା {
      in_ଚାବି_len = offset - in_ଚାବି;
    }

    curlx_dyn_init(&encoded_query_array[index].ଚାବି,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].ମୂଲ୍ୟ,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_ଚାବି, in_ଚାବି_len,
                             &encoded_query_array[index].ଚାବି);
    ଯଦି(result) {
      ଯାଅ fail;
    }

    /* Decode/encode the value if it exists */
    ଯଦି(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_ମୂଲ୍ୟ_len;
      ଅଚଳ ଅକ୍ଷର *in_ମୂଲ୍ୟ = offset + 1;
      in_ମୂଲ୍ୟ_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_ମୂଲ୍ୟ, in_ମୂଲ୍ୟ_len,
                               &encoded_query_array[index].ମୂଲ୍ୟ);
      ଯଦି(result) {
        ଯାଅ fail;
      }
    }
    ଅନ୍ୟଥା {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].ମୂଲ୍ୟ, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].ମୂଲ୍ୟ, "", 1);
    }

    ଯଦି(result) {
      ଯାଅ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, ଅଙ୍କ_query_components,
        ଆକାର(ସଂରଚନା pair), compare_func);

  /* Append the query components together to make a full query string */
  ପାଇଁ(index = 0; index < ଅଙ୍କ_query_components; index++) {

    ଯଦି(index)
      result = curlx_dyn_addn(dq, "&", 1);
    ଯଦି(!result) {
      ଅଚଳ ଅକ୍ଷର *ଚାବି_ptr = curlx_dyn_ptr(&encoded_query_array[index].ଚାବି);
      ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ_ptr = curlx_dyn_ptr(&encoded_query_array[index].ମୂଲ୍ୟ);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].ମୂଲ୍ୟ);
      ଯଦି(ମୂଲ୍ୟ_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", ଚାବି_ptr, ମୂଲ୍ୟ_ptr);
      }
      ଅନ୍ୟଥା {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", ଚାବି_ptr);
      }
    }
    ଯଦି(result)
      ଭାଙ୍ଗ;
  }

fail:
  ଯଦି(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], ଅଙ୍କ_query_components);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode parse_sigv4_params(ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ଅକ୍ଷର *hostname,
                                   ସଂରଚନା Curl_str *provider0,
                                   ସଂରଚନା Curl_str *provider1,
                                   ସଂରଚନା Curl_str *region,
                                   ସଂରଚନା Curl_str *service)
{
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  ଯଦି(!ଧାଡ଼ି || !*ଧାଡ଼ି)
    ଧାଡ଼ି = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  ଯଦି(curlx_str_until(&ଧାଡ଼ି, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଯଦି(curlx_str_single(&ଧାଡ଼ି, ':') ||
     curlx_str_until(&ଧାଡ଼ି, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  ଅନ୍ୟଥା ଯଦି(curlx_str_single(&ଧାଡ଼ି, ':') ||
          curlx_str_until(&ଧାଡ଼ି, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&ଧାଡ଼ି, ':') ||
          curlx_str_until(&ଧାଡ଼ି, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  ଯଦି(!curlx_strlen(service)) {
    ଅଚଳ ଅକ୍ଷର *p = hostname;
    ଯଦି(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(service), curlx_str(service));

    ଯଦି(!curlx_strlen(region)) {
      ଯଦି(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(region), curlx_str(region));
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode get_payload_hash(ସଂରଚନା Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 ସଂରଚନା Curl_str *provider0,
                                 ସଂରଚନା Curl_str *provider1,
                                 ସଂରଚନା Curl_str *service,
                                 unsigned ଅକ୍ଷର *sha_hash,
                                 ଅକ୍ଷର *sha_hex,
                                 ଅକ୍ଷର *content_sha256_hdr,
                                 ଅଚଳ ଅକ୍ଷର **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  ଯଦି(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    ଯଦି(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    ଅନ୍ୟଥା
      result = calc_payload_hash(data, sha_hash, sha_hex);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode get_timestamp(ଅକ୍ଷର *timestamp, size_t stampsize)
{
  time_t clock;
  ସଂରଚନା tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    ଅକ୍ଷର *force_timestamp = getenv("CURL_FORCETIME");
    ଯଦି(force_timestamp)
      clock = 0;
    ଅନ୍ୟଥା
      clock = time(ଖାଲି);
  }
#ଅନ୍ୟଥା
  clock = time(ଖାଲି);
#endif
  result = curlx_gmtime(clock, &tm);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode make_canonical_request(ସଂରଚନା Curl_easy *data,
                                       ଅକ୍ଷର *timestamp,
                                       ସଂରଚନା Curl_str *provider1,
                                       ସଂରଚନା Curl_str *service,
                                       ଅଚଳ ଅକ୍ଷର *method,
                                       ଅଚଳ ଅକ୍ଷର *payload_hash,
                                       size_t payload_hash_len,
                                       ଅକ୍ଷର **date_header_out,
                                       ଅକ୍ଷର *content_sha256_hdr,
                                       ସଂରଚନା dynbuf *canonical_headers,
                                       ସଂରଚନା dynbuf *signed_headers,
                                       ଅକ୍ଷର **canonical_request_out)
{
  ସଂରଚନା dynbuf canonical_query;
  ସଂରଚନା dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  ଯଦି(result)
    ଯାଅ fail;

  result = canon_query(data->state.up.query, &canonical_query);
  ଯଦି(result)
    ଯାଅ fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  ଯଦି(result)
    ଯାଅ fail;

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
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)payload_hash_len, payload_hash);
  ଯଦି(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode make_string_to_sign(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା Curl_str *provider0,
                                    ସଂରଚନା Curl_str *region,
                                    ସଂରଚନା Curl_str *service,
                                    ଅଚଳ ଅକ୍ଷର *date,
                                    ଅଚଳ ଅକ୍ଷର *timestamp,
                                    ଅଚଳ ଅକ୍ଷର *canonical_request,
                                    ଅକ୍ଷର **request_type_out,
                                    ଅକ୍ଷର **credential_scope_out,
                                    ଅକ୍ଷର **str_to_sign_out)
{
  ଅକ୍ଷର *request_type;
  ଅକ୍ଷର *credential_scope;
  ଅକ୍ଷର *str_to_sign;
  unsigned ଅକ୍ଷର sha_hash[CURL_SHA256_DIGEST_LENGTH];
  ଅକ୍ଷର sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(provider0),
                               curlx_str(provider0));
  ଯଦି(!request_type)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(region),
                                   curlx_str(region),
                                   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  ଯଦି(!credential_scope) {
    curlx_free(request_type);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(Curl_sha256it(sha_hash, (ଅଚଳ unsigned ଅକ୍ଷର *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
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
                              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  ଯଦି(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode sign_and_set_auth_headers(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା Curl_str *provider0,
                                          ସଂରଚନା Curl_str *region,
                                          ସଂରଚନା Curl_str *service,
                                          ଅଚଳ ଅକ୍ଷର *request_type,
                                          ଅଚଳ ଅକ୍ଷର *credential_scope,
                                          ଅଚଳ ଅକ୍ଷର *date,
                                          ଅଚଳ ଅକ୍ଷର *str_to_sign,
                                          ଅଚଳ ଅକ୍ଷର *date_header,
                                          ଅଚଳ ଅକ୍ଷର *content_sha256_hdr,
                                          ସଂରଚନା dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ଅଚଳ ଅକ୍ଷର *passwd = Curl_creds_passwd(data->state.creds);
  ଅକ୍ଷର *secret = ଖାଲି;
  unsigned ଅକ୍ଷର sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned ଅକ୍ଷର sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  ଅକ୍ଷର sha_hex[SHA256_HEX_LENGTH];
  ଅକ୍ଷର *auth_headers = ଖାଲି;
  ଅକ୍ଷର *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  ଯଦି(!user)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  ଯଦି(!secret)
    ଯାଅ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, ଆକାର(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, ଆକାର(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, ଆକାର(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, ଆକାର(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(provider0),
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
  ଯଦି(!auth_headers)
    ଯାଅ fail;

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
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_output_aws_sigv4(ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ସଂରଚନା Curl_str provider0 = { ଖାଲି, 0 };
  ସଂରଚନା Curl_str provider1 = { ଖାଲି, 0 };
  ସଂରଚନା Curl_str region = { ଖାଲି, 0 };
  ସଂରଚନା Curl_str service = { ଖାଲି, 0 };
  ଅଚଳ ଅକ୍ଷର *hostname = data->state.origin->hostname;
  ଅକ୍ଷର timestamp[TIMESTAMP_SIZE];
  ଅକ୍ଷର date[9];
  ସଂରଚନା dynbuf canonical_headers;
  ସଂରଚନା dynbuf signed_headers;
  ଅକ୍ଷର *date_header = ଖାଲି;
  Curl_HttpReq httpreq;
  ଅଚଳ ଅକ୍ଷର *method = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *payload_hash = ଖାଲି;
  size_t payload_hash_len = 0;
  unsigned ଅକ୍ଷର sha_hash[CURL_SHA256_DIGEST_LENGTH];
  ଅକ୍ଷର sha_hex[SHA256_HEX_LENGTH];
  ଅକ୍ଷର content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  ଅକ୍ଷର *canonical_request = ଖାଲି;
  ଅକ୍ଷର *request_type = ଖାଲି;
  ଅକ୍ଷର *credential_scope = ଖାଲି;
  ଅକ୍ଷର *str_to_sign = ଖାଲି;

  ଯଦି(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଯଦି(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    ଫେରନ୍ତୁ CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  ଯଦି(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  ଯଦି(!result)
    result = get_timestamp(timestamp, ଆକାର(timestamp));

  ଯଦି(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  ଯଦି(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, ଆକାର(date) - 1);
    date[ଆକାର(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  ଯଦି(!result)
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
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
