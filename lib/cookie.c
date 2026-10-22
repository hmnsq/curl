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
স্থির শূন্য cap_expires(time_t now, সংরচনা Cookie *co)
{
  যদি(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    যদি(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

স্থির শূন্য freecookie(সংরচনা Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->মান);
  যদি(maintoo)
    curlx_free(co);
}

স্থির bool cookie_tailmatch(ধ্রুব বর্ণ *cookie_domain,
                             ধ্রুব size_t cookie_domain_len,
                             ধ্রুব বর্ণ *hostname)
{
  size_t hostname_len = strlen(hostname);

  যদি(hostname_len < cookie_domain_len)
    প্রত্যাগম FALSE;

  যদি(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    প্রত্যাগম FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  যদি(hostname_len == cookie_domain_len)
    প্রত্যাগম TRUE;
  যদি('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    প্রত্যাগম TRUE;
  প্রত্যাগম FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
স্থির bool pathmatch(ধ্রুব বর্ণ *cookie_path, ধ্রুব বর্ণ *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  যদি(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    প্রত্যাগম TRUE;
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
    গচ্ছ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  যদি(strncmp(cookie_path, uri_path, cookie_path_len))
    গচ্ছ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  যদি(cookie_path_len == uri_path_len) {
    ret = TRUE;
    গচ্ছ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  যদি(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    গচ্ছ pathmatched;
  }

pathmatched:
  প্রত্যাগম ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
স্থির ধ্রুব বর্ণ *get_শীর্ষ_domain(ধ্রুব বর্ণ * ধ্রুব domain, size_t *outlen)
{
  size_t len = 0;
  ধ্রুব বর্ণ *first = রিক্ত, *last;

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

  প্রত্যাগম first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#যদি defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
স্থির size_t cookie_hash_domain(ধ্রুব বর্ণ *domain, ধ্রুব size_t len)
{
  ধ্রুব বর্ণ *end = domain + len;
  size_t h = 5381;

  যাবত্(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  প্রত্যাগম (h % COOKIE_HASH_SIZE);
}

#যদি defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
স্থির size_t cookiehash(ধ্রুব বর্ণ * ধ্রুব domain)
{
  ধ্রুব বর্ণ *শীর্ষ;
  size_t len;

  যদি(!domain || Curl_host_is_ipnum(domain))
    প্রত্যাগম 0;

  শীর্ষ = get_শীর্ষ_domain(domain, &len);
  প্রত্যাগম cookie_hash_domain(শীর্ষ, len);
}

/*
 * cookie path sanitize
 */
স্থির বর্ণ *sanitize_cookie_path(ধ্রুব বর্ণ *cookie_path, size_t len)
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
    প্রত্যাগম curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  যদি(len > 1 && cookie_path[len - 1] == '/')
    len--;

  প্রত্যাগম curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
স্থির CURLcode strstore(বর্ণ **str, ধ্রুব বর্ণ *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  যদি(!*str)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  প্রত্যাগম CURLE_OK;
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
স্থির শূন্য remove_expired(সংরচনা CookieInfo *ci)
{
  সংরচনা Cookie *co;
  curl_off_t now = (curl_off_t)time(রিক্ত);
  unsigned পূর্ণাঙ্ক i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  যদি(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    প্রত্যাগম;
  অন্যথা
    ci->next_expiration = CURL_OFF_T_MAX;

  প্রতি(i = 0; i < COOKIE_HASH_SIZE; i++) {
    সংরচনা Curl_llist_গ্রন্থি *n;
    সংরচনা Curl_llist_গ্রন্থি *e = রিক্ত;

    প্রতি(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_গ্রন্থি_elem(n);
      e = Curl_গ্রন্থি_next(n);
      যদি(co->expires) {
        যদি(co->expires < now) {
          Curl_গ্রন্থি_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        অন্যথা যদি(co->expires < ci->next_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->next_expiration = co->expires;
      }
    }
  }
}

#ifndef USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
স্থির bool bad_domain(ধ্রুব বর্ণ *domain, size_t len)
{
  যদি((len == 9) && curl_strnequal(domain, "localhost", 9))
    প্রত্যাগম FALSE;
  অন্যথা {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ধ্রুব বর্ণ *dot = memchr(domain, '.', len);
    যদি(dot) {
      size_t i = dot - domain;
      যদি((len - i) > 1)
        /* the dot is not the last byte */
        প্রত্যাগম FALSE;
    }
  }
  প্রত্যাগম TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
স্থির bool invalid_octets(ধ্রুব বর্ণ *ptr, size_t len)
{
  ধ্রুব unsigned বর্ণ *p = (ধ্রুব unsigned বর্ণ *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  যাবত্(len && *p) {
    যদি((*p < 0x20) || (*p == 0x7f))
      প্রত্যাগম TRUE;
    p++;
    len--;
  }
  প্রত্যাগম FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_মান  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

স্থির CURLcode storecookie(সংরচনা Cookie *co, ধ্রুব সংরচনা Curl_str *cp,
                            ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  যদি(!result)
    result = strstore(&co->মান, curlx_str(&cp[COOKIE_মান]),
                      curlx_strlen(&cp[COOKIE_মান]));
  যদি(!result) {
    size_t plen = 0;
    যদি(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    অন্যথা যদি(path) {
      /* No path was given in the header line, set the default */
      ধ্রুব বর্ণ *endslash = strrchr(path, '/');
      যদি(endslash)
        plen = endslash - path + 1; /* include end slash */
      অন্যথা
        plen = strlen(path);
    }

    যদি(path) {
      co->path = sanitize_cookie_path(path, plen);
      যদি(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  যদি(!result) {
    যদি(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    অন্যথা যদি(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      যদি(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  প্রত্যাগম result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
স্থির bool parse_first_pair(সংরচনা Curl_easy *data, সংরচনা Cookie *co,
                             সংরচনা Curl_str *cookie,
                             সংরচনা Curl_str *name,
                             সংরচনা Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  যদি(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    প্রত্যাগম FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  যদি((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    প্রত্যাগম FALSE;
  }

  /* Check if we have a reserved prefix set. */
  যদি(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  অন্যথা যদি(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_মান] = *val;
  প্রত্যাগম TRUE;
}

স্থির bool parse_domain(সংরচনা Curl_easy *data, সংরচনা Cookie *co,
                         সংরচনা Curl_str *cookie_domain,
                         সংরচনা Curl_str *val,
                         ধ্রুব বর্ণ **domainp)
{
  bool is_ip;
  ধ্রুব বর্ণ *domain = *domainp;
  ধ্রুব বর্ণ *v = curlx_str(val);
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
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    প্রত্যাগম FALSE;
  }
  প্রত্যাগম TRUE;
}

স্থির শূন্য parse_maxage(সংরচনা Cookie *co, সংরচনা Curl_str *val,
                         time_t *nowp)
{
  পূর্ণাঙ্ক rc;
  ধ্রুব বর্ণ *maxage = curlx_str(val);
  যদি(*maxage == '\"')
    maxage++;
  rc = curlx_str_সংখ্যা(&maxage, &co->expires, CURL_OFF_T_MAX);
  যদি(!*nowp)
    *nowp = time(রিক্ত);
  বিকল্প(rc) {
  পক্ষ STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    ভঙ্গ;
  স্বভাব:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    ভঙ্গ;
  পক্ষ STRE_OK:
    যদি(!co->expires)
      co->expires = 1; /* expire now */
    অন্যথা যদি(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    অন্যথা
      co->expires += *nowp;
    ভঙ্গ;
  }
  cap_expires(*nowp, co);
}

স্থির শূন্য parse_expires(সংরচনা Cookie *co, সংরচনা Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  যদি(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    বর্ণ dbuf[MAX_DATE_LENGTH + 1];
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
      *nowp = time(রিক্ত);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
স্থির CURLcode
parse_cookie_header(সংরচনা Curl_easy *data,
                    সংরচনা Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ধ্রুব বর্ণ *ptr, /* the header */
                    ধ্রুব বর্ণ *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ধ্রুব বর্ণ *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  সংরচনা Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  যদি(linelength > MAX_COOKIE_সারি)
    /* discard overly long lines at once */
    প্রত্যাগম CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, পরিমাণ(cookie));
  কুরু {
    সংরচনা Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    পূর্ণাঙ্ক rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    যদি(!rc || (rc == STRE_SHORT)) {
      সংরচনা Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      যদি(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        প্রত্যাগম CURLE_OK;
      }

      যদি(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        যদি(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        যদি(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          প্রত্যাগম CURLE_OK;
        }
      }
      অন্যথা
        curlx_str_init(&val);

      যদি(!curlx_strlen(&cookie[COOKIE_NAME])) {
        যদি(!parse_first_pair(data, co, cookie, &name, &val, sep))
          প্রত্যাগম CURLE_OK;
      }

      /* whether there's separator or not */
      অন্যথা যদি(curlx_str_casecompare(&name, "secure")) {
        যদি(secure_origin)
          co->secure = TRUE;
        অন্যথা {
          infof(data, "skipped cookie because not 'secure'");
          প্রত্যাগম CURLE_OK;
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
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            প্রত্যাগম CURLE_OK;
        }
        অন্যথা যদি(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        অন্যথা যদি(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } যাবত্(!curlx_str_single(&ptr, ';'));

  যদি(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    যদি(!result)
      *okay = TRUE;
  }
  প্রত্যাগম result;
}

স্থির CURLcode parse_netscape(সংরচনা Cookie *co,
                               ধ্রুব সংরচনা CookieInfo *ci,
                               bool *okay,
                               ধ্রুব বর্ণ *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ধ্রুব বর্ণ *ptr, *next;
  পূর্ণাঙ্ক fields;
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
    প্রত্যাগম CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  প্রতি(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : রিক্ত);
    বিকল্প(fields) {
    পক্ষ 0:
      যদি(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      যদি(!co->domain)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      ভঙ্গ;
    পক্ষ 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      ভঙ্গ;
    পক্ষ 2:
      /* The file format allows the path field to remain not filled in */
      যদি(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        যদি(!co->path)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;
        ভঙ্গ;
      }
      অন্যথা {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        যদি(!co->path)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    পক্ষ 3:
      co->secure = FALSE;
      যদি(curl_strnequal(ptr, "TRUE", len)) {
        যদি(secure_origin || ci->running)
          co->secure = TRUE;
        অন্যথা
          প্রত্যাগম CURLE_OK;
      }
      ভঙ্গ;
    পক্ষ 4:
      যদি(curlx_str_সংখ্যা(&ptr, &co->expires, CURL_OFF_T_MAX))
        প্রত্যাগম CURLE_OK;
      ভঙ্গ;
    পক্ষ 5:
      co->name = curlx_memdup0(ptr, len);
      যদি(!co->name)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      অন্যথা {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        যদি(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        অন্যথা যদি(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      ভঙ্গ;
    পক্ষ 6:
      co->মান = curlx_memdup0(ptr, len);
      যদি(!co->মান)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      ভঙ্গ;
    }
  }
  যদি(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->মান = curlx_strdup("");
    যদি(!co->মান)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    অন্যথা
      fields++;
  }

  যদি(fields != 7)
    /* we did not find the sufficient number of fields */
    প্রত্যাগম CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  যদি(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->মান, strlen(co->মান)))
    প্রত্যাগম CURLE_OK;

  *okay = TRUE;
  প্রত্যাগম CURLE_OK;
}

স্থির bool is_public_suffix(সংরচনা Curl_easy *data,
                             সংরচনা Cookie *co,
                             ধ্রুব বর্ণ *domain)
{
#ifdef USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(data);
  DEBUGASSERT(co);
  DEBUGF(infof(data, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  যদি(!co->domain || Curl_host_is_ipnum(co->domain))
    প্রত্যাগম FALSE;

  অন্যথা {
    bool acceptable = FALSE;
    বর্ণ lcase[256];
    বর্ণ lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    যদি(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    যদি(clen && (co->domain[clen - 1] == '.'))
      clen--;

    যদি((dlen < পরিমাণ(lcase)) && (clen < পরিমাণ(lcookie))) {
      ধ্রুব psl_ctx_t *psl = Curl_psl_use(data);
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
        Curl_psl_release(data);
      }
      অন্যথা
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    যদি(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      প্রত্যাগম TRUE;
    }
  }
#অন্যথা
  (শূন্য)data;
  (শূন্য)co;
  (শূন্য)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  প্রত্যাগম FALSE;
}

/* returns TRUE when replaced */
স্থির bool replace_existing(সংরচনা Curl_easy *data,
                             সংরচনা Cookie *co,
                             ধ্রুব সংরচনা CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  সংরচনা Curl_llist_গ্রন্থি *replace_n = রিক্ত;
  সংরচনা Curl_llist_গ্রন্থি *n;
  size_t myhash = cookiehash(co->domain);
  প্রতি(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_গ্রন্থি_next(n)) {
    সংরচনা Cookie *clist = Curl_গ্রন্থি_elem(n);
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
        ধ্রুব বর্ণ *sep = রিক্ত;

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
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          প্রত্যাগম FALSE;
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
        প্রত্যাগম FALSE;
      }
      যদি(replace_old)
        replace_n = n;
    }
  }
  যদি(replace_n) {
    সংরচনা Cookie *repl = Curl_গ্রন্থি_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_গ্রন্থি_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  প্রত্যাগম TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(সংরচনা Curl_easy *data,
                         সংরচনা CookieInfo *ci,
                         ধ্রুব বর্ণ *lineptr, /* first character of the line */
                         ধ্রুব বর্ণ *domain,  /* default domain */
                         ধ্রুব বর্ণ *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ধ্রুব পূর্ণাঙ্ক flags)
{
  সংরচনা Cookie comem;
  সংরচনা Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  যদি(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    প্রত্যাগম CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, পরিমাণ(comem));

  যদি(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  অন্যথা
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  যদি(result || !okay)
    গচ্ছ fail;

  যদি(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    গচ্ছ fail;

  যদি(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    গচ্ছ fail;

  যদি(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    যদি(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    অন্যথা
      গচ্ছ fail;
  }

  যদি(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    গচ্ছ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  যদি(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  যদি(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    গচ্ছ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, পরিমাণ(comem));
  যদি(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->গ্রন্থি);

  যদি(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->মান,
          co->domain, co->path, co->expires);

  যদি(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  যদি(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  যদি(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  প্রত্যাগম result;
fail:
  freecookie(co, FALSE);
  প্রত্যাগম result;
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
সংরচনা CookieInfo *Curl_cookie_init(শূন্য)
{
  পূর্ণাঙ্ক i;
  সংরচনা CookieInfo *ci = curlx_calloc(1, পরিমাণ(সংরচনা CookieInfo));
  যদি(!ci)
    প্রত্যাগম রিক্ত;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  প্রতি(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], রিক্ত);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  প্রত্যাগম ci;
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
স্থির CURLcode cookie_load(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *file,
                            সংরচনা CookieInfo *ci, পূর্ণাঙ্ক flags)
{
  FILE *handle = রিক্ত;
  CURLcode result = CURLE_OK;
  FILE *fp = রিক্ত;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  যদি(file && *file) {
    যদি(!strcmp(file, "-"))
      fp = stdin;
    অন্যথা {
      fp = curlx_fopen(file, "rb");
      যদি(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      অন্যথা {
        curlx_struct_stat stat;
        যদি((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = রিক্ত;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        অন্যথা
          handle = fp;
      }
    }
  }

  যদি(fp) {
    সংরচনা dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    কুরু {
      result = Curl_get_সারি(&buf, fp, &eof);
      যদি(!result) {
        ধ্রুব বর্ণ *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        যদি(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, রিক্ত, রিক্ত,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        যদি(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } যাবত্(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    যদি(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  প্রত্যাগম result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(সংরচনা Curl_easy *data,
                               পূর্ণাঙ্ক flags)
{
  CURLcode result = CURLE_OK;
  সংরচনা curl_slist *list = data->state.cookielist;
  যদি(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    যদি(!data->cookies)
      data->cookies = Curl_cookie_init();
    যদি(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    অন্যথা {
      data->state.cookie_engine = TRUE;
      যাবত্(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        যদি(result)
          ভঙ্গ;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  প্রত্যাগম result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
স্থির পূর্ণাঙ্ক cookie_sort(ধ্রুব শূন্য *p1, ধ্রুব শূন্য *p2)
{
  ধ্রুব সংরচনা Cookie *c1 = *(ধ্রুব সংরচনা Cookie * ধ্রুব *)p1;
  ধ্রুব সংরচনা Cookie *c2 = *(ধ্রুব সংরচনা Cookie * ধ্রুব *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  যদি(l1 != l2)
    প্রত্যাগম (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  যদি(l1 != l2)
    প্রত্যাগম (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  যদি(l1 != l2)
    প্রত্যাগম (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  প্রত্যাগম (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
স্থির পূর্ণাঙ্ক cookie_sort_ct(ধ্রুব শূন্য *p1, ধ্রুব শূন্য *p2)
{
  ধ্রুব সংরচনা Cookie *c1 = *(ধ্রুব সংরচনা Cookie * ধ্রুব *)p1;
  ধ্রুব সংরচনা Cookie *c2 = *(ধ্রুব সংরচনা Cookie * ধ্রুব *)p2;

  প্রত্যাগম (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *host)
{
  প্রত্যাগম Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(সংরচনা Curl_easy *data,
                             bool *okay,
                             ধ্রুব বর্ণ *host,
                             সংরচনা Curl_llist *list)
{
  size_t matches = 0;
  ধ্রুব bool is_ip = Curl_host_is_ipnum(host);
  ধ্রুব size_t myhash = cookiehash(host);
  সংরচনা Curl_llist_গ্রন্থি *n;
  ধ্রুব bool secure = Curl_secure_context(data, host);
  সংরচনা CookieInfo *ci = data->cookies;
  ধ্রুব বর্ণ *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, রিক্ত);

  যদি(!ci || !Curl_llist_গুনতি(&ci->cookielist[myhash]))
    প্রত্যাগম CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  প্রতি(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_গ্রন্থি_next(n)) {
    সংরচনা Cookie *co = Curl_গ্রন্থি_elem(n);

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
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            ভঙ্গ;
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
    সংরচনা Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(পরিমাণ(সংরচনা Cookie *) * matches);
    যদি(!array) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ fail;
    }

    n = Curl_llist_head(list);

    প্রতি(i = 0; n; n = Curl_গ্রন্থি_next(n))
      array[i++] = Curl_গ্রন্থি_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, পরিমাণ(সংরচনা Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, রিক্ত);

    প্রতি(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  প্রত্যাগম CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, রিক্ত);
  প্রত্যাগম result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
শূন্য Curl_cookie_clearall(সংরচনা CookieInfo *ci)
{
  যদি(ci) {
    unsigned পূর্ণাঙ্ক i;
    প্রতি(i = 0; i < COOKIE_HASH_SIZE; i++) {
      সংরচনা Curl_llist_গ্রন্থি *n;
      প্রতি(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        সংরচনা Cookie *c = Curl_গ্রন্থি_elem(n);
        সংরচনা Curl_llist_গ্রন্থি *e = Curl_গ্রন্থি_next(n);
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
শূন্য Curl_cookie_clearsess(সংরচনা CookieInfo *ci)
{
  unsigned পূর্ণাঙ্ক i;

  যদি(!ci)
    প্রত্যাগম;

  প্রতি(i = 0; i < COOKIE_HASH_SIZE; i++) {
    সংরচনা Curl_llist_গ্রন্থি *n = Curl_llist_head(&ci->cookielist[i]);
    সংরচনা Curl_llist_গ্রন্থি *e = রিক্ত;

    প্রতি(; n; n = e) {
      সংরচনা Cookie *curr = Curl_গ্রন্থি_elem(n);
      e = Curl_গ্রন্থি_next(n); /* in case the node is removed, get it early */
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
শূন্য Curl_cookie_cleanup(সংরচনা CookieInfo *ci)
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
স্থির বর্ণ *get_netscape_format(ধ্রুব সংরচনা Cookie *co)
{
  প্রত্যাগম curl_maprintf(
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
স্থির CURLcode cookie_output(সংরচনা Curl_easy *data,
                              সংরচনা CookieInfo *ci,
                              ধ্রুব বর্ণ *filename)
{
  FILE *out = রিক্ত;
  bool use_stdout = FALSE;
  বর্ণ *tempstore = রিক্ত;
  CURLcode result = CURLE_OK;

  যদি(!ci)
    /* no cookie engine alive */
    প্রত্যাগম CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  যদি(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  অন্যথা {
    result = Curl_fopen(data, filename, &out, &tempstore);
    যদি(result)
      গচ্ছ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  যদি(ci->numcookies) {
    unsigned পূর্ণাঙ্ক i;
    size_t nvalid = 0;
    সংরচনা Cookie **array;
    সংরচনা Curl_llist_গ্রন্থি *n;

    array = curlx_calloc(1, পরিমাণ(সংরচনা Cookie *) * ci->numcookies);
    যদি(!array) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ error;
    }

    /* only sort the cookies with a domain property */
    প্রতি(i = 0; i < COOKIE_HASH_SIZE; i++) {
      প্রতি(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_গ্রন্থি_next(n)) {
        সংরচনা Cookie *co = Curl_গ্রন্থি_elem(n);
        যদি(!co->domain)
          অনুবর্ত;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, পরিমাণ(সংরচনা Cookie *), cookie_sort_ct);

    প্রতি(i = 0; i < nvalid; i++) {
      বর্ণ *format_ptr = get_netscape_format(array[i]);
      যদি(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  যদি(!use_stdout) {
    curlx_fclose(out);
    out = রিক্ত;
    যদি(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      গচ্ছ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  প্রত্যাগম CURLE_OK;

error:
  যদি(out && !use_stdout)
    curlx_fclose(out);
  যদি(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  প্রত্যাগম result;
}

স্থির সংরচনা curl_slist *cookie_list(ধ্রুব সংরচনা Curl_easy *data)
{
  সংরচনা curl_slist *list = রিক্ত;
  সংরচনা curl_slist *beg;
  সংরচনা curl_slist **tail = &list;
  unsigned পূর্ণাঙ্ক i;
  সংরচনা Curl_llist_গ্রন্থি *n;

  যদি(!data->cookies || (data->cookies->numcookies == 0))
    প্রত্যাগম রিক্ত;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  প্রতি(i = 0; i < COOKIE_HASH_SIZE; i++) {
    প্রতি(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_গ্রন্থি_next(n)) {
      সংরচনা Cookie *c = Curl_গ্রন্থি_elem(n);
      বর্ণ *সারি;
      যদি(!c->domain)
        অনুবর্ত;
      সারি = get_netscape_format(c);
      যদি(!সারি) {
        curl_slist_free_all(list);
        প্রত্যাগম রিক্ত;
      }
      beg = Curl_slist_append_nodup(রিক্ত, সারি);
      যদি(!beg) {
        curlx_free(সারি);
        curl_slist_free_all(list);
        প্রত্যাগম রিক্ত;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  প্রত্যাগম list;
}

সংরচনা curl_slist *Curl_cookie_list(সংরচনা Curl_easy *data)
{
  সংরচনা curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  প্রত্যাগম list;
}

শূন্য Curl_flush_cookies(সংরচনা Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  যদি(data->cookies) {
    ধ্রুব বর্ণ *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    যদি(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      যদি(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    যদি(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = রিক্ত;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

শূন্য Curl_cookie_run(সংরচনা Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  যদি(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
