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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "urlapi-int.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/inet_ntop.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "idn.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curl_memrchr.h"

#ifdef _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#define STARTS_WITH_DRIVE_PREFIX(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#endif

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#define STARTS_WITH_URL_DRIVE_PREFIX(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#define MAX_SCHEME_LEN 40
#define MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#যদি !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

স্থির শূন্য free_urlhandle(সংরচনা Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->path);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
স্থির ধ্রুব বর্ণ *find_host_sep(ধ্রুব বর্ণ *url)
{
  /* Find the start of the hostname */
  ধ্রুব বর্ণ *sep = strstr(url, "//");
  যদি(!sep)
    sep = url;
  অন্যথা
    sep += 2;

  /* Find first / or ? */
  যাবত্(*sep && *sep != '/' && *sep != '?')
    sep++;

  প্রত্যাগম sep;
}

/* convert CURLcode to CURLUcode */
#define cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_OUT_OF_MEMORY)

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
UNITTEST CURLUcode urlencode_str(সংরচনা dynbuf *o, ধ্রুব বর্ণ *url,
                                 size_t len, bool relative,
                                 unsigned পূর্ণাঙ্ক query);
UNITTEST CURLUcode urlencode_str(সংরচনা dynbuf *o, ধ্রুব বর্ণ *url,
                                 size_t len, bool relative,
                                 unsigned পূর্ণাঙ্ক query)
{
  /* we must add this with whitespace-replacing */
  ধ্রুব unsigned বর্ণ *iptr;
  ধ্রুব unsigned বর্ণ *host_sep = (ধ্রুব unsigned বর্ণ *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  যদি(!relative) {
    size_t n;
    host_sep = (ধ্রুব unsigned বর্ণ *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ধ্রুব বর্ণ *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  প্রতি(iptr = host_sep; len && !result;) {
    যদি(*iptr == ' ') {
      যদি(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      অন্যথা
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    অন্যথা যদি((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned বর্ণ out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    অন্যথা যদি(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned বর্ণ hex = (unsigned বর্ণ)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned বর্ণ out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    অন্যথা {
      ধ্রুব unsigned বর্ণ *start = iptr;
      যাবত্(len) {
        যদি(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          ভঙ্গ;
        যদি(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          ভঙ্গ;
        যদি(*iptr == '?') {
          যদি(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            ভঙ্গ;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ধ্রুব বর্ণ *)start, (size_t)(iptr - start));
    }
  }

  যদি(result)
    প্রত্যাগম cc2cu(result);
  প্রত্যাগম CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ধ্রুব বর্ণ *url, বর্ণ *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (শূন্য)buflen; /* only used in debug-builds */
  যদি(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  যদি(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    প্রত্যাগম 0;
#endif
  যদি(ISALPHA(url[0])) {
    যদি(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    প্রতি(i = 1; i < MAX_SCHEME_LEN; ++i) {
      বর্ণ s = url[i];
      যদি(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        যদি(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      অন্যথা {
        ভঙ্গ;
      }
    }
  }
  যদি(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    যদি(buf)
      buf[i] = 0;
    প্রত্যাগম len;
  }
  যদি(buf)
    buf[0] = 0;
  প্রত্যাগম 0;
}

/* scan for byte values <= 31, 127 and maybe space */
স্থির bool badoctets(ধ্রুব বর্ণ *প্রবেশ, size_t n, পূর্ণাঙ্ক flags)
{
  ধ্রুব uint8_t *p = (ধ্রুব unsigned বর্ণ *)প্রবেশ;
  ধ্রুব uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  যাবত্(n--) {
    যদি(*p <= control || *p == 127)
      প্রত্যাগম TRUE;
    p++;
  }
  প্রত্যাগম FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(সংরচনা Curl_URL *u,
                                        ধ্রুব বর্ণ *login,
                                        size_t len,
                                        unsigned পূর্ণাঙ্ক flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(সংরচনা Curl_URL *u,
                                        ধ্রুব বর্ণ *login,
                                        size_t len,
                                        unsigned পূর্ণাঙ্ক flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  বর্ণ *userp = রিক্ত;
  বর্ণ *passwdp = রিক্ত;
  বর্ণ *optionsp = রিক্ত;
  ধ্রুব সংরচনা Curl_scheme *h = রিক্ত;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ধ্রুব বর্ণ *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  যদি(!ptr)
    গচ্ছ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  যদি(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : রিক্ত);
  যদি(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  যদি(userp) {
    যদি(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      গচ্ছ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  যদি(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  যদি(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  যদি(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  অন্যথা যদি(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  অন্যথা যদি(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_প্রবেশ;

  userp = passwdp = optionsp = রিক্ত;

  যদি(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    প্রত্যাগম CURLUE_OK;
  }

out:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  প্রত্যাগম ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(সংরচনা Curl_URL *u, সংরচনা dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(সংরচনা Curl_URL *u, সংরচনা dynbuf *host,
                              bool has_scheme)
{
  ধ্রুব বর্ণ *portptr;
  ধ্রুব বর্ণ *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  যদি(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    যদি(!portptr)
      প্রত্যাগম CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    যদি(*portptr) {
      যদি(*portptr != ':')
        প্রত্যাগম CURLUE_BAD_PORT_সংখ্যা;
    }
    অন্যথা
      portptr = রিক্ত;
  }
  অন্যথা
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  যদি(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    পূর্ণাঙ্ক rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    যদি(!*portptr)
      প্রত্যাগম has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_সংখ্যা;
    যদি(*portptr == '\\')
      প্রত্যাগম CURLUE_BACKSLASH;
    rc = curlx_str_সংখ্যা(&portptr, &port, 0xffff);
    যদি(rc)
      প্রত্যাগম CURLUE_BAD_PORT_সংখ্যা;
    অন্যথা যদি(*portptr == '\\')
      প্রত্যাগম CURLUE_BACKSLASH;
    অন্যথা যদি(*portptr)
      প্রত্যাগম CURLUE_BAD_PORT_সংখ্যা;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  প্রত্যাগম CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

স্থির ধ্রুব bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(সংরচনা Curl_URL *u, বর্ণ *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(সংরচনা Curl_URL *u, বর্ণ *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  যদি(hlen < 4) /* '[::]' is the shortest possible valid string */
    প্রত্যাগম CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  যদি(hlen != len) {
    hlen = len;
    যদি(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      বর্ণ zoneid[MAX_ZONEID_LEN];
      পূর্ণাঙ্ক i = 0;
      বর্ণ *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      যদি(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      যাবত্(!invalid_host_char[(unsigned বর্ণ)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      যদি(!i || (']' != *h))
        প্রত্যাগম CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      যদি(!u->zoneid)
        প্রত্যাগম CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    অন্যথা
      প্রত্যাগম CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    বর্ণ dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    যদি(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      প্রত্যাগম CURLUE_BAD_IPV6;
    যদি(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  প্রত্যাগম CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
স্থির CURLUcode hostname_check(বর্ণ *hostname, size_t hlen)
{
  size_t i;
  প্রতি(i = 0; i < hlen; i++) {
    যদি(invalid_host_char[(unsigned বর্ণ)hostname[i]])
      প্রত্যাগম CURLUE_BAD_HOSTNAME;
  }
  যদি((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    প্রত্যাগম CURLUE_BAD_HOSTNAME;
  অন্যথা যদি((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    প্রত্যাগম CURLUE_BAD_HOSTNAME;
  প্রত্যাগম CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
স্থির CURLUcode hostname_check6(সংরচনা Curl_URL *u, বর্ণ *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  যদি(!hlen)
    প্রত্যাগম CURLUE_NO_HOST;
  অন্যথা যদি(hostname[0] == '[')
    প্রত্যাগম ipv6_parse(u, hostname, hlen);

  প্রত্যাগম hostname_check(hostname, hlen);
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
UNITTEST পূর্ণাঙ্ক ipv4_normalize(সংরচনা dynbuf *host);
UNITTEST পূর্ণাঙ্ক ipv4_normalize(সংরচনা dynbuf *host)
{
  bool done = FALSE;
  পূর্ণাঙ্ক n = 0;
  ধ্রুব বর্ণ *c = curlx_dyn_ptr(host);
  unsigned পূর্ণাঙ্ক parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  যদি(!ISDIGIT(*c))
    প্রত্যাগম HOST_NAME;

  যাবত্(!done) {
    পূর্ণাঙ্ক rc;
    curl_off_t l;
    যদি(*c == '0') {
      যদি((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        যদি(rc)
          প্রত্যাগম HOST_NAME;
      }
      অন্যথা
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    অন্যথা
      rc = curlx_str_সংখ্যা(&c, &l, UINT_MAX);

    যদি(rc) {
      যদি(!n || (rc != STRE_NO_অঙ্ক) || *c)
        প্রত্যাগম HOST_NAME;
      n--;
    }
    অন্যথা
      parts[n] = (unsigned পূর্ণাঙ্ক)l;

    বিকল্প(*c) {
    পক্ষ '.':
      যদি(n == 3) {
        যদি(c[1])
          /* something follows this dot */
          প্রত্যাগম HOST_NAME;
        done = TRUE;
      }
      অন্যথা {
        n++;
        c++;
      }
      ভঙ্গ;

    পক্ষ '\0':
      done = TRUE;
      ভঙ্গ;

    স্বভাব:
      প্রত্যাগম HOST_NAME;
    }
  }

  বিকল্প(n) {
  পক্ষ 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    ভঙ্গ;
  পক্ষ 1: /* a.b -- 8.24 bits */
    যদি((parts[0] > 0xff) || (parts[1] > 0xffffff))
      প্রত্যাগম HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    ভঙ্গ;
  পক্ষ 2: /* a.b.c -- 8.8.16 bits */
    যদি((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      প্রত্যাগম HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    ভঙ্গ;
  পক্ষ 3: /* a.b.c.d -- 8.8.8.8 bits */
    যদি((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      প্রত্যাগম HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    ভঙ্গ;
  }
  যদি(result)
    প্রত্যাগম HOST_ERROR;
  প্রত্যাগম HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
স্থির CURLUcode urldecode_host(সংরচনা dynbuf *host)
{
  ধ্রুব বর্ণ *per;
  ধ্রুব বর্ণ *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  যদি(!per)
    /* nothing to decode */
    প্রত্যাগম CURLUE_OK;
  অন্যথা {
    /* encoded */
    size_t dlen;
    বর্ণ *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    যদি(result)
      প্রত্যাগম CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    যদি(result)
      প্রত্যাগম cc2cu(result);
  }

  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode parse_authority(সংরচনা Curl_URL *u,
                                 ধ্রুব বর্ণ *auth, size_t authlen,
                                 unsigned পূর্ণাঙ্ক flags,
                                 সংরচনা dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  যদি(uc)
    প্রত্যাগম uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  যদি(result) {
    uc = cc2cu(result);
    প্রত্যাগম uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  যদি(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  যদি(!uc)
    uc = urldecode_host(host);
  যদি(uc)
    ;
  অন্যথা যদি(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  অন্যথা {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    পূর্ণাঙ্ক type = ipv4_normalize(host);

    যদি(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    অন্যথা যদি(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  প্রত্যাগম uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ধ্রুব বর্ণ *authority)
{
  CURLUcode ures;
  সংরচনা dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_প্রবেশ_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  যদি(ures)
    curlx_dyn_free(&host);
  অন্যথা {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  প্রত্যাগম ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

স্থির bool is_dot(ধ্রুব বর্ণ **str, size_t *clen)
{
  ধ্রুব বর্ণ *p = *str;
  যদি(*p == '.') {
    (*str)++;
    (*clen)--;
    প্রত্যাগম TRUE;
  }
  অন্যথা যদি((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
স্থির bool needs_dedotdot(ধ্রুব বর্ণ *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  যদি(pn < 2)
    প্রত্যাগম FALSE;
  যদি(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    প্রত্যাগম FALSE;
  যাবত্(pn) {
    যদি(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      যদি(!pn || ISSLASH(*p))
        প্রত্যাগম TRUE;
      /* "../" or ".." before end of string */
      অন্যথা যদি(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        প্রত্যাগম TRUE;
    }
    অন্যথা {
      p++;
      pn--;
    }
  }
  প্রত্যাগম FALSE;
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
UNITTEST পূর্ণাঙ্ক dedotdotify(ধ্রুব বর্ণ *প্রবেশ, size_t clen, বর্ণ **outp);
UNITTEST পূর্ণাঙ্ক dedotdotify(ধ্রুব বর্ণ *প্রবেশ, size_t clen, বর্ণ **outp)
{
  সংরচনা dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ধ্রুব বর্ণ *dinput = প্রবেশ;
  size_t dlen = clen;

  *outp = রিক্ত;
  যদি(!needs_dedotdot(প্রবেশ, clen))
    প্রত্যাগম 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  যদি(is_dot(&dinput, &dlen)) {
    যদি(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      প্রবেশ = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    অন্যথা যদি(is_dot(&dinput, &dlen)) {
      যদি(!dlen)
        /* .. [end] */
        গচ্ছ end;
      অন্যথা যদি(ISSLASH(*dinput)) {
        /* ../ */
        প্রবেশ = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  যাবত্(clen && !result) { /* until end of path content */
    যদি(ISSLASH(*প্রবেশ)) {
      ধ্রুব বর্ণ *p = &প্রবেশ[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      যদি(is_dot(&p, &blen)) {
        যদি(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          ভঙ্গ;
        }
        অন্যথা যদি(ISSLASH(*p)) { /* /./ */
          প্রবেশ = p;
          clen = blen;
          অনুবর্ত;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        অন্যথা যদি(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          যদি(len) {
            ধ্রুব বর্ণ *ptr = curlx_dyn_ptr(&out);
            ধ্রুব বর্ণ *last = memrchr(ptr, '/', len);
            যদি(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          যদি(blen) { /* /../ */
            প্রবেশ = p;
            clen = blen;
            অনুবর্ত;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          ভঙ্গ;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, প্রবেশ, 1);
    প্রবেশ++;
    clen--;
  }
end:
  যদি(!result) {
    যদি(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    অন্যথা {
      *outp = curlx_strdup("");
      যদি(!*outp)
        প্রত্যাগম 1;
    }
  }
  প্রত্যাগম result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ধ্রুব বর্ণ *url, size_t urllen, CURLU *u,
                              ধ্রুব বর্ণ **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ধ্রুব বর্ণ *url, size_t urllen, CURLU *u,
                              ধ্রুব বর্ণ **pathp, size_t *pathlenp)
{
  ধ্রুব বর্ণ *path;
  size_t pathlen;

  *pathp = রিক্ত;
  *pathlenp = 0;
  যদি(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    প্রত্যাগম CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  যদি(path[0] != '/')
    প্রত্যাগম CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  যদি(path[1] == '/') {
    /* swallow the two slashes */
    ধ্রুব বর্ণ *ptr = &path[2];

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
    যদি(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      যদি(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      অন্যথা
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        প্রত্যাগম CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#যদি !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  যদি(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    প্রত্যাগম CURLUE_BAD_FILE_URL;
  }
#অন্যথা
  /* If the path starts with a slash and a drive letter, ditch the slash */
  যদি('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  যদি(!u->scheme)
    প্রত্যাগম CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode parse_scheme(ধ্রুব বর্ণ *url, CURLU *u, বর্ণ *schemebuf,
                              size_t schemelen, unsigned পূর্ণাঙ্ক flags,
                              ধ্রুব বর্ণ **hostpp)
{
  /* clear path */
  ধ্রুব বর্ণ *schemep = রিক্ত;

  যদি(schemelen) {
    পূর্ণাঙ্ক অঙ্ক_slashes = 0;
    ধ্রুব বর্ণ *p = &url[schemelen + 1];
    যদি(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      প্রত্যাগম CURLUE_UNSUPPORTED_SCHEME;

    যদি(!ISSLASH(*p))
      /* less than one */
      প্রত্যাগম CURLUE_BAD_SLASHES;
    যদি((flags & CURLU_NO_AUTHORITY)) {
      যাবত্(ISSLASH(*p) && (অঙ্ক_slashes < 2)) {
        p++;
        অঙ্ক_slashes++;
      }
    }
    অন্যথা {
      যাবত্(ISSLASH(*p) && (অঙ্ক_slashes < 4)) {
        p++;
        অঙ্ক_slashes++;
      }
      যদি(অঙ্ক_slashes > 3)
        প্রত্যাগম CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  অন্যথা {
    /* no scheme! */

    যদি(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      প্রত্যাগম CURLUE_BAD_SCHEME;

    যদি(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  যদি(schemep) {
    u->scheme = curlx_strdup(schemep);
    যদি(!u->scheme)
      প্রত্যাগম CURLUE_OUT_OF_MEMORY;
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode guess_scheme(CURLU *u, সংরচনা dynbuf *host)
{
  ধ্রুব বর্ণ *hostname = curlx_dyn_ptr(host);
  ধ্রুব বর্ণ *schemep = রিক্ত;
  /* legacy curl-style guess based on hostname */
  যদি(checkprefix("ftp.", hostname))
    schemep = "ftp";
  অন্যথা যদি(checkprefix("dict.", hostname))
    schemep = "dict";
  অন্যথা যদি(checkprefix("ldap.", hostname))
    schemep = "ldap";
  অন্যথা যদি(checkprefix("imap.", hostname))
    schemep = "imap";
  অন্যথা যদি(checkprefix("smtp.", hostname))
    schemep = "smtp";
  অন্যথা যদি(checkprefix("pop3.", hostname))
    schemep = "pop3";
  অন্যথা
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  যদি(!u->scheme)
    প্রত্যাগম CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode handle_fragment(CURLU *u, ধ্রুব বর্ণ *fragment,
                                 size_t fraglen, unsigned পূর্ণাঙ্ক flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  যদি(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    যদি(flags & CURLU_URLENCODE) {
      সংরচনা dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_প্রবেশ_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      যদি(ures)
        প্রত্যাগম ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    অন্যথা {
      যদি(badoctets(fragment, fraglen, flags))
        প্রত্যাগম CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      যদি(!u->fragment)
        প্রত্যাগম CURLUE_OUT_OF_MEMORY;
    }
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode handle_query(CURLU *u, ধ্রুব বর্ণ *query,
                              size_t qlen, unsigned পূর্ণাঙ্ক flags)
{
  u->query_present = TRUE;
  যদি(qlen > 1) {
    যদি(flags & CURLU_URLENCODE) {
      সংরচনা dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_প্রবেশ_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      যদি(ures)
        প্রত্যাগম ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    অন্যথা {
      যদি(badoctets(query, qlen, flags))
        প্রত্যাগম CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      যদি(!u->query)
        প্রত্যাগম CURLUE_OUT_OF_MEMORY;
    }
  }
  অন্যথা {
    /* single byte query */
    u->query = curlx_strdup("");
    যদি(!u->query)
      প্রত্যাগম CURLUE_OUT_OF_MEMORY;
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode handle_path(CURLU *u, ধ্রুব বর্ণ *path,
                             size_t pathlen, unsigned পূর্ণাঙ্ক flags,
                             bool is_file)
{
  CURLUcode ures;
  যদি(pathlen && (flags & CURLU_URLENCODE)) {
    সংরচনা dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_প্রবেশ_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    যদি(ures)
      প্রত্যাগম ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  যদি(pathlen >= (size_t)(1 + !is_file)) {
    যদি(badoctets(path, pathlen, flags))
      প্রত্যাগম CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    যদি(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      যদি(!u->path)
        প্রত্যাগম CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    অন্যথা যদি(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    যদি(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      বর্ণ *dedot;
      পূর্ণাঙ্ক err = dedotdotify(path, pathlen, &dedot);
      যদি(err)
        প্রত্যাগম CURLUE_OUT_OF_MEMORY;
      যদি(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode parseurl(ধ্রুব বর্ণ *url, CURLU *u, unsigned পূর্ণাঙ্ক flags)
{
  ধ্রুব বর্ণ *path;
  size_t pathlen;
  বর্ণ schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  সংরচনা dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  যদি(urllen > CURL_MAX_প্রবেশ_LENGTH)
    প্রত্যাগম CURLUE_MALFORMED_প্রবেশ;

  curlx_dyn_init(&host, CURL_MAX_প্রবেশ_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, পরিমাণ(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  যদি(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  অন্যথা {
    ধ্রুব বর্ণ *hostp = রিক্ত;
    ধ্রুব বর্ণ *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    যদি(ures)
      গচ্ছ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    যাবত্(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    যদি(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      যদি(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    অন্যথা যদি(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      যদি(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    অন্যথা
      ures = CURLUE_NO_HOST;
  }
  যদি(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ধ্রুব বর্ণ *fragment = memchr(path, '#', pathlen);
    যদি(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  যদি(!ures) {
    ধ্রুব বর্ণ *query = memchr(path, '?', pathlen);
    যদি(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  যদি(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  যদি(!ures) {
    u->host = curlx_dyn_ptr(&host);
    প্রত্যাগম CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  প্রত্যাগম ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
স্থির CURLUcode parseurl_and_replace(ধ্রুব বর্ণ *url, CURLU *u,
                                      unsigned পূর্ণাঙ্ক flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, পরিমাণ(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  যদি(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  প্রত্যাগম ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
স্থির CURLUcode redirect_url(ধ্রুব বর্ণ *base, ধ্রুব বর্ণ *relurl,
                              CURLU *u, unsigned পূর্ণাঙ্ক flags)
{
  সংরচনা dynbuf urlbuf;
  bool host_changed = FALSE;
  ধ্রুব বর্ণ *useurl = relurl;
  ধ্রুব বর্ণ *cutoff = রিক্ত;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ধ্রুব বর্ণ *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ধ্রুব বর্ণ *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  যদি(!base)
    প্রত্যাগম CURLUE_MALFORMED_প্রবেশ; /* should never happen */

  /* handle different relative URL types */
  বিকল্প(relurl[0]) {
  পক্ষ '/':
    যদি(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    অন্যথা
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    ভঙ্গ;

  পক্ষ '#':
    /* fragment-only change */
    যদি(u->fragment_present)
      cutoff = strchr(protsep, '#');
    ভঙ্গ;

  স্বভাব:
    /* path or query-only change */
    যদি(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    অন্যথা যদি(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    যদি(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      যদি(cutoff)
        cutoff++; /* truncate after last slash */
    }
    ভঙ্গ;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_প্রবেশ_LENGTH);

  যদি(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  অন্যথা
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  প্রত্যাগম uc;
}

/*
 */
CURLU *curl_url(শূন্য)
{
  প্রত্যাগম curlx_calloc(1, পরিমাণ(সংরচনা Curl_URL));
}

শূন্য curl_url_cleanup(CURLU *u)
{
  যদি(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  কুরু {                                          \
    যদি((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      যদি(!(dest)->name)                         \
        গচ্ছ fail;                              \
    }                                           \
  } যাবত্(0)

CURLU *curl_url_dup(ধ্রুব CURLU *in)
{
  সংরচনা Curl_URL *u;

  যদি(!in)
    প্রত্যাগম রিক্ত;

  u = curlx_calloc(1, পরিমাণ(সংরচনা Curl_URL));
  যদি(u) {
    DUP(u, in, scheme);
    DUP(u, in, user);
    DUP(u, in, password);
    DUP(u, in, options);
    DUP(u, in, host);
    DUP(u, in, path);
    DUP(u, in, query);
    DUP(u, in, fragment);
    DUP(u, in, zoneid);
    u->portnum = in->portnum;
    u->port_present = in->port_present;
    u->fragment_present = in->fragment_present;
    u->query_present = in->query_present;
    u->guessed_scheme = in->guessed_scheme;
  }
  প্রত্যাগম u;
fail:
  curl_url_cleanup(u);
  প্রত্যাগম রিক্ত;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#অন্যথা
স্থির CURLUcode host_decode(ধ্রুব বর্ণ *host, বর্ণ **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  যদি(result)
    প্রত্যাগম (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode host_encode(ধ্রুব বর্ণ *host, বর্ণ **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  যদি(result)
    প্রত্যাগম (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  প্রত্যাগম CURLUE_OK;
}
#endif

স্থির CURLUcode urlget_format(ধ্রুব CURLU *u, CURLUPart what,
                               ধ্রুব বর্ণ *ptr, বর্ণ **partp,
                               bool plusdecode, unsigned পূর্ণাঙ্ক flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  বর্ণ *part = curlx_memdup0(ptr, partlen);
  *partp = রিক্ত;
  যদি(!part)
    প্রত্যাগম CURLUE_OUT_OF_MEMORY;
  যদি(plusdecode) {
    /* convert + to space */
    বর্ণ *plus = part;
    size_t i = 0;
    প্রতি(i = 0; i < partlen; ++plus, i++) {
      যদি(*plus == '+')
        *plus = ' ';
    }
  }
  যদি(urldecode) {
    বর্ণ *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    যদি(result)
      প্রত্যাগম CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  যদি(urlencode) {
    সংরচনা dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_প্রবেশ_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    যদি(uc)
      প্রত্যাগম uc;
    part = curlx_dyn_ptr(&enc);
  }
  অন্যথা যদি(punycode) {
    যদি(!Curl_is_ASCII_name(u->host)) {
      বর্ণ *punyversion = রিক্ত;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      যদি(uc)
        প্রত্যাগম uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      যদি(uc) {
        curlx_free(punyversion);
        প্রত্যাগম uc;
      }
      part = punyversion;
    }
  }
  অন্যথা যদি(depunyfy && Curl_is_ASCII_name(u->host)) {
    বর্ণ *unpunified = রিক্ত;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    যদি(uc)
      প্রত্যাগম uc;
    part = unpunified;
  }
  *partp = part;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode file_url(ধ্রুব CURLU *u, বর্ণ **part,
                          ধ্রুব বর্ণ *fragmentsep,
                          ধ্রুব বর্ণ *querysep)
{
  বর্ণ *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  যদি(!url)
    প্রত্যাগম CURLUE_OUT_OF_MEMORY;

  *part = url;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode urlget_url(ধ্রুব CURLU *u, বর্ণ **part, unsigned পূর্ণাঙ্ক flags)
{
  বর্ণ *url;
  বর্ণ *allochost = রিক্ত;
  ধ্রুব বর্ণ *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  ধ্রুব বর্ণ *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  বর্ণ portbuf[7];
  যদি(curl_strequal("file", u->scheme))
    প্রত্যাগম file_url(u, part, fragmentsep, querysep);
  অন্যথা যদি(!u->host)
    প্রত্যাগম CURLUE_NO_HOST;
  অন্যথা {
    ধ্রুব বর্ণ *scheme;
    বর্ণ *options = u->options;
    বর্ণ *port = রিক্ত;
    ধ্রুব সংরচনা Curl_scheme *h = রিক্ত;
    বর্ণ schemebuf[MAX_SCHEME_LEN + 5];
    যদি(u->scheme)
      scheme = u->scheme;
    অন্যথা যদি(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    অন্যথা
      প্রত্যাগম CURLUE_NO_SCHEME;

    যদি(u->port_present) {
      curl_msnprintf(portbuf, পরিমাণ(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    যদি(h) {
      যদি(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, পরিমাণ(portbuf), "%u", h->defport);
        port = portbuf;
      }
      অন্যথা যদি(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = রিক্ত;
      }

      যদি(!(h->flags & PROTOPT_URLOPTIONS))
        options = রিক্ত;
    }

    যদি(u->host[0] == '[') {
      যদি(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        সংরচনা dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_প্রবেশ_LENGTH);
        যদি(curlx_dyn_addf(&enc, "%.*s%%25%s]", (পূর্ণাঙ্ক)hostlen - 1, u->host,
                          u->zoneid))
          প্রত্যাগম CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    অন্যথা যদি(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(রিক্ত, u->host, 0);
      যদি(!allochost)
        প্রত্যাগম CURLUE_OUT_OF_MEMORY;
    }
    অন্যথা যদি(flags & CURLU_PUNYCODE) {
      যদি(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        যদি(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        যদি(ret) {
          curlx_safefree(allochost);
          প্রত্যাগম ret;
        }
      }
    }
    অন্যথা যদি(flags & CURLU_PUNY2IDN) {
      যদি(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        যদি(ret)
          প্রত্যাগম ret;
      }
    }

    যদি(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, পরিমাণ(schemebuf), "%s://", scheme);
    অন্যথা
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
                        port ? ":" : "",
                        port ? port : "",
                        u->path ? u->path : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  যদি(!url)
    প্রত্যাগম CURLUE_OUT_OF_MEMORY;
  *part = url;
  প্রত্যাগম CURLUE_OK;
}

CURLUcode curl_url_get(ধ্রুব CURLU *u, CURLUPart what,
                       বর্ণ **part, unsigned পূর্ণাঙ্ক flags)
{
  ধ্রুব বর্ণ *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  বর্ণ portbuf[7];
  bool plusdecode = FALSE;
  যদি(!u)
    প্রত্যাগম CURLUE_BAD_HANDLE;
  যদি(!part)
    প্রত্যাগম CURLUE_BAD_PARTPOINTER;
  *part = রিক্ত;

  বিকল্প(what) {
  পক্ষ CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    যদি((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      প্রত্যাগম CURLUE_NO_SCHEME;
    ভঙ্গ;
  পক্ষ CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    ভঙ্গ;
  পক্ষ CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    ভঙ্গ;
  পক্ষ CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    ভঙ্গ;
  পক্ষ CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    ভঙ্গ;
  পক্ষ CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    ভঙ্গ;
  পক্ষ CURLUPART_PORT:
    ptr = রিক্ত;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    যদি(u->port_present) {
      ধ্রুব সংরচনা Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : রিক্ত;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      যদি(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = রিক্ত;
      }
      অন্যথা {
        curl_msnprintf(portbuf, পরিমাণ(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    অন্যথা যদি((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ধ্রুব সংরচনা Curl_scheme *h = Curl_get_scheme(u->scheme);
      যদি(h) {
        curl_msnprintf(portbuf, পরিমাণ(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    ভঙ্গ;
  পক্ষ CURLUPART_PATH:
    ptr = u->path;
    যদি(!ptr)
      ptr = "/";
    ভঙ্গ;
  পক্ষ CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    যদি(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = রিক্ত;
    ভঙ্গ;
  পক্ষ CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    যদি(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    ভঙ্গ;
  পক্ষ CURLUPART_URL:
    প্রত্যাগম urlget_url(u, part, flags);
  স্বভাব:
    ptr = রিক্ত;
    ভঙ্গ;
  }
  যদি(ptr)
    প্রত্যাগম urlget_format(u, what, ptr, part, plusdecode, flags);

  প্রত্যাগম ifmissing;
}

স্থির CURLUcode set_url_scheme(CURLU *u, ধ্রুব বর্ণ *scheme,
                                unsigned পূর্ণাঙ্ক flags)
{
  size_t plen = strlen(scheme);
  ধ্রুব সংরচনা Curl_scheme *h = রিক্ত;
  যদি((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    প্রত্যাগম CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  যদি(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    প্রত্যাগম CURLUE_UNSUPPORTED_SCHEME;
  যদি(!h) {
    ধ্রুব বর্ণ *s = scheme;
    যদি(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      যাবত্(--plen) {
        যদি(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        অন্যথা
          প্রত্যাগম CURLUE_BAD_SCHEME;
      }
    }
    অন্যথা
      প্রত্যাগম CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode set_url_port(CURLU *u, ধ্রুব বর্ণ *provided_port)
{
  curl_off_t port;
  যদি(!ISDIGIT(provided_port[0]))
    /* not a number */
    প্রত্যাগম CURLUE_BAD_PORT_সংখ্যা;
  যদি(curlx_str_সংখ্যা(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    প্রত্যাগম CURLUE_BAD_PORT_সংখ্যা;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode set_url(CURLU *u, ধ্রুব বর্ণ *url, size_t part_size,
                         unsigned পূর্ণাঙ্ক flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  বর্ণ *oldurl = রিক্ত;

  যদি(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    যদি(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      প্রত্যাগম CURLUE_OK;
    }
    যদি(uc == CURLUE_OUT_OF_MEMORY)
      প্রত্যাগম uc;
    প্রত্যাগম CURLUE_MALFORMED_প্রবেশ;
  }

  /* if the new URL is absolute replace the existing with the new. */
  যদি(Curl_is_absolute_url(url, রিক্ত, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    প্রত্যাগম parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  যদি(uc == CURLUE_OUT_OF_MEMORY)
    প্রত্যাগম uc;
  অন্যথা যদি(uc)
    প্রত্যাগম parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  প্রত্যাগম uc;
}

স্থির CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  বিকল্প(what) {
  পক্ষ CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, পরিমাণ(সংরচনা Curl_URL));
    ভঙ্গ;
  পক্ষ CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    ভঙ্গ;
  পক্ষ CURLUPART_USER:
    curlx_safefree(u->user);
    ভঙ্গ;
  পক্ষ CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    ভঙ্গ;
  পক্ষ CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    ভঙ্গ;
  পক্ষ CURLUPART_HOST:
    curlx_safefree(u->host);
    ভঙ্গ;
  পক্ষ CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    ভঙ্গ;
  পক্ষ CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    ভঙ্গ;
  পক্ষ CURLUPART_PATH:
    curlx_safefree(u->path);
    ভঙ্গ;
  পক্ষ CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    ভঙ্গ;
  পক্ষ CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLUE_UNKNOWN_PART;
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির bool allowed_in_path(unsigned বর্ণ x)
{
  বিকল্প(x) {
  পক্ষ '!':
  পক্ষ '$':
  পক্ষ '&':
  পক্ষ '\'':
  পক্ষ '(':
  পক্ষ ')':
  পক্ষ '{':
  পক্ষ '}':
  পক্ষ '[':
  পক্ষ ']':
  পক্ষ '*':
  পক্ষ '+':
  পক্ষ ',':
  পক্ষ ';':
  পক্ষ '=':
  পক্ষ ':':
  পক্ষ '@':
  পক্ষ '/':
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

স্থির CURLUcode url_encode_part(সংরচনা dynbuf *encp,
                                 ধ্রুব বর্ণ *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ধ্রুব unsigned বর্ণ *i;

  প্রতি(i = (ধ্রুব unsigned বর্ণ *)part; *i; i++) {
    CURLcode result;
    যদি((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    অন্যথা যদি(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      যদি((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    অন্যথা {
      unsigned বর্ণ out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    যদি(result)
      প্রত্যাগম cc2cu(result);
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode url_uppercasehex_part(সংরচনা dynbuf *encp,
                                       ধ্রুব বর্ণ *part)
{
  বর্ণ *p;
  CURLcode result = curlx_dyn_add(encp, part);
  যদি(result)
    প্রত্যাগম cc2cu(result);
  p = curlx_dyn_ptr(encp);
  যাবত্(*p) {
    /* make sure percent encoded are upper case */
    যদি((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    অন্যথা
      p++;
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode url_append_query(CURLU *u, সংরচনা dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  যদি(querylen) {
    সংরচনা dynbuf qbuf;
    CURLcode result;
    ধ্রুব বর্ণ *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_প্রবেশ_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    যদি(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    যদি(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    যদি(result)
      গচ্ছ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    প্রত্যাগম CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    প্রত্যাগম cc2cu(result);
  }
  অন্যথা {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  প্রত্যাগম CURLUE_OK;
}

স্থির CURLUcode url_sethost(CURLU *u, সংরচনা dynbuf *encp,
                             bool urlencode,
                             unsigned পূর্ণাঙ্ক flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  বর্ণ *newp = curlx_dyn_ptr(encp);
  যদি(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  অন্যথা যদি(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    বর্ণ *decoded = রিক্ত;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    যদি(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    অন্যথা {
      সংরচনা dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_প্রবেশ_LENGTH);
      যদি(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      অন্যথা যদি(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        যদি(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  অন্যথা যদি(hostname_check6(u, newp, n))
    bad = TRUE;
  অন্যথা যদি(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  যদি(bad) {
    curlx_dyn_free(encp);
    প্রত্যাগম CURLUE_BAD_HOSTNAME;
  }
  প্রত্যাগম CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ধ্রুব বর্ণ *part, unsigned পূর্ণাঙ্ক flags)
{
  বর্ণ **storep = রিক্ত;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  যদি(!u)
    প্রত্যাগম CURLUE_BAD_HANDLE;
  যদি(!part)
    /* setting a part to NULL clears it */
    প্রত্যাগম urlset_clear(u, what);

  nalloc = strlen(part);
  যদি(nalloc > CURL_MAX_প্রবেশ_LENGTH)
    /* excessive input length */
    প্রত্যাগম CURLUE_MALFORMED_প্রবেশ;

  বিকল্প(what) {
  পক্ষ CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    যদি(status)
      প্রত্যাগম status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    ভঙ্গ;
  }
  পক্ষ CURLUPART_USER:
    storep = &u->user;
    ভঙ্গ;
  পক্ষ CURLUPART_PASSWORD:
    storep = &u->password;
    ভঙ্গ;
  পক্ষ CURLUPART_OPTIONS:
    storep = &u->options;
    ভঙ্গ;
  পক্ষ CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    ভঙ্গ;
  পক্ষ CURLUPART_ZONEID:
    storep = &u->zoneid;
    ভঙ্গ;
  পক্ষ CURLUPART_PORT:
    প্রত্যাগম set_url_port(u, part);
  পক্ষ CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    ভঙ্গ;
  পক্ষ CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    ভঙ্গ;
  পক্ষ CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    ভঙ্গ;
  পক্ষ CURLUPART_URL:
    প্রত্যাগম set_url(u, part, nalloc, flags);
  স্বভাব:
    প্রত্যাগম CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ধ্রুব বর্ণ *newp = রিক্ত;
    সংরচনা dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    যদি(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      যদি(result)
        প্রত্যাগম cc2cu(result);
    }
    যদি(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    অন্যথা
      status = url_uppercasehex_part(&enc, part);
    যদি(!status) {
      newp = curlx_dyn_ptr(&enc);

      যদি(appendquery && newp)
        প্রত্যাগম url_append_query(u, &enc);
      অন্যথা যদি(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    যদি(status)
      প্রত্যাগম status;

    যদি(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (বর্ণ *)CURL_UNCONST(newp);
  }
  প্রত্যাগম CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ধ্রুব সংরচনা Curl_scheme *s = রিক্ত;

  /* base must be an absolute URL */
  যদি(!base->scheme || !base->host)
    প্রত্যাগম FALSE;
  যদি(href->scheme && !curl_strequal(base->scheme, href->scheme))
    প্রত্যাগম FALSE;
  যদি(href->host) {
    যদি(!curl_strequal(base->host, href->host))
      প্রত্যাগম FALSE;

    যদি(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      যদি(!s) /* Cannot match default port for unknown scheme */
        প্রত্যাগম FALSE;
      /* to match, the present one must be the default port */
      যদি((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        প্রত্যাগম FALSE;
    }
    অন্যথা যদি(base->portnum != href->portnum) /* both present or missing */
      প্রত্যাগম FALSE;

    যদি(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      প্রত্যাগম FALSE;
  }
  অন্যথা যদি(href->port_present) /* no host in href, then there must be no port */
    প্রত্যাগম FALSE;
  প্রত্যাগম TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  যদি(u->port_present) {
    *pport = u->portnum;
    প্রত্যাগম CURLUE_OK;
  }
  অন্যথা যদি(u->scheme) {
    ধ্রুব সংরচনা Curl_scheme *s = Curl_get_scheme(u->scheme);
    যদি(s && s->defport) {
      *pport = s->defport;
      প্রত্যাগম CURLUE_OK;
    }
  }
  *pport = 0;
  প্রত্যাগম CURLUE_NO_PORT;
}
