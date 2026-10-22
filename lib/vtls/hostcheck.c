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

#अगर defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN6_H
#समावेश <netinet/in6.h>
#पूर्ण_अगर

#समावेश "curl_memrchr.h"
#समावेश "vdns/hostip.h"
#समावेश "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
अटल bool pmatch(स्थिर अक्षर *hostname, माप_प्रकार hostlen,
                   स्थिर अक्षर *pattern, माप_प्रकार patternlen)
{
  अगर(hostlen != patternlen)
    वापस FALSE;
  वापस curl_strnequal(hostname, pattern, hostlen);
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

अटल bool hostmatch(स्थिर अक्षर *hostname,
                      माप_प्रकार hostlen,
                      स्थिर अक्षर *pattern,
                      माप_प्रकार patternlen)
{
  स्थिर अक्षर *pattern_label_end;

  DEBUGASSERT(pattern);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  अगर(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  अगर(patternlen > 0 && pattern[patternlen - 1] == '.')
    patternlen--;

  अगर(patternlen > 2 && म_भेदन(pattern, "*.", 2))
    वापस pmatch(hostname, hostlen, pattern, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  अन्यथा अगर(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    वापस FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  pattern_label_end = स_प्रथम(pattern, '.', patternlen);
  अगर(!pattern_label_end ||
     (memrchr(pattern, '.', patternlen) == pattern_label_end))
    वापस pmatch(hostname, hostlen, pattern, patternlen);
  अन्यथा {
    स्थिर अक्षर *hostname_label_end = स_प्रथम(hostname, '.', hostlen);
    अगर(hostname_label_end) {
      माप_प्रकार skiphost = hostname_label_end - hostname;
      माप_प्रकार skiplen = pattern_label_end - pattern;
      वापस pmatch(hostname_label_end, hostlen - skiphost,
                    pattern_label_end, patternlen - skiplen);
    }
  }
  वापस FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(स्थिर अक्षर *match, माप_प्रकार matchlen,
                         स्थिर अक्षर *hostname, माप_प्रकार hostlen)
{
  अगर(matchlen > 0 && hostlen > 0)
    वापस hostmatch(hostname, hostlen, match, matchlen);
  वापस FALSE;
}

#पूर्ण_अगर /* USE_OPENSSL || USE_SCHANNEL */
