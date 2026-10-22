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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "altsvc.h"
#అన్తర్భావ "curl_fopen.h"
#అన్తర్భావ "curl_get_line.h"
#అన్తర్భావ "parsedate.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "connect.h"

#నిర్వచించు MAX_ALTSVC_పంక్తి    4095
#నిర్వచించు MAX_ALTSVC_DATELEN 17
#నిర్వచించు MAX_ALTSVC_HOSTLEN 2048
#నిర్వచించు MAX_ALTSVC_ALPNLEN 10

#నిర్వచించు H3_సంస్కరణ "h3"

#ఒకవేళ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
స్థిర time_t altsvc_debugtime(శూన్య *వాడని)
{
  స్థిరరాశి అక్షరం *timestr = getenv("CURL_TIME");
  (శూన్య)వాడని;
  ఒకవేళ(timestr) {
    curl_off_t val;
    curlx_str_సంఖ్య(&timestr, &val, సమయం_T_MAX);
    తిరిగి (time_t)val;
  }
  తిరిగి సమయం(ఖాళీ);
}
#నిర్వచనం_రద్దు సమయం
#నిర్వచించు సమయం(x) altsvc_debugtime(x)
#అయితే_ముగింపు

/* Given the ALPN ID, return the name */
స్థిరరాశి అక్షరం *Curl_alpnid2str(గణన alpnid id)
{
  ఎంపిక(id) {
  స్థితి ALPN_h1:
    తిరిగి "h1";
  స్థితి ALPN_h2:
    తిరిగి "h2";
  స్థితి ALPN_h3:
    తిరిగి H3_సంస్కరణ;
  అప్రమేయ:
    తిరిగి ""; /* bad */
  }
}

