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

#അന്തര്ഭാവ "curlx/warnless.h"

#എങ്കിൽ defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned അക്ഷരം)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned പൂർണ്ണസംഖ്യ)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((unsigned long)~0)

#define CURL_MASK_USIZE_T ((size_t)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(തള്ളുക)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
unsigned അക്ഷരം curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  തിരികെ (unsigned അക്ഷരം)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
പൂർണ്ണസംഖ്യ curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  തിരികെ (പൂർണ്ണസംഖ്യ)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#എങ്കിൽ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  തിരികെ (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned പൂർണ്ണസംഖ്യ curlx_uztoui(size_t uznum)
{
#എങ്കിൽ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  തിരികെ (unsigned പൂർണ്ണസംഖ്യ)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
പൂർണ്ണസംഖ്യ curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#എങ്കിൽ INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  തിരികെ (പൂർണ്ണസംഖ്യ)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned പൂർണ്ണസംഖ്യ curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#എങ്കിൽ UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  തിരികെ (unsigned പൂർണ്ണസംഖ്യ)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  തിരികെ (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  തിരികെ (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  തിരികെ (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
പൂർണ്ണസംഖ്യ curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#എങ്കിൽ INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  തിരികെ (പൂർണ്ണസംഖ്യ)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned പൂർണ്ണസംഖ്യ uinum)
{
  DEBUGASSERT(uinum <= (unsigned പൂർണ്ണസംഖ്യ)CURL_MASK_USHORT);
  തിരികെ (unsigned short)(uinum & (unsigned പൂർണ്ണസംഖ്യ)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(പൂർണ്ണസംഖ്യ sinum)
{
  DEBUGASSERT(sinum >= 0);
  തിരികെ (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(എടുക്കുക)
#endif

size_t curlx_uitouz(unsigned പൂർണ്ണസംഖ്യ uinum)
{
  തിരികെ (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  എങ്കിൽ(sonum < 0)
    തിരികെ uzmin;
#എങ്കിൽ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  എങ്കിൽ(sonum > SIZE_MAX)
    തിരികെ uzmax;
#endif
  തിരികെ CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  എങ്കിൽ(sznum < 0) {
    *puznum = 0;
    തിരികെ FALSE;
  }
  *puznum = (size_t)sznum;
  തിരികെ TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  എങ്കിൽ(sonum < 0) {
    *puznum = 0;
    തിരികെ FALSE;
  }
#എങ്കിൽ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  എങ്കിൽ(sonum > SIZE_MAX) {
    *puznum = 0;
    തിരികെ FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  തിരികെ TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  എങ്കിൽ(slnum < 0) {
    *puznum = 0;
    തിരികെ FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  തിരികെ TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#എങ്കിൽ SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  എങ്കിൽ(uznum > (size_t)CURL_OFF_T_MAX)
    തിരികെ CURL_OFF_T_MAX;
#endif
  തിരികെ (curl_off_t)uznum;
}
