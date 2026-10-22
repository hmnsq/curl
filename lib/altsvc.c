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
/*
 * The Alt-Svc: header is defined in RFC 7838:
 * https://datatracker.ietf.org/doc/html/rfc7838
 */
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "connect.h"

#परिभाषय MAX_ALTSVC_पङ्क्तिः    4095
#परिभाषय MAX_ALTSVC_DATELEN 17
#परिभाषय MAX_ALTSVC_HOSTLEN 2048
#परिभाषय MAX_ALTSVC_ALPNLEN 10

#परिभाषय H3_संस्करणम् "h3"

#यदि defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
स्थिर time_t altsvc_debugtime(शून्य *अप्रयुक्तम्)
{
  ध्रुव वर्ण *timestr = getenv("CURL_TIME");
  (शून्य)अप्रयुक्तम्;
  यदि(timestr) {
    curl_off_t val;
    curlx_str_सङ्ख्या(&timestr, &val, TIME_T_MAX);
    प्रत्यागम (time_t)val;
  }
  प्रत्यागम time(रिक्त);
}
#अपरिभाषय time
#परिभाषय time(x) altsvc_debugtime(x)
#यदि_अन्तः

/* Given the ALPN ID, return the name */
ध्रुव वर्ण *Curl_alpnid2str(गणना alpnid id)
{
  विकल्प(id) {
  पक्ष ALPN_h1:
    प्रत्यागम "h1";
  पक्ष ALPN_h2:
    प्रत्यागम "h2";
  पक्ष ALPN_h3:
    प्रत्यागम H3_संस्करणम्;
  स्वभाव:
    प्रत्यागम ""; /* bad */
  }
}

