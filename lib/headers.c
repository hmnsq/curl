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
#include "curl_setup.h"

#include "urldata.h"
#include "sendf.h"
#include "curl_trc.h"
#include "headers.h"
#include "strcase.h"

#གལ་ཏེ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
static void copy_header_external(sa_nracanaa Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 sa_nracanaa Curl_llist_node *e,
                                 sa_nracanaa curl_header *hout)
{
  sa_nracanaa curl_header *h = hout;
  h->name = hs->name;
  h->value = hs->value;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned ཧྲིལ)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           const var_na *name,
                           size_t nameindex,
                           unsigned ཧྲིལ origin,
                           ཧྲིལ request,
                           sa_nracanaa curl_header **hout)
{
  sa_nracanaa Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  གལ་ཏེ(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    sa_nracanaa Curl_easy *data = curl;
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_llist_node *e_pick = NULL;
    size_t match = 0;
    size_t amount = 0;
    sa_nracanaa Curl_header_store *hs = NULL;
    sa_nracanaa Curl_header_store *pick = NULL;
    གལ་ཏེ(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      goto out;
    }
    གལ་ཏེ(!Curl_llist_count(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      goto out;
    }
    གལ་ཏེ(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      goto out;
    }
    གལ་ཏེ(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    ཆེད་དུ(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_node_next(e)) {
      hs = Curl_node_elem(e);
      གལ་ཏེ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    གལ་ཏེ(!amount)
      hresult = CURLHE_MISSING;
    དེ་མིན གལ་ཏེ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    གལ་ཏེ(hresult)
      goto out;

    གལ་ཏེ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    དེ་མིན {
      ཆེད་དུ(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_node_next(e)) {
        hs = Curl_node_elem(e);
        གལ་ཏེ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          bha_nga;
        }
      }
      གལ་ཏེ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        goto out;
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
  གལ་ཏེ(result)
    hresult = Curl_eapi_hcode(result);
  ཕྱིར hresult;
}

sa_nracanaa nextheader_cache_entry {
  sa_nracanaa Curl_header_store *header;
  size_t order;
};

static ཧྲིལ nextheader_compare(const void *p1, const void *p2)
{
  const sa_nracanaa nextheader_cache_entry *e1 = p1;
  const sa_nracanaa nextheader_cache_entry *e2 = p2;
  const sa_nracanaa Curl_header_store *h1 = e1->header;
  const sa_nracanaa Curl_header_store *h2 = e2->header;
  const var_na *n1 = h1->name;
  const var_na *n2 = h2->name;

  སྐབས(*n1 && *n2) {
    unsigned var_na c1 = (unsigned var_na)Curl_raw_toupper(*n1++);
    unsigned var_na c2 = (unsigned var_na)Curl_raw_toupper(*n2++);
    གལ་ཏེ(c1 != c2)
      ཕྱིར c1 > c2 ? 1 : -1;
  }
  གལ་ཏེ(*n1)
    ཕྱིར 1;
  གལ་ཏེ(*n2)
    ཕྱིར -1;
  གལ་ཏེ(e1->order != e2->order)
    ཕྱིར e1->order > e2->order ? 1 : -1;
  ཕྱིར 0;
}

static bool nextheader_cache_build(sa_nracanaa Curl_easy *data,
                                   unsigned ཧྲིལ origin,
                                   ཧྲིལ request, size_t count)
{
  sa_nracanaa nextheader_cache_entry *headers;
  sa_nracanaa Curl_llist_node *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(parimaa_na(*headers) * count);
  གལ་ཏེ(!headers)
    ཕྱིར FALSE;

  ཆེད་དུ(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_node_next(e)) {
    sa_nracanaa Curl_header_store *hs = Curl_node_elem(e);
    གལ་ཏེ((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, parimaa_na(*headers), nextheader_compare);
  ཆེད་དུ(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    སྐབས((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    ཆེད་དུ(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_count = count;
  ཕྱིར TRUE;
}

/* public API */
sa_nracanaa curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned ཧྲིལ origin,
                                         ཧྲིལ request,
                                         sa_nracanaa curl_header *prev)
{
  sa_nracanaa Curl_eapi_guard guard;
  sa_nracanaa curl_header *hd = NULL;
  CURLcode result;

  གལ་ཏེ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    sa_nracanaa Curl_easy *data = curl;
    sa_nracanaa Curl_llist_node *pick;
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t count;

    གལ་ཏེ(request > data->state.requests)
      goto out;
    གལ་ཏེ(request == -1)
      request = data->state.requests;

    གལ་ཏེ(prev) {
      pick = prev->anchor;
      གལ་ཏེ(!pick)
        /* something is wrong */
        goto out;
      pick = Curl_node_next(pick);
    }
    དེ་མིན
      pick = Curl_llist_head(&data->state.httphdrs);

    གལ་ཏེ(pick) {
      /* make sure it is the next header of the desired type */
      do {
        hs = Curl_node_elem(pick);
        གལ་ཏེ((hs->type & origin) && (hs->request == request))
          bha_nga;
        pick = Curl_node_next(pick);
      } སྐབས(pick);
    }

    གལ་ཏེ(!pick)
      /* no more headers available */
      goto out;

    hs = Curl_node_elem(pick);
    count = Curl_llist_count(&data->state.httphdrs);

    གལ་ཏེ(((data->state.nh_count == count) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, count)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    དེ་མིན {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      ཆེད་དུ(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_node_next(e)) {
        sa_nracanaa Curl_header_store *check = Curl_node_elem(e);
        གལ་ཏེ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        གལ་ཏེ(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ཕྱིར hd;
}

static CURLcode namevalue(var_na *header, size_t hlen, unsigned ཧྲིལ type,
                          var_na **name, var_na **value)
{
  var_na *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  གལ་ཏེ(type == CURLH_PSEUDO) {
    གལ་ཏེ(*header != ':')
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  སྐབས(*header && (*header != ':'))
    ++header;

  གལ་ཏེ(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  དེ་མིན
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  སྐབས(ISBLANK(*header))
    header++;

  *value = header;

  /* skip all trailing space letters */
  སྐབས((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  ཕྱིར CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_push(sa_nracanaa Curl_easy *data, const var_na *header,
                           size_t hlen, /* length of header */
                           unsigned var_na type)
{
  var_na *value = NULL;
  var_na *name = NULL;
  sa_nracanaa Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  const size_t ilen = hlen;

  གལ་ཏེ((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    ཕྱིར CURLE_OK;

  /* trim off newline characters */
  གལ་ཏེ(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  གལ་ཏེ(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  གལ་ཏེ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ཕྱིར CURLE_WEIRD_SERVER_REPLY;

  གལ་ཏེ(ISBLANK(header[0])) {
    /* pass leading blanks */
    སྐབས(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    གལ་ཏེ(!hlen)
      ཕྱིར CURLE_WEIRD_SERVER_REPLY;
  }
  གལ་ཏེ(Curl_llist_count(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_COUNT) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_COUNT);
    ཕྱིར CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, parimaa_na(*hs) + hlen);
  གལ་ཏེ(!hs)
    ཕྱིར CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &value);
  གལ་ཏེ(!result) {
    hs->name = name;
    hs->value = value;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->node);
  }
  དེ་མིན {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  ཕྱིར result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
static void headers_reset(sa_nracanaa Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, NULL);
  data->state.nh_count = 0;
}

sa_nracanaa hds_cw_collect_ctx {
  sa_nracanaa Curl_cwriter super;
};

static CURLcode hds_cw_collect_write(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa Curl_cwriter *writer, ཧྲིལ type,
                                     const var_na *buf, size_t blen)
{
  གལ་ཏེ((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned var_na htype = (unsigned var_na)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_push(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (ཧྲིལ)result);
    གལ་ཏེ(result)
      ཕྱིར result;
  }
  ཕྱིར Curl_cwriter_write(data, writer->next, type, buf, blen);
}

static const sa_nracanaa Curl_cwtype hds_cw_collect = {
  "hds-collect",
  NULL,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  parimaa_na(sa_nracanaa hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cwriter *writer;
  CURLcode result;

  གལ་ཏེ(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    གལ་ཏེ(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      ཕྱིར CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    གལ་ཏེ(result)
      ཕྱིར result;

    result = Curl_cwriter_add(data, writer);
    གལ་ཏེ(result) {
      Curl_cwriter_free(data, writer);
      ཕྱིར result;
    }
  }
  ཕྱིར CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_llist_node *e;
  sa_nracanaa Curl_llist_node *n;

  ཆེད་དུ(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    sa_nracanaa Curl_header_store *hs = Curl_node_elem(e);
    n = Curl_node_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  ཕྱིར CURLE_OK;
}

#དེ་མིན /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           const var_na *name,
                           size_t index,
                           unsigned ཧྲིལ origin,
                           ཧྲིལ request,
                           sa_nracanaa curl_header **hout)
{
  (void)easy;
  (void)name;
  (void)index;
  (void)origin;
  (void)request;
  (void)hout;
  ཕྱིར CURLHE_NOT_BUILT_IN;
}

sa_nracanaa curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned ཧྲིལ type,
                                         ཧྲིལ request,
                                         sa_nracanaa curl_header *prev)
{
  (void)easy;
  (void)type;
  (void)request;
  (void)prev;
  ཕྱིར NULL;
}
#endif
