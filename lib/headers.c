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
স্থির শূন্য copy_header_external(কাঠামো Curl_header_store *hs,
                                 size_t সূচক,
                                 size_t amount,
                                 কাঠামো Curl_llist_গ্রন্থি *e,
                                 কাঠামো curl_header *hout)
{
  কাঠামো curl_header *h = hout;
  h->name = hs->name;
  h->মান = hs->মান;
  h->amount = amount;
  h->সূচক = সূচক;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned পূর্ণসংখ্যা)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ধ্রুবক অক্ষর *name,
                           size_t nameindex,
                           unsigned পূর্ণসংখ্যা origin,
                           পূর্ণসংখ্যা request,
                           কাঠামো curl_header **hout)
{
  কাঠামো Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_header, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    কাঠামো Curl_llist_গ্রন্থি *e;
    কাঠামো Curl_llist_গ্রন্থি *e_pick = ফাঁকা;
    size_t match = 0;
    size_t amount = 0;
    কাঠামো Curl_header_store *hs = ফাঁকা;
    কাঠামো Curl_header_store *pick = ফাঁকা;
    যদি(!name || !hout || !তথ্য ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      যাও out;
    }
    যদি(!Curl_llist_গুনতি(&তথ্য->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      যাও out;
    }
    যদি(request > তথ্য->state.requests) {
      hresult = CURLHE_NOREQUEST;
      যাও out;
    }
    যদি(request == -1)
      request = তথ্য->state.requests;

    /* we need a first round to count amount of this header */
    জন্য(e = Curl_llist_শীর্ষ(&তথ্য->state.httphdrs); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
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
      যাও out;

    যদি(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    অন্যথা {
      জন্য(e = Curl_llist_শীর্ষ(&তথ্য->state.httphdrs); e;
          e = Curl_গ্রন্থি_পরবর্তী(e)) {
        hs = Curl_গ্রন্থি_elem(e);
        যদি(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          বিরতি;
        }
      }
      যদি(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        যাও out;
      }
    }
    /* this is the name we want */
    copy_header_external(hs, nameindex, amount, e_pick,
                         &তথ্য->state.headerout[0]);
    *hout = &তথ্য->state.headerout[0];
    hresult = CURLHE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  যদি(ফলাফল)
    hresult = Curl_eapi_hcode(ফলাফল);
  ফেরত hresult;
}

কাঠামো nextheader_cache_entry {
  কাঠামো Curl_header_store *header;
  size_t order;
};

স্থির পূর্ণসংখ্যা nextheader_compare(ধ্রুবক শূন্য *p1, ধ্রুবক শূন্য *p2)
{
  ধ্রুবক কাঠামো nextheader_cache_entry *e1 = p1;
  ধ্রুবক কাঠামো nextheader_cache_entry *e2 = p2;
  ধ্রুবক কাঠামো Curl_header_store *h1 = e1->header;
  ধ্রুবক কাঠামো Curl_header_store *h2 = e2->header;
  ধ্রুবক অক্ষর *n1 = h1->name;
  ধ্রুবক অক্ষর *n2 = h2->name;

  যখন(*n1 && *n2) {
    unsigned অক্ষর c1 = (unsigned অক্ষর)Curl_raw_toupper(*n1++);
    unsigned অক্ষর c2 = (unsigned অক্ষর)Curl_raw_toupper(*n2++);
    যদি(c1 != c2)
      ফেরত c1 > c2 ? 1 : -1;
  }
  যদি(*n1)
    ফেরত 1;
  যদি(*n2)
    ফেরত -1;
  যদি(e1->order != e2->order)
    ফেরত e1->order > e2->order ? 1 : -1;
  ফেরত 0;
}

স্থির bool nextheader_cache_build(কাঠামো Curl_easy *তথ্য,
                                   unsigned পূর্ণসংখ্যা origin,
                                   পূর্ণসংখ্যা request, size_t গুনতি)
{
  কাঠামো nextheader_cache_entry *headers;
  কাঠামো Curl_llist_গ্রন্থি *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(আকার(*headers) * গুনতি);
  যদি(!headers)
    ফেরত FALSE;

  জন্য(e = Curl_llist_শীর্ষ(&তথ্য->state.httphdrs); e;
      e = Curl_গ্রন্থি_পরবর্তী(e)) {
    কাঠামো Curl_header_store *hs = Curl_গ্রন্থি_elem(e);
    যদি((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, আকার(*headers), nextheader_compare);
  জন্য(first = 0; first < i;) {
    size_t last = first + 1;
    size_t সূচক;
    যখন((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    জন্য(সূচক = first; সূচক < last; সূচক++) {
      headers[সূচক].header->nh_amount = last - first;
      headers[সূচক].header->nh_সূচক = সূচক - first;
    }
    first = last;
  }
  curlx_free(headers);

  তথ্য->state.nh_origin = origin;
  তথ্য->state.nh_request = request;
  তথ্য->state.nh_গুনতি = গুনতি;
  ফেরত TRUE;
}

/* public API */
কাঠামো curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned পূর্ণসংখ্যা origin,
                                         পূর্ণসংখ্যা request,
                                         কাঠামো curl_header *পূর্ব)
{
  কাঠামো Curl_eapi_guard guard;
  কাঠামো curl_header *hd = ফাঁকা;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    কাঠামো Curl_llist_গ্রন্থি *pick;
    কাঠামো Curl_llist_গ্রন্থি *e;
    কাঠামো Curl_header_store *hs;
    size_t amount = 0;
    size_t সূচক = 0;
    size_t গুনতি;

    যদি(request > তথ্য->state.requests)
      যাও out;
    যদি(request == -1)
      request = তথ্য->state.requests;

    যদি(পূর্ব) {
      pick = পূর্ব->anchor;
      যদি(!pick)
        /* something is wrong */
        যাও out;
      pick = Curl_গ্রন্থি_পরবর্তী(pick);
    }
    অন্যথা
      pick = Curl_llist_শীর্ষ(&তথ্য->state.httphdrs);

    যদি(pick) {
      /* make sure it is the next header of the desired type */
      করো {
        hs = Curl_গ্রন্থি_elem(pick);
        যদি((hs->type & origin) && (hs->request == request))
          বিরতি;
        pick = Curl_গ্রন্থি_পরবর্তী(pick);
      } যখন(pick);
    }

    যদি(!pick)
      /* no more headers available */
      যাও out;

    hs = Curl_গ্রন্থি_elem(pick);
    গুনতি = Curl_llist_গুনতি(&তথ্য->state.httphdrs);

    যদি(((তথ্য->state.nh_গুনতি == গুনতি) &&
        (তথ্য->state.nh_origin == origin) &&
        (তথ্য->state.nh_request == request)) ||
       nextheader_cache_build(তথ্য, origin, request, গুনতি)) {
      amount = hs->nh_amount;
      সূচক = hs->nh_সূচক;
    }
    অন্যথা {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      জন্য(e = Curl_llist_শীর্ষ(&তথ্য->state.httphdrs); e;
          e = Curl_গ্রন্থি_পরবর্তী(e)) {
        কাঠামো Curl_header_store *check = Curl_গ্রন্থি_elem(e);
        যদি(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        যদি(e == pick)
          সূচক = amount - 1;
      }
    }
    copy_header_external(hs, সূচক, amount, pick,
                         &তথ্য->state.headerout[1]);
    hd = &তথ্য->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত hd;
}

স্থির CURLcode namevalue(অক্ষর *header, size_t hlen, unsigned পূর্ণসংখ্যা type,
                          অক্ষর **name, অক্ষর **মান)
{
  অক্ষর *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  যদি(type == CURLH_PSEUDO) {
    যদি(*header != ':')
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  যখন(*header && (*header != ':'))
    ++header;

  যদি(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  অন্যথা
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  যখন(ISBLANK(*header))
    header++;

  *মান = header;

  /* skip all trailing space letters */
  যখন((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  ফেরত CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ঠেলো(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *header,
                           size_t hlen, /* length of header */
                           unsigned অক্ষর type)
{
  অক্ষর *মান = ফাঁকা;
  অক্ষর *name = ফাঁকা;
  কাঠামো Curl_header_store *hs;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  ধ্রুবক size_t ilen = hlen;

  যদি((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    ফেরত CURLE_OK;

  /* trim off newline characters */
  যদি(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  যদি(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  যদি(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ফেরত CURLE_WEIRD_SERVER_REPLY;

  যদি(ISBLANK(header[0])) {
    /* pass leading blanks */
    যখন(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    যদি(!hlen)
      ফেরত CURLE_WEIRD_SERVER_REPLY;
  }
  যদি(Curl_llist_গুনতি(&তথ্য->state.httphdrs) >= MAX_HTTP_RESP_HEADER_গুনতি) {
    failf(তথ্য, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_গুনতি);
    ফেরত CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, আকার(*hs) + hlen);
  যদি(!hs)
    ফেরত CURLE_OUT_OF_MEMORY;
  memcpy(hs->বাফার, header, hlen);
  hs->বাফার[hlen] = 0; /* null-terminate */

  ফলাফল = namevalue(hs->বাফার, hlen, type, &name, &মান);
  যদি(!ফলাফল) {
    hs->name = name;
    hs->মান = মান;
    hs->type = type;
    hs->request = তথ্য->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&তথ্য->state.httphdrs, hs, &hs->গ্রন্থি);
  }
  অন্যথা {
    failf(তথ্য, "Invalid response header");
    curlx_free(hs);
  }
  ফেরত ফলাফল;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
স্থির শূন্য headers_reset(কাঠামো Curl_easy *তথ্য)
{
  Curl_llist_init(&তথ্য->state.httphdrs, ফাঁকা);
  তথ্য->state.nh_গুনতি = 0;
}

কাঠামো hds_cw_collect_ctx {
  কাঠামো Curl_cwriter super;
};

স্থির CURLcode hds_cw_collect_write(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_cwriter *writer, পূর্ণসংখ্যা type,
                                     ধ্রুবক অক্ষর *buf, size_t blen)
{
  যদি((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned অক্ষর htype = (unsigned অক্ষর)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode ফলাফল = Curl_headers_ঠেলো(তথ্য, buf, blen, htype);
    CURL_TRC_WRITE(তথ্য, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত Curl_cwriter_write(তথ্য, writer->পরবর্তী, type, buf, blen);
}

স্থির ধ্রুবক কাঠামো Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ফাঁকা,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  আকার(কাঠামো hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cwriter *writer;
  CURLcode ফলাফল;

  যদি(তথ্য->conn && (তথ্য->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    যদি(Curl_cwriter_get_by_name(তথ্য, hds_cw_collect.name))
      ফেরত CURLE_OK;

    ফলাফল = Curl_cwriter_create(&writer, তথ্য, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    ফলাফল = Curl_cwriter_add(তথ্য, writer);
    যদি(ফলাফল) {
      Curl_cwriter_free(তথ্য, writer);
      ফেরত ফলাফল;
    }
  }
  ফেরত CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_llist_গ্রন্থি *e;
  কাঠামো Curl_llist_গ্রন্থি *n;

  জন্য(e = Curl_llist_শীর্ষ(&তথ্য->state.httphdrs); e; e = n) {
    কাঠামো Curl_header_store *hs = Curl_গ্রন্থি_elem(e);
    n = Curl_গ্রন্থি_পরবর্তী(e);
    curlx_free(hs);
  }
  headers_reset(তথ্য);
  ফেরত CURLE_OK;
}

#অন্যথা /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ধ্রুবক অক্ষর *name,
                           size_t সূচক,
                           unsigned পূর্ণসংখ্যা origin,
                           পূর্ণসংখ্যা request,
                           কাঠামো curl_header **hout)
{
  (শূন্য)easy;
  (শূন্য)name;
  (শূন্য)সূচক;
  (শূন্য)origin;
  (শূন্য)request;
  (শূন্য)hout;
  ফেরত CURLHE_NOT_BUILT_IN;
}

কাঠামো curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned পূর্ণসংখ্যা type,
                                         পূর্ণসংখ্যা request,
                                         কাঠামো curl_header *পূর্ব)
{
  (শূন্য)easy;
  (শূন্য)type;
  (শূন্য)request;
  (শূন্য)পূর্ব;
  ফেরত ফাঁকা;
}
#endif
