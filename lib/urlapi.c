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

#समावेश "urldata.h"
#समावेश "urlapi-int.h"
#समावेश "strcase.h"
#समावेश "url.h"
#समावेश "escape.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/inet_ntop.h"
#समावेश "curlx/strdup.h"
#समावेश "idn.h"
#समावेश "curlx/strparse.h"
#समावेश "curl_memrchr.h"

#अगर_घोषित _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#घोषणा STARTS_WITH_DRIVE_PREFIX(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#पूर्ण_अगर

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#घोषणा STARTS_WITH_URL_DRIVE_PREFIX(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#घोषणा MAX_SCHEME_LEN 40
#घोषणा MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#अगर !defined(USE_IPV6) && !defined(AF_INET6)
#घोषणा AF_INET6 (AF_INET + 1)
#पूर्ण_अगर

#घोषणा DEFAULT_SCHEME "https"

अटल व्योम free_urlhandle(काष्ठा Curl_URL *u)
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
अटल स्थिर अक्षर *find_host_sep(स्थिर अक्षर *url)
{
  /* Find the start of the hostname */
  स्थिर अक्षर *sep = म_माला(url, "//");
  अगर(!sep)
    sep = url;
  अन्यथा
    sep += 2;

  /* Find first / or ? */
  जबतक(*sep && *sep != '/' && *sep != '?')
    sep++;

  वापस sep;
}

/* convert CURLcode to CURLUcode */
#घोषणा cc2cu(x) \
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
UNITTEST CURLUcode urlencode_str(काष्ठा dynbuf *o, स्थिर अक्षर *url,
                                 माप_प्रकार len, bool relative,
                                 अचिन्हित पूर्णांक query);
