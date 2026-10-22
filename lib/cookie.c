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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cookie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "psl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_get_line.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_memrchr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* number of seconds in 400 days */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ਸਥਿਰ ਖਾਲੀ cap_expires(time_t now, ਬਣਤਰ Cookie *co)
{
  ਜੇ(co->expires && (ਸਮਾਂ_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    ਜੇ(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ਸਥਿਰ ਖਾਲੀ freecookie(ਬਣਤਰ Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->ਰਸਤਾ);
  curlx_free(co->name);
  curlx_free(co->ਮੁੱਲ);
  ਜੇ(maintoo)
    curlx_free(co);
}

ਸਥਿਰ bool cookie_tailmatch(ਅਚਲ ਅੱਖਰ *cookie_domain,
                             ਅਚਲ size_t cookie_domain_len,
                             ਅਚਲ ਅੱਖਰ *hostname)
{
  size_t hostname_len = ਲੜੀ_ਲੰਬਾਈ(hostname);

  ਜੇ(hostname_len < cookie_domain_len)
    ਵਾਪਸ FALSE;

  ਜੇ(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    ਵਾਪਸ FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  ਜੇ(hostname_len == cookie_domain_len)
    ਵਾਪਸ TRUE;
  ਜੇ('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ਸਥਿਰ bool pathmatch(ਅਚਲ ਅੱਖਰ *cookie_ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *uri_ਰਸਤਾ)
{
  size_t cookie_ਰਸਤਾ_len;
  size_t uri_ਰਸਤਾ_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_ਰਸਤਾ_len = ਲੜੀ_ਲੰਬਾਈ(cookie_ਰਸਤਾ);
  ਜੇ(cookie_ਰਸਤਾ_len == 1) {
    /* cookie_path must be '/' */
    ਵਾਪਸ TRUE;
  }

  /* #-fragments are already cut off! */
  ਜੇ(ਲੜੀ_ਲੰਬਾਈ(uri_ਰਸਤਾ) == 0 || uri_ਰਸਤਾ[0] != '/')
    uri_ਰਸਤਾ = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_ਰਸਤਾ_len = ਲੜੀ_ਲੰਬਾਈ(uri_ਰਸਤਾ);

  ਜੇ(uri_ਰਸਤਾ_len < cookie_ਰਸਤਾ_len)
    ਜਾਓ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  ਜੇ(ਲੜੀ_ਤੁਲਨਾ(cookie_ਰਸਤਾ, uri_ਰਸਤਾ, cookie_ਰਸਤਾ_len))
    ਜਾਓ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  ਜੇ(cookie_ਰਸਤਾ_len == uri_ਰਸਤਾ_len) {
    ret = TRUE;
    ਜਾਓ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  ਜੇ(uri_ਰਸਤਾ[cookie_ਰਸਤਾ_len] == '/') {
    ret = TRUE;
    ਜਾਓ pathmatched;
  }

pathmatched:
  ਵਾਪਸ ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *get_ਸਿਖਰ_domain(ਅਚਲ ਅੱਖਰ * ਅਚਲ domain, size_t *outlen)
{
  size_t len = 0;
  ਅਚਲ ਅੱਖਰ *ਪਹਿਲਾ = ਨਿਰਾ, *ਆਖਰੀ;

  ਜੇ(domain) {
    len = ਲੜੀ_ਲੰਬਾਈ(domain);
    ਆਖਰੀ = memrchr(domain, '.', len);
    ਜੇ(ਆਖਰੀ) {
      ਪਹਿਲਾ = memrchr(domain, '.', (ਆਖਰੀ - domain));
      ਜੇ(ਪਹਿਲਾ)
        len -= (++ਪਹਿਲਾ - domain);
    }
  }

  ਜੇ(outlen)
    *outlen = len;

  ਵਾਪਸ ਪਹਿਲਾ ? ਪਹਿਲਾ : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#ਜੇ defined(_MSC_VER) && (_MSC_VER == 1900)
#ਹਦਾਇਤ optimize("", off)
#ਜੇ_ਅੰਤ

/*
 * A case-insensitive hash for the cookie domains.
 */
ਸਥਿਰ size_t cookie_hash_domain(ਅਚਲ ਅੱਖਰ *domain, ਅਚਲ size_t len)
{
  ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = domain + len;
  size_t h = 5381;

  ਜਦੋਂ(domain < ਸਿਰਾ) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  ਵਾਪਸ (h % COOKIE_HASH_ਮਾਪ);
}

#ਜੇ defined(_MSC_VER) && (_MSC_VER == 1900)
#ਹਦਾਇਤ optimize("", on)
#ਜੇ_ਅੰਤ

/*
 * Hash this domain.
 */
ਸਥਿਰ size_t cookiehash(ਅਚਲ ਅੱਖਰ * ਅਚਲ domain)
{
  ਅਚਲ ਅੱਖਰ *ਸਿਖਰ;
  size_t len;

  ਜੇ(!domain || Curl_host_is_ipnum(domain))
    ਵਾਪਸ 0;

  ਸਿਖਰ = get_ਸਿਖਰ_domain(domain, &len);
  ਵਾਪਸ cookie_hash_domain(ਸਿਖਰ, len);
}

/*
 * cookie path sanitize
 */
ਸਥਿਰ ਅੱਖਰ *sanitize_cookie_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *cookie_ਰਸਤਾ, size_t len)
{
  /* some sites send path attribute within '"'. */
  ਜੇ(len && (cookie_ਰਸਤਾ[0] == '\"')) {
    cookie_ਰਸਤਾ++;
    len--;

    ਜੇ(len && (cookie_ਰਸਤਾ[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  ਜੇ(!len || (cookie_ਰਸਤਾ[0] != '/'))
    /* Let cookie-path be the default-path. */
    ਵਾਪਸ curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  ਜੇ(len > 1 && cookie_ਰਸਤਾ[len - 1] == '/')
    len--;

  ਵਾਪਸ curlx_memdup0(cookie_ਰਸਤਾ, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ਸਥਿਰ CURLcode strstore(ਅੱਖਰ **str, ਅਚਲ ਅੱਖਰ *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  ਜੇ(!*str)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ CURLE_OK;
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
ਸਥਿਰ ਖਾਲੀ remove_expired(ਬਣਤਰ CookieInfo *ci)
{
  ਬਣਤਰ Cookie *co;
  curl_off_t now = (curl_off_t)ਸਮਾਂ(ਨਿਰਾ);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  ਜੇ(now < ci->ਅਗਲਾ_expiration &&
     ci->ਅਗਲਾ_expiration != CURL_OFF_T_MAX)
    ਵਾਪਸ;
  ਨਹੀਂਤਾਂ
    ci->ਅਗਲਾ_expiration = CURL_OFF_T_MAX;

  ਲਈ(i = 0; i < COOKIE_HASH_ਮਾਪ; i++) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;
    ਬਣਤਰ Curl_llist_ਗੰਢ *e = ਨਿਰਾ;

    ਲਈ(n = Curl_llist_ਸਿਰ(&ci->cookielist[i]); n; n = e) {
      co = Curl_ਗੰਢ_elem(n);
      e = Curl_ਗੰਢ_ਅਗਲਾ(n);
      ਜੇ(co->expires) {
        ਜੇ(co->expires < now) {
          Curl_ਗੰਢ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        ਨਹੀਂਤਾਂ ਜੇ(co->expires < ci->ਅਗਲਾ_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->ਅਗਲਾ_expiration = co->expires;
      }
    }
  }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
ਸਥਿਰ bool bad_domain(ਅਚਲ ਅੱਖਰ *domain, size_t len)
{
  ਜੇ((len == 9) && curl_strnequal(domain, "localhost", 9))
    ਵਾਪਸ FALSE;
  ਨਹੀਂਤਾਂ {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ਅਚਲ ਅੱਖਰ *dot = memchr(domain, '.', len);
    ਜੇ(dot) {
      size_t i = dot - domain;
      ਜੇ((len - i) > 1)
        /* the dot is not the last byte */
        ਵਾਪਸ FALSE;
    }
  }
  ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ਸਥਿਰ bool invalid_octets(ਅਚਲ ਅੱਖਰ *ptr, size_t len)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  ਜਦੋਂ(len && *p) {
    ਜੇ((*p < 0x20) || (*p == 0x7f))
      ਵਾਪਸ TRUE;
    p++;
    len--;
  }
  ਵਾਪਸ FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_DATE_ਲੰਮਾਈ 80

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_NAME   0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_ਮੁੱਲ  1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_DOMAIN 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_ਰਸਤਾ   3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_PIECES 4 /* the list above */

ਸਥਿਰ CURLcode storecookie(ਬਣਤਰ Cookie *co, ਅਚਲ ਬਣਤਰ Curl_str *cp,
                            ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *domain)
{
  CURLcode ਨਤੀਜਾ;
  ਨਤੀਜਾ = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = strstore(&co->ਮੁੱਲ, curlx_str(&cp[COOKIE_ਮੁੱਲ]),
                      curlx_strlen(&cp[COOKIE_ਮੁੱਲ]));
  ਜੇ(!ਨਤੀਜਾ) {
    size_t plen = 0;
    ਜੇ(curlx_strlen(&cp[COOKIE_ਰਸਤਾ])) {
      ਰਸਤਾ = curlx_str(&cp[COOKIE_ਰਸਤਾ]);
      plen = curlx_strlen(&cp[COOKIE_ਰਸਤਾ]);
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਰਸਤਾ) {
      /* No path was given in the header line, set the default */
      ਅਚਲ ਅੱਖਰ *endslash = strrchr(ਰਸਤਾ, '/');
      ਜੇ(endslash)
        plen = endslash - ਰਸਤਾ + 1; /* include end slash */
      ਨਹੀਂਤਾਂ
        plen = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
    }

    ਜੇ(ਰਸਤਾ) {
      co->ਰਸਤਾ = sanitize_cookie_ਰਸਤਾ(ਰਸਤਾ, plen);
      ਜੇ(!co->ਰਸਤਾ)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(curlx_strlen(&cp[COOKIE_DOMAIN]))
      ਨਤੀਜਾ = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    ਨਹੀਂਤਾਂ ਜੇ(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      ਜੇ(!co->domain)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ਸਥਿਰ bool parse_ਪਹਿਲਾ_pair(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Cookie *co,
                             ਬਣਤਰ Curl_str *cookie,
                             ਬਣਤਰ Curl_str *name,
                             ਬਣਤਰ Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  ਜੇ(!sep || !curlx_strlen(name)) {
    infof(ਅੰਕੜੇ, "invalid cookie, dropped");
    ਵਾਪਸ FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  ਜੇ((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(ਅੰਕੜੇ, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    ਵਾਪਸ FALSE;
  }

  /* Check if we have a reserved prefix set. */
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ("__Secure-", curlx_str(name), 9))
    co->ਅਗੇਤਰ_secure = TRUE;
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ("__Host-", curlx_str(name), 7))
    co->ਅਗੇਤਰ_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_ਮੁੱਲ] = *val;
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool parse_domain(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Cookie *co,
                         ਬਣਤਰ Curl_str *cookie_domain,
                         ਬਣਤਰ Curl_str *val,
                         ਅਚਲ ਅੱਖਰ **domainp)
{
  bool is_ip;
  ਅਚਲ ਅੱਖਰ *domain = *domainp;
  ਅਚਲ ਅੱਖਰ *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  ਜੇ('.' == *v)
    curlx_str_nudge(val, 1);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  ਜੇ(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#ਜੇ_ਅੰਤ

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  ਜੇ(!domain ||
     (is_ip &&
      !ਲੜੀ_ਤੁਲਨਾ(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == ਲੜੀ_ਲੰਬਾਈ(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    ਜੇ(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  ਨਹੀਂਤਾਂ {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(ਅੰਕੜੇ, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਖਾਲੀ parse_maxage(ਬਣਤਰ Cookie *co, ਬਣਤਰ Curl_str *val,
                         time_t *nowp)
{
  ਪੂਰਨ rc;
  ਅਚਲ ਅੱਖਰ *maxage = curlx_str(val);
  ਜੇ(*maxage == '\"')
    maxage++;
  rc = curlx_str_ਸੰਖਿਆ(&maxage, &co->expires, CURL_OFF_T_MAX);
  ਜੇ(!*nowp)
    *nowp = ਸਮਾਂ(ਨਿਰਾ);
  ਚੋਣ(rc) {
  ਹਾਲਤ STRE_ਉਛਾਲ:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    ਰੋਕੋ;
  ਮੂਲ:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    ਰੋਕੋ;
  ਹਾਲਤ STRE_OK:
    ਜੇ(!co->expires)
      co->expires = 1; /* expire now */
    ਨਹੀਂਤਾਂ ਜੇ(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    ਨਹੀਂਤਾਂ
      co->expires += *nowp;
    ਰੋਕੋ;
  }
  cap_expires(*nowp, co);
}

ਸਥਿਰ ਖਾਲੀ parse_expires(ਬਣਤਰ Cookie *co, ਬਣਤਰ Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  ਜੇ(!co->expires && (curlx_strlen(val) < MAX_DATE_ਲੰਮਾਈ)) {
    ਅੱਖਰ dbuf[MAX_DATE_ਲੰਮਾਈ + 1];
    time_t date = 0;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    ਜੇ(!Curl_getdate_capped(dbuf, &date)) {
      ਜੇ(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    ਨਹੀਂਤਾਂ
      co->expires = 0;
    ਜੇ(!*nowp)
      *nowp = ਸਮਾਂ(ਨਿਰਾ);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ਸਥਿਰ CURLcode
parse_cookie_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਬਣਤਰ Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ਅਚਲ ਅੱਖਰ *ptr, /* the header */
                    ਅਚਲ ਅੱਖਰ *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = ਲੜੀ_ਲੰਬਾਈ(ptr);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  ਜੇ(linelength > MAX_COOKIE_ਸਤਰ)
    /* discard overly long lines at once */
    ਵਾਪਸ CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, ਆਕਾਰ(cookie));
  ਕਰੋ {
    ਬਣਤਰ Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    ਪੂਰਨ rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    ਜੇ(!rc || (rc == STRE_SHORT)) {
      ਬਣਤਰ Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      ਜੇ(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(ਅੰਕੜੇ, "invalid octets in name, cookie dropped");
        ਵਾਪਸ CURLE_OK;
      }

      ਜੇ(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        ਜੇ(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        ਜੇ(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(ਅੰਕੜੇ, "invalid octets in value, cookie dropped");
          ਵਾਪਸ CURLE_OK;
        }
      }
      ਨਹੀਂਤਾਂ
        curlx_str_init(&val);

      ਜੇ(!curlx_strlen(&cookie[COOKIE_NAME])) {
        ਜੇ(!parse_ਪਹਿਲਾ_pair(ਅੰਕੜੇ, co, cookie, &name, &val, sep))
          ਵਾਪਸ CURLE_OK;
      }

      /* whether there's separator or not */
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "secure")) {
        ਜੇ(secure_origin)
          co->secure = TRUE;
        ਨਹੀਂਤਾਂ {
          infof(ਅੰਕੜੇ, "skipped cookie because not 'secure'");
          ਵਾਪਸ CURLE_OK;
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      ਨਹੀਂਤਾਂ ਜੇ(sep) {
        /* only if a separating equals sign is present */
        ਜੇ(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_ਰਸਤਾ] = val;
        ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "domain")) {
          ਜੇ(curlx_strlen(&val) &&
             !parse_domain(ਅੰਕੜੇ, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            ਵਾਪਸ CURLE_OK;
        }
        ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } ਜਦੋਂ(!curlx_str_single(&ptr, ';'));

  ਜੇ(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    ਨਤੀਜਾ = storecookie(co, &cookie[0], ਰਸਤਾ, domain);
    ਜੇ(!ਨਤੀਜਾ)
      *okay = TRUE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode parse_netscape(ਬਣਤਰ Cookie *co,
                               ਅਚਲ ਬਣਤਰ CookieInfo *ci,
                               bool *okay,
                               ਅਚਲ ਅੱਖਰ *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ਅਚਲ ਅੱਖਰ *ptr, *ਅਗਲਾ;
  ਪੂਰਨ fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  ਜੇ(lineptr[0] == '#')
    /* do not even try the comments */
    ਵਾਪਸ CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  ਲਈ(ਅਗਲਾ = lineptr; ਅਗਲਾ; fields++) {
    ptr = ਅਗਲਾ;
    len = strcspn(ptr, "\t\r\n");
    ਅਗਲਾ = (ptr[len] == '\t' ? &ptr[len + 1] : ਨਿਰਾ);
    ਚੋਣ(fields) {
    ਹਾਲਤ 0:
      ਜੇ(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      ਜੇ(!co->domain)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰੋਕੋ;
    ਹਾਲਤ 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      ਰੋਕੋ;
    ਹਾਲਤ 2:
      /* The file format allows the path field to remain not filled in */
      ਜੇ(ਲੜੀ_ਤੁਲਨਾ("TRUE", ptr, len) && ਲੜੀ_ਤੁਲਨਾ("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->ਰਸਤਾ = sanitize_cookie_ਰਸਤਾ(ptr, len);
        ਜੇ(!co->ਰਸਤਾ)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ {
        /* this does not look like a path, make one up! */
        co->ਰਸਤਾ = curlx_strdup("/");
        ਜੇ(!co->ਰਸਤਾ)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    ਹਾਲਤ 3:
      co->secure = FALSE;
      ਜੇ(curl_strnequal(ptr, "TRUE", len)) {
        ਜੇ(secure_origin || ci->running)
          co->secure = TRUE;
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ CURLE_OK;
      }
      ਰੋਕੋ;
    ਹਾਲਤ 4:
      ਜੇ(curlx_str_ਸੰਖਿਆ(&ptr, &co->expires, CURL_OFF_T_MAX))
        ਵਾਪਸ CURLE_OK;
      ਰੋਕੋ;
    ਹਾਲਤ 5:
      co->name = curlx_memdup0(ptr, len);
      ਜੇ(!co->name)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        ਜੇ(!ਲੜੀ_ਤੁਲਨਾ("__Secure-", co->name, 9))
          co->ਅਗੇਤਰ_secure = TRUE;
        ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ("__Host-", co->name, 7))
          co->ਅਗੇਤਰ_host = TRUE;
      }
      ਰੋਕੋ;
    ਹਾਲਤ 6:
      co->ਮੁੱਲ = curlx_memdup0(ptr, len);
      ਜੇ(!co->ਮੁੱਲ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰੋਕੋ;
    }
  }
  ਜੇ(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->ਮੁੱਲ = curlx_strdup("");
    ਜੇ(!co->ਮੁੱਲ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ
      fields++;
  }

  ਜੇ(fields != 7)
    /* we did not find the sufficient number of fields */
    ਵਾਪਸ CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  ਜੇ(invalid_octets(co->name, ਲੜੀ_ਲੰਬਾਈ(co->name)) ||
     invalid_octets(co->ਮੁੱਲ, ਲੜੀ_ਲੰਬਾਈ(co->ਮੁੱਲ)))
    ਵਾਪਸ CURLE_OK;

  *okay = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool is_ਜਨਤਕ_suffix(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Cookie *co,
                             ਅਚਲ ਅੱਖਰ *domain)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(co);
  DEBUGF(infof(ਅੰਕੜੇ, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  ਜੇ(!co->domain || Curl_host_is_ipnum(co->domain))
    ਵਾਪਸ FALSE;

  ਨਹੀਂਤਾਂ {
    bool acceptable = FALSE;
    ਅੱਖਰ lcase[256];
    ਅੱਖਰ lcookie[256];
    size_t dlen = domain ? ਲੜੀ_ਲੰਬਾਈ(domain) : 0;
    size_t clen = ਲੜੀ_ਲੰਬਾਈ(co->domain);

    /* trim trailing dots */
    ਜੇ(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    ਜੇ(clen && (co->domain[clen - 1] == '.'))
      clen--;

    ਜੇ((dlen < ਆਕਾਰ(lcase)) && (clen < ਆਕਾਰ(lcookie))) {
      ਅਚਲ psl_ctx_t *psl = Curl_psl_use(ਅੰਕੜੇ);
      ਜੇ(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        ਜੇ(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          ਜੇ(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_ਜਨਤਕ_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        ਨਹੀਂਤਾਂ {
          bool suffix = psl_is_ਜਨਤਕ_suffix(psl, lcookie);
          acceptable = TRUE;
          ਜੇ(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(ਅੰਕੜੇ);
      }
      ਨਹੀਂਤਾਂ
        infof(ਅੰਕੜੇ, "libpsl problem, rejecting cookie for safety");
    }

    ਜੇ(!acceptable) {
      infof(ਅੰਕੜੇ, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      ਵਾਪਸ TRUE;
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)co;
  (ਖਾਲੀ)domain;
  DEBUGF(infof(ਅੰਕੜੇ, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#ਜੇ_ਅੰਤ
  ਵਾਪਸ FALSE;
}

/* returns TRUE when replaced */
ਸਥਿਰ bool replace_existing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Cookie *co,
                             ਅਚਲ ਬਣਤਰ CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ਬਣਤਰ Curl_llist_ਗੰਢ *replace_n = ਨਿਰਾ;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;
  size_t myhash = cookiehash(co->domain);
  ਲਈ(n = Curl_llist_ਸਿਰ(&ci->cookielist[myhash]); n; n = Curl_ਗੰਢ_ਅਗਲਾ(n)) {
    ਬਣਤਰ Cookie *clist = Curl_ਗੰਢ_elem(n);
    ਜੇ(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      ਜੇ(clist->domain && co->domain) {
        ਜੇ(cookie_tailmatch(clist->domain, ਲੜੀ_ਲੰਬਾਈ(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, ਲੜੀ_ਲੰਬਾਈ(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!clist->domain && !co->domain)
        matching_domains = TRUE;

      ਜੇ(matching_domains && /* the domains were identical */
         clist->ਰਸਤਾ && co->ਰਸਤਾ && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ਅਚਲ ਅੱਖਰ *sep = ਨਿਰਾ;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->ਰਸਤਾ[0]);
        ਜੇ(clist->ਰਸਤਾ[0])
          sep = strchr(clist->ਰਸਤਾ + 1, '/');
        ਜੇ(sep)
          cllen = sep - clist->ਰਸਤਾ;
        ਨਹੀਂਤਾਂ
          cllen = ਲੜੀ_ਲੰਬਾਈ(clist->ਰਸਤਾ);

        ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(clist->ਰਸਤਾ, co->ਰਸਤਾ, cllen)) {
          infof(ਅੰਕੜੇ, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          ਵਾਪਸ FALSE;
        }
      }
    }

    ਜੇ(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      ਜੇ(clist->domain && co->domain) {
        ਜੇ(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!clist->domain && !co->domain)
        replace_old = TRUE;

      ਜੇ(replace_old) {
        /* the domains were identical */

        ਜੇ(clist->ਰਸਤਾ && co->ਰਸਤਾ &&
           strcmp(clist->ਰਸਤਾ, co->ਰਸਤਾ))
          replace_old = FALSE;
        ਨਹੀਂਤਾਂ ਜੇ(!clist->ਰਸਤਾ != !co->ਰਸਤਾ)
          replace_old = FALSE;
      }

      ਜੇ(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        ਵਾਪਸ FALSE;
      }
      ਜੇ(replace_old)
        replace_n = n;
    }
  }
  ਜੇ(replace_n) {
    ਬਣਤਰ Cookie *repl = Curl_ਗੰਢ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ਗੰਢ_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  ਵਾਪਸ TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ CookieInfo *ci,
                         ਅਚਲ ਅੱਖਰ *lineptr, /* first character of the line */
                         ਅਚਲ ਅੱਖਰ *domain,  /* default domain */
                         ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ਅਚਲ ਪੂਰਨ flags)
{
  ਬਣਤਰ Cookie comem;
  ਬਣਤਰ Cookie *co;
  size_t myhash;
  CURLcode ਨਤੀਜਾ;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(MAX_ਸੈੱਟ_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  ਜੇ(ਅੰਕੜੇ->req.setcookies >= MAX_ਸੈੱਟ_COOKIE_AMOUNT)
    ਵਾਪਸ CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, ਆਕਾਰ(comem));

  ਜੇ(flags & COOKIE_HTTPHEADER)
    ਨਤੀਜਾ = parse_cookie_ਸਿਰਲੇਖ(ਅੰਕੜੇ, co, &okay,
                                 lineptr, domain, ਰਸਤਾ, flags & COOKIE_SECURE);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  ਜੇ(ਨਤੀਜਾ || !okay)
    ਜਾਓ fail;

  ਜੇ(co->ਅਗੇਤਰ_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    ਜਾਓ fail;

  ਜੇ(!(flags & COOKIE_NOPSL) && is_ਜਨਤਕ_suffix(ਅੰਕੜੇ, co, domain))
    ਜਾਓ fail;

  ਜੇ(co->ਅਗੇਤਰ_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    ਜੇ(co->secure && co->ਰਸਤਾ && !strcmp(co->ਰਸਤਾ, "/") && !co->tailmatch)
      ;
    ਨਹੀਂਤਾਂ
      ਜਾਓ fail;
  }

  ਜੇ(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    ਜਾਓ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  ਜੇ(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  ਜੇ(!replace_existing(ਅੰਕੜੇ, co, ci, flags & COOKIE_SECURE, &replaces))
    ਜਾਓ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, ਆਕਾਰ(comem));
  ਜੇ(!co) {
    co = &comem;
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ਗੰਢ);

  ਜੇ(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(ਅੰਕੜੇ, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->ਮੁੱਲ,
          co->domain, co->ਰਸਤਾ, co->expires);

  ਜੇ(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  ਜੇ(co->expires && (co->expires < ci->ਅਗਲਾ_expiration))
    ci->ਅਗਲਾ_expiration = co->expires;

  ਜੇ(flags & COOKIE_HTTPHEADER)
    ਅੰਕੜੇ->req.setcookies++;

  ਵਾਪਸ ਨਤੀਜਾ;
fail:
  freecookie(co, FALSE);
  ਵਾਪਸ ਨਤੀਜਾ;
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
ਬਣਤਰ CookieInfo *Curl_cookie_init(ਖਾਲੀ)
{
  ਪੂਰਨ i;
  ਬਣਤਰ CookieInfo *ci = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ CookieInfo));
  ਜੇ(!ci)
    ਵਾਪਸ ਨਿਰਾ;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  ਲਈ(i = 0; i < COOKIE_HASH_ਮਾਪ; i++)
    Curl_llist_init(&ci->cookielist[i], ਨਿਰਾ);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->ਅਗਲਾ_expiration = CURL_OFF_T_MAX;

  ਵਾਪਸ ci;
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
ਸਥਿਰ CURLcode cookie_ਲੱਦੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *file,
                            ਬਣਤਰ CookieInfo *ci, ਪੂਰਨ flags)
{
  FILE *handle = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  FILE *fp = ਨਿਰਾ;
  DEBUGASSERT(ci);
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  ਜੇ(file && *file) {
    ਜੇ(!strcmp(file, "-"))
      fp = stdin;
    ਨਹੀਂਤਾਂ {
      fp = curlx_fopen(file, "rb");
      ਜੇ(!fp)
        infof(ਅੰਕੜੇ, "WARNING: failed to open cookie file \"%s\"", file);
      ਨਹੀਂਤਾਂ {
        curlx_struct_stat stat;
        ਜੇ((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_ਬਹੁਲਕ)) {
          curlx_fclose(fp);
          fp = ਨਿਰਾ;
          infof(ਅੰਕੜੇ, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        ਨਹੀਂਤਾਂ
          handle = fp;
      }
    }
  }

  ਜੇ(fp) {
    ਬਣਤਰ dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    ਕਰੋ {
      ਨਤੀਜਾ = Curl_get_ਸਤਰ(&buf, fp, &eof);
      ਜੇ(!ਨਤੀਜਾ) {
        ਅਚਲ ਅੱਖਰ *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        ਜੇ(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        ਨਤੀਜਾ = Curl_cookie_ਜੋੜੋ(ਅੰਕੜੇ, ci, lineptr, ਨਿਰਾ, ਨਿਰਾ,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        ਜੇ(ਨਤੀਜਾ != CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
          ਨਤੀਜਾ = CURLE_OK;
      }
    } ਜਦੋਂ(!ਨਤੀਜਾ && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    ਜੇ(handle)
      curlx_fclose(handle);
  }
  ਅੰਕੜੇ->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਪੂਰਨ flags)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ curl_slist *list = ਅੰਕੜੇ->state.cookielist;
  ਜੇ(list) {
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
    ਜੇ(!ਅੰਕੜੇ->cookies)
      ਅੰਕੜੇ->cookies = Curl_cookie_init();
    ਜੇ(!ਅੰਕੜੇ->cookies)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ {
      ਅੰਕੜੇ->state.cookie_engine = TRUE;
      ਜਦੋਂ(list) {
        ਨਤੀਜਾ = cookie_ਲੱਦੋ(ਅੰਕੜੇ, list->ਅੰਕੜੇ, ਅੰਕੜੇ->cookies, flags);
        ਜੇ(ਨਤੀਜਾ)
          ਰੋਕੋ;
        list = list->ਅਗਲਾ;
      }
    }
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ਸਥਿਰ ਪੂਰਨ cookie_ਛਾਂਟੋ(ਅਚਲ ਖਾਲੀ *p1, ਅਚਲ ਖਾਲੀ *p2)
{
  ਅਚਲ ਬਣਤਰ Cookie *c1 = *(ਅਚਲ ਬਣਤਰ Cookie * ਅਚਲ *)p1;
  ਅਚਲ ਬਣਤਰ Cookie *c2 = *(ਅਚਲ ਬਣਤਰ Cookie * ਅਚਲ *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->ਰਸਤਾ ? ਲੜੀ_ਲੰਬਾਈ(c1->ਰਸਤਾ) : 0;
  l2 = c2->ਰਸਤਾ ? ਲੜੀ_ਲੰਬਾਈ(c2->ਰਸਤਾ) : 0;

  ਜੇ(l1 != l2)
    ਵਾਪਸ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? ਲੜੀ_ਲੰਬਾਈ(c1->domain) : 0;
  l2 = c2->domain ? ਲੜੀ_ਲੰਬਾਈ(c2->domain) : 0;

  ਜੇ(l1 != l2)
    ਵਾਪਸ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? ਲੜੀ_ਲੰਬਾਈ(c1->name) : 0;
  l2 = c2->name ? ਲੜੀ_ਲੰਬਾਈ(c2->name) : 0;

  ਜੇ(l1 != l2)
    ਵਾਪਸ (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  ਵਾਪਸ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ਸਥਿਰ ਪੂਰਨ cookie_ਛਾਂਟੋ_ct(ਅਚਲ ਖਾਲੀ *p1, ਅਚਲ ਖਾਲੀ *p2)
{
  ਅਚਲ ਬਣਤਰ Cookie *c1 = *(ਅਚਲ ਬਣਤਰ Cookie * ਅਚਲ *)p1;
  ਅਚਲ ਬਣਤਰ Cookie *c2 = *(ਅਚਲ ਬਣਤਰ Cookie * ਅਚਲ *)p2;

  ਵਾਪਸ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *host)
{
  ਵਾਪਸ Curl_xfer_is_secure(ਅੰਕੜੇ) ||
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
CURLcode Curl_cookie_getlist(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool *okay,
                             ਅਚਲ ਅੱਖਰ *host,
                             ਬਣਤਰ Curl_llist *list)
{
  size_t matches = 0;
  ਅਚਲ bool is_ip = Curl_host_is_ipnum(host);
  ਅਚਲ size_t myhash = cookiehash(host);
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;
  ਅਚਲ bool secure = Curl_secure_context(ਅੰਕੜੇ, host);
  ਬਣਤਰ CookieInfo *ci = ਅੰਕੜੇ->cookies;
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ਨਿਰਾ);

  ਜੇ(!ci || !Curl_llist_ਗਿਣਤੀ(&ci->cookielist[myhash]))
    ਵਾਪਸ CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  ਲਈ(n = Curl_llist_ਸਿਰ(&ci->cookielist[myhash]); n; n = Curl_ਗੰਢ_ਅਗਲਾ(n)) {
    ਬਣਤਰ Cookie *co = Curl_ਗੰਢ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    ਜੇ(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      ਜੇ(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, ਲੜੀ_ਲੰਬਾਈ(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        ਜੇ(!co->ਰਸਤਾ || pathmatch(co->ਰਸਤਾ, ਰਸਤਾ)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          ਜੇ(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(ਅੰਕੜੇ, "Included max number of cookies (%zu) in request!",
                  matches);
            ਰੋਕੋ;
          }
        }
      }
    }
  }

  ਜੇ(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    ਬਣਤਰ Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ Cookie *) * matches);
    ਜੇ(!array) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ fail;
    }

    n = Curl_llist_ਸਿਰ(list);

    ਲਈ(i = 0; n; n = Curl_ਗੰਢ_ਅਗਲਾ(n))
      array[i++] = Curl_ਗੰਢ_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, ਆਕਾਰ(ਬਣਤਰ Cookie *), cookie_ਛਾਂਟੋ);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ਨਿਰਾ);

    ਲਈ(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  ਵਾਪਸ CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ਨਿਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
ਖਾਲੀ Curl_cookie_clearall(ਬਣਤਰ CookieInfo *ci)
{
  ਜੇ(ci) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
    ਲਈ(i = 0; i < COOKIE_HASH_ਮਾਪ; i++) {
      ਬਣਤਰ Curl_llist_ਗੰਢ *n;
      ਲਈ(n = Curl_llist_ਸਿਰ(&ci->cookielist[i]); n;) {
        ਬਣਤਰ Cookie *c = Curl_ਗੰਢ_elem(n);
        ਬਣਤਰ Curl_llist_ਗੰਢ *e = Curl_ਗੰਢ_ਅਗਲਾ(n);
        Curl_ਗੰਢ_remove(n);
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
ਖਾਲੀ Curl_cookie_clearsess(ਬਣਤਰ CookieInfo *ci)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

  ਜੇ(!ci)
    ਵਾਪਸ;

  ਲਈ(i = 0; i < COOKIE_HASH_ਮਾਪ; i++) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *n = Curl_llist_ਸਿਰ(&ci->cookielist[i]);
    ਬਣਤਰ Curl_llist_ਗੰਢ *e = ਨਿਰਾ;

    ਲਈ(; n; n = e) {
      ਬਣਤਰ Cookie *curr = Curl_ਗੰਢ_elem(n);
      e = Curl_ਗੰਢ_ਅਗਲਾ(n); /* in case the node is removed, get it early */
      ਜੇ(!curr->expires) {
        Curl_ਗੰਢ_remove(n);
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
ਖਾਲੀ Curl_cookie_cleanup(ਬਣਤਰ CookieInfo *ci)
{
  ਜੇ(ci) {
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
ਸਥਿਰ ਅੱਖਰ *get_netscape_format(ਅਚਲ ਬਣਤਰ Cookie *co)
{
  ਵਾਪਸ curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /*ਰਸਤਾ*/
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*ਮੁੱਲ*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->ਰਸਤਾ ? co->ਰਸਤਾ : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->ਮੁੱਲ ? co->ਮੁੱਲ : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ਸਥਿਰ CURLcode cookie_output(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ CookieInfo *ci,
                              ਅਚਲ ਅੱਖਰ *filename)
{
  FILE *ਬਾਹਰਲਾ = ਨਿਰਾ;
  bool use_stdout = FALSE;
  ਅੱਖਰ *tempstore = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ci)
    /* no cookie engine alive */
    ਵਾਪਸ CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  ਜੇ(!strcmp("-", filename)) {
    /* use stdout */
    ਬਾਹਰਲਾ = stdout;
    use_stdout = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_fopen(ਅੰਕੜੇ, filename, &ਬਾਹਰਲਾ, &tempstore);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        ਬਾਹਰਲਾ);

  ਜੇ(ci->numcookies) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
    size_t nvalid = 0;
    ਬਣਤਰ Cookie **array;
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;

    array = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Cookie *) * ci->numcookies);
    ਜੇ(!array) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ error;
    }

    /* only sort the cookies with a domain property */
    ਲਈ(i = 0; i < COOKIE_HASH_ਮਾਪ; i++) {
      ਲਈ(n = Curl_llist_ਸਿਰ(&ci->cookielist[i]); n; n = Curl_ਗੰਢ_ਅਗਲਾ(n)) {
        ਬਣਤਰ Cookie *co = Curl_ਗੰਢ_elem(n);
        ਜੇ(!co->domain)
          ਜਾਰੀ;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, ਆਕਾਰ(ਬਣਤਰ Cookie *), cookie_ਛਾਂਟੋ_ct);

    ਲਈ(i = 0; i < nvalid; i++) {
      ਅੱਖਰ *format_ptr = get_netscape_format(array[i]);
      ਜੇ(!format_ptr) {
        curlx_free(array);
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ error;
      }
      curl_mfprintf(ਬਾਹਰਲਾ, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  ਜੇ(!use_stdout) {
    curlx_fclose(ਬਾਹਰਲਾ);
    ਬਾਹਰਲਾ = ਨਿਰਾ;
    ਜੇ(tempstore && curlx_rename(tempstore, filename)) {
      ਨਤੀਜਾ = CURLE_WRITE_ERROR;
      ਜਾਓ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  ਵਾਪਸ CURLE_OK;

error:
  ਜੇ(ਬਾਹਰਲਾ && !use_stdout)
    curlx_fclose(ਬਾਹਰਲਾ);
  ਜੇ(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਬਣਤਰ curl_slist *cookie_list(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_slist *list = ਨਿਰਾ;
  ਬਣਤਰ curl_slist *beg;
  ਬਣਤਰ curl_slist **ਪੂਛ = &list;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;

  ਜੇ(!ਅੰਕੜੇ->cookies || (ਅੰਕੜੇ->cookies->numcookies == 0))
    ਵਾਪਸ ਨਿਰਾ;

  /* at first, remove expired cookies */
  remove_expired(ਅੰਕੜੇ->cookies);

  ਲਈ(i = 0; i < COOKIE_HASH_ਮਾਪ; i++) {
    ਲਈ(n = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->cookies->cookielist[i]); n;
        n = Curl_ਗੰਢ_ਅਗਲਾ(n)) {
      ਬਣਤਰ Cookie *c = Curl_ਗੰਢ_elem(n);
      ਅੱਖਰ *ਸਤਰ;
      ਜੇ(!c->domain)
        ਜਾਰੀ;
      ਸਤਰ = get_netscape_format(c);
      ਜੇ(!ਸਤਰ) {
        curl_slist_free_all(list);
        ਵਾਪਸ ਨਿਰਾ;
      }
      beg = Curl_slist_append_nodup(ਨਿਰਾ, ਸਤਰ);
      ਜੇ(!beg) {
        curlx_free(ਸਤਰ);
        curl_slist_free_all(list);
        ਵਾਪਸ ਨਿਰਾ;
      }
      *ਪੂਛ = beg;
      ਪੂਛ = &beg->ਅਗਲਾ;
    }
  }

  ਵਾਪਸ list;
}

ਬਣਤਰ curl_slist *Curl_cookie_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_slist *list;
  Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
  list = cookie_list(ਅੰਕੜੇ);
  Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
  ਵਾਪਸ list;
}

ਖਾਲੀ Curl_flush_cookies(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool cleanup)
{
  Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  ਜੇ(ਅੰਕੜੇ->cookies) {
    ਅਚਲ ਅੱਖਰ *cookiejar = CURL_EASY_STR(ਅੰਕੜੇ, STRING_COOKIEJAR);
    ਜੇ(cookiejar && ਅੰਕੜੇ->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode ਨਤੀਜਾ = cookie_output(ਅੰਕੜੇ, ਅੰਕੜੇ->cookies, cookiejar);
      ਜੇ(ਨਤੀਜਾ)
        infof(ਅੰਕੜੇ, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(ਨਤੀਜਾ));
    }

    ਜੇ(cleanup && (!ਅੰਕੜੇ->share || (ਅੰਕੜੇ->cookies != ਅੰਕੜੇ->share->cookies))) {
      Curl_cookie_cleanup(ਅੰਕੜੇ->cookies);
      ਅੰਕੜੇ->cookies = ਨਿਰਾ;
    }
  }
  Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
}

ਖਾਲੀ Curl_cookie_ਲੜੀਵਾਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
  ਜੇ(ਅੰਕੜੇ->cookies)
    ਅੰਕੜੇ->cookies->running = TRUE;
  Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
}

#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
