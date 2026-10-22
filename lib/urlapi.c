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
#𐤀𐤌 !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

ساكن فراغ free_urlhandle(بنية Curl_URL *u)
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
ساكن ثابت حرف *find_host_sep(ثابت حرف *url)
{
  /* Find the start of the hostname */
  ثابت حرف *sep = strstr(url, "//");
  𐤀𐤌(!sep)
    sep = url;
  وإلا
    sep += 2;

  /* Find first / or ? */
  طالما(*sep && *sep != '/' && *sep != '?')
    sep++;

  أرجع sep;
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
UNITTEST CURLUcode urlencode_str(بنية dynbuf *o, ثابت حرف *url,
                                 size_t len, bool relative,
                                 unsigned صحيح query);
UNITTEST CURLUcode urlencode_str(بنية dynbuf *o, ثابت حرف *url,
                                 size_t len, bool relative,
                                 unsigned صحيح query)
{
  /* we must add this with whitespace-replacing */
  ثابت unsigned حرف *iptr;
  ثابت unsigned حرف *host_sep = (ثابت unsigned حرف *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  𐤀𐤌(!relative) {
    size_t n;
    host_sep = (ثابت unsigned حرف *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ثابت حرف *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  لكل(iptr = host_sep; len && !result;) {
    𐤀𐤌(*iptr == ' ') {
      𐤀𐤌(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      وإلا
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    وإلا 𐤀𐤌((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned حرف out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    وإلا 𐤀𐤌(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned حرف hex = (unsigned حرف)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned حرف out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    وإلا {
      ثابت unsigned حرف *start = iptr;
      طالما(len) {
        𐤀𐤌(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          اكسر;
        𐤀𐤌(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          اكسر;
        𐤀𐤌(*iptr == '?') {
          𐤀𐤌(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            اكسر;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ثابت حرف *)start, (size_t)(iptr - start));
    }
  }

  𐤀𐤌(result)
    أرجع cc2cu(result);
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
  𐤀𐤌(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  𐤀𐤌(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    أرجع 0;
#endif
  𐤀𐤌(ISALPHA(url[0])) {
    𐤀𐤌(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    لكل(i = 1; i < MAX_SCHEME_LEN; ++i) {
      حرف s = url[i];
      𐤀𐤌(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        𐤀𐤌(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      وإلا {
        اكسر;
      }
    }
  }
  𐤀𐤌(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    𐤀𐤌(buf)
      buf[i] = 0;
    أرجع len;
  }
  𐤀𐤌(buf)
    buf[0] = 0;
  أرجع 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ساكن bool badoctets(ثابت حرف *input, size_t n, صحيح flags)
{
  ثابت uint8_t *p = (ثابت unsigned حرف *)input;
  ثابت uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  طالما(n--) {
    𐤀𐤌(*p <= control || *p == 127)
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
                                        unsigned صحيح flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(بنية Curl_URL *u,
                                        ثابت حرف *login,
                                        size_t len,
                                        unsigned صحيح flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
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

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  𐤀𐤌(!ptr)
    اذهب out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  𐤀𐤌(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : عدم);
  𐤀𐤌(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    اذهب out;
  }

  𐤀𐤌(userp) {
    𐤀𐤌(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      اذهب out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  𐤀𐤌(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  𐤀𐤌(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  𐤀𐤌(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  وإلا 𐤀𐤌(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  وإلا 𐤀𐤌(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_INPUT;

  userp = passwdp = optionsp = عدم;

  𐤀𐤌(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    أرجع CURLUE_OK;
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

  أرجع ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(بنية Curl_URL *u, بنية dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(بنية Curl_URL *u, بنية dynbuf *host,
                              bool has_scheme)
{
  ثابت حرف *portptr;
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  𐤀𐤌(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    𐤀𐤌(!portptr)
      أرجع CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    𐤀𐤌(*portptr) {
      𐤀𐤌(*portptr != ':')
        أرجع CURLUE_BAD_PORT_NUMBER;
    }
    وإلا
      portptr = عدم;
  }
  وإلا
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  𐤀𐤌(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    صحيح rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    𐤀𐤌(!*portptr)
      أرجع has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_NUMBER;
    𐤀𐤌(*portptr == '\\')
      أرجع CURLUE_BACKSLASH;
    rc = curlx_str_number(&portptr, &port, 0xffff);
    𐤀𐤌(rc)
      أرجع CURLUE_BAD_PORT_NUMBER;
    وإلا 𐤀𐤌(*portptr == '\\')
      أرجع CURLUE_BACKSLASH;
    وإلا 𐤀𐤌(*portptr)
      أرجع CURLUE_BAD_PORT_NUMBER;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
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
  𐤀𐤌(hlen < 4) /* '[::]' is the shortest possible valid string */
    أرجع CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  𐤀𐤌(hlen != len) {
    hlen = len;
    𐤀𐤌(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      حرف zoneid[MAX_ZONEID_LEN];
      صحيح i = 0;
      حرف *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      𐤀𐤌(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      طالما(!invalid_host_char[(unsigned حرف)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      𐤀𐤌(!i || (']' != *h))
        أرجع CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      𐤀𐤌(!u->zoneid)
        أرجع CURLUE_OUT_OF_MEMORY;
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
    𐤀𐤌(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      أرجع CURLUE_BAD_IPV6;
    𐤀𐤌(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
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
    𐤀𐤌(invalid_host_char[(unsigned حرف)hostname[i]])
      أرجع CURLUE_BAD_HOSTNAME;
  }
  𐤀𐤌((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    أرجع CURLUE_BAD_HOSTNAME;
  وإلا 𐤀𐤌((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    أرجع CURLUE_BAD_HOSTNAME;
  أرجع CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ساكن CURLUcode hostname_check6(بنية Curl_URL *u, حرف *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  𐤀𐤌(!hlen)
    أرجع CURLUE_NO_HOST;
  وإلا 𐤀𐤌(hostname[0] == '[')
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
  bool done = FALSE;
  صحيح n = 0;
  ثابت حرف *c = curlx_dyn_ptr(host);
  unsigned صحيح parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  𐤀𐤌(!ISDIGIT(*c))
    أرجع HOST_NAME;

  طالما(!done) {
    صحيح rc;
    curl_off_t l;
    𐤀𐤌(*c == '0') {
      𐤀𐤌((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        𐤀𐤌(rc)
          أرجع HOST_NAME;
      }
      وإلا
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    وإلا
      rc = curlx_str_number(&c, &l, UINT_MAX);

    𐤀𐤌(rc) {
      𐤀𐤌(!n || (rc != STRE_NO_NUM) || *c)
        أرجع HOST_NAME;
      n--;
    }
    وإلا
      parts[n] = (unsigned صحيح)l;

    بدل(*c) {
    حالة '.':
      𐤀𐤌(n == 3) {
        𐤀𐤌(c[1])
          /* something follows this dot */
          أرجع HOST_NAME;
        done = TRUE;
      }
      وإلا {
        n++;
        c++;
      }
      اكسر;

    حالة '\0':
      done = TRUE;
      اكسر;

    افتراضي:
      أرجع HOST_NAME;
    }
  }

  بدل(n) {
  حالة 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    اكسر;
  حالة 1: /* a.b -- 8.24 bits */
    𐤀𐤌((parts[0] > 0xff) || (parts[1] > 0xffffff))
      أرجع HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    اكسر;
  حالة 2: /* a.b.c -- 8.8.16 bits */
    𐤀𐤌((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      أرجع HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    اكسر;
  حالة 3: /* a.b.c.d -- 8.8.8.8 bits */
    𐤀𐤌((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      أرجع HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    اكسر;
  }
  𐤀𐤌(result)
    أرجع HOST_ERROR;
  أرجع HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ساكن CURLUcode urldecode_host(بنية dynbuf *host)
{
  ثابت حرف *per;
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  𐤀𐤌(!per)
    /* nothing to decode */
    أرجع CURLUE_OK;
  وإلا {
    /* encoded */
    size_t dlen;
    حرف *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    𐤀𐤌(result)
      أرجع CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    𐤀𐤌(result)
      أرجع cc2cu(result);
  }

  أرجع CURLUE_OK;
}

ساكن CURLUcode parse_authority(بنية Curl_URL *u,
                                 ثابت حرف *auth, size_t authlen,
                                 unsigned صحيح flags,
                                 بنية dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  𐤀𐤌(uc)
    أرجع uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  𐤀𐤌(result) {
    uc = cc2cu(result);
    أرجع uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  𐤀𐤌(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  𐤀𐤌(!uc)
    uc = urldecode_host(host);
  𐤀𐤌(uc)
    ;
  وإلا 𐤀𐤌(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  وإلا {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    صحيح type = ipv4_normalize(host);

    𐤀𐤌(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    وإلا 𐤀𐤌(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  أرجع uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ثابت حرف *authority)
{
  CURLUcode ures;
  بنية dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  𐤀𐤌(ures)
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
  𐤀𐤌(*p == '.') {
    (*str)++;
    (*clen)--;
    أرجع TRUE;
  }
  وإلا 𐤀𐤌((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    أرجع TRUE;
  }
  أرجع FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ساكن bool needs_dedotdot(ثابت حرف *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  𐤀𐤌(pn < 2)
    أرجع FALSE;
  𐤀𐤌(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    أرجع FALSE;
  طالما(pn) {
    𐤀𐤌(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      𐤀𐤌(!pn || ISSLASH(*p))
        أرجع TRUE;
      /* "../" or ".." before end of string */
      وإلا 𐤀𐤌(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
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
UNITTEST صحيح dedotdotify(ثابت حرف *input, size_t clen, حرف **outp);
UNITTEST صحيح dedotdotify(ثابت حرف *input, size_t clen, حرف **outp)
{
  بنية dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ثابت حرف *dinput = input;
  size_t dlen = clen;

  *outp = عدم;
  𐤀𐤌(!needs_dedotdot(input, clen))
    أرجع 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  𐤀𐤌(is_dot(&dinput, &dlen)) {
    𐤀𐤌(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      input = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    وإلا 𐤀𐤌(is_dot(&dinput, &dlen)) {
      𐤀𐤌(!dlen)
        /* .. [end] */
        اذهب end;
      وإلا 𐤀𐤌(ISSLASH(*dinput)) {
        /* ../ */
        input = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  طالما(clen && !result) { /* until end of path content */
    𐤀𐤌(ISSLASH(*input)) {
      ثابت حرف *p = &input[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      𐤀𐤌(is_dot(&p, &blen)) {
        𐤀𐤌(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          اكسر;
        }
        وإلا 𐤀𐤌(ISSLASH(*p)) { /* /./ */
          input = p;
          clen = blen;
          استمر;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        وإلا 𐤀𐤌(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          𐤀𐤌(len) {
            ثابت حرف *ptr = curlx_dyn_ptr(&out);
            ثابت حرف *last = memrchr(ptr, '/', len);
            𐤀𐤌(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          𐤀𐤌(blen) { /* /../ */
            input = p;
            clen = blen;
            استمر;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          اكسر;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, input, 1);
    input++;
    clen--;
  }
end:
  𐤀𐤌(!result) {
    𐤀𐤌(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    وإلا {
      *outp = curlx_strdup("");
      𐤀𐤌(!*outp)
        أرجع 1;
    }
  }
  أرجع result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ثابت حرف *url, size_t urllen, CURLU *u,
                              ثابت حرف **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ثابت حرف *url, size_t urllen, CURLU *u,
                              ثابت حرف **pathp, size_t *pathlenp)
{
  ثابت حرف *path;
  size_t pathlen;

  *pathp = عدم;
  *pathlenp = 0;
  𐤀𐤌(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    أرجع CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  𐤀𐤌(path[0] != '/')
    أرجع CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  𐤀𐤌(path[1] == '/') {
    /* swallow the two slashes */
    ثابت حرف *ptr = &path[2];

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
    𐤀𐤌(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      𐤀𐤌(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      وإلا
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        أرجع CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#𐤀𐤌 !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  𐤀𐤌(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    أرجع CURLUE_BAD_FILE_URL;
  }
#وإلا
  /* If the path starts with a slash and a drive letter, ditch the slash */
  𐤀𐤌('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  𐤀𐤌(!u->scheme)
    أرجع CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  أرجع CURLUE_OK;
}

ساكن CURLUcode parse_scheme(ثابت حرف *url, CURLU *u, حرف *schemebuf,
                              size_t schemelen, unsigned صحيح flags,
                              ثابت حرف **hostpp)
{
  /* clear path */
  ثابت حرف *schemep = عدم;

  𐤀𐤌(schemelen) {
    صحيح num_slashes = 0;
    ثابت حرف *p = &url[schemelen + 1];
    𐤀𐤌(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      أرجع CURLUE_UNSUPPORTED_SCHEME;

    𐤀𐤌(!ISSLASH(*p))
      /* less than one */
      أرجع CURLUE_BAD_SLASHES;
    𐤀𐤌((flags & CURLU_NO_AUTHORITY)) {
      طالما(ISSLASH(*p) && (num_slashes < 2)) {
        p++;
        num_slashes++;
      }
    }
    وإلا {
      طالما(ISSLASH(*p) && (num_slashes < 4)) {
        p++;
        num_slashes++;
      }
      𐤀𐤌(num_slashes > 3)
        أرجع CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  وإلا {
    /* no scheme! */

    𐤀𐤌(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      أرجع CURLUE_BAD_SCHEME;

    𐤀𐤌(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  𐤀𐤌(schemep) {
    u->scheme = curlx_strdup(schemep);
    𐤀𐤌(!u->scheme)
      أرجع CURLUE_OUT_OF_MEMORY;
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode guess_scheme(CURLU *u, بنية dynbuf *host)
{
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  ثابت حرف *schemep = عدم;
  /* legacy curl-style guess based on hostname */
  𐤀𐤌(checkprefix("ftp.", hostname))
    schemep = "ftp";
  وإلا 𐤀𐤌(checkprefix("dict.", hostname))
    schemep = "dict";
  وإلا 𐤀𐤌(checkprefix("ldap.", hostname))
    schemep = "ldap";
  وإلا 𐤀𐤌(checkprefix("imap.", hostname))
    schemep = "imap";
  وإلا 𐤀𐤌(checkprefix("smtp.", hostname))
    schemep = "smtp";
  وإلا 𐤀𐤌(checkprefix("pop3.", hostname))
    schemep = "pop3";
  وإلا
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  𐤀𐤌(!u->scheme)
    أرجع CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  أرجع CURLUE_OK;
}

ساكن CURLUcode handle_fragment(CURLU *u, ثابت حرف *fragment,
                                 size_t fraglen, unsigned صحيح flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  𐤀𐤌(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    𐤀𐤌(flags & CURLU_URLENCODE) {
      بنية dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      𐤀𐤌(ures)
        أرجع ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    وإلا {
      𐤀𐤌(badoctets(fragment, fraglen, flags))
        أرجع CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      𐤀𐤌(!u->fragment)
        أرجع CURLUE_OUT_OF_MEMORY;
    }
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode handle_query(CURLU *u, ثابت حرف *query,
                              size_t qlen, unsigned صحيح flags)
{
  u->query_present = TRUE;
  𐤀𐤌(qlen > 1) {
    𐤀𐤌(flags & CURLU_URLENCODE) {
      بنية dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      𐤀𐤌(ures)
        أرجع ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    وإلا {
      𐤀𐤌(badoctets(query, qlen, flags))
        أرجع CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      𐤀𐤌(!u->query)
        أرجع CURLUE_OUT_OF_MEMORY;
    }
  }
  وإلا {
    /* single byte query */
    u->query = curlx_strdup("");
    𐤀𐤌(!u->query)
      أرجع CURLUE_OUT_OF_MEMORY;
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode handle_path(CURLU *u, ثابت حرف *path,
                             size_t pathlen, unsigned صحيح flags,
                             bool is_file)
{
  CURLUcode ures;
  𐤀𐤌(pathlen && (flags & CURLU_URLENCODE)) {
    بنية dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    𐤀𐤌(ures)
      أرجع ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  𐤀𐤌(pathlen >= (size_t)(1 + !is_file)) {
    𐤀𐤌(badoctets(path, pathlen, flags))
      أرجع CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    𐤀𐤌(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      𐤀𐤌(!u->path)
        أرجع CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    وإلا 𐤀𐤌(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    𐤀𐤌(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      حرف *dedot;
      صحيح err = dedotdotify(path, pathlen, &dedot);
      𐤀𐤌(err)
        أرجع CURLUE_OUT_OF_MEMORY;
      𐤀𐤌(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode parseurl(ثابت حرف *url, CURLU *u, unsigned صحيح flags)
{
  ثابت حرف *path;
  size_t pathlen;
  حرف schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  بنية dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  𐤀𐤌(urllen > CURL_MAX_INPUT_LENGTH)
    أرجع CURLUE_MALFORMED_INPUT;

  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, حجم(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  𐤀𐤌(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  وإلا {
    ثابت حرف *hostp = عدم;
    ثابت حرف *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    𐤀𐤌(ures)
      اذهب fail;

    /* find the end of the hostname + port number */
    p = hostp;
    طالما(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    𐤀𐤌(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      𐤀𐤌(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    وإلا 𐤀𐤌(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      𐤀𐤌(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    وإلا
      ures = CURLUE_NO_HOST;
  }
  𐤀𐤌(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ثابت حرف *fragment = memchr(path, '#', pathlen);
    𐤀𐤌(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  𐤀𐤌(!ures) {
    ثابت حرف *query = memchr(path, '?', pathlen);
    𐤀𐤌(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  𐤀𐤌(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  𐤀𐤌(!ures) {
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
                                      unsigned صحيح flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, حجم(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  𐤀𐤌(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  أرجع ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ساكن CURLUcode redirect_url(ثابت حرف *base, ثابت حرف *relurl,
                              CURLU *u, unsigned صحيح flags)
{
  بنية dynbuf urlbuf;
  bool host_changed = FALSE;
  ثابت حرف *useurl = relurl;
  ثابت حرف *cutoff = عدم;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ثابت حرف *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ثابت حرف *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  𐤀𐤌(!base)
    أرجع CURLUE_MALFORMED_INPUT; /* should never happen */

  /* handle different relative URL types */
  بدل(relurl[0]) {
  حالة '/':
    𐤀𐤌(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    وإلا
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    اكسر;

  حالة '#':
    /* fragment-only change */
    𐤀𐤌(u->fragment_present)
      cutoff = strchr(protsep, '#');
    اكسر;

  افتراضي:
    /* path or query-only change */
    𐤀𐤌(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    وإلا 𐤀𐤌(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    𐤀𐤌(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      𐤀𐤌(cutoff)
        cutoff++; /* truncate after last slash */
    }
    اكسر;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_INPUT_LENGTH);

  𐤀𐤌(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  وإلا
    uc = CURLUE_OUT_OF_MEMORY;

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
  𐤀𐤌(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  𐤐𐤏𐤋 {                                          \
    𐤀𐤌((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      𐤀𐤌(!(dest)->name)                         \
        اذهب fail;                              \
    }                                           \
  } طالما(0)

CURLU *curl_url_dup(ثابت CURLU *in)
{
  بنية Curl_URL *u;

  𐤀𐤌(!in)
    أرجع عدم;

  u = curlx_calloc(1, حجم(بنية Curl_URL));
  𐤀𐤌(u) {
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
  أرجع u;
fail:
  curl_url_cleanup(u);
  أرجع عدم;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#وإلا
ساكن CURLUcode host_decode(ثابت حرف *host, حرف **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  𐤀𐤌(result)
    أرجع (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  أرجع CURLUE_OK;
}

ساكن CURLUcode host_encode(ثابت حرف *host, حرف **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  𐤀𐤌(result)
    أرجع (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  أرجع CURLUE_OK;
}
#endif

ساكن CURLUcode urlget_format(ثابت CURLU *u, CURLUPart what,
                               ثابت حرف *ptr, حرف **partp,
                               bool plusdecode, unsigned صحيح flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  حرف *part = curlx_memdup0(ptr, partlen);
  *partp = عدم;
  𐤀𐤌(!part)
    أرجع CURLUE_OUT_OF_MEMORY;
  𐤀𐤌(plusdecode) {
    /* convert + to space */
    حرف *plus = part;
    size_t i = 0;
    لكل(i = 0; i < partlen; ++plus, i++) {
      𐤀𐤌(*plus == '+')
        *plus = ' ';
    }
  }
  𐤀𐤌(urldecode) {
    حرف *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    𐤀𐤌(result)
      أرجع CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  𐤀𐤌(urlencode) {
    بنية dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    𐤀𐤌(uc)
      أرجع uc;
    part = curlx_dyn_ptr(&enc);
  }
  وإلا 𐤀𐤌(punycode) {
    𐤀𐤌(!Curl_is_ASCII_name(u->host)) {
      حرف *punyversion = عدم;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      𐤀𐤌(uc)
        أرجع uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      𐤀𐤌(uc) {
        curlx_free(punyversion);
        أرجع uc;
      }
      part = punyversion;
    }
  }
  وإلا 𐤀𐤌(depunyfy && Curl_is_ASCII_name(u->host)) {
    حرف *unpunified = عدم;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    𐤀𐤌(uc)
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
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  𐤀𐤌(!url)
    أرجع CURLUE_OUT_OF_MEMORY;

  *part = url;
  أرجع CURLUE_OK;
}

ساكن CURLUcode urlget_url(ثابت CURLU *u, حرف **part, unsigned صحيح flags)
{
  حرف *url;
  حرف *allochost = عدم;
  ثابت حرف *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  ثابت حرف *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  حرف portbuf[7];
  𐤀𐤌(curl_strequal("file", u->scheme))
    أرجع file_url(u, part, fragmentsep, querysep);
  وإلا 𐤀𐤌(!u->host)
    أرجع CURLUE_NO_HOST;
  وإلا {
    ثابت حرف *scheme;
    حرف *options = u->options;
    حرف *port = عدم;
    ثابت بنية Curl_scheme *h = عدم;
    حرف schemebuf[MAX_SCHEME_LEN + 5];
    𐤀𐤌(u->scheme)
      scheme = u->scheme;
    وإلا 𐤀𐤌(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    وإلا
      أرجع CURLUE_NO_SCHEME;

    𐤀𐤌(u->port_present) {
      curl_msnprintf(portbuf, حجم(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    𐤀𐤌(h) {
      𐤀𐤌(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, حجم(portbuf), "%u", h->defport);
        port = portbuf;
      }
      وإلا 𐤀𐤌(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = عدم;
      }

      𐤀𐤌(!(h->flags & PROTOPT_URLOPTIONS))
        options = عدم;
    }

    𐤀𐤌(u->host[0] == '[') {
      𐤀𐤌(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        بنية dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
        𐤀𐤌(curlx_dyn_addf(&enc, "%.*s%%25%s]", (صحيح)hostlen - 1, u->host,
                          u->zoneid))
          أرجع CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    وإلا 𐤀𐤌(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(عدم, u->host, 0);
      𐤀𐤌(!allochost)
        أرجع CURLUE_OUT_OF_MEMORY;
    }
    وإلا 𐤀𐤌(flags & CURLU_PUNYCODE) {
      𐤀𐤌(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        𐤀𐤌(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        𐤀𐤌(ret) {
          curlx_safefree(allochost);
          أرجع ret;
        }
      }
    }
    وإلا 𐤀𐤌(flags & CURLU_PUNY2IDN) {
      𐤀𐤌(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        𐤀𐤌(ret)
          أرجع ret;
      }
    }

    𐤀𐤌(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
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
                        port ? ":" : "",
                        port ? port : "",
                        u->path ? u->path : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  𐤀𐤌(!url)
    أرجع CURLUE_OUT_OF_MEMORY;
  *part = url;
  أرجع CURLUE_OK;
}

CURLUcode curl_url_get(ثابت CURLU *u, CURLUPart what,
                       حرف **part, unsigned صحيح flags)
{
  ثابت حرف *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  حرف portbuf[7];
  bool plusdecode = FALSE;
  𐤀𐤌(!u)
    أرجع CURLUE_BAD_HANDLE;
  𐤀𐤌(!part)
    أرجع CURLUE_BAD_PARTPOINTER;
  *part = عدم;

  بدل(what) {
  حالة CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    𐤀𐤌((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
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
  حالة CURLUPART_PORT:
    ptr = عدم;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    𐤀𐤌(u->port_present) {
      ثابت بنية Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : عدم;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      𐤀𐤌(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = عدم;
      }
      وإلا {
        curl_msnprintf(portbuf, حجم(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    وإلا 𐤀𐤌((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ثابت بنية Curl_scheme *h = Curl_get_scheme(u->scheme);
      𐤀𐤌(h) {
        curl_msnprintf(portbuf, حجم(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    اكسر;
  حالة CURLUPART_PATH:
    ptr = u->path;
    𐤀𐤌(!ptr)
      ptr = "/";
    اكسر;
  حالة CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    𐤀𐤌(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = عدم;
    اكسر;
  حالة CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    𐤀𐤌(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    اكسر;
  حالة CURLUPART_URL:
    أرجع urlget_url(u, part, flags);
  افتراضي:
    ptr = عدم;
    اكسر;
  }
  𐤀𐤌(ptr)
    أرجع urlget_format(u, what, ptr, part, plusdecode, flags);

  أرجع ifmissing;
}

ساكن CURLUcode set_url_scheme(CURLU *u, ثابت حرف *scheme,
                                unsigned صحيح flags)
{
  size_t plen = strlen(scheme);
  ثابت بنية Curl_scheme *h = عدم;
  𐤀𐤌((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    أرجع CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  𐤀𐤌(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    أرجع CURLUE_UNSUPPORTED_SCHEME;
  𐤀𐤌(!h) {
    ثابت حرف *s = scheme;
    𐤀𐤌(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      طالما(--plen) {
        𐤀𐤌(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
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

ساكن CURLUcode set_url_port(CURLU *u, ثابت حرف *provided_port)
{
  curl_off_t port;
  𐤀𐤌(!ISDIGIT(provided_port[0]))
    /* not a number */
    أرجع CURLUE_BAD_PORT_NUMBER;
  𐤀𐤌(curlx_str_number(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    أرجع CURLUE_BAD_PORT_NUMBER;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  أرجع CURLUE_OK;
}

ساكن CURLUcode set_url(CURLU *u, ثابت حرف *url, size_t part_size,
                         unsigned صحيح flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  حرف *oldurl = عدم;

  𐤀𐤌(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    𐤀𐤌(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      أرجع CURLUE_OK;
    }
    𐤀𐤌(uc == CURLUE_OUT_OF_MEMORY)
      أرجع uc;
    أرجع CURLUE_MALFORMED_INPUT;
  }

  /* if the new URL is absolute replace the existing with the new. */
  𐤀𐤌(Curl_is_absolute_url(url, عدم, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    أرجع parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  𐤀𐤌(uc == CURLUE_OUT_OF_MEMORY)
    أرجع uc;
  وإلا 𐤀𐤌(uc)
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
  حالة CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    اكسر;
  حالة CURLUPART_PATH:
    curlx_safefree(u->path);
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

ساكن bool allowed_in_path(unsigned حرف x)
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
  ثابت unsigned حرف *i;

  لكل(i = (ثابت unsigned حرف *)part; *i; i++) {
    CURLcode result;
    𐤀𐤌((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    وإلا 𐤀𐤌(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      𐤀𐤌((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    وإلا {
      unsigned حرف out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    𐤀𐤌(result)
      أرجع cc2cu(result);
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode url_uppercasehex_part(بنية dynbuf *encp,
                                       ثابت حرف *part)
{
  حرف *p;
  CURLcode result = curlx_dyn_add(encp, part);
  𐤀𐤌(result)
    أرجع cc2cu(result);
  p = curlx_dyn_ptr(encp);
  طالما(*p) {
    /* make sure percent encoded are upper case */
    𐤀𐤌((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
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

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  𐤀𐤌(querylen) {
    بنية dynbuf qbuf;
    CURLcode result;
    ثابت حرف *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_INPUT_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    𐤀𐤌(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    𐤀𐤌(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    𐤀𐤌(result)
      اذهب nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    أرجع CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    أرجع cc2cu(result);
  }
  وإلا {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  أرجع CURLUE_OK;
}

ساكن CURLUcode url_sethost(CURLU *u, بنية dynbuf *encp,
                             bool urlencode,
                             unsigned صحيح flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  حرف *newp = curlx_dyn_ptr(encp);
  𐤀𐤌(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  وإلا 𐤀𐤌(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    حرف *decoded = عدم;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    𐤀𐤌(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    وإلا {
      بنية dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_INPUT_LENGTH);
      𐤀𐤌(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      وإلا 𐤀𐤌(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        𐤀𐤌(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  وإلا 𐤀𐤌(hostname_check6(u, newp, n))
    bad = TRUE;
  وإلا 𐤀𐤌(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  𐤀𐤌(bad) {
    curlx_dyn_free(encp);
    أرجع CURLUE_BAD_HOSTNAME;
  }
  أرجع CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ثابت حرف *part, unsigned صحيح flags)
{
  حرف **storep = عدم;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  𐤀𐤌(!u)
    أرجع CURLUE_BAD_HANDLE;
  𐤀𐤌(!part)
    /* setting a part to NULL clears it */
    أرجع urlset_clear(u, what);

  nalloc = strlen(part);
  𐤀𐤌(nalloc > CURL_MAX_INPUT_LENGTH)
    /* excessive input length */
    أرجع CURLUE_MALFORMED_INPUT;

  بدل(what) {
  حالة CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    𐤀𐤌(status)
      أرجع status;
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
  حالة CURLUPART_PORT:
    أرجع set_url_port(u, part);
  حالة CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
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
    أرجع set_url(u, part, nalloc, flags);
  افتراضي:
    أرجع CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ثابت حرف *newp = عدم;
    بنية dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    𐤀𐤌(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      𐤀𐤌(result)
        أرجع cc2cu(result);
    }
    𐤀𐤌(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    وإلا
      status = url_uppercasehex_part(&enc, part);
    𐤀𐤌(!status) {
      newp = curlx_dyn_ptr(&enc);

      𐤀𐤌(appendquery && newp)
        أرجع url_append_query(u, &enc);
      وإلا 𐤀𐤌(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    𐤀𐤌(status)
      أرجع status;

    𐤀𐤌(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (حرف *)CURL_UNCONST(newp);
  }
  أرجع CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ثابت بنية Curl_scheme *s = عدم;

  /* base must be an absolute URL */
  𐤀𐤌(!base->scheme || !base->host)
    أرجع FALSE;
  𐤀𐤌(href->scheme && !curl_strequal(base->scheme, href->scheme))
    أرجع FALSE;
  𐤀𐤌(href->host) {
    𐤀𐤌(!curl_strequal(base->host, href->host))
      أرجع FALSE;

    𐤀𐤌(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      𐤀𐤌(!s) /* Cannot match default port for unknown scheme */
        أرجع FALSE;
      /* to match, the present one must be the default port */
      𐤀𐤌((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        أرجع FALSE;
    }
    وإلا 𐤀𐤌(base->portnum != href->portnum) /* both present or missing */
      أرجع FALSE;

    𐤀𐤌(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      أرجع FALSE;
  }
  وإلا 𐤀𐤌(href->port_present) /* no host in href, then there must be no port */
    أرجع FALSE;
  أرجع TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  𐤀𐤌(u->port_present) {
    *pport = u->portnum;
    أرجع CURLUE_OK;
  }
  وإلا 𐤀𐤌(u->scheme) {
    ثابت بنية Curl_scheme *s = Curl_get_scheme(u->scheme);
    𐤀𐤌(s && s->defport) {
      *pport = s->defport;
      أرجع CURLUE_OK;
    }
  }
  *pport = 0;
  أرجع CURLUE_NO_PORT;
}
