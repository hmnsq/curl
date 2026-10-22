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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "headers.h"
#అన్తర్భావ "strcase.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_శీర్షికలు_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
స్థిర శూన్య copy_శీర్షిక_external(నిర్మాణం Curl_శీర్షిక_store *hs,
                                 size_t సూచీ,
                                 size_t amount,
                                 నిర్మాణం Curl_llist_కణుపు *e,
                                 నిర్మాణం curl_శీర్షిక *hout)
{
  నిర్మాణం curl_శీర్షిక *h = hout;
  h->name = hs->name;
  h->విలువ = hs->విలువ;
  h->amount = amount;
  h->సూచీ = సూచీ;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (గుర్తులేని పూర్ణాంకం)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_శీర్షిక(CURL *curl,
                           స్థిరరాశి అక్షరం *name,
                           size_t nameindex,
                           గుర్తులేని పూర్ణాంకం origin,
                           పూర్ణాంకం request,
                           నిర్మాణం curl_శీర్షిక **hout)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_శీర్షిక, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_llist_కణుపు *e_pick = ఖాళీ;
    size_t సరిపోలిక = 0;
    size_t amount = 0;
    నిర్మాణం Curl_శీర్షిక_store *hs = ఖాళీ;
    నిర్మాణం Curl_శీర్షిక_store *pick = ఖాళీ;
    ఒకవేళ(!name || !hout || !దత్తాంశం ||
       (origin > (CURLH_శీర్షిక | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_వాదం;
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(!Curl_llist_లెక్క(&దత్తాంశం->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(request > దత్తాంశం->state.అభ్యర్థనలు) {
      hresult = CURLHE_NOREQUEST;
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(request == -1)
      request = దత్తాంశం->state.అభ్యర్థనలు;

    /* we need a first round to count amount of this header */
    కొరకు(e = Curl_llist_తల(&దత్తాంశం->state.httphdrs); e; e = Curl_కణుపు_తదుపరి(e)) {
      hs = Curl_కణుపు_elem(e);
      ఒకవేళ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ఒకవేళ(!amount)
      hresult = CURLHE_MISSING;
    లేకపోతే ఒకవేళ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ఒకవేళ(hresult)
      వెళ్ళు బహిర్గతం;

    ఒకవేళ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    లేకపోతే {
      కొరకు(e = Curl_llist_తల(&దత్తాంశం->state.httphdrs); e;
          e = Curl_కణుపు_తదుపరి(e)) {
        hs = Curl_కణుపు_elem(e);
        ఒకవేళ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (సరిపోలిక++ == nameindex)) {
          e_pick = e;
          ఆపు;
        }
      }
      ఒకవేళ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        వెళ్ళు బహిర్గతం;
      }
    }
    /* this is the name we want */
    copy_శీర్షిక_external(hs, nameindex, amount, e_pick,
                         &దత్తాంశం->state.headerout[0]);
    *hout = &దత్తాంశం->state.headerout[0];
    hresult = CURLHE_OK;
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  ఒకవేళ(ఫలితం)
    hresult = Curl_eapi_hcode(ఫలితం);
  తిరిగి hresult;
}

నిర్మాణం nextheader_cache_నమోదు {
  నిర్మాణం Curl_శీర్షిక_store *శీర్షిక;
  size_t క్రమం;
};

స్థిర పూర్ణాంకం nextheader_compare(స్థిరరాశి శూన్య *p1, స్థిరరాశి శూన్య *p2)
{
  స్థిరరాశి నిర్మాణం nextheader_cache_నమోదు *e1 = p1;
  స్థిరరాశి నిర్మాణం nextheader_cache_నమోదు *e2 = p2;
  స్థిరరాశి నిర్మాణం Curl_శీర్షిక_store *h1 = e1->శీర్షిక;
  స్థిరరాశి నిర్మాణం Curl_శీర్షిక_store *h2 = e2->శీర్షిక;
  స్థిరరాశి అక్షరం *n1 = h1->name;
  స్థిరరాశి అక్షరం *n2 = h2->name;

  అయితే(*n1 && *n2) {
    గుర్తులేని అక్షరం c1 = (గుర్తులేని అక్షరం)Curl_raw_toupper(*n1++);
    గుర్తులేని అక్షరం c2 = (గుర్తులేని అక్షరం)Curl_raw_toupper(*n2++);
    ఒకవేళ(c1 != c2)
      తిరిగి c1 > c2 ? 1 : -1;
  }
  ఒకవేళ(*n1)
    తిరిగి 1;
  ఒకవేళ(*n2)
    తిరిగి -1;
  ఒకవేళ(e1->క్రమం != e2->క్రమం)
    తిరిగి e1->క్రమం > e2->క్రమం ? 1 : -1;
  తిరిగి 0;
}

స్థిర bool nextheader_cache_build(నిర్మాణం Curl_easy *దత్తాంశం,
                                   గుర్తులేని పూర్ణాంకం origin,
                                   పూర్ణాంకం request, size_t లెక్క)
{
  నిర్మాణం nextheader_cache_నమోదు *శీర్షికలు;
  నిర్మాణం Curl_llist_కణుపు *e;
  size_t i = 0;
  size_t మొదటి;

  శీర్షికలు = curlx_malloc(పరిమాణం(*శీర్షికలు) * లెక్క);
  ఒకవేళ(!శీర్షికలు)
    తిరిగి FALSE;

  కొరకు(e = Curl_llist_తల(&దత్తాంశం->state.httphdrs); e;
      e = Curl_కణుపు_తదుపరి(e)) {
    నిర్మాణం Curl_శీర్షిక_store *hs = Curl_కణుపు_elem(e);
    ఒకవేళ((hs->type & origin) && (hs->request == request)) {
      శీర్షికలు[i].శీర్షిక = hs;
      శీర్షికలు[i].క్రమం = i;
      i++;
    }
  }

  qsort(శీర్షికలు, i, పరిమాణం(*శీర్షికలు), nextheader_compare);
  కొరకు(మొదటి = 0; మొదటి < i;) {
    size_t చివరి = మొదటి + 1;
    size_t సూచీ;
    అయితే((చివరి < i) &&
          curl_strequal(శీర్షికలు[మొదటి].శీర్షిక->name,
                        శీర్షికలు[చివరి].శీర్షిక->name))
      చివరి++;
    కొరకు(సూచీ = మొదటి; సూచీ < చివరి; సూచీ++) {
      శీర్షికలు[సూచీ].శీర్షిక->nh_amount = చివరి - మొదటి;
      శీర్షికలు[సూచీ].శీర్షిక->nh_సూచీ = సూచీ - మొదటి;
    }
    మొదటి = చివరి;
  }
  curlx_free(శీర్షికలు);

  దత్తాంశం->state.nh_origin = origin;
  దత్తాంశం->state.nh_request = request;
  దత్తాంశం->state.nh_లెక్క = లెక్క;
  తిరిగి TRUE;
}

/* public API */
నిర్మాణం curl_శీర్షిక *curl_easy_nextheader(CURL *curl,
                                         గుర్తులేని పూర్ణాంకం origin,
                                         పూర్ణాంకం request,
                                         నిర్మాణం curl_శీర్షిక *prev)
{
  నిర్మాణం Curl_eapi_guard guard;
  నిర్మాణం curl_శీర్షిక *hd = ఖాళీ;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    నిర్మాణం Curl_llist_కణుపు *pick;
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_శీర్షిక_store *hs;
    size_t amount = 0;
    size_t సూచీ = 0;
    size_t లెక్క;

    ఒకవేళ(request > దత్తాంశం->state.అభ్యర్థనలు)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(request == -1)
      request = దత్తాంశం->state.అభ్యర్థనలు;

    ఒకవేళ(prev) {
      pick = prev->anchor;
      ఒకవేళ(!pick)
        /* something is wrong */
        వెళ్ళు బహిర్గతం;
      pick = Curl_కణుపు_తదుపరి(pick);
    }
    లేకపోతే
      pick = Curl_llist_తల(&దత్తాంశం->state.httphdrs);

    ఒకవేళ(pick) {
      /* make sure it is the next header of the desired type */
      చేయి {
        hs = Curl_కణుపు_elem(pick);
        ఒకవేళ((hs->type & origin) && (hs->request == request))
          ఆపు;
        pick = Curl_కణుపు_తదుపరి(pick);
      } అయితే(pick);
    }

    ఒకవేళ(!pick)
      /* no more headers available */
      వెళ్ళు బహిర్గతం;

    hs = Curl_కణుపు_elem(pick);
    లెక్క = Curl_llist_లెక్క(&దత్తాంశం->state.httphdrs);

    ఒకవేళ(((దత్తాంశం->state.nh_లెక్క == లెక్క) &&
        (దత్తాంశం->state.nh_origin == origin) &&
        (దత్తాంశం->state.nh_request == request)) ||
       nextheader_cache_build(దత్తాంశం, origin, request, లెక్క)) {
      amount = hs->nh_amount;
      సూచీ = hs->nh_సూచీ;
    }
    లేకపోతే {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      కొరకు(e = Curl_llist_తల(&దత్తాంశం->state.httphdrs); e;
          e = Curl_కణుపు_తదుపరి(e)) {
        నిర్మాణం Curl_శీర్షిక_store *check = Curl_కణుపు_elem(e);
        ఒకవేళ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ఒకవేళ(e == pick)
          సూచీ = amount - 1;
      }
    }
    copy_శీర్షిక_external(hs, సూచీ, amount, pick,
                         &దత్తాంశం->state.headerout[1]);
    hd = &దత్తాంశం->state.headerout[1];
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి hd;
}

స్థిర CURLcode namevalue(అక్షరం *శీర్షిక, size_t hlen, గుర్తులేని పూర్ణాంకం type,
                          అక్షరం **name, అక్షరం **విలువ)
{
  అక్షరం *చివర = శీర్షిక + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = శీర్షిక;

  ఒకవేళ(type == CURLH_PSEUDO) {
    ఒకవేళ(*శీర్షిక != ':')
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    శీర్షిక++;
  }

  /* Find the end of the header name */
  అయితే(*శీర్షిక && (*శీర్షిక != ':'))
    ++శీర్షిక;

  ఒకవేళ(*శీర్షిక)
    /* Skip over colon, null it */
    *శీర్షిక++ = 0;
  లేకపోతే
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* skip all leading blank letters */
  అయితే(ISBLANK(*శీర్షిక))
    శీర్షిక++;

  *విలువ = శీర్షిక;

  /* skip all trailing space letters */
  అయితే((చివర > శీర్షిక) && ISBLANK(*చివర))
    *చివర-- = 0; /* null-terminate */
  తిరిగి CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_శీర్షికలు_నెట్టు(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *శీర్షిక,
                           size_t hlen, /* length of header */
                           గుర్తులేని అక్షరం type)
{
  అక్షరం *విలువ = ఖాళీ;
  అక్షరం *name = ఖాళీ;
  నిర్మాణం Curl_శీర్షిక_store *hs;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  స్థిరరాశి size_t ilen = hlen;

  ఒకవేళ((శీర్షిక[0] == '\r') || (శీర్షిక[0] == '\n'))
    /* ignore the body separator */
    తిరిగి CURLE_OK;

  /* trim off newline characters */
  ఒకవేళ(hlen && (శీర్షిక[hlen - 1] == '\n'))
    hlen--;
  ఒకవేళ(hlen && (శీర్షిక[hlen - 1] == '\r'))
    hlen--;
  ఒకవేళ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;

  ఒకవేళ(ISBLANK(శీర్షిక[0])) {
    /* pass leading blanks */
    అయితే(hlen && ISBLANK(*శీర్షిక)) {
      శీర్షిక++;
      hlen--;
    }
    ఒకవేళ(!hlen)
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }
  ఒకవేళ(Curl_llist_లెక్క(&దత్తాంశం->state.httphdrs) >= MAX_HTTP_RESP_శీర్షిక_లెక్క) {
    failf(దత్తాంశం, "Too many response headers, %d is max",
          MAX_HTTP_RESP_శీర్షిక_లెక్క);
    తిరిగి CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, పరిమాణం(*hs) + hlen);
  ఒకవేళ(!hs)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  జ్ఞాపకం_నకలు(hs->బఫరు, శీర్షిక, hlen);
  hs->బఫరు[hlen] = 0; /* null-terminate */

  ఫలితం = namevalue(hs->బఫరు, hlen, type, &name, &విలువ);
  ఒకవేళ(!ఫలితం) {
    hs->name = name;
    hs->విలువ = విలువ;
    hs->type = type;
    hs->request = దత్తాంశం->state.అభ్యర్థనలు;

    /* insert this node into the list of headers */
    Curl_llist_append(&దత్తాంశం->state.httphdrs, hs, &hs->కణుపు);
  }
  లేకపోతే {
    failf(దత్తాంశం, "Invalid response header");
    curlx_free(hs);
  }
  తిరిగి ఫలితం;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
స్థిర శూన్య శీర్షికలు_reset(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_llist_init(&దత్తాంశం->state.httphdrs, ఖాళీ);
  దత్తాంశం->state.nh_లెక్క = 0;
}

నిర్మాణం hds_cw_collect_ctx {
  నిర్మాణం Curl_cwriter super;
};

స్థిర CURLcode hds_cw_collect_write(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_cwriter *writer, పూర్ణాంకం type,
                                     స్థిరరాశి అక్షరం *buf, size_t blen)
{
  ఒకవేళ((type & CLIENTWRITE_శీర్షిక) && !(type & CLIENTWRITE_స్థితి)) {
    గుర్తులేని అక్షరం htype = (గుర్తులేని అక్షరం)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_శీర్షిక)));
    CURLcode ఫలితం = Curl_శీర్షికలు_నెట్టు(దత్తాంశం, buf, blen, htype);
    CURL_TRC_WRITE(దత్తాంశం, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి Curl_cwriter_write(దత్తాంశం, writer->తదుపరి, type, buf, blen);
}

స్థిర స్థిరరాశి నిర్మాణం Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ఖాళీ,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  పరిమాణం(నిర్మాణం hds_cw_collect_ctx)
};

CURLcode Curl_శీర్షికలు_init(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cwriter *writer;
  CURLcode ఫలితం;

  ఒకవేళ(దత్తాంశం->conn && (దత్తాంశం->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ఒకవేళ(Curl_cwriter_get_by_name(దత్తాంశం, hds_cw_collect.name))
      తిరిగి CURLE_OK;

    ఫలితం = Curl_cwriter_create(&writer, దత్తాంశం, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఫలితం = Curl_cwriter_కూడు(దత్తాంశం, writer);
    ఒకవేళ(ఫలితం) {
      Curl_cwriter_free(దత్తాంశం, writer);
      తిరిగి ఫలితం;
    }
  }
  తిరిగి CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_శీర్షికలు_cleanup(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_llist_కణుపు *e;
  నిర్మాణం Curl_llist_కణుపు *n;

  కొరకు(e = Curl_llist_తల(&దత్తాంశం->state.httphdrs); e; e = n) {
    నిర్మాణం Curl_శీర్షిక_store *hs = Curl_కణుపు_elem(e);
    n = Curl_కణుపు_తదుపరి(e);
    curlx_free(hs);
  }
  శీర్షికలు_reset(దత్తాంశం);
  తిరిగి CURLE_OK;
}

#లేకపోతే /* HTTP-disabled builds below */

CURLHcode curl_easy_శీర్షిక(CURL *easy,
                           స్థిరరాశి అక్షరం *name,
                           size_t సూచీ,
                           గుర్తులేని పూర్ణాంకం origin,
                           పూర్ణాంకం request,
                           నిర్మాణం curl_శీర్షిక **hout)
{
  (శూన్య)easy;
  (శూన్య)name;
  (శూన్య)సూచీ;
  (శూన్య)origin;
  (శూన్య)request;
  (శూన్య)hout;
  తిరిగి CURLHE_NOT_BUILT_లోపల;
}

నిర్మాణం curl_శీర్షిక *curl_easy_nextheader(CURL *easy,
                                         గుర్తులేని పూర్ణాంకం type,
                                         పూర్ణాంకం request,
                                         నిర్మాణం curl_శీర్షిక *prev)
{
  (శూన్య)easy;
  (శూన్య)type;
  (శూన్య)request;
  (శూన్య)prev;
  తిరిగి ఖాళీ;
}
#అయితే_ముగింపు
