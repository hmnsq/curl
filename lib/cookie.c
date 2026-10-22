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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cookie.h"
#অন্তর্ভাব "psl.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "slist.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curl_fopen.h"
#অন্তর্ভাব "curl_get_line.h"
#অন্তর্ভাব "curl_memrchr.h"
#অন্তর্ভাব "parsedate.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "llist.h"
#অন্তর্ভাব "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
স্থির শূন্য cap_expires(time_t now, কাঠামো Cookie *co)
{
  যদি(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    যদি(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

স্থির শূন্য freecookie(কাঠামো Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->মান);
  যদি(maintoo)
    curlx_free(co);
}

স্থির bool cookie_tailmatch(ধ্রুবক অক্ষর *cookie_domain,
                             ধ্রুবক size_t cookie_domain_len,
                             ধ্রুবক অক্ষর *hostname)
{
  size_t hostname_len = strlen(hostname);

  যদি(hostname_len < cookie_domain_len)
    ফেরত FALSE;

  যদি(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    ফেরত FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  যদি(hostname_len == cookie_domain_len)
    ফেরত TRUE;
  যদি('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    ফেরত TRUE;
  ফেরত FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
স্থির bool pathmatch(ধ্রুবক অক্ষর *cookie_path, ধ্রুবক অক্ষর *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  যদি(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    ফেরত TRUE;
  }

  /* #-fragments are already cut off! */
  যদি(strlen(uri_path) == 0 || uri_path[0] != '/')
    uri_path = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_path_len = strlen(uri_path);

  যদি(uri_path_len < cookie_path_len)
    যাও pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  যদি(strncmp(cookie_path, uri_path, cookie_path_len))
    যাও pathmatched;

  /* The cookie-path and the uri-path are identical. */
  যদি(cookie_path_len == uri_path_len) {
    ret = TRUE;
    যাও pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  যদি(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    যাও pathmatched;
  }

pathmatched:
  ফেরত ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
স্থির ধ্রুবক অক্ষর *get_শীর্ষ_domain(ধ্রুবক অক্ষর * ধ্রুবক domain, size_t *outlen)
{
  size_t len = 0;
  ধ্রুবক অক্ষর *first = ফাঁকা, *last;

  যদি(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    যদি(last) {
      first = memrchr(domain, '.', (last - domain));
      যদি(first)
        len -= (++first - domain);
    }
  }

  যদি(outlen)
    *outlen = len;

  ফেরত first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#যদি defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
স্থির size_t cookie_hash_domain(ধ্রুবক অক্ষর *domain, ধ্রুবক size_t len)
{
  ধ্রুবক অক্ষর *end = domain + len;
  size_t h = 5381;

  যখন(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  ফেরত (h % COOKIE_HASH_আকার);
}

#যদি defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
স্থির size_t cookiehash(ধ্রুবক অক্ষর * ধ্রুবক domain)
{
  ধ্রুবক অক্ষর *শীর্ষ_2;
  size_t len;

  যদি(!domain || Curl_host_is_ipnum(domain))
    ফেরত 0;

  শীর্ষ_2 = get_শীর্ষ_domain(domain, &len);
  ফেরত cookie_hash_domain(শীর্ষ_2, len);
}

/*
 * cookie path sanitize
 */
স্থির অক্ষর *sanitize_cookie_path(ধ্রুবক অক্ষর *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  যদি(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    যদি(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  যদি(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    ফেরত curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  যদি(len > 1 && cookie_path[len - 1] == '/')
    len--;

  ফেরত curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
স্থির CURLcode strstore(অক্ষর **str, ধ্রুবক অক্ষর *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  যদি(!*str)
    ফেরত CURLE_OUT_OF_MEMORY;
  ফেরত CURLE_OK;
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
স্থির শূন্য remove_expired(কাঠামো CookieInfo *ci)
{
  কাঠামো Cookie *co;
  curl_off_t now = (curl_off_t)time(ফাঁকা);
  unsigned পূর্ণসংখ্যা i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  যদি(now < ci->পরবর্তী_expiration &&
     ci->পরবর্তী_expiration != CURL_OFF_T_MAX)
    ফেরত;
  অন্যথা
    ci->পরবর্তী_expiration = CURL_OFF_T_MAX;

  জন্য(i = 0; i < COOKIE_HASH_আকার; i++) {
    কাঠামো Curl_llist_গ্রন্থি *n;
    কাঠামো Curl_llist_গ্রন্থি *e = ফাঁকা;

    জন্য(n = Curl_llist_শীর্ষ(&ci->cookielist[i]); n; n = e) {
      co = Curl_গ্রন্থি_elem(n);
      e = Curl_গ্রন্থি_পরবর্তী(n);
      যদি(co->expires) {
        যদি(co->expires < now) {
          Curl_গ্রন্থি_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        অন্যথা যদি(co->expires < ci->পরবর্তী_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->পরবর্তী_expiration = co->expires;
      }
    }
  }
}

#ifndef USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
স্থির bool bad_domain(ধ্রুবক অক্ষর *domain, size_t len)
{
  যদি((len == 9) && curl_strnequal(domain, "localhost", 9))
    ফেরত FALSE;
  অন্যথা {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ধ্রুবক অক্ষর *dot = memchr(domain, '.', len);
    যদি(dot) {
      size_t i = dot - domain;
      যদি((len - i) > 1)
        /* the dot is not the last byte */
        ফেরত FALSE;
    }
  }
  ফেরত TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
স্থির bool invalid_octets(ধ্রুবক অক্ষর *ptr, size_t len)
{
  ধ্রুবক unsigned অক্ষর *p = (ধ্রুবক unsigned অক্ষর *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  যখন(len && *p) {
    যদি((*p < 0x20) || (*p == 0x7f))
      ফেরত TRUE;
    p++;
    len--;
  }
  ফেরত FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_দৈর্ঘ্য 80

#define COOKIE_NAME   0
#define COOKIE_মান  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

স্থির CURLcode storecookie(কাঠামো Cookie *co, ধ্রুবক কাঠামো Curl_str *cp,
                            ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *domain)
{
  CURLcode ফলাফল;
  ফলাফল = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  যদি(!ফলাফল)
    ফলাফল = strstore(&co->মান, curlx_str(&cp[COOKIE_মান]),
                      curlx_strlen(&cp[COOKIE_মান]));
  যদি(!ফলাফল) {
    size_t plen = 0;
    যদি(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    অন্যথা যদি(path) {
      /* No path was given in the header line, set the default */
      ধ্রুবক অক্ষর *endslash = strrchr(path, '/');
      যদি(endslash)
        plen = endslash - path + 1; /* include end slash */
      অন্যথা
        plen = strlen(path);
    }

    যদি(path) {
      co->path = sanitize_cookie_path(path, plen);
      যদি(!co->path)
        ফলাফল = CURLE_OUT_OF_MEMORY;
    }
  }
  যদি(!ফলাফল) {
    যদি(curlx_strlen(&cp[COOKIE_DOMAIN]))
      ফলাফল = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    অন্যথা যদি(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      যদি(!co->domain)
        ফলাফল = CURLE_OUT_OF_MEMORY;
    }
  }
  ফেরত ফলাফল;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
স্থির bool parse_first_pair(কাঠামো Curl_easy *তথ্য, কাঠামো Cookie *co,
                             কাঠামো Curl_str *cookie,
                             কাঠামো Curl_str *name,
                             কাঠামো Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  যদি(!sep || !curlx_strlen(name)) {
    infof(তথ্য, "invalid cookie, dropped");
    ফেরত FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  যদি((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(তথ্য, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    ফেরত FALSE;
  }

  /* Check if we have a reserved prefix set. */
  যদি(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  অন্যথা যদি(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_মান] = *val;
  ফেরত TRUE;
}

স্থির bool parse_domain(কাঠামো Curl_easy *তথ্য, কাঠামো Cookie *co,
                         কাঠামো Curl_str *cookie_domain,
                         কাঠামো Curl_str *val,
                         ধ্রুবক অক্ষর **domainp)
{
  bool is_ip;
  ধ্রুবক অক্ষর *domain = *domainp;
  ধ্রুবক অক্ষর *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  যদি('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  যদি(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  যদি(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    যদি(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  অন্যথা {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(তথ্য, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির শূন্য parse_maxage(কাঠামো Cookie *co, কাঠামো Curl_str *val,
                         time_t *nowp)
{
  পূর্ণসংখ্যা rc;
  ধ্রুবক অক্ষর *maxage = curlx_str(val);
  যদি(*maxage == '\"')
    maxage++;
  rc = curlx_str_সংখ্যা(&maxage, &co->expires, CURL_OFF_T_MAX);
  যদি(!*nowp)
    *nowp = time(ফাঁকা);
  নির্বাচন(rc) {
  ক্ষেত্র STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    বিরতি;
  পূর্বনির্ধারিত:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    বিরতি;
  ক্ষেত্র STRE_OK:
    যদি(!co->expires)
      co->expires = 1; /* expire now */
    অন্যথা যদি(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    অন্যথা
      co->expires += *nowp;
    বিরতি;
  }
  cap_expires(*nowp, co);
}

স্থির শূন্য parse_expires(কাঠামো Cookie *co, কাঠামো Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  যদি(!co->expires && (curlx_strlen(val) < MAX_DATE_দৈর্ঘ্য)) {
    অক্ষর dbuf[MAX_DATE_দৈর্ঘ্য + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    যদি(!Curl_getdate_capped(dbuf, &date)) {
      যদি(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    অন্যথা
      co->expires = 0;
    যদি(!*nowp)
      *nowp = time(ফাঁকা);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
স্থির CURLcode
parse_cookie_header(কাঠামো Curl_easy *তথ্য,
                    কাঠামো Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ধ্রুবক অক্ষর *ptr, /* the header */
                    ধ্রুবক অক্ষর *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ধ্রুবক অক্ষর *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  যদি(linelength > MAX_COOKIE_সারি)
    /* discard overly long lines at once */
    ফেরত CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, আকার(cookie));
  করো {
    কাঠামো Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    পূর্ণসংখ্যা rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    যদি(!rc || (rc == STRE_SHORT)) {
      কাঠামো Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      যদি(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(তথ্য, "invalid octets in name, cookie dropped");
        ফেরত CURLE_OK;
      }

      যদি(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        যদি(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        যদি(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(তথ্য, "invalid octets in value, cookie dropped");
          ফেরত CURLE_OK;
        }
      }
      অন্যথা
        curlx_str_init(&val);

      যদি(!curlx_strlen(&cookie[COOKIE_NAME])) {
        যদি(!parse_first_pair(তথ্য, co, cookie, &name, &val, sep))
          ফেরত CURLE_OK;
      }

      /* whether there's separator or not */
      অন্যথা যদি(curlx_str_casecompare(&name, "secure")) {
        যদি(secure_origin)
          co->secure = TRUE;
        অন্যথা {
          infof(তথ্য, "skipped cookie because not 'secure'");
          ফেরত CURLE_OK;
        }
      }
      অন্যথা যদি(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      অন্যথা যদি(sep) {
        /* only if a separating equals sign is present */
        যদি(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        অন্যথা যদি(curlx_str_casecompare(&name, "domain")) {
          যদি(curlx_strlen(&val) &&
             !parse_domain(তথ্য, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            ফেরত CURLE_OK;
        }
        অন্যথা যদি(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        অন্যথা যদি(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } যখন(!curlx_str_single(&ptr, ';'));

  যদি(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    ফলাফল = storecookie(co, &cookie[0], path, domain);
    যদি(!ফলাফল)
      *okay = TRUE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode parse_netscape(কাঠামো Cookie *co,
                               ধ্রুবক কাঠামো CookieInfo *ci,
                               bool *okay,
                               ধ্রুবক অক্ষর *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ধ্রুবক অক্ষর *ptr, *পরবর্তী;
  পূর্ণসংখ্যা fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  যদি(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  যদি(lineptr[0] == '#')
    /* do not even try the comments */
    ফেরত CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  জন্য(পরবর্তী = lineptr; পরবর্তী; fields++) {
    ptr = পরবর্তী;
    len = strcspn(ptr, "\t\r\n");
    পরবর্তী = (ptr[len] == '\t' ? &ptr[len + 1] : ফাঁকা);
    নির্বাচন(fields) {
    ক্ষেত্র 0:
      যদি(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      যদি(!co->domain)
        ফেরত CURLE_OUT_OF_MEMORY;
      বিরতি;
    ক্ষেত্র 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      বিরতি;
    ক্ষেত্র 2:
      /* The file format allows the path field to remain not filled in */
      যদি(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        যদি(!co->path)
          ফেরত CURLE_OUT_OF_MEMORY;
        বিরতি;
      }
      অন্যথা {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        যদি(!co->path)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    ক্ষেত্র 3:
      co->secure = FALSE;
      যদি(curl_strnequal(ptr, "TRUE", len)) {
        যদি(secure_origin || ci->running)
          co->secure = TRUE;
        অন্যথা
          ফেরত CURLE_OK;
      }
      বিরতি;
    ক্ষেত্র 4:
      যদি(curlx_str_সংখ্যা(&ptr, &co->expires, CURL_OFF_T_MAX))
        ফেরত CURLE_OK;
      বিরতি;
    ক্ষেত্র 5:
      co->name = curlx_memdup0(ptr, len);
      যদি(!co->name)
        ফেরত CURLE_OUT_OF_MEMORY;
      অন্যথা {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        যদি(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        অন্যথা যদি(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      বিরতি;
    ক্ষেত্র 6:
      co->মান = curlx_memdup0(ptr, len);
      যদি(!co->মান)
        ফেরত CURLE_OUT_OF_MEMORY;
      বিরতি;
    }
  }
  যদি(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->মান = curlx_strdup("");
    যদি(!co->মান)
      ফেরত CURLE_OUT_OF_MEMORY;
    অন্যথা
      fields++;
  }

  যদি(fields != 7)
    /* we did not find the sufficient number of fields */
    ফেরত CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  যদি(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->মান, strlen(co->মান)))
    ফেরত CURLE_OK;

  *okay = TRUE;
  ফেরত CURLE_OK;
}

স্থির bool is_public_suffix(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Cookie *co,
                             ধ্রুবক অক্ষর *domain)
{
#ifdef USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(co);
  DEBUGF(infof(তথ্য, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  যদি(!co->domain || Curl_host_is_ipnum(co->domain))
    ফেরত FALSE;

  অন্যথা {
    bool acceptable = FALSE;
    অক্ষর lcase[256];
    অক্ষর lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    যদি(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    যদি(clen && (co->domain[clen - 1] == '.'))
      clen--;

    যদি((dlen < আকার(lcase)) && (clen < আকার(lcookie))) {
      ধ্রুবক psl_ctx_t *psl = Curl_psl_use(তথ্য);
      যদি(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        যদি(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          যদি(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        অন্যথা {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          যদি(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(তথ্য);
      }
      অন্যথা
        infof(তথ্য, "libpsl problem, rejecting cookie for safety");
    }

    যদি(!acceptable) {
      infof(তথ্য, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      ফেরত TRUE;
    }
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)co;
  (শূন্য)domain;
  DEBUGF(infof(তথ্য, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  ফেরত FALSE;
}

/* returns TRUE when replaced */
স্থির bool replace_existing(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Cookie *co,
                             ধ্রুবক কাঠামো CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  কাঠামো Curl_llist_গ্রন্থি *replace_n = ফাঁকা;
  কাঠামো Curl_llist_গ্রন্থি *n;
  size_t myhash = cookiehash(co->domain);
  জন্য(n = Curl_llist_শীর্ষ(&ci->cookielist[myhash]); n; n = Curl_গ্রন্থি_পরবর্তী(n)) {
    কাঠামো Cookie *clist = Curl_গ্রন্থি_elem(n);
    যদি(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      যদি(clist->domain && co->domain) {
        যদি(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      অন্যথা যদি(!clist->domain && !co->domain)
        matching_domains = TRUE;

      যদি(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ধ্রুবক অক্ষর *sep = ফাঁকা;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        যদি(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        যদি(sep)
          cllen = sep - clist->path;
        অন্যথা
          cllen = strlen(clist->path);

        যদি(!strncmp(clist->path, co->path, cllen)) {
          infof(তথ্য, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          ফেরত FALSE;
        }
      }
    }

    যদি(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      যদি(clist->domain && co->domain) {
        যদি(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      অন্যথা যদি(!clist->domain && !co->domain)
        replace_old = TRUE;

      যদি(replace_old) {
        /* the domains were identical */

        যদি(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        অন্যথা যদি(!clist->path != !co->path)
          replace_old = FALSE;
      }

      যদি(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        ফেরত FALSE;
      }
      যদি(replace_old)
        replace_n = n;
    }
  }
  যদি(replace_n) {
    কাঠামো Cookie *repl = Curl_গ্রন্থি_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_গ্রন্থি_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  ফেরত TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(কাঠামো Curl_easy *তথ্য,
                         কাঠামো CookieInfo *ci,
                         ধ্রুবক অক্ষর *lineptr, /* first character of the line */
                         ধ্রুবক অক্ষর *domain,  /* default domain */
                         ধ্রুবক অক্ষর *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ধ্রুবক পূর্ণসংখ্যা flags)
{
  কাঠামো Cookie comem;
  কাঠামো Cookie *co;
  size_t myhash;
  CURLcode ফলাফল;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  যদি(তথ্য->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    ফেরত CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, আকার(comem));

  যদি(flags & COOKIE_HTTPHEADER)
    ফলাফল = parse_cookie_header(তথ্য, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  অন্যথা
    ফলাফল = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  যদি(ফলাফল || !okay)
    যাও fail;

  যদি(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    যাও fail;

  যদি(!(flags & COOKIE_NOPSL) && is_public_suffix(তথ্য, co, domain))
    যাও fail;

  যদি(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    যদি(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    অন্যথা
      যাও fail;
  }

  যদি(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    যাও fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  যদি(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  যদি(!replace_existing(তথ্য, co, ci, flags & COOKIE_SECURE, &replaces))
    যাও fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, আকার(comem));
  যদি(!co) {
    co = &comem;
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->গ্রন্থি);

  যদি(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(তথ্য, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->মান,
          co->domain, co->path, co->expires);

  যদি(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  যদি(co->expires && (co->expires < ci->পরবর্তী_expiration))
    ci->পরবর্তী_expiration = co->expires;

  যদি(flags & COOKIE_HTTPHEADER)
    তথ্য->req.setcookies++;

  ফেরত ফলাফল;
fail:
  freecookie(co, FALSE);
  ফেরত ফলাফল;
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
কাঠামো CookieInfo *Curl_cookie_init(শূন্য)
{
  পূর্ণসংখ্যা i;
  কাঠামো CookieInfo *ci = curlx_calloc(1, আকার(কাঠামো CookieInfo));
  যদি(!ci)
    ফেরত ফাঁকা;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  জন্য(i = 0; i < COOKIE_HASH_আকার; i++)
    Curl_llist_init(&ci->cookielist[i], ফাঁকা);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->পরবর্তী_expiration = CURL_OFF_T_MAX;

  ফেরত ci;
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
স্থির CURLcode cookie_load(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *file,
                            কাঠামো CookieInfo *ci, পূর্ণসংখ্যা flags)
{
  FILE *handle = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  FILE *fp = ফাঁকা;
  DEBUGASSERT(ci);
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  যদি(file && *file) {
    যদি(!strcmp(file, "-"))
      fp = stdin;
    অন্যথা {
      fp = curlx_fopen(file, "rb");
      যদি(!fp)
        infof(তথ্য, "WARNING: failed to open cookie file \"%s\"", file);
      অন্যথা {
        curlx_struct_stat stat;
        যদি((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = ফাঁকা;
          infof(তথ্য, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        অন্যথা
          handle = fp;
      }
    }
  }

  যদি(fp) {
    কাঠামো dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    করো {
      ফলাফল = Curl_get_সারি(&buf, fp, &eof);
      যদি(!ফলাফল) {
        ধ্রুবক অক্ষর *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        যদি(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        ফলাফল = Curl_cookie_add(তথ্য, ci, lineptr, ফাঁকা, ফাঁকা,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        যদি(ফলাফল != CURLE_OUT_OF_MEMORY)
          ফলাফল = CURLE_OK;
      }
    } যখন(!ফলাফল && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    যদি(handle)
      curlx_fclose(handle);
  }
  তথ্য->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  ফেরত ফলাফল;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(কাঠামো Curl_easy *তথ্য,
                               পূর্ণসংখ্যা flags)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো curl_slist *list = তথ্য->state.cookielist;
  যদি(list) {
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    যদি(!তথ্য->cookies)
      তথ্য->cookies = Curl_cookie_init();
    যদি(!তথ্য->cookies)
      ফলাফল = CURLE_OUT_OF_MEMORY;
    অন্যথা {
      তথ্য->state.cookie_engine = TRUE;
      যখন(list) {
        ফলাফল = cookie_load(তথ্য, list->তথ্য, তথ্য->cookies, flags);
        যদি(ফলাফল)
          বিরতি;
        list = list->পরবর্তী;
      }
    }
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
  }
  ফেরত ফলাফল;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
স্থির পূর্ণসংখ্যা cookie_sort(ধ্রুবক শূন্য *p1, ধ্রুবক শূন্য *p2)
{
  ধ্রুবক কাঠামো Cookie *c1 = *(ধ্রুবক কাঠামো Cookie * ধ্রুবক *)p1;
  ধ্রুবক কাঠামো Cookie *c2 = *(ধ্রুবক কাঠামো Cookie * ধ্রুবক *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  যদি(l1 != l2)
    ফেরত (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  যদি(l1 != l2)
    ফেরত (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  যদি(l1 != l2)
    ফেরত (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  ফেরত (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
স্থির পূর্ণসংখ্যা cookie_sort_ct(ধ্রুবক শূন্য *p1, ধ্রুবক শূন্য *p2)
{
  ধ্রুবক কাঠামো Cookie *c1 = *(ধ্রুবক কাঠামো Cookie * ধ্রুবক *)p1;
  ধ্রুবক কাঠামো Cookie *c2 = *(ধ্রুবক কাঠামো Cookie * ধ্রুবক *)p2;

  ফেরত (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *host)
{
  ফেরত Curl_xfer_is_secure(তথ্য) ||
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
CURLcode Curl_cookie_getlist(কাঠামো Curl_easy *তথ্য,
                             bool *okay,
                             ধ্রুবক অক্ষর *host,
                             কাঠামো Curl_llist *list)
{
  size_t matches = 0;
  ধ্রুবক bool is_ip = Curl_host_is_ipnum(host);
  ধ্রুবক size_t myhash = cookiehash(host);
  কাঠামো Curl_llist_গ্রন্থি *n;
  ধ্রুবক bool secure = Curl_secure_context(তথ্য, host);
  কাঠামো CookieInfo *ci = তথ্য->cookies;
  ধ্রুবক অক্ষর *path = তথ্য->state.up.path;
  CURLcode ফলাফল = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ফাঁকা);

  যদি(!ci || !Curl_llist_গুনতি(&ci->cookielist[myhash]))
    ফেরত CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  জন্য(n = Curl_llist_শীর্ষ(&ci->cookielist[myhash]); n; n = Curl_গ্রন্থি_পরবর্তী(n)) {
    কাঠামো Cookie *co = Curl_গ্রন্থি_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    যদি(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      যদি(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, strlen(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        যদি(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          যদি(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(তথ্য, "Included max number of cookies (%zu) in request!",
                  matches);
            বিরতি;
          }
        }
      }
    }
  }

  যদি(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    কাঠামো Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(আকার(কাঠামো Cookie *) * matches);
    যদি(!array) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও fail;
    }

    n = Curl_llist_শীর্ষ(list);

    জন্য(i = 0; n; n = Curl_গ্রন্থি_পরবর্তী(n))
      array[i++] = Curl_গ্রন্থি_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, আকার(কাঠামো Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ফাঁকা);

    জন্য(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  ফেরত CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ফাঁকা);
  ফেরত ফলাফল; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
শূন্য Curl_cookie_clearall(কাঠামো CookieInfo *ci)
{
  যদি(ci) {
    unsigned পূর্ণসংখ্যা i;
    জন্য(i = 0; i < COOKIE_HASH_আকার; i++) {
      কাঠামো Curl_llist_গ্রন্থি *n;
      জন্য(n = Curl_llist_শীর্ষ(&ci->cookielist[i]); n;) {
        কাঠামো Cookie *c = Curl_গ্রন্থি_elem(n);
        কাঠামো Curl_llist_গ্রন্থি *e = Curl_গ্রন্থি_পরবর্তী(n);
        Curl_গ্রন্থি_remove(n);
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
শূন্য Curl_cookie_clearsess(কাঠামো CookieInfo *ci)
{
  unsigned পূর্ণসংখ্যা i;

  যদি(!ci)
    ফেরত;

  জন্য(i = 0; i < COOKIE_HASH_আকার; i++) {
    কাঠামো Curl_llist_গ্রন্থি *n = Curl_llist_শীর্ষ(&ci->cookielist[i]);
    কাঠামো Curl_llist_গ্রন্থি *e = ফাঁকা;

    জন্য(; n; n = e) {
      কাঠামো Cookie *curr = Curl_গ্রন্থি_elem(n);
      e = Curl_গ্রন্থি_পরবর্তী(n); /* in case the node is removed, get it early */
      যদি(!curr->expires) {
        Curl_গ্রন্থি_remove(n);
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
শূন্য Curl_cookie_cleanup(কাঠামো CookieInfo *ci)
{
  যদি(ci) {
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
স্থির অক্ষর *get_netscape_format(ধ্রুবক কাঠামো Cookie *co)
{
  ফেরত curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*মান*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->path ? co->path : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->মান ? co->মান : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
স্থির CURLcode cookie_output(কাঠামো Curl_easy *তথ্য,
                              কাঠামো CookieInfo *ci,
                              ধ্রুবক অক্ষর *filename)
{
  FILE *out = ফাঁকা;
  bool use_stdout = FALSE;
  অক্ষর *tempstore = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!ci)
    /* no cookie engine alive */
    ফেরত CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  যদি(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  অন্যথা {
    ফলাফল = Curl_fopen(তথ্য, filename, &out, &tempstore);
    যদি(ফলাফল)
      যাও error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  যদি(ci->numcookies) {
    unsigned পূর্ণসংখ্যা i;
    size_t nvalid = 0;
    কাঠামো Cookie **array;
    কাঠামো Curl_llist_গ্রন্থি *n;

    array = curlx_calloc(1, আকার(কাঠামো Cookie *) * ci->numcookies);
    যদি(!array) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও error;
    }

    /* only sort the cookies with a domain property */
    জন্য(i = 0; i < COOKIE_HASH_আকার; i++) {
      জন্য(n = Curl_llist_শীর্ষ(&ci->cookielist[i]); n; n = Curl_গ্রন্থি_পরবর্তী(n)) {
        কাঠামো Cookie *co = Curl_গ্রন্থি_elem(n);
        যদি(!co->domain)
          চালু;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, আকার(কাঠামো Cookie *), cookie_sort_ct);

    জন্য(i = 0; i < nvalid; i++) {
      অক্ষর *format_ptr = get_netscape_format(array[i]);
      যদি(!format_ptr) {
        curlx_free(array);
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  যদি(!use_stdout) {
    curlx_fclose(out);
    out = ফাঁকা;
    যদি(tempstore && curlx_rename(tempstore, filename)) {
      ফলাফল = CURLE_WRITE_ERROR;
      যাও error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  ফেরত CURLE_OK;

error:
  যদি(out && !use_stdout)
    curlx_fclose(out);
  যদি(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  ফেরত ফলাফল;
}

স্থির কাঠামো curl_slist *cookie_list(ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curl_slist *list = ফাঁকা;
  কাঠামো curl_slist *beg;
  কাঠামো curl_slist **লেজ = &list;
  unsigned পূর্ণসংখ্যা i;
  কাঠামো Curl_llist_গ্রন্থি *n;

  যদি(!তথ্য->cookies || (তথ্য->cookies->numcookies == 0))
    ফেরত ফাঁকা;

  /* at first, remove expired cookies */
  remove_expired(তথ্য->cookies);

  জন্য(i = 0; i < COOKIE_HASH_আকার; i++) {
    জন্য(n = Curl_llist_শীর্ষ(&তথ্য->cookies->cookielist[i]); n;
        n = Curl_গ্রন্থি_পরবর্তী(n)) {
      কাঠামো Cookie *c = Curl_গ্রন্থি_elem(n);
      অক্ষর *সারি;
      যদি(!c->domain)
        চালু;
      সারি = get_netscape_format(c);
      যদি(!সারি) {
        curl_slist_free_all(list);
        ফেরত ফাঁকা;
      }
      beg = Curl_slist_append_nodup(ফাঁকা, সারি);
      যদি(!beg) {
        curlx_free(সারি);
        curl_slist_free_all(list);
        ফেরত ফাঁকা;
      }
      *লেজ = beg;
      লেজ = &beg->পরবর্তী;
    }
  }

  ফেরত list;
}

কাঠামো curl_slist *Curl_cookie_list(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curl_slist *list;
  Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(তথ্য);
  Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
  ফেরত list;
}

শূন্য Curl_flush_cookies(কাঠামো Curl_easy *তথ্য, bool cleanup)
{
  Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  যদি(তথ্য->cookies) {
    ধ্রুবক অক্ষর *cookiejar = CURL_EASY_STR(তথ্য, STRING_COOKIEJAR);
    যদি(cookiejar && তথ্য->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode ফলাফল = cookie_output(তথ্য, তথ্য->cookies, cookiejar);
      যদি(ফলাফল)
        infof(তথ্য, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(ফলাফল));
    }

    যদি(cleanup && (!তথ্য->share || (তথ্য->cookies != তথ্য->share->cookies))) {
      Curl_cookie_cleanup(তথ্য->cookies);
      তথ্য->cookies = ফাঁকা;
    }
  }
  Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
}

শূন্য Curl_cookie_run(কাঠামো Curl_easy *তথ্য)
{
  Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  যদি(তথ্য->cookies)
    তথ্য->cookies->running = TRUE;
  Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
