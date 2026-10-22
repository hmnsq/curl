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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ضمن "urldata.h"
#ضمن "llist.h"
#ضمن "hsts.h"
#ضمن "curl_fopen.h"
#ضمن "curl_get_line.h"
#ضمن "parsedate.h"
#ضمن "curl_share.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/strcopy.h"

#وصف_ڪريو MAX_HSTS_سٽ    4095
#وصف_ڪريو MAX_HSTS_HOSTLEN 2048
#وصف_ڪريو MAX_HSTS_DATELEN 17
#وصف_ڪريو UNLIMITED        "unlimited"

#وصف_ڪريو CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#إذا defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ساكن time_t hsts_debugtime(فراغ *اڻ_استعمال_ٿيل)
{
  ثابت حرف *timestr = getenv("CURL_TIME");
  (فراغ)اڻ_استعمال_ٿيل;
  إذا(timestr) {
    curl_off_t val;
    إذا(!curlx_str_عدد(&timestr, &val, وقت_T_MAX))
      val += (curl_off_t)deltatime;
    أرجع (time_t)val;
  }
  أرجع وقت(عدم);
}
#وصف_رد وقت
#وصف_ڪريو وقت(x) hsts_debugtime(x)
#جيڪڏهن_پورو

بنية hsts *Curl_hsts_init(فراغ)
{
  بنية hsts *h = curlx_calloc(1, حجم(بنية hsts));
  إذا(h) {
    Curl_llist_init(&h->list, عدم);
  }
  أرجع h;
}

#وصف_ڪريو hsts_free(x) curlx_free(x)

