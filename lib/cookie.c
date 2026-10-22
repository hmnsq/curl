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
#अन्तर्भाव "curl_setup.h"

#सचे !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "psl.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "curl_memrchr.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "llist.h"
#अन्तर्भाव "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
स्थिर शून्य cap_expires(time_t now, संरचना Cookie *co)
{
  सचे(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    सचे(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

स्थिर शून्य freecookie(संरचना Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->मानम्);
  सचे(maintoo)
    curlx_free(co);
}

स्थिर bool cookie_tailmatch(ध्रुव वर्ण *cookie_domain,
                             ध्रुव size_t cookie_domain_len,
                             ध्रुव वर्ण *hostname)
{
  size_t hostname_len = strlen(hostname);

  सचे(hostname_len < cookie_domain_len)
    पटिनिवत्त FALSE;

  सचे(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    पटिनिवत्त FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  सचे(hostname_len == cookie_domain_len)
    पटिनिवत्त TRUE;
  सचे('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    पटिनिवत्त TRUE;
  पटिनिवत्त FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
स्थिर bool pathmatch(ध्रुव वर्ण *cookie_path, ध्रुव वर्ण *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  सचे(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    पटिनिवत्त TRUE;
  }

  /* #-fragments are already cut off! */
  सचे(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  सचे(uri_path_len < cookie_path_len)
    गच्छ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  सचे(strncmp(cookie_path, uri_path, cookie_path_len))
    गच्छ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  सचे(cookie_path_len == uri_path_len) {
    ret = TRUE;
    गच्छ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  सचे(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    गच्छ pathmatched;
  }

pathmatched:
  पटिनिवत्त ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
स्थिर ध्रुव वर्ण *get_शिखरम्_domain(ध्रुव वर्ण * ध्रुव domain, size_t *outlen)
{
  size_t len = 0;
  ध्रुव वर्ण *first = रिक्त, *last;

  सचे(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    सचे(last) {
      first = memrchr(domain, '.', (last - domain));
      सचे(first)
        len -= (++first - domain);
    }
  }

  सचे(outlen)
    *outlen = len;

  पटिनिवत्त first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#सचे defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
स्थिर size_t cookie_hash_domain(ध्रुव वर्ण *domain, ध्रुव size_t len)
{
  ध्रुव वर्ण *end = domain + len;
  size_t h = 5381;

  याव(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  पटिनिवत्त (h % COOKIE_HASH_SIZE);
}

#सचे defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
स्थिर size_t cookiehash(ध्रुव वर्ण * ध्रुव domain)
{
  ध्रुव वर्ण *शिखरम्;
  size_t len;

  सचे(!domain || Curl_host_is_ipnum(domain))
    पटिनिवत्त 0;

  शिखरम् = get_शिखरम्_domain(domain, &len);
  पटिनिवत्त cookie_hash_domain(शिखरम्, len);
}

/*
 * cookie path sanitize
 */
स्थिर वर्ण *sanitize_cookie_path(ध्रुव वर्ण *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  सचे(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    सचे(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  सचे(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    पटिनिवत्त curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  सचे(len > 1 && cookie_path[len - 1] == '/')
    len--;

  पटिनिवत्त curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
स्थिर CURLcode strstore(वर्ण **str, ध्रुव वर्ण *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  सचे(!*str)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  पटिनिवत्त CURLE_OK;
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
स्थिर शून्य remove_expired(संरचना CookieInfo *ci)
{
  संरचना Cookie *co;
  curl_off_t now = (curl_off_t)time(रिक्त);
  unsigned पूर्णाङ्क i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  सचे(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    पटिनिवत्त;
  अञ्ञथा
    ci->next_expiration = CURL_OFF_T_MAX;

  अत्थाय(i = 0; i < COOKIE_HASH_SIZE; i++) {
    संरचना Curl_llist_ग्रन्थिः *n;
    संरचना Curl_llist_ग्रन्थिः *e = रिक्त;

    अत्थाय(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_ग्रन्थिः_elem(n);
      e = Curl_ग्रन्थिः_next(n);
      सचे(co->expires) {
        सचे(co->expires < now) {
          Curl_ग्रन्थिः_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        अञ्ञथा सचे(co->expires < ci->next_expiration)
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
स्थिर bool bad_domain(ध्रुव वर्ण *domain, size_t len)
{
  सचे((len == 9) && curl_strnequal(domain, "localhost", 9))
    पटिनिवत्त FALSE;
  अञ्ञथा {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ध्रुव वर्ण *dot = memchr(domain, '.', len);
    सचे(dot) {
      size_t i = dot - domain;
      सचे((len - i) > 1)
        /* the dot is not the last byte */
        पटिनिवत्त FALSE;
    }
  }
  पटिनिवत्त TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
स्थिर bool invalid_octets(ध्रुव वर्ण *ptr, size_t len)
{
  ध्रुव unsigned वर्ण *p = (ध्रुव unsigned वर्ण *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  याव(len && *p) {
    सचे((*p < 0x20) || (*p == 0x7f))
      पटिनिवत्त TRUE;
    p++;
    len--;
  }
  पटिनिवत्त FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_मानम्  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

स्थिर CURLcode storecookie(संरचना Cookie *co, ध्रुव संरचना Curl_str *cp,
                            ध्रुव वर्ण *path, ध्रुव वर्ण *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  सचे(!result)
    result = strstore(&co->मानम्, curlx_str(&cp[COOKIE_मानम्]),
                      curlx_strlen(&cp[COOKIE_मानम्]));
  सचे(!result) {
    size_t plen = 0;
    सचे(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    अञ्ञथा सचे(path) {
      /* No path was given in the header line, set the default */
      ध्रुव वर्ण *endslash = strrchr(path, '/');
      सचे(endslash)
        plen = endslash - path + 1; /* include end slash */
      अञ्ञथा
        plen = strlen(path);
    }

    सचे(path) {
      co->path = sanitize_cookie_path(path, plen);
      सचे(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  सचे(!result) {
    सचे(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    अञ्ञथा सचे(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      सचे(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  पटिनिवत्त result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
स्थिर bool parse_first_pair(संरचना Curl_easy *data, संरचना Cookie *co,
                             संरचना Curl_str *cookie,
                             संरचना Curl_str *name,
                             संरचना Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  सचे(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    पटिनिवत्त FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  सचे((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    पटिनिवत्त FALSE;
  }

  /* Check if we have a reserved prefix set. */
  सचे(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  अञ्ञथा सचे(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_मानम्] = *val;
  पटिनिवत्त TRUE;
}

स्थिर bool parse_domain(संरचना Curl_easy *data, संरचना Cookie *co,
                         संरचना Curl_str *cookie_domain,
                         संरचना Curl_str *val,
                         ध्रुव वर्ण **domainp)
{
  bool is_ip;
  ध्रुव वर्ण *domain = *domainp;
  ध्रुव वर्ण *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  सचे('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  सचे(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  सचे(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    सचे(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  अञ्ञथा {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    पटिनिवत्त FALSE;
  }
  पटिनिवत्त TRUE;
}

स्थिर शून्य parse_maxage(संरचना Cookie *co, संरचना Curl_str *val,
                         time_t *nowp)
{
  पूर्णाङ्क rc;
  ध्रुव वर्ण *maxage = curlx_str(val);
  सचे(*maxage == '\"')
    maxage++;
  rc = curlx_str_सङ्ख्या(&maxage, &co->expires, CURL_OFF_T_MAX);
  सचे(!*nowp)
    *nowp = time(रिक्त);
  विकल्प(rc) {
  पक्ष STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    भङ्ग;
  स्वभाव:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    भङ्ग;
  पक्ष STRE_OK:
    सचे(!co->expires)
      co->expires = 1; /* expire now */
    अञ्ञथा सचे(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    अञ्ञथा
      co->expires += *nowp;
    भङ्ग;
  }
  cap_expires(*nowp, co);
}

स्थिर शून्य parse_expires(संरचना Cookie *co, संरचना Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  सचे(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    वर्ण dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    सचे(!Curl_getdate_capped(dbuf, &date)) {
      सचे(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    अञ्ञथा
      co->expires = 0;
    सचे(!*nowp)
      *nowp = time(रिक्त);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
स्थिर CURLcode
parse_cookie_header(संरचना Curl_easy *data,
                    संरचना Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ध्रुव वर्ण *ptr, /* the header */
                    ध्रुव वर्ण *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ध्रुव वर्ण *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  संरचना Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  सचे(linelength > MAX_COOKIE_पङ्क्तिः)
    /* discard overly long lines at once */
    पटिनिवत्त CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, परिमाण(cookie));
  करोहि {
    संरचना Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    पूर्णाङ्क rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    सचे(!rc || (rc == STRE_SHORT)) {
      संरचना Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      सचे(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        पटिनिवत्त CURLE_OK;
      }

      सचे(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        सचे(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        सचे(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          पटिनिवत्त CURLE_OK;
        }
      }
      अञ्ञथा
        curlx_str_init(&val);

      सचे(!curlx_strlen(&cookie[COOKIE_NAME])) {
        सचे(!parse_first_pair(data, co, cookie, &name, &val, sep))
          पटिनिवत्त CURLE_OK;
      }

      /* whether there's separator or not */
      अञ्ञथा सचे(curlx_str_casecompare(&name, "secure")) {
        सचे(secure_origin)
          co->secure = TRUE;
        अञ्ञथा {
          infof(data, "skipped cookie because not 'secure'");
          पटिनिवत्त CURLE_OK;
        }
      }
      अञ्ञथा सचे(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      अञ्ञथा सचे(sep) {
        /* only if a separating equals sign is present */
        सचे(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        अञ्ञथा सचे(curlx_str_casecompare(&name, "domain")) {
          सचे(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            पटिनिवत्त CURLE_OK;
        }
        अञ्ञथा सचे(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        अञ्ञथा सचे(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } याव(!curlx_str_single(&ptr, ';'));

  सचे(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    सचे(!result)
      *okay = TRUE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode parse_netscape(संरचना Cookie *co,
                               ध्रुव संरचना CookieInfo *ci,
                               bool *okay,
                               ध्रुव वर्ण *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ध्रुव वर्ण *ptr, *next;
  पूर्णाङ्क fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  सचे(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  सचे(lineptr[0] == '#')
    /* do not even try the comments */
    पटिनिवत्त CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  अत्थाय(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : रिक्त);
    विकल्प(fields) {
    पक्ष 0:
      सचे(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      सचे(!co->domain)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      भङ्ग;
    पक्ष 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      भङ्ग;
    पक्ष 2:
      /* The file format allows the path field to remain not filled in */
      सचे(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        सचे(!co->path)
          पटिनिवत्त CURLE_OUT_OF_MEMORY;
        भङ्ग;
      }
      अञ्ञथा {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        सचे(!co->path)
          पटिनिवत्त CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    पक्ष 3:
      co->secure = FALSE;
      सचे(curl_strnequal(ptr, "TRUE", len)) {
        सचे(secure_origin || ci->running)
          co->secure = TRUE;
        अञ्ञथा
          पटिनिवत्त CURLE_OK;
      }
      भङ्ग;
    पक्ष 4:
      सचे(curlx_str_सङ्ख्या(&ptr, &co->expires, CURL_OFF_T_MAX))
        पटिनिवत्त CURLE_OK;
      भङ्ग;
    पक्ष 5:
      co->name = curlx_memdup0(ptr, len);
      सचे(!co->name)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      अञ्ञथा {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        सचे(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        अञ्ञथा सचे(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      भङ्ग;
    पक्ष 6:
      co->मानम् = curlx_memdup0(ptr, len);
      सचे(!co->मानम्)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      भङ्ग;
    }
  }
  सचे(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->मानम् = curlx_strdup("");
    सचे(!co->मानम्)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    अञ्ञथा
      fields++;
  }

  सचे(fields != 7)
    /* we did not find the sufficient number of fields */
    पटिनिवत्त CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  सचे(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->मानम्, strlen(co->मानम्)))
    पटिनिवत्त CURLE_OK;

  *okay = TRUE;
  पटिनिवत्त CURLE_OK;
}

स्थिर bool is_public_suffix(संरचना Curl_easy *data,
                             संरचना Cookie *co,
                             ध्रुव वर्ण *domain)
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
  सचे(!co->domain || Curl_host_is_ipnum(co->domain))
    पटिनिवत्त FALSE;

  अञ्ञथा {
    bool acceptable = FALSE;
    वर्ण lcase[256];
    वर्ण lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    सचे(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    सचे(clen && (co->domain[clen - 1] == '.'))
      clen--;

    सचे((dlen < परिमाण(lcase)) && (clen < परिमाण(lcookie))) {
      ध्रुव psl_ctx_t *psl = Curl_psl_use(data);
      सचे(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        सचे(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          सचे(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        अञ्ञथा {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          सचे(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      अञ्ञथा
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    सचे(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      पटिनिवत्त TRUE;
    }
  }
#अञ्ञथा
  (शून्य)data;
  (शून्य)co;
  (शून्य)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  पटिनिवत्त FALSE;
}

/* returns TRUE when replaced */
स्थिर bool replace_existing(संरचना Curl_easy *data,
                             संरचना Cookie *co,
                             ध्रुव संरचना CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  संरचना Curl_llist_ग्रन्थिः *replace_n = रिक्त;
  संरचना Curl_llist_ग्रन्थिः *n;
  size_t myhash = cookiehash(co->domain);
  अत्थाय(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ग्रन्थिः_next(n)) {
    संरचना Cookie *clist = Curl_ग्रन्थिः_elem(n);
    सचे(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      सचे(clist->domain && co->domain) {
        सचे(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      अञ्ञथा सचे(!clist->domain && !co->domain)
        matching_domains = TRUE;

      सचे(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ध्रुव वर्ण *sep = रिक्त;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        सचे(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        सचे(sep)
          cllen = sep - clist->path;
        अञ्ञथा
          cllen = strlen(clist->path);

        सचे(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          पटिनिवत्त FALSE;
        }
      }
    }

    सचे(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      सचे(clist->domain && co->domain) {
        सचे(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      अञ्ञथा सचे(!clist->domain && !co->domain)
        replace_old = TRUE;

      सचे(replace_old) {
        /* the domains were identical */

        सचे(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        अञ्ञथा सचे(!clist->path != !co->path)
          replace_old = FALSE;
      }

      सचे(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        पटिनिवत्त FALSE;
      }
      सचे(replace_old)
        replace_n = n;
    }
  }
  सचे(replace_n) {
    संरचना Cookie *repl = Curl_ग्रन्थिः_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ग्रन्थिः_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  पटिनिवत्त TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(संरचना Curl_easy *data,
                         संरचना CookieInfo *ci,
                         ध्रुव वर्ण *lineptr, /* first character of the line */
                         ध्रुव वर्ण *domain,  /* default domain */
                         ध्रुव वर्ण *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ध्रुव पूर्णाङ्क flags)
{
  संरचना Cookie comem;
  संरचना Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  सचे(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    पटिनिवत्त CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, परिमाण(comem));

  सचे(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  अञ्ञथा
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  सचे(result || !okay)
    गच्छ fail;

  सचे(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    गच्छ fail;

  सचे(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    गच्छ fail;

  सचे(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    सचे(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    अञ्ञथा
      गच्छ fail;
  }

  सचे(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    गच्छ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  सचे(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  सचे(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    गच्छ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, परिमाण(comem));
  सचे(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ग्रन्थिः);

  सचे(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->मानम्,
          co->domain, co->path, co->expires);

  सचे(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  सचे(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  सचे(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  पटिनिवत्त result;
fail:
  freecookie(co, FALSE);
  पटिनिवत्त result;
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
संरचना CookieInfo *Curl_cookie_init(शून्य)
{
  पूर्णाङ्क i;
  संरचना CookieInfo *ci = curlx_calloc(1, परिमाण(संरचना CookieInfo));
  सचे(!ci)
    पटिनिवत्त रिक्त;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  अत्थाय(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], रिक्त);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  पटिनिवत्त ci;
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
स्थिर CURLcode cookie_load(संरचना Curl_easy *data, ध्रुव वर्ण *file,
                            संरचना CookieInfo *ci, पूर्णाङ्क flags)
{
  FILE *handle = रिक्त;
  CURLcode result = CURLE_OK;
  FILE *fp = रिक्त;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  सचे(file && *file) {
    सचे(!strcmp(file, "-"))
      fp = stdin;
    अञ्ञथा {
      fp = curlx_fopen(file, "rb");
      सचे(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      अञ्ञथा {
        curlx_struct_stat stat;
        सचे((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = रिक्त;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        अञ्ञथा
          handle = fp;
      }
    }
  }

  सचे(fp) {
    संरचना dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    करोहि {
      result = Curl_get_पङ्क्तिः(&buf, fp, &eof);
      सचे(!result) {
        ध्रुव वर्ण *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        सचे(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, रिक्त, रिक्त,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        सचे(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } याव(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    सचे(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  पटिनिवत्त result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(संरचना Curl_easy *data,
                               पूर्णाङ्क flags)
{
  CURLcode result = CURLE_OK;
  संरचना curl_slist *list = data->state.cookielist;
  सचे(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    सचे(!data->cookies)
      data->cookies = Curl_cookie_init();
    सचे(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    अञ्ञथा {
      data->state.cookie_engine = TRUE;
      याव(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        सचे(result)
          भङ्ग;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  पटिनिवत्त result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
स्थिर पूर्णाङ्क cookie_sort(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
  ध्रुव संरचना Cookie *c1 = *(ध्रुव संरचना Cookie * ध्रुव *)p1;
  ध्रुव संरचना Cookie *c2 = *(ध्रुव संरचना Cookie * ध्रुव *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  सचे(l1 != l2)
    पटिनिवत्त (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  सचे(l1 != l2)
    पटिनिवत्त (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  सचे(l1 != l2)
    पटिनिवत्त (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  पटिनिवत्त (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
स्थिर पूर्णाङ्क cookie_sort_ct(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
  ध्रुव संरचना Cookie *c1 = *(ध्रुव संरचना Cookie * ध्रुव *)p1;
  ध्रुव संरचना Cookie *c2 = *(ध्रुव संरचना Cookie * ध्रुव *)p2;

  पटिनिवत्त (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(संरचना Curl_easy *data, ध्रुव वर्ण *host)
{
  पटिनिवत्त Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(संरचना Curl_easy *data,
                             bool *okay,
                             ध्रुव वर्ण *host,
                             संरचना Curl_llist *list)
{
  size_t matches = 0;
  ध्रुव bool is_ip = Curl_host_is_ipnum(host);
  ध्रुव size_t myhash = cookiehash(host);
  संरचना Curl_llist_ग्रन्थिः *n;
  ध्रुव bool secure = Curl_secure_context(data, host);
  संरचना CookieInfo *ci = data->cookies;
  ध्रुव वर्ण *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, रिक्त);

  सचे(!ci || !Curl_llist_गणनम्(&ci->cookielist[myhash]))
    पटिनिवत्त CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  अत्थाय(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ग्रन्थिः_next(n)) {
    संरचना Cookie *co = Curl_ग्रन्थिः_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    सचे(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      सचे(!co->domain ||
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
        सचे(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          सचे(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            भङ्ग;
          }
        }
      }
    }
  }

  सचे(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    संरचना Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(परिमाण(संरचना Cookie *) * matches);
    सचे(!array) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ fail;
    }

    n = Curl_llist_head(list);

    अत्थाय(i = 0; n; n = Curl_ग्रन्थिः_next(n))
      array[i++] = Curl_ग्रन्थिः_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, परिमाण(संरचना Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, रिक्त);

    अत्थाय(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  पटिनिवत्त CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, रिक्त);
  पटिनिवत्त result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
शून्य Curl_cookie_clearall(संरचना CookieInfo *ci)
{
  सचे(ci) {
    unsigned पूर्णाङ्क i;
    अत्थाय(i = 0; i < COOKIE_HASH_SIZE; i++) {
      संरचना Curl_llist_ग्रन्थिः *n;
      अत्थाय(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        संरचना Cookie *c = Curl_ग्रन्थिः_elem(n);
        संरचना Curl_llist_ग्रन्थिः *e = Curl_ग्रन्थिः_next(n);
        Curl_ग्रन्थिः_remove(n);
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
शून्य Curl_cookie_clearsess(संरचना CookieInfo *ci)
{
  unsigned पूर्णाङ्क i;

  सचे(!ci)
    पटिनिवत्त;

  अत्थाय(i = 0; i < COOKIE_HASH_SIZE; i++) {
    संरचना Curl_llist_ग्रन्थिः *n = Curl_llist_head(&ci->cookielist[i]);
    संरचना Curl_llist_ग्रन्थिः *e = रिक्त;

    अत्थाय(; n; n = e) {
      संरचना Cookie *curr = Curl_ग्रन्थिः_elem(n);
      e = Curl_ग्रन्थिः_next(n); /* in case the node is removed, get it early */
      सचे(!curr->expires) {
        Curl_ग्रन्थिः_remove(n);
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
शून्य Curl_cookie_cleanup(संरचना CookieInfo *ci)
{
  सचे(ci) {
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
स्थिर वर्ण *get_netscape_format(ध्रुव संरचना Cookie *co)
{
  पटिनिवत्त curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*मानम्*/
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
    co->मानम् ? co->मानम् : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
स्थिर CURLcode cookie_output(संरचना Curl_easy *data,
                              संरचना CookieInfo *ci,
                              ध्रुव वर्ण *filename)
{
  FILE *out = रिक्त;
  bool use_stdout = FALSE;
  वर्ण *tempstore = रिक्त;
  CURLcode result = CURLE_OK;

  सचे(!ci)
    /* no cookie engine alive */
    पटिनिवत्त CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  सचे(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  अञ्ञथा {
    result = Curl_fopen(data, filename, &out, &tempstore);
    सचे(result)
      गच्छ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  सचे(ci->numcookies) {
    unsigned पूर्णाङ्क i;
    size_t nvalid = 0;
    संरचना Cookie **array;
    संरचना Curl_llist_ग्रन्थिः *n;

    array = curlx_calloc(1, परिमाण(संरचना Cookie *) * ci->numcookies);
    सचे(!array) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ error;
    }

    /* only sort the cookies with a domain property */
    अत्थाय(i = 0; i < COOKIE_HASH_SIZE; i++) {
      अत्थाय(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_ग्रन्थिः_next(n)) {
        संरचना Cookie *co = Curl_ग्रन्थिः_elem(n);
        सचे(!co->domain)
          अनुवर्त;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, परिमाण(संरचना Cookie *), cookie_sort_ct);

    अत्थाय(i = 0; i < nvalid; i++) {
      वर्ण *format_ptr = get_netscape_format(array[i]);
      सचे(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        गच्छ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  सचे(!use_stdout) {
    curlx_fclose(out);
    out = रिक्त;
    सचे(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      गच्छ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  पटिनिवत्त CURLE_OK;

error:
  सचे(out && !use_stdout)
    curlx_fclose(out);
  सचे(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  पटिनिवत्त result;
}

स्थिर संरचना curl_slist *cookie_list(ध्रुव संरचना Curl_easy *data)
{
  संरचना curl_slist *list = रिक्त;
  संरचना curl_slist *beg;
  संरचना curl_slist **tail = &list;
  unsigned पूर्णाङ्क i;
  संरचना Curl_llist_ग्रन्थिः *n;

  सचे(!data->cookies || (data->cookies->numcookies == 0))
    पटिनिवत्त रिक्त;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  अत्थाय(i = 0; i < COOKIE_HASH_SIZE; i++) {
    अत्थाय(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_ग्रन्थिः_next(n)) {
      संरचना Cookie *c = Curl_ग्रन्थिः_elem(n);
      वर्ण *पङ्क्तिः;
      सचे(!c->domain)
        अनुवर्त;
      पङ्क्तिः = get_netscape_format(c);
      सचे(!पङ्क्तिः) {
        curl_slist_free_all(list);
        पटिनिवत्त रिक्त;
      }
      beg = Curl_slist_append_nodup(रिक्त, पङ्क्तिः);
      सचे(!beg) {
        curlx_free(पङ्क्तिः);
        curl_slist_free_all(list);
        पटिनिवत्त रिक्त;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  पटिनिवत्त list;
}

संरचना curl_slist *Curl_cookie_list(संरचना Curl_easy *data)
{
  संरचना curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  पटिनिवत्त list;
}

शून्य Curl_flush_cookies(संरचना Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  सचे(data->cookies) {
    ध्रुव वर्ण *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    सचे(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      सचे(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    सचे(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = रिक्त;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

शून्य Curl_cookie_run(संरचना Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  सचे(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
