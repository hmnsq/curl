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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "curl_memrchr.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* number of seconds in 400 days */
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ಸ್ಥಿರ ಶೂನ್ಯ cap_expires(time_t now, ರಚನೆ Cookie *co)
{
  ಆದರೆ(co->expires && (ಸಮಯ_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    ಆದರೆ(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ freecookie(ರಚನೆ Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->ಮಾರ್ಗ);
  curlx_free(co->name);
  curlx_free(co->ಮೌಲ್ಯ);
  ಆದರೆ(maintoo)
    curlx_free(co);
}

ಸ್ಥಿರ bool cookie_tailmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cookie_domain,
                             ಸ್ಥಿರಾಂಕ size_t cookie_domain_len,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname)
{
  size_t hostname_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname);

  ಆದರೆ(hostname_len < cookie_domain_len)
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    ಹಿಂತಿರುಗಿ FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  ಆದರೆ(hostname_len == cookie_domain_len)
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ಸ್ಥಿರ bool pathmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cookie_ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uri_ಮಾರ್ಗ)
{
  size_t cookie_ಮಾರ್ಗ_len;
  size_t uri_ಮಾರ್ಗ_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_ಮಾರ್ಗ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cookie_ಮಾರ್ಗ);
  ಆದರೆ(cookie_ಮಾರ್ಗ_len == 1) {
    /* cookie_path must be '/' */
    ಹಿಂತಿರುಗಿ TRUE;
  }

  /* #-fragments are already cut off! */
  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(uri_ಮಾರ್ಗ) == 0 || uri_ಮಾರ್ಗ[0] != '/')
    uri_ಮಾರ್ಗ = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_ಮಾರ್ಗ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(uri_ಮಾರ್ಗ);

  ಆದರೆ(uri_ಮಾರ್ಗ_len < cookie_ಮಾರ್ಗ_len)
    ಹೋಗು pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cookie_ಮಾರ್ಗ, uri_ಮಾರ್ಗ, cookie_ಮಾರ್ಗ_len))
    ಹೋಗು pathmatched;

  /* The cookie-path and the uri-path are identical. */
  ಆದರೆ(cookie_ಮಾರ್ಗ_len == uri_ಮಾರ್ಗ_len) {
    ret = TRUE;
    ಹೋಗು pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  ಆದರೆ(uri_ಮಾರ್ಗ[cookie_ಮಾರ್ಗ_len] == '/') {
    ret = TRUE;
    ಹೋಗು pathmatched;
  }

pathmatched:
  ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get_ತುದಿ_domain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ domain, size_t *outlen)
{
  size_t len = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೊದಲ = ಖಾಲಿ, *ಕೊನೆಯ;

  ಆದರೆ(domain) {
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(domain);
    ಕೊನೆಯ = memrchr(domain, '.', len);
    ಆದರೆ(ಕೊನೆಯ) {
      ಮೊದಲ = memrchr(domain, '.', (ಕೊನೆಯ - domain));
      ಆದರೆ(ಮೊದಲ)
        len -= (++ಮೊದಲ - domain);
    }
  }

  ಆದರೆ(outlen)
    *outlen = len;

  ಹಿಂತಿರುಗಿ ಮೊದಲ ? ಮೊದಲ : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#ಆದರೆ defined(_MSC_VER) && (_MSC_VER == 1900)
#ಸೂಚನೆ optimize("", off)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * A case-insensitive hash for the cookie domains.
 */
ಸ್ಥಿರ size_t cookie_hash_domain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain, ಸ್ಥಿರಾಂಕ size_t len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = domain + len;
  size_t h = 5381;

  ಆಗ(domain < ಕೊನೆ) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  ಹಿಂತಿರುಗಿ (h % COOKIE_HASH_ಗಾತ್ರ);
}

#ಆದರೆ defined(_MSC_VER) && (_MSC_VER == 1900)
#ಸೂಚನೆ optimize("", on)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Hash this domain.
 */
ಸ್ಥಿರ size_t cookiehash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ domain)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತುದಿ;
  size_t len;

  ಆದರೆ(!domain || Curl_host_is_ipnum(domain))
    ಹಿಂತಿರುಗಿ 0;

  ತುದಿ = get_ತುದಿ_domain(domain, &len);
  ಹಿಂತಿರುಗಿ cookie_hash_domain(ತುದಿ, len);
}

/*
 * cookie path sanitize
 */
