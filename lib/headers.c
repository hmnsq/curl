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

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرخیاں_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ساکن خالی copy_سرخی_external(ساخت Curl_سرخی_store *hs,
                                 size_t اشاریہ,
                                 size_t amount,
                                 ساخت Curl_llist_گرہ *e,
                                 ساخت curl_سرخی *hout)
{
  ساخت curl_سرخی *h = hout;
  h->name = hs->name;
  h->قدر = hs->قدر;
  h->amount = amount;
  h->اشاریہ = اشاریہ;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (غیرعلامتی صحیح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_سرخی(CURL *curl,
                           ثابت حرف *name,
                           size_t nameindex,
                           غیرعلامتی صحیح origin,
                           صحیح request,
                           ساخت curl_سرخی **hout)
{
  ساخت Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_سرخی, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_llist_گرہ *e_pick = تہی;
    size_t مطابقت = 0;
    size_t amount = 0;
    ساخت Curl_سرخی_store *hs = تہی;
    ساخت Curl_سرخی_store *pick = تہی;
    اگر(!name || !hout || !ڈیٹا ||
       (origin > (CURLH_سرخی | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_دلیل;
      جاؤ برآمد;
    }
    اگر(!Curl_llist_گنتی(&ڈیٹا->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      جاؤ برآمد;
    }
    اگر(request > ڈیٹا->state.درخواستیں) {
      hresult = CURLHE_NOREQUEST;
      جاؤ برآمد;
    }
    اگر(request == -1)
      request = ڈیٹا->state.درخواستیں;

    /* we need a first round to count amount of this header */
    برائے(e = Curl_llist_سر(&ڈیٹا->state.httphdrs); e; e = Curl_گرہ_اگلا(e)) {
      hs = Curl_گرہ_elem(e);
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
    ورنہ اگر(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    اگر(hresult)
      جاؤ برآمد;

    اگر(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    ورنہ {
      برائے(e = Curl_llist_سر(&ڈیٹا->state.httphdrs); e;
          e = Curl_گرہ_اگلا(e)) {
        hs = Curl_گرہ_elem(e);
        اگر(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (مطابقت++ == nameindex)) {
          e_pick = e;
          توڑو;
        }
      }
      اگر(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        جاؤ برآمد;
      }
    }
    /* this is the name we want */
    copy_سرخی_external(hs, nameindex, amount, e_pick,
                         &ڈیٹا->state.headerout[0]);
    *hout = &ڈیٹا->state.headerout[0];
    hresult = CURLHE_OK;
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  اگر(نتیجہ)
    hresult = Curl_eapi_hcode(نتیجہ);
  واپس hresult;
}

ساخت nextheader_cache_اندراج {
  ساخت Curl_سرخی_store *سرخی;
  size_t ترتیب;
};

ساکن صحیح nextheader_compare(ثابت خالی *p1, ثابت خالی *p2)
{
  ثابت ساخت nextheader_cache_اندراج *e1 = p1;
  ثابت ساخت nextheader_cache_اندراج *e2 = p2;
  ثابت ساخت Curl_سرخی_store *h1 = e1->سرخی;
  ثابت ساخت Curl_سرخی_store *h2 = e2->سرخی;
  ثابت حرف *n1 = h1->name;
  ثابت حرف *n2 = h2->name;

  جب(*n1 && *n2) {
    غیرعلامتی حرف c1 = (غیرعلامتی حرف)Curl_raw_toupper(*n1++);
    غیرعلامتی حرف c2 = (غیرعلامتی حرف)Curl_raw_toupper(*n2++);
    اگر(c1 != c2)
      واپس c1 > c2 ? 1 : -1;
  }
  اگر(*n1)
    واپس 1;
  اگر(*n2)
    واپس -1;
  اگر(e1->ترتیب != e2->ترتیب)
    واپس e1->ترتیب > e2->ترتیب ? 1 : -1;
  واپس 0;
}

ساکن bool nextheader_cache_build(ساخت Curl_easy *ڈیٹا,
                                   غیرعلامتی صحیح origin,
                                   صحیح request, size_t گنتی)
{
  ساخت nextheader_cache_اندراج *سرخیاں;
  ساخت Curl_llist_گرہ *e;
  size_t i = 0;
  size_t پہلا;

  سرخیاں = curlx_malloc(حجم(*سرخیاں) * گنتی);
  اگر(!سرخیاں)
    واپس FALSE;

  برائے(e = Curl_llist_سر(&ڈیٹا->state.httphdrs); e;
      e = Curl_گرہ_اگلا(e)) {
    ساخت Curl_سرخی_store *hs = Curl_گرہ_elem(e);
    اگر((hs->type & origin) && (hs->request == request)) {
      سرخیاں[i].سرخی = hs;
      سرخیاں[i].ترتیب = i;
      i++;
    }
  }

  qsort(سرخیاں, i, حجم(*سرخیاں), nextheader_compare);
  برائے(پہلا = 0; پہلا < i;) {
    size_t آخری = پہلا + 1;
    size_t اشاریہ;
    جب((آخری < i) &&
          curl_strequal(سرخیاں[پہلا].سرخی->name,
                        سرخیاں[آخری].سرخی->name))
      آخری++;
    برائے(اشاریہ = پہلا; اشاریہ < آخری; اشاریہ++) {
      سرخیاں[اشاریہ].سرخی->nh_amount = آخری - پہلا;
      سرخیاں[اشاریہ].سرخی->nh_اشاریہ = اشاریہ - پہلا;
    }
    پہلا = آخری;
  }
  curlx_free(سرخیاں);

  ڈیٹا->state.nh_origin = origin;
  ڈیٹا->state.nh_request = request;
  ڈیٹا->state.nh_گنتی = گنتی;
  واپس TRUE;
}

/* public API */
ساخت curl_سرخی *curl_easy_nextheader(CURL *curl,
                                         غیرعلامتی صحیح origin,
                                         صحیح request,
                                         ساخت curl_سرخی *پچھلا)
{
  ساخت Curl_eapi_guard guard;
  ساخت curl_سرخی *hd = تہی;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    ساخت Curl_llist_گرہ *pick;
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_سرخی_store *hs;
    size_t amount = 0;
    size_t اشاریہ = 0;
    size_t گنتی;

    اگر(request > ڈیٹا->state.درخواستیں)
      جاؤ برآمد;
    اگر(request == -1)
      request = ڈیٹا->state.درخواستیں;

    اگر(پچھلا) {
      pick = پچھلا->anchor;
      اگر(!pick)
        /* something is wrong */
        جاؤ برآمد;
      pick = Curl_گرہ_اگلا(pick);
    }
    ورنہ
      pick = Curl_llist_سر(&ڈیٹا->state.httphdrs);

    اگر(pick) {
      /* make sure it is the next header of the desired type */
      کرو {
        hs = Curl_گرہ_elem(pick);
        اگر((hs->type & origin) && (hs->request == request))
          توڑو;
        pick = Curl_گرہ_اگلا(pick);
      } جب(pick);
    }

    اگر(!pick)
      /* no more headers available */
      جاؤ برآمد;

    hs = Curl_گرہ_elem(pick);
    گنتی = Curl_llist_گنتی(&ڈیٹا->state.httphdrs);

    اگر(((ڈیٹا->state.nh_گنتی == گنتی) &&
        (ڈیٹا->state.nh_origin == origin) &&
        (ڈیٹا->state.nh_request == request)) ||
       nextheader_cache_build(ڈیٹا, origin, request, گنتی)) {
      amount = hs->nh_amount;
      اشاریہ = hs->nh_اشاریہ;
    }
    ورنہ {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      برائے(e = Curl_llist_سر(&ڈیٹا->state.httphdrs); e;
          e = Curl_گرہ_اگلا(e)) {
        ساخت Curl_سرخی_store *check = Curl_گرہ_elem(e);
        اگر(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        اگر(e == pick)
          اشاریہ = amount - 1;
      }
    }
    copy_سرخی_external(hs, اشاریہ, amount, pick,
                         &ڈیٹا->state.headerout[1]);
    hd = &ڈیٹا->state.headerout[1];
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس hd;
}

ساکن CURLcode namevalue(حرف *سرخی, size_t hlen, غیرعلامتی صحیح type,
                          حرف **name, حرف **قدر)
{
  حرف *انتہا = سرخی + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = سرخی;

  اگر(type == CURLH_PSEUDO) {
    اگر(*سرخی != ':')
      واپس CURLE_BAD_FUNCTION_دلیل;
    سرخی++;
  }

  /* Find the end of the header name */
  جب(*سرخی && (*سرخی != ':'))
    ++سرخی;

  اگر(*سرخی)
    /* Skip over colon, null it */
    *سرخی++ = 0;
  ورنہ
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* skip all leading blank letters */
  جب(ISBLANK(*سرخی))
    سرخی++;

  *قدر = سرخی;

  /* skip all trailing space letters */
  جب((انتہا > سرخی) && ISBLANK(*انتہا))
    *انتہا-- = 0; /* null-terminate */
  واپس CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_سرخیاں_ڈالو(ساخت Curl_easy *ڈیٹا, ثابت حرف *سرخی,
                           size_t hlen, /* length of header */
                           غیرعلامتی حرف type)
{
  حرف *قدر = تہی;
  حرف *name = تہی;
  ساخت Curl_سرخی_store *hs;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  ثابت size_t ilen = hlen;

  اگر((سرخی[0] == '\r') || (سرخی[0] == '\n'))
    /* ignore the body separator */
    واپس CURLE_OK;

  /* trim off newline characters */
  اگر(hlen && (سرخی[hlen - 1] == '\n'))
    hlen--;
  اگر(hlen && (سرخی[hlen - 1] == '\r'))
    hlen--;
  اگر(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    واپس CURLE_WEIRD_SERVER_جواب;

  اگر(ISBLANK(سرخی[0])) {
    /* pass leading blanks */
    جب(hlen && ISBLANK(*سرخی)) {
      سرخی++;
      hlen--;
    }
    اگر(!hlen)
      واپس CURLE_WEIRD_SERVER_جواب;
  }
  اگر(Curl_llist_گنتی(&ڈیٹا->state.httphdrs) >= MAX_HTTP_RESP_سرخی_گنتی) {
    failf(ڈیٹا, "Too many response headers, %d is max",
          MAX_HTTP_RESP_سرخی_گنتی);
    واپس CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, حجم(*hs) + hlen);
  اگر(!hs)
    واپس CURLE_برآمد_کا_حافظہ;
  حافظہ_نقل(hs->بفر, سرخی, hlen);
  hs->بفر[hlen] = 0; /* null-terminate */

  نتیجہ = namevalue(hs->بفر, hlen, type, &name, &قدر);
  اگر(!نتیجہ) {
    hs->name = name;
    hs->قدر = قدر;
    hs->type = type;
    hs->request = ڈیٹا->state.درخواستیں;

    /* insert this node into the list of headers */
    Curl_llist_append(&ڈیٹا->state.httphdrs, hs, &hs->گرہ);
  }
  ورنہ {
    failf(ڈیٹا, "Invalid response header");
    curlx_free(hs);
  }
  واپس نتیجہ;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ساکن خالی سرخیاں_reset(ساخت Curl_easy *ڈیٹا)
{
  Curl_llist_init(&ڈیٹا->state.httphdrs, تہی);
  ڈیٹا->state.nh_گنتی = 0;
}

ساخت hds_cw_collect_ctx {
  ساخت Curl_cwriter super;
};

ساکن CURLcode hds_cw_collect_write(ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_cwriter *writer, صحیح type,
                                     ثابت حرف *buf, size_t blen)
{
  اگر((type & CLIENTWRITE_سرخی) && !(type & CLIENTWRITE_حالت)) {
    غیرعلامتی حرف htype = (غیرعلامتی حرف)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_سرخی)));
    CURLcode نتیجہ = Curl_سرخیاں_ڈالو(ڈیٹا, buf, blen, htype);
    CURL_TRC_WRITE(ڈیٹا, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحیح)نتیجہ);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس Curl_cwriter_write(ڈیٹا, writer->اگلا, type, buf, blen);
}

ساکن ثابت ساخت Curl_cwtype hds_cw_collect = {
  "hds-collect",
  تہی,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(ساخت hds_cw_collect_ctx)
};

CURLcode Curl_سرخیاں_init(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cwriter *writer;
  CURLcode نتیجہ;

  اگر(ڈیٹا->conn && (ڈیٹا->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    اگر(Curl_cwriter_get_by_name(ڈیٹا, hds_cw_collect.name))
      واپس CURLE_OK;

    نتیجہ = Curl_cwriter_create(&writer, ڈیٹا, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    اگر(نتیجہ)
      واپس نتیجہ;

    نتیجہ = Curl_cwriter_جوڑو(ڈیٹا, writer);
    اگر(نتیجہ) {
      Curl_cwriter_free(ڈیٹا, writer);
      واپس نتیجہ;
    }
  }
  واپس CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرخیاں_cleanup(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_llist_گرہ *e;
  ساخت Curl_llist_گرہ *n;

  برائے(e = Curl_llist_سر(&ڈیٹا->state.httphdrs); e; e = n) {
    ساخت Curl_سرخی_store *hs = Curl_گرہ_elem(e);
    n = Curl_گرہ_اگلا(e);
    curlx_free(hs);
  }
  سرخیاں_reset(ڈیٹا);
  واپس CURLE_OK;
}

#ورنہ /* HTTP-disabled builds below */

CURLHcode curl_easy_سرخی(CURL *easy,
                           ثابت حرف *name,
                           size_t اشاریہ,
                           غیرعلامتی صحیح origin,
                           صحیح request,
                           ساخت curl_سرخی **hout)
{
  (خالی)easy;
  (خالی)name;
  (خالی)اشاریہ;
  (خالی)origin;
  (خالی)request;
  (خالی)hout;
  واپس CURLHE_NOT_BUILT_اندر;
}

ساخت curl_سرخی *curl_easy_nextheader(CURL *easy,
                                         غیرعلامتی صحیح type,
                                         صحیح request,
                                         ساخت curl_سرخی *پچھلا)
{
  (خالی)easy;
  (خالی)type;
  (خالی)request;
  (خالی)پچھلا;
  واپس تہی;
}
#اگر_ختم
