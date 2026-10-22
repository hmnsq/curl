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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "headers.h"
#अन्तर्भाव "strcase.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
स्थिर शून्य copy_header_external(संरचना Curl_header_store *hs,
                                 size_t index,
                                 size_t amount,
                                 संरचना Curl_llist_गाठ *e,
                                 संरचना curl_header *hout)
{
  संरचना curl_header *h = hout;
  h->name = hs->name;
  h->मूल्य = hs->मूल्य;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (unsigned पूर्णाङ्क)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           ध्रुव वर्ण *name,
                           size_t nameindex,
                           unsigned पूर्णाङ्क origin,
                           पूर्णाङ्क request,
                           संरचना curl_header **hout)
{
  संरचना Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *e_pick = रिक्त;
    size_t match = 0;
    size_t amount = 0;
    संरचना Curl_header_store *hs = रिक्त;
    संरचना Curl_header_store *pick = रिक्त;
    यदि(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      गच्छ out;
    }
    यदि(!Curl_llist_मोजणी(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      गच्छ out;
    }
    यदि(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      गच्छ out;
    }
    यदि(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    प्रति(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_गाठ_next(e)) {
      hs = Curl_गाठ_elem(e);
      यदि(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    यदि(!amount)
      hresult = CURLHE_MISSING;
    अन्यथा यदि(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    यदि(hresult)
      गच्छ out;

    यदि(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    अन्यथा {
      प्रति(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_गाठ_next(e)) {
        hs = Curl_गाठ_elem(e);
        यदि(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          भङ्ग;
        }
      }
      यदि(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        गच्छ out;
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
  यदि(result)
    hresult = Curl_eapi_hcode(result);
  प्रत्यागम hresult;
}

संरचना nextheader_cache_entry {
  संरचना Curl_header_store *header;
  size_t order;
};

स्थिर पूर्णाङ्क nextheader_compare(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
  ध्रुव संरचना nextheader_cache_entry *e1 = p1;
  ध्रुव संरचना nextheader_cache_entry *e2 = p2;
  ध्रुव संरचना Curl_header_store *h1 = e1->header;
  ध्रुव संरचना Curl_header_store *h2 = e2->header;
  ध्रुव वर्ण *n1 = h1->name;
  ध्रुव वर्ण *n2 = h2->name;

  यावत्(*n1 && *n2) {
    unsigned वर्ण c1 = (unsigned वर्ण)Curl_raw_toupper(*n1++);
    unsigned वर्ण c2 = (unsigned वर्ण)Curl_raw_toupper(*n2++);
    यदि(c1 != c2)
      प्रत्यागम c1 > c2 ? 1 : -1;
  }
  यदि(*n1)
    प्रत्यागम 1;
  यदि(*n2)
    प्रत्यागम -1;
  यदि(e1->order != e2->order)
    प्रत्यागम e1->order > e2->order ? 1 : -1;
  प्रत्यागम 0;
}

स्थिर bool nextheader_cache_build(संरचना Curl_easy *data,
                                   unsigned पूर्णाङ्क origin,
                                   पूर्णाङ्क request, size_t मोजणी)
{
  संरचना nextheader_cache_entry *headers;
  संरचना Curl_llist_गाठ *e;
  size_t i = 0;
  size_t first;

  headers = curlx_malloc(परिमाण(*headers) * मोजणी);
  यदि(!headers)
    प्रत्यागम FALSE;

  प्रति(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_गाठ_next(e)) {
    संरचना Curl_header_store *hs = Curl_गाठ_elem(e);
    यदि((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  qsort(headers, i, परिमाण(*headers), nextheader_compare);
  प्रति(first = 0; first < i;) {
    size_t last = first + 1;
    size_t index;
    यावत्((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    प्रति(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_मोजणी = मोजणी;
  प्रत्यागम TRUE;
}

/* public API */
संरचना curl_header *curl_easy_nextheader(CURL *curl,
                                         unsigned पूर्णाङ्क origin,
                                         पूर्णाङ्क request,
                                         संरचना curl_header *prev)
{
  संरचना Curl_eapi_guard guard;
  संरचना curl_header *hd = रिक्त;
  CURLcode result;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_llist_गाठ *pick;
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_header_store *hs;
    size_t amount = 0;
    size_t index = 0;
    size_t मोजणी;

    यदि(request > data->state.requests)
      गच्छ out;
    यदि(request == -1)
      request = data->state.requests;

    यदि(prev) {
      pick = prev->anchor;
      यदि(!pick)
        /* something is wrong */
        गच्छ out;
      pick = Curl_गाठ_next(pick);
    }
    अन्यथा
      pick = Curl_llist_head(&data->state.httphdrs);

    यदि(pick) {
      /* make sure it is the next header of the desired type */
      कुरु {
        hs = Curl_गाठ_elem(pick);
        यदि((hs->type & origin) && (hs->request == request))
          भङ्ग;
        pick = Curl_गाठ_next(pick);
      } यावत्(pick);
    }

    यदि(!pick)
      /* no more headers available */
      गच्छ out;

    hs = Curl_गाठ_elem(pick);
    मोजणी = Curl_llist_मोजणी(&data->state.httphdrs);

    यदि(((data->state.nh_मोजणी == मोजणी) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, मोजणी)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    अन्यथा {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      प्रति(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_गाठ_next(e)) {
        संरचना Curl_header_store *check = Curl_गाठ_elem(e);
        यदि(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        यदि(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम hd;
}

स्थिर CURLcode namevalue(वर्ण *header, size_t hlen, unsigned पूर्णाङ्क type,
                          वर्ण **name, वर्ण **मूल्य)
{
  वर्ण *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  यदि(type == CURLH_PSEUDO) {
    यदि(*header != ':')
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  यावत्(*header && (*header != ':'))
    ++header;

  यदि(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  अन्यथा
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  यावत्(ISBLANK(*header))
    header++;

  *मूल्य = header;

  /* skip all trailing space letters */
  यावत्((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ढकला(संरचना Curl_easy *data, ध्रुव वर्ण *header,
                           size_t hlen, /* length of header */
                           unsigned वर्ण type)
{
  वर्ण *मूल्य = रिक्त;
  वर्ण *name = रिक्त;
  संरचना Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ध्रुव size_t ilen = hlen;

  यदि((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    प्रत्यागम CURLE_OK;

  /* trim off newline characters */
  यदि(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  यदि(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  यदि(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;

  यदि(ISBLANK(header[0])) {
    /* pass leading blanks */
    यावत्(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    यदि(!hlen)
      प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }
  यदि(Curl_llist_मोजणी(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_मोजणी) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_मोजणी);
    प्रत्यागम CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, परिमाण(*hs) + hlen);
  यदि(!hs)
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  memcpy(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &मूल्य);
  यदि(!result) {
    hs->name = name;
    hs->मूल्य = मूल्य;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->गाठ);
  }
  अन्यथा {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  प्रत्यागम result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
स्थिर शून्य headers_reset(संरचना Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, रिक्त);
  data->state.nh_मोजणी = 0;
}

संरचना hds_cw_collect_ctx {
  संरचना Curl_cwriter super;
};

स्थिर CURLcode hds_cw_collect_write(संरचना Curl_easy *data,
                                     संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                     ध्रुव वर्ण *buf, size_t blen)
{
  यदि((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    unsigned वर्ण htype = (unsigned वर्ण)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_ढकला(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (पूर्णाङ्क)result);
    यदि(result)
      प्रत्यागम result;
  }
  प्रत्यागम Curl_cwriter_write(data, writer->next, type, buf, blen);
}

स्थिर ध्रुव संरचना Curl_cwtype hds_cw_collect = {
  "hds-collect",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  परिमाण(संरचना hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(संरचना Curl_easy *data)
{
  संरचना Curl_cwriter *writer;
  CURLcode result;

  यदि(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    यदि(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      प्रत्यागम CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    यदि(result)
      प्रत्यागम result;

    result = Curl_cwriter_add(data, writer);
    यदि(result) {
      Curl_cwriter_free(data, writer);
      प्रत्यागम result;
    }
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(संरचना Curl_easy *data)
{
  संरचना Curl_llist_गाठ *e;
  संरचना Curl_llist_गाठ *n;

  प्रति(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    संरचना Curl_header_store *hs = Curl_गाठ_elem(e);
    n = Curl_गाठ_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  प्रत्यागम CURLE_OK;
}

#अन्यथा /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           ध्रुव वर्ण *name,
                           size_t index,
                           unsigned पूर्णाङ्क origin,
                           पूर्णाङ्क request,
                           संरचना curl_header **hout)
{
  (शून्य)easy;
  (शून्य)name;
  (शून्य)index;
  (शून्य)origin;
  (शून्य)request;
  (शून्य)hout;
  प्रत्यागम CURLHE_NOT_BUILT_IN;
}

संरचना curl_header *curl_easy_nextheader(CURL *easy,
                                         unsigned पूर्णाङ्क type,
                                         पूर्णाङ्क request,
                                         संरचना curl_header *prev)
{
  (शून्य)easy;
  (शून्य)type;
  (शून्य)request;
  (शून्य)prev;
  प्रत्यागम रिक्त;
}
#endif
