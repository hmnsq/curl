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

#ضمن "urldata.h"
#ضمن "urlapi-int.h"
#ضمن "strcase.h"
#ضمن "url.h"
#ضمن "escape.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/strdup.h"
#ضمن "idn.h"
#ضمن "curlx/strparse.h"
#ضمن "curl_memrchr.h"

#که_تعریف_شوی _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#تعریف_کړه STARTS_WITH_DRIVE_مختاړی(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#د_که_پای

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#تعریف_کړه STARTS_WITH_URL_DRIVE_مختاړی(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#تعریف_کړه MAX_SCHEME_LEN 40
#تعریف_کړه MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#إذا !defined(USE_IPV6) && !defined(AF_INET6)
#تعریف_کړه AF_INET6 (AF_INET + 1)
#د_که_پای

#تعریف_کړه DEFAULT_SCHEME "https"

ساكن فراغ free_urlhandle(بنية Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->لار);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
ساكن ثابت حرف *پیدا_کړه_host_sep(ثابت حرف *url)
{
  /* Find the start of the hostname */
  ثابت حرف *sep = مزي_پلټنه(url, "//");
  إذا(!sep)
    sep = url;
  وإلا
    sep += 2;

  /* Find first / or ? */
  طالما(*sep && *sep != '/' && *sep != '?')
    sep++;

  أرجع sep;
}

/* convert CURLcode to CURLUcode */
#تعریف_کړه cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_بهر_د_حافظه)

/* urlencode_str() writes data into an output dynbuf and URL-encodes the
 * spaces in the source URL accordingly.
 *
 * This function re-encodes the string, meaning that it leaves already encoded
 * bytes as-is and works by encoding only what *has* to be encoded - unless it
 * has to uppercase the hex to normalize.
 *
 * Illegal percent-encoding sequences are left as-is.
 *
 * URL encoding should be skipped for hostnames, otherwise IDN resolution
 * will fail.
 *
 * 'query' tells if it is a query part or not, or if it is allowed to
 * "transition" into a query part with a question mark.
 *
 * @unittest 1675
 */
UNITTEST CURLUcode urlencode_str(بنية dynbuf *o, ثابت حرف *url,
                                 size_t len, bool relative,
                                 بینښه صحيح query);
