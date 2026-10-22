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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "headers.h"
#ضمن "strcase.h"

#ܐܢ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ساكن فراغ copy_header_external(بنية Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 بنية Curl_llist_node *e,
                                 بنية curl_header *hout)
{
  بنية curl_header *h = hout;
  h->name = hs->name;
  h->value = hs->value;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned صحيح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ثابت حرف *name,
                           size_t nameindex,
                           unsigned صحيح origin,
                           صحيح request,
                           بنية curl_header **hout)
{
  بنية Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  ܐܢ(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    بنية Curl_easy *data = curl;
    بنية Curl_llist_node *e;
    بنية Curl_llist_node *e_pick = عدم;
    size_t match = 0;
    size_t amount = 0;
    بنية Curl_header_store *hs = عدم;
    بنية Curl_header_store *pick = عدم;
    ܐܢ(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      اذهب out;
    }
    ܐܢ(!Curl_llist_count(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      اذهب out;
    }
    ܐܢ(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      اذهب out;
    }
    ܐܢ(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    لكل(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_node_next(e)) {
      hs = Curl_node_elem(e);
      ܐܢ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ܐܢ(!amount)
      hresult = CURLHE_MISSING;
    ܐܚܪܢܐ ܐܢ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ܐܢ(hresult)
      اذهب out;

    ܐܢ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    ܐܚܪܢܐ {
      لكل(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_node_next(e)) {
        hs = Curl_node_elem(e);
        ܐܢ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          اكسر;
        }
      }
      ܐܢ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        اذهب out;
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
  ܐܢ(result)
    hresult = Curl_eapi_hcode(result);
  ܦܢܐ hresult;
}

بنية nextheader_cache_entry {
  بنية Curl_header_store *header;
  size_t order;
};

ساكن صحيح nextheader_compare(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية nextheader_cache_entry *e1 = p1;
  ثابت بنية nextheader_cache_entry *e2 = p2;
  ثابت بنية Curl_header_store *h1 = e1->header;
  ثابت بنية Curl_header_store *h2 = e2->header;
  ثابت حرف *n1 = h1->name;
  ثابت حرف *n2 = h2->name;

  ܟܡܐ(*n1 && *n2) {
    unsigned حرف c1 = (unsigned حرف)Curl_raw_toupper(*n1++);
    unsigned حرف c2 = (unsigned حرف)Curl_raw_toupper(*n2++);
    ܐܢ(c1 != c2)
      ܦܢܐ c1 > c2 ? 1 : -1;
  }
  ܐܢ(*n1)
    ܦܢܐ 1;
  ܐܢ(*n2)
    ܦܢܐ -1;
  ܐܢ(e1->order != e2->order)
    ܦܢܐ e1->order > e2->order ? 1 : -1;
  ܦܢܐ 0;
}

ساكن bool nextheader_cache_build(بنية Curl_easy *data,
                                   unsigned صحيح origin,
                                   صحيح request, size_t count)
{
  بنية nextheader_cache_entry *headers;
  بنية Curl_llist_node *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(حجم(*headers) * count);
  ܐܢ(!headers)
    ܦܢܐ FALSE;

  لكل(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_node_next(e)) {
    بنية Curl_header_store *hs = Curl_node_elem(e);
    ܐܢ((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, حجم(*headers), nextheader_compare);
  لكل(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    ܟܡܐ((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    لكل(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_count = count;
  ܦܢܐ TRUE;
}

/* public API */
بنية curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned صحيح origin,
                                         صحيح request,
                                         بنية curl_header *prev)
{
  بنية Curl_eapi_guard guard;
  بنية curl_header *hd = عدم;
  CURLcode result;

  ܐܢ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    بنية Curl_easy *data = curl;
    بنية Curl_llist_node *pick;
    بنية Curl_llist_node *e;
    بنية Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t count;

    ܐܢ(request > data->state.requests)
      اذهب out;
    ܐܢ(request == -1)
      request = data->state.requests;

    ܐܢ(prev) {
      pick = prev->anchor;
      ܐܢ(!pick)
        /* something is wrong */
        اذهب out;
      pick = Curl_node_next(pick);
    }
    ܐܚܪܢܐ
      pick = Curl_llist_head(&data->state.httphdrs);

    ܐܢ(pick) {
      /* make sure it is the next header of the desired type */
      ܥܒܕ {
        hs = Curl_node_elem(pick);
        ܐܢ((hs->type & origin) && (hs->request == request))
          اكسر;
        pick = Curl_node_next(pick);
      } ܟܡܐ(pick);
    }

    ܐܢ(!pick)
      /* no more headers available */
      اذهب out;

    hs = Curl_node_elem(pick);
    count = Curl_llist_count(&data->state.httphdrs);

    ܐܢ(((data->state.nh_count == count) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, count)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    ܐܚܪܢܐ {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      لكل(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_node_next(e)) {
        بنية Curl_header_store *check = Curl_node_elem(e);
        ܐܢ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ܐܢ(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ܦܢܐ hd;
}

ساكن CURLcode namevalue(حرف *header, size_t hlen, unsigned صحيح type,
                          حرف **name, حرف **value)
{
  حرف *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  ܐܢ(type == CURLH_PSEUDO) {
    ܐܢ(*header != ':')
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  ܟܡܐ(*header && (*header != ':'))
    ++header;

  ܐܢ(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  ܐܚܪܢܐ
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  ܟܡܐ(ISBLANK(*header))
    header++;

  *value = header;

  /* skip all trailing space letters */
  ܟܡܐ((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  ܦܢܐ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_push(بنية Curl_easy *data, ثابت حرف *header,
                           size_t hlen, /* length of header */
                           unsigned حرف type)
{
  حرف *value = عدم;
  حرف *name = عدم;
  بنية Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ثابت size_t ilen = hlen;

  ܐܢ((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    ܦܢܐ CURLE_OK;

  /* trim off newline characters */
  ܐܢ(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  ܐܢ(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  ܐܢ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ܦܢܐ CURLE_WEIRD_SERVER_REPLY;

  ܐܢ(ISBLANK(header[0])) {
    /* pass leading blanks */
    ܟܡܐ(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    ܐܢ(!hlen)
      ܦܢܐ CURLE_WEIRD_SERVER_REPLY;
  }
  ܐܢ(Curl_llist_count(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_COUNT) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_COUNT);
    ܦܢܐ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, حجم(*hs) + hlen);
  ܐܢ(!hs)
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &value);
  ܐܢ(!result) {
    hs->name = name;
    hs->value = value;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->node);
  }
  ܐܚܪܢܐ {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  ܦܢܐ result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ساكن فراغ headers_reset(بنية Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, عدم);
  data->state.nh_count = 0;
}

بنية hds_cw_collect_ctx {
  بنية Curl_cwriter super;
};

ساكن CURLcode hds_cw_collect_write(بنية Curl_easy *data,
                                     بنية Curl_cwriter *writer, صحيح type,
                                     ثابت حرف *buf, size_t blen)
{
  ܐܢ((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned حرف htype = (unsigned حرف)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_push(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحيح)result);
    ܐܢ(result)
      ܦܢܐ result;
  }
  ܦܢܐ Curl_cwriter_write(data, writer->next, type, buf, blen);
}

ساكن ثابت بنية Curl_cwtype hds_cw_collect = {
  "hds-collect",
  عدم,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(بنية Curl_easy *data)
{
  بنية Curl_cwriter *writer;
  CURLcode result;

  ܐܢ(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ܐܢ(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      ܦܢܐ CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ܐܢ(result)
      ܦܢܐ result;

    result = Curl_cwriter_add(data, writer);
    ܐܢ(result) {
      Curl_cwriter_free(data, writer);
      ܦܢܐ result;
    }
  }
  ܦܢܐ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(بنية Curl_easy *data)
{
  بنية Curl_llist_node *e;
  بنية Curl_llist_node *n;

  لكل(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    بنية Curl_header_store *hs = Curl_node_elem(e);
    n = Curl_node_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  ܦܢܐ CURLE_OK;
}

#ܐܚܪܢܐ /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ثابت حرف *name,
                           size_t index,
                           unsigned صحيح origin,
                           صحيح request,
                           بنية curl_header **hout)
{
  (فراغ)easy;
  (فراغ)name;
  (فراغ)index;
  (فراغ)origin;
  (فراغ)request;
  (فراغ)hout;
  ܦܢܐ CURLHE_NOT_BUILT_IN;
}

بنية curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned صحيح type,
                                         صحيح request,
                                         بنية curl_header *prev)
{
  (فراغ)easy;
  (فراغ)type;
  (فراغ)request;
  (فراغ)prev;
  ܦܢܐ عدم;
}
#endif