فراغ Curl_hsts_cleanup(بنية hsts **hp)
{
  بنية hsts *h = *hp;
  إذا(h) {
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_llist_ڳنڍ *n;
    لكل(e = Curl_llist_سر(&h->list); e; e = n) {
      بنية stsentry *sts = Curl_ڳنڍ_elem(e);
      n = Curl_ڳنڍ_ايندڙ(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = عدم;
  }
}

/* append the new entry to the list after possibly removing an old entry
پهريون*/
ساكن فراغ hsts_append(بنية hsts *h, بنية stsentry *sts)
{
  إذا(Curl_llist_ڳڻپ(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    بنية Curl_llist_ڳنڍ *e = Curl_llist_سر(&h->list);
    بنية stsentry *oldsts = Curl_ڳنڍ_elem(e);
    Curl_ڳنڍ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->ڳنڍ);
}

ساكن CURLcode hsts_create(بنية hsts *h,
                            ثابت حرف *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  إذا(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  إذا(hlen) {
    بنية stsentry *sts = curlx_calloc(1, حجم(بنية stsentry) + hlen);
    إذا(!sts)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    /* the null-terminator is already there */
    يادگيري_نقل(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  أرجع CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(بنية hsts *dst, بنية hsts *src)
{
  بنية Curl_llist_ڳنڍ *e;
  time_t now = وقت(عدم);
  لكل(e = Curl_llist_سر(&src->list); e; e = Curl_ڳنڍ_ايندڙ(e)) {
    بنية stsentry *sts = Curl_ڳنڍ_elem(e);
    إذا(sts->expires > now) {
      CURLcode نتيجو = hsts_create(dst, sts->host, لڙي_ڊيگهه(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }
  أرجع CURLE_OK;
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
UNITTEST بنية stsentry *hsts_check(بنية hsts *h, ثابت حرف *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST بنية stsentry *hsts_check(بنية hsts *h, ثابت حرف *hostname,
                                     size_t hlen, bool subdomain)
{
  بنية stsentry *bestsub = عدم;
  إذا(h) {
    time_t now = وقت(عدم);
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_llist_ڳنڍ *n;
    size_t blen = 0;

    إذا((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      أرجع عدم;
    إذا(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    لكل(e = Curl_llist_سر(&h->list); e; e = n) {
      بنية stsentry *sts = Curl_ڳنڍ_elem(e);
      size_t ntail;
      n = Curl_ڳنڍ_ايندڙ(e);
      إذا(sts->expires <= now) {
        /* remove expired entries */
        Curl_ڳنڍ_remove(&sts->ڳنڍ);
        hsts_free(sts);
        استمر;
      }
      ntail = لڙي_ڊيگهه(sts->host);
      إذا((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        إذا((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      إذا((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        أرجع sts;
    }
  }
  أرجع bestsub;
}

CURLcode Curl_hsts_parse(بنية hsts *h, ثابت حرف *hostname,
                         ثابت حرف *سرو)
{
  ثابت حرف *p = سرو;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  بنية stsentry *sts;
  time_t now = وقت(عدم);
  size_t hlen = لڙي_ڊيگهه(hostname);

  إذا(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    أرجع CURLE_OK;

  افعل {
    بنية Curl_str لفظ;
    بنية Curl_str val = { 0 };
    صحيح rc;
    bool سونپيو = FALSE;

    افعل {
      curlx_str_passblanks(&p);
      إذا(*p == ';')
        p++;
      وإلا
        اكسر;
    } طالما(1);
    إذا(curlx_str_cspn(&p, &لفظ, ";=\r\n \t"))
      اكسر;

    curlx_str_passblanks(&p);
    إذا(!curlx_str_single(&p, '=')) {
      سونپيو = TRUE;
      curlx_str_passblanks(&p);

      إذا(*p == '\"') {
        إذا(curlx_str_quotedword(&p, &val, MAX_HSTS_سٽ))
          اكسر;
      }
      وإلا {
        إذا(curlx_str_cspn(&p, &val, ", ;\r\n"))
          اكسر;
      }
    }

    إذا(سونپيو && curlx_str_casecompare(&لفظ, "max-age")) {
      ثابت حرف *vp = curlx_str(&val);
      إذا(gotma)
        أرجع CURLE_BAD_FUNCTION_دليل;
      rc = curlx_str_عدد(&vp, &expires, CAP_HSTS_MAX_AGE);
      إذا(rc == STRE_حد_کان_مٿي)
        expires = CAP_HSTS_MAX_AGE;
      وإلا إذا(rc)
        /* invalid max-age */
        أرجع CURLE_BAD_FUNCTION_دليل;

      gotma = TRUE;
    }
    وإلا إذا(curlx_str_casecompare(&لفظ, "includesubdomains")) {
      إذا(gotinc)
        أرجع CURLE_BAD_FUNCTION_دليل;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } طالما(*p);

  إذا(!gotma)
    /* max-age is mandatory */
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    إذا(sts) {
      Curl_ڳنڍ_remove(&sts->ڳنڍ);
      hsts_free(sts);
    }
    أرجع CURLE_OK;
  }

  إذا(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  وإلا
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  إذا(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  وإلا
    أرجع hsts_create(h, hostname, hlen, subdomains, expires);

  أرجع CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ساكن CURLcode hsts_ڌڪو(بنية Curl_easy *معلومات,
                          بنية curl_اشاريو *i,
                          بنية stsentry *sts,
                          bool *stop)
{
  بنية curl_hstsentry e;
  CURLSTScode sc;
  بنية tm stamp;
  CURLcode نتيجو;

  e.name = (حرف *)sts->host;
  e.namelen = لڙي_ڊيگهه(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  إذا(sts->expires != وقت_T_MAX) {
    نتيجو = curlx_gmtime((time_t)sts->expires, &stamp);
    إذا(نتيجو)
      أرجع نتيجو;

    curl_msnprintf(e.expire, حجم(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  وإلا
    curlx_strcopy(e.expire, حجم(e.expire), STRCONST(UNLIMITED));

  sc = معلومات->سيٽ.hsts_write(معلومات, &e, i, معلومات->سيٽ.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  أرجع sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_دليل : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ساكن فراغ hsts_ٻاهريون(بنية stsentry *sts, FILE *fp)
{
  بنية tm stamp;
  إذا(sts->expires != وقت_T_MAX) {
    CURLcode نتيجو = curlx_gmtime((time_t)sts->expires, &stamp);
    إذا(!نتيجو)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  وإلا
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(بنية Curl_easy *معلومات, بنية hsts *h,
                        ثابت حرف *file)
{
  بنية Curl_llist_ڳنڍ *e;
  بنية Curl_llist_ڳنڍ *n;
  CURLcode نتيجو = CURLE_OK;
  FILE *ٻاهريون;
  حرف *tempstore = عدم;

  إذا(!h)
    /* no cache activated */
    أرجع CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  إذا(!file && h->filename)
    file = h->filename;

  إذا((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    اذهب skipsave;

  نتيجو = Curl_fopen(معلومات, file, &ٻاهريون, &tempstore);
  إذا(!نتيجو) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          ٻاهريون);
    لكل(e = Curl_llist_سر(&h->list); e; e = n) {
      بنية stsentry *sts = Curl_ڳنڍ_elem(e);
      n = Curl_ڳنڍ_ايندڙ(e);
      hsts_ٻاهريون(sts, ٻاهريون);
    }
    curlx_fclose(ٻاهريون);
    إذا(!نتيجو && tempstore && curlx_rename(tempstore, file))
      نتيجو = CURLE_WRITE_ERROR;

    إذا(نتيجو && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  إذا(معلومات->سيٽ.hsts_write) {
    /* if there is a write callback */
    بنية curl_اشاريو i; /*ڳڻپ*/
    i.ڪل = Curl_llist_ڳڻپ(&h->list);
    i.اشاريو = 0;
    لكل(e = Curl_llist_سر(&h->list); e; e = n) {
      بنية stsentry *sts = Curl_ڳنڍ_elem(e);
      bool stop;
      n = Curl_ڳنڍ_ايندڙ(e);
      نتيجو = hsts_ڌڪو(معلومات, &i, sts, &stop);
      إذا(نتيجو || stop)
        اكسر;
      i.اشاريو++;
    }
  }
  أرجع نتيجو;
}

/* only returns SERIOUS errors */
ساكن CURLcode hsts_جوڙيو_host_expire(بنية hsts *h,
                                     ثابت حرف *host, size_t hostlen,
                                     ثابت حرف *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode نتيجو = CURLE_OK;
  بنية stsentry *e;
  حرف dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = وقت(عدم);

  /* The date parser works on a null-terminated string. */
  إذا(explen > MAX_HSTS_DATELEN)
    أرجع CURLE_BAD_FUNCTION_دليل;
  يادگيري_نقل(dbuf, expire, explen);
  dbuf[explen] = 0;

  إذا(!strcmp(dbuf, UNLIMITED))
    expires = وقت_T_MAX;
  وإلا
    Curl_getdate_capped(dbuf, &expires);

  إذا(expires <= now)
    /* this entry already expired */
    أرجع CURLE_OK;

  إذا(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  إذا(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  إذا(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    إذا(!e)
      نتيجو = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    وإلا إذا((hostlen == لڙي_ڊيگهه(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      إذا(expires > e->expires)
        e->expires = expires;
      إذا(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  أرجع نتيجو;
}

/* only returns SERIOUS errors */
ساكن CURLcode hsts_جوڙيو(بنية hsts *h, ثابت حرف *سٽ)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  بنية Curl_str host;
  بنية Curl_str date;

  إذا(curlx_str_لفظ(&سٽ, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_quotedword(&سٽ, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&سٽ))
    ;
  وإلا {
    أرجع hsts_جوڙيو_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  أرجع CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ساكن CURLcode hsts_pull(بنية Curl_easy *معلومات, بنية hsts *h)
{
  /* if the HSTS read callback is set, use it */
  إذا(معلومات->سيٽ.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    افعل {
      حرف بفر[MAX_HSTS_HOSTLEN + 1];
      بنية curl_hstsentry e;
      e.name = بفر;
      e.namelen = حجم(بفر) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = معلومات->سيٽ.hsts_read(معلومات, &e, معلومات->سيٽ.hsts_read_userp);
      إذا(sc == CURLSTS_OK) {
        CURLcode نتيجو;
        ثابت حرف *date = e.expire;
        إذا(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          أرجع CURLE_BAD_FUNCTION_دليل;
        إذا(!date[0])
          date = UNLIMITED;
        نتيجو = hsts_جوڙيو_host_expire(h, e.name, لڙي_ڊيگهه(e.name),
                                      date, لڙي_ڊيگهه(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        إذا(نتيجو)
          أرجع نتيجو;
      }
      وإلا إذا(sc == CURLSTS_FAIL)
        أرجع CURLE_ABORTED_BY_CALLBACK;
    } طالما(sc == CURLSTS_OK);
  }
  أرجع CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ساكن CURLcode hsts_ڀريو(بنية hsts *h, ثابت حرف *file)
{
  CURLcode نتيجو = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  إذا(!h->filename)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  إذا(fp) {
    curlx_struct_stat stat;
    إذا((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_گهڻائي)) {
      بنية dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_سٽ);
      افعل {
        نتيجو = Curl_get_سٽ(&buf, fp, &eof);
        إذا(!نتيجو) {
          ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          إذا((*lineptr == '#') || لڙي_ڊيگهه(lineptr) <= 1)
            استمر;

          hsts_جوڙيو(h, lineptr);
        }
      } طالما(!نتيجو && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  أرجع نتيجو;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(بنية Curl_easy *معلومات,
                            بنية hsts *h, ثابت حرف *file)
{
  DEBUGASSERT(h);
  (فراغ)معلومات;
  أرجع hsts_ڀريو(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(بنية Curl_easy *معلومات, بنية hsts *h)
{
  إذا(h)
    أرجع hsts_pull(معلومات, h);
  أرجع CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  بنية curl_slist *l = معلومات->state.hstslist;
  إذا(l) {
    Curl_share_تالو(معلومات, CURL_تالو_معلومات_HSTS, CURL_تالو_ACCESS_SINGLE);

    طالما(l) {
      نتيجو = Curl_hsts_loadfile(معلومات, معلومات->hsts, l->معلومات);
      إذا(نتيجو)
        اكسر;
      l = l->ايندڙ;
    }
    Curl_share_unlock(معلومات, CURL_تالو_معلومات_HSTS);
  }
  أرجع نتيجو;
}

bool Curl_hsts_applies(بنية hsts *h, ثابت بنية Curl_peer *dest)
{
  أرجع !!hsts_check(h, dest->hostname, لڙي_ڊيگهه(dest->hostname), TRUE);
}

#إذا defined(DEBUGBUILD) || defined(UNITTESTS)
#وصف_رد وقت
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
