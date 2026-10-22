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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "hsts.h"
#അന്തര്ഭാവ "curl_fopen.h"
#അന്തര്ഭാവ "curl_get_line.h"
#അന്തര്ഭാവ "parsedate.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

#define MAX_HSTS_വരി    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#എങ്കിൽ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
സ്ഥിതം time_t hsts_debugtime(ശൂന്യം *unused)
{
  സ്ഥിരം അക്ഷരം *timestr = getenv("CURL_TIME");
  (ശൂന്യം)unused;
  എങ്കിൽ(timestr) {
    curl_off_t val;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    തിരികെ (time_t)val;
  }
  തിരികെ time(ശൂന്യ);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

ഘടന hsts *Curl_hsts_init(ശൂന്യം)
{
  ഘടന hsts *h = curlx_calloc(1, വലുപ്പം(ഘടന hsts));
  എങ്കിൽ(h) {
    Curl_llist_init(&h->list, ശൂന്യ);
  }
  തിരികെ h;
}

#define hsts_free(x) curlx_free(x)

ശൂന്യം Curl_hsts_cleanup(ഘടന hsts **hp)
{
  ഘടന hsts *h = *hp;
  എങ്കിൽ(h) {
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_llist_കണ്ണി *n;
    വേണ്ടി(e = Curl_llist_head(&h->list); e; e = n) {
      ഘടന stsentry *sts = Curl_കണ്ണി_elem(e);
      n = Curl_കണ്ണി_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ശൂന്യ;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
സ്ഥിതം ശൂന്യം hsts_append(ഘടന hsts *h, ഘടന stsentry *sts)
{
  എങ്കിൽ(Curl_llist_എണ്ണം(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ഘടന Curl_llist_കണ്ണി *e = Curl_llist_head(&h->list);
    ഘടന stsentry *oldsts = Curl_കണ്ണി_elem(e);
    Curl_കണ്ണി_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->കണ്ണി);
}

സ്ഥിതം CURLcode hsts_create(ഘടന hsts *h,
                            സ്ഥിരം അക്ഷരം *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  എങ്കിൽ(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  എങ്കിൽ(hlen) {
    ഘടന stsentry *sts = curlx_calloc(1, വലുപ്പം(ഘടന stsentry) + hlen);
    എങ്കിൽ(!sts)
      തിരികെ CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  തിരികെ CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ഘടന hsts *dst, ഘടന hsts *src)
{
  ഘടന Curl_llist_കണ്ണി *e;
  time_t now = time(ശൂന്യ);
  വേണ്ടി(e = Curl_llist_head(&src->list); e; e = Curl_കണ്ണി_next(e)) {
    ഘടന stsentry *sts = Curl_കണ്ണി_elem(e);
    എങ്കിൽ(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }
  തിരികെ CURLE_OK;
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
UNITTEST ഘടന stsentry *hsts_check(ഘടന hsts *h, സ്ഥിരം അക്ഷരം *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ഘടന stsentry *hsts_check(ഘടന hsts *h, സ്ഥിരം അക്ഷരം *hostname,
                                     size_t hlen, bool subdomain)
{
  ഘടന stsentry *bestsub = ശൂന്യ;
  എങ്കിൽ(h) {
    time_t now = time(ശൂന്യ);
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_llist_കണ്ണി *n;
    size_t blen = 0;

    എങ്കിൽ((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      തിരികെ ശൂന്യ;
    എങ്കിൽ(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    വേണ്ടി(e = Curl_llist_head(&h->list); e; e = n) {
      ഘടന stsentry *sts = Curl_കണ്ണി_elem(e);
      size_t ntail;
      n = Curl_കണ്ണി_next(e);
      എങ്കിൽ(sts->expires <= now) {
        /* remove expired entries */
        Curl_കണ്ണി_remove(&sts->കണ്ണി);
        hsts_free(sts);
        തുടരുക;
      }
      ntail = strlen(sts->host);
      എങ്കിൽ((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        എങ്കിൽ((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      എങ്കിൽ((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        തിരികെ sts;
    }
  }
  തിരികെ bestsub;
}

CURLcode Curl_hsts_parse(ഘടന hsts *h, സ്ഥിരം അക്ഷരം *hostname,
                         സ്ഥിരം അക്ഷരം *header)
{
  സ്ഥിരം അക്ഷരം *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ഘടന stsentry *sts;
  time_t now = time(ശൂന്യ);
  size_t hlen = strlen(hostname);

  എങ്കിൽ(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    തിരികെ CURLE_OK;

  ചെയ്യുക {
    ഘടന Curl_str word;
    ഘടന Curl_str val = { 0 };
    പൂർണ്ണസംഖ്യ rc;
    bool assign = FALSE;

    ചെയ്യുക {
      curlx_str_passblanks(&p);
      എങ്കിൽ(*p == ';')
        p++;
      അല്ലെങ്കിൽ
        നിർത്തുക;
    } ആകുമ്പോൾ(1);
    എങ്കിൽ(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      നിർത്തുക;

    curlx_str_passblanks(&p);
    എങ്കിൽ(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      എങ്കിൽ(*p == '\"') {
        എങ്കിൽ(curlx_str_quotedword(&p, &val, MAX_HSTS_വരി))
          നിർത്തുക;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(curlx_str_cspn(&p, &val, ", ;\r\n"))
          നിർത്തുക;
      }
    }

    എങ്കിൽ(assign && curlx_str_casecompare(&word, "max-age")) {
      സ്ഥിരം അക്ഷരം *vp = curlx_str(&val);
      എങ്കിൽ(gotma)
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_സംഖ്യ(&vp, &expires, CAP_HSTS_MAX_AGE);
      എങ്കിൽ(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      അല്ലെങ്കിൽ എങ്കിൽ(rc)
        /* invalid max-age */
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&word, "includesubdomains")) {
      എങ്കിൽ(gotinc)
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } ആകുമ്പോൾ(*p);

  എങ്കിൽ(!gotma)
    /* max-age is mandatory */
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    എങ്കിൽ(sts) {
      Curl_കണ്ണി_remove(&sts->കണ്ണി);
      hsts_free(sts);
    }
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  അല്ലെങ്കിൽ
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  എങ്കിൽ(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  അല്ലെങ്കിൽ
    തിരികെ hsts_create(h, hostname, hlen, subdomains, expires);

  തിരികെ CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
സ്ഥിതം CURLcode hsts_തള്ളുക(ഘടന Curl_easy *data,
                          ഘടന curl_index *i,
                          ഘടന stsentry *sts,
                          bool *stop)
{
  ഘടന curl_hstsentry e;
  CURLSTScode sc;
  ഘടന tm stamp;
  CURLcode result;

  e.name = (അക്ഷരം *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  എങ്കിൽ(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    എങ്കിൽ(result)
      തിരികെ result;

    curl_msnprintf(e.expire, വലുപ്പം(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  അല്ലെങ്കിൽ
    curlx_strcopy(e.expire, വലുപ്പം(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  തിരികെ sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
സ്ഥിതം ശൂന്യം hsts_out(ഘടന stsentry *sts, FILE *fp)
{
  ഘടന tm stamp;
  എങ്കിൽ(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    എങ്കിൽ(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  അല്ലെങ്കിൽ
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ഘടന Curl_easy *data, ഘടന hsts *h,
                        സ്ഥിരം അക്ഷരം *file)
{
  ഘടന Curl_llist_കണ്ണി *e;
  ഘടന Curl_llist_കണ്ണി *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  അക്ഷരം *tempstore = ശൂന്യ;

  എങ്കിൽ(!h)
    /* no cache activated */
    തിരികെ CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  എങ്കിൽ(!file && h->filename)
    file = h->filename;

  എങ്കിൽ((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    പോകുക skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  എങ്കിൽ(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    വേണ്ടി(e = Curl_llist_head(&h->list); e; e = n) {
      ഘടന stsentry *sts = Curl_കണ്ണി_elem(e);
      n = Curl_കണ്ണി_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    എങ്കിൽ(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    എങ്കിൽ(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  എങ്കിൽ(data->set.hsts_write) {
    /* if there is a write callback */
    ഘടന curl_index i; /*എണ്ണം*/
    i.total = Curl_llist_എണ്ണം(&h->list);
    i.index = 0;
    വേണ്ടി(e = Curl_llist_head(&h->list); e; e = n) {
      ഘടന stsentry *sts = Curl_കണ്ണി_elem(e);
      bool stop;
      n = Curl_കണ്ണി_next(e);
      result = hsts_തള്ളുക(data, &i, sts, &stop);
      എങ്കിൽ(result || stop)
        നിർത്തുക;
      i.index++;
    }
  }
  തിരികെ result;
}

/* only returns SERIOUS errors */
സ്ഥിതം CURLcode hsts_add_host_expire(ഘടന hsts *h,
                                     സ്ഥിരം അക്ഷരം *host, size_t hostlen,
                                     സ്ഥിരം അക്ഷരം *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  ഘടന stsentry *e;
  അക്ഷരം dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(ശൂന്യ);

  /* The date parser works on a null-terminated string. */
  എങ്കിൽ(explen > MAX_HSTS_DATELEN)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  എങ്കിൽ(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  അല്ലെങ്കിൽ
    Curl_getdate_capped(dbuf, &expires);

  എങ്കിൽ(expires <= now)
    /* this entry already expired */
    തിരികെ CURLE_OK;

  എങ്കിൽ(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  എങ്കിൽ(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  എങ്കിൽ(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    എങ്കിൽ(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    അല്ലെങ്കിൽ എങ്കിൽ((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      എങ്കിൽ(expires > e->expires)
        e->expires = expires;
      എങ്കിൽ(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  തിരികെ result;
}

/* only returns SERIOUS errors */
സ്ഥിതം CURLcode hsts_add(ഘടന hsts *h, സ്ഥിരം അക്ഷരം *വരി)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ഘടന Curl_str host;
  ഘടന Curl_str date;

  എങ്കിൽ(curlx_str_word(&വരി, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_quotedword(&വരി, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&വരി))
    ;
  അല്ലെങ്കിൽ {
    തിരികെ hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  തിരികെ CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
സ്ഥിതം CURLcode hsts_pull(ഘടന Curl_easy *data, ഘടന hsts *h)
{
  /* if the HSTS read callback is set, use it */
  എങ്കിൽ(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    ചെയ്യുക {
      അക്ഷരം buffer[MAX_HSTS_HOSTLEN + 1];
      ഘടന curl_hstsentry e;
      e.name = buffer;
      e.namelen = വലുപ്പം(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      എങ്കിൽ(sc == CURLSTS_OK) {
        CURLcode result;
        സ്ഥിരം അക്ഷരം *date = e.expire;
        എങ്കിൽ(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
        എങ്കിൽ(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        എങ്കിൽ(result)
          തിരികെ result;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(sc == CURLSTS_FAIL)
        തിരികെ CURLE_ABORTED_BY_CALLBACK;
    } ആകുമ്പോൾ(sc == CURLSTS_OK);
  }
  തിരികെ CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
സ്ഥിതം CURLcode hsts_load(ഘടന hsts *h, സ്ഥിരം അക്ഷരം *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  എങ്കിൽ(!h->filename)
    തിരികെ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  എങ്കിൽ(fp) {
    curlx_struct_stat stat;
    എങ്കിൽ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      ഘടന dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_വരി);
      ചെയ്യുക {
        result = Curl_get_വരി(&buf, fp, &eof);
        എങ്കിൽ(!result) {
          സ്ഥിരം അക്ഷരം *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          എങ്കിൽ((*lineptr == '#') || strlen(lineptr) <= 1)
            തുടരുക;

          hsts_add(h, lineptr);
        }
      } ആകുമ്പോൾ(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  തിരികെ result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ഘടന Curl_easy *data,
                            ഘടന hsts *h, സ്ഥിരം അക്ഷരം *file)
{
  DEBUGASSERT(h);
  (ശൂന്യം)data;
  തിരികെ hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ഘടന Curl_easy *data, ഘടന hsts *h)
{
  എങ്കിൽ(h)
    തിരികെ hsts_pull(data, h);
  തിരികെ CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ഘടന curl_slist *l = data->state.hstslist;
  എങ്കിൽ(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    ആകുമ്പോൾ(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      എങ്കിൽ(result)
        നിർത്തുക;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  തിരികെ result;
}

bool Curl_hsts_applies(ഘടന hsts *h, സ്ഥിരം ഘടന Curl_peer *dest)
{
  തിരികെ !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#എങ്കിൽ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
