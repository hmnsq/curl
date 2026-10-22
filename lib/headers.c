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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرآیندها_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ساكن فراغ copy_سرآیند_external(بنية Curl_سرآیند_store *hs,
                                 size_t شاخص,
                                 size_t amount,
                                 بنية Curl_llist_گره *e,
                                 بنية curl_سرآیند *hout)
{
  بنية curl_سرآیند *h = hout;
  h->name = hs->name;
  h->مقدار = hs->مقدار;
  h->amount = amount;
  h->شاخص = شاخص;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (بیعلامت صحيح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_سرآیند(CURL *curl,
                           ثابت حرف *name,
                           size_t nameindex,
                           بیعلامت صحيح origin,
                           صحيح request,
                           بنية curl_سرآیند **hout)
{
  بنية Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_سرآیند, &نتیجه)) {
    بنية Curl_easy *داده = curl;
    بنية Curl_llist_گره *e;
    بنية Curl_llist_گره *e_pick = عدم;
    size_t تطابق = 0;
    size_t amount = 0;
    بنية Curl_سرآیند_store *hs = عدم;
    بنية Curl_سرآیند_store *pick = عدم;
    إذا(!name || !hout || !داده ||
       (origin > (CURLH_سرآیند | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_آرگومان;
      اذهب بیرون;
    }
    إذا(!Curl_llist_شمار(&داده->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      اذهب بیرون;
    }
    إذا(request > داده->state.درخواستها) {
      hresult = CURLHE_NOREQUEST;
      اذهب بیرون;
    }
    إذا(request == -1)
      request = داده->state.درخواستها;

    /* we need a first round to count amount of this header */
    لكل(e = Curl_llist_سر(&داده->state.httphdrs); e; e = Curl_گره_بعدی(e)) {
      hs = Curl_گره_elem(e);
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
      اذهب بیرون;

    إذا(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    وإلا {
      لكل(e = Curl_llist_سر(&داده->state.httphdrs); e;
          e = Curl_گره_بعدی(e)) {
        hs = Curl_گره_elem(e);
        إذا(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (تطابق++ == nameindex)) {
          e_pick = e;
          اكسر;
        }
      }
      إذا(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        اذهب بیرون;
      }
    }
    /* this is the name we want */
    copy_سرآیند_external(hs, nameindex, amount, e_pick,
                         &داده->state.headerout[0]);
    *hout = &داده->state.headerout[0];
    hresult = CURLHE_OK;
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  إذا(نتیجه)
    hresult = Curl_eapi_hcode(نتیجه);
  أرجع hresult;
}

بنية nextheader_cache_مدخل {
  بنية Curl_سرآیند_store *سرآیند;
  size_t ترتیب;
};

ساكن صحيح nextheader_compare(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية nextheader_cache_مدخل *e1 = p1;
  ثابت بنية nextheader_cache_مدخل *e2 = p2;
  ثابت بنية Curl_سرآیند_store *h1 = e1->سرآیند;
  ثابت بنية Curl_سرآیند_store *h2 = e2->سرآیند;
  ثابت حرف *n1 = h1->name;
  ثابت حرف *n2 = h2->name;

  طالما(*n1 && *n2) {
    بیعلامت حرف c1 = (بیعلامت حرف)Curl_raw_toupper(*n1++);
    بیعلامت حرف c2 = (بیعلامت حرف)Curl_raw_toupper(*n2++);
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

ساكن bool nextheader_cache_build(بنية Curl_easy *داده,
                                   بیعلامت صحيح origin,
                                   صحيح request, size_t شمار)
{
  بنية nextheader_cache_مدخل *سرآیندها;
  بنية Curl_llist_گره *e;
  size_t i = 0;
  size_t اولی;

  سرآیندها = curlx_malloc(حجم(*سرآیندها) * شمار);
  إذا(!سرآیندها)
    أرجع FALSE;

  لكل(e = Curl_llist_سر(&داده->state.httphdrs); e;
      e = Curl_گره_بعدی(e)) {
    بنية Curl_سرآیند_store *hs = Curl_گره_elem(e);
    إذا((hs->type & origin) && (hs->request == request)) {
      سرآیندها[i].سرآیند = hs;
      سرآیندها[i].ترتیب = i;
      i++;
    }
  }

  qsort(سرآیندها, i, حجم(*سرآیندها), nextheader_compare);
  لكل(اولی = 0; اولی < i;) {
    size_t آخرین = اولی + 1;
    size_t شاخص;
    طالما((آخرین < i) &&
          curl_strequal(سرآیندها[اولی].سرآیند->name,
                        سرآیندها[آخرین].سرآیند->name))
      آخرین++;
    لكل(شاخص = اولی; شاخص < آخرین; شاخص++) {
      سرآیندها[شاخص].سرآیند->nh_amount = آخرین - اولی;
      سرآیندها[شاخص].سرآیند->nh_شاخص = شاخص - اولی;
    }
    اولی = آخرین;
  }
  curlx_free(سرآیندها);

  داده->state.nh_origin = origin;
  داده->state.nh_request = request;
  داده->state.nh_شمار = شمار;
  أرجع TRUE;
}

/* public API */
بنية curl_سرآیند *curl_easy_nextheader(CURL *curl,
                                         بیعلامت صحيح origin,
                                         صحيح request,
                                         بنية curl_سرآیند *prev)
{
  بنية Curl_eapi_guard guard;
  بنية curl_سرآیند *hd = عدم;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &نتیجه)) {
    بنية Curl_easy *داده = curl;
    بنية Curl_llist_گره *pick;
    بنية Curl_llist_گره *e;
    بنية Curl_سرآیند_store *hs;
    size_t amount = 0;
    size_t شاخص = 0;
    size_t شمار;

    إذا(request > داده->state.درخواستها)
      اذهب بیرون;
    إذا(request == -1)
      request = داده->state.درخواستها;

    إذا(prev) {
      pick = prev->anchor;
      إذا(!pick)
        /* something is wrong */
        اذهب بیرون;
      pick = Curl_گره_بعدی(pick);
    }
    وإلا
      pick = Curl_llist_سر(&داده->state.httphdrs);

    إذا(pick) {
      /* make sure it is the next header of the desired type */
      افعل {
        hs = Curl_گره_elem(pick);
        إذا((hs->type & origin) && (hs->request == request))
          اكسر;
        pick = Curl_گره_بعدی(pick);
      } طالما(pick);
    }

    إذا(!pick)
      /* no more headers available */
      اذهب بیرون;

    hs = Curl_گره_elem(pick);
    شمار = Curl_llist_شمار(&داده->state.httphdrs);

    إذا(((داده->state.nh_شمار == شمار) &&
        (داده->state.nh_origin == origin) &&
        (داده->state.nh_request == request)) ||
       nextheader_cache_build(داده, origin, request, شمار)) {
      amount = hs->nh_amount;
      شاخص = hs->nh_شاخص;
    }
    وإلا {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      لكل(e = Curl_llist_سر(&داده->state.httphdrs); e;
          e = Curl_گره_بعدی(e)) {
        بنية Curl_سرآیند_store *check = Curl_گره_elem(e);
        إذا(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        إذا(e == pick)
          شاخص = amount - 1;
      }
    }
    copy_سرآیند_external(hs, شاخص, amount, pick,
                         &داده->state.headerout[1]);
    hd = &داده->state.headerout[1];
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع hd;
}

ساكن CURLcode namevalue(حرف *سرآیند, size_t hlen, بیعلامت صحيح type,
                          حرف **name, حرف **مقدار)
{
  حرف *انتها = سرآیند + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = سرآیند;

  إذا(type == CURLH_PSEUDO) {
    إذا(*سرآیند != ':')
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    سرآیند++;
  }

  /* Find the end of the header name */
  طالما(*سرآیند && (*سرآیند != ':'))
    ++سرآیند;

  إذا(*سرآیند)
    /* Skip over colon, null it */
    *سرآیند++ = 0;
  وإلا
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* skip all leading blank letters */
  طالما(ISBLANK(*سرآیند))
    سرآیند++;

  *مقدار = سرآیند;

  /* skip all trailing space letters */
  طالما((انتها > سرآیند) && ISBLANK(*انتها))
    *انتها-- = 0; /* null-terminate */
  أرجع CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_سرآیندها_افزودن(بنية Curl_easy *داده, ثابت حرف *سرآیند,
                           size_t hlen, /* length of header */
                           بیعلامت حرف type)
{
  حرف *مقدار = عدم;
  حرف *name = عدم;
  بنية Curl_سرآیند_store *hs;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  ثابت size_t ilen = hlen;

  إذا((سرآیند[0] == '\r') || (سرآیند[0] == '\n'))
    /* ignore the body separator */
    أرجع CURLE_OK;

  /* trim off newline characters */
  إذا(hlen && (سرآیند[hlen - 1] == '\n'))
    hlen--;
  إذا(hlen && (سرآیند[hlen - 1] == '\r'))
    hlen--;
  إذا(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    أرجع CURLE_WEIRD_SERVER_پاسخ;

  إذا(ISBLANK(سرآیند[0])) {
    /* pass leading blanks */
    طالما(hlen && ISBLANK(*سرآیند)) {
      سرآیند++;
      hlen--;
    }
    إذا(!hlen)
      أرجع CURLE_WEIRD_SERVER_پاسخ;
  }
  إذا(Curl_llist_شمار(&داده->state.httphdrs) >= MAX_HTTP_RESP_سرآیند_شمار) {
    failf(داده, "Too many response headers, %d is max",
          MAX_HTTP_RESP_سرآیند_شمار);
    أرجع CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, حجم(*hs) + hlen);
  إذا(!hs)
    أرجع CURLE_بیرون_از_حافظه;
  کپی_حافظه(hs->میانگیر, سرآیند, hlen);
  hs->میانگیر[hlen] = 0; /* null-terminate */

  نتیجه = namevalue(hs->میانگیر, hlen, type, &name, &مقدار);
  إذا(!نتیجه) {
    hs->name = name;
    hs->مقدار = مقدار;
    hs->type = type;
    hs->request = داده->state.درخواستها;

    /* insert this node into the list of headers */
    Curl_llist_append(&داده->state.httphdrs, hs, &hs->گره);
  }
  وإلا {
    failf(داده, "Invalid response header");
    curlx_free(hs);
  }
  أرجع نتیجه;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ساكن فراغ سرآیندها_reset(بنية Curl_easy *داده)
{
  Curl_llist_init(&داده->state.httphdrs, عدم);
  داده->state.nh_شمار = 0;
}

بنية hds_cw_collect_ctx {
  بنية Curl_cwriter super;
};

ساكن CURLcode hds_cw_collect_write(بنية Curl_easy *داده,
                                     بنية Curl_cwriter *writer, صحيح type,
                                     ثابت حرف *buf, size_t blen)
{
  إذا((type & CLIENTWRITE_سرآیند) && !(type & CLIENTWRITE_وضعیت)) {
    بیعلامت حرف htype = (بیعلامت حرف)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_سرآیند)));
    CURLcode نتیجه = Curl_سرآیندها_افزودن(داده, buf, blen, htype);
    CURL_TRC_WRITE(داده, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحيح)نتیجه);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع Curl_cwriter_write(داده, writer->بعدی, type, buf, blen);
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

CURLcode Curl_سرآیندها_init(بنية Curl_easy *داده)
{
  بنية Curl_cwriter *writer;
  CURLcode نتیجه;

  إذا(داده->conn && (داده->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    إذا(Curl_cwriter_get_by_name(داده, hds_cw_collect.name))
      أرجع CURLE_OK;

    نتیجه = Curl_cwriter_create(&writer, داده, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    إذا(نتیجه)
      أرجع نتیجه;

    نتیجه = Curl_cwriter_جمع_کن(داده, writer);
    إذا(نتیجه) {
      Curl_cwriter_free(داده, writer);
      أرجع نتیجه;
    }
  }
  أرجع CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرآیندها_cleanup(بنية Curl_easy *داده)
{
  بنية Curl_llist_گره *e;
  بنية Curl_llist_گره *n;

  لكل(e = Curl_llist_سر(&داده->state.httphdrs); e; e = n) {
    بنية Curl_سرآیند_store *hs = Curl_گره_elem(e);
    n = Curl_گره_بعدی(e);
    curlx_free(hs);
  }
  سرآیندها_reset(داده);
  أرجع CURLE_OK;
}

#وإلا /* HTTP-disabled builds below */

CURLHcode curl_easy_سرآیند(CURL *easy,
                           ثابت حرف *name,
                           size_t شاخص,
                           بیعلامت صحيح origin,
                           صحيح request,
                           بنية curl_سرآیند **hout)
{
  (فراغ)easy;
  (فراغ)name;
  (فراغ)شاخص;
  (فراغ)origin;
  (فراغ)request;
  (فراغ)hout;
  أرجع CURLHE_NOT_BUILT_درون;
}

بنية curl_سرآیند *curl_easy_nextheader(CURL *easy,
                                         بیعلامت صحيح type,
                                         صحيح request,
                                         بنية curl_سرآیند *prev)
{
  (فراغ)easy;
  (فراغ)type;
  (فراغ)request;
  (فراغ)prev;
  أرجع عدم;
}
#پایان_اگر
