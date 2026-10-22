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

#define MAX_HSTS_سطر    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ایستا time_t hsts_debugtime(تهی *unused)
{
  ثابت نویسه *timestr = getenv("CURL_TIME");
  (تهی)unused;
  اگر(timestr) {
    curl_off_t val;
    اگر(!curlx_str_عدد(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    بازگشت (time_t)val;
  }
  بازگشت time(پوچ);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

ساختار hsts *Curl_hsts_init(تهی)
{
  ساختار hsts *h = curlx_calloc(1, اندازه(ساختار hsts));
  اگر(h) {
    Curl_llist_init(&h->list, پوچ);
  }
  بازگشت h;
}

#define hsts_free(x) curlx_free(x)

تهی Curl_hsts_cleanup(ساختار hsts **hp)
{
  ساختار hsts *h = *hp;
  اگر(h) {
    ساختار Curl_llist_گره *e;
    ساختار Curl_llist_گره *n;
    برای(e = Curl_llist_head(&h->list); e; e = n) {
      ساختار stsentry *sts = Curl_گره_elem(e);
      n = Curl_گره_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = پوچ;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
ایستا تهی hsts_append(ساختار hsts *h, ساختار stsentry *sts)
{
  اگر(Curl_llist_شمار(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ساختار Curl_llist_گره *e = Curl_llist_head(&h->list);
    ساختار stsentry *oldsts = Curl_گره_elem(e);
    Curl_گره_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->گره);
}

ایستا CURLcode hsts_create(ساختار hsts *h,
                            ثابت نویسه *hostname,
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
    ساختار stsentry *sts = curlx_calloc(1, اندازه(ساختار stsentry) + hlen);
    اگر(!sts)
      بازگشت CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  بازگشت CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ساختار hsts *dst, ساختار hsts *src)
{
  ساختار Curl_llist_گره *e;
  time_t now = time(پوچ);
  برای(e = Curl_llist_head(&src->list); e; e = Curl_گره_next(e)) {
    ساختار stsentry *sts = Curl_گره_elem(e);
    اگر(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      اگر(result)
        بازگشت result;
    }
  }
  بازگشت CURLE_OK;
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
UNITTEST ساختار stsentry *hsts_check(ساختار hsts *h, ثابت نویسه *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ساختار stsentry *hsts_check(ساختار hsts *h, ثابت نویسه *hostname,
                                     size_t hlen, bool subdomain)
{
  ساختار stsentry *bestsub = پوچ;
  اگر(h) {
    time_t now = time(پوچ);
    ساختار Curl_llist_گره *e;
    ساختار Curl_llist_گره *n;
    size_t blen = 0;

    اگر((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      بازگشت پوچ;
    اگر(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    برای(e = Curl_llist_head(&h->list); e; e = n) {
      ساختار stsentry *sts = Curl_گره_elem(e);
      size_t ntail;
      n = Curl_گره_next(e);
      اگر(sts->expires <= now) {
        /* remove expired entries */
        Curl_گره_remove(&sts->گره);
        hsts_free(sts);
        ادامه;
      }
      ntail = strlen(sts->host);
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
        بازگشت sts;
    }
  }
  بازگشت bestsub;
}

CURLcode Curl_hsts_parse(ساختار hsts *h, ثابت نویسه *hostname,
                         ثابت نویسه *header)
{
  ثابت نویسه *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ساختار stsentry *sts;
  time_t now = time(پوچ);
  size_t hlen = strlen(hostname);

  اگر(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    بازگشت CURLE_OK;

  انجامبده {
    ساختار Curl_str word;
    ساختار Curl_str val = { 0 };
    صحیح rc;
    bool assign = FALSE;

    انجامبده {
      curlx_str_passblanks(&p);
      اگر(*p == ';')
        p++;
      وگرنه
        بشکن;
    } وقتی(1);
    اگر(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      بشکن;

    curlx_str_passblanks(&p);
    اگر(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      اگر(*p == '\"') {
        اگر(curlx_str_quotedword(&p, &val, MAX_HSTS_سطر))
          بشکن;
      }
      وگرنه {
        اگر(curlx_str_cspn(&p, &val, ", ;\r\n"))
          بشکن;
      }
    }

    اگر(assign && curlx_str_casecompare(&word, "max-age")) {
      ثابت نویسه *vp = curlx_str(&val);
      اگر(gotma)
        بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_عدد(&vp, &expires, CAP_HSTS_MAX_AGE);
      اگر(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      وگرنه اگر(rc)
        /* invalid max-age */
        بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    وگرنه اگر(curlx_str_casecompare(&word, "includesubdomains")) {
      اگر(gotinc)
        بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } وقتی(*p);

  اگر(!gotma)
    /* max-age is mandatory */
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    اگر(sts) {
      Curl_گره_remove(&sts->گره);
      hsts_free(sts);
    }
    بازگشت CURLE_OK;
  }

  اگر(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  وگرنه
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  اگر(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  وگرنه
    بازگشت hsts_create(h, hostname, hlen, subdomains, expires);

  بازگشت CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ایستا CURLcode hsts_افزودن(ساختار Curl_easy *data,
                          ساختار curl_index *i,
                          ساختار stsentry *sts,
                          bool *stop)
{
  ساختار curl_hstsentry e;
  CURLSTScode sc;
  ساختار tm stamp;
  CURLcode result;

  e.name = (نویسه *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  اگر(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    اگر(result)
      بازگشت result;

    curl_msnprintf(e.expire, اندازه(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  وگرنه
    curlx_strcopy(e.expire, اندازه(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  بازگشت sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ایستا تهی hsts_out(ساختار stsentry *sts, FILE *fp)
{
  ساختار tm stamp;
  اگر(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    اگر(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  وگرنه
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ساختار Curl_easy *data, ساختار hsts *h,
                        ثابت نویسه *file)
{
  ساختار Curl_llist_گره *e;
  ساختار Curl_llist_گره *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  نویسه *tempstore = پوچ;

  اگر(!h)
    /* no cache activated */
    بازگشت CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  اگر(!file && h->filename)
    file = h->filename;

  اگر((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    برو skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  اگر(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    برای(e = Curl_llist_head(&h->list); e; e = n) {
      ساختار stsentry *sts = Curl_گره_elem(e);
      n = Curl_گره_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    اگر(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    اگر(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  اگر(data->set.hsts_write) {
    /* if there is a write callback */
    ساختار curl_index i; /*شمار*/
    i.total = Curl_llist_شمار(&h->list);
    i.index = 0;
    برای(e = Curl_llist_head(&h->list); e; e = n) {
      ساختار stsentry *sts = Curl_گره_elem(e);
      bool stop;
      n = Curl_گره_next(e);
      result = hsts_افزودن(data, &i, sts, &stop);
      اگر(result || stop)
        بشکن;
      i.index++;
    }
  }
  بازگشت result;
}

/* only returns SERIOUS errors */
ایستا CURLcode hsts_add_host_expire(ساختار hsts *h,
                                     ثابت نویسه *host, size_t hostlen,
                                     ثابت نویسه *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  ساختار stsentry *e;
  نویسه dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(پوچ);

  /* The date parser works on a null-terminated string. */
  اگر(explen > MAX_HSTS_DATELEN)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  اگر(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  وگرنه
    Curl_getdate_capped(dbuf, &expires);

  اگر(expires <= now)
    /* this entry already expired */
    بازگشت CURLE_OK;

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
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    وگرنه اگر((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      اگر(expires > e->expires)
        e->expires = expires;
      اگر(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  بازگشت result;
}

/* only returns SERIOUS errors */
ایستا CURLcode hsts_add(ساختار hsts *h, ثابت نویسه *سطر)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ساختار Curl_str host;
  ساختار Curl_str date;

  اگر(curlx_str_word(&سطر, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_quotedword(&سطر, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&سطر))
    ;
  وگرنه {
    بازگشت hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  بازگشت CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ایستا CURLcode hsts_pull(ساختار Curl_easy *data, ساختار hsts *h)
{
  /* if the HSTS read callback is set, use it */
  اگر(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    انجامبده {
      نویسه buffer[MAX_HSTS_HOSTLEN + 1];
      ساختار curl_hstsentry e;
      e.name = buffer;
      e.namelen = اندازه(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      اگر(sc == CURLSTS_OK) {
        CURLcode result;
        ثابت نویسه *date = e.expire;
        اگر(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
        اگر(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        اگر(result)
          بازگشت result;
      }
      وگرنه اگر(sc == CURLSTS_FAIL)
        بازگشت CURLE_ABORTED_BY_CALLBACK;
    } وقتی(sc == CURLSTS_OK);
  }
  بازگشت CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ایستا CURLcode hsts_load(ساختار hsts *h, ثابت نویسه *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  اگر(!h->filename)
    بازگشت CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  اگر(fp) {
    curlx_struct_stat stat;
    اگر((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      ساختار dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_سطر);
      انجامبده {
        result = Curl_get_سطر(&buf, fp, &eof);
        اگر(!result) {
          ثابت نویسه *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          اگر((*lineptr == '#') || strlen(lineptr) <= 1)
            ادامه;

          hsts_add(h, lineptr);
        }
      } وقتی(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  بازگشت result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ساختار Curl_easy *data,
                            ساختار hsts *h, ثابت نویسه *file)
{
  DEBUGASSERT(h);
  (تهی)data;
  بازگشت hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ساختار Curl_easy *data, ساختار hsts *h)
{
  اگر(h)
    بازگشت hsts_pull(data, h);
  بازگشت CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ساختار curl_slist *l = data->state.hstslist;
  اگر(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    وقتی(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      اگر(result)
        بشکن;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  بازگشت result;
}

bool Curl_hsts_applies(ساختار hsts *h, ثابت ساختار Curl_peer *dest)
{
  بازگشت !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
