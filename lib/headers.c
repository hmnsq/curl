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

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
स्थिर शून्य copy_header_external(संरचना Curl_header_store *hs,
                                 माप_प्रकार index,
                                 माप_प्रकार amount,
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
  h->origin = (अचिन्हित पूर्णांक)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           स्थिरांक अक्षर *name,
                           माप_प्रकार nameindex,
                           अचिन्हित पूर्णांक origin,
                           पूर्णांक request,
                           संरचना curl_header **hout)
{
  संरचना Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode result;

  जर(CURL_EAPI_ENTER(&guard, curl, easy_header, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *e_pick = रिक्त;
    माप_प्रकार match = 0;
    माप_प्रकार amount = 0;
    संरचना Curl_header_store *hs = रिक्त;
    संरचना Curl_header_store *pick = रिक्त;
    जर(!name || !hout || !data ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      जा out;
    }
    जर(!Curl_llist_मोजणी(&data->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      जा out;
    }
    जर(request > data->state.requests) {
      hresult = CURLHE_NOREQUEST;
      जा out;
    }
    जर(request == -1)
      request = data->state.requests;

    /* we need a first round to count amount of this header */
    साठी(e = Curl_llist_head(&data->state.httphdrs); e; e = Curl_गाठ_next(e)) {
      hs = Curl_गाठ_elem(e);
      जर(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    जर(!amount)
      hresult = CURLHE_MISSING;
    अन्यथा जर(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    जर(hresult)
      जा out;

    जर(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    अन्यथा {
      साठी(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_गाठ_next(e)) {
        hs = Curl_गाठ_elem(e);
        जर(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          थांबा;
        }
      }
      जर(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        जा out;
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
  जर(result)
    hresult = Curl_eapi_hcode(result);
  परत hresult;
}

संरचना nextheader_cache_entry {
  संरचना Curl_header_store *header;
  माप_प्रकार order;
};

स्थिर पूर्णांक nextheader_compare(स्थिरांक शून्य *p1, स्थिरांक शून्य *p2)
{
  स्थिरांक संरचना nextheader_cache_entry *e1 = p1;
  स्थिरांक संरचना nextheader_cache_entry *e2 = p2;
  स्थिरांक संरचना Curl_header_store *h1 = e1->header;
  स्थिरांक संरचना Curl_header_store *h2 = e2->header;
  स्थिरांक अक्षर *n1 = h1->name;
  स्थिरांक अक्षर *n2 = h2->name;

  पर्यंत(*n1 && *n2) {
    अचिन्हित अक्षर c1 = (अचिन्हित अक्षर)Curl_raw_toupper(*n1++);
    अचिन्हित अक्षर c2 = (अचिन्हित अक्षर)Curl_raw_toupper(*n2++);
    जर(c1 != c2)
      परत c1 > c2 ? 1 : -1;
  }
  जर(*n1)
    परत 1;
  जर(*n2)
    परत -1;
  जर(e1->order != e2->order)
    परत e1->order > e2->order ? 1 : -1;
  परत 0;
}

स्थिर bool nextheader_cache_build(संरचना Curl_easy *data,
                                   अचिन्हित पूर्णांक origin,
                                   पूर्णांक request, माप_प्रकार मोजणी)
{
  संरचना nextheader_cache_entry *headers;
  संरचना Curl_llist_गाठ *e;
  माप_प्रकार i = 0;
  माप_प्रकार first;

  headers = curlx_malloc(आकार(*headers) * मोजणी);
  जर(!headers)
    परत FALSE;

  साठी(e = Curl_llist_head(&data->state.httphdrs); e;
      e = Curl_गाठ_next(e)) {
    संरचना Curl_header_store *hs = Curl_गाठ_elem(e);
    जर((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  क्विक(headers, i, आकार(*headers), nextheader_compare);
  साठी(first = 0; first < i;) {
    माप_प्रकार last = first + 1;
    माप_प्रकार index;
    पर्यंत((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    साठी(index = first; index < last; index++) {
      headers[index].header->nh_amount = last - first;
      headers[index].header->nh_index = index - first;
    }
    first = last;
  }
  curlx_free(headers);

  data->state.nh_origin = origin;
  data->state.nh_request = request;
  data->state.nh_मोजणी = मोजणी;
  परत TRUE;
}

/* public API */
संरचना curl_header *curl_easy_nextheader(CURL *curl,
                                         अचिन्हित पूर्णांक origin,
                                         पूर्णांक request,
                                         संरचना curl_header *prev)
{
  संरचना Curl_eapi_guard guard;
  संरचना curl_header *hd = रिक्त;
  CURLcode result;

  जर(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &result)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_llist_गाठ *pick;
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_header_store *hs;
    माप_प्रकार amount = 0;
    माप_प्रकार index = 0;
    माप_प्रकार मोजणी;

    जर(request > data->state.requests)
      जा out;
    जर(request == -1)
      request = data->state.requests;

    जर(prev) {
      pick = prev->anchor;
      जर(!pick)
        /* something is wrong */
        जा out;
      pick = Curl_गाठ_next(pick);
    }
    अन्यथा
      pick = Curl_llist_head(&data->state.httphdrs);

    जर(pick) {
      /* make sure it is the next header of the desired type */
      करा {
        hs = Curl_गाठ_elem(pick);
        जर((hs->type & origin) && (hs->request == request))
          थांबा;
        pick = Curl_गाठ_next(pick);
      } पर्यंत(pick);
    }

    जर(!pick)
      /* no more headers available */
      जा out;

    hs = Curl_गाठ_elem(pick);
    मोजणी = Curl_llist_मोजणी(&data->state.httphdrs);

    जर(((data->state.nh_मोजणी == मोजणी) &&
        (data->state.nh_origin == origin) &&
        (data->state.nh_request == request)) ||
       nextheader_cache_build(data, origin, request, मोजणी)) {
      amount = hs->nh_amount;
      index = hs->nh_index;
    }
    अन्यथा {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      साठी(e = Curl_llist_head(&data->state.httphdrs); e;
          e = Curl_गाठ_next(e)) {
        संरचना Curl_header_store *check = Curl_गाठ_elem(e);
        जर(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        जर(e == pick)
          index = amount - 1;
      }
    }
    copy_header_external(hs, index, amount, pick,
                         &data->state.headerout[1]);
    hd = &data->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  परत hd;
}

स्थिर CURLcode namevalue(अक्षर *header, माप_प्रकार hlen, अचिन्हित पूर्णांक type,
                          अक्षर **name, अक्षर **मूल्य)
{
  अक्षर *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  जर(type == CURLH_PSEUDO) {
    जर(*header != ':')
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  पर्यंत(*header && (*header != ':'))
    ++header;

  जर(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  अन्यथा
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  पर्यंत(ISBLANK(*header))
    header++;

  *मूल्य = header;

  /* skip all trailing space letters */
  पर्यंत((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  परत CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_ढकला(संरचना Curl_easy *data, स्थिरांक अक्षर *header,
                           माप_प्रकार hlen, /* length of header */
                           अचिन्हित अक्षर type)
{
  अक्षर *मूल्य = रिक्त;
  अक्षर *name = रिक्त;
  संरचना Curl_header_store *hs;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  स्थिरांक माप_प्रकार ilen = hlen;

  जर((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    परत CURLE_OK;

  /* trim off newline characters */
  जर(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  जर(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  जर(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    परत CURLE_WEIRD_SERVER_REPLY;

  जर(ISBLANK(header[0])) {
    /* pass leading blanks */
    पर्यंत(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    जर(!hlen)
      परत CURLE_WEIRD_SERVER_REPLY;
  }
  जर(Curl_llist_मोजणी(&data->state.httphdrs) >= MAX_HTTP_RESP_HEADER_मोजणी) {
    failf(data, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_मोजणी);
    परत CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, आकार(*hs) + hlen);
  जर(!hs)
    परत CURLE_OUT_OF_MEMORY;
  स_नकल(hs->buffer, header, hlen);
  hs->buffer[hlen] = 0; /* null-terminate */

  result = namevalue(hs->buffer, hlen, type, &name, &मूल्य);
  जर(!result) {
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
  परत result;
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
                                     संरचना Curl_cwriter *writer, पूर्णांक type,
                                     स्थिरांक अक्षर *buf, माप_प्रकार blen)
{
  जर((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    अचिन्हित अक्षर htype = (अचिन्हित अक्षर)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode result = Curl_headers_ढकला(data, buf, blen, htype);
    CURL_TRC_WRITE(data, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (पूर्णांक)result);
    जर(result)
      परत result;
  }
  परत Curl_cwriter_write(data, writer->next, type, buf, blen);
}

स्थिर स्थिरांक संरचना Curl_cwtype hds_cw_collect = {
  "hds-collect",
  रिक्त,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  आकार(संरचना hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(संरचना Curl_easy *data)
{
  संरचना Curl_cwriter *writer;
  CURLcode result;

  जर(data->conn && (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    जर(Curl_cwriter_get_by_name(data, hds_cw_collect.name))
      परत CURLE_OK;

    result = Curl_cwriter_create(&writer, data, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    जर(result)
      परत result;

    result = Curl_cwriter_add(data, writer);
    जर(result) {
      Curl_cwriter_free(data, writer);
      परत result;
    }
  }
  परत CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(संरचना Curl_easy *data)
{
  संरचना Curl_llist_गाठ *e;
  संरचना Curl_llist_गाठ *n;

  साठी(e = Curl_llist_head(&data->state.httphdrs); e; e = n) {
    संरचना Curl_header_store *hs = Curl_गाठ_elem(e);
    n = Curl_गाठ_next(e);
    curlx_free(hs);
  }
  headers_reset(data);
  परत CURLE_OK;
}

#अन्यथा /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           स्थिरांक अक्षर *name,
                           माप_प्रकार index,
                           अचिन्हित पूर्णांक origin,
                           पूर्णांक request,
                           संरचना curl_header **hout)
{
  (शून्य)easy;
  (शून्य)name;
  (शून्य)index;
  (शून्य)origin;
  (शून्य)request;
  (शून्य)hout;
  परत CURLHE_NOT_BUILT_IN;
}

संरचना curl_header *curl_easy_nextheader(CURL *easy,
                                         अचिन्हित पूर्णांक type,
                                         पूर्णांक request,
                                         संरचना curl_header *prev)
{
  (शून्य)easy;
  (शून्य)type;
  (शून्य)request;
  (शून्य)prev;
  परत रिक्त;
}
#endif