स्थिर गणना alpnid Curl_str2alpnid(ध्रुव संरचना Curl_str *cstr)
{
  प्रत्यागम Curl_alpn2alpnid((ध्रुव अचिह्नित वर्ण *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#परिभाषय altsvc_free(x) curlx_free(x)

स्थिर संरचना altsvc *altsvc_createid(ध्रुव वर्ण *srchost,
                                      size_t hlen,
                                      ध्रुव वर्ण *dsthost,
                                      size_t dlen, /* dsthost length */
                                      गणना alpnid srcalpnid,
                                      गणना alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  संरचना altsvc *as;
  यदि((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  अन्यथा यदि(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  यदि((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  यदि(!hlen || !dlen)
    /* bad input */
    प्रत्यागम रिक्त;
  /* struct size plus both strings */
  as = curlx_calloc(1, परिमाण(संरचना altsvc) + (hlen + 1) + (dlen + 1));
  यदि(!as)
    प्रत्यागम रिक्त;
  as->src.host = (वर्ण *)as + परिमाण(संरचना altsvc);
  स्मृतिं_प्रतिलिख(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (वर्ण *)as + परिमाण(संरचना altsvc) + hlen + 1;
  स्मृतिं_प्रतिलिख(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.पत्तनम् = (अचिह्नित ह्रस्व)srcport;
  as->dst.पत्तनम् = (अचिह्नित ह्रस्व)dstport;

  प्रत्यागम as;
}

स्थिर संरचना altsvc *altsvc_create(संरचना Curl_str *srchost,
                                    संरचना Curl_str *dsthost,
                                    संरचना Curl_str *srcalpn,
                                    संरचना Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  गणना alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  गणना alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  यदि(!srcalpnid || !dstalpnid)
    प्रत्यागम रिक्त;
  प्रत्यागम altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
स्थिर शून्य altsvc_append(संरचना altsvcinfo *asi, संरचना altsvc *as)
{
  यावत्(Curl_llist_गणनम्(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    संरचना Curl_llist_ग्रन्थिः *e = Curl_llist_शीर्ष(&asi->list);
    संरचना altsvc *oldas = Curl_ग्रन्थिः_elem(e);
    Curl_ग्रन्थिः_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ग्रन्थिः);
}

/* only returns SERIOUS errors */
स्थिर CURLcode altsvc_सङ्कलय(संरचना altsvcinfo *asi, ध्रुव वर्ण *पङ्क्तिः)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  संरचना Curl_str srchost;
  संरचना Curl_str dsthost;
  संरचना Curl_str srcalpn;
  संरचना Curl_str dstalpn;
  संरचना Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  यदि(curlx_str_word(&पङ्क्तिः, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_word(&पङ्क्तिः, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_सङ्ख्या(&पङ्क्तिः, &srcport, 65535) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_word(&पङ्क्तिः, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_word(&पङ्क्तिः, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_सङ्ख्या(&पङ्क्तिः, &dstport, 65535) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_quotedword(&पङ्क्तिः, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_सङ्ख्या(&पङ्क्तिः, &persist, 1) ||
     curlx_str_singlespace(&पङ्क्तिः) ||
     curlx_str_सङ्ख्या(&पङ्क्तिः, &prio, 0) ||
     curlx_str_newline(&पङ्क्तिः))
    ;
  अन्यथा {
    वर्ण dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(रिक्त);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    स्मृतिं_प्रतिलिख(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    यदि(now < expires) {
      संरचना altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      यदि(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      अन्यथा
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  प्रत्यागम CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
स्थिर CURLcode altsvc_आरोपय(संरचना altsvcinfo *asi, ध्रुव वर्ण *file)
{
  CURLcode परिणाम = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  यदि(!asi->filename)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  यदि(fp) {
    curlx_struct_stat stat;
    यदि((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_बहुलकम्)) {
      bool eof = FALSE;
      संरचना dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_पङ्क्तिः);
      कुरु {
        परिणाम = Curl_get_पङ्क्तिः(&buf, fp, &eof);
        यदि(!परिणाम) {
          ध्रुव वर्ण *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          यदि(curlx_str_single(&lineptr, '#'))
            altsvc_सङ्कलय(asi, lineptr);
        }
      } यावत्(!परिणाम && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  प्रत्यागम परिणाम;
}

/*
 * Write this single altsvc entry to a single output line
 */
स्थिर CURLcode altsvc_बहिर्गमनम्(संरचना altsvc *as, FILE *fp)
{
  संरचना tm stamp;
  ध्रुव वर्ण *dst6_pre = "";
  ध्रुव वर्ण *dst6_post = "";
  ध्रुव वर्ण *src6_pre = "";
  ध्रुव वर्ण *src6_post = "";
  CURLcode परिणाम = curlx_gmtime(as->expires, &stamp);
  यदि(परिणाम)
    /* fail silently for date errors so that only this entry gets
       skipped */
    प्रत्यागम CURLE_OK;
#यदि_परिभाषितम् USE_IPV6
  अन्यथा {
    वर्ण ipv6_अप्रयुक्तम्[16];
    यदि(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_अप्रयुक्तम्) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    यदि(curlx_inet_pton(AF_INET6, as->src.host, ipv6_अप्रयुक्तम्) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#यदि_अन्तः
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.पत्तनम्,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.पत्तनम्,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  प्रत्यागम CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
संरचना altsvcinfo *Curl_altsvc_init(शून्य)
{
  संरचना altsvcinfo *asi = curlx_calloc(1, परिमाण(संरचना altsvcinfo));
  यदि(!asi)
    प्रत्यागम रिक्त;
  Curl_llist_init(&asi->list, रिक्त);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#यदि_परिभाषितम् USE_HTTP2
    | CURLALTSVC_H2
#यदि_अन्तः
#यदि_परिभाषितम् USE_HTTP3
    | CURLALTSVC_H3
#यदि_अन्तः
    ;
  प्रत्यागम asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_आरोपय(संरचना altsvcinfo *asi, ध्रुव वर्ण *file)
{
  DEBUGASSERT(asi);
  प्रत्यागम altsvc_आरोपय(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(संरचना Curl_easy *दत्त, ध्रुव दीर्घ ctrl)
{
  DEBUGASSERT(दत्त);
  यदि(!ctrl)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(!दत्त->asi) {
    दत्त->asi = Curl_altsvc_init();
    यदि(!दत्त->asi)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  दत्त->asi->flags = ctrl;
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
शून्य Curl_altsvc_cleanup(संरचना altsvcinfo **asi)
{
  यदि(*asi) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *n;
    संरचना altsvcinfo *altsvc = *asi;
    प्रति(e = Curl_llist_शीर्ष(&altsvc->list); e; e = n) {
      संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
      n = Curl_ग्रन्थिः_अग्र(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = रिक्त; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(संरचना Curl_easy *दत्त,
                          संरचना altsvcinfo *asi, ध्रुव वर्ण *file)
{
  CURLcode परिणाम = CURLE_OK;
  FILE *बहिर्गमनम्;
  वर्ण *tempstore = रिक्त;

  यदि(!asi)
    /* no cache activated */
    प्रत्यागम CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  यदि(!file && asi->filename)
    file = asi->filename;

  यदि((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_fopen(दत्त, file, &बहिर्गमनम्, &tempstore);
  यदि(!परिणाम) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          बहिर्गमनम्);
    प्रति(e = Curl_llist_शीर्ष(&asi->list); e; e = n) {
      संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
      n = Curl_ग्रन्थिः_अग्र(e);
      परिणाम = altsvc_बहिर्गमनम्(as, बहिर्गमनम्);
      यदि(परिणाम)
        भङ्ग;
    }
    curlx_fclose(बहिर्गमनम्);
    यदि(!परिणाम && tempstore && curlx_rename(tempstore, file))
      परिणाम = CURLE_WRITE_ERROR;

    यदि(परिणाम && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  प्रत्यागम परिणाम;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
स्थिर bool hostcompare(ध्रुव वर्ण *host, ध्रुव वर्ण *check)
{
  size_t hlen = वाक्यदैर्घ्यम्(host);
  size_t clen = वाक्यदैर्घ्यम्(check);

  यदि(hlen && (host[hlen - 1] == '.'))
    hlen--;
  यदि(hlen != clen)
    /* they cannot match if they have different lengths */
    प्रत्यागम FALSE;
  प्रत्यागम curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
स्थिर शून्य altsvc_flush(संरचना altsvcinfo *asi,
                         संरचना Curl_peer *origin,
                         गणना alpnid origin_alpnid)
{
  संरचना Curl_llist_ग्रन्थिः *e;
  संरचना Curl_llist_ग्रन्थिः *n;
  प्रति(e = Curl_llist_शीर्ष(&asi->list); e; e = n) {
    संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
    n = Curl_ग्रन्थिः_अग्र(e);
    यदि((origin_alpnid == as->src.alpnid) &&
       (origin->पत्तनम् == as->src.पत्तनम्) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ग्रन्थिः_remove(e);
      altsvc_free(as);
    }
  }
}

#परिभाषय CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#परिभाषय ALTSVC_MA      1
#परिभाषय ALTSVC_PERSIST 2
स्थिर शून्य altsvc_parse_params(ध्रुव वर्ण **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  यदि(curlx_str_single(pp, ';'))
    प्रत्यागम;

  प्रति(;;) {
    संरचना Curl_str name;
    संरचना Curl_str val;
    curl_off_t अङ्कः;
    पूर्णाङ्क keyword = 0;

    /* allow some extra whitespaces around name and value */
    यदि(curlx_str_until(pp, &name, MAX_ALTSVC_पङ्क्तिः, '=') ||
       curlx_str_single(pp, '='))
      भङ्ग;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    यदि(**pp == '\"') {
      यदि(curlx_str_quotedword(pp, &val, MAX_ALTSVC_पङ्क्तिः))
        भङ्ग;
    }
    अन्यथा {
      यदि(curlx_str_cspn(pp, &val, ",;\r\n"))
        भङ्ग;
    }
    curlx_str_trimblanks(&val);

    यदि(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    अन्यथा यदि(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    यदि(keyword) {
      ध्रुव वर्ण *vp = curlx_str(&val);
      ध्रुव वर्ण *vend = vp + curlx_strlen(&val);
      यदि(curlx_str_सङ्ख्या(&vp, &अङ्कः, CURL_OFF_T_MAX))
        भङ्ग; /* not a number, skip further parameter parsing */
      यदि(vp != vend)
        भङ्ग; /* not entirely a number, skip further parameter parsing */
      यदि(keyword == ALTSVC_MA) {
        यदि(अङ्कः > CAP_ALTSVC_MAX_AGE)
          अङ्कः = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)अङ्कः;
      }
      अन्यथा यदि(अङ्कः == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    यदि(curlx_str_single(pp, ';'))
      भङ्ग; /* no further parameters */
  }
}

स्थिर bool altsvc_parse_dest(ध्रुव वर्ण **pp,
                              संरचना Curl_easy *दत्त,
                              संरचना Curl_peer *origin,
                              संरचना Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t पत्तनम् = 0;

  यदि(curlx_str_single(pp, '\"'))
    प्रत्यागम FALSE;

  /* quoted string, with hostname or just :port ? */
  यदि(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    यदि(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      यदि(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(दत्त, "Bad alt-svc hostname, ignoring.");
        प्रत्यागम FALSE;
      }
    }
    अन्यथा { /* IPv6 hostname */
      यदि(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(दत्त, "Bad alt-svc IPv6 hostname, ignoring.");
        प्रत्यागम FALSE;
      }
    }
    यदि(curlx_str_single(pp, ':'))
      प्रत्यागम FALSE; /* not followed by ':' */
  }
  अन्यथा  /* is only :port, hostname is effectively origin */
    curlx_str_नियोजय(dsthost, origin->hostname,
                     वाक्यदैर्घ्यम्(origin->hostname));

  यदि(curlx_str_सङ्ख्या(pp, &पत्तनम्, 0xffff)) {
    infof(दत्त, "Unknown alt-svc port number, ignoring.");
    प्रत्यागम FALSE;
  }

  *pdstport = (uint16_t)पत्तनम्;
  यदि(curlx_str_single(pp, '\"'))
    प्रत्यागम FALSE; /* quoted string not ending here as expected */
  प्रत्यागम TRUE;
}

/*
 * Curl_altsvc_parse() takes an incoming alt-svc response header and stores
 * the data correctly in the cache.
 *
 * 'value' points to the header *value*. That is contents to the right of the
 * header name.
 *
 * Currently this function rejects invalid data without returning an error.
 * Invalid hostname, port number will result in the specific alternative
 * being rejected. Unknown protocols are skipped.
 */
CURLcode Curl_altsvc_parse(संरचना Curl_easy *दत्त,
                           संरचना altsvcinfo *asi, ध्रुव वर्ण *मानम्,
                           संरचना Curl_peer *origin,
                           गणना alpnid origin_alpnid)
{
  संरचना altsvc *as;
  size_t entries = 0;
  संरचना Curl_str alpn;
  ध्रुव वर्ण *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = मानम्;
  यदि(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    यदि(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      प्रत्यागम CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  प्रति(p = मानम्; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    गणना alpnid dstalpnid;
    संरचना Curl_str dsthost;
    uint16_t dstport;

    यदि(curlx_str_until(&p, &alpn, MAX_ALTSVC_पङ्क्तिः, '='))
      भङ्ग; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    यदि(curlx_str_single(&p, '='))
      भङ्ग;

    /* Parse altsvc hostname:port */
    यदि(!altsvc_parse_dest(&p, दत्त, origin, &dsthost, &dstport))
      भङ्ग;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    यदि(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      यदि(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, वाक्यदैर्घ्यम्(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->पत्तनम्, dstport);
      यदि(as) {
        time_t secs = time(रिक्त);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        यदि(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        अन्यथा
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(दत्त, "Added alt-svc: %.*s:%u over %s",
              (पूर्णाङ्क)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      अन्यथा
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    /* When this is followed by a comma, we expect another entry */
    यदि(curlx_str_single(&p, ','))
      भङ्ग;
  }

  प्रत्यागम CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(संरचना altsvcinfo *asi,
                        संरचना Curl_peer *origin,
                        गणना alpnid origin_alpnid,
                        संरचना altsvc **dstentry,
                        ध्रुव पूर्णाङ्क versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  यदि(Curl_llist_गणनम्(&asi->list)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *n;
    time_t now = time(रिक्त);

    प्रति(e = Curl_llist_शीर्ष(&asi->list); e; e = n) {
      संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
      n = Curl_ग्रन्थिः_अग्र(e);
      यदि(as->expires < now) {
        /* an expired entry, remove */
        Curl_ग्रन्थिः_remove(e);
        altsvc_free(as);
        अनुवर्त;
      }
      यदि((origin_alpnid == as->src.alpnid) &&
         (versions & (पूर्णाङ्क)as->dst.alpnid) &&
         (origin->पत्तनम् == as->src.पत्तनम्) &&
         hostcompare(origin->hostname, as->src.host)) {
        /*मेलनम्*/
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->पत्तनम् == as->dst.पत्तनम्) &&
                             hostcompare(origin->hostname, as->dst.host);
        प्रत्यागम TRUE;
      }
    }
  }
  प्रत्यागम FALSE;
}

#यदि defined(DEBUGBUILD) || defined(UNITTESTS)
#अपरिभाषय time
#यदि_अन्तः

#यदि_अन्तः /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
