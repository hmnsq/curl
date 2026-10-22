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
#जर !defined(USE_IPV6) && !defined(AF_INET6)
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
स्थिर स्थिरांक अक्षर *find_host_sep(स्थिरांक अक्षर *url)
{
  /* Find the start of the hostname */
  स्थिरांक अक्षर *sep = म_माला(url, "//");
  जर(!sep)
    sep = url;
  अन्यथा
    sep += 2;

  /* Find first / or ? */
  पर्यंत(*sep && *sep != '/' && *sep != '?')
    sep++;

  परत sep;
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
UNITTEST CURLUcode urlencode_str(संरचना dynbuf *o, स्थिरांक अक्षर *url,
                                 माप_प्रकार len, bool relative,
                                 अचिन्हित पूर्णांक query);
UNITTEST CURLUcode urlencode_str(संरचना dynbuf *o, स्थिरांक अक्षर *url,
                                 माप_प्रकार len, bool relative,
                                 अचिन्हित पूर्णांक query)
{
  /* we must add this with whitespace-replacing */
  स्थिरांक अचिन्हित अक्षर *iptr;
  स्थिरांक अचिन्हित अक्षर *host_sep = (स्थिरांक अचिन्हित अक्षर *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  जर(!relative) {
    माप_प्रकार n;
    host_sep = (स्थिरांक अचिन्हित अक्षर *)find_host_sep(url);

    /* output the first piece as-is */
    n = (स्थिरांक अक्षर *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  साठी(iptr = host_sep; len && !result;) {
    जर(*iptr == ' ') {
      जर(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      अन्यथा
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    अन्यथा जर((*iptr < ' ') || (*iptr >= 0x7f)) {
      अचिन्हित अक्षर out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    अन्यथा जर(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      अचिन्हित अक्षर hex = (अचिन्हित अक्षर)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      अचिन्हित अक्षर out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    अन्यथा {
      स्थिरांक अचिन्हित अक्षर *start = iptr;
      पर्यंत(len) {
        जर(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          थांबा;
        जर(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          थांबा;
        जर(*iptr == '?') {
          जर(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            थांबा;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (स्थिरांक अक्षर *)start, (माप_प्रकार)(iptr - start));
    }
  }

  जर(result)
    परत cc2cu(result);
  परत CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
माप_प्रकार Curl_is_absolute_url(स्थिरांक अक्षर *url, अक्षर *buf, माप_प्रकार buflen,
                            bool guess_scheme)
{
  माप_प्रकार i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (शून्य)buflen; /* only used in debug-builds */
  जर(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  जर(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    परत 0;
#endif
  जर(ISALPHA(url[0])) {
    जर(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    साठी(i = 1; i < MAX_SCHEME_LEN; ++i) {
      अक्षर s = url[i];
      जर(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        जर(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      अन्यथा {
        थांबा;
      }
    }
  }
  जर(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    माप_प्रकार len = i;
    जर(buf)
      buf[i] = 0;
    परत len;
  }
  जर(buf)
    buf[0] = 0;
  परत 0;
}

/* scan for byte values <= 31, 127 and maybe space */
स्थिर bool badoctets(स्थिरांक अक्षर *आदान, माप_प्रकार n, पूर्णांक flags)
{
  स्थिरांक uint8_t *p = (स्थिरांक अचिन्हित अक्षर *)आदान;
  स्थिरांक uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  पर्यंत(n--) {
    जर(*p <= control || *p == 127)
      परत TRUE;
    p++;
  }
  परत FALSE;
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
                                        स्थिरांक अक्षर *login,
                                        माप_प्रकार len,
                                        अचिन्हित पूर्णांक flags,
                                        माप_प्रकार *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(संरचना Curl_URL *u,
                                        स्थिरांक अक्षर *login,
                                        माप_प्रकार len,
                                        अचिन्हित पूर्णांक flags,
                                        माप_प्रकार *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  अक्षर *userp = रिक्त;
  अक्षर *passwdp = रिक्त;
  अक्षर *optionsp = रिक्त;
  स्थिरांक संरचना Curl_scheme *h = रिक्त;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  स्थिरांक अक्षर *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = स_प्रथम(login, '@', len);
  जर(!ptr)
    जा out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  जर(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : रिक्त);
  जर(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    जा out;
  }

  जर(userp) {
    जर(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      जा out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  जर(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  जर(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  जर(userp && badoctets(userp, म_माप(userp), flags))
    ures = CURLUE_BAD_USER;
  अन्यथा जर(passwdp && badoctets(passwdp, म_माप(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  अन्यथा जर(optionsp && badoctets(optionsp, म_माप(optionsp), flags))
    ures = CURLUE_MALFORMED_आदान;

  userp = passwdp = optionsp = रिक्त;

  जर(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    परत CURLUE_OK;
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

  परत ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(संरचना Curl_URL *u, संरचना dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(संरचना Curl_URL *u, संरचना dynbuf *host,
                              bool has_scheme)
{
  स्थिरांक अक्षर *portptr;
  स्थिरांक अक्षर *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  जर(hostname[0] == '[') {
    portptr = स_प्रथम(hostname + 1, ']', curlx_dyn_len(host) - 1);
    जर(!portptr)
      परत CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    जर(*portptr) {
      जर(*portptr != ':')
        परत CURLUE_BAD_PORT_संख्या;
    }
    अन्यथा
      portptr = रिक्त;
  }
  अन्यथा
    portptr = स_प्रथम(hostname, ':', curlx_dyn_len(host));

  जर(portptr) {
    curl_off_t port;
    माप_प्रकार keep = portptr - hostname;
    पूर्णांक rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    जर(!*portptr)
      परत has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_संख्या;
    जर(*portptr == '\\')
      परत CURLUE_BACKSLASH;
    rc = curlx_str_संख्या(&portptr, &port, 0xffff);
    जर(rc)
      परत CURLUE_BAD_PORT_संख्या;
    अन्यथा जर(*portptr == '\\')
      परत CURLUE_BACKSLASH;
    अन्यथा जर(*portptr)
      परत CURLUE_BAD_PORT_संख्या;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  परत CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

स्थिर स्थिरांक bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(संरचना Curl_URL *u, अक्षर *hostname,
                              माप_प्रकार hlen);
UNITTEST CURLUcode ipv6_parse(संरचना Curl_URL *u, अक्षर *hostname,
                              माप_प्रकार hlen) /* length of hostname */
{
  माप_प्रकार len;
  DEBUGASSERT(*hostname == '[');
  जर(hlen < 4) /* '[::]' is the shortest possible valid string */
    परत CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = म_अखोज(hostname, "0123456789abcdefABCDEF:.");

  जर(hlen != len) {
    hlen = len;
    जर(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      अक्षर zoneid[MAX_ZONEID_LEN];
      पूर्णांक i = 0;
      अक्षर *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      जर(!म_भेदन(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      पर्यंत(!invalid_host_char[(अचिन्हित अक्षर)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      जर(!i || (']' != *h))
        परत CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      जर(!u->zoneid)
        परत CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    अन्यथा
      परत CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    अक्षर dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    जर(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      परत CURLUE_BAD_IPV6;
    जर(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = म_माप(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  परत CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
स्थिर CURLUcode hostname_check(अक्षर *hostname, माप_प्रकार hlen)
{
  माप_प्रकार i;
  साठी(i = 0; i < hlen; i++) {
    जर(invalid_host_char[(अचिन्हित अक्षर)hostname[i]])
      परत CURLUE_BAD_HOSTNAME;
  }
  जर((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    परत CURLUE_BAD_HOSTNAME;
  अन्यथा जर((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    परत CURLUE_BAD_HOSTNAME;
  परत CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
स्थिर CURLUcode hostname_check6(संरचना Curl_URL *u, अक्षर *hostname,
                                 माप_प्रकार hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  जर(!hlen)
    परत CURLUE_NO_HOST;
  अन्यथा जर(hostname[0] == '[')
    परत ipv6_parse(u, hostname, hlen);

  परत hostname_check(hostname, hlen);
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
UNITTEST पूर्णांक ipv4_normalize(संरचना dynbuf *host);
UNITTEST पूर्णांक ipv4_normalize(संरचना dynbuf *host)
{
  bool done = FALSE;
  पूर्णांक n = 0;
  स्थिरांक अक्षर *c = curlx_dyn_ptr(host);
  अचिन्हित पूर्णांक parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  जर(!ISDIGIT(*c))
    परत HOST_NAME;

  पर्यंत(!done) {
    पूर्णांक rc;
    curl_off_t l;
    जर(*c == '0') {
      जर((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, अच_पूर्णांक_उच्च);
        जर(rc)
          परत HOST_NAME;
      }
      अन्यथा
        rc = curlx_str_octal(&c, &l, अच_पूर्णांक_उच्च);
    }
    अन्यथा
      rc = curlx_str_संख्या(&c, &l, अच_पूर्णांक_उच्च);

    जर(rc) {
      जर(!n || (rc != STRE_NO_अंक) || *c)
        परत HOST_NAME;
      n--;
    }
    अन्यथा
      parts[n] = (अचिन्हित पूर्णांक)l;

    निवड(*c) {
    स्थिती '.':
      जर(n == 3) {
        जर(c[1])
          /* something follows this dot */
          परत HOST_NAME;
        done = TRUE;
      }
      अन्यथा {
        n++;
        c++;
      }
      थांबा;

    स्थिती '\0':
      done = TRUE;
      थांबा;

    पूर्वनिर्धारित:
      परत HOST_NAME;
    }
  }

  निवड(n) {
  स्थिती 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    थांबा;
  स्थिती 1: /* a.b -- 8.24 bits */
    जर((parts[0] > 0xff) || (parts[1] > 0xffffff))
      परत HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    थांबा;
  स्थिती 2: /* a.b.c -- 8.8.16 bits */
    जर((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      परत HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    थांबा;
  स्थिती 3: /* a.b.c.d -- 8.8.8.8 bits */
    जर((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      परत HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    थांबा;
  }
  जर(result)
    परत HOST_ERROR;
  परत HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
स्थिर CURLUcode urldecode_host(संरचना dynbuf *host)
{
  स्थिरांक अक्षर *per;
  स्थिरांक अक्षर *hostname = curlx_dyn_ptr(host);
  per = स_प्रथम(hostname, '%', curlx_dyn_len(host));
  जर(!per)
    /* nothing to decode */
    परत CURLUE_OK;
  अन्यथा {
    /* encoded */
    माप_प्रकार dlen;
    अक्षर *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    जर(result)
      परत CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    जर(result)
      परत cc2cu(result);
  }

  परत CURLUE_OK;
}

स्थिर CURLUcode parse_authority(संरचना Curl_URL *u,
                                 स्थिरांक अक्षर *auth, माप_प्रकार authlen,
                                 अचिन्हित पूर्णांक flags,
                                 संरचना dynbuf *host,
                                 bool has_scheme)
{
  माप_प्रकार offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  जर(uc)
    परत uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  जर(result) {
    uc = cc2cu(result);
    परत uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  जर(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  जर(!uc)
    uc = urldecode_host(host);
  जर(uc)
    ;
  अन्यथा जर(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  अन्यथा {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    पूर्णांक type = ipv4_normalize(host);

    जर(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    अन्यथा जर(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  परत uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, स्थिरांक अक्षर *authority)
{
  CURLUcode ures;
  संरचना dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_आदान_LENGTH);

  ures = parse_authority(u, authority, म_माप(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  जर(ures)
    curlx_dyn_free(&host);
  अन्यथा {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  परत ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

स्थिर bool is_dot(स्थिरांक अक्षर **str, माप_प्रकार *clen)
{
  स्थिरांक अक्षर *p = *str;
  जर(*p == '.') {
    (*str)++;
    (*clen)--;
    परत TRUE;
  }
  अन्यथा जर((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    परत TRUE;
  }
  परत FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
स्थिर bool needs_dedotdot(स्थिरांक अक्षर *p, माप_प्रकार pn)
{
  /* a single byte path cannot be cleaned up */
  जर(pn < 2)
    परत FALSE;
  जर(!स_प्रथम(p, '.', pn) && !स_प्रथम(p, '%', pn))
    परत FALSE;
  पर्यंत(pn) {
    जर(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      जर(!pn || ISSLASH(*p))
        परत TRUE;
      /* "../" or ".." before end of string */
      अन्यथा जर(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        परत TRUE;
    }
    अन्यथा {
      p++;
      pn--;
    }
  }
  परत FALSE;
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
UNITTEST पूर्णांक dedotdotify(स्थिरांक अक्षर *आदान, माप_प्रकार clen, अक्षर **outp);
UNITTEST पूर्णांक dedotdotify(स्थिरांक अक्षर *आदान, माप_प्रकार clen, अक्षर **outp)
{
  संरचना dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  स्थिरांक अक्षर *dinput = आदान;
  माप_प्रकार dlen = clen;

  *outp = रिक्त;
  जर(!needs_dedotdot(आदान, clen))
    परत 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  जर(is_dot(&dinput, &dlen)) {
    जर(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      आदान = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    अन्यथा जर(is_dot(&dinput, &dlen)) {
      जर(!dlen)
        /* .. [end] */
        जा end;
      अन्यथा जर(ISSLASH(*dinput)) {
        /* ../ */
        आदान = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  पर्यंत(clen && !result) { /* until end of path content */
    जर(ISSLASH(*आदान)) {
      स्थिरांक अक्षर *p = &आदान[1];
      माप_प्रकार blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      जर(is_dot(&p, &blen)) {
        जर(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          थांबा;
        }
        अन्यथा जर(ISSLASH(*p)) { /* /./ */
          आदान = p;
          clen = blen;
          सुरूठेवा;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        अन्यथा जर(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          माप_प्रकार len = curlx_dyn_len(&out);
          जर(len) {
            स्थिरांक अक्षर *ptr = curlx_dyn_ptr(&out);
            स्थिरांक अक्षर *last = memrchr(ptr, '/', len);
            जर(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          जर(blen) { /* /../ */
            आदान = p;
            clen = blen;
            सुरूठेवा;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          थांबा;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, आदान, 1);
    आदान++;
    clen--;
  }
end:
  जर(!result) {
    जर(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    अन्यथा {
      *outp = curlx_strdup("");
      जर(!*outp)
        परत 1;
    }
  }
  परत result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(स्थिरांक अक्षर *url, माप_प्रकार urllen, CURLU *u,
                              स्थिरांक अक्षर **pathp, माप_प्रकार *pathlenp);
UNITTEST CURLUcode parse_file(स्थिरांक अक्षर *url, माप_प्रकार urllen, CURLU *u,
                              स्थिरांक अक्षर **pathp, माप_प्रकार *pathlenp)
{
  स्थिरांक अक्षर *path;
  माप_प्रकार pathlen;

  *pathp = रिक्त;
  *pathlenp = 0;
  जर(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    परत CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  जर(path[0] != '/')
    परत CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  जर(path[1] == '/') {
    /* swallow the two slashes */
    स्थिरांक अक्षर *ptr = &path[2];

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
    जर(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      जर(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      अन्यथा
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        परत CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#जर !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  जर(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    परत CURLUE_BAD_FILE_URL;
  }
#अन्यथा
  /* If the path starts with a slash and a drive letter, ditch the slash */
  जर('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  जर(!u->scheme)
    परत CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  परत CURLUE_OK;
}

स्थिर CURLUcode parse_scheme(स्थिरांक अक्षर *url, CURLU *u, अक्षर *schemebuf,
                              माप_प्रकार schemelen, अचिन्हित पूर्णांक flags,
                              स्थिरांक अक्षर **hostpp)
{
  /* clear path */
  स्थिरांक अक्षर *schemep = रिक्त;

  जर(schemelen) {
    पूर्णांक अंक_slashes = 0;
    स्थिरांक अक्षर *p = &url[schemelen + 1];
    जर(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      परत CURLUE_UNSUPPORTED_SCHEME;

    जर(!ISSLASH(*p))
      /* less than one */
      परत CURLUE_BAD_SLASHES;
    जर((flags & CURLU_NO_AUTHORITY)) {
      पर्यंत(ISSLASH(*p) && (अंक_slashes < 2)) {
        p++;
        अंक_slashes++;
      }
    }
    अन्यथा {
      पर्यंत(ISSLASH(*p) && (अंक_slashes < 4)) {
        p++;
        अंक_slashes++;
      }
      जर(अंक_slashes > 3)
        परत CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  अन्यथा {
    /* no scheme! */

    जर(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      परत CURLUE_BAD_SCHEME;

    जर(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  जर(schemep) {
    u->scheme = curlx_strdup(schemep);
    जर(!u->scheme)
      परत CURLUE_OUT_OF_MEMORY;
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode guess_scheme(CURLU *u, संरचना dynbuf *host)
{
  स्थिरांक अक्षर *hostname = curlx_dyn_ptr(host);
  स्थिरांक अक्षर *schemep = रिक्त;
  /* legacy curl-style guess based on hostname */
  जर(checkprefix("ftp.", hostname))
    schemep = "ftp";
  अन्यथा जर(checkprefix("dict.", hostname))
    schemep = "dict";
  अन्यथा जर(checkprefix("ldap.", hostname))
    schemep = "ldap";
  अन्यथा जर(checkprefix("imap.", hostname))
    schemep = "imap";
  अन्यथा जर(checkprefix("smtp.", hostname))
    schemep = "smtp";
  अन्यथा जर(checkprefix("pop3.", hostname))
    schemep = "pop3";
  अन्यथा
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  जर(!u->scheme)
    परत CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  परत CURLUE_OK;
}

स्थिर CURLUcode handle_fragment(CURLU *u, स्थिरांक अक्षर *fragment,
                                 माप_प्रकार fraglen, अचिन्हित पूर्णांक flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  जर(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    जर(flags & CURLU_URLENCODE) {
      संरचना dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_आदान_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      जर(ures)
        परत ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    अन्यथा {
      जर(badoctets(fragment, fraglen, flags))
        परत CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      जर(!u->fragment)
        परत CURLUE_OUT_OF_MEMORY;
    }
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode handle_query(CURLU *u, स्थिरांक अक्षर *query,
                              माप_प्रकार qlen, अचिन्हित पूर्णांक flags)
{
  u->query_present = TRUE;
  जर(qlen > 1) {
    जर(flags & CURLU_URLENCODE) {
      संरचना dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_आदान_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      जर(ures)
        परत ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    अन्यथा {
      जर(badoctets(query, qlen, flags))
        परत CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      जर(!u->query)
        परत CURLUE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    /* single byte query */
    u->query = curlx_strdup("");
    जर(!u->query)
      परत CURLUE_OUT_OF_MEMORY;
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode handle_path(CURLU *u, स्थिरांक अक्षर *path,
                             माप_प्रकार pathlen, अचिन्हित पूर्णांक flags,
                             bool is_file)
{
  CURLUcode ures;
  जर(pathlen && (flags & CURLU_URLENCODE)) {
    संरचना dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_आदान_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    जर(ures)
      परत ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  जर(pathlen >= (माप_प्रकार)(1 + !is_file)) {
    जर(badoctets(path, pathlen, flags))
      परत CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    जर(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      जर(!u->path)
        परत CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    अन्यथा जर(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    जर(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      अक्षर *dedot;
      पूर्णांक err = dedotdotify(path, pathlen, &dedot);
      जर(err)
        परत CURLUE_OUT_OF_MEMORY;
      जर(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode parseurl(स्थिरांक अक्षर *url, CURLU *u, अचिन्हित पूर्णांक flags)
{
  स्थिरांक अक्षर *path;
  माप_प्रकार pathlen;
  अक्षर schemebuf[MAX_SCHEME_LEN + 1];
  माप_प्रकार schemelen = 0;
  माप_प्रकार urllen;
  CURLUcode ures = CURLUE_OK;
  संरचना dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = म_माप(url);
  जर(urllen > CURL_MAX_आदान_LENGTH)
    परत CURLUE_MALFORMED_आदान;

  curlx_dyn_init(&host, CURL_MAX_आदान_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, आकार(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  जर(schemelen == 4 && !स_भेद(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  अन्यथा {
    स्थिरांक अक्षर *hostp = रिक्त;
    स्थिरांक अक्षर *p;
    माप_प्रकार hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    जर(ures)
      जा fail;

    /* find the end of the hostname + port number */
    p = hostp;
    पर्यंत(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    जर(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      जर(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    अन्यथा जर(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      जर(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    अन्यथा
      ures = CURLUE_NO_HOST;
  }
  जर(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    स्थिरांक अक्षर *fragment = स_प्रथम(path, '#', pathlen);
    जर(fragment) {
      माप_प्रकार fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  जर(!ures) {
    स्थिरांक अक्षर *query = स_प्रथम(path, '?', pathlen);
    जर(query) {
      माप_प्रकार qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  जर(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  जर(!ures) {
    u->host = curlx_dyn_ptr(&host);
    परत CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  परत ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
स्थिर CURLUcode parseurl_and_replace(स्थिरांक अक्षर *url, CURLU *u,
                                      अचिन्हित पूर्णांक flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  स_रखो(&tmpurl, 0, आकार(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  जर(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  परत ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
स्थिर CURLUcode redirect_url(स्थिरांक अक्षर *base, स्थिरांक अक्षर *relurl,
                              CURLU *u, अचिन्हित पूर्णांक flags)
{
  संरचना dynbuf urlbuf;
  bool host_changed = FALSE;
  स्थिरांक अक्षर *useurl = relurl;
  स्थिरांक अक्षर *cutoff = रिक्त;
  माप_प्रकार prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  स्थिरांक अक्षर *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  स्थिरांक अक्षर *protsep = base + म_माप(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  जर(!base)
    परत CURLUE_MALFORMED_आदान; /* should never happen */

  /* handle different relative URL types */
  निवड(relurl[0]) {
  स्थिती '/':
    जर(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    अन्यथा
      /* absolute /path */
      cutoff = म_अक्षर(protsep, '/');
    थांबा;

  स्थिती '#':
    /* fragment-only change */
    जर(u->fragment_present)
      cutoff = म_अक्षर(protsep, '#');
    थांबा;

  पूर्वनिर्धारित:
    /* path or query-only change */
    जर(u->query_present)
      /* remove existing query */
      cutoff = म_अक्षर(protsep, '?');
    अन्यथा जर(u->fragment_present)
      /* Remove existing fragment */
      cutoff = म_अक्षर(protsep, '#');

    जर(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (माप_प्रकार)(cutoff - protsep) : म_माप(protsep));
      जर(cutoff)
        cutoff++; /* truncate after last slash */
    }
    थांबा;
  }

  prelen = cutoff ? (माप_प्रकार)(cutoff - base) : म_माप(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_आदान_LENGTH);

  जर(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, म_माप(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  अन्यथा
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  परत uc;
}

/*
 */
CURLU *curl_url(शून्य)
{
  परत curlx_calloc(1, आकार(संरचना Curl_URL));
}

शून्य curl_url_cleanup(CURLU *u)
{
  जर(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  करा {                                          \
    जर((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      जर(!(dest)->name)                         \
        जा fail;                              \
    }                                           \
  } पर्यंत(0)

CURLU *curl_url_dup(स्थिरांक CURLU *in)
{
  संरचना Curl_URL *u;

  जर(!in)
    परत रिक्त;

  u = curlx_calloc(1, आकार(संरचना Curl_URL));
  जर(u) {
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
  परत u;
fail:
  curl_url_cleanup(u);
  परत रिक्त;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#अन्यथा
स्थिर CURLUcode host_decode(स्थिरांक अक्षर *host, अक्षर **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  जर(result)
    परत (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  परत CURLUE_OK;
}

स्थिर CURLUcode host_encode(स्थिरांक अक्षर *host, अक्षर **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  जर(result)
    परत (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  परत CURLUE_OK;
}
#endif

स्थिर CURLUcode urlget_format(स्थिरांक CURLU *u, CURLUPart what,
                               स्थिरांक अक्षर *ptr, अक्षर **partp,
                               bool plusdecode, अचिन्हित पूर्णांक flags)
{
  CURLUcode uc = CURLUE_OK;
  माप_प्रकार partlen = म_माप(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  अक्षर *part = curlx_memdup0(ptr, partlen);
  *partp = रिक्त;
  जर(!part)
    परत CURLUE_OUT_OF_MEMORY;
  जर(plusdecode) {
    /* convert + to space */
    अक्षर *plus = part;
    माप_प्रकार i = 0;
    साठी(i = 0; i < partlen; ++plus, i++) {
      जर(*plus == '+')
        *plus = ' ';
    }
  }
  जर(urldecode) {
    अक्षर *decoded;
    माप_प्रकार dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    जर(result)
      परत CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  जर(urlencode) {
    संरचना dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_आदान_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    जर(uc)
      परत uc;
    part = curlx_dyn_ptr(&enc);
  }
  अन्यथा जर(punycode) {
    जर(!Curl_is_ASCII_name(u->host)) {
      अक्षर *punyversion = रिक्त;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      जर(uc)
        परत uc;
      uc = hostname_check(punyversion, म_माप(punyversion));
      जर(uc) {
        curlx_free(punyversion);
        परत uc;
      }
      part = punyversion;
    }
  }
  अन्यथा जर(depunyfy && Curl_is_ASCII_name(u->host)) {
    अक्षर *unpunified = रिक्त;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    जर(uc)
      परत uc;
    part = unpunified;
  }
  *partp = part;
  परत CURLUE_OK;
}

स्थिर CURLUcode file_url(स्थिरांक CURLU *u, अक्षर **part,
                          स्थिरांक अक्षर *fragmentsep,
                          स्थिरांक अक्षर *querysep)
{
  अक्षर *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  जर(!url)
    परत CURLUE_OUT_OF_MEMORY;

  *part = url;
  परत CURLUE_OK;
}

स्थिर CURLUcode urlget_url(स्थिरांक CURLU *u, अक्षर **part, अचिन्हित पूर्णांक flags)
{
  अक्षर *url;
  अक्षर *allochost = रिक्त;
  स्थिरांक अक्षर *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  स्थिरांक अक्षर *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  अक्षर portbuf[7];
  जर(curl_strequal("file", u->scheme))
    परत file_url(u, part, fragmentsep, querysep);
  अन्यथा जर(!u->host)
    परत CURLUE_NO_HOST;
  अन्यथा {
    स्थिरांक अक्षर *scheme;
    अक्षर *options = u->options;
    अक्षर *port = रिक्त;
    स्थिरांक संरचना Curl_scheme *h = रिक्त;
    अक्षर schemebuf[MAX_SCHEME_LEN + 5];
    जर(u->scheme)
      scheme = u->scheme;
    अन्यथा जर(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    अन्यथा
      परत CURLUE_NO_SCHEME;

    जर(u->port_present) {
      curl_msnprintf(portbuf, आकार(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    जर(h) {
      जर(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, आकार(portbuf), "%u", h->defport);
        port = portbuf;
      }
      अन्यथा जर(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = रिक्त;
      }

      जर(!(h->flags & PROTOPT_URLOPTIONS))
        options = रिक्त;
    }

    जर(u->host[0] == '[') {
      जर(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        संरचना dynbuf enc;
        माप_प्रकार hostlen = म_माप(u->host);
        curlx_dyn_init(&enc, CURL_MAX_आदान_LENGTH);
        जर(curlx_dyn_addf(&enc, "%.*s%%25%s]", (पूर्णांक)hostlen - 1, u->host,
                          u->zoneid))
          परत CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    अन्यथा जर(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(रिक्त, u->host, 0);
      जर(!allochost)
        परत CURLUE_OUT_OF_MEMORY;
    }
    अन्यथा जर(flags & CURLU_PUNYCODE) {
      जर(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        जर(!ret)
          ret = hostname_check(allochost, म_माप(allochost));
        जर(ret) {
          curlx_safefree(allochost);
          परत ret;
        }
      }
    }
    अन्यथा जर(flags & CURLU_PUNY2IDN) {
      जर(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        जर(ret)
          परत ret;
      }
    }

    जर(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, आकार(schemebuf), "%s://", scheme);
    अन्यथा
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
  जर(!url)
    परत CURLUE_OUT_OF_MEMORY;
  *part = url;
  परत CURLUE_OK;
}

CURLUcode curl_url_get(स्थिरांक CURLU *u, CURLUPart what,
                       अक्षर **part, अचिन्हित पूर्णांक flags)
{
  स्थिरांक अक्षर *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  अक्षर portbuf[7];
  bool plusdecode = FALSE;
  जर(!u)
    परत CURLUE_BAD_HANDLE;
  जर(!part)
    परत CURLUE_BAD_PARTPOINTER;
  *part = रिक्त;

  निवड(what) {
  स्थिती CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    जर((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      परत CURLUE_NO_SCHEME;
    थांबा;
  स्थिती CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    थांबा;
  स्थिती CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    थांबा;
  स्थिती CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    थांबा;
  स्थिती CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    थांबा;
  स्थिती CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    थांबा;
  स्थिती CURLUPART_PORT:
    ptr = रिक्त;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    जर(u->port_present) {
      स्थिरांक संरचना Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : रिक्त;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      जर(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = रिक्त;
      }
      अन्यथा {
        curl_msnprintf(portbuf, आकार(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    अन्यथा जर((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      स्थिरांक संरचना Curl_scheme *h = Curl_get_scheme(u->scheme);
      जर(h) {
        curl_msnprintf(portbuf, आकार(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    थांबा;
  स्थिती CURLUPART_PATH:
    ptr = u->path;
    जर(!ptr)
      ptr = "/";
    थांबा;
  स्थिती CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    जर(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = रिक्त;
    थांबा;
  स्थिती CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    जर(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    थांबा;
  स्थिती CURLUPART_URL:
    परत urlget_url(u, part, flags);
  पूर्वनिर्धारित:
    ptr = रिक्त;
    थांबा;
  }
  जर(ptr)
    परत urlget_format(u, what, ptr, part, plusdecode, flags);

  परत ifmissing;
}

स्थिर CURLUcode set_url_scheme(CURLU *u, स्थिरांक अक्षर *scheme,
                                अचिन्हित पूर्णांक flags)
{
  माप_प्रकार plen = म_माप(scheme);
  स्थिरांक संरचना Curl_scheme *h = रिक्त;
  जर((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    परत CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  जर(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    परत CURLUE_UNSUPPORTED_SCHEME;
  जर(!h) {
    स्थिरांक अक्षर *s = scheme;
    जर(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      पर्यंत(--plen) {
        जर(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        अन्यथा
          परत CURLUE_BAD_SCHEME;
      }
    }
    अन्यथा
      परत CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  परत CURLUE_OK;
}

स्थिर CURLUcode set_url_port(CURLU *u, स्थिरांक अक्षर *provided_port)
{
  curl_off_t port;
  जर(!ISDIGIT(provided_port[0]))
    /* not a number */
    परत CURLUE_BAD_PORT_संख्या;
  जर(curlx_str_संख्या(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    परत CURLUE_BAD_PORT_संख्या;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  परत CURLUE_OK;
}

स्थिर CURLUcode set_url(CURLU *u, स्थिरांक अक्षर *url, माप_प्रकार part_size,
                         अचिन्हित पूर्णांक flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  अक्षर *oldurl = रिक्त;

  जर(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    जर(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      परत CURLUE_OK;
    }
    जर(uc == CURLUE_OUT_OF_MEMORY)
      परत uc;
    परत CURLUE_MALFORMED_आदान;
  }

  /* if the new URL is absolute replace the existing with the new. */
  जर(Curl_is_absolute_url(url, रिक्त, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    परत parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  जर(uc == CURLUE_OUT_OF_MEMORY)
    परत uc;
  अन्यथा जर(uc)
    परत parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  परत uc;
}

स्थिर CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  निवड(what) {
  स्थिती CURLUPART_URL:
    free_urlhandle(u);
    स_रखो(u, 0, आकार(संरचना Curl_URL));
    थांबा;
  स्थिती CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    थांबा;
  स्थिती CURLUPART_USER:
    curlx_safefree(u->user);
    थांबा;
  स्थिती CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    थांबा;
  स्थिती CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    थांबा;
  स्थिती CURLUPART_HOST:
    curlx_safefree(u->host);
    थांबा;
  स्थिती CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    थांबा;
  स्थिती CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    थांबा;
  स्थिती CURLUPART_PATH:
    curlx_safefree(u->path);
    थांबा;
  स्थिती CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    थांबा;
  स्थिती CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    थांबा;
  पूर्वनिर्धारित:
    परत CURLUE_UNKNOWN_PART;
  }
  परत CURLUE_OK;
}

स्थिर bool allowed_in_path(अचिन्हित अक्षर x)
{
  निवड(x) {
  स्थिती '!':
  स्थिती '$':
  स्थिती '&':
  स्थिती '\'':
  स्थिती '(':
  स्थिती ')':
  स्थिती '{':
  स्थिती '}':
  स्थिती '[':
  स्थिती ']':
  स्थिती '*':
  स्थिती '+':
  स्थिती ',':
  स्थिती ';':
  स्थिती '=':
  स्थिती ':':
  स्थिती '@':
  स्थिती '/':
    परत TRUE;
  }
  परत FALSE;
}

स्थिर CURLUcode url_encode_part(संरचना dynbuf *encp,
                                 स्थिरांक अक्षर *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  स्थिरांक अचिन्हित अक्षर *i;

  साठी(i = (स्थिरांक अचिन्हित अक्षर *)part; *i; i++) {
    CURLcode result;
    जर((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    अन्यथा जर(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      जर((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    अन्यथा {
      अचिन्हित अक्षर out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    जर(result)
      परत cc2cu(result);
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode url_uppercasehex_part(संरचना dynbuf *encp,
                                       स्थिरांक अक्षर *part)
{
  अक्षर *p;
  CURLcode result = curlx_dyn_add(encp, part);
  जर(result)
    परत cc2cu(result);
  p = curlx_dyn_ptr(encp);
  पर्यंत(*p) {
    /* make sure percent encoded are upper case */
    जर((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    अन्यथा
      p++;
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode url_append_query(CURLU *u, संरचना dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  माप_प्रकार querylen = u->query ? म_माप(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  जर(querylen) {
    संरचना dynbuf qbuf;
    CURLcode result;
    स्थिरांक अक्षर *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_आदान_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    जर(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    जर(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    जर(result)
      जा nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    परत CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    परत cc2cu(result);
  }
  अन्यथा {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  परत CURLUE_OK;
}

स्थिर CURLUcode url_sethost(CURLU *u, संरचना dynbuf *encp,
                             bool urlencode,
                             अचिन्हित पूर्णांक flags)
{
  माप_प्रकार n = curlx_dyn_len(encp);
  bool bad = FALSE;
  अक्षर *newp = curlx_dyn_ptr(encp);
  जर(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  अन्यथा जर(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    माप_प्रकार dlen;
    अक्षर *decoded = रिक्त;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    जर(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    अन्यथा {
      संरचना dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_आदान_LENGTH);
      जर(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      अन्यथा जर(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        जर(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  अन्यथा जर(hostname_check6(u, newp, n))
    bad = TRUE;
  अन्यथा जर(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  जर(bad) {
    curlx_dyn_free(encp);
    परत CURLUE_BAD_HOSTNAME;
  }
  परत CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       स्थिरांक अक्षर *part, अचिन्हित पूर्णांक flags)
{
  अक्षर **storep = रिक्त;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  माप_प्रकार nalloc;

  जर(!u)
    परत CURLUE_BAD_HANDLE;
  जर(!part)
    /* setting a part to NULL clears it */
    परत urlset_clear(u, what);

  nalloc = म_माप(part);
  जर(nalloc > CURL_MAX_आदान_LENGTH)
    /* excessive input length */
    परत CURLUE_MALFORMED_आदान;

  निवड(what) {
  स्थिती CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    जर(status)
      परत status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    थांबा;
  }
  स्थिती CURLUPART_USER:
    storep = &u->user;
    थांबा;
  स्थिती CURLUPART_PASSWORD:
    storep = &u->password;
    थांबा;
  स्थिती CURLUPART_OPTIONS:
    storep = &u->options;
    थांबा;
  स्थिती CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    थांबा;
  स्थिती CURLUPART_ZONEID:
    storep = &u->zoneid;
    थांबा;
  स्थिती CURLUPART_PORT:
    परत set_url_port(u, part);
  स्थिती CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    थांबा;
  स्थिती CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    थांबा;
  स्थिती CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    थांबा;
  स्थिती CURLUPART_URL:
    परत set_url(u, part, nalloc, flags);
  पूर्वनिर्धारित:
    परत CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    स्थिरांक अक्षर *newp = रिक्त;
    संरचना dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    जर(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      जर(result)
        परत cc2cu(result);
    }
    जर(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    अन्यथा
      status = url_uppercasehex_part(&enc, part);
    जर(!status) {
      newp = curlx_dyn_ptr(&enc);

      जर(appendquery && newp)
        परत url_append_query(u, &enc);
      अन्यथा जर(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    जर(status)
      परत status;

    जर(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (अक्षर *)CURL_UNCONST(newp);
  }
  परत CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  स्थिरांक संरचना Curl_scheme *s = रिक्त;

  /* base must be an absolute URL */
  जर(!base->scheme || !base->host)
    परत FALSE;
  जर(href->scheme && !curl_strequal(base->scheme, href->scheme))
    परत FALSE;
  जर(href->host) {
    जर(!curl_strequal(base->host, href->host))
      परत FALSE;

    जर(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      जर(!s) /* Cannot match default port for unknown scheme */
        परत FALSE;
      /* to match, the present one must be the default port */
      जर((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        परत FALSE;
    }
    अन्यथा जर(base->portnum != href->portnum) /* both present or missing */
      परत FALSE;

    जर(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      परत FALSE;
  }
  अन्यथा जर(href->port_present) /* no host in href, then there must be no port */
    परत FALSE;
  परत TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  जर(u->port_present) {
    *pport = u->portnum;
    परत CURLUE_OK;
  }
  अन्यथा जर(u->scheme) {
    स्थिरांक संरचना Curl_scheme *s = Curl_get_scheme(u->scheme);
    जर(s && s->defport) {
      *pport = s->defport;
      परत CURLUE_OK;
    }
  }
  *pport = 0;
  परत CURLUE_NO_PORT;
}
