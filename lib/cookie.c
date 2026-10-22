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
#تعریف_کړه COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ساكن فراغ cap_expires(time_t now, بنية Cookie *co)
{
  إذا(co->expires && (وخت_T_MAX - COOKIES_MAXAGE - 30) > now) {
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
  curlx_free(co->لار);
  curlx_free(co->name);
  curlx_free(co->ارزښت);
  إذا(maintoo)
    curlx_free(co);
}

ساكن bool cookie_tailmatch(ثابت حرف *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت حرف *hostname)
{
  size_t hostname_len = مزي_اوږدوالی(hostname);

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
ساكن bool pathmatch(ثابت حرف *cookie_لار, ثابت حرف *uri_لار)
{
  size_t cookie_لار_len;
  size_t uri_لار_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_لار_len = مزي_اوږدوالی(cookie_لار);
  إذا(cookie_لار_len == 1) {
    /* cookie_path must be '/' */
    أرجع TRUE;
  }

  /* #-fragments are already cut off! */
  إذا(مزي_اوږدوالی(uri_لار) == 0 || uri_لار[0] != '/')
    uri_لار = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_لار_len = مزي_اوږدوالی(uri_لار);

  إذا(uri_لار_len < cookie_لار_len)
    اذهب pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  إذا(مزي_پرتله(cookie_لار, uri_لار, cookie_لار_len))
    اذهب pathmatched;

  /* The cookie-path and the uri-path are identical. */
  إذا(cookie_لار_len == uri_لار_len) {
    ret = TRUE;
    اذهب pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  إذا(uri_لار[cookie_لار_len] == '/') {
    ret = TRUE;
    اذهب pathmatched;
  }

pathmatched:
  أرجع ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ساكن ثابت حرف *get_سر_domain(ثابت حرف * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت حرف *لومړی = عدم, *وروستی;

  إذا(domain) {
    len = مزي_اوږدوالی(domain);
    وروستی = memrchr(domain, '.', len);
    إذا(وروستی) {
      لومړی = memrchr(domain, '.', (وروستی - domain));
      إذا(لومړی)
        len -= (++لومړی - domain);
    }
  }

  إذا(outlen)
    *outlen = len;

  أرجع لومړی ? لومړی : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#لارښوونه optimize("", off)
#د_که_پای

/*
 * A case-insensitive hash for the cookie domains.
 */
ساكن size_t cookie_hash_domain(ثابت حرف *domain, ثابت size_t len)
{
  ثابت حرف *پای_ټکی = domain + len;
  size_t h = 5381;

  طالما(domain < پای_ټکی) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  أرجع (h % COOKIE_HASH_اندازه);
}

#إذا defined(_MSC_VER) && (_MSC_VER == 1900)
#لارښوونه optimize("", on)
#د_که_پای

/*
 * Hash this domain.
 */
ساكن size_t cookiehash(ثابت حرف * ثابت domain)
{
  ثابت حرف *سر_2;
  size_t len;

  إذا(!domain || Curl_host_is_ipnum(domain))
    أرجع 0;

  سر_2 = get_سر_domain(domain, &len);
  أرجع cookie_hash_domain(سر_2, len);
}

/*
 * cookie path sanitize
 */
ساكن حرف *sanitize_cookie_لار(ثابت حرف *cookie_لار, size_t len)
{
  /* some sites send path attribute within '"'. */
  إذا(len && (cookie_لار[0] == '\"')) {
    cookie_لار++;
    len--;

    إذا(len && (cookie_لار[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  إذا(!len || (cookie_لار[0] != '/'))
    /* Let cookie-path be the default-path. */
    أرجع curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  إذا(len > 1 && cookie_لار[len - 1] == '/')
    len--;

  أرجع curlx_memdup0(cookie_لار, len);
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
    أرجع CURLE_بهر_د_حافظه;
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
  curl_off_t now = (curl_off_t)وخت(عدم);
  بینښه صحيح i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  إذا(now < ci->بل_expiration &&
     ci->بل_expiration != CURL_OFF_T_MAX)
    أرجع;
  وإلا
    ci->بل_expiration = CURL_OFF_T_MAX;

  لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
    بنية Curl_llist_غوټه *n;
    بنية Curl_llist_غوټه *e = عدم;

    لكل(n = Curl_llist_سر(&ci->cookielist[i]); n; n = e) {
      co = Curl_غوټه_elem(n);
      e = Curl_غوټه_بل(n);
      إذا(co->expires) {
        إذا(co->expires < now) {
          Curl_غوټه_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        وإلا إذا(co->expires < ci->بل_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->بل_expiration = co->expires;
      }
    }
  }
}

#که_نه_تعریف_شوی USE_LIBPSL
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
#د_که_پای

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ساكن bool invalid_octets(ثابت حرف *ptr, size_t len)
{
  ثابت بینښه حرف *p = (ثابت بینښه حرف *)ptr;
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
#تعریف_کړه MAX_DATE_اوږدتیا 80

#تعریف_کړه COOKIE_NAME   0
#تعریف_کړه COOKIE_ارزښت  1
#تعریف_کړه COOKIE_DOMAIN 2
#تعریف_کړه COOKIE_لار   3

#تعریف_کړه COOKIE_PIECES 4 /* the list above */

ساكن CURLcode storecookie(بنية Cookie *co, ثابت بنية Curl_str *cp,
                            ثابت حرف *لار, ثابت حرف *domain)
{
  CURLcode پایله;
  پایله = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  إذا(!پایله)
    پایله = strstore(&co->ارزښت, curlx_str(&cp[COOKIE_ارزښت]),
                      curlx_strlen(&cp[COOKIE_ارزښت]));
  إذا(!پایله) {
    size_t plen = 0;
    إذا(curlx_strlen(&cp[COOKIE_لار])) {
      لار = curlx_str(&cp[COOKIE_لار]);
      plen = curlx_strlen(&cp[COOKIE_لار]);
    }
    وإلا إذا(لار) {
      /* No path was given in the header line, set the default */
      ثابت حرف *endslash = strrchr(لار, '/');
      إذا(endslash)
        plen = endslash - لار + 1; /* include end slash */
      وإلا
        plen = مزي_اوږدوالی(لار);
    }

    إذا(لار) {
      co->لار = sanitize_cookie_لار(لار, plen);
      إذا(!co->لار)
        پایله = CURLE_بهر_د_حافظه;
    }
  }
  إذا(!پایله) {
    إذا(curlx_strlen(&cp[COOKIE_DOMAIN]))
      پایله = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    وإلا إذا(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      إذا(!co->domain)
        پایله = CURLE_بهر_د_حافظه;
    }
  }
  أرجع پایله;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ساكن bool parse_لومړی_pair(بنية Curl_easy *معلومات, بنية Cookie *co,
                             بنية Curl_str *cookie,
                             بنية Curl_str *name,
                             بنية Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  إذا(!sep || !curlx_strlen(name)) {
    infof(معلومات, "invalid cookie, dropped");
    أرجع FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  إذا((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(معلومات, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    أرجع FALSE;
  }

  /* Check if we have a reserved prefix set. */
  إذا(!مزي_پرتله("__Secure-", curlx_str(name), 9))
    co->مختاړی_secure = TRUE;
  وإلا إذا(!مزي_پرتله("__Host-", curlx_str(name), 7))
    co->مختاړی_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_ارزښت] = *val;
  أرجع TRUE;
}

ساكن bool parse_domain(بنية Curl_easy *معلومات, بنية Cookie *co,
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

#که_نه_تعریف_شوی USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  إذا(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#د_که_پای

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  إذا(!domain ||
     (is_ip &&
      !مزي_پرتله(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == مزي_اوږدوالی(domain))) ||
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
    infof(معلومات, "skipped cookie with bad tailmatch domain: %s",
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
  rc = curlx_str_شمېره(&maxage, &co->expires, CURL_OFF_T_MAX);
  إذا(!*nowp)
    *nowp = وخت(عدم);
  بدل(rc) {
  حالة STRE_ډکېدنه:
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
  إذا(!co->expires && (curlx_strlen(val) < MAX_DATE_اوږدتیا)) {
    حرف dbuf[MAX_DATE_اوږدتیا + 1];
    time_t date = 0;
    حافظه_نقل(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    إذا(!Curl_getdate_capped(dbuf, &date)) {
      إذا(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    وإلا
      co->expires = 0;
    إذا(!*nowp)
      *nowp = وخت(عدم);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ساكن CURLcode
parse_cookie_سرلیک(بنية Curl_easy *معلومات,
                    بنية Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ثابت حرف *ptr, /* the header */
                    ثابت حرف *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ثابت حرف *لار,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = مزي_اوږدوالی(ptr);
  CURLcode پایله = CURLE_OK;
  بنية Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  إذا(linelength > MAX_COOKIE_کرښه)
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
        infof(معلومات, "invalid octets in name, cookie dropped");
        أرجع CURLE_OK;
      }

      إذا(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        إذا(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        إذا(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(معلومات, "invalid octets in value, cookie dropped");
          أرجع CURLE_OK;
        }
      }
      وإلا
        curlx_str_init(&val);

      إذا(!curlx_strlen(&cookie[COOKIE_NAME])) {
        إذا(!parse_لومړی_pair(معلومات, co, cookie, &name, &val, sep))
          أرجع CURLE_OK;
      }

      /* whether there's separator or not */
      وإلا إذا(curlx_str_casecompare(&name, "secure")) {
        إذا(secure_origin)
          co->secure = TRUE;
        وإلا {
          infof(معلومات, "skipped cookie because not 'secure'");
          أرجع CURLE_OK;
        }
      }
      وإلا إذا(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      وإلا إذا(sep) {
        /* only if a separating equals sign is present */
        إذا(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_لار] = val;
        وإلا إذا(curlx_str_casecompare(&name, "domain")) {
          إذا(curlx_strlen(&val) &&
             !parse_domain(معلومات, co, &cookie[COOKIE_DOMAIN], &val, &domain))
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
    پایله = storecookie(co, &cookie[0], لار, domain);
    إذا(!پایله)
      *okay = TRUE;
  }
  أرجع پایله;
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
  ثابت حرف *ptr, *بل;
  صحيح fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  إذا(!مزي_پرتله(lineptr, "#HttpOnly_", 10)) {
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
  لكل(بل = lineptr; بل; fields++) {
    ptr = بل;
    len = strcspn(ptr, "\t\r\n");
    بل = (ptr[len] == '\t' ? &ptr[len + 1] : عدم);
    بدل(fields) {
    حالة 0:
      إذا(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      إذا(!co->domain)
        أرجع CURLE_بهر_د_حافظه;
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
      إذا(مزي_پرتله("TRUE", ptr, len) && مزي_پرتله("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->لار = sanitize_cookie_لار(ptr, len);
        إذا(!co->لار)
          أرجع CURLE_بهر_د_حافظه;
        اكسر;
      }
      وإلا {
        /* this does not look like a path, make one up! */
        co->لار = curlx_strdup("/");
        إذا(!co->لار)
          أرجع CURLE_بهر_د_حافظه;
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
      إذا(curlx_str_شمېره(&ptr, &co->expires, CURL_OFF_T_MAX))
        أرجع CURLE_OK;
      اكسر;
    حالة 5:
      co->name = curlx_memdup0(ptr, len);
      إذا(!co->name)
        أرجع CURLE_بهر_د_حافظه;
      وإلا {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        إذا(!مزي_پرتله("__Secure-", co->name, 9))
          co->مختاړی_secure = TRUE;
        وإلا إذا(!مزي_پرتله("__Host-", co->name, 7))
          co->مختاړی_host = TRUE;
      }
      اكسر;
    حالة 6:
      co->ارزښت = curlx_memdup0(ptr, len);
      إذا(!co->ارزښت)
        أرجع CURLE_بهر_د_حافظه;
      اكسر;
    }
  }
  إذا(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->ارزښت = curlx_strdup("");
    إذا(!co->ارزښت)
      أرجع CURLE_بهر_د_حافظه;
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
  إذا(invalid_octets(co->name, مزي_اوږدوالی(co->name)) ||
     invalid_octets(co->ارزښت, مزي_اوږدوالی(co->ارزښت)))
    أرجع CURLE_OK;

  *okay = TRUE;
  أرجع CURLE_OK;
}

ساكن bool is_عامه_suffix(بنية Curl_easy *معلومات,
                             بنية Cookie *co,
                             ثابت حرف *domain)
{
#که_تعریف_شوی USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(معلومات);
  DEBUGASSERT(co);
  DEBUGF(infof(معلومات, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  إذا(!co->domain || Curl_host_is_ipnum(co->domain))
    أرجع FALSE;

  وإلا {
    bool acceptable = FALSE;
    حرف lcase[256];
    حرف lcookie[256];
    size_t dlen = domain ? مزي_اوږدوالی(domain) : 0;
    size_t clen = مزي_اوږدوالی(co->domain);

    /* trim trailing dots */
    إذا(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    إذا(clen && (co->domain[clen - 1] == '.'))
      clen--;

    إذا((dlen < حجم(lcase)) && (clen < حجم(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(معلومات);
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
             psl_is_عامه_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        وإلا {
          bool suffix = psl_is_عامه_suffix(psl, lcookie);
          acceptable = TRUE;
          إذا(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(معلومات);
      }
      وإلا
        infof(معلومات, "libpsl problem, rejecting cookie for safety");
    }

    إذا(!acceptable) {
      infof(معلومات, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      أرجع TRUE;
    }
  }
#وإلا
  (فراغ)معلومات;
  (فراغ)co;
  (فراغ)domain;
  DEBUGF(infof(معلومات, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#د_که_پای
  أرجع FALSE;
}

/* returns TRUE when replaced */
ساكن bool replace_existing(بنية Curl_easy *معلومات,
                             بنية Cookie *co,
                             ثابت بنية CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  بنية Curl_llist_غوټه *replace_n = عدم;
  بنية Curl_llist_غوټه *n;
  size_t myhash = cookiehash(co->domain);
  لكل(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_غوټه_بل(n)) {
    بنية Cookie *clist = Curl_غوټه_elem(n);
    إذا(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      إذا(clist->domain && co->domain) {
        إذا(cookie_tailmatch(clist->domain, مزي_اوږدوالی(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, مزي_اوږدوالی(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      وإلا إذا(!clist->domain && !co->domain)
        matching_domains = TRUE;

      إذا(matching_domains && /* the domains were identical */
         clist->لار && co->لار && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ثابت حرف *sep = عدم;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->لار[0]);
        إذا(clist->لار[0])
          sep = strchr(clist->لار + 1, '/');
        إذا(sep)
          cllen = sep - clist->لار;
        وإلا
          cllen = مزي_اوږدوالی(clist->لار);

        إذا(!مزي_پرتله(clist->لار, co->لار, cllen)) {
          infof(معلومات, "cookie '%s' for domain '%s' dropped, would "
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

        إذا(clist->لار && co->لار &&
           strcmp(clist->لار, co->لار))
          replace_old = FALSE;
        وإلا إذا(!clist->لار != !co->لار)
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
    بنية Cookie *repl = Curl_غوټه_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_غوټه_remove(replace_n);

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
CURLcode Curl_cookie_جمع_کړه(بنية Curl_easy *معلومات,
                         بنية CookieInfo *ci,
                         ثابت حرف *lineptr, /* first character of the line */
                         ثابت حرف *domain,  /* default domain */
                         ثابت حرف *لار,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ثابت صحيح flags)
{
  بنية Cookie comem;
  بنية Cookie *co;
  size_t myhash;
  CURLcode پایله;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(MAX_سېټ_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  إذا(معلومات->req.setcookies >= MAX_سېټ_COOKIE_AMOUNT)
    أرجع CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, حجم(comem));

  إذا(flags & COOKIE_HTTPHEADER)
    پایله = parse_cookie_سرلیک(معلومات, co, &okay,
                                 lineptr, domain, لار, flags & COOKIE_SECURE);
  وإلا
    پایله = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  إذا(پایله || !okay)
    اذهب fail;

  إذا(co->مختاړی_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    اذهب fail;

  إذا(!(flags & COOKIE_NOPSL) && is_عامه_suffix(معلومات, co, domain))
    اذهب fail;

  إذا(co->مختاړی_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    إذا(co->secure && co->لار && !strcmp(co->لار, "/") && !co->tailmatch)
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
  إذا(!replace_existing(معلومات, co, ci, flags & COOKIE_SECURE, &replaces))
    اذهب fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, حجم(comem));
  إذا(!co) {
    co = &comem;
    پایله = CURLE_بهر_د_حافظه;
    اذهب fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->غوټه);

  إذا(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(معلومات, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->ارزښت,
          co->domain, co->لار, co->expires);

  إذا(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  إذا(co->expires && (co->expires < ci->بل_expiration))
    ci->بل_expiration = co->expires;

  إذا(flags & COOKIE_HTTPHEADER)
    معلومات->req.setcookies++;

  أرجع پایله;
fail:
  freecookie(co, FALSE);
  أرجع پایله;
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
  ci->بل_expiration = CURL_OFF_T_MAX;

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
ساكن CURLcode cookie_بارول(بنية Curl_easy *معلومات, ثابت حرف *file,
                            بنية CookieInfo *ci, صحيح flags)
{
  FILE *handle = عدم;
  CURLcode پایله = CURLE_OK;
  FILE *fp = عدم;
  DEBUGASSERT(ci);
  DEBUGASSERT(معلومات);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  إذا(file && *file) {
    إذا(!strcmp(file, "-"))
      fp = stdin;
    وإلا {
      fp = curlx_fopen(file, "rb");
      إذا(!fp)
        infof(معلومات, "WARNING: failed to open cookie file \"%s\"", file);
      وإلا {
        curlx_struct_stat stat;
        إذا((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_ډېرپېښ)) {
          curlx_fclose(fp);
          fp = عدم;
          infof(معلومات, "WARNING: cookie filename points to a directory: \"%s\"",
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
      پایله = Curl_get_کرښه(&buf, fp, &eof);
      إذا(!پایله) {
        ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        إذا(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        پایله = Curl_cookie_جمع_کړه(معلومات, ci, lineptr, عدم, عدم,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        إذا(پایله != CURLE_بهر_د_حافظه)
          پایله = CURLE_OK;
      }
    } طالما(!پایله && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    إذا(handle)
      curlx_fclose(handle);
  }
  معلومات->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  أرجع پایله;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(بنية Curl_easy *معلومات,
                               صحيح flags)
{
  CURLcode پایله = CURLE_OK;
  بنية curl_slist *list = معلومات->state.cookielist;
  إذا(list) {
    Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
    إذا(!معلومات->cookies)
      معلومات->cookies = Curl_cookie_init();
    إذا(!معلومات->cookies)
      پایله = CURLE_بهر_د_حافظه;
    وإلا {
      معلومات->state.cookie_engine = TRUE;
      طالما(list) {
        پایله = cookie_بارول(معلومات, list->معلومات, معلومات->cookies, flags);
        إذا(پایله)
          اكسر;
        list = list->بل;
      }
    }
    Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
  }
  أرجع پایله;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ساكن صحيح cookie_ترتیبول(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->لار ? مزي_اوږدوالی(c1->لار) : 0;
  l2 = c2->لار ? مزي_اوږدوالی(c2->لار) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? مزي_اوږدوالی(c1->domain) : 0;
  l2 = c2->domain ? مزي_اوږدوالی(c2->domain) : 0;

  إذا(l1 != l2)
    أرجع (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? مزي_اوږدوالی(c1->name) : 0;
  l2 = c2->name ? مزي_اوږدوالی(c2->name) : 0;

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
ساكن صحيح cookie_ترتیبول_ct(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية Cookie *c1 = *(ثابت بنية Cookie * ثابت *)p1;
  ثابت بنية Cookie *c2 = *(ثابت بنية Cookie * ثابت *)p2;

  أرجع (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(بنية Curl_easy *معلومات, ثابت حرف *host)
{
  أرجع Curl_xfer_is_secure(معلومات) ||
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
CURLcode Curl_cookie_getlist(بنية Curl_easy *معلومات,
                             bool *okay,
                             ثابت حرف *host,
                             بنية Curl_llist *list)
{
  size_t matches = 0;
  ثابت bool is_ip = Curl_host_is_ipnum(host);
  ثابت size_t myhash = cookiehash(host);
  بنية Curl_llist_غوټه *n;
  ثابت bool secure = Curl_secure_context(معلومات, host);
  بنية CookieInfo *ci = معلومات->cookies;
  ثابت حرف *لار = معلومات->state.پورته.لار;
  CURLcode پایله = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, عدم);

  إذا(!ci || !Curl_llist_شمېر(&ci->cookielist[myhash]))
    أرجع CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  لكل(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_غوټه_بل(n)) {
    بنية Cookie *co = Curl_غوټه_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    إذا(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      إذا(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, مزي_اوږدوالی(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        إذا(!co->لار || pathmatch(co->لار, لار)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          إذا(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(معلومات, "Included max number of cookies (%zu) in request!",
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
      پایله = CURLE_بهر_د_حافظه;
      اذهب fail;
    }

    n = Curl_llist_سر(list);

    لكل(i = 0; n; n = Curl_غوټه_بل(n))
      array[i++] = Curl_غوټه_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, حجم(بنية Cookie *), cookie_ترتیبول);

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
  أرجع پایله; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
فراغ Curl_cookie_clearall(بنية CookieInfo *ci)
{
  إذا(ci) {
    بینښه صحيح i;
    لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
      بنية Curl_llist_غوټه *n;
      لكل(n = Curl_llist_سر(&ci->cookielist[i]); n;) {
        بنية Cookie *c = Curl_غوټه_elem(n);
        بنية Curl_llist_غوټه *e = Curl_غوټه_بل(n);
        Curl_غوټه_remove(n);
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
  بینښه صحيح i;

  إذا(!ci)
    أرجع;

  لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
    بنية Curl_llist_غوټه *n = Curl_llist_سر(&ci->cookielist[i]);
    بنية Curl_llist_غوټه *e = عدم;

    لكل(; n; n = e) {
      بنية Cookie *curr = Curl_غوټه_elem(n);
      e = Curl_غوټه_بل(n); /* in case the node is removed, get it early */
      إذا(!curr->expires) {
        Curl_غوټه_remove(n);
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
    "%s\t"             /*لار*/
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*ارزښت*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->لار ? co->لار : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->ارزښت ? co->ارزښت : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ساكن CURLcode cookie_output(بنية Curl_easy *معلومات,
                              بنية CookieInfo *ci,
                              ثابت حرف *filename)
{
  FILE *بهر = عدم;
  bool use_stdout = FALSE;
  حرف *tempstore = عدم;
  CURLcode پایله = CURLE_OK;

  إذا(!ci)
    /* no cookie engine alive */
    أرجع CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  إذا(!strcmp("-", filename)) {
    /* use stdout */
    بهر = stdout;
    use_stdout = TRUE;
  }
  وإلا {
    پایله = Curl_fopen(معلومات, filename, &بهر, &tempstore);
    إذا(پایله)
      اذهب error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        بهر);

  إذا(ci->numcookies) {
    بینښه صحيح i;
    size_t nvalid = 0;
    بنية Cookie **array;
    بنية Curl_llist_غوټه *n;

    array = curlx_calloc(1, حجم(بنية Cookie *) * ci->numcookies);
    إذا(!array) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب error;
    }

    /* only sort the cookies with a domain property */
    لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
      لكل(n = Curl_llist_سر(&ci->cookielist[i]); n; n = Curl_غوټه_بل(n)) {
        بنية Cookie *co = Curl_غوټه_elem(n);
        إذا(!co->domain)
          استمر;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, حجم(بنية Cookie *), cookie_ترتیبول_ct);

    لكل(i = 0; i < nvalid; i++) {
      حرف *format_ptr = get_netscape_format(array[i]);
      إذا(!format_ptr) {
        curlx_free(array);
        پایله = CURLE_بهر_د_حافظه;
        اذهب error;
      }
      curl_mfprintf(بهر, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  إذا(!use_stdout) {
    curlx_fclose(بهر);
    بهر = عدم;
    إذا(tempstore && curlx_rename(tempstore, filename)) {
      پایله = CURLE_WRITE_ERROR;
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
  إذا(بهر && !use_stdout)
    curlx_fclose(بهر);
  إذا(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  أرجع پایله;
}

ساكن بنية curl_slist *cookie_list(ثابت بنية Curl_easy *معلومات)
{
  بنية curl_slist *list = عدم;
  بنية curl_slist *beg;
  بنية curl_slist **لکۍ = &list;
  بینښه صحيح i;
  بنية Curl_llist_غوټه *n;

  إذا(!معلومات->cookies || (معلومات->cookies->numcookies == 0))
    أرجع عدم;

  /* at first, remove expired cookies */
  remove_expired(معلومات->cookies);

  لكل(i = 0; i < COOKIE_HASH_اندازه; i++) {
    لكل(n = Curl_llist_سر(&معلومات->cookies->cookielist[i]); n;
        n = Curl_غوټه_بل(n)) {
      بنية Cookie *c = Curl_غوټه_elem(n);
      حرف *کرښه;
      إذا(!c->domain)
        استمر;
      کرښه = get_netscape_format(c);
      إذا(!کرښه) {
        curl_slist_free_all(list);
        أرجع عدم;
      }
      beg = Curl_slist_append_nodup(عدم, کرښه);
      إذا(!beg) {
        curlx_free(کرښه);
        curl_slist_free_all(list);
        أرجع عدم;
      }
      *لکۍ = beg;
      لکۍ = &beg->بل;
    }
  }

  أرجع list;
}

بنية curl_slist *Curl_cookie_list(بنية Curl_easy *معلومات)
{
  بنية curl_slist *list;
  Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
  list = cookie_list(معلومات);
  Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
  أرجع list;
}

فراغ Curl_flush_cookies(بنية Curl_easy *معلومات, bool cleanup)
{
  Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  إذا(معلومات->cookies) {
    ثابت حرف *cookiejar = CURL_EASY_STR(معلومات, STRING_COOKIEJAR);
    إذا(cookiejar && معلومات->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode پایله = cookie_output(معلومات, معلومات->cookies, cookiejar);
      إذا(پایله)
        infof(معلومات, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(پایله));
    }

    إذا(cleanup && (!معلومات->share || (معلومات->cookies != معلومات->share->cookies))) {
      Curl_cookie_cleanup(معلومات->cookies);
      معلومات->cookies = عدم;
    }
  }
  Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
}

فراغ Curl_cookie_پرلپسې(بنية Curl_easy *معلومات)
{
  Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
  إذا(معلومات->cookies)
    معلومات->cookies->running = TRUE;
  Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
}

#د_که_پای /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
