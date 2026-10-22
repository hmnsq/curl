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
 * The Strict-Transport-Security header is defined in RFC 6797:
 * https://datatracker.ietf.org/doc/html/rfc6797
 */
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "llist.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "curl_fopen.h"
#అన్తర్భావ "curl_get_line.h"
#అన్తర్భావ "parsedate.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/strcopy.h"

#నిర్వచించు MAX_HSTS_పంక్తి    4095
#నిర్వచించు MAX_HSTS_HOSTLEN 2048
#నిర్వచించు MAX_HSTS_DATELEN 17
#నిర్వచించు UNLIMITED        "unlimited"

#నిర్వచించు CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#ఒకవేళ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
స్థిర time_t hsts_debugtime(శూన్య *వాడని)
{
  స్థిరరాశి అక్షరం *timestr = getenv("CURL_TIME");
  (శూన్య)వాడని;
  ఒకవేళ(timestr) {
    curl_off_t val;
    ఒకవేళ(!curlx_str_సంఖ్య(&timestr, &val, సమయం_T_MAX))
      val += (curl_off_t)deltatime;
    తిరిగి (time_t)val;
  }
  తిరిగి సమయం(ఖాళీ);
}
#నిర్వచనం_రద్దు సమయం
#నిర్వచించు సమయం(x) hsts_debugtime(x)
#అయితే_ముగింపు

నిర్మాణం hsts *Curl_hsts_init(శూన్య)
{
  నిర్మాణం hsts *h = curlx_calloc(1, పరిమాణం(నిర్మాణం hsts));
  ఒకవేళ(h) {
    Curl_llist_init(&h->list, ఖాళీ);
  }
  తిరిగి h;
}

#నిర్వచించు hsts_free(x) curlx_free(x)

