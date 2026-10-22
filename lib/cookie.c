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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#ضمن "urldata.h"
#ضمن "cookie.h"
#ضمن "psl.h"
#ضمن "curl_trc.h"
#ضمن "transfer.h"
#ضمن "slist.h"
#ضمن "curl_share.h"
#ضمن "strcase.h"
#ضمن "curl_fopen.h"
#ضمن "curl_get_line.h"
#ضمن "curl_memrchr.h"
#ضمن "parsedate.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/dynbuf.h"
#ضمن "llist.h"
#ضمن "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ساكن فراغ cap_expires(time_t now, بنية Cookie *co)
{
  إذا(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    إذا(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ساكن فراغ freecookie(بنية Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->بەها);
  إذا(maintoo)
    curlx_free(co);
}

ساكن bool cookie_tailmatch(ثابت حرف *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت حرف *hostname)
{
  size_t hostname_len = strlen(hostname);

  إذا(hostname_len < cookie_domain_len)
    أرجع FALSE;

  إذا(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    أرجع FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  إذا(hostname_len == cookie_domain_len)
    أرجع TRUE;
  إذا('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    أرجع TRUE;
  أرجع FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ساكن bool pathmatch(ثابت حرف *cookie_path, ثابت حرف *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  إذا(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    أرجع TRUE;
  }

  /* #-fragments are already cut off! */
  إذا(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  إذا(uri_path_len < cookie_path_len)
    اذهب pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  إذا(strncmp(cookie_path, uri_path, cookie_path_len))
    اذهب pathmatched;

  /* The cookie-path and the uri-path are identical. */
  إذا(cookie_path_len == uri_path_len) {
    ret = TRUE;
    اذهب pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  إذا(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    اذهب pathmatched;
  }

pathmatched:
  أرجع ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ساكن ثابت حرف *get_لووتکە_domain(ثابت حرف * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت حرف *first = عدم, *last;

  إذا(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    إذا(last) {
      first = memrchr(domain, '.', (last - domain));
      إذا(first)
        len -= (++first - domain);
    }
  }

  إذا(outlen)
    *outlen = len;

  أرجع first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
ساكن size_t cookie_hash_domain(ثابت حرف *domain, ثابت size_t len)
{
  ثابت حرف *end = domain + len;
  size_t h = 5381;

  طالما(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  أرجع (h % COOKIE_HASH_SIZE);
}

#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
ساكن size_t cookiehash(ثابت حرف * ثابت domain)
{
  ثابت حرف *لووتکە;
  size_t len;

  إذا(!domain || Curl_host_is_ipnum(domain))
    أرجع 0;

  لووتکە = get_لووتکە_domain(domain, &len);
  أرجع cookie_hash_domain(لووتکە, len);
}

/*
 * cookie path sanitize
 */
ساكن حرف *sanitize_cookie_path(ثابت حرف *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  إذا(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    إذا(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  إذا(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    أرجع curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  إذا(len > 1 && cookie_path[len - 1] == '/')
    len--;

  أرجع curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ساكن CURLcode strstore(حرف **str, ثابت حرف *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  إذا(!*str)
    أرجع CURLE_OUT_OF_MEMORY;
  أرجع CURLE_OK;
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
ساكن فراغ remove_expired(بنية CookieInfo *ci)
{
  بنية Cookie *co;
  curl_off_t now = (curl_off_t)time(عدم);
  unsigned صحيح i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  إذا(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    أرجع;
  وإلا
    ci->next_expiration = CURL_OFF_T_MAX;

  لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
    بنية Curl_llist_گرێ *n;
    بنية Curl_llist_گرێ *e = عدم;

    لكل(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_گرێ_elem(n);
      e = Curl_گرێ_next(n);
      إذا(co->expires) {
        إذا(co->expires < now) {
          Curl_گرێ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        وإلا إذا(co->expires < ci->next_expiration)
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
ساكن bool bad_domain(ثابت حرف *domain, size_t len)
{
  إذا((len == 9) && curl_strnequal(domain, "localhost", 9))
    أرجع FALSE;
  وإلا {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ثابت حرف *dot = memchr(domain, '.', len);
    إذا(dot) {
      size_t i = dot - domain;
      إذا((len - i) > 1)
        /* the dot is not the last byte */
        أرجع FALSE;
    }
  }
  أرجع TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ساكن bool invalid_octets(ثابت حرف *ptr, size_t len)
{
  ثابت unsigned حرف *p = (ثابت unsigned حرف *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  طالما(len && *p) {
    إذا((*p < 0x20) || (*p == 0x7f))
      أرجع TRUE;
    p++;
    len--;
  }
  أرجع FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_بەها  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

ساكن CURLcode storecookie(بنية Cookie *co, ثابت بنية Curl_str *cp,
                            ثابت حرف *path, ثابت حرف *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  إذا(!result)
    result = strstore(&co->بەها, curlx_str(&cp[COOKIE_بەها]),
                      curlx_strlen(&cp[COOKIE_بەها]));
  إذا(!result) {
    size_t plen = 0;
    إذا(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    وإلا إذا(path) {
      /* No path was given in the header line, set the default */
      ثابت حرف *endslash = strrchr(path, '/');
      إذا(endslash)
        plen = endslash - path + 1; /* include end slash */
      وإلا
        plen = strlen(path);
    }

    إذا(path) {
      co->path = sanitize_cookie_path(path, plen);
      إذا(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  إذا(!result) {
    إذا(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    وإلا إذا(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      إذا(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  أرجع result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ساكن bool parse_first_pair(بنية Curl_easy *data, بنية Cookie *co,
                             بنية Curl_str *cookie,
                             بنية Curl_str *name,
                             بنية Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  إذا(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    أرجع FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  إذا((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    أرجع FALSE;
  }

  /* Check if we have a reserved prefix set. */
  إذا(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  وإلا إذا(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_بەها] = *val;
  أرجع TRUE;
}

ساكن bool parse_domain(بنية Curl_easy *data, بنية Cookie *co,
                         بنية Curl_str *cookie_domain,
                         بنية Curl_str *val,
                         ثابت حرف **domainp)
{
  bool is_ip;
  ثابت حرف *domain = *domainp;
  ثابت حرف *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  إذا('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  إذا(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  إذا(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    إذا(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  وإلا {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن فراغ parse_maxage(بنية Cookie *co, بنية Curl_str *val,
                         time_t *nowp)
{
  صحيح rc;
  ثابت حرف *maxage = curlx_str(val);
  إذا(*maxage == '\"')
    maxage++;
  rc = curlx_str_ژمار(&maxage, &co->expires, CURL_OFF_T_MAX);
  إذا(!*nowp)
    *nowp = time(عدم);
  بدل(rc) {
  حالة STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    اكسر;
  افتراضي:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    اكسر;
  حالة STRE_OK:
    إذا(!co->expires)
      co->expires = 1; /* expire now */
    وإلا إذا(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    وإلا
      co->expires += *nowp;
    اكسر;
  }
  cap_expires(*nowp, co);
}

ساكن فراغ parse_expires(بنية Cookie *co, بنية Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  إذا(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    حرف dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    إذا(!Curl_getdate_capped(dbuf, &date)) {
      إذا(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    وإلا
      co->expires = 0;
    إذا(!*nowp)
      *nowp = time(عدم);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ساكن CURLcode
parse_cookie_header(بنية Curl_easy *data,
                    بنية Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ثابت حرف *ptr, /* the header */
                    ثابت حرف *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ثابت حرف *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  بنية Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  إذا(linelength > MAX_COOKIE_هێڵ)
    /* discard overly long lines at once */
    أرجع CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, حجم(cookie));
  افعل {
    بنية Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    صحيح rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    إذا(!rc || (rc == STRE_SHORT)) {
      بنية Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      إذا(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        أرجع CURLE_OK;
      }

      إذا(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        إذا(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        إذا(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          أرجع CURLE_OK;
        }
      }
      وإلا
        curlx_str_init(&val);

      إذا(!curlx_strlen(&cookie[COOKIE_NAME])) {
        إذا(!parse_first_pair(data, co, cookie, &name, &val, sep))
          أرجع CURLE_OK;
      }

      /* whether there's separator or not */
      وإلا إذا(curlx_str_casecompare(&name, "secure")) {
        إذا(secure_origin)
          co->secure = TRUE;
        وإلا {
          infof(data, "skipped cookie because not 'secure'");
          أرجع CURLE_OK;
        }
      }
      وإلا إذا(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      وإلا إذا(sep) {
        /* only if a separating equals sign is present */
        إذا(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        وإلا إذا(curlx_str_casecompare(&name, "domain")) {
          إذا(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            أرجع CURLE_OK;
        }
        وإلا إذا(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        وإلا إذا(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } طالما(!curlx_str_single(&ptr, ';'));

  إذا(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    إذا(!result)
      *okay = TRUE;
  }
  أرجع result;
}

ساكن CURLcode parse_netscape(بنية Cookie *co,
                               ثابت بنية CookieInfo *ci,
                               bool *okay,
                               ثابت حرف *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ثابت حرف *ptr, *next;
  صحيح fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  إذا(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  إذا(lineptr[0] == '#')
    /* do not even try the comments */
    أرجع CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  لكل(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : عدم);
    بدل(fields) {
    حالة 0:
      إذا(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      إذا(!co->domain)
        أرجع CURLE_OUT_OF_MEMORY;
      اكسر;
    حالة 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      اكسر;
    حالة 2:
      /* The file format allows the path field to remain not filled in */
      إذا(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        إذا(!co->path)
          أرجع CURLE_OUT_OF_MEMORY;
        اكسر;
      }
      وإلا {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        إذا(!co->path)
          أرجع CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    حالة 3:
      co->secure = FALSE;
      إذا(curl_strnequal(ptr, "TRUE", len)) {
        إذا(secure_origin || ci->running)
          co->secure = TRUE;
        وإلا
          أرجع CURLE_OK;
      }
      اكسر;
    حالة 4:
      إذا(curlx_str_ژمار(&ptr, &co->expires, CURL_OFF_T_MAX))
        أرجع CURLE_OK;
      اكسر;
    حالة 5:
      co->name = curlx_memdup0(ptr, len);
      إذا(!co->name)
        أرجع CURLE_OUT_OF_MEMORY;
      وإلا {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        إذا(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        وإلا إذا(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      اكسر;
    حالة 6:
      co->بەها = curlx_memdup0(ptr, len);
      إذا(!co->بەها)
        أرجع CURLE_OUT_OF_MEMORY;
      اكسر;
    }
  }
  إذا(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->بەها = curlx_strdup("");
    إذا(!co->بەها)
      أرجع CURLE_OUT_OF_MEMORY;
    وإلا
      fields++;
  }

  إذا(fields != 7)
    /* we did not find the sufficient number of fields */
    أرجع CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  إذا(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->بەها, strlen(co->بەها)))
    أرجع CURLE_OK;

  *okay = TRUE;
  أرجع CURLE_OK;
}

ساكن bool is_public_suffix(بنية Curl_easy *data,
                             بنية Cookie *co,
                             ثابت حرف *domain)
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
  إذا(!co->domain || Curl_host_is_ipnum(co->domain))
    أرجع FALSE;

  وإلا {
    bool acceptable = FALSE;
    حرف lcase[256];
    حرف lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    إذا(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    إذا(clen && (co->domain[clen - 1] == '.'))
      clen--;

    إذا((dlen < حجم(lcase)) && (clen < حجم(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(data);
      إذا(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        إذا(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          إذا(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        وإلا {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          إذا(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      وإلا
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    إذا(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      أرجع TRUE;
    }
  }
#وإلا
  (فراغ)data;
  (فراغ)co;
  (فراغ)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  أرجع FALSE;
}

/* returns TRUE when replaced */
ساكن bool replace_existing(بنية Curl_easy *data,
                             بنية Cookie *co,
                             ثابت بنية CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  بنية Curl_llist_گرێ *replace_n = عدم;
  بنية Curl_llist_گرێ *n;
  size_t myhash = cookiehash(co->domain);
  لكل(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_گرێ_next(n)) {
    بنية Cookie *clist = Curl_گرێ_elem(n);
    إذا(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      إذا(clist->domain && co->domain) {
        إذا(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      وإلا إذا(!clist->domain && !co->domain)
        matching_domains = TRUE;

      إذا(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ثابت حرف *sep = عدم;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        إذا(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        إذا(sep)
          cllen = sep - clist->path;
        وإلا
          cllen = strlen(clist->path);

        إذا(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          أرجع FALSE;
        }
      }
    }

    إذا(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      إذا(clist->domain && co->domain) {
        إذا(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      وإلا إذا(!clist->domain && !co->domain)
        replace_old = TRUE;

      إذا(replace_old) {
        /* the domains were identical */

        إذا(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        وإلا إذا(!clist->path != !co->path)
          replace_old = FALSE;
      }

      إذا(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        أرجع FALSE;
      }
      إذا(replace_old)
        replace_n = n;
    }
  }
  إذا(replace_n) {
    بنية Cookie *repl = Curl_گرێ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_گرێ_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  أرجع TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(بنية Curl_easy *data,
                         بنية CookieInfo *ci,
                         ثابت حرف *lineptr, /* first character of the line */
                         ثابت حرف *domain,  /* default domain */
                         ثابت حرف *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ثابت صحيح flags)
{
  بنية Cookie comem;
  بنية Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  إذا(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    أرجع CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, حجم(comem));

  إذا(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  وإلا
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  إذا(result || !okay)
    اذهب fail;

  إذا(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    اذهب fail;

  إذا(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    اذهب fail;

  إذا(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    إذا(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    وإلا
      اذهب fail;
  }

  إذا(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    اذهب fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  إذا(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  إذا(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    اذهب fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, حجم(comem));
  إذا(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->گرێ);

  إذا(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->بەها,
          co->domain, co->path, co->expires);

  إذا(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  إذا(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  إذا(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  أرجع result;
fail:
  freecookie(co, FALSE);
  أرجع result;
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
بنية CookieInfo *Curl_cookie_init(فراغ)
{
  صحيح i;
  بنية CookieInfo *ci = curlx_calloc(1, حجم(بنية CookieInfo));
  إذا(!ci)
    أرجع عدم;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  لكل(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], عدم);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  أرجع ci;
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
ساكن CURLcode cookie_load(بنية Curl_easy *data, ثابت حرف *file,
                            بنية CookieInfo *ci, صحيح flags)
{
  FILE *handle = عدم;
  CURLcode result = CURLE_OK;
  FILE *fp = عدم;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  إذا(file && *file) {
    إذا(!strcmp(file, "-"))
      fp = stdin;
    وإلا {
      fp = curlx_fopen(file, "rb");
      إذا(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      وإلا {
        curlx_struct_stat stat;
        إذا((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = عدم;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        وإلا
          handle = fp;
      }
    }
  }

  إذا(fp) {
    بنية dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    افعل {
      result = Curl_get_هێڵ(&buf, fp, &eof);
      إذا(!result) {
        ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        إذا(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, عدم, عدم,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        إذا(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } طالما(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    إذا(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  أرجع result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(بنية Curl_easy *data,
                               صحيح flags)
{
  CURLcode result = CURLE_OK;
  بنية curl_slist *list = data->state.cookielist;
  إذا(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    إذا(!data->cookies)
      data->cookies = Curl_cookie_init();
    إذا(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    وإلا {
      data->state.cookie_engine = TRUE;
      طالما(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        إذا(result)
          اكسر;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  أرجع result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ساكن صحيح cookie_sort(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  أرجع (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ساكن صحيح cookie_sort_ct(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;

  أرجع (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(بنية Curl_easy *data, ثابت حرف *host)
{
  أرجع Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(بنية Curl_easy *data,
                             bool *okay,
                             ثابت حرف *host,
                             بنية Curl_llist *list)
{
  size_t matches = 0;
  ثابت bool is_ip = Curl_host_is_ipnum(host);
  ثابت size_t myhash = cookiehash(host);
  بنية Curl_llist_گرێ *n;
  ثابت bool secure = Curl_secure_context(data, host);
  بنية CookieInfo *ci = data->cookies;
  ثابت حرف *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, عدم);

  إذا(!ci || !Curl_llist_ژماردن(&ci->cookielist[myhash]))
    أرجع CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  لكل(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_گرێ_next(n)) {
    بنية Cookie *co = Curl_گرێ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    إذا(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      إذا(!co->domain ||
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
        إذا(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          إذا(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            اكسر;
          }
        }
      }
    }
  }

  إذا(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    بنية Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(حجم(بنية Cookie *) * matches);
    إذا(!array) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب fail;
    }

    n = Curl_llist_head(list);

    لكل(i = 0; n; n = Curl_گرێ_next(n))
      array[i++] = Curl_گرێ_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, حجم(بنية Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, عدم);

    لكل(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  أرجع CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, عدم);
  أرجع result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
فراغ Curl_cookie_clearall(بنية CookieInfo *ci)
{
  إذا(ci) {
    unsigned صحيح i;
    لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
      بنية Curl_llist_گرێ *n;
      لكل(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        بنية Cookie *c = Curl_گرێ_elem(n);
        بنية Curl_llist_گرێ *e = Curl_گرێ_next(n);
        Curl_گرێ_remove(n);
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
فراغ Curl_cookie_clearsess(بنية CookieInfo *ci)
{
  unsigned صحيح i;

  إذا(!ci)
    أرجع;

  لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
    بنية Curl_llist_گرێ *n = Curl_llist_head(&ci->cookielist[i]);
    بنية Curl_llist_گرێ *e = عدم;

    لكل(; n; n = e) {
      بنية Cookie *curr = Curl_گرێ_elem(n);
      e = Curl_گرێ_next(n); /* in case the node is removed, get it early */
      إذا(!curr->expires) {
        Curl_گرێ_remove(n);
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
فراغ Curl_cookie_cleanup(بنية CookieInfo *ci)
{
  إذا(ci) {
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
ساكن حرف *get_netscape_format(ثابت بنية Cookie *co)
{
  أرجع curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*بەها*/
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
    co->بەها ? co->بەها : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ساكن CURLcode cookie_output(بنية Curl_easy *data,
                              بنية CookieInfo *ci,
                              ثابت حرف *filename)
{
  FILE *out = عدم;
  bool use_stdout = FALSE;
  حرف *tempstore = عدم;
  CURLcode result = CURLE_OK;

  إذا(!ci)
    /* no cookie engine alive */
    أرجع CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  إذا(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  وإلا {
    result = Curl_fopen(data, filename, &out, &tempstore);
    إذا(result)
      اذهب error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  إذا(ci->numcookies) {
    unsigned صحيح i;
    size_t nvalid = 0;
    بنية Cookie **array;
    بنية Curl_llist_گرێ *n;

    array = curlx_calloc(1, حجم(بنية Cookie *) * ci->numcookies);
    إذا(!array) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب error;
    }

    /* only sort the cookies with a domain property */
    لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
      لكل(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_گرێ_next(n)) {
        بنية Cookie *co = Curl_گرێ_elem(n);
        إذا(!co->domain)
          استمر;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, حجم(بنية Cookie *), cookie_sort_ct);

    لكل(i = 0; i < nvalid; i++) {
      حرف *format_ptr = get_netscape_format(array[i]);
      إذا(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        اذهب error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  إذا(!use_stdout) {
    curlx_fclose(out);
    out = عدم;
    إذا(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      اذهب error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  أرجع CURLE_OK;

error:
  إذا(out && !use_stdout)
    curlx_fclose(out);
  إذا(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  أرجع result;
}

ساكن بنية curl_slist *cookie_list(ثابت بنية Curl_easy *data)
{
  بنية curl_slist *list = عدم;
  بنية curl_slist *beg;
  بنية curl_slist **tail = &list;
  unsigned صحيح i;
  بنية Curl_llist_گرێ *n;

  إذا(!data->cookies || (data->cookies->numcookies == 0))
    أرجع عدم;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
    لكل(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_گرێ_next(n)) {
      بنية Cookie *c = Curl_گرێ_elem(n);
      حرف *هێڵ;
      إذا(!c->domain)
        استمر;
      هێڵ = get_netscape_format(c);
      إذا(!هێڵ) {
        curl_slist_free_all(list);
        أرجع عدم;
      }
      beg = Curl_slist_append_nodup(عدم, هێڵ);
      إذا(!beg) {
        curlx_free(هێڵ);
        curl_slist_free_all(list);
        أرجع عدم;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  أرجع list;
}

بنية curl_slist *Curl_cookie_list(بنية Curl_easy *data)
{
  بنية curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  أرجع list;
}

فراغ Curl_flush_cookies(بنية Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  إذا(data->cookies) {
    ثابت حرف *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    إذا(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      إذا(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    إذا(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = عدم;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

فراغ Curl_cookie_run(بنية Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  إذا(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
