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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "headers.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ସ୍ଥିର ଶୂନ୍ୟ copy_header_external(ସଂରଚନା Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e,
                                 ସଂରଚନା curl_header *hout)
{
  ସଂରଚନା curl_header *h = hout;
  h->name = hs->name;
  h->ମୂଲ୍ୟ = hs->ମୂଲ୍ୟ;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ଅଚଳ ଅକ୍ଷର *name,
                           size_t nameindex,
                           unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା origin,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା request,
                           ସଂରଚନା curl_header **hout)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e_pick = ଖାଲି;
    size_t match = 0;
    size_t amount = 0;
    ସଂରଚନା Curl_header_store *hs = ଖାଲି;
    ସଂରଚନା Curl_header_store *pick = ଖାଲି;
    ଯଦି(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      ଯାଅ out;
    }
    ଯଦି(!Curl_llist_ଗଣନା(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      ଯାଅ out;
    }
    ଯଦି(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      ଯାଅ out;
    }
    ଯଦି(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    ପାଇଁ(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_ଗଣ୍ଠି_next(e)) {
      hs = Curl_ଗଣ୍ଠି_elem(e);
      ଯଦି(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ଯଦି(!amount)
      hresult = CURLHE_MISSING;
    ଅନ୍ୟଥା ଯଦି(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ଯଦି(hresult)
      ଯାଅ out;

    ଯଦି(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    ଅନ୍ୟଥା {
      ପାଇଁ(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_ଗଣ୍ଠି_next(e)) {
        hs = Curl_ଗଣ୍ଠି_elem(e);
        ଯଦି(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          ଭାଙ୍ଗ;
        }
      }
      ଯଦି(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        ଯାଅ out;
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
  ଯଦି(result)
    hresult = Curl_eapi_hcode(result);
  ଫେରନ୍ତୁ hresult;
}

ସଂରଚନା nextheader_cache_entry {
  ସଂରଚନା Curl_header_store *header;
  size_t order;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextheader_compare(ଅଚଳ ଶୂନ୍ୟ *p1, ଅଚଳ ଶୂନ୍ୟ *p2)
{
  ଅଚଳ ସଂରଚନା nextheader_cache_entry *e1 = p1;
  ଅଚଳ ସଂରଚନା nextheader_cache_entry *e2 = p2;
  ଅଚଳ ସଂରଚନା Curl_header_store *h1 = e1->header;
  ଅଚଳ ସଂରଚନା Curl_header_store *h2 = e2->header;
  ଅଚଳ ଅକ୍ଷର *n1 = h1->name;
  ଅଚଳ ଅକ୍ଷର *n2 = h2->name;

  ଯେତେବେଳେ(*n1 && *n2) {
    unsigned ଅକ୍ଷର c1 = (unsigned ଅକ୍ଷର)Curl_raw_toupper(*n1++);
    unsigned ଅକ୍ଷର c2 = (unsigned ଅକ୍ଷର)Curl_raw_toupper(*n2++);
    ଯଦି(c1 != c2)
      ଫେରନ୍ତୁ c1 > c2 ? 1 : -1;
  }
  ଯଦି(*n1)
    ଫେରନ୍ତୁ 1;
  ଯଦି(*n2)
    ଫେରନ୍ତୁ -1;
  ଯଦି(e1->order != e2->order)
    ଫେରନ୍ତୁ e1->order > e2->order ? 1 : -1;
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର bool nextheader_cache_build(ସଂରଚନା Curl_easy *data,
                                   unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା origin,
                                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା request, size_t ଗଣନା_2)
{
  ସଂରଚନା nextheader_cache_entry *headers;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(ଆକାର(*headers) * ଗଣନା_2);
  ଯଦି(!headers)
    ଫେରନ୍ତୁ FALSE;

  ପାଇଁ(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_ଗଣ୍ଠି_next(e)) {
    ସଂରଚନା Curl_header_store *hs = Curl_ଗଣ୍ଠି_elem(e);
    ଯଦି((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, ଆକାର(*headers), nextheader_compare);
  ପାଇଁ(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    ଯେତେବେଳେ((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    ପାଇଁ(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_ଗଣନା = ଗଣନା_2;
  ଫେରନ୍ତୁ TRUE;
}

/* public API */
ସଂରଚନା curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା origin,
                                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା request,
                                         ସଂରଚନା curl_header *prev)
{
  ସଂରଚନା Curl_eapi_guard guard;
  ସଂରଚନା curl_header *hd = ଖାଲି;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *pick;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t ଗଣନା_2;

    ଯଦି(request > data->state.requests)
      ଯାଅ out;
    ଯଦି(request == -1)
      request = data->state.requests;

    ଯଦି(prev) {
      pick = prev->anchor;
      ଯଦି(!pick)
        /* something is wrong */
        ଯାଅ out;
      pick = Curl_ଗଣ୍ଠି_next(pick);
    }
    ଅନ୍ୟଥା
      pick = Curl_llist_head(&data->state.httphdrs);

    ଯଦି(pick) {
      /* make sure it is the next header of the desired type */
      କର {
        hs = Curl_ଗଣ୍ଠି_elem(pick);
        ଯଦି((hs->type & origin) && (hs->request == request))
          ଭାଙ୍ଗ;
        pick = Curl_ଗଣ୍ଠି_next(pick);
      } ଯେତେବେଳେ(pick);
    }

    ଯଦି(!pick)
      /* no more headers available */
      ଯାଅ out;

    hs = Curl_ଗଣ୍ଠି_elem(pick);
    ଗଣନା_2 = Curl_llist_ଗଣନା(&data->state.httphdrs);

    ଯଦି(((data->state.nh_ଗଣନା == ଗଣନା_2) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, ଗଣନା_2)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    ଅନ୍ୟଥା {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      ପାଇଁ(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_ଗଣ୍ଠି_next(e)) {
        ସଂରଚନା Curl_header_store *check = Curl_ଗଣ୍ଠି_elem(e);
        ଯଦି(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ଯଦି(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ hd;
}

ସ୍ଥିର CURLcode namevalue(ଅକ୍ଷର *header, size_t hlen, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                          ଅକ୍ଷର **name, ଅକ୍ଷର **ମୂଲ୍ୟ)
{
  ଅକ୍ଷର *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  ଯଦି(type == CURLH_PSEUDO) {
    ଯଦି(*header != ':')
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  ଯେତେବେଳେ(*header && (*header != ':'))
    ++header;

  ଯଦି(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  ଯେତେବେଳେ(ISBLANK(*header))
    header++;

  *ମୂଲ୍ୟ = header;

  /* skip all trailing space letters */
  ଯେତେବେଳେ((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ଠେଲ(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *header,
                           size_t hlen, /* length of header */
                           unsigned ଅକ୍ଷର type)
{
  ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
  ଅକ୍ଷର *name = ଖାଲି;
  ସଂରଚନା Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ଅଚଳ size_t ilen = hlen;

  ଯଦି((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    ଫେରନ୍ତୁ CURLE_OK;

  /* trim off newline characters */
  ଯଦି(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  ଯଦି(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  ଯଦି(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;

  ଯଦି(ISBLANK(header[0])) {
    /* pass leading blanks */
    ଯେତେବେଳେ(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    ଯଦି(!hlen)
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }
  ଯଦି(Curl_llist_ଗଣନା(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_ଗଣନା) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_ଗଣନା);
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, ଆକାର(*hs) + hlen);
  ଯଦି(!hs)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &ମୂଲ୍ୟ);
  ଯଦି(!result) {
    hs->name = name;
    hs->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->ଗଣ୍ଠି);
  }
  ଅନ୍ୟଥା {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ସ୍ଥିର ଶୂନ୍ୟ headers_reset(ସଂରଚନା Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, ଖାଲି);
  data->state.nh_ଗଣନା = 0;
}

ସଂରଚନା hds_cw_collect_ctx {
  ସଂରଚନା Curl_cwriter super;
};

ସ୍ଥିର CURLcode hds_cw_collect_write(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା Curl_cwriter *writer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                     ଅଚଳ ଅକ୍ଷର *buf, size_t blen)
{
  ଯଦି((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned ଅକ୍ଷର htype = (unsigned ଅକ୍ଷର)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_ଠେଲ(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ Curl_cwriter_write(data, writer->next, type, buf, blen);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ଖାଲି,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ଆକାର(ସଂରଚନା hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cwriter *writer;
  CURLcode result;

  ଯଦି(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ଯଦି(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      ଫେରନ୍ତୁ CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    result = Curl_cwriter_add(data, writer);
    ଯଦି(result) {
      Curl_cwriter_free(data, writer);
      ଫେରନ୍ତୁ result;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;

  ପାଇଁ(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    ସଂରଚନା Curl_header_store *hs = Curl_ଗଣ୍ଠି_elem(e);
    n = Curl_ଗଣ୍ଠି_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  ଫେରନ୍ତୁ CURLE_OK;
}

#ଅନ୍ୟଥା /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ଅଚଳ ଅକ୍ଷର *name,
                           size_t index,
                           unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା origin,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା request,
                           ସଂରଚନା curl_header **hout)
{
  (ଶୂନ୍ୟ)easy;
  (ଶୂନ୍ୟ)name;
  (ଶୂନ୍ୟ)index;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)request;
  (ଶୂନ୍ୟ)hout;
  ଫେରନ୍ତୁ CURLHE_NOT_BUILT_IN;
}

ସଂରଚନା curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା request,
                                         ସଂରଚନା curl_header *prev)
{
  (ଶୂନ୍ୟ)easy;
  (ଶୂନ୍ୟ)type;
  (ଶୂନ୍ୟ)request;
  (ଶୂନ୍ୟ)prev;
  ଫେରନ୍ତୁ ଖାଲି;
}
#endif