UNITTEST CURLUcode urlencode_str(بنية dynbuf *o, ثابت حرف *url,
                                 size_t len, bool relative,
                                 بینښه صحيح query)
{
  /* we must add this with whitespace-replacing */
  ثابت بینښه حرف *iptr;
  ثابت بینښه حرف *host_sep = (ثابت بینښه حرف *)url;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  إذا(!relative) {
    size_t n;
    host_sep = (ثابت بینښه حرف *)پیدا_کړه_host_sep(url);

    /* output the first piece as-is */
    n = (ثابت حرف *)host_sep - url;
    پایله = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  لكل(iptr = host_sep; len && !پایله;) {
    إذا(*iptr == ' ') {
      إذا(query != QUERY_YES)
        پایله = curlx_dyn_addn(o, "%20", 3);
      وإلا
        پایله = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    وإلا إذا((*iptr < ' ') || (*iptr >= 0x7f)) {
      بینښه حرف بهر[3] = { '%' };
      Curl_hexbyte(&بهر[1], *iptr);
      پایله = curlx_dyn_addn(o, بهر, 3);
      iptr++;
      len--;
    }
    وإلا إذا(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      بینښه حرف hex = (بینښه حرف)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      بینښه حرف بهر[3] = { '%' };
      Curl_hexbyte(&بهر[1], hex);
      پایله = curlx_dyn_addn(o, بهر, 3);
      iptr += 3;
      len -= 3;
    }
    وإلا {
      ثابت بینښه حرف *پیل = iptr;
      طالما(len) {
        إذا(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          اكسر;
        إذا(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          اكسر;
        إذا(*iptr == '?') {
          إذا(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            اكسر;
          }
        }
        iptr++;
        len--;
      }
      پایله = curlx_dyn_addn(o, (ثابت حرف *)پیل, (size_t)(iptr - پیل));
    }
  }

  إذا(پایله)
    أرجع cc2cu(پایله);
  أرجع CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ثابت حرف *url, حرف *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (فراغ)buflen; /* only used in debug-builds */
  إذا(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#که_تعریف_شوی _WIN32
  إذا(guess_scheme && STARTS_WITH_DRIVE_مختاړی(url))
    أرجع 0;
#د_که_پای
  إذا(ISALPHA(url[0])) {
    إذا(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    لكل(i = 1; i < MAX_SCHEME_LEN; ++i) {
      حرف s = url[i];
      إذا(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        إذا(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      وإلا {
        اكسر;
      }
    }
  }
  إذا(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    إذا(buf)
      buf[i] = 0;
    أرجع len;
  }
  إذا(buf)
    buf[0] = 0;
  أرجع 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ساكن bool badoctets(ثابت حرف *ننوت, size_t n, صحيح flags)
{
  ثابت uint8_t *p = (ثابت بینښه حرف *)ننوت;
  ثابت uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  طالما(n--) {
    إذا(*p <= control || *p == 127)
      أرجع TRUE;
    p++;
  }
  أرجع FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(بنية Curl_URL *u,
                                        ثابت حرف *login,
                                        size_t len,
                                        بینښه صحيح flags,
                                        size_t *hostname_دننه_واټن);
UNITTEST CURLUcode parse_hostname_login(بنية Curl_URL *u,
                                        ثابت حرف *login,
                                        size_t len,
                                        بینښه صحيح flags,
                                        size_t *hostname_دننه_واټن)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode پایله;
  حرف *userp = عدم;
  حرف *passwdp = عدم;
  حرف *optionsp = عدم;
  ثابت بنية Curl_scheme *h = عدم;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ثابت حرف *ptr;

  DEBUGASSERT(login);

  *hostname_دننه_واټن = 0;
  ptr = memchr(login, '@', len);
  إذا(!ptr)
    اذهب بهر;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  إذا(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  پایله = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : عدم);
  إذا(پایله) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_بهر_د_حافظه;
    اذهب بهر;
  }

  إذا(userp) {
    إذا(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      اذهب بهر;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  إذا(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  إذا(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  إذا(userp && badoctets(userp, مزي_اوږدوالی(userp), flags))
    ures = CURLUE_BAD_USER;
  وإلا إذا(passwdp && badoctets(passwdp, مزي_اوږدوالی(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  وإلا إذا(optionsp && badoctets(optionsp, مزي_اوږدوالی(optionsp), flags))
    ures = CURLUE_MALFORMED_ننوت;

  userp = passwdp = optionsp = عدم;

  إذا(!ures) {
    /* the hostname starts at this offset */
    *hostname_دننه_واټن = ptr - login;
    أرجع CURLUE_OK;
  }

بهر:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  أرجع ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_پورټ(بنية Curl_URL *u, بنية dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_پورټ(بنية Curl_URL *u, بنية dynbuf *host,
                              bool has_scheme)
{
  ثابت حرف *portptr;
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->پورټ_present = FALSE;
  إذا(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    إذا(!portptr)
      أرجع CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    إذا(*portptr) {
      إذا(*portptr != ':')
        أرجع CURLUE_BAD_پورټ_شمېره;
    }
    وإلا
      portptr = عدم;
  }
  وإلا
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  إذا(portptr) {
    curl_off_t پورټ;
    size_t keep = portptr - hostname;
    صحيح rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    إذا(!*portptr)
      أرجع has_scheme ? CURLUE_OK : CURLUE_BAD_پورټ_شمېره;
    إذا(*portptr == '\\')
      أرجع CURLUE_BACKSLASH;
    rc = curlx_str_شمېره(&portptr, &پورټ, 0xffff);
    إذا(rc)
      أرجع CURLUE_BAD_پورټ_شمېره;
    وإلا إذا(*portptr == '\\')
      أرجع CURLUE_BACKSLASH;
    وإلا إذا(*portptr)
      أرجع CURLUE_BAD_پورټ_شمېره;

    u->portnum = (uint16_t)پورټ;
    u->پورټ_present = TRUE;
  }

  أرجع CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ساكن ثابت bool invalid_host_char[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00-0x0F */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x10-0x1F */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, /* 0x20-0x2F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, /* 0x30-0x3F */
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x40-0x4F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, /* 0x50-0x5F */
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x60-0x6F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1  /* 0x70-0x7F */
};

/* This function assumes 'hostname' now starts with [. It trims 'hostname' in
 * place and it sets u->zoneid if present.
 *
 * @unittest 1675
 */
UNITTEST CURLUcode ipv6_parse(بنية Curl_URL *u, حرف *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(بنية Curl_URL *u, حرف *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  إذا(hlen < 4) /* '[::]' is the shortest possible valid string */
    أرجع CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  إذا(hlen != len) {
    hlen = len;
    إذا(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      حرف zoneid[MAX_ZONEID_LEN];
      صحيح i = 0;
      حرف *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      إذا(!مزي_پرتله(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      طالما(!invalid_host_char[(بینښه حرف)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      إذا(!i || (']' != *h))
        أرجع CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      إذا(!u->zoneid)
        أرجع CURLUE_بهر_د_حافظه;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    وإلا
      أرجع CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    حرف dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    إذا(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      أرجع CURLUE_BAD_IPV6;
    إذا(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = مزي_اوږدوالی(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  أرجع CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ساكن CURLUcode hostname_check(حرف *hostname, size_t hlen)
{
  size_t i;
  لكل(i = 0; i < hlen; i++) {
    إذا(invalid_host_char[(بینښه حرف)hostname[i]])
      أرجع CURLUE_BAD_HOSTNAME;
  }
  إذا((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    أرجع CURLUE_BAD_HOSTNAME;
  وإلا إذا((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    أرجع CURLUE_BAD_HOSTNAME;
  أرجع CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ساكن CURLUcode hostname_check6(بنية Curl_URL *u, حرف *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  إذا(!hlen)
    أرجع CURLUE_NO_HOST;
  وإلا إذا(hostname[0] == '[')
    أرجع ipv6_parse(u, hostname, hlen);

  أرجع hostname_check(hostname, hlen);
}

/*
 * Handle partial IPv4 numerical addresses and different bases, like
 * '16843009', '0x7f', '0x7f.1' '0177.1.1.1' etc.
 *
 * If the given input string is syntactically wrong IPv4 or any part for
 * example is too big, this function returns HOST_NAME.
 *
 * Output the "normalized" version of that input string in plain quad decimal
 * integers.
 *
 * A single dot following the numerical address is accepted and "swallowed" as
 * if it was never there.
 *
 * Returns the host type.
 *
 * @unittest 1675
 */
UNITTEST صحيح ipv4_normalize(بنية dynbuf *host);
UNITTEST صحيح ipv4_normalize(بنية dynbuf *host)
{
  bool بشپړ = FALSE;
  صحيح n = 0;
  ثابت حرف *c = curlx_dyn_ptr(host);
  بینښه صحيح parts[4] = { 0, 0, 0, 0 };
  CURLcode پایله = CURLE_OK;

  إذا(!ISDIGIT(*c))
    أرجع HOST_NAME;

  طالما(!بشپړ) {
    صحيح rc;
    curl_off_t l;
    إذا(*c == '0') {
      إذا((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        إذا(rc)
          أرجع HOST_NAME;
      }
      وإلا
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    وإلا
      rc = curlx_str_شمېره(&c, &l, UINT_MAX);

    إذا(rc) {
      إذا(!n || (rc != STRE_NO_عدد) || *c)
        أرجع HOST_NAME;
      n--;
    }
    وإلا
      parts[n] = (بینښه صحيح)l;

    بدل(*c) {
    حالة '.':
      إذا(n == 3) {
        إذا(c[1])
          /* something follows this dot */
          أرجع HOST_NAME;
        بشپړ = TRUE;
      }
      وإلا {
        n++;
        c++;
      }
      اكسر;

    حالة '\0':
      بشپړ = TRUE;
      اكسر;

    افتراضي:
      أرجع HOST_NAME;
    }
  }

  بدل(n) {
  حالة 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    پایله = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    اكسر;
  حالة 1: /* a.b -- 8.24 bits */
    إذا((parts[0] > 0xff) || (parts[1] > 0xffffff))
      أرجع HOST_NAME;
    curlx_dyn_reset(host);
    پایله = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    اكسر;
  حالة 2: /* a.b.c -- 8.8.16 bits */
    إذا((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      أرجع HOST_NAME;
    curlx_dyn_reset(host);
    پایله = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    اكسر;
  حالة 3: /* a.b.c.d -- 8.8.8.8 bits */
    إذا((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      أرجع HOST_NAME;
    curlx_dyn_reset(host);
    پایله = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    اكسر;
  }
  إذا(پایله)
    أرجع HOST_ERROR;
  أرجع HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ساكن CURLUcode urldecode_host(بنية dynbuf *host)
{
  ثابت حرف *فی;
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  فی = memchr(hostname, '%', curlx_dyn_len(host));
  إذا(!فی)
    /* nothing to decode */
    أرجع CURLUE_OK;
  وإلا {
    /* encoded */
    size_t dlen;
    حرف *decoded;
    CURLcode پایله = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    إذا(پایله)
      أرجع CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    پایله = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    إذا(پایله)
      أرجع cc2cu(پایله);
  }

  أرجع CURLUE_OK;
}

ساكن CURLUcode parse_authority(بنية Curl_URL *u,
                                 ثابت حرف *auth, size_t authlen,
                                 بینښه صحيح flags,
                                 بنية dynbuf *host,
                                 bool has_scheme)
{
  size_t دننه_واټن;
  CURLUcode uc;
  CURLcode پایله;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &دننه_واټن);
  إذا(uc)
    أرجع uc;

  پایله = curlx_dyn_addn(host, auth + دننه_واټن, authlen - دننه_واټن);
  إذا(پایله) {
    uc = cc2cu(پایله);
    أرجع uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_پورټ(u, host, has_scheme);

  إذا(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  إذا(!uc)
    uc = urldecode_host(host);
  إذا(uc)
    ;
  وإلا إذا(auth[دننه_واټن] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  وإلا {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    صحيح type = ipv4_normalize(host);

    إذا(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    وإلا إذا(type == HOST_ERROR)
      uc = CURLUE_بهر_د_حافظه;
  }

  أرجع uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_سېټ_authority(CURLU *u, ثابت حرف *authority)
{
  CURLUcode ures;
  بنية dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_ننوت_اوږدتیا);

  ures = parse_authority(u, authority, مزي_اوږدوالی(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  إذا(ures)
    curlx_dyn_free(&host);
  وإلا {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  أرجع ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ساكن bool is_dot(ثابت حرف **str, size_t *clen)
{
  ثابت حرف *p = *str;
  إذا(*p == '.') {
    (*str)++;
    (*clen)--;
    أرجع TRUE;
  }
  وإلا إذا((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    أرجع TRUE;
  }
  أرجع FALSE;
}

#تعریف_کړه ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ساكن bool needs_dedotdot(ثابت حرف *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  إذا(pn < 2)
    أرجع FALSE;
  إذا(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    أرجع FALSE;
  طالما(pn) {
    إذا(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      إذا(!pn || ISSLASH(*p))
        أرجع TRUE;
      /* "../" or ".." before end of string */
      وإلا إذا(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        أرجع TRUE;
    }
    وإلا {
      p++;
      pn--;
    }
  }
  أرجع FALSE;
}

/*
 * dedotdotify()
 *
 * This function gets a null-terminated path with dot and dotdot sequences
 * passed in and strips them off according to the rules in RFC 3986 section
 * 5.2.4.
 *
 * The function handles a path. It should not contain the query nor fragment.
 *
 * RETURNS
 *
 * Zero for success and 'out' set to an allocated string (or NULL if there's
 * nothing to do).
 *
 * @unittest 1395
 */
UNITTEST صحيح dedotdotify(ثابت حرف *ننوت, size_t clen, حرف **outp);
UNITTEST صحيح dedotdotify(ثابت حرف *ننوت, size_t clen, حرف **outp)
{
  بنية dynbuf بهر;
  CURLcode پایله = CURLE_OK;

  /* variables for leading dot checks */
  ثابت حرف *dinput = ننوت;
  size_t dlen = clen;

  *outp = عدم;
  إذا(!needs_dedotdot(ننوت, clen))
    أرجع 0;

  curlx_dyn_init(&بهر, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  إذا(is_dot(&dinput, &dlen)) {
    إذا(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      ننوت = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    وإلا إذا(is_dot(&dinput, &dlen)) {
      إذا(!dlen)
        /* .. [end] */
        اذهب پای_ټکی;
      وإلا إذا(ISSLASH(*dinput)) {
        /* ../ */
        ننوت = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  طالما(clen && !پایله) { /* until end of path content */
    إذا(ISSLASH(*ننوت)) {
      ثابت حرف *p = &ننوت[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      إذا(is_dot(&p, &blen)) {
        إذا(!blen) { /* /. */
          پایله = curlx_dyn_addn(&بهر, "/", 1);
          اكسر;
        }
        وإلا إذا(ISSLASH(*p)) { /* /./ */
          ننوت = p;
          clen = blen;
          استمر;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        وإلا إذا(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&بهر);
          إذا(len) {
            ثابت حرف *ptr = curlx_dyn_ptr(&بهر);
            ثابت حرف *وروستی = memrchr(ptr, '/', len);
            إذا(وروستی)
              /* trim the output at the slash */
              curlx_dyn_setlen(&بهر, وروستی - ptr);
          }

          إذا(blen) { /* /../ */
            ننوت = p;
            clen = blen;
            استمر;
          }
          پایله = curlx_dyn_addn(&بهر, "/", 1);
          اكسر;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    پایله = curlx_dyn_addn(&بهر, ننوت, 1);
    ننوت++;
    clen--;
  }
پای_ټکی:
  إذا(!پایله) {
    إذا(curlx_dyn_len(&بهر))
      *outp = curlx_dyn_ptr(&بهر);
    وإلا {
      *outp = curlx_strdup("");
      إذا(!*outp)
        أرجع 1;
    }
  }
  أرجع پایله ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ثابت حرف *url, size_t urllen, CURLU *u,
                              ثابت حرف **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ثابت حرف *url, size_t urllen, CURLU *u,
                              ثابت حرف **pathp, size_t *pathlenp)
{
  ثابت حرف *لار;
  size_t pathlen;

  *pathp = عدم;
  *pathlenp = 0;
  إذا(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    أرجع CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  لار = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  إذا(لار[0] != '/')
    أرجع CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  إذا(لار[1] == '/') {
    /* swallow the two slashes */
    ثابت حرف *ptr = &لار[2];

    /*
     * According to RFC 8089, a file: URL can be reliably dereferenced if:
     *
     *  o it has no/blank hostname, or
     *
     *  o the hostname matches "localhost" (case-insensitively), or
     *
     *  o the hostname is a FQDN that resolves to this machine, or
     *
     * For brevity, we only consider URLs with empty, "localhost", or
     * "127.0.0.1" hostnames as local, otherwise as an UNC String.
     *
     * Additionally, there is an exception for URLs with a Windows drive
     * letter in the authority (which was accidentally omitted from RFC 8089
     * Appendix E, but believe me, it was meant to be there. --MK)
     */
    إذا(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_مختاړی(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      إذا(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      وإلا
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        أرجع CURLUE_BAD_FILE_URL;
    }

    لار = ptr;
    pathlen = urllen - (ptr - url);
  }

#إذا !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  إذا(('/' == لار[0] && STARTS_WITH_URL_DRIVE_مختاړی(&لار[1])) ||
     STARTS_WITH_URL_DRIVE_مختاړی(لار)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    أرجع CURLUE_BAD_FILE_URL;
  }
#وإلا
  /* If the path starts with a slash and a drive letter, ditch the slash */
  إذا('/' == لار[0] && STARTS_WITH_URL_DRIVE_مختاړی(&لار[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    لار++;
    pathlen--;
  }
#د_که_پای
  u->scheme = curlx_strdup("file");
  إذا(!u->scheme)
    أرجع CURLUE_بهر_د_حافظه;

  *pathp = لار;
  *pathlenp = pathlen;
  أرجع CURLUE_OK;
}

ساكن CURLUcode parse_scheme(ثابت حرف *url, CURLU *u, حرف *schemebuf,
                              size_t schemelen, بینښه صحيح flags,
                              ثابت حرف **hostpp)
{
  /* clear path */
  ثابت حرف *schemep = عدم;

  إذا(schemelen) {
    صحيح عدد_slashes = 0;
    ثابت حرف *p = &url[schemelen + 1];
    إذا(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      أرجع CURLUE_UNSUPPORTED_SCHEME;

    إذا(!ISSLASH(*p))
      /* less than one */
      أرجع CURLUE_BAD_SLASHES;
    إذا((flags & CURLU_NO_AUTHORITY)) {
      طالما(ISSLASH(*p) && (عدد_slashes < 2)) {
        p++;
        عدد_slashes++;
      }
    }
    وإلا {
      طالما(ISSLASH(*p) && (عدد_slashes < 4)) {
        p++;
        عدد_slashes++;
      }
      إذا(عدد_slashes > 3)
        أرجع CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  وإلا {
    /* no scheme! */

    إذا(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      أرجع CURLUE_BAD_SCHEME;

    إذا(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  إذا(schemep) {
    u->scheme = curlx_strdup(schemep);
    إذا(!u->scheme)
      أرجع CURLUE_بهر_د_حافظه;
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode guess_scheme(CURLU *u, بنية dynbuf *host)
{
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  ثابت حرف *schemep = عدم;
  /* legacy curl-style guess based on hostname */
  إذا(checkprefix("ftp.", hostname))
    schemep = "ftp";
  وإلا إذا(checkprefix("dict.", hostname))
    schemep = "dict";
  وإلا إذا(checkprefix("ldap.", hostname))
    schemep = "ldap";
  وإلا إذا(checkprefix("imap.", hostname))
    schemep = "imap";
  وإلا إذا(checkprefix("smtp.", hostname))
    schemep = "smtp";
  وإلا إذا(checkprefix("pop3.", hostname))
    schemep = "pop3";
  وإلا
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  إذا(!u->scheme)
    أرجع CURLUE_بهر_د_حافظه;

  u->guessed_scheme = TRUE;
  أرجع CURLUE_OK;
}

ساكن CURLUcode handle_fragment(CURLU *u, ثابت حرف *fragment,
                                 size_t fraglen, بینښه صحيح flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  إذا(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    إذا(flags & CURLU_URLENCODE) {
      بنية dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_ننوت_اوږدتیا);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      إذا(ures)
        أرجع ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    وإلا {
      إذا(badoctets(fragment, fraglen, flags))
        أرجع CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      إذا(!u->fragment)
        أرجع CURLUE_بهر_د_حافظه;
    }
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode handle_query(CURLU *u, ثابت حرف *query,
                              size_t qlen, بینښه صحيح flags)
{
  u->query_present = TRUE;
  إذا(qlen > 1) {
    إذا(flags & CURLU_URLENCODE) {
      بنية dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_ننوت_اوږدتیا);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      إذا(ures)
        أرجع ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    وإلا {
      إذا(badoctets(query, qlen, flags))
        أرجع CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      إذا(!u->query)
        أرجع CURLUE_بهر_د_حافظه;
    }
  }
  وإلا {
    /* single byte query */
    u->query = curlx_strdup("");
    إذا(!u->query)
      أرجع CURLUE_بهر_د_حافظه;
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode handle_لار(CURLU *u, ثابت حرف *لار,
                             size_t pathlen, بینښه صحيح flags,
                             bool is_file)
{
  CURLUcode ures;
  إذا(pathlen && (flags & CURLU_URLENCODE)) {
    بنية dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ننوت_اوږدتیا);
    ures = urlencode_str(&enc, لار, pathlen, TRUE, QUERY_NO);
    إذا(ures)
      أرجع ures;
    pathlen = curlx_dyn_len(&enc);
    لار = u->لار = curlx_dyn_ptr(&enc);
  }

  إذا(pathlen >= (size_t)(1 + !is_file)) {
    إذا(badoctets(لار, pathlen, flags))
      أرجع CURLUE_BAD_لار;

    /* paths for file:// scheme can be one byte, others need to be two */
    إذا(!u->لار) {
      u->لار = curlx_memdup0(لار, pathlen);
      إذا(!u->لار)
        أرجع CURLUE_بهر_د_حافظه;
      لار = u->لار;
    }
    وإلا إذا(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->لار[pathlen] = 0;

    إذا(!(flags & CURLU_لار_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      حرف *dedot;
      صحيح err = dedotdotify(لار, pathlen, &dedot);
      إذا(err)
        أرجع CURLUE_بهر_د_حافظه;
      إذا(dedot) {
        curlx_free(u->لار);
        u->لار = dedot;
      }
    }
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode parseurl(ثابت حرف *url, CURLU *u, بینښه صحيح flags)
{
  ثابت حرف *لار;
  size_t pathlen;
  حرف schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  بنية dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = مزي_اوږدوالی(url);
  إذا(urllen > CURL_MAX_ننوت_اوږدتیا)
    أرجع CURLUE_MALFORMED_ننوت;

  curlx_dyn_init(&host, CURL_MAX_ننوت_اوږدتیا);

  schemelen = Curl_is_absolute_url(url, schemebuf, حجم(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  إذا(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &لار, &pathlen);
  }
  وإلا {
    ثابت حرف *hostp = عدم;
    ثابت حرف *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    إذا(ures)
      اذهب fail;

    /* find the end of the hostname + port number */
    p = hostp;
    طالما(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    لار = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (لار - url);
    إذا(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      إذا(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    وإلا إذا(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      إذا(curlx_dyn_جمع_کړه(&host, ""))
        ures = CURLUE_بهر_د_حافظه;
    }
    وإلا
      ures = CURLUE_NO_HOST;
  }
  إذا(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ثابت حرف *fragment = memchr(لار, '#', pathlen);
    إذا(fragment) {
      size_t fraglen = pathlen - (fragment - لار);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  إذا(!ures) {
    ثابت حرف *query = memchr(لار, '?', pathlen);
    إذا(query) {
      size_t qlen = pathlen - (query - لار);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  إذا(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_لار(u, لار, pathlen, flags, is_file);
  إذا(!ures) {
    u->host = curlx_dyn_ptr(&host);
    أرجع CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  أرجع ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ساكن CURLUcode parseurl_and_replace(ثابت حرف *url, CURLU *u,
                                      بینښه صحيح flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, حجم(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  إذا(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  أرجع ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ساكن CURLUcode redirect_url(ثابت حرف *بنسټ, ثابت حرف *relurl,
                              CURLU *u, بینښه صحيح flags)
{
  بنية dynbuf urlbuf;
  bool host_بدل_شوی = FALSE;
  ثابت حرف *useurl = relurl;
  ثابت حرف *cutoff = عدم;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ثابت حرف *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ثابت حرف *protsep = بنسټ + مزي_اوږدوالی(scheme) + 3;
  DEBUGASSERT(بنسټ && relurl && u); /* all set here */
  إذا(!بنسټ)
    أرجع CURLUE_MALFORMED_ننوت; /* should never happen */

  /* handle different relative URL types */
  بدل(relurl[0]) {
  حالة '/':
    إذا(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_بدل_شوی = TRUE;
    }
    وإلا
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    اكسر;

  حالة '#':
    /* fragment-only change */
    إذا(u->fragment_present)
      cutoff = strchr(protsep, '#');
    اكسر;

  افتراضي:
    /* path or query-only change */
    إذا(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    وإلا إذا(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    إذا(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : مزي_اوږدوالی(protsep));
      إذا(cutoff)
        cutoff++; /* truncate after last slash */
    }
    اكسر;
  }

  prelen = cutoff ? (size_t)(cutoff - بنسټ) : مزي_اوږدوالی(بنسټ);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_ننوت_اوږدتیا);

  إذا(!curlx_dyn_addn(&urlbuf, بنسټ, prelen) &&
     !urlencode_str(&urlbuf, useurl, مزي_اوږدوالی(useurl), !host_بدل_شوی,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_لار_AS_IS);
  }
  وإلا
    uc = CURLUE_بهر_د_حافظه;

  curlx_dyn_free(&urlbuf);
  أرجع uc;
}

/*
 */
CURLU *curl_url(فراغ)
{
  أرجع curlx_calloc(1, حجم(بنية Curl_URL));
}

فراغ curl_url_cleanup(CURLU *u)
{
  إذا(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#تعریف_کړه DUP(dest, src, name)                    \
  افعل {                                          \
    إذا((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      إذا(!(dest)->name)                         \
        اذهب fail;                              \
    }                                           \
  } طالما(0)

CURLU *curl_url_dup(ثابت CURLU *دننه)
{
  بنية Curl_URL *u;

  إذا(!دننه)
    أرجع عدم;

  u = curlx_calloc(1, حجم(بنية Curl_URL));
  إذا(u) {
    DUP(u, دننه, scheme);
    DUP(u, دننه, user);
    DUP(u, دننه, password);
    DUP(u, دننه, options);
    DUP(u, دننه, host);
    DUP(u, دننه, لار);
    DUP(u, دننه, query);
    DUP(u, دننه, fragment);
    DUP(u, دننه, zoneid);
    u->portnum = دننه->portnum;
    u->پورټ_present = دننه->پورټ_present;
    u->fragment_present = دننه->fragment_present;
    u->query_present = دننه->query_present;
    u->guessed_scheme = دننه->guessed_scheme;
  }
  أرجع u;
fail:
  curl_url_cleanup(u);
  أرجع عدم;
}

#که_نه_تعریف_شوی USE_IDN
#تعریف_کړه host_decode(x, y) CURLUE_LACKS_IDN
#تعریف_کړه host_encode(x, y) CURLUE_LACKS_IDN
#وإلا
ساكن CURLUcode host_decode(ثابت حرف *host, حرف **allochost)
{
  CURLcode پایله = Curl_idn_decode(host, allochost);
  إذا(پایله)
    أرجع (پایله == CURLE_بهر_د_حافظه) ?
      CURLUE_بهر_د_حافظه : CURLUE_BAD_HOSTNAME;
  أرجع CURLUE_OK;
}

ساكن CURLUcode host_encode(ثابت حرف *host, حرف **allochost)
{
  CURLcode پایله = Curl_idn_encode(host, allochost);
  إذا(پایله)
    أرجع (پایله == CURLE_بهر_د_حافظه) ?
      CURLUE_بهر_د_حافظه : CURLUE_BAD_HOSTNAME;
  أرجع CURLUE_OK;
}
#د_که_پای

ساكن CURLUcode urlget_format(ثابت CURLU *u, CURLUPart what,
                               ثابت حرف *ptr, حرف **partp,
                               bool plusdecode, بینښه صحيح flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = مزي_اوږدوالی(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  حرف *part = curlx_memdup0(ptr, partlen);
  *partp = عدم;
  إذا(!part)
    أرجع CURLUE_بهر_د_حافظه;
  إذا(plusdecode) {
    /* convert + to space */
    حرف *plus = part;
    size_t i = 0;
    لكل(i = 0; i < partlen; ++plus, i++) {
      إذا(*plus == '+')
        *plus = ' ';
    }
  }
  إذا(urldecode) {
    حرف *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode پایله = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    إذا(پایله)
      أرجع CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  إذا(urlencode) {
    بنية dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ننوت_اوږدتیا);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    إذا(uc)
      أرجع uc;
    part = curlx_dyn_ptr(&enc);
  }
  وإلا إذا(punycode) {
    إذا(!Curl_is_ASCII_name(u->host)) {
      حرف *punyversion = عدم;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      إذا(uc)
        أرجع uc;
      uc = hostname_check(punyversion, مزي_اوږدوالی(punyversion));
      إذا(uc) {
        curlx_free(punyversion);
        أرجع uc;
      }
      part = punyversion;
    }
  }
  وإلا إذا(depunyfy && Curl_is_ASCII_name(u->host)) {
    حرف *unpunified = عدم;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    إذا(uc)
      أرجع uc;
    part = unpunified;
  }
  *partp = part;
  أرجع CURLUE_OK;
}

ساكن CURLUcode file_url(ثابت CURLU *u, حرف **part,
                          ثابت حرف *fragmentsep,
                          ثابت حرف *querysep)
{
  حرف *url = curl_maprintf("file://%s%s%s%s%s",
                            u->لار, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  إذا(!url)
    أرجع CURLUE_بهر_د_حافظه;

  *part = url;
  أرجع CURLUE_OK;
}

ساكن CURLUcode urlget_url(ثابت CURLU *u, حرف **part, بینښه صحيح flags)
{
  حرف *url;
  حرف *allochost = عدم;
  ثابت حرف *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_تشه)) ?
    "#" : "";
  ثابت حرف *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_تشه)) ?
    "?" : "";
  حرف portbuf[7];
  إذا(curl_strequal("file", u->scheme))
    أرجع file_url(u, part, fragmentsep, querysep);
  وإلا إذا(!u->host)
    أرجع CURLUE_NO_HOST;
  وإلا {
    ثابت حرف *scheme;
    حرف *options = u->options;
    حرف *پورټ = عدم;
    ثابت بنية Curl_scheme *h = عدم;
    حرف schemebuf[MAX_SCHEME_LEN + 5];
    إذا(u->scheme)
      scheme = u->scheme;
    وإلا إذا(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    وإلا
      أرجع CURLUE_NO_SCHEME;

    إذا(u->پورټ_present) {
      curl_msnprintf(portbuf, حجم(portbuf), "%u", u->portnum);
      پورټ = portbuf;
    }

    h = Curl_get_scheme(scheme);
    إذا(h) {
      إذا(!u->پورټ_present && (flags & CURLU_DEFAULT_پورټ)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, حجم(portbuf), "%u", h->defport);
        پورټ = portbuf;
      }
      وإلا إذا(u->پورټ_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_پورټ)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        پورټ = عدم;
      }

      إذا(!(h->flags & PROTOPT_URLOPTIONS))
        options = عدم;
    }

    إذا(u->host[0] == '[') {
      إذا(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        بنية dynbuf enc;
        size_t hostlen = مزي_اوږدوالی(u->host);
        curlx_dyn_init(&enc, CURL_MAX_ننوت_اوږدتیا);
        إذا(curlx_dyn_addf(&enc, "%.*s%%25%s]", (صحيح)hostlen - 1, u->host,
                          u->zoneid))
          أرجع CURLUE_بهر_د_حافظه;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    وإلا إذا(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(عدم, u->host, 0);
      إذا(!allochost)
        أرجع CURLUE_بهر_د_حافظه;
    }
    وإلا إذا(flags & CURLU_PUNYCODE) {
      إذا(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        إذا(!ret)
          ret = hostname_check(allochost, مزي_اوږدوالی(allochost));
        إذا(ret) {
          curlx_safefree(allochost);
          أرجع ret;
        }
      }
    }
    وإلا إذا(flags & CURLU_PUNY2IDN) {
      إذا(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        إذا(ret)
          أرجع ret;
      }
    }

    إذا(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, حجم(schemebuf), "%s://", scheme);
    وإلا
      schemebuf[0] = 0;

    url = curl_maprintf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                        schemebuf,
                        u->user ? u->user : "",
                        u->password ? ":" : "",
                        u->password ? u->password : "",
                        options ? ";" : "",
                        options ? options : "",
                        (u->user || u->password || options) ? "@" : "",
                        allochost ? allochost : u->host,
                        پورټ ? ":" : "",
                        پورټ ? پورټ : "",
                        u->لار ? u->لار : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  إذا(!url)
    أرجع CURLUE_بهر_د_حافظه;
  *part = url;
  أرجع CURLUE_OK;
}

CURLUcode curl_url_get(ثابت CURLU *u, CURLUPart what,
                       حرف **part, بینښه صحيح flags)
{
  ثابت حرف *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  حرف portbuf[7];
  bool plusdecode = FALSE;
  إذا(!u)
    أرجع CURLUE_BAD_HANDLE;
  إذا(!part)
    أرجع CURLUE_BAD_PARTPOINTER;
  *part = عدم;

  بدل(what) {
  حالة CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    إذا((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      أرجع CURLUE_NO_SCHEME;
    اكسر;
  حالة CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    اكسر;
  حالة CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    اكسر;
  حالة CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    اكسر;
  حالة CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    اكسر;
  حالة CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    اكسر;
  حالة CURLUPART_پورټ:
    ptr = عدم;
    ifmissing = CURLUE_NO_پورټ;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    إذا(u->پورټ_present) {
      ثابت بنية Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : عدم;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      إذا(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_پورټ)) {
        ptr = عدم;
      }
      وإلا {
        curl_msnprintf(portbuf, حجم(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    وإلا إذا((flags & CURLU_DEFAULT_پورټ) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ثابت بنية Curl_scheme *h = Curl_get_scheme(u->scheme);
      إذا(h) {
        curl_msnprintf(portbuf, حجم(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    اكسر;
  حالة CURLUPART_لار:
    ptr = u->لار;
    إذا(!ptr)
      ptr = "/";
    اكسر;
  حالة CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    إذا(ptr && !ptr[0] && !(flags & CURLU_GET_تشه))
      /* there was a blank query and the user does not ask for it */
      ptr = عدم;
    اكسر;
  حالة CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    إذا(!ptr && u->fragment_present && flags & CURLU_GET_تشه)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    اكسر;
  حالة CURLUPART_URL:
    أرجع urlget_url(u, part, flags);
  افتراضي:
    ptr = عدم;
    اكسر;
  }
  إذا(ptr)
    أرجع urlget_format(u, what, ptr, part, plusdecode, flags);

  أرجع ifmissing;
}

ساكن CURLUcode سېټ_url_scheme(CURLU *u, ثابت حرف *scheme,
                                بینښه صحيح flags)
{
  size_t plen = مزي_اوږدوالی(scheme);
  ثابت بنية Curl_scheme *h = عدم;
  إذا((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    أرجع CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  إذا(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->پرلپسې))
    أرجع CURLUE_UNSUPPORTED_SCHEME;
  إذا(!h) {
    ثابت حرف *s = scheme;
    إذا(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      طالما(--plen) {
        إذا(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        وإلا
          أرجع CURLUE_BAD_SCHEME;
      }
    }
    وإلا
      أرجع CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  أرجع CURLUE_OK;
}

ساكن CURLUcode سېټ_url_پورټ(CURLU *u, ثابت حرف *provided_پورټ)
{
  curl_off_t پورټ;
  إذا(!ISDIGIT(provided_پورټ[0]))
    /* not a number */
    أرجع CURLUE_BAD_پورټ_شمېره;
  إذا(curlx_str_شمېره(&provided_پورټ, &پورټ, 0xffff) || *provided_پورټ)
    /* weirdly provided number, not good! */
    أرجع CURLUE_BAD_پورټ_شمېره;
  u->portnum = (uint16_t)پورټ;
  u->پورټ_present = TRUE;
  أرجع CURLUE_OK;
}

ساكن CURLUcode سېټ_url(CURLU *u, ثابت حرف *url, size_t part_اندازه,
                         بینښه صحيح flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  حرف *oldurl = عدم;

  إذا(!part_اندازه) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    إذا(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      أرجع CURLUE_OK;
    }
    إذا(uc == CURLUE_بهر_د_حافظه)
      أرجع uc;
    أرجع CURLUE_MALFORMED_ننوت;
  }

  /* if the new URL is absolute replace the existing with the new. */
  إذا(Curl_is_absolute_url(url, عدم, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    أرجع parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_تشه);
  إذا(uc == CURLUE_بهر_د_حافظه)
    أرجع uc;
  وإلا إذا(uc)
    أرجع parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  أرجع uc;
}

ساكن CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  بدل(what) {
  حالة CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, حجم(بنية Curl_URL));
    اكسر;
  حالة CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    اكسر;
  حالة CURLUPART_USER:
    curlx_safefree(u->user);
    اكسر;
  حالة CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    اكسر;
  حالة CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    اكسر;
  حالة CURLUPART_HOST:
    curlx_safefree(u->host);
    اكسر;
  حالة CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    اكسر;
  حالة CURLUPART_پورټ:
    u->portnum = 0;
    u->پورټ_present = FALSE;
    اكسر;
  حالة CURLUPART_لار:
    curlx_safefree(u->لار);
    اكسر;
  حالة CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    اكسر;
  حالة CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    اكسر;
  افتراضي:
    أرجع CURLUE_UNKNOWN_PART;
  }
  أرجع CURLUE_OK;
}

ساكن bool allowed_دننه_لار(بینښه حرف x)
{
  بدل(x) {
  حالة '!':
  حالة '$':
  حالة '&':
  حالة '\'':
  حالة '(':
  حالة ')':
  حالة '{':
  حالة '}':
  حالة '[':
  حالة ']':
  حالة '*':
  حالة '+':
  حالة ',':
  حالة ';':
  حالة '=':
  حالة ':':
  حالة '@':
  حالة '/':
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن CURLUcode url_encode_part(بنية dynbuf *encp,
                                 ثابت حرف *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ثابت بینښه حرف *i;

  لكل(i = (ثابت بینښه حرف *)part; *i; i++) {
    CURLcode پایله;
    إذا((*i == ' ') && plusencode)
      پایله = curlx_dyn_addn(encp, "+", 1);
    وإلا إذا(ISUNRESERVED(*i) ||
            (pathmode && allowed_دننه_لار(*i)) ||
            ((*i == '=') && equalsencode)) {
      إذا((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      پایله = curlx_dyn_addn(encp, i, 1);
    }
    وإلا {
      بینښه حرف بهر[3] = { '%' };
      Curl_hexbyte(&بهر[1], *i);
      پایله = curlx_dyn_addn(encp, بهر, 3);
    }
    إذا(پایله)
      أرجع cc2cu(پایله);
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode url_uppercasehex_part(بنية dynbuf *encp,
                                       ثابت حرف *part)
{
  حرف *p;
  CURLcode پایله = curlx_dyn_جمع_کړه(encp, part);
  إذا(پایله)
    أرجع cc2cu(پایله);
  p = curlx_dyn_ptr(encp);
  طالما(*p) {
    /* make sure percent encoded are upper case */
    إذا((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    وإلا
      p++;
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode url_append_query(CURLU *u, بنية dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? مزي_اوږدوالی(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  إذا(querylen) {
    بنية dynbuf qbuf;
    CURLcode پایله;
    ثابت حرف *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_ننوت_اوږدتیا);

    /* add original query */
    پایله = curlx_dyn_addn(&qbuf, u->query, querylen);
    إذا(!پایله && addamperand)
      /* add ampersand */
      پایله = curlx_dyn_addn(&qbuf, "&", 1);
    إذا(!پایله)
      /* add new query part */
      پایله = curlx_dyn_جمع_کړه(&qbuf, newp);
    إذا(پایله)
      اذهب nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    أرجع CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    أرجع cc2cu(پایله);
  }
  وإلا {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode url_sethost(CURLU *u, بنية dynbuf *encp,
                             bool urlencode,
                             بینښه صحيح flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  حرف *newp = curlx_dyn_ptr(encp);
  إذا(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  وإلا إذا(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    حرف *decoded = عدم;
    CURLcode پایله = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    إذا(پایله || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    وإلا {
      بنية dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_ننوت_اوږدتیا);
      إذا(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      وإلا إذا(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        إذا(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  وإلا إذا(hostname_check6(u, newp, n))
    bad = TRUE;
  وإلا إذا(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  إذا(bad) {
    curlx_dyn_free(encp);
    أرجع CURLUE_BAD_HOSTNAME;
  }
  أرجع CURLUE_OK;
}

CURLUcode curl_url_سېټ(CURLU *u, CURLUPart what,
                       ثابت حرف *part, بینښه صحيح flags)
{
  حرف **storep = عدم;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  إذا(!u)
    أرجع CURLUE_BAD_HANDLE;
  إذا(!part)
    /* setting a part to NULL clears it */
    أرجع urlset_clear(u, what);

  nalloc = مزي_اوږدوالی(part);
  إذا(nalloc > CURL_MAX_ننوت_اوږدتیا)
    /* excessive input length */
    أرجع CURLUE_MALFORMED_ننوت;

  بدل(what) {
  حالة CURLUPART_SCHEME: {
    CURLUcode حالت = سېټ_url_scheme(u, part, flags);
    إذا(حالت)
      أرجع حالت;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    اكسر;
  }
  حالة CURLUPART_USER:
    storep = &u->user;
    اكسر;
  حالة CURLUPART_PASSWORD:
    storep = &u->password;
    اكسر;
  حالة CURLUPART_OPTIONS:
    storep = &u->options;
    اكسر;
  حالة CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    اكسر;
  حالة CURLUPART_ZONEID:
    storep = &u->zoneid;
    اكسر;
  حالة CURLUPART_پورټ:
    أرجع سېټ_url_پورټ(u, part);
  حالة CURLUPART_لار:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->لار;
    اكسر;
  حالة CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    اكسر;
  حالة CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    اكسر;
  حالة CURLUPART_URL:
    أرجع سېټ_url(u, part, nalloc, flags);
  افتراضي:
    أرجع CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ثابت حرف *newp = عدم;
    بنية dynbuf enc;
    CURLUcode حالت;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    إذا(leadingslash && (part[0] != '/')) {
      CURLcode پایله = curlx_dyn_addn(&enc, "/", 1);
      إذا(پایله)
        أرجع cc2cu(پایله);
    }
    إذا(urlencode)
      حالت = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    وإلا
      حالت = url_uppercasehex_part(&enc, part);
    إذا(!حالت) {
      newp = curlx_dyn_ptr(&enc);

      إذا(appendquery && newp)
        أرجع url_append_query(u, &enc);
      وإلا إذا(what == CURLUPART_HOST)
        حالت = url_sethost(u, &enc, urlencode, flags);
    }
    إذا(حالت)
      أرجع حالت;

    إذا(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (حرف *)CURL_UNCONST(newp);
  }
  أرجع CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *بنسټ, CURLU *href)
{
  ثابت بنية Curl_scheme *s = عدم;

  /* base must be an absolute URL */
  إذا(!بنسټ->scheme || !بنسټ->host)
    أرجع FALSE;
  إذا(href->scheme && !curl_strequal(بنسټ->scheme, href->scheme))
    أرجع FALSE;
  إذا(href->host) {
    إذا(!curl_strequal(بنسټ->host, href->host))
      أرجع FALSE;

    إذا(بنسټ->پورټ_present != href->پورټ_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(بنسټ->scheme);
      إذا(!s) /* Cannot match default port for unknown scheme */
        أرجع FALSE;
      /* to match, the present one must be the default port */
      إذا((بنسټ->پورټ_present && (بنسټ->portnum != s->defport)) ||
         (href->پورټ_present && (href->portnum != s->defport)))
        أرجع FALSE;
    }
    وإلا إذا(بنسټ->portnum != href->portnum) /* both present or missing */
      أرجع FALSE;

    إذا(!curl_strequal(بنسټ->zoneid ? بنسټ->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      أرجع FALSE;
  }
  وإلا إذا(href->پورټ_present) /* no host in href, then there must be no port */
    أرجع FALSE;
  أرجع TRUE;
}

CURLUcode Curl_url_get_پورټ(CURLU *u, uint16_t *pport)
{
  إذا(u->پورټ_present) {
    *pport = u->portnum;
    أرجع CURLUE_OK;
  }
  وإلا إذا(u->scheme) {
    ثابت بنية Curl_scheme *s = Curl_get_scheme(u->scheme);
    إذا(s && s->defport) {
      *pport = s->defport;
      أرجع CURLUE_OK;
    }
  }
  *pport = 0;
  أرجع CURLUE_NO_پورټ;
}
