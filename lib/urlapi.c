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

#شامل "urldata.h"
#شامل "urlapi-int.h"
#شامل "strcase.h"
#شامل "url.h"
#شامل "escape.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/inet_ntop.h"
#شامل "curlx/strdup.h"
#شامل "idn.h"
#شامل "curlx/strparse.h"
#شامل "curl_memrchr.h"

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
#اگر !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

ایستا تهی free_urlhandle(ساختار Curl_URL *u)
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
ایستا ثابت نویسه *find_host_sep(ثابت نویسه *url)
{
  /* Find the start of the hostname */
  ثابت نویسه *sep = strstr(url, "//");
  اگر(!sep)
    sep = url;
  وگرنه
    sep += 2;

  /* Find first / or ? */
  وقتی(*sep && *sep != '/' && *sep != '?')
    sep++;

  بازگشت sep;
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
UNITTEST CURLUcode urlencode_str(ساختار dynbuf *o, ثابت نویسه *url,
                                 size_t len, bool relative,
                                 unsigned صحیح query);
UNITTEST CURLUcode urlencode_str(ساختار dynbuf *o, ثابت نویسه *url,
                                 size_t len, bool relative,
                                 unsigned صحیح query)
{
  /* we must add this with whitespace-replacing */
  ثابت unsigned نویسه *iptr;
  ثابت unsigned نویسه *host_sep = (ثابت unsigned نویسه *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  اگر(!relative) {
    size_t n;
    host_sep = (ثابت unsigned نویسه *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ثابت نویسه *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  برای(iptr = host_sep; len && !result;) {
    اگر(*iptr == ' ') {
      اگر(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      وگرنه
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    وگرنه اگر((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned نویسه out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    وگرنه اگر(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned نویسه hex = (unsigned نویسه)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned نویسه out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    وگرنه {
      ثابت unsigned نویسه *start = iptr;
      وقتی(len) {
        اگر(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          بشکن;
        اگر(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          بشکن;
        اگر(*iptr == '?') {
          اگر(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            بشکن;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ثابت نویسه *)start, (size_t)(iptr - start));
    }
  }

  اگر(result)
    بازگشت cc2cu(result);
  بازگشت CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ثابت نویسه *url, نویسه *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (تهی)buflen; /* only used in debug-builds */
  اگر(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  اگر(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    بازگشت 0;
#endif
  اگر(ISALPHA(url[0])) {
    اگر(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    برای(i = 1; i < MAX_SCHEME_LEN; ++i) {
      نویسه s = url[i];
      اگر(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        اگر(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      وگرنه {
        بشکن;
      }
    }
  }
  اگر(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    اگر(buf)
      buf[i] = 0;
    بازگشت len;
  }
  اگر(buf)
    buf[0] = 0;
  بازگشت 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ایستا bool badoctets(ثابت نویسه *درون_داد, size_t n, صحیح flags)
{
  ثابت uint8_t *p = (ثابت unsigned نویسه *)درون_داد;
  ثابت uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  وقتی(n--) {
    اگر(*p <= control || *p == 127)
      بازگشت TRUE;
    p++;
  }
  بازگشت FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ساختار Curl_URL *u,
                                        ثابت نویسه *login,
                                        size_t len,
                                        unsigned صحیح flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(ساختار Curl_URL *u,
                                        ثابت نویسه *login,
                                        size_t len,
                                        unsigned صحیح flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  نویسه *userp = پوچ;
  نویسه *passwdp = پوچ;
  نویسه *optionsp = پوچ;
  ثابت ساختار Curl_scheme *h = پوچ;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ثابت نویسه *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  اگر(!ptr)
    برو out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  اگر(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : پوچ);
  اگر(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    برو out;
  }

  اگر(userp) {
    اگر(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      برو out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  اگر(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  اگر(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  اگر(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  وگرنه اگر(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  وگرنه اگر(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_درون_داد;

  userp = passwdp = optionsp = پوچ;

  اگر(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    بازگشت CURLUE_OK;
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

  بازگشت ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(ساختار Curl_URL *u, ساختار dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(ساختار Curl_URL *u, ساختار dynbuf *host,
                              bool has_scheme)
{
  ثابت نویسه *portptr;
  ثابت نویسه *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  اگر(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    اگر(!portptr)
      بازگشت CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    اگر(*portptr) {
      اگر(*portptr != ':')
        بازگشت CURLUE_BAD_PORT_عدد;
    }
    وگرنه
      portptr = پوچ;
  }
  وگرنه
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  اگر(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    صحیح rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    اگر(!*portptr)
      بازگشت has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_عدد;
    اگر(*portptr == '\\')
      بازگشت CURLUE_BACKSLASH;
    rc = curlx_str_عدد(&portptr, &port, 0xffff);
    اگر(rc)
      بازگشت CURLUE_BAD_PORT_عدد;
    وگرنه اگر(*portptr == '\\')
      بازگشت CURLUE_BACKSLASH;
    وگرنه اگر(*portptr)
      بازگشت CURLUE_BAD_PORT_عدد;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  بازگشت CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ایستا ثابت bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(ساختار Curl_URL *u, نویسه *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ساختار Curl_URL *u, نویسه *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  اگر(hlen < 4) /* '[::]' is the shortest possible valid string */
    بازگشت CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  اگر(hlen != len) {
    hlen = len;
    اگر(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      نویسه zoneid[MAX_ZONEID_LEN];
      صحیح i = 0;
      نویسه *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      اگر(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      وقتی(!invalid_host_char[(unsigned نویسه)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      اگر(!i || (']' != *h))
        بازگشت CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      اگر(!u->zoneid)
        بازگشت CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    وگرنه
      بازگشت CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    نویسه dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    اگر(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      بازگشت CURLUE_BAD_IPV6;
    اگر(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  بازگشت CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ایستا CURLUcode hostname_check(نویسه *hostname, size_t hlen)
{
  size_t i;
  برای(i = 0; i < hlen; i++) {
    اگر(invalid_host_char[(unsigned نویسه)hostname[i]])
      بازگشت CURLUE_BAD_HOSTNAME;
  }
  اگر((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    بازگشت CURLUE_BAD_HOSTNAME;
  وگرنه اگر((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    بازگشت CURLUE_BAD_HOSTNAME;
  بازگشت CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ایستا CURLUcode hostname_check6(ساختار Curl_URL *u, نویسه *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  اگر(!hlen)
    بازگشت CURLUE_NO_HOST;
  وگرنه اگر(hostname[0] == '[')
    بازگشت ipv6_parse(u, hostname, hlen);

  بازگشت hostname_check(hostname, hlen);
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
UNITTEST صحیح ipv4_normalize(ساختار dynbuf *host);
UNITTEST صحیح ipv4_normalize(ساختار dynbuf *host)
{
  bool done = FALSE;
  صحیح n = 0;
  ثابت نویسه *c = curlx_dyn_ptr(host);
  unsigned صحیح parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  اگر(!ISDIGIT(*c))
    بازگشت HOST_NAME;

  وقتی(!done) {
    صحیح rc;
    curl_off_t l;
    اگر(*c == '0') {
      اگر((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        اگر(rc)
          بازگشت HOST_NAME;
      }
      وگرنه
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    وگرنه
      rc = curlx_str_عدد(&c, &l, UINT_MAX);

    اگر(rc) {
      اگر(!n || (rc != STRE_NO_رقم) || *c)
        بازگشت HOST_NAME;
      n--;
    }
    وگرنه
      parts[n] = (unsigned صحیح)l;

    انتخاب(*c) {
    حالت '.':
      اگر(n == 3) {
        اگر(c[1])
          /* something follows this dot */
          بازگشت HOST_NAME;
        done = TRUE;
      }
      وگرنه {
        n++;
        c++;
      }
      بشکن;

    حالت '\0':
      done = TRUE;
      بشکن;

    پیشفرض:
      بازگشت HOST_NAME;
    }
  }

  انتخاب(n) {
  حالت 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    بشکن;
  حالت 1: /* a.b -- 8.24 bits */
    اگر((parts[0] > 0xff) || (parts[1] > 0xffffff))
      بازگشت HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    بشکن;
  حالت 2: /* a.b.c -- 8.8.16 bits */
    اگر((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      بازگشت HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    بشکن;
  حالت 3: /* a.b.c.d -- 8.8.8.8 bits */
    اگر((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      بازگشت HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    بشکن;
  }
  اگر(result)
    بازگشت HOST_ERROR;
  بازگشت HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ایستا CURLUcode urldecode_host(ساختار dynbuf *host)
{
  ثابت نویسه *per;
  ثابت نویسه *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  اگر(!per)
    /* nothing to decode */
    بازگشت CURLUE_OK;
  وگرنه {
    /* encoded */
    size_t dlen;
    نویسه *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    اگر(result)
      بازگشت CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    اگر(result)
      بازگشت cc2cu(result);
  }

  بازگشت CURLUE_OK;
}

ایستا CURLUcode parse_authority(ساختار Curl_URL *u,
                                 ثابت نویسه *auth, size_t authlen,
                                 unsigned صحیح flags,
                                 ساختار dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  اگر(uc)
    بازگشت uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  اگر(result) {
    uc = cc2cu(result);
    بازگشت uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  اگر(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  اگر(!uc)
    uc = urldecode_host(host);
  اگر(uc)
    ;
  وگرنه اگر(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  وگرنه {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    صحیح type = ipv4_normalize(host);

    اگر(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    وگرنه اگر(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  بازگشت uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ثابت نویسه *authority)
{
  CURLUcode ures;
  ساختار dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_درون_داد_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  اگر(ures)
    curlx_dyn_free(&host);
  وگرنه {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  بازگشت ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ایستا bool is_dot(ثابت نویسه **str, size_t *clen)
{
  ثابت نویسه *p = *str;
  اگر(*p == '.') {
    (*str)++;
    (*clen)--;
    بازگشت TRUE;
  }
  وگرنه اگر((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ایستا bool needs_dedotdot(ثابت نویسه *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  اگر(pn < 2)
    بازگشت FALSE;
  اگر(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    بازگشت FALSE;
  وقتی(pn) {
    اگر(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      اگر(!pn || ISSLASH(*p))
        بازگشت TRUE;
      /* "../" or ".." before end of string */
      وگرنه اگر(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        بازگشت TRUE;
    }
    وگرنه {
      p++;
      pn--;
    }
  }
  بازگشت FALSE;
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
UNITTEST صحیح dedotdotify(ثابت نویسه *درون_داد, size_t clen, نویسه **outp);
UNITTEST صحیح dedotdotify(ثابت نویسه *درون_داد, size_t clen, نویسه **outp)
{
  ساختار dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ثابت نویسه *dinput = درون_داد;
  size_t dlen = clen;

  *outp = پوچ;
  اگر(!needs_dedotdot(درون_داد, clen))
    بازگشت 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  اگر(is_dot(&dinput, &dlen)) {
    اگر(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      درون_داد = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    وگرنه اگر(is_dot(&dinput, &dlen)) {
      اگر(!dlen)
        /* .. [end] */
        برو end;
      وگرنه اگر(ISSLASH(*dinput)) {
        /* ../ */
        درون_داد = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  وقتی(clen && !result) { /* until end of path content */
    اگر(ISSLASH(*درون_داد)) {
      ثابت نویسه *p = &درون_داد[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      اگر(is_dot(&p, &blen)) {
        اگر(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          بشکن;
        }
        وگرنه اگر(ISSLASH(*p)) { /* /./ */
          درون_داد = p;
          clen = blen;
          ادامه;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        وگرنه اگر(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          اگر(len) {
            ثابت نویسه *ptr = curlx_dyn_ptr(&out);
            ثابت نویسه *last = memrchr(ptr, '/', len);
            اگر(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          اگر(blen) { /* /../ */
            درون_داد = p;
            clen = blen;
            ادامه;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          بشکن;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, درون_داد, 1);
    درون_داد++;
    clen--;
  }
end:
  اگر(!result) {
    اگر(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    وگرنه {
      *outp = curlx_strdup("");
      اگر(!*outp)
        بازگشت 1;
    }
  }
  بازگشت result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ثابت نویسه *url, size_t urllen, CURLU *u,
                              ثابت نویسه **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ثابت نویسه *url, size_t urllen, CURLU *u,
                              ثابت نویسه **pathp, size_t *pathlenp)
{
  ثابت نویسه *path;
  size_t pathlen;

  *pathp = پوچ;
  *pathlenp = 0;
  اگر(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    بازگشت CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  اگر(path[0] != '/')
    بازگشت CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  اگر(path[1] == '/') {
    /* swallow the two slashes */
    ثابت نویسه *ptr = &path[2];

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
    اگر(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      اگر(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      وگرنه
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        بازگشت CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#اگر !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  اگر(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    بازگشت CURLUE_BAD_FILE_URL;
  }
#وگرنه
  /* If the path starts with a slash and a drive letter, ditch the slash */
  اگر('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  اگر(!u->scheme)
    بازگشت CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode parse_scheme(ثابت نویسه *url, CURLU *u, نویسه *schemebuf,
                              size_t schemelen, unsigned صحیح flags,
                              ثابت نویسه **hostpp)
{
  /* clear path */
  ثابت نویسه *schemep = پوچ;

  اگر(schemelen) {
    صحیح رقم_slashes = 0;
    ثابت نویسه *p = &url[schemelen + 1];
    اگر(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      بازگشت CURLUE_UNSUPPORTED_SCHEME;

    اگر(!ISSLASH(*p))
      /* less than one */
      بازگشت CURLUE_BAD_SLASHES;
    اگر((flags & CURLU_NO_AUTHORITY)) {
      وقتی(ISSLASH(*p) && (رقم_slashes < 2)) {
        p++;
        رقم_slashes++;
      }
    }
    وگرنه {
      وقتی(ISSLASH(*p) && (رقم_slashes < 4)) {
        p++;
        رقم_slashes++;
      }
      اگر(رقم_slashes > 3)
        بازگشت CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  وگرنه {
    /* no scheme! */

    اگر(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      بازگشت CURLUE_BAD_SCHEME;

    اگر(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  اگر(schemep) {
    u->scheme = curlx_strdup(schemep);
    اگر(!u->scheme)
      بازگشت CURLUE_OUT_OF_MEMORY;
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode guess_scheme(CURLU *u, ساختار dynbuf *host)
{
  ثابت نویسه *hostname = curlx_dyn_ptr(host);
  ثابت نویسه *schemep = پوچ;
  /* legacy curl-style guess based on hostname */
  اگر(checkprefix("ftp.", hostname))
    schemep = "ftp";
  وگرنه اگر(checkprefix("dict.", hostname))
    schemep = "dict";
  وگرنه اگر(checkprefix("ldap.", hostname))
    schemep = "ldap";
  وگرنه اگر(checkprefix("imap.", hostname))
    schemep = "imap";
  وگرنه اگر(checkprefix("smtp.", hostname))
    schemep = "smtp";
  وگرنه اگر(checkprefix("pop3.", hostname))
    schemep = "pop3";
  وگرنه
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  اگر(!u->scheme)
    بازگشت CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode handle_fragment(CURLU *u, ثابت نویسه *fragment,
                                 size_t fraglen, unsigned صحیح flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  اگر(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    اگر(flags & CURLU_URLENCODE) {
      ساختار dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_درون_داد_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      اگر(ures)
        بازگشت ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    وگرنه {
      اگر(badoctets(fragment, fraglen, flags))
        بازگشت CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      اگر(!u->fragment)
        بازگشت CURLUE_OUT_OF_MEMORY;
    }
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode handle_query(CURLU *u, ثابت نویسه *query,
                              size_t qlen, unsigned صحیح flags)
{
  u->query_present = TRUE;
  اگر(qlen > 1) {
    اگر(flags & CURLU_URLENCODE) {
      ساختار dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_درون_داد_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      اگر(ures)
        بازگشت ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    وگرنه {
      اگر(badoctets(query, qlen, flags))
        بازگشت CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      اگر(!u->query)
        بازگشت CURLUE_OUT_OF_MEMORY;
    }
  }
  وگرنه {
    /* single byte query */
    u->query = curlx_strdup("");
    اگر(!u->query)
      بازگشت CURLUE_OUT_OF_MEMORY;
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode handle_path(CURLU *u, ثابت نویسه *path,
                             size_t pathlen, unsigned صحیح flags,
                             bool is_file)
{
  CURLUcode ures;
  اگر(pathlen && (flags & CURLU_URLENCODE)) {
    ساختار dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_درون_داد_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    اگر(ures)
      بازگشت ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  اگر(pathlen >= (size_t)(1 + !is_file)) {
    اگر(badoctets(path, pathlen, flags))
      بازگشت CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    اگر(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      اگر(!u->path)
        بازگشت CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    وگرنه اگر(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    اگر(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      نویسه *dedot;
      صحیح err = dedotdotify(path, pathlen, &dedot);
      اگر(err)
        بازگشت CURLUE_OUT_OF_MEMORY;
      اگر(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode parseurl(ثابت نویسه *url, CURLU *u, unsigned صحیح flags)
{
  ثابت نویسه *path;
  size_t pathlen;
  نویسه schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ساختار dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  اگر(urllen > CURL_MAX_درون_داد_LENGTH)
    بازگشت CURLUE_MALFORMED_درون_داد;

  curlx_dyn_init(&host, CURL_MAX_درون_داد_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, اندازه(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  اگر(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  وگرنه {
    ثابت نویسه *hostp = پوچ;
    ثابت نویسه *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    اگر(ures)
      برو fail;

    /* find the end of the hostname + port number */
    p = hostp;
    وقتی(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    اگر(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      اگر(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    وگرنه اگر(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      اگر(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    وگرنه
      ures = CURLUE_NO_HOST;
  }
  اگر(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ثابت نویسه *fragment = memchr(path, '#', pathlen);
    اگر(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  اگر(!ures) {
    ثابت نویسه *query = memchr(path, '?', pathlen);
    اگر(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  اگر(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  اگر(!ures) {
    u->host = curlx_dyn_ptr(&host);
    بازگشت CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  بازگشت ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ایستا CURLUcode parseurl_and_replace(ثابت نویسه *url, CURLU *u,
                                      unsigned صحیح flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, اندازه(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  اگر(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  بازگشت ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ایستا CURLUcode redirect_url(ثابت نویسه *base, ثابت نویسه *relurl,
                              CURLU *u, unsigned صحیح flags)
{
  ساختار dynbuf urlbuf;
  bool host_changed = FALSE;
  ثابت نویسه *useurl = relurl;
  ثابت نویسه *cutoff = پوچ;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ثابت نویسه *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ثابت نویسه *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  اگر(!base)
    بازگشت CURLUE_MALFORMED_درون_داد; /* should never happen */

  /* handle different relative URL types */
  انتخاب(relurl[0]) {
  حالت '/':
    اگر(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    وگرنه
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    بشکن;

  حالت '#':
    /* fragment-only change */
    اگر(u->fragment_present)
      cutoff = strchr(protsep, '#');
    بشکن;

  پیشفرض:
    /* path or query-only change */
    اگر(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    وگرنه اگر(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    اگر(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      اگر(cutoff)
        cutoff++; /* truncate after last slash */
    }
    بشکن;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_درون_داد_LENGTH);

  اگر(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  وگرنه
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  بازگشت uc;
}

/*
 */
CURLU *curl_url(تهی)
{
  بازگشت curlx_calloc(1, اندازه(ساختار Curl_URL));
}

تهی curl_url_cleanup(CURLU *u)
{
  اگر(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  انجامبده {                                          \
    اگر((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      اگر(!(dest)->name)                         \
        برو fail;                              \
    }                                           \
  } وقتی(0)

CURLU *curl_url_dup(ثابت CURLU *in)
{
  ساختار Curl_URL *u;

  اگر(!in)
    بازگشت پوچ;

  u = curlx_calloc(1, اندازه(ساختار Curl_URL));
  اگر(u) {
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
  بازگشت u;
fail:
  curl_url_cleanup(u);
  بازگشت پوچ;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#وگرنه
ایستا CURLUcode host_decode(ثابت نویسه *host, نویسه **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  اگر(result)
    بازگشت (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode host_encode(ثابت نویسه *host, نویسه **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  اگر(result)
    بازگشت (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  بازگشت CURLUE_OK;
}
#endif

ایستا CURLUcode urlget_format(ثابت CURLU *u, CURLUPart what,
                               ثابت نویسه *ptr, نویسه **partp,
                               bool plusdecode, unsigned صحیح flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  نویسه *part = curlx_memdup0(ptr, partlen);
  *partp = پوچ;
  اگر(!part)
    بازگشت CURLUE_OUT_OF_MEMORY;
  اگر(plusdecode) {
    /* convert + to space */
    نویسه *plus = part;
    size_t i = 0;
    برای(i = 0; i < partlen; ++plus, i++) {
      اگر(*plus == '+')
        *plus = ' ';
    }
  }
  اگر(urldecode) {
    نویسه *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    اگر(result)
      بازگشت CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  اگر(urlencode) {
    ساختار dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_درون_داد_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    اگر(uc)
      بازگشت uc;
    part = curlx_dyn_ptr(&enc);
  }
  وگرنه اگر(punycode) {
    اگر(!Curl_is_ASCII_name(u->host)) {
      نویسه *punyversion = پوچ;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      اگر(uc)
        بازگشت uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      اگر(uc) {
        curlx_free(punyversion);
        بازگشت uc;
      }
      part = punyversion;
    }
  }
  وگرنه اگر(depunyfy && Curl_is_ASCII_name(u->host)) {
    نویسه *unpunified = پوچ;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    اگر(uc)
      بازگشت uc;
    part = unpunified;
  }
  *partp = part;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode file_url(ثابت CURLU *u, نویسه **part,
                          ثابت نویسه *fragmentsep,
                          ثابت نویسه *querysep)
{
  نویسه *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  اگر(!url)
    بازگشت CURLUE_OUT_OF_MEMORY;

  *part = url;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode urlget_url(ثابت CURLU *u, نویسه **part, unsigned صحیح flags)
{
  نویسه *url;
  نویسه *allochost = پوچ;
  ثابت نویسه *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  ثابت نویسه *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  نویسه portbuf[7];
  اگر(curl_strequal("file", u->scheme))
    بازگشت file_url(u, part, fragmentsep, querysep);
  وگرنه اگر(!u->host)
    بازگشت CURLUE_NO_HOST;
  وگرنه {
    ثابت نویسه *scheme;
    نویسه *options = u->options;
    نویسه *port = پوچ;
    ثابت ساختار Curl_scheme *h = پوچ;
    نویسه schemebuf[MAX_SCHEME_LEN + 5];
    اگر(u->scheme)
      scheme = u->scheme;
    وگرنه اگر(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    وگرنه
      بازگشت CURLUE_NO_SCHEME;

    اگر(u->port_present) {
      curl_msnprintf(portbuf, اندازه(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    اگر(h) {
      اگر(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, اندازه(portbuf), "%u", h->defport);
        port = portbuf;
      }
      وگرنه اگر(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = پوچ;
      }

      اگر(!(h->flags & PROTOPT_URLOPTIONS))
        options = پوچ;
    }

    اگر(u->host[0] == '[') {
      اگر(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ساختار dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_درون_داد_LENGTH);
        اگر(curlx_dyn_addf(&enc, "%.*s%%25%s]", (صحیح)hostlen - 1, u->host,
                          u->zoneid))
          بازگشت CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    وگرنه اگر(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(پوچ, u->host, 0);
      اگر(!allochost)
        بازگشت CURLUE_OUT_OF_MEMORY;
    }
    وگرنه اگر(flags & CURLU_PUNYCODE) {
      اگر(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        اگر(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        اگر(ret) {
          curlx_safefree(allochost);
          بازگشت ret;
        }
      }
    }
    وگرنه اگر(flags & CURLU_PUNY2IDN) {
      اگر(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        اگر(ret)
          بازگشت ret;
      }
    }

    اگر(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, اندازه(schemebuf), "%s://", scheme);
    وگرنه
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
  اگر(!url)
    بازگشت CURLUE_OUT_OF_MEMORY;
  *part = url;
  بازگشت CURLUE_OK;
}

CURLUcode curl_url_get(ثابت CURLU *u, CURLUPart what,
                       نویسه **part, unsigned صحیح flags)
{
  ثابت نویسه *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  نویسه portbuf[7];
  bool plusdecode = FALSE;
  اگر(!u)
    بازگشت CURLUE_BAD_HANDLE;
  اگر(!part)
    بازگشت CURLUE_BAD_PARTPOINTER;
  *part = پوچ;

  انتخاب(what) {
  حالت CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    اگر((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      بازگشت CURLUE_NO_SCHEME;
    بشکن;
  حالت CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    بشکن;
  حالت CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    بشکن;
  حالت CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    بشکن;
  حالت CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    بشکن;
  حالت CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    بشکن;
  حالت CURLUPART_PORT:
    ptr = پوچ;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    اگر(u->port_present) {
      ثابت ساختار Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : پوچ;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      اگر(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = پوچ;
      }
      وگرنه {
        curl_msnprintf(portbuf, اندازه(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    وگرنه اگر((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ثابت ساختار Curl_scheme *h = Curl_get_scheme(u->scheme);
      اگر(h) {
        curl_msnprintf(portbuf, اندازه(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    بشکن;
  حالت CURLUPART_PATH:
    ptr = u->path;
    اگر(!ptr)
      ptr = "/";
    بشکن;
  حالت CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    اگر(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = پوچ;
    بشکن;
  حالت CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    اگر(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    بشکن;
  حالت CURLUPART_URL:
    بازگشت urlget_url(u, part, flags);
  پیشفرض:
    ptr = پوچ;
    بشکن;
  }
  اگر(ptr)
    بازگشت urlget_format(u, what, ptr, part, plusdecode, flags);

  بازگشت ifmissing;
}

ایستا CURLUcode set_url_scheme(CURLU *u, ثابت نویسه *scheme,
                                unsigned صحیح flags)
{
  size_t plen = strlen(scheme);
  ثابت ساختار Curl_scheme *h = پوچ;
  اگر((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    بازگشت CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  اگر(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    بازگشت CURLUE_UNSUPPORTED_SCHEME;
  اگر(!h) {
    ثابت نویسه *s = scheme;
    اگر(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      وقتی(--plen) {
        اگر(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        وگرنه
          بازگشت CURLUE_BAD_SCHEME;
      }
    }
    وگرنه
      بازگشت CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode set_url_port(CURLU *u, ثابت نویسه *provided_port)
{
  curl_off_t port;
  اگر(!ISDIGIT(provided_port[0]))
    /* not a number */
    بازگشت CURLUE_BAD_PORT_عدد;
  اگر(curlx_str_عدد(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    بازگشت CURLUE_BAD_PORT_عدد;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  بازگشت CURLUE_OK;
}

ایستا CURLUcode set_url(CURLU *u, ثابت نویسه *url, size_t part_size,
                         unsigned صحیح flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  نویسه *oldurl = پوچ;

  اگر(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    اگر(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      بازگشت CURLUE_OK;
    }
    اگر(uc == CURLUE_OUT_OF_MEMORY)
      بازگشت uc;
    بازگشت CURLUE_MALFORMED_درون_داد;
  }

  /* if the new URL is absolute replace the existing with the new. */
  اگر(Curl_is_absolute_url(url, پوچ, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    بازگشت parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  اگر(uc == CURLUE_OUT_OF_MEMORY)
    بازگشت uc;
  وگرنه اگر(uc)
    بازگشت parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  بازگشت uc;
}

ایستا CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  انتخاب(what) {
  حالت CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, اندازه(ساختار Curl_URL));
    بشکن;
  حالت CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    بشکن;
  حالت CURLUPART_USER:
    curlx_safefree(u->user);
    بشکن;
  حالت CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    بشکن;
  حالت CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    بشکن;
  حالت CURLUPART_HOST:
    curlx_safefree(u->host);
    بشکن;
  حالت CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    بشکن;
  حالت CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    بشکن;
  حالت CURLUPART_PATH:
    curlx_safefree(u->path);
    بشکن;
  حالت CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    بشکن;
  حالت CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    بشکن;
  پیشفرض:
    بازگشت CURLUE_UNKNOWN_PART;
  }
  بازگشت CURLUE_OK;
}

ایستا bool allowed_in_path(unsigned نویسه x)
{
  انتخاب(x) {
  حالت '!':
  حالت '$':
  حالت '&':
  حالت '\'':
  حالت '(':
  حالت ')':
  حالت '{':
  حالت '}':
  حالت '[':
  حالت ']':
  حالت '*':
  حالت '+':
  حالت ',':
  حالت ';':
  حالت '=':
  حالت ':':
  حالت '@':
  حالت '/':
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا CURLUcode url_encode_part(ساختار dynbuf *encp,
                                 ثابت نویسه *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ثابت unsigned نویسه *i;

  برای(i = (ثابت unsigned نویسه *)part; *i; i++) {
    CURLcode result;
    اگر((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    وگرنه اگر(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      اگر((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    وگرنه {
      unsigned نویسه out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    اگر(result)
      بازگشت cc2cu(result);
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode url_uppercasehex_part(ساختار dynbuf *encp,
                                       ثابت نویسه *part)
{
  نویسه *p;
  CURLcode result = curlx_dyn_add(encp, part);
  اگر(result)
    بازگشت cc2cu(result);
  p = curlx_dyn_ptr(encp);
  وقتی(*p) {
    /* make sure percent encoded are upper case */
    اگر((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    وگرنه
      p++;
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode url_append_query(CURLU *u, ساختار dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  اگر(querylen) {
    ساختار dynbuf qbuf;
    CURLcode result;
    ثابت نویسه *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_درون_داد_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    اگر(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    اگر(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    اگر(result)
      برو nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    بازگشت CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    بازگشت cc2cu(result);
  }
  وگرنه {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  بازگشت CURLUE_OK;
}

ایستا CURLUcode url_sethost(CURLU *u, ساختار dynbuf *encp,
                             bool urlencode,
                             unsigned صحیح flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  نویسه *newp = curlx_dyn_ptr(encp);
  اگر(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  وگرنه اگر(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    نویسه *decoded = پوچ;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    اگر(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    وگرنه {
      ساختار dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_درون_داد_LENGTH);
      اگر(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      وگرنه اگر(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        اگر(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  وگرنه اگر(hostname_check6(u, newp, n))
    bad = TRUE;
  وگرنه اگر(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  اگر(bad) {
    curlx_dyn_free(encp);
    بازگشت CURLUE_BAD_HOSTNAME;
  }
  بازگشت CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ثابت نویسه *part, unsigned صحیح flags)
{
  نویسه **storep = پوچ;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  اگر(!u)
    بازگشت CURLUE_BAD_HANDLE;
  اگر(!part)
    /* setting a part to NULL clears it */
    بازگشت urlset_clear(u, what);

  nalloc = strlen(part);
  اگر(nalloc > CURL_MAX_درون_داد_LENGTH)
    /* excessive input length */
    بازگشت CURLUE_MALFORMED_درون_داد;

  انتخاب(what) {
  حالت CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    اگر(status)
      بازگشت status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    بشکن;
  }
  حالت CURLUPART_USER:
    storep = &u->user;
    بشکن;
  حالت CURLUPART_PASSWORD:
    storep = &u->password;
    بشکن;
  حالت CURLUPART_OPTIONS:
    storep = &u->options;
    بشکن;
  حالت CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    بشکن;
  حالت CURLUPART_ZONEID:
    storep = &u->zoneid;
    بشکن;
  حالت CURLUPART_PORT:
    بازگشت set_url_port(u, part);
  حالت CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    بشکن;
  حالت CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    بشکن;
  حالت CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    بشکن;
  حالت CURLUPART_URL:
    بازگشت set_url(u, part, nalloc, flags);
  پیشفرض:
    بازگشت CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ثابت نویسه *newp = پوچ;
    ساختار dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    اگر(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      اگر(result)
        بازگشت cc2cu(result);
    }
    اگر(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    وگرنه
      status = url_uppercasehex_part(&enc, part);
    اگر(!status) {
      newp = curlx_dyn_ptr(&enc);

      اگر(appendquery && newp)
        بازگشت url_append_query(u, &enc);
      وگرنه اگر(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    اگر(status)
      بازگشت status;

    اگر(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (نویسه *)CURL_UNCONST(newp);
  }
  بازگشت CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ثابت ساختار Curl_scheme *s = پوچ;

  /* base must be an absolute URL */
  اگر(!base->scheme || !base->host)
    بازگشت FALSE;
  اگر(href->scheme && !curl_strequal(base->scheme, href->scheme))
    بازگشت FALSE;
  اگر(href->host) {
    اگر(!curl_strequal(base->host, href->host))
      بازگشت FALSE;

    اگر(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      اگر(!s) /* Cannot match default port for unknown scheme */
        بازگشت FALSE;
      /* to match, the present one must be the default port */
      اگر((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        بازگشت FALSE;
    }
    وگرنه اگر(base->portnum != href->portnum) /* both present or missing */
      بازگشت FALSE;

    اگر(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      بازگشت FALSE;
  }
  وگرنه اگر(href->port_present) /* no host in href, then there must be no port */
    بازگشت FALSE;
  بازگشت TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  اگر(u->port_present) {
    *pport = u->portnum;
    بازگشت CURLUE_OK;
  }
  وگرنه اگر(u->scheme) {
    ثابت ساختار Curl_scheme *s = Curl_get_scheme(u->scheme);
    اگر(s && s->defport) {
      *pport = s->defport;
      بازگشت CURLUE_OK;
    }
  }
  *pport = 0;
  بازگشت CURLUE_NO_PORT;
}