స్థిర గణన alpnid Curl_str2alpnid(స్థిరరాశి నిర్మాణం Curl_str *cstr)
{
  తిరిగి Curl_alpn2alpnid((స్థిరరాశి గుర్తులేని అక్షరం *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#నిర్వచించు altsvc_free(x) curlx_free(x)

స్థిర నిర్మాణం altsvc *altsvc_createid(స్థిరరాశి అక్షరం *srchost,
                                      size_t hlen,
                                      స్థిరరాశి అక్షరం *dsthost,
                                      size_t dlen, /* dsthost length */
                                      గణన alpnid srcalpnid,
                                      గణన alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  నిర్మాణం altsvc *as;
  ఒకవేళ((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  లేకపోతే ఒకవేళ(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  ఒకవేళ((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  ఒకవేళ(!hlen || !dlen)
    /* bad input */
    తిరిగి ఖాళీ;
  /* struct size plus both strings */
  as = curlx_calloc(1, పరిమాణం(నిర్మాణం altsvc) + (hlen + 1) + (dlen + 1));
  ఒకవేళ(!as)
    తిరిగి ఖాళీ;
  as->src.host = (అక్షరం *)as + పరిమాణం(నిర్మాణం altsvc);
  జ్ఞాపకం_నకలు(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (అక్షరం *)as + పరిమాణం(నిర్మాణం altsvc) + hlen + 1;
  జ్ఞాపకం_నకలు(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.పోర్టు = (గుర్తులేని పొట్టి)srcport;
  as->dst.పోర్టు = (గుర్తులేని పొట్టి)dstport;

  తిరిగి as;
}

స్థిర నిర్మాణం altsvc *altsvc_create(నిర్మాణం Curl_str *srchost,
                                    నిర్మాణం Curl_str *dsthost,
                                    నిర్మాణం Curl_str *srcalpn,
                                    నిర్మాణం Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  గణన alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  గణన alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  ఒకవేళ(!srcalpnid || !dstalpnid)
    తిరిగి ఖాళీ;
  తిరిగి altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
మొదటి*/
స్థిర శూన్య altsvc_append(నిర్మాణం altsvcinfo *asi, నిర్మాణం altsvc *as)
{
  అయితే(Curl_llist_లెక్క(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    నిర్మాణం Curl_llist_కణుపు *e = Curl_llist_తల(&asi->list);
    నిర్మాణం altsvc *oldas = Curl_కణుపు_elem(e);
    Curl_కణుపు_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->కణుపు);
}

/* only returns SERIOUS errors */
స్థిర CURLcode altsvc_కూడు(నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *పంక్తి)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  నిర్మాణం Curl_str srchost;
  నిర్మాణం Curl_str dsthost;
  నిర్మాణం Curl_str srcalpn;
  నిర్మాణం Curl_str dstalpn;
  నిర్మాణం Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  ఒకవేళ(curlx_str_మాట(&పంక్తి, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_మాట(&పంక్తి, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_సంఖ్య(&పంక్తి, &srcport, 65535) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_మాట(&పంక్తి, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_మాట(&పంక్తి, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_సంఖ్య(&పంక్తి, &dstport, 65535) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_quotedword(&పంక్తి, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_సంఖ్య(&పంక్తి, &persist, 1) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_సంఖ్య(&పంక్తి, &prio, 0) ||
     curlx_str_newline(&పంక్తి))
    ;
  లేకపోతే {
    అక్షరం dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = సమయం(ఖాళీ);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    జ్ఞాపకం_నకలు(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    ఒకవేళ(now < expires) {
      నిర్మాణం altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      ఒకవేళ(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      లేకపోతే
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  తిరిగి CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
స్థిర CURLcode altsvc_ఎక్కించు(నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *file)
{
  CURLcode ఫలితం = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  ఒకవేళ(!asi->filename)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ఒకవేళ(fp) {
    curlx_struct_stat stat;
    ఒకవేళ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_బహుళకం)) {
      bool eof = FALSE;
      నిర్మాణం dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_పంక్తి);
      చేయి {
        ఫలితం = Curl_get_పంక్తి(&buf, fp, &eof);
        ఒకవేళ(!ఫలితం) {
          స్థిరరాశి అక్షరం *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          ఒకవేళ(curlx_str_single(&lineptr, '#'))
            altsvc_కూడు(asi, lineptr);
        }
      } అయితే(!ఫలితం && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  తిరిగి ఫలితం;
}

/*
 * Write this single altsvc entry to a single output line
 */
స్థిర CURLcode altsvc_బహిర్గతం(నిర్మాణం altsvc *as, FILE *fp)
{
  నిర్మాణం tm stamp;
  స్థిరరాశి అక్షరం *dst6_pre = "";
  స్థిరరాశి అక్షరం *dst6_post = "";
  స్థిరరాశి అక్షరం *src6_pre = "";
  స్థిరరాశి అక్షరం *src6_post = "";
  CURLcode ఫలితం = curlx_gmtime(as->expires, &stamp);
  ఒకవేళ(ఫలితం)
    /* fail silently for date errors so that only this entry gets
       skipped */
    తిరిగి CURLE_OK;
#నిర్వచితమైతే USE_IPV6
  లేకపోతే {
    అక్షరం ipv6_వాడని[16];
    ఒకవేళ(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_వాడని) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    ఒకవేళ(curlx_inet_pton(AF_INET6, as->src.host, ipv6_వాడని) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#అయితే_ముగింపు
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.పోర్టు,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.పోర్టు,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  తిరిగి CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
నిర్మాణం altsvcinfo *Curl_altsvc_init(శూన్య)
{
  నిర్మాణం altsvcinfo *asi = curlx_calloc(1, పరిమాణం(నిర్మాణం altsvcinfo));
  ఒకవేళ(!asi)
    తిరిగి ఖాళీ;
  Curl_llist_init(&asi->list, ఖాళీ);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#నిర్వచితమైతే USE_HTTP2
    | CURLALTSVC_H2
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTP3
    | CURLALTSVC_H3
#అయితే_ముగింపు
    ;
  తిరిగి asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_ఎక్కించు(నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *file)
{
  DEBUGASSERT(asi);
  తిరిగి altsvc_ఎక్కించు(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి దీర్ఘ ctrl)
{
  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(!ctrl)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(!దత్తాంశం->asi) {
    దత్తాంశం->asi = Curl_altsvc_init();
    ఒకవేళ(!దత్తాంశం->asi)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  దత్తాంశం->asi->flags = ctrl;
  తిరిగి CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
శూన్య Curl_altsvc_cleanup(నిర్మాణం altsvcinfo **asi)
{
  ఒకవేళ(*asi) {
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_llist_కణుపు *n;
    నిర్మాణం altsvcinfo *altsvc = *asi;
    కొరకు(e = Curl_llist_తల(&altsvc->list); e; e = n) {
      నిర్మాణం altsvc *as = Curl_కణుపు_elem(e);
      n = Curl_కణుపు_తదుపరి(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ఖాళీ; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *file)
{
  CURLcode ఫలితం = CURLE_OK;
  FILE *బహిర్గతం;
  అక్షరం *tempstore = ఖాళీ;

  ఒకవేళ(!asi)
    /* no cache activated */
    తిరిగి CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  ఒకవేళ(!file && asi->filename)
    file = asi->filename;

  ఒకవేళ((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    తిరిగి CURLE_OK;

  ఫలితం = Curl_fopen(దత్తాంశం, file, &బహిర్గతం, &tempstore);
  ఒకవేళ(!ఫలితం) {
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_llist_కణుపు *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          బహిర్గతం);
    కొరకు(e = Curl_llist_తల(&asi->list); e; e = n) {
      నిర్మాణం altsvc *as = Curl_కణుపు_elem(e);
      n = Curl_కణుపు_తదుపరి(e);
      ఫలితం = altsvc_బహిర్గతం(as, బహిర్గతం);
      ఒకవేళ(ఫలితం)
        ఆపు;
    }
    curlx_fclose(బహిర్గతం);
    ఒకవేళ(!ఫలితం && tempstore && curlx_rename(tempstore, file))
      ఫలితం = CURLE_WRITE_ERROR;

    ఒకవేళ(ఫలితం && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  తిరిగి ఫలితం;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
స్థిర bool hostcompare(స్థిరరాశి అక్షరం *host, స్థిరరాశి అక్షరం *check)
{
  size_t hlen = అక్షరశ్రేణి_పొడవు(host);
  size_t clen = అక్షరశ్రేణి_పొడవు(check);

  ఒకవేళ(hlen && (host[hlen - 1] == '.'))
    hlen--;
  ఒకవేళ(hlen != clen)
    /* they cannot match if they have different lengths */
    తిరిగి FALSE;
  తిరిగి curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
స్థిర శూన్య altsvc_flush(నిర్మాణం altsvcinfo *asi,
                         నిర్మాణం Curl_peer *origin,
                         గణన alpnid origin_alpnid)
{
  నిర్మాణం Curl_llist_కణుపు *e;
  నిర్మాణం Curl_llist_కణుపు *n;
  కొరకు(e = Curl_llist_తల(&asi->list); e; e = n) {
    నిర్మాణం altsvc *as = Curl_కణుపు_elem(e);
    n = Curl_కణుపు_తదుపరి(e);
    ఒకవేళ((origin_alpnid == as->src.alpnid) &&
       (origin->పోర్టు == as->src.పోర్టు) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_కణుపు_remove(e);
      altsvc_free(as);
    }
  }
}

#నిర్వచించు CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#నిర్వచించు ALTSVC_MA      1
#నిర్వచించు ALTSVC_PERSIST 2
స్థిర శూన్య altsvc_parse_params(స్థిరరాశి అక్షరం **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  ఒకవేళ(curlx_str_single(pp, ';'))
    తిరిగి;

  కొరకు(;;) {
    నిర్మాణం Curl_str name;
    నిర్మాణం Curl_str val;
    curl_off_t అంకె;
    పూర్ణాంకం keyword = 0;

    /* allow some extra whitespaces around name and value */
    ఒకవేళ(curlx_str_until(pp, &name, MAX_ALTSVC_పంక్తి, '=') ||
       curlx_str_single(pp, '='))
      ఆపు;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    ఒకవేళ(**pp == '\"') {
      ఒకవేళ(curlx_str_quotedword(pp, &val, MAX_ALTSVC_పంక్తి))
        ఆపు;
    }
    లేకపోతే {
      ఒకవేళ(curlx_str_cspn(pp, &val, ",;\r\n"))
        ఆపు;
    }
    curlx_str_trimblanks(&val);

    ఒకవేళ(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    ఒకవేళ(keyword) {
      స్థిరరాశి అక్షరం *vp = curlx_str(&val);
      స్థిరరాశి అక్షరం *vend = vp + curlx_strlen(&val);
      ఒకవేళ(curlx_str_సంఖ్య(&vp, &అంకె, CURL_OFF_T_MAX))
        ఆపు; /* not a number, skip further parameter parsing */
      ఒకవేళ(vp != vend)
        ఆపు; /* not entirely a number, skip further parameter parsing */
      ఒకవేళ(keyword == ALTSVC_MA) {
        ఒకవేళ(అంకె > CAP_ALTSVC_MAX_AGE)
          అంకె = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)అంకె;
      }
      లేకపోతే ఒకవేళ(అంకె == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    ఒకవేళ(curlx_str_single(pp, ';'))
      ఆపు; /* no further parameters */
  }
}

స్థిర bool altsvc_parse_dest(స్థిరరాశి అక్షరం **pp,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_peer *origin,
                              నిర్మాణం Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t పోర్టు = 0;

  ఒకవేళ(curlx_str_single(pp, '\"'))
    తిరిగి FALSE;

  /* quoted string, with hostname or just :port ? */
  ఒకవేళ(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    ఒకవేళ(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      ఒకవేళ(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(దత్తాంశం, "Bad alt-svc hostname, ignoring.");
        తిరిగి FALSE;
      }
    }
    లేకపోతే { /* IPv6 hostname */
      ఒకవేళ(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(దత్తాంశం, "Bad alt-svc IPv6 hostname, ignoring.");
        తిరిగి FALSE;
      }
    }
    ఒకవేళ(curlx_str_single(pp, ':'))
      తిరిగి FALSE; /* not followed by ':' */
  }
  లేకపోతే  /* is only :port, hostname is effectively origin */
    curlx_str_కేటాయించు(dsthost, origin->hostname,
                     అక్షరశ్రేణి_పొడవు(origin->hostname));

  ఒకవేళ(curlx_str_సంఖ్య(pp, &పోర్టు, 0xffff)) {
    infof(దత్తాంశం, "Unknown alt-svc port number, ignoring.");
    తిరిగి FALSE;
  }

  *pdstport = (uint16_t)పోర్టు;
  ఒకవేళ(curlx_str_single(pp, '\"'))
    తిరిగి FALSE; /* quoted string not ending here as expected */
  తిరిగి TRUE;
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
CURLcode Curl_altsvc_parse(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *విలువ,
                           నిర్మాణం Curl_peer *origin,
                           గణన alpnid origin_alpnid)
{
  నిర్మాణం altsvc *as;
  size_t entries = 0;
  నిర్మాణం Curl_str alpn;
  స్థిరరాశి అక్షరం *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = విలువ;
  ఒకవేళ(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    ఒకవేళ(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      తిరిగి CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  కొరకు(p = విలువ; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    గణన alpnid dstalpnid;
    నిర్మాణం Curl_str dsthost;
    uint16_t dstport;

    ఒకవేళ(curlx_str_until(&p, &alpn, MAX_ALTSVC_పంక్తి, '='))
      ఆపు; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    ఒకవేళ(curlx_str_single(&p, '='))
      ఆపు;

    /* Parse altsvc hostname:port */
    ఒకవేళ(!altsvc_parse_dest(&p, దత్తాంశం, origin, &dsthost, &dstport))
      ఆపు;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    ఒకవేళ(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      ఒకవేళ(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, అక్షరశ్రేణి_పొడవు(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->పోర్టు, dstport);
      ఒకవేళ(as) {
        time_t secs = సమయం(ఖాళీ);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        ఒకవేళ(maxage > (సమయం_T_MAX - secs))
          as->expires = సమయం_T_MAX;
        లేకపోతే
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(దత్తాంశం, "Added alt-svc: %.*s:%u over %s",
              (పూర్ణాంకం)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      లేకపోతే
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    /* When this is followed by a comma, we expect another entry */
    ఒకవేళ(curlx_str_single(&p, ','))
      ఆపు;
  }

  తిరిగి CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(నిర్మాణం altsvcinfo *asi,
                        నిర్మాణం Curl_peer *origin,
                        గణన alpnid origin_alpnid,
                        నిర్మాణం altsvc **dstentry,
                        స్థిరరాశి పూర్ణాంకం versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  ఒకవేళ(Curl_llist_లెక్క(&asi->list)) {
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_llist_కణుపు *n;
    time_t now = సమయం(ఖాళీ);

    కొరకు(e = Curl_llist_తల(&asi->list); e; e = n) {
      నిర్మాణం altsvc *as = Curl_కణుపు_elem(e);
      n = Curl_కణుపు_తదుపరి(e);
      ఒకవేళ(as->expires < now) {
        /* an expired entry, remove */
        Curl_కణుపు_remove(e);
        altsvc_free(as);
        కొనసాగు;
      }
      ఒకవేళ((origin_alpnid == as->src.alpnid) &&
         (versions & (పూర్ణాంకం)as->dst.alpnid) &&
         (origin->పోర్టు == as->src.పోర్టు) &&
         hostcompare(origin->hostname, as->src.host)) {
        /*సరిపోలిక*/
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->పోర్టు == as->dst.పోర్టు) &&
                             hostcompare(origin->hostname, as->dst.host);
        తిరిగి TRUE;
      }
    }
  }
  తిరిగి FALSE;
}

#ఒకవేళ defined(DEBUGBUILD) || defined(UNITTESTS)
#నిర్వచనం_రద్దు సమయం
#అయితే_ముగింపు

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
