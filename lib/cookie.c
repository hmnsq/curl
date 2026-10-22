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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cookie.h"
#അന്തര്ഭാവ "psl.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "slist.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "curl_fopen.h"
#അന്തര്ഭാവ "curl_get_line.h"
#അന്തര്ഭാവ "curl_memrchr.h"
#അന്തര്ഭാവ "parsedate.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
സ്ഥിതം ശൂന്യം cap_expires(time_t now, ഘടന Cookie *co)
{
  എങ്കിൽ(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    എങ്കിൽ(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

സ്ഥിതം ശൂന്യം freecookie(ഘടന Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->മൂല്യം);
  എങ്കിൽ(maintoo)
    curlx_free(co);
}

സ്ഥിതം bool cookie_tailmatch(സ്ഥിരം അക്ഷരം *cookie_domain,
                             സ്ഥിരം size_t cookie_domain_len,
                             സ്ഥിരം അക്ഷരം *hostname)
{
  size_t hostname_len = strlen(hostname);

  എങ്കിൽ(hostname_len < cookie_domain_len)
    തിരികെ FALSE;

  എങ്കിൽ(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    തിരികെ FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  എങ്കിൽ(hostname_len == cookie_domain_len)
    തിരികെ TRUE;
  എങ്കിൽ('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    തിരികെ TRUE;
  തിരികെ FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
സ്ഥിതം bool pathmatch(സ്ഥിരം അക്ഷരം *cookie_path, സ്ഥിരം അക്ഷരം *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  എങ്കിൽ(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    തിരികെ TRUE;
  }

  /* #-fragments are already cut off! */
  എങ്കിൽ(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  എങ്കിൽ(uri_path_len < cookie_path_len)
    പോകുക pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  എങ്കിൽ(strncmp(cookie_path, uri_path, cookie_path_len))
    പോകുക pathmatched;

  /* The cookie-path and the uri-path are identical. */
  എങ്കിൽ(cookie_path_len == uri_path_len) {
    ret = TRUE;
    പോകുക pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  എങ്കിൽ(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    പോകുക pathmatched;
  }

pathmatched:
  തിരികെ ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *get_മുകൾ_domain(സ്ഥിരം അക്ഷരം * സ്ഥിരം domain, size_t *outlen)
{
  size_t len = 0;
  സ്ഥിരം അക്ഷരം *first = ശൂന്യ, *last;

  എങ്കിൽ(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    എങ്കിൽ(last) {
      first = memrchr(domain, '.', (last - domain));
      എങ്കിൽ(first)
        len -= (++first - domain);
    }
  }

  എങ്കിൽ(outlen)
    *outlen = len;

  തിരികെ first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#എങ്കിൽ defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
സ്ഥിതം size_t cookie_hash_domain(സ്ഥിരം അക്ഷരം *domain, സ്ഥിരം size_t len)
{
  സ്ഥിരം അക്ഷരം *end = domain + len;
  size_t h = 5381;

  ആകുമ്പോൾ(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  തിരികെ (h % COOKIE_HASH_SIZE);
}

#എങ്കിൽ defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
സ്ഥിതം size_t cookiehash(സ്ഥിരം അക്ഷരം * സ്ഥിരം domain)
{
  സ്ഥിരം അക്ഷരം *മുകൾ;
  size_t len;

  എങ്കിൽ(!domain || Curl_host_is_ipnum(domain))
    തിരികെ 0;

  മുകൾ = get_മുകൾ_domain(domain, &len);
  തിരികെ cookie_hash_domain(മുകൾ, len);
}

/*
 * cookie path sanitize
 */
സ്ഥിതം അക്ഷരം *sanitize_cookie_path(സ്ഥിരം അക്ഷരം *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  എങ്കിൽ(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    എങ്കിൽ(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  എങ്കിൽ(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    തിരികെ curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  എങ്കിൽ(len > 1 && cookie_path[len - 1] == '/')
    len--;

  തിരികെ curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
സ്ഥിതം CURLcode strstore(അക്ഷരം **str, സ്ഥിരം അക്ഷരം *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  എങ്കിൽ(!*str)
    തിരികെ CURLE_OUT_OF_MEMORY;
  തിരികെ CURLE_OK;
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
സ്ഥിതം ശൂന്യം remove_expired(ഘടന CookieInfo *ci)
{
  ഘടന Cookie *co;
  curl_off_t now = (curl_off_t)time(ശൂന്യ);
  unsigned പൂർണ്ണസംഖ്യ i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  എങ്കിൽ(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    തിരികെ;
  അല്ലെങ്കിൽ
    ci->next_expiration = CURL_OFF_T_MAX;

  വേണ്ടി(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ഘടന Curl_llist_കണ്ണി *n;
    ഘടന Curl_llist_കണ്ണി *e = ശൂന്യ;

    വേണ്ടി(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_കണ്ണി_elem(n);
      e = Curl_കണ്ണി_next(n);
      എങ്കിൽ(co->expires) {
        എങ്കിൽ(co->expires < now) {
          Curl_കണ്ണി_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(co->expires < ci->next_expiration)
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
സ്ഥിതം bool bad_domain(സ്ഥിരം അക്ഷരം *domain, size_t len)
{
  എങ്കിൽ((len == 9) && curl_strnequal(domain, "localhost", 9))
    തിരികെ FALSE;
  അല്ലെങ്കിൽ {
    /* there must be a dot present, but that dot must not be a trailing dot */
    സ്ഥിരം അക്ഷരം *dot = memchr(domain, '.', len);
    എങ്കിൽ(dot) {
      size_t i = dot - domain;
      എങ്കിൽ((len - i) > 1)
        /* the dot is not the last byte */
        തിരികെ FALSE;
    }
  }
  തിരികെ TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
സ്ഥിതം bool invalid_octets(സ്ഥിരം അക്ഷരം *ptr, size_t len)
{
  സ്ഥിരം unsigned അക്ഷരം *p = (സ്ഥിരം unsigned അക്ഷരം *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  ആകുമ്പോൾ(len && *p) {
    എങ്കിൽ((*p < 0x20) || (*p == 0x7f))
      തിരികെ TRUE;
    p++;
    len--;
  }
  തിരികെ FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_മൂല്യം  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

സ്ഥിതം CURLcode storecookie(ഘടന Cookie *co, സ്ഥിരം ഘടന Curl_str *cp,
                            സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  എങ്കിൽ(!result)
    result = strstore(&co->മൂല്യം, curlx_str(&cp[COOKIE_മൂല്യം]),
                      curlx_strlen(&cp[COOKIE_മൂല്യം]));
  എങ്കിൽ(!result) {
    size_t plen = 0;
    എങ്കിൽ(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(path) {
      /* No path was given in the header line, set the default */
      സ്ഥിരം അക്ഷരം *endslash = strrchr(path, '/');
      എങ്കിൽ(endslash)
        plen = endslash - path + 1; /* include end slash */
      അല്ലെങ്കിൽ
        plen = strlen(path);
    }

    എങ്കിൽ(path) {
      co->path = sanitize_cookie_path(path, plen);
      എങ്കിൽ(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  എങ്കിൽ(!result) {
    എങ്കിൽ(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    അല്ലെങ്കിൽ എങ്കിൽ(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      എങ്കിൽ(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  തിരികെ result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
സ്ഥിതം bool parse_first_pair(ഘടന Curl_easy *data, ഘടന Cookie *co,
                             ഘടന Curl_str *cookie,
                             ഘടന Curl_str *name,
                             ഘടന Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  എങ്കിൽ(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    തിരികെ FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  എങ്കിൽ((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    തിരികെ FALSE;
  }

  /* Check if we have a reserved prefix set. */
  എങ്കിൽ(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_മൂല്യം] = *val;
  തിരികെ TRUE;
}

സ്ഥിതം bool parse_domain(ഘടന Curl_easy *data, ഘടന Cookie *co,
                         ഘടന Curl_str *cookie_domain,
                         ഘടന Curl_str *val,
                         സ്ഥിരം അക്ഷരം **domainp)
{
  bool is_ip;
  സ്ഥിരം അക്ഷരം *domain = *domainp;
  സ്ഥിരം അക്ഷരം *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  എങ്കിൽ('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  എങ്കിൽ(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  എങ്കിൽ(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    എങ്കിൽ(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  അല്ലെങ്കിൽ {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    തിരികെ FALSE;
  }
  തിരികെ TRUE;
}

സ്ഥിതം ശൂന്യം parse_maxage(ഘടന Cookie *co, ഘടന Curl_str *val,
                         time_t *nowp)
{
  പൂർണ്ണസംഖ്യ rc;
  സ്ഥിരം അക്ഷരം *maxage = curlx_str(val);
  എങ്കിൽ(*maxage == '\"')
    maxage++;
  rc = curlx_str_സംഖ്യ(&maxage, &co->expires, CURL_OFF_T_MAX);
  എങ്കിൽ(!*nowp)
    *nowp = time(ശൂന്യ);
  തിരഞ്ഞെടുപ്പ്(rc) {
  അവസ്ഥ STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    നിർത്തുക;
  അവസ്ഥ STRE_OK:
    എങ്കിൽ(!co->expires)
      co->expires = 1; /* expire now */
    അല്ലെങ്കിൽ എങ്കിൽ(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    അല്ലെങ്കിൽ
      co->expires += *nowp;
    നിർത്തുക;
  }
  cap_expires(*nowp, co);
}

സ്ഥിതം ശൂന്യം parse_expires(ഘടന Cookie *co, ഘടന Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  എങ്കിൽ(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    അക്ഷരം dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    എങ്കിൽ(!Curl_getdate_capped(dbuf, &date)) {
      എങ്കിൽ(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    അല്ലെങ്കിൽ
      co->expires = 0;
    എങ്കിൽ(!*nowp)
      *nowp = time(ശൂന്യ);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
സ്ഥിതം CURLcode
parse_cookie_header(ഘടന Curl_easy *data,
                    ഘടന Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    സ്ഥിരം അക്ഷരം *ptr, /* the header */
                    സ്ഥിരം അക്ഷരം *domain, /* default domain */
                    /* full path used when this cookie is set */
                    സ്ഥിരം അക്ഷരം *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  ഘടന Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  എങ്കിൽ(linelength > MAX_COOKIE_വരി)
    /* discard overly long lines at once */
    തിരികെ CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, വലുപ്പം(cookie));
  ചെയ്യുക {
    ഘടന Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    പൂർണ്ണസംഖ്യ rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    എങ്കിൽ(!rc || (rc == STRE_SHORT)) {
      ഘടന Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      എങ്കിൽ(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        തിരികെ CURLE_OK;
      }

      എങ്കിൽ(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        എങ്കിൽ(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        എങ്കിൽ(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          തിരികെ CURLE_OK;
        }
      }
      അല്ലെങ്കിൽ
        curlx_str_init(&val);

      എങ്കിൽ(!curlx_strlen(&cookie[COOKIE_NAME])) {
        എങ്കിൽ(!parse_first_pair(data, co, cookie, &name, &val, sep))
          തിരികെ CURLE_OK;
      }

      /* whether there's separator or not */
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "secure")) {
        എങ്കിൽ(secure_origin)
          co->secure = TRUE;
        അല്ലെങ്കിൽ {
          infof(data, "skipped cookie because not 'secure'");
          തിരികെ CURLE_OK;
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      അല്ലെങ്കിൽ എങ്കിൽ(sep) {
        /* only if a separating equals sign is present */
        എങ്കിൽ(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "domain")) {
          എങ്കിൽ(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            തിരികെ CURLE_OK;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } ആകുമ്പോൾ(!curlx_str_single(&ptr, ';'));

  എങ്കിൽ(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    എങ്കിൽ(!result)
      *okay = TRUE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode parse_netscape(ഘടന Cookie *co,
                               സ്ഥിരം ഘടന CookieInfo *ci,
                               bool *okay,
                               സ്ഥിരം അക്ഷരം *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  സ്ഥിരം അക്ഷരം *ptr, *next;
  പൂർണ്ണസംഖ്യ fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  എങ്കിൽ(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  എങ്കിൽ(lineptr[0] == '#')
    /* do not even try the comments */
    തിരികെ CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  വേണ്ടി(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : ശൂന്യ);
    തിരഞ്ഞെടുപ്പ്(fields) {
    അവസ്ഥ 0:
      എങ്കിൽ(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      എങ്കിൽ(!co->domain)
        തിരികെ CURLE_OUT_OF_MEMORY;
      നിർത്തുക;
    അവസ്ഥ 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      നിർത്തുക;
    അവസ്ഥ 2:
      /* The file format allows the path field to remain not filled in */
      എങ്കിൽ(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        എങ്കിൽ(!co->path)
          തിരികെ CURLE_OUT_OF_MEMORY;
        നിർത്തുക;
      }
      അല്ലെങ്കിൽ {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        എങ്കിൽ(!co->path)
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    അവസ്ഥ 3:
      co->secure = FALSE;
      എങ്കിൽ(curl_strnequal(ptr, "TRUE", len)) {
        എങ്കിൽ(secure_origin || ci->running)
          co->secure = TRUE;
        അല്ലെങ്കിൽ
          തിരികെ CURLE_OK;
      }
      നിർത്തുക;
    അവസ്ഥ 4:
      എങ്കിൽ(curlx_str_സംഖ്യ(&ptr, &co->expires, CURL_OFF_T_MAX))
        തിരികെ CURLE_OK;
      നിർത്തുക;
    അവസ്ഥ 5:
      co->name = curlx_memdup0(ptr, len);
      എങ്കിൽ(!co->name)
        തിരികെ CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        എങ്കിൽ(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        അല്ലെങ്കിൽ എങ്കിൽ(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      നിർത്തുക;
    അവസ്ഥ 6:
      co->മൂല്യം = curlx_memdup0(ptr, len);
      എങ്കിൽ(!co->മൂല്യം)
        തിരികെ CURLE_OUT_OF_MEMORY;
      നിർത്തുക;
    }
  }
  എങ്കിൽ(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->മൂല്യം = curlx_strdup("");
    എങ്കിൽ(!co->മൂല്യം)
      തിരികെ CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ
      fields++;
  }

  എങ്കിൽ(fields != 7)
    /* we did not find the sufficient number of fields */
    തിരികെ CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  എങ്കിൽ(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->മൂല്യം, strlen(co->മൂല്യം)))
    തിരികെ CURLE_OK;

  *okay = TRUE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool is_public_suffix(ഘടന Curl_easy *data,
                             ഘടന Cookie *co,
                             സ്ഥിരം അക്ഷരം *domain)
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
  എങ്കിൽ(!co->domain || Curl_host_is_ipnum(co->domain))
    തിരികെ FALSE;

  അല്ലെങ്കിൽ {
    bool acceptable = FALSE;
    അക്ഷരം lcase[256];
    അക്ഷരം lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    എങ്കിൽ(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    എങ്കിൽ(clen && (co->domain[clen - 1] == '.'))
      clen--;

    എങ്കിൽ((dlen < വലുപ്പം(lcase)) && (clen < വലുപ്പം(lcookie))) {
      സ്ഥിരം psl_ctx_t *psl = Curl_psl_use(data);
      എങ്കിൽ(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        എങ്കിൽ(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          എങ്കിൽ(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        അല്ലെങ്കിൽ {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          എങ്കിൽ(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      അല്ലെങ്കിൽ
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    എങ്കിൽ(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      തിരികെ TRUE;
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)co;
  (ശൂന്യം)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  തിരികെ FALSE;
}

/* returns TRUE when replaced */
സ്ഥിതം bool replace_existing(ഘടന Curl_easy *data,
                             ഘടന Cookie *co,
                             സ്ഥിരം ഘടന CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ഘടന Curl_llist_കണ്ണി *replace_n = ശൂന്യ;
  ഘടന Curl_llist_കണ്ണി *n;
  size_t myhash = cookiehash(co->domain);
  വേണ്ടി(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_കണ്ണി_next(n)) {
    ഘടന Cookie *clist = Curl_കണ്ണി_elem(n);
    എങ്കിൽ(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      എങ്കിൽ(clist->domain && co->domain) {
        എങ്കിൽ(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!clist->domain && !co->domain)
        matching_domains = TRUE;

      എങ്കിൽ(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        സ്ഥിരം അക്ഷരം *sep = ശൂന്യ;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        എങ്കിൽ(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        എങ്കിൽ(sep)
          cllen = sep - clist->path;
        അല്ലെങ്കിൽ
          cllen = strlen(clist->path);

        എങ്കിൽ(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          തിരികെ FALSE;
        }
      }
    }

    എങ്കിൽ(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      എങ്കിൽ(clist->domain && co->domain) {
        എങ്കിൽ(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!clist->domain && !co->domain)
        replace_old = TRUE;

      എങ്കിൽ(replace_old) {
        /* the domains were identical */

        എങ്കിൽ(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        അല്ലെങ്കിൽ എങ്കിൽ(!clist->path != !co->path)
          replace_old = FALSE;
      }

      എങ്കിൽ(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        തിരികെ FALSE;
      }
      എങ്കിൽ(replace_old)
        replace_n = n;
    }
  }
  എങ്കിൽ(replace_n) {
    ഘടന Cookie *repl = Curl_കണ്ണി_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_കണ്ണി_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  തിരികെ TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(ഘടന Curl_easy *data,
                         ഘടന CookieInfo *ci,
                         സ്ഥിരം അക്ഷരം *lineptr, /* first character of the line */
                         സ്ഥിരം അക്ഷരം *domain,  /* default domain */
                         സ്ഥിരം അക്ഷരം *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         സ്ഥിരം പൂർണ്ണസംഖ്യ flags)
{
  ഘടന Cookie comem;
  ഘടന Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  എങ്കിൽ(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    തിരികെ CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, വലുപ്പം(comem));

  എങ്കിൽ(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  അല്ലെങ്കിൽ
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  എങ്കിൽ(result || !okay)
    പോകുക fail;

  എങ്കിൽ(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    പോകുക fail;

  എങ്കിൽ(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    പോകുക fail;

  എങ്കിൽ(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    എങ്കിൽ(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    അല്ലെങ്കിൽ
      പോകുക fail;
  }

  എങ്കിൽ(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    പോകുക fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  എങ്കിൽ(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  എങ്കിൽ(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    പോകുക fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, വലുപ്പം(comem));
  എങ്കിൽ(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    പോകുക fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->കണ്ണി);

  എങ്കിൽ(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->മൂല്യം,
          co->domain, co->path, co->expires);

  എങ്കിൽ(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  എങ്കിൽ(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  എങ്കിൽ(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  തിരികെ result;
fail:
  freecookie(co, FALSE);
  തിരികെ result;
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
ഘടന CookieInfo *Curl_cookie_init(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ i;
  ഘടന CookieInfo *ci = curlx_calloc(1, വലുപ്പം(ഘടന CookieInfo));
  എങ്കിൽ(!ci)
    തിരികെ ശൂന്യ;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  വേണ്ടി(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], ശൂന്യ);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  തിരികെ ci;
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
സ്ഥിതം CURLcode cookie_load(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *file,
                            ഘടന CookieInfo *ci, പൂർണ്ണസംഖ്യ flags)
{
  FILE *handle = ശൂന്യ;
  CURLcode result = CURLE_OK;
  FILE *fp = ശൂന്യ;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  എങ്കിൽ(file && *file) {
    എങ്കിൽ(!strcmp(file, "-"))
      fp = stdin;
    അല്ലെങ്കിൽ {
      fp = curlx_fopen(file, "rb");
      എങ്കിൽ(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      അല്ലെങ്കിൽ {
        curlx_struct_stat stat;
        എങ്കിൽ((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = ശൂന്യ;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        അല്ലെങ്കിൽ
          handle = fp;
      }
    }
  }

  എങ്കിൽ(fp) {
    ഘടന dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    ചെയ്യുക {
      result = Curl_get_വരി(&buf, fp, &eof);
      എങ്കിൽ(!result) {
        സ്ഥിരം അക്ഷരം *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        എങ്കിൽ(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, ശൂന്യ, ശൂന്യ,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        എങ്കിൽ(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } ആകുമ്പോൾ(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    എങ്കിൽ(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  തിരികെ result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ഘടന Curl_easy *data,
                               പൂർണ്ണസംഖ്യ flags)
{
  CURLcode result = CURLE_OK;
  ഘടന curl_slist *list = data->state.cookielist;
  എങ്കിൽ(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    എങ്കിൽ(!data->cookies)
      data->cookies = Curl_cookie_init();
    എങ്കിൽ(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ {
      data->state.cookie_engine = TRUE;
      ആകുമ്പോൾ(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        എങ്കിൽ(result)
          നിർത്തുക;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  തിരികെ result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ cookie_sort(സ്ഥിരം ശൂന്യം *p1, സ്ഥിരം ശൂന്യം *p2)
{
  സ്ഥിരം ഘടന Cookie *c1 = *(സ്ഥിരം ഘടന Cookie * സ്ഥിരം *)p1;
  സ്ഥിരം ഘടന Cookie *c2 = *(സ്ഥിരം ഘടന Cookie * സ്ഥിരം *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  എങ്കിൽ(l1 != l2)
    തിരികെ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  എങ്കിൽ(l1 != l2)
    തിരികെ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  എങ്കിൽ(l1 != l2)
    തിരികെ (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  തിരികെ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ cookie_sort_ct(സ്ഥിരം ശൂന്യം *p1, സ്ഥിരം ശൂന്യം *p2)
{
  സ്ഥിരം ഘടന Cookie *c1 = *(സ്ഥിരം ഘടന Cookie * സ്ഥിരം *)p1;
  സ്ഥിരം ഘടന Cookie *c2 = *(സ്ഥിരം ഘടന Cookie * സ്ഥിരം *)p2;

  തിരികെ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *host)
{
  തിരികെ Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(ഘടന Curl_easy *data,
                             bool *okay,
                             സ്ഥിരം അക്ഷരം *host,
                             ഘടന Curl_llist *list)
{
  size_t matches = 0;
  സ്ഥിരം bool is_ip = Curl_host_is_ipnum(host);
  സ്ഥിരം size_t myhash = cookiehash(host);
  ഘടന Curl_llist_കണ്ണി *n;
  സ്ഥിരം bool secure = Curl_secure_context(data, host);
  ഘടന CookieInfo *ci = data->cookies;
  സ്ഥിരം അക്ഷരം *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ശൂന്യ);

  എങ്കിൽ(!ci || !Curl_llist_എണ്ണം(&ci->cookielist[myhash]))
    തിരികെ CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  വേണ്ടി(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_കണ്ണി_next(n)) {
    ഘടന Cookie *co = Curl_കണ്ണി_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    എങ്കിൽ(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      എങ്കിൽ(!co->domain ||
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
        എങ്കിൽ(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          എങ്കിൽ(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            നിർത്തുക;
          }
        }
      }
    }
  }

  എങ്കിൽ(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    ഘടന Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(വലുപ്പം(ഘടന Cookie *) * matches);
    എങ്കിൽ(!array) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക fail;
    }

    n = Curl_llist_head(list);

    വേണ്ടി(i = 0; n; n = Curl_കണ്ണി_next(n))
      array[i++] = Curl_കണ്ണി_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, വലുപ്പം(ഘടന Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ശൂന്യ);

    വേണ്ടി(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  തിരികെ CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ശൂന്യ);
  തിരികെ result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
ശൂന്യം Curl_cookie_clearall(ഘടന CookieInfo *ci)
{
  എങ്കിൽ(ci) {
    unsigned പൂർണ്ണസംഖ്യ i;
    വേണ്ടി(i = 0; i < COOKIE_HASH_SIZE; i++) {
      ഘടന Curl_llist_കണ്ണി *n;
      വേണ്ടി(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        ഘടന Cookie *c = Curl_കണ്ണി_elem(n);
        ഘടന Curl_llist_കണ്ണി *e = Curl_കണ്ണി_next(n);
        Curl_കണ്ണി_remove(n);
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
ശൂന്യം Curl_cookie_clearsess(ഘടന CookieInfo *ci)
{
  unsigned പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ(!ci)
    തിരികെ;

  വേണ്ടി(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ഘടന Curl_llist_കണ്ണി *n = Curl_llist_head(&ci->cookielist[i]);
    ഘടന Curl_llist_കണ്ണി *e = ശൂന്യ;

    വേണ്ടി(; n; n = e) {
      ഘടന Cookie *curr = Curl_കണ്ണി_elem(n);
      e = Curl_കണ്ണി_next(n); /* in case the node is removed, get it early */
      എങ്കിൽ(!curr->expires) {
        Curl_കണ്ണി_remove(n);
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
ശൂന്യം Curl_cookie_cleanup(ഘടന CookieInfo *ci)
{
  എങ്കിൽ(ci) {
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
സ്ഥിതം അക്ഷരം *get_netscape_format(സ്ഥിരം ഘടന Cookie *co)
{
  തിരികെ curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*മൂല്യം*/
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
    co->മൂല്യം ? co->മൂല്യം : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
സ്ഥിതം CURLcode cookie_output(ഘടന Curl_easy *data,
                              ഘടന CookieInfo *ci,
                              സ്ഥിരം അക്ഷരം *filename)
{
  FILE *out = ശൂന്യ;
  bool use_stdout = FALSE;
  അക്ഷരം *tempstore = ശൂന്യ;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!ci)
    /* no cookie engine alive */
    തിരികെ CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  എങ്കിൽ(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  അല്ലെങ്കിൽ {
    result = Curl_fopen(data, filename, &out, &tempstore);
    എങ്കിൽ(result)
      പോകുക error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  എങ്കിൽ(ci->numcookies) {
    unsigned പൂർണ്ണസംഖ്യ i;
    size_t nvalid = 0;
    ഘടന Cookie **array;
    ഘടന Curl_llist_കണ്ണി *n;

    array = curlx_calloc(1, വലുപ്പം(ഘടന Cookie *) * ci->numcookies);
    എങ്കിൽ(!array) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക error;
    }

    /* only sort the cookies with a domain property */
    വേണ്ടി(i = 0; i < COOKIE_HASH_SIZE; i++) {
      വേണ്ടി(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_കണ്ണി_next(n)) {
        ഘടന Cookie *co = Curl_കണ്ണി_elem(n);
        എങ്കിൽ(!co->domain)
          തുടരുക;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, വലുപ്പം(ഘടന Cookie *), cookie_sort_ct);

    വേണ്ടി(i = 0; i < nvalid; i++) {
      അക്ഷരം *format_ptr = get_netscape_format(array[i]);
      എങ്കിൽ(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        പോകുക error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  എങ്കിൽ(!use_stdout) {
    curlx_fclose(out);
    out = ശൂന്യ;
    എങ്കിൽ(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      പോകുക error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  തിരികെ CURLE_OK;

error:
  എങ്കിൽ(out && !use_stdout)
    curlx_fclose(out);
  എങ്കിൽ(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  തിരികെ result;
}

സ്ഥിതം ഘടന curl_slist *cookie_list(സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന curl_slist *list = ശൂന്യ;
  ഘടന curl_slist *beg;
  ഘടന curl_slist **tail = &list;
  unsigned പൂർണ്ണസംഖ്യ i;
  ഘടന Curl_llist_കണ്ണി *n;

  എങ്കിൽ(!data->cookies || (data->cookies->numcookies == 0))
    തിരികെ ശൂന്യ;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  വേണ്ടി(i = 0; i < COOKIE_HASH_SIZE; i++) {
    വേണ്ടി(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_കണ്ണി_next(n)) {
      ഘടന Cookie *c = Curl_കണ്ണി_elem(n);
      അക്ഷരം *വരി;
      എങ്കിൽ(!c->domain)
        തുടരുക;
      വരി = get_netscape_format(c);
      എങ്കിൽ(!വരി) {
        curl_slist_free_all(list);
        തിരികെ ശൂന്യ;
      }
      beg = Curl_slist_append_nodup(ശൂന്യ, വരി);
      എങ്കിൽ(!beg) {
        curlx_free(വരി);
        curl_slist_free_all(list);
        തിരികെ ശൂന്യ;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  തിരികെ list;
}

ഘടന curl_slist *Curl_cookie_list(ഘടന Curl_easy *data)
{
  ഘടന curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  തിരികെ list;
}

ശൂന്യം Curl_flush_cookies(ഘടന Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  എങ്കിൽ(data->cookies) {
    സ്ഥിരം അക്ഷരം *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    എങ്കിൽ(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      എങ്കിൽ(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    എങ്കിൽ(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = ശൂന്യ;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

ശൂന്യം Curl_cookie_run(ഘടന Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  എങ്കിൽ(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
