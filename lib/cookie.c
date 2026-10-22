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

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

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
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
स्थिर शून्य cap_expires(समय_प्रकार now, संरचना Cookie *co)
{
  जर(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    जर(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

स्थिर शून्य freecookie(संरचना Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->मूल्य);
  जर(maintoo)
    curlx_free(co);
}

स्थिर bool cookie_tailmatch(स्थिरांक अक्षर *cookie_domain,
                             स्थिरांक माप_प्रकार cookie_domain_len,
                             स्थिरांक अक्षर *hostname)
{
  माप_प्रकार hostname_len = म_माप(hostname);

  जर(hostname_len < cookie_domain_len)
    परत FALSE;

  जर(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    परत FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  जर(hostname_len == cookie_domain_len)
    परत TRUE;
  जर('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    परत TRUE;
  परत FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
स्थिर bool pathmatch(स्थिरांक अक्षर *cookie_path, स्थिरांक अक्षर *uri_path)
{
  माप_प्रकार cookie_path_len;
  माप_प्रकार uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = म_माप(cookie_path);
  जर(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    परत TRUE;
  }

  /* #-fragments are already cut off! */
  जर(म_माप(uri_path) == 0 || uri_path[0] != '/')
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

  uri_path_len = म_माप(uri_path);

  जर(uri_path_len < cookie_path_len)
    जा pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  जर(म_भेदन(cookie_path, uri_path, cookie_path_len))
    जा pathmatched;

  /* The cookie-path and the uri-path are identical. */
  जर(cookie_path_len == uri_path_len) {
    ret = TRUE;
    जा pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  जर(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    जा pathmatched;
  }

pathmatched:
  परत ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
स्थिर स्थिरांक अक्षर *get_शीर्ष_domain(स्थिरांक अक्षर * स्थिरांक domain, माप_प्रकार *outlen)
{
  माप_प्रकार len = 0;
  स्थिरांक अक्षर *first = रिक्त, *last;

  जर(domain) {
    len = म_माप(domain);
    last = memrchr(domain, '.', len);
    जर(last) {
      first = memrchr(domain, '.', (last - domain));
      जर(first)
        len -= (++first - domain);
    }
  }

  जर(outlen)
    *outlen = len;

  परत first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#जर defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
स्थिर माप_प्रकार cookie_hash_domain(स्थिरांक अक्षर *domain, स्थिरांक माप_प्रकार len)
{
  स्थिरांक अक्षर *end = domain + len;
  माप_प्रकार h = 5381;

  पर्यंत(domain < end) {
    माप_प्रकार j = (माप_प्रकार)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  परत (h % COOKIE_HASH_SIZE);
}

#जर defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
स्थिर माप_प्रकार cookiehash(स्थिरांक अक्षर * स्थिरांक domain)
{
  स्थिरांक अक्षर *शीर्ष;
  माप_प्रकार len;

  जर(!domain || Curl_host_is_ipnum(domain))
    परत 0;

  शीर्ष = get_शीर्ष_domain(domain, &len);
  परत cookie_hash_domain(शीर्ष, len);
}

/*
 * cookie path sanitize
 */
स्थिर अक्षर *sanitize_cookie_path(स्थिरांक अक्षर *cookie_path, माप_प्रकार len)
{
  /* some sites send path attribute within '"'. */
  जर(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    जर(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  जर(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    परत curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  जर(len > 1 && cookie_path[len - 1] == '/')
    len--;

  परत curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
स्थिर CURLcode strstore(अक्षर **str, स्थिरांक अक्षर *newstr, माप_प्रकार len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  जर(!*str)
    परत CURLE_OUT_OF_MEMORY;
  परत CURLE_OK;
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
  curl_off_t now = (curl_off_t)समय(रिक्त);
  अचिन्हित पूर्णांक i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  जर(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    परत;
  अन्यथा
    ci->next_expiration = CURL_OFF_T_MAX;

  साठी(i = 0; i < COOKIE_HASH_SIZE; i++) {
    संरचना Curl_llist_गाठ *n;
    संरचना Curl_llist_गाठ *e = रिक्त;

    साठी(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_गाठ_elem(n);
      e = Curl_गाठ_next(n);
      जर(co->expires) {
        जर(co->expires < now) {
          Curl_गाठ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        अन्यथा जर(co->expires < ci->next_expiration)
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
स्थिर bool bad_domain(स्थिरांक अक्षर *domain, माप_प्रकार len)
{
  जर((len == 9) && curl_strnequal(domain, "localhost", 9))
    परत FALSE;
  अन्यथा {
    /* there must be a dot present, but that dot must not be a trailing dot */
    स्थिरांक अक्षर *dot = स_प्रथम(domain, '.', len);
    जर(dot) {
      माप_प्रकार i = dot - domain;
      जर((len - i) > 1)
        /* the dot is not the last byte */
        परत FALSE;
    }
  }
  परत TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
स्थिर bool invalid_octets(स्थिरांक अक्षर *ptr, माप_प्रकार len)
{
  स्थिरांक अचिन्हित अक्षर *p = (स्थिरांक अचिन्हित अक्षर *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  पर्यंत(len && *p) {
    जर((*p < 0x20) || (*p == 0x7f))
      परत TRUE;
    p++;
    len--;
  }
  परत FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_मूल्य  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

स्थिर CURLcode storecookie(संरचना Cookie *co, स्थिरांक संरचना Curl_str *cp,
                            स्थिरांक अक्षर *path, स्थिरांक अक्षर *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  जर(!result)
    result = strstore(&co->मूल्य, curlx_str(&cp[COOKIE_मूल्य]),
                      curlx_strlen(&cp[COOKIE_मूल्य]));
  जर(!result) {
    माप_प्रकार plen = 0;
    जर(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    अन्यथा जर(path) {
      /* No path was given in the header line, set the default */
      स्थिरांक अक्षर *endslash = म_खोजप(path, '/');
      जर(endslash)
        plen = endslash - path + 1; /* include end slash */
      अन्यथा
        plen = म_माप(path);
    }

    जर(path) {
      co->path = sanitize_cookie_path(path, plen);
      जर(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  जर(!result) {
    जर(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    अन्यथा जर(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      जर(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  परत result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
स्थिर bool parse_first_pair(संरचना Curl_easy *data, संरचना Cookie *co,
                             संरचना Curl_str *cookie,
                             संरचना Curl_str *name,
                             संरचना Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  जर(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    परत FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  जर((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    परत FALSE;
  }

  /* Check if we have a reserved prefix set. */
  जर(!म_भेदन("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  अन्यथा जर(!म_भेदन("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_मूल्य] = *val;
  परत TRUE;
}

स्थिर bool parse_domain(संरचना Curl_easy *data, संरचना Cookie *co,
                         संरचना Curl_str *cookie_domain,
                         संरचना Curl_str *val,
                         स्थिरांक अक्षर **domainp)
{
  bool is_ip;
  स्थिरांक अक्षर *domain = *domainp;
  स्थिरांक अक्षर *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  जर('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  जर(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  जर(!domain ||
     (is_ip &&
      !म_भेदन(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == म_माप(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    जर(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  अन्यथा {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    परत FALSE;
  }
  परत TRUE;
}

स्थिर शून्य parse_maxage(संरचना Cookie *co, संरचना Curl_str *val,
                         समय_प्रकार *nowp)
{
  पूर्णांक rc;
  स्थिरांक अक्षर *maxage = curlx_str(val);
  जर(*maxage == '\"')
    maxage++;
  rc = curlx_str_संख्या(&maxage, &co->expires, CURL_OFF_T_MAX);
  जर(!*nowp)
    *nowp = समय(रिक्त);
  निवड(rc) {
  स्थिती STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    थांबा;
  पूर्वनिर्धारित:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    थांबा;
  स्थिती STRE_OK:
    जर(!co->expires)
      co->expires = 1; /* expire now */
    अन्यथा जर(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    अन्यथा
      co->expires += *nowp;
    थांबा;
  }
  cap_expires(*nowp, co);
}

स्थिर शून्य parse_expires(संरचना Cookie *co, संरचना Curl_str *val,
                          समय_प्रकार *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  जर(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    अक्षर dbuf[MAX_DATE_LENGTH + 1];
    समय_प्रकार date = 0;
    स_नकल(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    जर(!Curl_getdate_capped(dbuf, &date)) {
      जर(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    अन्यथा
      co->expires = 0;
    जर(!*nowp)
      *nowp = समय(रिक्त);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
स्थिर CURLcode
parse_cookie_header(संरचना Curl_easy *data,
                    संरचना Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    स्थिरांक अक्षर *ptr, /* the header */
                    स्थिरांक अक्षर *domain, /* default domain */
                    /* full path used when this cookie is set */
                    स्थिरांक अक्षर *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  समय_प्रकार now = 0;
  माप_प्रकार linelength = म_माप(ptr);
  CURLcode result = CURLE_OK;
  संरचना Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  जर(linelength > MAX_COOKIE_ओळ)
    /* discard overly long lines at once */
    परत CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  स_रखो(cookie, 0, आकार(cookie));
  करा {
    संरचना Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    पूर्णांक rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    जर(!rc || (rc == STRE_SHORT)) {
      संरचना Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      जर(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        परत CURLE_OK;
      }

      जर(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        जर(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        जर(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          परत CURLE_OK;
        }
      }
      अन्यथा
        curlx_str_init(&val);

      जर(!curlx_strlen(&cookie[COOKIE_NAME])) {
        जर(!parse_first_pair(data, co, cookie, &name, &val, sep))
          परत CURLE_OK;
      }

      /* whether there's separator or not */
      अन्यथा जर(curlx_str_casecompare(&name, "secure")) {
        जर(secure_origin)
          co->secure = TRUE;
        अन्यथा {
          infof(data, "skipped cookie because not 'secure'");
          परत CURLE_OK;
        }
      }
      अन्यथा जर(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      अन्यथा जर(sep) {
        /* only if a separating equals sign is present */
        जर(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        अन्यथा जर(curlx_str_casecompare(&name, "domain")) {
          जर(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            परत CURLE_OK;
        }
        अन्यथा जर(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        अन्यथा जर(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } पर्यंत(!curlx_str_single(&ptr, ';'));

  जर(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    जर(!result)
      *okay = TRUE;
  }
  परत result;
}

स्थिर CURLcode parse_netscape(संरचना Cookie *co,
                               स्थिरांक संरचना CookieInfo *ci,
                               bool *okay,
                               स्थिरांक अक्षर *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  स्थिरांक अक्षर *ptr, *next;
  पूर्णांक fields;
  माप_प्रकार len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  जर(!म_भेदन(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  जर(lineptr[0] == '#')
    /* do not even try the comments */
    परत CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  साठी(next = lineptr; next; fields++) {
    ptr = next;
    len = म_खोज(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : रिक्त);
    निवड(fields) {
    स्थिती 0:
      जर(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      जर(!co->domain)
        परत CURLE_OUT_OF_MEMORY;
      थांबा;
    स्थिती 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      थांबा;
    स्थिती 2:
      /* The file format allows the path field to remain not filled in */
      जर(म_भेदन("TRUE", ptr, len) && म_भेदन("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        जर(!co->path)
          परत CURLE_OUT_OF_MEMORY;
        थांबा;
      }
      अन्यथा {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        जर(!co->path)
          परत CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    स्थिती 3:
      co->secure = FALSE;
      जर(curl_strnequal(ptr, "TRUE", len)) {
        जर(secure_origin || ci->running)
          co->secure = TRUE;
        अन्यथा
          परत CURLE_OK;
      }
      थांबा;
    स्थिती 4:
      जर(curlx_str_संख्या(&ptr, &co->expires, CURL_OFF_T_MAX))
        परत CURLE_OK;
      थांबा;
    स्थिती 5:
      co->name = curlx_memdup0(ptr, len);
      जर(!co->name)
        परत CURLE_OUT_OF_MEMORY;
      अन्यथा {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        जर(!म_भेदन("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        अन्यथा जर(!म_भेदन("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      थांबा;
    स्थिती 6:
      co->मूल्य = curlx_memdup0(ptr, len);
      जर(!co->मूल्य)
        परत CURLE_OUT_OF_MEMORY;
      थांबा;
    }
  }
  जर(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->मूल्य = curlx_strdup("");
    जर(!co->मूल्य)
      परत CURLE_OUT_OF_MEMORY;
    अन्यथा
      fields++;
  }

  जर(fields != 7)
    /* we did not find the sufficient number of fields */
    परत CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  जर(invalid_octets(co->name, म_माप(co->name)) ||
     invalid_octets(co->मूल्य, म_माप(co->मूल्य)))
    परत CURLE_OK;

  *okay = TRUE;
  परत CURLE_OK;
}

स्थिर bool is_public_suffix(संरचना Curl_easy *data,
                             संरचना Cookie *co,
                             स्थिरांक अक्षर *domain)
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
  जर(!co->domain || Curl_host_is_ipnum(co->domain))
    परत FALSE;

  अन्यथा {
    bool acceptable = FALSE;
    अक्षर lcase[256];
    अक्षर lcookie[256];
    माप_प्रकार dlen = domain ? म_माप(domain) : 0;
    माप_प्रकार clen = म_माप(co->domain);

    /* trim trailing dots */
    जर(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    जर(clen && (co->domain[clen - 1] == '.'))
      clen--;

    जर((dlen < आकार(lcase)) && (clen < आकार(lcookie))) {
      स्थिरांक psl_ctx_t *psl = Curl_psl_use(data);
      जर(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        जर(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          जर(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        अन्यथा {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          जर(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      अन्यथा
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    जर(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      परत TRUE;
    }
  }
#अन्यथा
  (शून्य)data;
  (शून्य)co;
  (शून्य)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  परत FALSE;
}

/* returns TRUE when replaced */
स्थिर bool replace_existing(संरचना Curl_easy *data,
                             संरचना Cookie *co,
                             स्थिरांक संरचना CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  संरचना Curl_llist_गाठ *replace_n = रिक्त;
  संरचना Curl_llist_गाठ *n;
  माप_प्रकार myhash = cookiehash(co->domain);
  साठी(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_गाठ_next(n)) {
    संरचना Cookie *clist = Curl_गाठ_elem(n);
    जर(!म_भेद(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      जर(clist->domain && co->domain) {
        जर(cookie_tailmatch(clist->domain, म_माप(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, म_माप(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      अन्यथा जर(!clist->domain && !co->domain)
        matching_domains = TRUE;

      जर(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        माप_प्रकार cllen;
        स्थिरांक अक्षर *sep = रिक्त;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        जर(clist->path[0])
          sep = म_अक्षर(clist->path + 1, '/');
        जर(sep)
          cllen = sep - clist->path;
        अन्यथा
          cllen = म_माप(clist->path);

        जर(!म_भेदन(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          परत FALSE;
        }
      }
    }

    जर(!replace_n && !म_भेद(clist->name, co->name)) {
      /* the names are identical */

      जर(clist->domain && co->domain) {
        जर(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      अन्यथा जर(!clist->domain && !co->domain)
        replace_old = TRUE;

      जर(replace_old) {
        /* the domains were identical */

        जर(clist->path && co->path &&
           म_भेद(clist->path, co->path))
          replace_old = FALSE;
        अन्यथा जर(!clist->path != !co->path)
          replace_old = FALSE;
      }

      जर(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        परत FALSE;
      }
      जर(replace_old)
        replace_n = n;
    }
  }
  जर(replace_n) {
    संरचना Cookie *repl = Curl_गाठ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_गाठ_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  परत TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(संरचना Curl_easy *data,
                         संरचना CookieInfo *ci,
                         स्थिरांक अक्षर *lineptr, /* first character of the line */
                         स्थिरांक अक्षर *domain,  /* default domain */
                         स्थिरांक अक्षर *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         स्थिरांक पूर्णांक flags)
{
  संरचना Cookie comem;
  संरचना Cookie *co;
  माप_प्रकार myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  जर(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    परत CURLE_OK; /* silently ignore */

  co = &comem;
  स_रखो(co, 0, आकार(comem));

  जर(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  अन्यथा
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  जर(result || !okay)
    जा fail;

  जर(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    जा fail;

  जर(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    जा fail;

  जर(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    जर(co->secure && co->path && !म_भेद(co->path, "/") && !co->tailmatch)
      ;
    अन्यथा
      जा fail;
  }

  जर(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    जा fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  जर(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  जर(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    जा fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, आकार(comem));
  जर(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    जा fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->गाठ);

  जर(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->मूल्य,
          co->domain, co->path, co->expires);

  जर(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  जर(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  जर(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  परत result;
fail:
  freecookie(co, FALSE);
  परत result;
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
  पूर्णांक i;
  संरचना CookieInfo *ci = curlx_calloc(1, आकार(संरचना CookieInfo));
  जर(!ci)
    परत रिक्त;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  साठी(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], रिक्त);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  परत ci;
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
स्थिर CURLcode cookie_load(संरचना Curl_easy *data, स्थिरांक अक्षर *file,
                            संरचना CookieInfo *ci, पूर्णांक flags)
{
  खाता *handle = रिक्त;
  CURLcode result = CURLE_OK;
  खाता *fp = रिक्त;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  जर(file && *file) {
    जर(!म_भेद(file, "-"))
      fp = मानक_निवेश;
    अन्यथा {
      fp = curlx_fopen(file, "rb");
      जर(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      अन्यथा {
        curlx_struct_stat stat;
        जर((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = रिक्त;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        अन्यथा
          handle = fp;
      }
    }
  }

  जर(fp) {
    संरचना dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    करा {
      result = Curl_get_ओळ(&buf, fp, &eof);
      जर(!result) {
        स्थिरांक अक्षर *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        जर(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, रिक्त, रिक्त,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        जर(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } पर्यंत(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    जर(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  परत result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(संरचना Curl_easy *data,
                               पूर्णांक flags)
{
  CURLcode result = CURLE_OK;
  संरचना curl_slist *list = data->state.cookielist;
  जर(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    जर(!data->cookies)
      data->cookies = Curl_cookie_init();
    जर(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      data->state.cookie_engine = TRUE;
      पर्यंत(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        जर(result)
          थांबा;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  परत result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
स्थिर पूर्णांक cookie_sort(स्थिरांक शून्य *p1, स्थिरांक शून्य *p2)
{
  स्थिरांक संरचना Cookie *c1 = *(स्थिरांक संरचना Cookie * स्थिरांक *)p1;
  स्थिरांक संरचना Cookie *c2 = *(स्थिरांक संरचना Cookie * स्थिरांक *)p2;
  माप_प्रकार l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? म_माप(c1->path) : 0;
  l2 = c2->path ? म_माप(c2->path) : 0;

  जर(l1 != l2)
    परत (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? म_माप(c1->domain) : 0;
  l2 = c2->domain ? म_माप(c2->domain) : 0;

  जर(l1 != l2)
    परत (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? म_माप(c1->name) : 0;
  l2 = c2->name ? म_माप(c2->name) : 0;

  जर(l1 != l2)
    परत (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  परत (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
स्थिर पूर्णांक cookie_sort_ct(स्थिरांक शून्य *p1, स्थिरांक शून्य *p2)
{
  स्थिरांक संरचना Cookie *c1 = *(स्थिरांक संरचना Cookie * स्थिरांक *)p1;
  स्थिरांक संरचना Cookie *c2 = *(स्थिरांक संरचना Cookie * स्थिरांक *)p2;

  परत (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(संरचना Curl_easy *data, स्थिरांक अक्षर *host)
{
  परत Curl_xfer_is_secure(data) ||
    curl_strequal("localhost", host) ||
    !म_भेद(host, "127.0.0.1") ||
    !म_भेद(host, "::1");
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
CURLcode Curl_cookie_getlist(संरचना Curl_easy *data,
                             bool *okay,
                             स्थिरांक अक्षर *host,
                             संरचना Curl_llist *list)
{
  माप_प्रकार matches = 0;
  स्थिरांक bool is_ip = Curl_host_is_ipnum(host);
  स्थिरांक माप_प्रकार myhash = cookiehash(host);
  संरचना Curl_llist_गाठ *n;
  स्थिरांक bool secure = Curl_secure_context(data, host);
  संरचना CookieInfo *ci = data->cookies;
  स्थिरांक अक्षर *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, रिक्त);

  जर(!ci || !Curl_llist_मोजणी(&ci->cookielist[myhash]))
    परत CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  साठी(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_गाठ_next(n)) {
    संरचना Cookie *co = Curl_गाठ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    जर(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      जर(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, म_माप(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        जर(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          जर(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            थांबा;
          }
        }
      }
    }
  }

  जर(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    संरचना Cookie **array;
    माप_प्रकार i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(आकार(संरचना Cookie *) * matches);
    जर(!array) {
      result = CURLE_OUT_OF_MEMORY;
      जा fail;
    }

    n = Curl_llist_head(list);

    साठी(i = 0; n; n = Curl_गाठ_next(n))
      array[i++] = Curl_गाठ_elem(n);

    /* now sort the cookie pointers in path length order */
    क्विक(array, matches, आकार(संरचना Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, रिक्त);

    साठी(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  परत CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, रिक्त);
  परत result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
शून्य Curl_cookie_clearall(संरचना CookieInfo *ci)
{
  जर(ci) {
    अचिन्हित पूर्णांक i;
    साठी(i = 0; i < COOKIE_HASH_SIZE; i++) {
      संरचना Curl_llist_गाठ *n;
      साठी(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        संरचना Cookie *c = Curl_गाठ_elem(n);
        संरचना Curl_llist_गाठ *e = Curl_गाठ_next(n);
        Curl_गाठ_remove(n);
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
  अचिन्हित पूर्णांक i;

  जर(!ci)
    परत;

  साठी(i = 0; i < COOKIE_HASH_SIZE; i++) {
    संरचना Curl_llist_गाठ *n = Curl_llist_head(&ci->cookielist[i]);
    संरचना Curl_llist_गाठ *e = रिक्त;

    साठी(; n; n = e) {
      संरचना Cookie *curr = Curl_गाठ_elem(n);
      e = Curl_गाठ_next(n); /* in case the node is removed, get it early */
      जर(!curr->expires) {
        Curl_गाठ_remove(n);
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
  जर(ci) {
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
स्थिर अक्षर *get_netscape_format(स्थिरांक संरचना Cookie *co)
{
  परत curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*मूल्य*/
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
    co->मूल्य ? co->मूल्य : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
स्थिर CURLcode cookie_output(संरचना Curl_easy *data,
                              संरचना CookieInfo *ci,
                              स्थिरांक अक्षर *filename)
{
  खाता *out = रिक्त;
  bool use_stdout = FALSE;
  अक्षर *tempstore = रिक्त;
  CURLcode result = CURLE_OK;

  जर(!ci)
    /* no cookie engine alive */
    परत CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  जर(!म_भेद("-", filename)) {
    /* use stdout */
    out = मानक_निकास;
    use_stdout = TRUE;
  }
  अन्यथा {
    result = Curl_fopen(data, filename, &out, &tempstore);
    जर(result)
      जा error;
  }

  ख_माला_दो("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  जर(ci->numcookies) {
    अचिन्हित पूर्णांक i;
    माप_प्रकार nvalid = 0;
    संरचना Cookie **array;
    संरचना Curl_llist_गाठ *n;

    array = curlx_calloc(1, आकार(संरचना Cookie *) * ci->numcookies);
    जर(!array) {
      result = CURLE_OUT_OF_MEMORY;
      जा error;
    }

    /* only sort the cookies with a domain property */
    साठी(i = 0; i < COOKIE_HASH_SIZE; i++) {
      साठी(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_गाठ_next(n)) {
        संरचना Cookie *co = Curl_गाठ_elem(n);
        जर(!co->domain)
          सुरूठेवा;
        array[nvalid++] = co;
      }
    }

    क्विक(array, nvalid, आकार(संरचना Cookie *), cookie_sort_ct);

    साठी(i = 0; i < nvalid; i++) {
      अक्षर *format_ptr = get_netscape_format(array[i]);
      जर(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        जा error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  जर(!use_stdout) {
    curlx_fclose(out);
    out = रिक्त;
    जर(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      जा error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  परत CURLE_OK;

error:
  जर(out && !use_stdout)
    curlx_fclose(out);
  जर(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  परत result;
}

स्थिर संरचना curl_slist *cookie_list(स्थिरांक संरचना Curl_easy *data)
{
  संरचना curl_slist *list = रिक्त;
  संरचना curl_slist *beg;
  संरचना curl_slist **tail = &list;
  अचिन्हित पूर्णांक i;
  संरचना Curl_llist_गाठ *n;

  जर(!data->cookies || (data->cookies->numcookies == 0))
    परत रिक्त;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  साठी(i = 0; i < COOKIE_HASH_SIZE; i++) {
    साठी(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_गाठ_next(n)) {
      संरचना Cookie *c = Curl_गाठ_elem(n);
      अक्षर *ओळ;
      जर(!c->domain)
        सुरूठेवा;
      ओळ = get_netscape_format(c);
      जर(!ओळ) {
        curl_slist_free_all(list);
        परत रिक्त;
      }
      beg = Curl_slist_append_nodup(रिक्त, ओळ);
      जर(!beg) {
        curlx_free(ओळ);
        curl_slist_free_all(list);
        परत रिक्त;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  परत list;
}

संरचना curl_slist *Curl_cookie_list(संरचना Curl_easy *data)
{
  संरचना curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  परत list;
}

शून्य Curl_flush_cookies(संरचना Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  जर(data->cookies) {
    स्थिरांक अक्षर *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    जर(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      जर(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    जर(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = रिक्त;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

शून्य Curl_cookie_run(संरचना Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  जर(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
