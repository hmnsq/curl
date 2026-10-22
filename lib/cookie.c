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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cookie.h"
#અન્તર્ભાવ "psl.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "slist.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "curl_fopen.h"
#અન્તર્ભાવ "curl_get_line.h"
#અન્તર્ભાવ "curl_memrchr.h"
#અન્તર્ભાવ "parsedate.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "llist.h"
#અન્તર્ભાવ "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
સ્થિર શૂન્ય cap_expires(time_t now, સંરચના Cookie *co)
{
  જો(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    જો(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

સ્થિર શૂન્ય freecookie(સંરચના Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->કિંમત);
  જો(maintoo)
    curlx_free(co);
}

સ્થિર bool cookie_tailmatch(અચલ અક્ષર *cookie_domain,
                             અચલ size_t cookie_domain_len,
                             અચલ અક્ષર *hostname)
{
  size_t hostname_len = strlen(hostname);

  જો(hostname_len < cookie_domain_len)
    પાછા FALSE;

  જો(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    પાછા FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  જો(hostname_len == cookie_domain_len)
    પાછા TRUE;
  જો('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    પાછા TRUE;
  પાછા FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
સ્થિર bool pathmatch(અચલ અક્ષર *cookie_path, અચલ અક્ષર *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  જો(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    પાછા TRUE;
  }

  /* #-fragments are already cut off! */
  જો(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  જો(uri_path_len < cookie_path_len)
    જાઓ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  જો(strncmp(cookie_path, uri_path, cookie_path_len))
    જાઓ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  જો(cookie_path_len == uri_path_len) {
    ret = TRUE;
    જાઓ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  જો(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    જાઓ pathmatched;
  }

pathmatched:
  પાછા ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
સ્થિર અચલ અક્ષર *get_ટોચ_domain(અચલ અક્ષર * અચલ domain, size_t *outlen)
{
  size_t len = 0;
  અચલ અક્ષર *first = ખાલી, *last;

  જો(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    જો(last) {
      first = memrchr(domain, '.', (last - domain));
      જો(first)
        len -= (++first - domain);
    }
  }

  જો(outlen)
    *outlen = len;

  પાછા first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#જો defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
સ્થિર size_t cookie_hash_domain(અચલ અક્ષર *domain, અચલ size_t len)
{
  અચલ અક્ષર *end = domain + len;
  size_t h = 5381;

  જ્યારે(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  પાછા (h % COOKIE_HASH_SIZE);
}

#જો defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
સ્થિર size_t cookiehash(અચલ અક્ષર * અચલ domain)
{
  અચલ અક્ષર *ટોચ;
  size_t len;

  જો(!domain || Curl_host_is_ipnum(domain))
    પાછા 0;

  ટોચ = get_ટોચ_domain(domain, &len);
  પાછા cookie_hash_domain(ટોચ, len);
}

/*
 * cookie path sanitize
 */
સ્થિર અક્ષર *sanitize_cookie_path(અચલ અક્ષર *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  જો(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    જો(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  જો(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    પાછા curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  જો(len > 1 && cookie_path[len - 1] == '/')
    len--;

  પાછા curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
સ્થિર CURLcode strstore(અક્ષર **str, અચલ અક્ષર *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  જો(!*str)
    પાછા CURLE_OUT_OF_MEMORY;
  પાછા CURLE_OK;
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
સ્થિર શૂન્ય remove_expired(સંરચના CookieInfo *ci)
{
  સંરચના Cookie *co;
  curl_off_t now = (curl_off_t)time(ખાલી);
  unsigned પૂર્ણાંક i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  જો(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    પાછા;
  નહીંતર
    ci->next_expiration = CURL_OFF_T_MAX;

  માટે(i = 0; i < COOKIE_HASH_SIZE; i++) {
    સંરચના Curl_llist_ગાંઠ *n;
    સંરચના Curl_llist_ગાંઠ *e = ખાલી;

    માટે(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_ગાંઠ_elem(n);
      e = Curl_ગાંઠ_next(n);
      જો(co->expires) {
        જો(co->expires < now) {
          Curl_ગાંઠ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        નહીંતર જો(co->expires < ci->next_expiration)
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
સ્થિર bool bad_domain(અચલ અક્ષર *domain, size_t len)
{
  જો((len == 9) && curl_strnequal(domain, "localhost", 9))
    પાછા FALSE;
  નહીંતર {
    /* there must be a dot present, but that dot must not be a trailing dot */
    અચલ અક્ષર *dot = memchr(domain, '.', len);
    જો(dot) {
      size_t i = dot - domain;
      જો((len - i) > 1)
        /* the dot is not the last byte */
        પાછા FALSE;
    }
  }
  પાછા TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
સ્થિર bool invalid_octets(અચલ અક્ષર *ptr, size_t len)
{
  અચલ unsigned અક્ષર *p = (અચલ unsigned અક્ષર *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  જ્યારે(len && *p) {
    જો((*p < 0x20) || (*p == 0x7f))
      પાછા TRUE;
    p++;
    len--;
  }
  પાછા FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_કિંમત  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

સ્થિર CURLcode storecookie(સંરચના Cookie *co, અચલ સંરચના Curl_str *cp,
                            અચલ અક્ષર *path, અચલ અક્ષર *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  જો(!result)
    result = strstore(&co->કિંમત, curlx_str(&cp[COOKIE_કિંમત]),
                      curlx_strlen(&cp[COOKIE_કિંમત]));
  જો(!result) {
    size_t plen = 0;
    જો(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    નહીંતર જો(path) {
      /* No path was given in the header line, set the default */
      અચલ અક્ષર *endslash = strrchr(path, '/');
      જો(endslash)
        plen = endslash - path + 1; /* include end slash */
      નહીંતર
        plen = strlen(path);
    }

    જો(path) {
      co->path = sanitize_cookie_path(path, plen);
      જો(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  જો(!result) {
    જો(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    નહીંતર જો(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      જો(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  પાછા result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
સ્થિર bool parse_first_pair(સંરચના Curl_easy *data, સંરચના Cookie *co,
                             સંરચના Curl_str *cookie,
                             સંરચના Curl_str *name,
                             સંરચના Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  જો(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    પાછા FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  જો((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    પાછા FALSE;
  }

  /* Check if we have a reserved prefix set. */
  જો(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  નહીંતર જો(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_કિંમત] = *val;
  પાછા TRUE;
}

સ્થિર bool parse_domain(સંરચના Curl_easy *data, સંરચના Cookie *co,
                         સંરચના Curl_str *cookie_domain,
                         સંરચના Curl_str *val,
                         અચલ અક્ષર **domainp)
{
  bool is_ip;
  અચલ અક્ષર *domain = *domainp;
  અચલ અક્ષર *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  જો('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  જો(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  જો(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    જો(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  નહીંતર {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    પાછા FALSE;
  }
  પાછા TRUE;
}

સ્થિર શૂન્ય parse_maxage(સંરચના Cookie *co, સંરચના Curl_str *val,
                         time_t *nowp)
{
  પૂર્ણાંક rc;
  અચલ અક્ષર *maxage = curlx_str(val);
  જો(*maxage == '\"')
    maxage++;
  rc = curlx_str_સંખ્યા(&maxage, &co->expires, CURL_OFF_T_MAX);
  જો(!*nowp)
    *nowp = time(ખાલી);
  પસંદગી(rc) {
  સ્થિતિ STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    તોડો;
  મૂળભૂત:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    તોડો;
  સ્થિતિ STRE_OK:
    જો(!co->expires)
      co->expires = 1; /* expire now */
    નહીંતર જો(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    નહીંતર
      co->expires += *nowp;
    તોડો;
  }
  cap_expires(*nowp, co);
}

સ્થિર શૂન્ય parse_expires(સંરચના Cookie *co, સંરચના Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  જો(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    અક્ષર dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    જો(!Curl_getdate_capped(dbuf, &date)) {
      જો(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    નહીંતર
      co->expires = 0;
    જો(!*nowp)
      *nowp = time(ખાલી);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
સ્થિર CURLcode
parse_cookie_header(સંરચના Curl_easy *data,
                    સંરચના Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    અચલ અક્ષર *ptr, /* the header */
                    અચલ અક્ષર *domain, /* default domain */
                    /* full path used when this cookie is set */
                    અચલ અક્ષર *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  સંરચના Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  જો(linelength > MAX_COOKIE_લીટી)
    /* discard overly long lines at once */
    પાછા CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, કદ(cookie));
  કરો {
    સંરચના Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    પૂર્ણાંક rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    જો(!rc || (rc == STRE_SHORT)) {
      સંરચના Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      જો(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        પાછા CURLE_OK;
      }

      જો(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        જો(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        જો(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          પાછા CURLE_OK;
        }
      }
      નહીંતર
        curlx_str_init(&val);

      જો(!curlx_strlen(&cookie[COOKIE_NAME])) {
        જો(!parse_first_pair(data, co, cookie, &name, &val, sep))
          પાછા CURLE_OK;
      }

      /* whether there's separator or not */
      નહીંતર જો(curlx_str_casecompare(&name, "secure")) {
        જો(secure_origin)
          co->secure = TRUE;
        નહીંતર {
          infof(data, "skipped cookie because not 'secure'");
          પાછા CURLE_OK;
        }
      }
      નહીંતર જો(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      નહીંતર જો(sep) {
        /* only if a separating equals sign is present */
        જો(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        નહીંતર જો(curlx_str_casecompare(&name, "domain")) {
          જો(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            પાછા CURLE_OK;
        }
        નહીંતર જો(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        નહીંતર જો(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } જ્યારે(!curlx_str_single(&ptr, ';'));

  જો(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    જો(!result)
      *okay = TRUE;
  }
  પાછા result;
}

સ્થિર CURLcode parse_netscape(સંરચના Cookie *co,
                               અચલ સંરચના CookieInfo *ci,
                               bool *okay,
                               અચલ અક્ષર *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  અચલ અક્ષર *ptr, *next;
  પૂર્ણાંક fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  જો(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  જો(lineptr[0] == '#')
    /* do not even try the comments */
    પાછા CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  માટે(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : ખાલી);
    પસંદગી(fields) {
    સ્થિતિ 0:
      જો(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      જો(!co->domain)
        પાછા CURLE_OUT_OF_MEMORY;
      તોડો;
    સ્થિતિ 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      તોડો;
    સ્થિતિ 2:
      /* The file format allows the path field to remain not filled in */
      જો(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        જો(!co->path)
          પાછા CURLE_OUT_OF_MEMORY;
        તોડો;
      }
      નહીંતર {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        જો(!co->path)
          પાછા CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    સ્થિતિ 3:
      co->secure = FALSE;
      જો(curl_strnequal(ptr, "TRUE", len)) {
        જો(secure_origin || ci->running)
          co->secure = TRUE;
        નહીંતર
          પાછા CURLE_OK;
      }
      તોડો;
    સ્થિતિ 4:
      જો(curlx_str_સંખ્યા(&ptr, &co->expires, CURL_OFF_T_MAX))
        પાછા CURLE_OK;
      તોડો;
    સ્થિતિ 5:
      co->name = curlx_memdup0(ptr, len);
      જો(!co->name)
        પાછા CURLE_OUT_OF_MEMORY;
      નહીંતર {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        જો(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        નહીંતર જો(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      તોડો;
    સ્થિતિ 6:
      co->કિંમત = curlx_memdup0(ptr, len);
      જો(!co->કિંમત)
        પાછા CURLE_OUT_OF_MEMORY;
      તોડો;
    }
  }
  જો(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->કિંમત = curlx_strdup("");
    જો(!co->કિંમત)
      પાછા CURLE_OUT_OF_MEMORY;
    નહીંતર
      fields++;
  }

  જો(fields != 7)
    /* we did not find the sufficient number of fields */
    પાછા CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  જો(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->કિંમત, strlen(co->કિંમત)))
    પાછા CURLE_OK;

  *okay = TRUE;
  પાછા CURLE_OK;
}

સ્થિર bool is_public_suffix(સંરચના Curl_easy *data,
                             સંરચના Cookie *co,
                             અચલ અક્ષર *domain)
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
  જો(!co->domain || Curl_host_is_ipnum(co->domain))
    પાછા FALSE;

  નહીંતર {
    bool acceptable = FALSE;
    અક્ષર lcase[256];
    અક્ષર lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    જો(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    જો(clen && (co->domain[clen - 1] == '.'))
      clen--;

    જો((dlen < કદ(lcase)) && (clen < કદ(lcookie))) {
      અચલ psl_ctx_t *psl = Curl_psl_use(data);
      જો(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        જો(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          જો(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        નહીંતર {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          જો(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      નહીંતર
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    જો(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      પાછા TRUE;
    }
  }
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)co;
  (શૂન્ય)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  પાછા FALSE;
}

/* returns TRUE when replaced */
સ્થિર bool replace_existing(સંરચના Curl_easy *data,
                             સંરચના Cookie *co,
                             અચલ સંરચના CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  સંરચના Curl_llist_ગાંઠ *replace_n = ખાલી;
  સંરચના Curl_llist_ગાંઠ *n;
  size_t myhash = cookiehash(co->domain);
  માટે(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ગાંઠ_next(n)) {
    સંરચના Cookie *clist = Curl_ગાંઠ_elem(n);
    જો(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      જો(clist->domain && co->domain) {
        જો(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      નહીંતર જો(!clist->domain && !co->domain)
        matching_domains = TRUE;

      જો(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        અચલ અક્ષર *sep = ખાલી;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        જો(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        જો(sep)
          cllen = sep - clist->path;
        નહીંતર
          cllen = strlen(clist->path);

        જો(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          પાછા FALSE;
        }
      }
    }

    જો(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      જો(clist->domain && co->domain) {
        જો(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      નહીંતર જો(!clist->domain && !co->domain)
        replace_old = TRUE;

      જો(replace_old) {
        /* the domains were identical */

        જો(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        નહીંતર જો(!clist->path != !co->path)
          replace_old = FALSE;
      }

      જો(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        પાછા FALSE;
      }
      જો(replace_old)
        replace_n = n;
    }
  }
  જો(replace_n) {
    સંરચના Cookie *repl = Curl_ગાંઠ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ગાંઠ_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  પાછા TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(સંરચના Curl_easy *data,
                         સંરચના CookieInfo *ci,
                         અચલ અક્ષર *lineptr, /* first character of the line */
                         અચલ અક્ષર *domain,  /* default domain */
                         અચલ અક્ષર *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         અચલ પૂર્ણાંક flags)
{
  સંરચના Cookie comem;
  સંરચના Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  જો(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    પાછા CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, કદ(comem));

  જો(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  નહીંતર
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  જો(result || !okay)
    જાઓ fail;

  જો(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    જાઓ fail;

  જો(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    જાઓ fail;

  જો(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    જો(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    નહીંતર
      જાઓ fail;
  }

  જો(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    જાઓ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  જો(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  જો(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    જાઓ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, કદ(comem));
  જો(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    જાઓ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ગાંઠ);

  જો(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->કિંમત,
          co->domain, co->path, co->expires);

  જો(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  જો(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  જો(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  પાછા result;
fail:
  freecookie(co, FALSE);
  પાછા result;
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
સંરચના CookieInfo *Curl_cookie_init(શૂન્ય)
{
  પૂર્ણાંક i;
  સંરચના CookieInfo *ci = curlx_calloc(1, કદ(સંરચના CookieInfo));
  જો(!ci)
    પાછા ખાલી;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  માટે(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], ખાલી);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  પાછા ci;
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
સ્થિર CURLcode cookie_load(સંરચના Curl_easy *data, અચલ અક્ષર *file,
                            સંરચના CookieInfo *ci, પૂર્ણાંક flags)
{
  FILE *handle = ખાલી;
  CURLcode result = CURLE_OK;
  FILE *fp = ખાલી;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  જો(file && *file) {
    જો(!strcmp(file, "-"))
      fp = stdin;
    નહીંતર {
      fp = curlx_fopen(file, "rb");
      જો(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      નહીંતર {
        curlx_struct_stat stat;
        જો((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = ખાલી;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        નહીંતર
          handle = fp;
      }
    }
  }

  જો(fp) {
    સંરચના dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    કરો {
      result = Curl_get_લીટી(&buf, fp, &eof);
      જો(!result) {
        અચલ અક્ષર *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        જો(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, ખાલી, ખાલી,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        જો(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } જ્યારે(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    જો(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  પાછા result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(સંરચના Curl_easy *data,
                               પૂર્ણાંક flags)
{
  CURLcode result = CURLE_OK;
  સંરચના curl_slist *list = data->state.cookielist;
  જો(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    જો(!data->cookies)
      data->cookies = Curl_cookie_init();
    જો(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    નહીંતર {
      data->state.cookie_engine = TRUE;
      જ્યારે(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        જો(result)
          તોડો;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  પાછા result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
સ્થિર પૂર્ણાંક cookie_sort(અચલ શૂન્ય *p1, અચલ શૂન્ય *p2)
{
  અચલ સંરચના Cookie *c1 = *(અચલ સંરચના Cookie * અચલ *)p1;
  અચલ સંરચના Cookie *c2 = *(અચલ સંરચના Cookie * અચલ *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  જો(l1 != l2)
    પાછા (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  જો(l1 != l2)
    પાછા (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  જો(l1 != l2)
    પાછા (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  પાછા (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
સ્થિર પૂર્ણાંક cookie_sort_ct(અચલ શૂન્ય *p1, અચલ શૂન્ય *p2)
{
  અચલ સંરચના Cookie *c1 = *(અચલ સંરચના Cookie * અચલ *)p1;
  અચલ સંરચના Cookie *c2 = *(અચલ સંરચના Cookie * અચલ *)p2;

  પાછા (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(સંરચના Curl_easy *data, અચલ અક્ષર *host)
{
  પાછા Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(સંરચના Curl_easy *data,
                             bool *okay,
                             અચલ અક્ષર *host,
                             સંરચના Curl_llist *list)
{
  size_t matches = 0;
  અચલ bool is_ip = Curl_host_is_ipnum(host);
  અચલ size_t myhash = cookiehash(host);
  સંરચના Curl_llist_ગાંઠ *n;
  અચલ bool secure = Curl_secure_context(data, host);
  સંરચના CookieInfo *ci = data->cookies;
  અચલ અક્ષર *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ખાલી);

  જો(!ci || !Curl_llist_ગણતરી(&ci->cookielist[myhash]))
    પાછા CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  માટે(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ગાંઠ_next(n)) {
    સંરચના Cookie *co = Curl_ગાંઠ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    જો(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      જો(!co->domain ||
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
        જો(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          જો(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            તોડો;
          }
        }
      }
    }
  }

  જો(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    સંરચના Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(કદ(સંરચના Cookie *) * matches);
    જો(!array) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ fail;
    }

    n = Curl_llist_head(list);

    માટે(i = 0; n; n = Curl_ગાંઠ_next(n))
      array[i++] = Curl_ગાંઠ_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, કદ(સંરચના Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ખાલી);

    માટે(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  પાછા CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ખાલી);
  પાછા result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
શૂન્ય Curl_cookie_clearall(સંરચના CookieInfo *ci)
{
  જો(ci) {
    unsigned પૂર્ણાંક i;
    માટે(i = 0; i < COOKIE_HASH_SIZE; i++) {
      સંરચના Curl_llist_ગાંઠ *n;
      માટે(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        સંરચના Cookie *c = Curl_ગાંઠ_elem(n);
        સંરચના Curl_llist_ગાંઠ *e = Curl_ગાંઠ_next(n);
        Curl_ગાંઠ_remove(n);
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
શૂન્ય Curl_cookie_clearsess(સંરચના CookieInfo *ci)
{
  unsigned પૂર્ણાંક i;

  જો(!ci)
    પાછા;

  માટે(i = 0; i < COOKIE_HASH_SIZE; i++) {
    સંરચના Curl_llist_ગાંઠ *n = Curl_llist_head(&ci->cookielist[i]);
    સંરચના Curl_llist_ગાંઠ *e = ખાલી;

    માટે(; n; n = e) {
      સંરચના Cookie *curr = Curl_ગાંઠ_elem(n);
      e = Curl_ગાંઠ_next(n); /* in case the node is removed, get it early */
      જો(!curr->expires) {
        Curl_ગાંઠ_remove(n);
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
શૂન્ય Curl_cookie_cleanup(સંરચના CookieInfo *ci)
{
  જો(ci) {
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
સ્થિર અક્ષર *get_netscape_format(અચલ સંરચના Cookie *co)
{
  પાછા curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*કિંમત*/
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
    co->કિંમત ? co->કિંમત : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
સ્થિર CURLcode cookie_output(સંરચના Curl_easy *data,
                              સંરચના CookieInfo *ci,
                              અચલ અક્ષર *filename)
{
  FILE *out = ખાલી;
  bool use_stdout = FALSE;
  અક્ષર *tempstore = ખાલી;
  CURLcode result = CURLE_OK;

  જો(!ci)
    /* no cookie engine alive */
    પાછા CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  જો(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  નહીંતર {
    result = Curl_fopen(data, filename, &out, &tempstore);
    જો(result)
      જાઓ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  જો(ci->numcookies) {
    unsigned પૂર્ણાંક i;
    size_t nvalid = 0;
    સંરચના Cookie **array;
    સંરચના Curl_llist_ગાંઠ *n;

    array = curlx_calloc(1, કદ(સંરચના Cookie *) * ci->numcookies);
    જો(!array) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ error;
    }

    /* only sort the cookies with a domain property */
    માટે(i = 0; i < COOKIE_HASH_SIZE; i++) {
      માટે(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_ગાંઠ_next(n)) {
        સંરચના Cookie *co = Curl_ગાંઠ_elem(n);
        જો(!co->domain)
          ચાલુ;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, કદ(સંરચના Cookie *), cookie_sort_ct);

    માટે(i = 0; i < nvalid; i++) {
      અક્ષર *format_ptr = get_netscape_format(array[i]);
      જો(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        જાઓ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  જો(!use_stdout) {
    curlx_fclose(out);
    out = ખાલી;
    જો(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      જાઓ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  પાછા CURLE_OK;

error:
  જો(out && !use_stdout)
    curlx_fclose(out);
  જો(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  પાછા result;
}

સ્થિર સંરચના curl_slist *cookie_list(અચલ સંરચના Curl_easy *data)
{
  સંરચના curl_slist *list = ખાલી;
  સંરચના curl_slist *beg;
  સંરચના curl_slist **tail = &list;
  unsigned પૂર્ણાંક i;
  સંરચના Curl_llist_ગાંઠ *n;

  જો(!data->cookies || (data->cookies->numcookies == 0))
    પાછા ખાલી;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  માટે(i = 0; i < COOKIE_HASH_SIZE; i++) {
    માટે(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_ગાંઠ_next(n)) {
      સંરચના Cookie *c = Curl_ગાંઠ_elem(n);
      અક્ષર *લીટી;
      જો(!c->domain)
        ચાલુ;
      લીટી = get_netscape_format(c);
      જો(!લીટી) {
        curl_slist_free_all(list);
        પાછા ખાલી;
      }
      beg = Curl_slist_append_nodup(ખાલી, લીટી);
      જો(!beg) {
        curlx_free(લીટી);
        curl_slist_free_all(list);
        પાછા ખાલી;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  પાછા list;
}

સંરચના curl_slist *Curl_cookie_list(સંરચના Curl_easy *data)
{
  સંરચના curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  પાછા list;
}

શૂન્ય Curl_flush_cookies(સંરચના Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  જો(data->cookies) {
    અચલ અક્ષર *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    જો(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      જો(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    જો(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = ખાલી;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

શૂન્ય Curl_cookie_run(સંરચના Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  જો(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
