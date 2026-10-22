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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "headers.h"
#समावेश "strcase.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
अटल व्योम copy_header_external(काष्ठा Curl_header_store *hs,
                                 माप_प्रकार सूचकांक,
                                 माप_प्रकार amount,
                                 काष्ठा Curl_llist_गाँठ *e,
                                 काष्ठा curl_header *hout)
{
  काष्ठा curl_header *h = hout;
  h->name = hs->name;
  h->मूल्य = hs->मूल्य;
  h->amount = amount;
  h->सूचकांक = सूचकांक;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (अचिन्हित पूर्णांक)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_header(CURL *curl,
                           स्थिर अक्षर *name,
                           माप_प्रकार nameindex,
                           अचिन्हित पूर्णांक origin,
                           पूर्णांक request,
                           काष्ठा curl_header **hout)
{
  काष्ठा Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_header, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_llist_गाँठ *e_pick = शून्य;
    माप_प्रकार match = 0;
    माप_प्रकार amount = 0;
    काष्ठा Curl_header_store *hs = शून्य;
    काष्ठा Curl_header_store *pick = शून्य;
    अगर(!name || !hout || !तथ्य ||
       (origin > (CURLH_HEADER | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ARGUMENT;
      जाओ out;
    }
    अगर(!Curl_llist_गिनती(&तथ्य->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      जाओ out;
    }
    अगर(request > तथ्य->state.requests) {
      hresult = CURLHE_NOREQUEST;
      जाओ out;
    }
    अगर(request == -1)
      request = तथ्य->state.requests;

    /* we need a first round to count amount of this header */
    क्रम(e = Curl_llist_शीर्ष(&तथ्य->state.httphdrs); e; e = Curl_गाँठ_अग्र(e)) {
      hs = Curl_गाँठ_elem(e);
      अगर(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    अगर(!amount)
      hresult = CURLHE_MISSING;
    अन्यथा अगर(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    अगर(hresult)
      जाओ out;

    अगर(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    अन्यथा {
      क्रम(e = Curl_llist_शीर्ष(&तथ्य->state.httphdrs); e;
          e = Curl_गाँठ_अग्र(e)) {
        hs = Curl_गाँठ_elem(e);
        अगर(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (match++ == nameindex)) {
          e_pick = e;
          अवरोध;
        }
      }
      अगर(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        जाओ out;
      }
    }
    /* this is the name we want */
    copy_header_external(hs, nameindex, amount, e_pick,
                         &तथ्य->state.headerout[0]);
    *hout = &तथ्य->state.headerout[0];
    hresult = CURLHE_OK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  अगर(परिणाम)
    hresult = Curl_eapi_hcode(परिणाम);
  वापस hresult;
}

काष्ठा nextheader_cache_entry {
  काष्ठा Curl_header_store *header;
  माप_प्रकार order;
};

अटल पूर्णांक nextheader_compare(स्थिर व्योम *p1, स्थिर व्योम *p2)
{
  स्थिर काष्ठा nextheader_cache_entry *e1 = p1;
  स्थिर काष्ठा nextheader_cache_entry *e2 = p2;
  स्थिर काष्ठा Curl_header_store *h1 = e1->header;
  स्थिर काष्ठा Curl_header_store *h2 = e2->header;
  स्थिर अक्षर *n1 = h1->name;
  स्थिर अक्षर *n2 = h2->name;

  जबतक(*n1 && *n2) {
    अचिन्हित अक्षर c1 = (अचिन्हित अक्षर)Curl_raw_toupper(*n1++);
    अचिन्हित अक्षर c2 = (अचिन्हित अक्षर)Curl_raw_toupper(*n2++);
    अगर(c1 != c2)
      वापस c1 > c2 ? 1 : -1;
  }
  अगर(*n1)
    वापस 1;
  अगर(*n2)
    वापस -1;
  अगर(e1->order != e2->order)
    वापस e1->order > e2->order ? 1 : -1;
  वापस 0;
}

अटल bool nextheader_cache_build(काष्ठा Curl_easy *तथ्य,
                                   अचिन्हित पूर्णांक origin,
                                   पूर्णांक request, माप_प्रकार गिनती)
{
  काष्ठा nextheader_cache_entry *headers;
  काष्ठा Curl_llist_गाँठ *e;
  माप_प्रकार i = 0;
  माप_प्रकार first;

  headers = curlx_malloc(माप(*headers) * गिनती);
  अगर(!headers)
    वापस FALSE;

  क्रम(e = Curl_llist_शीर्ष(&तथ्य->state.httphdrs); e;
      e = Curl_गाँठ_अग्र(e)) {
    काष्ठा Curl_header_store *hs = Curl_गाँठ_elem(e);
    अगर((hs->type & origin) && (hs->request == request)) {
      headers[i].header = hs;
      headers[i].order = i;
      i++;
    }
  }

  क्विक(headers, i, माप(*headers), nextheader_compare);
  क्रम(first = 0; first < i;) {
    माप_प्रकार last = first + 1;
    माप_प्रकार सूचकांक;
    जबतक((last < i) &&
          curl_strequal(headers[first].header->name,
                        headers[last].header->name))
      last++;
    क्रम(सूचकांक = first; सूचकांक < last; सूचकांक++) {
      headers[सूचकांक].header->nh_amount = last - first;
      headers[सूचकांक].header->nh_सूचकांक = सूचकांक - first;
    }
    first = last;
  }
  curlx_free(headers);

  तथ्य->state.nh_origin = origin;
  तथ्य->state.nh_request = request;
  तथ्य->state.nh_गिनती = गिनती;
  वापस TRUE;
}

/* public API */
काष्ठा curl_header *curl_easy_nextheader(CURL *curl,
                                         अचिन्हित पूर्णांक origin,
                                         पूर्णांक request,
                                         काष्ठा curl_header *पूर्व)
{
  काष्ठा Curl_eapi_guard guard;
  काष्ठा curl_header *hd = शून्य;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    काष्ठा Curl_llist_गाँठ *pick;
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_header_store *hs;
    माप_प्रकार amount = 0;
    माप_प्रकार सूचकांक = 0;
    माप_प्रकार गिनती;

    अगर(request > तथ्य->state.requests)
      जाओ out;
    अगर(request == -1)
      request = तथ्य->state.requests;

    अगर(पूर्व) {
      pick = पूर्व->anchor;
      अगर(!pick)
        /* something is wrong */
        जाओ out;
      pick = Curl_गाँठ_अग्र(pick);
    }
    अन्यथा
      pick = Curl_llist_शीर्ष(&तथ्य->state.httphdrs);

    अगर(pick) {
      /* make sure it is the next header of the desired type */
      करो {
        hs = Curl_गाँठ_elem(pick);
        अगर((hs->type & origin) && (hs->request == request))
          अवरोध;
        pick = Curl_गाँठ_अग्र(pick);
      } जबतक(pick);
    }

    अगर(!pick)
      /* no more headers available */
      जाओ out;

    hs = Curl_गाँठ_elem(pick);
    गिनती = Curl_llist_गिनती(&तथ्य->state.httphdrs);

    अगर(((तथ्य->state.nh_गिनती == गिनती) &&
        (तथ्य->state.nh_origin == origin) &&
        (तथ्य->state.nh_request == request)) ||
       nextheader_cache_build(तथ्य, origin, request, गिनती)) {
      amount = hs->nh_amount;
      सूचकांक = hs->nh_सूचकांक;
    }
    अन्यथा {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      क्रम(e = Curl_llist_शीर्ष(&तथ्य->state.httphdrs); e;
          e = Curl_गाँठ_अग्र(e)) {
        काष्ठा Curl_header_store *check = Curl_गाँठ_elem(e);
        अगर(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        अगर(e == pick)
          सूचकांक = amount - 1;
      }
    }
    copy_header_external(hs, सूचकांक, amount, pick,
                         &तथ्य->state.headerout[1]);
    hd = &तथ्य->state.headerout[1];
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस hd;
}

अटल CURLcode namevalue(अक्षर *header, माप_प्रकार hlen, अचिन्हित पूर्णांक type,
                          अक्षर **name, अक्षर **मूल्य)
{
  अक्षर *end = header + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = header;

  अगर(type == CURLH_PSEUDO) {
    अगर(*header != ':')
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    header++;
  }

  /* Find the end of the header name */
  जबतक(*header && (*header != ':'))
    ++header;

  अगर(*header)
    /* Skip over colon, null it */
    *header++ = 0;
  अन्यथा
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* skip all leading blank letters */
  जबतक(ISBLANK(*header))
    header++;

  *मूल्य = header;

  /* skip all trailing space letters */
  जबतक((end > header) && ISBLANK(*end))
    *end-- = 0; /* null-terminate */
  वापस CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_headers_धकेलो(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *header,
                           माप_प्रकार hlen, /* length of header */
                           अचिन्हित अक्षर type)
{
  अक्षर *मूल्य = शून्य;
  अक्षर *name = शून्य;
  काष्ठा Curl_header_store *hs;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  स्थिर माप_प्रकार ilen = hlen;

  अगर((header[0] == '\r') || (header[0] == '\n'))
    /* ignore the body separator */
    वापस CURLE_OK;

  /* trim off newline characters */
  अगर(hlen && (header[hlen - 1] == '\n'))
    hlen--;
  अगर(hlen && (header[hlen - 1] == '\r'))
    hlen--;
  अगर(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    वापस CURLE_WEIRD_SERVER_REPLY;

  अगर(ISBLANK(header[0])) {
    /* pass leading blanks */
    जबतक(hlen && ISBLANK(*header)) {
      header++;
      hlen--;
    }
    अगर(!hlen)
      वापस CURLE_WEIRD_SERVER_REPLY;
  }
  अगर(Curl_llist_गिनती(&तथ्य->state.httphdrs) >= MAX_HTTP_RESP_HEADER_गिनती) {
    failf(तथ्य, "Too many response headers, %d is max",
          MAX_HTTP_RESP_HEADER_गिनती);
    वापस CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, माप(*hs) + hlen);
  अगर(!hs)
    वापस CURLE_OUT_OF_MEMORY;
  स_नकल(hs->बफर, header, hlen);
  hs->बफर[hlen] = 0; /* null-terminate */

  परिणाम = namevalue(hs->बफर, hlen, type, &name, &मूल्य);
  अगर(!परिणाम) {
    hs->name = name;
    hs->मूल्य = मूल्य;
    hs->type = type;
    hs->request = तथ्य->state.requests;

    /* insert this node into the list of headers */
    Curl_llist_append(&तथ्य->state.httphdrs, hs, &hs->गाँठ);
  }
  अन्यथा {
    failf(तथ्य, "Invalid response header");
    curlx_free(hs);
  }
  वापस परिणाम;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
अटल व्योम headers_reset(काष्ठा Curl_easy *तथ्य)
{
  Curl_llist_init(&तथ्य->state.httphdrs, शून्य);
  तथ्य->state.nh_गिनती = 0;
}

काष्ठा hds_cw_collect_ctx {
  काष्ठा Curl_cwriter super;
};

अटल CURLcode hds_cw_collect_write(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_cwriter *writer, पूर्णांक type,
                                     स्थिर अक्षर *buf, माप_प्रकार blen)
{
  अगर((type & CLIENTWRITE_HEADER) && !(type & CLIENTWRITE_STATUS)) {
    अचिन्हित अक्षर htype = (अचिन्हित अक्षर)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_HEADER)));
    CURLcode परिणाम = Curl_headers_धकेलो(तथ्य, buf, blen, htype);
    CURL_TRC_WRITE(तथ्य, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (पूर्णांक)परिणाम);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस Curl_cwriter_write(तथ्य, writer->अग्र, type, buf, blen);
}

अटल स्थिर काष्ठा Curl_cwtype hds_cw_collect = {
  "hds-collect",
  शून्य,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  माप(काष्ठा hds_cw_collect_ctx)
};

CURLcode Curl_headers_init(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cwriter *writer;
  CURLcode परिणाम;

  अगर(तथ्य->conn && (तथ्य->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    अगर(Curl_cwriter_get_by_name(तथ्य, hds_cw_collect.name))
      वापस CURLE_OK;

    परिणाम = Curl_cwriter_create(&writer, तथ्य, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    अगर(परिणाम)
      वापस परिणाम;

    परिणाम = Curl_cwriter_add(तथ्य, writer);
    अगर(परिणाम) {
      Curl_cwriter_free(तथ्य, writer);
      वापस परिणाम;
    }
  }
  वापस CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_llist_गाँठ *e;
  काष्ठा Curl_llist_गाँठ *n;

  क्रम(e = Curl_llist_शीर्ष(&तथ्य->state.httphdrs); e; e = n) {
    काष्ठा Curl_header_store *hs = Curl_गाँठ_elem(e);
    n = Curl_गाँठ_अग्र(e);
    curlx_free(hs);
  }
  headers_reset(तथ्य);
  वापस CURLE_OK;
}

#अन्यथा /* HTTP-disabled builds below */

CURLHcode curl_easy_header(CURL *easy,
                           स्थिर अक्षर *name,
                           माप_प्रकार सूचकांक,
                           अचिन्हित पूर्णांक origin,
                           पूर्णांक request,
                           काष्ठा curl_header **hout)
{
  (व्योम)easy;
  (व्योम)name;
  (व्योम)सूचकांक;
  (व्योम)origin;
  (व्योम)request;
  (व्योम)hout;
  वापस CURLHE_NOT_BUILT_IN;
}

काष्ठा curl_header *curl_easy_nextheader(CURL *easy,
                                         अचिन्हित पूर्णांक type,
                                         पूर्णांक request,
                                         काष्ठा curl_header *पूर्व)
{
  (व्योम)easy;
  (व्योम)type;
  (व्योम)request;
  (व्योम)पूर्व;
  वापस शून्य;
}
#पूर्ण_अगर
