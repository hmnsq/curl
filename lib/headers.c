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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "headers.h"
#அந்தர்பாவ "strcase.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
நிலையான வெறுமை copy_header_external(கட்டமைப்பு Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 கட்டமைப்பு Curl_llist_முனை *e,
                                 கட்டமைப்பு curl_header *hout)
{
  கட்டமைப்பு curl_header *h = hout;
  h->name = hs->name;
  h->மதிப்பு = hs->மதிப்பு;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned முழு)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           மாறிலி எழுத்து *name,
                           size_t nameindex,
                           unsigned முழு origin,
                           முழு request,
                           கட்டமைப்பு curl_header **hout)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_llist_முனை *e_pick = வெற்று;
    size_t match = 0;
    size_t amount = 0;
    கட்டமைப்பு Curl_header_store *hs = வெற்று;
    கட்டமைப்பு Curl_header_store *pick = வெற்று;
    எனில்(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      செல் out;
    }
    எனில்(!Curl_llist_மொத்தம்(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      செல் out;
    }
    எனில்(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      செல் out;
    }
    எனில்(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    க்கு(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_முனை_next(e)) {
      hs = Curl_முனை_elem(e);
      எனில்(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    எனில்(!amount)
      hresult = CURLHE_MISSING;
    இல்லையெனில் எனில்(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    எனில்(hresult)
      செல் out;

    எனில்(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    இல்லையெனில் {
      க்கு(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_முனை_next(e)) {
        hs = Curl_முனை_elem(e);
        எனில்(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          நிறுத்து;
        }
      }
      எனில்(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        செல் out;
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
  எனில்(result)
    hresult = Curl_eapi_hcode(result);
  திரும்பு hresult;
}

கட்டமைப்பு nextheader_cache_entry {
  கட்டமைப்பு Curl_header_store *header;
  size_t order;
};

நிலையான முழு nextheader_compare(மாறிலி வெறுமை *p1, மாறிலி வெறுமை *p2)
{
  மாறிலி கட்டமைப்பு nextheader_cache_entry *e1 = p1;
  மாறிலி கட்டமைப்பு nextheader_cache_entry *e2 = p2;
  மாறிலி கட்டமைப்பு Curl_header_store *h1 = e1->header;
  மாறிலி கட்டமைப்பு Curl_header_store *h2 = e2->header;
  மாறிலி எழுத்து *n1 = h1->name;
  மாறிலி எழுத்து *n2 = h2->name;

  போது(*n1 && *n2) {
    unsigned எழுத்து c1 = (unsigned எழுத்து)Curl_raw_toupper(*n1++);
    unsigned எழுத்து c2 = (unsigned எழுத்து)Curl_raw_toupper(*n2++);
    எனில்(c1 != c2)
      திரும்பு c1 > c2 ? 1 : -1;
  }
  எனில்(*n1)
    திரும்பு 1;
  எனில்(*n2)
    திரும்பு -1;
  எனில்(e1->order != e2->order)
    திரும்பு e1->order > e2->order ? 1 : -1;
  திரும்பு 0;
}

நிலையான bool nextheader_cache_build(கட்டமைப்பு Curl_easy *data,
                                   unsigned முழு origin,
                                   முழு request, size_t மொத்தம்)
{
  கட்டமைப்பு nextheader_cache_entry *headers;
  கட்டமைப்பு Curl_llist_முனை *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(அளவு(*headers) * மொத்தம்);
  எனில்(!headers)
    திரும்பு FALSE;

  க்கு(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_முனை_next(e)) {
    கட்டமைப்பு Curl_header_store *hs = Curl_முனை_elem(e);
    எனில்((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, அளவு(*headers), nextheader_compare);
  க்கு(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    போது((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    க்கு(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_மொத்தம் = மொத்தம்;
  திரும்பு TRUE;
}

/* public API */
கட்டமைப்பு curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned முழு origin,
                                         முழு request,
                                         கட்டமைப்பு curl_header *prev)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  கட்டமைப்பு curl_header *hd = வெற்று;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    கட்டமைப்பு Curl_llist_முனை *pick;
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t மொத்தம்;

    எனில்(request > data->state.requests)
      செல் out;
    எனில்(request == -1)
      request = data->state.requests;

    எனில்(prev) {
      pick = prev->anchor;
      எனில்(!pick)
        /* something is wrong */
        செல் out;
      pick = Curl_முனை_next(pick);
    }
    இல்லையெனில்
      pick = Curl_llist_head(&data->state.httphdrs);

    எனில்(pick) {
      /* make sure it is the next header of the desired type */
      செய் {
        hs = Curl_முனை_elem(pick);
        எனில்((hs->type & origin) && (hs->request == request))
          நிறுத்து;
        pick = Curl_முனை_next(pick);
      } போது(pick);
    }

    எனில்(!pick)
      /* no more headers available */
      செல் out;

    hs = Curl_முனை_elem(pick);
    மொத்தம் = Curl_llist_மொத்தம்(&data->state.httphdrs);

    எனில்(((data->state.nh_மொத்தம் == மொத்தம்) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, மொத்தம்)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    இல்லையெனில் {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      க்கு(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_முனை_next(e)) {
        கட்டமைப்பு Curl_header_store *check = Curl_முனை_elem(e);
        எனில்(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        எனில்(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு hd;
}

நிலையான CURLcode namevalue(எழுத்து *header, size_t hlen, unsigned முழு type,
                          எழுத்து **name, எழுத்து **மதிப்பு)
{
  எழுத்து *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  எனில்(type == CURLH_PSEUDO) {
    எனில்(*header != ':')
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  போது(*header && (*header != ':'))
    ++header;

  எனில்(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  இல்லையெனில்
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  போது(ISBLANK(*header))
    header++;

  *மதிப்பு = header;

  /* skip all trailing space letters */
  போது((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  திரும்பு CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_தள்ளு(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *header,
                           size_t hlen, /* length of header */
                           unsigned எழுத்து type)
{
  எழுத்து *மதிப்பு = வெற்று;
  எழுத்து *name = வெற்று;
  கட்டமைப்பு Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  மாறிலி size_t ilen = hlen;

  எனில்((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    திரும்பு CURLE_OK;

  /* trim off newline characters */
  எனில்(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  எனில்(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  எனில்(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    திரும்பு CURLE_WEIRD_SERVER_REPLY;

  எனில்(ISBLANK(header[0])) {
    /* pass leading blanks */
    போது(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    எனில்(!hlen)
      திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }
  எனில்(Curl_llist_மொத்தம்(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_மொத்தம்) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_மொத்தம்);
    திரும்பு CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, அளவு(*hs) + hlen);
  எனில்(!hs)
    திரும்பு CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &மதிப்பு);
  எனில்(!result) {
    hs->name = name;
    hs->மதிப்பு = மதிப்பு;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->முனை);
  }
  இல்லையெனில் {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  திரும்பு result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
நிலையான வெறுமை headers_reset(கட்டமைப்பு Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, வெற்று);
  data->state.nh_மொத்தம் = 0;
}

கட்டமைப்பு hds_cw_collect_ctx {
  கட்டமைப்பு Curl_cwriter super;
};

நிலையான CURLcode hds_cw_collect_write(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_cwriter *writer, முழு type,
                                     மாறிலி எழுத்து *buf, size_t blen)
{
  எனில்((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned எழுத்து htype = (unsigned எழுத்து)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_தள்ளு(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (முழு)result);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு Curl_cwriter_write(data, writer->next, type, buf, blen);
}

நிலையான மாறிலி கட்டமைப்பு Curl_cwtype hds_cw_collect = {
  "hds-collect",
  வெற்று,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  அளவு(கட்டமைப்பு hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cwriter *writer;
  CURLcode result;

  எனில்(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    எனில்(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      திரும்பு CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    எனில்(result)
      திரும்பு result;

    result = Curl_cwriter_add(data, writer);
    எனில்(result) {
      Curl_cwriter_free(data, writer);
      திரும்பு result;
    }
  }
  திரும்பு CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_llist_முனை *e;
  கட்டமைப்பு Curl_llist_முனை *n;

  க்கு(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    கட்டமைப்பு Curl_header_store *hs = Curl_முனை_elem(e);
    n = Curl_முனை_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  திரும்பு CURLE_OK;
}

#இல்லையெனில் /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           மாறிலி எழுத்து *name,
                           size_t index,
                           unsigned முழு origin,
                           முழு request,
                           கட்டமைப்பு curl_header **hout)
{
  (வெறுமை)easy;
  (வெறுமை)name;
  (வெறுமை)index;
  (வெறுமை)origin;
  (வெறுமை)request;
  (வெறுமை)hout;
  திரும்பு CURLHE_NOT_BUILT_IN;
}

கட்டமைப்பு curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned முழு type,
                                         முழு request,
                                         கட்டமைப்பு curl_header *prev)
{
  (வெறுமை)easy;
  (வெறுமை)type;
  (வெறுமை)request;
  (வெறுமை)prev;
  திரும்பு வெற்று;
}
#endif
