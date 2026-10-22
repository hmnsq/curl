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

#𐡄𐡍 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

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
  𐡄𐡍(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    𐡄𐡍(co->expires > cap) {
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
  curlx_free(co->value);
  𐡄𐡍(maintoo)
    curlx_free(co);
}

ساكن bool cookie_tailmatch(ثابت حرف *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت حرف *hostname)
{
  size_t hostname_len = strlen(hostname);

  𐡄𐡍(hostname_len < cookie_domain_len)
    أرجع FALSE;

  𐡄𐡍(!curl_strnequal(cookie_domain,
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
  𐡄𐡍(hostname_len == cookie_domain_len)
    أرجع TRUE;
  𐡄𐡍('.' == *(hostname + hostname_len - cookie_domain_len - 1))
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
  𐡄𐡍(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    أرجع TRUE;
  }

  /* #-fragments are already cut off! */
  𐡄𐡍(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  𐡄𐡍(uri_path_len < cookie_path_len)
    اذهب pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  𐡄𐡍(strncmp(cookie_path, uri_path, cookie_path_len))
    اذهب pathmatched;

  /* The cookie-path and the uri-path are identical. */
  𐡄𐡍(cookie_path_len == uri_path_len) {
    ret = TRUE;
    اذهب pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  𐡄𐡍(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    اذهب pathmatched;
  }

pathmatched:
  أرجع ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ساكن ثابت حرف *get_top_domain(ثابت حرف * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت حرف *first = عدم, *last;

  𐡄𐡍(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    𐡄𐡍(last) {
      first = memrchr(domain, '.', (last - domain));
      𐡄𐡍(first)
        len -= (++first - domain);
    }
  }

  𐡄𐡍(outlen)
    *outlen = len;

  أرجع first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#𐡄𐡍 defined(_MSC_VER) && (_MSC_VER == 1900)
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

#𐡄𐡍 defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
ساكن size_t cookiehash(ثابت حرف * ثابت domain)
{
  ثابت حرف *top;
  size_t len;

  𐡄𐡍(!domain || Curl_host_is_ipnum(domain))
    أرجع 0;

  top = get_top_domain(domain, &len);
  أرجع cookie_hash_domain(top, len);
}

/*
 * cookie path sanitize
 */
ساكن حرف *sanitize_cookie_path(ثابت حرف *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  𐡄𐡍(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    𐡄𐡍(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  𐡄𐡍(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    أرجع curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  𐡄𐡍(len > 1 && cookie_path[len - 1] == '/')
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
  𐡄𐡍(!*str)
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
  𐡄𐡍(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    أرجع;
  وإلا
    ci->next_expiration = CURL_OFF_T_MAX;

  لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
    بنية Curl_llist_node *n;
    بنية Curl_llist_node *e = عدم;

    لكل(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_node_elem(n);
      e = Curl_node_next(n);
      𐡄𐡍(co->expires) {
        𐡄𐡍(co->expires < now) {
          Curl_node_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        وإلا 𐡄𐡍(co->expires < ci->next_expiration)
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
  𐡄𐡍((len == 9) && curl_strnequal(domain, "localhost", 9))
    أرجع FALSE;
  وإلا {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ثابت حرف *dot = memchr(domain, '.', len);
    𐡄𐡍(dot) {
      size_t i = dot - domain;
      𐡄𐡍((len - i) > 1)
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
    𐡄𐡍((*p < 0x20) || (*p == 0x7f))
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
#define COOKIE_VALUE  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

ساكن CURLcode storecookie(بنية Cookie *co, ثابت بنية Curl_str *cp,
                            ثابت حرف *path, ثابت حرف *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  𐡄𐡍(!result)
    result = strstore(&co->value, curlx_str(&cp[COOKIE_VALUE]),
                      curlx_strlen(&cp[COOKIE_VALUE]));
  𐡄𐡍(!result) {
    size_t plen = 0;
    𐡄𐡍(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    وإلا 𐡄𐡍(path) {
      /* No path was given in the header line, set the default */
      ثابت حرف *endslash = strrchr(path, '/');
      𐡄𐡍(endslash)
        plen = endslash - path + 1; /* include end slash */
      وإلا
        plen = strlen(path);
    }

    𐡄𐡍(path) {
      co->path = sanitize_cookie_path(path, plen);
      𐡄𐡍(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  𐡄𐡍(!result) {
    𐡄𐡍(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    وإلا 𐡄𐡍(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      𐡄𐡍(!co->domain)
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
  𐡄𐡍(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    أرجع FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  𐡄𐡍((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    أرجع FALSE;
  }

  /* Check if we have a reserved prefix set. */
  𐡄𐡍(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  وإلا 𐡄𐡍(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_VALUE] = *val;
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

  𐡄𐡍('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  𐡄𐡍(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  𐡄𐡍(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    𐡄𐡍(!is_ip)
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
  𐡄𐡍(*maxage == '\"')
    maxage++;
  rc = curlx_str_number(&maxage, &co->expires, CURL_OFF_T_MAX);
  𐡄𐡍(!*nowp)
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
    𐡄𐡍(!co->expires)
      co->expires = 1; /* expire now */
    وإلا 𐡄𐡍(CURL_OFF_T_MAX - *nowp < co->expires)
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
  𐡄𐡍(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    حرف dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    𐡄𐡍(!Curl_getdate_capped(dbuf, &date)) {
      𐡄𐡍(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    وإلا
      co->expires = 0;
    𐡄𐡍(!*nowp)
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
  𐡄𐡍(linelength > MAX_COOKIE_LINE)
    /* discard overly long lines at once */
    أرجع CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, حجم(cookie));
  𐡏𐡁𐡃 {
    بنية Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    صحيح rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    𐡄𐡍(!rc || (rc == STRE_SHORT)) {
      بنية Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      𐡄𐡍(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        أرجع CURLE_OK;
      }

      𐡄𐡍(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        𐡄𐡍(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        𐡄𐡍(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          أرجع CURLE_OK;
        }
      }
      وإلا
        curlx_str_init(&val);

      𐡄𐡍(!curlx_strlen(&cookie[COOKIE_NAME])) {
        𐡄𐡍(!parse_first_pair(data, co, cookie, &name, &val, sep))
          أرجع CURLE_OK;
      }

      /* whether there's separator or not */
      وإلا 𐡄𐡍(curlx_str_casecompare(&name, "secure")) {
        𐡄𐡍(secure_origin)
          co->secure = TRUE;
        وإلا {
          infof(data, "skipped cookie because not 'secure'");
          أرجع CURLE_OK;
        }
      }
      وإلا 𐡄𐡍(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      وإلا 𐡄𐡍(sep) {
        /* only if a separating equals sign is present */
        𐡄𐡍(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        وإلا 𐡄𐡍(curlx_str_casecompare(&name, "domain")) {
          𐡄𐡍(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            أرجع CURLE_OK;
        }
        وإلا 𐡄𐡍(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        وإلا 𐡄𐡍(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } طالما(!curlx_str_single(&ptr, ';'));

  𐡄𐡍(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    𐡄𐡍(!result)
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
  𐡄𐡍(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  𐡄𐡍(lineptr[0] == '#')
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
      𐡄𐡍(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      𐡄𐡍(!co->domain)
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
      𐡄𐡍(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        𐡄𐡍(!co->path)
          أرجع CURLE_OUT_OF_MEMORY;
        اكسر;
      }
      وإلا {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        𐡄𐡍(!co->path)
          أرجع CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    حالة 3:
      co->secure = FALSE;
      𐡄𐡍(curl_strnequal(ptr, "TRUE", len)) {
        𐡄𐡍(secure_origin || ci->running)
          co->secure = TRUE;
        وإلا
          أرجع CURLE_OK;
      }
      اكسر;
    حالة 4:
      𐡄𐡍(curlx_str_number(&ptr, &co->expires, CURL_OFF_T_MAX))
        أرجع CURLE_OK;
      اكسر;
    حالة 5:
      co->name = curlx_memdup0(ptr, len);
      𐡄𐡍(!co->name)
        أرجع CURLE_OUT_OF_MEMORY;
      وإلا {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        𐡄𐡍(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        وإلا 𐡄𐡍(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      اكسر;
    حالة 6:
      co->value = curlx_memdup0(ptr, len);
      𐡄𐡍(!co->value)
        أرجع CURLE_OUT_OF_MEMORY;
      اكسر;
    }
  }
  𐡄𐡍(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->value = curlx_strdup("");
    𐡄𐡍(!co->value)
      أرجع CURLE_OUT_OF_MEMORY;
    وإلا
      fields++;
  }

  𐡄𐡍(fields != 7)
    /* we did not find the sufficient number of fields */
    أرجع CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  𐡄𐡍(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->value, strlen(co->value)))
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
  𐡄𐡍(!co->domain || Curl_host_is_ipnum(co->domain))
    أرجع FALSE;

  وإلا {
    bool acceptable = FALSE;
    حرف lcase[256];
    حرف lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    𐡄𐡍(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    𐡄𐡍(clen && (co->domain[clen - 1] == '.'))
      clen--;

    𐡄𐡍((dlen < حجم(lcase)) && (clen < حجم(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(data);
      𐡄𐡍(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        𐡄𐡍(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          𐡄𐡍(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        وإلا {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          𐡄𐡍(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      وإلا
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    𐡄𐡍(!acceptable) {
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
  بنية Curl_llist_node *replace_n = عدم;
  بنية Curl_llist_node *n;
  size_t myhash = cookiehash(co->domain);
  لكل(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_node_next(n)) {
    بنية Cookie *clist = Curl_node_elem(n);
    𐡄𐡍(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      𐡄𐡍(clist->domain && co->domain) {
        𐡄𐡍(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      وإلا 𐡄𐡍(!clist->domain && !co->domain)
        matching_domains = TRUE;

      𐡄𐡍(matching_domains && /* the domains were identical */
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
        𐡄𐡍(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        𐡄𐡍(sep)
          cllen = sep - clist->path;
        وإلا
          cllen = strlen(clist->path);

        𐡄𐡍(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          أرجع FALSE;
        }
      }
    }

    𐡄𐡍(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      𐡄𐡍(clist->domain && co->domain) {
        𐡄𐡍(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      وإلا 𐡄𐡍(!clist->domain && !co->domain)
        replace_old = TRUE;

      𐡄𐡍(replace_old) {
        /* the domains were identical */

        𐡄𐡍(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        وإلا 𐡄𐡍(!clist->path != !co->path)
          replace_old = FALSE;
      }

      𐡄𐡍(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        أرجع FALSE;
      }
      𐡄𐡍(replace_old)
        replace_n = n;
    }
  }
  𐡄𐡍(replace_n) {
    بنية Cookie *repl = Curl_node_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_node_remove(replace_n);

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
  𐡄𐡍(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    أرجع CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, حجم(comem));

  𐡄𐡍(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  وإلا
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  𐡄𐡍(result || !okay)
    اذهب fail;

  𐡄𐡍(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    اذهب fail;

  𐡄𐡍(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    اذهب fail;

  𐡄𐡍(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    𐡄𐡍(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    وإلا
      اذهب fail;
  }

  𐡄𐡍(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    اذهب fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  𐡄𐡍(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  𐡄𐡍(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    اذهب fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, حجم(comem));
  𐡄𐡍(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    اذهب fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->node);

  𐡄𐡍(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->value,
          co->domain, co->path, co->expires);

  𐡄𐡍(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  𐡄𐡍(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  𐡄𐡍(flags & COOKIE_HTTPHEADER)
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
  𐡄𐡍(!ci)
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

  𐡄𐡍(file && *file) {
    𐡄𐡍(!strcmp(file, "-"))
      fp = stdin;
    وإلا {
      fp = curlx_fopen(file, "rb");
      𐡄𐡍(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      وإلا {
        curlx_struct_stat stat;
        𐡄𐡍((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
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

  𐡄𐡍(fp) {
    بنية dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    𐡏𐡁𐡃 {
      result = Curl_get_line(&buf, fp, &eof);
      𐡄𐡍(!result) {
        ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        𐡄𐡍(checkprefix("Set-Cookie:", lineptr)) {
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
        𐡄𐡍(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } طالما(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    𐡄𐡍(handle)
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
  𐡄𐡍(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    𐡄𐡍(!data->cookies)
      data->cookies = Curl_cookie_init();
    𐡄𐡍(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    وإلا {
      data->state.cookie_engine = TRUE;
      طالما(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        𐡄𐡍(result)
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

  𐡄𐡍(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  𐡄𐡍(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  𐡄𐡍(l1 != l2)
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
  بنية Curl_llist_node *n;
  ثابت bool secure = Curl_secure_context(data, host);
  بنية CookieInfo *ci = data->cookies;
  ثابت حرف *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, عدم);

  𐡄𐡍(!ci || !Curl_llist_count(&ci->cookielist[myhash]))
    أرجع CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  لكل(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_node_next(n)) {
    بنية Cookie *co = Curl_node_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    𐡄𐡍(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      𐡄𐡍(!co->domain ||
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
        𐡄𐡍(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          𐡄𐡍(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            اكسر;
          }
        }
      }
    }
  }

  𐡄𐡍(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    بنية Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(حجم(بنية Cookie *) * matches);
    𐡄𐡍(!array) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب fail;
    }

    n = Curl_llist_head(list);

    لكل(i = 0; n; n = Curl_node_next(n))
      array[i++] = Curl_node_elem(n);

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
  𐡄𐡍(ci) {
    unsigned صحيح i;
    لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
      بنية Curl_llist_node *n;
      لكل(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        بنية Cookie *c = Curl_node_elem(n);
        بنية Curl_llist_node *e = Curl_node_next(n);
        Curl_node_remove(n);
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

  𐡄𐡍(!ci)
    أرجع;

  لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
    بنية Curl_llist_node *n = Curl_llist_head(&ci->cookielist[i]);
    بنية Curl_llist_node *e = عدم;

    لكل(; n; n = e) {
      بنية Cookie *curr = Curl_node_elem(n);
      e = Curl_node_next(n); /* in case the node is removed, get it early */
      𐡄𐡍(!curr->expires) {
        Curl_node_remove(n);
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
  𐡄𐡍(ci) {
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
    "%s",              /* value */
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
    co->value ? co->value : "");
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

  𐡄𐡍(!ci)
    /* no cookie engine alive */
    أرجع CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  𐡄𐡍(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  وإلا {
    result = Curl_fopen(data, filename, &out, &tempstore);
    𐡄𐡍(result)
      اذهب error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  𐡄𐡍(ci->numcookies) {
    unsigned صحيح i;
    size_t nvalid = 0;
    بنية Cookie **array;
    بنية Curl_llist_node *n;

    array = curlx_calloc(1, حجم(بنية Cookie *) * ci->numcookies);
    𐡄𐡍(!array) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب error;
    }

    /* only sort the cookies with a domain property */
    لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
      لكل(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_node_next(n)) {
        بنية Cookie *co = Curl_node_elem(n);
        𐡄𐡍(!co->domain)
          استمر;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, حجم(بنية Cookie *), cookie_sort_ct);

    لكل(i = 0; i < nvalid; i++) {
      حرف *format_ptr = get_netscape_format(array[i]);
      𐡄𐡍(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        اذهب error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  𐡄𐡍(!use_stdout) {
    curlx_fclose(out);
    out = عدم;
    𐡄𐡍(tempstore && curlx_rename(tempstore, filename)) {
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
  𐡄𐡍(out && !use_stdout)
    curlx_fclose(out);
  𐡄𐡍(tempstore) {
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
  بنية Curl_llist_node *n;

  𐡄𐡍(!data->cookies || (data->cookies->numcookies == 0))
    أرجع عدم;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  لكل(i = 0; i < COOKIE_HASH_SIZE; i++) {
    لكل(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_node_next(n)) {
      بنية Cookie *c = Curl_node_elem(n);
      حرف *line;
      𐡄𐡍(!c->domain)
        استمر;
      line = get_netscape_format(c);
      𐡄𐡍(!line) {
        curl_slist_free_all(list);
        أرجع عدم;
      }
      beg = Curl_slist_append_nodup(عدم, line);
      𐡄𐡍(!beg) {
        curlx_free(line);
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
  𐡄𐡍(data->cookies) {
    ثابت حرف *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    𐡄𐡍(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      𐡄𐡍(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    𐡄𐡍(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = عدم;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

فراغ Curl_cookie_run(بنية Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  𐡄𐡍(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
