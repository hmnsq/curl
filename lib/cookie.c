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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#समावेश "urldata.h"
#समावेश "cookie.h"
#समावेश "psl.h"
#समावेश "curl_trc.h"
#समावेश "transfer.h"
#समावेश "slist.h"
#समावेश "curl_share.h"
#समावेश "strcase.h"
#समावेश "curl_fopen.h"
#समावेश "curl_get_line.h"
#समावेश "curl_memrchr.h"
#समावेश "parsedate.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/dynbuf.h"
#समावेश "llist.h"
#समावेश "curlx/strparse.h"

/* number of seconds in 400 days */
#घोषणा COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
अटल व्योम cap_expires(समय_प्रकार now, काष्ठा Cookie *co)
{
  अगर(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    अगर(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

अटल व्योम freecookie(काष्ठा Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->मूल्य);
  अगर(maintoo)
    curlx_free(co);
}

अटल bool cookie_tailmatch(स्थिर अक्षर *cookie_domain,
                             स्थिर माप_प्रकार cookie_domain_len,
                             स्थिर अक्षर *hostname)
{
  माप_प्रकार hostname_len = म_माप(hostname);

  अगर(hostname_len < cookie_domain_len)
    वापस FALSE;

  अगर(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    वापस FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  अगर(hostname_len == cookie_domain_len)
    वापस TRUE;
  अगर('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    वापस TRUE;
  वापस FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
अटल bool pathmatch(स्थिर अक्षर *cookie_path, स्थिर अक्षर *uri_path)
{
  माप_प्रकार cookie_path_len;
  माप_प्रकार uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = म_माप(cookie_path);
  अगर(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    वापस TRUE;
  }

  /* #-fragments are already cut off! */
  अगर(म_माप(uri_path) == 0 || uri_path[0] != '/')
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

  अगर(uri_path_len < cookie_path_len)
    जाओ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  अगर(म_भेदन(cookie_path, uri_path, cookie_path_len))
    जाओ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  अगर(cookie_path_len == uri_path_len) {
    ret = TRUE;
    जाओ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  अगर(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    जाओ pathmatched;
  }

pathmatched:
  वापस ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
अटल स्थिर अक्षर *get_शीर्ष_domain(स्थिर अक्षर * स्थिर domain, माप_प्रकार *outlen)
{
  माप_प्रकार len = 0;
  स्थिर अक्षर *first = शून्य, *last;

  अगर(domain) {
    len = म_माप(domain);
    last = memrchr(domain, '.', len);
    अगर(last) {
      first = memrchr(domain, '.', (last - domain));
      अगर(first)
        len -= (++first - domain);
    }
  }

  अगर(outlen)
    *outlen = len;

  वापस first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#अगर defined(_MSC_VER) && (_MSC_VER == 1900)
#आशय optimize("", off)
#पूर्ण_अगर

/*
 * A case-insensitive hash for the cookie domains.
 */
अटल माप_प्रकार cookie_hash_domain(स्थिर अक्षर *domain, स्थिर माप_प्रकार len)
{
  स्थिर अक्षर *end = domain + len;
  माप_प्रकार h = 5381;

  जबतक(domain < end) {
    माप_प्रकार j = (माप_प्रकार)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  वापस (h % COOKIE_HASH_आकार);
}

#अगर defined(_MSC_VER) && (_MSC_VER == 1900)
#आशय optimize("", on)
#पूर्ण_अगर

/*
 * Hash this domain.
 */
अटल माप_प्रकार cookiehash(स्थिर अक्षर * स्थिर domain)
{
  स्थिर अक्षर *शीर्ष_2;
  माप_प्रकार len;

  अगर(!domain || Curl_host_is_ipnum(domain))
    वापस 0;

  शीर्ष_2 = get_शीर्ष_domain(domain, &len);
  वापस cookie_hash_domain(शीर्ष_2, len);
}

/*
 * cookie path sanitize
 */
अटल अक्षर *sanitize_cookie_path(स्थिर अक्षर *cookie_path, माप_प्रकार len)
{
  /* some sites send path attribute within '"'. */
  अगर(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    अगर(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  अगर(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    वापस curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  अगर(len > 1 && cookie_path[len - 1] == '/')
    len--;

  वापस curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
अटल CURLcode strstore(अक्षर **str, स्थिर अक्षर *newstr, माप_प्रकार len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  अगर(!*str)
    वापस CURLE_OUT_OF_MEMORY;
  वापस CURLE_OK;
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
अटल व्योम remove_expired(काष्ठा CookieInfo *ci)
{
  काष्ठा Cookie *co;
  curl_off_t now = (curl_off_t)समय(शून्य);
  अचिन्हित पूर्णांक i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  अगर(now < ci->अग्र_expiration &&
     ci->अग्र_expiration != CURL_OFF_T_MAX)
    वापस;
  अन्यथा
    ci->अग्र_expiration = CURL_OFF_T_MAX;

  क्रम(i = 0; i < COOKIE_HASH_आकार; i++) {
    काष्ठा Curl_llist_गाँठ *n;
    काष्ठा Curl_llist_गाँठ *e = शून्य;

    क्रम(n = Curl_llist_शीर्ष(&ci->cookielist[i]); n; n = e) {
      co = Curl_गाँठ_elem(n);
      e = Curl_गाँठ_अग्र(n);
      अगर(co->expires) {
        अगर(co->expires < now) {
          Curl_गाँठ_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        अन्यथा अगर(co->expires < ci->अग्र_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->अग्र_expiration = co->expires;
      }
    }
  }
}

#अगर_अघोषित USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
अटल bool bad_domain(स्थिर अक्षर *domain, माप_प्रकार len)
{
  अगर((len == 9) && curl_strnequal(domain, "localhost", 9))
    वापस FALSE;
  अन्यथा {
    /* there must be a dot present, but that dot must not be a trailing dot */
    स्थिर अक्षर *dot = स_प्रथम(domain, '.', len);
    अगर(dot) {
      माप_प्रकार i = dot - domain;
      अगर((len - i) > 1)
        /* the dot is not the last byte */
        वापस FALSE;
    }
  }
  वापस TRUE;
}
#पूर्ण_अगर

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
अटल bool invalid_octets(स्थिर अक्षर *ptr, माप_प्रकार len)
{
  स्थिर अचिन्हित अक्षर *p = (स्थिर अचिन्हित अक्षर *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  जबतक(len && *p) {
    अगर((*p < 0x20) || (*p == 0x7f))
      वापस TRUE;
    p++;
    len--;
  }
  वापस FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#घोषणा MAX_DATE_लंबाई 80

#घोषणा COOKIE_NAME   0
#घोषणा COOKIE_मूल्य  1
#घोषणा COOKIE_DOMAIN 2
#घोषणा COOKIE_PATH   3

#घोषणा COOKIE_PIECES 4 /* the list above */

अटल CURLcode storecookie(काष्ठा Cookie *co, स्थिर काष्ठा Curl_str *cp,
                            स्थिर अक्षर *path, स्थिर अक्षर *domain)
{
  CURLcode परिणाम;
  परिणाम = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  अगर(!परिणाम)
    परिणाम = strstore(&co->मूल्य, curlx_str(&cp[COOKIE_मूल्य]),
                      curlx_strlen(&cp[COOKIE_मूल्य]));
  अगर(!परिणाम) {
    माप_प्रकार plen = 0;
    अगर(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    अन्यथा अगर(path) {
      /* No path was given in the header line, set the default */
      स्थिर अक्षर *endslash = म_खोजप(path, '/');
      अगर(endslash)
        plen = endslash - path + 1; /* include end slash */
      अन्यथा
        plen = म_माप(path);
    }

    अगर(path) {
      co->path = sanitize_cookie_path(path, plen);
      अगर(!co->path)
        परिणाम = CURLE_OUT_OF_MEMORY;
    }
  }
  अगर(!परिणाम) {
    अगर(curlx_strlen(&cp[COOKIE_DOMAIN]))
      परिणाम = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    अन्यथा अगर(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      अगर(!co->domain)
        परिणाम = CURLE_OUT_OF_MEMORY;
    }
  }
  वापस परिणाम;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
अटल bool parse_first_pair(काष्ठा Curl_easy *तथ्य, काष्ठा Cookie *co,
                             काष्ठा Curl_str *cookie,
                             काष्ठा Curl_str *name,
                             काष्ठा Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  अगर(!sep || !curlx_strlen(name)) {
    infof(तथ्य, "invalid cookie, dropped");
    वापस FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  अगर((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(तथ्य, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    वापस FALSE;
  }

  /* Check if we have a reserved prefix set. */
  अगर(!म_भेदन("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  अन्यथा अगर(!म_भेदन("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_मूल्य] = *val;
  वापस TRUE;
}

अटल bool parse_domain(काष्ठा Curl_easy *तथ्य, काष्ठा Cookie *co,
                         काष्ठा Curl_str *cookie_domain,
                         काष्ठा Curl_str *val,
                         स्थिर अक्षर **domainp)
{
  bool is_ip;
  स्थिर अक्षर *domain = *domainp;
  स्थिर अक्षर *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  अगर('.' == *v)
    curlx_str_nudge(val, 1);

#अगर_अघोषित USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  अगर(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#पूर्ण_अगर

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  अगर(!domain ||
     (is_ip &&
      !म_भेदन(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == म_माप(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    अगर(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  अन्यथा {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(तथ्य, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    वापस FALSE;
  }
  वापस TRUE;
}

अटल व्योम parse_maxage(काष्ठा Cookie *co, काष्ठा Curl_str *val,
                         समय_प्रकार *nowp)
{
  पूर्णांक rc;
  स्थिर अक्षर *maxage = curlx_str(val);
  अगर(*maxage == '\"')
    maxage++;
  rc = curlx_str_संख्या(&maxage, &co->expires, CURL_OFF_T_MAX);
  अगर(!*nowp)
    *nowp = समय(शून्य);
  चयन(rc) {
  हाल STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    अवरोध;
  शेष:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    अवरोध;
  हाल STRE_OK:
    अगर(!co->expires)
      co->expires = 1; /* expire now */
    अन्यथा अगर(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    अन्यथा
      co->expires += *nowp;
    अवरोध;
  }
  cap_expires(*nowp, co);
}

अटल व्योम parse_expires(काष्ठा Cookie *co, काष्ठा Curl_str *val,
                          समय_प्रकार *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  अगर(!co->expires && (curlx_strlen(val) < MAX_DATE_लंबाई)) {
    अक्षर dbuf[MAX_DATE_लंबाई + 1];
    समय_प्रकार date = 0;
    स_नकल(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    अगर(!Curl_getdate_capped(dbuf, &date)) {
      अगर(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    अन्यथा
      co->expires = 0;
    अगर(!*nowp)
      *nowp = समय(शून्य);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
अटल CURLcode
parse_cookie_header(काष्ठा Curl_easy *तथ्य,
                    काष्ठा Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    स्थिर अक्षर *ptr, /* the header */
                    स्थिर अक्षर *domain, /* default domain */
                    /* full path used when this cookie is set */
                    स्थिर अक्षर *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  समय_प्रकार now = 0;
  माप_प्रकार linelength = म_माप(ptr);
  CURLcode परिणाम = CURLE_OK;
  काष्ठा Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  अगर(linelength > MAX_COOKIE_पंक्ति)
    /* discard overly long lines at once */
    वापस CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  स_रखो(cookie, 0, माप(cookie));
  करो {
    काष्ठा Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    पूर्णांक rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    अगर(!rc || (rc == STRE_SHORT)) {
      काष्ठा Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      अगर(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(तथ्य, "invalid octets in name, cookie dropped");
        वापस CURLE_OK;
      }

      अगर(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        अगर(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        अगर(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(तथ्य, "invalid octets in value, cookie dropped");
          वापस CURLE_OK;
        }
      }
      अन्यथा
        curlx_str_init(&val);

      अगर(!curlx_strlen(&cookie[COOKIE_NAME])) {
        अगर(!parse_first_pair(तथ्य, co, cookie, &name, &val, sep))
          वापस CURLE_OK;
      }

      /* whether there's separator or not */
      अन्यथा अगर(curlx_str_casecompare(&name, "secure")) {
        अगर(secure_origin)
          co->secure = TRUE;
        अन्यथा {
          infof(तथ्य, "skipped cookie because not 'secure'");
          वापस CURLE_OK;
        }
      }
      अन्यथा अगर(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      अन्यथा अगर(sep) {
        /* only if a separating equals sign is present */
        अगर(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        अन्यथा अगर(curlx_str_casecompare(&name, "domain")) {
          अगर(curlx_strlen(&val) &&
             !parse_domain(तथ्य, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            वापस CURLE_OK;
        }
        अन्यथा अगर(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        अन्यथा अगर(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } जबतक(!curlx_str_single(&ptr, ';'));

  अगर(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    परिणाम = storecookie(co, &cookie[0], path, domain);
    अगर(!परिणाम)
      *okay = TRUE;
  }
  वापस परिणाम;
}

अटल CURLcode parse_netscape(काष्ठा Cookie *co,
                               स्थिर काष्ठा CookieInfo *ci,
                               bool *okay,
                               स्थिर अक्षर *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  स्थिर अक्षर *ptr, *अग्र;
  पूर्णांक fields;
  माप_प्रकार len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  अगर(!म_भेदन(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  अगर(lineptr[0] == '#')
    /* do not even try the comments */
    वापस CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  क्रम(अग्र = lineptr; अग्र; fields++) {
    ptr = अग्र;
    len = म_खोज(ptr, "\t\r\n");
    अग्र = (ptr[len] == '\t' ? &ptr[len + 1] : शून्य);
    चयन(fields) {
    हाल 0:
      अगर(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      अगर(!co->domain)
        वापस CURLE_OUT_OF_MEMORY;
      अवरोध;
    हाल 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      अवरोध;
    हाल 2:
      /* The file format allows the path field to remain not filled in */
      अगर(म_भेदन("TRUE", ptr, len) && म_भेदन("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        अगर(!co->path)
          वापस CURLE_OUT_OF_MEMORY;
        अवरोध;
      }
      अन्यथा {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        अगर(!co->path)
          वापस CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    हाल 3:
      co->secure = FALSE;
      अगर(curl_strnequal(ptr, "TRUE", len)) {
        अगर(secure_origin || ci->running)
          co->secure = TRUE;
        अन्यथा
          वापस CURLE_OK;
      }
      अवरोध;
    हाल 4:
      अगर(curlx_str_संख्या(&ptr, &co->expires, CURL_OFF_T_MAX))
        वापस CURLE_OK;
      अवरोध;
    हाल 5:
      co->name = curlx_memdup0(ptr, len);
      अगर(!co->name)
        वापस CURLE_OUT_OF_MEMORY;
      अन्यथा {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        अगर(!म_भेदन("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        अन्यथा अगर(!म_भेदन("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      अवरोध;
    हाल 6:
      co->मूल्य = curlx_memdup0(ptr, len);
      अगर(!co->मूल्य)
        वापस CURLE_OUT_OF_MEMORY;
      अवरोध;
    }
  }
  अगर(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->मूल्य = curlx_strdup("");
    अगर(!co->मूल्य)
      वापस CURLE_OUT_OF_MEMORY;
    अन्यथा
      fields++;
  }

  अगर(fields != 7)
    /* we did not find the sufficient number of fields */
    वापस CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  अगर(invalid_octets(co->name, म_माप(co->name)) ||
     invalid_octets(co->मूल्य, म_माप(co->मूल्य)))
    वापस CURLE_OK;

  *okay = TRUE;
  वापस CURLE_OK;
}

अटल bool is_public_suffix(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Cookie *co,
                             स्थिर अक्षर *domain)
{
#अगर_घोषित USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(co);
  DEBUGF(infof(तथ्य, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  अगर(!co->domain || Curl_host_is_ipnum(co->domain))
    वापस FALSE;

  अन्यथा {
    bool acceptable = FALSE;
    अक्षर lcase[256];
    अक्षर lcookie[256];
    माप_प्रकार dlen = domain ? म_माप(domain) : 0;
    माप_प्रकार clen = म_माप(co->domain);

    /* trim trailing dots */
    अगर(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    अगर(clen && (co->domain[clen - 1] == '.'))
      clen--;

    अगर((dlen < माप(lcase)) && (clen < माप(lcookie))) {
      स्थिर psl_ctx_t *psl = Curl_psl_use(तथ्य);
      अगर(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        अगर(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          अगर(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        अन्यथा {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          अगर(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(तथ्य);
      }
      अन्यथा
        infof(तथ्य, "libpsl problem, rejecting cookie for safety");
    }

    अगर(!acceptable) {
      infof(तथ्य, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      वापस TRUE;
    }
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)co;
  (व्योम)domain;
  DEBUGF(infof(तथ्य, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#पूर्ण_अगर
  वापस FALSE;
}

/* returns TRUE when replaced */
अटल bool replace_existing(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Cookie *co,
                             स्थिर काष्ठा CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  काष्ठा Curl_llist_गाँठ *replace_n = शून्य;
  काष्ठा Curl_llist_गाँठ *n;
  माप_प्रकार myhash = cookiehash(co->domain);
  क्रम(n = Curl_llist_शीर्ष(&ci->cookielist[myhash]); n; n = Curl_गाँठ_अग्र(n)) {
    काष्ठा Cookie *clist = Curl_गाँठ_elem(n);
    अगर(!म_भेद(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      अगर(clist->domain && co->domain) {
        अगर(cookie_tailmatch(clist->domain, म_माप(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, म_माप(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      अन्यथा अगर(!clist->domain && !co->domain)
        matching_domains = TRUE;

      अगर(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        माप_प्रकार cllen;
        स्थिर अक्षर *sep = शून्य;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        अगर(clist->path[0])
          sep = म_अक्षर(clist->path + 1, '/');
        अगर(sep)
          cllen = sep - clist->path;
        अन्यथा
          cllen = म_माप(clist->path);

        अगर(!म_भेदन(clist->path, co->path, cllen)) {
          infof(तथ्य, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          वापस FALSE;
        }
      }
    }

    अगर(!replace_n && !म_भेद(clist->name, co->name)) {
      /* the names are identical */

      अगर(clist->domain && co->domain) {
        अगर(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      अन्यथा अगर(!clist->domain && !co->domain)
        replace_old = TRUE;

      अगर(replace_old) {
        /* the domains were identical */

        अगर(clist->path && co->path &&
           म_भेद(clist->path, co->path))
          replace_old = FALSE;
        अन्यथा अगर(!clist->path != !co->path)
          replace_old = FALSE;
      }

      अगर(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        वापस FALSE;
      }
      अगर(replace_old)
        replace_n = n;
    }
  }
  अगर(replace_n) {
    काष्ठा Cookie *repl = Curl_गाँठ_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_गाँठ_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  वापस TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(काष्ठा Curl_easy *तथ्य,
                         काष्ठा CookieInfo *ci,
                         स्थिर अक्षर *lineptr, /* first character of the line */
                         स्थिर अक्षर *domain,  /* default domain */
                         स्थिर अक्षर *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         स्थिर पूर्णांक flags)
{
  काष्ठा Cookie comem;
  काष्ठा Cookie *co;
  माप_प्रकार myhash;
  CURLcode परिणाम;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  अगर(तथ्य->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    वापस CURLE_OK; /* silently ignore */

  co = &comem;
  स_रखो(co, 0, माप(comem));

  अगर(flags & COOKIE_HTTPHEADER)
    परिणाम = parse_cookie_header(तथ्य, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  अन्यथा
    परिणाम = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  अगर(परिणाम || !okay)
    जाओ fail;

  अगर(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    जाओ fail;

  अगर(!(flags & COOKIE_NOPSL) && is_public_suffix(तथ्य, co, domain))
    जाओ fail;

  अगर(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    अगर(co->secure && co->path && !म_भेद(co->path, "/") && !co->tailmatch)
      ;
    अन्यथा
      जाओ fail;
  }

  अगर(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    जाओ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  अगर(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  अगर(!replace_existing(तथ्य, co, ci, flags & COOKIE_SECURE, &replaces))
    जाओ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, माप(comem));
  अगर(!co) {
    co = &comem;
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->गाँठ);

  अगर(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(तथ्य, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->मूल्य,
          co->domain, co->path, co->expires);

  अगर(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  अगर(co->expires && (co->expires < ci->अग्र_expiration))
    ci->अग्र_expiration = co->expires;

  अगर(flags & COOKIE_HTTPHEADER)
    तथ्य->req.setcookies++;

  वापस परिणाम;
fail:
  freecookie(co, FALSE);
  वापस परिणाम;
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
काष्ठा CookieInfo *Curl_cookie_init(व्योम)
{
  पूर्णांक i;
  काष्ठा CookieInfo *ci = curlx_calloc(1, माप(काष्ठा CookieInfo));
  अगर(!ci)
    वापस शून्य;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  क्रम(i = 0; i < COOKIE_HASH_आकार; i++)
    Curl_llist_init(&ci->cookielist[i], शून्य);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->अग्र_expiration = CURL_OFF_T_MAX;

  वापस ci;
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
अटल CURLcode cookie_load(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *file,
                            काष्ठा CookieInfo *ci, पूर्णांक flags)
{
  खाता *handle = शून्य;
  CURLcode परिणाम = CURLE_OK;
  खाता *fp = शून्य;
  DEBUGASSERT(ci);
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  अगर(file && *file) {
    अगर(!म_भेद(file, "-"))
      fp = मानक_निवेश;
    अन्यथा {
      fp = curlx_fopen(file, "rb");
      अगर(!fp)
        infof(तथ्य, "WARNING: failed to open cookie file \"%s\"", file);
      अन्यथा {
        curlx_struct_stat stat;
        अगर((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = शून्य;
          infof(तथ्य, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        अन्यथा
          handle = fp;
      }
    }
  }

  अगर(fp) {
    काष्ठा dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    करो {
      परिणाम = Curl_get_पंक्ति(&buf, fp, &eof);
      अगर(!परिणाम) {
        स्थिर अक्षर *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        अगर(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        परिणाम = Curl_cookie_add(तथ्य, ci, lineptr, शून्य, शून्य,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        अगर(परिणाम != CURLE_OUT_OF_MEMORY)
          परिणाम = CURLE_OK;
      }
    } जबतक(!परिणाम && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    अगर(handle)
      curlx_fclose(handle);
  }
  तथ्य->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  वापस परिणाम;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(काष्ठा Curl_easy *तथ्य,
                               पूर्णांक flags)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा curl_slist *list = तथ्य->state.cookielist;
  अगर(list) {
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    अगर(!तथ्य->cookies)
      तथ्य->cookies = Curl_cookie_init();
    अगर(!तथ्य->cookies)
      परिणाम = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      तथ्य->state.cookie_engine = TRUE;
      जबतक(list) {
        परिणाम = cookie_load(तथ्य, list->तथ्य, तथ्य->cookies, flags);
        अगर(परिणाम)
          अवरोध;
        list = list->अग्र;
      }
    }
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
  }
  वापस परिणाम;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
अटल पूर्णांक cookie_sort(स्थिर व्योम *p1, स्थिर व्योम *p2)
{
  स्थिर काष्ठा Cookie *c1 = *(स्थिर काष्ठा Cookie * स्थिर *)p1;
  स्थिर काष्ठा Cookie *c2 = *(स्थिर काष्ठा Cookie * स्थिर *)p2;
  माप_प्रकार l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? म_माप(c1->path) : 0;
  l2 = c2->path ? म_माप(c2->path) : 0;

  अगर(l1 != l2)
    वापस (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? म_माप(c1->domain) : 0;
  l2 = c2->domain ? म_माप(c2->domain) : 0;

  अगर(l1 != l2)
    वापस (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? म_माप(c1->name) : 0;
  l2 = c2->name ? म_माप(c2->name) : 0;

  अगर(l1 != l2)
    वापस (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  वापस (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
अटल पूर्णांक cookie_sort_ct(स्थिर व्योम *p1, स्थिर व्योम *p2)
{
  स्थिर काष्ठा Cookie *c1 = *(स्थिर काष्ठा Cookie * स्थिर *)p1;
  स्थिर काष्ठा Cookie *c2 = *(स्थिर काष्ठा Cookie * स्थिर *)p2;

  वापस (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *host)
{
  वापस Curl_xfer_is_secure(तथ्य) ||
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
CURLcode Curl_cookie_getlist(काष्ठा Curl_easy *तथ्य,
                             bool *okay,
                             स्थिर अक्षर *host,
                             काष्ठा Curl_llist *list)
{
  माप_प्रकार matches = 0;
  स्थिर bool is_ip = Curl_host_is_ipnum(host);
  स्थिर माप_प्रकार myhash = cookiehash(host);
  काष्ठा Curl_llist_गाँठ *n;
  स्थिर bool secure = Curl_secure_context(तथ्य, host);
  काष्ठा CookieInfo *ci = तथ्य->cookies;
  स्थिर अक्षर *path = तथ्य->state.up.path;
  CURLcode परिणाम = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, शून्य);

  अगर(!ci || !Curl_llist_गिनती(&ci->cookielist[myhash]))
    वापस CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  क्रम(n = Curl_llist_शीर्ष(&ci->cookielist[myhash]); n; n = Curl_गाँठ_अग्र(n)) {
    काष्ठा Cookie *co = Curl_गाँठ_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    अगर(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      अगर(!co->domain ||
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
        अगर(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          अगर(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(तथ्य, "Included max number of cookies (%zu) in request!",
                  matches);
            अवरोध;
          }
        }
      }
    }
  }

  अगर(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    काष्ठा Cookie **array;
    माप_प्रकार i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(माप(काष्ठा Cookie *) * matches);
    अगर(!array) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ fail;
    }

    n = Curl_llist_शीर्ष(list);

    क्रम(i = 0; n; n = Curl_गाँठ_अग्र(n))
      array[i++] = Curl_गाँठ_elem(n);

    /* now sort the cookie pointers in path length order */
    क्विक(array, matches, माप(काष्ठा Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, शून्य);

    क्रम(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  वापस CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, शून्य);
  वापस परिणाम; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
व्योम Curl_cookie_clearall(काष्ठा CookieInfo *ci)
{
  अगर(ci) {
    अचिन्हित पूर्णांक i;
    क्रम(i = 0; i < COOKIE_HASH_आकार; i++) {
      काष्ठा Curl_llist_गाँठ *n;
      क्रम(n = Curl_llist_शीर्ष(&ci->cookielist[i]); n;) {
        काष्ठा Cookie *c = Curl_गाँठ_elem(n);
        काष्ठा Curl_llist_गाँठ *e = Curl_गाँठ_अग्र(n);
        Curl_गाँठ_remove(n);
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
व्योम Curl_cookie_clearsess(काष्ठा CookieInfo *ci)
{
  अचिन्हित पूर्णांक i;

  अगर(!ci)
    वापस;

  क्रम(i = 0; i < COOKIE_HASH_आकार; i++) {
    काष्ठा Curl_llist_गाँठ *n = Curl_llist_शीर्ष(&ci->cookielist[i]);
    काष्ठा Curl_llist_गाँठ *e = शून्य;

    क्रम(; n; n = e) {
      काष्ठा Cookie *curr = Curl_गाँठ_elem(n);
      e = Curl_गाँठ_अग्र(n); /* in case the node is removed, get it early */
      अगर(!curr->expires) {
        Curl_गाँठ_remove(n);
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
व्योम Curl_cookie_cleanup(काष्ठा CookieInfo *ci)
{
  अगर(ci) {
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
अटल अक्षर *get_netscape_format(स्थिर काष्ठा Cookie *co)
{
  वापस curl_maprintf(
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
अटल CURLcode cookie_output(काष्ठा Curl_easy *तथ्य,
                              काष्ठा CookieInfo *ci,
                              स्थिर अक्षर *filename)
{
  खाता *out = शून्य;
  bool use_stdout = FALSE;
  अक्षर *tempstore = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(!ci)
    /* no cookie engine alive */
    वापस CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  अगर(!म_भेद("-", filename)) {
    /* use stdout */
    out = मानक_निकास;
    use_stdout = TRUE;
  }
  अन्यथा {
    परिणाम = Curl_fopen(तथ्य, filename, &out, &tempstore);
    अगर(परिणाम)
      जाओ error;
  }

  ख_माला_दो("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  अगर(ci->numcookies) {
    अचिन्हित पूर्णांक i;
    माप_प्रकार nvalid = 0;
    काष्ठा Cookie **array;
    काष्ठा Curl_llist_गाँठ *n;

    array = curlx_calloc(1, माप(काष्ठा Cookie *) * ci->numcookies);
    अगर(!array) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ error;
    }

    /* only sort the cookies with a domain property */
    क्रम(i = 0; i < COOKIE_HASH_आकार; i++) {
      क्रम(n = Curl_llist_शीर्ष(&ci->cookielist[i]); n; n = Curl_गाँठ_अग्र(n)) {
        काष्ठा Cookie *co = Curl_गाँठ_elem(n);
        अगर(!co->domain)
          जारी;
        array[nvalid++] = co;
      }
    }

    क्विक(array, nvalid, माप(काष्ठा Cookie *), cookie_sort_ct);

    क्रम(i = 0; i < nvalid; i++) {
      अक्षर *format_ptr = get_netscape_format(array[i]);
      अगर(!format_ptr) {
        curlx_free(array);
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  अगर(!use_stdout) {
    curlx_fclose(out);
    out = शून्य;
    अगर(tempstore && curlx_rename(tempstore, filename)) {
      परिणाम = CURLE_WRITE_ERROR;
      जाओ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  वापस CURLE_OK;

error:
  अगर(out && !use_stdout)
    curlx_fclose(out);
  अगर(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  वापस परिणाम;
}

अटल काष्ठा curl_slist *cookie_list(स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curl_slist *list = शून्य;
  काष्ठा curl_slist *beg;
  काष्ठा curl_slist **पुच्छ = &list;
  अचिन्हित पूर्णांक i;
  काष्ठा Curl_llist_गाँठ *n;

  अगर(!तथ्य->cookies || (तथ्य->cookies->numcookies == 0))
    वापस शून्य;

  /* at first, remove expired cookies */
  remove_expired(तथ्य->cookies);

  क्रम(i = 0; i < COOKIE_HASH_आकार; i++) {
    क्रम(n = Curl_llist_शीर्ष(&तथ्य->cookies->cookielist[i]); n;
        n = Curl_गाँठ_अग्र(n)) {
      काष्ठा Cookie *c = Curl_गाँठ_elem(n);
      अक्षर *पंक्ति;
      अगर(!c->domain)
        जारी;
      पंक्ति = get_netscape_format(c);
      अगर(!पंक्ति) {
        curl_slist_free_all(list);
        वापस शून्य;
      }
      beg = Curl_slist_append_nodup(शून्य, पंक्ति);
      अगर(!beg) {
        curlx_free(पंक्ति);
        curl_slist_free_all(list);
        वापस शून्य;
      }
      *पुच्छ = beg;
      पुच्छ = &beg->अग्र;
    }
  }

  वापस list;
}

काष्ठा curl_slist *Curl_cookie_list(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curl_slist *list;
  Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(तथ्य);
  Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
  वापस list;
}

व्योम Curl_flush_cookies(काष्ठा Curl_easy *तथ्य, bool cleanup)
{
  Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  अगर(तथ्य->cookies) {
    स्थिर अक्षर *cookiejar = CURL_EASY_STR(तथ्य, STRING_COOKIEJAR);
    अगर(cookiejar && तथ्य->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode परिणाम = cookie_output(तथ्य, तथ्य->cookies, cookiejar);
      अगर(परिणाम)
        infof(तथ्य, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(परिणाम));
    }

    अगर(cleanup && (!तथ्य->share || (तथ्य->cookies != तथ्य->share->cookies))) {
      Curl_cookie_cleanup(तथ्य->cookies);
      तथ्य->cookies = शून्य;
    }
  }
  Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
}

व्योम Curl_cookie_run(काष्ठा Curl_easy *तथ्य)
{
  Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  अगर(तथ्य->cookies)
    तथ्य->cookies->running = TRUE;
  Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
}

#पूर्ण_अगर /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
