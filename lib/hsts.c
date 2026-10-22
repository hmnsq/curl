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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#define MAX_HSTS_ಸಾಲು    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#ಯದಿ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ಸ್ಥಿರ time_t hsts_debugtime(ಶೂನ್ಯ *unused)
{
  ಧ್ರುವ ವರ್ಣ *timestr = getenv("CURL_TIME");
  (ಶೂನ್ಯ)unused;
  ಯದಿ(timestr) {
    curl_off_t val;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    ಪ್ರತ್ಯಾಗಮ (time_t)val;
  }
  ಪ್ರತ್ಯಾಗಮ time(ರಿಕ್ತ);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

ಸಂರಚನಾ hsts *Curl_hsts_init(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ hsts *h = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ hsts));
  ಯದಿ(h) {
    Curl_llist_init(&h->list, ರಿಕ್ತ);
  }
  ಪ್ರತ್ಯಾಗಮ h;
}

#define hsts_free(x) curlx_free(x)

ಶೂನ್ಯ Curl_hsts_cleanup(ಸಂರಚನಾ hsts **hp)
{
  ಸಂರಚನಾ hsts *h = *hp;
  ಯದಿ(h) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
    ಪ್ರತಿ(e = Curl_llist_head(&h->list); e; e = n) {
      ಸಂರಚನಾ stsentry *sts = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ರಿಕ್ತ;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
ಸ್ಥಿರ ಶೂನ್ಯ hsts_append(ಸಂರಚನಾ hsts *h, ಸಂರಚನಾ stsentry *sts)
{
  ಯದಿ(Curl_llist_ಎಣಿಕೆ(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e = Curl_llist_head(&h->list);
    ಸಂರಚನಾ stsentry *oldsts = Curl_ಗಂಟು_elem(e);
    Curl_ಗಂಟು_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->ಗಂಟು);
}

ಸ್ಥಿರ CURLcode hsts_create(ಸಂರಚನಾ hsts *h,
                            ಧ್ರುವ ವರ್ಣ *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  ಯದಿ(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  ಯದಿ(hlen) {
    ಸಂರಚನಾ stsentry *sts = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ stsentry) + hlen);
    ಯದಿ(!sts)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ಸಂರಚನಾ hsts *dst, ಸಂರಚನಾ hsts *src)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  time_t now = time(ರಿಕ್ತ);
  ಪ್ರತಿ(e = Curl_llist_head(&src->list); e; e = Curl_ಗಂಟು_next(e)) {
    ಸಂರಚನಾ stsentry *sts = Curl_ಗಂಟು_elem(e);
    ಯದಿ(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
UNITTEST ಸಂರಚನಾ stsentry *hsts_check(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ಸಂರಚನಾ stsentry *hsts_check(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *hostname,
                                     size_t hlen, bool subdomain)
{
  ಸಂರಚನಾ stsentry *bestsub = ರಿಕ್ತ;
  ಯದಿ(h) {
    time_t now = time(ರಿಕ್ತ);
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
    size_t blen = 0;

    ಯದಿ((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    ಪ್ರತಿ(e = Curl_llist_head(&h->list); e; e = n) {
      ಸಂರಚನಾ stsentry *sts = Curl_ಗಂಟು_elem(e);
      size_t ntail;
      n = Curl_ಗಂಟು_next(e);
      ಯದಿ(sts->expires <= now) {
        /* remove expired entries */
        Curl_ಗಂಟು_remove(&sts->ಗಂಟು);
        hsts_free(sts);
        ಅನುವರ್ತ;
      }
      ntail = strlen(sts->host);
      ಯದಿ((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        ಯದಿ((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      ಯದಿ((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        ಪ್ರತ್ಯಾಗಮ sts;
    }
  }
  ಪ್ರತ್ಯಾಗಮ bestsub;
}

CURLcode Curl_hsts_parse(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *hostname,
                         ಧ್ರುವ ವರ್ಣ *header)
{
  ಧ್ರುವ ವರ್ಣ *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ಸಂರಚನಾ stsentry *sts;
  time_t now = time(ರಿಕ್ತ);
  size_t hlen = strlen(hostname);

  ಯದಿ(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಕುರು {
    ಸಂರಚನಾ Curl_str word;
    ಸಂರಚನಾ Curl_str val = { 0 };
    ಪೂರ್ಣಾಙ್ಕ rc;
    bool assign = FALSE;

    ಕುರು {
      curlx_str_passblanks(&p);
      ಯದಿ(*p == ';')
        p++;
      ಅನ್ಯಥಾ
        ಭಙ್ಗ;
    } ಯಾವತ್(1);
    ಯದಿ(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      ಭಙ್ಗ;

    curlx_str_passblanks(&p);
    ಯದಿ(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      ಯದಿ(*p == '\"') {
        ಯದಿ(curlx_str_quotedword(&p, &val, MAX_HSTS_ಸಾಲು))
          ಭಙ್ಗ;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(curlx_str_cspn(&p, &val, ", ;\r\n"))
          ಭಙ್ಗ;
      }
    }

    ಯದಿ(assign && curlx_str_casecompare(&word, "max-age")) {
      ಧ್ರುವ ವರ್ಣ *vp = curlx_str(&val);
      ಯದಿ(gotma)
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_ಸಂಖ್ಯೆ(&vp, &expires, CAP_HSTS_MAX_AGE);
      ಯದಿ(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      ಅನ್ಯಥಾ ಯದಿ(rc)
        /* invalid max-age */
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&word, "includesubdomains")) {
      ಯದಿ(gotinc)
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } ಯಾವತ್(*p);

  ಯದಿ(!gotma)
    /* max-age is mandatory */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    ಯದಿ(sts) {
      Curl_ಗಂಟು_remove(&sts->ಗಂಟು);
      hsts_free(sts);
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  ಅನ್ಯಥಾ
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  ಯದಿ(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ hsts_create(h, hostname, hlen, subdomains, expires);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ಸ್ಥಿರ CURLcode hsts_ತಳ್ಳು(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ curl_index *i,
                          ಸಂರಚನಾ stsentry *sts,
                          bool *stop)
{
  ಸಂರಚನಾ curl_hstsentry e;
  CURLSTScode sc;
  ಸಂರಚನಾ tm stamp;
  CURLcode result;

  e.name = (ವರ್ಣ *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  ಯದಿ(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    curl_msnprintf(e.expire, ಪರಿಮಾಣ(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ಅನ್ಯಥಾ
    curlx_strcopy(e.expire, ಪರಿಮಾಣ(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ಪ್ರತ್ಯಾಗಮ sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ಸ್ಥಿರ ಶೂನ್ಯ hsts_out(ಸಂರಚನಾ stsentry *sts, FILE *fp)
{
  ಸಂರಚನಾ tm stamp;
  ಯದಿ(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    ಯದಿ(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ಅನ್ಯಥಾ
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ hsts *h,
                        ಧ್ರುವ ವರ್ಣ *file)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  ವರ್ಣ *tempstore = ರಿಕ್ತ;

  ಯದಿ(!h)
    /* no cache activated */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  ಯದಿ(!file && h->filename)
    file = h->filename;

  ಯದಿ((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ಗಚ್ಛ skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  ಯದಿ(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    ಪ್ರತಿ(e = Curl_llist_head(&h->list); e; e = n) {
      ಸಂರಚನಾ stsentry *sts = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    ಯದಿ(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    ಯದಿ(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  ಯದಿ(data->set.hsts_write) {
    /* if there is a write callback */
    ಸಂರಚನಾ curl_index i; /*ಎಣಿಕೆ*/
    i.total = Curl_llist_ಎಣಿಕೆ(&h->list);
    i.index = 0;
    ಪ್ರತಿ(e = Curl_llist_head(&h->list); e; e = n) {
      ಸಂರಚನಾ stsentry *sts = Curl_ಗಂಟು_elem(e);
      bool stop;
      n = Curl_ಗಂಟು_next(e);
      result = hsts_ತಳ್ಳು(data, &i, sts, &stop);
      ಯದಿ(result || stop)
        ಭಙ್ಗ;
      i.index++;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* only returns SERIOUS errors */
ಸ್ಥಿರ CURLcode hsts_add_host_expire(ಸಂರಚನಾ hsts *h,
                                     ಧ್ರುವ ವರ್ಣ *host, size_t hostlen,
                                     ಧ್ರುವ ವರ್ಣ *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ stsentry *e;
  ವರ್ಣ dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(ರಿಕ್ತ);

  /* The date parser works on a null-terminated string. */
  ಯದಿ(explen > MAX_HSTS_DATELEN)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  ಯದಿ(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  ಅನ್ಯಥಾ
    Curl_getdate_capped(dbuf, &expires);

  ಯದಿ(expires <= now)
    /* this entry already expired */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  ಯದಿ(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  ಯದಿ(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    ಯದಿ(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    ಅನ್ಯಥಾ ಯದಿ((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      ಯದಿ(expires > e->expires)
        e->expires = expires;
      ಯದಿ(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* only returns SERIOUS errors */
ಸ್ಥಿರ CURLcode hsts_add(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ಸಂರಚನಾ Curl_str host;
  ಸಂರಚನಾ Curl_str date;

  ಯದಿ(curlx_str_word(&ಸಾಲು, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_quotedword(&ಸಾಲು, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&ಸಾಲು))
    ;
  ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ಸ್ಥಿರ CURLcode hsts_pull(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ hsts *h)
{
  /* if the HSTS read callback is set, use it */
  ಯದಿ(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    ಕುರು {
      ವರ್ಣ buffer[MAX_HSTS_HOSTLEN + 1];
      ಸಂರಚನಾ curl_hstsentry e;
      e.name = buffer;
      e.namelen = ಪರಿಮಾಣ(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      ಯದಿ(sc == CURLSTS_OK) {
        CURLcode result;
        ಧ್ರುವ ವರ್ಣ *date = e.expire;
        ಯದಿ(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
        ಯದಿ(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
      ಅನ್ಯಥಾ ಯದಿ(sc == CURLSTS_FAIL)
        ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;
    } ಯಾವತ್(sc == CURLSTS_OK);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ಸ್ಥಿರ CURLcode hsts_load(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  ಯದಿ(!h->filename)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ಯದಿ(fp) {
    curlx_struct_stat stat;
    ಯದಿ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      ಸಂರಚನಾ dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_ಸಾಲು);
      ಕುರು {
        result = Curl_get_ಸಾಲು(&buf, fp, &eof);
        ಯದಿ(!result) {
          ಧ್ರುವ ವರ್ಣ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          ಯದಿ((*lineptr == '#') || strlen(lineptr) <= 1)
            ಅನುವರ್ತ;

          hsts_add(h, lineptr);
        }
      } ಯಾವತ್(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ hsts *h, ಧ್ರುವ ವರ್ಣ *file)
{
  DEBUGASSERT(h);
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ hsts *h)
{
  ಯದಿ(h)
    ಪ್ರತ್ಯಾಗಮ hsts_pull(data, h);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ curl_slist *l = data->state.hstslist;
  ಯದಿ(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    ಯಾವತ್(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      ಯದಿ(result)
        ಭಙ್ಗ;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_hsts_applies(ಸಂರಚನಾ hsts *h, ಧ್ರುವ ಸಂರಚನಾ Curl_peer *dest)
{
  ಪ್ರತ್ಯಾಗಮ !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#ಯದಿ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
