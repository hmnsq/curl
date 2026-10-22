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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cookie.h"
#అన్తర్భావ "psl.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "slist.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "curl_fopen.h"
#అన్తర్భావ "curl_get_line.h"
#అన్తర్భావ "curl_memrchr.h"
#అన్తర్భావ "parsedate.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "llist.h"
#అన్తర్భావ "curlx/strparse.h"

/* number of seconds in 400 days */
#నిర్వచించు COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
స్థిర శూన్య cap_expires(time_t now, నిర్మాణం Cookie *co)
{
  ఒకవేళ(co->expires && (సమయం_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    ఒకవేళ(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

స్థిర శూన్య freecookie(నిర్మాణం Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->మార్గం);
  curlx_free(co->name);
  curlx_free(co->విలువ);
  ఒకవేళ(maintoo)
    curlx_free(co);
}

స్థిర bool cookie_tailmatch(స్థిరరాశి అక్షరం *cookie_domain,
                             స్థిరరాశి size_t cookie_domain_len,
                             స్థిరరాశి అక్షరం *hostname)
{
  size_t hostname_len = అక్షరశ్రేణి_పొడవు(hostname);

  ఒకవేళ(hostname_len < cookie_domain_len)
    తిరిగి FALSE;

  ఒకవేళ(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    తిరిగి FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  ఒకవేళ(hostname_len == cookie_domain_len)
    తిరిగి TRUE;
  ఒకవేళ('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    తిరిగి TRUE;
  తిరిగి FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
స్థిర bool pathmatch(స్థిరరాశి అక్షరం *cookie_మార్గం, స్థిరరాశి అక్షరం *uri_మార్గం)
{
  size_t cookie_మార్గం_len;
  size_t uri_మార్గం_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_మార్గం_len = అక్షరశ్రేణి_పొడవు(cookie_మార్గం);
  ఒకవేళ(cookie_మార్గం_len == 1) {
    /* cookie_path must be '/' */
    తిరిగి TRUE;
  }

  /* #-fragments are already cut off! */
  ఒకవేళ(అక్షరశ్రేణి_పొడవు(uri_మార్గం) == 0 || uri_మార్గం[0] != '/')
    uri_మార్గం = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_మార్గం_len = అక్షరశ్రేణి_పొడవు(uri_మార్గం);

  ఒకవేళ(uri_మార్గం_len < cookie_మార్గం_len)
    వెళ్ళు pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  ఒకవేళ(అక్షరశ్రేణి_పోలిక(cookie_మార్గం, uri_మార్గం, cookie_మార్గం_len))
    వెళ్ళు pathmatched;

  /* The cookie-path and the uri-path are identical. */
  ఒకవేళ(cookie_మార్గం_len == uri_మార్గం_len) {
    ret = TRUE;
    వెళ్ళు pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  ఒకవేళ(uri_మార్గం[cookie_మార్గం_len] == '/') {
    ret = TRUE;
    వెళ్ళు pathmatched;
  }

pathmatched:
  తిరిగి ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
స్థిర స్థిరరాశి అక్షరం *get_శిఖరం_domain(స్థిరరాశి అక్షరం * స్థిరరాశి domain, size_t *outlen)
{
  size_t len = 0;
  స్థిరరాశి అక్షరం *మొదటి = ఖాళీ, *చివరి;

  ఒకవేళ(domain) {
    len = అక్షరశ్రేణి_పొడవు(domain);
    చివరి = memrchr(domain, '.', len);
    ఒకవేళ(చివరి) {
      మొదటి = memrchr(domain, '.', (చివరి - domain));
      ఒకవేళ(మొదటి)
        len -= (++మొదటి - domain);
    }
  }

  ఒకవేళ(outlen)
    *outlen = len;

  తిరిగి మొదటి ? మొదటి : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#ఒకవేళ defined(_MSC_VER) && (_MSC_VER == 1900)
#సూచన optimize("", off)
#అయితే_ముగింపు

/*
 * A case-insensitive hash for the cookie domains.
 */
స్థిర size_t cookie_hash_domain(స్థిరరాశి అక్షరం *domain, స్థిరరాశి size_t len)
{
  స్థిరరాశి అక్షరం *చివర = domain + len;
  size_t h = 5381;

  అయితే(domain < చివర) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  తిరిగి (h % COOKIE_HASH_కొలత);
}

#ఒకవేళ defined(_MSC_VER) && (_MSC_VER == 1900)
#సూచన optimize("", on)
#అయితే_ముగింపు

/*
 * Hash this domain.
 */
స్థిర size_t cookiehash(స్థిరరాశి అక్షరం * స్థిరరాశి domain)
{
  స్థిరరాశి అక్షరం *శిఖరం;
  size_t len;

  ఒకవేళ(!domain || Curl_host_is_ipnum(domain))
    తిరిగి 0;

  శిఖరం = get_శిఖరం_domain(domain, &len);
  తిరిగి cookie_hash_domain(శిఖరం, len);
}

/*
 * cookie path sanitize
 */
స్థిర అక్షరం *sanitize_cookie_మార్గం(స్థిరరాశి అక్షరం *cookie_మార్గం, size_t len)
{
  /* some sites send path attribute within '"'. */
  ఒకవేళ(len && (cookie_మార్గం[0] == '\"')) {
    cookie_మార్గం++;
    len--;

    ఒకవేళ(len && (cookie_మార్గం[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  ఒకవేళ(!len || (cookie_మార్గం[0] != '/'))
    /* Let cookie-path be the default-path. */
    తిరిగి curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  ఒకవేళ(len > 1 && cookie_మార్గం[len - 1] == '/')
    len--;

  తిరిగి curlx_memdup0(cookie_మార్గం, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
స్థిర CURLcode strstore(అక్షరం **str, స్థిరరాశి అక్షరం *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  ఒకవేళ(!*str)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి CURLE_OK;
}

/*
 * remove_expired
 *
 * Remove expired cookies from the hash by inspecting the expires timestamp on
 * each cookie in the hash, freeing and deleting any where the timestamp is in
 * the past. If the cookiejar has recorded the next timestamp at which one or
 * more cookies expire, then processing will exit early in case this timestamp
 * is in the future.
 */
స్థిర శూన్య remove_expired(నిర్మాణం CookieInfo *ci)
{
  నిర్మాణం Cookie *co;
  curl_off_t now = (curl_off_t)సమయం(ఖాళీ);
  గుర్తులేని పూర్ణాంకం i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  ఒకవేళ(now < ci->తదుపరి_expiration &&
     ci->తదుపరి_expiration != CURL_OFF_T_MAX)
    తిరిగి;
  లేకపోతే
    ci->తదుపరి_expiration = CURL_OFF_T_MAX;

  కొరకు(i = 0; i < COOKIE_HASH_కొలత; i++) {
    నిర్మాణం Curl_llist_కణుపు *n;
    నిర్మాణం Curl_llist_కణుపు *e = ఖాళీ;

    కొరకు(n = Curl_llist_తల(&ci->cookielist[i]); n; n = e) {
      co = Curl_కణుపు_elem(n);
      e = Curl_కణుపు_తదుపరి(n);
      ఒకవేళ(co->expires) {
        ఒకవేళ(co->expires < now) {
          Curl_కణుపు_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        లేకపోతే ఒకవేళ(co->expires < ci->తదుపరి_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->తదుపరి_expiration = co->expires;
      }
    }
  }
}

#నిర్వచితం_కాకపోతే USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
స్థిర bool bad_domain(స్థిరరాశి అక్షరం *domain, size_t len)
{
  ఒకవేళ((len == 9) && curl_strnequal(domain, "localhost", 9))
    తిరిగి FALSE;
  లేకపోతే {
    /* there must be a dot present, but that dot must not be a trailing dot */
    స్థిరరాశి అక్షరం *dot = memchr(domain, '.', len);
    ఒకవేళ(dot) {
      size_t i = dot - domain;
      ఒకవేళ((len - i) > 1)
        /* the dot is not the last byte */
        తిరిగి FALSE;
    }
  }
  తిరిగి TRUE;
}
#అయితే_ముగింపు

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
స్థిర bool invalid_octets(స్థిరరాశి అక్షరం *ptr, size_t len)
{
  స్థిరరాశి గుర్తులేని అక్షరం *p = (స్థిరరాశి గుర్తులేని అక్షరం *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  అయితే(len && *p) {
    ఒకవేళ((*p < 0x20) || (*p == 0x7f))
      తిరిగి TRUE;
    p++;
    len--;
  }
  తిరిగి FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#నిర్వచించు MAX_DATE_నిడివి 80

#నిర్వచించు COOKIE_NAME   0
#నిర్వచించు COOKIE_విలువ  1
#నిర్వచించు COOKIE_DOMAIN 2
#నిర్వచించు COOKIE_మార్గం   3

#నిర్వచించు COOKIE_PIECES 4 /* the list above */

స్థిర CURLcode storecookie(నిర్మాణం Cookie *co, స్థిరరాశి నిర్మాణం Curl_str *cp,
                            స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *domain)
{
  CURLcode ఫలితం;
  ఫలితం = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  ఒకవేళ(!ఫలితం)
    ఫలితం = strstore(&co->విలువ, curlx_str(&cp[COOKIE_విలువ]),
                      curlx_strlen(&cp[COOKIE_విలువ]));
  ఒకవేళ(!ఫలితం) {
    size_t plen = 0;
    ఒకవేళ(curlx_strlen(&cp[COOKIE_మార్గం])) {
      మార్గం = curlx_str(&cp[COOKIE_మార్గం]);
      plen = curlx_strlen(&cp[COOKIE_మార్గం]);
    }
    లేకపోతే ఒకవేళ(మార్గం) {
      /* No path was given in the header line, set the default */
      స్థిరరాశి అక్షరం *endslash = strrchr(మార్గం, '/');
      ఒకవేళ(endslash)
        plen = endslash - మార్గం + 1; /* include end slash */
      లేకపోతే
        plen = అక్షరశ్రేణి_పొడవు(మార్గం);
    }

    ఒకవేళ(మార్గం) {
      co->మార్గం = sanitize_cookie_మార్గం(మార్గం, plen);
      ఒకవేళ(!co->మార్గం)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(curlx_strlen(&cp[COOKIE_DOMAIN]))
      ఫలితం = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    లేకపోతే ఒకవేళ(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      ఒకవేళ(!co->domain)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  తిరిగి ఫలితం;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
స్థిర bool parse_మొదటి_pair(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Cookie *co,
                             నిర్మాణం Curl_str *cookie,
                             నిర్మాణం Curl_str *name,
                             నిర్మాణం Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  ఒకవేళ(!sep || !curlx_strlen(name)) {
    infof(దత్తాంశం, "invalid cookie, dropped");
    తిరిగి FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  ఒకవేళ((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(దత్తాంశం, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    తిరిగి FALSE;
  }

  /* Check if we have a reserved prefix set. */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక("__Secure-", curlx_str(name), 9))
    co->ఉపసర్గ_secure = TRUE;
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక("__Host-", curlx_str(name), 7))
    co->ఉపసర్గ_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_విలువ] = *val;
  తిరిగి TRUE;
}

స్థిర bool parse_domain(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Cookie *co,
                         నిర్మాణం Curl_str *cookie_domain,
                         నిర్మాణం Curl_str *val,
                         స్థిరరాశి అక్షరం **domainp)
{
  bool is_ip;
  స్థిరరాశి అక్షరం *domain = *domainp;
  స్థిరరాశి అక్షరం *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  ఒకవేళ('.' == *v)
    curlx_str_nudge(val, 1);

#నిర్వచితం_కాకపోతే USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  ఒకవేళ(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#అయితే_ముగింపు

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  ఒకవేళ(!domain ||
     (is_ip &&
      !అక్షరశ్రేణి_పోలిక(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == అక్షరశ్రేణి_పొడవు(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    ఒకవేళ(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  లేకపోతే {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(దత్తాంశం, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర శూన్య parse_maxage(నిర్మాణం Cookie *co, నిర్మాణం Curl_str *val,
                         time_t *nowp)
{
  పూర్ణాంకం rc;
  స్థిరరాశి అక్షరం *maxage = curlx_str(val);
  ఒకవేళ(*maxage == '\"')
    maxage++;
  rc = curlx_str_సంఖ్య(&maxage, &co->expires, CURL_OFF_T_MAX);
  ఒకవేళ(!*nowp)
    *nowp = సమయం(ఖాళీ);
  ఎంపిక(rc) {
  స్థితి STRE_అధికప్రవాహం:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    ఆపు;
  అప్రమేయ:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    ఆపు;
  స్థితి STRE_OK:
    ఒకవేళ(!co->expires)
      co->expires = 1; /* expire now */
    లేకపోతే ఒకవేళ(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    లేకపోతే
      co->expires += *nowp;
    ఆపు;
  }
  cap_expires(*nowp, co);
}

స్థిర శూన్య parse_expires(నిర్మాణం Cookie *co, నిర్మాణం Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  ఒకవేళ(!co->expires && (curlx_strlen(val) < MAX_DATE_నిడివి)) {
    అక్షరం dbuf[MAX_DATE_నిడివి + 1];
    time_t date = 0;
    జ్ఞాపకం_నకలు(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    ఒకవేళ(!Curl_getdate_capped(dbuf, &date)) {
      ఒకవేళ(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    లేకపోతే
      co->expires = 0;
    ఒకవేళ(!*nowp)
      *nowp = సమయం(ఖాళీ);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
స్థిర CURLcode
parse_cookie_శీర్షిక(నిర్మాణం Curl_easy *దత్తాంశం,
                    నిర్మాణం Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    స్థిరరాశి అక్షరం *ptr, /* the header */
                    స్థిరరాశి అక్షరం *domain, /* default domain */
                    /* full path used when this cookie is set */
                    స్థిరరాశి అక్షరం *మార్గం,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = అక్షరశ్రేణి_పొడవు(ptr);
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  ఒకవేళ(linelength > MAX_COOKIE_పంక్తి)
    /* discard overly long lines at once */
    తిరిగి CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, పరిమాణం(cookie));
  చేయి {
    నిర్మాణం Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    పూర్ణాంకం rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    ఒకవేళ(!rc || (rc == STRE_SHORT)) {
      నిర్మాణం Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      ఒకవేళ(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(దత్తాంశం, "invalid octets in name, cookie dropped");
        తిరిగి CURLE_OK;
      }

      ఒకవేళ(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        ఒకవేళ(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        ఒకవేళ(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(దత్తాంశం, "invalid octets in value, cookie dropped");
          తిరిగి CURLE_OK;
        }
      }
      లేకపోతే
        curlx_str_init(&val);

      ఒకవేళ(!curlx_strlen(&cookie[COOKIE_NAME])) {
        ఒకవేళ(!parse_మొదటి_pair(దత్తాంశం, co, cookie, &name, &val, sep))
          తిరిగి CURLE_OK;
      }

      /* whether there's separator or not */
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "secure")) {
        ఒకవేళ(secure_origin)
          co->secure = TRUE;
        లేకపోతే {
          infof(దత్తాంశం, "skipped cookie because not 'secure'");
          తిరిగి CURLE_OK;
        }
      }
      లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      లేకపోతే ఒకవేళ(sep) {
        /* only if a separating equals sign is present */
        ఒకవేళ(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_మార్గం] = val;
        లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "domain")) {
          ఒకవేళ(curlx_strlen(&val) &&
             !parse_domain(దత్తాంశం, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            తిరిగి CURLE_OK;
        }
        లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        లేకపోతే ఒకవేళ(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } అయితే(!curlx_str_single(&ptr, ';'));

  ఒకవేళ(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    ఫలితం = storecookie(co, &cookie[0], మార్గం, domain);
    ఒకవేళ(!ఫలితం)
      *okay = TRUE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode parse_netscape(నిర్మాణం Cookie *co,
                               స్థిరరాశి నిర్మాణం CookieInfo *ci,
                               bool *okay,
                               స్థిరరాశి అక్షరం *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  స్థిరరాశి అక్షరం *ptr, *తదుపరి;
  పూర్ణాంకం fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  ఒకవేళ(lineptr[0] == '#')
    /* do not even try the comments */
    తిరిగి CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  కొరకు(తదుపరి = lineptr; తదుపరి; fields++) {
    ptr = తదుపరి;
    len = strcspn(ptr, "\t\r\n");
    తదుపరి = (ptr[len] == '\t' ? &ptr[len + 1] : ఖాళీ);
    ఎంపిక(fields) {
    స్థితి 0:
      ఒకవేళ(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      ఒకవేళ(!co->domain)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఆపు;
    స్థితి 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      ఆపు;
    స్థితి 2:
      /* The file format allows the path field to remain not filled in */
      ఒకవేళ(అక్షరశ్రేణి_పోలిక("TRUE", ptr, len) && అక్షరశ్రేణి_పోలిక("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->మార్గం = sanitize_cookie_మార్గం(ptr, len);
        ఒకవేళ(!co->మార్గం)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        ఆపు;
      }
      లేకపోతే {
        /* this does not look like a path, make one up! */
        co->మార్గం = curlx_strdup("/");
        ఒకవేళ(!co->మార్గం)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    స్థితి 3:
      co->secure = FALSE;
      ఒకవేళ(curl_strnequal(ptr, "TRUE", len)) {
        ఒకవేళ(secure_origin || ci->running)
          co->secure = TRUE;
        లేకపోతే
          తిరిగి CURLE_OK;
      }
      ఆపు;
    స్థితి 4:
      ఒకవేళ(curlx_str_సంఖ్య(&ptr, &co->expires, CURL_OFF_T_MAX))
        తిరిగి CURLE_OK;
      ఆపు;
    స్థితి 5:
      co->name = curlx_memdup0(ptr, len);
      ఒకవేళ(!co->name)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        ఒకవేళ(!అక్షరశ్రేణి_పోలిక("__Secure-", co->name, 9))
          co->ఉపసర్గ_secure = TRUE;
        లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక("__Host-", co->name, 7))
          co->ఉపసర్గ_host = TRUE;
      }
      ఆపు;
    స్థితి 6:
      co->విలువ = curlx_memdup0(ptr, len);
      ఒకవేళ(!co->విలువ)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఆపు;
    }
  }
  ఒకవేళ(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->విలువ = curlx_strdup("");
    ఒకవేళ(!co->విలువ)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే
      fields++;
  }

  ఒకవేళ(fields != 7)
    /* we did not find the sufficient number of fields */
    తిరిగి CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  ఒకవేళ(invalid_octets(co->name, అక్షరశ్రేణి_పొడవు(co->name)) ||
     invalid_octets(co->విలువ, అక్షరశ్రేణి_పొడవు(co->విలువ)))
    తిరిగి CURLE_OK;

  *okay = TRUE;
  తిరిగి CURLE_OK;
}

స్థిర bool is_ప్రజా_suffix(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Cookie *co,
                             స్థిరరాశి అక్షరం *domain)
{
#నిర్వచితమైతే USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(co);
  DEBUGF(infof(దత్తాంశం, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  ఒకవేళ(!co->domain || Curl_host_is_ipnum(co->domain))
    తిరిగి FALSE;

  లేకపోతే {
    bool acceptable = FALSE;
    అక్షరం lcase[256];
    అక్షరం lcookie[256];
    size_t dlen = domain ? అక్షరశ్రేణి_పొడవు(domain) : 0;
    size_t clen = అక్షరశ్రేణి_పొడవు(co->domain);

    /* trim trailing dots */
    ఒకవేళ(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    ఒకవేళ(clen && (co->domain[clen - 1] == '.'))
      clen--;

    ఒకవేళ((dlen < పరిమాణం(lcase)) && (clen < పరిమాణం(lcookie))) {
      స్థిరరాశి psl_ctx_t *psl = Curl_psl_use(దత్తాంశం);
      ఒకవేళ(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        ఒకవేళ(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          ఒకవేళ(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_ప్రజా_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        లేకపోతే {
          bool suffix = psl_is_ప్రజా_suffix(psl, lcookie);
          acceptable = TRUE;
          ఒకవేళ(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(దత్తాంశం);
      }
      లేకపోతే
        infof(దత్తాంశం, "libpsl problem, rejecting cookie for safety");
    }

    ఒకవేళ(!acceptable) {
      infof(దత్తాంశం, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      తిరిగి TRUE;
    }
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)co;
  (శూన్య)domain;
  DEBUGF(infof(దత్తాంశం, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#అయితే_ముగింపు
  తిరిగి FALSE;
}

/* returns TRUE when replaced */
స్థిర bool replace_existing(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Cookie *co,
                             స్థిరరాశి నిర్మాణం CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  నిర్మాణం Curl_llist_కణుపు *replace_n = ఖాళీ;
  నిర్మాణం Curl_llist_కణుపు *n;
  size_t myhash = cookiehash(co->domain);
  కొరకు(n = Curl_llist_తల(&ci->cookielist[myhash]); n; n = Curl_కణుపు_తదుపరి(n)) {
    నిర్మాణం Cookie *clist = Curl_కణుపు_elem(n);
    ఒకవేళ(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      ఒకవేళ(clist->domain && co->domain) {
        ఒకవేళ(cookie_tailmatch(clist->domain, అక్షరశ్రేణి_పొడవు(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, అక్షరశ్రేణి_పొడవు(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      లేకపోతే ఒకవేళ(!clist->domain && !co->domain)
        matching_domains = TRUE;

      ఒకవేళ(matching_domains && /* the domains were identical */
         clist->మార్గం && co->మార్గం && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        స్థిరరాశి అక్షరం *sep = ఖాళీ;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->మార్గం[0]);
        ఒకవేళ(clist->మార్గం[0])
          sep = strchr(clist->మార్గం + 1, '/');
        ఒకవేళ(sep)
          cllen = sep - clist->మార్గం;
        లేకపోతే
          cllen = అక్షరశ్రేణి_పొడవు(clist->మార్గం);

        ఒకవేళ(!అక్షరశ్రేణి_పోలిక(clist->మార్గం, co->మార్గం, cllen)) {
          infof(దత్తాంశం, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          తిరిగి FALSE;
        }
      }
    }

    ఒకవేళ(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      ఒకవేళ(clist->domain && co->domain) {
        ఒకవేళ(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      లేకపోతే ఒకవేళ(!clist->domain && !co->domain)
        replace_old = TRUE;

      ఒకవేళ(replace_old) {
        /* the domains were identical */

        ఒకవేళ(clist->మార్గం && co->మార్గం &&
           strcmp(clist->మార్గం, co->మార్గం))
          replace_old = FALSE;
        లేకపోతే ఒకవేళ(!clist->మార్గం != !co->మార్గం)
          replace_old = FALSE;
      }

      ఒకవేళ(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        తిరిగి FALSE;
      }
      ఒకవేళ(replace_old)
        replace_n = n;
    }
  }
  ఒకవేళ(replace_n) {
    నిర్మాణం Cookie *repl = Curl_కణుపు_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_కణుపు_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  తిరిగి TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం CookieInfo *ci,
                         స్థిరరాశి అక్షరం *lineptr, /* first character of the line */
                         స్థిరరాశి అక్షరం *domain,  /* default domain */
                         స్థిరరాశి అక్షరం *మార్గం,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         స్థిరరాశి పూర్ణాంకం flags)
{
  నిర్మాణం Cookie comem;
  నిర్మాణం Cookie *co;
  size_t myhash;
  CURLcode ఫలితం;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(MAX_సమితి_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  ఒకవేళ(దత్తాంశం->req.setcookies >= MAX_సమితి_COOKIE_AMOUNT)
    తిరిగి CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, పరిమాణం(comem));

  ఒకవేళ(flags & COOKIE_HTTPHEADER)
    ఫలితం = parse_cookie_శీర్షిక(దత్తాంశం, co, &okay,
                                 lineptr, domain, మార్గం, flags & COOKIE_SECURE);
  లేకపోతే
    ఫలితం = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  ఒకవేళ(ఫలితం || !okay)
    వెళ్ళు fail;

  ఒకవేళ(co->ఉపసర్గ_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    వెళ్ళు fail;

  ఒకవేళ(!(flags & COOKIE_NOPSL) && is_ప్రజా_suffix(దత్తాంశం, co, domain))
    వెళ్ళు fail;

  ఒకవేళ(co->ఉపసర్గ_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    ఒకవేళ(co->secure && co->మార్గం && !strcmp(co->మార్గం, "/") && !co->tailmatch)
      ;
    లేకపోతే
      వెళ్ళు fail;
  }

  ఒకవేళ(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    వెళ్ళు fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  ఒకవేళ(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  ఒకవేళ(!replace_existing(దత్తాంశం, co, ci, flags & COOKIE_SECURE, &replaces))
    వెళ్ళు fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, పరిమాణం(comem));
  ఒకవేళ(!co) {
    co = &comem;
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->కణుపు);

  ఒకవేళ(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(దత్తాంశం, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->విలువ,
          co->domain, co->మార్గం, co->expires);

  ఒకవేళ(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  ఒకవేళ(co->expires && (co->expires < ci->తదుపరి_expiration))
    ci->తదుపరి_expiration = co->expires;

  ఒకవేళ(flags & COOKIE_HTTPHEADER)
    దత్తాంశం->req.setcookies++;

  తిరిగి ఫలితం;
fail:
  freecookie(co, FALSE);
  తిరిగి ఫలితం;
}

/*
 * Curl_cookie_init()
 *
 * Inits a cookie struct to read data from a local file. This is always
 * called before any cookies are set. File may be NULL in which case only the
 * struct is initialized. Is file is "-" then STDIN is read.
 *
 * If 'newsession' is TRUE, discard all "session cookies" on read from file.
 *
 * Note that 'data' might be called as NULL pointer. If data is NULL, 'file'
 * will be ignored.
 *
 * Returns NULL on out of memory.
 */
నిర్మాణం CookieInfo *Curl_cookie_init(శూన్య)
{
  పూర్ణాంకం i;
  నిర్మాణం CookieInfo *ci = curlx_calloc(1, పరిమాణం(నిర్మాణం CookieInfo));
  ఒకవేళ(!ci)
    తిరిగి ఖాళీ;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  కొరకు(i = 0; i < COOKIE_HASH_కొలత; i++)
    Curl_llist_init(&ci->cookielist[i], ఖాళీ);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->తదుపరి_expiration = CURL_OFF_T_MAX;

  తిరిగి ci;
}

/*
 * cookie_load()
 *
 * Reads cookies from a local file. This is always called before any cookies
 * are set. If file is "-" then STDIN is read.
 *
 * If 'flags' has the COOKIE_NOSESSION bit set, discard all "session cookies"
 * read from file.
 */
స్థిర CURLcode cookie_ఎక్కించు(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *file,
                            నిర్మాణం CookieInfo *ci, పూర్ణాంకం flags)
{
  FILE *handle = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  FILE *fp = ఖాళీ;
  DEBUGASSERT(ci);
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  ఒకవేళ(file && *file) {
    ఒకవేళ(!strcmp(file, "-"))
      fp = stdin;
    లేకపోతే {
      fp = curlx_fopen(file, "rb");
      ఒకవేళ(!fp)
        infof(దత్తాంశం, "WARNING: failed to open cookie file \"%s\"", file);
      లేకపోతే {
        curlx_struct_stat stat;
        ఒకవేళ((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_బహుళకం)) {
          curlx_fclose(fp);
          fp = ఖాళీ;
          infof(దత్తాంశం, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        లేకపోతే
          handle = fp;
      }
    }
  }

  ఒకవేళ(fp) {
    నిర్మాణం dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    చేయి {
      ఫలితం = Curl_get_పంక్తి(&buf, fp, &eof);
      ఒకవేళ(!ఫలితం) {
        స్థిరరాశి అక్షరం *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        ఒకవేళ(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        ఫలితం = Curl_cookie_కూడు(దత్తాంశం, ci, lineptr, ఖాళీ, ఖాళీ,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        ఒకవేళ(ఫలితం != CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
          ఫలితం = CURLE_OK;
      }
    } అయితే(!ఫలితం && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    ఒకవేళ(handle)
      curlx_fclose(handle);
  }
  దత్తాంశం->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  తిరిగి ఫలితం;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(నిర్మాణం Curl_easy *దత్తాంశం,
                               పూర్ణాంకం flags)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం curl_slist *list = దత్తాంశం->state.cookielist;
  ఒకవేళ(list) {
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
    ఒకవేళ(!దత్తాంశం->cookies)
      దత్తాంశం->cookies = Curl_cookie_init();
    ఒకవేళ(!దత్తాంశం->cookies)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే {
      దత్తాంశం->state.cookie_engine = TRUE;
      అయితే(list) {
        ఫలితం = cookie_ఎక్కించు(దత్తాంశం, list->దత్తాంశం, దత్తాంశం->cookies, flags);
        ఒకవేళ(ఫలితం)
          ఆపు;
        list = list->తదుపరి;
      }
    }
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
  }
  తిరిగి ఫలితం;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
స్థిర పూర్ణాంకం cookie_క్రమీకరించు(స్థిరరాశి శూన్య *p1, స్థిరరాశి శూన్య *p2)
{
  స్థిరరాశి నిర్మాణం Cookie *c1 = *(స్థిరరాశి నిర్మాణం Cookie * స్థిరరాశి *)p1;
  స్థిరరాశి నిర్మాణం Cookie *c2 = *(స్థిరరాశి నిర్మాణం Cookie * స్థిరరాశి *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->మార్గం ? అక్షరశ్రేణి_పొడవు(c1->మార్గం) : 0;
  l2 = c2->మార్గం ? అక్షరశ్రేణి_పొడవు(c2->మార్గం) : 0;

  ఒకవేళ(l1 != l2)
    తిరిగి (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? అక్షరశ్రేణి_పొడవు(c1->domain) : 0;
  l2 = c2->domain ? అక్షరశ్రేణి_పొడవు(c2->domain) : 0;

  ఒకవేళ(l1 != l2)
    తిరిగి (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? అక్షరశ్రేణి_పొడవు(c1->name) : 0;
  l2 = c2->name ? అక్షరశ్రేణి_పొడవు(c2->name) : 0;

  ఒకవేళ(l1 != l2)
    తిరిగి (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  తిరిగి (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
స్థిర పూర్ణాంకం cookie_క్రమీకరించు_ct(స్థిరరాశి శూన్య *p1, స్థిరరాశి శూన్య *p2)
{
  స్థిరరాశి నిర్మాణం Cookie *c1 = *(స్థిరరాశి నిర్మాణం Cookie * స్థిరరాశి *)p1;
  స్థిరరాశి నిర్మాణం Cookie *c2 = *(స్థిరరాశి నిర్మాణం Cookie * స్థిరరాశి *)p2;

  తిరిగి (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *host)
{
  తిరిగి Curl_xfer_is_secure(దత్తాంశం) ||
    curl_strequal("localhost", host) ||
    !strcmp(host, "127.0.0.1") ||
    !strcmp(host, "::1");
}

/*
 * Curl_cookie_getlist
 *
 * For a given host and path, return a linked list of cookies that the client
 * should send to the server if used now.
 *
 * It shall only return cookies that have not expired.
 *
 * 'okay' is TRUE when there is a list returned.
 */
CURLcode Curl_cookie_getlist(నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *okay,
                             స్థిరరాశి అక్షరం *host,
                             నిర్మాణం Curl_llist *list)
{
  size_t matches = 0;
  స్థిరరాశి bool is_ip = Curl_host_is_ipnum(host);
  స్థిరరాశి size_t myhash = cookiehash(host);
  నిర్మాణం Curl_llist_కణుపు *n;
  స్థిరరాశి bool secure = Curl_secure_context(దత్తాంశం, host);
  నిర్మాణం CookieInfo *ci = దత్తాంశం->cookies;
  స్థిరరాశి అక్షరం *మార్గం = దత్తాంశం->state.పైకి.మార్గం;
  CURLcode ఫలితం = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ఖాళీ);

  ఒకవేళ(!ci || !Curl_llist_లెక్క(&ci->cookielist[myhash]))
    తిరిగి CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  కొరకు(n = Curl_llist_తల(&ci->cookielist[myhash]); n; n = Curl_కణుపు_తదుపరి(n)) {
    నిర్మాణం Cookie *co = Curl_కణుపు_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    ఒకవేళ(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      ఒకవేళ(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, అక్షరశ్రేణి_పొడవు(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        ఒకవేళ(!co->మార్గం || pathmatch(co->మార్గం, మార్గం)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          ఒకవేళ(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(దత్తాంశం, "Included max number of cookies (%zu) in request!",
                  matches);
            ఆపు;
          }
        }
      }
    }
  }

  ఒకవేళ(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    నిర్మాణం Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(పరిమాణం(నిర్మాణం Cookie *) * matches);
    ఒకవేళ(!array) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు fail;
    }

    n = Curl_llist_తల(list);

    కొరకు(i = 0; n; n = Curl_కణుపు_తదుపరి(n))
      array[i++] = Curl_కణుపు_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, పరిమాణం(నిర్మాణం Cookie *), cookie_క్రమీకరించు);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ఖాళీ);

    కొరకు(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  తిరిగి CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ఖాళీ);
  తిరిగి ఫలితం; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
శూన్య Curl_cookie_clearall(నిర్మాణం CookieInfo *ci)
{
  ఒకవేళ(ci) {
    గుర్తులేని పూర్ణాంకం i;
    కొరకు(i = 0; i < COOKIE_HASH_కొలత; i++) {
      నిర్మాణం Curl_llist_కణుపు *n;
      కొరకు(n = Curl_llist_తల(&ci->cookielist[i]); n;) {
        నిర్మాణం Cookie *c = Curl_కణుపు_elem(n);
        నిర్మాణం Curl_llist_కణుపు *e = Curl_కణుపు_తదుపరి(n);
        Curl_కణుపు_remove(n);
        freecookie(c, TRUE);
        n = e;
      }
    }
    ci->numcookies = 0;
  }
}

/*
 * Curl_cookie_clearsess
 *
 * Free all session cookies in the cookies list.
 */
శూన్య Curl_cookie_clearsess(నిర్మాణం CookieInfo *ci)
{
  గుర్తులేని పూర్ణాంకం i;

  ఒకవేళ(!ci)
    తిరిగి;

  కొరకు(i = 0; i < COOKIE_HASH_కొలత; i++) {
    నిర్మాణం Curl_llist_కణుపు *n = Curl_llist_తల(&ci->cookielist[i]);
    నిర్మాణం Curl_llist_కణుపు *e = ఖాళీ;

    కొరకు(; n; n = e) {
      నిర్మాణం Cookie *curr = Curl_కణుపు_elem(n);
      e = Curl_కణుపు_తదుపరి(n); /* in case the node is removed, get it early */
      ఒకవేళ(!curr->expires) {
        Curl_కణుపు_remove(n);
        freecookie(curr, TRUE);
        ci->numcookies--;
      }
    }
  }
}

/*
 * Curl_cookie_cleanup()
 *
 * Free a "cookie object" previous created with Curl_cookie_init().
 */
శూన్య Curl_cookie_cleanup(నిర్మాణం CookieInfo *ci)
{
  ఒకవేళ(ci) {
    Curl_cookie_clearall(ci);
    curlx_free(ci); /* free the base struct as well */
  }
}

/*
 * get_netscape_format()
 *
 * Formats a string for Netscape output file, w/o a newline at the end.
 * Function returns a char * to a formatted line. The caller is responsible
 * for freeing the returned pointer.
 */
స్థిర అక్షరం *get_netscape_format(స్థిరరాశి నిర్మాణం Cookie *co)
{
  తిరిగి curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /*మార్గం*/
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*విలువ*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->మార్గం ? co->మార్గం : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->విలువ ? co->విలువ : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
స్థిర CURLcode cookie_output(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం CookieInfo *ci,
                              స్థిరరాశి అక్షరం *filename)
{
  FILE *బహిర్గతం = ఖాళీ;
  bool use_stdout = FALSE;
  అక్షరం *tempstore = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!ci)
    /* no cookie engine alive */
    తిరిగి CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  ఒకవేళ(!strcmp("-", filename)) {
    /* use stdout */
    బహిర్గతం = stdout;
    use_stdout = TRUE;
  }
  లేకపోతే {
    ఫలితం = Curl_fopen(దత్తాంశం, filename, &బహిర్గతం, &tempstore);
    ఒకవేళ(ఫలితం)
      వెళ్ళు error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        బహిర్గతం);

  ఒకవేళ(ci->numcookies) {
    గుర్తులేని పూర్ణాంకం i;
    size_t nvalid = 0;
    నిర్మాణం Cookie **array;
    నిర్మాణం Curl_llist_కణుపు *n;

    array = curlx_calloc(1, పరిమాణం(నిర్మాణం Cookie *) * ci->numcookies);
    ఒకవేళ(!array) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు error;
    }

    /* only sort the cookies with a domain property */
    కొరకు(i = 0; i < COOKIE_HASH_కొలత; i++) {
      కొరకు(n = Curl_llist_తల(&ci->cookielist[i]); n; n = Curl_కణుపు_తదుపరి(n)) {
        నిర్మాణం Cookie *co = Curl_కణుపు_elem(n);
        ఒకవేళ(!co->domain)
          కొనసాగు;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, పరిమాణం(నిర్మాణం Cookie *), cookie_క్రమీకరించు_ct);

    కొరకు(i = 0; i < nvalid; i++) {
      అక్షరం *format_ptr = get_netscape_format(array[i]);
      ఒకవేళ(!format_ptr) {
        curlx_free(array);
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు error;
      }
      curl_mfprintf(బహిర్గతం, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  ఒకవేళ(!use_stdout) {
    curlx_fclose(బహిర్గతం);
    బహిర్గతం = ఖాళీ;
    ఒకవేళ(tempstore && curlx_rename(tempstore, filename)) {
      ఫలితం = CURLE_WRITE_ERROR;
      వెళ్ళు error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  తిరిగి CURLE_OK;

error:
  ఒకవేళ(బహిర్గతం && !use_stdout)
    curlx_fclose(బహిర్గతం);
  ఒకవేళ(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  తిరిగి ఫలితం;
}

స్థిర నిర్మాణం curl_slist *cookie_list(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curl_slist *list = ఖాళీ;
  నిర్మాణం curl_slist *beg;
  నిర్మాణం curl_slist **తోక = &list;
  గుర్తులేని పూర్ణాంకం i;
  నిర్మాణం Curl_llist_కణుపు *n;

  ఒకవేళ(!దత్తాంశం->cookies || (దత్తాంశం->cookies->numcookies == 0))
    తిరిగి ఖాళీ;

  /* at first, remove expired cookies */
  remove_expired(దత్తాంశం->cookies);

  కొరకు(i = 0; i < COOKIE_HASH_కొలత; i++) {
    కొరకు(n = Curl_llist_తల(&దత్తాంశం->cookies->cookielist[i]); n;
        n = Curl_కణుపు_తదుపరి(n)) {
      నిర్మాణం Cookie *c = Curl_కణుపు_elem(n);
      అక్షరం *పంక్తి;
      ఒకవేళ(!c->domain)
        కొనసాగు;
      పంక్తి = get_netscape_format(c);
      ఒకవేళ(!పంక్తి) {
        curl_slist_free_all(list);
        తిరిగి ఖాళీ;
      }
      beg = Curl_slist_append_nodup(ఖాళీ, పంక్తి);
      ఒకవేళ(!beg) {
        curlx_free(పంక్తి);
        curl_slist_free_all(list);
        తిరిగి ఖాళీ;
      }
      *తోక = beg;
      తోక = &beg->తదుపరి;
    }
  }

  తిరిగి list;
}

నిర్మాణం curl_slist *Curl_cookie_list(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curl_slist *list;
  Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
  list = cookie_list(దత్తాంశం);
  Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
  తిరిగి list;
}

శూన్య Curl_flush_cookies(నిర్మాణం Curl_easy *దత్తాంశం, bool cleanup)
{
  Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  ఒకవేళ(దత్తాంశం->cookies) {
    స్థిరరాశి అక్షరం *cookiejar = CURL_EASY_STR(దత్తాంశం, STRING_COOKIEJAR);
    ఒకవేళ(cookiejar && దత్తాంశం->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode ఫలితం = cookie_output(దత్తాంశం, దత్తాంశం->cookies, cookiejar);
      ఒకవేళ(ఫలితం)
        infof(దత్తాంశం, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(ఫలితం));
    }

    ఒకవేళ(cleanup && (!దత్తాంశం->share || (దత్తాంశం->cookies != దత్తాంశం->share->cookies))) {
      Curl_cookie_cleanup(దత్తాంశం->cookies);
      దత్తాంశం->cookies = ఖాళీ;
    }
  }
  Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
}

శూన్య Curl_cookie_నిరంతరం(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
  ఒకవేళ(దత్తాంశం->cookies)
    దత్తాంశం->cookies->running = TRUE;
  Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
}

#అయితే_ముగింపు /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
