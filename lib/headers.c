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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_शीर्षकाणि_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
स्थिर शून्य copy_शीर्षकम्_external(संरचना Curl_शीर्षकम्_store *hs,
                                 size_t सूचकांक,
                                 size_t amount,
                                 संरचना Curl_llist_ग्रन्थिः *e,
                                 संरचना curl_शीर्षकम् *hout)
{
  संरचना curl_शीर्षकम् *h = hout;
  h->name = hs->name;
  h->मानम् = hs->मानम्;
  h->amount = amount;
  h->सूचकांक = सूचकांक;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (अचिह्नित पूर्णाङ्क)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_शीर्षकम्(CURL *curl,
                           ध्रुव वर्ण *name,
                           size_t nameindex,
                           अचिह्नित पूर्णाङ्क origin,
                           पूर्णाङ्क request,
                           संरचना curl_शीर्षकम् **hout)
{
  संरचना Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_शीर्षकम्, &परिणाम)) {
    संरचना Curl_easy *दत्त = curl;
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *e_pick = रिक्त;
    size_t मेलनम् = 0;
    size_t amount = 0;
    संरचना Curl_शीर्षकम्_store *hs = रिक्त;
    संरचना Curl_शीर्षकम्_store *pick = रिक्त;
    यदि(!name || !hout || !दत्त ||
       (origin > (CURLH_शीर्षकम् | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_तर्कः;
      गच्छ बहिर्गमनम्;
    }
    यदि(!Curl_llist_गणनम्(&दत्त->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      गच्छ बहिर्गमनम्;
    }
    यदि(request > दत्त->state.अनुरोधाः) {
      hresult = CURLHE_NOREQUEST;
      गच्छ बहिर्गमनम्;
    }
    यदि(request == -1)
      request = दत्त->state.अनुरोधाः;

    /* we need a first round to count amount of this header */
    प्रति(e = Curl_llist_शीर्ष(&दत्त->state.httphdrs); e; e = Curl_ग्रन्थिः_अग्र(e)) {
      hs = Curl_ग्रन्थिः_elem(e);
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
      गच्छ बहिर्गमनम्;

    यदि(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    अन्यथा {
      प्रति(e = Curl_llist_शीर्ष(&दत्त->state.httphdrs); e;
          e = Curl_ग्रन्थिः_अग्र(e)) {
        hs = Curl_ग्रन्थिः_elem(e);
        यदि(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (मेलनम्++ == nameindex)) {
          e_pick = e;
          भङ्ग;
        }
      }
      यदि(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        गच्छ बहिर्गमनम्;
      }
    }
    /* this is the name we want */
    copy_शीर्षकम्_external(hs, nameindex, amount, e_pick,
                         &दत्त->state.headerout[0]);
    *hout = &दत्त->state.headerout[0];
    hresult = CURLHE_OK;
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  यदि(परिणाम)
    hresult = Curl_eapi_hcode(परिणाम);
  प्रत्यागम hresult;
}

संरचना nextheader_cache_प्रविष्टिः {
  संरचना Curl_शीर्षकम्_store *शीर्षकम्;
  size_t क्रमः;
};

स्थिर पूर्णाङ्क nextheader_compare(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
  ध्रुव संरचना nextheader_cache_प्रविष्टिः *e1 = p1;
  ध्रुव संरचना nextheader_cache_प्रविष्टिः *e2 = p2;
  ध्रुव संरचना Curl_शीर्षकम्_store *h1 = e1->शीर्षकम्;
  ध्रुव संरचना Curl_शीर्षकम्_store *h2 = e2->शीर्षकम्;
  ध्रुव वर्ण *n1 = h1->name;
  ध्रुव वर्ण *n2 = h2->name;

  यावत्(*n1 && *n2) {
    अचिह्नित वर्ण c1 = (अचिह्नित वर्ण)Curl_raw_toupper(*n1++);
    अचिह्नित वर्ण c2 = (अचिह्नित वर्ण)Curl_raw_toupper(*n2++);
    यदि(c1 != c2)
      प्रत्यागम c1 > c2 ? 1 : -1;
  }
  यदि(*n1)
    प्रत्यागम 1;
  यदि(*n2)
    प्रत्यागम -1;
  यदि(e1->क्रमः != e2->क्रमः)
    प्रत्यागम e1->क्रमः > e2->क्रमः ? 1 : -1;
  प्रत्यागम 0;
}

स्थिर bool nextheader_cache_build(संरचना Curl_easy *दत्त,
                                   अचिह्नित पूर्णाङ्क origin,
                                   पूर्णाङ्क request, size_t गणनम्)
{
  संरचना nextheader_cache_प्रविष्टिः *शीर्षकाणि;
  संरचना Curl_llist_ग्रन्थिः *e;
  size_t i = 0;
  size_t first;

  शीर्षकाणि = curlx_malloc(परिमाण(*शीर्षकाणि) * गणनम्);
  यदि(!शीर्षकाणि)
    प्रत्यागम FALSE;

  प्रति(e = Curl_llist_शीर्ष(&दत्त->state.httphdrs); e;
      e = Curl_ग्रन्थिः_अग्र(e)) {
    संरचना Curl_शीर्षकम्_store *hs = Curl_ग्रन्थिः_elem(e);
    यदि((hs->type & origin) && (hs->request == request)) {
      शीर्षकाणि[i].शीर्षकम् = hs;
      शीर्षकाणि[i].क्रमः = i;
      i++;
    }
  }

  शीघ्रं_क्रमय(शीर्षकाणि, i, परिमाण(*शीर्षकाणि), nextheader_compare);
  प्रति(first = 0; first < i;) {
    size_t चरमः = first + 1;
    size_t सूचकांक;
    यावत्((चरमः < i) &&
          curl_strequal(शीर्षकाणि[first].शीर्षकम्->name,
                        शीर्षकाणि[चरमः].शीर्षकम्->name))
      चरमः++;
    प्रति(सूचकांक = first; सूचकांक < चरमः; सूचकांक++) {
      शीर्षकाणि[सूचकांक].शीर्षकम्->nh_amount = चरमः - first;
      शीर्षकाणि[सूचकांक].शीर्षकम्->nh_सूचकांक = सूचकांक - first;
    }
    first = चरमः;
  }
  curlx_free(शीर्षकाणि);

  दत्त->state.nh_origin = origin;
  दत्त->state.nh_request = request;
  दत्त->state.nh_गणनम् = गणनम्;
  प्रत्यागम TRUE;
}

/* public API */
संरचना curl_शीर्षकम् *curl_easy_nextheader(CURL *curl,
                                         अचिह्नित पूर्णाङ्क origin,
                                         पूर्णाङ्क request,
                                         संरचना curl_शीर्षकम् *पूर्व)
{
  संरचना Curl_eapi_guard guard;
  संरचना curl_शीर्षकम् *hd = रिक्त;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &परिणाम)) {
    संरचना Curl_easy *दत्त = curl;
    संरचना Curl_llist_ग्रन्थिः *pick;
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_शीर्षकम्_store *hs;
    size_t amount = 0;
    size_t सूचकांक = 0;
    size_t गणनम्;

    यदि(request > दत्त->state.अनुरोधाः)
      गच्छ बहिर्गमनम्;
    यदि(request == -1)
      request = दत्त->state.अनुरोधाः;

    यदि(पूर्व) {
      pick = पूर्व->anchor;
      यदि(!pick)
        /* something is wrong */
        गच्छ बहिर्गमनम्;
      pick = Curl_ग्रन्थिः_अग्र(pick);
    }
    अन्यथा
      pick = Curl_llist_शीर्ष(&दत्त->state.httphdrs);

    यदि(pick) {
      /* make sure it is the next header of the desired type */
      कुरु {
        hs = Curl_ग्रन्थिः_elem(pick);
        यदि((hs->type & origin) && (hs->request == request))
          भङ्ग;
        pick = Curl_ग्रन्थिः_अग्र(pick);
      } यावत्(pick);
    }

    यदि(!pick)
      /* no more headers available */
      गच्छ बहिर्गमनम्;

    hs = Curl_ग्रन्थिः_elem(pick);
    गणनम् = Curl_llist_गणनम्(&दत्त->state.httphdrs);

    यदि(((दत्त->state.nh_गणनम् == गणनम्) &&
        (दत्त->state.nh_origin == origin) &&
        (दत्त->state.nh_request == request)) ||
       nextheader_cache_build(दत्त, origin, request, गणनम्)) {
      amount = hs->nh_amount;
      सूचकांक = hs->nh_सूचकांक;
    }
    अन्यथा {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      प्रति(e = Curl_llist_शीर्ष(&दत्त->state.httphdrs); e;
          e = Curl_ग्रन्थिः_अग्र(e)) {
        संरचना Curl_शीर्षकम्_store *check = Curl_ग्रन्थिः_elem(e);
        यदि(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        यदि(e == pick)
          सूचकांक = amount - 1;
      }
    }
    copy_शीर्षकम्_external(hs, सूचकांक, amount, pick,
                         &दत्त->state.headerout[1]);
    hd = &दत्त->state.headerout[1];
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम hd;
}

स्थिर CURLcode namevalue(वर्ण *शीर्षकम्, size_t hlen, अचिह्नित पूर्णाङ्क type,
                          वर्ण **name, वर्ण **मानम्)
{
  वर्ण *अन्तः = शीर्षकम् + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = शीर्षकम्;

  यदि(type == CURLH_PSEUDO) {
    यदि(*शीर्षकम् != ':')
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    शीर्षकम्++;
  }

  /* Find the end of the header name */
  यावत्(*शीर्षकम् && (*शीर्षकम् != ':'))
    ++शीर्षकम्;

  यदि(*शीर्षकम्)
    /* Skip over colon, null it */
    *शीर्षकम्++ = 0;
  अन्यथा
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* skip all leading blank letters */
  यावत्(ISBLANK(*शीर्षकम्))
    शीर्षकम्++;

  *मानम् = शीर्षकम्;

  /* skip all trailing space letters */
  यावत्((अन्तः > शीर्षकम्) && ISBLANK(*अन्तः))
    *अन्तः-- = 0; /* null-terminate */
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_शीर्षकाणि_प्रक्षिप(संरचना Curl_easy *दत्त, ध्रुव वर्ण *शीर्षकम्,
                           size_t hlen, /* length of header */
                           अचिह्नित वर्ण type)
{
  वर्ण *मानम् = रिक्त;
  वर्ण *name = रिक्त;
  संरचना Curl_शीर्षकम्_store *hs;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  ध्रुव size_t ilen = hlen;

  यदि((शीर्षकम्[0] == '\r') || (शीर्षकम्[0] == '\n'))
    /* ignore the body separator */
    प्रत्यागम CURLE_OK;

  /* trim off newline characters */
  यदि(hlen && (शीर्षकम्[hlen - 1] == '\n'))
    hlen--;
  यदि(hlen && (शीर्षकम्[hlen - 1] == '\r'))
    hlen--;
  यदि(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

  यदि(ISBLANK(शीर्षकम्[0])) {
    /* pass leading blanks */
    यावत्(hlen && ISBLANK(*शीर्षकम्)) {
      शीर्षकम्++;
      hlen--;
    }
    यदि(!hlen)
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }
  यदि(Curl_llist_गणनम्(&दत्त->state.httphdrs) >= MAX_HTTP_RESP_शीर्षकम्_गणनम्) {
    failf(दत्त, "Too many response headers, %d is max",
          MAX_HTTP_RESP_शीर्षकम्_गणनम्);
    प्रत्यागम CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, परिमाण(*hs) + hlen);
  यदि(!hs)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  स्मृतिं_प्रतिलिख(hs->बफर, शीर्षकम्, hlen);
  hs->बफर[hlen] = 0; /* null-terminate */

  परिणाम = namevalue(hs->बफर, hlen, type, &name, &मानम्);
  यदि(!परिणाम) {
    hs->name = name;
    hs->मानम् = मानम्;
    hs->type = type;
    hs->request = दत्त->state.अनुरोधाः;

    /* insert this node into the list of headers */
    Curl_llist_append(&दत्त->state.httphdrs, hs, &hs->ग्रन्थिः);
  }
  अन्यथा {
    failf(दत्त, "Invalid response header");
    curlx_free(hs);
  }
  प्रत्यागम परिणाम;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
स्थिर शून्य शीर्षकाणि_reset(संरचना Curl_easy *दत्त)
{
  Curl_llist_init(&दत्त->state.httphdrs, रिक्त);
  दत्त->state.nh_गणनम् = 0;
}

संरचना hds_cw_collect_ctx {
  संरचना Curl_cwriter super;
};

स्थिर CURLcode hds_cw_collect_write(संरचना Curl_easy *दत्त,
                                     संरचना Curl_cwriter *writer, पूर्णाङ्क type,
                                     ध्रुव वर्ण *buf, size_t blen)
{
  यदि((type & CLIENTWRITE_शीर्षकम्) && !(type & CLIENTWRITE_स्थितिः)) {
    अचिह्नित वर्ण htype = (अचिह्नित वर्ण)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_शीर्षकम्)));
    CURLcode परिणाम = Curl_शीर्षकाणि_प्रक्षिप(दत्त, buf, blen, htype);
    CURL_TRC_WRITE(दत्त, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम Curl_cwriter_write(दत्त, writer->अग्र, type, buf, blen);
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

CURLcode Curl_शीर्षकाणि_init(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cwriter *writer;
  CURLcode परिणाम;

  यदि(दत्त->conn && (दत्त->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    यदि(Curl_cwriter_get_by_name(दत्त, hds_cw_collect.name))
      प्रत्यागम CURLE_OK;

    परिणाम = Curl_cwriter_create(&writer, दत्त, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    परिणाम = Curl_cwriter_सङ्कलय(दत्त, writer);
    यदि(परिणाम) {
      Curl_cwriter_free(दत्त, writer);
      प्रत्यागम परिणाम;
    }
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_शीर्षकाणि_cleanup(संरचना Curl_easy *दत्त)
{
  संरचना Curl_llist_ग्रन्थिः *e;
  संरचना Curl_llist_ग्रन्थिः *n;

  प्रति(e = Curl_llist_शीर्ष(&दत्त->state.httphdrs); e; e = n) {
    संरचना Curl_शीर्षकम्_store *hs = Curl_ग्रन्थिः_elem(e);
    n = Curl_ग्रन्थिः_अग्र(e);
    curlx_free(hs);
  }
  शीर्षकाणि_reset(दत्त);
  प्रत्यागम CURLE_OK;
}

#अन्यथा /* HTTP-disabled builds below */

CURLHcode curl_easy_शीर्षकम्(CURL *easy,
                           ध्रुव वर्ण *name,
                           size_t सूचकांक,
                           अचिह्नित पूर्णाङ्क origin,
                           पूर्णाङ्क request,
                           संरचना curl_शीर्षकम् **hout)
{
  (शून्य)easy;
  (शून्य)name;
  (शून्य)सूचकांक;
  (शून्य)origin;
  (शून्य)request;
  (शून्य)hout;
  प्रत्यागम CURLHE_NOT_BUILT_अन्तर्;
}

संरचना curl_शीर्षकम् *curl_easy_nextheader(CURL *easy,
                                         अचिह्नित पूर्णाङ्क type,
                                         पूर्णाङ्क request,
                                         संरचना curl_शीर्षकम् *पूर्व)
{
  (शून्य)easy;
  (शून्य)type;
  (शून्य)request;
  (शून्य)पूर्व;
  प्रत्यागम रिक्त;
}
#यदि_अन्तः
