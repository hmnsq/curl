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

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "connect.h"

#define MAX_ALTSVC_पङ्क्तिः    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#जइ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
स्थिर time_t altsvc_debugtime(शून्य *unused)
{
  ध्रुव वर्ण *timestr = getenv("CURL_TIME");
  (शून्य)unused;
  जइ(timestr) {
    curl_off_t val;
    curlx_str_सङ्ख्या(&timestr, &val, TIME_T_MAX);
    प्रत्यागम (time_t)val;
  }
  प्रत्यागम time(रिक्त);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
ध्रुव वर्ण *Curl_alpnid2str(गणना alpnid id)
{
  विकल्प(id) {
  पक्ष ALPN_h1:
    प्रत्यागम "h1";
  पक्ष ALPN_h2:
    प्रत्यागम "h2";
  पक्ष ALPN_h3:
    प्रत्यागम H3VERSION;
  स्वभाव:
    प्रत्यागम ""; /* bad */
  }
}

स्थिर गणना alpnid Curl_str2alpnid(ध्रुव संरचना Curl_str *cstr)
{
  प्रत्यागम Curl_alpn2alpnid((ध्रुव unsigned वर्ण *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

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
  जइ((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  अण्णहा जइ(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  जइ((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  जइ(!hlen || !dlen)
    /* bad input */
    प्रत्यागम रिक्त;
  /* struct size plus both strings */
  as = curlx_calloc(1, परिमाण(संरचना altsvc) + (hlen + 1) + (dlen + 1));
  जइ(!as)
    प्रत्यागम रिक्त;
  as->src.host = (वर्ण *)as + परिमाण(संरचना altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (वर्ण *)as + परिमाण(संरचना altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

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
  जइ(!srcalpnid || !dstalpnid)
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
  जाव(Curl_llist_गणनम्(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    संरचना Curl_llist_ग्रन्थिः *e = Curl_llist_head(&asi->list);
    संरचना altsvc *oldas = Curl_ग्रन्थिः_elem(e);
    Curl_ग्रन्थिः_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ग्रन्थिः);
}

/* only returns SERIOUS errors */
स्थिर CURLcode altsvc_add(संरचना altsvcinfo *asi, ध्रुव वर्ण *पङ्क्तिः)
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

  जइ(curlx_str_word(&पङ्क्तिः, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
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
  अण्णहा {
    वर्ण dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(रिक्त);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    जइ(now < expires) {
      संरचना altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      जइ(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      अण्णहा
        प्रत्यागम CURLE_OUT_OF_MEMORY;
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
स्थिर CURLcode altsvc_load(संरचना altsvcinfo *asi, ध्रुव वर्ण *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  जइ(!asi->filename)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  जइ(fp) {
    curlx_struct_stat stat;
    जइ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      संरचना dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_पङ्क्तिः);
      करेहि {
        result = Curl_get_पङ्क्तिः(&buf, fp, &eof);
        जइ(!result) {
          ध्रुव वर्ण *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          जइ(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } जाव(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  प्रत्यागम result;
}

/*
 * Write this single altsvc entry to a single output line
 */
स्थिर CURLcode altsvc_out(संरचना altsvc *as, FILE *fp)
{
  संरचना tm stamp;
  ध्रुव वर्ण *dst6_pre = "";
  ध्रुव वर्ण *dst6_post = "";
  ध्रुव वर्ण *src6_pre = "";
  ध्रुव वर्ण *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  जइ(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    प्रत्यागम CURLE_OK;
#ifdef USE_IPV6
  अण्णहा {
    वर्ण ipv6_unused[16];
    जइ(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    जइ(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#endif
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.port,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.port,

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
  जइ(!asi)
    प्रत्यागम रिक्त;
  Curl_llist_init(&asi->list, रिक्त);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  प्रत्यागम asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(संरचना altsvcinfo *asi, ध्रुव वर्ण *file)
{
  DEBUGASSERT(asi);
  प्रत्यागम altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(संरचना Curl_easy *data, ध्रुव long ctrl)
{
  DEBUGASSERT(data);
  जइ(!ctrl)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  जइ(!data->asi) {
    data->asi = Curl_altsvc_init();
    जइ(!data->asi)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  प्रत्यागम CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
शून्य Curl_altsvc_cleanup(संरचना altsvcinfo **asi)
{
  जइ(*asi) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *n;
    संरचना altsvcinfo *altsvc = *asi;
    प्रति(e = Curl_llist_head(&altsvc->list); e; e = n) {
      संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
      n = Curl_ग्रन्थिः_next(e);
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
CURLcode Curl_altsvc_save(संरचना Curl_easy *data,
                          संरचना altsvcinfo *asi, ध्रुव वर्ण *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  वर्ण *tempstore = रिक्त;

  जइ(!asi)
    /* no cache activated */
    प्रत्यागम CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  जइ(!file && asi->filename)
    file = asi->filename;

  जइ((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    प्रत्यागम CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  जइ(!result) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    प्रति(e = Curl_llist_head(&asi->list); e; e = n) {
      संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
      n = Curl_ग्रन्थिः_next(e);
      result = altsvc_out(as, out);
      जइ(result)
        भङ्ग;
    }
    curlx_fclose(out);
    जइ(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    जइ(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  प्रत्यागम result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
स्थिर bool hostcompare(ध्रुव वर्ण *host, ध्रुव वर्ण *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  जइ(hlen && (host[hlen - 1] == '.'))
    hlen--;
  जइ(hlen != clen)
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
  प्रति(e = Curl_llist_head(&asi->list); e; e = n) {
    संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
    n = Curl_ग्रन्थिः_next(e);
    जइ((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ग्रन्थिः_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
स्थिर शून्य altsvc_parse_params(ध्रुव वर्ण **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  जइ(curlx_str_single(pp, ';'))
    प्रत्यागम;

  प्रति(;;) {
    संरचना Curl_str name;
    संरचना Curl_str val;
    curl_off_t अङ्कः;
    पूर्णाङ्क keyword = 0;

    /* allow some extra whitespaces around name and value */
    जइ(curlx_str_until(pp, &name, MAX_ALTSVC_पङ्क्तिः, '=') ||
       curlx_str_single(pp, '='))
      भङ्ग;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    जइ(**pp == '\"') {
      जइ(curlx_str_quotedword(pp, &val, MAX_ALTSVC_पङ्क्तिः))
        भङ्ग;
    }
    अण्णहा {
      जइ(curlx_str_cspn(pp, &val, ",;\r\n"))
        भङ्ग;
    }
    curlx_str_trimblanks(&val);

    जइ(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    अण्णहा जइ(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    जइ(keyword) {
      ध्रुव वर्ण *vp = curlx_str(&val);
      ध्रुव वर्ण *vend = vp + curlx_strlen(&val);
      जइ(curlx_str_सङ्ख्या(&vp, &अङ्कः, CURL_OFF_T_MAX))
        भङ्ग; /* not a number, skip further parameter parsing */
      जइ(vp != vend)
        भङ्ग; /* not entirely a number, skip further parameter parsing */
      जइ(keyword == ALTSVC_MA) {
        जइ(अङ्कः > CAP_ALTSVC_MAX_AGE)
          अङ्कः = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)अङ्कः;
      }
      अण्णहा जइ(अङ्कः == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    जइ(curlx_str_single(pp, ';'))
      भङ्ग; /* no further parameters */
  }
}

स्थिर bool altsvc_parse_dest(ध्रुव वर्ण **pp,
                              संरचना Curl_easy *data,
                              संरचना Curl_peer *origin,
                              संरचना Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  जइ(curlx_str_single(pp, '\"'))
    प्रत्यागम FALSE;

  /* quoted string, with hostname or just :port ? */
  जइ(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    जइ(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      जइ(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        प्रत्यागम FALSE;
      }
    }
    अण्णहा { /* IPv6 hostname */
      जइ(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        प्रत्यागम FALSE;
      }
    }
    जइ(curlx_str_single(pp, ':'))
      प्रत्यागम FALSE; /* not followed by ':' */
  }
  अण्णहा  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  जइ(curlx_str_सङ्ख्या(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    प्रत्यागम FALSE;
  }

  *pdstport = (uint16_t)port;
  जइ(curlx_str_single(pp, '\"'))
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
CURLcode Curl_altsvc_parse(संरचना Curl_easy *data,
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
  जइ(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    जइ(curlx_str_casecompare(&alpn, "clear")) {
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

    जइ(curlx_str_until(&p, &alpn, MAX_ALTSVC_पङ्क्तिः, '='))
      भङ्ग; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    जइ(curlx_str_single(&p, '='))
      भङ्ग;

    /* Parse altsvc hostname:port */
    जइ(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      भङ्ग;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    जइ(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      जइ(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      जइ(as) {
        time_t secs = time(रिक्त);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        जइ(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        अण्णहा
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (पूर्णाङ्क)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      अण्णहा
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    जइ(curlx_str_single(&p, ','))
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

  जइ(Curl_llist_गणनम्(&asi->list)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना Curl_llist_ग्रन्थिः *n;
    time_t now = time(रिक्त);

    प्रति(e = Curl_llist_head(&asi->list); e; e = n) {
      संरचना altsvc *as = Curl_ग्रन्थिः_elem(e);
      n = Curl_ग्रन्थिः_next(e);
      जइ(as->expires < now) {
        /* an expired entry, remove */
        Curl_ग्रन्थिः_remove(e);
        altsvc_free(as);
        अनुवर्त;
      }
      जइ((origin_alpnid == as->src.alpnid) &&
         (versions & (पूर्णाङ्क)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        प्रत्यागम TRUE;
      }
    }
  }
  प्रत्यागम FALSE;
}

#जइ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
