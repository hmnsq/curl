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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "urlapi-int.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "idn.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curl_memrchr.h"

#నిర్వచితమైతే _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#నిర్వచించు STARTS_WITH_DRIVE_ఉపసర్గ(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#అయితే_ముగింపు

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#నిర్వచించు STARTS_WITH_URL_DRIVE_ఉపసర్గ(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#నిర్వచించు MAX_SCHEME_LEN 40
#నిర్వచించు MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ఒకవేళ !defined(USE_IPV6) && !defined(AF_INET6)
#నిర్వచించు AF_INET6 (AF_INET + 1)
#అయితే_ముగింపు

#నిర్వచించు DEFAULT_SCHEME "https"

స్థిర శూన్య free_urlhandle(నిర్మాణం Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->మార్గం);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
స్థిర స్థిరరాశి అక్షరం *కనుగొను_host_sep(స్థిరరాశి అక్షరం *url)
{
  /* Find the start of the hostname */
  స్థిరరాశి అక్షరం *sep = అక్షరశ్రేణి_శోధన(url, "//");
  ఒకవేళ(!sep)
    sep = url;
  లేకపోతే
    sep += 2;

  /* Find first / or ? */
  అయితే(*sep && *sep != '/' && *sep != '?')
    sep++;

  తిరిగి sep;
}

/* convert CURLcode to CURLUcode */
#నిర్వచించు cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం)

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
UNITTEST CURLUcode urlencode_str(నిర్మాణం dynbuf *o, స్థిరరాశి అక్షరం *url,
                                 size_t len, bool relative,
                                 గుర్తులేని పూర్ణాంకం query);
