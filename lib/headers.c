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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "headers.h"
#অন্তর্ভাব "strcase.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
স্থির শূন্য copy_header_external(সংরচনা Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 সংরচনা Curl_llist_গ্রন্থি *e,
                                 সংরচনা curl_header *hout)
{
  সংরচনা curl_header *h = hout;
  h->name = hs->name;
  h->মান = hs->মান;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned পূর্ণাঙ্ক)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ধ্রুব বর্ণ *name,
                           size_t nameindex,
                           unsigned পূর্ণাঙ্ক origin,
                           পূর্ণাঙ্ক request,
                           সংরচনা curl_header **hout)
{
  সংরচনা Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    সংরচনা Curl_easy *data = curl;
    সংরচনা Curl_llist_গ্রন্থি *e;
    সংরচনা Curl_llist_গ্রন্থি *e_pick = রিক্ত;
    size_t match = 0;
    size_t amount = 0;
    সংরচনা Curl_header_store *hs = রিক্ত;
    সংরচনা Curl_header_store *pick = রিক্ত;
    যদি(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      গচ্ছ out;
    }
    যদি(!Curl_llist_গুনতি(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      গচ্ছ out;
    }
    যদি(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      গচ্ছ out;
    }
    যদি(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    প্রতি(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_গ্রন্থি_next(e)) {
      hs = Curl_গ্রন্থি_elem(e);
      যদি(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    যদি(!amount)
      hresult = CURLHE_MISSING;
    অন্যথা যদি(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    যদি(hresult)
      গচ্ছ out;

    যদি(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    অন্যথা {
      প্রতি(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_গ্রন্থি_next(e)) {
        hs = Curl_গ্রন্থি_elem(e);
        যদি(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          ভঙ্গ;
        }
      }
      যদি(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        গচ্ছ out;
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
  যদি(result)
    hresult = Curl_eapi_hcode(result);
  প্রত্যাগম hresult;
}

সংরচনা nextheader_cache_entry {
  সংরচনা Curl_header_store *header;
  size_t order;
};

স্থির পূর্ণাঙ্ক nextheader_compare(ধ্রুব শূন্য *p1, ধ্রুব শূন্য *p2)
{
  ধ্রুব সংরচনা nextheader_cache_entry *e1 = p1;
  ধ্রুব সংরচনা nextheader_cache_entry *e2 = p2;
  ধ্রুব সংরচনা Curl_header_store *h1 = e1->header;
  ধ্রুব সংরচনা Curl_header_store *h2 = e2->header;
  ধ্রুব বর্ণ *n1 = h1->name;
  ধ্রুব বর্ণ *n2 = h2->name;

  যাবত্(*n1 && *n2) {
    unsigned বর্ণ c1 = (unsigned বর্ণ)Curl_raw_toupper(*n1++);
    unsigned বর্ণ c2 = (unsigned বর্ণ)Curl_raw_toupper(*n2++);
    যদি(c1 != c2)
      প্রত্যাগম c1 > c2 ? 1 : -1;
  }
  যদি(*n1)
    প্রত্যাগম 1;
  যদি(*n2)
    প্রত্যাগম -1;
  যদি(e1->order != e2->order)
    প্রত্যাগম e1->order > e2->order ? 1 : -1;
  প্রত্যাগম 0;
}

স্থির bool nextheader_cache_build(সংরচনা Curl_easy *data,
                                   unsigned পূর্ণাঙ্ক origin,
                                   পূর্ণাঙ্ক request, size_t গুনতি)
{
  সংরচনা nextheader_cache_entry *headers;
  সংরচনা Curl_llist_গ্রন্থি *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(পরিমাণ(*headers) * গুনতি);
  যদি(!headers)
    প্রত্যাগম FALSE;

  প্রতি(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_গ্রন্থি_next(e)) {
    সংরচনা Curl_header_store *hs = Curl_গ্রন্থি_elem(e);
    যদি((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, পরিমাণ(*headers), nextheader_compare);
  প্রতি(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    যাবত্((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    প্রতি(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_গুনতি = গুনতি;
  প্রত্যাগম TRUE;
}

/* public API */
সংরচনা curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned পূর্ণাঙ্ক origin,
                                         পূর্ণাঙ্ক request,
                                         সংরচনা curl_header *prev)
{
  সংরচনা Curl_eapi_guard guard;
  সংরচনা curl_header *hd = রিক্ত;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    সংরচনা Curl_easy *data = curl;
    সংরচনা Curl_llist_গ্রন্থি *pick;
    সংরচনা Curl_llist_গ্রন্থি *e;
    সংরচনা Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t গুনতি;

    যদি(request > data->state.requests)
      গচ্ছ out;
    যদি(request == -1)
      request = data->state.requests;

    যদি(prev) {
      pick = prev->anchor;
      যদি(!pick)
        /* something is wrong */
        গচ্ছ out;
      pick = Curl_গ্রন্থি_next(pick);
    }
    অন্যথা
      pick = Curl_llist_head(&data->state.httphdrs);

    যদি(pick) {
      /* make sure it is the next header of the desired type */
      কুরু {
        hs = Curl_গ্রন্থি_elem(pick);
        যদি((hs->type & origin) && (hs->request == request))
          ভঙ্গ;
        pick = Curl_গ্রন্থি_next(pick);
      } যাবত্(pick);
    }

    যদি(!pick)
      /* no more headers available */
      গচ্ছ out;

    hs = Curl_গ্রন্থি_elem(pick);
    গুনতি = Curl_llist_গুনতি(&data->state.httphdrs);

    যদি(((data->state.nh_গুনতি == গুনতি) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, গুনতি)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    অন্যথা {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      প্রতি(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_গ্রন্থি_next(e)) {
        সংরচনা Curl_header_store *check = Curl_গ্রন্থি_elem(e);
        যদি(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        যদি(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  প্রত্যাগম hd;
}

স্থির CURLcode namevalue(বর্ণ *header, size_t hlen, unsigned পূর্ণাঙ্ক type,
                          বর্ণ **name, বর্ণ **মান)
{
  বর্ণ *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  যদি(type == CURLH_PSEUDO) {
    যদি(*header != ':')
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  যাবত্(*header && (*header != ':'))
    ++header;

  যদি(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  অন্যথা
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  যাবত্(ISBLANK(*header))
    header++;

  *মান = header;

  /* skip all trailing space letters */
  যাবত্((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  প্রত্যাগম CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ঠেলো(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *header,
                           size_t hlen, /* length of header */
                           unsigned বর্ণ type)
{
  বর্ণ *মান = রিক্ত;
  বর্ণ *name = রিক্ত;
  সংরচনা Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ধ্রুব size_t ilen = hlen;

  যদি((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    প্রত্যাগম CURLE_OK;

  /* trim off newline characters */
  যদি(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  যদি(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  যদি(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;

  যদি(ISBLANK(header[0])) {
    /* pass leading blanks */
    যাবত্(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    যদি(!hlen)
      প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
  }
  যদি(Curl_llist_গুনতি(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_গুনতি) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_গুনতি);
    প্রত্যাগম CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, পরিমাণ(*hs) + hlen);
  যদি(!hs)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &মান);
  যদি(!result) {
    hs->name = name;
    hs->মান = মান;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->গ্রন্থি);
  }
  অন্যথা {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  প্রত্যাগম result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
স্থির শূন্য headers_reset(সংরচনা Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, রিক্ত);
  data->state.nh_গুনতি = 0;
}

সংরচনা hds_cw_collect_ctx {
  সংরচনা Curl_cwriter super;
};

স্থির CURLcode hds_cw_collect_write(সংরচনা Curl_easy *data,
                                     সংরচনা Curl_cwriter *writer, পূর্ণাঙ্ক type,
                                     ধ্রুব বর্ণ *buf, size_t blen)
{
  যদি((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned বর্ণ htype = (unsigned বর্ণ)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_ঠেলো(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (পূর্ণাঙ্ক)result);
    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম Curl_cwriter_write(data, writer->next, type, buf, blen);
}

স্থির ধ্রুব সংরচনা Curl_cwtype hds_cw_collect = {
  "hds-collect",
  রিক্ত,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  পরিমাণ(সংরচনা hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cwriter *writer;
  CURLcode result;

  যদি(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    যদি(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      প্রত্যাগম CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    যদি(result)
      প্রত্যাগম result;

    result = Curl_cwriter_add(data, writer);
    যদি(result) {
      Curl_cwriter_free(data, writer);
      প্রত্যাগম result;
    }
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_llist_গ্রন্থি *e;
  সংরচনা Curl_llist_গ্রন্থি *n;

  প্রতি(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    সংরচনা Curl_header_store *hs = Curl_গ্রন্থি_elem(e);
    n = Curl_গ্রন্থি_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  প্রত্যাগম CURLE_OK;
}

#অন্যথা /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ধ্রুব বর্ণ *name,
                           size_t index,
                           unsigned পূর্ণাঙ্ক origin,
                           পূর্ণাঙ্ক request,
                           সংরচনা curl_header **hout)
{
  (শূন্য)easy;
  (শূন্য)name;
  (শূন্য)index;
  (শূন্য)origin;
  (শূন্য)request;
  (শূন্য)hout;
  প্রত্যাগম CURLHE_NOT_BUILT_IN;
}

সংরচনা curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned পূর্ণাঙ্ক type,
                                         পূর্ণাঙ্ক request,
                                         সংরচনা curl_header *prev)
{
  (শূন্য)easy;
  (শূন্য)type;
  (শূন্য)request;
  (শূন্য)prev;
  প্রত্যাগম রিক্ত;
}
#endif
