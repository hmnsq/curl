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
#تعریف_کرو COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ساكن فراغ cap_expires(time_t now, بنية Cookie *co)
{
  إذا(co->expires && (ویلا_T_MAX - COOKIES_MAXAGE - 30) > now) {
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
  curlx_free(co->رستہ);
  curlx_free(co->name);
  curlx_free(co->قدر);
  إذا(maintoo)
    curlx_free(co);
}

ساكن bool cookie_tailmatch(ثابت حرف *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت حرف *hostname)
{
  size_t hostname_len = لڑی_لمبائی(hostname);

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
ساكن bool pathmatch(ثابت حرف *cookie_رستہ, ثابت حرف *uri_رستہ)
{
  size_t cookie_رستہ_len;
  size_t uri_رستہ_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_رستہ_len = لڑی_لمبائی(cookie_رستہ);
  إذا(cookie_رستہ_len == 1) {
    /* cookie_path must be '/' */
    أرجع TRUE;
  }

  /* #-fragments are already cut off! */
  إذا(لڑی_لمبائی(uri_رستہ) == 0 || uri_رستہ[0] != '/')
    uri_رستہ = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_رستہ_len = لڑی_لمبائی(uri_رستہ);

  إذا(uri_رستہ_len < cookie_رستہ_len)
    اذهب pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  إذا(لڑی_مقابلہ(cookie_رستہ, uri_رستہ, cookie_رستہ_len))
    اذهب pathmatched;

  /* The cookie-path and the uri-path are identical. */
  إذا(cookie_رستہ_len == uri_رستہ_len) {
    ret = TRUE;
    اذهب pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  إذا(uri_رستہ[cookie_رستہ_len] == '/') {
    ret = TRUE;
    اذهب pathmatched;
  }

pathmatched:
  أرجع ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ساكن ثابت حرف *get_چوٹی_domain(ثابت حرف * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت حرف *پہلا = عدم, *آخری;

  إذا(domain) {
    len = لڑی_لمبائی(domain);
    آخری = memrchr(domain, '.', len);
    إذا(آخری) {
      پہلا = memrchr(domain, '.', (آخری - domain));
      إذا(پہلا)
        len -= (++پہلا - domain);
    }
  }

  إذا(outlen)
    *outlen = len;

  أرجع پہلا ? پہلا : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#ہدایت optimize("", off)
#جے_انت

/*
 * A case-insensitive hash for the cookie domains.
 */
ساكن size_t cookie_hash_domain(ثابت حرف *domain, ثابت size_t len)
{
  ثابت حرف *سرا = domain + len;
  size_t h = 5381;

  طالما(domain < سرا) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  أرجع (h % COOKIE_HASH_ناپ);
}

#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#ہدایت optimize("", on)
#جے_انت

/*
 * Hash this domain.
 */
ساكن size_t cookiehash(ثابت حرف * ثابت domain)
{
  ثابت حرف *چوٹی;
  size_t len;

  إذا(!domain || Curl_host_is_ipnum(domain))
    أرجع 0;

  چوٹی = get_چوٹی_domain(domain, &len);
  أرجع cookie_hash_domain(چوٹی, len);
}

/*
 * cookie path sanitize
 */
ساكن حرف *sanitize_cookie_رستہ(ثابت حرف *cookie_رستہ, size_t len)
{
  /* some sites send path attribute within '"'. */
  إذا(len && (cookie_رستہ[0] == '\"')) {
    cookie_رستہ++;
    len--;

    إذا(len && (cookie_رستہ[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  إذا(!len || (cookie_رستہ[0] != '/'))
    /* Let cookie-path be the default-path. */
    أرجع curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  إذا(len > 1 && cookie_رستہ[len - 1] == '/')
    len--;

  أرجع curlx_memdup0(cookie_رستہ, len);
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
    أرجع CURLE_باہرلا_دا_حافظہ;
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
  curl_off_t now = (curl_off_t)ویلا(عدم);
  بنانشانی صحيح i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  إذا(now < ci->اگلا_expiration &&
     ci->اگلا_expiration != CURL_OFF_T_MAX)
    أرجع;
  وإلا
    ci->اگلا_expiration = CURL_OFF_T_MAX;

  لكل(i = 0; i < COOKIE_HASH_ناپ; i++) {
    بنية Curl_llist_گرہ *n;
    بنية Curl_llist_گرہ *e = عدم;

    لكل(n = Curl_llist_سر(&ci->cookielist[i]); n; n = e) {
      co = Curl_گرہ_elem(n);
      e = Curl_گرہ_اگلا(n);
      إذا(co->expires) {
        إذا(co->expires < now) {
          Curl_گرہ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        وإلا إذا(co->expires < ci->اگلا_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->اگلا_expiration = co->expires;
      }
    }
  }
}

#جے_بنا_تعریف USE_LIBPSL
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
#جے_انت

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ساكن bool invalid_octets(ثابت حرف *ptr, size_t len)
{
  ثابت بنانشانی حرف *p = (ثابت بنانشانی حرف *)ptr;
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
#تعریف_کرو MAX_DATE_لمائی 80

#تعریف_کرو COOKIE_NAME   0
#تعریف_کرو COOKIE_قدر  1
#تعریف_کرو COOKIE_DOMAIN 2
#تعریف_کرو COOKIE_رستہ   3

#تعریف_کرو COOKIE_PIECES 4 /* the list above */

ساكن CURLcode storecookie(بنية Cookie *co, ثابت بنية Curl_str *cp,
                            ثابت حرف *رستہ, ثابت حرف *domain)
{
  CURLcode نتیجہ;
  نتیجہ = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  إذا(!نتیجہ)
    نتیجہ = strstore(&co->قدر, curlx_str(&cp[COOKIE_قدر]),
                      curlx_strlen(&cp[COOKIE_قدر]));
  إذا(!نتیجہ) {
    size_t plen = 0;
    إذا(curlx_strlen(&cp[COOKIE_رستہ])) {
      رستہ = curlx_str(&cp[COOKIE_رستہ]);
      plen = curlx_strlen(&cp[COOKIE_رستہ]);
    }
    وإلا إذا(رستہ) {
      /* No path was given in the header line, set the default */
      ثابت حرف *endslash = strrchr(رستہ, '/');
      إذا(endslash)
        plen = endslash - رستہ + 1; /* include end slash */
      وإلا
        plen = لڑی_لمبائی(رستہ);
    }

    إذا(رستہ) {
      co->رستہ = sanitize_cookie_رستہ(رستہ, plen);
      إذا(!co->رستہ)
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
    }
  }
  إذا(!نتیجہ) {
    إذا(curlx_strlen(&cp[COOKIE_DOMAIN]))
      نتیجہ = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    وإلا إذا(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      إذا(!co->domain)
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
    }
  }
  أرجع نتیجہ;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ساكن bool parse_پہلا_pair(بنية Curl_easy *انکڑے, بنية Cookie *co,
                             بنية Curl_str *cookie,
                             بنية Curl_str *name,
                             بنية Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  إذا(!sep || !curlx_strlen(name)) {
    infof(انکڑے, "invalid cookie, dropped");
    أرجع FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  إذا((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(انکڑے, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    أرجع FALSE;
  }

  /* Check if we have a reserved prefix set. */
  إذا(!لڑی_مقابلہ("__Secure-", curlx_str(name), 9))
    co->اگیتر_secure = TRUE;
  وإلا إذا(!لڑی_مقابلہ("__Host-", curlx_str(name), 7))
    co->اگیتر_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_قدر] = *val;
  أرجع TRUE;
}

ساكن bool parse_domain(بنية Curl_easy *انکڑے, بنية Cookie *co,
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

#جے_بنا_تعریف USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  إذا(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#جے_انت

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  إذا(!domain ||
     (is_ip &&
      !لڑی_مقابلہ(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == لڑی_لمبائی(domain))) ||
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
    infof(انکڑے, "skipped cookie with bad tailmatch domain: %s",
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
  rc = curlx_str_نمبر(&maxage, &co->expires, CURL_OFF_T_MAX);
  إذا(!*nowp)
    *nowp = ویلا(عدم);
  بدل(rc) {
  حالة STRE_حد_توں_ودھ:
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
  إذا(!co->expires && (curlx_strlen(val) < MAX_DATE_لمائی)) {
    حرف dbuf[MAX_DATE_لمائی + 1];
    time_t date = 0;
    حافظہ_نقل(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    إذا(!Curl_getdate_capped(dbuf, &date)) {
      إذا(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    وإلا
      co->expires = 0;
    إذا(!*nowp)
      *nowp = ویلا(عدم);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ساكن CURLcode
parse_cookie_سرخی(بنية Curl_easy *انکڑے,
                    بنية Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ثابت حرف *ptr, /* the header */
                    ثابت حرف *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ثابت حرف *رستہ,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = لڑی_لمبائی(ptr);
  CURLcode نتیجہ = CURLE_OK;
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
        infof(انکڑے, "invalid octets in name, cookie dropped");
        أرجع CURLE_OK;
      }

      إذا(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        إذا(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        إذا(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(انکڑے, "invalid octets in value, cookie dropped");
          أرجع CURLE_OK;
        }
      }
      وإلا
        curlx_str_init(&val);

      إذا(!curlx_strlen(&cookie[COOKIE_NAME])) {
        إذا(!parse_پہلا_pair(انکڑے, co, cookie, &name, &val, sep))
          أرجع CURLE_OK;
      }

      /* whether there's separator or not */
      وإلا إذا(curlx_str_casecompare(&name, "secure")) {
        إذا(secure_origin)
          co->secure = TRUE;
        وإلا {
          infof(انکڑے, "skipped cookie because not 'secure'");
          أرجع CURLE_OK;
        }
      }
      وإلا إذا(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      وإلا إذا(sep) {
        /* only if a separating equals sign is present */
        إذا(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_رستہ] = val;
        وإلا إذا(curlx_str_casecompare(&name, "domain")) {
          إذا(curlx_strlen(&val) &&
             !parse_domain(انکڑے, co, &cookie[COOKIE_DOMAIN], &val, &domain))
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
    نتیجہ = storecookie(co, &cookie[0], رستہ, domain);
    إذا(!نتیجہ)
      *okay = TRUE;
  }
  أرجع نتیجہ;
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
  ثابت حرف *ptr, *اگلا;
  صحيح fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  إذا(!لڑی_مقابلہ(lineptr, "#HttpOnly_", 10)) {
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
  لكل(اگلا = lineptr; اگلا; fields++) {
    ptr = اگلا;
    len = strcspn(ptr, "\t\r\n");
    اگلا = (ptr[len] == '\t' ? &ptr[len + 1] : عدم);
    بدل(fields) {
    حالة 0:
      إذا(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      إذا(!co->domain)
        أرجع CURLE_باہرلا_دا_حافظہ;
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
      إذا(لڑی_مقابلہ("TRUE", ptr, len) && لڑی_مقابلہ("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->رستہ = sanitize_cookie_رستہ(ptr, len);
        إذا(!co->رستہ)
          أرجع CURLE_باہرلا_دا_حافظہ;
        اكسر;
      }
      وإلا {
        /* this does not look like a path, make one up! */
        co->رستہ = curlx_strdup("/");
        إذا(!co->رستہ)
          أرجع CURLE_باہرلا_دا_حافظہ;
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
      إذا(curlx_str_نمبر(&ptr, &co->expires, CURL_OFF_T_MAX))
        أرجع CURLE_OK;
      اكسر;
    حالة 5:
      co->name = curlx_memdup0(ptr, len);
      إذا(!co->name)
        أرجع CURLE_باہرلا_دا_حافظہ;
      وإلا {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        إذا(!لڑی_مقابلہ("__Secure-", co->name, 9))
          co->اگیتر_secure = TRUE;
        وإلا إذا(!لڑی_مقابلہ("__Host-", co->name, 7))
          co->اگیتر_host = TRUE;
      }
      اكسر;
    حالة 6:
      co->قدر = curlx_memdup0(ptr, len);
      إذا(!co->قدر)
        أرجع CURLE_باہرلا_دا_حافظہ;
      اكسر;
    }
  }
  إذا(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->قدر = curlx_strdup("");
    إذا(!co->قدر)
      أرجع CURLE_باہرلا_دا_حافظہ;
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
  إذا(invalid_octets(co->name, لڑی_لمبائی(co->name)) ||
     invalid_octets(co->قدر, لڑی_لمبائی(co->قدر)))
    أرجع CURLE_OK;

  *okay = TRUE;
  أرجع CURLE_OK;
}

ساكن bool is_عوامی_suffix(بنية Curl_easy *انکڑے,
                             بنية Cookie *co,
                             ثابت حرف *domain)
{
#جے_تعریف_شدہ USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(co);
  DEBUGF(infof(انکڑے, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  إذا(!co->domain || Curl_host_is_ipnum(co->domain))
    أرجع FALSE;

  وإلا {
    bool acceptable = FALSE;
    حرف lcase[256];
    حرف lcookie[256];
    size_t dlen = domain ? لڑی_لمبائی(domain) : 0;
    size_t clen = لڑی_لمبائی(co->domain);

    /* trim trailing dots */
    إذا(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    إذا(clen && (co->domain[clen - 1] == '.'))
      clen--;

    إذا((dlen < حجم(lcase)) && (clen < حجم(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(انکڑے);
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
             psl_is_عوامی_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        وإلا {
          bool suffix = psl_is_عوامی_suffix(psl, lcookie);
          acceptable = TRUE;
          إذا(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(انکڑے);
      }
      وإلا
        infof(انکڑے, "libpsl problem, rejecting cookie for safety");
    }

    إذا(!acceptable) {
      infof(انکڑے, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      أرجع TRUE;
    }
  }
#وإلا
  (فراغ)انکڑے;
  (فراغ)co;
  (فراغ)domain;
  DEBUGF(infof(انکڑے, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#جے_انت
  أرجع FALSE;
}

/* returns TRUE when replaced */
ساكن bool replace_existing(بنية Curl_easy *انکڑے,
                             بنية Cookie *co,
                             ثابت بنية CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  بنية Curl_llist_گرہ *replace_n = عدم;
  بنية Curl_llist_گرہ *n;
  size_t myhash = cookiehash(co->domain);
  لكل(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_گرہ_اگلا(n)) {
    بنية Cookie *clist = Curl_گرہ_elem(n);
    إذا(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      إذا(clist->domain && co->domain) {
        إذا(cookie_tailmatch(clist->domain, لڑی_لمبائی(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, لڑی_لمبائی(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      وإلا إذا(!clist->domain && !co->domain)
        matching_domains = TRUE;

      إذا(matching_domains && /* the domains were identical */
         clist->رستہ && co->رستہ && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ثابت حرف *sep = عدم;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->رستہ[0]);
        إذا(clist->رستہ[0])
          sep = strchr(clist->رستہ + 1, '/');
        إذا(sep)
          cllen = sep - clist->رستہ;
        وإلا
          cllen = لڑی_لمبائی(clist->رستہ);

        إذا(!لڑی_مقابلہ(clist->رستہ, co->رستہ, cllen)) {
          infof(انکڑے, "cookie '%s' for domain '%s' dropped, would "
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

        إذا(clist->رستہ && co->رستہ &&
           strcmp(clist->رستہ, co->رستہ))
          replace_old = FALSE;
        وإلا إذا(!clist->رستہ != !co->رستہ)
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
    بنية Cookie *repl = Curl_گرہ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_گرہ_remove(replace_n);

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
CURLcode Curl_cookie_جوڑو(بنية Curl_easy *انکڑے,
                         بنية CookieInfo *ci,
                         ثابت حرف *lineptr, /* first character of the line */
                         ثابت حرف *domain,  /* default domain */
                         ثابت حرف *رستہ,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ثابت صحيح flags)
{
  بنية Cookie comem;
  بنية Cookie *co;
  size_t myhash;
  CURLcode نتیجہ;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(انکڑے);
  DEBUGASSERT(MAX_سیٹ_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  إذا(انکڑے->req.setcookies >= MAX_سیٹ_COOKIE_AMOUNT)
    أرجع CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, حجم(comem));

  إذا(flags & COOKIE_HTTPHEADER)
    نتیجہ = parse_cookie_سرخی(انکڑے, co, &okay,
                                 lineptr, domain, رستہ, flags & COOKIE_SECURE);
  وإلا
    نتیجہ = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  إذا(نتیجہ || !okay)
    اذهب fail;

  إذا(co->اگیتر_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    اذهب fail;

  إذا(!(flags & COOKIE_NOPSL) && is_عوامی_suffix(انکڑے, co, domain))
    اذهب fail;

  إذا(co->اگیتر_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    إذا(co->secure && co->رستہ && !strcmp(co->رستہ, "/") && !co->tailmatch)
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
  إذا(!replace_existing(انکڑے, co, ci, flags & COOKIE_SECURE, &replaces))
    اذهب fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, حجم(comem));
  إذا(!co) {
    co = &comem;
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->گرہ);

  إذا(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(انکڑے, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->قدر,
          co->domain, co->رستہ, co->expires);

  إذا(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  إذا(co->expires && (co->expires < ci->اگلا_expiration))
    ci->اگلا_expiration = co->expires;

  إذا(flags & COOKIE_HTTPHEADER)
    انکڑے->req.setcookies++;

  أرجع نتیجہ;
fail:
  freecookie(co, FALSE);
  أرجع نتیجہ;
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
  لكل(i = 0; i < COOKIE_HASH_ناپ; i++)
    Curl_llist_init(&ci->cookielist[i], عدم);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->اگلا_expiration = CURL_OFF_T_MAX;

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
ساكن CURLcode cookie_لدو(بنية Curl_easy *انکڑے, ثابت حرف *file,
                            بنية CookieInfo *ci, صحيح flags)
{
  FILE *handle = عدم;
  CURLcode نتیجہ = CURLE_OK;
  FILE *fp = عدم;
  DEBUGASSERT(ci);
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  إذا(file && *file) {
    إذا(!strcmp(file, "-"))
      fp = stdin;
    وإلا {
      fp = curlx_fopen(file, "rb");
      إذا(!fp)
        infof(انکڑے, "WARNING: failed to open cookie file \"%s\"", file);
      وإلا {
        curlx_struct_stat stat;
        إذا((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_منوال)) {
          curlx_fclose(fp);
          fp = عدم;
          infof(انکڑے, "WARNING: cookie filename points to a directory: \"%s\"",
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
      نتیجہ = Curl_get_سطر(&buf, fp, &eof);
      إذا(!نتیجہ) {
        ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        إذا(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        نتیجہ = Curl_cookie_جوڑو(انکڑے, ci, lineptr, عدم, عدم,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        إذا(نتیجہ != CURLE_باہرلا_دا_حافظہ)
          نتیجہ = CURLE_OK;
      }
    } طالما(!نتیجہ && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    إذا(handle)
      curlx_fclose(handle);
  }
  انکڑے->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  أرجع نتیجہ;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(بنية Curl_easy *انکڑے,
                               صحيح flags)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية curl_slist *list = انکڑے->state.cookielist;
  إذا(list) {
    Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_COOKIE, CURL_جندرا_ACCESS_SINGLE);
    إذا(!انکڑے->cookies)
      انکڑے->cookies = Curl_cookie_init();
    إذا(!انکڑے->cookies)
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
    وإلا {
      انکڑے->state.cookie_engine = TRUE;
      طالما(list) {
        نتیجہ = cookie_لدو(انکڑے, list->انکڑے, انکڑے->cookies, flags);
        إذا(نتیجہ)
          اكسر;
        list = list->اگلا;
      }
    }
    Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_COOKIE);
  }
  أرجع نتیجہ;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ساكن صحيح cookie_ترتیب_دیو(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->رستہ ? لڑی_لمبائی(c1->رستہ) : 0;
  l2 = c2->رستہ ? لڑی_لمبائی(c2->رستہ) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? لڑی_لمبائی(c1->domain) : 0;
  l2 = c2->domain ? لڑی_لمبائی(c2->domain) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? لڑی_لمبائی(c1->name) : 0;
  l2 = c2->name ? لڑی_لمبائی(c2->name) : 0;

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
ساكن صحيح cookie_ترتیب_دیو_ct(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;

  أرجع (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(بنية Curl_easy *انکڑے, ثابت حرف *host)
{
  أرجع Curl_xfer_is_secure(انکڑے) ||
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
CURLcode Curl_cookie_getlist(بنية Curl_easy *انکڑے,
                             bool *okay,
                             ثابت حرف *host,
                             بنية Curl_llist *list)
{
  size_t matches = 0;
  ثابت bool is_ip = Curl_host_is_ipnum(host);
  ثابت size_t myhash = cookiehash(host);
  بنية Curl_llist_گرہ *n;
  ثابت bool secure = Curl_secure_context(انکڑے, host);
  بنية CookieInfo *ci = انکڑے->cookies;
  ثابت حرف *رستہ = انکڑے->state.اتے.رستہ;
  CURLcode نتیجہ = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, عدم);

  إذا(!ci || !Curl_llist_گنتی(&ci->cookielist[myhash]))
    أرجع CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  لكل(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_گرہ_اگلا(n)) {
    بنية Cookie *co = Curl_گرہ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    إذا(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      إذا(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, لڑی_لمبائی(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        إذا(!co->رستہ || pathmatch(co->رستہ, رستہ)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          إذا(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(انکڑے, "Included max number of cookies (%zu) in request!",
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
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب fail;
    }

    n = Curl_llist_سر(list);

    لكل(i = 0; n; n = Curl_گرہ_اگلا(n))
      array[i++] = Curl_گرہ_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, حجم(بنية Cookie *), cookie_ترتیب_دیو);

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
  أرجع نتیجہ; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
فراغ Curl_cookie_clearall(بنية CookieInfo *ci)
{
  إذا(ci) {
    بنانشانی صحيح i;
    لكل(i = 0; i < COOKIE_HASH_ناپ; i++) {
      بنية Curl_llist_گرہ *n;
      لكل(n = Curl_llist_سر(&ci->cookielist[i]); n;) {
        بنية Cookie *c = Curl_گرہ_elem(n);
        بنية Curl_llist_گرہ *e = Curl_گرہ_اگلا(n);
        Curl_گرہ_remove(n);
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
  بنانشانی صحيح i;

  إذا(!ci)
    أرجع;

  لكل(i = 0; i < COOKIE_HASH_ناپ; i++) {
    بنية Curl_llist_گرہ *n = Curl_llist_سر(&ci->cookielist[i]);
    بنية Curl_llist_گرہ *e = عدم;

    لكل(; n; n = e) {
      بنية Cookie *curr = Curl_گرہ_elem(n);
      e = Curl_گرہ_اگلا(n); /* in case the node is removed, get it early */
      إذا(!curr->expires) {
        Curl_گرہ_remove(n);
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
    "%s\t"             /*رستہ*/
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*قدر*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->رستہ ? co->رستہ : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->قدر ? co->قدر : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ساكن CURLcode cookie_output(بنية Curl_easy *انکڑے,
                              بنية CookieInfo *ci,
                              ثابت حرف *filename)
{
  FILE *باہرلا = عدم;
  bool use_stdout = FALSE;
  حرف *tempstore = عدم;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!ci)
    /* no cookie engine alive */
    أرجع CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  إذا(!strcmp("-", filename)) {
    /* use stdout */
    باہرلا = stdout;
    use_stdout = TRUE;
  }
  وإلا {
    نتیجہ = Curl_fopen(انکڑے, filename, &باہرلا, &tempstore);
    إذا(نتیجہ)
      اذهب error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        باہرلا);

  إذا(ci->numcookies) {
    بنانشانی صحيح i;
    size_t nvalid = 0;
    بنية Cookie **array;
    بنية Curl_llist_گرہ *n;

    array = curlx_calloc(1, حجم(بنية Cookie *) * ci->numcookies);
    إذا(!array) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب error;
    }

    /* only sort the cookies with a domain property */
    لكل(i = 0; i < COOKIE_HASH_ناپ; i++) {
      لكل(n = Curl_llist_سر(&ci->cookielist[i]); n; n = Curl_گرہ_اگلا(n)) {
        بنية Cookie *co = Curl_گرہ_elem(n);
        إذا(!co->domain)
          استمر;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, حجم(بنية Cookie *), cookie_ترتیب_دیو_ct);

    لكل(i = 0; i < nvalid; i++) {
      حرف *format_ptr = get_netscape_format(array[i]);
      إذا(!format_ptr) {
        curlx_free(array);
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب error;
      }
      curl_mfprintf(باہرلا, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  إذا(!use_stdout) {
    curlx_fclose(باہرلا);
    باہرلا = عدم;
    إذا(tempstore && curlx_rename(tempstore, filename)) {
      نتیجہ = CURLE_WRITE_ERROR;
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
  إذا(باہرلا && !use_stdout)
    curlx_fclose(باہرلا);
  إذا(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  أرجع نتیجہ;
}

ساكن بنية curl_slist *cookie_list(ثابت بنية Curl_easy *انکڑے)
{
  بنية curl_slist *list = عدم;
  بنية curl_slist *beg;
  بنية curl_slist **پوچھ = &list;
  بنانشانی صحيح i;
  بنية Curl_llist_گرہ *n;

  إذا(!انکڑے->cookies || (انکڑے->cookies->numcookies == 0))
    أرجع عدم;

  /* at first, remove expired cookies */
  remove_expired(انکڑے->cookies);

  لكل(i = 0; i < COOKIE_HASH_ناپ; i++) {
    لكل(n = Curl_llist_سر(&انکڑے->cookies->cookielist[i]); n;
        n = Curl_گرہ_اگلا(n)) {
      بنية Cookie *c = Curl_گرہ_elem(n);
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
      *پوچھ = beg;
      پوچھ = &beg->اگلا;
    }
  }

  أرجع list;
}

بنية curl_slist *Curl_cookie_list(بنية Curl_easy *انکڑے)
{
  بنية curl_slist *list;
  Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_COOKIE, CURL_جندرا_ACCESS_SINGLE);
  list = cookie_list(انکڑے);
  Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_COOKIE);
  أرجع list;
}

فراغ Curl_flush_cookies(بنية Curl_easy *انکڑے, bool cleanup)
{
  Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_COOKIE, CURL_جندرا_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  إذا(انکڑے->cookies) {
    ثابت حرف *cookiejar = CURL_EASY_STR(انکڑے, STRING_COOKIEJAR);
    إذا(cookiejar && انکڑے->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode نتیجہ = cookie_output(انکڑے, انکڑے->cookies, cookiejar);
      إذا(نتیجہ)
        infof(انکڑے, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(نتیجہ));
    }

    إذا(cleanup && (!انکڑے->share || (انکڑے->cookies != انکڑے->share->cookies))) {
      Curl_cookie_cleanup(انکڑے->cookies);
      انکڑے->cookies = عدم;
    }
  }
  Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_COOKIE);
}

فراغ Curl_cookie_لگاتار(بنية Curl_easy *انکڑے)
{
  Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_COOKIE, CURL_جندرا_ACCESS_SINGLE);
  إذا(انکڑے->cookies)
    انکڑے->cookies->running = TRUE;
  Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_COOKIE);
}

#جے_انت /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