UNITTEST CURLUcode urlencode_str(काष्ठा dynbuf *o, स्थिर अक्षर *url,
                                 माप_प्रकार len, bool relative,
                                 अचिन्हित पूर्णांक query)
{
  /* we must add this with whitespace-replacing */
  स्थिर अचिन्हित अक्षर *iptr;
  स्थिर अचिन्हित अक्षर *host_sep = (स्थिर अचिन्हित अक्षर *)url;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  अगर(!relative) {
    माप_प्रकार n;
    host_sep = (स्थिर अचिन्हित अक्षर *)find_host_sep(url);

    /* output the first piece as-is */
    n = (स्थिर अक्षर *)host_sep - url;
    परिणाम = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  क्रम(iptr = host_sep; len && !परिणाम;) {
    अगर(*iptr == ' ') {
      अगर(query != QUERY_YES)
        परिणाम = curlx_dyn_addn(o, "%20", 3);
      अन्यथा
        परिणाम = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    अन्यथा अगर((*iptr < ' ') || (*iptr >= 0x7f)) {
      अचिन्हित अक्षर out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      परिणाम = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    अन्यथा अगर(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      अचिन्हित अक्षर hex = (अचिन्हित अक्षर)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      अचिन्हित अक्षर out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      परिणाम = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    अन्यथा {
      स्थिर अचिन्हित अक्षर *start = iptr;
      जबतक(len) {
        अगर(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          अवरोध;
        अगर(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          अवरोध;
        अगर(*iptr == '?') {
          अगर(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            अवरोध;
          }
        }
        iptr++;
        len--;
      }
      परिणाम = curlx_dyn_addn(o, (स्थिर अक्षर *)start, (माप_प्रकार)(iptr - start));
    }
  }

  अगर(परिणाम)
    वापस cc2cu(परिणाम);
  वापस CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
माप_प्रकार Curl_is_absolute_url(स्थिर अक्षर *url, अक्षर *buf, माप_प्रकार buflen,
                            bool guess_scheme)
{
  माप_प्रकार i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (व्योम)buflen; /* only used in debug-builds */
  अगर(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#अगर_घोषित _WIN32
  अगर(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    वापस 0;
#पूर्ण_अगर
  अगर(ISALPHA(url[0])) {
    अगर(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    क्रम(i = 1; i < MAX_SCHEME_LEN; ++i) {
      अक्षर s = url[i];
      अगर(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        अगर(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      अन्यथा {
        अवरोध;
      }
    }
  }
  अगर(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    माप_प्रकार len = i;
    अगर(buf)
      buf[i] = 0;
    वापस len;
  }
  अगर(buf)
    buf[0] = 0;
  वापस 0;
}

/* scan for byte values <= 31, 127 and maybe space */
अटल bool badoctets(स्थिर अक्षर *निवेश, माप_प्रकार n, पूर्णांक flags)
{
  स्थिर uint8_t *p = (स्थिर अचिन्हित अक्षर *)निवेश;
  स्थिर uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  जबतक(n--) {
    अगर(*p <= control || *p == 127)
      वापस TRUE;
    p++;
  }
  वापस FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(काष्ठा Curl_URL *u,
                                        स्थिर अक्षर *login,
                                        माप_प्रकार len,
                                        अचिन्हित पूर्णांक flags,
                                        माप_प्रकार *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(काष्ठा Curl_URL *u,
                                        स्थिर अक्षर *login,
                                        माप_प्रकार len,
                                        अचिन्हित पूर्णांक flags,
                                        माप_प्रकार *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode परिणाम;
  अक्षर *userp = शून्य;
  अक्षर *passwdp = शून्य;
  अक्षर *optionsp = शून्य;
  स्थिर काष्ठा Curl_scheme *h = शून्य;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  स्थिर अक्षर *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = स_प्रथम(login, '@', len);
  अगर(!ptr)
    जाओ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  अगर(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  परिणाम = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : शून्य);
  अगर(परिणाम) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    जाओ out;
  }

  अगर(userp) {
    अगर(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      जाओ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  अगर(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  अगर(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  अगर(userp && badoctets(userp, म_माप(userp), flags))
    ures = CURLUE_BAD_USER;
  अन्यथा अगर(passwdp && badoctets(passwdp, म_माप(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  अन्यथा अगर(optionsp && badoctets(optionsp, म_माप(optionsp), flags))
    ures = CURLUE_MALFORMED_निवेश;

  userp = passwdp = optionsp = शून्य;

  अगर(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    वापस CURLUE_OK;
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

  वापस ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(काष्ठा Curl_URL *u, काष्ठा dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(काष्ठा Curl_URL *u, काष्ठा dynbuf *host,
                              bool has_scheme)
{
  स्थिर अक्षर *portptr;
  स्थिर अक्षर *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  अगर(hostname[0] == '[') {
    portptr = स_प्रथम(hostname + 1, ']', curlx_dyn_len(host) - 1);
    अगर(!portptr)
      वापस CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    अगर(*portptr) {
      अगर(*portptr != ':')
        वापस CURLUE_BAD_PORT_संख्या;
    }
    अन्यथा
      portptr = शून्य;
  }
  अन्यथा
    portptr = स_प्रथम(hostname, ':', curlx_dyn_len(host));

  अगर(portptr) {
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
    अगर(!*portptr)
      वापस has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_संख्या;
    अगर(*portptr == '\\')
      वापस CURLUE_BACKSLASH;
    rc = curlx_str_संख्या(&portptr, &port, 0xffff);
    अगर(rc)
      वापस CURLUE_BAD_PORT_संख्या;
    अन्यथा अगर(*portptr == '\\')
      वापस CURLUE_BACKSLASH;
    अन्यथा अगर(*portptr)
      वापस CURLUE_BAD_PORT_संख्या;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  वापस CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

अटल स्थिर bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(काष्ठा Curl_URL *u, अक्षर *hostname,
                              माप_प्रकार hlen);
UNITTEST CURLUcode ipv6_parse(काष्ठा Curl_URL *u, अक्षर *hostname,
                              माप_प्रकार hlen) /* length of hostname */
{
  माप_प्रकार len;
  DEBUGASSERT(*hostname == '[');
  अगर(hlen < 4) /* '[::]' is the shortest possible valid string */
    वापस CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = म_अखोज(hostname, "0123456789abcdefABCDEF:.");

  अगर(hlen != len) {
    hlen = len;
    अगर(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      अक्षर zoneid[MAX_ZONEID_LEN];
      पूर्णांक i = 0;
      अक्षर *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      अगर(!म_भेदन(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      जबतक(!invalid_host_char[(अचिन्हित अक्षर)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      अगर(!i || (']' != *h))
        वापस CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      अगर(!u->zoneid)
        वापस CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    अन्यथा
      वापस CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    अक्षर dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    अगर(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      वापस CURLUE_BAD_IPV6;
    अगर(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = म_माप(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  वापस CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
अटल CURLUcode hostname_check(अक्षर *hostname, माप_प्रकार hlen)
{
  माप_प्रकार i;
  क्रम(i = 0; i < hlen; i++) {
    अगर(invalid_host_char[(अचिन्हित अक्षर)hostname[i]])
      वापस CURLUE_BAD_HOSTNAME;
  }
  अगर((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    वापस CURLUE_BAD_HOSTNAME;
  अन्यथा अगर((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    वापस CURLUE_BAD_HOSTNAME;
  वापस CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
अटल CURLUcode hostname_check6(काष्ठा Curl_URL *u, अक्षर *hostname,
                                 माप_प्रकार hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  अगर(!hlen)
    वापस CURLUE_NO_HOST;
  अन्यथा अगर(hostname[0] == '[')
    वापस ipv6_parse(u, hostname, hlen);

  वापस hostname_check(hostname, hlen);
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
UNITTEST पूर्णांक ipv4_normalize(काष्ठा dynbuf *host);
UNITTEST पूर्णांक ipv4_normalize(काष्ठा dynbuf *host)
{
  bool done = FALSE;
  पूर्णांक n = 0;
  स्थिर अक्षर *c = curlx_dyn_ptr(host);
  अचिन्हित पूर्णांक parts[4] = { 0, 0, 0, 0 };
  CURLcode परिणाम = CURLE_OK;

  अगर(!ISDIGIT(*c))
    वापस HOST_NAME;

  जबतक(!done) {
    पूर्णांक rc;
    curl_off_t l;
    अगर(*c == '0') {
      अगर((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, अच_पूर्णांक_उच्च);
        अगर(rc)
          वापस HOST_NAME;
      }
      अन्यथा
        rc = curlx_str_octal(&c, &l, अच_पूर्णांक_उच्च);
    }
    अन्यथा
      rc = curlx_str_संख्या(&c, &l, अच_पूर्णांक_उच्च);

    अगर(rc) {
      अगर(!n || (rc != STRE_NO_अंक) || *c)
        वापस HOST_NAME;
      n--;
    }
    अन्यथा
      parts[n] = (अचिन्हित पूर्णांक)l;

    चयन(*c) {
    हाल '.':
      अगर(n == 3) {
        अगर(c[1])
          /* something follows this dot */
          वापस HOST_NAME;
        done = TRUE;
      }
      अन्यथा {
        n++;
        c++;
      }
      अवरोध;

    हाल '\0':
      done = TRUE;
      अवरोध;

    शेष:
      वापस HOST_NAME;
    }
  }

  चयन(n) {
  हाल 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    परिणाम = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    अवरोध;
  हाल 1: /* a.b -- 8.24 bits */
    अगर((parts[0] > 0xff) || (parts[1] > 0xffffff))
      वापस HOST_NAME;
    curlx_dyn_reset(host);
    परिणाम = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    अवरोध;
  हाल 2: /* a.b.c -- 8.8.16 bits */
    अगर((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      वापस HOST_NAME;
    curlx_dyn_reset(host);
    परिणाम = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    अवरोध;
  हाल 3: /* a.b.c.d -- 8.8.8.8 bits */
    अगर((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      वापस HOST_NAME;
    curlx_dyn_reset(host);
    परिणाम = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    अवरोध;
  }
  अगर(परिणाम)
    वापस HOST_ERROR;
  वापस HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
अटल CURLUcode urldecode_host(काष्ठा dynbuf *host)
{
  स्थिर अक्षर *per;
  स्थिर अक्षर *hostname = curlx_dyn_ptr(host);
  per = स_प्रथम(hostname, '%', curlx_dyn_len(host));
  अगर(!per)
    /* nothing to decode */
    वापस CURLUE_OK;
  अन्यथा {
    /* encoded */
    माप_प्रकार dlen;
    अक्षर *decoded;
    CURLcode परिणाम = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    अगर(परिणाम)
      वापस CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    परिणाम = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    अगर(परिणाम)
      वापस cc2cu(परिणाम);
  }

  वापस CURLUE_OK;
}

अटल CURLUcode parse_authority(काष्ठा Curl_URL *u,
                                 स्थिर अक्षर *auth, माप_प्रकार authlen,
                                 अचिन्हित पूर्णांक flags,
                                 काष्ठा dynbuf *host,
                                 bool has_scheme)
{
  माप_प्रकार offset;
  CURLUcode uc;
  CURLcode परिणाम;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  अगर(uc)
    वापस uc;

  परिणाम = curlx_dyn_addn(host, auth + offset, authlen - offset);
  अगर(परिणाम) {
    uc = cc2cu(परिणाम);
    वापस uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  अगर(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  अगर(!uc)
    uc = urldecode_host(host);
  अगर(uc)
    ;
  अन्यथा अगर(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  अन्यथा {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    पूर्णांक type = ipv4_normalize(host);

    अगर(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    अन्यथा अगर(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  वापस uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, स्थिर अक्षर *authority)
{
  CURLUcode ures;
  काष्ठा dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_निवेश_लंबाई);

  ures = parse_authority(u, authority, म_माप(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  अगर(ures)
    curlx_dyn_free(&host);
  अन्यथा {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  वापस ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

अटल bool is_dot(स्थिर अक्षर **str, माप_प्रकार *clen)
{
  स्थिर अक्षर *p = *str;
  अगर(*p == '.') {
    (*str)++;
    (*clen)--;
    वापस TRUE;
  }
  अन्यथा अगर((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    वापस TRUE;
  }
  वापस FALSE;
}

#घोषणा ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
अटल bool needs_dedotdot(स्थिर अक्षर *p, माप_प्रकार pn)
{
  /* a single byte path cannot be cleaned up */
  अगर(pn < 2)
    वापस FALSE;
  अगर(!स_प्रथम(p, '.', pn) && !स_प्रथम(p, '%', pn))
    वापस FALSE;
  जबतक(pn) {
    अगर(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      अगर(!pn || ISSLASH(*p))
        वापस TRUE;
      /* "../" or ".." before end of string */
      अन्यथा अगर(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        वापस TRUE;
    }
    अन्यथा {
      p++;
      pn--;
    }
  }
  वापस FALSE;
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
UNITTEST पूर्णांक dedotdotify(स्थिर अक्षर *निवेश, माप_प्रकार clen, अक्षर **outp);
UNITTEST पूर्णांक dedotdotify(स्थिर अक्षर *निवेश, माप_प्रकार clen, अक्षर **outp)
{
  काष्ठा dynbuf out;
  CURLcode परिणाम = CURLE_OK;

  /* variables for leading dot checks */
  स्थिर अक्षर *dinput = निवेश;
  माप_प्रकार dlen = clen;

  *outp = शून्य;
  अगर(!needs_dedotdot(निवेश, clen))
    वापस 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  अगर(is_dot(&dinput, &dlen)) {
    अगर(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      निवेश = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    अन्यथा अगर(is_dot(&dinput, &dlen)) {
      अगर(!dlen)
        /* .. [end] */
        जाओ end;
      अन्यथा अगर(ISSLASH(*dinput)) {
        /* ../ */
        निवेश = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  जबतक(clen && !परिणाम) { /* until end of path content */
    अगर(ISSLASH(*निवेश)) {
      स्थिर अक्षर *p = &निवेश[1];
      माप_प्रकार blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      अगर(is_dot(&p, &blen)) {
        अगर(!blen) { /* /. */
          परिणाम = curlx_dyn_addn(&out, "/", 1);
          अवरोध;
        }
        अन्यथा अगर(ISSLASH(*p)) { /* /./ */
          निवेश = p;
          clen = blen;
          जारी;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        अन्यथा अगर(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          माप_प्रकार len = curlx_dyn_len(&out);
          अगर(len) {
            स्थिर अक्षर *ptr = curlx_dyn_ptr(&out);
            स्थिर अक्षर *last = memrchr(ptr, '/', len);
            अगर(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          अगर(blen) { /* /../ */
            निवेश = p;
            clen = blen;
            जारी;
          }
          परिणाम = curlx_dyn_addn(&out, "/", 1);
          अवरोध;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    परिणाम = curlx_dyn_addn(&out, निवेश, 1);
    निवेश++;
    clen--;
  }
end:
  अगर(!परिणाम) {
    अगर(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    अन्यथा {
      *outp = curlx_strdup("");
      अगर(!*outp)
        वापस 1;
    }
  }
  वापस परिणाम ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(स्थिर अक्षर *url, माप_प्रकार urllen, CURLU *u,
                              स्थिर अक्षर **pathp, माप_प्रकार *pathlenp);
UNITTEST CURLUcode parse_file(स्थिर अक्षर *url, माप_प्रकार urllen, CURLU *u,
                              स्थिर अक्षर **pathp, माप_प्रकार *pathlenp)
{
  स्थिर अक्षर *path;
  माप_प्रकार pathlen;

  *pathp = शून्य;
  *pathlenp = 0;
  अगर(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    वापस CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  अगर(path[0] != '/')
    वापस CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  अगर(path[1] == '/') {
    /* swallow the two slashes */
    स्थिर अक्षर *ptr = &path[2];

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
    अगर(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      अगर(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      अन्यथा
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        वापस CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#अगर !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  अगर(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    वापस CURLUE_BAD_FILE_URL;
  }
#अन्यथा
  /* If the path starts with a slash and a drive letter, ditch the slash */
  अगर('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#पूर्ण_अगर
  u->scheme = curlx_strdup("file");
  अगर(!u->scheme)
    वापस CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  वापस CURLUE_OK;
}

अटल CURLUcode parse_scheme(स्थिर अक्षर *url, CURLU *u, अक्षर *schemebuf,
                              माप_प्रकार schemelen, अचिन्हित पूर्णांक flags,
                              स्थिर अक्षर **hostpp)
{
  /* clear path */
  स्थिर अक्षर *schemep = शून्य;

  अगर(schemelen) {
    पूर्णांक अंक_slashes = 0;
    स्थिर अक्षर *p = &url[schemelen + 1];
    अगर(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      वापस CURLUE_UNSUPPORTED_SCHEME;

    अगर(!ISSLASH(*p))
      /* less than one */
      वापस CURLUE_BAD_SLASHES;
    अगर((flags & CURLU_NO_AUTHORITY)) {
      जबतक(ISSLASH(*p) && (अंक_slashes < 2)) {
        p++;
        अंक_slashes++;
      }
    }
    अन्यथा {
      जबतक(ISSLASH(*p) && (अंक_slashes < 4)) {
        p++;
        अंक_slashes++;
      }
      अगर(अंक_slashes > 3)
        वापस CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  अन्यथा {
    /* no scheme! */

    अगर(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      वापस CURLUE_BAD_SCHEME;

    अगर(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  अगर(schemep) {
    u->scheme = curlx_strdup(schemep);
    अगर(!u->scheme)
      वापस CURLUE_OUT_OF_MEMORY;
  }
  वापस CURLUE_OK;
}

अटल CURLUcode guess_scheme(CURLU *u, काष्ठा dynbuf *host)
{
  स्थिर अक्षर *hostname = curlx_dyn_ptr(host);
  स्थिर अक्षर *schemep = शून्य;
  /* legacy curl-style guess based on hostname */
  अगर(checkprefix("ftp.", hostname))
    schemep = "ftp";
  अन्यथा अगर(checkprefix("dict.", hostname))
    schemep = "dict";
  अन्यथा अगर(checkprefix("ldap.", hostname))
    schemep = "ldap";
  अन्यथा अगर(checkprefix("imap.", hostname))
    schemep = "imap";
  अन्यथा अगर(checkprefix("smtp.", hostname))
    schemep = "smtp";
  अन्यथा अगर(checkprefix("pop3.", hostname))
    schemep = "pop3";
  अन्यथा
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  अगर(!u->scheme)
    वापस CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  वापस CURLUE_OK;
}

अटल CURLUcode handle_fragment(CURLU *u, स्थिर अक्षर *fragment,
                                 माप_प्रकार fraglen, अचिन्हित पूर्णांक flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  अगर(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    अगर(flags & CURLU_URLENCODE) {
      काष्ठा dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_निवेश_लंबाई);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      अगर(ures)
        वापस ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    अन्यथा {
      अगर(badoctets(fragment, fraglen, flags))
        वापस CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      अगर(!u->fragment)
        वापस CURLUE_OUT_OF_MEMORY;
    }
  }
  वापस CURLUE_OK;
}

अटल CURLUcode handle_query(CURLU *u, स्थिर अक्षर *query,
                              माप_प्रकार qlen, अचिन्हित पूर्णांक flags)
{
  u->query_present = TRUE;
  अगर(qlen > 1) {
    अगर(flags & CURLU_URLENCODE) {
      काष्ठा dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_निवेश_लंबाई);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      अगर(ures)
        वापस ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    अन्यथा {
      अगर(badoctets(query, qlen, flags))
        वापस CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      अगर(!u->query)
        वापस CURLUE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    /* single byte query */
    u->query = curlx_strdup("");
    अगर(!u->query)
      वापस CURLUE_OUT_OF_MEMORY;
  }
  वापस CURLUE_OK;
}

अटल CURLUcode handle_path(CURLU *u, स्थिर अक्षर *path,
                             माप_प्रकार pathlen, अचिन्हित पूर्णांक flags,
                             bool is_file)
{
  CURLUcode ures;
  अगर(pathlen && (flags & CURLU_URLENCODE)) {
    काष्ठा dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_निवेश_लंबाई);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    अगर(ures)
      वापस ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  अगर(pathlen >= (माप_प्रकार)(1 + !is_file)) {
    अगर(badoctets(path, pathlen, flags))
      वापस CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    अगर(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      अगर(!u->path)
        वापस CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    अन्यथा अगर(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    अगर(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      अक्षर *dedot;
      पूर्णांक err = dedotdotify(path, pathlen, &dedot);
      अगर(err)
        वापस CURLUE_OUT_OF_MEMORY;
      अगर(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  वापस CURLUE_OK;
}

अटल CURLUcode parseurl(स्थिर अक्षर *url, CURLU *u, अचिन्हित पूर्णांक flags)
{
  स्थिर अक्षर *path;
  माप_प्रकार pathlen;
  अक्षर schemebuf[MAX_SCHEME_LEN + 1];
  माप_प्रकार schemelen = 0;
  माप_प्रकार urllen;
  CURLUcode ures = CURLUE_OK;
  काष्ठा dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = म_माप(url);
  अगर(urllen > CURL_MAX_निवेश_लंबाई)
    वापस CURLUE_MALFORMED_निवेश;

  curlx_dyn_init(&host, CURL_MAX_निवेश_लंबाई);

  schemelen = Curl_is_absolute_url(url, schemebuf, माप(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  अगर(schemelen == 4 && !स_भेद(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  अन्यथा {
    स्थिर अक्षर *hostp = शून्य;
    स्थिर अक्षर *p;
    माप_प्रकार hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    अगर(ures)
      जाओ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    जबतक(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    अगर(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      अगर(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    अन्यथा अगर(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      अगर(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    अन्यथा
      ures = CURLUE_NO_HOST;
  }
  अगर(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    स्थिर अक्षर *fragment = स_प्रथम(path, '#', pathlen);
    अगर(fragment) {
      माप_प्रकार fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  अगर(!ures) {
    स्थिर अक्षर *query = स_प्रथम(path, '?', pathlen);
    अगर(query) {
      माप_प्रकार qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  अगर(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  अगर(!ures) {
    u->host = curlx_dyn_ptr(&host);
    वापस CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  वापस ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
अटल CURLUcode parseurl_and_replace(स्थिर अक्षर *url, CURLU *u,
                                      अचिन्हित पूर्णांक flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  स_रखो(&tmpurl, 0, माप(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  अगर(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  वापस ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
अटल CURLUcode redirect_url(स्थिर अक्षर *base, स्थिर अक्षर *relurl,
                              CURLU *u, अचिन्हित पूर्णांक flags)
{
  काष्ठा dynbuf urlbuf;
  bool host_changed = FALSE;
  स्थिर अक्षर *useurl = relurl;
  स्थिर अक्षर *cutoff = शून्य;
  माप_प्रकार prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  स्थिर अक्षर *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  स्थिर अक्षर *protsep = base + म_माप(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  अगर(!base)
    वापस CURLUE_MALFORMED_निवेश; /* should never happen */

  /* handle different relative URL types */
  चयन(relurl[0]) {
  हाल '/':
    अगर(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    अन्यथा
      /* absolute /path */
      cutoff = म_अक्षर(protsep, '/');
    अवरोध;

  हाल '#':
    /* fragment-only change */
    अगर(u->fragment_present)
      cutoff = म_अक्षर(protsep, '#');
    अवरोध;

  शेष:
    /* path or query-only change */
    अगर(u->query_present)
      /* remove existing query */
      cutoff = म_अक्षर(protsep, '?');
    अन्यथा अगर(u->fragment_present)
      /* Remove existing fragment */
      cutoff = म_अक्षर(protsep, '#');

    अगर(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (माप_प्रकार)(cutoff - protsep) : म_माप(protsep));
      अगर(cutoff)
        cutoff++; /* truncate after last slash */
    }
    अवरोध;
  }

  prelen = cutoff ? (माप_प्रकार)(cutoff - base) : म_माप(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_निवेश_लंबाई);

  अगर(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, म_माप(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  अन्यथा
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  वापस uc;
}

/*
 */
CURLU *curl_url(व्योम)
{
  वापस curlx_calloc(1, माप(काष्ठा Curl_URL));
}

व्योम curl_url_cleanup(CURLU *u)
{
  अगर(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#घोषणा DUP(dest, src, name)                    \
  करो {                                          \
    अगर((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      अगर(!(dest)->name)                         \
        जाओ fail;                              \
    }                                           \
  } जबतक(0)

CURLU *curl_url_dup(स्थिर CURLU *in)
{
  काष्ठा Curl_URL *u;

  अगर(!in)
    वापस शून्य;

  u = curlx_calloc(1, माप(काष्ठा Curl_URL));
  अगर(u) {
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
  वापस u;
fail:
  curl_url_cleanup(u);
  वापस शून्य;
}

#अगर_अघोषित USE_IDN
#घोषणा host_decode(x, y) CURLUE_LACKS_IDN
#घोषणा host_encode(x, y) CURLUE_LACKS_IDN
#अन्यथा
अटल CURLUcode host_decode(स्थिर अक्षर *host, अक्षर **allochost)
{
  CURLcode परिणाम = Curl_idn_decode(host, allochost);
  अगर(परिणाम)
    वापस (परिणाम == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  वापस CURLUE_OK;
}

अटल CURLUcode host_encode(स्थिर अक्षर *host, अक्षर **allochost)
{
  CURLcode परिणाम = Curl_idn_encode(host, allochost);
  अगर(परिणाम)
    वापस (परिणाम == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  वापस CURLUE_OK;
}
#पूर्ण_अगर

अटल CURLUcode urlget_format(स्थिर CURLU *u, CURLUPart what,
                               स्थिर अक्षर *ptr, अक्षर **partp,
                               bool plusdecode, अचिन्हित पूर्णांक flags)
{
  CURLUcode uc = CURLUE_OK;
  माप_प्रकार partlen = म_माप(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  अक्षर *part = curlx_memdup0(ptr, partlen);
  *partp = शून्य;
  अगर(!part)
    वापस CURLUE_OUT_OF_MEMORY;
  अगर(plusdecode) {
    /* convert + to space */
    अक्षर *plus = part;
    माप_प्रकार i = 0;
    क्रम(i = 0; i < partlen; ++plus, i++) {
      अगर(*plus == '+')
        *plus = ' ';
    }
  }
  अगर(urldecode) {
    अक्षर *decoded;
    माप_प्रकार dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode परिणाम = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    अगर(परिणाम)
      वापस CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  अगर(urlencode) {
    काष्ठा dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_निवेश_लंबाई);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    अगर(uc)
      वापस uc;
    part = curlx_dyn_ptr(&enc);
  }
  अन्यथा अगर(punycode) {
    अगर(!Curl_is_ASCII_name(u->host)) {
      अक्षर *punyversion = शून्य;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      अगर(uc)
        वापस uc;
      uc = hostname_check(punyversion, म_माप(punyversion));
      अगर(uc) {
        curlx_free(punyversion);
        वापस uc;
      }
      part = punyversion;
    }
  }
  अन्यथा अगर(depunyfy && Curl_is_ASCII_name(u->host)) {
    अक्षर *unpunified = शून्य;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    अगर(uc)
      वापस uc;
    part = unpunified;
  }
  *partp = part;
  वापस CURLUE_OK;
}

अटल CURLUcode file_url(स्थिर CURLU *u, अक्षर **part,
                          स्थिर अक्षर *fragmentsep,
                          स्थिर अक्षर *querysep)
{
  अक्षर *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  अगर(!url)
    वापस CURLUE_OUT_OF_MEMORY;

  *part = url;
  वापस CURLUE_OK;
}

अटल CURLUcode urlget_url(स्थिर CURLU *u, अक्षर **part, अचिन्हित पूर्णांक flags)
{
  अक्षर *url;
  अक्षर *allochost = शून्य;
  स्थिर अक्षर *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  स्थिर अक्षर *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  अक्षर portbuf[7];
  अगर(curl_strequal("file", u->scheme))
    वापस file_url(u, part, fragmentsep, querysep);
  अन्यथा अगर(!u->host)
    वापस CURLUE_NO_HOST;
  अन्यथा {
    स्थिर अक्षर *scheme;
    अक्षर *options = u->options;
    अक्षर *port = शून्य;
    स्थिर काष्ठा Curl_scheme *h = शून्य;
    अक्षर schemebuf[MAX_SCHEME_LEN + 5];
    अगर(u->scheme)
      scheme = u->scheme;
    अन्यथा अगर(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    अन्यथा
      वापस CURLUE_NO_SCHEME;

    अगर(u->port_present) {
      curl_msnprintf(portbuf, माप(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    अगर(h) {
      अगर(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, माप(portbuf), "%u", h->defport);
        port = portbuf;
      }
      अन्यथा अगर(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = शून्य;
      }

      अगर(!(h->flags & PROTOPT_URLOPTIONS))
        options = शून्य;
    }

    अगर(u->host[0] == '[') {
      अगर(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        काष्ठा dynbuf enc;
        माप_प्रकार hostlen = म_माप(u->host);
        curlx_dyn_init(&enc, CURL_MAX_निवेश_लंबाई);
        अगर(curlx_dyn_addf(&enc, "%.*s%%25%s]", (पूर्णांक)hostlen - 1, u->host,
                          u->zoneid))
          वापस CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    अन्यथा अगर(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(शून्य, u->host, 0);
      अगर(!allochost)
        वापस CURLUE_OUT_OF_MEMORY;
    }
    अन्यथा अगर(flags & CURLU_PUNYCODE) {
      अगर(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        अगर(!ret)
          ret = hostname_check(allochost, म_माप(allochost));
        अगर(ret) {
          curlx_safefree(allochost);
          वापस ret;
        }
      }
    }
    अन्यथा अगर(flags & CURLU_PUNY2IDN) {
      अगर(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        अगर(ret)
          वापस ret;
      }
    }

    अगर(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, माप(schemebuf), "%s://", scheme);
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
  अगर(!url)
    वापस CURLUE_OUT_OF_MEMORY;
  *part = url;
  वापस CURLUE_OK;
}

CURLUcode curl_url_get(स्थिर CURLU *u, CURLUPart what,
                       अक्षर **part, अचिन्हित पूर्णांक flags)
{
  स्थिर अक्षर *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  अक्षर portbuf[7];
  bool plusdecode = FALSE;
  अगर(!u)
    वापस CURLUE_BAD_HANDLE;
  अगर(!part)
    वापस CURLUE_BAD_PARTPOINTER;
  *part = शून्य;

  चयन(what) {
  हाल CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    अगर((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      वापस CURLUE_NO_SCHEME;
    अवरोध;
  हाल CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    अवरोध;
  हाल CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    अवरोध;
  हाल CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    अवरोध;
  हाल CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    अवरोध;
  हाल CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    अवरोध;
  हाल CURLUPART_PORT:
    ptr = शून्य;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    अगर(u->port_present) {
      स्थिर काष्ठा Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : शून्य;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      अगर(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = शून्य;
      }
      अन्यथा {
        curl_msnprintf(portbuf, माप(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    अन्यथा अगर((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      स्थिर काष्ठा Curl_scheme *h = Curl_get_scheme(u->scheme);
      अगर(h) {
        curl_msnprintf(portbuf, माप(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    अवरोध;
  हाल CURLUPART_PATH:
    ptr = u->path;
    अगर(!ptr)
      ptr = "/";
    अवरोध;
  हाल CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    अगर(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = शून्य;
    अवरोध;
  हाल CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    अगर(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    अवरोध;
  हाल CURLUPART_URL:
    वापस urlget_url(u, part, flags);
  शेष:
    ptr = शून्य;
    अवरोध;
  }
  अगर(ptr)
    वापस urlget_format(u, what, ptr, part, plusdecode, flags);

  वापस ifmissing;
}

अटल CURLUcode set_url_scheme(CURLU *u, स्थिर अक्षर *scheme,
                                अचिन्हित पूर्णांक flags)
{
  माप_प्रकार plen = म_माप(scheme);
  स्थिर काष्ठा Curl_scheme *h = शून्य;
  अगर((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    वापस CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  अगर(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    वापस CURLUE_UNSUPPORTED_SCHEME;
  अगर(!h) {
    स्थिर अक्षर *s = scheme;
    अगर(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      जबतक(--plen) {
        अगर(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        अन्यथा
          वापस CURLUE_BAD_SCHEME;
      }
    }
    अन्यथा
      वापस CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  वापस CURLUE_OK;
}

अटल CURLUcode set_url_port(CURLU *u, स्थिर अक्षर *provided_port)
{
  curl_off_t port;
  अगर(!ISDIGIT(provided_port[0]))
    /* not a number */
    वापस CURLUE_BAD_PORT_संख्या;
  अगर(curlx_str_संख्या(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    वापस CURLUE_BAD_PORT_संख्या;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  वापस CURLUE_OK;
}

अटल CURLUcode set_url(CURLU *u, स्थिर अक्षर *url, माप_प्रकार part_आकार,
                         अचिन्हित पूर्णांक flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  अक्षर *oldurl = शून्य;

  अगर(!part_आकार) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    अगर(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      वापस CURLUE_OK;
    }
    अगर(uc == CURLUE_OUT_OF_MEMORY)
      वापस uc;
    वापस CURLUE_MALFORMED_निवेश;
  }

  /* if the new URL is absolute replace the existing with the new. */
  अगर(Curl_is_absolute_url(url, शून्य, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    वापस parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  अगर(uc == CURLUE_OUT_OF_MEMORY)
    वापस uc;
  अन्यथा अगर(uc)
    वापस parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  वापस uc;
}

अटल CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  चयन(what) {
  हाल CURLUPART_URL:
    free_urlhandle(u);
    स_रखो(u, 0, माप(काष्ठा Curl_URL));
    अवरोध;
  हाल CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    अवरोध;
  हाल CURLUPART_USER:
    curlx_safefree(u->user);
    अवरोध;
  हाल CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    अवरोध;
  हाल CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    अवरोध;
  हाल CURLUPART_HOST:
    curlx_safefree(u->host);
    अवरोध;
  हाल CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    अवरोध;
  हाल CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    अवरोध;
  हाल CURLUPART_PATH:
    curlx_safefree(u->path);
    अवरोध;
  हाल CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    अवरोध;
  हाल CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    अवरोध;
  शेष:
    वापस CURLUE_UNKNOWN_PART;
  }
  वापस CURLUE_OK;
}

अटल bool allowed_in_path(अचिन्हित अक्षर x)
{
  चयन(x) {
  हाल '!':
  हाल '$':
  हाल '&':
  हाल '\'':
  हाल '(':
  हाल ')':
  हाल '{':
  हाल '}':
  हाल '[':
  हाल ']':
  हाल '*':
  हाल '+':
  हाल ',':
  हाल ';':
  हाल '=':
  हाल ':':
  हाल '@':
  हाल '/':
    वापस TRUE;
  }
  वापस FALSE;
}

अटल CURLUcode url_encode_part(काष्ठा dynbuf *encp,
                                 स्थिर अक्षर *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  स्थिर अचिन्हित अक्षर *i;

  क्रम(i = (स्थिर अचिन्हित अक्षर *)part; *i; i++) {
    CURLcode परिणाम;
    अगर((*i == ' ') && plusencode)
      परिणाम = curlx_dyn_addn(encp, "+", 1);
    अन्यथा अगर(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      अगर((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      परिणाम = curlx_dyn_addn(encp, i, 1);
    }
    अन्यथा {
      अचिन्हित अक्षर out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      परिणाम = curlx_dyn_addn(encp, out, 3);
    }
    अगर(परिणाम)
      वापस cc2cu(परिणाम);
  }
  वापस CURLUE_OK;
}

अटल CURLUcode url_uppercasehex_part(काष्ठा dynbuf *encp,
                                       स्थिर अक्षर *part)
{
  अक्षर *p;
  CURLcode परिणाम = curlx_dyn_add(encp, part);
  अगर(परिणाम)
    वापस cc2cu(परिणाम);
  p = curlx_dyn_ptr(encp);
  जबतक(*p) {
    /* make sure percent encoded are upper case */
    अगर((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    अन्यथा
      p++;
  }
  वापस CURLUE_OK;
}

अटल CURLUcode url_append_query(CURLU *u, काष्ठा dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  माप_प्रकार querylen = u->query ? म_माप(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  अगर(querylen) {
    काष्ठा dynbuf qbuf;
    CURLcode परिणाम;
    स्थिर अक्षर *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_निवेश_लंबाई);

    /* add original query */
    परिणाम = curlx_dyn_addn(&qbuf, u->query, querylen);
    अगर(!परिणाम && addamperand)
      /* add ampersand */
      परिणाम = curlx_dyn_addn(&qbuf, "&", 1);
    अगर(!परिणाम)
      /* add new query part */
      परिणाम = curlx_dyn_add(&qbuf, newp);
    अगर(परिणाम)
      जाओ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    वापस CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    वापस cc2cu(परिणाम);
  }
  अन्यथा {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  वापस CURLUE_OK;
}

अटल CURLUcode url_sethost(CURLU *u, काष्ठा dynbuf *encp,
                             bool urlencode,
                             अचिन्हित पूर्णांक flags)
{
  माप_प्रकार n = curlx_dyn_len(encp);
  bool bad = FALSE;
  अक्षर *newp = curlx_dyn_ptr(encp);
  अगर(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  अन्यथा अगर(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    माप_प्रकार dlen;
    अक्षर *decoded = शून्य;
    CURLcode परिणाम = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    अगर(परिणाम || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    अन्यथा {
      काष्ठा dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_निवेश_लंबाई);
      अगर(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      अन्यथा अगर(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        अगर(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  अन्यथा अगर(hostname_check6(u, newp, n))
    bad = TRUE;
  अन्यथा अगर(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  अगर(bad) {
    curlx_dyn_free(encp);
    वापस CURLUE_BAD_HOSTNAME;
  }
  वापस CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       स्थिर अक्षर *part, अचिन्हित पूर्णांक flags)
{
  अक्षर **storep = शून्य;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  माप_प्रकार nalloc;

  अगर(!u)
    वापस CURLUE_BAD_HANDLE;
  अगर(!part)
    /* setting a part to NULL clears it */
    वापस urlset_clear(u, what);

  nalloc = म_माप(part);
  अगर(nalloc > CURL_MAX_निवेश_लंबाई)
    /* excessive input length */
    वापस CURLUE_MALFORMED_निवेश;

  चयन(what) {
  हाल CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    अगर(status)
      वापस status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    अवरोध;
  }
  हाल CURLUPART_USER:
    storep = &u->user;
    अवरोध;
  हाल CURLUPART_PASSWORD:
    storep = &u->password;
    अवरोध;
  हाल CURLUPART_OPTIONS:
    storep = &u->options;
    अवरोध;
  हाल CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    अवरोध;
  हाल CURLUPART_ZONEID:
    storep = &u->zoneid;
    अवरोध;
  हाल CURLUPART_PORT:
    वापस set_url_port(u, part);
  हाल CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    अवरोध;
  हाल CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    अवरोध;
  हाल CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    अवरोध;
  हाल CURLUPART_URL:
    वापस set_url(u, part, nalloc, flags);
  शेष:
    वापस CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    स्थिर अक्षर *newp = शून्य;
    काष्ठा dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    अगर(leadingslash && (part[0] != '/')) {
      CURLcode परिणाम = curlx_dyn_addn(&enc, "/", 1);
      अगर(परिणाम)
        वापस cc2cu(परिणाम);
    }
    अगर(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    अन्यथा
      status = url_uppercasehex_part(&enc, part);
    अगर(!status) {
      newp = curlx_dyn_ptr(&enc);

      अगर(appendquery && newp)
        वापस url_append_query(u, &enc);
      अन्यथा अगर(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    अगर(status)
      वापस status;

    अगर(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (अक्षर *)CURL_UNCONST(newp);
  }
  वापस CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  स्थिर काष्ठा Curl_scheme *s = शून्य;

  /* base must be an absolute URL */
  अगर(!base->scheme || !base->host)
    वापस FALSE;
  अगर(href->scheme && !curl_strequal(base->scheme, href->scheme))
    वापस FALSE;
  अगर(href->host) {
    अगर(!curl_strequal(base->host, href->host))
      वापस FALSE;

    अगर(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      अगर(!s) /* Cannot match default port for unknown scheme */
        वापस FALSE;
      /* to match, the present one must be the default port */
      अगर((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        वापस FALSE;
    }
    अन्यथा अगर(base->portnum != href->portnum) /* both present or missing */
      वापस FALSE;

    अगर(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      वापस FALSE;
  }
  अन्यथा अगर(href->port_present) /* no host in href, then there must be no port */
    वापस FALSE;
  वापस TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  अगर(u->port_present) {
    *pport = u->portnum;
    वापस CURLUE_OK;
  }
  अन्यथा अगर(u->scheme) {
    स्थिर काष्ठा Curl_scheme *s = Curl_get_scheme(u->scheme);
    अगर(s && s->defport) {
      *pport = s->defport;
      वापस CURLUE_OK;
    }
  }
  *pport = 0;
  वापस CURLUE_NO_PORT;
}
