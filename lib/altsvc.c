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

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "connect.h"

#define MAX_ALTSVC_ओळ    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#जर defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
स्थिर समय_प्रकार altsvc_debugtime(शून्य *unused)
{
  स्थिरांक अक्षर *timestr = दो_पर्या("CURL_TIME");
  (शून्य)unused;
  जर(timestr) {
    curl_off_t val;
    curlx_str_संख्या(&timestr, &val, TIME_T_MAX);
    परत (समय_प्रकार)val;
  }
  परत समय(रिक्त);
}
#undef समय
#define समय(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
स्थिरांक अक्षर *Curl_alpnid2str(गणना alpnid id)
{
  निवड(id) {
  स्थिती ALPN_h1:
    परत "h1";
  स्थिती ALPN_h2:
    परत "h2";
  स्थिती ALPN_h3:
    परत H3VERSION;
  पूर्वनिर्धारित:
    परत ""; /* bad */
  }
}

स्थिर गणना alpnid Curl_str2alpnid(स्थिरांक संरचना Curl_str *cstr)
{
  परत Curl_alpn2alpnid((स्थिरांक अचिन्हित अक्षर *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

स्थिर संरचना altsvc *altsvc_createid(स्थिरांक अक्षर *srchost,
                                      माप_प्रकार hlen,
                                      स्थिरांक अक्षर *dsthost,
                                      माप_प्रकार dlen, /* dsthost length */
                                      गणना alpnid srcalpnid,
                                      गणना alpnid dstalpnid,
                                      माप_प्रकार srcport,
                                      माप_प्रकार dstport)
{
  संरचना altsvc *as;
  जर((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  अन्यथा जर(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  जर((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  जर(!hlen || !dlen)
    /* bad input */
    परत रिक्त;
  /* struct size plus both strings */
  as = curlx_calloc(1, आकार(संरचना altsvc) + (hlen + 1) + (dlen + 1));
  जर(!as)
    परत रिक्त;
  as->src.host = (अक्षर *)as + आकार(संरचना altsvc);
  स_नकल(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (अक्षर *)as + आकार(संरचना altsvc) + hlen + 1;
  स_नकल(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (अचिन्हित लघु)srcport;
  as->dst.port = (अचिन्हित लघु)dstport;

  परत as;
}

स्थिर संरचना altsvc *altsvc_create(संरचना Curl_str *srchost,
                                    संरचना Curl_str *dsthost,
                                    संरचना Curl_str *srcalpn,
                                    संरचना Curl_str *dstalpn,
                                    माप_प्रकार srcport,
                                    माप_प्रकार dstport)
{
  गणना alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  गणना alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  जर(!srcalpnid || !dstalpnid)
    परत रिक्त;
  परत altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
स्थिर शून्य altsvc_append(संरचना altsvcinfo *asi, संरचना altsvc *as)
{
  पर्यंत(Curl_llist_मोजणी(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    संरचना Curl_llist_गाठ *e = Curl_llist_head(&asi->list);
    संरचना altsvc *oldas = Curl_गाठ_elem(e);
    Curl_गाठ_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->गाठ);
}

/* only returns SERIOUS errors */
स्थिर CURLcode altsvc_add(संरचना altsvcinfo *asi, स्थिरांक अक्षर *ओळ)
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

  जर(curlx_str_word(&ओळ, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_word(&ओळ, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_संख्या(&ओळ, &srcport, 65535) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_word(&ओळ, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_word(&ओळ, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_संख्या(&ओळ, &dstport, 65535) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_quotedword(&ओळ, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_संख्या(&ओळ, &persist, 1) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_संख्या(&ओळ, &prio, 0) ||
     curlx_str_newline(&ओळ))
    ;
  अन्यथा {
    अक्षर dbuf[MAX_ALTSVC_DATELEN + 1];
    समय_प्रकार expires = 0;
    समय_प्रकार now = समय(रिक्त);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    स_नकल(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    जर(now < expires) {
      संरचना altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (माप_प्रकार)srcport, (माप_प्रकार)dstport);
      जर(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      अन्यथा
        परत CURLE_OUT_OF_MEMORY;
    }
  }

  परत CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
स्थिर CURLcode altsvc_load(संरचना altsvcinfo *asi, स्थिरांक अक्षर *file)
{
  CURLcode result = CURLE_OK;
  खाता *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  जर(!asi->filename)
    परत CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  जर(fp) {
    curlx_struct_stat stat;
    जर((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      संरचना dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_ओळ);
      करा {
        result = Curl_get_ओळ(&buf, fp, &eof);
        जर(!result) {
          स्थिरांक अक्षर *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          जर(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } पर्यंत(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  परत result;
}

/*
 * Write this single altsvc entry to a single output line
 */
स्थिर CURLcode altsvc_out(संरचना altsvc *as, खाता *fp)
{
  संरचना पंचांग stamp;
  स्थिरांक अक्षर *dst6_pre = "";
  स्थिरांक अक्षर *dst6_post = "";
  स्थिरांक अक्षर *src6_pre = "";
  स्थिरांक अक्षर *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  जर(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    परत CURLE_OK;
#ifdef USE_IPV6
  अन्यथा {
    अक्षर ipv6_unused[16];
    जर(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    जर(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  परत CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
संरचना altsvcinfo *Curl_altsvc_init(शून्य)
{
  संरचना altsvcinfo *asi = curlx_calloc(1, आकार(संरचना altsvcinfo));
  जर(!asi)
    परत रिक्त;
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
  परत asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(संरचना altsvcinfo *asi, स्थिरांक अक्षर *file)
{
  DEBUGASSERT(asi);
  परत altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(संरचना Curl_easy *data, स्थिरांक दीर्घ ctrl)
{
  DEBUGASSERT(data);
  जर(!ctrl)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(!data->asi) {
    data->asi = Curl_altsvc_init();
    जर(!data->asi)
      परत CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  परत CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
शून्य Curl_altsvc_cleanup(संरचना altsvcinfo **asi)
{
  जर(*asi) {
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *n;
    संरचना altsvcinfo *altsvc = *asi;
    साठी(e = Curl_llist_head(&altsvc->list); e; e = n) {
      संरचना altsvc *as = Curl_गाठ_elem(e);
      n = Curl_गाठ_next(e);
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
                          संरचना altsvcinfo *asi, स्थिरांक अक्षर *file)
{
  CURLcode result = CURLE_OK;
  खाता *out;
  अक्षर *tempstore = रिक्त;

  जर(!asi)
    /* no cache activated */
    परत CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  जर(!file && asi->filename)
    file = asi->filename;

  जर((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    परत CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  जर(!result) {
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *n;
    ख_माला_दो("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    साठी(e = Curl_llist_head(&asi->list); e; e = n) {
      संरचना altsvc *as = Curl_गाठ_elem(e);
      n = Curl_गाठ_next(e);
      result = altsvc_out(as, out);
      जर(result)
        थांबा;
    }
    curlx_fclose(out);
    जर(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    जर(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  परत result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
स्थिर bool hostcompare(स्थिरांक अक्षर *host, स्थिरांक अक्षर *check)
{
  माप_प्रकार hlen = म_माप(host);
  माप_प्रकार clen = म_माप(check);

  जर(hlen && (host[hlen - 1] == '.'))
    hlen--;
  जर(hlen != clen)
    /* they cannot match if they have different lengths */
    परत FALSE;
  परत curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
स्थिर शून्य altsvc_flush(संरचना altsvcinfo *asi,
                         संरचना Curl_peer *origin,
                         गणना alpnid origin_alpnid)
{
  संरचना Curl_llist_गाठ *e;
  संरचना Curl_llist_गाठ *n;
  साठी(e = Curl_llist_head(&asi->list); e; e = n) {
    संरचना altsvc *as = Curl_गाठ_elem(e);
    n = Curl_गाठ_next(e);
    जर((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_गाठ_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
स्थिर शून्य altsvc_parse_params(स्थिरांक अक्षर **pp,
                                समय_प्रकार *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  जर(curlx_str_single(pp, ';'))
    परत;

  साठी(;;) {
    संरचना Curl_str name;
    संरचना Curl_str val;
    curl_off_t अंक;
    पूर्णांक keyword = 0;

    /* allow some extra whitespaces around name and value */
    जर(curlx_str_until(pp, &name, MAX_ALTSVC_ओळ, '=') ||
       curlx_str_single(pp, '='))
      थांबा;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    जर(**pp == '\"') {
      जर(curlx_str_quotedword(pp, &val, MAX_ALTSVC_ओळ))
        थांबा;
    }
    अन्यथा {
      जर(curlx_str_cspn(pp, &val, ",;\r\n"))
        थांबा;
    }
    curlx_str_trimblanks(&val);

    जर(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    अन्यथा जर(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    जर(keyword) {
      स्थिरांक अक्षर *vp = curlx_str(&val);
      स्थिरांक अक्षर *vend = vp + curlx_strlen(&val);
      जर(curlx_str_संख्या(&vp, &अंक, CURL_OFF_T_MAX))
        थांबा; /* not a number, skip further parameter parsing */
      जर(vp != vend)
        थांबा; /* not entirely a number, skip further parameter parsing */
      जर(keyword == ALTSVC_MA) {
        जर(अंक > CAP_ALTSVC_MAX_AGE)
          अंक = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (समय_प्रकार)अंक;
      }
      अन्यथा जर(अंक == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    जर(curlx_str_single(pp, ';'))
      थांबा; /* no further parameters */
  }
}

स्थिर bool altsvc_parse_dest(स्थिरांक अक्षर **pp,
                              संरचना Curl_easy *data,
                              संरचना Curl_peer *origin,
                              संरचना Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  जर(curlx_str_single(pp, '\"'))
    परत FALSE;

  /* quoted string, with hostname or just :port ? */
  जर(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    जर(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      जर(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        परत FALSE;
      }
    }
    अन्यथा { /* IPv6 hostname */
      जर(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        परत FALSE;
      }
    }
    जर(curlx_str_single(pp, ':'))
      परत FALSE; /* not followed by ':' */
  }
  अन्यथा  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     म_माप(origin->hostname));

  जर(curlx_str_संख्या(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    परत FALSE;
  }

  *pdstport = (uint16_t)port;
  जर(curlx_str_single(pp, '\"'))
    परत FALSE; /* quoted string not ending here as expected */
  परत TRUE;
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
                           संरचना altsvcinfo *asi, स्थिरांक अक्षर *मूल्य,
                           संरचना Curl_peer *origin,
                           गणना alpnid origin_alpnid)
{
  संरचना altsvc *as;
  माप_प्रकार entries = 0;
  संरचना Curl_str alpn;
  स्थिरांक अक्षर *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = मूल्य;
  जर(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    जर(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      परत CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  साठी(p = मूल्य; *p;) {
    समय_प्रकार maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    गणना alpnid dstalpnid;
    संरचना Curl_str dsthost;
    uint16_t dstport;

    जर(curlx_str_until(&p, &alpn, MAX_ALTSVC_ओळ, '='))
      थांबा; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    जर(curlx_str_single(&p, '='))
      थांबा;

    /* Parse altsvc hostname:port */
    जर(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      थांबा;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    जर(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      जर(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, म_माप(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      जर(as) {
        समय_प्रकार secs = समय(रिक्त);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        जर(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        अन्यथा
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (पूर्णांक)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      अन्यथा
        परत CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    जर(curlx_str_single(&p, ','))
      थांबा;
  }

  परत CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(संरचना altsvcinfo *asi,
                        संरचना Curl_peer *origin,
                        गणना alpnid origin_alpnid,
                        संरचना altsvc **dstentry,
                        स्थिरांक पूर्णांक versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  जर(Curl_llist_मोजणी(&asi->list)) {
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *n;
    समय_प्रकार now = समय(रिक्त);

    साठी(e = Curl_llist_head(&asi->list); e; e = n) {
      संरचना altsvc *as = Curl_गाठ_elem(e);
      n = Curl_गाठ_next(e);
      जर(as->expires < now) {
        /* an expired entry, remove */
        Curl_गाठ_remove(e);
        altsvc_free(as);
        सुरूठेवा;
      }
      जर((origin_alpnid == as->src.alpnid) &&
         (versions & (पूर्णांक)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        परत TRUE;
      }
    }
  }
  परत FALSE;
}

#जर defined(DEBUGBUILD) || defined(UNITTESTS)
#undef समय
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
