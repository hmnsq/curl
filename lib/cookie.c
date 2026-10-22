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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

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
#परिभाषय COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
स्थिर शून्य cap_expires(time_t now, संरचना Cookie *co)
{
  यदि(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    यदि(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

स्थिर शून्य freecookie(संरचना Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->मार्गः);
  curlx_free(co->name);
  curlx_free(co->मानम्);
  यदि(maintoo)
    curlx_free(co);
}

स्थिर bool cookie_tailmatch(ध्रुव वर्ण *cookie_domain,
                             ध्रुव size_t cookie_domain_len,
                             ध्रुव वर्ण *hostname)
{
  size_t hostname_len = वाक्यदैर्घ्यम्(hostname);

  यदि(hostname_len < cookie_domain_len)
    प्रत्यागम FALSE;

  यदि(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    प्रत्यागम FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  यदि(hostname_len == cookie_domain_len)
    प्रत्यागम TRUE;
  यदि('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
स्थिर bool pathmatch(ध्रुव वर्ण *cookie_मार्गः, ध्रुव वर्ण *uri_मार्गः)
{
  size_t cookie_मार्गः_len;
  size_t uri_मार्गः_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_मार्गः_len = वाक्यदैर्घ्यम्(cookie_मार्गः);
  यदि(cookie_मार्गः_len == 1) {
    /* cookie_path must be '/' */
    प्रत्यागम TRUE;
  }

  /* #-fragments are already cut off! */
  यदि(वाक्यदैर्घ्यम्(uri_मार्गः) == 0 || uri_मार्गः[0] != '/')
    uri_मार्गः = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_मार्गः_len = वाक्यदैर्घ्यम्(uri_मार्गः);

  यदि(uri_मार्गः_len < cookie_मार्गः_len)
    गच्छ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  यदि(वाक्ये_तुलय(cookie_मार्गः, uri_मार्गः, cookie_मार्गः_len))
    गच्छ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  यदि(cookie_मार्गः_len == uri_मार्गः_len) {
    ret = TRUE;
    गच्छ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  यदि(uri_मार्गः[cookie_मार्गः_len] == '/') {
    ret = TRUE;
    गच्छ pathmatched;
  }

pathmatched:
  प्रत्यागम ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
स्थिर ध्रुव वर्ण *get_शिखरम्_domain(ध्रुव वर्ण * ध्रुव domain, size_t *outlen)
{
  size_t len = 0;
  ध्रुव वर्ण *first = रिक्त, *चरमः;

  यदि(domain) {
    len = वाक्यदैर्घ्यम्(domain);
    चरमः = memrchr(domain, '.', len);
    यदि(चरमः) {
      first = memrchr(domain, '.', (चरमः - domain));
      यदि(first)
        len -= (++first - domain);
    }
  }

  यदि(outlen)
    *outlen = len;

  प्रत्यागम first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#यदि defined(_MSC_VER) && (_MSC_VER == 1900)
#निर्देशः optimize("", off)
#यदि_अन्तः

/*
 * A case-insensitive hash for the cookie domains.
 */
स्थिर size_t cookie_hash_domain(ध्रुव वर्ण *domain, ध्रुव size_t len)
{
  ध्रुव वर्ण *अन्तः = domain + len;
  size_t h = 5381;

  यावत्(domain < अन्तः) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  प्रत्यागम (h % COOKIE_HASH_आकार);
}

#यदि defined(_MSC_VER) && (_MSC_VER == 1900)
#निर्देशः optimize("", on)
#यदि_अन्तः

/*
 * Hash this domain.
 */
स्थिर size_t cookiehash(ध्रुव वर्ण * ध्रुव domain)
{
  ध्रुव वर्ण *शिखरम्;
  size_t len;

  यदि(!domain || Curl_host_is_ipnum(domain))
    प्रत्यागम 0;

  शिखरम् = get_शिखरम्_domain(domain, &len);
  प्रत्यागम cookie_hash_domain(शिखरम्, len);
}

/*
 * cookie path sanitize
 */
स्थिर वर्ण *sanitize_cookie_मार्गः(ध्रुव वर्ण *cookie_मार्गः, size_t len)
{
  /* some sites send path attribute within '"'. */
  यदि(len && (cookie_मार्गः[0] == '\"')) {
    cookie_मार्गः++;
    len--;

    यदि(len && (cookie_मार्गः[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  यदि(!len || (cookie_मार्गः[0] != '/'))
    /* Let cookie-path be the default-path. */
    प्रत्यागम curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  यदि(len > 1 && cookie_मार्गः[len - 1] == '/')
    len--;

  प्रत्यागम curlx_memdup0(cookie_मार्गः, len);
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
  यदि(!*str)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  प्रत्यागम CURLE_OK;
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
  अचिह्नित पूर्णाङ्क i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  यदि(now < ci->अग्र_expiration &&
     ci->अग्र_expiration != CURL_OFF_T_MAX)
    प्रत्यागम;
  अन्यथा
    ci->अग्र_expiration = CURL_OFF_T_MAX;

  प्रति(i = 0; i < COOKIE_HASH_आकार; i++) {
    संरचना Curl_llist_ग्रन्थिः *n;
    संरचना Curl_llist_ग्रन्थिः *e = रिक्त;

    प्रति(n = Curl_llist_शीर्ष(&ci->cookielist[i]); n; n = e) {
      co = Curl_ग्रन्थिः_elem(n);
      e = Curl_ग्रन्थिः_अग्र(n);
      यदि(co->expires) {
        यदि(co->expires < now) {
          Curl_ग्रन्थिः_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        अन्यथा यदि(co->expires < ci->अग्र_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->अग्र_expiration = co->expires;
      }
    }
  }
}

#यदि_अपरिभाषितम् USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
स्थिर bool bad_domain(ध्रुव वर्ण *domain, size_t len)
{
  यदि((len == 9) && curl_strnequal(domain, "localhost", 9))
    प्रत्यागम FALSE;
  अन्यथा {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ध्रुव वर्ण *dot = memchr(domain, '.', len);
    यदि(dot) {
      size_t i = dot - domain;
      यदि((len - i) > 1)
        /* the dot is not the last byte */
        प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
}
#यदि_अन्तः

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
स्थिर bool invalid_octets(ध्रुव वर्ण *ptr, size_t len)
{
  ध्रुव अचिह्नित वर्ण *p = (ध्रुव अचिह्नित वर्ण *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  यावत्(len && *p) {
    यदि((*p < 0x20) || (*p == 0x7f))
      प्रत्यागम TRUE;
    p++;
    len--;
  }
  प्रत्यागम FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#परिभाषय MAX_DATE_दीर्घता 80

#परिभाषय COOKIE_NAME   0
#परिभाषय COOKIE_मानम्  1
#परिभाषय COOKIE_DOMAIN 2
#परिभाषय COOKIE_मार्गः   3

#परिभाषय COOKIE_PIECES 4 /* the list above */

स्थिर CURLcode storecookie(संरचना Cookie *co, ध्रुव संरचना Curl_str *cp,
                            ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *domain)
{
  CURLcode परिणाम;
  परिणाम = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  यदि(!परिणाम)
    परिणाम = strstore(&co->मानम्, curlx_str(&cp[COOKIE_मानम्]),
                      curlx_strlen(&cp[COOKIE_मानम्]));
  यदि(!परिणाम) {
    size_t plen = 0;
    यदि(curlx_strlen(&cp[COOKIE_मार्गः])) {
      मार्गः = curlx_str(&cp[COOKIE_मार्गः]);
      plen = curlx_strlen(&cp[COOKIE_मार्गः]);
    }
    अन्यथा यदि(मार्गः) {
      /* No path was given in the header line, set the default */
      ध्रुव वर्ण *endslash = strrchr(मार्गः, '/');
      यदि(endslash)
        plen = endslash - मार्गः + 1; /* include end slash */
      अन्यथा
        plen = वाक्यदैर्घ्यम्(मार्गः);
    }

    यदि(मार्गः) {
      co->मार्गः = sanitize_cookie_मार्गः(मार्गः, plen);
      यदि(!co->मार्गः)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  यदि(!परिणाम) {
    यदि(curlx_strlen(&cp[COOKIE_DOMAIN]))
      परिणाम = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    अन्यथा यदि(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      यदि(!co->domain)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  प्रत्यागम परिणाम;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
स्थिर bool parse_first_pair(संरचना Curl_easy *दत्त, संरचना Cookie *co,
                             संरचना Curl_str *cookie,
                             संरचना Curl_str *name,
                             संरचना Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  यदि(!sep || !curlx_strlen(name)) {
    infof(दत्त, "invalid cookie, dropped");
    प्रत्यागम FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  यदि((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(दत्त, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    प्रत्यागम FALSE;
  }

  /* Check if we have a reserved prefix set. */
  यदि(!वाक्ये_तुलय("__Secure-", curlx_str(name), 9))
    co->उपसर्गः_secure = TRUE;
  अन्यथा यदि(!वाक्ये_तुलय("__Host-", curlx_str(name), 7))
    co->उपसर्गः_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_मानम्] = *val;
  प्रत्यागम TRUE;
}

स्थिर bool parse_domain(संरचना Curl_easy *दत्त, संरचना Cookie *co,
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

  यदि('.' == *v)
    curlx_str_nudge(val, 1);

#यदि_अपरिभाषितम् USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  यदि(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#यदि_अन्तः

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  यदि(!domain ||
     (is_ip &&
      !वाक्ये_तुलय(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == वाक्यदैर्घ्यम्(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    यदि(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  अन्यथा {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(दत्त, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर शून्य parse_maxage(संरचना Cookie *co, संरचना Curl_str *val,
                         time_t *nowp)
{
  पूर्णाङ्क rc;
  ध्रुव वर्ण *maxage = curlx_str(val);
  यदि(*maxage == '\"')
    maxage++;
  rc = curlx_str_सङ्ख्या(&maxage, &co->expires, CURL_OFF_T_MAX);
  यदि(!*nowp)
    *nowp = time(रिक्त);
  विकल्प(rc) {
  पक्ष STRE_अतिप्रवाहः:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    भङ्ग;
  स्वभाव:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    भङ्ग;
  पक्ष STRE_OK:
    यदि(!co->expires)
      co->expires = 1; /* expire now */
    अन्यथा यदि(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    अन्यथा
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
  यदि(!co->expires && (curlx_strlen(val) < MAX_DATE_दीर्घता)) {
    वर्ण dbuf[MAX_DATE_दीर्घता + 1];
    time_t date = 0;
    स्मृतिं_प्रतिलिख(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    यदि(!Curl_getdate_capped(dbuf, &date)) {
      यदि(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    अन्यथा
      co->expires = 0;
    यदि(!*nowp)
      *nowp = time(रिक्त);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
स्थिर CURLcode
parse_cookie_शीर्षकम्(संरचना Curl_easy *दत्त,
                    संरचना Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ध्रुव वर्ण *ptr, /* the header */
                    ध्रुव वर्ण *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ध्रुव वर्ण *मार्गः,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = वाक्यदैर्घ्यम्(ptr);
  CURLcode परिणाम = CURLE_OK;
  संरचना Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  यदि(linelength > MAX_COOKIE_पङ्क्तिः)
    /* discard overly long lines at once */
    प्रत्यागम CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  स्मृतिं_पूरय(cookie, 0, परिमाण(cookie));
  कुरु {
    संरचना Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    पूर्णाङ्क rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    यदि(!rc || (rc == STRE_SHORT)) {
      संरचना Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      यदि(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(दत्त, "invalid octets in name, cookie dropped");
        प्रत्यागम CURLE_OK;
      }

      यदि(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        यदि(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        यदि(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(दत्त, "invalid octets in value, cookie dropped");
          प्रत्यागम CURLE_OK;
        }
      }
      अन्यथा
        curlx_str_init(&val);

      यदि(!curlx_strlen(&cookie[COOKIE_NAME])) {
        यदि(!parse_first_pair(दत्त, co, cookie, &name, &val, sep))
          प्रत्यागम CURLE_OK;
      }

      /* whether there's separator or not */
      अन्यथा यदि(curlx_str_casecompare(&name, "secure")) {
        यदि(secure_origin)
          co->secure = TRUE;
        अन्यथा {
          infof(दत्त, "skipped cookie because not 'secure'");
          प्रत्यागम CURLE_OK;
        }
      }
      अन्यथा यदि(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      अन्यथा यदि(sep) {
        /* only if a separating equals sign is present */
        यदि(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_मार्गः] = val;
        अन्यथा यदि(curlx_str_casecompare(&name, "domain")) {
          यदि(curlx_strlen(&val) &&
             !parse_domain(दत्त, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            प्रत्यागम CURLE_OK;
        }
        अन्यथा यदि(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        अन्यथा यदि(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } यावत्(!curlx_str_single(&ptr, ';'));

  यदि(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    परिणाम = storecookie(co, &cookie[0], मार्गः, domain);
    यदि(!परिणाम)
      *okay = TRUE;
  }
  प्रत्यागम परिणाम;
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
  ध्रुव वर्ण *ptr, *अग्र;
  पूर्णाङ्क fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  यदि(!वाक्ये_तुलय(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  यदि(lineptr[0] == '#')
    /* do not even try the comments */
    प्रत्यागम CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  प्रति(अग्र = lineptr; अग्र; fields++) {
    ptr = अग्र;
    len = strcspn(ptr, "\t\r\n");
    अग्र = (ptr[len] == '\t' ? &ptr[len + 1] : रिक्त);
    विकल्प(fields) {
    पक्ष 0:
      यदि(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      यदि(!co->domain)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
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
      यदि(वाक्ये_तुलय("TRUE", ptr, len) && वाक्ये_तुलय("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->मार्गः = sanitize_cookie_मार्गः(ptr, len);
        यदि(!co->मार्गः)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        भङ्ग;
      }
      अन्यथा {
        /* this does not look like a path, make one up! */
        co->मार्गः = curlx_strdup("/");
        यदि(!co->मार्गः)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    पक्ष 3:
      co->secure = FALSE;
      यदि(curl_strnequal(ptr, "TRUE", len)) {
        यदि(secure_origin || ci->running)
          co->secure = TRUE;
        अन्यथा
          प्रत्यागम CURLE_OK;
      }
      भङ्ग;
    पक्ष 4:
      यदि(curlx_str_सङ्ख्या(&ptr, &co->expires, CURL_OFF_T_MAX))
        प्रत्यागम CURLE_OK;
      भङ्ग;
    पक्ष 5:
      co->name = curlx_memdup0(ptr, len);
      यदि(!co->name)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      अन्यथा {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        यदि(!वाक्ये_तुलय("__Secure-", co->name, 9))
          co->उपसर्गः_secure = TRUE;
        अन्यथा यदि(!वाक्ये_तुलय("__Host-", co->name, 7))
          co->उपसर्गः_host = TRUE;
      }
      भङ्ग;
    पक्ष 6:
      co->मानम् = curlx_memdup0(ptr, len);
      यदि(!co->मानम्)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      भङ्ग;
    }
  }
  यदि(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->मानम् = curlx_strdup("");
    यदि(!co->मानम्)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा
      fields++;
  }

  यदि(fields != 7)
    /* we did not find the sufficient number of fields */
    प्रत्यागम CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  यदि(invalid_octets(co->name, वाक्यदैर्घ्यम्(co->name)) ||
     invalid_octets(co->मानम्, वाक्यदैर्घ्यम्(co->मानम्)))
    प्रत्यागम CURLE_OK;

  *okay = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर bool is_लौकिकम्_suffix(संरचना Curl_easy *दत्त,
                             संरचना Cookie *co,
                             ध्रुव वर्ण *domain)
{
#यदि_परिभाषितम् USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(दत्त);
  DEBUGASSERT(co);
  DEBUGF(infof(दत्त, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  यदि(!co->domain || Curl_host_is_ipnum(co->domain))
    प्रत्यागम FALSE;

  अन्यथा {
    bool acceptable = FALSE;
    वर्ण lcase[256];
    वर्ण lcookie[256];
    size_t dlen = domain ? वाक्यदैर्घ्यम्(domain) : 0;
    size_t clen = वाक्यदैर्घ्यम्(co->domain);

    /* trim trailing dots */
    यदि(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    यदि(clen && (co->domain[clen - 1] == '.'))
      clen--;

    यदि((dlen < परिमाण(lcase)) && (clen < परिमाण(lcookie))) {
      ध्रुव psl_ctx_t *psl = Curl_psl_use(दत्त);
      यदि(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        यदि(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          यदि(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_लौकिकम्_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        अन्यथा {
          bool suffix = psl_is_लौकिकम्_suffix(psl, lcookie);
          acceptable = TRUE;
          यदि(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(दत्त);
      }
      अन्यथा
        infof(दत्त, "libpsl problem, rejecting cookie for safety");
    }

    यदि(!acceptable) {
      infof(दत्त, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      प्रत्यागम TRUE;
    }
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)co;
  (शून्य)domain;
  DEBUGF(infof(दत्त, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#यदि_अन्तः
  प्रत्यागम FALSE;
}

/* returns TRUE when replaced */
स्थिर bool replace_existing(संरचना Curl_easy *दत्त,
                             संरचना Cookie *co,
                             ध्रुव संरचना CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  संरचना Curl_llist_ग्रन्थिः *replace_n = रिक्त;
  संरचना Curl_llist_ग्रन्थिः *n;
  size_t myhash = cookiehash(co->domain);
  प्रति(n = Curl_llist_शीर्ष(&ci->cookielist[myhash]); n; n = Curl_ग्रन्थिः_अग्र(n)) {
    संरचना Cookie *clist = Curl_ग्रन्थिः_elem(n);
    यदि(!वाक्यं_तुलय(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      यदि(clist->domain && co->domain) {
        यदि(cookie_tailmatch(clist->domain, वाक्यदैर्घ्यम्(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, वाक्यदैर्घ्यम्(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      अन्यथा यदि(!clist->domain && !co->domain)
        matching_domains = TRUE;

      यदि(matching_domains && /* the domains were identical */
         clist->मार्गः && co->मार्गः && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ध्रुव वर्ण *sep = रिक्त;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->मार्गः[0]);
        यदि(clist->मार्गः[0])
          sep = strchr(clist->मार्गः + 1, '/');
        यदि(sep)
          cllen = sep - clist->मार्गः;
        अन्यथा
          cllen = वाक्यदैर्घ्यम्(clist->मार्गः);

        यदि(!वाक्ये_तुलय(clist->मार्गः, co->मार्गः, cllen)) {
          infof(दत्त, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          प्रत्यागम FALSE;
        }
      }
    }

    यदि(!replace_n && !वाक्यं_तुलय(clist->name, co->name)) {
      /* the names are identical */

      यदि(clist->domain && co->domain) {
        यदि(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      अन्यथा यदि(!clist->domain && !co->domain)
        replace_old = TRUE;

      यदि(replace_old) {
        /* the domains were identical */

        यदि(clist->मार्गः && co->मार्गः &&
           वाक्यं_तुलय(clist->मार्गः, co->मार्गः))
          replace_old = FALSE;
        अन्यथा यदि(!clist->मार्गः != !co->मार्गः)
          replace_old = FALSE;
      }

      यदि(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        प्रत्यागम FALSE;
      }
      यदि(replace_old)
        replace_n = n;
    }
  }
  यदि(replace_n) {
    संरचना Cookie *repl = Curl_ग्रन्थिः_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ग्रन्थिः_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  प्रत्यागम TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_सङ्कलय(संरचना Curl_easy *दत्त,
                         संरचना CookieInfo *ci,
                         ध्रुव वर्ण *lineptr, /* first character of the line */
                         ध्रुव वर्ण *domain,  /* default domain */
                         ध्रुव वर्ण *मार्गः,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ध्रुव पूर्णाङ्क flags)
{
  संरचना Cookie comem;
  संरचना Cookie *co;
  size_t myhash;
  CURLcode परिणाम;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(MAX_समुच्चयः_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  यदि(दत्त->req.setcookies >= MAX_समुच्चयः_COOKIE_AMOUNT)
    प्रत्यागम CURLE_OK; /* silently ignore */

  co = &comem;
  स्मृतिं_पूरय(co, 0, परिमाण(comem));

  यदि(flags & COOKIE_HTTPHEADER)
    परिणाम = parse_cookie_शीर्षकम्(दत्त, co, &okay,
                                 lineptr, domain, मार्गः, flags & COOKIE_SECURE);
  अन्यथा
    परिणाम = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  यदि(परिणाम || !okay)
    गच्छ fail;

  यदि(co->उपसर्गः_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    गच्छ fail;

  यदि(!(flags & COOKIE_NOPSL) && is_लौकिकम्_suffix(दत्त, co, domain))
    गच्छ fail;

  यदि(co->उपसर्गः_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    यदि(co->secure && co->मार्गः && !वाक्यं_तुलय(co->मार्गः, "/") && !co->tailmatch)
      ;
    अन्यथा
      गच्छ fail;
  }

  यदि(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    गच्छ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  यदि(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  यदि(!replace_existing(दत्त, co, ci, flags & COOKIE_SECURE, &replaces))
    गच्छ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, परिमाण(comem));
  यदि(!co) {
    co = &comem;
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ग्रन्थिः);

  यदि(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(दत्त, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->मानम्,
          co->domain, co->मार्गः, co->expires);

  यदि(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  यदि(co->expires && (co->expires < ci->अग्र_expiration))
    ci->अग्र_expiration = co->expires;

  यदि(flags & COOKIE_HTTPHEADER)
    दत्त->req.setcookies++;

  प्रत्यागम परिणाम;
fail:
  freecookie(co, FALSE);
  प्रत्यागम परिणाम;
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
  यदि(!ci)
    प्रत्यागम रिक्त;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  प्रति(i = 0; i < COOKIE_HASH_आकार; i++)
    Curl_llist_init(&ci->cookielist[i], रिक्त);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->अग्र_expiration = CURL_OFF_T_MAX;

  प्रत्यागम ci;
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
स्थिर CURLcode cookie_आरोपय(संरचना Curl_easy *दत्त, ध्रुव वर्ण *file,
                            संरचना CookieInfo *ci, पूर्णाङ्क flags)
{
  FILE *handle = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  FILE *fp = रिक्त;
  DEBUGASSERT(ci);
  DEBUGASSERT(दत्त);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  यदि(file && *file) {
    यदि(!वाक्यं_तुलय(file, "-"))
      fp = stdin;
    अन्यथा {
      fp = curlx_fopen(file, "rb");
      यदि(!fp)
        infof(दत्त, "WARNING: failed to open cookie file \"%s\"", file);
      अन्यथा {
        curlx_struct_stat stat;
        यदि((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_बहुलकम्)) {
          curlx_fclose(fp);
          fp = रिक्त;
          infof(दत्त, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        अन्यथा
          handle = fp;
      }
    }
  }

  यदि(fp) {
    संरचना dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    कुरु {
      परिणाम = Curl_get_पङ्क्तिः(&buf, fp, &eof);
      यदि(!परिणाम) {
        ध्रुव वर्ण *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        यदि(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        परिणाम = Curl_cookie_सङ्कलय(दत्त, ci, lineptr, रिक्त, रिक्त,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        यदि(परिणाम != CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
          परिणाम = CURLE_OK;
      }
    } यावत्(!परिणाम && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    यदि(handle)
      curlx_fclose(handle);
  }
  दत्त->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  प्रत्यागम परिणाम;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(संरचना Curl_easy *दत्त,
                               पूर्णाङ्क flags)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना curl_slist *list = दत्त->state.cookielist;
  यदि(list) {
    Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_COOKIE, CURL_तालकम्_ACCESS_SINGLE);
    यदि(!दत्त->cookies)
      दत्त->cookies = Curl_cookie_init();
    यदि(!दत्त->cookies)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा {
      दत्त->state.cookie_engine = TRUE;
      यावत्(list) {
        परिणाम = cookie_आरोपय(दत्त, list->दत्त, दत्त->cookies, flags);
        यदि(परिणाम)
          भङ्ग;
        list = list->अग्र;
      }
    }
    Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_COOKIE);
  }
  प्रत्यागम परिणाम;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
स्थिर पूर्णाङ्क cookie_क्रमय(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
  ध्रुव संरचना Cookie *c1 = *(ध्रुव संरचना Cookie * ध्रुव *)p1;
  ध्रुव संरचना Cookie *c2 = *(ध्रुव संरचना Cookie * ध्रुव *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->मार्गः ? वाक्यदैर्घ्यम्(c1->मार्गः) : 0;
  l2 = c2->मार्गः ? वाक्यदैर्घ्यम्(c2->मार्गः) : 0;

  यदि(l1 != l2)
    प्रत्यागम (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? वाक्यदैर्घ्यम्(c1->domain) : 0;
  l2 = c2->domain ? वाक्यदैर्घ्यम्(c2->domain) : 0;

  यदि(l1 != l2)
    प्रत्यागम (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? वाक्यदैर्घ्यम्(c1->name) : 0;
  l2 = c2->name ? वाक्यदैर्घ्यम्(c2->name) : 0;

  यदि(l1 != l2)
    प्रत्यागम (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  प्रत्यागम (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
स्थिर पूर्णाङ्क cookie_क्रमय_ct(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
  ध्रुव संरचना Cookie *c1 = *(ध्रुव संरचना Cookie * ध्रुव *)p1;
  ध्रुव संरचना Cookie *c2 = *(ध्रुव संरचना Cookie * ध्रुव *)p2;

  प्रत्यागम (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(संरचना Curl_easy *दत्त, ध्रुव वर्ण *host)
{
  प्रत्यागम Curl_xfer_is_secure(दत्त) ||
    curl_strequal("localhost", host) ||
    !वाक्यं_तुलय(host, "127.0.0.1") ||
    !वाक्यं_तुलय(host, "::1");
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
CURLcode Curl_cookie_getlist(संरचना Curl_easy *दत्त,
                             bool *okay,
                             ध्रुव वर्ण *host,
                             संरचना Curl_llist *list)
{
  size_t matches = 0;
  ध्रुव bool is_ip = Curl_host_is_ipnum(host);
  ध्रुव size_t myhash = cookiehash(host);
  संरचना Curl_llist_ग्रन्थिः *n;
  ध्रुव bool secure = Curl_secure_context(दत्त, host);
  संरचना CookieInfo *ci = दत्त->cookies;
  ध्रुव वर्ण *मार्गः = दत्त->state.ऊर्ध्वम्.मार्गः;
  CURLcode परिणाम = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, रिक्त);

  यदि(!ci || !Curl_llist_गणनम्(&ci->cookielist[myhash]))
    प्रत्यागम CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  प्रति(n = Curl_llist_शीर्ष(&ci->cookielist[myhash]); n; n = Curl_ग्रन्थिः_अग्र(n)) {
    संरचना Cookie *co = Curl_ग्रन्थिः_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    यदि(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      यदि(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, वाक्यदैर्घ्यम्(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        यदि(!co->मार्गः || pathmatch(co->मार्गः, मार्गः)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          यदि(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(दत्त, "Included max number of cookies (%zu) in request!",
                  matches);
            भङ्ग;
          }
        }
      }
    }
  }

  यदि(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    संरचना Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(परिमाण(संरचना Cookie *) * matches);
    यदि(!array) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ fail;
    }

    n = Curl_llist_शीर्ष(list);

    प्रति(i = 0; n; n = Curl_ग्रन्थिः_अग्र(n))
      array[i++] = Curl_ग्रन्थिः_elem(n);

    /* now sort the cookie pointers in path length order */
    शीघ्रं_क्रमय(array, matches, परिमाण(संरचना Cookie *), cookie_क्रमय);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, रिक्त);

    प्रति(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  प्रत्यागम CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, रिक्त);
  प्रत्यागम परिणाम; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
शून्य Curl_cookie_clearall(संरचना CookieInfo *ci)
{
  यदि(ci) {
    अचिह्नित पूर्णाङ्क i;
    प्रति(i = 0; i < COOKIE_HASH_आकार; i++) {
      संरचना Curl_llist_ग्रन्थिः *n;
      प्रति(n = Curl_llist_शीर्ष(&ci->cookielist[i]); n;) {
        संरचना Cookie *c = Curl_ग्रन्थिः_elem(n);
        संरचना Curl_llist_ग्रन्थिः *e = Curl_ग्रन्थिः_अग्र(n);
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
  अचिह्नित पूर्णाङ्क i;

  यदि(!ci)
    प्रत्यागम;

  प्रति(i = 0; i < COOKIE_HASH_आकार; i++) {
    संरचना Curl_llist_ग्रन्थिः *n = Curl_llist_शीर्ष(&ci->cookielist[i]);
    संरचना Curl_llist_ग्रन्थिः *e = रिक्त;

    प्रति(; n; n = e) {
      संरचना Cookie *curr = Curl_ग्रन्थिः_elem(n);
      e = Curl_ग्रन्थिः_अग्र(n); /* in case the node is removed, get it early */
      यदि(!curr->expires) {
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
  यदि(ci) {
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
  प्रत्यागम curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /*मार्गः*/
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
    co->मार्गः ? co->मार्गः : "/",
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
स्थिर CURLcode cookie_output(संरचना Curl_easy *दत्त,
                              संरचना CookieInfo *ci,
                              ध्रुव वर्ण *filename)
{
  FILE *बहिर्गमनम् = रिक्त;
  bool use_stdout = FALSE;
  वर्ण *tempstore = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(!ci)
    /* no cookie engine alive */
    प्रत्यागम CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  यदि(!वाक्यं_तुलय("-", filename)) {
    /* use stdout */
    बहिर्गमनम् = stdout;
    use_stdout = TRUE;
  }
  अन्यथा {
    परिणाम = Curl_fopen(दत्त, filename, &बहिर्गमनम्, &tempstore);
    यदि(परिणाम)
      गच्छ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        बहिर्गमनम्);

  यदि(ci->numcookies) {
    अचिह्नित पूर्णाङ्क i;
    size_t nvalid = 0;
    संरचना Cookie **array;
    संरचना Curl_llist_ग्रन्थिः *n;

    array = curlx_calloc(1, परिमाण(संरचना Cookie *) * ci->numcookies);
    यदि(!array) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ error;
    }

    /* only sort the cookies with a domain property */
    प्रति(i = 0; i < COOKIE_HASH_आकार; i++) {
      प्रति(n = Curl_llist_शीर्ष(&ci->cookielist[i]); n; n = Curl_ग्रन्थिः_अग्र(n)) {
        संरचना Cookie *co = Curl_ग्रन्थिः_elem(n);
        यदि(!co->domain)
          अनुवर्त;
        array[nvalid++] = co;
      }
    }

    शीघ्रं_क्रमय(array, nvalid, परिमाण(संरचना Cookie *), cookie_क्रमय_ct);

    प्रति(i = 0; i < nvalid; i++) {
      वर्ण *format_ptr = get_netscape_format(array[i]);
      यदि(!format_ptr) {
        curlx_free(array);
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ error;
      }
      curl_mfprintf(बहिर्गमनम्, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  यदि(!use_stdout) {
    curlx_fclose(बहिर्गमनम्);
    बहिर्गमनम् = रिक्त;
    यदि(tempstore && curlx_rename(tempstore, filename)) {
      परिणाम = CURLE_WRITE_ERROR;
      गच्छ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  प्रत्यागम CURLE_OK;

error:
  यदि(बहिर्गमनम् && !use_stdout)
    curlx_fclose(बहिर्गमनम्);
  यदि(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  प्रत्यागम परिणाम;
}

स्थिर संरचना curl_slist *cookie_list(ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना curl_slist *list = रिक्त;
  संरचना curl_slist *beg;
  संरचना curl_slist **पुच्छ = &list;
  अचिह्नित पूर्णाङ्क i;
  संरचना Curl_llist_ग्रन्थिः *n;

  यदि(!दत्त->cookies || (दत्त->cookies->numcookies == 0))
    प्रत्यागम रिक्त;

  /* at first, remove expired cookies */
  remove_expired(दत्त->cookies);

  प्रति(i = 0; i < COOKIE_HASH_आकार; i++) {
    प्रति(n = Curl_llist_शीर्ष(&दत्त->cookies->cookielist[i]); n;
        n = Curl_ग्रन्थिः_अग्र(n)) {
      संरचना Cookie *c = Curl_ग्रन्थिः_elem(n);
      वर्ण *पङ्क्तिः;
      यदि(!c->domain)
        अनुवर्त;
      पङ्क्तिः = get_netscape_format(c);
      यदि(!पङ्क्तिः) {
        curl_slist_free_all(list);
        प्रत्यागम रिक्त;
      }
      beg = Curl_slist_append_nodup(रिक्त, पङ्क्तिः);
      यदि(!beg) {
        curlx_free(पङ्क्तिः);
        curl_slist_free_all(list);
        प्रत्यागम रिक्त;
      }
      *पुच्छ = beg;
      पुच्छ = &beg->अग्र;
    }
  }

  प्रत्यागम list;
}

संरचना curl_slist *Curl_cookie_list(संरचना Curl_easy *दत्त)
{
  संरचना curl_slist *list;
  Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_COOKIE, CURL_तालकम्_ACCESS_SINGLE);
  list = cookie_list(दत्त);
  Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_COOKIE);
  प्रत्यागम list;
}

शून्य Curl_flush_cookies(संरचना Curl_easy *दत्त, bool cleanup)
{
  Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_COOKIE, CURL_तालकम्_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  यदि(दत्त->cookies) {
    ध्रुव वर्ण *cookiejar = CURL_EASY_STR(दत्त, STRING_COOKIEJAR);
    यदि(cookiejar && दत्त->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode परिणाम = cookie_output(दत्त, दत्त->cookies, cookiejar);
      यदि(परिणाम)
        infof(दत्त, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(परिणाम));
    }

    यदि(cleanup && (!दत्त->share || (दत्त->cookies != दत्त->share->cookies))) {
      Curl_cookie_cleanup(दत्त->cookies);
      दत्त->cookies = रिक्त;
    }
  }
  Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_COOKIE);
}

शून्य Curl_cookie_सततम्(संरचना Curl_easy *दत्त)
{
  Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_COOKIE, CURL_तालकम्_ACCESS_SINGLE);
  यदि(दत्त->cookies)
    दत्त->cookies->running = TRUE;
  Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_COOKIE);
}

#यदि_अन्तः /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
