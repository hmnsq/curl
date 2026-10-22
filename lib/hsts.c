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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#شامل "urldata.h"
#شامل "llist.h"
#شامل "hsts.h"
#شامل "curl_fopen.h"
#شامل "curl_get_line.h"
#شامل "parsedate.h"
#شامل "curl_share.h"
#شامل "curlx/strparse.h"
#شامل "curlx/strcopy.h"

#تعریف_کرو MAX_HSTS_سطر    4095
#تعریف_کرو MAX_HSTS_HOSTLEN 2048
#تعریف_کرو MAX_HSTS_DATELEN 17
#تعریف_کرو UNLIMITED        "unlimited"

#تعریف_کرو CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ساکن time_t hsts_debugtime(خالی *غیر_مستعمل)
{
  ثابت حرف *timestr = getenv("CURL_TIME");
  (خالی)غیر_مستعمل;
  اگر(timestr) {
    curl_off_t val;
    اگر(!curlx_str_نمبر(&timestr, &val, وقت_T_MAX))
      val += (curl_off_t)deltatime;
    واپس (time_t)val;
  }
  واپس وقت(تہی);
}
#تعریف_منسوخ وقت
#تعریف_کرو وقت(x) hsts_debugtime(x)
#اگر_ختم

ساخت hsts *Curl_hsts_init(خالی)
{
  ساخت hsts *h = curlx_calloc(1, حجم(ساخت hsts));
  اگر(h) {
    Curl_llist_init(&h->list, تہی);
  }
  واپس h;
}

#تعریف_کرو hsts_free(x) curlx_free(x)

خالی Curl_hsts_cleanup(ساخت hsts **hp)
{
  ساخت hsts *h = *hp;
  اگر(h) {
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_llist_گرہ *n;
    برائے(e = Curl_llist_سر(&h->list); e; e = n) {
      ساخت stsentry *sts = Curl_گرہ_elem(e);
      n = Curl_گرہ_اگلا(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = تہی;
  }
}

/* append the new entry to the list after possibly removing an old entry
پہلا*/
ساکن خالی hsts_append(ساخت hsts *h, ساخت stsentry *sts)
{
  اگر(Curl_llist_گنتی(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ساخت Curl_llist_گرہ *e = Curl_llist_سر(&h->list);
    ساخت stsentry *oldsts = Curl_گرہ_elem(e);
    Curl_گرہ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->گرہ);
}

ساکن CURLcode hsts_create(ساخت hsts *h,
                            ثابت حرف *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  اگر(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  اگر(hlen) {
    ساخت stsentry *sts = curlx_calloc(1, حجم(ساخت stsentry) + hlen);
    اگر(!sts)
      واپس CURLE_برآمد_کا_حافظہ;
    /* the null-terminator is already there */
    حافظہ_نقل(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  واپس CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ساخت hsts *dst, ساخت hsts *src)
{
  ساخت Curl_llist_گرہ *e;
  time_t now = وقت(تہی);
  برائے(e = Curl_llist_سر(&src->list); e; e = Curl_گرہ_اگلا(e)) {
    ساخت stsentry *sts = Curl_گرہ_elem(e);
    اگر(sts->expires > now) {
      CURLcode نتیجہ = hsts_create(dst, sts->host, لڑی_لمبائی(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }
  واپس CURLE_OK;
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
UNITTEST ساخت stsentry *hsts_check(ساخت hsts *h, ثابت حرف *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ساخت stsentry *hsts_check(ساخت hsts *h, ثابت حرف *hostname,
                                     size_t hlen, bool subdomain)
{
  ساخت stsentry *bestsub = تہی;
  اگر(h) {
    time_t now = وقت(تہی);
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_llist_گرہ *n;
    size_t blen = 0;

    اگر((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      واپس تہی;
    اگر(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    برائے(e = Curl_llist_سر(&h->list); e; e = n) {
      ساخت stsentry *sts = Curl_گرہ_elem(e);
      size_t ntail;
      n = Curl_گرہ_اگلا(e);
      اگر(sts->expires <= now) {
        /* remove expired entries */
        Curl_گرہ_remove(&sts->گرہ);
        hsts_free(sts);
        جاری;
      }
      ntail = لڑی_لمبائی(sts->host);
      اگر((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        اگر((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      اگر((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        واپس sts;
    }
  }
  واپس bestsub;
}

CURLcode Curl_hsts_parse(ساخت hsts *h, ثابت حرف *hostname,
                         ثابت حرف *سرخی)
{
  ثابت حرف *p = سرخی;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ساخت stsentry *sts;
  time_t now = وقت(تہی);
  size_t hlen = لڑی_لمبائی(hostname);

  اگر(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    واپس CURLE_OK;

  کرو {
    ساخت Curl_str لفظ;
    ساخت Curl_str val = { 0 };
    صحیح rc;
    bool تفویض = FALSE;

    کرو {
      curlx_str_passblanks(&p);
      اگر(*p == ';')
        p++;
      ورنہ
        توڑو;
    } جب(1);
    اگر(curlx_str_cspn(&p, &لفظ, ";=\r\n \t"))
      توڑو;

    curlx_str_passblanks(&p);
    اگر(!curlx_str_single(&p, '=')) {
      تفویض = TRUE;
      curlx_str_passblanks(&p);

      اگر(*p == '\"') {
        اگر(curlx_str_quotedword(&p, &val, MAX_HSTS_سطر))
          توڑو;
      }
      ورنہ {
        اگر(curlx_str_cspn(&p, &val, ", ;\r\n"))
          توڑو;
      }
    }

    اگر(تفویض && curlx_str_casecompare(&لفظ, "max-age")) {
      ثابت حرف *vp = curlx_str(&val);
      اگر(gotma)
        واپس CURLE_BAD_FUNCTION_دلیل;
      rc = curlx_str_نمبر(&vp, &expires, CAP_HSTS_MAX_AGE);
      اگر(rc == STRE_حد_سے_تجاوز)
        expires = CAP_HSTS_MAX_AGE;
      ورنہ اگر(rc)
        /* invalid max-age */
        واپس CURLE_BAD_FUNCTION_دلیل;

      gotma = TRUE;
    }
    ورنہ اگر(curlx_str_casecompare(&لفظ, "includesubdomains")) {
      اگر(gotinc)
        واپس CURLE_BAD_FUNCTION_دلیل;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } جب(*p);

  اگر(!gotma)
    /* max-age is mandatory */
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    اگر(sts) {
      Curl_گرہ_remove(&sts->گرہ);
      hsts_free(sts);
    }
    واپس CURLE_OK;
  }

  اگر(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  ورنہ
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  اگر(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  ورنہ
    واپس hsts_create(h, hostname, hlen, subdomains, expires);

  واپس CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ساکن CURLcode hsts_ڈالو(ساخت Curl_easy *ڈیٹا,
                          ساخت curl_اشاریہ *i,
                          ساخت stsentry *sts,
                          bool *stop)
{
  ساخت curl_hstsentry e;
  CURLSTScode sc;
  ساخت tm stamp;
  CURLcode نتیجہ;

  e.name = (حرف *)sts->host;
  e.namelen = لڑی_لمبائی(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  اگر(sts->expires != وقت_T_MAX) {
    نتیجہ = curlx_gmtime((time_t)sts->expires, &stamp);
    اگر(نتیجہ)
      واپس نتیجہ;

    curl_msnprintf(e.expire, حجم(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ورنہ
    curlx_strcopy(e.expire, حجم(e.expire), STRCONST(UNLIMITED));

  sc = ڈیٹا->سیٹ.hsts_write(ڈیٹا, &e, i, ڈیٹا->سیٹ.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  واپس sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_دلیل : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ساکن خالی hsts_برآمد(ساخت stsentry *sts, FILE *fp)
{
  ساخت tm stamp;
  اگر(sts->expires != وقت_T_MAX) {
    CURLcode نتیجہ = curlx_gmtime((time_t)sts->expires, &stamp);
    اگر(!نتیجہ)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ورنہ
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ساخت Curl_easy *ڈیٹا, ساخت hsts *h,
                        ثابت حرف *file)
{
  ساخت Curl_llist_گرہ *e;
  ساخت Curl_llist_گرہ *n;
  CURLcode نتیجہ = CURLE_OK;
  FILE *برآمد;
  حرف *tempstore = تہی;

  اگر(!h)
    /* no cache activated */
    واپس CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  اگر(!file && h->filename)
    file = h->filename;

  اگر((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    جاؤ skipsave;

  نتیجہ = Curl_fopen(ڈیٹا, file, &برآمد, &tempstore);
  اگر(!نتیجہ) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          برآمد);
    برائے(e = Curl_llist_سر(&h->list); e; e = n) {
      ساخت stsentry *sts = Curl_گرہ_elem(e);
      n = Curl_گرہ_اگلا(e);
      hsts_برآمد(sts, برآمد);
    }
    curlx_fclose(برآمد);
    اگر(!نتیجہ && tempstore && curlx_rename(tempstore, file))
      نتیجہ = CURLE_WRITE_ERROR;

    اگر(نتیجہ && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  اگر(ڈیٹا->سیٹ.hsts_write) {
    /* if there is a write callback */
    ساخت curl_اشاریہ i; /*گنتی*/
    i.کل = Curl_llist_گنتی(&h->list);
    i.اشاریہ = 0;
    برائے(e = Curl_llist_سر(&h->list); e; e = n) {
      ساخت stsentry *sts = Curl_گرہ_elem(e);
      bool stop;
      n = Curl_گرہ_اگلا(e);
      نتیجہ = hsts_ڈالو(ڈیٹا, &i, sts, &stop);
      اگر(نتیجہ || stop)
        توڑو;
      i.اشاریہ++;
    }
  }
  واپس نتیجہ;
}

/* only returns SERIOUS errors */
ساکن CURLcode hsts_جوڑو_host_expire(ساخت hsts *h,
                                     ثابت حرف *host, size_t hostlen,
                                     ثابت حرف *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت stsentry *e;
  حرف dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = وقت(تہی);

  /* The date parser works on a null-terminated string. */
  اگر(explen > MAX_HSTS_DATELEN)
    واپس CURLE_BAD_FUNCTION_دلیل;
  حافظہ_نقل(dbuf, expire, explen);
  dbuf[explen] = 0;

  اگر(!strcmp(dbuf, UNLIMITED))
    expires = وقت_T_MAX;
  ورنہ
    Curl_getdate_capped(dbuf, &expires);

  اگر(expires <= now)
    /* this entry already expired */
    واپس CURLE_OK;

  اگر(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  اگر(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  اگر(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    اگر(!e)
      نتیجہ = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    ورنہ اگر((hostlen == لڑی_لمبائی(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      اگر(expires > e->expires)
        e->expires = expires;
      اگر(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  واپس نتیجہ;
}

/* only returns SERIOUS errors */
ساکن CURLcode hsts_جوڑو(ساخت hsts *h, ثابت حرف *سطر)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ساخت Curl_str host;
  ساخت Curl_str date;

  اگر(curlx_str_لفظ(&سطر, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_quotedword(&سطر, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&سطر))
    ;
  ورنہ {
    واپس hsts_جوڑو_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  واپس CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ساکن CURLcode hsts_pull(ساخت Curl_easy *ڈیٹا, ساخت hsts *h)
{
  /* if the HSTS read callback is set, use it */
  اگر(ڈیٹا->سیٹ.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    کرو {
      حرف بفر[MAX_HSTS_HOSTLEN + 1];
      ساخت curl_hstsentry e;
      e.name = بفر;
      e.namelen = حجم(بفر) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = ڈیٹا->سیٹ.hsts_read(ڈیٹا, &e, ڈیٹا->سیٹ.hsts_read_userp);
      اگر(sc == CURLSTS_OK) {
        CURLcode نتیجہ;
        ثابت حرف *date = e.expire;
        اگر(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          واپس CURLE_BAD_FUNCTION_دلیل;
        اگر(!date[0])
          date = UNLIMITED;
        نتیجہ = hsts_جوڑو_host_expire(h, e.name, لڑی_لمبائی(e.name),
                                      date, لڑی_لمبائی(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
      ورنہ اگر(sc == CURLSTS_FAIL)
        واپس CURLE_ABORTED_BY_CALLBACK;
    } جب(sc == CURLSTS_OK);
  }
  واپس CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ساکن CURLcode hsts_بھرو(ساخت hsts *h, ثابت حرف *file)
{
  CURLcode نتیجہ = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  اگر(!h->filename)
    واپس CURLE_برآمد_کا_حافظہ;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  اگر(fp) {
    curlx_struct_stat stat;
    اگر((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_منوال)) {
      ساخت dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_سطر);
      کرو {
        نتیجہ = Curl_get_سطر(&buf, fp, &eof);
        اگر(!نتیجہ) {
          ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          اگر((*lineptr == '#') || لڑی_لمبائی(lineptr) <= 1)
            جاری;

          hsts_جوڑو(h, lineptr);
        }
      } جب(!نتیجہ && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  واپس نتیجہ;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ساخت Curl_easy *ڈیٹا,
                            ساخت hsts *h, ثابت حرف *file)
{
  DEBUGASSERT(h);
  (خالی)ڈیٹا;
  واپس hsts_بھرو(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ساخت Curl_easy *ڈیٹا, ساخت hsts *h)
{
  اگر(h)
    واپس hsts_pull(ڈیٹا, h);
  واپس CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت curl_slist *l = ڈیٹا->state.hstslist;
  اگر(l) {
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_HSTS, CURL_تالا_ACCESS_SINGLE);

    جب(l) {
      نتیجہ = Curl_hsts_loadfile(ڈیٹا, ڈیٹا->hsts, l->ڈیٹا);
      اگر(نتیجہ)
        توڑو;
      l = l->اگلا;
    }
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_HSTS);
  }
  واپس نتیجہ;
}

bool Curl_hsts_applies(ساخت hsts *h, ثابت ساخت Curl_peer *dest)
{
  واپس !!hsts_check(h, dest->hostname, لڑی_لمبائی(dest->hostname), TRUE);
}

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
#تعریف_منسوخ وقت
#اگر_ختم

#اگر_ختم /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
