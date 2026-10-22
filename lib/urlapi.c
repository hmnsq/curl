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
#यदि !defined(USE_IPV6) && !defined(AF_INET6)
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
  ध्रुव वर्ण *sep = म_माला(url, "//");
  यदि(!sep)
    sep = url;
  अन्यथा
    sep += 2;

  /* Find first / or ? */
  यावत्(*sep && *sep != '/' && *sep != '?')
    sep++;

  प्रत्यागम sep;
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
                                 माप_प्रकार len, bool relative,
                                 अचिन्हित पूर्णाङ्क query);
UNITTEST CURLUcode urlencode_str(संरचना dynbuf *o, ध्रुव वर्ण *url,
                                 माप_प्रकार len, bool relative,
                                 अचिन्हित पूर्णाङ्क query)
{
  /* we must add this with whitespace-replacing */
  ध्रुव अचिन्हित वर्ण *iptr;
  ध्रुव अचिन्हित वर्ण *host_sep = (ध्रुव अचिन्हित वर्ण *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  यदि(!relative) {
    माप_प्रकार n;
    host_sep = (ध्रुव अचिन्हित वर्ण *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ध्रुव वर्ण *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  प्रति(iptr = host_sep; len && !result;) {
    यदि(*iptr == ' ') {
      यदि(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      अन्यथा
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    अन्यथा यदि((*iptr < ' ') || (*iptr >= 0x7f)) {
      अचिन्हित वर्ण out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    अन्यथा यदि(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      अचिन्हित वर्ण hex = (अचिन्हित वर्ण)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      अचिन्हित वर्ण out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    अन्यथा {
      ध्रुव अचिन्हित वर्ण *start = iptr;
      यावत्(len) {
        यदि(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          भङ्ग;
        यदि(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          भङ्ग;
        यदि(*iptr == '?') {
          यदि(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            भङ्ग;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ध्रुव वर्ण *)start, (माप_प्रकार)(iptr - start));
    }
  }

  यदि(result)
    प्रत्यागम cc2cu(result);
  प्रत्यागम CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
माप_प्रकार Curl_is_absolute_url(ध्रुव वर्ण *url, वर्ण *buf, माप_प्रकार buflen,
                            bool guess_scheme)
{
  माप_प्रकार i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (शून्य)buflen; /* only used in debug-builds */
  यदि(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  यदि(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    प्रत्यागम 0;
#endif
  यदि(ISALPHA(url[0])) {
    यदि(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    प्रति(i = 1; i < MAX_SCHEME_LEN; ++i) {
      वर्ण s = url[i];
      यदि(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        यदि(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      अन्यथा {
        भङ्ग;
      }
    }
  }
  यदि(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    माप_प्रकार len = i;
    यदि(buf)
      buf[i] = 0;
    प्रत्यागम len;
  }
  यदि(buf)
    buf[0] = 0;
  प्रत्यागम 0;
}

/* scan for byte values <= 31, 127 and maybe space */
स्थिर bool badoctets(ध्रुव वर्ण *निवेश, माप_प्रकार n, पूर्णाङ्क flags)
{
  ध्रुव uint8_t *p = (ध्रुव अचिन्हित वर्ण *)निवेश;
  ध्रुव uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  यावत्(n--) {
    यदि(*p <= control || *p == 127)
      प्रत्यागम TRUE;
    p++;
  }
  प्रत्यागम FALSE;
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
                                        माप_प्रकार len,
                                        अचिन्हित पूर्णाङ्क flags,
                                        माप_प्रकार *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(संरचना Curl_URL *u,
                                        ध्रुव वर्ण *login,
                                        माप_प्रकार len,
                                        अचिन्हित पूर्णाङ्क flags,
                                        माप_प्रकार *hostname_offset)
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
  ptr = स_प्रथम(login, '@', len);
  यदि(!ptr)
    गच्छ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  यदि(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : रिक्त);
  यदि(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    गच्छ out;
  }

  यदि(userp) {
    यदि(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      गच्छ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  यदि(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  यदि(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  यदि(userp && badoctets(userp, म_माप(userp), flags))
    ures = CURLUE_BAD_USER;
  अन्यथा यदि(passwdp && badoctets(passwdp, म_माप(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  अन्यथा यदि(optionsp && badoctets(optionsp, म_माप(optionsp), flags))
    ures = CURLUE_MALFORMED_निवेश;

  userp = passwdp = optionsp = रिक्त;

  यदि(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    प्रत्यागम CURLUE_OK;
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

  प्रत्यागम ures;
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
  यदि(hostname[0] == '[') {
    portptr = स_प्रथम(hostname + 1, ']', curlx_dyn_len(host) - 1);
    यदि(!portptr)
      प्रत्यागम CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    यदि(*portptr) {
      यदि(*portptr != ':')
        प्रत्यागम CURLUE_BAD_PORT_संख्या;
    }
    अन्यथा
      portptr = रिक्त;
  }
  अन्यथा
    portptr = स_प्रथम(hostname, ':', curlx_dyn_len(host));

  यदि(portptr) {
    curl_off_t port;
    माप_प्रकार keep = portptr - hostname;
    पूर्णाङ्क rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    यदि(!*portptr)
      प्रत्यागम has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_संख्या;
    यदि(*portptr == '\\')
      प्रत्यागम CURLUE_BACKSLASH;
    rc = curlx_str_संख्या(&portptr, &port, 0xffff);
    यदि(rc)
      प्रत्यागम CURLUE_BAD_PORT_संख्या;
    अन्यथा यदि(*portptr == '\\')
      प्रत्यागम CURLUE_BACKSLASH;
    अन्यथा यदि(*portptr)
      प्रत्यागम CURLUE_BAD_PORT_संख्या;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  प्रत्यागम CURLUE_OK;
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
                              माप_प्रकार hlen);
UNITTEST CURLUcode ipv6_parse(संरचना Curl_URL *u, वर्ण *hostname,
                              माप_प्रकार hlen) /* length of hostname */
{
  माप_प्रकार len;
  DEBUGASSERT(*hostname == '[');
  यदि(hlen < 4) /* '[::]' is the shortest possible valid string */
    प्रत्यागम CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = म_अखोज(hostname, "0123456789abcdefABCDEF:.");

  यदि(hlen != len) {
    hlen = len;
    यदि(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      वर्ण zoneid[MAX_ZONEID_LEN];
      पूर्णाङ्क i = 0;
      वर्ण *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      यदि(!म_भेदन(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      यावत्(!invalid_host_char[(अचिन्हित वर्ण)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      यदि(!i || (']' != *h))
        प्रत्यागम CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      यदि(!u->zoneid)
        प्रत्यागम CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    अन्यथा
      प्रत्यागम CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    वर्ण dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    यदि(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      प्रत्यागम CURLUE_BAD_IPV6;
    यदि(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = म_माप(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  प्रत्यागम CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
स्थिर CURLUcode hostname_check(वर्ण *hostname, माप_प्रकार hlen)
{
  माप_प्रकार i;
  प्रति(i = 0; i < hlen; i++) {
    यदि(invalid_host_char[(अचिन्हित वर्ण)hostname[i]])
      प्रत्यागम CURLUE_BAD_HOSTNAME;
  }
  यदि((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    प्रत्यागम CURLUE_BAD_HOSTNAME;
  अन्यथा यदि((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    प्रत्यागम CURLUE_BAD_HOSTNAME;
  प्रत्यागम CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
स्थिर CURLUcode hostname_check6(संरचना Curl_URL *u, वर्ण *hostname,
                                 माप_प्रकार hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  यदि(!hlen)
    प्रत्यागम CURLUE_NO_HOST;
  अन्यथा यदि(hostname[0] == '[')
    प्रत्यागम ipv6_parse(u, hostname, hlen);

  प्रत्यागम hostname_check(hostname, hlen);
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
  अचिन्हित पूर्णाङ्क parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  यदि(!ISDIGIT(*c))
    प्रत्यागम HOST_NAME;

  यावत्(!done) {
    पूर्णाङ्क rc;
    curl_off_t l;
    यदि(*c == '0') {
      यदि((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, अच_पूर्णांक_उच्च);
        यदि(rc)
          प्रत्यागम HOST_NAME;
      }
      अन्यथा
        rc = curlx_str_octal(&c, &l, अच_पूर्णांक_उच्च);
    }
    अन्यथा
      rc = curlx_str_संख्या(&c, &l, अच_पूर्णांक_उच्च);

    यदि(rc) {
      यदि(!n || (rc != STRE_NO_अंक) || *c)
        प्रत्यागम HOST_NAME;
      n--;
    }
    अन्यथा
      parts[n] = (अचिन्हित पूर्णाङ्क)l;

    विकल्प(*c) {
    पक्ष '.':
      यदि(n == 3) {
        यदि(c[1])
          /* something follows this dot */
          प्रत्यागम HOST_NAME;
        done = TRUE;
      }
      अन्यथा {
        n++;
        c++;
      }
      भङ्ग;

    पक्ष '\0':
      done = TRUE;
      भङ्ग;

    स्वभाव:
      प्रत्यागम HOST_NAME;
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
    यदि((parts[0] > 0xff) || (parts[1] > 0xffffff))
      प्रत्यागम HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    भङ्ग;
  पक्ष 2: /* a.b.c -- 8.8.16 bits */
    यदि((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      प्रत्यागम HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    भङ्ग;
  पक्ष 3: /* a.b.c.d -- 8.8.8.8 bits */
    यदि((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      प्रत्यागम HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    भङ्ग;
  }
  यदि(result)
    प्रत्यागम HOST_ERROR;
  प्रत्यागम HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
स्थिर CURLUcode urldecode_host(संरचना dynbuf *host)
{
  ध्रुव वर्ण *per;
  ध्रुव वर्ण *hostname = curlx_dyn_ptr(host);
  per = स_प्रथम(hostname, '%', curlx_dyn_len(host));
  यदि(!per)
    /* nothing to decode */
    प्रत्यागम CURLUE_OK;
  अन्यथा {
    /* encoded */
    माप_प्रकार dlen;
    वर्ण *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    यदि(result)
      प्रत्यागम CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    यदि(result)
      प्रत्यागम cc2cu(result);
  }

  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode parse_authority(संरचना Curl_URL *u,
                                 ध्रुव वर्ण *auth, माप_प्रकार authlen,
                                 अचिन्हित पूर्णाङ्क flags,
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
  यदि(uc)
    प्रत्यागम uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  यदि(result) {
    uc = cc2cu(result);
    प्रत्यागम uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  यदि(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  यदि(!uc)
    uc = urldecode_host(host);
  यदि(uc)
    ;
  अन्यथा यदि(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  अन्यथा {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    पूर्णाङ्क type = ipv4_normalize(host);

    यदि(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    अन्यथा यदि(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  प्रत्यागम uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ध्रुव वर्ण *authority)
{
  CURLUcode ures;
  संरचना dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_निवेश_LENGTH);

  ures = parse_authority(u, authority, म_माप(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  यदि(ures)
    curlx_dyn_free(&host);
  अन्यथा {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  प्रत्यागम ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

स्थिर bool is_dot(ध्रुव वर्ण **str, माप_प्रकार *clen)
{
  ध्रुव वर्ण *p = *str;
  यदि(*p == '.') {
    (*str)++;
    (*clen)--;
    प्रत्यागम TRUE;
  }
  अन्यथा यदि((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
स्थिर bool needs_dedotdot(ध्रुव वर्ण *p, माप_प्रकार pn)
{
  /* a single byte path cannot be cleaned up */
  यदि(pn < 2)
    प्रत्यागम FALSE;
  यदि(!स_प्रथम(p, '.', pn) && !स_प्रथम(p, '%', pn))
    प्रत्यागम FALSE;
  यावत्(pn) {
    यदि(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      यदि(!pn || ISSLASH(*p))
        प्रत्यागम TRUE;
      /* "../" or ".." before end of string */
      अन्यथा यदि(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        प्रत्यागम TRUE;
    }
    अन्यथा {
      p++;
      pn--;
    }
  }
  प्रत्यागम FALSE;
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
UNITTEST पूर्णाङ्क dedotdotify(ध्रुव वर्ण *निवेश, माप_प्रकार clen, वर्ण **outp);
UNITTEST पूर्णाङ्क dedotdotify(ध्रुव वर्ण *निवेश, माप_प्रकार clen, वर्ण **outp)
{
  संरचना dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ध्रुव वर्ण *dinput = निवेश;
  माप_प्रकार dlen = clen;

  *outp = रिक्त;
  यदि(!needs_dedotdot(निवेश, clen))
    प्रत्यागम 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  यदि(is_dot(&dinput, &dlen)) {
    यदि(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      निवेश = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    अन्यथा यदि(is_dot(&dinput, &dlen)) {
      यदि(!dlen)
        /* .. [end] */
        गच्छ end;
      अन्यथा यदि(ISSLASH(*dinput)) {
        /* ../ */
        निवेश = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  यावत्(clen && !result) { /* until end of path content */
    यदि(ISSLASH(*निवेश)) {
      ध्रुव वर्ण *p = &निवेश[1];
      माप_प्रकार blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      यदि(is_dot(&p, &blen)) {
        यदि(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          भङ्ग;
        }
        अन्यथा यदि(ISSLASH(*p)) { /* /./ */
          निवेश = p;
          clen = blen;
          अनुवर्त;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        अन्यथा यदि(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          माप_प्रकार len = curlx_dyn_len(&out);
          यदि(len) {
            ध्रुव वर्ण *ptr = curlx_dyn_ptr(&out);
            ध्रुव वर्ण *last = memrchr(ptr, '/', len);
            यदि(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          यदि(blen) { /* /../ */
            निवेश = p;
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

    result = curlx_dyn_addn(&out, निवेश, 1);
    निवेश++;
    clen--;
  }
end:
  यदि(!result) {
    यदि(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    अन्यथा {
      *outp = curlx_strdup("");
      यदि(!*outp)
        प्रत्यागम 1;
    }
  }
  प्रत्यागम result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ध्रुव वर्ण *url, माप_प्रकार urllen, CURLU *u,
                              ध्रुव वर्ण **pathp, माप_प्रकार *pathlenp);
UNITTEST CURLUcode parse_file(ध्रुव वर्ण *url, माप_प्रकार urllen, CURLU *u,
                              ध्रुव वर्ण **pathp, माप_प्रकार *pathlenp)
{
  ध्रुव वर्ण *path;
  माप_प्रकार pathlen;

  *pathp = रिक्त;
  *pathlenp = 0;
  यदि(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    प्रत्यागम CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  यदि(path[0] != '/')
    प्रत्यागम CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  यदि(path[1] == '/') {
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
    यदि(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      यदि(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      अन्यथा
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        प्रत्यागम CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#यदि !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  यदि(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    प्रत्यागम CURLUE_BAD_FILE_URL;
  }
#अन्यथा
  /* If the path starts with a slash and a drive letter, ditch the slash */
  यदि('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  यदि(!u->scheme)
    प्रत्यागम CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode parse_scheme(ध्रुव वर्ण *url, CURLU *u, वर्ण *schemebuf,
                              माप_प्रकार schemelen, अचिन्हित पूर्णाङ्क flags,
                              ध्रुव वर्ण **hostpp)
{
  /* clear path */
  ध्रुव वर्ण *schemep = रिक्त;

  यदि(schemelen) {
    पूर्णाङ्क अंक_slashes = 0;
    ध्रुव वर्ण *p = &url[schemelen + 1];
    यदि(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      प्रत्यागम CURLUE_UNSUPPORTED_SCHEME;

    यदि(!ISSLASH(*p))
      /* less than one */
      प्रत्यागम CURLUE_BAD_SLASHES;
    यदि((flags & CURLU_NO_AUTHORITY)) {
      यावत्(ISSLASH(*p) && (अंक_slashes < 2)) {
        p++;
        अंक_slashes++;
      }
    }
    अन्यथा {
      यावत्(ISSLASH(*p) && (अंक_slashes < 4)) {
        p++;
        अंक_slashes++;
      }
      यदि(अंक_slashes > 3)
        प्रत्यागम CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  अन्यथा {
    /* no scheme! */

    यदि(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      प्रत्यागम CURLUE_BAD_SCHEME;

    यदि(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  यदि(schemep) {
    u->scheme = curlx_strdup(schemep);
    यदि(!u->scheme)
      प्रत्यागम CURLUE_OUT_OF_MEMORY;
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode guess_scheme(CURLU *u, संरचना dynbuf *host)
{
  ध्रुव वर्ण *hostname = curlx_dyn_ptr(host);
  ध्रुव वर्ण *schemep = रिक्त;
  /* legacy curl-style guess based on hostname */
  यदि(checkprefix("ftp.", hostname))
    schemep = "ftp";
  अन्यथा यदि(checkprefix("dict.", hostname))
    schemep = "dict";
  अन्यथा यदि(checkprefix("ldap.", hostname))
    schemep = "ldap";
  अन्यथा यदि(checkprefix("imap.", hostname))
    schemep = "imap";
  अन्यथा यदि(checkprefix("smtp.", hostname))
    schemep = "smtp";
  अन्यथा यदि(checkprefix("pop3.", hostname))
    schemep = "pop3";
  अन्यथा
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  यदि(!u->scheme)
    प्रत्यागम CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode handle_fragment(CURLU *u, ध्रुव वर्ण *fragment,
                                 माप_प्रकार fraglen, अचिन्हित पूर्णाङ्क flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  यदि(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    यदि(flags & CURLU_URLENCODE) {
      संरचना dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_निवेश_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      यदि(ures)
        प्रत्यागम ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    अन्यथा {
      यदि(badoctets(fragment, fraglen, flags))
        प्रत्यागम CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      यदि(!u->fragment)
        प्रत्यागम CURLUE_OUT_OF_MEMORY;
    }
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode handle_query(CURLU *u, ध्रुव वर्ण *query,
                              माप_प्रकार qlen, अचिन्हित पूर्णाङ्क flags)
{
  u->query_present = TRUE;
  यदि(qlen > 1) {
    यदि(flags & CURLU_URLENCODE) {
      संरचना dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_निवेश_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      यदि(ures)
        प्रत्यागम ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    अन्यथा {
      यदि(badoctets(query, qlen, flags))
        प्रत्यागम CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      यदि(!u->query)
        प्रत्यागम CURLUE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    /* single byte query */
    u->query = curlx_strdup("");
    यदि(!u->query)
      प्रत्यागम CURLUE_OUT_OF_MEMORY;
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode handle_path(CURLU *u, ध्रुव वर्ण *path,
                             माप_प्रकार pathlen, अचिन्हित पूर्णाङ्क flags,
                             bool is_file)
{
  CURLUcode ures;
  यदि(pathlen && (flags & CURLU_URLENCODE)) {
    संरचना dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_निवेश_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    यदि(ures)
      प्रत्यागम ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  यदि(pathlen >= (माप_प्रकार)(1 + !is_file)) {
    यदि(badoctets(path, pathlen, flags))
      प्रत्यागम CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    यदि(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      यदि(!u->path)
        प्रत्यागम CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    अन्यथा यदि(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    यदि(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      वर्ण *dedot;
      पूर्णाङ्क err = dedotdotify(path, pathlen, &dedot);
      यदि(err)
        प्रत्यागम CURLUE_OUT_OF_MEMORY;
      यदि(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode parseurl(ध्रुव वर्ण *url, CURLU *u, अचिन्हित पूर्णाङ्क flags)
{
  ध्रुव वर्ण *path;
  माप_प्रकार pathlen;
  वर्ण schemebuf[MAX_SCHEME_LEN + 1];
  माप_प्रकार schemelen = 0;
  माप_प्रकार urllen;
  CURLUcode ures = CURLUE_OK;
  संरचना dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = म_माप(url);
  यदि(urllen > CURL_MAX_निवेश_LENGTH)
    प्रत्यागम CURLUE_MALFORMED_निवेश;

  curlx_dyn_init(&host, CURL_MAX_निवेश_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, परिमाण(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  यदि(schemelen == 4 && !स_भेद(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  अन्यथा {
    ध्रुव वर्ण *hostp = रिक्त;
    ध्रुव वर्ण *p;
    माप_प्रकार hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    यदि(ures)
      गच्छ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    यावत्(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    यदि(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      यदि(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    अन्यथा यदि(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      यदि(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    अन्यथा
      ures = CURLUE_NO_HOST;
  }
  यदि(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ध्रुव वर्ण *fragment = स_प्रथम(path, '#', pathlen);
    यदि(fragment) {
      माप_प्रकार fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  यदि(!ures) {
    ध्रुव वर्ण *query = स_प्रथम(path, '?', pathlen);
    यदि(query) {
      माप_प्रकार qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  यदि(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  यदि(!ures) {
    u->host = curlx_dyn_ptr(&host);
    प्रत्यागम CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  प्रत्यागम ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
स्थिर CURLUcode parseurl_and_replace(ध्रुव वर्ण *url, CURLU *u,
                                      अचिन्हित पूर्णाङ्क flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  स_रखो(&tmpurl, 0, परिमाण(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  यदि(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  प्रत्यागम ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
स्थिर CURLUcode redirect_url(ध्रुव वर्ण *base, ध्रुव वर्ण *relurl,
                              CURLU *u, अचिन्हित पूर्णाङ्क flags)
{
  संरचना dynbuf urlbuf;
  bool host_changed = FALSE;
  ध्रुव वर्ण *useurl = relurl;
  ध्रुव वर्ण *cutoff = रिक्त;
  माप_प्रकार prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ध्रुव वर्ण *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ध्रुव वर्ण *protsep = base + म_माप(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  यदि(!base)
    प्रत्यागम CURLUE_MALFORMED_निवेश; /* should never happen */

  /* handle different relative URL types */
  विकल्प(relurl[0]) {
  पक्ष '/':
    यदि(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    अन्यथा
      /* absolute /path */
      cutoff = म_अक्षर(protsep, '/');
    भङ्ग;

  पक्ष '#':
    /* fragment-only change */
    यदि(u->fragment_present)
      cutoff = म_अक्षर(protsep, '#');
    भङ्ग;

  स्वभाव:
    /* path or query-only change */
    यदि(u->query_present)
      /* remove existing query */
      cutoff = म_अक्षर(protsep, '?');
    अन्यथा यदि(u->fragment_present)
      /* Remove existing fragment */
      cutoff = म_अक्षर(protsep, '#');

    यदि(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (माप_प्रकार)(cutoff - protsep) : म_माप(protsep));
      यदि(cutoff)
        cutoff++; /* truncate after last slash */
    }
    भङ्ग;
  }

  prelen = cutoff ? (माप_प्रकार)(cutoff - base) : म_माप(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_निवेश_LENGTH);

  यदि(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, म_माप(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  अन्यथा
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  प्रत्यागम uc;
}

/*
 */
CURLU *curl_url(शून्य)
{
  प्रत्यागम curlx_calloc(1, परिमाण(संरचना Curl_URL));
}

शून्य curl_url_cleanup(CURLU *u)
{
  यदि(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  कुरु {                                          \
    यदि((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      यदि(!(dest)->name)                         \
        गच्छ fail;                              \
    }                                           \
  } यावत्(0)

CURLU *curl_url_dup(ध्रुव CURLU *in)
{
  संरचना Curl_URL *u;

  यदि(!in)
    प्रत्यागम रिक्त;

  u = curlx_calloc(1, परिमाण(संरचना Curl_URL));
  यदि(u) {
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
  प्रत्यागम u;
fail:
  curl_url_cleanup(u);
  प्रत्यागम रिक्त;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#अन्यथा
स्थिर CURLUcode host_decode(ध्रुव वर्ण *host, वर्ण **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  यदि(result)
    प्रत्यागम (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode host_encode(ध्रुव वर्ण *host, वर्ण **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  यदि(result)
    प्रत्यागम (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  प्रत्यागम CURLUE_OK;
}
#endif

स्थिर CURLUcode urlget_format(ध्रुव CURLU *u, CURLUPart what,
                               ध्रुव वर्ण *ptr, वर्ण **partp,
                               bool plusdecode, अचिन्हित पूर्णाङ्क flags)
{
  CURLUcode uc = CURLUE_OK;
  माप_प्रकार partlen = म_माप(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  वर्ण *part = curlx_memdup0(ptr, partlen);
  *partp = रिक्त;
  यदि(!part)
    प्रत्यागम CURLUE_OUT_OF_MEMORY;
  यदि(plusdecode) {
    /* convert + to space */
    वर्ण *plus = part;
    माप_प्रकार i = 0;
    प्रति(i = 0; i < partlen; ++plus, i++) {
      यदि(*plus == '+')
        *plus = ' ';
    }
  }
  यदि(urldecode) {
    वर्ण *decoded;
    माप_प्रकार dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    यदि(result)
      प्रत्यागम CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  यदि(urlencode) {
    संरचना dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_निवेश_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    यदि(uc)
      प्रत्यागम uc;
    part = curlx_dyn_ptr(&enc);
  }
  अन्यथा यदि(punycode) {
    यदि(!Curl_is_ASCII_name(u->host)) {
      वर्ण *punyversion = रिक्त;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      यदि(uc)
        प्रत्यागम uc;
      uc = hostname_check(punyversion, म_माप(punyversion));
      यदि(uc) {
        curlx_free(punyversion);
        प्रत्यागम uc;
      }
      part = punyversion;
    }
  }
  अन्यथा यदि(depunyfy && Curl_is_ASCII_name(u->host)) {
    वर्ण *unpunified = रिक्त;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    यदि(uc)
      प्रत्यागम uc;
    part = unpunified;
  }
  *partp = part;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode file_url(ध्रुव CURLU *u, वर्ण **part,
                          ध्रुव वर्ण *fragmentsep,
                          ध्रुव वर्ण *querysep)
{
  वर्ण *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  यदि(!url)
    प्रत्यागम CURLUE_OUT_OF_MEMORY;

  *part = url;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode urlget_url(ध्रुव CURLU *u, वर्ण **part, अचिन्हित पूर्णाङ्क flags)
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
  यदि(curl_strequal("file", u->scheme))
    प्रत्यागम file_url(u, part, fragmentsep, querysep);
  अन्यथा यदि(!u->host)
    प्रत्यागम CURLUE_NO_HOST;
  अन्यथा {
    ध्रुव वर्ण *scheme;
    वर्ण *options = u->options;
    वर्ण *port = रिक्त;
    ध्रुव संरचना Curl_scheme *h = रिक्त;
    वर्ण schemebuf[MAX_SCHEME_LEN + 5];
    यदि(u->scheme)
      scheme = u->scheme;
    अन्यथा यदि(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    अन्यथा
      प्रत्यागम CURLUE_NO_SCHEME;

    यदि(u->port_present) {
      curl_msnprintf(portbuf, परिमाण(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    यदि(h) {
      यदि(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, परिमाण(portbuf), "%u", h->defport);
        port = portbuf;
      }
      अन्यथा यदि(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = रिक्त;
      }

      यदि(!(h->flags & PROTOPT_URLOPTIONS))
        options = रिक्त;
    }

    यदि(u->host[0] == '[') {
      यदि(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        संरचना dynbuf enc;
        माप_प्रकार hostlen = म_माप(u->host);
        curlx_dyn_init(&enc, CURL_MAX_निवेश_LENGTH);
        यदि(curlx_dyn_addf(&enc, "%.*s%%25%s]", (पूर्णाङ्क)hostlen - 1, u->host,
                          u->zoneid))
          प्रत्यागम CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    अन्यथा यदि(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(रिक्त, u->host, 0);
      यदि(!allochost)
        प्रत्यागम CURLUE_OUT_OF_MEMORY;
    }
    अन्यथा यदि(flags & CURLU_PUNYCODE) {
      यदि(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        यदि(!ret)
          ret = hostname_check(allochost, म_माप(allochost));
        यदि(ret) {
          curlx_safefree(allochost);
          प्रत्यागम ret;
        }
      }
    }
    अन्यथा यदि(flags & CURLU_PUNY2IDN) {
      यदि(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        यदि(ret)
          प्रत्यागम ret;
      }
    }

    यदि(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, परिमाण(schemebuf), "%s://", scheme);
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
  यदि(!url)
    प्रत्यागम CURLUE_OUT_OF_MEMORY;
  *part = url;
  प्रत्यागम CURLUE_OK;
}

CURLUcode curl_url_get(ध्रुव CURLU *u, CURLUPart what,
                       वर्ण **part, अचिन्हित पूर्णाङ्क flags)
{
  ध्रुव वर्ण *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  वर्ण portbuf[7];
  bool plusdecode = FALSE;
  यदि(!u)
    प्रत्यागम CURLUE_BAD_HANDLE;
  यदि(!part)
    प्रत्यागम CURLUE_BAD_PARTPOINTER;
  *part = रिक्त;

  विकल्प(what) {
  पक्ष CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    यदि((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      प्रत्यागम CURLUE_NO_SCHEME;
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
    यदि(u->port_present) {
      ध्रुव संरचना Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : रिक्त;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      यदि(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = रिक्त;
      }
      अन्यथा {
        curl_msnprintf(portbuf, परिमाण(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    अन्यथा यदि((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ध्रुव संरचना Curl_scheme *h = Curl_get_scheme(u->scheme);
      यदि(h) {
        curl_msnprintf(portbuf, परिमाण(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    भङ्ग;
  पक्ष CURLUPART_PATH:
    ptr = u->path;
    यदि(!ptr)
      ptr = "/";
    भङ्ग;
  पक्ष CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    यदि(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = रिक्त;
    भङ्ग;
  पक्ष CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    यदि(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    भङ्ग;
  पक्ष CURLUPART_URL:
    प्रत्यागम urlget_url(u, part, flags);
  स्वभाव:
    ptr = रिक्त;
    भङ्ग;
  }
  यदि(ptr)
    प्रत्यागम urlget_format(u, what, ptr, part, plusdecode, flags);

  प्रत्यागम ifmissing;
}

स्थिर CURLUcode set_url_scheme(CURLU *u, ध्रुव वर्ण *scheme,
                                अचिन्हित पूर्णाङ्क flags)
{
  माप_प्रकार plen = म_माप(scheme);
  ध्रुव संरचना Curl_scheme *h = रिक्त;
  यदि((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    प्रत्यागम CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  यदि(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    प्रत्यागम CURLUE_UNSUPPORTED_SCHEME;
  यदि(!h) {
    ध्रुव वर्ण *s = scheme;
    यदि(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      यावत्(--plen) {
        यदि(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        अन्यथा
          प्रत्यागम CURLUE_BAD_SCHEME;
      }
    }
    अन्यथा
      प्रत्यागम CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode set_url_port(CURLU *u, ध्रुव वर्ण *provided_port)
{
  curl_off_t port;
  यदि(!ISDIGIT(provided_port[0]))
    /* not a number */
    प्रत्यागम CURLUE_BAD_PORT_संख्या;
  यदि(curlx_str_संख्या(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    प्रत्यागम CURLUE_BAD_PORT_संख्या;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode set_url(CURLU *u, ध्रुव वर्ण *url, माप_प्रकार part_size,
                         अचिन्हित पूर्णाङ्क flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  वर्ण *oldurl = रिक्त;

  यदि(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    यदि(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      प्रत्यागम CURLUE_OK;
    }
    यदि(uc == CURLUE_OUT_OF_MEMORY)
      प्रत्यागम uc;
    प्रत्यागम CURLUE_MALFORMED_निवेश;
  }

  /* if the new URL is absolute replace the existing with the new. */
  यदि(Curl_is_absolute_url(url, रिक्त, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    प्रत्यागम parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  यदि(uc == CURLUE_OUT_OF_MEMORY)
    प्रत्यागम uc;
  अन्यथा यदि(uc)
    प्रत्यागम parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  प्रत्यागम uc;
}

स्थिर CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  विकल्प(what) {
  पक्ष CURLUPART_URL:
    free_urlhandle(u);
    स_रखो(u, 0, परिमाण(संरचना Curl_URL));
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
    प्रत्यागम CURLUE_UNKNOWN_PART;
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर bool allowed_in_path(अचिन्हित वर्ण x)
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
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर CURLUcode url_encode_part(संरचना dynbuf *encp,
                                 ध्रुव वर्ण *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ध्रुव अचिन्हित वर्ण *i;

  प्रति(i = (ध्रुव अचिन्हित वर्ण *)part; *i; i++) {
    CURLcode result;
    यदि((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    अन्यथा यदि(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      यदि((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    अन्यथा {
      अचिन्हित वर्ण out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    यदि(result)
      प्रत्यागम cc2cu(result);
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode url_uppercasehex_part(संरचना dynbuf *encp,
                                       ध्रुव वर्ण *part)
{
  वर्ण *p;
  CURLcode result = curlx_dyn_add(encp, part);
  यदि(result)
    प्रत्यागम cc2cu(result);
  p = curlx_dyn_ptr(encp);
  यावत्(*p) {
    /* make sure percent encoded are upper case */
    यदि((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    अन्यथा
      p++;
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode url_append_query(CURLU *u, संरचना dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  माप_प्रकार querylen = u->query ? म_माप(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  यदि(querylen) {
    संरचना dynbuf qbuf;
    CURLcode result;
    ध्रुव वर्ण *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_निवेश_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    यदि(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    यदि(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    यदि(result)
      गच्छ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    प्रत्यागम CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    प्रत्यागम cc2cu(result);
  }
  अन्यथा {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  प्रत्यागम CURLUE_OK;
}

स्थिर CURLUcode url_sethost(CURLU *u, संरचना dynbuf *encp,
                             bool urlencode,
                             अचिन्हित पूर्णाङ्क flags)
{
  माप_प्रकार n = curlx_dyn_len(encp);
  bool bad = FALSE;
  वर्ण *newp = curlx_dyn_ptr(encp);
  यदि(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  अन्यथा यदि(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    माप_प्रकार dlen;
    वर्ण *decoded = रिक्त;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    यदि(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    अन्यथा {
      संरचना dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_निवेश_LENGTH);
      यदि(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      अन्यथा यदि(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        यदि(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  अन्यथा यदि(hostname_check6(u, newp, n))
    bad = TRUE;
  अन्यथा यदि(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  यदि(bad) {
    curlx_dyn_free(encp);
    प्रत्यागम CURLUE_BAD_HOSTNAME;
  }
  प्रत्यागम CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ध्रुव वर्ण *part, अचिन्हित पूर्णाङ्क flags)
{
  वर्ण **storep = रिक्त;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  माप_प्रकार nalloc;

  यदि(!u)
    प्रत्यागम CURLUE_BAD_HANDLE;
  यदि(!part)
    /* setting a part to NULL clears it */
    प्रत्यागम urlset_clear(u, what);

  nalloc = म_माप(part);
  यदि(nalloc > CURL_MAX_निवेश_LENGTH)
    /* excessive input length */
    प्रत्यागम CURLUE_MALFORMED_निवेश;

  विकल्प(what) {
  पक्ष CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    यदि(status)
      प्रत्यागम status;
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
    प्रत्यागम set_url_port(u, part);
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
    प्रत्यागम set_url(u, part, nalloc, flags);
  स्वभाव:
    प्रत्यागम CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ध्रुव वर्ण *newp = रिक्त;
    संरचना dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    यदि(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      यदि(result)
        प्रत्यागम cc2cu(result);
    }
    यदि(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    अन्यथा
      status = url_uppercasehex_part(&enc, part);
    यदि(!status) {
      newp = curlx_dyn_ptr(&enc);

      यदि(appendquery && newp)
        प्रत्यागम url_append_query(u, &enc);
      अन्यथा यदि(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    यदि(status)
      प्रत्यागम status;

    यदि(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (वर्ण *)CURL_UNCONST(newp);
  }
  प्रत्यागम CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ध्रुव संरचना Curl_scheme *s = रिक्त;

  /* base must be an absolute URL */
  यदि(!base->scheme || !base->host)
    प्रत्यागम FALSE;
  यदि(href->scheme && !curl_strequal(base->scheme, href->scheme))
    प्रत्यागम FALSE;
  यदि(href->host) {
    यदि(!curl_strequal(base->host, href->host))
      प्रत्यागम FALSE;

    यदि(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      यदि(!s) /* Cannot match default port for unknown scheme */
        प्रत्यागम FALSE;
      /* to match, the present one must be the default port */
      यदि((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        प्रत्यागम FALSE;
    }
    अन्यथा यदि(base->portnum != href->portnum) /* both present or missing */
      प्रत्यागम FALSE;

    यदि(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(href->port_present) /* no host in href, then there must be no port */
    प्रत्यागम FALSE;
  प्रत्यागम TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  यदि(u->port_present) {
    *pport = u->portnum;
    प्रत्यागम CURLUE_OK;
  }
  अन्यथा यदि(u->scheme) {
    ध्रुव संरचना Curl_scheme *s = Curl_get_scheme(u->scheme);
    यदि(s && s->defport) {
      *pport = s->defport;
      प्रत्यागम CURLUE_OK;
    }
  }
  *pport = 0;
  प्रत्यागम CURLUE_NO_PORT;
}
