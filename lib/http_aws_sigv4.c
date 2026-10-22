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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#समावेश "urldata.h"
#समावेश "strcase.h"
#समावेश "curlx/strdup.h"
#समावेश "http_aws_sigv4.h"
#समावेश "curl_sha256.h"
#समावेश "transfer.h"
#समावेश "curl_trc.h"
#समावेश "escape.h"
#समावेश "curlx/strparse.h"
#समावेश "slist.h"

#समावेश <time.h>

#घोषणा HMAC_SHA256(k, kl, d, dl, o)                 \
  करो {                                               \
    परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (स्थिर अचिन्हित अक्षर *)(k), \
                         kl,                         \
                         (स्थिर अचिन्हित अक्षर *)(d), \
                         dl, o);                     \
    अगर(परिणाम) {                                     \
      जाओ fail;                                     \
    }                                                \
  } जबतक(0)

#घोषणा TIMESTAMP_आकार 17

/* hex-encoded with null-terminator */
#घोषणा SHA256_HEX_लंबाई ((2 * CURL_SHA256_DIGEST_लंबाई) + 1)

#घोषणा MAX_QUERY_COMPONENTS 128

काष्ठा pair {
  काष्ठा dynbuf कुंजी;
  काष्ठा dynbuf मूल्य;
};

अटल व्योम sha256_to_hex(अक्षर *dst, अचिन्हित अक्षर *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_लंबाई,
                 (अचिन्हित अक्षर *)dst, SHA256_HEX_लंबाई);
}

