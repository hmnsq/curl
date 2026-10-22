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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "hsts.h"
#அந்தர்பாவ "curl_fopen.h"
#அந்தர்பாவ "curl_get_line.h"
#அந்தர்பாவ "parsedate.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/strcopy.h"

#define MAX_HSTS_வரி    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#எனில் defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
நிலையான time_t hsts_debugtime(வெறுமை *unused)
{
  மாறிலி எழுத்து *timestr = getenv("CURL_TIME");
  (வெறுமை)unused;
  எனில்(timestr) {
    curl_off_t val;
    எனில்(!curlx_str_எண்(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    திரும்பு (time_t)val;
  }
  திரும்பு time(வெற்று);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

கட்டமைப்பு hsts *Curl_hsts_init(வெறுமை)
{
  கட்டமைப்பு hsts *h = curlx_calloc(1, அளவு(கட்டமைப்பு hsts));
  எனில்(h) {
    Curl_llist_init(&h->list, வெற்று);
  }
  திரும்பு h;
}

#define hsts_free(x) curlx_free(x)

வெறுமை Curl_hsts_cleanup(கட்டமைப்பு hsts **hp)
{
  கட்டமைப்பு hsts *h = *hp;
  எனில்(h) {
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_llist_முனை *n;
    க்கு(e = Curl_llist_head(&h->list); e; e = n) {
      கட்டமைப்பு stsentry *sts = Curl_முனை_elem(e);
      n = Curl_முனை_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = வெற்று;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
நிலையான வெறுமை hsts_append(கட்டமைப்பு hsts *h, கட்டமைப்பு stsentry *sts)
{
  எனில்(Curl_llist_மொத்தம்(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    கட்டமைப்பு Curl_llist_முனை *e = Curl_llist_head(&h->list);
    கட்டமைப்பு stsentry *oldsts = Curl_முனை_elem(e);
    Curl_முனை_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->முனை);
}

நிலையான CURLcode hsts_create(கட்டமைப்பு hsts *h,
                            மாறிலி எழுத்து *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  எனில்(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  எனில்(hlen) {
    கட்டமைப்பு stsentry *sts = curlx_calloc(1, அளவு(கட்டமைப்பு stsentry) + hlen);
    எனில்(!sts)
      திரும்பு CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  திரும்பு CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(கட்டமைப்பு hsts *dst, கட்டமைப்பு hsts *src)
{
  கட்டமைப்பு Curl_llist_முனை *e;
  time_t now = time(வெற்று);
  க்கு(e = Curl_llist_head(&src->list); e; e = Curl_முனை_next(e)) {
    கட்டமைப்பு stsentry *sts = Curl_முனை_elem(e);
    எனில்(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      எனில்(result)
        திரும்பு result;
    }
  }
  திரும்பு CURLE_OK;
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
UNITTEST கட்டமைப்பு stsentry *hsts_check(கட்டமைப்பு hsts *h, மாறிலி எழுத்து *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST கட்டமைப்பு stsentry *hsts_check(கட்டமைப்பு hsts *h, மாறிலி எழுத்து *hostname,
                                     size_t hlen, bool subdomain)
{
  கட்டமைப்பு stsentry *bestsub = வெற்று;
  எனில்(h) {
    time_t now = time(வெற்று);
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_llist_முனை *n;
    size_t blen = 0;

    எனில்((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      திரும்பு வெற்று;
    எனில்(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    க்கு(e = Curl_llist_head(&h->list); e; e = n) {
      கட்டமைப்பு stsentry *sts = Curl_முனை_elem(e);
      size_t ntail;
      n = Curl_முனை_next(e);
      எனில்(sts->expires <= now) {
        /* remove expired entries */
        Curl_முனை_remove(&sts->முனை);
        hsts_free(sts);
        தொடர்;
      }
      ntail = strlen(sts->host);
      எனில்((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        எனில்((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      எனில்((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        திரும்பு sts;
    }
  }
  திரும்பு bestsub;
}

CURLcode Curl_hsts_parse(கட்டமைப்பு hsts *h, மாறிலி எழுத்து *hostname,
                         மாறிலி எழுத்து *header)
{
  மாறிலி எழுத்து *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  கட்டமைப்பு stsentry *sts;
  time_t now = time(வெற்று);
  size_t hlen = strlen(hostname);

  எனில்(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    திரும்பு CURLE_OK;

  செய் {
    கட்டமைப்பு Curl_str word;
    கட்டமைப்பு Curl_str val = { 0 };
    முழு rc;
    bool assign = FALSE;

    செய் {
      curlx_str_passblanks(&p);
      எனில்(*p == ';')
        p++;
      இல்லையெனில்
        நிறுத்து;
    } போது(1);
    எனில்(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      நிறுத்து;

    curlx_str_passblanks(&p);
    எனில்(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      எனில்(*p == '\"') {
        எனில்(curlx_str_quotedword(&p, &val, MAX_HSTS_வரி))
          நிறுத்து;
      }
      இல்லையெனில் {
        எனில்(curlx_str_cspn(&p, &val, ", ;\r\n"))
          நிறுத்து;
      }
    }

    எனில்(assign && curlx_str_casecompare(&word, "max-age")) {
      மாறிலி எழுத்து *vp = curlx_str(&val);
      எனில்(gotma)
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_எண்(&vp, &expires, CAP_HSTS_MAX_AGE);
      எனில்(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      இல்லையெனில் எனில்(rc)
        /* invalid max-age */
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    இல்லையெனில் எனில்(curlx_str_casecompare(&word, "includesubdomains")) {
      எனில்(gotinc)
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } போது(*p);

  எனில்(!gotma)
    /* max-age is mandatory */
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    எனில்(sts) {
      Curl_முனை_remove(&sts->முனை);
      hsts_free(sts);
    }
    திரும்பு CURLE_OK;
  }

  எனில்(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  இல்லையெனில்
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  எனில்(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  இல்லையெனில்
    திரும்பு hsts_create(h, hostname, hlen, subdomains, expires);

  திரும்பு CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
நிலையான CURLcode hsts_தள்ளு(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு curl_index *i,
                          கட்டமைப்பு stsentry *sts,
                          bool *stop)
{
  கட்டமைப்பு curl_hstsentry e;
  CURLSTScode sc;
  கட்டமைப்பு tm stamp;
  CURLcode result;

  e.name = (எழுத்து *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  எனில்(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    எனில்(result)
      திரும்பு result;

    curl_msnprintf(e.expire, அளவு(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  இல்லையெனில்
    curlx_strcopy(e.expire, அளவு(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  திரும்பு sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
நிலையான வெறுமை hsts_out(கட்டமைப்பு stsentry *sts, FILE *fp)
{
  கட்டமைப்பு tm stamp;
  எனில்(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    எனில்(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  இல்லையெனில்
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு hsts *h,
                        மாறிலி எழுத்து *file)
{
  கட்டமைப்பு Curl_llist_முனை *e;
  கட்டமைப்பு Curl_llist_முனை *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  எழுத்து *tempstore = வெற்று;

  எனில்(!h)
    /* no cache activated */
    திரும்பு CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  எனில்(!file && h->filename)
    file = h->filename;

  எனில்((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    செல் skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  எனில்(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    க்கு(e = Curl_llist_head(&h->list); e; e = n) {
      கட்டமைப்பு stsentry *sts = Curl_முனை_elem(e);
      n = Curl_முனை_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    எனில்(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    எனில்(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  எனில்(data->set.hsts_write) {
    /* if there is a write callback */
    கட்டமைப்பு curl_index i; /*மொத்தம்*/
    i.total = Curl_llist_மொத்தம்(&h->list);
    i.index = 0;
    க்கு(e = Curl_llist_head(&h->list); e; e = n) {
      கட்டமைப்பு stsentry *sts = Curl_முனை_elem(e);
      bool stop;
      n = Curl_முனை_next(e);
      result = hsts_தள்ளு(data, &i, sts, &stop);
      எனில்(result || stop)
        நிறுத்து;
      i.index++;
    }
  }
  திரும்பு result;
}

/* only returns SERIOUS errors */
நிலையான CURLcode hsts_add_host_expire(கட்டமைப்பு hsts *h,
                                     மாறிலி எழுத்து *host, size_t hostlen,
                                     மாறிலி எழுத்து *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு stsentry *e;
  எழுத்து dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(வெற்று);

  /* The date parser works on a null-terminated string. */
  எனில்(explen > MAX_HSTS_DATELEN)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  எனில்(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  இல்லையெனில்
    Curl_getdate_capped(dbuf, &expires);

  எனில்(expires <= now)
    /* this entry already expired */
    திரும்பு CURLE_OK;

  எனில்(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  எனில்(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  எனில்(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    எனில்(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    இல்லையெனில் எனில்((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      எனில்(expires > e->expires)
        e->expires = expires;
      எனில்(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  திரும்பு result;
}

/* only returns SERIOUS errors */
நிலையான CURLcode hsts_add(கட்டமைப்பு hsts *h, மாறிலி எழுத்து *வரி)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  கட்டமைப்பு Curl_str host;
  கட்டமைப்பு Curl_str date;

  எனில்(curlx_str_word(&வரி, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_quotedword(&வரி, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&வரி))
    ;
  இல்லையெனில் {
    திரும்பு hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  திரும்பு CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
நிலையான CURLcode hsts_pull(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு hsts *h)
{
  /* if the HSTS read callback is set, use it */
  எனில்(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    செய் {
      எழுத்து buffer[MAX_HSTS_HOSTLEN + 1];
      கட்டமைப்பு curl_hstsentry e;
      e.name = buffer;
      e.namelen = அளவு(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      எனில்(sc == CURLSTS_OK) {
        CURLcode result;
        மாறிலி எழுத்து *date = e.expire;
        எனில்(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
        எனில்(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        எனில்(result)
          திரும்பு result;
      }
      இல்லையெனில் எனில்(sc == CURLSTS_FAIL)
        திரும்பு CURLE_ABORTED_BY_CALLBACK;
    } போது(sc == CURLSTS_OK);
  }
  திரும்பு CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
நிலையான CURLcode hsts_load(கட்டமைப்பு hsts *h, மாறிலி எழுத்து *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  எனில்(!h->filename)
    திரும்பு CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  எனில்(fp) {
    curlx_struct_stat stat;
    எனில்((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      கட்டமைப்பு dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_வரி);
      செய் {
        result = Curl_get_வரி(&buf, fp, &eof);
        எனில்(!result) {
          மாறிலி எழுத்து *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          எனில்((*lineptr == '#') || strlen(lineptr) <= 1)
            தொடர்;

          hsts_add(h, lineptr);
        }
      } போது(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  திரும்பு result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு hsts *h, மாறிலி எழுத்து *file)
{
  DEBUGASSERT(h);
  (வெறுமை)data;
  திரும்பு hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு hsts *h)
{
  எனில்(h)
    திரும்பு hsts_pull(data, h);
  திரும்பு CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு curl_slist *l = data->state.hstslist;
  எனில்(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    போது(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      எனில்(result)
        நிறுத்து;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  திரும்பு result;
}

bool Curl_hsts_applies(கட்டமைப்பு hsts *h, மாறிலி கட்டமைப்பு Curl_peer *dest)
{
  திரும்பு !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#எனில் defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
