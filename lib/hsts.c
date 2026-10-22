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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HSTS_ಸಾಲು    4095
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HSTS_HOSTLEN 2048
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HSTS_DATELEN 17
#ವ್ಯಾಖ್ಯಾನಿಸು UNLIMITED        "unlimited"

#ವ್ಯಾಖ್ಯಾನಿಸು CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#ಆದರೆ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ಸ್ಥಿರ time_t hsts_debugtime(ಶೂನ್ಯ *ಬಳಸದ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *timestr = getenv("CURL_TIME");
  (ಶೂನ್ಯ)ಬಳಸದ;
  ಆದರೆ(timestr) {
    curl_off_t val;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&timestr, &val, ಸಮಯ_T_MAX))
      val += (curl_off_t)deltatime;
    ಹಿಂತಿರುಗಿ (time_t)val;
  }
  ಹಿಂತಿರುಗಿ ಸಮಯ(ಖಾಲಿ);
}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸಮಯ
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಮಯ(x) hsts_debugtime(x)
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ hsts *Curl_hsts_init(ಶೂನ್ಯ)
{
  ರಚನೆ hsts *h = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ hsts));
  ಆದರೆ(h) {
    Curl_llist_init(&h->list, ಖಾಲಿ);
  }
  ಹಿಂತಿರುಗಿ h;
}

#ವ್ಯಾಖ್ಯಾನಿಸು hsts_free(x) curlx_free(x)

ಶೂನ್ಯ Curl_hsts_cleanup(ರಚನೆ hsts **hp)
{
  ರಚನೆ hsts *h = *hp;
  ಆದರೆ(h) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_llist_ಗಂಟು *n;
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&h->list); e; e = n) {
      ರಚನೆ stsentry *sts = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ಖಾಲಿ;
  }
}

