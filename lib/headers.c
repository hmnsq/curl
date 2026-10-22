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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "headers.h"
#ضمن "strcase.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرخیاں_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ساكن فراغ copy_سرخی_external(بنية Curl_سرخی_store *hs,
                                 size_t اشاریہ,
                                 size_t amount,
                                 بنية Curl_llist_گرہ *e,
                                 بنية curl_سرخی *hout)
{
  بنية curl_سرخی *h = hout;
  h->name = hs->name;
  h->قدر = hs->قدر;
  h->amount = amount;
  h->اشاریہ = اشاریہ;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (بنانشانی صحيح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_سرخی(CURL *curl,
                           ثابت حرف *name,
                           size_t nameindex,
                           بنانشانی صحيح origin,
                           صحيح request,
                           بنية curl_سرخی **hout)
{
  بنية Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode نتیجہ;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_سرخی, &نتیجہ)) {
    بنية Curl_easy *انکڑے = curl;
    بنية Curl_llist_گرہ *e;
    بنية Curl_llist_گرہ *e_pick = عدم;
    size_t میل = 0;
    size_t amount = 0;
    بنية Curl_سرخی_store *hs = عدم;
    بنية Curl_سرخی_store *pick = عدم;
    إذا(!name || !hout || !انکڑے ||
       (origin > (CURLH_سرخی | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_دلیل;
      اذهب باہرلا;
    }
    إذا(!Curl_llist_گنتی(&انکڑے->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      اذهب باہرلا;
    }
    إذا(request > انکڑے->state.بینتیاں) {
      hresult = CURLHE_NOREQUEST;
      اذهب باہرلا;
    }
    إذا(request == -1)
      request = انکڑے->state.بینتیاں;

    /* we need a first round to count amount of this header */
    لكل(e = Curl_llist_سر(&انکڑے->state.httphdrs); e; e = Curl_گرہ_اگلا(e)) {
      hs = Curl_گرہ_elem(e);
      إذا(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    إذا(!amount)
      hresult = CURLHE_MISSING;
    وإلا إذا(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    إذا(hresult)
      اذهب باہرلا;

    إذا(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    وإلا {
      لكل(e = Curl_llist_سر(&انکڑے->state.httphdrs); e;
          e = Curl_گرہ_اگلا(e)) {
        hs = Curl_گرہ_elem(e);
        إذا(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (میل++ == nameindex)) {
          e_pick = e;
          اكسر;
        }
      }
      إذا(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        اذهب باہرلا;
      }
    }
    /* this is the name we want */
    copy_سرخی_external(hs, nameindex, amount, e_pick,
                         &انکڑے->state.headerout[0]);
    *hout = &انکڑے->state.headerout[0];
    hresult = CURLHE_OK;
  }
باہرلا:
  CURL_EAPI_LEAVE(&guard);
  إذا(نتیجہ)
    hresult = Curl_eapi_hcode(نتیجہ);
  أرجع hresult;
}

بنية nextheader_cache_اندراج {
  بنية Curl_سرخی_store *سرخی;
  size_t ترتیب;
};

ساكن صحيح nextheader_compare(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية nextheader_cache_اندراج *e1 = p1;
  ثابت بنية nextheader_cache_اندراج *e2 = p2;
  ثابت بنية Curl_سرخی_store *h1 = e1->سرخی;
  ثابت بنية Curl_سرخی_store *h2 = e2->سرخی;
  ثابت حرف *n1 = h1->name;
  ثابت حرف *n2 = h2->name;

  طالما(*n1 && *n2) {
    بنانشانی حرف c1 = (بنانشانی حرف)Curl_raw_toupper(*n1++);
    بنانشانی حرف c2 = (بنانشانی حرف)Curl_raw_toupper(*n2++);
    إذا(c1 != c2)
      أرجع c1 > c2 ? 1 : -1;
  }
  إذا(*n1)
    أرجع 1;
  إذا(*n2)
    أرجع -1;
  إذا(e1->ترتیب != e2->ترتیب)
    أرجع e1->ترتیب > e2->ترتیب ? 1 : -1;
  أرجع 0;
}

ساكن bool nextheader_cache_build(بنية Curl_easy *انکڑے,
                                   بنانشانی صحيح origin,
                                   صحيح request, size_t گنتی)
{
  بنية nextheader_cache_اندراج *سرخیاں;
  بنية Curl_llist_گرہ *e;
  size_t i = 0;
  size_t پہلا;

  سرخیاں = curlx_malloc(حجم(*سرخیاں) * گنتی);
  إذا(!سرخیاں)
    أرجع FALSE;

  لكل(e = Curl_llist_سر(&انکڑے->state.httphdrs); e;
      e = Curl_گرہ_اگلا(e)) {
    بنية Curl_سرخی_store *hs = Curl_گرہ_elem(e);
    إذا((hs->type & origin) && (hs->request == request)) {
      سرخیاں[i].سرخی = hs;
      سرخیاں[i].ترتیب = i;
      i++;
    }
  }

  qsort(سرخیاں, i, حجم(*سرخیاں), nextheader_compare);
  لكل(پہلا = 0; پہلا < i;) {
    size_t آخری = پہلا + 1;
    size_t اشاریہ;
    طالما((آخری < i) &&
          curl_strequal(سرخیاں[پہلا].سرخی->name,
                        سرخیاں[آخری].سرخی->name))
      آخری++;
    لكل(اشاریہ = پہلا; اشاریہ < آخری; اشاریہ++) {
      سرخیاں[اشاریہ].سرخی->nh_amount = آخری - پہلا;
      سرخیاں[اشاریہ].سرخی->nh_اشاریہ = اشاریہ - پہلا;
    }
    پہلا = آخری;
  }
  curlx_free(سرخیاں);

  انکڑے->state.nh_origin = origin;
  انکڑے->state.nh_request = request;
  انکڑے->state.nh_گنتی = گنتی;
  أرجع TRUE;
}

/* public API */
بنية curl_سرخی *curl_easy_nextheader(CURL *curl,
                                         بنانشانی صحيح origin,
                                         صحيح request,
                                         بنية curl_سرخی *prev)
{
  بنية Curl_eapi_guard guard;
  بنية curl_سرخی *hd = عدم;
  CURLcode نتیجہ;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &نتیجہ)) {
    بنية Curl_easy *انکڑے = curl;
    بنية Curl_llist_گرہ *pick;
    بنية Curl_llist_گرہ *e;
    بنية Curl_سرخی_store *hs;
    size_t amount = 0;
    size_t اشاریہ = 0;
    size_t گنتی;

    إذا(request > انکڑے->state.بینتیاں)
      اذهب باہرلا;
    إذا(request == -1)
      request = انکڑے->state.بینتیاں;

    إذا(prev) {
      pick = prev->anchor;
      إذا(!pick)
        /* something is wrong */
        اذهب باہرلا;
      pick = Curl_گرہ_اگلا(pick);
    }
    وإلا
      pick = Curl_llist_سر(&انکڑے->state.httphdrs);

    إذا(pick) {
      /* make sure it is the next header of the desired type */
      افعل {
        hs = Curl_گرہ_elem(pick);
        إذا((hs->type & origin) && (hs->request == request))
          اكسر;
        pick = Curl_گرہ_اگلا(pick);
      } طالما(pick);
    }

    إذا(!pick)
      /* no more headers available */
      اذهب باہرلا;

    hs = Curl_گرہ_elem(pick);
    گنتی = Curl_llist_گنتی(&انکڑے->state.httphdrs);

    إذا(((انکڑے->state.nh_گنتی == گنتی) &&
        (انکڑے->state.nh_origin == origin) &&
        (انکڑے->state.nh_request == request)) ||
       nextheader_cache_build(انکڑے, origin, request, گنتی)) {
      amount = hs->nh_amount;
      اشاریہ = hs->nh_اشاریہ;
    }
    وإلا {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      لكل(e = Curl_llist_سر(&انکڑے->state.httphdrs); e;
          e = Curl_گرہ_اگلا(e)) {
        بنية Curl_سرخی_store *check = Curl_گرہ_elem(e);
        إذا(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        إذا(e == pick)
          اشاریہ = amount - 1;
      }
    }
    copy_سرخی_external(hs, اشاریہ, amount, pick,
                         &انکڑے->state.headerout[1]);
    hd = &انکڑے->state.headerout[1];
  }
باہرلا:
  CURL_EAPI_LEAVE(&guard);
  أرجع hd;
}

ساكن CURLcode namevalue(حرف *سرخی, size_t hlen, بنانشانی صحيح type,
                          حرف **name, حرف **قدر)
{
  حرف *سرا = سرخی + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = سرخی;

  إذا(type == CURLH_PSEUDO) {
    إذا(*سرخی != ':')
      أرجع CURLE_BAD_FUNCTION_دلیل;
    سرخی++;
  }

  /* Find the end of the header name */
  طالما(*سرخی && (*سرخی != ':'))
    ++سرخی;

  إذا(*سرخی)
    /* Skip over colon, null it */
    *سرخی++ = 0;
  وإلا
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* skip all leading blank letters */
  طالما(ISBLANK(*سرخی))
    سرخی++;

  *قدر = سرخی;

  /* skip all trailing space letters */
  طالما((سرا > سرخی) && ISBLANK(*سرا))
    *سرا-- = 0; /* null-terminate */
  أرجع CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_سرخیاں_ڈالو(بنية Curl_easy *انکڑے, ثابت حرف *سرخی,
                           size_t hlen, /* length of header */
                           بنانشانی حرف type)
{
  حرف *قدر = عدم;
  حرف *name = عدم;
  بنية Curl_سرخی_store *hs;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  ثابت size_t ilen = hlen;

  إذا((سرخی[0] == '\r') || (سرخی[0] == '\n'))
    /* ignore the body separator */
    أرجع CURLE_OK;

  /* trim off newline characters */
  إذا(hlen && (سرخی[hlen - 1] == '\n'))
    hlen--;
  إذا(hlen && (سرخی[hlen - 1] == '\r'))
    hlen--;
  إذا(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(ISBLANK(سرخی[0])) {
    /* pass leading blanks */
    طالما(hlen && ISBLANK(*سرخی)) {
      سرخی++;
      hlen--;
    }
    إذا(!hlen)
      أرجع CURLE_WEIRD_SERVER_جواب;
  }
  إذا(Curl_llist_گنتی(&انکڑے->state.httphdrs) >= MAX_HTTP_RESP_سرخی_گنتی) {
    failf(انکڑے, "Too many response headers, %d is max",
          MAX_HTTP_RESP_سرخی_گنتی);
    أرجع CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, حجم(*hs) + hlen);
  إذا(!hs)
    أرجع CURLE_باہرلا_دا_حافظہ;
  حافظہ_نقل(hs->بفر, سرخی, hlen);
  hs->بفر[hlen] = 0; /* null-terminate */

  نتیجہ = namevalue(hs->بفر, hlen, type, &name, &قدر);
  إذا(!نتیجہ) {
    hs->name = name;
    hs->قدر = قدر;
    hs->type = type;
    hs->request = انکڑے->state.بینتیاں;

    /* insert this node into the list of headers */
    Curl_llist_append(&انکڑے->state.httphdrs, hs, &hs->گرہ);
  }
  وإلا {
    failf(انکڑے, "Invalid response header");
    curlx_free(hs);
  }
  أرجع نتیجہ;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ساكن فراغ سرخیاں_reset(بنية Curl_easy *انکڑے)
{
  Curl_llist_init(&انکڑے->state.httphdrs, عدم);
  انکڑے->state.nh_گنتی = 0;
}

بنية hds_cw_collect_ctx {
  بنية Curl_cwriter super;
};

ساكن CURLcode hds_cw_collect_write(بنية Curl_easy *انکڑے,
                                     بنية Curl_cwriter *writer, صحيح type,
                                     ثابت حرف *buf, size_t blen)
{
  إذا((type & CLIENTWRITE_سرخی) && !(type & CLIENTWRITE_حالت)) {
    بنانشانی حرف htype = (بنانشانی حرف)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_سرخی)));
    CURLcode نتیجہ = Curl_سرخیاں_ڈالو(انکڑے, buf, blen, htype);
    CURL_TRC_WRITE(انکڑے, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحيح)نتیجہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  أرجع Curl_cwriter_write(انکڑے, writer->اگلا, type, buf, blen);
}

ساكن ثابت بنية Curl_cwtype hds_cw_collect = {
  "hds-collect",
  عدم,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية hds_cw_collect_ctx)
};

CURLcode Curl_سرخیاں_init(بنية Curl_easy *انکڑے)
{
  بنية Curl_cwriter *writer;
  CURLcode نتیجہ;

  إذا(انکڑے->conn && (انکڑے->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    إذا(Curl_cwriter_get_by_name(انکڑے, hds_cw_collect.name))
      أرجع CURLE_OK;

    نتیجہ = Curl_cwriter_create(&writer, انکڑے, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    إذا(نتیجہ)
      أرجع نتیجہ;

    نتیجہ = Curl_cwriter_جوڑو(انکڑے, writer);
    إذا(نتیجہ) {
      Curl_cwriter_free(انکڑے, writer);
      أرجع نتیجہ;
    }
  }
  أرجع CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرخیاں_cleanup(بنية Curl_easy *انکڑے)
{
  بنية Curl_llist_گرہ *e;
  بنية Curl_llist_گرہ *n;

  لكل(e = Curl_llist_سر(&انکڑے->state.httphdrs); e; e = n) {
    بنية Curl_سرخی_store *hs = Curl_گرہ_elem(e);
    n = Curl_گرہ_اگلا(e);
    curlx_free(hs);
  }
  سرخیاں_reset(انکڑے);
  أرجع CURLE_OK;
}

#وإلا /* HTTP-disabled builds below */

CURLHcode curl_easy_سرخی(CURL *easy,
                           ثابت حرف *name,
                           size_t اشاریہ,
                           بنانشانی صحيح origin,
                           صحيح request,
                           بنية curl_سرخی **hout)
{
  (فراغ)easy;
  (فراغ)name;
  (فراغ)اشاریہ;
  (فراغ)origin;
  (فراغ)request;
  (فراغ)hout;
  أرجع CURLHE_NOT_BUILT_وچ;
}

بنية curl_سرخی *curl_easy_nextheader(CURL *easy,
                                         بنانشانی صحيح type,
                                         صحيح request,
                                         بنية curl_سرخی *prev)
{
  (فراغ)easy;
  (فراغ)type;
  (فراغ)request;
  (فراغ)prev;
  أرجع عدم;
}
#جے_انت
