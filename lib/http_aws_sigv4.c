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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

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

#परिभाषय HMAC_SHA256(k, kl, d, dl, o)                 \
  कुरु {                                               \
    परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (ध्रुव अचिह्नित वर्ण *)(k), \
                         kl,                         \
                         (ध्रुव अचिह्नित वर्ण *)(d), \
                         dl, o);                     \
    यदि(परिणाम) {                                     \
      गच्छ fail;                                     \
    }                                                \
  } यावत्(0)

#परिभाषय TIMESTAMP_आकार 17

/* hex-encoded with null-terminator */
#परिभाषय SHA256_HEX_दीर्घता ((2 * CURL_SHA256_DIGEST_दीर्घता) + 1)

#परिभाषय MAX_QUERY_COMPONENTS 128

संरचना pair {
  संरचना dynbuf कुञ्चिका;
  संरचना dynbuf मानम्;
};

स्थिर शून्य sha256_गन्तव्यम्_hex(वर्ण *dst, अचिह्नित वर्ण *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_दीर्घता,
                 (अचिह्नित वर्ण *)dst, SHA256_HEX_दीर्घता);
}

स्थिर वर्ण *find_date_hdr(संरचना Curl_easy *दत्त, ध्रुव वर्ण *sig_hdr)
{
  वर्ण *tmp = Curl_checkheaders(दत्त, sig_hdr, वाक्यदैर्घ्यम्(sig_hdr));

  यदि(tmp)
    प्रत्यागम tmp;
  प्रत्यागम Curl_checkheaders(दत्त, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
स्थिर शून्य trim_शीर्षकाणि(संरचना curl_slist *शीर्ष)
{
  संरचना curl_slist *l;
  प्रति(l = शीर्ष; l; l = l->अग्र) {
    ध्रुव वर्ण *मानम्; /* to read from */
    वर्ण *store;
    size_t colon = strcspn(l->दत्त, ":");
    Curl_strntolower(l->दत्त, l->दत्त, colon);

    मानम् = &l->दत्त[colon];
    यदि(!*मानम्)
      अनुवर्त;
    ++मानम्;
    store = (वर्ण *)CURL_UNCONST(मानम्);

    /* skip leading whitespace */
    curlx_str_passblanks(&मानम्);

    यावत्(*मानम्) {
      पूर्णाङ्क space = 0;
      यावत्(ISBLANK(*मानम्)) {
        मानम्++;
        space++;
      }
      यदि(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        यदि(*मानम्)
          *store++ = ' ';
      }
      अन्यथा
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
  size_t सूचकांक;

  प्रति(सूचकांक = 0; सूचकांक != अङ्कः_elements; सूचकांक++) {
    curlx_dyn_free(&pair_array[सूचकांक].कुञ्चिका);
    curlx_dyn_free(&pair_array[सूचकांक].मानम्);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
स्थिर शून्य dyn_array_free(संरचना dynbuf *db, size_t अङ्कः_elements)
{
  size_t सूचकांक;

  प्रति(सूचकांक = 0; सूचकांक < अङ्कः_elements; सूचकांक++)
    curlx_dyn_free((&db[सूचकांक]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#परिभाषय विभज_BY '&'

स्थिर CURLcode विभज_गन्तव्यम्_dyn_array(ध्रुव वर्ण *source,
                                   संरचना dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *अङ्कः_splits_बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;
  size_t len = वाक्यदैर्घ्यम्(source);
  size_t pos;         /* Position in result buffer */
  size_t start = 0;   /* Start of current segment */
  size_t segment_दीर्घता = 0;
  size_t सूचकांक = 0;
  size_t अङ्कः_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  प्रति(pos = 0; pos < len; pos++) {
    यदि(source[pos] == विभज_BY) {
      यदि(segment_दीर्घता) {
        curlx_dyn_init(&db[सूचकांक], segment_दीर्घता + 1);
        परिणाम = curlx_dyn_addn(&db[सूचकांक], &source[start], segment_दीर्घता);
        यदि(परिणाम)
          गच्छ fail;

        segment_दीर्घता = 0;
        सूचकांक++;
        यदि(++अङ्कः_splits == MAX_QUERY_COMPONENTS) {
          परिणाम = CURLE_TOO_LARGE;
          गच्छ fail;
        }
      }
      start = pos + 1;
    }
    अन्यथा {
      segment_दीर्घता++;
    }
  }

  यदि(segment_दीर्घता) {
    curlx_dyn_init(&db[सूचकांक], segment_दीर्घता + 1);
    परिणाम = curlx_dyn_addn(&db[सूचकांक], &source[start], segment_दीर्घता);
    यदि(!परिणाम && ++अङ्कः_splits == MAX_QUERY_COMPONENTS)
      परिणाम = CURLE_TOO_LARGE;
  }
fail:
  *अङ्कः_splits_बहिर्गमनम् = अङ्कः_splits;
  प्रत्यागम परिणाम;
}

स्थिर bool is_reserved_char(ध्रुव वर्ण c)
{
  प्रत्यागम (ISALNUM(c) || ISURLPUNTCS(c));
}

स्थिर CURLcode uri_encode_मार्गः(संरचना Curl_str *original_मार्गः,
                                संरचना dynbuf *new_मार्गः)
{
  ध्रुव वर्ण *p = curlx_str(original_मार्गः);
  size_t i;

  प्रति(i = 0; i < curlx_strlen(original_मार्गः); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode परिणाम = CURLE_OK;
    अचिह्नित वर्ण c = p[i];
    यदि(is_reserved_char(c) || c == '/')
      परिणाम = curlx_dyn_addn(new_मार्गः, &c, 1);
    अन्यथा
      परिणाम = curlx_dyn_addf(new_मार्गः, "%%%02X", c);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  प्रत्यागम CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
स्थिर CURLcode normalize_query(ध्रुव वर्ण *string, size_t len,
                                संरचना dynbuf *db)
{
  CURLcode परिणाम = CURLE_OK;

  यावत्(len && !परिणाम) {
    अचिह्नित वर्ण अन्तर् = (अचिह्नित वर्ण)*string;
    यदि(('%' == अन्तर्) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      अन्तर् = (अचिह्नित वर्ण)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      यदि(अन्तर् == '+') {
        /* decodes to plus, so leave this encoded */
        परिणाम = curlx_dyn_addn(db, "%2B", 3);
        अनुवर्त;
      }
    }
    अन्यथा {
      string++;
      len--;
    }

    यदि(is_reserved_char(अन्तर्))
      /* Escape unreserved chars from RFC 3986 */
      परिणाम = curlx_dyn_addn(db, &अन्तर्, 1);
    अन्यथा यदि(अन्तर् == '+')
      /* Encode '+' as space */
      परिणाम = curlx_dyn_सङ्कलय(db, "%20");
    अन्यथा
      परिणाम = curlx_dyn_addf(db, "%%%02X", अन्तर्);
  }

  प्रत्यागम परिणाम;
}

स्थिर bool should_urlencode(संरचना Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  यदि(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

/* maximum length for the aws sivg4 parts */
#परिभाषय MAX_SIGV4_LEN    64
#परिभाषय DATE_HDR_कुञ्चिका_LEN (MAX_SIGV4_LEN + परिमाण("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#परिभाषय DATE_पूर्णम्_HDR_LEN (DATE_HDR_कुञ्चिका_LEN + TIMESTAMP_आकार + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
स्थिर पूर्णाङ्क compare_शीर्षकम्_नामानि(ध्रुव वर्ण *a, ध्रुव वर्ण *b)
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

  len_a = colon_a ? (size_t)(colon_a - a) : वाक्यदैर्घ्यम्(a);
  len_b = colon_b ? (size_t)(colon_b - b) : वाक्यदैर्घ्यम्(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = वाक्ये_तुलय(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  यदि(!cmp)
    प्रत्यागम (पूर्णाङ्क)(len_a - len_b);

  प्रत्यागम cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
स्थिर CURLcode merge_duplicate_शीर्षकाणि(संरचना curl_slist *शीर्ष)
{
  संरचना curl_slist *curr = शीर्ष;
  CURLcode परिणाम = CURLE_OK;

  यावत्(curr) {
    संरचना curl_slist *अग्र = curr->अग्र;
    यदि(!अग्र)
      भङ्ग;

    यदि(compare_शीर्षकम्_नामानि(curr->दत्त, अग्र->दत्त) == 0) {
      संरचना dynbuf buf;
      ध्रुव वर्ण *colon_अग्र;
      ध्रुव वर्ण *val_अग्र;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_शीर्षकम्);

      परिणाम = curlx_dyn_सङ्कलय(&buf, curr->दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      colon_अग्र = strchr(अग्र->दत्त, ':');
      DEBUGASSERT(colon_अग्र);
      val_अग्र = colon_अग्र + 1;

      परिणाम = curlx_dyn_addn(&buf, ",", 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      परिणाम = curlx_dyn_सङ्कलय(&buf, val_अग्र);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      curlx_free(curr->दत्त);
      curr->दत्त = curlx_dyn_ptr(&buf);

      curr->अग्र = अग्र->अग्र;
      curlx_free(अग्र->दत्त);
      curlx_free(अग्र);
    }
    अन्यथा {
      curr = curr->अग्र;
    }
  }

  प्रत्यागम CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
स्थिर CURLcode make_शीर्षकाणि(संरचना Curl_easy *दत्त,
                             वर्ण *timestamp,
                             ध्रुव वर्ण *provider1,
                             size_t plen, /* length of provider1 */
                             वर्ण **date_शीर्षकम्,
                             वर्ण *content_sha256_शीर्षकम्,
                             संरचना dynbuf *canonical_शीर्षकाणि,
                             संरचना dynbuf *signed_शीर्षकाणि)
{
  वर्ण date_hdr_कुञ्चिका[DATE_HDR_कुञ्चिका_LEN];
  वर्ण date_पूर्णम्_hdr[DATE_पूर्णम्_HDR_LEN];
  संरचना curl_slist *शीर्ष = रिक्त;
  संरचना curl_slist *tmp_शीर्ष = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  संरचना curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_कुञ्चिका, DATE_HDR_कुञ्चिका_LEN, "X-%.*s-Date",
                 (पूर्णाङ्क)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_कुञ्चिका[2], provider1, plen);
  date_hdr_कुञ्चिका[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_पूर्णम्_hdr, DATE_पूर्णम्_HDR_LEN,
                 "x-%.*s-date:%s", (पूर्णाङ्क)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_पूर्णम्_hdr[2], provider1, plen);

  यदि(!Curl_checkheaders(दत्त, STRCONST("Host")) &&
     दत्त->state.http_host) {
    /* Host: [host]:[port] */
    वर्ण *fullhost = curlx_strdup(दत्त->state.http_host);

    यदि(fullhost)
      शीर्ष = Curl_slist_append_nodup(रिक्त, fullhost);
    यदि(!शीर्ष) {
      curlx_free(fullhost);
      गच्छ fail;
    }
  }

  यदि(*content_sha256_शीर्षकम्) {
    tmp_शीर्ष = curl_slist_append(शीर्ष, content_sha256_शीर्षकम्);
    यदि(!tmp_शीर्ष)
      गच्छ fail;
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
  प्रति(l = दत्त->समुच्चयः.शीर्षकाणि; l; l = l->अग्र) {
    वर्ण *dupdata;
    ध्रुव वर्ण *ptr;
    ध्रुव वर्ण *sep = strchr(l->दत्त, ':');
    यदि(!sep)
      sep = strchr(l->दत्त, ';');
    यदि(!sep || (*sep == ':' && !*(sep + 1)))
      अनुवर्त;
    प्रति(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    यदि(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      अनुवर्त;
    dupdata = curlx_strdup(l->दत्त);
    यदि(!dupdata)
      गच्छ fail;
    dupdata[sep - l->दत्त] = ':';
    tmp_शीर्ष = Curl_slist_append_nodup(शीर्ष, dupdata);
    यदि(!tmp_शीर्ष) {
      curlx_free(dupdata);
      गच्छ fail;
    }
    शीर्ष = tmp_शीर्ष;
  }

  trim_शीर्षकाणि(शीर्ष);

  *date_शीर्षकम् = find_date_hdr(दत्त, date_hdr_कुञ्चिका);
  यदि(!*date_शीर्षकम्) {
    tmp_शीर्ष = curl_slist_append(शीर्ष, date_पूर्णम्_hdr);
    यदि(!tmp_शीर्ष)
      गच्छ fail;
    शीर्ष = tmp_शीर्ष;
    *date_शीर्षकम् = curl_maprintf("%s: %s\r\n", date_hdr_कुञ्चिका, timestamp);
    यदि(!*date_शीर्षकम्)
      गच्छ fail;
  }
  अन्यथा {
    ध्रुव वर्ण *मानम्;
    ध्रुव वर्ण *endp;
    मानम् = strchr(*date_शीर्षकम्, ':');
    यदि(!मानम्) {
      *date_शीर्षकम् = रिक्त;
      गच्छ fail;
    }
    ++मानम्;
    curlx_str_passblanks(&मानम्);
    endp = मानम्;
    यावत्(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    यदि((endp - मानम्) == TIMESTAMP_आकार - 1) {
      स्मृतिं_प्रतिलिख(timestamp, मानम्, TIMESTAMP_आकार - 1);
      timestamp[TIMESTAMP_आकार - 1] = 0;
    }
    अन्यथा
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_शीर्षकम् = रिक्त;
  }

  /* alpha-sort by header name in a case sensitive manner */
  कुरु {
    again = FALSE;
    प्रति(l = शीर्ष; l; l = l->अग्र) {
      संरचना curl_slist *अग्र = l->अग्र;

      यदि(अग्र && compare_शीर्षकम्_नामानि(l->दत्त, अग्र->दत्त) > 0) {
        वर्ण *tmp = l->दत्त;

        l->दत्त = अग्र->दत्त;
        अग्र->दत्त = tmp;
        again = TRUE;
      }
    }
  } यावत्(again);

  परिणाम = merge_duplicate_शीर्षकाणि(शीर्ष);
  यदि(परिणाम)
    गच्छ fail;

  प्रति(l = शीर्ष; l; l = l->अग्र) {
    वर्ण *tmp;

    यदि(curlx_dyn_सङ्कलय(canonical_शीर्षकाणि, l->दत्त))
      गच्छ fail;
    यदि(curlx_dyn_सङ्कलय(canonical_शीर्षकाणि, "\n"))
      गच्छ fail;

    tmp = strchr(l->दत्त, ':');
    यदि(tmp)
      *tmp = 0;

    यदि(l != शीर्ष && curlx_dyn_सङ्कलय(signed_शीर्षकाणि, ";"))
      गच्छ fail;
    यदि(curlx_dyn_सङ्कलय(signed_शीर्षकाणि, l->दत्त))
      गच्छ fail;
  }

  परिणाम = CURLE_OK;
fail:
  curl_slist_free_all(शीर्ष);

  प्रत्यागम परिणाम;
}

#परिभाषय CONTENT_SHA256_कुञ्चिका_LEN (MAX_SIGV4_LEN + परिमाण("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#परिभाषय CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_कुञ्चिका_LEN + 2 + SHA256_HEX_दीर्घता)

/* try to parse a payload hash from the content-sha256 header */
स्थिर ध्रुव वर्ण *parse_content_sha_hdr(संरचना Curl_easy *दत्त,
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

  मानम् = Curl_checkheaders(दत्त, कुञ्चिका, कुञ्चिका_len);
  यदि(!मानम्)
    प्रत्यागम रिक्त;

  मानम् = strchr(मानम्, ':');
  यदि(!मानम्)
    प्रत्यागम रिक्त;
  ++मानम्;

  curlx_str_passblanks(&मानम्);

  len = वाक्यदैर्घ्यम्(मानम्);
  यावत्(len > 0 && ISBLANK(मानम्[len - 1]))
    --len;

  *मानम्_len = len;
  प्रत्यागम मानम्;
}

स्थिर CURLcode calc_payload_hash(संरचना Curl_easy *दत्त,
                                  अचिह्नित वर्ण *sha_hash, वर्ण *sha_hex)
{
  ध्रुव वर्ण *post_दत्त = दत्त->समुच्चयः.postfields;
  size_t post_दत्त_len = 0;
  CURLcode परिणाम;

  यदि(post_दत्त) {
    यदि(दत्त->समुच्चयः.postfieldsize < 0)
      post_दत्त_len = वाक्यदैर्घ्यम्(post_दत्त);
    अन्यथा
      post_दत्त_len = (size_t)दत्त->समुच्चयः.postfieldsize;
  }
  परिणाम = Curl_sha256it(sha_hash, (ध्रुव अचिह्नित वर्ण *)post_दत्त,
                         post_दत्त_len);
  यदि(!परिणाम)
    sha256_गन्तव्यम्_hex(sha_hex, sha_hash);
  प्रत्यागम परिणाम;
}

#परिभाषय S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

स्थिर CURLcode calc_s3_payload_hash(संरचना Curl_easy *दत्त,
                                     Curl_HttpReq httpreq,
                                     ध्रुव वर्ण *provider1,
                                     size_t plen,
                                     अचिह्नित वर्ण *sha_hash,
                                     वर्ण *sha_hex, वर्ण *शीर्षकम्)
{
  bool रिक्तानि_विधानम् = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_शीर्ष);
  /* The request method or filesize indicate no request payload */
  bool रिक्तानि_payload = (रिक्तानि_विधानम् || दत्त->समुच्चयः.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && दत्त->समुच्चयः.postfields);
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(रिक्तानि_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    परिणाम = calc_payload_hash(दत्त, sha_hash, sha_hex);
    यदि(परिणाम)
      गच्छ fail;
  }
  अन्यथा {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_दीर्घता); /* 16 < 65 */
    स्मृतिं_प्रतिलिख(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(शीर्षकम्, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (पूर्णाङ्क)plen, provider1, sha_hex);

  परिणाम = CURLE_OK;
fail:
  प्रत्यागम परिणाम;
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
  यदि((aa_कुञ्चिका_len == 0) && (bb_कुञ्चिका_len == 0))
    प्रत्यागम 0;
  यदि(aa_कुञ्चिका_len == 0)
    प्रत्यागम -1;
  यदि(bb_कुञ्चिका_len == 0)
    प्रत्यागम 1;
  compare = वाक्यं_तुलय(curlx_dyn_ptr(&aa->कुञ्चिका), curlx_dyn_ptr(&bb->कुञ्चिका));
  यदि(compare) {
    प्रत्यागम compare;
  }

  /* Compare values */
  यदि((aa_मानम्_len == 0) && (bb_मानम्_len == 0))
    प्रत्यागम 0;
  यदि(aa_मानम्_len == 0)
    प्रत्यागम -1;
  यदि(bb_मानम्_len == 0)
    प्रत्यागम 1;
  compare = वाक्यं_तुलय(curlx_dyn_ptr(&aa->मानम्), curlx_dyn_ptr(&bb->मानम्));

  प्रत्यागम compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_मार्गः(ध्रुव वर्ण *q, size_t len,
                             संरचना dynbuf *new_मार्गः,
                             bool do_uri_encode);
UNITTEST CURLcode canon_मार्गः(ध्रुव वर्ण *q, size_t len,
                             संरचना dynbuf *new_मार्गः,
                             bool do_uri_encode)
{
  CURLcode परिणाम = CURLE_OK;

  संरचना Curl_str original_मार्गः;

  curlx_str_नियोजय(&original_मार्गः, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  यदि(do_uri_encode)
    परिणाम = uri_encode_मार्गः(&original_मार्गः, new_मार्गः);
  अन्यथा
    परिणाम = curlx_dyn_addn(new_मार्गः, q, len);

  यदि(!परिणाम && curlx_dyn_len(new_मार्गः) == 0)
    परिणाम = curlx_dyn_सङ्कलय(new_मार्गः, "/");

  प्रत्यागम परिणाम;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(ध्रुव वर्ण *query, संरचना dynbuf *dq);
UNITTEST CURLcode canon_query(ध्रुव वर्ण *query, संरचना dynbuf *dq)
{
  CURLcode परिणाम = CURLE_OK;

  संरचना dynbuf query_array[MAX_QUERY_COMPONENTS];
  संरचना pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t अङ्कः_query_components;
  size_t counted_query_components = 0;
  size_t सूचकांक;

  यदि(!query)
    प्रत्यागम परिणाम;

  परिणाम = विभज_गन्तव्यम्_dyn_array(query, &query_array[0], &अङ्कः_query_components);
  यदि(परिणाम) {
    गच्छ fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  प्रति(सूचकांक = 0; सूचकांक < अङ्कः_query_components; सूचकांक++) {
    ध्रुव वर्ण *अन्तर्_कुञ्चिका;
    size_t अन्तर्_कुञ्चिका_len;
    ध्रुव वर्ण *विस्थापनम्;
    size_t query_part_len = curlx_dyn_len(&query_array[सूचकांक]);
    ध्रुव वर्ण *query_part = curlx_dyn_ptr(&query_array[सूचकांक]);

    अन्तर्_कुञ्चिका = query_part;

    विस्थापनम् = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    यदि(!विस्थापनम्) {
      अन्तर्_कुञ्चिका_len = वाक्यदैर्घ्यम्(अन्तर्_कुञ्चिका);
    }
    अन्यथा {
      अन्तर्_कुञ्चिका_len = विस्थापनम् - अन्तर्_कुञ्चिका;
    }

    curlx_dyn_init(&encoded_query_array[सूचकांक].कुञ्चिका,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[सूचकांक].मानम्,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    परिणाम = normalize_query(अन्तर्_कुञ्चिका, अन्तर्_कुञ्चिका_len,
                             &encoded_query_array[सूचकांक].कुञ्चिका);
    यदि(परिणाम) {
      गच्छ fail;
    }

    /* Decode/encode the value if it exists */
    यदि(विस्थापनम् && विस्थापनम् != (query_part + query_part_len - 1)) {
      size_t अन्तर्_मानम्_len;
      ध्रुव वर्ण *अन्तर्_मानम् = विस्थापनम् + 1;
      अन्तर्_मानम्_len = query_part + query_part_len - (विस्थापनम् + 1);
      परिणाम = normalize_query(अन्तर्_मानम्, अन्तर्_मानम्_len,
                               &encoded_query_array[सूचकांक].मानम्);
      यदि(परिणाम) {
        गच्छ fail;
      }
    }
    अन्यथा {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[सूचकांक].मानम्, 2);
      परिणाम = curlx_dyn_addn(&encoded_query_array[सूचकांक].मानम्, "", 1);
    }

    यदि(परिणाम) {
      गच्छ fail;
    }
  }

  /* Sort the encoded query components by key and value */
  शीघ्रं_क्रमय(&encoded_query_array, अङ्कः_query_components,
        परिमाण(संरचना pair), compare_func);

  /* Append the query components together to make a full query string */
  प्रति(सूचकांक = 0; सूचकांक < अङ्कः_query_components; सूचकांक++) {

    यदि(सूचकांक)
      परिणाम = curlx_dyn_addn(dq, "&", 1);
    यदि(!परिणाम) {
      ध्रुव वर्ण *कुञ्चिका_ptr = curlx_dyn_ptr(&encoded_query_array[सूचकांक].कुञ्चिका);
      ध्रुव वर्ण *मानम्_ptr = curlx_dyn_ptr(&encoded_query_array[सूचकांक].मानम्);
      size_t vlen = curlx_dyn_len(&encoded_query_array[सूचकांक].मानम्);
      यदि(मानम्_ptr && vlen) {
        परिणाम = curlx_dyn_addf(dq, "%s=%s", कुञ्चिका_ptr, मानम्_ptr);
      }
      अन्यथा {
        /* Empty value is always encoded to key= */
        परिणाम = curlx_dyn_addf(dq, "%s=", कुञ्चिका_ptr);
      }
    }
    यदि(परिणाम)
      भङ्ग;
  }

fail:
  यदि(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], अङ्कः_query_components);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode parse_sigv4_params(संरचना Curl_easy *दत्त,
                                   ध्रुव वर्ण *hostname,
                                   संरचना Curl_str *provider0,
                                   संरचना Curl_str *provider1,
                                   संरचना Curl_str *region,
                                   संरचना Curl_str *service)
{
  ध्रुव वर्ण *पङ्क्तिः = CURL_EASY_STR(दत्त, STRING_AWS_SIGV4);
  यदि(!पङ्क्तिः || !*पङ्क्तिः)
    पङ्क्तिः = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  यदि(curlx_str_until(&पङ्क्तिः, provider0, MAX_SIGV4_LEN, ':')) {
    failf(दत्त, "first aws-sigv4 provider cannot be empty");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  यदि(curlx_str_single(&पङ्क्तिः, ':') ||
     curlx_str_until(&पङ्क्तिः, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  अन्यथा यदि(curlx_str_single(&पङ्क्तिः, ':') ||
          curlx_str_until(&पङ्क्तिः, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&पङ्क्तिः, ':') ||
          curlx_str_until(&पङ्क्तिः, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  यदि(!curlx_strlen(service)) {
    ध्रुव वर्ण *p = hostname;
    यदि(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(दत्त, "aws-sigv4: service missing in parameters and hostname");
      प्रत्यागम CURLE_URL_MALFORMAT;
    }

    infof(दत्त, "aws_sigv4: picked service %.*s from host",
          (पूर्णाङ्क)curlx_strlen(service), curlx_str(service));

    यदि(!curlx_strlen(region)) {
      यदि(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(दत्त, "aws-sigv4: region missing in parameters and hostname");
        प्रत्यागम CURLE_URL_MALFORMAT;
      }
      infof(दत्त, "aws_sigv4: picked region %.*s from host",
            (पूर्णाङ्क)curlx_strlen(region), curlx_str(region));
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode get_payload_hash(संरचना Curl_easy *दत्त,
                                 Curl_HttpReq httpreq,
                                 संरचना Curl_str *provider0,
                                 संरचना Curl_str *provider1,
                                 संरचना Curl_str *service,
                                 अचिह्नित वर्ण *sha_hash,
                                 वर्ण *sha_hex,
                                 वर्ण *content_sha256_hdr,
                                 ध्रुव वर्ण **payload_hash_बहिर्गमनम्,
                                 size_t *payload_hash_len_बहिर्गमनम्)
{
  *payload_hash_बहिर्गमनम् =
    parse_content_sha_hdr(दत्त, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_बहिर्गमनम्);

  यदि(!*payload_hash_बहिर्गमनम्) {
    CURLcode परिणाम;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    यदि(sign_as_s3)
      परिणाम = calc_s3_payload_hash(दत्त, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    अन्यथा
      परिणाम = calc_payload_hash(दत्त, sha_hash, sha_hex);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    *payload_hash_बहिर्गमनम् = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_बहिर्गमनम् = वाक्यदैर्घ्यम्(sha_hex);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode get_timestamp(वर्ण *timestamp, size_t stampsize)
{
  time_t clock;
  संरचना tm tm;
  CURLcode परिणाम;

#यदि_परिभाषितम् DEBUGBUILD
  {
    वर्ण *force_timestamp = getenv("CURL_FORCETIME");
    यदि(force_timestamp)
      clock = 0;
    अन्यथा
      clock = time(रिक्त);
  }
#अन्यथा
  clock = time(रिक्त);
#यदि_अन्तः
  परिणाम = curlx_gmtime(clock, &tm);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode make_canonical_request(संरचना Curl_easy *दत्त,
                                       वर्ण *timestamp,
                                       संरचना Curl_str *provider1,
                                       संरचना Curl_str *service,
                                       ध्रुव वर्ण *विधानम्,
                                       ध्रुव वर्ण *payload_hash,
                                       size_t payload_hash_len,
                                       वर्ण **date_शीर्षकम्_बहिर्गमनम्,
                                       वर्ण *content_sha256_hdr,
                                       संरचना dynbuf *canonical_शीर्षकाणि,
                                       संरचना dynbuf *signed_शीर्षकाणि,
                                       वर्ण **canonical_request_बहिर्गमनम्)
{
  संरचना dynbuf canonical_query;
  संरचना dynbuf canonical_मार्गः;
  CURLcode परिणाम;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_शीर्षकम्);
  curlx_dyn_init(&canonical_मार्गः, CURL_MAX_HTTP_शीर्षकम्);

  परिणाम = make_शीर्षकाणि(दत्त, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_शीर्षकम्_बहिर्गमनम्, content_sha256_hdr,
                        canonical_शीर्षकाणि, signed_शीर्षकाणि);
  यदि(परिणाम)
    गच्छ fail;

  परिणाम = canon_query(दत्त->state.ऊर्ध्वम्.query, &canonical_query);
  यदि(परिणाम)
    गच्छ fail;

  परिणाम = canon_मार्गः(दत्त->state.ऊर्ध्वम्.मार्गः, वाक्यदैर्घ्यम्(दत्त->state.ऊर्ध्वम्.मार्गः),
                      &canonical_मार्गः,
                      should_urlencode(service));
  यदि(परिणाम)
    गच्छ fail;

  *canonical_request_बहिर्गमनम् =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  विधानम्,
                  curlx_dyn_ptr(&canonical_मार्गः),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_शीर्षकाणि),
                  curlx_dyn_ptr(signed_शीर्षकाणि),
                  (पूर्णाङ्क)payload_hash_len, payload_hash);
  यदि(!*canonical_request_बहिर्गमनम्) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

  परिणाम = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_मार्गः);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode make_string_गन्तव्यम्_sign(संरचना Curl_easy *दत्त,
                                    संरचना Curl_str *provider0,
                                    संरचना Curl_str *region,
                                    संरचना Curl_str *service,
                                    ध्रुव वर्ण *date,
                                    ध्रुव वर्ण *timestamp,
                                    ध्रुव वर्ण *canonical_request,
                                    वर्ण **request_type_बहिर्गमनम्,
                                    वर्ण **credential_scope_बहिर्गमनम्,
                                    वर्ण **str_गन्तव्यम्_sign_बहिर्गमनम्)
{
  वर्ण *request_type;
  वर्ण *credential_scope;
  वर्ण *str_गन्तव्यम्_sign;
  अचिह्नित वर्ण sha_hash[CURL_SHA256_DIGEST_दीर्घता];
  वर्ण sha_hex[SHA256_HEX_दीर्घता];

  request_type = curl_maprintf("%.*s4_request",
                               (पूर्णाङ्क)curlx_strlen(provider0),
                               curlx_str(provider0));
  यदि(!request_type)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (पूर्णाङ्क)curlx_strlen(region),
                                   curlx_str(region),
                                   (पूर्णाङ्क)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  यदि(!credential_scope) {
    curlx_free(request_type);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(Curl_sha256it(sha_hash, (ध्रुव अचिह्नित वर्ण *)canonical_request,
                   वाक्यदैर्घ्यम्(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  sha256_गन्तव्यम्_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_गन्तव्यम्_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (पूर्णाङ्क)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  यदि(!str_गन्तव्यम्_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_गन्तव्यम्_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(दत्त, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_गन्तव्यम्_sign);

  *request_type_बहिर्गमनम् = request_type;
  *credential_scope_बहिर्गमनम् = credential_scope;
  *str_गन्तव्यम्_sign_बहिर्गमनम् = str_गन्तव्यम्_sign;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode sign_and_समुच्चयः_auth_शीर्षकाणि(संरचना Curl_easy *दत्त,
                                          संरचना Curl_str *provider0,
                                          संरचना Curl_str *region,
                                          संरचना Curl_str *service,
                                          ध्रुव वर्ण *request_type,
                                          ध्रुव वर्ण *credential_scope,
                                          ध्रुव वर्ण *date,
                                          ध्रुव वर्ण *str_गन्तव्यम्_sign,
                                          ध्रुव वर्ण *date_शीर्षकम्,
                                          ध्रुव वर्ण *content_sha256_hdr,
                                          संरचना dynbuf *signed_शीर्षकाणि)
{
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  ध्रुव वर्ण *passwd = Curl_creds_passwd(दत्त->state.creds);
  वर्ण *secret = रिक्त;
  अचिह्नित वर्ण sign0[CURL_SHA256_DIGEST_दीर्घता] = { 0 };
  अचिह्नित वर्ण sign1[CURL_SHA256_DIGEST_दीर्घता] = { 0 };
  वर्ण sha_hex[SHA256_HEX_दीर्घता];
  वर्ण *auth_शीर्षकाणि = रिक्त;
  वर्ण *user = curl_escape(Curl_creds_user(दत्त->state.creds), 0);
  यदि(!user)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  secret = curl_maprintf("%.*s4%s", (पूर्णाङ्क)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  यदि(!secret)
    गच्छ fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, वाक्यदैर्घ्यम्(secret), date, वाक्यदैर्घ्यम्(date), sign0);
  HMAC_SHA256(sign0, परिमाण(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, परिमाण(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, परिमाण(sign0),
              request_type, वाक्यदैर्घ्यम्(request_type), sign1);
  HMAC_SHA256(sign1, परिमाण(sign1),
              str_गन्तव्यम्_sign, वाक्यदैर्घ्यम्(str_गन्तव्यम्_sign), sign0);

  sha256_गन्तव्यम्_hex(sha_hex, sign0);

  infof(दत्त, "aws_sigv4: Signature - %s", sha_hex);

  auth_शीर्षकाणि = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (पूर्णाङ्क)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_शीर्षकाणि),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_शीर्षकम् ? date_शीर्षकम् : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  यदि(!auth_शीर्षकाणि)
    गच्छ fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_शीर्षकाणि[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(दत्त->req.hd_auth);
  दत्त->req.hd_auth = auth_शीर्षकाणि;
  दत्त->state.authhost.समाप्तम् = TRUE;
  परिणाम = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  प्रत्यागम परिणाम;
}

CURLcode Curl_output_aws_sigv4(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  संरचना Curl_str provider0 = { रिक्त, 0 };
  संरचना Curl_str provider1 = { रिक्त, 0 };
  संरचना Curl_str region = { रिक्त, 0 };
  संरचना Curl_str service = { रिक्त, 0 };
  ध्रुव वर्ण *hostname = दत्त->state.origin->hostname;
  वर्ण timestamp[TIMESTAMP_आकार];
  वर्ण date[9];
  संरचना dynbuf canonical_शीर्षकाणि;
  संरचना dynbuf signed_शीर्षकाणि;
  वर्ण *date_शीर्षकम् = रिक्त;
  Curl_HttpReq httpreq;
  ध्रुव वर्ण *विधानम् = रिक्त;
  ध्रुव वर्ण *payload_hash = रिक्त;
  size_t payload_hash_len = 0;
  अचिह्नित वर्ण sha_hash[CURL_SHA256_DIGEST_दीर्घता];
  वर्ण sha_hex[SHA256_HEX_दीर्घता];
  वर्ण content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  वर्ण *canonical_request = रिक्त;
  वर्ण *request_type = रिक्त;
  वर्ण *credential_scope = रिक्त;
  वर्ण *str_गन्तव्यम्_sign = रिक्त;

  यदि(दत्त->समुच्चयः.मार्गः_as_is) {
    failf(दत्त, "Cannot use sigv4 authentication with path-as-is flag");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  यदि(Curl_checkheaders(दत्त, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    प्रत्यागम CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_शीर्षकाणि, CURL_MAX_HTTP_शीर्षकम्);
  curlx_dyn_init(&signed_शीर्षकाणि, CURL_MAX_HTTP_शीर्षकम्);

  परिणाम = parse_sigv4_params(दत्त, hostname, &provider0, &provider1,
                              &region, &service);
  यदि(!परिणाम) {
    Curl_http_विधानम्(दत्त, &विधानम्, &httpreq);
    परिणाम = get_payload_hash(दत्त, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  यदि(!परिणाम)
    परिणाम = get_timestamp(timestamp, परिमाण(timestamp));

  यदि(!परिणाम)
    परिणाम = make_canonical_request(दत्त, timestamp,
                                    &provider1, &service,
                                    विधानम्, payload_hash, payload_hash_len,
                                    &date_शीर्षकम्, content_sha256_hdr,
                                    &canonical_शीर्षकाणि, &signed_शीर्षकाणि,
                                    &canonical_request);
  यदि(!परिणाम) {
    /* the timestamp might have been updated in make_canonical_request */
    स्मृतिं_प्रतिलिख(date, timestamp, परिमाण(date) - 1);
    date[परिमाण(date) - 1] = 0;

    परिणाम = make_string_गन्तव्यम्_sign(दत्त, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_गन्तव्यम्_sign);
  }
  यदि(!परिणाम)
    परिणाम = sign_and_समुच्चयः_auth_शीर्षकाणि(दत्त, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_गन्तव्यम्_sign, date_शीर्षकम्,
                                       content_sha256_hdr, &signed_शीर्षकाणि);

  curlx_dyn_free(&canonical_शीर्षकाणि);
  curlx_dyn_free(&signed_शीर्षकाणि);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_गन्तव्यम्_sign);
  curlx_free(date_शीर्षकम्);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
