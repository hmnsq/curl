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
                                 माप_प्रकार index,
                                 माप_प्रकार amount,
                                 संरचना Curl_llist_गाँठो *e,
                                 संरचना curl_header *hout)
{
  संरचना curl_header *h = hout;
  h->name = hs->name;
  h->मान = hs->मान;
  h->amount = amount;
  h->index = index;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (अचिन्हित पूर्णाङ्क)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           अचर वर्ण *name,
                           माप_प्रकार nameindex,
                           अचिन्हित पूर्णाङ्क origin,
                           पूर्णाङ्क request,
                           संरचना curl_header **hout)
{
  संरचना Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_llist_गाँठो *e;
    संरचना Curl_llist_गाँठो *e_pick = रिक्त;
    माप_प्रकार match = 0;
    माप_प्रकार amount = 0;
    संरचना Curl_header_store *hs = रिक्त;
    संरचना Curl_header_store *pick = रिक्त;
    यदि(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      गच्छ out;
    }
    यदि(!Curl_llist_गन्ती(&data->state.httphdrs)) {
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
    लागि(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_गाँठो_next(e)) {
      hs = Curl_गाँठो_elem(e);
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
      लागि(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_गाँठो_next(e)) {
        hs = Curl_गाँठो_elem(e);
        यदि(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          रोक;
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
  फर्काऊ hresult;
}

संरचना nextheader_cache_entry {
  संरचना Curl_header_store *header;
  माप_प्रकार order;
};

स्थिर पूर्णाङ्क nextheader_compare(अचर शून्य *p1, अचर शून्य *p2)
{
  अचर संरचना nextheader_cache_entry *e1 = p1;
  अचर संरचना nextheader_cache_entry *e2 = p2;
  अचर संरचना Curl_header_store *h1 = e1->header;
  अचर संरचना Curl_header_store *h2 = e2->header;
  अचर वर्ण *n1 = h1->name;
  अचर वर्ण *n2 = h2->name;

  जबसम्म(*n1 && *n2) {
    अचिन्हित वर्ण c1 = (अचिन्हित वर्ण)Curl_raw_toupper(*n1++);
    अचिन्हित वर्ण c2 = (अचिन्हित वर्ण)Curl_raw_toupper(*n2++);
    यदि(c1 != c2)
      फर्काऊ c1 > c2 ? 1 : -1;
  }
  यदि(*n1)
    फर्काऊ 1;
  यदि(*n2)
    फर्काऊ -1;
  यदि(e1->order != e2->order)
    फर्काऊ e1->order > e2->order ? 1 : -1;
  फर्काऊ 0;
}

स्थिर bool nextheader_cache_build(संरचना Curl_easy *data,
                                   अचिन्हित पूर्णाङ्क origin,
                                   पूर्णाङ्क request, माप_प्रकार गन्ती)
{
  संरचना nextheader_cache_entry *headers;
  संरचना Curl_llist_गाँठो *e;
  माप_प्रकार i = 0;
  माप_प्रकार first;

  headers = curlx_malloc(परिमाण(*headers) * गन्ती);
  यदि(!headers)
    फर्काऊ FALSE;

  लागि(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_गाँठो_next(e)) {
    संरचना Curl_header_store *hs = Curl_गाँठो_elem(e);
    यदि((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  क्विक(headers, i, परिमाण(*headers), nextheader_compare);
  लागि(first = 0; first < i;) {
    माप_प्रकार last = first + 1;
    माप_प्रकार index;
    जबसम्म((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    लागि(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_गन्ती = गन्ती;
  फर्काऊ TRUE;
}

/* public API */
संरचना curl_header *curl_easy_nextheader(CURL *curl,
                                         अचिन्हित पूर्णाङ्क origin,
                                         पूर्णाङ्क request,
                                         संरचना curl_header *prev)
{
  संरचना Curl_eapi_guard guard;
  संरचना curl_header *hd = रिक्त;
  CURLcode result;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_llist_गाँठो *pick;
    संरचना Curl_llist_गाँठो *e;
    संरचना Curl_header_store *hs;
    माप_प्रकार amount = 0;
    माप_प्रकार index = 0;
    माप_प्रकार गन्ती;

    यदि(request > data->state.requests)
      गच्छ out;
    यदि(request == -1)
      request = data->state.requests;

    यदि(prev) {
      pick = prev->anchor;
      यदि(!pick)
        /* something is wrong */
        गच्छ out;
      pick = Curl_गाँठो_next(pick);
    }
    अन्यथा
      pick = Curl_llist_head(&data->state.httphdrs);

    यदि(pick) {
      /* make sure it is the next header of the desired type */
      गर {
        hs = Curl_गाँठो_elem(pick);
        यदि((hs->type & origin) && (hs->request == request))
          रोक;
        pick = Curl_गाँठो_next(pick);
      } जबसम्म(pick);
    }

    यदि(!pick)
      /* no more headers available */
      गच्छ out;

    hs = Curl_गाँठो_elem(pick);
    गन्ती = Curl_llist_गन्ती(&data->state.httphdrs);

    यदि(((data->state.nh_गन्ती == गन्ती) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, गन्ती)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    अन्यथा {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      लागि(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_गाँठो_next(e)) {
        संरचना Curl_header_store *check = Curl_गाँठो_elem(e);
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
  फर्काऊ hd;
}

स्थिर CURLcode namevalue(वर्ण *header, माप_प्रकार hlen, अचिन्हित पूर्णाङ्क type,
                          वर्ण **name, वर्ण **मान)
{
  वर्ण *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  यदि(type == CURLH_PSEUDO) {
    यदि(*header != ':')
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  जबसम्म(*header && (*header != ':'))
    ++header;

  यदि(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  अन्यथा
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  जबसम्म(ISBLANK(*header))
    header++;

  *मान = header;

  /* skip all trailing space letters */
  जबसम्म((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  फर्काऊ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_थप(संरचना Curl_easy *data, अचर वर्ण *header,
                           माप_प्रकार hlen, /* length of header */
                           अचिन्हित वर्ण type)
{
  वर्ण *मान = रिक्त;
  वर्ण *name = रिक्त;
  संरचना Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  अचर माप_प्रकार ilen = hlen;

  यदि((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    फर्काऊ CURLE_OK;

  /* trim off newline characters */
  यदि(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  यदि(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  यदि(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    फर्काऊ CURLE_WEIRD_SERVER_REPLY;

  यदि(ISBLANK(header[0])) {
    /* pass leading blanks */
    जबसम्म(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    यदि(!hlen)
      फर्काऊ CURLE_WEIRD_SERVER_REPLY;
  }
  यदि(Curl_llist_गन्ती(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_गन्ती) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_गन्ती);
    फर्काऊ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, परिमाण(*hs) + hlen);
  यदि(!hs)
    फर्काऊ CURLE_OUT_OF_MEMORY;
  स_नकल(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &मान);
  यदि(!result) {
    hs->name = name;
    hs->मान = मान;
    hs->type = type;
    hs->request = data->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&data->state.httphdrs, hs, &hs->गाँठो);
  }
  अन्यथा {
    failf(data, "Invalid response header");
    curlx_free(hs);
  }
  फर्काऊ result;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
स्थिर शून्य headers_reset(संरचना Curl_easy *data)
{
  Curl_llist_init(&data->state.httphdrs, रिक्त);
  data->state.nh_गन्ती = 0;
}

संरचना hds_cw_collect_ctx {
  संरचना Curl_cwriter super;
};

स्थिर CURLcode hds_cw_collect_write(संरचना Curl_easy *data,
                                     संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                     अचर वर्ण *buf, माप_प्रकार blen)
{
  यदि((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    अचिन्हित वर्ण htype = (अचिन्हित वर्ण)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_थप(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (पूर्णाङ्क)result);
    यदि(result)
      फर्काऊ result;
  }
  फर्काऊ Curl_cwriter_write(data, writer->next, type, buf, blen);
}

स्थिर अचर संरचना Curl_cwtype hds_cw_collect = {
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
      फर्काऊ CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    यदि(result)
      फर्काऊ result;

    result = Curl_cwriter_add(data, writer);
    यदि(result) {
      Curl_cwriter_free(data, writer);
      फर्काऊ result;
    }
  }
  फर्काऊ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(संरचना Curl_easy *data)
{
  संरचना Curl_llist_गाँठो *e;
  संरचना Curl_llist_गाँठो *n;

  लागि(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    संरचना Curl_header_store *hs = Curl_गाँठो_elem(e);
    n = Curl_गाँठो_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  फर्काऊ CURLE_OK;
}

#अन्यथा /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           अचर वर्ण *name,
                           माप_प्रकार index,
                           अचिन्हित पूर्णाङ्क origin,
                           पूर्णाङ्क request,
                           संरचना curl_header **hout)
{
  (शून्य)easy;
  (शून्य)name;
  (शून्य)index;
  (शून्य)origin;
  (शून्य)request;
  (शून्य)hout;
  फर्काऊ CURLHE_NOT_BUILT_IN;
}

संरचना curl_header *curl_easy_nextheader(CURL *easy,
                                         अचिन्हित पूर्णाङ्क type,
                                         पूर्णाङ्क request,
                                         संरचना curl_header *prev)
{
  (शून्य)easy;
  (शून्य)type;
  (शून्य)request;
  (शून्य)prev;
  फर्काऊ रिक्त;
}
#endif
