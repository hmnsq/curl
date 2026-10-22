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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "curlx/warnless.h"

#যদি defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned অক্ষর)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned পূর্ণসংখ্যা)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((unsigned long)~0)

#define CURL_MASK_USIZE_T ((size_t)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(ঠেলো)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
unsigned অক্ষর curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  ফেরত (unsigned অক্ষর)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
পূর্ণসংখ্যা curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  ফেরত (পূর্ণসংখ্যা)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#যদি ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  ফেরত (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned পূর্ণসংখ্যা curlx_uztoui(size_t uznum)
{
#যদি UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  ফেরত (unsigned পূর্ণসংখ্যা)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
পূর্ণসংখ্যা curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#যদি INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  ফেরত (পূর্ণসংখ্যা)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned পূর্ণসংখ্যা curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#যদি UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  ফেরত (unsigned পূর্ণসংখ্যা)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  ফেরত (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  ফেরত (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  ফেরত (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
পূর্ণসংখ্যা curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#যদি INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  ফেরত (পূর্ণসংখ্যা)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned পূর্ণসংখ্যা uinum)
{
  DEBUGASSERT(uinum <= (unsigned পূর্ণসংখ্যা)CURL_MASK_USHORT);
  ফেরত (unsigned short)(uinum & (unsigned পূর্ণসংখ্যা)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(পূর্ণসংখ্যা sinum)
{
  DEBUGASSERT(sinum >= 0);
  ফেরত (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(বের_করো)
#endif

size_t curlx_uitouz(unsigned পূর্ণসংখ্যা uinum)
{
  ফেরত (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  যদি(sonum < 0)
    ফেরত uzmin;
#যদি SIZEOF_CURL_OFF_T > SIZEOF_আকার_T
  যদি(sonum > SIZE_MAX)
    ফেরত uzmax;
#endif
  ফেরত CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  যদি(sznum < 0) {
    *puznum = 0;
    ফেরত FALSE;
  }
  *puznum = (size_t)sznum;
  ফেরত TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  যদি(sonum < 0) {
    *puznum = 0;
    ফেরত FALSE;
  }
#যদি SIZEOF_CURL_OFF_T > SIZEOF_আকার_T
  যদি(sonum > SIZE_MAX) {
    *puznum = 0;
    ফেরত FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  ফেরত TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  যদি(slnum < 0) {
    *puznum = 0;
    ফেরত FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  ফেরত TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#যদি SIZEOF_আকার_T >= SIZEOF_CURL_OFF_T
  যদি(uznum > (size_t)CURL_OFF_T_MAX)
    ফেরত CURL_OFF_T_MAX;
#endif
  ফেরত (curl_off_t)uznum;
}
