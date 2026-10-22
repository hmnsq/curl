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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "llist.h"
#অন্তর্ভাৱ "hsts.h"
#অন্তর্ভাৱ "curl_fopen.h"
#অন্তর্ভাৱ "curl_get_line.h"
#অন্তর্ভাৱ "parsedate.h"
#অন্তর্ভাৱ "curl_share.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "curlx/strcopy.h"

#define MAX_HSTS_সাৰি    4095
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
  ধ্রুৱ ৱর্ণ *timestr = getenv("CURL_TIME");
  (শূন্য)unused;
  যদি(timestr) {
    curl_off_t val;
    যদি(!curlx_str_সংখ্যা(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    ঘূৰাই (time_t)val;
  }
  ঘূৰাই time(রিক্ত);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

সংরচনা hsts *Curl_hsts_init(শূন্য)
{
  সংরচনা hsts *h = curlx_calloc(1, পরিমাণ(সংরচনা hsts));
  যদি(h) {
    Curl_llist_init(&h->list, রিক্ত);
  }
  ঘূৰাই h;
}

#define hsts_free(x) curlx_free(x)

শূন্য Curl_hsts_cleanup(সংরচনা hsts **hp)
{
  সংরচনা hsts *h = *hp;
  যদি(h) {
    সংরচনা Curl_llist_গ্ৰন্থি *e;
    সংরচনা Curl_llist_গ্ৰন্থি *n;
    বাবে(e = Curl_llist_head(&h->list); e; e = n) {
      সংরচনা stsentry *sts = Curl_গ্ৰন্থি_elem(e);
      n = Curl_গ্ৰন্থি_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = রিক্ত;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
স্থির শূন্য hsts_append(সংরচনা hsts *h, সংরচনা stsentry *sts)
{
  যদি(Curl_llist_গুনতি(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    সংরচনা Curl_llist_গ্ৰন্থি *e = Curl_llist_head(&h->list);
    সংরচনা stsentry *oldsts = Curl_গ্ৰন্থি_elem(e);
    Curl_গ্ৰন্থি_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->গ্ৰন্থি);
}

স্থির CURLcode hsts_create(সংরচনা hsts *h,
                            ধ্রুৱ ৱর্ণ *hostname,
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
    সংরচনা stsentry *sts = curlx_calloc(1, পরিমাণ(সংরচনা stsentry) + hlen);
    যদি(!sts)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ঘূৰাই CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(সংরচনা hsts *dst, সংরচনা hsts *src)
{
  সংরচনা Curl_llist_গ্ৰন্থি *e;
  time_t now = time(রিক্ত);
  বাবে(e = Curl_llist_head(&src->list); e; e = Curl_গ্ৰন্থি_next(e)) {
    সংরচনা stsentry *sts = Curl_গ্ৰন্থি_elem(e);
    যদি(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      যদি(result)
        ঘূৰাই result;
    }
  }
  ঘূৰাই CURLE_OK;
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
UNITTEST সংরচনা stsentry *hsts_check(সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST সংরচনা stsentry *hsts_check(সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *hostname,
                                     size_t hlen, bool subdomain)
{
  সংরচনা stsentry *bestsub = রিক্ত;
  যদি(h) {
    time_t now = time(রিক্ত);
    সংরচনা Curl_llist_গ্ৰন্থি *e;
    সংরচনা Curl_llist_গ্ৰন্থি *n;
    size_t blen = 0;

    যদি((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ঘূৰাই রিক্ত;
    যদি(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    বাবে(e = Curl_llist_head(&h->list); e; e = n) {
      সংরচনা stsentry *sts = Curl_গ্ৰন্থি_elem(e);
      size_t ntail;
      n = Curl_গ্ৰন্থি_next(e);
      যদি(sts->expires <= now) {
        /* remove expired entries */
        Curl_গ্ৰন্থি_remove(&sts->গ্ৰন্থি);
        hsts_free(sts);
        অনুৱর্ত;
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
        ঘূৰাই sts;
    }
  }
  ঘূৰাই bestsub;
}

CURLcode Curl_hsts_parse(সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *hostname,
                         ধ্রুৱ ৱর্ণ *header)
{
  ধ্রুৱ ৱর্ণ *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  সংরচনা stsentry *sts;
  time_t now = time(রিক্ত);
  size_t hlen = strlen(hostname);

  যদি(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ঘূৰাই CURLE_OK;

  কৰা {
    সংরচনা Curl_str word;
    সংরচনা Curl_str val = { 0 };
    পূর্ণাঙ্ক rc;
    bool assign = FALSE;

    কৰা {
      curlx_str_passblanks(&p);
      যদি(*p == ';')
        p++;
      অন্যথা
        ভঙ্গ;
    } যেতিয়ালৈ(1);
    যদি(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      ভঙ্গ;

    curlx_str_passblanks(&p);
    যদি(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      যদি(*p == '\"') {
        যদি(curlx_str_quotedword(&p, &val, MAX_HSTS_সাৰি))
          ভঙ্গ;
      }
      অন্যথা {
        যদি(curlx_str_cspn(&p, &val, ", ;\r\n"))
          ভঙ্গ;
      }
    }

    যদি(assign && curlx_str_casecompare(&word, "max-age")) {
      ধ্রুৱ ৱর্ণ *vp = curlx_str(&val);
      যদি(gotma)
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_সংখ্যা(&vp, &expires, CAP_HSTS_MAX_AGE);
      যদি(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      অন্যথা যদি(rc)
        /* invalid max-age */
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    অন্যথা যদি(curlx_str_casecompare(&word, "includesubdomains")) {
      যদি(gotinc)
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } যেতিয়ালৈ(*p);

  যদি(!gotma)
    /* max-age is mandatory */
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    যদি(sts) {
      Curl_গ্ৰন্থি_remove(&sts->গ্ৰন্থি);
      hsts_free(sts);
    }
    ঘূৰাই CURLE_OK;
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
    ঘূৰাই hsts_create(h, hostname, hlen, subdomains, expires);

  ঘূৰাই CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
স্থির CURLcode hsts_ঠেলো(সংরচনা Curl_easy *data,
                          সংরচনা curl_index *i,
                          সংরচনা stsentry *sts,
                          bool *stop)
{
  সংরচনা curl_hstsentry e;
  CURLSTScode sc;
  সংরচনা tm stamp;
  CURLcode result;

  e.name = (ৱর্ণ *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  যদি(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    যদি(result)
      ঘূৰাই result;

    curl_msnprintf(e.expire, পরিমাণ(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  অন্যথা
    curlx_strcopy(e.expire, পরিমাণ(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ঘূৰাই sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
স্থির শূন্য hsts_out(সংরচনা stsentry *sts, FILE *fp)
{
  সংরচনা tm stamp;
  যদি(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    যদি(!result)
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
CURLcode Curl_hsts_save(সংরচনা Curl_easy *data, সংরচনা hsts *h,
                        ধ্রুৱ ৱর্ণ *file)
{
  সংরচনা Curl_llist_গ্ৰন্থি *e;
  সংরচনা Curl_llist_গ্ৰন্থি *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  ৱর্ণ *tempstore = রিক্ত;

  যদি(!h)
    /* no cache activated */
    ঘূৰাই CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  যদি(!file && h->filename)
    file = h->filename;

  যদি((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    গচ্ছ skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  যদি(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    বাবে(e = Curl_llist_head(&h->list); e; e = n) {
      সংরচনা stsentry *sts = Curl_গ্ৰন্থি_elem(e);
      n = Curl_গ্ৰন্থি_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    যদি(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    যদি(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  যদি(data->set.hsts_write) {
    /* if there is a write callback */
    সংরচনা curl_index i; /*গুনতি*/
    i.total = Curl_llist_গুনতি(&h->list);
    i.index = 0;
    বাবে(e = Curl_llist_head(&h->list); e; e = n) {
      সংরচনা stsentry *sts = Curl_গ্ৰন্থি_elem(e);
      bool stop;
      n = Curl_গ্ৰন্থি_next(e);
      result = hsts_ঠেলো(data, &i, sts, &stop);
      যদি(result || stop)
        ভঙ্গ;
      i.index++;
    }
  }
  ঘূৰাই result;
}

/* only returns SERIOUS errors */
স্থির CURLcode hsts_add_host_expire(সংরচনা hsts *h,
                                     ধ্রুৱ ৱর্ণ *host, size_t hostlen,
                                     ধ্রুৱ ৱর্ণ *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  সংরচনা stsentry *e;
  ৱর্ণ dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(রিক্ত);

  /* The date parser works on a null-terminated string. */
  যদি(explen > MAX_HSTS_DATELEN)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  যদি(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  অন্যথা
    Curl_getdate_capped(dbuf, &expires);

  যদি(expires <= now)
    /* this entry already expired */
    ঘূৰাই CURLE_OK;

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
      result = hsts_create(h, host, hostlen, subdomain, expires);
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
  ঘূৰাই result;
}

/* only returns SERIOUS errors */
স্থির CURLcode hsts_add(সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *সাৰি)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  সংরচনা Curl_str host;
  সংরচনা Curl_str date;

  যদি(curlx_str_word(&সাৰি, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&সাৰি) ||
     curlx_str_quotedword(&সাৰি, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&সাৰি))
    ;
  অন্যথা {
    ঘূৰাই hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ঘূৰাই CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
স্থির CURLcode hsts_pull(সংরচনা Curl_easy *data, সংরচনা hsts *h)
{
  /* if the HSTS read callback is set, use it */
  যদি(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    কৰা {
      ৱর্ণ buffer[MAX_HSTS_HOSTLEN + 1];
      সংরচনা curl_hstsentry e;
      e.name = buffer;
      e.namelen = পরিমাণ(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      যদি(sc == CURLSTS_OK) {
        CURLcode result;
        ধ্রুৱ ৱর্ণ *date = e.expire;
        যদি(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
        যদি(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        যদি(result)
          ঘূৰাই result;
      }
      অন্যথা যদি(sc == CURLSTS_FAIL)
        ঘূৰাই CURLE_ABORTED_BY_CALLBACK;
    } যেতিয়ালৈ(sc == CURLSTS_OK);
  }
  ঘূৰাই CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
স্থির CURLcode hsts_load(সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  যদি(!h->filename)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  যদি(fp) {
    curlx_struct_stat stat;
    যদি((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      সংরচনা dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_সাৰি);
      কৰা {
        result = Curl_get_সাৰি(&buf, fp, &eof);
        যদি(!result) {
          ধ্রুৱ ৱর্ণ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          যদি((*lineptr == '#') || strlen(lineptr) <= 1)
            অনুৱর্ত;

          hsts_add(h, lineptr);
        }
      } যেতিয়ালৈ(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ঘূৰাই result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(সংরচনা Curl_easy *data,
                            সংরচনা hsts *h, ধ্রুৱ ৱর্ণ *file)
{
  DEBUGASSERT(h);
  (শূন্য)data;
  ঘূৰাই hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(সংরচনা Curl_easy *data, সংরচনা hsts *h)
{
  যদি(h)
    ঘূৰাই hsts_pull(data, h);
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  সংরচনা curl_slist *l = data->state.hstslist;
  যদি(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    যেতিয়ালৈ(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      যদি(result)
        ভঙ্গ;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  ঘূৰাই result;
}

bool Curl_hsts_applies(সংরচনা hsts *h, ধ্রুৱ সংরচনা Curl_peer *dest)
{
  ঘূৰাই !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