/* append the new entry to the list after possibly removing an old entry
ಮೊದಲ*/
ಸ್ಥಿರ ಶೂನ್ಯ hsts_append(ರಚನೆ hsts *h, ರಚನೆ stsentry *sts)
{
  ಆದರೆ(Curl_llist_ಎಣಿಕೆ(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ರಚನೆ Curl_llist_ಗಂಟು *e = Curl_llist_ತಲೆ(&h->list);
    ರಚನೆ stsentry *oldsts = Curl_ಗಂಟು_elem(e);
    Curl_ಗಂಟು_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->ಗಂಟು);
}

ಸ್ಥಿರ CURLcode hsts_create(ರಚನೆ hsts *h,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  ಆದರೆ(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  ಆದರೆ(hlen) {
    ರಚನೆ stsentry *sts = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ stsentry) + hlen);
    ಆದರೆ(!sts)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    /* the null-terminator is already there */
    ಸ್ಮರಣೆ_ನಕಲು(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ರಚನೆ hsts *dst, ರಚನೆ hsts *src)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  time_t now = ಸಮಯ(ಖಾಲಿ);
  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&src->list); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
    ರಚನೆ stsentry *sts = Curl_ಗಂಟು_elem(e);
    ಆದರೆ(sts->expires > now) {
      CURLcode ಫಲಿತಾಂಶ = hsts_create(dst, sts->host, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
UNITTEST ರಚನೆ stsentry *hsts_check(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ರಚನೆ stsentry *hsts_check(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                                     size_t hlen, bool subdomain)
{
  ರಚನೆ stsentry *bestsub = ಖಾಲಿ;
  ಆದರೆ(h) {
    time_t now = ಸಮಯ(ಖಾಲಿ);
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_llist_ಗಂಟು *n;
    size_t blen = 0;

    ಆದರೆ((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&h->list); e; e = n) {
      ರಚನೆ stsentry *sts = Curl_ಗಂಟು_elem(e);
      size_t ntail;
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      ಆದರೆ(sts->expires <= now) {
        /* remove expired entries */
        Curl_ಗಂಟು_remove(&sts->ಗಂಟು);
        hsts_free(sts);
        ಮುಂದುವರಿ;
      }
      ntail = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sts->host);
      ಆದರೆ((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        ಆದರೆ((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      ಆದರೆ((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        ಹಿಂತಿರುಗಿ sts;
    }
  }
  ಹಿಂತಿರುಗಿ bestsub;
}

CURLcode Curl_hsts_parse(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಶೀರ್ಷಿಕೆ;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ರಚನೆ stsentry *sts;
  time_t now = ಸಮಯ(ಖಾಲಿ);
  size_t hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname);

  ಆದರೆ(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಮಾಡು {
    ರಚನೆ Curl_str ಶಬ್ದ;
    ರಚನೆ Curl_str val = { 0 };
    ಪೂರ್ಣಾಂಕ rc;
    bool ನಿಯೋಜಿಸು = FALSE;

    ಮಾಡು {
      curlx_str_passblanks(&p);
      ಆದರೆ(*p == ';')
        p++;
      ಇಲ್ಲದಿದ್ದರೆ
        ನಿಲ್ಲಿಸು;
    } ಆಗ(1);
    ಆದರೆ(curlx_str_cspn(&p, &ಶಬ್ದ, ";=\r\n \t"))
      ನಿಲ್ಲಿಸು;

    curlx_str_passblanks(&p);
    ಆದರೆ(!curlx_str_single(&p, '=')) {
      ನಿಯೋಜಿಸು = TRUE;
      curlx_str_passblanks(&p);

      ಆದರೆ(*p == '\"') {
        ಆದರೆ(curlx_str_quotedword(&p, &val, MAX_HSTS_ಸಾಲು))
          ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(curlx_str_cspn(&p, &val, ", ;\r\n"))
          ನಿಲ್ಲಿಸು;
      }
    }

    ಆದರೆ(ನಿಯೋಜಿಸು && curlx_str_casecompare(&ಶಬ್ದ, "max-age")) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *vp = curlx_str(&val);
      ಆದರೆ(gotma)
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      rc = curlx_str_ಸಂಖ್ಯೆ(&vp, &expires, CAP_HSTS_MAX_AGE);
      ಆದರೆ(rc == STRE_ಅತಿಪ್ರವಾಹ)
        expires = CAP_HSTS_MAX_AGE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc)
        /* invalid max-age */
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

      gotma = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಶಬ್ದ, "includesubdomains")) {
      ಆದರೆ(gotinc)
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } ಆಗ(*p);

  ಆದರೆ(!gotma)
    /* max-age is mandatory */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    ಆದರೆ(sts) {
      Curl_ಗಂಟು_remove(&sts->ಗಂಟು);
      hsts_free(sts);
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  ಇಲ್ಲದಿದ್ದರೆ
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  ಆದರೆ(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ hsts_create(h, hostname, hlen, subdomains, expires);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ಸ್ಥಿರ CURLcode hsts_ತಳ್ಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ curl_ಸೂಚ್ಯಂಕ *i,
                          ರಚನೆ stsentry *sts,
                          bool *stop)
{
  ರಚನೆ curl_hstsentry e;
  CURLSTScode sc;
  ರಚನೆ tm stamp;
  CURLcode ಫಲಿತಾಂಶ;

  e.name = (ಅಕ್ಷರ *)sts->host;
  e.namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  ಆದರೆ(sts->expires != ಸಮಯ_T_MAX) {
    ಫಲಿತಾಂಶ = curlx_gmtime((time_t)sts->expires, &stamp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    curl_msnprintf(e.expire, ಗಾತ್ರ(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    curlx_strcopy(e.expire, ಗಾತ್ರ(e.expire), STRCONST(UNLIMITED));

  sc = ದತ್ತಾಂಶ->ಗಣ.hsts_write(ದತ್ತಾಂಶ, &e, i, ದತ್ತಾಂಶ->ಗಣ.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ಹಿಂತಿರುಗಿ sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ವಾದ : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ಸ್ಥಿರ ಶೂನ್ಯ hsts_ಹೊರಹೋಗು(ರಚನೆ stsentry *sts, FILE *fp)
{
  ರಚನೆ tm stamp;
  ಆದರೆ(sts->expires != ಸಮಯ_T_MAX) {
    CURLcode ಫಲಿತಾಂಶ = curlx_gmtime((time_t)sts->expires, &stamp);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ hsts *h,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  ರಚನೆ Curl_llist_ಗಂಟು *n;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  FILE *ಹೊರಹೋಗು;
  ಅಕ್ಷರ *tempstore = ಖಾಲಿ;

  ಆದರೆ(!h)
    /* no cache activated */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  ಆದರೆ(!file && h->filename)
    file = h->filename;

  ಆದರೆ((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ಹೋಗು skipsave;

  ಫಲಿತಾಂಶ = Curl_fopen(ದತ್ತಾಂಶ, file, &ಹೊರಹೋಗು, &tempstore);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          ಹೊರಹೋಗು);
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&h->list); e; e = n) {
      ರಚನೆ stsentry *sts = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      hsts_ಹೊರಹೋಗು(sts, ಹೊರಹೋಗು);
    }
    curlx_fclose(ಹೊರಹೋಗು);
    ಆದರೆ(!ಫಲಿತಾಂಶ && tempstore && curlx_rename(tempstore, file))
      ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;

    ಆದರೆ(ಫಲಿತಾಂಶ && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.hsts_write) {
    /* if there is a write callback */
    ರಚನೆ curl_ಸೂಚ್ಯಂಕ i; /*ಎಣಿಕೆ*/
    i.ಒಟ್ಟು = Curl_llist_ಎಣಿಕೆ(&h->list);
    i.ಸೂಚ್ಯಂಕ = 0;
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&h->list); e; e = n) {
      ರಚನೆ stsentry *sts = Curl_ಗಂಟು_elem(e);
      bool stop;
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      ಫಲಿತಾಂಶ = hsts_ತಳ್ಳು(ದತ್ತಾಂಶ, &i, sts, &stop);
      ಆದರೆ(ಫಲಿತಾಂಶ || stop)
        ನಿಲ್ಲಿಸು;
      i.ಸೂಚ್ಯಂಕ++;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* only returns SERIOUS errors */
ಸ್ಥಿರ CURLcode hsts_ಕೂಡಿಸು_host_expire(ರಚನೆ hsts *h,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host, size_t hostlen,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ stsentry *e;
  ಅಕ್ಷರ dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = ಸಮಯ(ಖಾಲಿ);

  /* The date parser works on a null-terminated string. */
  ಆದರೆ(explen > MAX_HSTS_DATELEN)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಸ್ಮರಣೆ_ನಕಲು(dbuf, expire, explen);
  dbuf[explen] = 0;

  ಆದರೆ(!strcmp(dbuf, UNLIMITED))
    expires = ಸಮಯ_T_MAX;
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_getdate_capped(dbuf, &expires);

  ಆದರೆ(expires <= now)
    /* this entry already expired */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  ಆದರೆ(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  ಆದರೆ(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    ಆದರೆ(!e)
      ಫಲಿತಾಂಶ = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((hostlen == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      ಆದರೆ(expires > e->expires)
        e->expires = expires;
      ಆದರೆ(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* only returns SERIOUS errors */
ಸ್ಥಿರ CURLcode hsts_ಕೂಡಿಸು(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ರಚನೆ Curl_str host;
  ರಚನೆ Curl_str date;

  ಆದರೆ(curlx_str_ಶಬ್ದ(&ಸಾಲು, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_quotedword(&ಸಾಲು, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&ಸಾಲು))
    ;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ hsts_ಕೂಡಿಸು_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ಸ್ಥಿರ CURLcode hsts_pull(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ hsts *h)
{
  /* if the HSTS read callback is set, use it */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    ಮಾಡು {
      ಅಕ್ಷರ ಬಫರ್[MAX_HSTS_HOSTLEN + 1];
      ರಚನೆ curl_hstsentry e;
      e.name = ಬಫರ್;
      e.namelen = ಗಾತ್ರ(ಬಫರ್) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = ದತ್ತಾಂಶ->ಗಣ.hsts_read(ದತ್ತಾಂಶ, &e, ದತ್ತಾಂಶ->ಗಣ.hsts_read_userp);
      ಆದರೆ(sc == CURLSTS_OK) {
        CURLcode ಫಲಿತಾಂಶ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date = e.expire;
        ಆದರೆ(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
        ಆದರೆ(!date[0])
          date = UNLIMITED;
        ಫಲಿತಾಂಶ = hsts_ಕೂಡಿಸು_host_expire(h, e.name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(e.name),
                                      date, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sc == CURLSTS_FAIL)
        ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;
    } ಆಗ(sc == CURLSTS_OK);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ಸ್ಥಿರ CURLcode hsts_ತುಂಬು(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  ಆದರೆ(!h->filename)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ಆದರೆ(fp) {
    curlx_struct_stat stat;
    ಆದರೆ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_ಬಹುಲಕ)) {
      ರಚನೆ dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_ಸಾಲು);
      ಮಾಡು {
        ಫಲಿತಾಂಶ = Curl_get_ಸಾಲು(&buf, fp, &eof);
        ಆದರೆ(!ಫಲಿತಾಂಶ) {
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          ಆದರೆ((*lineptr == '#') || ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(lineptr) <= 1)
            ಮುಂದುವರಿ;

          hsts_ಕೂಡಿಸು(h, lineptr);
        }
      } ಆಗ(!ಫಲಿತಾಂಶ && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  DEBUGASSERT(h);
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ hsts_ತುಂಬು(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ hsts *h)
{
  ಆದರೆ(h)
    ಹಿಂತಿರುಗಿ hsts_pull(ದತ್ತಾಂಶ, h);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ curl_slist *l = ದತ್ತಾಂಶ->state.hstslist;
  ಆದರೆ(l) {
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_HSTS, CURL_ಬೀಗ_ACCESS_SINGLE);

    ಆಗ(l) {
      ಫಲಿತಾಂಶ = Curl_hsts_loadfile(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->hsts, l->ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
      l = l->ಮುಂದಿನ;
    }
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_HSTS);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_hsts_applies(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_peer *dest)
{
  ಹಿಂತಿರುಗಿ !!hsts_check(h, dest->hostname, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(dest->hostname), TRUE);
}

#ಆದರೆ defined(DEBUGBUILD) || defined(UNITTESTS)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸಮಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
