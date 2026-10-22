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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ಸ್ಥಿರ ಶೂನ್ಯ copy_header_external(ಸಂರಚನಾ Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 ಸಂರಚನಾ Curl_llist_ಗಂಟು *e,
                                 ಸಂರಚನಾ curl_header *hout)
{
  ಸಂರಚನಾ curl_header *h = hout;
  h->name = hs->name;
  h->ಮೌಲ್ಯ = hs->ಮೌಲ್ಯ;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned ಪೂರ್ಣಾಙ್ಕ)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ಧ್ರುವ ವರ್ಣ *name,
                           size_t nameindex,
                           unsigned ಪೂರ್ಣಾಙ್ಕ origin,
                           ಪೂರ್ಣಾಙ್ಕ request,
                           ಸಂರಚನಾ curl_header **hout)
{
  ಸಂರಚನಾ Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  ಯದಿ(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e_pick = ರಿಕ್ತ;
    size_t match = 0;
    size_t amount = 0;
    ಸಂರಚನಾ Curl_header_store *hs = ರಿಕ್ತ;
    ಸಂರಚನಾ Curl_header_store *pick = ರಿಕ್ತ;
    ಯದಿ(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      ಗಚ್ಛ out;
    }
    ಯದಿ(!Curl_llist_ಎಣಿಕೆ(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      ಗಚ್ಛ out;
    }
    ಯದಿ(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      ಗಚ್ಛ out;
    }
    ಯದಿ(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    ಪ್ರತಿ(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_ಗಂಟು_next(e)) {
      hs = Curl_ಗಂಟು_elem(e);
      ಯದಿ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ಯದಿ(!amount)
      hresult = CURLHE_MISSING;
    ಅನ್ಯಥಾ ಯದಿ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ಯದಿ(hresult)
      ಗಚ್ಛ out;

    ಯದಿ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    ಅನ್ಯಥಾ {
      ಪ್ರತಿ(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_ಗಂಟು_next(e)) {
        hs = Curl_ಗಂಟು_elem(e);
        ಯದಿ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          ಭಙ್ಗ;
        }
      }
      ಯದಿ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        ಗಚ್ಛ out;
      }
    }
    /* this is the name we want */
    copy_header_external(hs, nameindex, amount, e_pick,
                         &data->state.headerout[0]);
    *hout = &data->state.headerout[0];
    hresult = CURLHE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ಯದಿ(result)
    hresult = Curl_eapi_hcode(result);
  ಪ್ರತ್ಯಾಗಮ hresult;
}

ಸಂರಚನಾ nextheader_cache_entry {
  ಸಂರಚನಾ Curl_header_store *header;
  size_t order;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nextheader_compare(ಧ್ರುವ ಶೂನ್ಯ *p1, ಧ್ರುವ ಶೂನ್ಯ *p2)
{
  ಧ್ರುವ ಸಂರಚನಾ nextheader_cache_entry *e1 = p1;
  ಧ್ರುವ ಸಂರಚನಾ nextheader_cache_entry *e2 = p2;
  ಧ್ರುವ ಸಂರಚನಾ Curl_header_store *h1 = e1->header;
  ಧ್ರುವ ಸಂರಚನಾ Curl_header_store *h2 = e2->header;
  ಧ್ರುವ ವರ್ಣ *n1 = h1->name;
  ಧ್ರುವ ವರ್ಣ *n2 = h2->name;

  ಯಾವತ್(*n1 && *n2) {
    unsigned ವರ್ಣ c1 = (unsigned ವರ್ಣ)Curl_raw_toupper(*n1++);
    unsigned ವರ್ಣ c2 = (unsigned ವರ್ಣ)Curl_raw_toupper(*n2++);
    ಯದಿ(c1 != c2)
      ಪ್ರತ್ಯಾಗಮ c1 > c2 ? 1 : -1;
  }
  ಯದಿ(*n1)
    ಪ್ರತ್ಯಾಗಮ 1;
  ಯದಿ(*n2)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(e1->order != e2->order)
    ಪ್ರತ್ಯಾಗಮ e1->order > e2->order ? 1 : -1;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ bool nextheader_cache_build(ಸಂರಚನಾ Curl_easy *data,
                                   unsigned ಪೂರ್ಣಾಙ್ಕ origin,
                                   ಪೂರ್ಣಾಙ್ಕ request, size_t ಎಣಿಕೆ)
{
  ಸಂರಚನಾ nextheader_cache_entry *headers;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(ಪರಿಮಾಣ(*headers) * ಎಣಿಕೆ);
  ಯದಿ(!headers)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಪ್ರತಿ(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_ಗಂಟು_next(e)) {
    ಸಂರಚನಾ Curl_header_store *hs = Curl_ಗಂಟು_elem(e);
    ಯದಿ((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, ಪರಿಮಾಣ(*headers), nextheader_compare);
  ಪ್ರತಿ(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    ಯಾವತ್((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    ಪ್ರತಿ(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_ಎಣಿಕೆ = ಎಣಿಕೆ;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

/* public API */
ಸಂರಚನಾ curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned ಪೂರ್ಣಾಙ್ಕ origin,
                                         ಪೂರ್ಣಾಙ್ಕ request,
                                         ಸಂರಚನಾ curl_header *prev)
{
  ಸಂರಚನಾ Curl_eapi_guard guard;
  ಸಂರಚನಾ curl_header *hd = ರಿಕ್ತ;
  CURLcode result;

  ಯದಿ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *pick;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t ಎಣಿಕೆ;

    ಯದಿ(request > data->state.requests)
      ಗಚ್ಛ out;
    ಯದಿ(request == -1)
      request = data->state.requests;

    ಯದಿ(prev) {
      pick = prev->anchor;
      ಯದಿ(!pick)
        /* something is wrong */
        ಗಚ್ಛ out;
      pick = Curl_ಗಂಟು_next(pick);
    }
    ಅನ್ಯಥಾ
      pick = Curl_llist_head(&data->state.httphdrs);

    ಯದಿ(pick) {
      /* make sure it is the next header of the desired type */
      ಕುರು {
        hs = Curl_ಗಂಟು_elem(pick);
        ಯದಿ((hs->type & origin) && (hs->request == request))
          ಭಙ್ಗ;
        pick = Curl_ಗಂಟು_next(pick);
      } ಯಾವತ್(pick);
    }

    ಯದಿ(!pick)
      /* no more headers available */
      ಗಚ್ಛ out;

    hs = Curl_ಗಂಟು_elem(pick);
    ಎಣಿಕೆ = Curl_llist_ಎಣಿಕೆ(&data->state.httphdrs);

    ಯದಿ(((data->state.nh_ಎಣಿಕೆ == ಎಣಿಕೆ) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, ಎಣಿಕೆ)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    ಅನ್ಯಥಾ {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      ಪ್ರತಿ(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_ಗಂಟು_next(e)) {
        ಸಂರಚನಾ Curl_header_store *check = Curl_ಗಂಟು_elem(e);
        ಯದಿ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ಯದಿ(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ hd;
}

ಸ್ಥಿರ CURLcode namevalue(ವರ್ಣ *header, size_t hlen, unsigned ಪೂರ್ಣಾಙ್ಕ type,
                          ವರ್ಣ **name, ವರ್ಣ **ಮೌಲ್ಯ)
{
  ವರ್ಣ *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  ಯದಿ(type == CURLH_PSEUDO) {
    ಯದಿ(*header != ':')
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  ಯಾವತ್(*header && (*header != ':'))
    ++header;

  ಯದಿ(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  ಯಾವತ್(ISBLANK(*header))
    header++;

  *ಮೌಲ್ಯ = header;

  /* skip all trailing space letters */
  ಯಾವತ್((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ತಳ್ಳು(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *header,
                           size_t hlen, /* length of header */
                           unsigned ವರ್ಣ type)
{
  ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
  ವರ್ಣ *name = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಧ್ರುವ size_t ilen = hlen;

  ಯದಿ((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* trim off newline characters */
  ಯದಿ(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  ಯದಿ(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  ಯದಿ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;

  ಯದಿ(ISBLANK(header[0])) {
    /* pass leading blanks */
    ಯಾವತ್(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    ಯದಿ(!hlen)
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }
  ಯದಿ(Curl_llist_ಎಣಿಕೆ(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_ಎಣಿಕೆ) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_ಎಣಿಕೆ);
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, ಪರಿಮಾಣ(*hs) + hlen);
  ಯದಿ(!hs)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &ಮೌಲ್ಯ);
  ಯದಿ(!result) {
    hs->name = name;
    hs->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->ಗಂಟು);
  }
  ಅನ್ಯಥಾ {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ಸ್ಥಿರ ಶೂನ್ಯ headers_reset(ಸಂರಚನಾ Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, ರಿಕ್ತ);
  data->state.nh_ಎಣಿಕೆ = 0;
}

ಸಂರಚನಾ hds_cw_collect_ctx {
  ಸಂರಚನಾ Curl_cwriter super;
};

ಸ್ಥಿರ CURLcode hds_cw_collect_write(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_cwriter *writer, ಪೂರ್ಣಾಙ್ಕ type,
                                     ಧ್ರುವ ವರ್ಣ *buf, size_t blen)
{
  ಯದಿ((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned ವರ್ಣ htype = (unsigned ವರ್ಣ)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_ತಳ್ಳು(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ Curl_cwriter_write(data, writer->next, type, buf, blen);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ರಿಕ್ತ,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಪರಿಮಾಣ(ಸಂರಚನಾ hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cwriter *writer;
  CURLcode result;

  ಯದಿ(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ಯದಿ(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    result = Curl_cwriter_add(data, writer);
    ಯದಿ(result) {
      Curl_cwriter_free(data, writer);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;

  ಪ್ರತಿ(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    ಸಂರಚನಾ Curl_header_store *hs = Curl_ಗಂಟು_elem(e);
    n = Curl_ಗಂಟು_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಅನ್ಯಥಾ /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ಧ್ರುವ ವರ್ಣ *name,
                           size_t index,
                           unsigned ಪೂರ್ಣಾಙ್ಕ origin,
                           ಪೂರ್ಣಾಙ್ಕ request,
                           ಸಂರಚನಾ curl_header **hout)
{
  (ಶೂನ್ಯ)easy;
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)index;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)hout;
  ಪ್ರತ್ಯಾಗಮ CURLHE_NOT_BUILT_IN;
}

ಸಂರಚನಾ curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned ಪೂರ್ಣಾಙ್ಕ type,
                                         ಪೂರ್ಣಾಙ್ಕ request,
                                         ಸಂರಚನಾ curl_header *prev)
{
  (ಶೂನ್ಯ)easy;
  (ಶೂನ್ಯ)type;
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)prev;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif
