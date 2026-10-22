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

#ఒకవేళ defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IN6_H
#అన్తర్భావ <netinet/in6.h>
#అయితే_ముగింపు

#అన్తర్భావ "curl_memrchr.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
స్థిర bool pmatch(స్థిరరాశి అక్షరం *hostname, size_t hostlen,
                   స్థిరరాశి అక్షరం *నమూనా, size_t patternlen)
{
  ఒకవేళ(hostlen != patternlen)
    తిరిగి FALSE;
  తిరిగి curl_strnequal(hostname, నమూనా, hostlen);
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

స్థిర bool hostmatch(స్థిరరాశి అక్షరం *hostname,
                      size_t hostlen,
                      స్థిరరాశి అక్షరం *నమూనా,
                      size_t patternlen)
{
  స్థిరరాశి అక్షరం *నమూనా_label_చివర;

  DEBUGASSERT(నమూనా);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  ఒకవేళ(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  ఒకవేళ(patternlen > 0 && నమూనా[patternlen - 1] == '.')
    patternlen--;

  ఒకవేళ(patternlen > 2 && అక్షరశ్రేణి_పోలిక(నమూనా, "*.", 2))
    తిరిగి pmatch(hostname, hostlen, నమూనా, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  లేకపోతే ఒకవేళ(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    తిరిగి FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  నమూనా_label_చివర = memchr(నమూనా, '.', patternlen);
  ఒకవేళ(!నమూనా_label_చివర ||
     (memrchr(నమూనా, '.', patternlen) == నమూనా_label_చివర))
    తిరిగి pmatch(hostname, hostlen, నమూనా, patternlen);
  లేకపోతే {
    స్థిరరాశి అక్షరం *hostname_label_చివర = memchr(hostname, '.', hostlen);
    ఒకవేళ(hostname_label_చివర) {
      size_t skiphost = hostname_label_చివర - hostname;
      size_t skiplen = నమూనా_label_చివర - నమూనా;
      తిరిగి pmatch(hostname_label_చివర, hostlen - skiphost,
                    నమూనా_label_చివర, patternlen - skiplen);
    }
  }
  తిరిగి FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(స్థిరరాశి అక్షరం *సరిపోలిక, size_t matchlen,
                         స్థిరరాశి అక్షరం *hostname, size_t hostlen)
{
  ఒకవేళ(matchlen > 0 && hostlen > 0)
    తిరిగి hostmatch(hostname, hostlen, సరిపోలిక, matchlen);
  తిరిగి FALSE;
}

#అయితే_ముగింపు /* USE_OPENSSL || USE_SCHANNEL */
