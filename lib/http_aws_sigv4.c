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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_AWS)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "http_aws_sigv4.h"
#అన్తర్భావ "curl_sha256.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "slist.h"

#అన్తర్భావ <time.h>

#నిర్వచించు HMAC_SHA256(k, kl, d, dl, o)                 \
  చేయి {                                               \
    ఫలితం = Curl_hmacit(&Curl_HMAC_SHA256,          \
                         (స్థిరరాశి గుర్తులేని అక్షరం *)(k), \
                         kl,                         \
                         (స్థిరరాశి గుర్తులేని అక్షరం *)(d), \
                         dl, o);                     \
    ఒకవేళ(ఫలితం) {                                     \
      వెళ్ళు fail;                                     \
    }                                                \
  } అయితే(0)

#నిర్వచించు TIMESTAMP_కొలత 17

/* hex-encoded with null-terminator */
#నిర్వచించు SHA256_HEX_నిడివి ((2 * CURL_SHA256_DIGEST_నిడివి) + 1)

#నిర్వచించు MAX_QUERY_COMPONENTS 128

నిర్మాణం pair {
  నిర్మాణం dynbuf కీలకం;
  నిర్మాణం dynbuf విలువ;
};

స్థిర శూన్య sha256_గమ్యం_hex(అక్షరం *dst, గుర్తులేని అక్షరం *sha)
{
  Curl_hexencode(sha, CURL_SHA256_DIGEST_నిడివి,
                 (గుర్తులేని అక్షరం *)dst, SHA256_HEX_నిడివి);
}

స్థిర అక్షరం *కనుగొను_date_hdr(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *sig_hdr)
{
  అక్షరం *tmp = Curl_checkheaders(దత్తాంశం, sig_hdr, అక్షరశ్రేణి_పొడవు(sig_hdr));

  ఒకవేళ(tmp)
    తిరిగి tmp;
  తిరిగి Curl_checkheaders(దత్తాంశం, STRCONST("Date"));
}

/* remove whitespace, and lowercase all headers */
స్థిర శూన్య trim_శీర్షికలు(నిర్మాణం curl_slist *తల)
{
  నిర్మాణం curl_slist *l;
  కొరకు(l = తల; l; l = l->తదుపరి) {
    స్థిరరాశి అక్షరం *విలువ; /* to read from */
    అక్షరం *store;
    size_t colon = strcspn(l->దత్తాంశం, ":");
    Curl_strntolower(l->దత్తాంశం, l->దత్తాంశం, colon);

    విలువ = &l->దత్తాంశం[colon];
    ఒకవేళ(!*విలువ)
      కొనసాగు;
    ++విలువ;
    store = (అక్షరం *)CURL_UNCONST(విలువ);

    /* skip leading whitespace */
    curlx_str_passblanks(&విలువ);

    అయితే(*విలువ) {
      పూర్ణాంకం space = 0;
      అయితే(ISBLANK(*విలువ)) {
        విలువ++;
        space++;
      }
      ఒకవేళ(space) {
        /* replace any number of consecutive whitespace with a single space,
           unless at the end of the string, then nothing */
        ఒకవేళ(*విలువ)
          *store++ = ' ';
      }
      లేకపోతే
        *store++ = *విలువ++;
    }
    *store = 0; /* null-terminate */
  }
}

/*
 * Frees all allocated strings in a dynbuf pair array, and the dynbuf itself
 */
స్థిర శూన్య pair_array_free(నిర్మాణం pair *pair_array, size_t అంకె_elements)
{
  size_t సూచీ;

  కొరకు(సూచీ = 0; సూచీ != అంకె_elements; సూచీ++) {
    curlx_dyn_free(&pair_array[సూచీ].కీలకం);
    curlx_dyn_free(&pair_array[సూచీ].విలువ);
  }
}

/*
 * Frees all allocated strings in a split dynbuf, and the dynbuf itself
 */
స్థిర శూన్య dyn_array_free(నిర్మాణం dynbuf *db, size_t అంకె_elements)
{
  size_t సూచీ;

  కొరకు(సూచీ = 0; సూచీ < అంకె_elements; సూచీ++)
    curlx_dyn_free((&db[సూచీ]));
}

/*
 * Splits source string by SPLIT_BY, and creates an array of dynbuf in db.
 * db is initialized by this function.
 * Caller is responsible for freeing the array elements with dyn_array_free
 */

#నిర్వచించు విభజించు_BY '&'

స్థిర CURLcode విభజించు_గమ్యం_dyn_array(స్థిరరాశి అక్షరం *ఆధారం,
                                   నిర్మాణం dynbuf db[MAX_QUERY_COMPONENTS],
                                   size_t *అంకె_splits_బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t len = అక్షరశ్రేణి_పొడవు(ఆధారం);
  size_t pos;         /* Position in result buffer */
  size_t ప్రారంభం = 0;   /* Start of current segment */
  size_t segment_నిడివి = 0;
  size_t సూచీ = 0;
  size_t అంకె_splits = 0;

  /* Split source_ptr on SPLIT_BY and store the segment offsets and length in
   * array */
  కొరకు(pos = 0; pos < len; pos++) {
    ఒకవేళ(ఆధారం[pos] == విభజించు_BY) {
      ఒకవేళ(segment_నిడివి) {
        curlx_dyn_init(&db[సూచీ], segment_నిడివి + 1);
        ఫలితం = curlx_dyn_addn(&db[సూచీ], &ఆధారం[ప్రారంభం], segment_నిడివి);
        ఒకవేళ(ఫలితం)
          వెళ్ళు fail;

        segment_నిడివి = 0;
        సూచీ++;
        ఒకవేళ(++అంకె_splits == MAX_QUERY_COMPONENTS) {
          ఫలితం = CURLE_TOO_LARGE;
          వెళ్ళు fail;
        }
      }
      ప్రారంభం = pos + 1;
    }
    లేకపోతే {
      segment_నిడివి++;
    }
  }

  ఒకవేళ(segment_నిడివి) {
    curlx_dyn_init(&db[సూచీ], segment_నిడివి + 1);
    ఫలితం = curlx_dyn_addn(&db[సూచీ], &ఆధారం[ప్రారంభం], segment_నిడివి);
    ఒకవేళ(!ఫలితం && ++అంకె_splits == MAX_QUERY_COMPONENTS)
      ఫలితం = CURLE_TOO_LARGE;
  }
fail:
  *అంకె_splits_బహిర్గతం = అంకె_splits;
  తిరిగి ఫలితం;
}

