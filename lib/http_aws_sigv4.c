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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "http_aws_sigv4.h"
#അന്തര്ഭാവ "curl_sha256.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "slist.h"

#അന്തര്ഭാവ <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  ചെയ്യുക {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (സ്ഥിരം unsigned അക്ഷരം *)(k), \
                         kl,                         \
                         (സ്ഥിരം unsigned അക്ഷരം *)(d), \
                         dl, o);                     \
    എങ്കിൽ(result) {                                     \
      പോകുക fail;                                     \
    }                                                \
  } ആകുമ്പോൾ(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

ഘടന pair {
  ഘടന dynbuf താക്കോൽ;
  ഘടന dynbuf മൂല്യം;
};

സ്ഥിതം ശൂന്യം sha256_to_hex(അക്ഷരം *dst, unsigned അക്ഷരം *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned അക്ഷരം *)dst, SHA256_HEX_LENGTH);
}

സ്ഥിതം അക്ഷരം *find_date_hdr(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *sig_hdr)
{
  അക്ഷരം *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  എങ്കിൽ(tmp)
    തിരികെ tmp;
  തിരികെ Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
സ്ഥിതം ശൂന്യം trim_headers(ഘടന curl_slist *head)
{
  ഘടന curl_slist *l;
  വേണ്ടി(l = head; l; l = l->next) {
    സ്ഥിരം അക്ഷരം *മൂല്യം; /* to read from */
    അക്ഷരം *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    മൂല്യം = &l->data[colon];
    എങ്കിൽ(!*മൂല്യം)
      തുടരുക;
    ++മൂല്യം;
    store = (അക്ഷരം *)CURL_UNCONST(മൂല്യം);

    /* skip leading whitespace */
    curlx_str_passblanks(&മൂല്യം);

    ആകുമ്പോൾ(*മൂല്യം) {
      പൂർണ്ണസംഖ്യ space = 0;
      ആകുമ്പോൾ(ISBLANK(*മൂല്യം)) {
        മൂല്യം++;
        space++;
      }
      എങ്കിൽ(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        എങ്കിൽ(*മൂല്യം)
          *store++ = ' ';
      }
      അല്ലെങ്കിൽ
        *store++ = *മൂല്യം++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
സ്ഥിതം ശൂന്യം pair_array_free(ഘടന pair *pair_array, size_t അക്കം_elements)
{
  size_t index;

  വേണ്ടി(index = 0; index != അക്കം_elements; index++) {
    curlx_dyn_free(&pair_array[index].താക്കോൽ);
    curlx_dyn_free(&pair_array[index].മൂല്യം);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
സ്ഥിതം ശൂന്യം dyn_array_free(ഘടന dynbuf *db, size_t അക്കം_elements)
{
  size_t index;

  വേണ്ടി(index = 0; index < അക്കം_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

സ്ഥിതം CURLcode split_to_dyn_array(സ്ഥിരം അക്ഷരം *source,
                                   ഘടന dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *അക്കം_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t അക്കം_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  വേണ്ടി(pos = 0; pos < len; pos++) {
    എങ്കിൽ(source[pos] == SPLIT_BY) {
      എങ്കിൽ(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        എങ്കിൽ(result)
          പോകുക fail;

        segment_length = 0;
        index++;
        എങ്കിൽ(++അക്കം_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          പോകുക fail;
        }
      }
      start = pos + 1;
    }
    അല്ലെങ്കിൽ {
      segment_length++;
    }
  }

  എങ്കിൽ(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    എങ്കിൽ(!result && ++അക്കം_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *അക്കം_splits_out = അക്കം_splits;
  തിരികെ result;
}

സ്ഥിതം bool is_reserved_char(സ്ഥിരം അക്ഷരം c)
{
  തിരികെ (ISALNUM(c) || ISURLPUNTCS(c));
}

സ്ഥിതം CURLcode uri_encode_path(ഘടന Curl_str *original_path,
                                ഘടന dynbuf *new_path)
{
  സ്ഥിരം അക്ഷരം *p = curlx_str(original_path);
  size_t i;

  വേണ്ടി(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned അക്ഷരം c = p[i];
    എങ്കിൽ(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    അല്ലെങ്കിൽ
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  തിരികെ CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
സ്ഥിതം CURLcode normalize_query(സ്ഥിരം അക്ഷരം *string, size_t len,
                                ഘടന dynbuf *db)
{
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(len && !result) {
    unsigned അക്ഷരം in = (unsigned അക്ഷരം)*string;
    എങ്കിൽ(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned അക്ഷരം)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      എങ്കിൽ(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        തുടരുക;
      }
    }
    അല്ലെങ്കിൽ {
      string++;
      len--;
    }

    എങ്കിൽ(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    അല്ലെങ്കിൽ എങ്കിൽ(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    അല്ലെങ്കിൽ
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  തിരികെ result;
}

സ്ഥിതം bool should_urlencode(ഘടന Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  എങ്കിൽ(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    തിരികെ FALSE;
  }
  തിരികെ TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_താക്കോൽ_LEN (MAX_SIGV4_LEN + വലുപ്പം("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_താക്കോൽ_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
സ്ഥിതം പൂർണ്ണസംഖ്യ compare_header_names(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b)
{
  സ്ഥിരം അക്ഷരം *colon_a;
  സ്ഥിരം അക്ഷരം *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  പൂർണ്ണസംഖ്യ cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  എങ്കിൽ(!cmp)
    തിരികെ (പൂർണ്ണസംഖ്യ)(len_a - len_b);

  തിരികെ cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
സ്ഥിതം CURLcode merge_duplicate_headers(ഘടന curl_slist *head)
{
  ഘടന curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(curr) {
    ഘടന curl_slist *next = curr->next;
    എങ്കിൽ(!next)
      നിർത്തുക;

    എങ്കിൽ(compare_header_names(curr->data, next->data) == 0) {
      ഘടന dynbuf buf;
      സ്ഥിരം അക്ഷരം *colon_next;
      സ്ഥിരം അക്ഷരം *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      എങ്കിൽ(result)
        തിരികെ result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      എങ്കിൽ(result)
        തിരികെ result;

      result = curlx_dyn_add(&buf, val_next);
      എങ്കിൽ(result)
        തിരികെ result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    അല്ലെങ്കിൽ {
      curr = curr->next;
    }
  }

  തിരികെ CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
സ്ഥിതം CURLcode make_headers(ഘടന Curl_easy *data,
                             അക്ഷരം *timestamp,
                             സ്ഥിരം അക്ഷരം *provider1,
                             size_t plen, /* length of provider1 */
                             അക്ഷരം **date_header,
                             അക്ഷരം *content_sha256_header,
                             ഘടന dynbuf *canonical_headers,
                             ഘടന dynbuf *signed_headers)
{
  അക്ഷരം date_hdr_താക്കോൽ[DATE_HDR_താക്കോൽ_LEN];
  അക്ഷരം date_full_hdr[DATE_FULL_HDR_LEN];
  ഘടന curl_slist *head = ശൂന്യ;
  ഘടന curl_slist *tmp_head = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ഘടന curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_താക്കോൽ, DATE_HDR_താക്കോൽ_LEN, "X-%.*s-Date",
                 (പൂർണ്ണസംഖ്യ)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_താക്കോൽ[2], provider1, plen);
  date_hdr_താക്കോൽ[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (പൂർണ്ണസംഖ്യ)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  എങ്കിൽ(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    അക്ഷരം *fullhost = curlx_strdup(data->state.http_host);

    എങ്കിൽ(fullhost)
      head = Curl_slist_append_nodup(ശൂന്യ, fullhost);
    എങ്കിൽ(!head) {
      curlx_free(fullhost);
      പോകുക fail;
    }
  }

  എങ്കിൽ(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    എങ്കിൽ(!tmp_head)
      പോകുക fail;
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
  വേണ്ടി(l = data->set.headers; l; l = l->next) {
    അക്ഷരം *dupdata;
    സ്ഥിരം അക്ഷരം *ptr;
    സ്ഥിരം അക്ഷരം *sep = strchr(l->data, ':');
    എങ്കിൽ(!sep)
      sep = strchr(l->data, ';');
    എങ്കിൽ(!sep || (*sep == ':' && !*(sep + 1)))
      തുടരുക;
    വേണ്ടി(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    എങ്കിൽ(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      തുടരുക;
    dupdata = curlx_strdup(l->data);
    എങ്കിൽ(!dupdata)
      പോകുക fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    എങ്കിൽ(!tmp_head) {
      curlx_free(dupdata);
      പോകുക fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_താക്കോൽ);
  എങ്കിൽ(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    എങ്കിൽ(!tmp_head)
      പോകുക fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_താക്കോൽ, timestamp);
    എങ്കിൽ(!*date_header)
      പോകുക fail;
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം അക്ഷരം *മൂല്യം;
    സ്ഥിരം അക്ഷരം *endp;
    മൂല്യം = strchr(*date_header, ':');
    എങ്കിൽ(!മൂല്യം) {
      *date_header = ശൂന്യ;
      പോകുക fail;
    }
    ++മൂല്യം;
    curlx_str_passblanks(&മൂല്യം);
    endp = മൂല്യം;
    ആകുമ്പോൾ(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    എങ്കിൽ((endp - മൂല്യം) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, മൂല്യം, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    അല്ലെങ്കിൽ
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = ശൂന്യ;
  }

  /* alpha-sort by header name in a case sensitive manner */
  ചെയ്യുക {
    again = FALSE;
    വേണ്ടി(l = head; l; l = l->next) {
      ഘടന curl_slist *next = l->next;

      എങ്കിൽ(next && compare_header_names(l->data, next->data) > 0) {
        അക്ഷരം *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } ആകുമ്പോൾ(again);

  result = merge_duplicate_headers(head);
  എങ്കിൽ(result)
    പോകുക fail;

  വേണ്ടി(l = head; l; l = l->next) {
    അക്ഷരം *tmp;

    എങ്കിൽ(curlx_dyn_add(canonical_headers, l->data))
      പോകുക fail;
    എങ്കിൽ(curlx_dyn_add(canonical_headers, "\n"))
      പോകുക fail;

    tmp = strchr(l->data, ':');
    എങ്കിൽ(tmp)
      *tmp = 0;

    എങ്കിൽ(l != head && curlx_dyn_add(signed_headers, ";"))
      പോകുക fail;
    എങ്കിൽ(curlx_dyn_add(signed_headers, l->data))
      പോകുക fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  തിരികെ result;
}

#define CONTENT_SHA256_താക്കോൽ_LEN (MAX_SIGV4_LEN + വലുപ്പം("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_താക്കോൽ_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
സ്ഥിതം സ്ഥിരം അക്ഷരം *parse_content_sha_hdr(ഘടന Curl_easy *data,
                                         സ്ഥിരം അക്ഷരം *provider1,
                                         size_t plen,
                                         size_t *മൂല്യം_len)
{
  അക്ഷരം താക്കോൽ[CONTENT_SHA256_താക്കോൽ_LEN];
  size_t താക്കോൽ_len;
  സ്ഥിരം അക്ഷരം *മൂല്യം;
  size_t len;

  താക്കോൽ_len = curl_msnprintf(താക്കോൽ, വലുപ്പം(താക്കോൽ), "x-%.*s-content-sha256",
                           (പൂർണ്ണസംഖ്യ)plen, provider1);

  മൂല്യം = Curl_checkheaders(data, താക്കോൽ, താക്കോൽ_len);
  എങ്കിൽ(!മൂല്യം)
    തിരികെ ശൂന്യ;

  മൂല്യം = strchr(മൂല്യം, ':');
  എങ്കിൽ(!മൂല്യം)
    തിരികെ ശൂന്യ;
  ++മൂല്യം;

  curlx_str_passblanks(&മൂല്യം);

  len = strlen(മൂല്യം);
  ആകുമ്പോൾ(len > 0 && ISBLANK(മൂല്യം[len - 1]))
    --len;

  *മൂല്യം_len = len;
  തിരികെ മൂല്യം;
}

സ്ഥിതം CURLcode calc_payload_hash(ഘടന Curl_easy *data,
                                  unsigned അക്ഷരം *sha_hash, അക്ഷരം *sha_hex)
{
  സ്ഥിരം അക്ഷരം *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  എങ്കിൽ(post_data) {
    എങ്കിൽ(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    അല്ലെങ്കിൽ
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (സ്ഥിരം unsigned അക്ഷരം *)post_data,
                         post_data_len);
  എങ്കിൽ(!result)
    sha256_to_hex(sha_hex, sha_hash);
  തിരികെ result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

സ്ഥിതം CURLcode calc_s3_payload_hash(ഘടന Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     സ്ഥിരം അക്ഷരം *provider1,
                                     size_t plen,
                                     unsigned അക്ഷരം *sha_hash,
                                     അക്ഷരം *sha_hex, അക്ഷരം *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    എങ്കിൽ(result)
      പോകുക fail;
  }
  അല്ലെങ്കിൽ {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (പൂർണ്ണസംഖ്യ)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compare_func(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
{
  സ്ഥിരം ഘടന pair *aa = a;
  സ്ഥിരം ഘടന pair *bb = b;
  സ്ഥിരം size_t aa_താക്കോൽ_len = curlx_dyn_len(&aa->താക്കോൽ);
  സ്ഥിരം size_t bb_താക്കോൽ_len = curlx_dyn_len(&bb->താക്കോൽ);
  സ്ഥിരം size_t aa_മൂല്യം_len = curlx_dyn_len(&aa->മൂല്യം);
  സ്ഥിരം size_t bb_മൂല്യം_len = curlx_dyn_len(&bb->മൂല്യം);
  പൂർണ്ണസംഖ്യ compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  എങ്കിൽ((aa_താക്കോൽ_len == 0) && (bb_താക്കോൽ_len == 0))
    തിരികെ 0;
  എങ്കിൽ(aa_താക്കോൽ_len == 0)
    തിരികെ -1;
  എങ്കിൽ(bb_താക്കോൽ_len == 0)
    തിരികെ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->താക്കോൽ), curlx_dyn_ptr(&bb->താക്കോൽ));
  എങ്കിൽ(compare) {
    തിരികെ compare;
  }

  /* Compare values */
  എങ്കിൽ((aa_മൂല്യം_len == 0) && (bb_മൂല്യം_len == 0))
    തിരികെ 0;
  എങ്കിൽ(aa_മൂല്യം_len == 0)
    തിരികെ -1;
  എങ്കിൽ(bb_മൂല്യം_len == 0)
    തിരികെ 1;
  compare = strcmp(curlx_dyn_ptr(&aa->മൂല്യം), curlx_dyn_ptr(&bb->മൂല്യം));

  തിരികെ compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(സ്ഥിരം അക്ഷരം *q, size_t len,
                             ഘടന dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(സ്ഥിരം അക്ഷരം *q, size_t len,
                             ഘടന dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  ഘടന Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  എങ്കിൽ(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  അല്ലെങ്കിൽ
    result = curlx_dyn_addn(new_path, q, len);

  എങ്കിൽ(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  തിരികെ result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(സ്ഥിരം അക്ഷരം *query, ഘടന dynbuf *dq);
UNITTEST CURLcode canon_query(സ്ഥിരം അക്ഷരം *query, ഘടന dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  ഘടന dynbuf query_array[MAX_QUERY_COMPONENTS];
  ഘടന pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t അക്കം_query_components;
  size_t counted_query_components = 0;
  size_t index;

  എങ്കിൽ(!query)
    തിരികെ result;

  result = split_to_dyn_array(query, &query_array[0], &അക്കം_query_components);
  എങ്കിൽ(result) {
    പോകുക fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  വേണ്ടി(index = 0; index < അക്കം_query_components; index++) {
    സ്ഥിരം അക്ഷരം *in_താക്കോൽ;
    size_t in_താക്കോൽ_len;
    സ്ഥിരം അക്ഷരം *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    സ്ഥിരം അക്ഷരം *query_part = curlx_dyn_ptr(&query_array[index]);

    in_താക്കോൽ = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    എങ്കിൽ(!offset) {
      in_താക്കോൽ_len = strlen(in_താക്കോൽ);
    }
    അല്ലെങ്കിൽ {
      in_താക്കോൽ_len = offset - in_താക്കോൽ;
    }

    curlx_dyn_init(&encoded_query_array[index].താക്കോൽ,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].മൂല്യം,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_താക്കോൽ, in_താക്കോൽ_len,
                             &encoded_query_array[index].താക്കോൽ);
    എങ്കിൽ(result) {
      പോകുക fail;
    }

    /* Decode/encode the value if it exists */
    എങ്കിൽ(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_മൂല്യം_len;
      സ്ഥിരം അക്ഷരം *in_മൂല്യം = offset + 1;
      in_മൂല്യം_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_മൂല്യം, in_മൂല്യം_len,
                               &encoded_query_array[index].മൂല്യം);
      എങ്കിൽ(result) {
        പോകുക fail;
      }
    }
    അല്ലെങ്കിൽ {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].മൂല്യം, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].മൂല്യം, "", 1);
    }

    എങ്കിൽ(result) {
      പോകുക fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, അക്കം_query_components,
        വലുപ്പം(ഘടന pair), compare_func);

  /* Append the query components together to make a full query string */
  വേണ്ടി(index = 0; index < അക്കം_query_components; index++) {

    എങ്കിൽ(index)
      result = curlx_dyn_addn(dq, "&", 1);
    എങ്കിൽ(!result) {
      സ്ഥിരം അക്ഷരം *താക്കോൽ_ptr = curlx_dyn_ptr(&encoded_query_array[index].താക്കോൽ);
      സ്ഥിരം അക്ഷരം *മൂല്യം_ptr = curlx_dyn_ptr(&encoded_query_array[index].മൂല്യം);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].മൂല്യം);
      എങ്കിൽ(മൂല്യം_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", താക്കോൽ_ptr, മൂല്യം_ptr);
      }
      അല്ലെങ്കിൽ {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", താക്കോൽ_ptr);
      }
    }
    എങ്കിൽ(result)
      നിർത്തുക;
  }

fail:
  എങ്കിൽ(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], അക്കം_query_components);
  തിരികെ result;
}

സ്ഥിതം CURLcode parse_sigv4_params(ഘടന Curl_easy *data,
                                   സ്ഥിരം അക്ഷരം *hostname,
                                   ഘടന Curl_str *provider0,
                                   ഘടന Curl_str *provider1,
                                   ഘടന Curl_str *region,
                                   ഘടന Curl_str *service)
{
  സ്ഥിരം അക്ഷരം *വരി = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  എങ്കിൽ(!വരി || !*വരി)
    വരി = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  എങ്കിൽ(curlx_str_until(&വരി, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  എങ്കിൽ(curlx_str_single(&വരി, ':') ||
     curlx_str_until(&വരി, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_single(&വരി, ':') ||
          curlx_str_until(&വരി, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&വരി, ':') ||
          curlx_str_until(&വരി, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  എങ്കിൽ(!curlx_strlen(service)) {
    സ്ഥിരം അക്ഷരം *p = hostname;
    എങ്കിൽ(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      തിരികെ CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (പൂർണ്ണസംഖ്യ)curlx_strlen(service), curlx_str(service));

    എങ്കിൽ(!curlx_strlen(region)) {
      എങ്കിൽ(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        തിരികെ CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (പൂർണ്ണസംഖ്യ)curlx_strlen(region), curlx_str(region));
    }
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode get_payload_hash(ഘടന Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 ഘടന Curl_str *provider0,
                                 ഘടന Curl_str *provider1,
                                 ഘടന Curl_str *service,
                                 unsigned അക്ഷരം *sha_hash,
                                 അക്ഷരം *sha_hex,
                                 അക്ഷരം *content_sha256_hdr,
                                 സ്ഥിരം അക്ഷരം **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  എങ്കിൽ(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    എങ്കിൽ(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    അല്ലെങ്കിൽ
      result = calc_payload_hash(data, sha_hash, sha_hex);
    എങ്കിൽ(result)
      തിരികെ result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode get_timestamp(അക്ഷരം *timestamp, size_t stampsize)
{
  time_t clock;
  ഘടന tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    അക്ഷരം *force_timestamp = getenv("CURL_FORCETIME");
    എങ്കിൽ(force_timestamp)
      clock = 0;
    അല്ലെങ്കിൽ
      clock = time(ശൂന്യ);
  }
#അല്ലെങ്കിൽ
  clock = time(ശൂന്യ);
#endif
  result = curlx_gmtime(clock, &tm);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode make_canonical_request(ഘടന Curl_easy *data,
                                       അക്ഷരം *timestamp,
                                       ഘടന Curl_str *provider1,
                                       ഘടന Curl_str *service,
                                       സ്ഥിരം അക്ഷരം *method,
                                       സ്ഥിരം അക്ഷരം *payload_hash,
                                       size_t payload_hash_len,
                                       അക്ഷരം **date_header_out,
                                       അക്ഷരം *content_sha256_hdr,
                                       ഘടന dynbuf *canonical_headers,
                                       ഘടന dynbuf *signed_headers,
                                       അക്ഷരം **canonical_request_out)
{
  ഘടന dynbuf canonical_query;
  ഘടന dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  എങ്കിൽ(result)
    പോകുക fail;

  result = canon_query(data->state.up.query, &canonical_query);
  എങ്കിൽ(result)
    പോകുക fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  എങ്കിൽ(result)
    പോകുക fail;

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
                  (പൂർണ്ണസംഖ്യ)payload_hash_len, payload_hash);
  എങ്കിൽ(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  തിരികെ result;
}

സ്ഥിതം CURLcode make_string_to_sign(ഘടന Curl_easy *data,
                                    ഘടന Curl_str *provider0,
                                    ഘടന Curl_str *region,
                                    ഘടന Curl_str *service,
                                    സ്ഥിരം അക്ഷരം *date,
                                    സ്ഥിരം അക്ഷരം *timestamp,
                                    സ്ഥിരം അക്ഷരം *canonical_request,
                                    അക്ഷരം **request_type_out,
                                    അക്ഷരം **credential_scope_out,
                                    അക്ഷരം **str_to_sign_out)
{
  അക്ഷരം *request_type;
  അക്ഷരം *credential_scope;
  അക്ഷരം *str_to_sign;
  unsigned അക്ഷരം sha_hash[CURL_SHA256_DIGEST_LENGTH];
  അക്ഷരം sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (പൂർണ്ണസംഖ്യ)curlx_strlen(provider0),
                               curlx_str(provider0));
  എങ്കിൽ(!request_type)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (പൂർണ്ണസംഖ്യ)curlx_strlen(region),
                                   curlx_str(region),
                                   (പൂർണ്ണസംഖ്യ)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  എങ്കിൽ(!credential_scope) {
    curlx_free(request_type);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(Curl_sha256it(sha_hash, (സ്ഥിരം unsigned അക്ഷരം *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    തിരികെ CURLE_OUT_OF_MEMORY;
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
                              (പൂർണ്ണസംഖ്യ)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  എങ്കിൽ(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode sign_and_set_auth_headers(ഘടന Curl_easy *data,
                                          ഘടന Curl_str *provider0,
                                          ഘടന Curl_str *region,
                                          ഘടന Curl_str *service,
                                          സ്ഥിരം അക്ഷരം *request_type,
                                          സ്ഥിരം അക്ഷരം *credential_scope,
                                          സ്ഥിരം അക്ഷരം *date,
                                          സ്ഥിരം അക്ഷരം *str_to_sign,
                                          സ്ഥിരം അക്ഷരം *date_header,
                                          സ്ഥിരം അക്ഷരം *content_sha256_hdr,
                                          ഘടന dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  സ്ഥിരം അക്ഷരം *passwd = Curl_creds_passwd(data->state.creds);
  അക്ഷരം *secret = ശൂന്യ;
  unsigned അക്ഷരം sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned അക്ഷരം sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  അക്ഷരം sha_hex[SHA256_HEX_LENGTH];
  അക്ഷരം *auth_headers = ശൂന്യ;
  അക്ഷരം *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  എങ്കിൽ(!user)
    തിരികെ CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (പൂർണ്ണസംഖ്യ)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  എങ്കിൽ(!secret)
    പോകുക fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, വലുപ്പം(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, വലുപ്പം(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, വലുപ്പം(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, വലുപ്പം(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (പൂർണ്ണസംഖ്യ)curlx_strlen(provider0),
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
  എങ്കിൽ(!auth_headers)
    പോകുക fail;

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
  തിരികെ result;
}

CURLcode Curl_output_aws_sigv4(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ഘടന Curl_str provider0 = { ശൂന്യ, 0 };
  ഘടന Curl_str provider1 = { ശൂന്യ, 0 };
  ഘടന Curl_str region = { ശൂന്യ, 0 };
  ഘടന Curl_str service = { ശൂന്യ, 0 };
  സ്ഥിരം അക്ഷരം *hostname = data->state.origin->hostname;
  അക്ഷരം timestamp[TIMESTAMP_SIZE];
  അക്ഷരം date[9];
  ഘടന dynbuf canonical_headers;
  ഘടന dynbuf signed_headers;
  അക്ഷരം *date_header = ശൂന്യ;
  Curl_HttpReq httpreq;
  സ്ഥിരം അക്ഷരം *method = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *payload_hash = ശൂന്യ;
  size_t payload_hash_len = 0;
  unsigned അക്ഷരം sha_hash[CURL_SHA256_DIGEST_LENGTH];
  അക്ഷരം sha_hex[SHA256_HEX_LENGTH];
  അക്ഷരം content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  അക്ഷരം *canonical_request = ശൂന്യ;
  അക്ഷരം *request_type = ശൂന്യ;
  അക്ഷരം *credential_scope = ശൂന്യ;
  അക്ഷരം *str_to_sign = ശൂന്യ;

  എങ്കിൽ(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  എങ്കിൽ(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    തിരികെ CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  എങ്കിൽ(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  എങ്കിൽ(!result)
    result = get_timestamp(timestamp, വലുപ്പം(timestamp));

  എങ്കിൽ(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  എങ്കിൽ(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, വലുപ്പം(date) - 1);
    date[വലുപ്പം(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  എങ്കിൽ(!result)
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
  തിരികെ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
