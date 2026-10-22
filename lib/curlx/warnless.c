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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/warnless.h"

#ਜੇ defined(__INTEL_COMPILER) && defined(__unix__)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* __INTEL_COMPILER && __unix__ */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_UCHAR   ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)~0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_USHORT  ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)~0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_UINT    ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)~0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_SINT    (CURL_ਨਕਾਬ_UINT >> 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_ULONG   ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)~0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_USIZE_T ((size_t)~0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਨਕਾਬ_SSIZE_T (CURL_ਨਕਾਬ_USIZE_T >> 1)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
#ਹਦਾਇਤ warning(ਧੱਕੋ)
#ਹਦਾਇਤ warning(disable:810) /* conversion may lose significant bits */
#ਜੇ_ਅੰਤ

/*
 * unsigned long to unsigned char
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ curlx_ultouc(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ulnum)
{
  DEBUGASSERT(ulnum <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)CURL_ਨਕਾਬ_UCHAR);
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ulnum & (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)CURL_ਨਕਾਬ_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
ਪੂਰਨ curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ਨਕਾਬ_SINT);
  ਵਾਪਸ (ਪੂਰਨ)(uznum & (size_t)CURL_ਨਕਾਬ_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ curlx_uztoul(size_t uznum)
{
#ਜੇ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ਨਕਾਬ_ULONG);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(uznum & (size_t)CURL_ਨਕਾਬ_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curlx_uztoui(size_t uznum)
{
#ਜੇ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ਨਕਾਬ_UINT);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(uznum & (size_t)CURL_ਨਕਾਬ_UINT);
}

/*
 * signed long to signed int
 */
ਪੂਰਨ curlx_sltosi(ਲੰਮਾ slnum)
{
  DEBUGASSERT(slnum >= 0);
#ਜੇ ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ < LONG_MAX
  DEBUGASSERT((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)slnum <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)CURL_ਨਕਾਬ_SINT);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (ਪੂਰਨ)(slnum & (ਲੰਮਾ)CURL_ਨਕਾਬ_SINT);
}

/*
 * signed long to unsigned int
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curlx_sltoui(ਲੰਮਾ slnum)
{
  DEBUGASSERT(slnum >= 0);
#ਜੇ UINT_MAX < LONG_MAX
  DEBUGASSERT((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)slnum <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)CURL_ਨਕਾਬ_UINT);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(slnum & (ਲੰਮਾ)CURL_ਨਕਾਬ_UINT);
}

/*
 * signed long to unsigned short
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ curlx_sltous(ਲੰਮਾ slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)slnum <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)CURL_ਨਕਾਬ_USHORT);
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)(slnum & (ਲੰਮਾ)CURL_ਨਕਾਬ_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ਨਕਾਬ_SSIZE_T);
  ਵਾਪਸ (ssize_t)(uznum & (size_t)CURL_ਨਕਾਬ_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  ਵਾਪਸ (size_t)(sonum & (curl_off_t)CURL_ਨਕਾਬ_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
ਪੂਰਨ curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#ਜੇ ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_ਨਕਾਬ_SINT);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (ਪੂਰਨ)(sznum & (ssize_t)CURL_ਨਕਾਬ_SINT);
}

/*
 * unsigned int to unsigned short
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ curlx_uitous(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uinum)
{
  DEBUGASSERT(uinum <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURL_ਨਕਾਬ_USHORT);
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)(uinum & (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURL_ਨਕਾਬ_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(ਪੂਰਨ sinum)
{
  DEBUGASSERT(sinum >= 0);
  ਵਾਪਸ (size_t)sinum;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
#ਹਦਾਇਤ warning(ਕੱਢੋ)
#ਜੇ_ਅੰਤ

size_t curlx_uitouz(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uinum)
{
  ਵਾਪਸ (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  ਜੇ(sonum < 0)
    ਵਾਪਸ uzmin;
#ਜੇ SIZEOF_CURL_OFF_T > SIZEOF_ਮਾਪ_T
  ਜੇ(sonum > SIZE_MAX)
    ਵਾਪਸ uzmax;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  ਜੇ(sznum < 0) {
    *puznum = 0;
    ਵਾਪਸ FALSE;
  }
  *puznum = (size_t)sznum;
  ਵਾਪਸ TRUE;
}

bool curlx_sotouz_ਸਮਾਉਂਦਾ(curl_off_t sonum, size_t *puznum)
{
  ਜੇ(sonum < 0) {
    *puznum = 0;
    ਵਾਪਸ FALSE;
  }
#ਜੇ SIZEOF_CURL_OFF_T > SIZEOF_ਮਾਪ_T
  ਜੇ(sonum > SIZE_MAX) {
    *puznum = 0;
    ਵਾਪਸ FALSE;
  }
#ਜੇ_ਅੰਤ
  *puznum = (size_t)sonum;
  ਵਾਪਸ TRUE;
}

bool curlx_sltouz(ਲੰਮਾ slnum, size_t *puznum)
{
  ਜੇ(slnum < 0) {
    *puznum = 0;
    ਵਾਪਸ FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  ਵਾਪਸ TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#ਜੇ SIZEOF_ਮਾਪ_T >= SIZEOF_CURL_OFF_T
  ਜੇ(uznum > (size_t)CURL_OFF_T_MAX)
    ਵਾਪਸ CURL_OFF_T_MAX;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (curl_off_t)uznum;
}
