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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ଅନ୍ତର୍ଭାଵ <netinet/in6.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curl_memrchr.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
ସ୍ଥିର bool pmatch(ଅଚଳ ଅକ୍ଷର *hostname, size_t hostlen,
                   ଅଚଳ ଅକ୍ଷର *pattern, size_t patternlen)
{
  ଯଦି(hostlen != patternlen)
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ curl_strnequal(hostname, pattern, hostlen);
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

ସ୍ଥିର bool hostmatch(ଅଚଳ ଅକ୍ଷର *hostname,
                      size_t hostlen,
                      ଅଚଳ ଅକ୍ଷର *pattern,
                      size_t patternlen)
{
  ଅଚଳ ଅକ୍ଷର *pattern_label_end;

  DEBUGASSERT(pattern);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  ଯଦି(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  ଯଦି(patternlen > 0 && pattern[patternlen - 1] == '.')
    patternlen--;

  ଯଦି(patternlen > 2 && strncmp(pattern, "*.", 2))
    ଫେରନ୍ତୁ pmatch(hostname, hostlen, pattern, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  ଅନ୍ୟଥା ଯଦି(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    ଫେରନ୍ତୁ FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  pattern_label_end = memchr(pattern, '.', patternlen);
  ଯଦି(!pattern_label_end ||
     (memrchr(pattern, '.', patternlen) == pattern_label_end))
    ଫେରନ୍ତୁ pmatch(hostname, hostlen, pattern, patternlen);
  ଅନ୍ୟଥା {
    ଅଚଳ ଅକ୍ଷର *hostname_label_end = memchr(hostname, '.', hostlen);
    ଯଦି(hostname_label_end) {
      size_t skiphost = hostname_label_end - hostname;
      size_t skiplen = pattern_label_end - pattern;
      ଫେରନ୍ତୁ pmatch(hostname_label_end, hostlen - skiphost,
                    pattern_label_end, patternlen - skiplen);
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(ଅଚଳ ଅକ୍ଷର *match, size_t matchlen,
                         ଅଚଳ ଅକ୍ଷର *hostname, size_t hostlen)
{
  ଯଦି(matchlen > 0 && hostlen > 0)
    ଫେରନ୍ତୁ hostmatch(hostname, hostlen, match, matchlen);
  ଫେରନ୍ତୁ FALSE;
}

#endif /* USE_OPENSSL || USE_SCHANNEL */
