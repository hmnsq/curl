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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "headers.h"
#അന്തര്ഭാവ "strcase.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
സ്ഥിതം ശൂന്യം copy_header_external(ഘടന Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 ഘടന Curl_llist_കണ്ണി *e,
                                 ഘടന curl_header *hout)
{
  ഘടന curl_header *h = hout;
  h->name = hs->name;
  h->മൂല്യം = hs->മൂല്യം;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned പൂർണ്ണസംഖ്യ)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           സ്ഥിരം അക്ഷരം *name,
                           size_t nameindex,
                           unsigned പൂർണ്ണസംഖ്യ origin,
                           പൂർണ്ണസംഖ്യ request,
                           ഘടന curl_header **hout)
{
  ഘടന Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    ഘടന Curl_easy *data = curl;
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_llist_കണ്ണി *e_pick = ശൂന്യ;
    size_t match = 0;
    size_t amount = 0;
    ഘടന Curl_header_store *hs = ശൂന്യ;
    ഘടന Curl_header_store *pick = ശൂന്യ;
    എങ്കിൽ(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      പോകുക out;
    }
    എങ്കിൽ(!Curl_llist_എണ്ണം(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      പോകുക out;
    }
    എങ്കിൽ(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      പോകുക out;
    }
    എങ്കിൽ(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    വേണ്ടി(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_കണ്ണി_next(e)) {
      hs = Curl_കണ്ണി_elem(e);
      എങ്കിൽ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    എങ്കിൽ(!amount)
      hresult = CURLHE_MISSING;
    അല്ലെങ്കിൽ എങ്കിൽ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    എങ്കിൽ(hresult)
      പോകുക out;

    എങ്കിൽ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    അല്ലെങ്കിൽ {
      വേണ്ടി(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_കണ്ണി_next(e)) {
        hs = Curl_കണ്ണി_elem(e);
        എങ്കിൽ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          നിർത്തുക;
        }
      }
      എങ്കിൽ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        പോകുക out;
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
  എങ്കിൽ(result)
    hresult = Curl_eapi_hcode(result);
  തിരികെ hresult;
}

ഘടന nextheader_cache_entry {
  ഘടന Curl_header_store *header;
  size_t order;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ nextheader_compare(സ്ഥിരം ശൂന്യം *p1, സ്ഥിരം ശൂന്യം *p2)
{
  സ്ഥിരം ഘടന nextheader_cache_entry *e1 = p1;
  സ്ഥിരം ഘടന nextheader_cache_entry *e2 = p2;
  സ്ഥിരം ഘടന Curl_header_store *h1 = e1->header;
  സ്ഥിരം ഘടന Curl_header_store *h2 = e2->header;
  സ്ഥിരം അക്ഷരം *n1 = h1->name;
  സ്ഥിരം അക്ഷരം *n2 = h2->name;

  ആകുമ്പോൾ(*n1 && *n2) {
    unsigned അക്ഷരം c1 = (unsigned അക്ഷരം)Curl_raw_toupper(*n1++);
    unsigned അക്ഷരം c2 = (unsigned അക്ഷരം)Curl_raw_toupper(*n2++);
    എങ്കിൽ(c1 != c2)
      തിരികെ c1 > c2 ? 1 : -1;
  }
  എങ്കിൽ(*n1)
    തിരികെ 1;
  എങ്കിൽ(*n2)
    തിരികെ -1;
  എങ്കിൽ(e1->order != e2->order)
    തിരികെ e1->order > e2->order ? 1 : -1;
  തിരികെ 0;
}

സ്ഥിതം bool nextheader_cache_build(ഘടന Curl_easy *data,
                                   unsigned പൂർണ്ണസംഖ്യ origin,
                                   പൂർണ്ണസംഖ്യ request, size_t എണ്ണം)
{
  ഘടന nextheader_cache_entry *headers;
  ഘടന Curl_llist_കണ്ണി *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(വലുപ്പം(*headers) * എണ്ണം);
  എങ്കിൽ(!headers)
    തിരികെ FALSE;

  വേണ്ടി(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_കണ്ണി_next(e)) {
    ഘടന Curl_header_store *hs = Curl_കണ്ണി_elem(e);
    എങ്കിൽ((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, വലുപ്പം(*headers), nextheader_compare);
  വേണ്ടി(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    ആകുമ്പോൾ((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    വേണ്ടി(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_എണ്ണം = എണ്ണം;
  തിരികെ TRUE;
}

/* public API */
ഘടന curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned പൂർണ്ണസംഖ്യ origin,
                                         പൂർണ്ണസംഖ്യ request,
                                         ഘടന curl_header *prev)
{
  ഘടന Curl_eapi_guard guard;
  ഘടന curl_header *hd = ശൂന്യ;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    ഘടന Curl_easy *data = curl;
    ഘടന Curl_llist_കണ്ണി *pick;
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t എണ്ണം;

    എങ്കിൽ(request > data->state.requests)
      പോകുക out;
    എങ്കിൽ(request == -1)
      request = data->state.requests;

    എങ്കിൽ(prev) {
      pick = prev->anchor;
      എങ്കിൽ(!pick)
        /* something is wrong */
        പോകുക out;
      pick = Curl_കണ്ണി_next(pick);
    }
    അല്ലെങ്കിൽ
      pick = Curl_llist_head(&data->state.httphdrs);

    എങ്കിൽ(pick) {
      /* make sure it is the next header of the desired type */
      ചെയ്യുക {
        hs = Curl_കണ്ണി_elem(pick);
        എങ്കിൽ((hs->type & origin) && (hs->request == request))
          നിർത്തുക;
        pick = Curl_കണ്ണി_next(pick);
      } ആകുമ്പോൾ(pick);
    }

    എങ്കിൽ(!pick)
      /* no more headers available */
      പോകുക out;

    hs = Curl_കണ്ണി_elem(pick);
    എണ്ണം = Curl_llist_എണ്ണം(&data->state.httphdrs);

    എങ്കിൽ(((data->state.nh_എണ്ണം == എണ്ണം) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, എണ്ണം)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    അല്ലെങ്കിൽ {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      വേണ്ടി(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_കണ്ണി_next(e)) {
        ഘടന Curl_header_store *check = Curl_കണ്ണി_elem(e);
        എങ്കിൽ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        എങ്കിൽ(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  തിരികെ hd;
}

സ്ഥിതം CURLcode namevalue(അക്ഷരം *header, size_t hlen, unsigned പൂർണ്ണസംഖ്യ type,
                          അക്ഷരം **name, അക്ഷരം **മൂല്യം)
{
  അക്ഷരം *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  എങ്കിൽ(type == CURLH_PSEUDO) {
    എങ്കിൽ(*header != ':')
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  ആകുമ്പോൾ(*header && (*header != ':'))
    ++header;

  എങ്കിൽ(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  അല്ലെങ്കിൽ
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  ആകുമ്പോൾ(ISBLANK(*header))
    header++;

  *മൂല്യം = header;

  /* skip all trailing space letters */
  ആകുമ്പോൾ((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  തിരികെ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_തള്ളുക(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *header,
                           size_t hlen, /* length of header */
                           unsigned അക്ഷരം type)
{
  അക്ഷരം *മൂല്യം = ശൂന്യ;
  അക്ഷരം *name = ശൂന്യ;
  ഘടന Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  സ്ഥിരം size_t ilen = hlen;

  എങ്കിൽ((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    തിരികെ CURLE_OK;

  /* trim off newline characters */
  എങ്കിൽ(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  എങ്കിൽ(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  എങ്കിൽ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    തിരികെ CURLE_WEIRD_SERVER_REPLY;

  എങ്കിൽ(ISBLANK(header[0])) {
    /* pass leading blanks */
    ആകുമ്പോൾ(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    എങ്കിൽ(!hlen)
      തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }
  എങ്കിൽ(Curl_llist_എണ്ണം(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_എണ്ണം) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_എണ്ണം);
    തിരികെ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, വലുപ്പം(*hs) + hlen);
  എങ്കിൽ(!hs)
    തിരികെ CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &മൂല്യം);
  എങ്കിൽ(!result) {
    hs->name = name;
    hs->മൂല്യം = മൂല്യം;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->കണ്ണി);
  }
  അല്ലെങ്കിൽ {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  തിരികെ result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
സ്ഥിതം ശൂന്യം headers_reset(ഘടന Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, ശൂന്യ);
  data->state.nh_എണ്ണം = 0;
}

ഘടന hds_cw_collect_ctx {
  ഘടന Curl_cwriter super;
};

സ്ഥിതം CURLcode hds_cw_collect_write(ഘടന Curl_easy *data,
                                     ഘടന Curl_cwriter *writer, പൂർണ്ണസംഖ്യ type,
                                     സ്ഥിരം അക്ഷരം *buf, size_t blen)
{
  എങ്കിൽ((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned അക്ഷരം htype = (unsigned അക്ഷരം)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_തള്ളുക(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ Curl_cwriter_write(data, writer->next, type, buf, blen);
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ശൂന്യ,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  വലുപ്പം(ഘടന hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(ഘടന Curl_easy *data)
{
  ഘടന Curl_cwriter *writer;
  CURLcode result;

  എങ്കിൽ(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    എങ്കിൽ(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      തിരികെ CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    എങ്കിൽ(result)
      തിരികെ result;

    result = Curl_cwriter_add(data, writer);
    എങ്കിൽ(result) {
      Curl_cwriter_free(data, writer);
      തിരികെ result;
    }
  }
  തിരികെ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(ഘടന Curl_easy *data)
{
  ഘടന Curl_llist_കണ്ണി *e;
  ഘടന Curl_llist_കണ്ണി *n;

  വേണ്ടി(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    ഘടന Curl_header_store *hs = Curl_കണ്ണി_elem(e);
    n = Curl_കണ്ണി_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  തിരികെ CURLE_OK;
}

#അല്ലെങ്കിൽ /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           സ്ഥിരം അക്ഷരം *name,
                           size_t index,
                           unsigned പൂർണ്ണസംഖ്യ origin,
                           പൂർണ്ണസംഖ്യ request,
                           ഘടന curl_header **hout)
{
  (ശൂന്യം)easy;
  (ശൂന്യം)name;
  (ശൂന്യം)index;
  (ശൂന്യം)origin;
  (ശൂന്യം)request;
  (ശൂന്യം)hout;
  തിരികെ CURLHE_NOT_BUILT_IN;
}

ഘടന curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned പൂർണ്ണസംഖ്യ type,
                                         പൂർണ്ണസംഖ്യ request,
                                         ഘടന curl_header *prev)
{
  (ശൂന്യം)easy;
  (ശൂന്യം)type;
  (ശൂന്യം)request;
  (ശൂന്യം)prev;
  തിരികെ ശൂന്യ;
}
#endif
