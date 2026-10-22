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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_get_line.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HSTS_ਸਤਰ    4095
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HSTS_HOSTLEN 2048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HSTS_DATELEN 17
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNLIMITED        "unlimited"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#ਜੇ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ਸਥਿਰ time_t hsts_debugtime(ਖਾਲੀ *ਅਣਵਰਤਿਆ)
{
  ਅਚਲ ਅੱਖਰ *timestr = getenv("CURL_TIME");
  (ਖਾਲੀ)ਅਣਵਰਤਿਆ;
  ਜੇ(timestr) {
    curl_off_t val;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&timestr, &val, ਸਮਾਂ_T_MAX))
      val += (curl_off_t)deltatime;
    ਵਾਪਸ (time_t)val;
  }
  ਵਾਪਸ ਸਮਾਂ(ਨਿਰਾ);
}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸਮਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮਾਂ(x) hsts_debugtime(x)
#ਜੇ_ਅੰਤ

ਬਣਤਰ hsts *Curl_hsts_init(ਖਾਲੀ)
{
  ਬਣਤਰ hsts *h = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ hsts));
  ਜੇ(h) {
    Curl_llist_init(&h->list, ਨਿਰਾ);
  }
  ਵਾਪਸ h;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hsts_free(x) curlx_free(x)

ਖਾਲੀ Curl_hsts_cleanup(ਬਣਤਰ hsts **hp)
{
  ਬਣਤਰ hsts *h = *hp;
  ਜੇ(h) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;
    ਲਈ(e = Curl_llist_ਸਿਰ(&h->list); e; e = n) {
      ਬਣਤਰ stsentry *sts = Curl_ਗੰਢ_elem(e);
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ਨਿਰਾ;
  }
}

