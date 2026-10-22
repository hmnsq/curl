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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#شامل "urldata.h"
#شامل "cookie.h"
#شامل "psl.h"
#شامل "curl_trc.h"
#شامل "transfer.h"
#شامل "slist.h"
#شامل "curl_share.h"
#شامل "strcase.h"
#شامل "curl_fopen.h"
#شامل "curl_get_line.h"
#شامل "curl_memrchr.h"
#شامل "parsedate.h"
#شامل "curlx/strdup.h"
#شامل "curlx/dynbuf.h"
#شامل "llist.h"
#شامل "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ایستا تهی cap_expires(time_t now, ساختار Cookie *co)
{
  اگر(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    اگر(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ایستا تهی freecookie(ساختار Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->مقدار);
  اگر(maintoo)
    curlx_free(co);
}

ایستا bool cookie_tailmatch(ثابت نویسه *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت نویسه *hostname)
{
  size_t hostname_len = strlen(hostname);

  اگر(hostname_len < cookie_domain_len)
    بازگشت FALSE;

  اگر(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    بازگشت FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  اگر(hostname_len == cookie_domain_len)
    بازگشت TRUE;
  اگر('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    بازگشت TRUE;
  بازگشت FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ایستا bool pathmatch(ثابت نویسه *cookie_path, ثابت نویسه *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  اگر(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    بازگشت TRUE;
  }

  /* #-fragments are already cut off! */
  اگر(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  اگر(uri_path_len < cookie_path_len)
    برو pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  اگر(strncmp(cookie_path, uri_path, cookie_path_len))
    برو pathmatched;

  /* The cookie-path and the uri-path are identical. */
  اگر(cookie_path_len == uri_path_len) {
    ret = TRUE;
    برو pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  اگر(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    برو pathmatched;
  }

pathmatched:
  بازگشت ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ایستا ثابت نویسه *get_بالاترین_domain(ثابت نویسه * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت نویسه *first = پوچ, *last;

  اگر(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    اگر(last) {
      first = memrchr(domain, '.', (last - domain));
      اگر(first)
        len -= (++first - domain);
    }
  }

  اگر(outlen)
    *outlen = len;

  بازگشت first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#اگر defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
ایستا size_t cookie_hash_domain(ثابت نویسه *domain, ثابت size_t len)
{
  ثابت نویسه *end = domain + len;
  size_t h = 5381;

  وقتی(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  بازگشت (h % COOKIE_HASH_SIZE);
}

#اگر defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
ایستا size_t cookiehash(ثابت نویسه * ثابت domain)
{
  ثابت نویسه *بالاترین;
  size_t len;

  اگر(!domain || Curl_host_is_ipnum(domain))
    بازگشت 0;

  بالاترین = get_بالاترین_domain(domain, &len);
  بازگشت cookie_hash_domain(بالاترین, len);
}

/*
 * cookie path sanitize
 */
ایستا نویسه *sanitize_cookie_path(ثابت نویسه *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  اگر(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    اگر(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  اگر(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    بازگشت curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  اگر(len > 1 && cookie_path[len - 1] == '/')
    len--;

  بازگشت curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ایستا CURLcode strstore(نویسه **str, ثابت نویسه *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  اگر(!*str)
    بازگشت CURLE_OUT_OF_MEMORY;
  بازگشت CURLE_OK;
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
ایستا تهی remove_expired(ساختار CookieInfo *ci)
{
  ساختار Cookie *co;
  curl_off_t now = (curl_off_t)time(پوچ);
  unsigned صحیح i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  اگر(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    بازگشت;
  وگرنه
    ci->next_expiration = CURL_OFF_T_MAX;

  برای(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ساختار Curl_llist_گره *n;
    ساختار Curl_llist_گره *e = پوچ;

    برای(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_گره_elem(n);
      e = Curl_گره_next(n);
      اگر(co->expires) {
        اگر(co->expires < now) {
          Curl_گره_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        وگرنه اگر(co->expires < ci->next_expiration)
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
ایستا bool bad_domain(ثابت نویسه *domain, size_t len)
{
  اگر((len == 9) && curl_strnequal(domain, "localhost", 9))
    بازگشت FALSE;
  وگرنه {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ثابت نویسه *dot = memchr(domain, '.', len);
    اگر(dot) {
      size_t i = dot - domain;
      اگر((len - i) > 1)
        /* the dot is not the last byte */
        بازگشت FALSE;
    }
  }
  بازگشت TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ایستا bool invalid_octets(ثابت نویسه *ptr, size_t len)
{
  ثابت unsigned نویسه *p = (ثابت unsigned نویسه *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  وقتی(len && *p) {
    اگر((*p < 0x20) || (*p == 0x7f))
      بازگشت TRUE;
    p++;
    len--;
  }
  بازگشت FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_مقدار  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

ایستا CURLcode storecookie(ساختار Cookie *co, ثابت ساختار Curl_str *cp,
                            ثابت نویسه *path, ثابت نویسه *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  اگر(!result)
    result = strstore(&co->مقدار, curlx_str(&cp[COOKIE_مقدار]),
                      curlx_strlen(&cp[COOKIE_مقدار]));
  اگر(!result) {
    size_t plen = 0;
    اگر(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    وگرنه اگر(path) {
      /* No path was given in the header line, set the default */
      ثابت نویسه *endslash = strrchr(path, '/');
      اگر(endslash)
        plen = endslash - path + 1; /* include end slash */
      وگرنه
        plen = strlen(path);
    }

    اگر(path) {
      co->path = sanitize_cookie_path(path, plen);
      اگر(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  اگر(!result) {
    اگر(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    وگرنه اگر(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      اگر(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  بازگشت result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ایستا bool parse_first_pair(ساختار Curl_easy *data, ساختار Cookie *co,
                             ساختار Curl_str *cookie,
                             ساختار Curl_str *name,
                             ساختار Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  اگر(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    بازگشت FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  اگر((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    بازگشت FALSE;
  }

  /* Check if we have a reserved prefix set. */
  اگر(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  وگرنه اگر(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_مقدار] = *val;
  بازگشت TRUE;
}

ایستا bool parse_domain(ساختار Curl_easy *data, ساختار Cookie *co,
                         ساختار Curl_str *cookie_domain,
                         ساختار Curl_str *val,
                         ثابت نویسه **domainp)
{
  bool is_ip;
  ثابت نویسه *domain = *domainp;
  ثابت نویسه *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  اگر('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  اگر(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  اگر(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    اگر(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  وگرنه {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    بازگشت FALSE;
  }
  بازگشت TRUE;
}

ایستا تهی parse_maxage(ساختار Cookie *co, ساختار Curl_str *val,
                         time_t *nowp)
{
  صحیح rc;
  ثابت نویسه *maxage = curlx_str(val);
  اگر(*maxage == '\"')
    maxage++;
  rc = curlx_str_عدد(&maxage, &co->expires, CURL_OFF_T_MAX);
  اگر(!*nowp)
    *nowp = time(پوچ);
  انتخاب(rc) {
  حالت STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    بشکن;
  پیشفرض:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    بشکن;
  حالت STRE_OK:
    اگر(!co->expires)
      co->expires = 1; /* expire now */
    وگرنه اگر(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    وگرنه
      co->expires += *nowp;
    بشکن;
  }
  cap_expires(*nowp, co);
}

ایستا تهی parse_expires(ساختار Cookie *co, ساختار Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  اگر(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    نویسه dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    اگر(!Curl_getdate_capped(dbuf, &date)) {
      اگر(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    وگرنه
      co->expires = 0;
    اگر(!*nowp)
      *nowp = time(پوچ);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ایستا CURLcode
parse_cookie_header(ساختار Curl_easy *data,
                    ساختار Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ثابت نویسه *ptr, /* the header */
                    ثابت نویسه *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ثابت نویسه *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  ساختار Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  اگر(linelength > MAX_COOKIE_سطر)
    /* discard overly long lines at once */
    بازگشت CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, اندازه(cookie));
  انجامبده {
    ساختار Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    صحیح rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    اگر(!rc || (rc == STRE_SHORT)) {
      ساختار Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      اگر(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        بازگشت CURLE_OK;
      }

      اگر(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        اگر(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        اگر(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          بازگشت CURLE_OK;
        }
      }
      وگرنه
        curlx_str_init(&val);

      اگر(!curlx_strlen(&cookie[COOKIE_NAME])) {
        اگر(!parse_first_pair(data, co, cookie, &name, &val, sep))
          بازگشت CURLE_OK;
      }

      /* whether there's separator or not */
      وگرنه اگر(curlx_str_casecompare(&name, "secure")) {
        اگر(secure_origin)
          co->secure = TRUE;
        وگرنه {
          infof(data, "skipped cookie because not 'secure'");
          بازگشت CURLE_OK;
        }
      }
      وگرنه اگر(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      وگرنه اگر(sep) {
        /* only if a separating equals sign is present */
        اگر(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        وگرنه اگر(curlx_str_casecompare(&name, "domain")) {
          اگر(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            بازگشت CURLE_OK;
        }
        وگرنه اگر(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        وگرنه اگر(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } وقتی(!curlx_str_single(&ptr, ';'));

  اگر(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    اگر(!result)
      *okay = TRUE;
  }
  بازگشت result;
}

ایستا CURLcode parse_netscape(ساختار Cookie *co,
                               ثابت ساختار CookieInfo *ci,
                               bool *okay,
                               ثابت نویسه *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ثابت نویسه *ptr, *next;
  صحیح fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  اگر(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  اگر(lineptr[0] == '#')
    /* do not even try the comments */
    بازگشت CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  برای(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : پوچ);
    انتخاب(fields) {
    حالت 0:
      اگر(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      اگر(!co->domain)
        بازگشت CURLE_OUT_OF_MEMORY;
      بشکن;
    حالت 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      بشکن;
    حالت 2:
      /* The file format allows the path field to remain not filled in */
      اگر(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        اگر(!co->path)
          بازگشت CURLE_OUT_OF_MEMORY;
        بشکن;
      }
      وگرنه {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        اگر(!co->path)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    حالت 3:
      co->secure = FALSE;
      اگر(curl_strnequal(ptr, "TRUE", len)) {
        اگر(secure_origin || ci->running)
          co->secure = TRUE;
        وگرنه
          بازگشت CURLE_OK;
      }
      بشکن;
    حالت 4:
      اگر(curlx_str_عدد(&ptr, &co->expires, CURL_OFF_T_MAX))
        بازگشت CURLE_OK;
      بشکن;
    حالت 5:
      co->name = curlx_memdup0(ptr, len);
      اگر(!co->name)
        بازگشت CURLE_OUT_OF_MEMORY;
      وگرنه {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        اگر(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        وگرنه اگر(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      بشکن;
    حالت 6:
      co->مقدار = curlx_memdup0(ptr, len);
      اگر(!co->مقدار)
        بازگشت CURLE_OUT_OF_MEMORY;
      بشکن;
    }
  }
  اگر(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->مقدار = curlx_strdup("");
    اگر(!co->مقدار)
      بازگشت CURLE_OUT_OF_MEMORY;
    وگرنه
      fields++;
  }

  اگر(fields != 7)
    /* we did not find the sufficient number of fields */
    بازگشت CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  اگر(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->مقدار, strlen(co->مقدار)))
    بازگشت CURLE_OK;

  *okay = TRUE;
  بازگشت CURLE_OK;
}

ایستا bool is_public_suffix(ساختار Curl_easy *data,
                             ساختار Cookie *co,
                             ثابت نویسه *domain)
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
  اگر(!co->domain || Curl_host_is_ipnum(co->domain))
    بازگشت FALSE;

  وگرنه {
    bool acceptable = FALSE;
    نویسه lcase[256];
    نویسه lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    اگر(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    اگر(clen && (co->domain[clen - 1] == '.'))
      clen--;

    اگر((dlen < اندازه(lcase)) && (clen < اندازه(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(data);
      اگر(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        اگر(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          اگر(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        وگرنه {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          اگر(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      وگرنه
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    اگر(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      بازگشت TRUE;
    }
  }
#وگرنه
  (تهی)data;
  (تهی)co;
  (تهی)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  بازگشت FALSE;
}

/* returns TRUE when replaced */
ایستا bool replace_existing(ساختار Curl_easy *data,
                             ساختار Cookie *co,
                             ثابت ساختار CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ساختار Curl_llist_گره *replace_n = پوچ;
  ساختار Curl_llist_گره *n;
  size_t myhash = cookiehash(co->domain);
  برای(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_گره_next(n)) {
    ساختار Cookie *clist = Curl_گره_elem(n);
    اگر(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      اگر(clist->domain && co->domain) {
        اگر(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      وگرنه اگر(!clist->domain && !co->domain)
        matching_domains = TRUE;

      اگر(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ثابت نویسه *sep = پوچ;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        اگر(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        اگر(sep)
          cllen = sep - clist->path;
        وگرنه
          cllen = strlen(clist->path);

        اگر(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          بازگشت FALSE;
        }
      }
    }

    اگر(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      اگر(clist->domain && co->domain) {
        اگر(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      وگرنه اگر(!clist->domain && !co->domain)
        replace_old = TRUE;

      اگر(replace_old) {
        /* the domains were identical */

        اگر(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        وگرنه اگر(!clist->path != !co->path)
          replace_old = FALSE;
      }

      اگر(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        بازگشت FALSE;
      }
      اگر(replace_old)
        replace_n = n;
    }
  }
  اگر(replace_n) {
    ساختار Cookie *repl = Curl_گره_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_گره_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  بازگشت TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(ساختار Curl_easy *data,
                         ساختار CookieInfo *ci,
                         ثابت نویسه *lineptr, /* first character of the line */
                         ثابت نویسه *domain,  /* default domain */
                         ثابت نویسه *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ثابت صحیح flags)
{
  ساختار Cookie comem;
  ساختار Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  اگر(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    بازگشت CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, اندازه(comem));

  اگر(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  وگرنه
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  اگر(result || !okay)
    برو fail;

  اگر(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    برو fail;

  اگر(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    برو fail;

  اگر(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    اگر(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    وگرنه
      برو fail;
  }

  اگر(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    برو fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  اگر(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  اگر(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    برو fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, اندازه(comem));
  اگر(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    برو fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->گره);

  اگر(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->مقدار,
          co->domain, co->path, co->expires);

  اگر(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  اگر(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  اگر(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  بازگشت result;
fail:
  freecookie(co, FALSE);
  بازگشت result;
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
ساختار CookieInfo *Curl_cookie_init(تهی)
{
  صحیح i;
  ساختار CookieInfo *ci = curlx_calloc(1, اندازه(ساختار CookieInfo));
  اگر(!ci)
    بازگشت پوچ;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  برای(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], پوچ);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  بازگشت ci;
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
ایستا CURLcode cookie_load(ساختار Curl_easy *data, ثابت نویسه *file,
                            ساختار CookieInfo *ci, صحیح flags)
{
  FILE *handle = پوچ;
  CURLcode result = CURLE_OK;
  FILE *fp = پوچ;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  اگر(file && *file) {
    اگر(!strcmp(file, "-"))
      fp = stdin;
    وگرنه {
      fp = curlx_fopen(file, "rb");
      اگر(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      وگرنه {
        curlx_struct_stat stat;
        اگر((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = پوچ;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        وگرنه
          handle = fp;
      }
    }
  }

  اگر(fp) {
    ساختار dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    انجامبده {
      result = Curl_get_سطر(&buf, fp, &eof);
      اگر(!result) {
        ثابت نویسه *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        اگر(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, پوچ, پوچ,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        اگر(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } وقتی(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    اگر(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  بازگشت result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ساختار Curl_easy *data,
                               صحیح flags)
{
  CURLcode result = CURLE_OK;
  ساختار curl_slist *list = data->state.cookielist;
  اگر(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    اگر(!data->cookies)
      data->cookies = Curl_cookie_init();
    اگر(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    وگرنه {
      data->state.cookie_engine = TRUE;
      وقتی(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        اگر(result)
          بشکن;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  بازگشت result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ایستا صحیح cookie_sort(ثابت تهی *p1, ثابت تهی *p2)
{
  ثابت ساختار Cookie *c1 = *(ثابت ساختار Cookie * ثابت *)p1;
  ثابت ساختار Cookie *c2 = *(ثابت ساختار Cookie * ثابت *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  اگر(l1 != l2)
    بازگشت (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  اگر(l1 != l2)
    بازگشت (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  اگر(l1 != l2)
    بازگشت (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  بازگشت (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ایستا صحیح cookie_sort_ct(ثابت تهی *p1, ثابت تهی *p2)
{
  ثابت ساختار Cookie *c1 = *(ثابت ساختار Cookie * ثابت *)p1;
  ثابت ساختار Cookie *c2 = *(ثابت ساختار Cookie * ثابت *)p2;

  بازگشت (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ساختار Curl_easy *data, ثابت نویسه *host)
{
  بازگشت Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(ساختار Curl_easy *data,
                             bool *okay,
                             ثابت نویسه *host,
                             ساختار Curl_llist *list)
{
  size_t matches = 0;
  ثابت bool is_ip = Curl_host_is_ipnum(host);
  ثابت size_t myhash = cookiehash(host);
  ساختار Curl_llist_گره *n;
  ثابت bool secure = Curl_secure_context(data, host);
  ساختار CookieInfo *ci = data->cookies;
  ثابت نویسه *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, پوچ);

  اگر(!ci || !Curl_llist_شمار(&ci->cookielist[myhash]))
    بازگشت CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  برای(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_گره_next(n)) {
    ساختار Cookie *co = Curl_گره_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    اگر(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      اگر(!co->domain ||
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
        اگر(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          اگر(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            بشکن;
          }
        }
      }
    }
  }

  اگر(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    ساختار Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(اندازه(ساختار Cookie *) * matches);
    اگر(!array) {
      result = CURLE_OUT_OF_MEMORY;
      برو fail;
    }

    n = Curl_llist_head(list);

    برای(i = 0; n; n = Curl_گره_next(n))
      array[i++] = Curl_گره_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, اندازه(ساختار Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, پوچ);

    برای(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  بازگشت CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, پوچ);
  بازگشت result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
تهی Curl_cookie_clearall(ساختار CookieInfo *ci)
{
  اگر(ci) {
    unsigned صحیح i;
    برای(i = 0; i < COOKIE_HASH_SIZE; i++) {
      ساختار Curl_llist_گره *n;
      برای(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        ساختار Cookie *c = Curl_گره_elem(n);
        ساختار Curl_llist_گره *e = Curl_گره_next(n);
        Curl_گره_remove(n);
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
تهی Curl_cookie_clearsess(ساختار CookieInfo *ci)
{
  unsigned صحیح i;

  اگر(!ci)
    بازگشت;

  برای(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ساختار Curl_llist_گره *n = Curl_llist_head(&ci->cookielist[i]);
    ساختار Curl_llist_گره *e = پوچ;

    برای(; n; n = e) {
      ساختار Cookie *curr = Curl_گره_elem(n);
      e = Curl_گره_next(n); /* in case the node is removed, get it early */
      اگر(!curr->expires) {
        Curl_گره_remove(n);
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
تهی Curl_cookie_cleanup(ساختار CookieInfo *ci)
{
  اگر(ci) {
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
ایستا نویسه *get_netscape_format(ثابت ساختار Cookie *co)
{
  بازگشت curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*مقدار*/
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
    co->مقدار ? co->مقدار : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ایستا CURLcode cookie_output(ساختار Curl_easy *data,
                              ساختار CookieInfo *ci,
                              ثابت نویسه *filename)
{
  FILE *out = پوچ;
  bool use_stdout = FALSE;
  نویسه *tempstore = پوچ;
  CURLcode result = CURLE_OK;

  اگر(!ci)
    /* no cookie engine alive */
    بازگشت CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  اگر(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  وگرنه {
    result = Curl_fopen(data, filename, &out, &tempstore);
    اگر(result)
      برو error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  اگر(ci->numcookies) {
    unsigned صحیح i;
    size_t nvalid = 0;
    ساختار Cookie **array;
    ساختار Curl_llist_گره *n;

    array = curlx_calloc(1, اندازه(ساختار Cookie *) * ci->numcookies);
    اگر(!array) {
      result = CURLE_OUT_OF_MEMORY;
      برو error;
    }

    /* only sort the cookies with a domain property */
    برای(i = 0; i < COOKIE_HASH_SIZE; i++) {
      برای(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_گره_next(n)) {
        ساختار Cookie *co = Curl_گره_elem(n);
        اگر(!co->domain)
          ادامه;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, اندازه(ساختار Cookie *), cookie_sort_ct);

    برای(i = 0; i < nvalid; i++) {
      نویسه *format_ptr = get_netscape_format(array[i]);
      اگر(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        برو error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  اگر(!use_stdout) {
    curlx_fclose(out);
    out = پوچ;
    اگر(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      برو error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  بازگشت CURLE_OK;

error:
  اگر(out && !use_stdout)
    curlx_fclose(out);
  اگر(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  بازگشت result;
}

ایستا ساختار curl_slist *cookie_list(ثابت ساختار Curl_easy *data)
{
  ساختار curl_slist *list = پوچ;
  ساختار curl_slist *beg;
  ساختار curl_slist **tail = &list;
  unsigned صحیح i;
  ساختار Curl_llist_گره *n;

  اگر(!data->cookies || (data->cookies->numcookies == 0))
    بازگشت پوچ;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  برای(i = 0; i < COOKIE_HASH_SIZE; i++) {
    برای(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_گره_next(n)) {
      ساختار Cookie *c = Curl_گره_elem(n);
      نویسه *سطر;
      اگر(!c->domain)
        ادامه;
      سطر = get_netscape_format(c);
      اگر(!سطر) {
        curl_slist_free_all(list);
        بازگشت پوچ;
      }
      beg = Curl_slist_append_nodup(پوچ, سطر);
      اگر(!beg) {
        curlx_free(سطر);
        curl_slist_free_all(list);
        بازگشت پوچ;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  بازگشت list;
}

ساختار curl_slist *Curl_cookie_list(ساختار Curl_easy *data)
{
  ساختار curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  بازگشت list;
}

تهی Curl_flush_cookies(ساختار Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  اگر(data->cookies) {
    ثابت نویسه *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    اگر(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      اگر(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    اگر(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = پوچ;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

تهی Curl_cookie_run(ساختار Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  اگر(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
