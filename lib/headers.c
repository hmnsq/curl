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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرا_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ساكن فراغ copy_سرو_external(بنية Curl_سرو_store *hs,
                                 size_t اشاريو,
                                 size_t amount,
                                 بنية Curl_llist_ڳنڍ *e,
                                 بنية curl_سرو *hout)
{
  بنية curl_سرو *h = hout;
  h->name = hs->name;
  h->قدر = hs->قدر;
  h->amount = amount;
  h->اشاريو = اشاريو;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (بنانشان صحيح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_سرو(CURL *curl,
                           ثابت حرف *name,
                           size_t nameindex,
                           بنانشان صحيح origin,
                           صحيح request,
                           بنية curl_سرو **hout)
{
  بنية Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_سرو, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_llist_ڳنڍ *e_pick = عدم;
    size_t ميلاپ = 0;
    size_t amount = 0;
    بنية Curl_سرو_store *hs = عدم;
    بنية Curl_سرو_store *pick = عدم;
    إذا(!name || !hout || !معلومات ||
       (origin > (CURLH_سرو | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_دليل;
      اذهب ٻاهريون;
    }
    إذا(!Curl_llist_ڳڻپ(&معلومات->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      اذهب ٻاهريون;
    }
    إذا(request > معلومات->state.درخواستون) {
      hresult = CURLHE_NOREQUEST;
      اذهب ٻاهريون;
    }
    إذا(request == -1)
      request = معلومات->state.درخواستون;

    /* we need a first round to count amount of this header */
    لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e; e = Curl_ڳنڍ_ايندڙ(e)) {
      hs = Curl_ڳنڍ_elem(e);
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
      اذهب ٻاهريون;

    إذا(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    وإلا {
      لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e;
          e = Curl_ڳنڍ_ايندڙ(e)) {
        hs = Curl_ڳنڍ_elem(e);
        إذا(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (ميلاپ++ == nameindex)) {
          e_pick = e;
          اكسر;
        }
      }
      إذا(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        اذهب ٻاهريون;
      }
    }
    /* this is the name we want */
    copy_سرو_external(hs, nameindex, amount, e_pick,
                         &معلومات->state.headerout[0]);
    *hout = &معلومات->state.headerout[0];
    hresult = CURLHE_OK;
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  إذا(نتيجو)
    hresult = Curl_eapi_hcode(نتيجو);
  أرجع hresult;
}

بنية nextheader_cache_داخلا {
  بنية Curl_سرو_store *سرو;
  size_t ترتيب;
};

ساكن صحيح nextheader_compare(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية nextheader_cache_داخلا *e1 = p1;
  ثابت بنية nextheader_cache_داخلا *e2 = p2;
  ثابت بنية Curl_سرو_store *h1 = e1->سرو;
  ثابت بنية Curl_سرو_store *h2 = e2->سرو;
  ثابت حرف *n1 = h1->name;
  ثابت حرف *n2 = h2->name;

  طالما(*n1 && *n2) {
    بنانشان حرف c1 = (بنانشان حرف)Curl_raw_toupper(*n1++);
    بنانشان حرف c2 = (بنانشان حرف)Curl_raw_toupper(*n2++);
    إذا(c1 != c2)
      أرجع c1 > c2 ? 1 : -1;
  }
  إذا(*n1)
    أرجع 1;
  إذا(*n2)
    أرجع -1;
  إذا(e1->ترتيب != e2->ترتيب)
    أرجع e1->ترتيب > e2->ترتيب ? 1 : -1;
  أرجع 0;
}

ساكن bool nextheader_cache_build(بنية Curl_easy *معلومات,
                                   بنانشان صحيح origin,
                                   صحيح request, size_t ڳڻپ)
{
  بنية nextheader_cache_داخلا *سرا;
  بنية Curl_llist_ڳنڍ *e;
  size_t i = 0;
  size_t پهريون;

  سرا = curlx_malloc(حجم(*سرا) * ڳڻپ);
  إذا(!سرا)
    أرجع FALSE;

  لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e;
      e = Curl_ڳنڍ_ايندڙ(e)) {
    بنية Curl_سرو_store *hs = Curl_ڳنڍ_elem(e);
    إذا((hs->type & origin) && (hs->request == request)) {
      سرا[i].سرو = hs;
      سرا[i].ترتيب = i;
      i++;
    }
  }

  qsort(سرا, i, حجم(*سرا), nextheader_compare);
  لكل(پهريون = 0; پهريون < i;) {
    size_t آخري = پهريون + 1;
    size_t اشاريو;
    طالما((آخري < i) &&
          curl_strequal(سرا[پهريون].سرو->name,
                        سرا[آخري].سرو->name))
      آخري++;
    لكل(اشاريو = پهريون; اشاريو < آخري; اشاريو++) {
      سرا[اشاريو].سرو->nh_amount = آخري - پهريون;
      سرا[اشاريو].سرو->nh_اشاريو = اشاريو - پهريون;
    }
    پهريون = آخري;
  }
  curlx_free(سرا);

  معلومات->state.nh_origin = origin;
  معلومات->state.nh_request = request;
  معلومات->state.nh_ڳڻپ = ڳڻپ;
  أرجع TRUE;
}

/* public API */
بنية curl_سرو *curl_easy_nextheader(CURL *curl,
                                         بنانشان صحيح origin,
                                         صحيح request,
                                         بنية curl_سرو *prev)
{
  بنية Curl_eapi_guard guard;
  بنية curl_سرو *hd = عدم;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    بنية Curl_llist_ڳنڍ *pick;
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_سرو_store *hs;
    size_t amount = 0;
    size_t اشاريو = 0;
    size_t ڳڻپ;

    إذا(request > معلومات->state.درخواستون)
      اذهب ٻاهريون;
    إذا(request == -1)
      request = معلومات->state.درخواستون;

    إذا(prev) {
      pick = prev->anchor;
      إذا(!pick)
        /* something is wrong */
        اذهب ٻاهريون;
      pick = Curl_ڳنڍ_ايندڙ(pick);
    }
    وإلا
      pick = Curl_llist_سر(&معلومات->state.httphdrs);

    إذا(pick) {
      /* make sure it is the next header of the desired type */
      افعل {
        hs = Curl_ڳنڍ_elem(pick);
        إذا((hs->type & origin) && (hs->request == request))
          اكسر;
        pick = Curl_ڳنڍ_ايندڙ(pick);
      } طالما(pick);
    }

    إذا(!pick)
      /* no more headers available */
      اذهب ٻاهريون;

    hs = Curl_ڳنڍ_elem(pick);
    ڳڻپ = Curl_llist_ڳڻپ(&معلومات->state.httphdrs);

    إذا(((معلومات->state.nh_ڳڻپ == ڳڻپ) &&
        (معلومات->state.nh_origin == origin) &&
        (معلومات->state.nh_request == request)) ||
       nextheader_cache_build(معلومات, origin, request, ڳڻپ)) {
      amount = hs->nh_amount;
      اشاريو = hs->nh_اشاريو;
    }
    وإلا {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e;
          e = Curl_ڳنڍ_ايندڙ(e)) {
        بنية Curl_سرو_store *check = Curl_ڳنڍ_elem(e);
        إذا(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        إذا(e == pick)
          اشاريو = amount - 1;
      }
    }
    copy_سرو_external(hs, اشاريو, amount, pick,
                         &معلومات->state.headerout[1]);
    hd = &معلومات->state.headerout[1];
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع hd;
}

ساكن CURLcode namevalue(حرف *سرو, size_t hlen, بنانشان صحيح type,
                          حرف **name, حرف **قدر)
{
  حرف *پڇاڙي = سرو + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = سرو;

  إذا(type == CURLH_PSEUDO) {
    إذا(*سرو != ':')
      أرجع CURLE_BAD_FUNCTION_دليل;
    سرو++;
  }

  /* Find the end of the header name */
  طالما(*سرو && (*سرو != ':'))
    ++سرو;

  إذا(*سرو)
    /* Skip over colon, null it */
    *سرو++ = 0;
  وإلا
    أرجع CURLE_BAD_FUNCTION_دليل;

  /* skip all leading blank letters */
  طالما(ISBLANK(*سرو))
    سرو++;

  *قدر = سرو;

  /* skip all trailing space letters */
  طالما((پڇاڙي > سرو) && ISBLANK(*پڇاڙي))
    *پڇاڙي-- = 0; /* null-terminate */
  أرجع CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_سرا_ڌڪو(بنية Curl_easy *معلومات, ثابت حرف *سرو,
                           size_t hlen, /* length of header */
                           بنانشان حرف type)
{
  حرف *قدر = عدم;
  حرف *name = عدم;
  بنية Curl_سرو_store *hs;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  ثابت size_t ilen = hlen;

  إذا((سرو[0] == '\r') || (سرو[0] == '\n'))
    /* ignore the body separator */
    أرجع CURLE_OK;

  /* trim off newline characters */
  إذا(hlen && (سرو[hlen - 1] == '\n'))
    hlen--;
  إذا(hlen && (سرو[hlen - 1] == '\r'))
    hlen--;
  إذا(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(ISBLANK(سرو[0])) {
    /* pass leading blanks */
    طالما(hlen && ISBLANK(*سرو)) {
      سرو++;
      hlen--;
    }
    إذا(!hlen)
      أرجع CURLE_WEIRD_SERVER_جواب;
  }
  إذا(Curl_llist_ڳڻپ(&معلومات->state.httphdrs) >= MAX_HTTP_RESP_سرو_ڳڻپ) {
    failf(معلومات, "Too many response headers, %d is max",
          MAX_HTTP_RESP_سرو_ڳڻپ);
    أرجع CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, حجم(*hs) + hlen);
  إذا(!hs)
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  يادگيري_نقل(hs->بفر, سرو, hlen);
  hs->بفر[hlen] = 0; /* null-terminate */

  نتيجو = namevalue(hs->بفر, hlen, type, &name, &قدر);
  إذا(!نتيجو) {
    hs->name = name;
    hs->قدر = قدر;
    hs->type = type;
    hs->request = معلومات->state.درخواستون;

    /* insert this node into the list of headers */
    Curl_llist_append(&معلومات->state.httphdrs, hs, &hs->ڳنڍ);
  }
  وإلا {
    failf(معلومات, "Invalid response header");
    curlx_free(hs);
  }
  أرجع نتيجو;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ساكن فراغ سرا_reset(بنية Curl_easy *معلومات)
{
  Curl_llist_init(&معلومات->state.httphdrs, عدم);
  معلومات->state.nh_ڳڻپ = 0;
}

بنية hds_cw_collect_ctx {
  بنية Curl_cwriter super;
};

ساكن CURLcode hds_cw_collect_write(بنية Curl_easy *معلومات,
                                     بنية Curl_cwriter *writer, صحيح type,
                                     ثابت حرف *buf, size_t blen)
{
  إذا((type & CLIENTWRITE_سرو) && !(type & CLIENTWRITE_حالت)) {
    بنانشان حرف htype = (بنانشان حرف)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_سرو)));
    CURLcode نتيجو = Curl_سرا_ڌڪو(معلومات, buf, blen, htype);
    CURL_TRC_WRITE(معلومات, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحيح)نتيجو);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع Curl_cwriter_write(معلومات, writer->ايندڙ, type, buf, blen);
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

CURLcode Curl_سرا_init(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer;
  CURLcode نتيجو;

  إذا(معلومات->conn && (معلومات->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    إذا(Curl_cwriter_get_by_name(معلومات, hds_cw_collect.name))
      أرجع CURLE_OK;

    نتيجو = Curl_cwriter_create(&writer, معلومات, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    إذا(نتيجو)
      أرجع نتيجو;

    نتيجو = Curl_cwriter_جوڙيو(معلومات, writer);
    إذا(نتيجو) {
      Curl_cwriter_free(معلومات, writer);
      أرجع نتيجو;
    }
  }
  أرجع CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرا_cleanup(بنية Curl_easy *معلومات)
{
  بنية Curl_llist_ڳنڍ *e;
  بنية Curl_llist_ڳنڍ *n;

  لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e; e = n) {
    بنية Curl_سرو_store *hs = Curl_ڳنڍ_elem(e);
    n = Curl_ڳنڍ_ايندڙ(e);
    curlx_free(hs);
  }
  سرا_reset(معلومات);
  أرجع CURLE_OK;
}

#وإلا /* HTTP-disabled builds below */

CURLHcode curl_easy_سرو(CURL *easy,
                           ثابت حرف *name,
                           size_t اشاريو,
                           بنانشان صحيح origin,
                           صحيح request,
                           بنية curl_سرو **hout)
{
  (فراغ)easy;
  (فراغ)name;
  (فراغ)اشاريو;
  (فراغ)origin;
  (فراغ)request;
  (فراغ)hout;
  أرجع CURLHE_NOT_BUILT_اندريون;
}

بنية curl_سرو *curl_easy_nextheader(CURL *easy,
                                         بنانشان صحيح type,
                                         صحيح request,
                                         بنية curl_سرو *prev)
{
  (فراغ)easy;
  (فراغ)type;
  (فراغ)request;
  (فراغ)prev;
  أرجع عدم;
}
#جيڪڏهن_پورو
