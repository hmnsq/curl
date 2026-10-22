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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "headers.h"
#અન્તર્ભાવ "strcase.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
સ્થિર શૂન્ય copy_header_external(સંરચના Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 સંરચના Curl_llist_ગાંઠ *e,
                                 સંરચના curl_header *hout)
{
  સંરચના curl_header *h = hout;
  h->name = hs->name;
  h->કિંમત = hs->કિંમત;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned પૂર્ણાંક)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           અચલ અક્ષર *name,
                           size_t nameindex,
                           unsigned પૂર્ણાંક origin,
                           પૂર્ણાંક request,
                           સંરચના curl_header **hout)
{
  સંરચના Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    સંરચના Curl_easy *data = curl;
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_llist_ગાંઠ *e_pick = ખાલી;
    size_t match = 0;
    size_t amount = 0;
    સંરચના Curl_header_store *hs = ખાલી;
    સંરચના Curl_header_store *pick = ખાલી;
    જો(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      જાઓ out;
    }
    જો(!Curl_llist_ગણતરી(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      જાઓ out;
    }
    જો(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      જાઓ out;
    }
    જો(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    માટે(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_ગાંઠ_next(e)) {
      hs = Curl_ગાંઠ_elem(e);
      જો(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    જો(!amount)
      hresult = CURLHE_MISSING;
    નહીંતર જો(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    જો(hresult)
      જાઓ out;

    જો(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    નહીંતર {
      માટે(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_ગાંઠ_next(e)) {
        hs = Curl_ગાંઠ_elem(e);
        જો(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          તોડો;
        }
      }
      જો(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        જાઓ out;
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
  જો(result)
    hresult = Curl_eapi_hcode(result);
  પાછા hresult;
}

સંરચના nextheader_cache_entry {
  સંરચના Curl_header_store *header;
  size_t order;
};

સ્થિર પૂર્ણાંક nextheader_compare(અચલ શૂન્ય *p1, અચલ શૂન્ય *p2)
{
  અચલ સંરચના nextheader_cache_entry *e1 = p1;
  અચલ સંરચના nextheader_cache_entry *e2 = p2;
  અચલ સંરચના Curl_header_store *h1 = e1->header;
  અચલ સંરચના Curl_header_store *h2 = e2->header;
  અચલ અક્ષર *n1 = h1->name;
  અચલ અક્ષર *n2 = h2->name;

  જ્યારે(*n1 && *n2) {
    unsigned અક્ષર c1 = (unsigned અક્ષર)Curl_raw_toupper(*n1++);
    unsigned અક્ષર c2 = (unsigned અક્ષર)Curl_raw_toupper(*n2++);
    જો(c1 != c2)
      પાછા c1 > c2 ? 1 : -1;
  }
  જો(*n1)
    પાછા 1;
  જો(*n2)
    પાછા -1;
  જો(e1->order != e2->order)
    પાછા e1->order > e2->order ? 1 : -1;
  પાછા 0;
}

સ્થિર bool nextheader_cache_build(સંરચના Curl_easy *data,
                                   unsigned પૂર્ણાંક origin,
                                   પૂર્ણાંક request, size_t ગણતરી)
{
  સંરચના nextheader_cache_entry *headers;
  સંરચના Curl_llist_ગાંઠ *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(કદ(*headers) * ગણતરી);
  જો(!headers)
    પાછા FALSE;

  માટે(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_ગાંઠ_next(e)) {
    સંરચના Curl_header_store *hs = Curl_ગાંઠ_elem(e);
    જો((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, કદ(*headers), nextheader_compare);
  માટે(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    જ્યારે((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    માટે(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_ગણતરી = ગણતરી;
  પાછા TRUE;
}

/* public API */
સંરચના curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned પૂર્ણાંક origin,
                                         પૂર્ણાંક request,
                                         સંરચના curl_header *prev)
{
  સંરચના Curl_eapi_guard guard;
  સંરચના curl_header *hd = ખાલી;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    સંરચના Curl_easy *data = curl;
    સંરચના Curl_llist_ગાંઠ *pick;
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t ગણતરી;

    જો(request > data->state.requests)
      જાઓ out;
    જો(request == -1)
      request = data->state.requests;

    જો(prev) {
      pick = prev->anchor;
      જો(!pick)
        /* something is wrong */
        જાઓ out;
      pick = Curl_ગાંઠ_next(pick);
    }
    નહીંતર
      pick = Curl_llist_head(&data->state.httphdrs);

    જો(pick) {
      /* make sure it is the next header of the desired type */
      કરો {
        hs = Curl_ગાંઠ_elem(pick);
        જો((hs->type & origin) && (hs->request == request))
          તોડો;
        pick = Curl_ગાંઠ_next(pick);
      } જ્યારે(pick);
    }

    જો(!pick)
      /* no more headers available */
      જાઓ out;

    hs = Curl_ગાંઠ_elem(pick);
    ગણતરી = Curl_llist_ગણતરી(&data->state.httphdrs);

    જો(((data->state.nh_ગણતરી == ગણતરી) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, ગણતરી)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    નહીંતર {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      માટે(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_ગાંઠ_next(e)) {
        સંરચના Curl_header_store *check = Curl_ગાંઠ_elem(e);
        જો(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        જો(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા hd;
}

સ્થિર CURLcode namevalue(અક્ષર *header, size_t hlen, unsigned પૂર્ણાંક type,
                          અક્ષર **name, અક્ષર **કિંમત)
{
  અક્ષર *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  જો(type == CURLH_PSEUDO) {
    જો(*header != ':')
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  જ્યારે(*header && (*header != ':'))
    ++header;

  જો(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  નહીંતર
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  જ્યારે(ISBLANK(*header))
    header++;

  *કિંમત = header;

  /* skip all trailing space letters */
  જ્યારે((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  પાછા CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ધકેલો(સંરચના Curl_easy *data, અચલ અક્ષર *header,
                           size_t hlen, /* length of header */
                           unsigned અક્ષર type)
{
  અક્ષર *કિંમત = ખાલી;
  અક્ષર *name = ખાલી;
  સંરચના Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  અચલ size_t ilen = hlen;

  જો((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    પાછા CURLE_OK;

  /* trim off newline characters */
  જો(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  જો(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  જો(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    પાછા CURLE_WEIRD_SERVER_REPLY;

  જો(ISBLANK(header[0])) {
    /* pass leading blanks */
    જ્યારે(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    જો(!hlen)
      પાછા CURLE_WEIRD_SERVER_REPLY;
  }
  જો(Curl_llist_ગણતરી(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_ગણતરી) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_ગણતરી);
    પાછા CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, કદ(*hs) + hlen);
  જો(!hs)
    પાછા CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &કિંમત);
  જો(!result) {
    hs->name = name;
    hs->કિંમત = કિંમત;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->ગાંઠ);
  }
  નહીંતર {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  પાછા result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
સ્થિર શૂન્ય headers_reset(સંરચના Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, ખાલી);
  data->state.nh_ગણતરી = 0;
}

સંરચના hds_cw_collect_ctx {
  સંરચના Curl_cwriter super;
};

સ્થિર CURLcode hds_cw_collect_write(સંરચના Curl_easy *data,
                                     સંરચના Curl_cwriter *writer, પૂર્ણાંક type,
                                     અચલ અક્ષર *buf, size_t blen)
{
  જો((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned અક્ષર htype = (unsigned અક્ષર)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_ધકેલો(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (પૂર્ણાંક)result);
    જો(result)
      પાછા result;
  }
  પાછા Curl_cwriter_write(data, writer->next, type, buf, blen);
}

સ્થિર અચલ સંરચના Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ખાલી,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  કદ(સંરચના hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(સંરચના Curl_easy *data)
{
  સંરચના Curl_cwriter *writer;
  CURLcode result;

  જો(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    જો(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      પાછા CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    જો(result)
      પાછા result;

    result = Curl_cwriter_add(data, writer);
    જો(result) {
      Curl_cwriter_free(data, writer);
      પાછા result;
    }
  }
  પાછા CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(સંરચના Curl_easy *data)
{
  સંરચના Curl_llist_ગાંઠ *e;
  સંરચના Curl_llist_ગાંઠ *n;

  માટે(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    સંરચના Curl_header_store *hs = Curl_ગાંઠ_elem(e);
    n = Curl_ગાંઠ_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  પાછા CURLE_OK;
}

#નહીંતર /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           અચલ અક્ષર *name,
                           size_t index,
                           unsigned પૂર્ણાંક origin,
                           પૂર્ણાંક request,
                           સંરચના curl_header **hout)
{
  (શૂન્ય)easy;
  (શૂન્ય)name;
  (શૂન્ય)index;
  (શૂન્ય)origin;
  (શૂન્ય)request;
  (શૂન્ય)hout;
  પાછા CURLHE_NOT_BUILT_IN;
}

સંરચના curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned પૂર્ણાંક type,
                                         પૂર્ણાંક request,
                                         સંરચના curl_header *prev)
{
  (શૂન્ય)easy;
  (શૂન્ય)type;
  (શૂન્ય)request;
  (શૂન્ય)prev;
  પાછા ખાલી;
}
#endif
