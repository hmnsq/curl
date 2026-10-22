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

#यदि defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#यदि_अन्तः

#अन्तर्भाव "curl_memrchr.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
स्थिर bool pmatch(ध्रुव वर्ण *hostname, size_t hostlen,
                   ध्रुव वर्ण *आदर्शः, size_t patternlen)
{
  यदि(hostlen != patternlen)
    प्रत्यागम FALSE;
  प्रत्यागम curl_strnequal(hostname, आदर्शः, hostlen);
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

स्थिर bool hostmatch(ध्रुव वर्ण *hostname,
                      size_t hostlen,
                      ध्रुव वर्ण *आदर्शः,
                      size_t patternlen)
{
  ध्रुव वर्ण *आदर्शः_label_अन्तः;

  DEBUGASSERT(आदर्शः);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  यदि(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  यदि(patternlen > 0 && आदर्शः[patternlen - 1] == '.')
    patternlen--;

  यदि(patternlen > 2 && वाक्ये_तुलय(आदर्शः, "*.", 2))
    प्रत्यागम pmatch(hostname, hostlen, आदर्शः, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  अन्यथा यदि(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    प्रत्यागम FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  आदर्शः_label_अन्तः = memchr(आदर्शः, '.', patternlen);
  यदि(!आदर्शः_label_अन्तः ||
     (memrchr(आदर्शः, '.', patternlen) == आदर्शः_label_अन्तः))
    प्रत्यागम pmatch(hostname, hostlen, आदर्शः, patternlen);
  अन्यथा {
    ध्रुव वर्ण *hostname_label_अन्तः = memchr(hostname, '.', hostlen);
    यदि(hostname_label_अन्तः) {
      size_t skiphost = hostname_label_अन्तः - hostname;
      size_t skiplen = आदर्शः_label_अन्तः - आदर्शः;
      प्रत्यागम pmatch(hostname_label_अन्तः, hostlen - skiphost,
                    आदर्शः_label_अन्तः, patternlen - skiplen);
    }
  }
  प्रत्यागम FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(ध्रुव वर्ण *मेलनम्, size_t matchlen,
                         ध्रुव वर्ण *hostname, size_t hostlen)
{
  यदि(matchlen > 0 && hostlen > 0)
    प्रत्यागम hostmatch(hostname, hostlen, मेलनम्, matchlen);
  प्रत्यागम FALSE;
}

#यदि_अन्तः /* USE_OPENSSL || USE_SCHANNEL */
