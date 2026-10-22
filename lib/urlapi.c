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

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "urlapi-int.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "idn.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curl_memrchr.h"

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
#सचे !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

स्थिर शून्य free_urlhandle(संरचना Curl_URL *u)
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
स्थिर ध्रुव वर्ण *find_host_sep(ध्रुव वर्ण *url)
{
  /* Find the start of the hostname */
  ध्रुव वर्ण *sep = strstr(url, "//");
  सचे(!sep)
    sep = url;
  अञ्ञथा
    sep += 2;

  /* Find first / or ? */
  याव(*sep && *sep != '/' && *sep != '?')
    sep++;

  पटिनिवत्त sep;
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
UNITTEST CURLUcode urlencode_str(संरचना dynbuf *o, ध्रुव वर्ण *url,
                                 size_t len, bool relative,
                                 unsigned पूर्णाङ्क query);
UNITTEST CURLUcode urlencode_str(संरचना dynbuf *o, ध्रुव वर्ण *url,
                                 size_t len, bool relative,
                                 unsigned पूर्णाङ्क query)
{
  /* we must add this with whitespace-replacing */
  ध्रुव unsigned वर्ण *iptr;
  ध्रुव unsigned वर्ण *host_sep = (ध्रुव unsigned वर्ण *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  सचे(!relative) {
    size_t n;
    host_sep = (ध्रुव unsigned वर्ण *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ध्रुव वर्ण *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  अत्थाय(iptr = host_sep; len && !result;) {
    सचे(*iptr == ' ') {
      सचे(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      अञ्ञथा
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    अञ्ञथा सचे((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned वर्ण out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    अञ्ञथा सचे(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned वर्ण hex = (unsigned वर्ण)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned वर्ण out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    अञ्ञथा {
      ध्रुव unsigned वर्ण *start = iptr;
      याव(len) {
        सचे(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          भङ्ग;
        सचे(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          भङ्ग;
        सचे(*iptr == '?') {
          सचे(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            भङ्ग;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ध्रुव वर्ण *)start, (size_t)(iptr - start));
    }
  }

  सचे(result)
    पटिनिवत्त cc2cu(result);
  पटिनिवत्त CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ध्रुव वर्ण *url, वर्ण *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (शून्य)buflen; /* only used in debug-builds */
  सचे(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  सचे(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    पटिनिवत्त 0;
#endif
  सचे(ISALPHA(url[0])) {
    सचे(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    अत्थाय(i = 1; i < MAX_SCHEME_LEN; ++i) {
      वर्ण s = url[i];
      सचे(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        सचे(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      अञ्ञथा {
        भङ्ग;
      }
    }
  }
  सचे(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    सचे(buf)
      buf[i] = 0;
    पटिनिवत्त len;
  }
  सचे(buf)
    buf[0] = 0;
  पटिनिवत्त 0;
}

/* scan for byte values <= 31, 127 and maybe space */
स्थिर bool badoctets(ध्रुव वर्ण *निवेशः, size_t n, पूर्णाङ्क flags)
{
  ध्रुव uint8_t *p = (ध्रुव unsigned वर्ण *)निवेशः;
  ध्रुव uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  याव(n--) {
    सचे(*p <= control || *p == 127)
      पटिनिवत्त TRUE;
    p++;
  }
  पटिनिवत्त FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(संरचना Curl_URL *u,
                                        ध्रुव वर्ण *login,
                                        size_t len,
                                        unsigned पूर्णाङ्क flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(संरचना Curl_URL *u,
                                        ध्रुव वर्ण *login,
                                        size_t len,
                                        unsigned पूर्णाङ्क flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  वर्ण *userp = रिक्त;
  वर्ण *passwdp = रिक्त;
  वर्ण *optionsp = रिक्त;
  ध्रुव संरचना Curl_scheme *h = रिक्त;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ध्रुव वर्ण *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  सचे(!ptr)
    गच्छ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  सचे(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : रिक्त);
  सचे(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    गच्छ out;
  }

  सचे(userp) {
    सचे(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      गच्छ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  सचे(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  सचे(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  सचे(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  अञ्ञथा सचे(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  अञ्ञथा सचे(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_निवेशः;

  userp = passwdp = optionsp = रिक्त;

  सचे(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    पटिनिवत्त CURLUE_OK;
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

  पटिनिवत्त ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(संरचना Curl_URL *u, संरचना dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(संरचना Curl_URL *u, संरचना dynbuf *host,
                              bool has_scheme)
{
  ध्रुव वर्ण *portptr;
  ध्रुव वर्ण *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  सचे(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    सचे(!portptr)
      पटिनिवत्त CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    सचे(*portptr) {
      सचे(*portptr != ':')
        पटिनिवत्त CURLUE_BAD_PORT_सङ्ख्या;
    }
    अञ्ञथा
      portptr = रिक्त;
  }
  अञ्ञथा
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  सचे(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    पूर्णाङ्क rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    सचे(!*portptr)
      पटिनिवत्त has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_सङ्ख्या;
    सचे(*portptr == '\\')
      पटिनिवत्त CURLUE_BACKSLASH;
    rc = curlx_str_सङ्ख्या(&portptr, &port, 0xffff);
    सचे(rc)
      पटिनिवत्त CURLUE_BAD_PORT_सङ्ख्या;
    अञ्ञथा सचे(*portptr == '\\')
      पटिनिवत्त CURLUE_BACKSLASH;
    अञ्ञथा सचे(*portptr)
      पटिनिवत्त CURLUE_BAD_PORT_सङ्ख्या;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  पटिनिवत्त CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

स्थिर ध्रुव bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(संरचना Curl_URL *u, वर्ण *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(संरचना Curl_URL *u, वर्ण *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  सचे(hlen < 4) /* '[::]' is the shortest possible valid string */
    पटिनिवत्त CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  सचे(hlen != len) {
    hlen = len;
    सचे(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      वर्ण zoneid[MAX_ZONEID_LEN];
      पूर्णाङ्क i = 0;
      वर्ण *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      सचे(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      याव(!invalid_host_char[(unsigned वर्ण)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      सचे(!i || (']' != *h))
        पटिनिवत्त CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      सचे(!u->zoneid)
        पटिनिवत्त CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    अञ्ञथा
      पटिनिवत्त CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    वर्ण dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    सचे(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      पटिनिवत्त CURLUE_BAD_IPV6;
    सचे(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  पटिनिवत्त CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
स्थिर CURLUcode hostname_check(वर्ण *hostname, size_t hlen)
{
  size_t i;
  अत्थाय(i = 0; i < hlen; i++) {
    सचे(invalid_host_char[(unsigned वर्ण)hostname[i]])
      पटिनिवत्त CURLUE_BAD_HOSTNAME;
  }
  सचे((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    पटिनिवत्त CURLUE_BAD_HOSTNAME;
  अञ्ञथा सचे((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    पटिनिवत्त CURLUE_BAD_HOSTNAME;
  पटिनिवत्त CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
स्थिर CURLUcode hostname_check6(संरचना Curl_URL *u, वर्ण *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  सचे(!hlen)
    पटिनिवत्त CURLUE_NO_HOST;
  अञ्ञथा सचे(hostname[0] == '[')
    पटिनिवत्त ipv6_parse(u, hostname, hlen);

  पटिनिवत्त hostname_check(hostname, hlen);
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
UNITTEST पूर्णाङ्क ipv4_normalize(संरचना dynbuf *host);
UNITTEST पूर्णाङ्क ipv4_normalize(संरचना dynbuf *host)
{
  bool done = FALSE;
  पूर्णाङ्क n = 0;
  ध्रुव वर्ण *c = curlx_dyn_ptr(host);
  unsigned पूर्णाङ्क parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  सचे(!ISDIGIT(*c))
    पटिनिवत्त HOST_NAME;

  याव(!done) {
    पूर्णाङ्क rc;
    curl_off_t l;
    सचे(*c == '0') {
      सचे((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        सचे(rc)
          पटिनिवत्त HOST_NAME;
      }
      अञ्ञथा
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    अञ्ञथा
      rc = curlx_str_सङ्ख्या(&c, &l, UINT_MAX);

    सचे(rc) {
      सचे(!n || (rc != STRE_NO_अङ्कः) || *c)
        पटिनिवत्त HOST_NAME;
      n--;
    }
    अञ्ञथा
      parts[n] = (unsigned पूर्णाङ्क)l;

    विकल्प(*c) {
    पक्ष '.':
      सचे(n == 3) {
        सचे(c[1])
          /* something follows this dot */
          पटिनिवत्त HOST_NAME;
        done = TRUE;
      }
      अञ्ञथा {
        n++;
        c++;
      }
      भङ्ग;

    पक्ष '\0':
      done = TRUE;
      भङ्ग;

    स्वभाव:
      पटिनिवत्त HOST_NAME;
    }
  }

  विकल्प(n) {
  पक्ष 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    भङ्ग;
  पक्ष 1: /* a.b -- 8.24 bits */
    सचे((parts[0] > 0xff) || (parts[1] > 0xffffff))
      पटिनिवत्त HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    भङ्ग;
  पक्ष 2: /* a.b.c -- 8.8.16 bits */
    सचे((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      पटिनिवत्त HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    भङ्ग;
  पक्ष 3: /* a.b.c.d -- 8.8.8.8 bits */
    सचे((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      पटिनिवत्त HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    भङ्ग;
  }
  सचे(result)
    पटिनिवत्त HOST_ERROR;
  पटिनिवत्त HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
स्थिर CURLUcode urldecode_host(संरचना dynbuf *host)
{
  ध्रुव वर्ण *per;
  ध्रुव वर्ण *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  सचे(!per)
    /* nothing to decode */
    पटिनिवत्त CURLUE_OK;
  अञ्ञथा {
    /* encoded */
    size_t dlen;
    वर्ण *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    सचे(result)
      पटिनिवत्त CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    सचे(result)
      पटिनिवत्त cc2cu(result);
  }

  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode parse_authority(संरचना Curl_URL *u,
                                 ध्रुव वर्ण *auth, size_t authlen,
                                 unsigned पूर्णाङ्क flags,
                                 संरचना dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  सचे(uc)
    पटिनिवत्त uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  सचे(result) {
    uc = cc2cu(result);
    पटिनिवत्त uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  सचे(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  सचे(!uc)
    uc = urldecode_host(host);
  सचे(uc)
    ;
  अञ्ञथा सचे(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  अञ्ञथा {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    पूर्णाङ्क type = ipv4_normalize(host);

    सचे(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    अञ्ञथा सचे(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  पटिनिवत्त uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ध्रुव वर्ण *authority)
{
  CURLUcode ures;
  संरचना dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_निवेशः_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  सचे(ures)
    curlx_dyn_free(&host);
  अञ्ञथा {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  पटिनिवत्त ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

स्थिर bool is_dot(ध्रुव वर्ण **str, size_t *clen)
{
  ध्रुव वर्ण *p = *str;
  सचे(*p == '.') {
    (*str)++;
    (*clen)--;
    पटिनिवत्त TRUE;
  }
  अञ्ञथा सचे((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
स्थिर bool needs_dedotdot(ध्रुव वर्ण *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  सचे(pn < 2)
    पटिनिवत्त FALSE;
  सचे(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    पटिनिवत्त FALSE;
  याव(pn) {
    सचे(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      सचे(!pn || ISSLASH(*p))
        पटिनिवत्त TRUE;
      /* "../" or ".." before end of string */
      अञ्ञथा सचे(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        पटिनिवत्त TRUE;
    }
    अञ्ञथा {
      p++;
      pn--;
    }
  }
  पटिनिवत्त FALSE;
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
UNITTEST पूर्णाङ्क dedotdotify(ध्रुव वर्ण *निवेशः, size_t clen, वर्ण **outp);
UNITTEST पूर्णाङ्क dedotdotify(ध्रुव वर्ण *निवेशः, size_t clen, वर्ण **outp)
{
  संरचना dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ध्रुव वर्ण *dinput = निवेशः;
  size_t dlen = clen;

  *outp = रिक्त;
  सचे(!needs_dedotdot(निवेशः, clen))
    पटिनिवत्त 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  सचे(is_dot(&dinput, &dlen)) {
    सचे(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      निवेशः = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    अञ्ञथा सचे(is_dot(&dinput, &dlen)) {
      सचे(!dlen)
        /* .. [end] */
        गच्छ end;
      अञ्ञथा सचे(ISSLASH(*dinput)) {
        /* ../ */
        निवेशः = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  याव(clen && !result) { /* until end of path content */
    सचे(ISSLASH(*निवेशः)) {
      ध्रुव वर्ण *p = &निवेशः[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      सचे(is_dot(&p, &blen)) {
        सचे(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          भङ्ग;
        }
        अञ्ञथा सचे(ISSLASH(*p)) { /* /./ */
          निवेशः = p;
          clen = blen;
          अनुवर्त;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        अञ्ञथा सचे(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          सचे(len) {
            ध्रुव वर्ण *ptr = curlx_dyn_ptr(&out);
            ध्रुव वर्ण *last = memrchr(ptr, '/', len);
            सचे(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          सचे(blen) { /* /../ */
            निवेशः = p;
            clen = blen;
            अनुवर्त;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          भङ्ग;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, निवेशः, 1);
    निवेशः++;
    clen--;
  }
end:
  सचे(!result) {
    सचे(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    अञ्ञथा {
      *outp = curlx_strdup("");
      सचे(!*outp)
        पटिनिवत्त 1;
    }
  }
  पटिनिवत्त result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ध्रुव वर्ण *url, size_t urllen, CURLU *u,
                              ध्रुव वर्ण **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ध्रुव वर्ण *url, size_t urllen, CURLU *u,
                              ध्रुव वर्ण **pathp, size_t *pathlenp)
{
  ध्रुव वर्ण *path;
  size_t pathlen;

  *pathp = रिक्त;
  *pathlenp = 0;
  सचे(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    पटिनिवत्त CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  सचे(path[0] != '/')
    पटिनिवत्त CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  सचे(path[1] == '/') {
    /* swallow the two slashes */
    ध्रुव वर्ण *ptr = &path[2];

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
    सचे(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      सचे(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      अञ्ञथा
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        पटिनिवत्त CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#सचे !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  सचे(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    पटिनिवत्त CURLUE_BAD_FILE_URL;
  }
#अञ्ञथा
  /* If the path starts with a slash and a drive letter, ditch the slash */
  सचे('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  सचे(!u->scheme)
    पटिनिवत्त CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode parse_scheme(ध्रुव वर्ण *url, CURLU *u, वर्ण *schemebuf,
                              size_t schemelen, unsigned पूर्णाङ्क flags,
                              ध्रुव वर्ण **hostpp)
{
  /* clear path */
  ध्रुव वर्ण *schemep = रिक्त;

  सचे(schemelen) {
    पूर्णाङ्क अङ्कः_slashes = 0;
    ध्रुव वर्ण *p = &url[schemelen + 1];
    सचे(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      पटिनिवत्त CURLUE_UNSUPPORTED_SCHEME;

    सचे(!ISSLASH(*p))
      /* less than one */
      पटिनिवत्त CURLUE_BAD_SLASHES;
    सचे((flags & CURLU_NO_AUTHORITY)) {
      याव(ISSLASH(*p) && (अङ्कः_slashes < 2)) {
        p++;
        अङ्कः_slashes++;
      }
    }
    अञ्ञथा {
      याव(ISSLASH(*p) && (अङ्कः_slashes < 4)) {
        p++;
        अङ्कः_slashes++;
      }
      सचे(अङ्कः_slashes > 3)
        पटिनिवत्त CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  अञ्ञथा {
    /* no scheme! */

    सचे(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      पटिनिवत्त CURLUE_BAD_SCHEME;

    सचे(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  सचे(schemep) {
    u->scheme = curlx_strdup(schemep);
    सचे(!u->scheme)
      पटिनिवत्त CURLUE_OUT_OF_MEMORY;
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode guess_scheme(CURLU *u, संरचना dynbuf *host)
{
  ध्रुव वर्ण *hostname = curlx_dyn_ptr(host);
  ध्रुव वर्ण *schemep = रिक्त;
  /* legacy curl-style guess based on hostname */
  सचे(checkprefix("ftp.", hostname))
    schemep = "ftp";
  अञ्ञथा सचे(checkprefix("dict.", hostname))
    schemep = "dict";
  अञ्ञथा सचे(checkprefix("ldap.", hostname))
    schemep = "ldap";
  अञ्ञथा सचे(checkprefix("imap.", hostname))
    schemep = "imap";
  अञ्ञथा सचे(checkprefix("smtp.", hostname))
    schemep = "smtp";
  अञ्ञथा सचे(checkprefix("pop3.", hostname))
    schemep = "pop3";
  अञ्ञथा
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  सचे(!u->scheme)
    पटिनिवत्त CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode handle_fragment(CURLU *u, ध्रुव वर्ण *fragment,
                                 size_t fraglen, unsigned पूर्णाङ्क flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  सचे(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    सचे(flags & CURLU_URLENCODE) {
      संरचना dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_निवेशः_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      सचे(ures)
        पटिनिवत्त ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    अञ्ञथा {
      सचे(badoctets(fragment, fraglen, flags))
        पटिनिवत्त CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      सचे(!u->fragment)
        पटिनिवत्त CURLUE_OUT_OF_MEMORY;
    }
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode handle_query(CURLU *u, ध्रुव वर्ण *query,
                              size_t qlen, unsigned पूर्णाङ्क flags)
{
  u->query_present = TRUE;
  सचे(qlen > 1) {
    सचे(flags & CURLU_URLENCODE) {
      संरचना dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_निवेशः_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      सचे(ures)
        पटिनिवत्त ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    अञ्ञथा {
      सचे(badoctets(query, qlen, flags))
        पटिनिवत्त CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      सचे(!u->query)
        पटिनिवत्त CURLUE_OUT_OF_MEMORY;
    }
  }
  अञ्ञथा {
    /* single byte query */
    u->query = curlx_strdup("");
    सचे(!u->query)
      पटिनिवत्त CURLUE_OUT_OF_MEMORY;
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode handle_path(CURLU *u, ध्रुव वर्ण *path,
                             size_t pathlen, unsigned पूर्णाङ्क flags,
                             bool is_file)
{
  CURLUcode ures;
  सचे(pathlen && (flags & CURLU_URLENCODE)) {
    संरचना dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_निवेशः_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    सचे(ures)
      पटिनिवत्त ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  सचे(pathlen >= (size_t)(1 + !is_file)) {
    सचे(badoctets(path, pathlen, flags))
      पटिनिवत्त CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    सचे(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      सचे(!u->path)
        पटिनिवत्त CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    अञ्ञथा सचे(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    सचे(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      वर्ण *dedot;
      पूर्णाङ्क err = dedotdotify(path, pathlen, &dedot);
      सचे(err)
        पटिनिवत्त CURLUE_OUT_OF_MEMORY;
      सचे(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode parseurl(ध्रुव वर्ण *url, CURLU *u, unsigned पूर्णाङ्क flags)
{
  ध्रुव वर्ण *path;
  size_t pathlen;
  वर्ण schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  संरचना dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  सचे(urllen > CURL_MAX_निवेशः_LENGTH)
    पटिनिवत्त CURLUE_MALFORMED_निवेशः;

  curlx_dyn_init(&host, CURL_MAX_निवेशः_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, परिमाण(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  सचे(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  अञ्ञथा {
    ध्रुव वर्ण *hostp = रिक्त;
    ध्रुव वर्ण *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    सचे(ures)
      गच्छ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    याव(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    सचे(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      सचे(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    अञ्ञथा सचे(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      सचे(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    अञ्ञथा
      ures = CURLUE_NO_HOST;
  }
  सचे(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ध्रुव वर्ण *fragment = memchr(path, '#', pathlen);
    सचे(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  सचे(!ures) {
    ध्रुव वर्ण *query = memchr(path, '?', pathlen);
    सचे(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  सचे(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  सचे(!ures) {
    u->host = curlx_dyn_ptr(&host);
    पटिनिवत्त CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  पटिनिवत्त ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
स्थिर CURLUcode parseurl_and_replace(ध्रुव वर्ण *url, CURLU *u,
                                      unsigned पूर्णाङ्क flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, परिमाण(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  सचे(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  पटिनिवत्त ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
स्थिर CURLUcode redirect_url(ध्रुव वर्ण *base, ध्रुव वर्ण *relurl,
                              CURLU *u, unsigned पूर्णाङ्क flags)
{
  संरचना dynbuf urlbuf;
  bool host_changed = FALSE;
  ध्रुव वर्ण *useurl = relurl;
  ध्रुव वर्ण *cutoff = रिक्त;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ध्रुव वर्ण *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ध्रुव वर्ण *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  सचे(!base)
    पटिनिवत्त CURLUE_MALFORMED_निवेशः; /* should never happen */

  /* handle different relative URL types */
  विकल्प(relurl[0]) {
  पक्ष '/':
    सचे(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    अञ्ञथा
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    भङ्ग;

  पक्ष '#':
    /* fragment-only change */
    सचे(u->fragment_present)
      cutoff = strchr(protsep, '#');
    भङ्ग;

  स्वभाव:
    /* path or query-only change */
    सचे(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    अञ्ञथा सचे(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    सचे(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      सचे(cutoff)
        cutoff++; /* truncate after last slash */
    }
    भङ्ग;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_निवेशः_LENGTH);

  सचे(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  अञ्ञथा
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  पटिनिवत्त uc;
}

/*
 */
CURLU *curl_url(शून्य)
{
  पटिनिवत्त curlx_calloc(1, परिमाण(संरचना Curl_URL));
}

शून्य curl_url_cleanup(CURLU *u)
{
  सचे(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  करोहि {                                          \
    सचे((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      सचे(!(dest)->name)                         \
        गच्छ fail;                              \
    }                                           \
  } याव(0)

CURLU *curl_url_dup(ध्रुव CURLU *in)
{
  संरचना Curl_URL *u;

  सचे(!in)
    पटिनिवत्त रिक्त;

  u = curlx_calloc(1, परिमाण(संरचना Curl_URL));
  सचे(u) {
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
  पटिनिवत्त u;
fail:
  curl_url_cleanup(u);
  पटिनिवत्त रिक्त;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#अञ्ञथा
स्थिर CURLUcode host_decode(ध्रुव वर्ण *host, वर्ण **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  सचे(result)
    पटिनिवत्त (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode host_encode(ध्रुव वर्ण *host, वर्ण **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  सचे(result)
    पटिनिवत्त (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  पटिनिवत्त CURLUE_OK;
}
#endif

स्थिर CURLUcode urlget_format(ध्रुव CURLU *u, CURLUPart what,
                               ध्रुव वर्ण *ptr, वर्ण **partp,
                               bool plusdecode, unsigned पूर्णाङ्क flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  वर्ण *part = curlx_memdup0(ptr, partlen);
  *partp = रिक्त;
  सचे(!part)
    पटिनिवत्त CURLUE_OUT_OF_MEMORY;
  सचे(plusdecode) {
    /* convert + to space */
    वर्ण *plus = part;
    size_t i = 0;
    अत्थाय(i = 0; i < partlen; ++plus, i++) {
      सचे(*plus == '+')
        *plus = ' ';
    }
  }
  सचे(urldecode) {
    वर्ण *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    सचे(result)
      पटिनिवत्त CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  सचे(urlencode) {
    संरचना dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_निवेशः_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    सचे(uc)
      पटिनिवत्त uc;
    part = curlx_dyn_ptr(&enc);
  }
  अञ्ञथा सचे(punycode) {
    सचे(!Curl_is_ASCII_name(u->host)) {
      वर्ण *punyversion = रिक्त;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      सचे(uc)
        पटिनिवत्त uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      सचे(uc) {
        curlx_free(punyversion);
        पटिनिवत्त uc;
      }
      part = punyversion;
    }
  }
  अञ्ञथा सचे(depunyfy && Curl_is_ASCII_name(u->host)) {
    वर्ण *unpunified = रिक्त;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    सचे(uc)
      पटिनिवत्त uc;
    part = unpunified;
  }
  *partp = part;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode file_url(ध्रुव CURLU *u, वर्ण **part,
                          ध्रुव वर्ण *fragmentsep,
                          ध्रुव वर्ण *querysep)
{
  वर्ण *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  सचे(!url)
    पटिनिवत्त CURLUE_OUT_OF_MEMORY;

  *part = url;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode urlget_url(ध्रुव CURLU *u, वर्ण **part, unsigned पूर्णाङ्क flags)
{
  वर्ण *url;
  वर्ण *allochost = रिक्त;
  ध्रुव वर्ण *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  ध्रुव वर्ण *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  वर्ण portbuf[7];
  सचे(curl_strequal("file", u->scheme))
    पटिनिवत्त file_url(u, part, fragmentsep, querysep);
  अञ्ञथा सचे(!u->host)
    पटिनिवत्त CURLUE_NO_HOST;
  अञ्ञथा {
    ध्रुव वर्ण *scheme;
    वर्ण *options = u->options;
    वर्ण *port = रिक्त;
    ध्रुव संरचना Curl_scheme *h = रिक्त;
    वर्ण schemebuf[MAX_SCHEME_LEN + 5];
    सचे(u->scheme)
      scheme = u->scheme;
    अञ्ञथा सचे(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    अञ्ञथा
      पटिनिवत्त CURLUE_NO_SCHEME;

    सचे(u->port_present) {
      curl_msnprintf(portbuf, परिमाण(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    सचे(h) {
      सचे(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, परिमाण(portbuf), "%u", h->defport);
        port = portbuf;
      }
      अञ्ञथा सचे(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = रिक्त;
      }

      सचे(!(h->flags & PROTOPT_URLOPTIONS))
        options = रिक्त;
    }

    सचे(u->host[0] == '[') {
      सचे(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        संरचना dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_निवेशः_LENGTH);
        सचे(curlx_dyn_addf(&enc, "%.*s%%25%s]", (पूर्णाङ्क)hostlen - 1, u->host,
                          u->zoneid))
          पटिनिवत्त CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    अञ्ञथा सचे(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(रिक्त, u->host, 0);
      सचे(!allochost)
        पटिनिवत्त CURLUE_OUT_OF_MEMORY;
    }
    अञ्ञथा सचे(flags & CURLU_PUNYCODE) {
      सचे(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        सचे(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        सचे(ret) {
          curlx_safefree(allochost);
          पटिनिवत्त ret;
        }
      }
    }
    अञ्ञथा सचे(flags & CURLU_PUNY2IDN) {
      सचे(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        सचे(ret)
          पटिनिवत्त ret;
      }
    }

    सचे(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, परिमाण(schemebuf), "%s://", scheme);
    अञ्ञथा
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
  सचे(!url)
    पटिनिवत्त CURLUE_OUT_OF_MEMORY;
  *part = url;
  पटिनिवत्त CURLUE_OK;
}

CURLUcode curl_url_get(ध्रुव CURLU *u, CURLUPart what,
                       वर्ण **part, unsigned पूर्णाङ्क flags)
{
  ध्रुव वर्ण *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  वर्ण portbuf[7];
  bool plusdecode = FALSE;
  सचे(!u)
    पटिनिवत्त CURLUE_BAD_HANDLE;
  सचे(!part)
    पटिनिवत्त CURLUE_BAD_PARTPOINTER;
  *part = रिक्त;

  विकल्प(what) {
  पक्ष CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    सचे((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      पटिनिवत्त CURLUE_NO_SCHEME;
    भङ्ग;
  पक्ष CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    भङ्ग;
  पक्ष CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    भङ्ग;
  पक्ष CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    भङ्ग;
  पक्ष CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    भङ्ग;
  पक्ष CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    भङ्ग;
  पक्ष CURLUPART_PORT:
    ptr = रिक्त;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    सचे(u->port_present) {
      ध्रुव संरचना Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : रिक्त;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      सचे(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = रिक्त;
      }
      अञ्ञथा {
        curl_msnprintf(portbuf, परिमाण(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    अञ्ञथा सचे((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ध्रुव संरचना Curl_scheme *h = Curl_get_scheme(u->scheme);
      सचे(h) {
        curl_msnprintf(portbuf, परिमाण(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    भङ्ग;
  पक्ष CURLUPART_PATH:
    ptr = u->path;
    सचे(!ptr)
      ptr = "/";
    भङ्ग;
  पक्ष CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    सचे(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = रिक्त;
    भङ्ग;
  पक्ष CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    सचे(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    भङ्ग;
  पक्ष CURLUPART_URL:
    पटिनिवत्त urlget_url(u, part, flags);
  स्वभाव:
    ptr = रिक्त;
    भङ्ग;
  }
  सचे(ptr)
    पटिनिवत्त urlget_format(u, what, ptr, part, plusdecode, flags);

  पटिनिवत्त ifmissing;
}

स्थिर CURLUcode set_url_scheme(CURLU *u, ध्रुव वर्ण *scheme,
                                unsigned पूर्णाङ्क flags)
{
  size_t plen = strlen(scheme);
  ध्रुव संरचना Curl_scheme *h = रिक्त;
  सचे((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    पटिनिवत्त CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  सचे(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    पटिनिवत्त CURLUE_UNSUPPORTED_SCHEME;
  सचे(!h) {
    ध्रुव वर्ण *s = scheme;
    सचे(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      याव(--plen) {
        सचे(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        अञ्ञथा
          पटिनिवत्त CURLUE_BAD_SCHEME;
      }
    }
    अञ्ञथा
      पटिनिवत्त CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode set_url_port(CURLU *u, ध्रुव वर्ण *provided_port)
{
  curl_off_t port;
  सचे(!ISDIGIT(provided_port[0]))
    /* not a number */
    पटिनिवत्त CURLUE_BAD_PORT_सङ्ख्या;
  सचे(curlx_str_सङ्ख्या(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    पटिनिवत्त CURLUE_BAD_PORT_सङ्ख्या;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode set_url(CURLU *u, ध्रुव वर्ण *url, size_t part_size,
                         unsigned पूर्णाङ्क flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  वर्ण *oldurl = रिक्त;

  सचे(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    सचे(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      पटिनिवत्त CURLUE_OK;
    }
    सचे(uc == CURLUE_OUT_OF_MEMORY)
      पटिनिवत्त uc;
    पटिनिवत्त CURLUE_MALFORMED_निवेशः;
  }

  /* if the new URL is absolute replace the existing with the new. */
  सचे(Curl_is_absolute_url(url, रिक्त, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    पटिनिवत्त parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  सचे(uc == CURLUE_OUT_OF_MEMORY)
    पटिनिवत्त uc;
  अञ्ञथा सचे(uc)
    पटिनिवत्त parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  पटिनिवत्त uc;
}

स्थिर CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  विकल्प(what) {
  पक्ष CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, परिमाण(संरचना Curl_URL));
    भङ्ग;
  पक्ष CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    भङ्ग;
  पक्ष CURLUPART_USER:
    curlx_safefree(u->user);
    भङ्ग;
  पक्ष CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    भङ्ग;
  पक्ष CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    भङ्ग;
  पक्ष CURLUPART_HOST:
    curlx_safefree(u->host);
    भङ्ग;
  पक्ष CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    भङ्ग;
  पक्ष CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    भङ्ग;
  पक्ष CURLUPART_PATH:
    curlx_safefree(u->path);
    भङ्ग;
  पक्ष CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    भङ्ग;
  पक्ष CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    भङ्ग;
  स्वभाव:
    पटिनिवत्त CURLUE_UNKNOWN_PART;
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर bool allowed_in_path(unsigned वर्ण x)
{
  विकल्प(x) {
  पक्ष '!':
  पक्ष '$':
  पक्ष '&':
  पक्ष '\'':
  पक्ष '(':
  पक्ष ')':
  पक्ष '{':
  पक्ष '}':
  पक्ष '[':
  पक्ष ']':
  पक्ष '*':
  पक्ष '+':
  पक्ष ',':
  पक्ष ';':
  पक्ष '=':
  पक्ष ':':
  पक्ष '@':
  पक्ष '/':
    पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}

स्थिर CURLUcode url_encode_part(संरचना dynbuf *encp,
                                 ध्रुव वर्ण *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ध्रुव unsigned वर्ण *i;

  अत्थाय(i = (ध्रुव unsigned वर्ण *)part; *i; i++) {
    CURLcode result;
    सचे((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    अञ्ञथा सचे(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      सचे((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    अञ्ञथा {
      unsigned वर्ण out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    सचे(result)
      पटिनिवत्त cc2cu(result);
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode url_uppercasehex_part(संरचना dynbuf *encp,
                                       ध्रुव वर्ण *part)
{
  वर्ण *p;
  CURLcode result = curlx_dyn_add(encp, part);
  सचे(result)
    पटिनिवत्त cc2cu(result);
  p = curlx_dyn_ptr(encp);
  याव(*p) {
    /* make sure percent encoded are upper case */
    सचे((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    अञ्ञथा
      p++;
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode url_append_query(CURLU *u, संरचना dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  सचे(querylen) {
    संरचना dynbuf qbuf;
    CURLcode result;
    ध्रुव वर्ण *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_निवेशः_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    सचे(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    सचे(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    सचे(result)
      गच्छ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    पटिनिवत्त CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    पटिनिवत्त cc2cu(result);
  }
  अञ्ञथा {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  पटिनिवत्त CURLUE_OK;
}

स्थिर CURLUcode url_sethost(CURLU *u, संरचना dynbuf *encp,
                             bool urlencode,
                             unsigned पूर्णाङ्क flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  वर्ण *newp = curlx_dyn_ptr(encp);
  सचे(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  अञ्ञथा सचे(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    वर्ण *decoded = रिक्त;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    सचे(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    अञ्ञथा {
      संरचना dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_निवेशः_LENGTH);
      सचे(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      अञ्ञथा सचे(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        सचे(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  अञ्ञथा सचे(hostname_check6(u, newp, n))
    bad = TRUE;
  अञ्ञथा सचे(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  सचे(bad) {
    curlx_dyn_free(encp);
    पटिनिवत्त CURLUE_BAD_HOSTNAME;
  }
  पटिनिवत्त CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ध्रुव वर्ण *part, unsigned पूर्णाङ्क flags)
{
  वर्ण **storep = रिक्त;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  सचे(!u)
    पटिनिवत्त CURLUE_BAD_HANDLE;
  सचे(!part)
    /* setting a part to NULL clears it */
    पटिनिवत्त urlset_clear(u, what);

  nalloc = strlen(part);
  सचे(nalloc > CURL_MAX_निवेशः_LENGTH)
    /* excessive input length */
    पटिनिवत्त CURLUE_MALFORMED_निवेशः;

  विकल्प(what) {
  पक्ष CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    सचे(status)
      पटिनिवत्त status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    भङ्ग;
  }
  पक्ष CURLUPART_USER:
    storep = &u->user;
    भङ्ग;
  पक्ष CURLUPART_PASSWORD:
    storep = &u->password;
    भङ्ग;
  पक्ष CURLUPART_OPTIONS:
    storep = &u->options;
    भङ्ग;
  पक्ष CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    भङ्ग;
  पक्ष CURLUPART_ZONEID:
    storep = &u->zoneid;
    भङ्ग;
  पक्ष CURLUPART_PORT:
    पटिनिवत्त set_url_port(u, part);
  पक्ष CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    भङ्ग;
  पक्ष CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    भङ्ग;
  पक्ष CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    भङ्ग;
  पक्ष CURLUPART_URL:
    पटिनिवत्त set_url(u, part, nalloc, flags);
  स्वभाव:
    पटिनिवत्त CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ध्रुव वर्ण *newp = रिक्त;
    संरचना dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    सचे(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      सचे(result)
        पटिनिवत्त cc2cu(result);
    }
    सचे(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    अञ्ञथा
      status = url_uppercasehex_part(&enc, part);
    सचे(!status) {
      newp = curlx_dyn_ptr(&enc);

      सचे(appendquery && newp)
        पटिनिवत्त url_append_query(u, &enc);
      अञ्ञथा सचे(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    सचे(status)
      पटिनिवत्त status;

    सचे(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (वर्ण *)CURL_UNCONST(newp);
  }
  पटिनिवत्त CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ध्रुव संरचना Curl_scheme *s = रिक्त;

  /* base must be an absolute URL */
  सचे(!base->scheme || !base->host)
    पटिनिवत्त FALSE;
  सचे(href->scheme && !curl_strequal(base->scheme, href->scheme))
    पटिनिवत्त FALSE;
  सचे(href->host) {
    सचे(!curl_strequal(base->host, href->host))
      पटिनिवत्त FALSE;

    सचे(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      सचे(!s) /* Cannot match default port for unknown scheme */
        पटिनिवत्त FALSE;
      /* to match, the present one must be the default port */
      सचे((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        पटिनिवत्त FALSE;
    }
    अञ्ञथा सचे(base->portnum != href->portnum) /* both present or missing */
      पटिनिवत्त FALSE;

    सचे(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      पटिनिवत्त FALSE;
  }
  अञ्ञथा सचे(href->port_present) /* no host in href, then there must be no port */
    पटिनिवत्त FALSE;
  पटिनिवत्त TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  सचे(u->port_present) {
    *pport = u->portnum;
    पटिनिवत्त CURLUE_OK;
  }
  अञ्ञथा सचे(u->scheme) {
    ध्रुव संरचना Curl_scheme *s = Curl_get_scheme(u->scheme);
    सचे(s && s->defport) {
      *pport = s->defport;
      पटिनिवत्त CURLUE_OK;
    }
  }
  *pport = 0;
  पटिनिवत्त CURLUE_NO_PORT;
}
