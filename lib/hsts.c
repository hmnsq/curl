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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "llist.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "curl_fopen.h"
#অন্তর্ভাব "curl_get_line.h"
#অন্তর্ভাব "parsedate.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/strcopy.h"

#define MAX_HSTS_সারি    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
স্থির time_t hsts_debugtime(শূন্য *unused)
{
  ধ্রুবক অক্ষর *timestr = getenv("CURL_TIME");
  (শূন্য)unused;
  যদি(timestr) {
    curl_off_t val;
    যদি(!curlx_str_সংখ্যা(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    ফেরত (time_t)val;
  }
  ফেরত time(ফাঁকা);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

কাঠামো hsts *Curl_hsts_init(শূন্য)
{
  কাঠামো hsts *h = curlx_calloc(1, আকার(কাঠামো hsts));
  যদি(h) {
    Curl_llist_init(&h->list, ফাঁকা);
  }
  ফেরত h;
}

#define hsts_free(x) curlx_free(x)

শূন্য Curl_hsts_cleanup(কাঠামো hsts **hp)
{
  কাঠামো hsts *h = *hp;
  যদি(h) {
    কাঠামো Curl_llist_গ্রন্থি *e;
    কাঠামো Curl_llist_গ্রন্থি *n;
    জন্য(e = Curl_llist_শীর্ষ(&h->list); e; e = n) {
      কাঠামো stsentry *sts = Curl_গ্রন্থি_elem(e);
      n = Curl_গ্রন্থি_পরবর্তী(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ফাঁকা;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
স্থির শূন্য hsts_append(কাঠামো hsts *h, কাঠামো stsentry *sts)
{
  যদি(Curl_llist_গুনতি(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    কাঠামো Curl_llist_গ্রন্থি *e = Curl_llist_শীর্ষ(&h->list);
    কাঠামো stsentry *oldsts = Curl_গ্রন্থি_elem(e);
    Curl_গ্রন্থি_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->গ্রন্থি);
}

স্থির CURLcode hsts_create(কাঠামো hsts *h,
                            ধ্রুবক অক্ষর *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  যদি(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  যদি(hlen) {
    কাঠামো stsentry *sts = curlx_calloc(1, আকার(কাঠামো stsentry) + hlen);
    যদি(!sts)
      ফেরত CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ফেরত CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(কাঠামো hsts *dst, কাঠামো hsts *src)
{
  কাঠামো Curl_llist_গ্রন্থি *e;
  time_t now = time(ফাঁকা);
  জন্য(e = Curl_llist_শীর্ষ(&src->list); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
    কাঠামো stsentry *sts = Curl_গ্রন্থি_elem(e);
    যদি(sts->expires > now) {
      CURLcode ফলাফল = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
  }
  ফেরত CURLE_OK;
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
UNITTEST কাঠামো stsentry *hsts_check(কাঠামো hsts *h, ধ্রুবক অক্ষর *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST কাঠামো stsentry *hsts_check(কাঠামো hsts *h, ধ্রুবক অক্ষর *hostname,
                                     size_t hlen, bool subdomain)
{
  কাঠামো stsentry *bestsub = ফাঁকা;
  যদি(h) {
    time_t now = time(ফাঁকা);
    কাঠামো Curl_llist_গ্রন্থি *e;
    কাঠামো Curl_llist_গ্রন্থি *n;
    size_t blen = 0;

    যদি((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ফেরত ফাঁকা;
    যদি(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    জন্য(e = Curl_llist_শীর্ষ(&h->list); e; e = n) {
      কাঠামো stsentry *sts = Curl_গ্রন্থি_elem(e);
      size_t ntail;
      n = Curl_গ্রন্থি_পরবর্তী(e);
      যদি(sts->expires <= now) {
        /* remove expired entries */
        Curl_গ্রন্থি_remove(&sts->গ্রন্থি);
        hsts_free(sts);
        চালু;
      }
      ntail = strlen(sts->host);
      যদি((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        যদি((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      যদি((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        ফেরত sts;
    }
  }
  ফেরত bestsub;
}

CURLcode Curl_hsts_parse(কাঠামো hsts *h, ধ্রুবক অক্ষর *hostname,
                         ধ্রুবক অক্ষর *header)
{
  ধ্রুবক অক্ষর *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  কাঠামো stsentry *sts;
  time_t now = time(ফাঁকা);
  size_t hlen = strlen(hostname);

  যদি(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ফেরত CURLE_OK;

  করো {
    কাঠামো Curl_str word;
    কাঠামো Curl_str val = { 0 };
    পূর্ণসংখ্যা rc;
    bool assign = FALSE;

    করো {
      curlx_str_passblanks(&p);
      যদি(*p == ';')
        p++;
      অন্যথা
        বিরতি;
    } যখন(1);
    যদি(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      বিরতি;

    curlx_str_passblanks(&p);
    যদি(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      যদি(*p == '\"') {
        যদি(curlx_str_quotedword(&p, &val, MAX_HSTS_সারি))
          বিরতি;
      }
      অন্যথা {
        যদি(curlx_str_cspn(&p, &val, ", ;\r\n"))
          বিরতি;
      }
    }

    যদি(assign && curlx_str_casecompare(&word, "max-age")) {
      ধ্রুবক অক্ষর *vp = curlx_str(&val);
      যদি(gotma)
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_সংখ্যা(&vp, &expires, CAP_HSTS_MAX_AGE);
      যদি(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      অন্যথা যদি(rc)
        /* invalid max-age */
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    অন্যথা যদি(curlx_str_casecompare(&word, "includesubdomains")) {
      যদি(gotinc)
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } যখন(*p);

  যদি(!gotma)
    /* max-age is mandatory */
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    যদি(sts) {
      Curl_গ্রন্থি_remove(&sts->গ্রন্থি);
      hsts_free(sts);
    }
    ফেরত CURLE_OK;
  }

  যদি(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  অন্যথা
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  যদি(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  অন্যথা
    ফেরত hsts_create(h, hostname, hlen, subdomains, expires);

  ফেরত CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
স্থির CURLcode hsts_ঠেলো(কাঠামো Curl_easy *তথ্য,
                          কাঠামো curl_সূচক *i,
                          কাঠামো stsentry *sts,
                          bool *stop)
{
  কাঠামো curl_hstsentry e;
  CURLSTScode sc;
  কাঠামো tm stamp;
  CURLcode ফলাফল;

  e.name = (অক্ষর *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  যদি(sts->expires != TIME_T_MAX) {
    ফলাফল = curlx_gmtime((time_t)sts->expires, &stamp);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    curl_msnprintf(e.expire, আকার(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  অন্যথা
    curlx_strcopy(e.expire, আকার(e.expire), STRCONST(UNLIMITED));

  sc = তথ্য->set.hsts_write(তথ্য, &e, i, তথ্য->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ফেরত sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
স্থির শূন্য hsts_out(কাঠামো stsentry *sts, FILE *fp)
{
  কাঠামো tm stamp;
  যদি(sts->expires != TIME_T_MAX) {
    CURLcode ফলাফল = curlx_gmtime((time_t)sts->expires, &stamp);
    যদি(!ফলাফল)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  অন্যথা
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(কাঠামো Curl_easy *তথ্য, কাঠামো hsts *h,
                        ধ্রুবক অক্ষর *file)
{
  কাঠামো Curl_llist_গ্রন্থি *e;
  কাঠামো Curl_llist_গ্রন্থি *n;
  CURLcode ফলাফল = CURLE_OK;
  FILE *out;
  অক্ষর *tempstore = ফাঁকা;

  যদি(!h)
    /* no cache activated */
    ফেরত CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  যদি(!file && h->filename)
    file = h->filename;

  যদি((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    যাও skipsave;

  ফলাফল = Curl_fopen(তথ্য, file, &out, &tempstore);
  যদি(!ফলাফল) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    জন্য(e = Curl_llist_শীর্ষ(&h->list); e; e = n) {
      কাঠামো stsentry *sts = Curl_গ্রন্থি_elem(e);
      n = Curl_গ্রন্থি_পরবর্তী(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    যদি(!ফলাফল && tempstore && curlx_rename(tempstore, file))
      ফলাফল = CURLE_WRITE_ERROR;

    যদি(ফলাফল && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  যদি(তথ্য->set.hsts_write) {
    /* if there is a write callback */
    কাঠামো curl_সূচক i; /*গুনতি*/
    i.যোগ = Curl_llist_গুনতি(&h->list);
    i.সূচক = 0;
    জন্য(e = Curl_llist_শীর্ষ(&h->list); e; e = n) {
      কাঠামো stsentry *sts = Curl_গ্রন্থি_elem(e);
      bool stop;
      n = Curl_গ্রন্থি_পরবর্তী(e);
      ফলাফল = hsts_ঠেলো(তথ্য, &i, sts, &stop);
      যদি(ফলাফল || stop)
        বিরতি;
      i.সূচক++;
    }
  }
  ফেরত ফলাফল;
}

/* only returns SERIOUS errors */
স্থির CURLcode hsts_add_host_expire(কাঠামো hsts *h,
                                     ধ্রুবক অক্ষর *host, size_t hostlen,
                                     ধ্রুবক অক্ষর *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো stsentry *e;
  অক্ষর dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(ফাঁকা);

  /* The date parser works on a null-terminated string. */
  যদি(explen > MAX_HSTS_DATELEN)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  যদি(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  অন্যথা
    Curl_getdate_capped(dbuf, &expires);

  যদি(expires <= now)
    /* this entry already expired */
    ফেরত CURLE_OK;

  যদি(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  যদি(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  যদি(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    যদি(!e)
      ফলাফল = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    অন্যথা যদি((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      যদি(expires > e->expires)
        e->expires = expires;
      যদি(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  ফেরত ফলাফল;
}

/* only returns SERIOUS errors */
স্থির CURLcode hsts_add(কাঠামো hsts *h, ধ্রুবক অক্ষর *সারি)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  কাঠামো Curl_str host;
  কাঠামো Curl_str date;

  যদি(curlx_str_word(&সারি, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_quotedword(&সারি, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&সারি))
    ;
  অন্যথা {
    ফেরত hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ফেরত CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
স্থির CURLcode hsts_pull(কাঠামো Curl_easy *তথ্য, কাঠামো hsts *h)
{
  /* if the HSTS read callback is set, use it */
  যদি(তথ্য->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    করো {
      অক্ষর বাফার[MAX_HSTS_HOSTLEN + 1];
      কাঠামো curl_hstsentry e;
      e.name = বাফার;
      e.namelen = আকার(বাফার) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = তথ্য->set.hsts_read(তথ্য, &e, তথ্য->set.hsts_read_userp);
      যদি(sc == CURLSTS_OK) {
        CURLcode ফলাফল;
        ধ্রুবক অক্ষর *date = e.expire;
        যদি(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
        যদি(!date[0])
          date = UNLIMITED;
        ফলাফল = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
      অন্যথা যদি(sc == CURLSTS_FAIL)
        ফেরত CURLE_ABORTED_BY_CALLBACK;
    } যখন(sc == CURLSTS_OK);
  }
  ফেরত CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
স্থির CURLcode hsts_load(কাঠামো hsts *h, ধ্রুবক অক্ষর *file)
{
  CURLcode ফলাফল = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  যদি(!h->filename)
    ফেরত CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  যদি(fp) {
    curlx_struct_stat stat;
    যদি((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      কাঠামো dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_সারি);
      করো {
        ফলাফল = Curl_get_সারি(&buf, fp, &eof);
        যদি(!ফলাফল) {
          ধ্রুবক অক্ষর *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          যদি((*lineptr == '#') || strlen(lineptr) <= 1)
            চালু;

          hsts_add(h, lineptr);
        }
      } যখন(!ফলাফল && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ফেরত ফলাফল;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(কাঠামো Curl_easy *তথ্য,
                            কাঠামো hsts *h, ধ্রুবক অক্ষর *file)
{
  DEBUGASSERT(h);
  (শূন্য)তথ্য;
  ফেরত hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(কাঠামো Curl_easy *তথ্য, কাঠামো hsts *h)
{
  যদি(h)
    ফেরত hsts_pull(তথ্য, h);
  ফেরত CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো curl_slist *l = তথ্য->state.hstslist;
  যদি(l) {
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_HSTS, CURL_LOCK_ACCESS_SINGLE);

    যখন(l) {
      ফলাফল = Curl_hsts_loadfile(তথ্য, তথ্য->hsts, l->তথ্য);
      যদি(ফলাফল)
        বিরতি;
      l = l->পরবর্তী;
    }
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_HSTS);
  }
  ফেরত ফলাফল;
}

bool Curl_hsts_applies(কাঠামো hsts *h, ধ্রুবক কাঠামো Curl_peer *dest)
{
  ফেরত !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