अटल अक्षर *find_date_hdr(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *sig_hdr)
{
  अक्षर *tmp = Curl_checkheaders(तथ्य, sig_hdr, म_माप(sig_hdr));

  अगर(tmp)
    वापस tmp;
  वापस Curl_checkheaders(तथ्य, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
अटल व्योम trim_headers(काष्ठा curl_slist *शीर्ष)
{
  काष्ठा curl_slist *l;
  क्रम(l = शीर्ष; l; l = l->अग्र) {
    स्थिर अक्षर *मूल्य; /* to read from */
    अक्षर *store;
    माप_प्रकार colon = म_खोज(l->तथ्य, ":");
    Curl_strntolower(l->तथ्य, l->तथ्य, colon);

    मूल्य = &l->तथ्य[colon];
    अगर(!*मूल्य)
      जारी;
    ++मूल्य;
    store = (अक्षर *)CURL_UNCONST(मूल्य);

    /* skip leading whitespace */
    curlx_str_passblanks(&मूल्य);

    जबतक(*मूल्य) {
      पूर्णांक space = 0;
      जबतक(ISBLANK(*मूल्य)) {
        मूल्य++;
        space++;
      }
      अगर(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        अगर(*मूल्य)
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
अटल व्योम pair_array_free(काष्ठा pair *pair_array, माप_प्रकार अंक_elements)
{
  माप_प्रकार सूचकांक;

  क्रम(सूचकांक = 0; सूचकांक != अंक_elements; सूचकांक++) {
    curlx_dyn_free(&pair_array[सूचकांक].कुंजी);
    curlx_dyn_free(&pair_array[सूचकांक].मूल्य);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
अटल व्योम dyn_array_free(काष्ठा dynbuf *db, माप_प्रकार अंक_elements)
{
  माप_प्रकार सूचकांक;

  क्रम(सूचकांक = 0; सूचकांक < अंक_elements; सूचकांक++)
    curlx_dyn_free((&db[सूचकांक]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#घोषणा SPLIT_BY '&'

अटल CURLcode split_to_dyn_array(स्थिर अक्षर *source,
                                   काष्ठा dynbuf db[MAX_QUERY_COMPONENTS],
                                   माप_प्रकार *अंक_splits_out)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार len = म_माप(source);
  माप_प्रकार pos;         /* Position in result buffer */
  माप_प्रकार start = 0;   /* Start of current segment */
  माप_प्रकार segment_लंबाई = 0;
  माप_प्रकार सूचकांक = 0;
  माप_प्रकार अंक_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  क्रम(pos = 0; pos < len; pos++) {
    अगर(source[pos] == SPLIT_BY) {
      अगर(segment_लंबाई) {
        curlx_dyn_init(&db[सूचकांक], segment_लंबाई + 1);
        परिणाम = curlx_dyn_addn(&db[सूचकांक], &source[start], segment_लंबाई);
        अगर(परिणाम)
          जाओ fail;

        segment_लंबाई = 0;
        सूचकांक++;
        अगर(++अंक_splits == MAX_QUERY_COMPONENTS) {
          परिणाम = CURLE_TOO_LARGE;
          जाओ fail;
        }
      }
      start = pos + 1;
    }
    अन्यथा {
      segment_लंबाई++;
    }
  }

  अगर(segment_लंबाई) {
    curlx_dyn_init(&db[सूचकांक], segment_लंबाई + 1);
    परिणाम = curlx_dyn_addn(&db[सूचकांक], &source[start], segment_लंबाई);
    अगर(!परिणाम && ++अंक_splits == MAX_QUERY_COMPONENTS)
      परिणाम = CURLE_TOO_LARGE;
  }
fail:
  *अंक_splits_out = अंक_splits;
  वापस परिणाम;
}

अटल bool is_reserved_char(स्थिर अक्षर c)
{
  वापस (ISALNUM(c) || ISURLPUNTCS(c));
}

अटल CURLcode uri_encode_path(काष्ठा Curl_str *original_path,
                                काष्ठा dynbuf *new_path)
{
  स्थिर अक्षर *p = curlx_str(original_path);
  माप_प्रकार i;

  क्रम(i = 0; i < curlx_strlen(original_path); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode परिणाम = CURLE_OK;
    अचिन्हित अक्षर c = p[i];
    अगर(is_reserved_char(c) || c == '/')
      परिणाम = curlx_dyn_addn(new_path, &c, 1);
    अन्यथा
      परिणाम = curlx_dyn_addf(new_path, "%%%02X", c);
    अगर(परिणाम)
      वापस परिणाम;
  }

  वापस CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
अटल CURLcode normalize_query(स्थिर अक्षर *string, माप_प्रकार len,
                                काष्ठा dynbuf *db)
{
  CURLcode परिणाम = CURLE_OK;

  जबतक(len && !परिणाम) {
    अचिन्हित अक्षर in = (अचिन्हित अक्षर)*string;
    अगर(('%' == in) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      in = (अचिन्हित अक्षर)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      अगर(in == '+') {
        /* decodes to plus, so leave this encoded */
        परिणाम = curlx_dyn_addn(db, "%2B", 3);
        जारी;
      }
    }
    अन्यथा {
      string++;
      len--;
    }

    अगर(is_reserved_char(in))
      /* Escape unreserved chars from RFC 3986 */
      परिणाम = curlx_dyn_addn(db, &in, 1);
    अन्यथा अगर(in == '+')
      /* Encode '+' as space */
      परिणाम = curlx_dyn_add(db, "%20");
    अन्यथा
      परिणाम = curlx_dyn_addf(db, "%%%02X", in);
  }

  वापस परिणाम;
}

अटल bool should_urlencode(काष्ठा Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  अगर(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    वापस FALSE;
  }
  वापस TRUE;
}

/* maximum length for the aws sivg4 parts */
#घोषणा MAX_SIGV4_LEN    64
#घोषणा DATE_HDR_कुंजी_LEN (MAX_SIGV4_LEN + माप("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#घोषणा DATE_FULL_HDR_LEN (DATE_HDR_कुंजी_LEN + TIMESTAMP_आकार + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
अटल पूर्णांक compare_header_names(स्थिर अक्षर *a, स्थिर अक्षर *b)
{
  स्थिर अक्षर *colon_a;
  स्थिर अक्षर *colon_b;
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
  अगर(!cmp)
    वापस (पूर्णांक)(len_a - len_b);

  वापस cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
अटल CURLcode merge_duplicate_headers(काष्ठा curl_slist *शीर्ष)
{
  काष्ठा curl_slist *curr = शीर्ष;
  CURLcode परिणाम = CURLE_OK;

  जबतक(curr) {
    काष्ठा curl_slist *अग्र = curr->अग्र;
    अगर(!अग्र)
      अवरोध;

    अगर(compare_header_names(curr->तथ्य, अग्र->तथ्य) == 0) {
      काष्ठा dynbuf buf;
      स्थिर अक्षर *colon_अग्र;
      स्थिर अक्षर *val_अग्र;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_HEADER);

      परिणाम = curlx_dyn_add(&buf, curr->तथ्य);
      अगर(परिणाम)
        वापस परिणाम;

      colon_अग्र = म_अक्षर(अग्र->तथ्य, ':');
      DEBUGASSERT(colon_अग्र);
      val_अग्र = colon_अग्र + 1;

      परिणाम = curlx_dyn_addn(&buf, ",", 1);
      अगर(परिणाम)
        वापस परिणाम;

      परिणाम = curlx_dyn_add(&buf, val_अग्र);
      अगर(परिणाम)
        वापस परिणाम;

      curlx_free(curr->तथ्य);
      curr->तथ्य = curlx_dyn_ptr(&buf);

      curr->अग्र = अग्र->अग्र;
      curlx_free(अग्र->तथ्य);
      curlx_free(अग्र);
    }
    अन्यथा {
      curr = curr->अग्र;
    }
  }

  वापस CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
अटल CURLcode make_headers(काष्ठा Curl_easy *तथ्य,
                             अक्षर *timestamp,
                             स्थिर अक्षर *provider1,
                             माप_प्रकार plen, /* length of provider1 */
                             अक्षर **date_header,
                             अक्षर *content_sha256_header,
                             काष्ठा dynbuf *canonical_headers,
                             काष्ठा dynbuf *signed_headers)
{
  अक्षर date_hdr_कुंजी[DATE_HDR_कुंजी_LEN];
  अक्षर date_full_hdr[DATE_FULL_HDR_LEN];
  काष्ठा curl_slist *शीर्ष = शून्य;
  काष्ठा curl_slist *tmp_शीर्ष = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  काष्ठा curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_कुंजी, DATE_HDR_कुंजी_LEN, "X-%.*s-Date",
                 (पूर्णांक)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_कुंजी[2], provider1, plen);
  date_hdr_कुंजी[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_full_hdr, DATE_FULL_HDR_LEN,
                 "x-%.*s-date:%s", (पूर्णांक)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_full_hdr[2], provider1, plen);

  अगर(!Curl_checkheaders(तथ्य, STRCONST("Host")) &&
     तथ्य->state.http_host) {
    /* Host: [host]:[port] */
    अक्षर *fullhost = curlx_strdup(तथ्य->state.http_host);

    अगर(fullhost)
      शीर्ष = Curl_slist_append_nodup(शून्य, fullhost);
    अगर(!शीर्ष) {
      curlx_free(fullhost);
      जाओ fail;
    }
  }

  अगर(*content_sha256_header) {
    tmp_शीर्ष = curl_slist_append(शीर्ष, content_sha256_header);
    अगर(!tmp_शीर्ष)
      जाओ fail;
    शीर्ष = tmp_शीर्ष;
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
  क्रम(l = तथ्य->set.headers; l; l = l->अग्र) {
    अक्षर *dupdata;
    स्थिर अक्षर *ptr;
    स्थिर अक्षर *sep = म_अक्षर(l->तथ्य, ':');
    अगर(!sep)
      sep = म_अक्षर(l->तथ्य, ';');
    अगर(!sep || (*sep == ':' && !*(sep + 1)))
      जारी;
    क्रम(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    अगर(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      जारी;
    dupdata = curlx_strdup(l->तथ्य);
    अगर(!dupdata)
      जाओ fail;
    dupdata[sep - l->तथ्य] = ':';
    tmp_शीर्ष = Curl_slist_append_nodup(शीर्ष, dupdata);
    अगर(!tmp_शीर्ष) {
      curlx_free(dupdata);
      जाओ fail;
    }
    शीर्ष = tmp_शीर्ष;
  }

  trim_headers(शीर्ष);

  *date_header = find_date_hdr(तथ्य, date_hdr_कुंजी);
  अगर(!*date_header) {
    tmp_शीर्ष = curl_slist_append(शीर्ष, date_full_hdr);
    अगर(!tmp_शीर्ष)
      जाओ fail;
    शीर्ष = tmp_शीर्ष;
    *date_header = curl_maprintf("%s: %s\r\n", date_hdr_कुंजी, timestamp);
    अगर(!*date_header)
      जाओ fail;
  }
  अन्यथा {
    स्थिर अक्षर *मूल्य;
    स्थिर अक्षर *endp;
    मूल्य = म_अक्षर(*date_header, ':');
    अगर(!मूल्य) {
      *date_header = शून्य;
      जाओ fail;
    }
    ++मूल्य;
    curlx_str_passblanks(&मूल्य);
    endp = मूल्य;
    जबतक(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    अगर((endp - मूल्य) == TIMESTAMP_आकार - 1) {
      स_नकल(timestamp, मूल्य, TIMESTAMP_आकार - 1);
      timestamp[TIMESTAMP_आकार - 1] = 0;
    }
    अन्यथा
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_header = शून्य;
  }

  /* alpha-sort by header name in a case sensitive manner */
  करो {
    again = FALSE;
    क्रम(l = शीर्ष; l; l = l->अग्र) {
      काष्ठा curl_slist *अग्र = l->अग्र;

      अगर(अग्र && compare_header_names(l->तथ्य, अग्र->तथ्य) > 0) {
        अक्षर *tmp = l->तथ्य;

        l->तथ्य = अग्र->तथ्य;
        अग्र->तथ्य = tmp;
        again = TRUE;
      }
    }
  } जबतक(again);

  परिणाम = merge_duplicate_headers(शीर्ष);
  अगर(परिणाम)
    जाओ fail;

  क्रम(l = शीर्ष; l; l = l->अग्र) {
    अक्षर *tmp;

    अगर(curlx_dyn_add(canonical_headers, l->तथ्य))
      जाओ fail;
    अगर(curlx_dyn_add(canonical_headers, "\n"))
      जाओ fail;

    tmp = म_अक्षर(l->तथ्य, ':');
    अगर(tmp)
      *tmp = 0;

    अगर(l != शीर्ष && curlx_dyn_add(signed_headers, ";"))
      जाओ fail;
    अगर(curlx_dyn_add(signed_headers, l->तथ्य))
      जाओ fail;
  }

  परिणाम = CURLE_OK;
fail:
  curl_slist_free_all(शीर्ष);

  वापस परिणाम;
}

#घोषणा CONTENT_SHA256_कुंजी_LEN (MAX_SIGV4_LEN + माप("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#घोषणा CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_कुंजी_LEN + 2 + SHA256_HEX_लंबाई)

/* try to parse a payload hash from the content-sha256 header */
अटल स्थिर अक्षर *parse_content_sha_hdr(काष्ठा Curl_easy *तथ्य,
                                         स्थिर अक्षर *provider1,
                                         माप_प्रकार plen,
                                         माप_प्रकार *मूल्य_len)
{
  अक्षर कुंजी[CONTENT_SHA256_कुंजी_LEN];
  माप_प्रकार कुंजी_len;
  स्थिर अक्षर *मूल्य;
  माप_प्रकार len;

  कुंजी_len = curl_msnprintf(कुंजी, माप(कुंजी), "x-%.*s-content-sha256",
                           (पूर्णांक)plen, provider1);

  मूल्य = Curl_checkheaders(तथ्य, कुंजी, कुंजी_len);
  अगर(!मूल्य)
    वापस शून्य;

  मूल्य = म_अक्षर(मूल्य, ':');
  अगर(!मूल्य)
    वापस शून्य;
  ++मूल्य;

  curlx_str_passblanks(&मूल्य);

  len = म_माप(मूल्य);
  जबतक(len > 0 && ISBLANK(मूल्य[len - 1]))
    --len;

  *मूल्य_len = len;
  वापस मूल्य;
}

अटल CURLcode calc_payload_hash(काष्ठा Curl_easy *तथ्य,
                                  अचिन्हित अक्षर *sha_hash, अक्षर *sha_hex)
{
  स्थिर अक्षर *post_तथ्य = तथ्य->set.postfields;
  माप_प्रकार post_तथ्य_len = 0;
  CURLcode परिणाम;

  अगर(post_तथ्य) {
    अगर(तथ्य->set.postfieldsize < 0)
      post_तथ्य_len = म_माप(post_तथ्य);
    अन्यथा
      post_तथ्य_len = (माप_प्रकार)तथ्य->set.postfieldsize;
  }
  परिणाम = Curl_sha256it(sha_hash, (स्थिर अचिन्हित अक्षर *)post_तथ्य,
                         post_तथ्य_len);
  अगर(!परिणाम)
    sha256_to_hex(sha_hex, sha_hash);
  वापस परिणाम;
}

#घोषणा S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

अटल CURLcode calc_s3_payload_hash(काष्ठा Curl_easy *तथ्य,
                                     Curl_HttpReq httpreq,
                                     स्थिर अक्षर *provider1,
                                     माप_प्रकार plen,
                                     अचिन्हित अक्षर *sha_hash,
                                     अक्षर *sha_hex, अक्षर *header)
{
  bool empty_method = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_शीर्ष);
  /* The request method or filesize indicate no request payload */
  bool empty_payload = (empty_method || तथ्य->set.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && तथ्य->set.postfields);
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  अगर(empty_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    परिणाम = calc_payload_hash(तथ्य, sha_hash, sha_hex);
    अगर(परिणाम)
      जाओ fail;
  }
  अन्यथा {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    माप_प्रकार len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_लंबाई); /* 16 < 65 */
    स_नकल(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(header, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (पूर्णांक)plen, provider1, sha_hex);

  परिणाम = CURLE_OK;
fail:
  वापस परिणाम;
}

अटल पूर्णांक compare_func(स्थिर व्योम *a, स्थिर व्योम *b)
{
  स्थिर काष्ठा pair *aa = a;
  स्थिर काष्ठा pair *bb = b;
  स्थिर माप_प्रकार aa_कुंजी_len = curlx_dyn_len(&aa->कुंजी);
  स्थिर माप_प्रकार bb_कुंजी_len = curlx_dyn_len(&bb->कुंजी);
  स्थिर माप_प्रकार aa_मूल्य_len = curlx_dyn_len(&aa->मूल्य);
  स्थिर माप_प्रकार bb_मूल्य_len = curlx_dyn_len(&bb->मूल्य);
  पूर्णांक compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  अगर((aa_कुंजी_len == 0) && (bb_कुंजी_len == 0))
    वापस 0;
  अगर(aa_कुंजी_len == 0)
    वापस -1;
  अगर(bb_कुंजी_len == 0)
    वापस 1;
  compare = म_भेद(curlx_dyn_ptr(&aa->कुंजी), curlx_dyn_ptr(&bb->कुंजी));
  अगर(compare) {
    वापस compare;
  }

  /* Compare values */
  अगर((aa_मूल्य_len == 0) && (bb_मूल्य_len == 0))
    वापस 0;
  अगर(aa_मूल्य_len == 0)
    वापस -1;
  अगर(bb_मूल्य_len == 0)
    वापस 1;
  compare = म_भेद(curlx_dyn_ptr(&aa->मूल्य), curlx_dyn_ptr(&bb->मूल्य));

  वापस compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_path(स्थिर अक्षर *q, माप_प्रकार len,
                             काष्ठा dynbuf *new_path,
                             bool do_uri_encode);
UNITTEST CURLcode canon_path(स्थिर अक्षर *q, माप_प्रकार len,
                             काष्ठा dynbuf *new_path,
                             bool do_uri_encode)
{
  CURLcode परिणाम = CURLE_OK;

  काष्ठा Curl_str original_path;

  curlx_str_assign(&original_path, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  अगर(do_uri_encode)
    परिणाम = uri_encode_path(&original_path, new_path);
  अन्यथा
    परिणाम = curlx_dyn_addn(new_path, q, len);

  अगर(!परिणाम && curlx_dyn_len(new_path) == 0)
    परिणाम = curlx_dyn_add(new_path, "/");

  वापस परिणाम;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(स्थिर अक्षर *query, काष्ठा dynbuf *dq);
UNITTEST CURLcode canon_query(स्थिर अक्षर *query, काष्ठा dynbuf *dq)
{
  CURLcode परिणाम = CURLE_OK;

  काष्ठा dynbuf query_array[MAX_QUERY_COMPONENTS];
  काष्ठा pair encoded_query_array[MAX_QUERY_COMPONENTS];
  माप_प्रकार अंक_query_components;
  माप_प्रकार counted_query_components = 0;
  माप_प्रकार सूचकांक;

  अगर(!query)
    वापस परिणाम;

  परिणाम = split_to_dyn_array(query, &query_array[0], &अंक_query_components);
  अगर(परिणाम) {
    जाओ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  क्रम(सूचकांक = 0; सूचकांक < अंक_query_components; सूचकांक++) {
    स्थिर अक्षर *in_कुंजी;
    माप_प्रकार in_कुंजी_len;
    स्थिर अक्षर *offset;
    माप_प्रकार query_part_len = curlx_dyn_len(&query_array[सूचकांक]);
    स्थिर अक्षर *query_part = curlx_dyn_ptr(&query_array[सूचकांक]);

    in_कुंजी = query_part;

    offset = म_अक्षर(query_part, '=');
    /* If there is no equals, this key has no value */
    अगर(!offset) {
      in_कुंजी_len = म_माप(in_कुंजी);
    }
    अन्यथा {
      in_कुंजी_len = offset - in_कुंजी;
    }

    curlx_dyn_init(&encoded_query_array[सूचकांक].कुंजी,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[सूचकांक].मूल्य,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    परिणाम = normalize_query(in_कुंजी, in_कुंजी_len,
                             &encoded_query_array[सूचकांक].कुंजी);
    अगर(परिणाम) {
      जाओ fail;
    }

    /* Decode/encode the value if it exists */
    अगर(offset && offset != (query_part + query_part_len - 1)) {
      माप_प्रकार in_मूल्य_len;
      स्थिर अक्षर *in_मूल्य = offset + 1;
      in_मूल्य_len = query_part + query_part_len - (offset + 1);
      परिणाम = normalize_query(in_मूल्य, in_मूल्य_len,
                               &encoded_query_array[सूचकांक].मूल्य);
      अगर(परिणाम) {
        जाओ fail;
      }
    }
    अन्यथा {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[सूचकांक].मूल्य, 2);
      परिणाम = curlx_dyn_addn(&encoded_query_array[सूचकांक].मूल्य, "", 1);
    }

    अगर(परिणाम) {
      जाओ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  क्विक(&encoded_query_array, अंक_query_components,
        माप(काष्ठा pair), compare_func);

  /* Append the query components together to make a full query string */
  क्रम(सूचकांक = 0; सूचकांक < अंक_query_components; सूचकांक++) {

    अगर(सूचकांक)
      परिणाम = curlx_dyn_addn(dq, "&", 1);
    अगर(!परिणाम) {
      स्थिर अक्षर *कुंजी_ptr = curlx_dyn_ptr(&encoded_query_array[सूचकांक].कुंजी);
      स्थिर अक्षर *मूल्य_ptr = curlx_dyn_ptr(&encoded_query_array[सूचकांक].मूल्य);
      माप_प्रकार vlen = curlx_dyn_len(&encoded_query_array[सूचकांक].मूल्य);
      अगर(मूल्य_ptr && vlen) {
        परिणाम = curlx_dyn_addf(dq, "%s=%s", कुंजी_ptr, मूल्य_ptr);
      }
      अन्यथा {
        /* Empty value is always encoded to key= */
        परिणाम = curlx_dyn_addf(dq, "%s=", कुंजी_ptr);
      }
    }
    अगर(परिणाम)
      अवरोध;
  }

fail:
  अगर(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], अंक_query_components);
  वापस परिणाम;
}

अटल CURLcode parse_sigv4_params(काष्ठा Curl_easy *तथ्य,
                                   स्थिर अक्षर *hostname,
                                   काष्ठा Curl_str *provider0,
                                   काष्ठा Curl_str *provider1,
                                   काष्ठा Curl_str *region,
                                   काष्ठा Curl_str *service)
{
  स्थिर अक्षर *पंक्ति = CURL_EASY_STR(तथ्य, STRING_AWS_SIGV4);
  अगर(!पंक्ति || !*पंक्ति)
    पंक्ति = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  अगर(curlx_str_until(&पंक्ति, provider0, MAX_SIGV4_LEN, ':')) {
    failf(तथ्य, "first aws-sigv4 provider cannot be empty");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अगर(curlx_str_single(&पंक्ति, ':') ||
     curlx_str_until(&पंक्ति, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  अन्यथा अगर(curlx_str_single(&पंक्ति, ':') ||
          curlx_str_until(&पंक्ति, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&पंक्ति, ':') ||
          curlx_str_until(&पंक्ति, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  अगर(!curlx_strlen(service)) {
    स्थिर अक्षर *p = hostname;
    अगर(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(तथ्य, "aws-sigv4: service missing in parameters and hostname");
      वापस CURLE_URL_MALFORMAT;
    }

    infof(तथ्य, "aws_sigv4: picked service %.*s from host",
          (पूर्णांक)curlx_strlen(service), curlx_str(service));

    अगर(!curlx_strlen(region)) {
      अगर(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(तथ्य, "aws-sigv4: region missing in parameters and hostname");
        वापस CURLE_URL_MALFORMAT;
      }
      infof(तथ्य, "aws_sigv4: picked region %.*s from host",
            (पूर्णांक)curlx_strlen(region), curlx_str(region));
    }
  }

  वापस CURLE_OK;
}

अटल CURLcode get_payload_hash(काष्ठा Curl_easy *तथ्य,
                                 Curl_HttpReq httpreq,
                                 काष्ठा Curl_str *provider0,
                                 काष्ठा Curl_str *provider1,
                                 काष्ठा Curl_str *service,
                                 अचिन्हित अक्षर *sha_hash,
                                 अक्षर *sha_hex,
                                 अक्षर *content_sha256_hdr,
                                 स्थिर अक्षर **payload_hash_out,
                                 माप_प्रकार *payload_hash_len_out)
{
  *payload_hash_out =
    parse_content_sha_hdr(तथ्य, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_out);

  अगर(!*payload_hash_out) {
    CURLcode परिणाम;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    अगर(sign_as_s3)
      परिणाम = calc_s3_payload_hash(तथ्य, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    अन्यथा
      परिणाम = calc_payload_hash(तथ्य, sha_hash, sha_hex);
    अगर(परिणाम)
      वापस परिणाम;

    *payload_hash_out = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_out = म_माप(sha_hex);
  }
  वापस CURLE_OK;
}

अटल CURLcode get_timestamp(अक्षर *timestamp, माप_प्रकार stampsize)
{
  समय_प्रकार घड़ी;
  काष्ठा पंचांग पंचांग;
  CURLcode परिणाम;

#अगर_घोषित DEBUGBUILD
  {
    अक्षर *force_timestamp = दो_पर्या("CURL_FORCETIME");
    अगर(force_timestamp)
      घड़ी = 0;
    अन्यथा
      घड़ी = समय(शून्य);
  }
#अन्यथा
  घड़ी = समय(शून्य);
#पूर्ण_अगर
  परिणाम = curlx_gmtime(घड़ी, &पंचांग);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(!स_माला(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &पंचांग))
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

अटल CURLcode make_canonical_request(काष्ठा Curl_easy *तथ्य,
                                       अक्षर *timestamp,
                                       काष्ठा Curl_str *provider1,
                                       काष्ठा Curl_str *service,
                                       स्थिर अक्षर *method,
                                       स्थिर अक्षर *payload_hash,
                                       माप_प्रकार payload_hash_len,
                                       अक्षर **date_header_out,
                                       अक्षर *content_sha256_hdr,
                                       काष्ठा dynbuf *canonical_headers,
                                       काष्ठा dynbuf *signed_headers,
                                       अक्षर **canonical_request_out)
{
  काष्ठा dynbuf canonical_query;
  काष्ठा dynbuf canonical_path;
  CURLcode परिणाम;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&canonical_path, CURL_MAX_HTTP_HEADER);

  परिणाम = make_headers(तथ्य, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_header_out, content_sha256_hdr,
                        canonical_headers, signed_headers);
  अगर(परिणाम)
    जाओ fail;

  परिणाम = canon_query(तथ्य->state.up.query, &canonical_query);
  अगर(परिणाम)
    जाओ fail;

  परिणाम = canon_path(तथ्य->state.up.path, म_माप(तथ्य->state.up.path),
                      &canonical_path,
                      should_urlencode(service));
  अगर(परिणाम)
    जाओ fail;

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
  अगर(!*canonical_request_out) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

  परिणाम = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_path);
  वापस परिणाम;
}

अटल CURLcode make_string_to_sign(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा Curl_str *provider0,
                                    काष्ठा Curl_str *region,
                                    काष्ठा Curl_str *service,
                                    स्थिर अक्षर *date,
                                    स्थिर अक्षर *timestamp,
                                    स्थिर अक्षर *canonical_request,
                                    अक्षर **request_type_out,
                                    अक्षर **credential_scope_out,
                                    अक्षर **str_to_sign_out)
{
  अक्षर *request_type;
  अक्षर *credential_scope;
  अक्षर *str_to_sign;
  अचिन्हित अक्षर sha_hash[CURL_SHA256_DIGEST_लंबाई];
  अक्षर sha_hex[SHA256_HEX_लंबाई];

  request_type = curl_maprintf("%.*s4_request",
                               (पूर्णांक)curlx_strlen(provider0),
                               curlx_str(provider0));
  अगर(!request_type)
    वापस CURLE_OUT_OF_MEMORY;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (पूर्णांक)curlx_strlen(region),
                                   curlx_str(region),
                                   (पूर्णांक)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  अगर(!credential_scope) {
    curlx_free(request_type);
    वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(Curl_sha256it(sha_hash, (स्थिर अचिन्हित अक्षर *)canonical_request,
                   म_माप(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    वापस CURLE_OUT_OF_MEMORY;
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
  अगर(!str_to_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    वापस CURLE_OUT_OF_MEMORY;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_to_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(तथ्य, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_to_sign);

  *request_type_out = request_type;
  *credential_scope_out = credential_scope;
  *str_to_sign_out = str_to_sign;
  वापस CURLE_OK;
}

अटल CURLcode sign_and_set_auth_headers(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_str *provider0,
                                          काष्ठा Curl_str *region,
                                          काष्ठा Curl_str *service,
                                          स्थिर अक्षर *request_type,
                                          स्थिर अक्षर *credential_scope,
                                          स्थिर अक्षर *date,
                                          स्थिर अक्षर *str_to_sign,
                                          स्थिर अक्षर *date_header,
                                          स्थिर अक्षर *content_sha256_hdr,
                                          काष्ठा dynbuf *signed_headers)
{
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  स्थिर अक्षर *passwd = Curl_creds_passwd(तथ्य->state.creds);
  अक्षर *secret = शून्य;
  अचिन्हित अक्षर sign0[CURL_SHA256_DIGEST_लंबाई] = { 0 };
  अचिन्हित अक्षर sign1[CURL_SHA256_DIGEST_लंबाई] = { 0 };
  अक्षर sha_hex[SHA256_HEX_लंबाई];
  अक्षर *auth_headers = शून्य;
  अक्षर *user = curl_escape(Curl_creds_user(तथ्य->state.creds), 0);
  अगर(!user)
    वापस CURLE_OUT_OF_MEMORY;

  secret = curl_maprintf("%.*s4%s", (पूर्णांक)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  अगर(!secret)
    जाओ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, म_माप(secret), date, म_माप(date), sign0);
  HMAC_SHA256(sign0, माप(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, माप(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, माप(sign0),
              request_type, म_माप(request_type), sign1);
  HMAC_SHA256(sign1, माप(sign1),
              str_to_sign, म_माप(str_to_sign), sign0);

  sha256_to_hex(sha_hex, sign0);

  infof(तथ्य, "aws_sigv4: Signature - %s", sha_hex);

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
  अगर(!auth_headers)
    जाओ fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_headers[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(तथ्य->req.hd_auth);
  तथ्य->req.hd_auth = auth_headers;
  तथ्य->state.authhost.done = TRUE;
  परिणाम = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  वापस परिणाम;
}

CURLcode Curl_output_aws_sigv4(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  काष्ठा Curl_str provider0 = { शून्य, 0 };
  काष्ठा Curl_str provider1 = { शून्य, 0 };
  काष्ठा Curl_str region = { शून्य, 0 };
  काष्ठा Curl_str service = { शून्य, 0 };
  स्थिर अक्षर *hostname = तथ्य->state.origin->hostname;
  अक्षर timestamp[TIMESTAMP_आकार];
  अक्षर date[9];
  काष्ठा dynbuf canonical_headers;
  काष्ठा dynbuf signed_headers;
  अक्षर *date_header = शून्य;
  Curl_HttpReq httpreq;
  स्थिर अक्षर *method = शून्य;
  स्थिर अक्षर *payload_hash = शून्य;
  माप_प्रकार payload_hash_len = 0;
  अचिन्हित अक्षर sha_hash[CURL_SHA256_DIGEST_लंबाई];
  अक्षर sha_hex[SHA256_HEX_लंबाई];
  अक्षर content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  अक्षर *canonical_request = शून्य;
  अक्षर *request_type = शून्य;
  अक्षर *credential_scope = शून्य;
  अक्षर *str_to_sign = शून्य;

  अगर(तथ्य->set.path_as_is) {
    failf(तथ्य, "Cannot use sigv4 authentication with path-as-is flag");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अगर(Curl_checkheaders(तथ्य, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    वापस CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_headers, CURL_MAX_HTTP_HEADER);
  curlx_dyn_init(&signed_headers, CURL_MAX_HTTP_HEADER);

  परिणाम = parse_sigv4_params(तथ्य, hostname, &provider0, &provider1,
                              &region, &service);
  अगर(!परिणाम) {
    Curl_http_method(तथ्य, &method, &httpreq);
    परिणाम = get_payload_hash(तथ्य, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  अगर(!परिणाम)
    परिणाम = get_timestamp(timestamp, माप(timestamp));

  अगर(!परिणाम)
    परिणाम = make_canonical_request(तथ्य, timestamp,
                                    &provider1, &service,
                                    method, payload_hash, payload_hash_len,
                                    &date_header, content_sha256_hdr,
                                    &canonical_headers, &signed_headers,
                                    &canonical_request);
  अगर(!परिणाम) {
    /* the timestamp might have been updated in make_canonical_request */
    स_नकल(date, timestamp, माप(date) - 1);
    date[माप(date) - 1] = 0;

    परिणाम = make_string_to_sign(तथ्य, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_to_sign);
  }
  अगर(!परिणाम)
    परिणाम = sign_and_set_auth_headers(तथ्य, &provider0, &region, &service,
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
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
