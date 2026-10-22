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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "headers.h"
#شامل "strcase.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ایستا تهی copy_header_external(ساختار Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 ساختار Curl_llist_گره *e,
                                 ساختار curl_header *hout)
{
  ساختار curl_header *h = hout;
  h->name = hs->name;
  h->مقدار = hs->مقدار;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned صحیح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ثابت نویسه *name,
                           size_t nameindex,
                           unsigned صحیح origin,
                           صحیح request,
                           ساختار curl_header **hout)
{
  ساختار Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    ساختار Curl_easy *data = curl;
    ساختار Curl_llist_گره *e;
    ساختار Curl_llist_گره *e_pick = پوچ;
    size_t match = 0;
    size_t amount = 0;
    ساختار Curl_header_store *hs = پوچ;
    ساختار Curl_header_store *pick = پوچ;
    اگر(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      برو out;
    }
    اگر(!Curl_llist_شمار(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      برو out;
    }
    اگر(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      برو out;
    }
    اگر(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    برای(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_گره_next(e)) {
      hs = Curl_گره_elem(e);
      اگر(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    اگر(!amount)
      hresult = CURLHE_MISSING;
    وگرنه اگر(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    اگر(hresult)
      برو out;

    اگر(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    وگرنه {
      برای(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_گره_next(e)) {
        hs = Curl_گره_elem(e);
        اگر(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          بشکن;
        }
      }
      اگر(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        برو out;
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
  اگر(result)
    hresult = Curl_eapi_hcode(result);
  بازگشت hresult;
}

ساختار nextheader_cache_entry {
  ساختار Curl_header_store *header;
  size_t order;
};

ایستا صحیح nextheader_compare(ثابت تهی *p1, ثابت تهی *p2)
{
  ثابت ساختار nextheader_cache_entry *e1 = p1;
  ثابت ساختار nextheader_cache_entry *e2 = p2;
  ثابت ساختار Curl_header_store *h1 = e1->header;
  ثابت ساختار Curl_header_store *h2 = e2->header;
  ثابت نویسه *n1 = h1->name;
  ثابت نویسه *n2 = h2->name;

  وقتی(*n1 && *n2) {
    unsigned نویسه c1 = (unsigned نویسه)Curl_raw_toupper(*n1++);
    unsigned نویسه c2 = (unsigned نویسه)Curl_raw_toupper(*n2++);
    اگر(c1 != c2)
      بازگشت c1 > c2 ? 1 : -1;
  }
  اگر(*n1)
    بازگشت 1;
  اگر(*n2)
    بازگشت -1;
  اگر(e1->order != e2->order)
    بازگشت e1->order > e2->order ? 1 : -1;
  بازگشت 0;
}

ایستا bool nextheader_cache_build(ساختار Curl_easy *data,
                                   unsigned صحیح origin,
                                   صحیح request, size_t شمار)
{
  ساختار nextheader_cache_entry *headers;
  ساختار Curl_llist_گره *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(اندازه(*headers) * شمار);
  اگر(!headers)
    بازگشت FALSE;

  برای(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_گره_next(e)) {
    ساختار Curl_header_store *hs = Curl_گره_elem(e);
    اگر((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, اندازه(*headers), nextheader_compare);
  برای(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    وقتی((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    برای(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_شمار = شمار;
  بازگشت TRUE;
}

/* public API */
ساختار curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned صحیح origin,
                                         صحیح request,
                                         ساختار curl_header *prev)
{
  ساختار Curl_eapi_guard guard;
  ساختار curl_header *hd = پوچ;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    ساختار Curl_easy *data = curl;
    ساختار Curl_llist_گره *pick;
    ساختار Curl_llist_گره *e;
    ساختار Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t شمار;

    اگر(request > data->state.requests)
      برو out;
    اگر(request == -1)
      request = data->state.requests;

    اگر(prev) {
      pick = prev->anchor;
      اگر(!pick)
        /* something is wrong */
        برو out;
      pick = Curl_گره_next(pick);
    }
    وگرنه
      pick = Curl_llist_head(&data->state.httphdrs);

    اگر(pick) {
      /* make sure it is the next header of the desired type */
      انجامبده {
        hs = Curl_گره_elem(pick);
        اگر((hs->type & origin) && (hs->request == request))
          بشکن;
        pick = Curl_گره_next(pick);
      } وقتی(pick);
    }

    اگر(!pick)
      /* no more headers available */
      برو out;

    hs = Curl_گره_elem(pick);
    شمار = Curl_llist_شمار(&data->state.httphdrs);

    اگر(((data->state.nh_شمار == شمار) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, شمار)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    وگرنه {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      برای(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_گره_next(e)) {
        ساختار Curl_header_store *check = Curl_گره_elem(e);
        اگر(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        اگر(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت hd;
}

ایستا CURLcode namevalue(نویسه *header, size_t hlen, unsigned صحیح type,
                          نویسه **name, نویسه **مقدار)
{
  نویسه *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  اگر(type == CURLH_PSEUDO) {
    اگر(*header != ':')
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  وقتی(*header && (*header != ':'))
    ++header;

  اگر(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  وگرنه
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  وقتی(ISBLANK(*header))
    header++;

  *مقدار = header;

  /* skip all trailing space letters */
  وقتی((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  بازگشت CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_افزودن(ساختار Curl_easy *data, ثابت نویسه *header,
                           size_t hlen, /* length of header */
                           unsigned نویسه type)
{
  نویسه *مقدار = پوچ;
  نویسه *name = پوچ;
  ساختار Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ثابت size_t ilen = hlen;

  اگر((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    بازگشت CURLE_OK;

  /* trim off newline characters */
  اگر(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  اگر(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  اگر(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    بازگشت CURLE_WEIRD_SERVER_REPLY;

  اگر(ISBLANK(header[0])) {
    /* pass leading blanks */
    وقتی(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    اگر(!hlen)
      بازگشت CURLE_WEIRD_SERVER_REPLY;
  }
  اگر(Curl_llist_شمار(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_شمار) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_شمار);
    بازگشت CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, اندازه(*hs) + hlen);
  اگر(!hs)
    بازگشت CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &مقدار);
  اگر(!result) {
    hs->name = name;
    hs->مقدار = مقدار;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->گره);
  }
  وگرنه {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  بازگشت result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ایستا تهی headers_reset(ساختار Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, پوچ);
  data->state.nh_شمار = 0;
}

ساختار hds_cw_collect_ctx {
  ساختار Curl_cwriter super;
};

ایستا CURLcode hds_cw_collect_write(ساختار Curl_easy *data,
                                     ساختار Curl_cwriter *writer, صحیح type,
                                     ثابت نویسه *buf, size_t blen)
{
  اگر((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned نویسه htype = (unsigned نویسه)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_افزودن(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحیح)result);
    اگر(result)
      بازگشت result;
  }
  بازگشت Curl_cwriter_write(data, writer->next, type, buf, blen);
}

ایستا ثابت ساختار Curl_cwtype hds_cw_collect = {
  "hds-collect",
  پوچ,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  اندازه(ساختار hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(ساختار Curl_easy *data)
{
  ساختار Curl_cwriter *writer;
  CURLcode result;

  اگر(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    اگر(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      بازگشت CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    اگر(result)
      بازگشت result;

    result = Curl_cwriter_add(data, writer);
    اگر(result) {
      Curl_cwriter_free(data, writer);
      بازگشت result;
    }
  }
  بازگشت CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(ساختار Curl_easy *data)
{
  ساختار Curl_llist_گره *e;
  ساختار Curl_llist_گره *n;

  برای(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    ساختار Curl_header_store *hs = Curl_گره_elem(e);
    n = Curl_گره_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  بازگشت CURLE_OK;
}

#وگرنه /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ثابت نویسه *name,
                           size_t index,
                           unsigned صحیح origin,
                           صحیح request,
                           ساختار curl_header **hout)
{
  (تهی)easy;
  (تهی)name;
  (تهی)index;
  (تهی)origin;
  (تهی)request;
  (تهی)hout;
  بازگشت CURLHE_NOT_BUILT_IN;
}

ساختار curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned صحیح type,
                                         صحیح request,
                                         ساختار curl_header *prev)
{
  (تهی)easy;
  (تهی)type;
  (تهی)request;
  (تهی)prev;
  بازگشت پوچ;
}
#endif
