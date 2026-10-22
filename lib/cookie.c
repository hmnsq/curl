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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cookie.h"
#அந்தர்பாவ "psl.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "slist.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "curl_fopen.h"
#அந்தர்பாவ "curl_get_line.h"
#அந்தர்பாவ "curl_memrchr.h"
#அந்தர்பாவ "parsedate.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
நிலையான வெறுமை cap_expires(time_t now, கட்டமைப்பு Cookie *co)
{
  எனில்(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    எனில்(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

நிலையான வெறுமை freecookie(கட்டமைப்பு Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->மதிப்பு);
  எனில்(maintoo)
    curlx_free(co);
}

நிலையான bool cookie_tailmatch(மாறிலி எழுத்து *cookie_domain,
                             மாறிலி size_t cookie_domain_len,
                             மாறிலி எழுத்து *hostname)
{
  size_t hostname_len = strlen(hostname);

  எனில்(hostname_len < cookie_domain_len)
    திரும்பு FALSE;

  எனில்(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    திரும்பு FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  எனில்(hostname_len == cookie_domain_len)
    திரும்பு TRUE;
  எனில்('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    திரும்பு TRUE;
  திரும்பு FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
நிலையான bool pathmatch(மாறிலி எழுத்து *cookie_path, மாறிலி எழுத்து *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  எனில்(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    திரும்பு TRUE;
  }

  /* #-fragments are already cut off! */
  எனில்(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  எனில்(uri_path_len < cookie_path_len)
    செல் pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  எனில்(strncmp(cookie_path, uri_path, cookie_path_len))
    செல் pathmatched;

  /* The cookie-path and the uri-path are identical. */
  எனில்(cookie_path_len == uri_path_len) {
    ret = TRUE;
    செல் pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  எனில்(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    செல் pathmatched;
  }

pathmatched:
  திரும்பு ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
நிலையான மாறிலி எழுத்து *get_உச்சி_domain(மாறிலி எழுத்து * மாறிலி domain, size_t *outlen)
{
  size_t len = 0;
  மாறிலி எழுத்து *first = வெற்று, *last;

  எனில்(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    எனில்(last) {
      first = memrchr(domain, '.', (last - domain));
      எனில்(first)
        len -= (++first - domain);
    }
  }

  எனில்(outlen)
    *outlen = len;

  திரும்பு first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#எனில் defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
நிலையான size_t cookie_hash_domain(மாறிலி எழுத்து *domain, மாறிலி size_t len)
{
  மாறிலி எழுத்து *end = domain + len;
  size_t h = 5381;

  போது(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  திரும்பு (h % COOKIE_HASH_SIZE);
}

#எனில் defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
நிலையான size_t cookiehash(மாறிலி எழுத்து * மாறிலி domain)
{
  மாறிலி எழுத்து *உச்சி;
  size_t len;

  எனில்(!domain || Curl_host_is_ipnum(domain))
    திரும்பு 0;

  உச்சி = get_உச்சி_domain(domain, &len);
  திரும்பு cookie_hash_domain(உச்சி, len);
}

/*
 * cookie path sanitize
 */
நிலையான எழுத்து *sanitize_cookie_path(மாறிலி எழுத்து *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  எனில்(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    எனில்(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  எனில்(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    திரும்பு curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  எனில்(len > 1 && cookie_path[len - 1] == '/')
    len--;

  திரும்பு curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
நிலையான CURLcode strstore(எழுத்து **str, மாறிலி எழுத்து *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  எனில்(!*str)
    திரும்பு CURLE_OUT_OF_MEMORY;
  திரும்பு CURLE_OK;
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
நிலையான வெறுமை remove_expired(கட்டமைப்பு CookieInfo *ci)
{
  கட்டமைப்பு Cookie *co;
  curl_off_t now = (curl_off_t)time(வெற்று);
  unsigned முழு i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  எனில்(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    திரும்பு;
  இல்லையெனில்
    ci->next_expiration = CURL_OFF_T_MAX;

  க்கு(i = 0; i < COOKIE_HASH_SIZE; i++) {
    கட்டமைப்பு Curl_llist_முனை *n;
    கட்டமைப்பு Curl_llist_முனை *e = வெற்று;

    க்கு(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_முனை_elem(n);
      e = Curl_முனை_next(n);
      எனில்(co->expires) {
        எனில்(co->expires < now) {
          Curl_முனை_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        இல்லையெனில் எனில்(co->expires < ci->next_expiration)
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
நிலையான bool bad_domain(மாறிலி எழுத்து *domain, size_t len)
{
  எனில்((len == 9) && curl_strnequal(domain, "localhost", 9))
    திரும்பு FALSE;
  இல்லையெனில் {
    /* there must be a dot present, but that dot must not be a trailing dot */
    மாறிலி எழுத்து *dot = memchr(domain, '.', len);
    எனில்(dot) {
      size_t i = dot - domain;
      எனில்((len - i) > 1)
        /* the dot is not the last byte */
        திரும்பு FALSE;
    }
  }
  திரும்பு TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
நிலையான bool invalid_octets(மாறிலி எழுத்து *ptr, size_t len)
{
  மாறிலி unsigned எழுத்து *p = (மாறிலி unsigned எழுத்து *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  போது(len && *p) {
    எனில்((*p < 0x20) || (*p == 0x7f))
      திரும்பு TRUE;
    p++;
    len--;
  }
  திரும்பு FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_மதிப்பு  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

நிலையான CURLcode storecookie(கட்டமைப்பு Cookie *co, மாறிலி கட்டமைப்பு Curl_str *cp,
                            மாறிலி எழுத்து *path, மாறிலி எழுத்து *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  எனில்(!result)
    result = strstore(&co->மதிப்பு, curlx_str(&cp[COOKIE_மதிப்பு]),
                      curlx_strlen(&cp[COOKIE_மதிப்பு]));
  எனில்(!result) {
    size_t plen = 0;
    எனில்(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    இல்லையெனில் எனில்(path) {
      /* No path was given in the header line, set the default */
      மாறிலி எழுத்து *endslash = strrchr(path, '/');
      எனில்(endslash)
        plen = endslash - path + 1; /* include end slash */
      இல்லையெனில்
        plen = strlen(path);
    }

    எனில்(path) {
      co->path = sanitize_cookie_path(path, plen);
      எனில்(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  எனில்(!result) {
    எனில்(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    இல்லையெனில் எனில்(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      எனில்(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  திரும்பு result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
நிலையான bool parse_first_pair(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Cookie *co,
                             கட்டமைப்பு Curl_str *cookie,
                             கட்டமைப்பு Curl_str *name,
                             கட்டமைப்பு Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  எனில்(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    திரும்பு FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  எனில்((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    திரும்பு FALSE;
  }

  /* Check if we have a reserved prefix set. */
  எனில்(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  இல்லையெனில் எனில்(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_மதிப்பு] = *val;
  திரும்பு TRUE;
}

நிலையான bool parse_domain(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Cookie *co,
                         கட்டமைப்பு Curl_str *cookie_domain,
                         கட்டமைப்பு Curl_str *val,
                         மாறிலி எழுத்து **domainp)
{
  bool is_ip;
  மாறிலி எழுத்து *domain = *domainp;
  மாறிலி எழுத்து *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  எனில்('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  எனில்(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  எனில்(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    எனில்(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  இல்லையெனில் {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான வெறுமை parse_maxage(கட்டமைப்பு Cookie *co, கட்டமைப்பு Curl_str *val,
                         time_t *nowp)
{
  முழு rc;
  மாறிலி எழுத்து *maxage = curlx_str(val);
  எனில்(*maxage == '\"')
    maxage++;
  rc = curlx_str_எண்(&maxage, &co->expires, CURL_OFF_T_MAX);
  எனில்(!*nowp)
    *nowp = time(வெற்று);
  தேர்வு(rc) {
  நிலை STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    நிறுத்து;
  இயல்பு:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    நிறுத்து;
  நிலை STRE_OK:
    எனில்(!co->expires)
      co->expires = 1; /* expire now */
    இல்லையெனில் எனில்(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    இல்லையெனில்
      co->expires += *nowp;
    நிறுத்து;
  }
  cap_expires(*nowp, co);
}

நிலையான வெறுமை parse_expires(கட்டமைப்பு Cookie *co, கட்டமைப்பு Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  எனில்(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    எழுத்து dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    எனில்(!Curl_getdate_capped(dbuf, &date)) {
      எனில்(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    இல்லையெனில்
      co->expires = 0;
    எனில்(!*nowp)
      *nowp = time(வெற்று);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
நிலையான CURLcode
parse_cookie_header(கட்டமைப்பு Curl_easy *data,
                    கட்டமைப்பு Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    மாறிலி எழுத்து *ptr, /* the header */
                    மாறிலி எழுத்து *domain, /* default domain */
                    /* full path used when this cookie is set */
                    மாறிலி எழுத்து *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  கட்டமைப்பு Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  எனில்(linelength > MAX_COOKIE_வரி)
    /* discard overly long lines at once */
    திரும்பு CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, அளவு(cookie));
  செய் {
    கட்டமைப்பு Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    முழு rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    எனில்(!rc || (rc == STRE_SHORT)) {
      கட்டமைப்பு Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      எனில்(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        திரும்பு CURLE_OK;
      }

      எனில்(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        எனில்(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        எனில்(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          திரும்பு CURLE_OK;
        }
      }
      இல்லையெனில்
        curlx_str_init(&val);

      எனில்(!curlx_strlen(&cookie[COOKIE_NAME])) {
        எனில்(!parse_first_pair(data, co, cookie, &name, &val, sep))
          திரும்பு CURLE_OK;
      }

      /* whether there's separator or not */
      இல்லையெனில் எனில்(curlx_str_casecompare(&name, "secure")) {
        எனில்(secure_origin)
          co->secure = TRUE;
        இல்லையெனில் {
          infof(data, "skipped cookie because not 'secure'");
          திரும்பு CURLE_OK;
        }
      }
      இல்லையெனில் எனில்(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      இல்லையெனில் எனில்(sep) {
        /* only if a separating equals sign is present */
        எனில்(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        இல்லையெனில் எனில்(curlx_str_casecompare(&name, "domain")) {
          எனில்(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            திரும்பு CURLE_OK;
        }
        இல்லையெனில் எனில்(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        இல்லையெனில் எனில்(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } போது(!curlx_str_single(&ptr, ';'));

  எனில்(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    எனில்(!result)
      *okay = TRUE;
  }
  திரும்பு result;
}

நிலையான CURLcode parse_netscape(கட்டமைப்பு Cookie *co,
                               மாறிலி கட்டமைப்பு CookieInfo *ci,
                               bool *okay,
                               மாறிலி எழுத்து *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  மாறிலி எழுத்து *ptr, *next;
  முழு fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  எனில்(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  எனில்(lineptr[0] == '#')
    /* do not even try the comments */
    திரும்பு CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  க்கு(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : வெற்று);
    தேர்வு(fields) {
    நிலை 0:
      எனில்(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      எனில்(!co->domain)
        திரும்பு CURLE_OUT_OF_MEMORY;
      நிறுத்து;
    நிலை 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      நிறுத்து;
    நிலை 2:
      /* The file format allows the path field to remain not filled in */
      எனில்(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        எனில்(!co->path)
          திரும்பு CURLE_OUT_OF_MEMORY;
        நிறுத்து;
      }
      இல்லையெனில் {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        எனில்(!co->path)
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    நிலை 3:
      co->secure = FALSE;
      எனில்(curl_strnequal(ptr, "TRUE", len)) {
        எனில்(secure_origin || ci->running)
          co->secure = TRUE;
        இல்லையெனில்
          திரும்பு CURLE_OK;
      }
      நிறுத்து;
    நிலை 4:
      எனில்(curlx_str_எண்(&ptr, &co->expires, CURL_OFF_T_MAX))
        திரும்பு CURLE_OK;
      நிறுத்து;
    நிலை 5:
      co->name = curlx_memdup0(ptr, len);
      எனில்(!co->name)
        திரும்பு CURLE_OUT_OF_MEMORY;
      இல்லையெனில் {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        எனில்(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        இல்லையெனில் எனில்(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      நிறுத்து;
    நிலை 6:
      co->மதிப்பு = curlx_memdup0(ptr, len);
      எனில்(!co->மதிப்பு)
        திரும்பு CURLE_OUT_OF_MEMORY;
      நிறுத்து;
    }
  }
  எனில்(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->மதிப்பு = curlx_strdup("");
    எனில்(!co->மதிப்பு)
      திரும்பு CURLE_OUT_OF_MEMORY;
    இல்லையெனில்
      fields++;
  }

  எனில்(fields != 7)
    /* we did not find the sufficient number of fields */
    திரும்பு CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  எனில்(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->மதிப்பு, strlen(co->மதிப்பு)))
    திரும்பு CURLE_OK;

  *okay = TRUE;
  திரும்பு CURLE_OK;
}

நிலையான bool is_public_suffix(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Cookie *co,
                             மாறிலி எழுத்து *domain)
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
  எனில்(!co->domain || Curl_host_is_ipnum(co->domain))
    திரும்பு FALSE;

  இல்லையெனில் {
    bool acceptable = FALSE;
    எழுத்து lcase[256];
    எழுத்து lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    எனில்(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    எனில்(clen && (co->domain[clen - 1] == '.'))
      clen--;

    எனில்((dlen < அளவு(lcase)) && (clen < அளவு(lcookie))) {
      மாறிலி psl_ctx_t *psl = Curl_psl_use(data);
      எனில்(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        எனில்(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          எனில்(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        இல்லையெனில் {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          எனில்(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      இல்லையெனில்
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    எனில்(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      திரும்பு TRUE;
    }
  }
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)co;
  (வெறுமை)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  திரும்பு FALSE;
}

/* returns TRUE when replaced */
நிலையான bool replace_existing(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Cookie *co,
                             மாறிலி கட்டமைப்பு CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  கட்டமைப்பு Curl_llist_முனை *replace_n = வெற்று;
  கட்டமைப்பு Curl_llist_முனை *n;
  size_t myhash = cookiehash(co->domain);
  க்கு(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_முனை_next(n)) {
    கட்டமைப்பு Cookie *clist = Curl_முனை_elem(n);
    எனில்(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      எனில்(clist->domain && co->domain) {
        எனில்(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      இல்லையெனில் எனில்(!clist->domain && !co->domain)
        matching_domains = TRUE;

      எனில்(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        மாறிலி எழுத்து *sep = வெற்று;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        எனில்(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        எனில்(sep)
          cllen = sep - clist->path;
        இல்லையெனில்
          cllen = strlen(clist->path);

        எனில்(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          திரும்பு FALSE;
        }
      }
    }

    எனில்(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      எனில்(clist->domain && co->domain) {
        எனில்(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      இல்லையெனில் எனில்(!clist->domain && !co->domain)
        replace_old = TRUE;

      எனில்(replace_old) {
        /* the domains were identical */

        எனில்(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        இல்லையெனில் எனில்(!clist->path != !co->path)
          replace_old = FALSE;
      }

      எனில்(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        திரும்பு FALSE;
      }
      எனில்(replace_old)
        replace_n = n;
    }
  }
  எனில்(replace_n) {
    கட்டமைப்பு Cookie *repl = Curl_முனை_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_முனை_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  திரும்பு TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு CookieInfo *ci,
                         மாறிலி எழுத்து *lineptr, /* first character of the line */
                         மாறிலி எழுத்து *domain,  /* default domain */
                         மாறிலி எழுத்து *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         மாறிலி முழு flags)
{
  கட்டமைப்பு Cookie comem;
  கட்டமைப்பு Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  எனில்(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    திரும்பு CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, அளவு(comem));

  எனில்(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  இல்லையெனில்
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  எனில்(result || !okay)
    செல் fail;

  எனில்(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    செல் fail;

  எனில்(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    செல் fail;

  எனில்(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    எனில்(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    இல்லையெனில்
      செல் fail;
  }

  எனில்(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    செல் fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  எனில்(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  எனில்(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    செல் fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, அளவு(comem));
  எனில்(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    செல் fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->முனை);

  எனில்(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->மதிப்பு,
          co->domain, co->path, co->expires);

  எனில்(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  எனில்(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  எனில்(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  திரும்பு result;
fail:
  freecookie(co, FALSE);
  திரும்பு result;
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
கட்டமைப்பு CookieInfo *Curl_cookie_init(வெறுமை)
{
  முழு i;
  கட்டமைப்பு CookieInfo *ci = curlx_calloc(1, அளவு(கட்டமைப்பு CookieInfo));
  எனில்(!ci)
    திரும்பு வெற்று;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  க்கு(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], வெற்று);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  திரும்பு ci;
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
நிலையான CURLcode cookie_load(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *file,
                            கட்டமைப்பு CookieInfo *ci, முழு flags)
{
  FILE *handle = வெற்று;
  CURLcode result = CURLE_OK;
  FILE *fp = வெற்று;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  எனில்(file && *file) {
    எனில்(!strcmp(file, "-"))
      fp = stdin;
    இல்லையெனில் {
      fp = curlx_fopen(file, "rb");
      எனில்(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      இல்லையெனில் {
        curlx_struct_stat stat;
        எனில்((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = வெற்று;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        இல்லையெனில்
          handle = fp;
      }
    }
  }

  எனில்(fp) {
    கட்டமைப்பு dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    செய் {
      result = Curl_get_வரி(&buf, fp, &eof);
      எனில்(!result) {
        மாறிலி எழுத்து *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        எனில்(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, வெற்று, வெற்று,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        எனில்(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } போது(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    எனில்(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  திரும்பு result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(கட்டமைப்பு Curl_easy *data,
                               முழு flags)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு curl_slist *list = data->state.cookielist;
  எனில்(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    எனில்(!data->cookies)
      data->cookies = Curl_cookie_init();
    எனில்(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    இல்லையெனில் {
      data->state.cookie_engine = TRUE;
      போது(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        எனில்(result)
          நிறுத்து;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  திரும்பு result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
நிலையான முழு cookie_sort(மாறிலி வெறுமை *p1, மாறிலி வெறுமை *p2)
{
  மாறிலி கட்டமைப்பு Cookie *c1 = *(மாறிலி கட்டமைப்பு Cookie * மாறிலி *)p1;
  மாறிலி கட்டமைப்பு Cookie *c2 = *(மாறிலி கட்டமைப்பு Cookie * மாறிலி *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  எனில்(l1 != l2)
    திரும்பு (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  எனில்(l1 != l2)
    திரும்பு (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  எனில்(l1 != l2)
    திரும்பு (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  திரும்பு (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
நிலையான முழு cookie_sort_ct(மாறிலி வெறுமை *p1, மாறிலி வெறுமை *p2)
{
  மாறிலி கட்டமைப்பு Cookie *c1 = *(மாறிலி கட்டமைப்பு Cookie * மாறிலி *)p1;
  மாறிலி கட்டமைப்பு Cookie *c2 = *(மாறிலி கட்டமைப்பு Cookie * மாறிலி *)p2;

  திரும்பு (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *host)
{
  திரும்பு Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(கட்டமைப்பு Curl_easy *data,
                             bool *okay,
                             மாறிலி எழுத்து *host,
                             கட்டமைப்பு Curl_llist *list)
{
  size_t matches = 0;
  மாறிலி bool is_ip = Curl_host_is_ipnum(host);
  மாறிலி size_t myhash = cookiehash(host);
  கட்டமைப்பு Curl_llist_முனை *n;
  மாறிலி bool secure = Curl_secure_context(data, host);
  கட்டமைப்பு CookieInfo *ci = data->cookies;
  மாறிலி எழுத்து *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, வெற்று);

  எனில்(!ci || !Curl_llist_மொத்தம்(&ci->cookielist[myhash]))
    திரும்பு CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  க்கு(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_முனை_next(n)) {
    கட்டமைப்பு Cookie *co = Curl_முனை_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    எனில்(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      எனில்(!co->domain ||
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
        எனில்(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          எனில்(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            நிறுத்து;
          }
        }
      }
    }
  }

  எனில்(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    கட்டமைப்பு Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(அளவு(கட்டமைப்பு Cookie *) * matches);
    எனில்(!array) {
      result = CURLE_OUT_OF_MEMORY;
      செல் fail;
    }

    n = Curl_llist_head(list);

    க்கு(i = 0; n; n = Curl_முனை_next(n))
      array[i++] = Curl_முனை_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, அளவு(கட்டமைப்பு Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, வெற்று);

    க்கு(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  திரும்பு CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, வெற்று);
  திரும்பு result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
வெறுமை Curl_cookie_clearall(கட்டமைப்பு CookieInfo *ci)
{
  எனில்(ci) {
    unsigned முழு i;
    க்கு(i = 0; i < COOKIE_HASH_SIZE; i++) {
      கட்டமைப்பு Curl_llist_முனை *n;
      க்கு(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        கட்டமைப்பு Cookie *c = Curl_முனை_elem(n);
        கட்டமைப்பு Curl_llist_முனை *e = Curl_முனை_next(n);
        Curl_முனை_remove(n);
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
வெறுமை Curl_cookie_clearsess(கட்டமைப்பு CookieInfo *ci)
{
  unsigned முழு i;

  எனில்(!ci)
    திரும்பு;

  க்கு(i = 0; i < COOKIE_HASH_SIZE; i++) {
    கட்டமைப்பு Curl_llist_முனை *n = Curl_llist_head(&ci->cookielist[i]);
    கட்டமைப்பு Curl_llist_முனை *e = வெற்று;

    க்கு(; n; n = e) {
      கட்டமைப்பு Cookie *curr = Curl_முனை_elem(n);
      e = Curl_முனை_next(n); /* in case the node is removed, get it early */
      எனில்(!curr->expires) {
        Curl_முனை_remove(n);
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
வெறுமை Curl_cookie_cleanup(கட்டமைப்பு CookieInfo *ci)
{
  எனில்(ci) {
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
நிலையான எழுத்து *get_netscape_format(மாறிலி கட்டமைப்பு Cookie *co)
{
  திரும்பு curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*மதிப்பு*/
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
    co->மதிப்பு ? co->மதிப்பு : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
நிலையான CURLcode cookie_output(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு CookieInfo *ci,
                              மாறிலி எழுத்து *filename)
{
  FILE *out = வெற்று;
  bool use_stdout = FALSE;
  எழுத்து *tempstore = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்(!ci)
    /* no cookie engine alive */
    திரும்பு CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  எனில்(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  இல்லையெனில் {
    result = Curl_fopen(data, filename, &out, &tempstore);
    எனில்(result)
      செல் error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  எனில்(ci->numcookies) {
    unsigned முழு i;
    size_t nvalid = 0;
    கட்டமைப்பு Cookie **array;
    கட்டமைப்பு Curl_llist_முனை *n;

    array = curlx_calloc(1, அளவு(கட்டமைப்பு Cookie *) * ci->numcookies);
    எனில்(!array) {
      result = CURLE_OUT_OF_MEMORY;
      செல் error;
    }

    /* only sort the cookies with a domain property */
    க்கு(i = 0; i < COOKIE_HASH_SIZE; i++) {
      க்கு(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_முனை_next(n)) {
        கட்டமைப்பு Cookie *co = Curl_முனை_elem(n);
        எனில்(!co->domain)
          தொடர்;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, அளவு(கட்டமைப்பு Cookie *), cookie_sort_ct);

    க்கு(i = 0; i < nvalid; i++) {
      எழுத்து *format_ptr = get_netscape_format(array[i]);
      எனில்(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        செல் error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  எனில்(!use_stdout) {
    curlx_fclose(out);
    out = வெற்று;
    எனில்(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      செல் error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  திரும்பு CURLE_OK;

error:
  எனில்(out && !use_stdout)
    curlx_fclose(out);
  எனில்(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  திரும்பு result;
}

நிலையான கட்டமைப்பு curl_slist *cookie_list(மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு curl_slist *list = வெற்று;
  கட்டமைப்பு curl_slist *beg;
  கட்டமைப்பு curl_slist **tail = &list;
  unsigned முழு i;
  கட்டமைப்பு Curl_llist_முனை *n;

  எனில்(!data->cookies || (data->cookies->numcookies == 0))
    திரும்பு வெற்று;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  க்கு(i = 0; i < COOKIE_HASH_SIZE; i++) {
    க்கு(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_முனை_next(n)) {
      கட்டமைப்பு Cookie *c = Curl_முனை_elem(n);
      எழுத்து *வரி;
      எனில்(!c->domain)
        தொடர்;
      வரி = get_netscape_format(c);
      எனில்(!வரி) {
        curl_slist_free_all(list);
        திரும்பு வெற்று;
      }
      beg = Curl_slist_append_nodup(வெற்று, வரி);
      எனில்(!beg) {
        curlx_free(வரி);
        curl_slist_free_all(list);
        திரும்பு வெற்று;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  திரும்பு list;
}

கட்டமைப்பு curl_slist *Curl_cookie_list(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  திரும்பு list;
}

வெறுமை Curl_flush_cookies(கட்டமைப்பு Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  எனில்(data->cookies) {
    மாறிலி எழுத்து *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    எனில்(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      எனில்(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    எனில்(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = வெற்று;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

வெறுமை Curl_cookie_run(கட்டமைப்பு Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  எனில்(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