శూన్య Curl_hsts_cleanup(నిర్మాణం hsts **hp)
{
  నిర్మాణం hsts *h = *hp;
  ఒకవేళ(h) {
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_llist_కణుపు *n;
    కొరకు(e = Curl_llist_తల(&h->list); e; e = n) {
      నిర్మాణం stsentry *sts = Curl_కణుపు_elem(e);
      n = Curl_కణుపు_తదుపరి(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ఖాళీ;
  }
}

/* append the new entry to the list after possibly removing an old entry
మొదటి*/
స్థిర శూన్య hsts_append(నిర్మాణం hsts *h, నిర్మాణం stsentry *sts)
{
  ఒకవేళ(Curl_llist_లెక్క(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    నిర్మాణం Curl_llist_కణుపు *e = Curl_llist_తల(&h->list);
    నిర్మాణం stsentry *oldsts = Curl_కణుపు_elem(e);
    Curl_కణుపు_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->కణుపు);
}

స్థిర CURLcode hsts_create(నిర్మాణం hsts *h,
                            స్థిరరాశి అక్షరం *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  ఒకవేళ(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  ఒకవేళ(hlen) {
    నిర్మాణం stsentry *sts = curlx_calloc(1, పరిమాణం(నిర్మాణం stsentry) + hlen);
    ఒకవేళ(!sts)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    /* the null-terminator is already there */
    జ్ఞాపకం_నకలు(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  తిరిగి CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(నిర్మాణం hsts *dst, నిర్మాణం hsts *src)
{
  నిర్మాణం Curl_llist_కణుపు *e;
  time_t now = సమయం(ఖాళీ);
  కొరకు(e = Curl_llist_తల(&src->list); e; e = Curl_కణుపు_తదుపరి(e)) {
    నిర్మాణం stsentry *sts = Curl_కణుపు_elem(e);
    ఒకవేళ(sts->expires > now) {
      CURLcode ఫలితం = hsts_create(dst, sts->host, అక్షరశ్రేణి_పొడవు(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }
  తిరిగి CURLE_OK;
}

/*
 * Return the matching HSTS entry, or NULL if the given hostname is not
 * currently an HSTS one.
 *
 * The 'subdomain' argument tells the function if subdomain matching should be
 * attempted.
 *
 * @unittest 1660
 */
UNITTEST నిర్మాణం stsentry *hsts_check(నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST నిర్మాణం stsentry *hsts_check(నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *hostname,
                                     size_t hlen, bool subdomain)
{
  నిర్మాణం stsentry *bestsub = ఖాళీ;
  ఒకవేళ(h) {
    time_t now = సమయం(ఖాళీ);
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం Curl_llist_కణుపు *n;
    size_t blen = 0;

    ఒకవేళ((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      తిరిగి ఖాళీ;
    ఒకవేళ(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    కొరకు(e = Curl_llist_తల(&h->list); e; e = n) {
      నిర్మాణం stsentry *sts = Curl_కణుపు_elem(e);
      size_t ntail;
      n = Curl_కణుపు_తదుపరి(e);
      ఒకవేళ(sts->expires <= now) {
        /* remove expired entries */
        Curl_కణుపు_remove(&sts->కణుపు);
        hsts_free(sts);
        కొనసాగు;
      }
      ntail = అక్షరశ్రేణి_పొడవు(sts->host);
      ఒకవేళ((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        ఒకవేళ((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      ఒకవేళ((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        తిరిగి sts;
    }
  }
  తిరిగి bestsub;
}

CURLcode Curl_hsts_parse(నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *hostname,
                         స్థిరరాశి అక్షరం *శీర్షిక)
{
  స్థిరరాశి అక్షరం *p = శీర్షిక;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  నిర్మాణం stsentry *sts;
  time_t now = సమయం(ఖాళీ);
  size_t hlen = అక్షరశ్రేణి_పొడవు(hostname);

  ఒకవేళ(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    తిరిగి CURLE_OK;

  చేయి {
    నిర్మాణం Curl_str మాట;
    నిర్మాణం Curl_str val = { 0 };
    పూర్ణాంకం rc;
    bool కేటాయించు = FALSE;

    చేయి {
      curlx_str_passblanks(&p);
      ఒకవేళ(*p == ';')
        p++;
      లేకపోతే
        ఆపు;
    } అయితే(1);
    ఒకవేళ(curlx_str_cspn(&p, &మాట, ";=\r\n \t"))
      ఆపు;

    curlx_str_passblanks(&p);
    ఒకవేళ(!curlx_str_single(&p, '=')) {
      కేటాయించు = TRUE;
      curlx_str_passblanks(&p);

      ఒకవేళ(*p == '\"') {
        ఒకవేళ(curlx_str_quotedword(&p, &val, MAX_HSTS_పంక్తి))
          ఆపు;
      }
      లేకపోతే {
        ఒకవేళ(curlx_str_cspn(&p, &val, ", ;\r\n"))
          ఆపు;
      }
    }

    ఒకవేళ(కేటాయించు && curlx_str_casecompare(&మాట, "max-age")) {
      స్థిరరాశి అక్షరం *vp = curlx_str(&val);
      ఒకవేళ(gotma)
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      rc = curlx_str_సంఖ్య(&vp, &expires, CAP_HSTS_MAX_AGE);
      ఒకవేళ(rc == STRE_అధికప్రవాహం)
        expires = CAP_HSTS_MAX_AGE;
      లేకపోతే ఒకవేళ(rc)
        /* invalid max-age */
        తిరిగి CURLE_BAD_FUNCTION_వాదం;

      gotma = TRUE;
    }
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&మాట, "includesubdomains")) {
      ఒకవేళ(gotinc)
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } అయితే(*p);

  ఒకవేళ(!gotma)
    /* max-age is mandatory */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    ఒకవేళ(sts) {
      Curl_కణుపు_remove(&sts->కణుపు);
      hsts_free(sts);
    }
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  లేకపోతే
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  ఒకవేళ(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  లేకపోతే
    తిరిగి hsts_create(h, hostname, hlen, subdomains, expires);

  తిరిగి CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
స్థిర CURLcode hsts_నెట్టు(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం curl_సూచీ *i,
                          నిర్మాణం stsentry *sts,
                          bool *stop)
{
  నిర్మాణం curl_hstsentry e;
  CURLSTScode sc;
  నిర్మాణం tm stamp;
  CURLcode ఫలితం;

  e.name = (అక్షరం *)sts->host;
  e.namelen = అక్షరశ్రేణి_పొడవు(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  ఒకవేళ(sts->expires != సమయం_T_MAX) {
    ఫలితం = curlx_gmtime((time_t)sts->expires, &stamp);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    curl_msnprintf(e.expire, పరిమాణం(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  లేకపోతే
    curlx_strcopy(e.expire, పరిమాణం(e.expire), STRCONST(UNLIMITED));

  sc = దత్తాంశం->సమితి.hsts_write(దత్తాంశం, &e, i, దత్తాంశం->సమితి.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  తిరిగి sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_వాదం : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
స్థిర శూన్య hsts_బహిర్గతం(నిర్మాణం stsentry *sts, FILE *fp)
{
  నిర్మాణం tm stamp;
  ఒకవేళ(sts->expires != సమయం_T_MAX) {
    CURLcode ఫలితం = curlx_gmtime((time_t)sts->expires, &stamp);
    ఒకవేళ(!ఫలితం)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  లేకపోతే
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం hsts *h,
                        స్థిరరాశి అక్షరం *file)
{
  నిర్మాణం Curl_llist_కణుపు *e;
  నిర్మాణం Curl_llist_కణుపు *n;
  CURLcode ఫలితం = CURLE_OK;
  FILE *బహిర్గతం;
  అక్షరం *tempstore = ఖాళీ;

  ఒకవేళ(!h)
    /* no cache activated */
    తిరిగి CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  ఒకవేళ(!file && h->filename)
    file = h->filename;

  ఒకవేళ((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    వెళ్ళు skipsave;

  ఫలితం = Curl_fopen(దత్తాంశం, file, &బహిర్గతం, &tempstore);
  ఒకవేళ(!ఫలితం) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          బహిర్గతం);
    కొరకు(e = Curl_llist_తల(&h->list); e; e = n) {
      నిర్మాణం stsentry *sts = Curl_కణుపు_elem(e);
      n = Curl_కణుపు_తదుపరి(e);
      hsts_బహిర్గతం(sts, బహిర్గతం);
    }
    curlx_fclose(బహిర్గతం);
    ఒకవేళ(!ఫలితం && tempstore && curlx_rename(tempstore, file))
      ఫలితం = CURLE_WRITE_ERROR;

    ఒకవేళ(ఫలితం && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  ఒకవేళ(దత్తాంశం->సమితి.hsts_write) {
    /* if there is a write callback */
    నిర్మాణం curl_సూచీ i; /*లెక్క*/
    i.మొత్తం = Curl_llist_లెక్క(&h->list);
    i.సూచీ = 0;
    కొరకు(e = Curl_llist_తల(&h->list); e; e = n) {
      నిర్మాణం stsentry *sts = Curl_కణుపు_elem(e);
      bool stop;
      n = Curl_కణుపు_తదుపరి(e);
      ఫలితం = hsts_నెట్టు(దత్తాంశం, &i, sts, &stop);
      ఒకవేళ(ఫలితం || stop)
        ఆపు;
      i.సూచీ++;
    }
  }
  తిరిగి ఫలితం;
}

/* only returns SERIOUS errors */
స్థిర CURLcode hsts_కూడు_host_expire(నిర్మాణం hsts *h,
                                     స్థిరరాశి అక్షరం *host, size_t hostlen,
                                     స్థిరరాశి అక్షరం *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం stsentry *e;
  అక్షరం dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = సమయం(ఖాళీ);

  /* The date parser works on a null-terminated string. */
  ఒకవేళ(explen > MAX_HSTS_DATELEN)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  జ్ఞాపకం_నకలు(dbuf, expire, explen);
  dbuf[explen] = 0;

  ఒకవేళ(!strcmp(dbuf, UNLIMITED))
    expires = సమయం_T_MAX;
  లేకపోతే
    Curl_getdate_capped(dbuf, &expires);

  ఒకవేళ(expires <= now)
    /* this entry already expired */
    తిరిగి CURLE_OK;

  ఒకవేళ(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  ఒకవేళ(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  ఒకవేళ(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    ఒకవేళ(!e)
      ఫలితం = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    లేకపోతే ఒకవేళ((hostlen == అక్షరశ్రేణి_పొడవు(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      ఒకవేళ(expires > e->expires)
        e->expires = expires;
      ఒకవేళ(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  తిరిగి ఫలితం;
}

/* only returns SERIOUS errors */
స్థిర CURLcode hsts_కూడు(నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *పంక్తి)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  నిర్మాణం Curl_str host;
  నిర్మాణం Curl_str date;

  ఒకవేళ(curlx_str_మాట(&పంక్తి, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&పంక్తి) ||
     curlx_str_quotedword(&పంక్తి, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&పంక్తి))
    ;
  లేకపోతే {
    తిరిగి hsts_కూడు_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  తిరిగి CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
స్థిర CURLcode hsts_pull(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం hsts *h)
{
  /* if the HSTS read callback is set, use it */
  ఒకవేళ(దత్తాంశం->సమితి.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    చేయి {
      అక్షరం బఫరు[MAX_HSTS_HOSTLEN + 1];
      నిర్మాణం curl_hstsentry e;
      e.name = బఫరు;
      e.namelen = పరిమాణం(బఫరు) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = దత్తాంశం->సమితి.hsts_read(దత్తాంశం, &e, దత్తాంశం->సమితి.hsts_read_userp);
      ఒకవేళ(sc == CURLSTS_OK) {
        CURLcode ఫలితం;
        స్థిరరాశి అక్షరం *date = e.expire;
        ఒకవేళ(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          తిరిగి CURLE_BAD_FUNCTION_వాదం;
        ఒకవేళ(!date[0])
          date = UNLIMITED;
        ఫలితం = hsts_కూడు_host_expire(h, e.name, అక్షరశ్రేణి_పొడవు(e.name),
                                      date, అక్షరశ్రేణి_పొడవు(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
      లేకపోతే ఒకవేళ(sc == CURLSTS_FAIL)
        తిరిగి CURLE_ABORTED_BY_CALLBACK;
    } అయితే(sc == CURLSTS_OK);
  }
  తిరిగి CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
స్థిర CURLcode hsts_ఎక్కించు(నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *file)
{
  CURLcode ఫలితం = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  ఒకవేళ(!h->filename)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ఒకవేళ(fp) {
    curlx_struct_stat stat;
    ఒకవేళ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_బహుళకం)) {
      నిర్మాణం dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_పంక్తి);
      చేయి {
        ఫలితం = Curl_get_పంక్తి(&buf, fp, &eof);
        ఒకవేళ(!ఫలితం) {
          స్థిరరాశి అక్షరం *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          ఒకవేళ((*lineptr == '#') || అక్షరశ్రేణి_పొడవు(lineptr) <= 1)
            కొనసాగు;

          hsts_కూడు(h, lineptr);
        }
      } అయితే(!ఫలితం && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  తిరిగి ఫలితం;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *file)
{
  DEBUGASSERT(h);
  (శూన్య)దత్తాంశం;
  తిరిగి hsts_ఎక్కించు(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం hsts *h)
{
  ఒకవేళ(h)
    తిరిగి hsts_pull(దత్తాంశం, h);
  తిరిగి CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం curl_slist *l = దత్తాంశం->state.hstslist;
  ఒకవేళ(l) {
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_HSTS, CURL_తాళం_ACCESS_SINGLE);

    అయితే(l) {
      ఫలితం = Curl_hsts_loadfile(దత్తాంశం, దత్తాంశం->hsts, l->దత్తాంశం);
      ఒకవేళ(ఫలితం)
        ఆపు;
      l = l->తదుపరి;
    }
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_HSTS);
  }
  తిరిగి ఫలితం;
}

bool Curl_hsts_applies(నిర్మాణం hsts *h, స్థిరరాశి నిర్మాణం Curl_peer *dest)
{
  తిరిగి !!hsts_check(h, dest->hostname, అక్షరశ్రేణి_పొడవు(dest->hostname), TRUE);
}

#ఒకవేళ defined(DEBUGBUILD) || defined(UNITTESTS)
#నిర్వచనం_రద్దు సమయం
#అయితే_ముగింపు

#అయితే_ముగింపు /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
