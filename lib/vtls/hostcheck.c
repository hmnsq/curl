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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#അന്തര്ഭാവ <netinet/in6.h>
#endif

#അന്തര്ഭാവ "curl_memrchr.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
സ്ഥിതം bool pmatch(സ്ഥിരം അക്ഷരം *hostname, size_t hostlen,
                   സ്ഥിരം അക്ഷരം *pattern, size_t patternlen)
{
  എങ്കിൽ(hostlen != patternlen)
    തിരികെ FALSE;
  തിരികെ curl_strnequal(hostname, pattern, hostlen);
}

/*
 * Match a hostname against a wildcard pattern.
 * E.g.
 *  "foo.host.com" matches "*.host.com".
 *
 * We use the matching rule described in RFC6125, section 6.4.3.
 * https://datatracker.ietf.org/doc/html/rfc6125#section-6.4.3
 *
 * In addition: ignore trailing dots in the hostnames and wildcards, so that
 * the names are used normalized. This is what the browsers do.
 *
 * Do not allow wildcard matching on IP numbers. There are apparently
 * certificates being used with an IP address in the CN field, thus making no
 * apparent distinction between a name and an IP. We need to detect the use of
 * an IP address and not wildcard match on such names.
 *
 * Only match on "*" being used for the leftmost label, not "a*", "a*b" nor
 * "*b".
 *
 * The pattern string does not necessarily need to be null-terminated.
 *
 * Return TRUE on a match. FALSE if not.
 *
 * @unittest: 1397
 */

സ്ഥിതം bool hostmatch(സ്ഥിരം അക്ഷരം *hostname,
                      size_t hostlen,
                      സ്ഥിരം അക്ഷരം *pattern,
                      size_t patternlen)
{
  സ്ഥിരം അക്ഷരം *pattern_label_end;

  DEBUGASSERT(pattern);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  എങ്കിൽ(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  എങ്കിൽ(patternlen > 0 && pattern[patternlen - 1] == '.')
    patternlen--;

  എങ്കിൽ(patternlen > 2 && strncmp(pattern, "*.", 2))
    തിരികെ pmatch(hostname, hostlen, pattern, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  അല്ലെങ്കിൽ എങ്കിൽ(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    തിരികെ FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  pattern_label_end = memchr(pattern, '.', patternlen);
  എങ്കിൽ(!pattern_label_end ||
     (memrchr(pattern, '.', patternlen) == pattern_label_end))
    തിരികെ pmatch(hostname, hostlen, pattern, patternlen);
  അല്ലെങ്കിൽ {
    സ്ഥിരം അക്ഷരം *hostname_label_end = memchr(hostname, '.', hostlen);
    എങ്കിൽ(hostname_label_end) {
      size_t skiphost = hostname_label_end - hostname;
      size_t skiplen = pattern_label_end - pattern;
      തിരികെ pmatch(hostname_label_end, hostlen - skiphost,
                    pattern_label_end, patternlen - skiplen);
    }
  }
  തിരികെ FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(സ്ഥിരം അക്ഷരം *match, size_t matchlen,
                         സ്ഥിരം അക്ഷരം *hostname, size_t hostlen)
{
  എങ്കിൽ(matchlen > 0 && hostlen > 0)
    തിരികെ hostmatch(hostname, hostlen, match, matchlen);
  തിരികെ FALSE;
}

#endif /* USE_OPENSSL || USE_SCHANNEL */
