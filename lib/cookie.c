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
#تعریف_کن COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ساكن فراغ cap_expires(time_t now, بنية Cookie *co)
{
  إذا(co->expires && (زمان_T_MAX - COOKIES_MAXAGE - 30) > now) {
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
  curlx_free(co->مسیر);
  curlx_free(co->name);
  curlx_free(co->مقدار);
  إذا(maintoo)
    curlx_free(co);
}

ساكن bool cookie_tailmatch(ثابت حرف *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت حرف *hostname)
{
  size_t hostname_len = طول_رشته(hostname);

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
ساكن bool pathmatch(ثابت حرف *cookie_مسیر, ثابت حرف *uri_مسیر)
{
  size_t cookie_مسیر_len;
  size_t uri_مسیر_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_مسیر_len = طول_رشته(cookie_مسیر);
  إذا(cookie_مسیر_len == 1) {
    /* cookie_path must be '/' */
    أرجع TRUE;
  }

  /* #-fragments are already cut off! */
  إذا(طول_رشته(uri_مسیر) == 0 || uri_مسیر[0] != '/')
    uri_مسیر = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_مسیر_len = طول_رشته(uri_مسیر);

  إذا(uri_مسیر_len < cookie_مسیر_len)
    اذهب pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  إذا(مقایسه_رشته(cookie_مسیر, uri_مسیر, cookie_مسیر_len))
    اذهب pathmatched;

  /* The cookie-path and the uri-path are identical. */
  إذا(cookie_مسیر_len == uri_مسیر_len) {
    ret = TRUE;
    اذهب pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  إذا(uri_مسیر[cookie_مسیر_len] == '/') {
    ret = TRUE;
    اذهب pathmatched;
  }

pathmatched:
  أرجع ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ساكن ثابت حرف *get_بالاترین_domain(ثابت حرف * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت حرف *اولی = عدم, *آخرین;

  إذا(domain) {
    len = طول_رشته(domain);
    آخرین = memrchr(domain, '.', len);
    إذا(آخرین) {
      اولی = memrchr(domain, '.', (آخرین - domain));
      إذا(اولی)
        len -= (++اولی - domain);
    }
  }

  إذا(outlen)
    *outlen = len;

  أرجع اولی ? اولی : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#رهنمود optimize("", off)
#پایان_اگر

/*
 * A case-insensitive hash for the cookie domains.
 */
ساكن size_t cookie_hash_domain(ثابت حرف *domain, ثابت size_t len)
{
  ثابت حرف *انتها = domain + len;
  size_t h = 5381;

  طالما(domain < انتها) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  أرجع (h % COOKIE_HASH_اندازه);
}

#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#رهنمود optimize("", on)
#پایان_اگر

/*
 * Hash this domain.
 */
ساكن size_t cookiehash(ثابت حرف * ثابت domain)
{
  ثابت حرف *بالاترین;
  size_t len;

  إذا(!domain || Curl_host_is_ipnum(domain))
    أرجع 0;

  بالاترین = get_بالاترین_domain(domain, &len);
  أرجع cookie_hash_domain(بالاترین, len);
}

/*
 * cookie path sanitize
 */
ساكن حرف *sanitize_cookie_مسیر(ثابت حرف *cookie_مسیر, size_t len)
{
  /* some sites send path attribute within '"'. */
  إذا(len && (cookie_مسیر[0] == '\"')) {
    cookie_مسیر++;
    len--;

    إذا(len && (cookie_مسیر[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  إذا(!len || (cookie_مسیر[0] != '/'))
    /* Let cookie-path be the default-path. */
    أرجع curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  إذا(len > 1 && cookie_مسیر[len - 1] == '/')
    len--;

  أرجع curlx_memdup0(cookie_مسیر, len);
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
    أرجع CURLE_بیرون_از_حافظه;
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
  curl_off_t now = (curl_off_t)زمان(عدم);
  بیعلامت صحيح i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  إذا(now < ci->بعدی_expiration &&
     ci->بعدی_expiration != CURL_OFF_T_MAX)
    أرجع;
  وإلا
    ci->بعدی_expiration = CURL_OFF_T_MAX;

  لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
    بنية Curl_llist_گره *n;
    بنية Curl_llist_گره *e = عدم;

    لكل(n = Curl_llist_سر(&ci->cookielist[i]); n; n = e) {
      co = Curl_گره_elem(n);
      e = Curl_گره_بعدی(n);
      إذا(co->expires) {
        إذا(co->expires < now) {
          Curl_گره_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        وإلا إذا(co->expires < ci->بعدی_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->بعدی_expiration = co->expires;
      }
    }
  }
}

#اگر_تعریف_نشده USE_LIBPSL
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
#پایان_اگر

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ساكن bool invalid_octets(ثابت حرف *ptr, size_t len)
{
  ثابت بیعلامت حرف *p = (ثابت بیعلامت حرف *)ptr;
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
#تعریف_کن MAX_DATE_درازی 80

#تعریف_کن COOKIE_NAME   0
#تعریف_کن COOKIE_مقدار  1
#تعریف_کن COOKIE_DOMAIN 2
#تعریف_کن COOKIE_مسیر   3

#تعریف_کن COOKIE_PIECES 4 /* the list above */

ساكن CURLcode storecookie(بنية Cookie *co, ثابت بنية Curl_str *cp,
                            ثابت حرف *مسیر, ثابت حرف *domain)
{
  CURLcode نتیجه;
  نتیجه = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  إذا(!نتیجه)
    نتیجه = strstore(&co->مقدار, curlx_str(&cp[COOKIE_مقدار]),
                      curlx_strlen(&cp[COOKIE_مقدار]));
  إذا(!نتیجه) {
    size_t plen = 0;
    إذا(curlx_strlen(&cp[COOKIE_مسیر])) {
      مسیر = curlx_str(&cp[COOKIE_مسیر]);
      plen = curlx_strlen(&cp[COOKIE_مسیر]);
    }
    وإلا إذا(مسیر) {
      /* No path was given in the header line, set the default */
      ثابت حرف *endslash = strrchr(مسیر, '/');
      إذا(endslash)
        plen = endslash - مسیر + 1; /* include end slash */
      وإلا
        plen = طول_رشته(مسیر);
    }

    إذا(مسیر) {
      co->مسیر = sanitize_cookie_مسیر(مسیر, plen);
      إذا(!co->مسیر)
        نتیجه = CURLE_بیرون_از_حافظه;
    }
  }
  إذا(!نتیجه) {
    إذا(curlx_strlen(&cp[COOKIE_DOMAIN]))
      نتیجه = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    وإلا إذا(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      إذا(!co->domain)
        نتیجه = CURLE_بیرون_از_حافظه;
    }
  }
  أرجع نتیجه;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ساكن bool parse_اولی_pair(بنية Curl_easy *داده, بنية Cookie *co,
                             بنية Curl_str *cookie,
                             بنية Curl_str *name,
                             بنية Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  إذا(!sep || !curlx_strlen(name)) {
    infof(داده, "invalid cookie, dropped");
    أرجع FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  إذا((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(داده, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    أرجع FALSE;
  }

  /* Check if we have a reserved prefix set. */
  إذا(!مقایسه_رشته("__Secure-", curlx_str(name), 9))
    co->پیشوند_secure = TRUE;
  وإلا إذا(!مقایسه_رشته("__Host-", curlx_str(name), 7))
    co->پیشوند_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_مقدار] = *val;
  أرجع TRUE;
}

ساكن bool parse_domain(بنية Curl_easy *داده, بنية Cookie *co,
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

#اگر_تعریف_نشده USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  إذا(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#پایان_اگر

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  إذا(!domain ||
     (is_ip &&
      !مقایسه_رشته(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == طول_رشته(domain))) ||
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
    infof(داده, "skipped cookie with bad tailmatch domain: %s",
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
  rc = curlx_str_عدد(&maxage, &co->expires, CURL_OFF_T_MAX);
  إذا(!*nowp)
    *nowp = زمان(عدم);
  بدل(rc) {
  حالة STRE_سرریز:
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
  إذا(!co->expires && (curlx_strlen(val) < MAX_DATE_درازی)) {
    حرف dbuf[MAX_DATE_درازی + 1];
    time_t date = 0;
    کپی_حافظه(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    إذا(!Curl_getdate_capped(dbuf, &date)) {
      إذا(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    وإلا
      co->expires = 0;
    إذا(!*nowp)
      *nowp = زمان(عدم);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ساكن CURLcode
parse_cookie_سرآیند(بنية Curl_easy *داده,
                    بنية Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ثابت حرف *ptr, /* the header */
                    ثابت حرف *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ثابت حرف *مسیر,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = طول_رشته(ptr);
  CURLcode نتیجه = CURLE_OK;
  بنية Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  إذا(linelength > MAX_COOKIE_سطر)
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
        infof(داده, "invalid octets in name, cookie dropped");
        أرجع CURLE_OK;
      }

      إذا(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        إذا(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        إذا(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(داده, "invalid octets in value, cookie dropped");
          أرجع CURLE_OK;
        }
      }
      وإلا
        curlx_str_init(&val);

      إذا(!curlx_strlen(&cookie[COOKIE_NAME])) {
        إذا(!parse_اولی_pair(داده, co, cookie, &name, &val, sep))
          أرجع CURLE_OK;
      }

      /* whether there's separator or not */
      وإلا إذا(curlx_str_casecompare(&name, "secure")) {
        إذا(secure_origin)
          co->secure = TRUE;
        وإلا {
          infof(داده, "skipped cookie because not 'secure'");
          أرجع CURLE_OK;
        }
      }
      وإلا إذا(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      وإلا إذا(sep) {
        /* only if a separating equals sign is present */
        إذا(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_مسیر] = val;
        وإلا إذا(curlx_str_casecompare(&name, "domain")) {
          إذا(curlx_strlen(&val) &&
             !parse_domain(داده, co, &cookie[COOKIE_DOMAIN], &val, &domain))
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
    نتیجه = storecookie(co, &cookie[0], مسیر, domain);
    إذا(!نتیجه)
      *okay = TRUE;
  }
  أرجع نتیجه;
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
  ثابت حرف *ptr, *بعدی;
  صحيح fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  إذا(!مقایسه_رشته(lineptr, "#HttpOnly_", 10)) {
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
  لكل(بعدی = lineptr; بعدی; fields++) {
    ptr = بعدی;
    len = strcspn(ptr, "\t\r\n");
    بعدی = (ptr[len] == '\t' ? &ptr[len + 1] : عدم);
    بدل(fields) {
    حالة 0:
      إذا(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      إذا(!co->domain)
        أرجع CURLE_بیرون_از_حافظه;
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
      إذا(مقایسه_رشته("TRUE", ptr, len) && مقایسه_رشته("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->مسیر = sanitize_cookie_مسیر(ptr, len);
        إذا(!co->مسیر)
          أرجع CURLE_بیرون_از_حافظه;
        اكسر;
      }
      وإلا {
        /* this does not look like a path, make one up! */
        co->مسیر = curlx_strdup("/");
        إذا(!co->مسیر)
          أرجع CURLE_بیرون_از_حافظه;
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
      إذا(curlx_str_عدد(&ptr, &co->expires, CURL_OFF_T_MAX))
        أرجع CURLE_OK;
      اكسر;
    حالة 5:
      co->name = curlx_memdup0(ptr, len);
      إذا(!co->name)
        أرجع CURLE_بیرون_از_حافظه;
      وإلا {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        إذا(!مقایسه_رشته("__Secure-", co->name, 9))
          co->پیشوند_secure = TRUE;
        وإلا إذا(!مقایسه_رشته("__Host-", co->name, 7))
          co->پیشوند_host = TRUE;
      }
      اكسر;
    حالة 6:
      co->مقدار = curlx_memdup0(ptr, len);
      إذا(!co->مقدار)
        أرجع CURLE_بیرون_از_حافظه;
      اكسر;
    }
  }
  إذا(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->مقدار = curlx_strdup("");
    إذا(!co->مقدار)
      أرجع CURLE_بیرون_از_حافظه;
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
  إذا(invalid_octets(co->name, طول_رشته(co->name)) ||
     invalid_octets(co->مقدار, طول_رشته(co->مقدار)))
    أرجع CURLE_OK;

  *okay = TRUE;
  أرجع CURLE_OK;
}

ساكن bool is_عمومی_suffix(بنية Curl_easy *داده,
                             بنية Cookie *co,
                             ثابت حرف *domain)
{
#اگر_تعریف_شده USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(داده);
  DEBUGASSERT(co);
  DEBUGF(infof(داده, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  إذا(!co->domain || Curl_host_is_ipnum(co->domain))
    أرجع FALSE;

  وإلا {
    bool acceptable = FALSE;
    حرف lcase[256];
    حرف lcookie[256];
    size_t dlen = domain ? طول_رشته(domain) : 0;
    size_t clen = طول_رشته(co->domain);

    /* trim trailing dots */
    إذا(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    إذا(clen && (co->domain[clen - 1] == '.'))
      clen--;

    إذا((dlen < حجم(lcase)) && (clen < حجم(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(داده);
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
             psl_is_عمومی_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        وإلا {
          bool suffix = psl_is_عمومی_suffix(psl, lcookie);
          acceptable = TRUE;
          إذا(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(داده);
      }
      وإلا
        infof(داده, "libpsl problem, rejecting cookie for safety");
    }

    إذا(!acceptable) {
      infof(داده, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      أرجع TRUE;
    }
  }
#وإلا
  (فراغ)داده;
  (فراغ)co;
  (فراغ)domain;
  DEBUGF(infof(داده, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#پایان_اگر
  أرجع FALSE;
}

/* returns TRUE when replaced */
ساكن bool replace_existing(بنية Curl_easy *داده,
                             بنية Cookie *co,
                             ثابت بنية CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  بنية Curl_llist_گره *replace_n = عدم;
  بنية Curl_llist_گره *n;
  size_t myhash = cookiehash(co->domain);
  لكل(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_گره_بعدی(n)) {
    بنية Cookie *clist = Curl_گره_elem(n);
    إذا(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      إذا(clist->domain && co->domain) {
        إذا(cookie_tailmatch(clist->domain, طول_رشته(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, طول_رشته(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      وإلا إذا(!clist->domain && !co->domain)
        matching_domains = TRUE;

      إذا(matching_domains && /* the domains were identical */
         clist->مسیر && co->مسیر && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ثابت حرف *sep = عدم;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->مسیر[0]);
        إذا(clist->مسیر[0])
          sep = strchr(clist->مسیر + 1, '/');
        إذا(sep)
          cllen = sep - clist->مسیر;
        وإلا
          cllen = طول_رشته(clist->مسیر);

        إذا(!مقایسه_رشته(clist->مسیر, co->مسیر, cllen)) {
          infof(داده, "cookie '%s' for domain '%s' dropped, would "
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

        إذا(clist->مسیر && co->مسیر &&
           strcmp(clist->مسیر, co->مسیر))
          replace_old = FALSE;
        وإلا إذا(!clist->مسیر != !co->مسیر)
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
    بنية Cookie *repl = Curl_گره_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_گره_remove(replace_n);

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
CURLcode Curl_cookie_جمع_کن(بنية Curl_easy *داده,
                         بنية CookieInfo *ci,
                         ثابت حرف *lineptr, /* first character of the line */
                         ثابت حرف *domain,  /* default domain */
                         ثابت حرف *مسیر,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ثابت صحيح flags)
{
  بنية Cookie comem;
  بنية Cookie *co;
  size_t myhash;
  CURLcode نتیجه;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(داده);
  DEBUGASSERT(MAX_مجموعه_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  إذا(داده->req.setcookies >= MAX_مجموعه_COOKIE_AMOUNT)
    أرجع CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, حجم(comem));

  إذا(flags & COOKIE_HTTPHEADER)
    نتیجه = parse_cookie_سرآیند(داده, co, &okay,
                                 lineptr, domain, مسیر, flags & COOKIE_SECURE);
  وإلا
    نتیجه = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  إذا(نتیجه || !okay)
    اذهب fail;

  إذا(co->پیشوند_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    اذهب fail;

  إذا(!(flags & COOKIE_NOPSL) && is_عمومی_suffix(داده, co, domain))
    اذهب fail;

  إذا(co->پیشوند_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    إذا(co->secure && co->مسیر && !strcmp(co->مسیر, "/") && !co->tailmatch)
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
  إذا(!replace_existing(داده, co, ci, flags & COOKIE_SECURE, &replaces))
    اذهب fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, حجم(comem));
  إذا(!co) {
    co = &comem;
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->گره);

  إذا(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(داده, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->مقدار,
          co->domain, co->مسیر, co->expires);

  إذا(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  إذا(co->expires && (co->expires < ci->بعدی_expiration))
    ci->بعدی_expiration = co->expires;

  إذا(flags & COOKIE_HTTPHEADER)
    داده->req.setcookies++;

  أرجع نتیجه;
fail:
  freecookie(co, FALSE);
  أرجع نتیجه;
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
  لكل(i = 0; i < COOKIE_HASH_اندازه; i++)
    Curl_llist_init(&ci->cookielist[i], عدم);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->بعدی_expiration = CURL_OFF_T_MAX;

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
ساكن CURLcode cookie_بارگیری(بنية Curl_easy *داده, ثابت حرف *file,
                            بنية CookieInfo *ci, صحيح flags)
{
  FILE *handle = عدم;
  CURLcode نتیجه = CURLE_OK;
  FILE *fp = عدم;
  DEBUGASSERT(ci);
  DEBUGASSERT(داده);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  إذا(file && *file) {
    إذا(!strcmp(file, "-"))
      fp = stdin;
    وإلا {
      fp = curlx_fopen(file, "rb");
      إذا(!fp)
        infof(داده, "WARNING: failed to open cookie file \"%s\"", file);
      وإلا {
        curlx_struct_stat stat;
        إذا((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_نما)) {
          curlx_fclose(fp);
          fp = عدم;
          infof(داده, "WARNING: cookie filename points to a directory: \"%s\"",
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
      نتیجه = Curl_get_سطر(&buf, fp, &eof);
      إذا(!نتیجه) {
        ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        إذا(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        نتیجه = Curl_cookie_جمع_کن(داده, ci, lineptr, عدم, عدم,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        إذا(نتیجه != CURLE_بیرون_از_حافظه)
          نتیجه = CURLE_OK;
      }
    } طالما(!نتیجه && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    إذا(handle)
      curlx_fclose(handle);
  }
  داده->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  أرجع نتیجه;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(بنية Curl_easy *داده,
                               صحيح flags)
{
  CURLcode نتیجه = CURLE_OK;
  بنية curl_slist *list = داده->state.cookielist;
  إذا(list) {
    Curl_share_قفل(داده, CURL_قفل_داده_COOKIE, CURL_قفل_ACCESS_SINGLE);
    إذا(!داده->cookies)
      داده->cookies = Curl_cookie_init();
    إذا(!داده->cookies)
      نتیجه = CURLE_بیرون_از_حافظه;
    وإلا {
      داده->state.cookie_engine = TRUE;
      طالما(list) {
        نتیجه = cookie_بارگیری(داده, list->داده, داده->cookies, flags);
        إذا(نتیجه)
          اكسر;
        list = list->بعدی;
      }
    }
    Curl_share_unlock(داده, CURL_قفل_داده_COOKIE);
  }
  أرجع نتیجه;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ساكن صحيح cookie_مرتب_سازی(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->مسیر ? طول_رشته(c1->مسیر) : 0;
  l2 = c2->مسیر ? طول_رشته(c2->مسیر) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? طول_رشته(c1->domain) : 0;
  l2 = c2->domain ? طول_رشته(c2->domain) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? طول_رشته(c1->name) : 0;
  l2 = c2->name ? طول_رشته(c2->name) : 0;

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
ساكن صحيح cookie_مرتب_سازی_ct(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;

  أرجع (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(بنية Curl_easy *داده, ثابت حرف *host)
{
  أرجع Curl_xfer_is_secure(داده) ||
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
CURLcode Curl_cookie_getlist(بنية Curl_easy *داده,
                             bool *okay,
                             ثابت حرف *host,
                             بنية Curl_llist *list)
{
  size_t matches = 0;
  ثابت bool is_ip = Curl_host_is_ipnum(host);
  ثابت size_t myhash = cookiehash(host);
  بنية Curl_llist_گره *n;
  ثابت bool secure = Curl_secure_context(داده, host);
  بنية CookieInfo *ci = داده->cookies;
  ثابت حرف *مسیر = داده->state.به_بالا.مسیر;
  CURLcode نتیجه = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, عدم);

  إذا(!ci || !Curl_llist_شمار(&ci->cookielist[myhash]))
    أرجع CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  لكل(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_گره_بعدی(n)) {
    بنية Cookie *co = Curl_گره_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    إذا(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      إذا(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, طول_رشته(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        إذا(!co->مسیر || pathmatch(co->مسیر, مسیر)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          إذا(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(داده, "Included max number of cookies (%zu) in request!",
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
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب fail;
    }

    n = Curl_llist_سر(list);

    لكل(i = 0; n; n = Curl_گره_بعدی(n))
      array[i++] = Curl_گره_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, حجم(بنية Cookie *), cookie_مرتب_سازی);

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
  أرجع نتیجه; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
فراغ Curl_cookie_clearall(بنية CookieInfo *ci)
{
  إذا(ci) {
    بیعلامت صحيح i;
    لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
      بنية Curl_llist_گره *n;
      لكل(n = Curl_llist_سر(&ci->cookielist[i]); n;) {
        بنية Cookie *c = Curl_گره_elem(n);
        بنية Curl_llist_گره *e = Curl_گره_بعدی(n);
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
فراغ Curl_cookie_clearsess(بنية CookieInfo *ci)
{
  بیعلامت صحيح i;

  إذا(!ci)
    أرجع;

  لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
    بنية Curl_llist_گره *n = Curl_llist_سر(&ci->cookielist[i]);
    بنية Curl_llist_گره *e = عدم;

    لكل(; n; n = e) {
      بنية Cookie *curr = Curl_گره_elem(n);
      e = Curl_گره_بعدی(n); /* in case the node is removed, get it early */
      إذا(!curr->expires) {
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
    "%s\t"             /*مسیر*/
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
    co->مسیر ? co->مسیر : "/",
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
ساكن CURLcode cookie_output(بنية Curl_easy *داده,
                              بنية CookieInfo *ci,
                              ثابت حرف *filename)
{
  FILE *بیرون = عدم;
  bool use_stdout = FALSE;
  حرف *tempstore = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا(!ci)
    /* no cookie engine alive */
    أرجع CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  إذا(!strcmp("-", filename)) {
    /* use stdout */
    بیرون = stdout;
    use_stdout = TRUE;
  }
  وإلا {
    نتیجه = Curl_fopen(داده, filename, &بیرون, &tempstore);
    إذا(نتیجه)
      اذهب error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        بیرون);

  إذا(ci->numcookies) {
    بیعلامت صحيح i;
    size_t nvalid = 0;
    بنية Cookie **array;
    بنية Curl_llist_گره *n;

    array = curlx_calloc(1, حجم(بنية Cookie *) * ci->numcookies);
    إذا(!array) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب error;
    }

    /* only sort the cookies with a domain property */
    لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
      لكل(n = Curl_llist_سر(&ci->cookielist[i]); n; n = Curl_گره_بعدی(n)) {
        بنية Cookie *co = Curl_گره_elem(n);
        إذا(!co->domain)
          استمر;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, حجم(بنية Cookie *), cookie_مرتب_سازی_ct);

    لكل(i = 0; i < nvalid; i++) {
      حرف *format_ptr = get_netscape_format(array[i]);
      إذا(!format_ptr) {
        curlx_free(array);
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب error;
      }
      curl_mfprintf(بیرون, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  إذا(!use_stdout) {
    curlx_fclose(بیرون);
    بیرون = عدم;
    إذا(tempstore && curlx_rename(tempstore, filename)) {
      نتیجه = CURLE_WRITE_ERROR;
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
  إذا(بیرون && !use_stdout)
    curlx_fclose(بیرون);
  إذا(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  أرجع نتیجه;
}

ساكن بنية curl_slist *cookie_list(ثابت بنية Curl_easy *داده)
{
  بنية curl_slist *list = عدم;
  بنية curl_slist *beg;
  بنية curl_slist **دم = &list;
  بیعلامت صحيح i;
  بنية Curl_llist_گره *n;

  إذا(!داده->cookies || (داده->cookies->numcookies == 0))
    أرجع عدم;

  /* at first, remove expired cookies */
  remove_expired(داده->cookies);

  لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
    لكل(n = Curl_llist_سر(&داده->cookies->cookielist[i]); n;
        n = Curl_گره_بعدی(n)) {
      بنية Cookie *c = Curl_گره_elem(n);
      حرف *سطر;
      إذا(!c->domain)
        استمر;
      سطر = get_netscape_format(c);
      إذا(!سطر) {
        curl_slist_free_all(list);
        أرجع عدم;
      }
      beg = Curl_slist_append_nodup(عدم, سطر);
      إذا(!beg) {
        curlx_free(سطر);
        curl_slist_free_all(list);
        أرجع عدم;
      }
      *دم = beg;
      دم = &beg->بعدی;
    }
  }

  أرجع list;
}

بنية curl_slist *Curl_cookie_list(بنية Curl_easy *داده)
{
  بنية curl_slist *list;
  Curl_share_قفل(داده, CURL_قفل_داده_COOKIE, CURL_قفل_ACCESS_SINGLE);
  list = cookie_list(داده);
  Curl_share_unlock(داده, CURL_قفل_داده_COOKIE);
  أرجع list;
}

فراغ Curl_flush_cookies(بنية Curl_easy *داده, bool cleanup)
{
  Curl_share_قفل(داده, CURL_قفل_داده_COOKIE, CURL_قفل_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  إذا(داده->cookies) {
    ثابت حرف *cookiejar = CURL_EASY_STR(داده, STRING_COOKIEJAR);
    إذا(cookiejar && داده->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode نتیجه = cookie_output(داده, داده->cookies, cookiejar);
      إذا(نتیجه)
        infof(داده, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(نتیجه));
    }

    إذا(cleanup && (!داده->share || (داده->cookies != داده->share->cookies))) {
      Curl_cookie_cleanup(داده->cookies);
      داده->cookies = عدم;
    }
  }
  Curl_share_unlock(داده, CURL_قفل_داده_COOKIE);
}

فراغ Curl_cookie_پیاپی(بنية Curl_easy *داده)
{
  Curl_share_قفل(داده, CURL_قفل_داده_COOKIE, CURL_قفل_ACCESS_SINGLE);
  إذا(داده->cookies)
    داده->cookies->running = TRUE;
  Curl_share_unlock(داده, CURL_قفل_داده_COOKIE);
}

#پایان_اگر /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
