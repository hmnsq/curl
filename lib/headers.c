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

#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
static void copy_header_external(struct Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 struct Curl_llist_node *e,
                                 struct curl_header *hout)
{
  struct curl_header *h = hout;
  h->name = hs->name;
  h->value = hs->value;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned цяло)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           const char *name,
                           size_t nameindex,
                           unsigned цяло origin,
                           цяло request,
                           struct curl_header **hout)
{
  struct Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  ако(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    struct Curl_easy *data = curl;
    struct Curl_llist_node *e;
    struct Curl_llist_node *e_pick = NULL;
    size_t match = 0;
    size_t amount = 0;
    struct Curl_header_store *hs = NULL;
    struct Curl_header_store *pick = NULL;
    ако(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      goto out;
    }
    ако(!Curl_llist_count(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      goto out;
    }
    ако(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      goto out;
    }
    ако(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    за(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_node_next(e)) {
      hs = Curl_node_elem(e);
      ако(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ако(!amount)
      hresult = CURLHE_MISSING;
    иначе ако(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ако(hresult)
      goto out;

    ако(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    иначе {
      за(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_node_next(e)) {
        hs = Curl_node_elem(e);
        ако(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          break;
        }
      }
      ако(!e) { /* this should not happen */
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
  ако(result)
    hresult = Curl_eapi_hcode(result);
  върни hresult;
}

struct nextheader_cache_entry {
  struct Curl_header_store *header;
  size_t order;
};

static цяло nextheader_compare(const void *p1, const void *p2)
{
  const struct nextheader_cache_entry *e1 = p1;
  const struct nextheader_cache_entry *e2 = p2;
  const struct Curl_header_store *h1 = e1->header;
  const struct Curl_header_store *h2 = e2->header;
  const char *n1 = h1->name;
  const char *n2 = h2->name;

  докато(*n1 && *n2) {
    unsigned char c1 = (unsigned char)Curl_raw_toupper(*n1++);
    unsigned char c2 = (unsigned char)Curl_raw_toupper(*n2++);
    ако(c1 != c2)
      върни c1 > c2 ? 1 : -1;
  }
  ако(*n1)
    върни 1;
  ако(*n2)
    върни -1;
  ако(e1->order != e2->order)
    върни e1->order > e2->order ? 1 : -1;
  върни 0;
}

static bool nextheader_cache_build(struct Curl_easy *data,
                                   unsigned цяло origin,
                                   цяло request, size_t count)
{
  struct nextheader_cache_entry *headers;
  struct Curl_llist_node *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(sizeof(*headers) * count);
  ако(!headers)
    върни FALSE;

  за(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_node_next(e)) {
    struct Curl_header_store *hs = Curl_node_elem(e);
    ако((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, sizeof(*headers), nextheader_compare);
  за(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    докато((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    за(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_count = count;
  върни TRUE;
}

/* public API */
struct curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned цяло origin,
                                         цяло request,
                                         struct curl_header *prev)
{
  struct Curl_eapi_guard guard;
  struct curl_header *hd = NULL;
  CURLcode result;

  ако(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    struct Curl_easy *data = curl;
    struct Curl_llist_node *pick;
    struct Curl_llist_node *e;
    struct Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t count;

    ако(request > data->state.requests)
      goto out;
    ако(request == -1)
      request = data->state.requests;

    ако(prev) {
      pick = prev->anchor;
      ако(!pick)
        /* something is wrong */
        goto out;
      pick = Curl_node_next(pick);
    }
    иначе
      pick = Curl_llist_head(&data->state.httphdrs);

    ако(pick) {
      /* make sure it is the next header of the desired type */
      do {
        hs = Curl_node_elem(pick);
        ако((hs->type & origin) && (hs->request == request))
          break;
        pick = Curl_node_next(pick);
      } докато(pick);
    }

    ако(!pick)
      /* no more headers available */
      goto out;

    hs = Curl_node_elem(pick);
    count = Curl_llist_count(&data->state.httphdrs);

    ако(((data->state.nh_count == count) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, count)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    иначе {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      за(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_node_next(e)) {
        struct Curl_header_store *check = Curl_node_elem(e);
        ако(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ако(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  върни hd;
}

static CURLcode namevalue(char *header, size_t hlen, unsigned цяло type,
                          char **name, char **value)
{
  char *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  ако(type == CURLH_PSEUDO) {
    ако(*header != ':')
      върни CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  докато(*header && (*header != ':'))
    ++header;

  ако(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  иначе
    върни CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  докато(ISBLANK(*header))
    header++;

  *value = header;

  /* skip all trailing space letters */
  докато((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  върни CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_push(struct Curl_easy *data, const char *header,
                           size_t hlen, /* length of header */
                           unsigned char type)
{
  char *value = NULL;
  char *name = NULL;
  struct Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  const size_t ilen = hlen;

  ако((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    върни CURLE_OK;

  /* trim off newline characters */
  ако(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  ако(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  ако(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    върни CURLE_WEIRD_SERVER_REPLY;

  ако(ISBLANK(header[0])) {
    /* pass leading blanks */
    докато(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    ако(!hlen)
      върни CURLE_WEIRD_SERVER_REPLY;
  }
  ако(Curl_llist_count(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_COUNT) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_COUNT);
    върни CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, sizeof(*hs) + hlen);
  ако(!hs)
    върни CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &value);
  ако(!result) {
    hs->name = name;
    hs->value = value;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->node);
  }
  иначе {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  върни result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
static void headers_reset(struct Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, NULL);
  data->state.nh_count = 0;
}

struct hds_cw_collect_ctx {
  struct Curl_cwriter super;
};

static CURLcode hds_cw_collect_write(struct Curl_easy *data,
                                     struct Curl_cwriter *writer, цяло type,
                                     const char *buf, size_t blen)
{
  ако((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned char htype = (unsigned char)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_push(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (цяло)result);
    ако(result)
      върни result;
  }
  върни Curl_cwriter_write(data, writer->next, type, buf, blen);
}

static const struct Curl_cwtype hds_cw_collect = {
  "hds-collect",
  NULL,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  sizeof(struct hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(struct Curl_easy *data)
{
  struct Curl_cwriter *writer;
  CURLcode result;

  ако(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ако(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      върни CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ако(result)
      върни result;

    result = Curl_cwriter_add(data, writer);
    ако(result) {
      Curl_cwriter_free(data, writer);
      върни result;
    }
  }
  върни CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(struct Curl_easy *data)
{
  struct Curl_llist_node *e;
  struct Curl_llist_node *n;

  за(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    struct Curl_header_store *hs = Curl_node_elem(e);
    n = Curl_node_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  върни CURLE_OK;
}

#иначе /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           const char *name,
                           size_t index,
                           unsigned цяло origin,
                           цяло request,
                           struct curl_header **hout)
{
  (void)easy;
  (void)name;
  (void)index;
  (void)origin;
  (void)request;
  (void)hout;
  върни CURLHE_NOT_BUILT_IN;
}

struct curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned цяло type,
                                         цяло request,
                                         struct curl_header *prev)
{
  (void)easy;
  (void)type;
  (void)request;
  (void)prev;
  върни NULL;
}
#endif