స్థిర bool is_reserved_char(స్థిరరాశి అక్షరం c)
{
  తిరిగి (ISALNUM(c) || ISURLPUNTCS(c));
}

స్థిర CURLcode uri_encode_మార్గం(నిర్మాణం Curl_str *original_మార్గం,
                                నిర్మాణం dynbuf *new_మార్గం)
{
  స్థిరరాశి అక్షరం *p = curlx_str(original_మార్గం);
  size_t i;

  కొరకు(i = 0; i < curlx_strlen(original_మార్గం); i++) {
    /* Do not encode slashes or unreserved chars from RFC 3986 */
    CURLcode ఫలితం = CURLE_OK;
    గుర్తులేని అక్షరం c = p[i];
    ఒకవేళ(is_reserved_char(c) || c == '/')
      ఫలితం = curlx_dyn_addn(new_మార్గం, &c, 1);
    లేకపోతే
      ఫలితం = curlx_dyn_addf(new_మార్గం, "%%%02X", c);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  తిరిగి CURLE_OK;
}

/* Normalize the query part. Make sure %2B is left percent encoded, and not
   decoded to plus, then encoded to space. */
స్థిర CURLcode normalize_query(స్థిరరాశి అక్షరం *string, size_t len,
                                నిర్మాణం dynbuf *db)
{
  CURLcode ఫలితం = CURLE_OK;

  అయితే(len && !ఫలితం) {
    గుర్తులేని అక్షరం లోపల = (గుర్తులేని అక్షరం)*string;
    ఒకవేళ(('%' == లోపల) && (len > 2) &&
       ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      లోపల = (గుర్తులేని అక్షరం)((curlx_hexval(string[1]) << 4) |
                           curlx_hexval(string[2]));
      string += 3;
      len -= 3;
      ఒకవేళ(లోపల == '+') {
        /* decodes to plus, so leave this encoded */
        ఫలితం = curlx_dyn_addn(db, "%2B", 3);
        కొనసాగు;
      }
    }
    లేకపోతే {
      string++;
      len--;
    }

    ఒకవేళ(is_reserved_char(లోపల))
      /* Escape unreserved chars from RFC 3986 */
      ఫలితం = curlx_dyn_addn(db, &లోపల, 1);
    లేకపోతే ఒకవేళ(లోపల == '+')
      /* Encode '+' as space */
      ఫలితం = curlx_dyn_కూడు(db, "%20");
    లేకపోతే
      ఫలితం = curlx_dyn_addf(db, "%%%02X", లోపల);
  }

  తిరిగి ఫలితం;
}

స్థిర bool should_urlencode(నిర్మాణం Curl_str *service_name)
{
  /*
   * These services require unmodified (not additionally URL-encoded) URL
   * paths. They are the S3 signing names, which have disableDoubleEncoding
   * set in the AWS S3 endpoint rules. Urls are already normalized by the curl
   * URL parser
   */
  ఒకవేళ(curlx_str_cmp(service_name, "s3") ||
     curlx_str_cmp(service_name, "s3express") ||
     curlx_str_cmp(service_name, "s3-outposts") ||
     curlx_str_cmp(service_name, "s3-object-lambda")) {
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

/* maximum length for the aws sivg4 parts */
#నిర్వచించు MAX_SIGV4_LEN    64
#నిర్వచించు DATE_HDR_కీలకం_LEN (MAX_SIGV4_LEN + పరిమాణం("X--Date"))

/* string been x-PROVIDER-date:TIMESTAMP, I need +1 for ':' */
#నిర్వచించు DATE_నిండిన_HDR_LEN (DATE_HDR_కీలకం_LEN + TIMESTAMP_కొలత + 1)

/* alphabetically compare two headers by their name, expecting
   headers to use ':' at this point */
స్థిర పూర్ణాంకం compare_శీర్షిక_పేర్లు(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b)
{
  స్థిరరాశి అక్షరం *colon_a;
  స్థిరరాశి అక్షరం *colon_b;
  size_t len_a;
  size_t len_b;
  size_t min_len;
  పూర్ణాంకం cmp;

  colon_a = strchr(a, ':');
  colon_b = strchr(b, ':');

  DEBUGASSERT(colon_a);
  DEBUGASSERT(colon_b);

  len_a = colon_a ? (size_t)(colon_a - a) : అక్షరశ్రేణి_పొడవు(a);
  len_b = colon_b ? (size_t)(colon_b - b) : అక్షరశ్రేణి_పొడవు(b);

  min_len = (len_a < len_b) ? len_a : len_b;

  cmp = అక్షరశ్రేణి_పోలిక(a, b, min_len);

  /* return the shorter of the two if one is shorter */
  ఒకవేళ(!cmp)
    తిరిగి (పూర్ణాంకం)(len_a - len_b);

  తిరిగి cmp;
}

/* Merge duplicate header definitions by comma delimiting their values
   in the order defined the headers are defined, expecting headers to
   be alpha-sorted and use ':' at this point */
స్థిర CURLcode విలీనం_duplicate_శీర్షికలు(నిర్మాణం curl_slist *తల)
{
  నిర్మాణం curl_slist *curr = తల;
  CURLcode ఫలితం = CURLE_OK;

  అయితే(curr) {
    నిర్మాణం curl_slist *తదుపరి = curr->తదుపరి;
    ఒకవేళ(!తదుపరి)
      ఆపు;

    ఒకవేళ(compare_శీర్షిక_పేర్లు(curr->దత్తాంశం, తదుపరి->దత్తాంశం) == 0) {
      నిర్మాణం dynbuf buf;
      స్థిరరాశి అక్షరం *colon_తదుపరి;
      స్థిరరాశి అక్షరం *val_తదుపరి;

      curlx_dyn_init(&buf, CURL_MAX_HTTP_శీర్షిక);

      ఫలితం = curlx_dyn_కూడు(&buf, curr->దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      colon_తదుపరి = strchr(తదుపరి->దత్తాంశం, ':');
      DEBUGASSERT(colon_తదుపరి);
      val_తదుపరి = colon_తదుపరి + 1;

      ఫలితం = curlx_dyn_addn(&buf, ",", 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఫలితం = curlx_dyn_కూడు(&buf, val_తదుపరి);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      curlx_free(curr->దత్తాంశం);
      curr->దత్తాంశం = curlx_dyn_ptr(&buf);

      curr->తదుపరి = తదుపరి->తదుపరి;
      curlx_free(తదుపరి->దత్తాంశం);
      curlx_free(తదుపరి);
    }
    లేకపోతే {
      curr = curr->తదుపరి;
    }
  }

  తిరిగి CURLE_OK;
}

/* timestamp should point to a buffer of at last TIMESTAMP_SIZE bytes */
స్థిర CURLcode make_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                             అక్షరం *timestamp,
                             స్థిరరాశి అక్షరం *provider1,
                             size_t plen, /* length of provider1 */
                             అక్షరం **date_శీర్షిక,
                             అక్షరం *content_sha256_శీర్షిక,
                             నిర్మాణం dynbuf *canonical_శీర్షికలు,
                             నిర్మాణం dynbuf *signed_శీర్షికలు)
{
  అక్షరం date_hdr_కీలకం[DATE_HDR_కీలకం_LEN];
  అక్షరం date_నిండిన_hdr[DATE_నిండిన_HDR_LEN];
  నిర్మాణం curl_slist *తల = ఖాళీ;
  నిర్మాణం curl_slist *tmp_తల = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  నిర్మాణం curl_slist *l;
  bool again = TRUE;

  curl_msnprintf(date_hdr_కీలకం, DATE_HDR_కీలకం_LEN, "X-%.*s-Date",
                 (పూర్ణాంకం)plen, provider1);
  /* provider1 ucfirst */
  Curl_strntolower(&date_hdr_కీలకం[2], provider1, plen);
  date_hdr_కీలకం[2] = Curl_raw_toupper(provider1[0]);

  curl_msnprintf(date_నిండిన_hdr, DATE_నిండిన_HDR_LEN,
                 "x-%.*s-date:%s", (పూర్ణాంకం)plen, provider1, timestamp);
  /* provider1 lowercase */
  Curl_strntolower(&date_నిండిన_hdr[2], provider1, plen);

  ఒకవేళ(!Curl_checkheaders(దత్తాంశం, STRCONST("Host")) &&
     దత్తాంశం->state.http_host) {
    /* Host: [host]:[port] */
    అక్షరం *fullhost = curlx_strdup(దత్తాంశం->state.http_host);

    ఒకవేళ(fullhost)
      తల = Curl_slist_append_nodup(ఖాళీ, fullhost);
    ఒకవేళ(!తల) {
      curlx_free(fullhost);
      వెళ్ళు fail;
    }
  }

  ఒకవేళ(*content_sha256_శీర్షిక) {
    tmp_తల = curl_slist_append(తల, content_sha256_శీర్షిక);
    ఒకవేళ(!tmp_తల)
      వెళ్ళు fail;
    తల = tmp_తల;
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
  కొరకు(l = దత్తాంశం->సమితి.శీర్షికలు; l; l = l->తదుపరి) {
    అక్షరం *dupdata;
    స్థిరరాశి అక్షరం *ptr;
    స్థిరరాశి అక్షరం *sep = strchr(l->దత్తాంశం, ':');
    ఒకవేళ(!sep)
      sep = strchr(l->దత్తాంశం, ';');
    ఒకవేళ(!sep || (*sep == ':' && !*(sep + 1)))
      కొనసాగు;
    కొరకు(ptr = sep + 1; ISBLANK(*ptr); ++ptr)
      ;
    ఒకవేళ(!*ptr && ptr != sep + 1) /* a value of whitespace only */
      కొనసాగు;
    dupdata = curlx_strdup(l->దత్తాంశం);
    ఒకవేళ(!dupdata)
      వెళ్ళు fail;
    dupdata[sep - l->దత్తాంశం] = ':';
    tmp_తల = Curl_slist_append_nodup(తల, dupdata);
    ఒకవేళ(!tmp_తల) {
      curlx_free(dupdata);
      వెళ్ళు fail;
    }
    తల = tmp_తల;
  }

  trim_శీర్షికలు(తల);

  *date_శీర్షిక = కనుగొను_date_hdr(దత్తాంశం, date_hdr_కీలకం);
  ఒకవేళ(!*date_శీర్షిక) {
    tmp_తల = curl_slist_append(తల, date_నిండిన_hdr);
    ఒకవేళ(!tmp_తల)
      వెళ్ళు fail;
    తల = tmp_తల;
    *date_శీర్షిక = curl_maprintf("%s: %s\r\n", date_hdr_కీలకం, timestamp);
    ఒకవేళ(!*date_శీర్షిక)
      వెళ్ళు fail;
  }
  లేకపోతే {
    స్థిరరాశి అక్షరం *విలువ;
    స్థిరరాశి అక్షరం *endp;
    విలువ = strchr(*date_శీర్షిక, ':');
    ఒకవేళ(!విలువ) {
      *date_శీర్షిక = ఖాళీ;
      వెళ్ళు fail;
    }
    ++విలువ;
    curlx_str_passblanks(&విలువ);
    endp = విలువ;
    అయితే(*endp && ISALNUM(*endp))
      ++endp;
    /* 16 bytes => "19700101T000000Z" */
    ఒకవేళ((endp - విలువ) == TIMESTAMP_కొలత - 1) {
      జ్ఞాపకం_నకలు(timestamp, విలువ, TIMESTAMP_కొలత - 1);
      timestamp[TIMESTAMP_కొలత - 1] = 0;
    }
    లేకపోతే
      /* bad timestamp length */
      timestamp[0] = 0;
    *date_శీర్షిక = ఖాళీ;
  }

  /* alpha-sort by header name in a case sensitive manner */
  చేయి {
    again = FALSE;
    కొరకు(l = తల; l; l = l->తదుపరి) {
      నిర్మాణం curl_slist *తదుపరి = l->తదుపరి;

      ఒకవేళ(తదుపరి && compare_శీర్షిక_పేర్లు(l->దత్తాంశం, తదుపరి->దత్తాంశం) > 0) {
        అక్షరం *tmp = l->దత్తాంశం;

        l->దత్తాంశం = తదుపరి->దత్తాంశం;
        తదుపరి->దత్తాంశం = tmp;
        again = TRUE;
      }
    }
  } అయితే(again);

  ఫలితం = విలీనం_duplicate_శీర్షికలు(తల);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  కొరకు(l = తల; l; l = l->తదుపరి) {
    అక్షరం *tmp;

    ఒకవేళ(curlx_dyn_కూడు(canonical_శీర్షికలు, l->దత్తాంశం))
      వెళ్ళు fail;
    ఒకవేళ(curlx_dyn_కూడు(canonical_శీర్షికలు, "\n"))
      వెళ్ళు fail;

    tmp = strchr(l->దత్తాంశం, ':');
    ఒకవేళ(tmp)
      *tmp = 0;

    ఒకవేళ(l != తల && curlx_dyn_కూడు(signed_శీర్షికలు, ";"))
      వెళ్ళు fail;
    ఒకవేళ(curlx_dyn_కూడు(signed_శీర్షికలు, l->దత్తాంశం))
      వెళ్ళు fail;
  }

  ఫలితం = CURLE_OK;
fail:
  curl_slist_free_all(తల);

  తిరిగి ఫలితం;
}

#నిర్వచించు CONTENT_SHA256_కీలకం_LEN (MAX_SIGV4_LEN + పరిమాణం("X--Content-Sha256"))
/* add 2 for ": " between header name and value */
#నిర్వచించు CONTENT_SHA256_HDR_LEN (CONTENT_SHA256_కీలకం_LEN + 2 + SHA256_HEX_నిడివి)

/* try to parse a payload hash from the content-sha256 header */
స్థిర స్థిరరాశి అక్షరం *parse_content_sha_hdr(నిర్మాణం Curl_easy *దత్తాంశం,
                                         స్థిరరాశి అక్షరం *provider1,
                                         size_t plen,
                                         size_t *విలువ_len)
{
  అక్షరం కీలకం[CONTENT_SHA256_కీలకం_LEN];
  size_t కీలకం_len;
  స్థిరరాశి అక్షరం *విలువ;
  size_t len;

  కీలకం_len = curl_msnprintf(కీలకం, పరిమాణం(కీలకం), "x-%.*s-content-sha256",
                           (పూర్ణాంకం)plen, provider1);

  విలువ = Curl_checkheaders(దత్తాంశం, కీలకం, కీలకం_len);
  ఒకవేళ(!విలువ)
    తిరిగి ఖాళీ;

  విలువ = strchr(విలువ, ':');
  ఒకవేళ(!విలువ)
    తిరిగి ఖాళీ;
  ++విలువ;

  curlx_str_passblanks(&విలువ);

  len = అక్షరశ్రేణి_పొడవు(విలువ);
  అయితే(len > 0 && ISBLANK(విలువ[len - 1]))
    --len;

  *విలువ_len = len;
  తిరిగి విలువ;
}

స్థిర CURLcode calc_payload_hash(నిర్మాణం Curl_easy *దత్తాంశం,
                                  గుర్తులేని అక్షరం *sha_hash, అక్షరం *sha_hex)
{
  స్థిరరాశి అక్షరం *post_దత్తాంశం = దత్తాంశం->సమితి.postfields;
  size_t post_దత్తాంశం_len = 0;
  CURLcode ఫలితం;

  ఒకవేళ(post_దత్తాంశం) {
    ఒకవేళ(దత్తాంశం->సమితి.postfieldsize < 0)
      post_దత్తాంశం_len = అక్షరశ్రేణి_పొడవు(post_దత్తాంశం);
    లేకపోతే
      post_దత్తాంశం_len = (size_t)దత్తాంశం->సమితి.postfieldsize;
  }
  ఫలితం = Curl_sha256it(sha_hash, (స్థిరరాశి గుర్తులేని అక్షరం *)post_దత్తాంశం,
                         post_దత్తాంశం_len);
  ఒకవేళ(!ఫలితం)
    sha256_గమ్యం_hex(sha_hex, sha_hash);
  తిరిగి ఫలితం;
}

#నిర్వచించు S3_UNSIGNED_PAYLOAD "UNSIGNED-PAYLOAD"

స్థిర CURLcode calc_s3_payload_hash(నిర్మాణం Curl_easy *దత్తాంశం,
                                     Curl_HttpReq httpreq,
                                     స్థిరరాశి అక్షరం *provider1,
                                     size_t plen,
                                     గుర్తులేని అక్షరం *sha_hash,
                                     అక్షరం *sha_hex, అక్షరం *శీర్షిక)
{
  bool ఖాళీలు_పద్ధతి = (httpreq == HTTPREQ_GET || httpreq == HTTPREQ_తల);
  /* The request method or filesize indicate no request payload */
  bool ఖాళీలు_payload = (ఖాళీలు_పద్ధతి || దత్తాంశం->సమితి.filesize == 0);
  /* The POST payload is in memory */
  bool post_payload = (httpreq == HTTPREQ_POST && దత్తాంశం->సమితి.postfields);
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(ఖాళీలు_payload || post_payload) {
    /* Calculate a real hash when we know the request payload */
    ఫలితం = calc_payload_hash(దత్తాంశం, sha_hash, sha_hex);
    ఒకవేళ(ఫలితం)
      వెళ్ళు fail;
  }
  లేకపోతే {
    /* Fall back to s3's UNSIGNED-PAYLOAD */
    size_t len = CURL_CSTRLEN(S3_UNSIGNED_PAYLOAD);
    DEBUGASSERT(len < SHA256_HEX_నిడివి); /* 16 < 65 */
    జ్ఞాపకం_నకలు(sha_hex, S3_UNSIGNED_PAYLOAD, len);
    sha_hex[len] = 0;
  }

  /* format the required content-sha256 header */
  curl_msnprintf(శీర్షిక, CONTENT_SHA256_HDR_LEN,
                 "x-%.*s-content-sha256: %s", (పూర్ణాంకం)plen, provider1, sha_hex);

  ఫలితం = CURLE_OK;
fail:
  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం compare_func(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
  స్థిరరాశి నిర్మాణం pair *aa = a;
  స్థిరరాశి నిర్మాణం pair *bb = b;
  స్థిరరాశి size_t aa_కీలకం_len = curlx_dyn_len(&aa->కీలకం);
  స్థిరరాశి size_t bb_కీలకం_len = curlx_dyn_len(&bb->కీలకం);
  స్థిరరాశి size_t aa_విలువ_len = curlx_dyn_len(&aa->విలువ);
  స్థిరరాశి size_t bb_విలువ_len = curlx_dyn_len(&bb->విలువ);
  పూర్ణాంకం compare;

  /* If one element is empty, the other is always sorted higher */

  /* Compare keys */
  ఒకవేళ((aa_కీలకం_len == 0) && (bb_కీలకం_len == 0))
    తిరిగి 0;
  ఒకవేళ(aa_కీలకం_len == 0)
    తిరిగి -1;
  ఒకవేళ(bb_కీలకం_len == 0)
    తిరిగి 1;
  compare = strcmp(curlx_dyn_ptr(&aa->కీలకం), curlx_dyn_ptr(&bb->కీలకం));
  ఒకవేళ(compare) {
    తిరిగి compare;
  }

  /* Compare values */
  ఒకవేళ((aa_విలువ_len == 0) && (bb_విలువ_len == 0))
    తిరిగి 0;
  ఒకవేళ(aa_విలువ_len == 0)
    తిరిగి -1;
  ఒకవేళ(bb_విలువ_len == 0)
    తిరిగి 1;
  compare = strcmp(curlx_dyn_ptr(&aa->విలువ), curlx_dyn_ptr(&bb->విలువ));

  తిరిగి compare;
}

/* @unittest 1979 */
UNITTEST CURLcode canon_మార్గం(స్థిరరాశి అక్షరం *q, size_t len,
                             నిర్మాణం dynbuf *new_మార్గం,
                             bool do_uri_encode);
UNITTEST CURLcode canon_మార్గం(స్థిరరాశి అక్షరం *q, size_t len,
                             నిర్మాణం dynbuf *new_మార్గం,
                             bool do_uri_encode)
{
  CURLcode ఫలితం = CURLE_OK;

  నిర్మాణం Curl_str original_మార్గం;

  curlx_str_కేటాయించు(&original_మార్గం, q, len);

  /* Normalized path will be either the same or shorter than the original
   * path, plus trailing slash */

  ఒకవేళ(do_uri_encode)
    ఫలితం = uri_encode_మార్గం(&original_మార్గం, new_మార్గం);
  లేకపోతే
    ఫలితం = curlx_dyn_addn(new_మార్గం, q, len);

  ఒకవేళ(!ఫలితం && curlx_dyn_len(new_మార్గం) == 0)
    ఫలితం = curlx_dyn_కూడు(new_మార్గం, "/");

  తిరిగి ఫలితం;
}

/* @unittest 1980 */
UNITTEST CURLcode canon_query(స్థిరరాశి అక్షరం *query, నిర్మాణం dynbuf *dq);
UNITTEST CURLcode canon_query(స్థిరరాశి అక్షరం *query, నిర్మాణం dynbuf *dq)
{
  CURLcode ఫలితం = CURLE_OK;

  నిర్మాణం dynbuf query_array[MAX_QUERY_COMPONENTS];
  నిర్మాణం pair encoded_query_array[MAX_QUERY_COMPONENTS];
  size_t అంకె_query_components;
  size_t counted_query_components = 0;
  size_t సూచీ;

  ఒకవేళ(!query)
    తిరిగి ఫలితం;

  ఫలితం = విభజించు_గమ్యం_dyn_array(query, &query_array[0], &అంకె_query_components);
  ఒకవేళ(ఫలితం) {
    వెళ్ళు fail;
  }

  /* Create list of pairs, each pair containing an encoded query
   * component */

  కొరకు(సూచీ = 0; సూచీ < అంకె_query_components; సూచీ++) {
    స్థిరరాశి అక్షరం *లోపల_కీలకం;
    size_t లోపల_కీలకం_len;
    స్థిరరాశి అక్షరం *స్థానభ్రంశం;
    size_t query_part_len = curlx_dyn_len(&query_array[సూచీ]);
    స్థిరరాశి అక్షరం *query_part = curlx_dyn_ptr(&query_array[సూచీ]);

    లోపల_కీలకం = query_part;

    స్థానభ్రంశం = strchr(query_part, '=');
    /* If there is no equals, this key has no value */
    ఒకవేళ(!స్థానభ్రంశం) {
      లోపల_కీలకం_len = అక్షరశ్రేణి_పొడవు(లోపల_కీలకం);
    }
    లేకపోతే {
      లోపల_కీలకం_len = స్థానభ్రంశం - లోపల_కీలకం;
    }

    curlx_dyn_init(&encoded_query_array[సూచీ].కీలకం,
      (query_part_len * 3) + 1);
    curlx_dyn_init(&encoded_query_array[సూచీ].విలువ,
      (query_part_len * 3) + 1);
    counted_query_components++;

    /* Decode/encode the key */
    ఫలితం = normalize_query(లోపల_కీలకం, లోపల_కీలకం_len,
                             &encoded_query_array[సూచీ].కీలకం);
    ఒకవేళ(ఫలితం) {
      వెళ్ళు fail;
    }

    /* Decode/encode the value if it exists */
    ఒకవేళ(స్థానభ్రంశం && స్థానభ్రంశం != (query_part + query_part_len - 1)) {
      size_t లోపల_విలువ_len;
      స్థిరరాశి అక్షరం *లోపల_విలువ = స్థానభ్రంశం + 1;
      లోపల_విలువ_len = query_part + query_part_len - (స్థానభ్రంశం + 1);
      ఫలితం = normalize_query(లోపల_విలువ, లోపల_విలువ_len,
                               &encoded_query_array[సూచీ].విలువ);
      ఒకవేళ(ఫలితం) {
        వెళ్ళు fail;
      }
    }
    లేకపోతే {
      /* If there is no value, the value is an empty string */
      curlx_dyn_init(&encoded_query_array[సూచీ].విలువ, 2);
      ఫలితం = curlx_dyn_addn(&encoded_query_array[సూచీ].విలువ, "", 1);
    }

    ఒకవేళ(ఫలితం) {
      వెళ్ళు fail;
    }
  }

  /* Sort the encoded query components by key and value */
  qsort(&encoded_query_array, అంకె_query_components,
        పరిమాణం(నిర్మాణం pair), compare_func);

  /* Append the query components together to make a full query string */
  కొరకు(సూచీ = 0; సూచీ < అంకె_query_components; సూచీ++) {

    ఒకవేళ(సూచీ)
      ఫలితం = curlx_dyn_addn(dq, "&", 1);
    ఒకవేళ(!ఫలితం) {
      స్థిరరాశి అక్షరం *కీలకం_ptr = curlx_dyn_ptr(&encoded_query_array[సూచీ].కీలకం);
      స్థిరరాశి అక్షరం *విలువ_ptr = curlx_dyn_ptr(&encoded_query_array[సూచీ].విలువ);
      size_t vlen = curlx_dyn_len(&encoded_query_array[సూచీ].విలువ);
      ఒకవేళ(విలువ_ptr && vlen) {
        ఫలితం = curlx_dyn_addf(dq, "%s=%s", కీలకం_ptr, విలువ_ptr);
      }
      లేకపోతే {
        /* Empty value is always encoded to key= */
        ఫలితం = curlx_dyn_addf(dq, "%s=", కీలకం_ptr);
      }
    }
    ఒకవేళ(ఫలితం)
      ఆపు;
  }

fail:
  ఒకవేళ(counted_query_components)
    /* the encoded_query_array might not be initialized yet */
    pair_array_free(&encoded_query_array[0], counted_query_components);
  dyn_array_free(&query_array[0], అంకె_query_components);
  తిరిగి ఫలితం;
}

స్థిర CURLcode parse_sigv4_params(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి అక్షరం *hostname,
                                   నిర్మాణం Curl_str *provider0,
                                   నిర్మాణం Curl_str *provider1,
                                   నిర్మాణం Curl_str *region,
                                   నిర్మాణం Curl_str *service)
{
  స్థిరరాశి అక్షరం *పంక్తి = CURL_EASY_STR(దత్తాంశం, STRING_AWS_SIGV4);
  ఒకవేళ(!పంక్తి || !*పంక్తి)
    పంక్తి = "aws:amz";

  /* provider0[:provider1[:region[:service]]]

     No string can be longer than N bytes of non-whitespace */
  ఒకవేళ(curlx_str_until(&పంక్తి, provider0, MAX_SIGV4_LEN, ':')) {
    failf(దత్తాంశం, "first aws-sigv4 provider cannot be empty");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  ఒకవేళ(curlx_str_single(&పంక్తి, ':') ||
     curlx_str_until(&పంక్తి, provider1, MAX_SIGV4_LEN, ':')) {
    *provider1 = *provider0;
  }
  లేకపోతే ఒకవేళ(curlx_str_single(&పంక్తి, ':') ||
          curlx_str_until(&పంక్తి, region, MAX_SIGV4_LEN, ':') ||
          curlx_str_single(&పంక్తి, ':') ||
          curlx_str_until(&పంక్తి, service, MAX_SIGV4_LEN, ':')) {
    /* nothing to do */
  }

  ఒకవేళ(!curlx_strlen(service)) {
    స్థిరరాశి అక్షరం *p = hostname;
    ఒకవేళ(curlx_str_until(&p, service, MAX_SIGV4_LEN, '.') ||
       curlx_str_single(&p, '.')) {
      failf(దత్తాంశం, "aws-sigv4: service missing in parameters and hostname");
      తిరిగి CURLE_URL_MALFORMAT;
    }

    infof(దత్తాంశం, "aws_sigv4: picked service %.*s from host",
          (పూర్ణాంకం)curlx_strlen(service), curlx_str(service));

    ఒకవేళ(!curlx_strlen(region)) {
      ఒకవేళ(curlx_str_until(&p, region, MAX_SIGV4_LEN, '.') ||
         curlx_str_single(&p, '.')) {
        failf(దత్తాంశం, "aws-sigv4: region missing in parameters and hostname");
        తిరిగి CURLE_URL_MALFORMAT;
      }
      infof(దత్తాంశం, "aws_sigv4: picked region %.*s from host",
            (పూర్ణాంకం)curlx_strlen(region), curlx_str(region));
    }
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode get_payload_hash(నిర్మాణం Curl_easy *దత్తాంశం,
                                 Curl_HttpReq httpreq,
                                 నిర్మాణం Curl_str *provider0,
                                 నిర్మాణం Curl_str *provider1,
                                 నిర్మాణం Curl_str *service,
                                 గుర్తులేని అక్షరం *sha_hash,
                                 అక్షరం *sha_hex,
                                 అక్షరం *content_sha256_hdr,
                                 స్థిరరాశి అక్షరం **payload_hash_బహిర్గతం,
                                 size_t *payload_hash_len_బహిర్గతం)
{
  *payload_hash_బహిర్గతం =
    parse_content_sha_hdr(దత్తాంశం, curlx_str(provider1),
                          curlx_strlen(provider1), payload_hash_len_బహిర్గతం);

  ఒకవేళ(!*payload_hash_బహిర్గతం) {
    CURLcode ఫలితం;
    /* AWS S3 requires a x-amz-content-sha256 header, and supports special
     * values like UNSIGNED-PAYLOAD */
    bool sign_as_s3 = curlx_str_casecompare(provider0, "aws") &&
                      curlx_str_casecompare(service, "s3");

    ఒకవేళ(sign_as_s3)
      ఫలితం = calc_s3_payload_hash(దత్తాంశం, httpreq, curlx_str(provider1),
                                    curlx_strlen(provider1), sha_hash,
                                    sha_hex, content_sha256_hdr);
    లేకపోతే
      ఫలితం = calc_payload_hash(దత్తాంశం, sha_hash, sha_hex);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    *payload_hash_బహిర్గతం = sha_hex;
    /* may be shorter than SHA256_HEX_LENGTH, like S3_UNSIGNED_PAYLOAD */
    *payload_hash_len_బహిర్గతం = అక్షరశ్రేణి_పొడవు(sha_hex);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode get_timestamp(అక్షరం *timestamp, size_t stampsize)
{
  time_t clock;
  నిర్మాణం tm tm;
  CURLcode ఫలితం;

#నిర్వచితమైతే DEBUGBUILD
  {
    అక్షరం *force_timestamp = getenv("CURL_FORCETIME");
    ఒకవేళ(force_timestamp)
      clock = 0;
    లేకపోతే
      clock = సమయం(ఖాళీ);
  }
#లేకపోతే
  clock = సమయం(ఖాళీ);
#అయితే_ముగింపు
  ఫలితం = curlx_gmtime(clock, &tm);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(!strftime(timestamp, stampsize, "%Y%m%dT%H%M%SZ", &tm))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode make_canonical_request(నిర్మాణం Curl_easy *దత్తాంశం,
                                       అక్షరం *timestamp,
                                       నిర్మాణం Curl_str *provider1,
                                       నిర్మాణం Curl_str *service,
                                       స్థిరరాశి అక్షరం *పద్ధతి,
                                       స్థిరరాశి అక్షరం *payload_hash,
                                       size_t payload_hash_len,
                                       అక్షరం **date_శీర్షిక_బహిర్గతం,
                                       అక్షరం *content_sha256_hdr,
                                       నిర్మాణం dynbuf *canonical_శీర్షికలు,
                                       నిర్మాణం dynbuf *signed_శీర్షికలు,
                                       అక్షరం **canonical_request_బహిర్గతం)
{
  నిర్మాణం dynbuf canonical_query;
  నిర్మాణం dynbuf canonical_మార్గం;
  CURLcode ఫలితం;

  curlx_dyn_init(&canonical_query, CURL_MAX_HTTP_శీర్షిక);
  curlx_dyn_init(&canonical_మార్గం, CURL_MAX_HTTP_శీర్షిక);

  ఫలితం = make_శీర్షికలు(దత్తాంశం, timestamp,
                        curlx_str(provider1), curlx_strlen(provider1),
                        date_శీర్షిక_బహిర్గతం, content_sha256_hdr,
                        canonical_శీర్షికలు, signed_శీర్షికలు);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఫలితం = canon_query(దత్తాంశం->state.పైకి.query, &canonical_query);
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  ఫలితం = canon_మార్గం(దత్తాంశం->state.పైకి.మార్గం, అక్షరశ్రేణి_పొడవు(దత్తాంశం->state.పైకి.మార్గం),
                      &canonical_మార్గం,
                      should_urlencode(service));
  ఒకవేళ(ఫలితం)
    వెళ్ళు fail;

  *canonical_request_బహిర్గతం =
    curl_maprintf("%s\n" /* HTTPRequestMethod */
                  "%s\n" /* CanonicalURI */
                  "%s\n" /* CanonicalQueryString */
                  "%s\n" /* CanonicalHeaders */
                  "%s\n" /* SignedHeaders */
                  "%.*s",  /* HashedRequestPayload in hex */
                  పద్ధతి,
                  curlx_dyn_ptr(&canonical_మార్గం),
                  curlx_dyn_ptr(&canonical_query) ?
                  curlx_dyn_ptr(&canonical_query) : "",
                  curlx_dyn_ptr(canonical_శీర్షికలు),
                  curlx_dyn_ptr(signed_శీర్షికలు),
                  (పూర్ణాంకం)payload_hash_len, payload_hash);
  ఒకవేళ(!*canonical_request_బహిర్గతం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail;
  }

  ఫలితం = CURLE_OK;
fail:
  curlx_dyn_free(&canonical_query);
  curlx_dyn_free(&canonical_మార్గం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode make_string_గమ్యం_sign(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం Curl_str *provider0,
                                    నిర్మాణం Curl_str *region,
                                    నిర్మాణం Curl_str *service,
                                    స్థిరరాశి అక్షరం *date,
                                    స్థిరరాశి అక్షరం *timestamp,
                                    స్థిరరాశి అక్షరం *canonical_request,
                                    అక్షరం **request_type_బహిర్గతం,
                                    అక్షరం **credential_scope_బహిర్గతం,
                                    అక్షరం **str_గమ్యం_sign_బహిర్గతం)
{
  అక్షరం *request_type;
  అక్షరం *credential_scope;
  అక్షరం *str_గమ్యం_sign;
  గుర్తులేని అక్షరం sha_hash[CURL_SHA256_DIGEST_నిడివి];
  అక్షరం sha_hex[SHA256_HEX_నిడివి];

  request_type = curl_maprintf("%.*s4_request",
                               (పూర్ణాంకం)curlx_strlen(provider0),
                               curlx_str(provider0));
  ఒకవేళ(!request_type)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* provider0 is lowercased *after* curl_maprintf() so that the buffer
     can be written to */
  Curl_strntolower(request_type, request_type, curlx_strlen(provider0));

  credential_scope = curl_maprintf("%s/%.*s/%.*s/%s", date,
                                   (పూర్ణాంకం)curlx_strlen(region),
                                   curlx_str(region),
                                   (పూర్ణాంకం)curlx_strlen(service),
                                   curlx_str(service),
                                   request_type);
  ఒకవేళ(!credential_scope) {
    curlx_free(request_type);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(Curl_sha256it(sha_hash, (స్థిరరాశి గుర్తులేని అక్షరం *)canonical_request,
                   అక్షరశ్రేణి_పొడవు(canonical_request))) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  sha256_గమ్యం_hex(sha_hex, sha_hash);

  /*
   * Google allows using RSA key instead of HMAC, so this code might change
   * in the future. For now we only support HMAC.
   */
  str_గమ్యం_sign = curl_maprintf("%.*s4-HMAC-SHA256\n" /* Algorithm */
                              "%s\n" /* RequestDateTime */
                              "%s\n" /* CredentialScope */
                              "%s",  /* HashedCanonicalRequest in hex */
                              (పూర్ణాంకం)curlx_strlen(provider0),
                              curlx_str(provider0),
                              timestamp,
                              credential_scope,
                              sha_hex);
  ఒకవేళ(!str_గమ్యం_sign) {
    curlx_free(request_type);
    curlx_free(credential_scope);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* make provider0 part done uppercase */
  Curl_strntoupper(str_గమ్యం_sign, curlx_str(provider0),
                   curlx_strlen(provider0));

  infof(దత్తాంశం, "aws_sigv4: String to sign (enclosed in []) - [%s]",
        str_గమ్యం_sign);

  *request_type_బహిర్గతం = request_type;
  *credential_scope_బహిర్గతం = credential_scope;
  *str_గమ్యం_sign_బహిర్గతం = str_గమ్యం_sign;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode sign_and_సమితి_auth_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_str *provider0,
                                          నిర్మాణం Curl_str *region,
                                          నిర్మాణం Curl_str *service,
                                          స్థిరరాశి అక్షరం *request_type,
                                          స్థిరరాశి అక్షరం *credential_scope,
                                          స్థిరరాశి అక్షరం *date,
                                          స్థిరరాశి అక్షరం *str_గమ్యం_sign,
                                          స్థిరరాశి అక్షరం *date_శీర్షిక,
                                          స్థిరరాశి అక్షరం *content_sha256_hdr,
                                          నిర్మాణం dynbuf *signed_శీర్షికలు)
{
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  స్థిరరాశి అక్షరం *passwd = Curl_creds_passwd(దత్తాంశం->state.creds);
  అక్షరం *secret = ఖాళీ;
  గుర్తులేని అక్షరం sign0[CURL_SHA256_DIGEST_నిడివి] = { 0 };
  గుర్తులేని అక్షరం sign1[CURL_SHA256_DIGEST_నిడివి] = { 0 };
  అక్షరం sha_hex[SHA256_HEX_నిడివి];
  అక్షరం *auth_శీర్షికలు = ఖాళీ;
  అక్షరం *user = curl_escape(Curl_creds_user(దత్తాంశం->state.creds), 0);
  ఒకవేళ(!user)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  secret = curl_maprintf("%.*s4%s", (పూర్ణాంకం)curlx_strlen(provider0),
                         curlx_str(provider0), passwd);
  ఒకవేళ(!secret)
    వెళ్ళు fail;
  /* make provider0 part done uppercase */
  Curl_strntoupper(secret, curlx_str(provider0), curlx_strlen(provider0));

  HMAC_SHA256(secret, అక్షరశ్రేణి_పొడవు(secret), date, అక్షరశ్రేణి_పొడవు(date), sign0);
  HMAC_SHA256(sign0, పరిమాణం(sign0),
              curlx_str(region), curlx_strlen(region), sign1);
  HMAC_SHA256(sign1, పరిమాణం(sign1),
              curlx_str(service), curlx_strlen(service), sign0);
  HMAC_SHA256(sign0, పరిమాణం(sign0),
              request_type, అక్షరశ్రేణి_పొడవు(request_type), sign1);
  HMAC_SHA256(sign1, పరిమాణం(sign1),
              str_గమ్యం_sign, అక్షరశ్రేణి_పొడవు(str_గమ్యం_sign), sign0);

  sha256_గమ్యం_hex(sha_hex, sign0);

  infof(దత్తాంశం, "aws_sigv4: Signature - %s", sha_hex);

  auth_శీర్షికలు = curl_maprintf("Authorization: %.*s4-HMAC-SHA256 "
                               "Credential=%s/%s, "
                               "SignedHeaders=%s, "
                               "Signature=%s\r\n"
                               "%s"
                               "%s%s",
                               (పూర్ణాంకం)curlx_strlen(provider0),
                               curlx_str(provider0),
                               user,
                               credential_scope,
                               curlx_dyn_ptr(signed_శీర్షికలు),
                               sha_hex,
                               /*
                                * date_header is added here, only if it was not
                                * user-specified (using CURLOPT_HTTPHEADER).
                                * date_header includes \r\n
                                */
                               date_శీర్షిక ? date_శీర్షిక : "",
                               content_sha256_hdr,
                               content_sha256_hdr[0] ? "\r\n": "");
  ఒకవేళ(!auth_శీర్షికలు)
    వెళ్ళు fail;

  /* provider 0 uppercase */
  Curl_strntoupper(&auth_శీర్షికలు[CURL_CSTRLEN("Authorization: ")],
                   curlx_str(provider0), curlx_strlen(provider0));

  curlx_free(దత్తాంశం->req.hd_auth);
  దత్తాంశం->req.hd_auth = auth_శీర్షికలు;
  దత్తాంశం->state.authhost.పూర్తి = TRUE;
  ఫలితం = CURLE_OK;

fail:
  curlx_free(user);
  curlx_free(secret);
  తిరిగి ఫలితం;
}

CURLcode Curl_output_aws_sigv4(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  నిర్మాణం Curl_str provider0 = { ఖాళీ, 0 };
  నిర్మాణం Curl_str provider1 = { ఖాళీ, 0 };
  నిర్మాణం Curl_str region = { ఖాళీ, 0 };
  నిర్మాణం Curl_str service = { ఖాళీ, 0 };
  స్థిరరాశి అక్షరం *hostname = దత్తాంశం->state.origin->hostname;
  అక్షరం timestamp[TIMESTAMP_కొలత];
  అక్షరం date[9];
  నిర్మాణం dynbuf canonical_శీర్షికలు;
  నిర్మాణం dynbuf signed_శీర్షికలు;
  అక్షరం *date_శీర్షిక = ఖాళీ;
  Curl_HttpReq httpreq;
  స్థిరరాశి అక్షరం *పద్ధతి = ఖాళీ;
  స్థిరరాశి అక్షరం *payload_hash = ఖాళీ;
  size_t payload_hash_len = 0;
  గుర్తులేని అక్షరం sha_hash[CURL_SHA256_DIGEST_నిడివి];
  అక్షరం sha_hex[SHA256_HEX_నిడివి];
  అక్షరం content_sha256_hdr[CONTENT_SHA256_HDR_LEN + 2] = ""; /* add \r\n */
  అక్షరం *canonical_request = ఖాళీ;
  అక్షరం *request_type = ఖాళీ;
  అక్షరం *credential_scope = ఖాళీ;
  అక్షరం *str_గమ్యం_sign = ఖాళీ;

  ఒకవేళ(దత్తాంశం->సమితి.మార్గం_as_is) {
    failf(దత్తాంశం, "Cannot use sigv4 authentication with path-as-is flag");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  ఒకవేళ(Curl_checkheaders(దత్తాంశం, STRCONST("Authorization")))
    /* Authorization already present, Bailing out */
    తిరిగి CURLE_OK;

  /* we init those buffers here, so goto fail will free initialized dynbuf */
  curlx_dyn_init(&canonical_శీర్షికలు, CURL_MAX_HTTP_శీర్షిక);
  curlx_dyn_init(&signed_శీర్షికలు, CURL_MAX_HTTP_శీర్షిక);

  ఫలితం = parse_sigv4_params(దత్తాంశం, hostname, &provider0, &provider1,
                              &region, &service);
  ఒకవేళ(!ఫలితం) {
    Curl_http_పద్ధతి(దత్తాంశం, &పద్ధతి, &httpreq);
    ఫలితం = get_payload_hash(దత్తాంశం, httpreq, &provider0, &provider1, &service,
                              sha_hash, sha_hex, content_sha256_hdr,
                              &payload_hash, &payload_hash_len);
  }

  ఒకవేళ(!ఫలితం)
    ఫలితం = get_timestamp(timestamp, పరిమాణం(timestamp));

  ఒకవేళ(!ఫలితం)
    ఫలితం = make_canonical_request(దత్తాంశం, timestamp,
                                    &provider1, &service,
                                    పద్ధతి, payload_hash, payload_hash_len,
                                    &date_శీర్షిక, content_sha256_hdr,
                                    &canonical_శీర్షికలు, &signed_శీర్షికలు,
                                    &canonical_request);
  ఒకవేళ(!ఫలితం) {
    /* the timestamp might have been updated in make_canonical_request */
    జ్ఞాపకం_నకలు(date, timestamp, పరిమాణం(date) - 1);
    date[పరిమాణం(date) - 1] = 0;

    ఫలితం = make_string_గమ్యం_sign(దత్తాంశం, &provider0, &region, &service,
                                 date, timestamp, canonical_request,
                                 &request_type, &credential_scope,
                                 &str_గమ్యం_sign);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = sign_and_సమితి_auth_శీర్షికలు(దత్తాంశం, &provider0, &region, &service,
                                       request_type, credential_scope,
                                       date, str_గమ్యం_sign, date_శీర్షిక,
                                       content_sha256_hdr, &signed_శీర్షికలు);

  curlx_dyn_free(&canonical_శీర్షికలు);
  curlx_dyn_free(&signed_శీర్షికలు);
  curlx_free(canonical_request);
  curlx_free(request_type);
  curlx_free(credential_scope);
  curlx_free(str_గమ్యం_sign);
  curlx_free(date_శీర్షిక);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_AWS */