ಸ್ಥಿರ ಅಕ್ಷರ *sanitize_cookie_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cookie_ಮಾರ್ಗ, size_t len)
{
  /* some sites send path attribute within '"'. */
  ಆದರೆ(len && (cookie_ಮಾರ್ಗ[0] == '\"')) {
    cookie_ಮಾರ್ಗ++;
    len--;

    ಆದರೆ(len && (cookie_ಮಾರ್ಗ[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  ಆದರೆ(!len || (cookie_ಮಾರ್ಗ[0] != '/'))
    /* Let cookie-path be the default-path. */
    ಹಿಂತಿರುಗಿ curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  ಆದರೆ(len > 1 && cookie_ಮಾರ್ಗ[len - 1] == '/')
    len--;

  ಹಿಂತಿರುಗಿ curlx_memdup0(cookie_ಮಾರ್ಗ, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ಸ್ಥಿರ CURLcode strstore(ಅಕ್ಷರ **str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  ಆದರೆ(!*str)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಸ್ಥಿರ ಶೂನ್ಯ remove_expired(ರಚನೆ CookieInfo *ci)
{
  ರಚನೆ Cookie *co;
  curl_off_t now = (curl_off_t)ಸಮಯ(ಖಾಲಿ);
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  ಆದರೆ(now < ci->ಮುಂದಿನ_expiration &&
     ci->ಮುಂದಿನ_expiration != CURL_OFF_T_MAX)
    ಹಿಂತಿರುಗಿ;
  ಇಲ್ಲದಿದ್ದರೆ
    ci->ಮುಂದಿನ_expiration = CURL_OFF_T_MAX;

  ಗಾಗಿ(i = 0; i < COOKIE_HASH_ಗಾತ್ರ; i++) {
    ರಚನೆ Curl_llist_ಗಂಟು *n;
    ರಚನೆ Curl_llist_ಗಂಟು *e = ಖಾಲಿ;

    ಗಾಗಿ(n = Curl_llist_ತಲೆ(&ci->cookielist[i]); n; n = e) {
      co = Curl_ಗಂಟು_elem(n);
      e = Curl_ಗಂಟು_ಮುಂದಿನ(n);
      ಆದರೆ(co->expires) {
        ಆದರೆ(co->expires < now) {
          Curl_ಗಂಟು_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(co->expires < ci->ಮುಂದಿನ_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->ಮುಂದಿನ_expiration = co->expires;
      }
    }
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
ಸ್ಥಿರ bool bad_domain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain, size_t len)
{
  ಆದರೆ((len == 9) && curl_strnequal(domain, "localhost", 9))
    ಹಿಂತಿರುಗಿ FALSE;
  ಇಲ್ಲದಿದ್ದರೆ {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dot = memchr(domain, '.', len);
    ಆದರೆ(dot) {
      size_t i = dot - domain;
      ಆದರೆ((len - i) > 1)
        /* the dot is not the last byte */
        ಹಿಂತಿರುಗಿ FALSE;
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ಸ್ಥಿರ bool invalid_octets(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, size_t len)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  ಆಗ(len && *p) {
    ಆದರೆ((*p < 0x20) || (*p == 0x7f))
      ಹಿಂತಿರುಗಿ TRUE;
    p++;
    len--;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_DATE_ಉದ್ದಳತೆ 80

#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_NAME   0
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_ಮೌಲ್ಯ  1
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_DOMAIN 2
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_ಮಾರ್ಗ   3

#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_PIECES 4 /* the list above */

ಸ್ಥಿರ CURLcode storecookie(ರಚನೆ Cookie *co, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_str *cp,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = strstore(&co->ಮೌಲ್ಯ, curlx_str(&cp[COOKIE_ಮೌಲ್ಯ]),
                      curlx_strlen(&cp[COOKIE_ಮೌಲ್ಯ]));
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    size_t plen = 0;
    ಆದರೆ(curlx_strlen(&cp[COOKIE_ಮಾರ್ಗ])) {
      ಮಾರ್ಗ = curlx_str(&cp[COOKIE_ಮಾರ್ಗ]);
      plen = curlx_strlen(&cp[COOKIE_ಮಾರ್ಗ]);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮಾರ್ಗ) {
      /* No path was given in the header line, set the default */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *endslash = strrchr(ಮಾರ್ಗ, '/');
      ಆದರೆ(endslash)
        plen = endslash - ಮಾರ್ಗ + 1; /* include end slash */
      ಇಲ್ಲದಿದ್ದರೆ
        plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
    }

    ಆದರೆ(ಮಾರ್ಗ) {
      co->ಮಾರ್ಗ = sanitize_cookie_ಮಾರ್ಗ(ಮಾರ್ಗ, plen);
      ಆದರೆ(!co->ಮಾರ್ಗ)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(curlx_strlen(&cp[COOKIE_DOMAIN]))
      ಫಲಿತಾಂಶ = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      ಆದರೆ(!co->domain)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ಸ್ಥಿರ bool parse_ಮೊದಲ_pair(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Cookie *co,
                             ರಚನೆ Curl_str *cookie,
                             ರಚನೆ Curl_str *name,
                             ರಚನೆ Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  ಆದರೆ(!sep || !curlx_strlen(name)) {
    infof(ದತ್ತಾಂಶ, "invalid cookie, dropped");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  ಆದರೆ((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(ದತ್ತಾಂಶ, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    ಹಿಂತಿರುಗಿ FALSE;
  }

  /* Check if we have a reserved prefix set. */
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("__Secure-", curlx_str(name), 9))
    co->ಪೂರ್ವಪ್ರತ್ಯಯ_secure = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("__Host-", curlx_str(name), 7))
    co->ಪೂರ್ವಪ್ರತ್ಯಯ_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_ಮೌಲ್ಯ] = *val;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool parse_domain(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Cookie *co,
                         ರಚನೆ Curl_str *cookie_domain,
                         ರಚನೆ Curl_str *val,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **domainp)
{
  bool is_ip;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain = *domainp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  ಆದರೆ('.' == *v)
    curlx_str_nudge(val, 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  ಆದರೆ(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#ಆದರೆ_ಅಂತ್ಯ

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  ಆದರೆ(!domain ||
     (is_ip &&
      !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    ಆದರೆ(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(ದತ್ತಾಂಶ, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ parse_maxage(ರಚನೆ Cookie *co, ರಚನೆ Curl_str *val,
                         time_t *nowp)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *maxage = curlx_str(val);
  ಆದರೆ(*maxage == '\"')
    maxage++;
  rc = curlx_str_ಸಂಖ್ಯೆ(&maxage, &co->expires, CURL_OFF_T_MAX);
  ಆದರೆ(!*nowp)
    *nowp = ಸಮಯ(ಖಾಲಿ);
  ಆಯ್ಕೆ(rc) {
  ಸ್ಥಿತಿ STRE_ಅತಿಪ್ರವಾಹ:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ STRE_OK:
    ಆದರೆ(!co->expires)
      co->expires = 1; /* expire now */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    ಇಲ್ಲದಿದ್ದರೆ
      co->expires += *nowp;
    ನಿಲ್ಲಿಸು;
  }
  cap_expires(*nowp, co);
}

ಸ್ಥಿರ ಶೂನ್ಯ parse_expires(ರಚನೆ Cookie *co, ರಚನೆ Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  ಆದರೆ(!co->expires && (curlx_strlen(val) < MAX_DATE_ಉದ್ದಳತೆ)) {
    ಅಕ್ಷರ dbuf[MAX_DATE_ಉದ್ದಳತೆ + 1];
    time_t date = 0;
    ಸ್ಮರಣೆ_ನಕಲು(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    ಆದರೆ(!Curl_getdate_capped(dbuf, &date)) {
      ಆದರೆ(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      co->expires = 0;
    ಆದರೆ(!*nowp)
      *nowp = ಸಮಯ(ಖಾಲಿ);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ಸ್ಥಿರ CURLcode
parse_cookie_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ರಚನೆ Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, /* the header */
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptr);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  ಆದರೆ(linelength > MAX_COOKIE_ಸಾಲು)
    /* discard overly long lines at once */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, ಗಾತ್ರ(cookie));
  ಮಾಡು {
    ರಚನೆ Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    ಪೂರ್ಣಾಂಕ rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    ಆದರೆ(!rc || (rc == STRE_SHORT)) {
      ರಚನೆ Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      ಆದರೆ(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(ದತ್ತಾಂಶ, "invalid octets in name, cookie dropped");
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }

      ಆದರೆ(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        ಆದರೆ(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        ಆದರೆ(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(ದತ್ತಾಂಶ, "invalid octets in value, cookie dropped");
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ
        curlx_str_init(&val);

      ಆದರೆ(!curlx_strlen(&cookie[COOKIE_NAME])) {
        ಆದರೆ(!parse_ಮೊದಲ_pair(ದತ್ತಾಂಶ, co, cookie, &name, &val, sep))
          ಹಿಂತಿರುಗಿ CURLE_OK;
      }

      /* whether there's separator or not */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "secure")) {
        ಆದರೆ(secure_origin)
          co->secure = TRUE;
        ಇಲ್ಲದಿದ್ದರೆ {
          infof(ದತ್ತಾಂಶ, "skipped cookie because not 'secure'");
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sep) {
        /* only if a separating equals sign is present */
        ಆದರೆ(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_ಮಾರ್ಗ] = val;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "domain")) {
          ಆದರೆ(curlx_strlen(&val) &&
             !parse_domain(ದತ್ತಾಂಶ, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } ಆಗ(!curlx_str_single(&ptr, ';'));

  ಆದರೆ(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    ಫಲಿತಾಂಶ = storecookie(co, &cookie[0], ಮಾರ್ಗ, domain);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *okay = TRUE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode parse_netscape(ರಚನೆ Cookie *co,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ CookieInfo *ci,
                               bool *okay,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, *ಮುಂದಿನ;
  ಪೂರ್ಣಾಂಕ fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  ಆದರೆ(lineptr[0] == '#')
    /* do not even try the comments */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  ಗಾಗಿ(ಮುಂದಿನ = lineptr; ಮುಂದಿನ; fields++) {
    ptr = ಮುಂದಿನ;
    len = strcspn(ptr, "\t\r\n");
    ಮುಂದಿನ = (ptr[len] == '\t' ? &ptr[len + 1] : ಖಾಲಿ);
    ಆಯ್ಕೆ(fields) {
    ಸ್ಥಿತಿ 0:
      ಆದರೆ(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      ಆದರೆ(!co->domain)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
      /* The file format allows the path field to remain not filled in */
      ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("TRUE", ptr, len) && ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->ಮಾರ್ಗ = sanitize_cookie_ಮಾರ್ಗ(ptr, len);
        ಆದರೆ(!co->ಮಾರ್ಗ)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* this does not look like a path, make one up! */
        co->ಮಾರ್ಗ = curlx_strdup("/");
        ಆದರೆ(!co->ಮಾರ್ಗ)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    ಸ್ಥಿತಿ 3:
      co->secure = FALSE;
      ಆದರೆ(curl_strnequal(ptr, "TRUE", len)) {
        ಆದರೆ(secure_origin || ci->running)
          co->secure = TRUE;
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 4:
      ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&ptr, &co->expires, CURL_OFF_T_MAX))
        ಹಿಂತಿರುಗಿ CURLE_OK;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 5:
      co->name = curlx_memdup0(ptr, len);
      ಆದರೆ(!co->name)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("__Secure-", co->name, 9))
          co->ಪೂರ್ವಪ್ರತ್ಯಯ_secure = TRUE;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("__Host-", co->name, 7))
          co->ಪೂರ್ವಪ್ರತ್ಯಯ_host = TRUE;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 6:
      co->ಮೌಲ್ಯ = curlx_memdup0(ptr, len);
      ಆದರೆ(!co->ಮೌಲ್ಯ)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ನಿಲ್ಲಿಸು;
    }
  }
  ಆದರೆ(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->ಮೌಲ್ಯ = curlx_strdup("");
    ಆದರೆ(!co->ಮೌಲ್ಯ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ
      fields++;
  }

  ಆದರೆ(fields != 7)
    /* we did not find the sufficient number of fields */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  ಆದರೆ(invalid_octets(co->name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->name)) ||
     invalid_octets(co->ಮೌಲ್ಯ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->ಮೌಲ್ಯ)))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  *okay = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool is_ಬಹಿರಂಗ_suffix(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Cookie *co,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(co);
  DEBUGF(infof(ದತ್ತಾಂಶ, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  ಆದರೆ(!co->domain || Curl_host_is_ipnum(co->domain))
    ಹಿಂತಿರುಗಿ FALSE;

  ಇಲ್ಲದಿದ್ದರೆ {
    bool acceptable = FALSE;
    ಅಕ್ಷರ lcase[256];
    ಅಕ್ಷರ lcookie[256];
    size_t dlen = domain ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(domain) : 0;
    size_t clen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->domain);

    /* trim trailing dots */
    ಆದರೆ(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    ಆದರೆ(clen && (co->domain[clen - 1] == '.'))
      clen--;

    ಆದರೆ((dlen < ಗಾತ್ರ(lcase)) && (clen < ಗಾತ್ರ(lcookie))) {
      ಸ್ಥಿರಾಂಕ psl_ctx_t *psl = Curl_psl_use(ದತ್ತಾಂಶ);
      ಆದರೆ(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        ಆದರೆ(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          ಆದರೆ(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_ಬಹಿರಂಗ_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          bool suffix = psl_is_ಬಹಿರಂಗ_suffix(psl, lcookie);
          acceptable = TRUE;
          ಆದರೆ(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(ದತ್ತಾಂಶ);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        infof(ದತ್ತಾಂಶ, "libpsl problem, rejecting cookie for safety");
    }

    ಆದರೆ(!acceptable) {
      infof(ದತ್ತಾಂಶ, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)co;
  (ಶೂನ್ಯ)domain;
  DEBUGF(infof(ದತ್ತಾಂಶ, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ FALSE;
}

/* returns TRUE when replaced */
ಸ್ಥಿರ bool replace_existing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Cookie *co,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ರಚನೆ Curl_llist_ಗಂಟು *replace_n = ಖಾಲಿ;
  ರಚನೆ Curl_llist_ಗಂಟು *n;
  size_t myhash = cookiehash(co->domain);
  ಗಾಗಿ(n = Curl_llist_ತಲೆ(&ci->cookielist[myhash]); n; n = Curl_ಗಂಟು_ಮುಂದಿನ(n)) {
    ರಚನೆ Cookie *clist = Curl_ಗಂಟು_elem(n);
    ಆದರೆ(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      ಆದರೆ(clist->domain && co->domain) {
        ಆದರೆ(cookie_tailmatch(clist->domain, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!clist->domain && !co->domain)
        matching_domains = TRUE;

      ಆದರೆ(matching_domains && /* the domains were identical */
         clist->ಮಾರ್ಗ && co->ಮಾರ್ಗ && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = ಖಾಲಿ;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->ಮಾರ್ಗ[0]);
        ಆದರೆ(clist->ಮಾರ್ಗ[0])
          sep = strchr(clist->ಮಾರ್ಗ + 1, '/');
        ಆದರೆ(sep)
          cllen = sep - clist->ಮಾರ್ಗ;
        ಇಲ್ಲದಿದ್ದರೆ
          cllen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clist->ಮಾರ್ಗ);

        ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(clist->ಮಾರ್ಗ, co->ಮಾರ್ಗ, cllen)) {
          infof(ದತ್ತಾಂಶ, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          ಹಿಂತಿರುಗಿ FALSE;
        }
      }
    }

    ಆದರೆ(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      ಆದರೆ(clist->domain && co->domain) {
        ಆದರೆ(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!clist->domain && !co->domain)
        replace_old = TRUE;

      ಆದರೆ(replace_old) {
        /* the domains were identical */

        ಆದರೆ(clist->ಮಾರ್ಗ && co->ಮಾರ್ಗ &&
           strcmp(clist->ಮಾರ್ಗ, co->ಮಾರ್ಗ))
          replace_old = FALSE;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!clist->ಮಾರ್ಗ != !co->ಮಾರ್ಗ)
          replace_old = FALSE;
      }

      ಆದರೆ(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        ಹಿಂತಿರುಗಿ FALSE;
      }
      ಆದರೆ(replace_old)
        replace_n = n;
    }
  }
  ಆದರೆ(replace_n) {
    ರಚನೆ Cookie *repl = Curl_ಗಂಟು_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ಗಂಟು_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ CookieInfo *ci,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lineptr, /* first character of the line */
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain,  /* default domain */
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ flags)
{
  ರಚನೆ Cookie comem;
  ರಚನೆ Cookie *co;
  size_t myhash;
  CURLcode ಫಲಿತಾಂಶ;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(MAX_ಗಣ_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  ಆದರೆ(ದತ್ತಾಂಶ->req.setcookies >= MAX_ಗಣ_COOKIE_AMOUNT)
    ಹಿಂತಿರುಗಿ CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, ಗಾತ್ರ(comem));

  ಆದರೆ(flags & COOKIE_HTTPHEADER)
    ಫಲಿತಾಂಶ = parse_cookie_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, co, &okay,
                                 lineptr, domain, ಮಾರ್ಗ, flags & COOKIE_SECURE);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  ಆದರೆ(ಫಲಿತಾಂಶ || !okay)
    ಹೋಗು fail;

  ಆದರೆ(co->ಪೂರ್ವಪ್ರತ್ಯಯ_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    ಹೋಗು fail;

  ಆದರೆ(!(flags & COOKIE_NOPSL) && is_ಬಹಿರಂಗ_suffix(ದತ್ತಾಂಶ, co, domain))
    ಹೋಗು fail;

  ಆದರೆ(co->ಪೂರ್ವಪ್ರತ್ಯಯ_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    ಆದರೆ(co->secure && co->ಮಾರ್ಗ && !strcmp(co->ಮಾರ್ಗ, "/") && !co->tailmatch)
      ;
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೋಗು fail;
  }

  ಆದರೆ(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    ಹೋಗು fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  ಆದರೆ(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  ಆದರೆ(!replace_existing(ದತ್ತಾಂಶ, co, ci, flags & COOKIE_SECURE, &replaces))
    ಹೋಗು fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, ಗಾತ್ರ(comem));
  ಆದರೆ(!co) {
    co = &comem;
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ಗಂಟು);

  ಆದರೆ(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(ದತ್ತಾಂಶ, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->ಮೌಲ್ಯ,
          co->domain, co->ಮಾರ್ಗ, co->expires);

  ಆದರೆ(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  ಆದರೆ(co->expires && (co->expires < ci->ಮುಂದಿನ_expiration))
    ci->ಮುಂದಿನ_expiration = co->expires;

  ಆದರೆ(flags & COOKIE_HTTPHEADER)
    ದತ್ತಾಂಶ->req.setcookies++;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
fail:
  freecookie(co, FALSE);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ರಚನೆ CookieInfo *Curl_cookie_init(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ i;
  ರಚನೆ CookieInfo *ci = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ CookieInfo));
  ಆದರೆ(!ci)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  ಗಾಗಿ(i = 0; i < COOKIE_HASH_ಗಾತ್ರ; i++)
    Curl_llist_init(&ci->cookielist[i], ಖಾಲಿ);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->ಮುಂದಿನ_expiration = CURL_OFF_T_MAX;

  ಹಿಂತಿರುಗಿ ci;
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
ಸ್ಥಿರ CURLcode cookie_ತುಂಬು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                            ರಚನೆ CookieInfo *ci, ಪೂರ್ಣಾಂಕ flags)
{
  FILE *handle = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  FILE *fp = ಖಾಲಿ;
  DEBUGASSERT(ci);
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  ಆದರೆ(file && *file) {
    ಆದರೆ(!strcmp(file, "-"))
      fp = stdin;
    ಇಲ್ಲದಿದ್ದರೆ {
      fp = curlx_fopen(file, "rb");
      ಆದರೆ(!fp)
        infof(ದತ್ತಾಂಶ, "WARNING: failed to open cookie file \"%s\"", file);
      ಇಲ್ಲದಿದ್ದರೆ {
        curlx_struct_stat stat;
        ಆದರೆ((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_ಬಹುಲಕ)) {
          curlx_fclose(fp);
          fp = ಖಾಲಿ;
          infof(ದತ್ತಾಂಶ, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          handle = fp;
      }
    }
  }

  ಆದರೆ(fp) {
    ರಚನೆ dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    ಮಾಡು {
      ಫಲಿತಾಂಶ = Curl_get_ಸಾಲು(&buf, fp, &eof);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        ಆದರೆ(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        ಫಲಿತಾಂಶ = Curl_cookie_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ci, lineptr, ಖಾಲಿ, ಖಾಲಿ,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
          ಫಲಿತಾಂಶ = CURLE_OK;
      }
    } ಆಗ(!ಫಲಿತಾಂಶ && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    ಆದರೆ(handle)
      curlx_fclose(handle);
  }
  ದತ್ತಾಂಶ->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಪೂರ್ಣಾಂಕ flags)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ curl_slist *list = ದತ್ತಾಂಶ->state.cookielist;
  ಆದರೆ(list) {
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
    ಆದರೆ(!ದತ್ತಾಂಶ->cookies)
      ದತ್ತಾಂಶ->cookies = Curl_cookie_init();
    ಆದರೆ(!ದತ್ತಾಂಶ->cookies)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ {
      ದತ್ತಾಂಶ->state.cookie_engine = TRUE;
      ಆಗ(list) {
        ಫಲಿತಾಂಶ = cookie_ತುಂಬು(ದತ್ತಾಂಶ, list->ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->cookies, flags);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ನಿಲ್ಲಿಸು;
        list = list->ಮುಂದಿನ;
      }
    }
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cookie_ವಿಂಗಡಿಸು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p2)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie *c1 = *(ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie * ಸ್ಥಿರಾಂಕ *)p1;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie *c2 = *(ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie * ಸ್ಥಿರಾಂಕ *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->ಮಾರ್ಗ ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c1->ಮಾರ್ಗ) : 0;
  l2 = c2->ಮಾರ್ಗ ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c2->ಮಾರ್ಗ) : 0;

  ಆದರೆ(l1 != l2)
    ಹಿಂತಿರುಗಿ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c1->domain) : 0;
  l2 = c2->domain ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c2->domain) : 0;

  ಆದರೆ(l1 != l2)
    ಹಿಂತಿರುಗಿ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c1->name) : 0;
  l2 = c2->name ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(c2->name) : 0;

  ಆದರೆ(l1 != l2)
    ಹಿಂತಿರುಗಿ (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  ಹಿಂತಿರುಗಿ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cookie_ವಿಂಗಡಿಸು_ct(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p2)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie *c1 = *(ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie * ಸ್ಥಿರಾಂಕ *)p1;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie *c2 = *(ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie * ಸ್ಥಿರಾಂಕ *)p2;

  ಹಿಂತಿರುಗಿ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host)
{
  ಹಿಂತಿರುಗಿ Curl_xfer_is_secure(ದತ್ತಾಂಶ) ||
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
CURLcode Curl_cookie_getlist(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool *okay,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                             ರಚನೆ Curl_llist *list)
{
  size_t matches = 0;
  ಸ್ಥಿರಾಂಕ bool is_ip = Curl_host_is_ipnum(host);
  ಸ್ಥಿರಾಂಕ size_t myhash = cookiehash(host);
  ರಚನೆ Curl_llist_ಗಂಟು *n;
  ಸ್ಥಿರಾಂಕ bool secure = Curl_secure_context(ದತ್ತಾಂಶ, host);
  ರಚನೆ CookieInfo *ci = ದತ್ತಾಂಶ->cookies;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ಖಾಲಿ);

  ಆದರೆ(!ci || !Curl_llist_ಎಣಿಕೆ(&ci->cookielist[myhash]))
    ಹಿಂತಿರುಗಿ CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  ಗಾಗಿ(n = Curl_llist_ತಲೆ(&ci->cookielist[myhash]); n; n = Curl_ಗಂಟು_ಮುಂದಿನ(n)) {
    ರಚನೆ Cookie *co = Curl_ಗಂಟು_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    ಆದರೆ(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      ಆದರೆ(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        ಆದರೆ(!co->ಮಾರ್ಗ || pathmatch(co->ಮಾರ್ಗ, ಮಾರ್ಗ)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          ಆದರೆ(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(ದತ್ತಾಂಶ, "Included max number of cookies (%zu) in request!",
                  matches);
            ನಿಲ್ಲಿಸು;
          }
        }
      }
    }
  }

  ಆದರೆ(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    ರಚನೆ Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(ಗಾತ್ರ(ರಚನೆ Cookie *) * matches);
    ಆದರೆ(!array) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು fail;
    }

    n = Curl_llist_ತಲೆ(list);

    ಗಾಗಿ(i = 0; n; n = Curl_ಗಂಟು_ಮುಂದಿನ(n))
      array[i++] = Curl_ಗಂಟು_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, ಗಾತ್ರ(ರಚನೆ Cookie *), cookie_ವಿಂಗಡಿಸು);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ಖಾಲಿ);

    ಗಾಗಿ(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
ಶೂನ್ಯ Curl_cookie_clearall(ರಚನೆ CookieInfo *ci)
{
  ಆದರೆ(ci) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ(i = 0; i < COOKIE_HASH_ಗಾತ್ರ; i++) {
      ರಚನೆ Curl_llist_ಗಂಟು *n;
      ಗಾಗಿ(n = Curl_llist_ತಲೆ(&ci->cookielist[i]); n;) {
        ರಚನೆ Cookie *c = Curl_ಗಂಟು_elem(n);
        ರಚನೆ Curl_llist_ಗಂಟು *e = Curl_ಗಂಟು_ಮುಂದಿನ(n);
        Curl_ಗಂಟು_remove(n);
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
ಶೂನ್ಯ Curl_cookie_clearsess(ರಚನೆ CookieInfo *ci)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ(!ci)
    ಹಿಂತಿರುಗಿ;

  ಗಾಗಿ(i = 0; i < COOKIE_HASH_ಗಾತ್ರ; i++) {
    ರಚನೆ Curl_llist_ಗಂಟು *n = Curl_llist_ತಲೆ(&ci->cookielist[i]);
    ರಚನೆ Curl_llist_ಗಂಟು *e = ಖಾಲಿ;

    ಗಾಗಿ(; n; n = e) {
      ರಚನೆ Cookie *curr = Curl_ಗಂಟು_elem(n);
      e = Curl_ಗಂಟು_ಮುಂದಿನ(n); /* in case the node is removed, get it early */
      ಆದರೆ(!curr->expires) {
        Curl_ಗಂಟು_remove(n);
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
ಶೂನ್ಯ Curl_cookie_cleanup(ರಚನೆ CookieInfo *ci)
{
  ಆದರೆ(ci) {
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
ಸ್ಥಿರ ಅಕ್ಷರ *get_netscape_format(ಸ್ಥಿರಾಂಕ ರಚನೆ Cookie *co)
{
  ಹಿಂತಿರುಗಿ curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /*ಮಾರ್ಗ*/
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*ಮೌಲ್ಯ*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->ಮಾರ್ಗ ? co->ಮಾರ್ಗ : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->ಮೌಲ್ಯ ? co->ಮೌಲ್ಯ : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ಸ್ಥಿರ CURLcode cookie_output(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ CookieInfo *ci,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename)
{
  FILE *ಹೊರಹೋಗು = ಖಾಲಿ;
  bool use_stdout = FALSE;
  ಅಕ್ಷರ *tempstore = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ci)
    /* no cookie engine alive */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  ಆದರೆ(!strcmp("-", filename)) {
    /* use stdout */
    ಹೊರಹೋಗು = stdout;
    use_stdout = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_fopen(ದತ್ತಾಂಶ, filename, &ಹೊರಹೋಗು, &tempstore);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        ಹೊರಹೋಗು);

  ಆದರೆ(ci->numcookies) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
    size_t nvalid = 0;
    ರಚನೆ Cookie **array;
    ರಚನೆ Curl_llist_ಗಂಟು *n;

    array = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Cookie *) * ci->numcookies);
    ಆದರೆ(!array) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು error;
    }

    /* only sort the cookies with a domain property */
    ಗಾಗಿ(i = 0; i < COOKIE_HASH_ಗಾತ್ರ; i++) {
      ಗಾಗಿ(n = Curl_llist_ತಲೆ(&ci->cookielist[i]); n; n = Curl_ಗಂಟು_ಮುಂದಿನ(n)) {
        ರಚನೆ Cookie *co = Curl_ಗಂಟು_elem(n);
        ಆದರೆ(!co->domain)
          ಮುಂದುವರಿ;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, ಗಾತ್ರ(ರಚನೆ Cookie *), cookie_ವಿಂಗಡಿಸು_ct);

    ಗಾಗಿ(i = 0; i < nvalid; i++) {
      ಅಕ್ಷರ *format_ptr = get_netscape_format(array[i]);
      ಆದರೆ(!format_ptr) {
        curlx_free(array);
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು error;
      }
      curl_mfprintf(ಹೊರಹೋಗು, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  ಆದರೆ(!use_stdout) {
    curlx_fclose(ಹೊರಹೋಗು);
    ಹೊರಹೋಗು = ಖಾಲಿ;
    ಆದರೆ(tempstore && curlx_rename(tempstore, filename)) {
      ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
      ಹೋಗು error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  ಹಿಂತಿರುಗಿ CURLE_OK;

error:
  ಆದರೆ(ಹೊರಹೋಗು && !use_stdout)
    curlx_fclose(ಹೊರಹೋಗು);
  ಆದರೆ(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ರಚನೆ curl_slist *cookie_list(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_slist *list = ಖಾಲಿ;
  ರಚನೆ curl_slist *beg;
  ರಚನೆ curl_slist **ಬಾಲ = &list;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  ರಚನೆ Curl_llist_ಗಂಟು *n;

  ಆದರೆ(!ದತ್ತಾಂಶ->cookies || (ದತ್ತಾಂಶ->cookies->numcookies == 0))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* at first, remove expired cookies */
  remove_expired(ದತ್ತಾಂಶ->cookies);

  ಗಾಗಿ(i = 0; i < COOKIE_HASH_ಗಾತ್ರ; i++) {
    ಗಾಗಿ(n = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->cookies->cookielist[i]); n;
        n = Curl_ಗಂಟು_ಮುಂದಿನ(n)) {
      ರಚನೆ Cookie *c = Curl_ಗಂಟು_elem(n);
      ಅಕ್ಷರ *ಸಾಲು;
      ಆದರೆ(!c->domain)
        ಮುಂದುವರಿ;
      ಸಾಲು = get_netscape_format(c);
      ಆದರೆ(!ಸಾಲು) {
        curl_slist_free_all(list);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
      beg = Curl_slist_append_nodup(ಖಾಲಿ, ಸಾಲು);
      ಆದರೆ(!beg) {
        curlx_free(ಸಾಲು);
        curl_slist_free_all(list);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
      *ಬಾಲ = beg;
      ಬಾಲ = &beg->ಮುಂದಿನ;
    }
  }

  ಹಿಂತಿರುಗಿ list;
}

ರಚನೆ curl_slist *Curl_cookie_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_slist *list;
  Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
  list = cookie_list(ದತ್ತಾಂಶ);
  Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
  ಹಿಂತಿರುಗಿ list;
}

ಶೂನ್ಯ Curl_flush_cookies(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool cleanup)
{
  Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  ಆದರೆ(ದತ್ತಾಂಶ->cookies) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cookiejar = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_COOKIEJAR);
    ಆದರೆ(cookiejar && ದತ್ತಾಂಶ->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode ಫಲಿತಾಂಶ = cookie_output(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->cookies, cookiejar);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        infof(ದತ್ತಾಂಶ, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(ಫಲಿತಾಂಶ));
    }

    ಆದರೆ(cleanup && (!ದತ್ತಾಂಶ->share || (ದತ್ತಾಂಶ->cookies != ದತ್ತಾಂಶ->share->cookies))) {
      Curl_cookie_cleanup(ದತ್ತಾಂಶ->cookies);
      ದತ್ತಾಂಶ->cookies = ಖಾಲಿ;
    }
  }
  Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
}

ಶೂನ್ಯ Curl_cookie_ಸತತ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
  ಆದರೆ(ದತ್ತಾಂಶ->cookies)
    ದತ್ತಾಂಶ->cookies->running = TRUE;
  Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
