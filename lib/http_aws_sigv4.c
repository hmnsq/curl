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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "http_aws_sigv4.h"
#અન્તર્ભાવ "curl_sha256.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "slist.h"

#અન્તર્ભાવ <time.h>

#define HMAC_SHA256(k, kl, d, dl, o)                 \
  કરો {                                               \
    result = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (અચલ unsigned અક્ષર *)(k), \
                         kl,                         \
                         (અચલ unsigned અક્ષર *)(d), \
                         dl, o);                     \
    જો(result) {                                     \
      જાઓ fail;                                     \
    }                                                \
  } જ્યારે(0)

#define TIMESTAMP_SIZE 17

/* hex-encoded with null-terminator */
#define SHA256_HEX_LENGTH ((2 * CURL_SHA256_DIGEST_LENGTH) + 1)

#define MAX_QUERY_COMPONENTS 128

સંરચના pair {
  સંરચના dynbuf ચાવી;
  સંરચના dynbuf કિંમત;
};

સ્થિર શૂન્ય sha256_to_hex(અક્ષર *dst, unsigned અક્ષર *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_LENGTH,
                 (unsigned અક્ષર *)dst, SHA256_HEX_LENGTH);
}

સ્થિર અક્ષર *find_date_hdr(સંરચના Curl_easy *data, અચલ અક્ષર *sig_hdr)
{
  અક્ષર *tmp = Curl_checkheaders(data, sig_hdr, strlen(sig_hdr));

  જો(tmp)
    પાછા tmp;
  પાછા Curl_checkheaders(data, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
સ્થિર શૂન્ય trim_headers(સંરચના curl_slist *head)
{
  સંરચના curl_slist *l;
  માટે(l = head; l; l = l->next) {
    અચલ અક્ષર *કિંમત; /* to read from */
    અક્ષર *store;
    size_t colon = strcspn(l->data, ":");
    Curl_strntolower(l->data, l->data, colon);

    કિંમત = &l->data[colon];
    જો(!*કિંમત)
      ચાલુ;
    ++કિંમત;
    store = (અક્ષર *)CURL_UNCONST(કિંમત);

    /* skip leading whitespace */
    curlx_str_passblanks(&કિંમત);

    જ્યારે(*કિંમત) {
      પૂર્ણાંક space = 0;
      જ્યારે(ISBLANK(*કિંમત)) {
        કિંમત++;
        space++;
      }
      જો(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        જો(*કિંમત)
          *store++ = ' ';
      }
      નહીંતર
        *store++ = *કિંમત++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
સ્થિર શૂન્ય pair_array_free(સંરચના pair *pair_array, size_t અંક_elements)
{
  size_t index;

  માટે(index = 0; index != અંક_elements; index++) {
    curlx_dyn_free(&pair_array[index].ચાવી);
    curlx_dyn_free(&pair_array[index].કિંમત);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
સ્થિર શૂન્ય dyn_array_free(સંરચના dynbuf *db, size_t અંક_elements)
{
  size_t index;

  માટે(index = 0; index < અંક_elements; index++)
    curlx_dyn_free((&db[index]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#define SPLIT_BY '&'

સ્થિર CURLcode split_to_dyn_array(અચલ અક્ષર *source,
                                   સંરચના dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *અંક_splits_out)
{
  CURLcode result = CURLE_OK;
  size_t len = strlen(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_length = 0;
  size_t index = 0;
  size_t અંક_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  માટે(pos = 0; pos < len; pos++) {
    જો(source[pos] == SPLIT_BY) {
      જો(segment_length) {
        curlx_dyn_init(&db[index], segment_length + 1);
        result = curlx_dyn_addn(&db[index], &source[start], segment_length);
        જો(result)
          જાઓ fail;

        segment_length = 0;
        index++;
        જો(++અંક_splits == MAX_QUERY_COMPONENTS) {
          result = CURLE_TOO_LARGE;
          જાઓ fail;
        }
      }
      start = pos + 1;
    }
    નહીંતર {
      segment_length++;
    }
  }

  જો(segment_length) {
    curlx_dyn_init(&db[index], segment_length + 1);
    result = curlx_dyn_addn(&db[index], &source[start], segment_length);
    જો(!result && ++અંક_splits == MAX_QUERY_COMPONENTS)
      result = CURLE_TOO_LARGE;
  }
fail:
  *અંક_splits_out = અંક_splits;
  પાછા result;
}

સ્થિર bool is_reserved_char(અચલ અક્ષર c)
{
  પાછા (ISALNUM(c) || ISURLPUNTCS(c));
}

સ્થિર CURLcode uri_encode_path(સંરચના Curl_str *original_path,
                                સંરચના dynbuf *new_path)
{
  અચલ અક્ષર *p = curlx_str(original_path);
  size_t i;

  માટે(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode result = CURLE_OK;
    unsigned અક્ષર c = p[i];
    જો(is_reserved_char(c) || c == '/')
      result = curlx_dyn_addn(new_path, &c, 1);
    નહીંતર
      result = curlx_dyn_addf(new_path, "%%%02X", c);
    જો(result)
      પાછા result;
  }

  પાછા CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
સ્થિર CURLcode normalize_query(અચલ અક્ષર *string, size_t len,
                                સંરચના dynbuf *db)
{
  CURLcode result = CURLE_OK;

  જ્યારે(len && !result) {
    unsigned અક્ષર in = (unsigned અક્ષર)*string;
    જો(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (unsigned અક્ષર)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      જો(in == '+') {
        /* decodes to plus, so leave this encoded */
        result = curlx_dyn_addn(db, "%2B", 3);
        ચાલુ;
      }
    }
    નહીંતર {
      string++;
      len--;
    }

    જો(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      result = curlx_dyn_addn(db, &in, 1);
    નહીંતર જો(in == '+')
      /* Encode '+' as space */
      result = curlx_dyn_add(db, "%20");
    નહીંતર
      result = curlx_dyn_addf(db, "%%%02X", in);
  }

  પાછા result;
}

સ્થિર bool should_urlencode(સંરચના Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  જો(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    પાછા FALSE;
  }
  પાછા TRUE;
}

/* maximum length for the aws sivg4 parts */
#define MAX_SIGV4_LEN    64
#define DATE_HDR_ચાવી_LEN (MAX_SIGV4_LEN + કદ("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#define DATE_FULL_HDR_LEN (DATE_HDR_ચાવી_LEN + TIMESTAMP_SIZE + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
સ્થિર પૂર્ણાંક compare_header_names(અચલ અક્ષર *a, અચલ અક્ષર *b)
{
  અચલ અક્ષર *colon_a;
  અચલ અક્ષર *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  પૂર્ણાંક cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : strlen(a);
  len_b = colon_b ? (size_t)(colon_b - b) : strlen(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = strncmp(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  જો(!cmp)
    પાછા (પૂર્ણાંક)(len_a - len_b);

  પાછા cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
સ્થિર CURLcode merge_duplicate_headers(સંરચના curl_slist *head)
{
  સંરચના curl_slist *curr = head;
  CURLcode result = CURLE_OK;

  જ્યારે(curr) {
    સંરચના curl_slist *next = curr->next;
    જો(!next)
      તોડો;

    જો(compare_header_names(curr->data, next->data) == 0) {
      સંરચના dynbuf buf;
      અચલ અક્ષર *colon_next;
      અચલ અક્ષર *val_next;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      result = curlx_dyn_add(&buf, curr->data);
      જો(result)
        પાછા result;

      colon_next = strchr(next->data, ':');
      DEBUGASSERT(colon_next);
      val_next = colon_next + 1;

      result = curlx_dyn_addn(&buf, ",", 1);
      જો(result)
        પાછા result;

      result = curlx_dyn_add(&buf, val_next);
      જો(result)
        પાછા result;

      curlx_free(curr->data);
      curr->data = curlx_dyn_ptr(&buf);

      curr->next = next->next;
      curlx_free(next->data);
      curlx_free(next);
    }
    નહીંતર {
      curr = curr->next;
    }
  }

  પાછા CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
સ્થિર CURLcode make_headers(સંરચના Curl_easy *data,
                             અક્ષર *timestamp,
                             અચલ અક્ષર *provider1,
                             size_t plen, /* length of provider1 */
                             અક્ષર **date_header,
                             અક્ષર *content_sha256_header,
                             સંરચના dynbuf *canonical_headers,
                             સંરચના dynbuf *signed_headers)
{
  અક્ષર date_hdr_ચાવી[DATE_HDR_ચાવી_LEN];
  અક્ષર date_full_hdr[DATE_FULL_HDR_LEN];
  સંરચના curl_slist *head = ખાલી;
  સંરચના curl_slist *tmp_head = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  સંરચના curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_ચાવી, DATE_HDR_ચાવી_LEN, "X-%.*s-Date",
                 (પૂર્ણાંક)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_ચાવી[2], provider1, plen);
  date_hdr_ચાવી[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (પૂર્ણાંક)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  જો(!Curl_checkheaders(data, STRCONST("Host")) &&
     data->state.http_host) {
    /* Host: [host]:[port] */
    અક્ષર *fullhost = curlx_strdup(data->state.http_host);

    જો(fullhost)
      head = Curl_slist_append_nodup(ખાલી, fullhost);
    જો(!head) {
      curlx_free(fullhost);
      જાઓ fail;
    }
  }

  જો(*content_sha256_header) {
    tmp_head = curl_slist_append(head, content_sha256_header);
    જો(!tmp_head)
      જાઓ fail;
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
  માટે(l = data->set.headers; l; l = l->next) {
    અક્ષર *dupdata;
    અચલ અક્ષર *ptr;
    અચલ અક્ષર *sep = strchr(l->data, ':');
    જો(!sep)
      sep = strchr(l->data, ';');
    જો(!sep || (*sep == ':' && !*(sep + 1)))
      ચાલુ;
    માટે(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    જો(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      ચાલુ;
    dupdata = curlx_strdup(l->data);
    જો(!dupdata)
      જાઓ fail;
    dupdata[sep - l->data] = ':';
    tmp_head = Curl_slist_append_nodup(head, dupdata);
    જો(!tmp_head) {
      curlx_free(dupdata);
      જાઓ fail;
    }
    head = tmp_head;
  }

  trim_headers(head);

  *date_header = find_date_hdr(data, date_hdr_ચાવી);
  જો(!*date_header) {
    tmp_head = curl_slist_append(head, date_full_hdr);
    જો(!tmp_head)
      જાઓ fail;
    head = tmp_head;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_ચાવી, timestamp);
    જો(!*date_header)
      જાઓ fail;
  }
  નહીંતર {
    અચલ અક્ષર *કિંમત;
    અચલ અક્ષર *endp;
    કિંમત = strchr(*date_header, ':');
    જો(!કિંમત) {
      *date_header = ખાલી;
      જાઓ fail;
    }
    ++કિંમત;
    curlx_str_passblanks(&કિંમત);
    endp = કિંમત;
    જ્યારે(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    જો((endp - કિંમત) == TIMESTAMP_SIZE - 1) {
      memcpy(timestamp, કિંમત, TIMESTAMP_SIZE - 1);
      timestamp[TIMESTAMP_SIZE - 1] = 0;
    }
    નહીંતર
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = ખાલી;
  }

  /* alpha-sort by header name in a case sensitive manner */
  કરો {
    again = FALSE;
    માટે(l = head; l; l = l->next) {
      સંરચના curl_slist *next = l->next;

      જો(next && compare_header_names(l->data, next->data) > 0) {
        અક્ષર *tmp = l->data;

        l->data = next->data;
        next->data = tmp;
        again = TRUE;
      }
    }
  } જ્યારે(again);

  result = merge_duplicate_headers(head);
  જો(result)
    જાઓ fail;

  માટે(l = head; l; l = l->next) {
    અક્ષર *tmp;

    જો(curlx_dyn_add(canonical_headers, l->data))
      જાઓ fail;
    જો(curlx_dyn_add(canonical_headers, "\n"))
      જાઓ fail;

    tmp = strchr(l->data, ':');
    જો(tmp)
      *tmp = 0;

    જો(l != head && curlx_dyn_add(signed_headers, ";"))
      જાઓ fail;
    જો(curlx_dyn_add(signed_headers, l->data))
      જાઓ fail;
  }

  result = CURLE_OK;
fail:
  curl_slist_free_all(head);

  પાછા result;
}

#define CONTENT_SHA256_ચાવી_LEN (MAX_SIGV4_LEN + કદ("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#define CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_ચાવી_LEN + 2 + SHA256_HEX_LENGTH)

/* try to parse a payload hash from the content-sha256 header */
સ્થિર અચલ અક્ષર *parse_content_sha_hdr(સંરચના Curl_easy *data,
                                         અચલ અક્ષર *provider1,
                                         size_t plen,
                                         size_t *કિંમત_len)
{
  અક્ષર ચાવી[CONTENT_SHA256_ચાવી_LEN];
  size_t ચાવી_len;
  અચલ અક્ષર *કિંમત;
  size_t len;

  ચાવી_len = curl_msnprintf(ચાવી, કદ(ચાવી), "x-%.*s-content-sha256",
                           (પૂર્ણાંક)plen, provider1);

  કિંમત = Curl_checkheaders(data, ચાવી, ચાવી_len);
  જો(!કિંમત)
    પાછા ખાલી;

  કિંમત = strchr(કિંમત, ':');
  જો(!કિંમત)
    પાછા ખાલી;
  ++કિંમત;

  curlx_str_passblanks(&કિંમત);

  len = strlen(કિંમત);
  જ્યારે(len > 0 && ISBLANK(કિંમત[len - 1]))
    --len;

  *કિંમત_len = len;
  પાછા કિંમત;
}

સ્થિર CURLcode calc_payload_hash(સંરચના Curl_easy *data,
                                  unsigned અક્ષર *sha_hash, અક્ષર *sha_hex)
{
  અચલ અક્ષર *post_data = data->set.postfields;
  size_t post_data_len = 0;
  CURLcode result;

  જો(post_data) {
    જો(data->set.postfieldsize < 0)
      post_data_len = strlen(post_data);
    નહીંતર
      post_data_len = (size_t)data->set.postfieldsize;
  }
  result = Curl_sha256it(sha_hash, (અચલ unsigned અક્ષર *)post_data,
                         post_data_len);
  જો(!result)
    sha256_to_hex(sha_hex, sha_hash);
  પાછા result;
}

#define S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

સ્થિર CURLcode calc_s3_payload_hash(સંરચના Curl_easy *data,
                                     Curl_HttpReq httpreq,
                                     અચલ અક્ષર *provider1,
                                     size_t plen,
                                     unsigned અક્ષર *sha_hash,
                                     અક્ષર *sha_hex, અક્ષર *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_HEAD);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || data->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && data->set.postfields);
  CURLcode result = CURLE_OUT_OF_MEMORY;

  જો(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    result = calc_payload_hash(data, sha_hash, sha_hex);
    જો(result)
      જાઓ fail;
  }
  નહીંતર {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_LENGTH); /* 16 < 65 */
    memcpy(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (પૂર્ણાંક)plen, provider1, sha_hex);

  result = CURLE_OK;
fail:
  પાછા result;
}

સ્થિર પૂર્ણાંક compare_func(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
  અચલ સંરચના pair *aa = a;
  અચલ સંરચના pair *bb = b;
  અચલ size_t aa_ચાવી_len = curlx_dyn_len(&aa->ચાવી);
  અચલ size_t bb_ચાવી_len = curlx_dyn_len(&bb->ચાવી);
  અચલ size_t aa_કિંમત_len = curlx_dyn_len(&aa->કિંમત);
  અચલ size_t bb_કિંમત_len = curlx_dyn_len(&bb->કિંમત);
  પૂર્ણાંક compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  જો((aa_ચાવી_len == 0) && (bb_ચાવી_len == 0))
    પાછા 0;
  જો(aa_ચાવી_len == 0)
    પાછા -1;
  જો(bb_ચાવી_len == 0)
    પાછા 1;
  compare = strcmp(curlx_dyn_ptr(&aa->ચાવી), curlx_dyn_ptr(&bb->ચાવી));
  જો(compare) {
    પાછા compare;
  }

  /* Compare values */
  જો((aa_કિંમત_len == 0) && (bb_કિંમત_len == 0))
    પાછા 0;
  જો(aa_કિંમત_len == 0)
    પાછા -1;
  જો(bb_કિંમત_len == 0)
    પાછા 1;
  compare = strcmp(curlx_dyn_ptr(&aa->કિંમત), curlx_dyn_ptr(&bb->કિંમત));

  પાછા compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(અચલ અક્ષર *q, size_t len,
                             સંરચના dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(અચલ અક્ષર *q, size_t len,
                             સંરચના dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode result = CURLE_OK;

  સંરચના Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  જો(do_uri_encode)
    result = uri_encode_path(&original_path, new_path);
  નહીંતર
    result = curlx_dyn_addn(new_path, q, len);

  જો(!result && curlx_dyn_len(new_path) == 0)
    result = curlx_dyn_add(new_path, "/");

  પાછા result;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(અચલ અક્ષર *query, સંરચના dynbuf *dq);
UNITTEST CURLcode canon_query(અચલ અક્ષર *query, સંરચના dynbuf *dq)
{
  CURLcode result = CURLE_OK;

  સંરચના dynbuf query_array[MAX_QUERY_COMPONENTS];
  સંરચના pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t અંક_query_components;
  size_t counted_query_components = 0;
  size_t index;

  જો(!query)
    પાછા result;

  result = split_to_dyn_array(query, &query_array[0], &અંક_query_components);
  જો(result) {
    જાઓ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  માટે(index = 0; index < અંક_query_components; index++) {
    અચલ અક્ષર *in_ચાવી;
    size_t in_ચાવી_len;
    અચલ અક્ષર *offset;
    size_t query_part_len = curlx_dyn_len(&query_array[index]);
    અચલ અક્ષર *query_part = curlx_dyn_ptr(&query_array[index]);

    in_ચાવી = query_part;

    offset = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    જો(!offset) {
      in_ચાવી_len = strlen(in_ચાવી);
    }
    નહીંતર {
      in_ચાવી_len = offset - in_ચાવી;
    }

    curlx_dyn_init(&encoded_query_array[index].ચાવી,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[index].કિંમત,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    result = normalize_query(in_ચાવી, in_ચાવી_len,
                             &encoded_query_array[index].ચાવી);
    જો(result) {
      જાઓ fail;
    }

    /* Decode/encode the value if it exists */
    જો(offset && offset != (query_part + query_part_len - 1)) {
      size_t in_કિંમત_len;
      અચલ અક્ષર *in_કિંમત = offset + 1;
      in_કિંમત_len = query_part + query_part_len - (offset + 1);
      result = normalize_query(in_કિંમત, in_કિંમત_len,
                               &encoded_query_array[index].કિંમત);
      જો(result) {
        જાઓ fail;
      }
    }
    નહીંતર {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[index].કિંમત, 2);
      result = curlx_dyn_addn(&encoded_query_array[index].કિંમત, "", 1);
    }

    જો(result) {
      જાઓ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, અંક_query_components,
        કદ(સંરચના pair), compare_func);

  /* Append the query components together to make a full query string */
  માટે(index = 0; index < અંક_query_components; index++) {

    જો(index)
      result = curlx_dyn_addn(dq, "&", 1);
    જો(!result) {
      અચલ અક્ષર *ચાવી_ptr = curlx_dyn_ptr(&encoded_query_array[index].ચાવી);
      અચલ અક્ષર *કિંમત_ptr = curlx_dyn_ptr(&encoded_query_array[index].કિંમત);
      size_t vlen = curlx_dyn_len(&encoded_query_array[index].કિંમત);
      જો(કિંમત_ptr && vlen) {
        result = curlx_dyn_addf(dq, "%s=%s", ચાવી_ptr, કિંમત_ptr);
      }
      નહીંતર {
        /* Empty value is always encoded to key= */
        result = curlx_dyn_addf(dq, "%s=", ચાવી_ptr);
      }
    }
    જો(result)
      તોડો;
  }

fail:
  જો(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], અંક_query_components);
  પાછા result;
}

સ્થિર CURLcode parse_sigv4_params(સંરચના Curl_easy *data,
                                   અચલ અક્ષર *hostname,
                                   સંરચના Curl_str *provider0,
                                   સંરચના Curl_str *provider1,
                                   સંરચના Curl_str *region,
                                   સંરચના Curl_str *service)
{
  અચલ અક્ષર *લીટી = CURL_EASY_STR(data, STRING_AWS_SIGV4);
  જો(!લીટી || !*લીટી)
    લીટી = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  જો(curlx_str_until(&લીટી, provider0, MAX_SIGV4_LEN, ':')) {
    failf(data, "first aws-sigv4 provider cannot be empty");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  જો(curlx_str_single(&લીટી, ':') ||
     curlx_str_until(&લીટી, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  નહીંતર જો(curlx_str_single(&લીટી, ':') ||
          curlx_str_until(&લીટી, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&લીટી, ':') ||
          curlx_str_until(&લીટી, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  જો(!curlx_strlen(service)) {
    અચલ અક્ષર *p = hostname;
    જો(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(data, "aws-sigv4: service missing in parameters and hostname");
      પાછા CURLE_URL_MALFORMAT;
    }

    infof(data, "aws_sigv4: picked service %.*s from host",
          (પૂર્ણાંક)curlx_strlen(service), curlx_str(service));

    જો(!curlx_strlen(region)) {
      જો(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(data, "aws-sigv4: region missing in parameters and hostname");
        પાછા CURLE_URL_MALFORMAT;
      }
      infof(data, "aws_sigv4: picked region %.*s from host",
            (પૂર્ણાંક)curlx_strlen(region), curlx_str(region));
    }
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode get_payload_hash(સંરચના Curl_easy *data,
                                 Curl_HttpReq httpreq,
                                 સંરચના Curl_str *provider0,
                                 સંરચના Curl_str *provider1,
                                 સંરચના Curl_str *service,
                                 unsigned અક્ષર *sha_hash,
                                 અક્ષર *sha_hex,
                                 અક્ષર *content_sha256_hdr,
                                 અચલ અક્ષર **payload_hash_out,
                                 size_t *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(data, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  જો(!*payload_hash_out) {
    CURLcode result;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    જો(sign_as_s3)
      result = calc_s3_payload_hash(data, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    નહીંતર
      result = calc_payload_hash(data, sha_hash, sha_hex);
    જો(result)
      પાછા result;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = strlen(sha_hex);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode get_timestamp(અક્ષર *timestamp, size_t stampsize)
{
  time_t clock;
  સંરચના tm tm;
  CURLcode result;

#ifdef DEBUGBUILD
  {
    અક્ષર *force_timestamp = getenv("CURL_FORCETIME");
    જો(force_timestamp)
      clock = 0;
    નહીંતર
      clock = time(ખાલી);
  }
#નહીંતર
  clock = time(ખાલી);
#endif
  result = curlx_gmtime(clock, &tm);
  જો(result)
    પાછા result;

  જો(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા CURLE_OK;
}

સ્થિર CURLcode make_canonical_request(સંરચના Curl_easy *data,
                                       અક્ષર *timestamp,
                                       સંરચના Curl_str *provider1,
                                       સંરચના Curl_str *service,
                                       અચલ અક્ષર *method,
                                       અચલ અક્ષર *payload_hash,
                                       size_t payload_hash_len,
                                       અક્ષર **date_header_out,
                                       અક્ષર *content_sha256_hdr,
                                       સંરચના dynbuf *canonical_headers,
                                       સંરચના dynbuf *signed_headers,
                                       અક્ષર **canonical_request_out)
{
  સંરચના dynbuf canonical_query;
  સંરચના dynbuf canonical_path;
  CURLcode result;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  result = make_headers(data, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  જો(result)
    જાઓ fail;

  result = canon_query(data->state.up.query, &canonical_query);
  જો(result)
    જાઓ fail;

  result = canon_path(data->state.up.path, strlen(data->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  જો(result)
    જાઓ fail;

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
                  (પૂર્ણાંક)payload_hash_len, payload_hash);
  જો(!*canonical_request_out) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail;
  }

  result = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  પાછા result;
}

સ્થિર CURLcode make_string_to_sign(સંરચના Curl_easy *data,
                                    સંરચના Curl_str *provider0,
                                    સંરચના Curl_str *region,
                                    સંરચના Curl_str *service,
                                    અચલ અક્ષર *date,
                                    અચલ અક્ષર *timestamp,
                                    અચલ અક્ષર *canonical_request,
                                    અક્ષર **request_type_out,
                                    અક્ષર **credential_scope_out,
                                    અક્ષર **str_to_sign_out)
{
  અક્ષર *request_type;
  અક્ષર *credential_scope;
  અક્ષર *str_to_sign;
  unsigned અક્ષર sha_hash[CURL_SHA256_DIGEST_LENGTH];
  અક્ષર sha_hex[SHA256_HEX_LENGTH];

  request_type = curl_maprintf("%.*s4_request",
                               (પૂર્ણાંક)curlx_strlen(provider0),
                               curlx_str(provider0));
  જો(!request_type)
    પાછા CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (પૂર્ણાંક)curlx_strlen(region),
                                   curlx_str(region),
                                   (પૂર્ણાંક)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  જો(!credential_scope) {
    curlx_free(request_type);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(Curl_sha256it(sha_hash, (અચલ unsigned અક્ષર *)canonical_request,
                   strlen(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    પાછા CURLE_OUT_OF_MEMORY;
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
                              (પૂર્ણાંક)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  જો(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(data, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  પાછા CURLE_OK;
}

સ્થિર CURLcode sign_and_set_auth_headers(સંરચના Curl_easy *data,
                                          સંરચના Curl_str *provider0,
                                          સંરચના Curl_str *region,
                                          સંરચના Curl_str *service,
                                          અચલ અક્ષર *request_type,
                                          અચલ અક્ષર *credential_scope,
                                          અચલ અક્ષર *date,
                                          અચલ અક્ષર *str_to_sign,
                                          અચલ અક્ષર *date_header,
                                          અચલ અક્ષર *content_sha256_hdr,
                                          સંરચના dynbuf *signed_headers)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  અચલ અક્ષર *passwd = Curl_creds_passwd(data->state.creds);
  અક્ષર *secret = ખાલી;
  unsigned અક્ષર sign0[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  unsigned અક્ષર sign1[CURL_SHA256_DIGEST_LENGTH] = { 0 };
  અક્ષર sha_hex[SHA256_HEX_LENGTH];
  અક્ષર *auth_headers = ખાલી;
  અક્ષર *user = curl_escape(Curl_creds_user(data->state.creds), 0);
  જો(!user)
    પાછા CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (પૂર્ણાંક)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  જો(!secret)
    જાઓ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, strlen(secret), date, strlen(date), sign0);
  HMAC_SHA256(sign0, કદ(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, કદ(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, કદ(sign0),
              request_type, strlen(request_type), sign1);
  HMAC_SHA256(sign1, કદ(sign1),
              str_to_sign, strlen(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(data, "aws_sigv4: Signature - %s", sha_hex);

  auth_headers = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (પૂર્ણાંક)curlx_strlen(provider0),
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
  જો(!auth_headers)
    જાઓ fail;

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
  પાછા result;
}

CURLcode Curl_output_aws_sigv4(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;
  સંરચના Curl_str provider0 = { ખાલી, 0 };
  સંરચના Curl_str provider1 = { ખાલી, 0 };
  સંરચના Curl_str region = { ખાલી, 0 };
  સંરચના Curl_str service = { ખાલી, 0 };
  અચલ અક્ષર *hostname = data->state.origin->hostname;
  અક્ષર timestamp[TIMESTAMP_SIZE];
  અક્ષર date[9];
  સંરચના dynbuf canonical_headers;
  સંરચના dynbuf signed_headers;
  અક્ષર *date_header = ખાલી;
  Curl_HttpReq httpreq;
  અચલ અક્ષર *method = ખાલી;
  અચલ અક્ષર *payload_hash = ખાલી;
  size_t payload_hash_len = 0;
  unsigned અક્ષર sha_hash[CURL_SHA256_DIGEST_LENGTH];
  અક્ષર sha_hex[SHA256_HEX_LENGTH];
  અક્ષર content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  અક્ષર *canonical_request = ખાલી;
  અક્ષર *request_type = ખાલી;
  અક્ષર *credential_scope = ખાલી;
  અક્ષર *str_to_sign = ખાલી;

  જો(data->set.path_as_is) {
    failf(data, "Cannot use sigv4 authentication with path-as-is flag");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  જો(Curl_checkheaders(data, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    પાછા CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  result = parse_sigv4_params(data, hostname, &provider0, &provider1,
                              &region, &service);
  જો(!result) {
    Curl_http_method(data, &method, &httpreq);
    result = get_payload_hash(data, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  જો(!result)
    result = get_timestamp(timestamp, કદ(timestamp));

  જો(!result)
    result = make_canonical_request(data, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  જો(!result) {
    /* the timestamp might have been updated in make_canonical_request */
    memcpy(date, timestamp, કદ(date) - 1);
    date[કદ(date) - 1] = 0;

    result = make_string_to_sign(data, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  જો(!result)
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
  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