/* append the new entry to the list after possibly removing an old entry
ਪਹਿਲਾ*/
ਸਥਿਰ ਖਾਲੀ hsts_append(ਬਣਤਰ hsts *h, ਬਣਤਰ stsentry *sts)
{
  ਜੇ(Curl_llist_ਗਿਣਤੀ(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ਬਣਤਰ Curl_llist_ਗੰਢ *e = Curl_llist_ਸਿਰ(&h->list);
    ਬਣਤਰ stsentry *oldsts = Curl_ਗੰਢ_elem(e);
    Curl_ਗੰਢ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->ਗੰਢ);
}

ਸਥਿਰ CURLcode hsts_create(ਬਣਤਰ hsts *h,
                            ਅਚਲ ਅੱਖਰ *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  ਜੇ(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  ਜੇ(hlen) {
    ਬਣਤਰ stsentry *sts = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ stsentry) + hlen);
    ਜੇ(!sts)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    /* the null-terminator is already there */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ਵਾਪਸ CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ਬਣਤਰ hsts *dst, ਬਣਤਰ hsts *src)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  time_t now = ਸਮਾਂ(ਨਿਰਾ);
  ਲਈ(e = Curl_llist_ਸਿਰ(&src->list); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
    ਬਣਤਰ stsentry *sts = Curl_ਗੰਢ_elem(e);
    ਜੇ(sts->expires > now) {
      CURLcode ਨਤੀਜਾ = hsts_create(dst, sts->host, ਲੜੀ_ਲੰਬਾਈ(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਵਾਪਸ CURLE_OK;
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
UNITTEST ਬਣਤਰ stsentry *hsts_check(ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ਬਣਤਰ stsentry *hsts_check(ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *hostname,
                                     size_t hlen, bool subdomain)
{
  ਬਣਤਰ stsentry *bestsub = ਨਿਰਾ;
  ਜੇ(h) {
    time_t now = ਸਮਾਂ(ਨਿਰਾ);
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;
    size_t blen = 0;

    ਜੇ((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ਵਾਪਸ ਨਿਰਾ;
    ਜੇ(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    ਲਈ(e = Curl_llist_ਸਿਰ(&h->list); e; e = n) {
      ਬਣਤਰ stsentry *sts = Curl_ਗੰਢ_elem(e);
      size_t ntail;
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      ਜੇ(sts->expires <= now) {
        /* remove expired entries */
        Curl_ਗੰਢ_remove(&sts->ਗੰਢ);
        hsts_free(sts);
        ਜਾਰੀ;
      }
      ntail = ਲੜੀ_ਲੰਬਾਈ(sts->host);
      ਜੇ((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        ਜੇ((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      ਜੇ((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        ਵਾਪਸ sts;
    }
  }
  ਵਾਪਸ bestsub;
}

CURLcode Curl_hsts_parse(ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *hostname,
                         ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  ਅਚਲ ਅੱਖਰ *p = ਸਿਰਲੇਖ;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ਬਣਤਰ stsentry *sts;
  time_t now = ਸਮਾਂ(ਨਿਰਾ);
  size_t hlen = ਲੜੀ_ਲੰਬਾਈ(hostname);

  ਜੇ(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ਵਾਪਸ CURLE_OK;

  ਕਰੋ {
    ਬਣਤਰ Curl_str ਸ਼ਬਦ;
    ਬਣਤਰ Curl_str val = { 0 };
    ਪੂਰਨ rc;
    bool ਸੌਂਪੋ = FALSE;

    ਕਰੋ {
      curlx_str_passblanks(&p);
      ਜੇ(*p == ';')
        p++;
      ਨਹੀਂਤਾਂ
        ਰੋਕੋ;
    } ਜਦੋਂ(1);
    ਜੇ(curlx_str_cspn(&p, &ਸ਼ਬਦ, ";=\r\n \t"))
      ਰੋਕੋ;

    curlx_str_passblanks(&p);
    ਜੇ(!curlx_str_single(&p, '=')) {
      ਸੌਂਪੋ = TRUE;
      curlx_str_passblanks(&p);

      ਜੇ(*p == '\"') {
        ਜੇ(curlx_str_quotedword(&p, &val, MAX_HSTS_ਸਤਰ))
          ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(curlx_str_cspn(&p, &val, ", ;\r\n"))
          ਰੋਕੋ;
      }
    }

    ਜੇ(ਸੌਂਪੋ && curlx_str_casecompare(&ਸ਼ਬਦ, "max-age")) {
      ਅਚਲ ਅੱਖਰ *vp = curlx_str(&val);
      ਜੇ(gotma)
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      rc = curlx_str_ਸੰਖਿਆ(&vp, &expires, CAP_HSTS_MAX_AGE);
      ਜੇ(rc == STRE_ਉਛਾਲ)
        expires = CAP_HSTS_MAX_AGE;
      ਨਹੀਂਤਾਂ ਜੇ(rc)
        /* invalid max-age */
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

      gotma = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਸ਼ਬਦ, "includesubdomains")) {
      ਜੇ(gotinc)
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } ਜਦੋਂ(*p);

  ਜੇ(!gotma)
    /* max-age is mandatory */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    ਜੇ(sts) {
      Curl_ਗੰਢ_remove(&sts->ਗੰਢ);
      hsts_free(sts);
    }
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  ਨਹੀਂਤਾਂ
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  ਜੇ(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ hsts_create(h, hostname, hlen, subdomains, expires);

  ਵਾਪਸ CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ਸਥਿਰ CURLcode hsts_ਧੱਕੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ curl_ਸੂਚਕਾਂਕ *i,
                          ਬਣਤਰ stsentry *sts,
                          bool *stop)
{
  ਬਣਤਰ curl_hstsentry e;
  CURLSTScode sc;
  ਬਣਤਰ tm stamp;
  CURLcode ਨਤੀਜਾ;

  e.name = (ਅੱਖਰ *)sts->host;
  e.namelen = ਲੜੀ_ਲੰਬਾਈ(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  ਜੇ(sts->expires != ਸਮਾਂ_T_MAX) {
    ਨਤੀਜਾ = curlx_gmtime((time_t)sts->expires, &stamp);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    curl_msnprintf(e.expire, ਆਕਾਰ(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ਨਹੀਂਤਾਂ
    curlx_strcopy(e.expire, ਆਕਾਰ(e.expire), STRCONST(UNLIMITED));

  sc = ਅੰਕੜੇ->ਸੈੱਟ.hsts_write(ਅੰਕੜੇ, &e, i, ਅੰਕੜੇ->ਸੈੱਟ.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ਵਾਪਸ sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ਦਲੀਲ : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ਸਥਿਰ ਖਾਲੀ hsts_ਬਾਹਰਲਾ(ਬਣਤਰ stsentry *sts, FILE *fp)
{
  ਬਣਤਰ tm stamp;
  ਜੇ(sts->expires != ਸਮਾਂ_T_MAX) {
    CURLcode ਨਤੀਜਾ = curlx_gmtime((time_t)sts->expires, &stamp);
    ਜੇ(!ਨਤੀਜਾ)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ਨਹੀਂਤਾਂ
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ hsts *h,
                        ਅਚਲ ਅੱਖਰ *file)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  FILE *ਬਾਹਰਲਾ;
  ਅੱਖਰ *tempstore = ਨਿਰਾ;

  ਜੇ(!h)
    /* no cache activated */
    ਵਾਪਸ CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  ਜੇ(!file && h->filename)
    file = h->filename;

  ਜੇ((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ਜਾਓ skipsave;

  ਨਤੀਜਾ = Curl_fopen(ਅੰਕੜੇ, file, &ਬਾਹਰਲਾ, &tempstore);
  ਜੇ(!ਨਤੀਜਾ) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          ਬਾਹਰਲਾ);
    ਲਈ(e = Curl_llist_ਸਿਰ(&h->list); e; e = n) {
      ਬਣਤਰ stsentry *sts = Curl_ਗੰਢ_elem(e);
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      hsts_ਬਾਹਰਲਾ(sts, ਬਾਹਰਲਾ);
    }
    curlx_fclose(ਬਾਹਰਲਾ);
    ਜੇ(!ਨਤੀਜਾ && tempstore && curlx_rename(tempstore, file))
      ਨਤੀਜਾ = CURLE_WRITE_ERROR;

    ਜੇ(ਨਤੀਜਾ && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.hsts_write) {
    /* if there is a write callback */
    ਬਣਤਰ curl_ਸੂਚਕਾਂਕ i; /*ਗਿਣਤੀ*/
    i.ਕੁੱਲ = Curl_llist_ਗਿਣਤੀ(&h->list);
    i.ਸੂਚਕਾਂਕ = 0;
    ਲਈ(e = Curl_llist_ਸਿਰ(&h->list); e; e = n) {
      ਬਣਤਰ stsentry *sts = Curl_ਗੰਢ_elem(e);
      bool stop;
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      ਨਤੀਜਾ = hsts_ਧੱਕੋ(ਅੰਕੜੇ, &i, sts, &stop);
      ਜੇ(ਨਤੀਜਾ || stop)
        ਰੋਕੋ;
      i.ਸੂਚਕਾਂਕ++;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* only returns SERIOUS errors */
ਸਥਿਰ CURLcode hsts_ਜੋੜੋ_host_expire(ਬਣਤਰ hsts *h,
                                     ਅਚਲ ਅੱਖਰ *host, size_t hostlen,
                                     ਅਚਲ ਅੱਖਰ *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ stsentry *e;
  ਅੱਖਰ dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = ਸਮਾਂ(ਨਿਰਾ);

  /* The date parser works on a null-terminated string. */
  ਜੇ(explen > MAX_HSTS_DATELEN)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(dbuf, expire, explen);
  dbuf[explen] = 0;

  ਜੇ(!strcmp(dbuf, UNLIMITED))
    expires = ਸਮਾਂ_T_MAX;
  ਨਹੀਂਤਾਂ
    Curl_getdate_capped(dbuf, &expires);

  ਜੇ(expires <= now)
    /* this entry already expired */
    ਵਾਪਸ CURLE_OK;

  ਜੇ(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  ਜੇ(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  ਜੇ(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    ਜੇ(!e)
      ਨਤੀਜਾ = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    ਨਹੀਂਤਾਂ ਜੇ((hostlen == ਲੜੀ_ਲੰਬਾਈ(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      ਜੇ(expires > e->expires)
        e->expires = expires;
      ਜੇ(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* only returns SERIOUS errors */
ਸਥਿਰ CURLcode hsts_ਜੋੜੋ(ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ਬਣਤਰ Curl_str host;
  ਬਣਤਰ Curl_str date;

  ਜੇ(curlx_str_ਸ਼ਬਦ(&ਸਤਰ, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_quotedword(&ਸਤਰ, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&ਸਤਰ))
    ;
  ਨਹੀਂਤਾਂ {
    ਵਾਪਸ hsts_ਜੋੜੋ_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ਸਥਿਰ CURLcode hsts_pull(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ hsts *h)
{
  /* if the HSTS read callback is set, use it */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    ਕਰੋ {
      ਅੱਖਰ ਬਫਰ[MAX_HSTS_HOSTLEN + 1];
      ਬਣਤਰ curl_hstsentry e;
      e.name = ਬਫਰ;
      e.namelen = ਆਕਾਰ(ਬਫਰ) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = ਅੰਕੜੇ->ਸੈੱਟ.hsts_read(ਅੰਕੜੇ, &e, ਅੰਕੜੇ->ਸੈੱਟ.hsts_read_userp);
      ਜੇ(sc == CURLSTS_OK) {
        CURLcode ਨਤੀਜਾ;
        ਅਚਲ ਅੱਖਰ *date = e.expire;
        ਜੇ(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜੇ(!date[0])
          date = UNLIMITED;
        ਨਤੀਜਾ = hsts_ਜੋੜੋ_host_expire(h, e.name, ਲੜੀ_ਲੰਬਾਈ(e.name),
                                      date, ਲੜੀ_ਲੰਬਾਈ(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(sc == CURLSTS_FAIL)
        ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;
    } ਜਦੋਂ(sc == CURLSTS_OK);
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ਸਥਿਰ CURLcode hsts_ਲੱਦੋ(ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *file)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  ਜੇ(!h->filename)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ਜੇ(fp) {
    curlx_struct_stat stat;
    ਜੇ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_ਬਹੁਲਕ)) {
      ਬਣਤਰ dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_ਸਤਰ);
      ਕਰੋ {
        ਨਤੀਜਾ = Curl_get_ਸਤਰ(&buf, fp, &eof);
        ਜੇ(!ਨਤੀਜਾ) {
          ਅਚਲ ਅੱਖਰ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          ਜੇ((*lineptr == '#') || ਲੜੀ_ਲੰਬਾਈ(lineptr) <= 1)
            ਜਾਰੀ;

          hsts_ਜੋੜੋ(h, lineptr);
        }
      } ਜਦੋਂ(!ਨਤੀਜਾ && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ hsts *h, ਅਚਲ ਅੱਖਰ *file)
{
  DEBUGASSERT(h);
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ hsts_ਲੱਦੋ(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ hsts *h)
{
  ਜੇ(h)
    ਵਾਪਸ hsts_pull(ਅੰਕੜੇ, h);
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ curl_slist *l = ਅੰਕੜੇ->state.hstslist;
  ਜੇ(l) {
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_HSTS, CURL_ਤਾਲਾ_ACCESS_SINGLE);

    ਜਦੋਂ(l) {
      ਨਤੀਜਾ = Curl_hsts_loadfile(ਅੰਕੜੇ, ਅੰਕੜੇ->hsts, l->ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
      l = l->ਅਗਲਾ;
    }
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_HSTS);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_hsts_applies(ਬਣਤਰ hsts *h, ਅਚਲ ਬਣਤਰ Curl_peer *dest)
{
  ਵਾਪਸ !!hsts_check(h, dest->hostname, ਲੜੀ_ਲੰਬਾਈ(dest->hostname), TRUE);
}

#ਜੇ defined(DEBUGBUILD) || defined(UNITTESTS)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸਮਾਂ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
