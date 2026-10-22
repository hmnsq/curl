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
#تعریف_کرو COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ساکن خالی cap_expires(time_t now, ساخت Cookie *co)
{
  اگر(co->expires && (وقت_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    اگر(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ساکن خالی freecookie(ساخت Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->راستہ);
  curlx_free(co->name);
  curlx_free(co->قدر);
  اگر(maintoo)
    curlx_free(co);
}

ساکن bool cookie_tailmatch(ثابت حرف *cookie_domain,
                             ثابت size_t cookie_domain_len,
                             ثابت حرف *hostname)
{
  size_t hostname_len = لڑی_لمبائی(hostname);

  اگر(hostname_len < cookie_domain_len)
    واپس FALSE;

  اگر(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    واپس FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  اگر(hostname_len == cookie_domain_len)
    واپس TRUE;
  اگر('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    واپس TRUE;
  واپس FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ساکن bool pathmatch(ثابت حرف *cookie_راستہ, ثابت حرف *uri_راستہ)
{
  size_t cookie_راستہ_len;
  size_t uri_راستہ_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_راستہ_len = لڑی_لمبائی(cookie_راستہ);
  اگر(cookie_راستہ_len == 1) {
    /* cookie_path must be '/' */
    واپس TRUE;
  }

  /* #-fragments are already cut off! */
  اگر(لڑی_لمبائی(uri_راستہ) == 0 || uri_راستہ[0] != '/')
    uri_راستہ = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_راستہ_len = لڑی_لمبائی(uri_راستہ);

  اگر(uri_راستہ_len < cookie_راستہ_len)
    جاؤ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  اگر(لڑی_موازنہ(cookie_راستہ, uri_راستہ, cookie_راستہ_len))
    جاؤ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  اگر(cookie_راستہ_len == uri_راستہ_len) {
    ret = TRUE;
    جاؤ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  اگر(uri_راستہ[cookie_راستہ_len] == '/') {
    ret = TRUE;
    جاؤ pathmatched;
  }

pathmatched:
  واپس ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ساکن ثابت حرف *get_چوٹی_domain(ثابت حرف * ثابت domain, size_t *outlen)
{
  size_t len = 0;
  ثابت حرف *پہلا = تہی, *آخری;

  اگر(domain) {
    len = لڑی_لمبائی(domain);
    آخری = memrchr(domain, '.', len);
    اگر(آخری) {
      پہلا = memrchr(domain, '.', (آخری - domain));
      اگر(پہلا)
        len -= (++پہلا - domain);
    }
  }

  اگر(outlen)
    *outlen = len;

  واپس پہلا ? پہلا : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#اگر defined(_MSC_VER) && (_MSC_VER == 1900)
#ہدایت optimize("", off)
#اگر_ختم

/*
 * A case-insensitive hash for the cookie domains.
 */
ساکن size_t cookie_hash_domain(ثابت حرف *domain, ثابت size_t len)
{
  ثابت حرف *انتہا = domain + len;
  size_t h = 5381;

  جب(domain < انتہا) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  واپس (h % COOKIE_HASH_سائز);
}

#اگر defined(_MSC_VER) && (_MSC_VER == 1900)
#ہدایت optimize("", on)
#اگر_ختم

/*
 * Hash this domain.
 */
ساکن size_t cookiehash(ثابت حرف * ثابت domain)
{
  ثابت حرف *چوٹی;
  size_t len;

  اگر(!domain || Curl_host_is_ipnum(domain))
    واپس 0;

  چوٹی = get_چوٹی_domain(domain, &len);
  واپس cookie_hash_domain(چوٹی, len);
}

/*
 * cookie path sanitize
 */
ساکن حرف *sanitize_cookie_راستہ(ثابت حرف *cookie_راستہ, size_t len)
{
  /* some sites send path attribute within '"'. */
  اگر(len && (cookie_راستہ[0] == '\"')) {
    cookie_راستہ++;
    len--;

    اگر(len && (cookie_راستہ[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  اگر(!len || (cookie_راستہ[0] != '/'))
    /* Let cookie-path be the default-path. */
    واپس curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  اگر(len > 1 && cookie_راستہ[len - 1] == '/')
    len--;

  واپس curlx_memdup0(cookie_راستہ, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ساکن CURLcode strstore(حرف **str, ثابت حرف *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  اگر(!*str)
    واپس CURLE_برآمد_کا_حافظہ;
  واپس CURLE_OK;
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
ساکن خالی remove_expired(ساخت CookieInfo *ci)
{
  ساخت Cookie *co;
  curl_off_t now = (curl_off_t)وقت(تہی);
  غیرعلامتی صحیح i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  اگر(now < ci->اگلا_expiration &&
     ci->اگلا_expiration != CURL_OFF_T_MAX)
    واپس;
  ورنہ
    ci->اگلا_expiration = CURL_OFF_T_MAX;

  برائے(i = 0; i < COOKIE_HASH_سائز; i++) {
    ساخت Curl_llist_گرہ *n;
    ساخت Curl_llist_گرہ *e = تہی;

    برائے(n = Curl_llist_سر(&ci->cookielist[i]); n; n = e) {
      co = Curl_گرہ_elem(n);
      e = Curl_گرہ_اگلا(n);
      اگر(co->expires) {
        اگر(co->expires < now) {
          Curl_گرہ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        ورنہ اگر(co->expires < ci->اگلا_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->اگلا_expiration = co->expires;
      }
    }
  }
}

#اگر_غیر_تعریف_شدہ USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
ساکن bool bad_domain(ثابت حرف *domain, size_t len)
{
  اگر((len == 9) && curl_strnequal(domain, "localhost", 9))
    واپس FALSE;
  ورنہ {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ثابت حرف *dot = memchr(domain, '.', len);
    اگر(dot) {
      size_t i = dot - domain;
      اگر((len - i) > 1)
        /* the dot is not the last byte */
        واپس FALSE;
    }
  }
  واپس TRUE;
}
#اگر_ختم

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ساکن bool invalid_octets(ثابت حرف *ptr, size_t len)
{
  ثابت غیرعلامتی حرف *p = (ثابت غیرعلامتی حرف *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  جب(len && *p) {
    اگر((*p < 0x20) || (*p == 0x7f))
      واپس TRUE;
    p++;
    len--;
  }
  واپس FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#تعریف_کرو MAX_DATE_لمبائی 80

#تعریف_کرو COOKIE_NAME   0
#تعریف_کرو COOKIE_قدر  1
#تعریف_کرو COOKIE_DOMAIN 2
#تعریف_کرو COOKIE_راستہ   3

#تعریف_کرو COOKIE_PIECES 4 /* the list above */

ساکن CURLcode storecookie(ساخت Cookie *co, ثابت ساخت Curl_str *cp,
                            ثابت حرف *راستہ, ثابت حرف *domain)
{
  CURLcode نتیجہ;
  نتیجہ = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  اگر(!نتیجہ)
    نتیجہ = strstore(&co->قدر, curlx_str(&cp[COOKIE_قدر]),
                      curlx_strlen(&cp[COOKIE_قدر]));
  اگر(!نتیجہ) {
    size_t plen = 0;
    اگر(curlx_strlen(&cp[COOKIE_راستہ])) {
      راستہ = curlx_str(&cp[COOKIE_راستہ]);
      plen = curlx_strlen(&cp[COOKIE_راستہ]);
    }
    ورنہ اگر(راستہ) {
      /* No path was given in the header line, set the default */
      ثابت حرف *endslash = strrchr(راستہ, '/');
      اگر(endslash)
        plen = endslash - راستہ + 1; /* include end slash */
      ورنہ
        plen = لڑی_لمبائی(راستہ);
    }

    اگر(راستہ) {
      co->راستہ = sanitize_cookie_راستہ(راستہ, plen);
      اگر(!co->راستہ)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
    }
  }
  اگر(!نتیجہ) {
    اگر(curlx_strlen(&cp[COOKIE_DOMAIN]))
      نتیجہ = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    ورنہ اگر(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      اگر(!co->domain)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
    }
  }
  واپس نتیجہ;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ساکن bool parse_پہلا_pair(ساخت Curl_easy *ڈیٹا, ساخت Cookie *co,
                             ساخت Curl_str *cookie,
                             ساخت Curl_str *name,
                             ساخت Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  اگر(!sep || !curlx_strlen(name)) {
    infof(ڈیٹا, "invalid cookie, dropped");
    واپس FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  اگر((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(ڈیٹا, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    واپس FALSE;
  }

  /* Check if we have a reserved prefix set. */
  اگر(!لڑی_موازنہ("__Secure-", curlx_str(name), 9))
    co->سابقہ_secure = TRUE;
  ورنہ اگر(!لڑی_موازنہ("__Host-", curlx_str(name), 7))
    co->سابقہ_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_قدر] = *val;
  واپس TRUE;
}

ساکن bool parse_domain(ساخت Curl_easy *ڈیٹا, ساخت Cookie *co,
                         ساخت Curl_str *cookie_domain,
                         ساخت Curl_str *val,
                         ثابت حرف **domainp)
{
  bool is_ip;
  ثابت حرف *domain = *domainp;
  ثابت حرف *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  اگر('.' == *v)
    curlx_str_nudge(val, 1);

#اگر_غیر_تعریف_شدہ USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  اگر(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#اگر_ختم

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  اگر(!domain ||
     (is_ip &&
      !لڑی_موازنہ(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == لڑی_لمبائی(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    اگر(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  ورنہ {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(ڈیٹا, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    واپس FALSE;
  }
  واپس TRUE;
}

ساکن خالی parse_maxage(ساخت Cookie *co, ساخت Curl_str *val,
                         time_t *nowp)
{
  صحیح rc;
  ثابت حرف *maxage = curlx_str(val);
  اگر(*maxage == '\"')
    maxage++;
  rc = curlx_str_نمبر(&maxage, &co->expires, CURL_OFF_T_MAX);
  اگر(!*nowp)
    *nowp = وقت(تہی);
  انتخاب(rc) {
  حالت STRE_حد_سے_تجاوز:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    توڑو;
  طےشدہ:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    توڑو;
  حالت STRE_OK:
    اگر(!co->expires)
      co->expires = 1; /* expire now */
    ورنہ اگر(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    ورنہ
      co->expires += *nowp;
    توڑو;
  }
  cap_expires(*nowp, co);
}

ساکن خالی parse_expires(ساخت Cookie *co, ساخت Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  اگر(!co->expires && (curlx_strlen(val) < MAX_DATE_لمبائی)) {
    حرف dbuf[MAX_DATE_لمبائی + 1];
    time_t date = 0;
    حافظہ_نقل(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    اگر(!Curl_getdate_capped(dbuf, &date)) {
      اگر(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    ورنہ
      co->expires = 0;
    اگر(!*nowp)
      *nowp = وقت(تہی);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ساکن CURLcode
parse_cookie_سرخی(ساخت Curl_easy *ڈیٹا,
                    ساخت Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ثابت حرف *ptr, /* the header */
                    ثابت حرف *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ثابت حرف *راستہ,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = لڑی_لمبائی(ptr);
  CURLcode نتیجہ = CURLE_OK;
  ساخت Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  اگر(linelength > MAX_COOKIE_سطر)
    /* discard overly long lines at once */
    واپس CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, حجم(cookie));
  کرو {
    ساخت Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    صحیح rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    اگر(!rc || (rc == STRE_SHORT)) {
      ساخت Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      اگر(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(ڈیٹا, "invalid octets in name, cookie dropped");
        واپس CURLE_OK;
      }

      اگر(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        اگر(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        اگر(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(ڈیٹا, "invalid octets in value, cookie dropped");
          واپس CURLE_OK;
        }
      }
      ورنہ
        curlx_str_init(&val);

      اگر(!curlx_strlen(&cookie[COOKIE_NAME])) {
        اگر(!parse_پہلا_pair(ڈیٹا, co, cookie, &name, &val, sep))
          واپس CURLE_OK;
      }

      /* whether there's separator or not */
      ورنہ اگر(curlx_str_casecompare(&name, "secure")) {
        اگر(secure_origin)
          co->secure = TRUE;
        ورنہ {
          infof(ڈیٹا, "skipped cookie because not 'secure'");
          واپس CURLE_OK;
        }
      }
      ورنہ اگر(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      ورنہ اگر(sep) {
        /* only if a separating equals sign is present */
        اگر(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_راستہ] = val;
        ورنہ اگر(curlx_str_casecompare(&name, "domain")) {
          اگر(curlx_strlen(&val) &&
             !parse_domain(ڈیٹا, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            واپس CURLE_OK;
        }
        ورنہ اگر(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        ورنہ اگر(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } جب(!curlx_str_single(&ptr, ';'));

  اگر(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    نتیجہ = storecookie(co, &cookie[0], راستہ, domain);
    اگر(!نتیجہ)
      *okay = TRUE;
  }
  واپس نتیجہ;
}

ساکن CURLcode parse_netscape(ساخت Cookie *co,
                               ثابت ساخت CookieInfo *ci,
                               bool *okay,
                               ثابت حرف *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ثابت حرف *ptr, *اگلا;
  صحیح fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  اگر(!لڑی_موازنہ(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  اگر(lineptr[0] == '#')
    /* do not even try the comments */
    واپس CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  برائے(اگلا = lineptr; اگلا; fields++) {
    ptr = اگلا;
    len = strcspn(ptr, "\t\r\n");
    اگلا = (ptr[len] == '\t' ? &ptr[len + 1] : تہی);
    انتخاب(fields) {
    حالت 0:
      اگر(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      اگر(!co->domain)
        واپس CURLE_برآمد_کا_حافظہ;
      توڑو;
    حالت 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      توڑو;
    حالت 2:
      /* The file format allows the path field to remain not filled in */
      اگر(لڑی_موازنہ("TRUE", ptr, len) && لڑی_موازنہ("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->راستہ = sanitize_cookie_راستہ(ptr, len);
        اگر(!co->راستہ)
          واپس CURLE_برآمد_کا_حافظہ;
        توڑو;
      }
      ورنہ {
        /* this does not look like a path, make one up! */
        co->راستہ = curlx_strdup("/");
        اگر(!co->راستہ)
          واپس CURLE_برآمد_کا_حافظہ;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    حالت 3:
      co->secure = FALSE;
      اگر(curl_strnequal(ptr, "TRUE", len)) {
        اگر(secure_origin || ci->running)
          co->secure = TRUE;
        ورنہ
          واپس CURLE_OK;
      }
      توڑو;
    حالت 4:
      اگر(curlx_str_نمبر(&ptr, &co->expires, CURL_OFF_T_MAX))
        واپس CURLE_OK;
      توڑو;
    حالت 5:
      co->name = curlx_memdup0(ptr, len);
      اگر(!co->name)
        واپس CURLE_برآمد_کا_حافظہ;
      ورنہ {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        اگر(!لڑی_موازنہ("__Secure-", co->name, 9))
          co->سابقہ_secure = TRUE;
        ورنہ اگر(!لڑی_موازنہ("__Host-", co->name, 7))
          co->سابقہ_host = TRUE;
      }
      توڑو;
    حالت 6:
      co->قدر = curlx_memdup0(ptr, len);
      اگر(!co->قدر)
        واپس CURLE_برآمد_کا_حافظہ;
      توڑو;
    }
  }
  اگر(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->قدر = curlx_strdup("");
    اگر(!co->قدر)
      واپس CURLE_برآمد_کا_حافظہ;
    ورنہ
      fields++;
  }

  اگر(fields != 7)
    /* we did not find the sufficient number of fields */
    واپس CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  اگر(invalid_octets(co->name, لڑی_لمبائی(co->name)) ||
     invalid_octets(co->قدر, لڑی_لمبائی(co->قدر)))
    واپس CURLE_OK;

  *okay = TRUE;
  واپس CURLE_OK;
}

ساکن bool is_عوامی_suffix(ساخت Curl_easy *ڈیٹا,
                             ساخت Cookie *co,
                             ثابت حرف *domain)
{
#اگر_تعریف_شدہ USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(co);
  DEBUGF(infof(ڈیٹا, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  اگر(!co->domain || Curl_host_is_ipnum(co->domain))
    واپس FALSE;

  ورنہ {
    bool acceptable = FALSE;
    حرف lcase[256];
    حرف lcookie[256];
    size_t dlen = domain ? لڑی_لمبائی(domain) : 0;
    size_t clen = لڑی_لمبائی(co->domain);

    /* trim trailing dots */
    اگر(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    اگر(clen && (co->domain[clen - 1] == '.'))
      clen--;

    اگر((dlen < حجم(lcase)) && (clen < حجم(lcookie))) {
      ثابت psl_ctx_t *psl = Curl_psl_use(ڈیٹا);
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
             psl_is_عوامی_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        ورنہ {
          bool suffix = psl_is_عوامی_suffix(psl, lcookie);
          acceptable = TRUE;
          اگر(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(ڈیٹا);
      }
      ورنہ
        infof(ڈیٹا, "libpsl problem, rejecting cookie for safety");
    }

    اگر(!acceptable) {
      infof(ڈیٹا, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      واپس TRUE;
    }
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)co;
  (خالی)domain;
  DEBUGF(infof(ڈیٹا, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#اگر_ختم
  واپس FALSE;
}

/* returns TRUE when replaced */
ساکن bool replace_existing(ساخت Curl_easy *ڈیٹا,
                             ساخت Cookie *co,
                             ثابت ساخت CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ساخت Curl_llist_گرہ *replace_n = تہی;
  ساخت Curl_llist_گرہ *n;
  size_t myhash = cookiehash(co->domain);
  برائے(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_گرہ_اگلا(n)) {
    ساخت Cookie *clist = Curl_گرہ_elem(n);
    اگر(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      اگر(clist->domain && co->domain) {
        اگر(cookie_tailmatch(clist->domain, لڑی_لمبائی(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, لڑی_لمبائی(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      ورنہ اگر(!clist->domain && !co->domain)
        matching_domains = TRUE;

      اگر(matching_domains && /* the domains were identical */
         clist->راستہ && co->راستہ && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ثابت حرف *sep = تہی;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->راستہ[0]);
        اگر(clist->راستہ[0])
          sep = strchr(clist->راستہ + 1, '/');
        اگر(sep)
          cllen = sep - clist->راستہ;
        ورنہ
          cllen = لڑی_لمبائی(clist->راستہ);

        اگر(!لڑی_موازنہ(clist->راستہ, co->راستہ, cllen)) {
          infof(ڈیٹا, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          واپس FALSE;
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
      ورنہ اگر(!clist->domain && !co->domain)
        replace_old = TRUE;

      اگر(replace_old) {
        /* the domains were identical */

        اگر(clist->راستہ && co->راستہ &&
           strcmp(clist->راستہ, co->راستہ))
          replace_old = FALSE;
        ورنہ اگر(!clist->راستہ != !co->راستہ)
          replace_old = FALSE;
      }

      اگر(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        واپس FALSE;
      }
      اگر(replace_old)
        replace_n = n;
    }
  }
  اگر(replace_n) {
    ساخت Cookie *repl = Curl_گرہ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_گرہ_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  واپس TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_جوڑو(ساخت Curl_easy *ڈیٹا,
                         ساخت CookieInfo *ci,
                         ثابت حرف *lineptr, /* first character of the line */
                         ثابت حرف *domain,  /* default domain */
                         ثابت حرف *راستہ,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ثابت صحیح flags)
{
  ساخت Cookie comem;
  ساخت Cookie *co;
  size_t myhash;
  CURLcode نتیجہ;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(MAX_سیٹ_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  اگر(ڈیٹا->req.setcookies >= MAX_سیٹ_COOKIE_AMOUNT)
    واپس CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, حجم(comem));

  اگر(flags & COOKIE_HTTPHEADER)
    نتیجہ = parse_cookie_سرخی(ڈیٹا, co, &okay,
                                 lineptr, domain, راستہ, flags & COOKIE_SECURE);
  ورنہ
    نتیجہ = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  اگر(نتیجہ || !okay)
    جاؤ fail;

  اگر(co->سابقہ_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    جاؤ fail;

  اگر(!(flags & COOKIE_NOPSL) && is_عوامی_suffix(ڈیٹا, co, domain))
    جاؤ fail;

  اگر(co->سابقہ_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    اگر(co->secure && co->راستہ && !strcmp(co->راستہ, "/") && !co->tailmatch)
      ;
    ورنہ
      جاؤ fail;
  }

  اگر(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    جاؤ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  اگر(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  اگر(!replace_existing(ڈیٹا, co, ci, flags & COOKIE_SECURE, &replaces))
    جاؤ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, حجم(comem));
  اگر(!co) {
    co = &comem;
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->گرہ);

  اگر(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(ڈیٹا, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->قدر,
          co->domain, co->راستہ, co->expires);

  اگر(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  اگر(co->expires && (co->expires < ci->اگلا_expiration))
    ci->اگلا_expiration = co->expires;

  اگر(flags & COOKIE_HTTPHEADER)
    ڈیٹا->req.setcookies++;

  واپس نتیجہ;
fail:
  freecookie(co, FALSE);
  واپس نتیجہ;
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
ساخت CookieInfo *Curl_cookie_init(خالی)
{
  صحیح i;
  ساخت CookieInfo *ci = curlx_calloc(1, حجم(ساخت CookieInfo));
  اگر(!ci)
    واپس تہی;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  برائے(i = 0; i < COOKIE_HASH_سائز; i++)
    Curl_llist_init(&ci->cookielist[i], تہی);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->اگلا_expiration = CURL_OFF_T_MAX;

  واپس ci;
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
ساکن CURLcode cookie_بھرو(ساخت Curl_easy *ڈیٹا, ثابت حرف *file,
                            ساخت CookieInfo *ci, صحیح flags)
{
  FILE *handle = تہی;
  CURLcode نتیجہ = CURLE_OK;
  FILE *fp = تہی;
  DEBUGASSERT(ci);
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  اگر(file && *file) {
    اگر(!strcmp(file, "-"))
      fp = stdin;
    ورنہ {
      fp = curlx_fopen(file, "rb");
      اگر(!fp)
        infof(ڈیٹا, "WARNING: failed to open cookie file \"%s\"", file);
      ورنہ {
        curlx_struct_stat stat;
        اگر((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_منوال)) {
          curlx_fclose(fp);
          fp = تہی;
          infof(ڈیٹا, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        ورنہ
          handle = fp;
      }
    }
  }

  اگر(fp) {
    ساخت dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    کرو {
      نتیجہ = Curl_get_سطر(&buf, fp, &eof);
      اگر(!نتیجہ) {
        ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        اگر(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        نتیجہ = Curl_cookie_جوڑو(ڈیٹا, ci, lineptr, تہی, تہی,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        اگر(نتیجہ != CURLE_برآمد_کا_حافظہ)
          نتیجہ = CURLE_OK;
      }
    } جب(!نتیجہ && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    اگر(handle)
      curlx_fclose(handle);
  }
  ڈیٹا->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  واپس نتیجہ;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ساخت Curl_easy *ڈیٹا,
                               صحیح flags)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت curl_slist *list = ڈیٹا->state.cookielist;
  اگر(list) {
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
    اگر(!ڈیٹا->cookies)
      ڈیٹا->cookies = Curl_cookie_init();
    اگر(!ڈیٹا->cookies)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
    ورنہ {
      ڈیٹا->state.cookie_engine = TRUE;
      جب(list) {
        نتیجہ = cookie_بھرو(ڈیٹا, list->ڈیٹا, ڈیٹا->cookies, flags);
        اگر(نتیجہ)
          توڑو;
        list = list->اگلا;
      }
    }
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
  }
  واپس نتیجہ;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ساکن صحیح cookie_ترتیب_دو(ثابت خالی *p1, ثابت خالی *p2)
{
  ثابت ساخت Cookie *c1 = *(ثابت ساخت Cookie * ثابت *)p1;
  ثابت ساخت Cookie *c2 = *(ثابت ساخت Cookie * ثابت *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->راستہ ? لڑی_لمبائی(c1->راستہ) : 0;
  l2 = c2->راستہ ? لڑی_لمبائی(c2->راستہ) : 0;

  اگر(l1 != l2)
    واپس (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? لڑی_لمبائی(c1->domain) : 0;
  l2 = c2->domain ? لڑی_لمبائی(c2->domain) : 0;

  اگر(l1 != l2)
    واپس (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? لڑی_لمبائی(c1->name) : 0;
  l2 = c2->name ? لڑی_لمبائی(c2->name) : 0;

  اگر(l1 != l2)
    واپس (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  واپس (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ساکن صحیح cookie_ترتیب_دو_ct(ثابت خالی *p1, ثابت خالی *p2)
{
  ثابت ساخت Cookie *c1 = *(ثابت ساخت Cookie * ثابت *)p1;
  ثابت ساخت Cookie *c2 = *(ثابت ساخت Cookie * ثابت *)p2;

  واپس (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ساخت Curl_easy *ڈیٹا, ثابت حرف *host)
{
  واپس Curl_xfer_is_secure(ڈیٹا) ||
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
CURLcode Curl_cookie_getlist(ساخت Curl_easy *ڈیٹا,
                             bool *okay,
                             ثابت حرف *host,
                             ساخت Curl_llist *list)
{
  size_t matches = 0;
  ثابت bool is_ip = Curl_host_is_ipnum(host);
  ثابت size_t myhash = cookiehash(host);
  ساخت Curl_llist_گرہ *n;
  ثابت bool secure = Curl_secure_context(ڈیٹا, host);
  ساخت CookieInfo *ci = ڈیٹا->cookies;
  ثابت حرف *راستہ = ڈیٹا->state.اوپر.راستہ;
  CURLcode نتیجہ = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, تہی);

  اگر(!ci || !Curl_llist_گنتی(&ci->cookielist[myhash]))
    واپس CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  برائے(n = Curl_llist_سر(&ci->cookielist[myhash]); n; n = Curl_گرہ_اگلا(n)) {
    ساخت Cookie *co = Curl_گرہ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    اگر(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      اگر(!co->domain ||
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
        اگر(!co->راستہ || pathmatch(co->راستہ, راستہ)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          اگر(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(ڈیٹا, "Included max number of cookies (%zu) in request!",
                  matches);
            توڑو;
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
    ساخت Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(حجم(ساخت Cookie *) * matches);
    اگر(!array) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ fail;
    }

    n = Curl_llist_سر(list);

    برائے(i = 0; n; n = Curl_گرہ_اگلا(n))
      array[i++] = Curl_گرہ_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, حجم(ساخت Cookie *), cookie_ترتیب_دو);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, تہی);

    برائے(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  واپس CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, تہی);
  واپس نتیجہ; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
خالی Curl_cookie_clearall(ساخت CookieInfo *ci)
{
  اگر(ci) {
    غیرعلامتی صحیح i;
    برائے(i = 0; i < COOKIE_HASH_سائز; i++) {
      ساخت Curl_llist_گرہ *n;
      برائے(n = Curl_llist_سر(&ci->cookielist[i]); n;) {
        ساخت Cookie *c = Curl_گرہ_elem(n);
        ساخت Curl_llist_گرہ *e = Curl_گرہ_اگلا(n);
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
خالی Curl_cookie_clearsess(ساخت CookieInfo *ci)
{
  غیرعلامتی صحیح i;

  اگر(!ci)
    واپس;

  برائے(i = 0; i < COOKIE_HASH_سائز; i++) {
    ساخت Curl_llist_گرہ *n = Curl_llist_سر(&ci->cookielist[i]);
    ساخت Curl_llist_گرہ *e = تہی;

    برائے(; n; n = e) {
      ساخت Cookie *curr = Curl_گرہ_elem(n);
      e = Curl_گرہ_اگلا(n); /* in case the node is removed, get it early */
      اگر(!curr->expires) {
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
خالی Curl_cookie_cleanup(ساخت CookieInfo *ci)
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
ساکن حرف *get_netscape_format(ثابت ساخت Cookie *co)
{
  واپس curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /*راستہ*/
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
    co->راستہ ? co->راستہ : "/",
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
ساکن CURLcode cookie_output(ساخت Curl_easy *ڈیٹا,
                              ساخت CookieInfo *ci,
                              ثابت حرف *filename)
{
  FILE *برآمد = تہی;
  bool use_stdout = FALSE;
  حرف *tempstore = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!ci)
    /* no cookie engine alive */
    واپس CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  اگر(!strcmp("-", filename)) {
    /* use stdout */
    برآمد = stdout;
    use_stdout = TRUE;
  }
  ورنہ {
    نتیجہ = Curl_fopen(ڈیٹا, filename, &برآمد, &tempstore);
    اگر(نتیجہ)
      جاؤ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        برآمد);

  اگر(ci->numcookies) {
    غیرعلامتی صحیح i;
    size_t nvalid = 0;
    ساخت Cookie **array;
    ساخت Curl_llist_گرہ *n;

    array = curlx_calloc(1, حجم(ساخت Cookie *) * ci->numcookies);
    اگر(!array) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ error;
    }

    /* only sort the cookies with a domain property */
    برائے(i = 0; i < COOKIE_HASH_سائز; i++) {
      برائے(n = Curl_llist_سر(&ci->cookielist[i]); n; n = Curl_گرہ_اگلا(n)) {
        ساخت Cookie *co = Curl_گرہ_elem(n);
        اگر(!co->domain)
          جاری;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, حجم(ساخت Cookie *), cookie_ترتیب_دو_ct);

    برائے(i = 0; i < nvalid; i++) {
      حرف *format_ptr = get_netscape_format(array[i]);
      اگر(!format_ptr) {
        curlx_free(array);
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ error;
      }
      curl_mfprintf(برآمد, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  اگر(!use_stdout) {
    curlx_fclose(برآمد);
    برآمد = تہی;
    اگر(tempstore && curlx_rename(tempstore, filename)) {
      نتیجہ = CURLE_WRITE_ERROR;
      جاؤ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  واپس CURLE_OK;

error:
  اگر(برآمد && !use_stdout)
    curlx_fclose(برآمد);
  اگر(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  واپس نتیجہ;
}

ساکن ساخت curl_slist *cookie_list(ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت curl_slist *list = تہی;
  ساخت curl_slist *beg;
  ساخت curl_slist **دم = &list;
  غیرعلامتی صحیح i;
  ساخت Curl_llist_گرہ *n;

  اگر(!ڈیٹا->cookies || (ڈیٹا->cookies->numcookies == 0))
    واپس تہی;

  /* at first, remove expired cookies */
  remove_expired(ڈیٹا->cookies);

  برائے(i = 0; i < COOKIE_HASH_سائز; i++) {
    برائے(n = Curl_llist_سر(&ڈیٹا->cookies->cookielist[i]); n;
        n = Curl_گرہ_اگلا(n)) {
      ساخت Cookie *c = Curl_گرہ_elem(n);
      حرف *سطر;
      اگر(!c->domain)
        جاری;
      سطر = get_netscape_format(c);
      اگر(!سطر) {
        curl_slist_free_all(list);
        واپس تہی;
      }
      beg = Curl_slist_append_nodup(تہی, سطر);
      اگر(!beg) {
        curlx_free(سطر);
        curl_slist_free_all(list);
        واپس تہی;
      }
      *دم = beg;
      دم = &beg->اگلا;
    }
  }

  واپس list;
}

ساخت curl_slist *Curl_cookie_list(ساخت Curl_easy *ڈیٹا)
{
  ساخت curl_slist *list;
  Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
  list = cookie_list(ڈیٹا);
  Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
  واپس list;
}

خالی Curl_flush_cookies(ساخت Curl_easy *ڈیٹا, bool cleanup)
{
  Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  اگر(ڈیٹا->cookies) {
    ثابت حرف *cookiejar = CURL_EASY_STR(ڈیٹا, STRING_COOKIEJAR);
    اگر(cookiejar && ڈیٹا->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode نتیجہ = cookie_output(ڈیٹا, ڈیٹا->cookies, cookiejar);
      اگر(نتیجہ)
        infof(ڈیٹا, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(نتیجہ));
    }

    اگر(cleanup && (!ڈیٹا->share || (ڈیٹا->cookies != ڈیٹا->share->cookies))) {
      Curl_cookie_cleanup(ڈیٹا->cookies);
      ڈیٹا->cookies = تہی;
    }
  }
  Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
}

خالی Curl_cookie_تسلسل(ساخت Curl_easy *ڈیٹا)
{
  Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
  اگر(ڈیٹا->cookies)
    ڈیٹا->cookies->running = TRUE;
  Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
}

#اگر_ختم /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