UNITTEST CURLUcode urlencode_str(నిర్మాణం dynbuf *o, స్థిరరాశి అక్షరం *url,
                                 size_t len, bool relative,
                                 గుర్తులేని పూర్ణాంకం query)
{
  /* we must add this with whitespace-replacing */
  స్థిరరాశి గుర్తులేని అక్షరం *iptr;
  స్థిరరాశి గుర్తులేని అక్షరం *host_sep = (స్థిరరాశి గుర్తులేని అక్షరం *)url;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  ఒకవేళ(!relative) {
    size_t n;
    host_sep = (స్థిరరాశి గుర్తులేని అక్షరం *)కనుగొను_host_sep(url);

    /* output the first piece as-is */
    n = (స్థిరరాశి అక్షరం *)host_sep - url;
    ఫలితం = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  కొరకు(iptr = host_sep; len && !ఫలితం;) {
    ఒకవేళ(*iptr == ' ') {
      ఒకవేళ(query != QUERY_YES)
        ఫలితం = curlx_dyn_addn(o, "%20", 3);
      లేకపోతే
        ఫలితం = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    లేకపోతే ఒకవేళ((*iptr < ' ') || (*iptr >= 0x7f)) {
      గుర్తులేని అక్షరం బహిర్గతం[3] = { '%' };
      Curl_hexbyte(&బహిర్గతం[1], *iptr);
      ఫలితం = curlx_dyn_addn(o, బహిర్గతం, 3);
      iptr++;
      len--;
    }
    లేకపోతే ఒకవేళ(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      గుర్తులేని అక్షరం hex = (గుర్తులేని అక్షరం)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      గుర్తులేని అక్షరం బహిర్గతం[3] = { '%' };
      Curl_hexbyte(&బహిర్గతం[1], hex);
      ఫలితం = curlx_dyn_addn(o, బహిర్గతం, 3);
      iptr += 3;
      len -= 3;
    }
    లేకపోతే {
      స్థిరరాశి గుర్తులేని అక్షరం *ప్రారంభం = iptr;
      అయితే(len) {
        ఒకవేళ(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          ఆపు;
        ఒకవేళ(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          ఆపు;
        ఒకవేళ(*iptr == '?') {
          ఒకవేళ(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            ఆపు;
          }
        }
        iptr++;
        len--;
      }
      ఫలితం = curlx_dyn_addn(o, (స్థిరరాశి అక్షరం *)ప్రారంభం, (size_t)(iptr - ప్రారంభం));
    }
  }

  ఒకవేళ(ఫలితం)
    తిరిగి cc2cu(ఫలితం);
  తిరిగి CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(స్థిరరాశి అక్షరం *url, అక్షరం *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (శూన్య)buflen; /* only used in debug-builds */
  ఒకవేళ(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#నిర్వచితమైతే _WIN32
  ఒకవేళ(guess_scheme && STARTS_WITH_DRIVE_ఉపసర్గ(url))
    తిరిగి 0;
#అయితే_ముగింపు
  ఒకవేళ(ISALPHA(url[0])) {
    ఒకవేళ(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    కొరకు(i = 1; i < MAX_SCHEME_LEN; ++i) {
      అక్షరం s = url[i];
      ఒకవేళ(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        ఒకవేళ(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      లేకపోతే {
        ఆపు;
      }
    }
  }
  ఒకవేళ(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    ఒకవేళ(buf)
      buf[i] = 0;
    తిరిగి len;
  }
  ఒకవేళ(buf)
    buf[0] = 0;
  తిరిగి 0;
}

/* scan for byte values <= 31, 127 and maybe space */
స్థిర bool badoctets(స్థిరరాశి అక్షరం *ఆదానం, size_t n, పూర్ణాంకం flags)
{
  స్థిరరాశి uint8_t *p = (స్థిరరాశి గుర్తులేని అక్షరం *)ఆదానం;
  స్థిరరాశి uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  అయితే(n--) {
    ఒకవేళ(*p <= control || *p == 127)
      తిరిగి TRUE;
    p++;
  }
  తిరిగి FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(నిర్మాణం Curl_URL *u,
                                        స్థిరరాశి అక్షరం *login,
                                        size_t len,
                                        గుర్తులేని పూర్ణాంకం flags,
                                        size_t *hostname_స్థానభ్రంశం);
UNITTEST CURLUcode parse_hostname_login(నిర్మాణం Curl_URL *u,
                                        స్థిరరాశి అక్షరం *login,
                                        size_t len,
                                        గుర్తులేని పూర్ణాంకం flags,
                                        size_t *hostname_స్థానభ్రంశం)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode ఫలితం;
  అక్షరం *userp = ఖాళీ;
  అక్షరం *passwdp = ఖాళీ;
  అక్షరం *optionsp = ఖాళీ;
  స్థిరరాశి నిర్మాణం Curl_scheme *h = ఖాళీ;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  స్థిరరాశి అక్షరం *ptr;

  DEBUGASSERT(login);

  *hostname_స్థానభ్రంశం = 0;
  ptr = memchr(login, '@', len);
  ఒకవేళ(!ptr)
    వెళ్ళు బహిర్గతం;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  ఒకవేళ(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  ఫలితం = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ఖాళీ);
  ఒకవేళ(ఫలితం) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(userp) {
    ఒకవేళ(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      వెళ్ళు బహిర్గతం;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  ఒకవేళ(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  ఒకవేళ(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  ఒకవేళ(userp && badoctets(userp, అక్షరశ్రేణి_పొడవు(userp), flags))
    ures = CURLUE_BAD_USER;
  లేకపోతే ఒకవేళ(passwdp && badoctets(passwdp, అక్షరశ్రేణి_పొడవు(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  లేకపోతే ఒకవేళ(optionsp && badoctets(optionsp, అక్షరశ్రేణి_పొడవు(optionsp), flags))
    ures = CURLUE_MALFORMED_ఆదానం;

  userp = passwdp = optionsp = ఖాళీ;

  ఒకవేళ(!ures) {
    /* the hostname starts at this offset */
    *hostname_స్థానభ్రంశం = ptr - login;
    తిరిగి CURLUE_OK;
  }

బహిర్గతం:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  తిరిగి ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_పోర్టు(నిర్మాణం Curl_URL *u, నిర్మాణం dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_పోర్టు(నిర్మాణం Curl_URL *u, నిర్మాణం dynbuf *host,
                              bool has_scheme)
{
  స్థిరరాశి అక్షరం *portptr;
  స్థిరరాశి అక్షరం *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->పోర్టు_present = FALSE;
  ఒకవేళ(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    ఒకవేళ(!portptr)
      తిరిగి CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    ఒకవేళ(*portptr) {
      ఒకవేళ(*portptr != ':')
        తిరిగి CURLUE_BAD_పోర్టు_సంఖ్య;
    }
    లేకపోతే
      portptr = ఖాళీ;
  }
  లేకపోతే
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  ఒకవేళ(portptr) {
    curl_off_t పోర్టు;
    size_t keep = portptr - hostname;
    పూర్ణాంకం rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    ఒకవేళ(!*portptr)
      తిరిగి has_scheme ? CURLUE_OK : CURLUE_BAD_పోర్టు_సంఖ్య;
    ఒకవేళ(*portptr == '\\')
      తిరిగి CURLUE_BACKSLASH;
    rc = curlx_str_సంఖ్య(&portptr, &పోర్టు, 0xffff);
    ఒకవేళ(rc)
      తిరిగి CURLUE_BAD_పోర్టు_సంఖ్య;
    లేకపోతే ఒకవేళ(*portptr == '\\')
      తిరిగి CURLUE_BACKSLASH;
    లేకపోతే ఒకవేళ(*portptr)
      తిరిగి CURLUE_BAD_పోర్టు_సంఖ్య;

    u->portnum = (uint16_t)పోర్టు;
    u->పోర్టు_present = TRUE;
  }

  తిరిగి CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

స్థిర స్థిరరాశి bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(నిర్మాణం Curl_URL *u, అక్షరం *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(నిర్మాణం Curl_URL *u, అక్షరం *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  ఒకవేళ(hlen < 4) /* '[::]' is the shortest possible valid string */
    తిరిగి CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  ఒకవేళ(hlen != len) {
    hlen = len;
    ఒకవేళ(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      అక్షరం zoneid[MAX_ZONEID_LEN];
      పూర్ణాంకం i = 0;
      అక్షరం *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      ఒకవేళ(!అక్షరశ్రేణి_పోలిక(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      అయితే(!invalid_host_char[(గుర్తులేని అక్షరం)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      ఒకవేళ(!i || (']' != *h))
        తిరిగి CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      ఒకవేళ(!u->zoneid)
        తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    లేకపోతే
      తిరిగి CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    అక్షరం dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    ఒకవేళ(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      తిరిగి CURLUE_BAD_IPV6;
    ఒకవేళ(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = అక్షరశ్రేణి_పొడవు(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  తిరిగి CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
స్థిర CURLUcode hostname_check(అక్షరం *hostname, size_t hlen)
{
  size_t i;
  కొరకు(i = 0; i < hlen; i++) {
    ఒకవేళ(invalid_host_char[(గుర్తులేని అక్షరం)hostname[i]])
      తిరిగి CURLUE_BAD_HOSTNAME;
  }
  ఒకవేళ((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    తిరిగి CURLUE_BAD_HOSTNAME;
  లేకపోతే ఒకవేళ((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    తిరిగి CURLUE_BAD_HOSTNAME;
  తిరిగి CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
స్థిర CURLUcode hostname_check6(నిర్మాణం Curl_URL *u, అక్షరం *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  ఒకవేళ(!hlen)
    తిరిగి CURLUE_NO_HOST;
  లేకపోతే ఒకవేళ(hostname[0] == '[')
    తిరిగి ipv6_parse(u, hostname, hlen);

  తిరిగి hostname_check(hostname, hlen);
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
UNITTEST పూర్ణాంకం ipv4_normalize(నిర్మాణం dynbuf *host);
UNITTEST పూర్ణాంకం ipv4_normalize(నిర్మాణం dynbuf *host)
{
  bool పూర్తి = FALSE;
  పూర్ణాంకం n = 0;
  స్థిరరాశి అక్షరం *c = curlx_dyn_ptr(host);
  గుర్తులేని పూర్ణాంకం parts[4] = { 0, 0, 0, 0 };
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!ISDIGIT(*c))
    తిరిగి HOST_NAME;

  అయితే(!పూర్తి) {
    పూర్ణాంకం rc;
    curl_off_t l;
    ఒకవేళ(*c == '0') {
      ఒకవేళ((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        ఒకవేళ(rc)
          తిరిగి HOST_NAME;
      }
      లేకపోతే
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    లేకపోతే
      rc = curlx_str_సంఖ్య(&c, &l, UINT_MAX);

    ఒకవేళ(rc) {
      ఒకవేళ(!n || (rc != STRE_NO_అంకె) || *c)
        తిరిగి HOST_NAME;
      n--;
    }
    లేకపోతే
      parts[n] = (గుర్తులేని పూర్ణాంకం)l;

    ఎంపిక(*c) {
    స్థితి '.':
      ఒకవేళ(n == 3) {
        ఒకవేళ(c[1])
          /* something follows this dot */
          తిరిగి HOST_NAME;
        పూర్తి = TRUE;
      }
      లేకపోతే {
        n++;
        c++;
      }
      ఆపు;

    స్థితి '\0':
      పూర్తి = TRUE;
      ఆపు;

    అప్రమేయ:
      తిరిగి HOST_NAME;
    }
  }

  ఎంపిక(n) {
  స్థితి 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    ఫలితం = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    ఆపు;
  స్థితి 1: /* a.b -- 8.24 bits */
    ఒకవేళ((parts[0] > 0xff) || (parts[1] > 0xffffff))
      తిరిగి HOST_NAME;
    curlx_dyn_reset(host);
    ఫలితం = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    ఆపు;
  స్థితి 2: /* a.b.c -- 8.8.16 bits */
    ఒకవేళ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      తిరిగి HOST_NAME;
    curlx_dyn_reset(host);
    ఫలితం = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    ఆపు;
  స్థితి 3: /* a.b.c.d -- 8.8.8.8 bits */
    ఒకవేళ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      తిరిగి HOST_NAME;
    curlx_dyn_reset(host);
    ఫలితం = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    ఆపు;
  }
  ఒకవేళ(ఫలితం)
    తిరిగి HOST_ERROR;
  తిరిగి HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
స్థిర CURLUcode urldecode_host(నిర్మాణం dynbuf *host)
{
  స్థిరరాశి అక్షరం *చొప్పున;
  స్థిరరాశి అక్షరం *hostname = curlx_dyn_ptr(host);
  చొప్పున = memchr(hostname, '%', curlx_dyn_len(host));
  ఒకవేళ(!చొప్పున)
    /* nothing to decode */
    తిరిగి CURLUE_OK;
  లేకపోతే {
    /* encoded */
    size_t dlen;
    అక్షరం *decoded;
    CURLcode ఫలితం = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    ఒకవేళ(ఫలితం)
      తిరిగి CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    ఫలితం = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    ఒకవేళ(ఫలితం)
      తిరిగి cc2cu(ఫలితం);
  }

  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode parse_authority(నిర్మాణం Curl_URL *u,
                                 స్థిరరాశి అక్షరం *auth, size_t authlen,
                                 గుర్తులేని పూర్ణాంకం flags,
                                 నిర్మాణం dynbuf *host,
                                 bool has_scheme)
{
  size_t స్థానభ్రంశం;
  CURLUcode uc;
  CURLcode ఫలితం;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &స్థానభ్రంశం);
  ఒకవేళ(uc)
    తిరిగి uc;

  ఫలితం = curlx_dyn_addn(host, auth + స్థానభ్రంశం, authlen - స్థానభ్రంశం);
  ఒకవేళ(ఫలితం) {
    uc = cc2cu(ఫలితం);
    తిరిగి uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_పోర్టు(u, host, has_scheme);

  ఒకవేళ(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  ఒకవేళ(!uc)
    uc = urldecode_host(host);
  ఒకవేళ(uc)
    ;
  లేకపోతే ఒకవేళ(auth[స్థానభ్రంశం] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  లేకపోతే {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    పూర్ణాంకం type = ipv4_normalize(host);

    ఒకవేళ(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    లేకపోతే ఒకవేళ(type == HOST_ERROR)
      uc = CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_సమితి_authority(CURLU *u, స్థిరరాశి అక్షరం *authority)
{
  CURLUcode ures;
  నిర్మాణం dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_ఆదానం_నిడివి);

  ures = parse_authority(u, authority, అక్షరశ్రేణి_పొడవు(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  ఒకవేళ(ures)
    curlx_dyn_free(&host);
  లేకపోతే {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  తిరిగి ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

స్థిర bool is_dot(స్థిరరాశి అక్షరం **str, size_t *clen)
{
  స్థిరరాశి అక్షరం *p = *str;
  ఒకవేళ(*p == '.') {
    (*str)++;
    (*clen)--;
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

#నిర్వచించు ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
స్థిర bool needs_dedotdot(స్థిరరాశి అక్షరం *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  ఒకవేళ(pn < 2)
    తిరిగి FALSE;
  ఒకవేళ(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    తిరిగి FALSE;
  అయితే(pn) {
    ఒకవేళ(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      ఒకవేళ(!pn || ISSLASH(*p))
        తిరిగి TRUE;
      /* "../" or ".." before end of string */
      లేకపోతే ఒకవేళ(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        తిరిగి TRUE;
    }
    లేకపోతే {
      p++;
      pn--;
    }
  }
  తిరిగి FALSE;
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
UNITTEST పూర్ణాంకం dedotdotify(స్థిరరాశి అక్షరం *ఆదానం, size_t clen, అక్షరం **outp);
UNITTEST పూర్ణాంకం dedotdotify(స్థిరరాశి అక్షరం *ఆదానం, size_t clen, అక్షరం **outp)
{
  నిర్మాణం dynbuf బహిర్గతం;
  CURLcode ఫలితం = CURLE_OK;

  /* variables for leading dot checks */
  స్థిరరాశి అక్షరం *dinput = ఆదానం;
  size_t dlen = clen;

  *outp = ఖాళీ;
  ఒకవేళ(!needs_dedotdot(ఆదానం, clen))
    తిరిగి 0;

  curlx_dyn_init(&బహిర్గతం, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  ఒకవేళ(is_dot(&dinput, &dlen)) {
    ఒకవేళ(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      ఆదానం = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    లేకపోతే ఒకవేళ(is_dot(&dinput, &dlen)) {
      ఒకవేళ(!dlen)
        /* .. [end] */
        వెళ్ళు చివర;
      లేకపోతే ఒకవేళ(ISSLASH(*dinput)) {
        /* ../ */
        ఆదానం = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  అయితే(clen && !ఫలితం) { /* until end of path content */
    ఒకవేళ(ISSLASH(*ఆదానం)) {
      స్థిరరాశి అక్షరం *p = &ఆదానం[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      ఒకవేళ(is_dot(&p, &blen)) {
        ఒకవేళ(!blen) { /* /. */
          ఫలితం = curlx_dyn_addn(&బహిర్గతం, "/", 1);
          ఆపు;
        }
        లేకపోతే ఒకవేళ(ISSLASH(*p)) { /* /./ */
          ఆదానం = p;
          clen = blen;
          కొనసాగు;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        లేకపోతే ఒకవేళ(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&బహిర్గతం);
          ఒకవేళ(len) {
            స్థిరరాశి అక్షరం *ptr = curlx_dyn_ptr(&బహిర్గతం);
            స్థిరరాశి అక్షరం *చివరి = memrchr(ptr, '/', len);
            ఒకవేళ(చివరి)
              /* trim the output at the slash */
              curlx_dyn_setlen(&బహిర్గతం, చివరి - ptr);
          }

          ఒకవేళ(blen) { /* /../ */
            ఆదానం = p;
            clen = blen;
            కొనసాగు;
          }
          ఫలితం = curlx_dyn_addn(&బహిర్గతం, "/", 1);
          ఆపు;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    ఫలితం = curlx_dyn_addn(&బహిర్గతం, ఆదానం, 1);
    ఆదానం++;
    clen--;
  }
చివర:
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(curlx_dyn_len(&బహిర్గతం))
      *outp = curlx_dyn_ptr(&బహిర్గతం);
    లేకపోతే {
      *outp = curlx_strdup("");
      ఒకవేళ(!*outp)
        తిరిగి 1;
    }
  }
  తిరిగి ఫలితం ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(స్థిరరాశి అక్షరం *url, size_t urllen, CURLU *u,
                              స్థిరరాశి అక్షరం **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(స్థిరరాశి అక్షరం *url, size_t urllen, CURLU *u,
                              స్థిరరాశి అక్షరం **pathp, size_t *pathlenp)
{
  స్థిరరాశి అక్షరం *మార్గం;
  size_t pathlen;

  *pathp = ఖాళీ;
  *pathlenp = 0;
  ఒకవేళ(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    తిరిగి CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  మార్గం = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  ఒకవేళ(మార్గం[0] != '/')
    తిరిగి CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  ఒకవేళ(మార్గం[1] == '/') {
    /* swallow the two slashes */
    స్థిరరాశి అక్షరం *ptr = &మార్గం[2];

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
    ఒకవేళ(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_ఉపసర్గ(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      ఒకవేళ(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      లేకపోతే
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        తిరిగి CURLUE_BAD_FILE_URL;
    }

    మార్గం = ptr;
    pathlen = urllen - (ptr - url);
  }

#ఒకవేళ !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  ఒకవేళ(('/' == మార్గం[0] && STARTS_WITH_URL_DRIVE_ఉపసర్గ(&మార్గం[1])) ||
     STARTS_WITH_URL_DRIVE_ఉపసర్గ(మార్గం)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    తిరిగి CURLUE_BAD_FILE_URL;
  }
#లేకపోతే
  /* If the path starts with a slash and a drive letter, ditch the slash */
  ఒకవేళ('/' == మార్గం[0] && STARTS_WITH_URL_DRIVE_ఉపసర్గ(&మార్గం[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    మార్గం++;
    pathlen--;
  }
#అయితే_ముగింపు
  u->scheme = curlx_strdup("file");
  ఒకవేళ(!u->scheme)
    తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  *pathp = మార్గం;
  *pathlenp = pathlen;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode parse_scheme(స్థిరరాశి అక్షరం *url, CURLU *u, అక్షరం *schemebuf,
                              size_t schemelen, గుర్తులేని పూర్ణాంకం flags,
                              స్థిరరాశి అక్షరం **hostpp)
{
  /* clear path */
  స్థిరరాశి అక్షరం *schemep = ఖాళీ;

  ఒకవేళ(schemelen) {
    పూర్ణాంకం అంకె_slashes = 0;
    స్థిరరాశి అక్షరం *p = &url[schemelen + 1];
    ఒకవేళ(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      తిరిగి CURLUE_UNSUPPORTED_SCHEME;

    ఒకవేళ(!ISSLASH(*p))
      /* less than one */
      తిరిగి CURLUE_BAD_SLASHES;
    ఒకవేళ((flags & CURLU_NO_AUTHORITY)) {
      అయితే(ISSLASH(*p) && (అంకె_slashes < 2)) {
        p++;
        అంకె_slashes++;
      }
    }
    లేకపోతే {
      అయితే(ISSLASH(*p) && (అంకె_slashes < 4)) {
        p++;
        అంకె_slashes++;
      }
      ఒకవేళ(అంకె_slashes > 3)
        తిరిగి CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  లేకపోతే {
    /* no scheme! */

    ఒకవేళ(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      తిరిగి CURLUE_BAD_SCHEME;

    ఒకవేళ(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  ఒకవేళ(schemep) {
    u->scheme = curlx_strdup(schemep);
    ఒకవేళ(!u->scheme)
      తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode guess_scheme(CURLU *u, నిర్మాణం dynbuf *host)
{
  స్థిరరాశి అక్షరం *hostname = curlx_dyn_ptr(host);
  స్థిరరాశి అక్షరం *schemep = ఖాళీ;
  /* legacy curl-style guess based on hostname */
  ఒకవేళ(checkprefix("ftp.", hostname))
    schemep = "ftp";
  లేకపోతే ఒకవేళ(checkprefix("dict.", hostname))
    schemep = "dict";
  లేకపోతే ఒకవేళ(checkprefix("ldap.", hostname))
    schemep = "ldap";
  లేకపోతే ఒకవేళ(checkprefix("imap.", hostname))
    schemep = "imap";
  లేకపోతే ఒకవేళ(checkprefix("smtp.", hostname))
    schemep = "smtp";
  లేకపోతే ఒకవేళ(checkprefix("pop3.", hostname))
    schemep = "pop3";
  లేకపోతే
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  ఒకవేళ(!u->scheme)
    తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  u->guessed_scheme = TRUE;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode handle_fragment(CURLU *u, స్థిరరాశి అక్షరం *fragment,
                                 size_t fraglen, గుర్తులేని పూర్ణాంకం flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  ఒకవేళ(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    ఒకవేళ(flags & CURLU_URLENCODE) {
      నిర్మాణం dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_ఆదానం_నిడివి);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      ఒకవేళ(ures)
        తిరిగి ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    లేకపోతే {
      ఒకవేళ(badoctets(fragment, fraglen, flags))
        తిరిగి CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      ఒకవేళ(!u->fragment)
        తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode handle_query(CURLU *u, స్థిరరాశి అక్షరం *query,
                              size_t qlen, గుర్తులేని పూర్ణాంకం flags)
{
  u->query_present = TRUE;
  ఒకవేళ(qlen > 1) {
    ఒకవేళ(flags & CURLU_URLENCODE) {
      నిర్మాణం dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_ఆదానం_నిడివి);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      ఒకవేళ(ures)
        తిరిగి ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    లేకపోతే {
      ఒకవేళ(badoctets(query, qlen, flags))
        తిరిగి CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      ఒకవేళ(!u->query)
        తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  లేకపోతే {
    /* single byte query */
    u->query = curlx_strdup("");
    ఒకవేళ(!u->query)
      తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode handle_మార్గం(CURLU *u, స్థిరరాశి అక్షరం *మార్గం,
                             size_t pathlen, గుర్తులేని పూర్ణాంకం flags,
                             bool is_file)
{
  CURLUcode ures;
  ఒకవేళ(pathlen && (flags & CURLU_URLENCODE)) {
    నిర్మాణం dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ఆదానం_నిడివి);
    ures = urlencode_str(&enc, మార్గం, pathlen, TRUE, QUERY_NO);
    ఒకవేళ(ures)
      తిరిగి ures;
    pathlen = curlx_dyn_len(&enc);
    మార్గం = u->మార్గం = curlx_dyn_ptr(&enc);
  }

  ఒకవేళ(pathlen >= (size_t)(1 + !is_file)) {
    ఒకవేళ(badoctets(మార్గం, pathlen, flags))
      తిరిగి CURLUE_BAD_మార్గం;

    /* paths for file:// scheme can be one byte, others need to be two */
    ఒకవేళ(!u->మార్గం) {
      u->మార్గం = curlx_memdup0(మార్గం, pathlen);
      ఒకవేళ(!u->మార్గం)
        తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      మార్గం = u->మార్గం;
    }
    లేకపోతే ఒకవేళ(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->మార్గం[pathlen] = 0;

    ఒకవేళ(!(flags & CURLU_మార్గం_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      అక్షరం *dedot;
      పూర్ణాంకం err = dedotdotify(మార్గం, pathlen, &dedot);
      ఒకవేళ(err)
        తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఒకవేళ(dedot) {
        curlx_free(u->మార్గం);
        u->మార్గం = dedot;
      }
    }
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode parseurl(స్థిరరాశి అక్షరం *url, CURLU *u, గుర్తులేని పూర్ణాంకం flags)
{
  స్థిరరాశి అక్షరం *మార్గం;
  size_t pathlen;
  అక్షరం schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  నిర్మాణం dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = అక్షరశ్రేణి_పొడవు(url);
  ఒకవేళ(urllen > CURL_MAX_ఆదానం_నిడివి)
    తిరిగి CURLUE_MALFORMED_ఆదానం;

  curlx_dyn_init(&host, CURL_MAX_ఆదానం_నిడివి);

  schemelen = Curl_is_absolute_url(url, schemebuf, పరిమాణం(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  ఒకవేళ(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &మార్గం, &pathlen);
  }
  లేకపోతే {
    స్థిరరాశి అక్షరం *hostp = ఖాళీ;
    స్థిరరాశి అక్షరం *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    ఒకవేళ(ures)
      వెళ్ళు fail;

    /* find the end of the hostname + port number */
    p = hostp;
    అయితే(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    మార్గం = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (మార్గం - url);
    ఒకవేళ(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      ఒకవేళ(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    లేకపోతే ఒకవేళ(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      ఒకవేళ(curlx_dyn_కూడు(&host, ""))
        ures = CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    లేకపోతే
      ures = CURLUE_NO_HOST;
  }
  ఒకవేళ(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    స్థిరరాశి అక్షరం *fragment = memchr(మార్గం, '#', pathlen);
    ఒకవేళ(fragment) {
      size_t fraglen = pathlen - (fragment - మార్గం);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  ఒకవేళ(!ures) {
    స్థిరరాశి అక్షరం *query = memchr(మార్గం, '?', pathlen);
    ఒకవేళ(query) {
      size_t qlen = pathlen - (query - మార్గం);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  ఒకవేళ(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_మార్గం(u, మార్గం, pathlen, flags, is_file);
  ఒకవేళ(!ures) {
    u->host = curlx_dyn_ptr(&host);
    తిరిగి CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  తిరిగి ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
స్థిర CURLUcode parseurl_and_replace(స్థిరరాశి అక్షరం *url, CURLU *u,
                                      గుర్తులేని పూర్ణాంకం flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, పరిమాణం(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  ఒకవేళ(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  తిరిగి ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
స్థిర CURLUcode redirect_url(స్థిరరాశి అక్షరం *పునాది, స్థిరరాశి అక్షరం *relurl,
                              CURLU *u, గుర్తులేని పూర్ణాంకం flags)
{
  నిర్మాణం dynbuf urlbuf;
  bool host_మారింది = FALSE;
  స్థిరరాశి అక్షరం *useurl = relurl;
  స్థిరరాశి అక్షరం *cutoff = ఖాళీ;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  స్థిరరాశి అక్షరం *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  స్థిరరాశి అక్షరం *protsep = పునాది + అక్షరశ్రేణి_పొడవు(scheme) + 3;
  DEBUGASSERT(పునాది && relurl && u); /* all set here */
  ఒకవేళ(!పునాది)
    తిరిగి CURLUE_MALFORMED_ఆదానం; /* should never happen */

  /* handle different relative URL types */
  ఎంపిక(relurl[0]) {
  స్థితి '/':
    ఒకవేళ(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_మారింది = TRUE;
    }
    లేకపోతే
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    ఆపు;

  స్థితి '#':
    /* fragment-only change */
    ఒకవేళ(u->fragment_present)
      cutoff = strchr(protsep, '#');
    ఆపు;

  అప్రమేయ:
    /* path or query-only change */
    ఒకవేళ(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    లేకపోతే ఒకవేళ(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    ఒకవేళ(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : అక్షరశ్రేణి_పొడవు(protsep));
      ఒకవేళ(cutoff)
        cutoff++; /* truncate after last slash */
    }
    ఆపు;
  }

  prelen = cutoff ? (size_t)(cutoff - పునాది) : అక్షరశ్రేణి_పొడవు(పునాది);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_ఆదానం_నిడివి);

  ఒకవేళ(!curlx_dyn_addn(&urlbuf, పునాది, prelen) &&
     !urlencode_str(&urlbuf, useurl, అక్షరశ్రేణి_పొడవు(useurl), !host_మారింది,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_మార్గం_AS_IS);
  }
  లేకపోతే
    uc = CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  curlx_dyn_free(&urlbuf);
  తిరిగి uc;
}

/*
 */
CURLU *curl_url(శూన్య)
{
  తిరిగి curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_URL));
}

శూన్య curl_url_cleanup(CURLU *u)
{
  ఒకవేళ(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#నిర్వచించు DUP(dest, src, name)                    \
  చేయి {                                          \
    ఒకవేళ((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      ఒకవేళ(!(dest)->name)                         \
        వెళ్ళు fail;                              \
    }                                           \
  } అయితే(0)

CURLU *curl_url_dup(స్థిరరాశి CURLU *లోపల)
{
  నిర్మాణం Curl_URL *u;

  ఒకవేళ(!లోపల)
    తిరిగి ఖాళీ;

  u = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_URL));
  ఒకవేళ(u) {
    DUP(u, లోపల, scheme);
    DUP(u, లోపల, user);
    DUP(u, లోపల, password);
    DUP(u, లోపల, options);
    DUP(u, లోపల, host);
    DUP(u, లోపల, మార్గం);
    DUP(u, లోపల, query);
    DUP(u, లోపల, fragment);
    DUP(u, లోపల, zoneid);
    u->portnum = లోపల->portnum;
    u->పోర్టు_present = లోపల->పోర్టు_present;
    u->fragment_present = లోపల->fragment_present;
    u->query_present = లోపల->query_present;
    u->guessed_scheme = లోపల->guessed_scheme;
  }
  తిరిగి u;
fail:
  curl_url_cleanup(u);
  తిరిగి ఖాళీ;
}

#నిర్వచితం_కాకపోతే USE_IDN
#నిర్వచించు host_decode(x, y) CURLUE_LACKS_IDN
#నిర్వచించు host_encode(x, y) CURLUE_LACKS_IDN
#లేకపోతే
స్థిర CURLUcode host_decode(స్థిరరాశి అక్షరం *host, అక్షరం **allochost)
{
  CURLcode ఫలితం = Curl_idn_decode(host, allochost);
  ఒకవేళ(ఫలితం)
    తిరిగి (ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం) ?
      CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం : CURLUE_BAD_HOSTNAME;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode host_encode(స్థిరరాశి అక్షరం *host, అక్షరం **allochost)
{
  CURLcode ఫలితం = Curl_idn_encode(host, allochost);
  ఒకవేళ(ఫలితం)
    తిరిగి (ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం) ?
      CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం : CURLUE_BAD_HOSTNAME;
  తిరిగి CURLUE_OK;
}
#అయితే_ముగింపు

స్థిర CURLUcode urlget_format(స్థిరరాశి CURLU *u, CURLUPart what,
                               స్థిరరాశి అక్షరం *ptr, అక్షరం **partp,
                               bool plusdecode, గుర్తులేని పూర్ణాంకం flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = అక్షరశ్రేణి_పొడవు(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  అక్షరం *part = curlx_memdup0(ptr, partlen);
  *partp = ఖాళీ;
  ఒకవేళ(!part)
    తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  ఒకవేళ(plusdecode) {
    /* convert + to space */
    అక్షరం *plus = part;
    size_t i = 0;
    కొరకు(i = 0; i < partlen; ++plus, i++) {
      ఒకవేళ(*plus == '+')
        *plus = ' ';
    }
  }
  ఒకవేళ(urldecode) {
    అక్షరం *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode ఫలితం = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    ఒకవేళ(ఫలితం)
      తిరిగి CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  ఒకవేళ(urlencode) {
    నిర్మాణం dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ఆదానం_నిడివి);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    ఒకవేళ(uc)
      తిరిగి uc;
    part = curlx_dyn_ptr(&enc);
  }
  లేకపోతే ఒకవేళ(punycode) {
    ఒకవేళ(!Curl_is_ASCII_name(u->host)) {
      అక్షరం *punyversion = ఖాళీ;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      ఒకవేళ(uc)
        తిరిగి uc;
      uc = hostname_check(punyversion, అక్షరశ్రేణి_పొడవు(punyversion));
      ఒకవేళ(uc) {
        curlx_free(punyversion);
        తిరిగి uc;
      }
      part = punyversion;
    }
  }
  లేకపోతే ఒకవేళ(depunyfy && Curl_is_ASCII_name(u->host)) {
    అక్షరం *unpunified = ఖాళీ;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    ఒకవేళ(uc)
      తిరిగి uc;
    part = unpunified;
  }
  *partp = part;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode file_url(స్థిరరాశి CURLU *u, అక్షరం **part,
                          స్థిరరాశి అక్షరం *fragmentsep,
                          స్థిరరాశి అక్షరం *querysep)
{
  అక్షరం *url = curl_maprintf("file://%s%s%s%s%s",
                            u->మార్గం, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  ఒకవేళ(!url)
    తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  *part = url;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode urlget_url(స్థిరరాశి CURLU *u, అక్షరం **part, గుర్తులేని పూర్ణాంకం flags)
{
  అక్షరం *url;
  అక్షరం *allochost = ఖాళీ;
  స్థిరరాశి అక్షరం *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_ఖాళీలు)) ?
    "#" : "";
  స్థిరరాశి అక్షరం *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_ఖాళీలు)) ?
    "?" : "";
  అక్షరం portbuf[7];
  ఒకవేళ(curl_strequal("file", u->scheme))
    తిరిగి file_url(u, part, fragmentsep, querysep);
  లేకపోతే ఒకవేళ(!u->host)
    తిరిగి CURLUE_NO_HOST;
  లేకపోతే {
    స్థిరరాశి అక్షరం *scheme;
    అక్షరం *options = u->options;
    అక్షరం *పోర్టు = ఖాళీ;
    స్థిరరాశి నిర్మాణం Curl_scheme *h = ఖాళీ;
    అక్షరం schemebuf[MAX_SCHEME_LEN + 5];
    ఒకవేళ(u->scheme)
      scheme = u->scheme;
    లేకపోతే ఒకవేళ(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    లేకపోతే
      తిరిగి CURLUE_NO_SCHEME;

    ఒకవేళ(u->పోర్టు_present) {
      curl_msnprintf(portbuf, పరిమాణం(portbuf), "%u", u->portnum);
      పోర్టు = portbuf;
    }

    h = Curl_get_scheme(scheme);
    ఒకవేళ(h) {
      ఒకవేళ(!u->పోర్టు_present && (flags & CURLU_DEFAULT_పోర్టు)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, పరిమాణం(portbuf), "%u", h->defport);
        పోర్టు = portbuf;
      }
      లేకపోతే ఒకవేళ(u->పోర్టు_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_పోర్టు)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        పోర్టు = ఖాళీ;
      }

      ఒకవేళ(!(h->flags & PROTOPT_URLOPTIONS))
        options = ఖాళీ;
    }

    ఒకవేళ(u->host[0] == '[') {
      ఒకవేళ(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        నిర్మాణం dynbuf enc;
        size_t hostlen = అక్షరశ్రేణి_పొడవు(u->host);
        curlx_dyn_init(&enc, CURL_MAX_ఆదానం_నిడివి);
        ఒకవేళ(curlx_dyn_addf(&enc, "%.*s%%25%s]", (పూర్ణాంకం)hostlen - 1, u->host,
                          u->zoneid))
          తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    లేకపోతే ఒకవేళ(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ఖాళీ, u->host, 0);
      ఒకవేళ(!allochost)
        తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    లేకపోతే ఒకవేళ(flags & CURLU_PUNYCODE) {
      ఒకవేళ(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        ఒకవేళ(!ret)
          ret = hostname_check(allochost, అక్షరశ్రేణి_పొడవు(allochost));
        ఒకవేళ(ret) {
          curlx_safefree(allochost);
          తిరిగి ret;
        }
      }
    }
    లేకపోతే ఒకవేళ(flags & CURLU_PUNY2IDN) {
      ఒకవేళ(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        ఒకవేళ(ret)
          తిరిగి ret;
      }
    }

    ఒకవేళ(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, పరిమాణం(schemebuf), "%s://", scheme);
    లేకపోతే
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
                        పోర్టు ? ":" : "",
                        పోర్టు ? పోర్టు : "",
                        u->మార్గం ? u->మార్గం : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  ఒకవేళ(!url)
    తిరిగి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  *part = url;
  తిరిగి CURLUE_OK;
}

CURLUcode curl_url_get(స్థిరరాశి CURLU *u, CURLUPart what,
                       అక్షరం **part, గుర్తులేని పూర్ణాంకం flags)
{
  స్థిరరాశి అక్షరం *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  అక్షరం portbuf[7];
  bool plusdecode = FALSE;
  ఒకవేళ(!u)
    తిరిగి CURLUE_BAD_HANDLE;
  ఒకవేళ(!part)
    తిరిగి CURLUE_BAD_PARTPOINTER;
  *part = ఖాళీ;

  ఎంపిక(what) {
  స్థితి CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    ఒకవేళ((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      తిరిగి CURLUE_NO_SCHEME;
    ఆపు;
  స్థితి CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    ఆపు;
  స్థితి CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    ఆపు;
  స్థితి CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    ఆపు;
  స్థితి CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    ఆపు;
  స్థితి CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    ఆపు;
  స్థితి CURLUPART_పోర్టు:
    ptr = ఖాళీ;
    ifmissing = CURLUE_NO_పోర్టు;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    ఒకవేళ(u->పోర్టు_present) {
      స్థిరరాశి నిర్మాణం Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ఖాళీ;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      ఒకవేళ(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_పోర్టు)) {
        ptr = ఖాళీ;
      }
      లేకపోతే {
        curl_msnprintf(portbuf, పరిమాణం(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    లేకపోతే ఒకవేళ((flags & CURLU_DEFAULT_పోర్టు) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      స్థిరరాశి నిర్మాణం Curl_scheme *h = Curl_get_scheme(u->scheme);
      ఒకవేళ(h) {
        curl_msnprintf(portbuf, పరిమాణం(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    ఆపు;
  స్థితి CURLUPART_మార్గం:
    ptr = u->మార్గం;
    ఒకవేళ(!ptr)
      ptr = "/";
    ఆపు;
  స్థితి CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    ఒకవేళ(ptr && !ptr[0] && !(flags & CURLU_GET_ఖాళీలు))
      /* there was a blank query and the user does not ask for it */
      ptr = ఖాళీ;
    ఆపు;
  స్థితి CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    ఒకవేళ(!ptr && u->fragment_present && flags & CURLU_GET_ఖాళీలు)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    ఆపు;
  స్థితి CURLUPART_URL:
    తిరిగి urlget_url(u, part, flags);
  అప్రమేయ:
    ptr = ఖాళీ;
    ఆపు;
  }
  ఒకవేళ(ptr)
    తిరిగి urlget_format(u, what, ptr, part, plusdecode, flags);

  తిరిగి ifmissing;
}

స్థిర CURLUcode సమితి_url_scheme(CURLU *u, స్థిరరాశి అక్షరం *scheme,
                                గుర్తులేని పూర్ణాంకం flags)
{
  size_t plen = అక్షరశ్రేణి_పొడవు(scheme);
  స్థిరరాశి నిర్మాణం Curl_scheme *h = ఖాళీ;
  ఒకవేళ((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    తిరిగి CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  ఒకవేళ(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->నిరంతరం))
    తిరిగి CURLUE_UNSUPPORTED_SCHEME;
  ఒకవేళ(!h) {
    స్థిరరాశి అక్షరం *s = scheme;
    ఒకవేళ(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      అయితే(--plen) {
        ఒకవేళ(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        లేకపోతే
          తిరిగి CURLUE_BAD_SCHEME;
      }
    }
    లేకపోతే
      తిరిగి CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode సమితి_url_పోర్టు(CURLU *u, స్థిరరాశి అక్షరం *provided_పోర్టు)
{
  curl_off_t పోర్టు;
  ఒకవేళ(!ISDIGIT(provided_పోర్టు[0]))
    /* not a number */
    తిరిగి CURLUE_BAD_పోర్టు_సంఖ్య;
  ఒకవేళ(curlx_str_సంఖ్య(&provided_పోర్టు, &పోర్టు, 0xffff) || *provided_పోర్టు)
    /* weirdly provided number, not good! */
    తిరిగి CURLUE_BAD_పోర్టు_సంఖ్య;
  u->portnum = (uint16_t)పోర్టు;
  u->పోర్టు_present = TRUE;
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode సమితి_url(CURLU *u, స్థిరరాశి అక్షరం *url, size_t part_కొలత,
                         గుర్తులేని పూర్ణాంకం flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  అక్షరం *oldurl = ఖాళీ;

  ఒకవేళ(!part_కొలత) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    ఒకవేళ(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      తిరిగి CURLUE_OK;
    }
    ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం)
      తిరిగి uc;
    తిరిగి CURLUE_MALFORMED_ఆదానం;
  }

  /* if the new URL is absolute replace the existing with the new. */
  ఒకవేళ(Curl_is_absolute_url(url, ఖాళీ, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    తిరిగి parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_ఖాళీలు);
  ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం)
    తిరిగి uc;
  లేకపోతే ఒకవేళ(uc)
    తిరిగి parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  తిరిగి uc;
}

స్థిర CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  ఎంపిక(what) {
  స్థితి CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, పరిమాణం(నిర్మాణం Curl_URL));
    ఆపు;
  స్థితి CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    ఆపు;
  స్థితి CURLUPART_USER:
    curlx_safefree(u->user);
    ఆపు;
  స్థితి CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    ఆపు;
  స్థితి CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    ఆపు;
  స్థితి CURLUPART_HOST:
    curlx_safefree(u->host);
    ఆపు;
  స్థితి CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    ఆపు;
  స్థితి CURLUPART_పోర్టు:
    u->portnum = 0;
    u->పోర్టు_present = FALSE;
    ఆపు;
  స్థితి CURLUPART_మార్గం:
    curlx_safefree(u->మార్గం);
    ఆపు;
  స్థితి CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    ఆపు;
  స్థితి CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLUE_UNKNOWN_PART;
  }
  తిరిగి CURLUE_OK;
}

స్థిర bool allowed_లోపల_మార్గం(గుర్తులేని అక్షరం x)
{
  ఎంపిక(x) {
  స్థితి '!':
  స్థితి '$':
  స్థితి '&':
  స్థితి '\'':
  స్థితి '(':
  స్థితి ')':
  స్థితి '{':
  స్థితి '}':
  స్థితి '[':
  స్థితి ']':
  స్థితి '*':
  స్థితి '+':
  స్థితి ',':
  స్థితి ';':
  స్థితి '=':
  స్థితి ':':
  స్థితి '@':
  స్థితి '/':
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర CURLUcode url_encode_part(నిర్మాణం dynbuf *encp,
                                 స్థిరరాశి అక్షరం *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  స్థిరరాశి గుర్తులేని అక్షరం *i;

  కొరకు(i = (స్థిరరాశి గుర్తులేని అక్షరం *)part; *i; i++) {
    CURLcode ఫలితం;
    ఒకవేళ((*i == ' ') && plusencode)
      ఫలితం = curlx_dyn_addn(encp, "+", 1);
    లేకపోతే ఒకవేళ(ISUNRESERVED(*i) ||
            (pathmode && allowed_లోపల_మార్గం(*i)) ||
            ((*i == '=') && equalsencode)) {
      ఒకవేళ((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      ఫలితం = curlx_dyn_addn(encp, i, 1);
    }
    లేకపోతే {
      గుర్తులేని అక్షరం బహిర్గతం[3] = { '%' };
      Curl_hexbyte(&బహిర్గతం[1], *i);
      ఫలితం = curlx_dyn_addn(encp, బహిర్గతం, 3);
    }
    ఒకవేళ(ఫలితం)
      తిరిగి cc2cu(ఫలితం);
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode url_uppercasehex_part(నిర్మాణం dynbuf *encp,
                                       స్థిరరాశి అక్షరం *part)
{
  అక్షరం *p;
  CURLcode ఫలితం = curlx_dyn_కూడు(encp, part);
  ఒకవేళ(ఫలితం)
    తిరిగి cc2cu(ఫలితం);
  p = curlx_dyn_ptr(encp);
  అయితే(*p) {
    /* make sure percent encoded are upper case */
    ఒకవేళ((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    లేకపోతే
      p++;
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode url_append_query(CURLU *u, నిర్మాణం dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? అక్షరశ్రేణి_పొడవు(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  ఒకవేళ(querylen) {
    నిర్మాణం dynbuf qbuf;
    CURLcode ఫలితం;
    స్థిరరాశి అక్షరం *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_ఆదానం_నిడివి);

    /* add original query */
    ఫలితం = curlx_dyn_addn(&qbuf, u->query, querylen);
    ఒకవేళ(!ఫలితం && addamperand)
      /* add ampersand */
      ఫలితం = curlx_dyn_addn(&qbuf, "&", 1);
    ఒకవేళ(!ఫలితం)
      /* add new query part */
      ఫలితం = curlx_dyn_కూడు(&qbuf, newp);
    ఒకవేళ(ఫలితం)
      వెళ్ళు nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    తిరిగి CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    తిరిగి cc2cu(ఫలితం);
  }
  లేకపోతే {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  తిరిగి CURLUE_OK;
}

స్థిర CURLUcode url_sethost(CURLU *u, నిర్మాణం dynbuf *encp,
                             bool urlencode,
                             గుర్తులేని పూర్ణాంకం flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  అక్షరం *newp = curlx_dyn_ptr(encp);
  ఒకవేళ(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  లేకపోతే ఒకవేళ(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    అక్షరం *decoded = ఖాళీ;
    CURLcode ఫలితం = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    ఒకవేళ(ఫలితం || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    లేకపోతే {
      నిర్మాణం dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_ఆదానం_నిడివి);
      ఒకవేళ(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      లేకపోతే ఒకవేళ(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        ఒకవేళ(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  లేకపోతే ఒకవేళ(hostname_check6(u, newp, n))
    bad = TRUE;
  లేకపోతే ఒకవేళ(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  ఒకవేళ(bad) {
    curlx_dyn_free(encp);
    తిరిగి CURLUE_BAD_HOSTNAME;
  }
  తిరిగి CURLUE_OK;
}

CURLUcode curl_url_సమితి(CURLU *u, CURLUPart what,
                       స్థిరరాశి అక్షరం *part, గుర్తులేని పూర్ణాంకం flags)
{
  అక్షరం **storep = ఖాళీ;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  ఒకవేళ(!u)
    తిరిగి CURLUE_BAD_HANDLE;
  ఒకవేళ(!part)
    /* setting a part to NULL clears it */
    తిరిగి urlset_clear(u, what);

  nalloc = అక్షరశ్రేణి_పొడవు(part);
  ఒకవేళ(nalloc > CURL_MAX_ఆదానం_నిడివి)
    /* excessive input length */
    తిరిగి CURLUE_MALFORMED_ఆదానం;

  ఎంపిక(what) {
  స్థితి CURLUPART_SCHEME: {
    CURLUcode స్థితి_2 = సమితి_url_scheme(u, part, flags);
    ఒకవేళ(స్థితి_2)
      తిరిగి స్థితి_2;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    ఆపు;
  }
  స్థితి CURLUPART_USER:
    storep = &u->user;
    ఆపు;
  స్థితి CURLUPART_PASSWORD:
    storep = &u->password;
    ఆపు;
  స్థితి CURLUPART_OPTIONS:
    storep = &u->options;
    ఆపు;
  స్థితి CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    ఆపు;
  స్థితి CURLUPART_ZONEID:
    storep = &u->zoneid;
    ఆపు;
  స్థితి CURLUPART_పోర్టు:
    తిరిగి సమితి_url_పోర్టు(u, part);
  స్థితి CURLUPART_మార్గం:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->మార్గం;
    ఆపు;
  స్థితి CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    ఆపు;
  స్థితి CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    ఆపు;
  స్థితి CURLUPART_URL:
    తిరిగి సమితి_url(u, part, nalloc, flags);
  అప్రమేయ:
    తిరిగి CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    స్థిరరాశి అక్షరం *newp = ఖాళీ;
    నిర్మాణం dynbuf enc;
    CURLUcode స్థితి_2;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    ఒకవేళ(leadingslash && (part[0] != '/')) {
      CURLcode ఫలితం = curlx_dyn_addn(&enc, "/", 1);
      ఒకవేళ(ఫలితం)
        తిరిగి cc2cu(ఫలితం);
    }
    ఒకవేళ(urlencode)
      స్థితి_2 = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    లేకపోతే
      స్థితి_2 = url_uppercasehex_part(&enc, part);
    ఒకవేళ(!స్థితి_2) {
      newp = curlx_dyn_ptr(&enc);

      ఒకవేళ(appendquery && newp)
        తిరిగి url_append_query(u, &enc);
      లేకపోతే ఒకవేళ(what == CURLUPART_HOST)
        స్థితి_2 = url_sethost(u, &enc, urlencode, flags);
    }
    ఒకవేళ(స్థితి_2)
      తిరిగి స్థితి_2;

    ఒకవేళ(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (అక్షరం *)CURL_UNCONST(newp);
  }
  తిరిగి CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *పునాది, CURLU *href)
{
  స్థిరరాశి నిర్మాణం Curl_scheme *s = ఖాళీ;

  /* base must be an absolute URL */
  ఒకవేళ(!పునాది->scheme || !పునాది->host)
    తిరిగి FALSE;
  ఒకవేళ(href->scheme && !curl_strequal(పునాది->scheme, href->scheme))
    తిరిగి FALSE;
  ఒకవేళ(href->host) {
    ఒకవేళ(!curl_strequal(పునాది->host, href->host))
      తిరిగి FALSE;

    ఒకవేళ(పునాది->పోర్టు_present != href->పోర్టు_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(పునాది->scheme);
      ఒకవేళ(!s) /* Cannot match default port for unknown scheme */
        తిరిగి FALSE;
      /* to match, the present one must be the default port */
      ఒకవేళ((పునాది->పోర్టు_present && (పునాది->portnum != s->defport)) ||
         (href->పోర్టు_present && (href->portnum != s->defport)))
        తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ(పునాది->portnum != href->portnum) /* both present or missing */
      తిరిగి FALSE;

    ఒకవేళ(!curl_strequal(పునాది->zoneid ? పునాది->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(href->పోర్టు_present) /* no host in href, then there must be no port */
    తిరిగి FALSE;
  తిరిగి TRUE;
}

CURLUcode Curl_url_get_పోర్టు(CURLU *u, uint16_t *pport)
{
  ఒకవేళ(u->పోర్టు_present) {
    *pport = u->portnum;
    తిరిగి CURLUE_OK;
  }
  లేకపోతే ఒకవేళ(u->scheme) {
    స్థిరరాశి నిర్మాణం Curl_scheme *s = Curl_get_scheme(u->scheme);
    ఒకవేళ(s && s->defport) {
      *pport = s->defport;
      తిరిగి CURLUE_OK;
    }
  }
  *pport = 0;
  తిరిగి CURLUE_NO_పోర్టు;
}
