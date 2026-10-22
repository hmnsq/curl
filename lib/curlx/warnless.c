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
#include "curl_setup.h"

#include "curlx/warnless.h"

#အကယ်၍ defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned var_na)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned ကိန်းပြည့်)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((unsigned long)~0)

#define CURL_MASK_USIZE_T ((size_t)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
unsigned var_na curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  ပြန်ပေး (unsigned var_na)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
ကိန်းပြည့် curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  ပြန်ပေး (ကိန်းပြည့်)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#အကယ်၍ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  ပြန်ပေး (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned ကိန်းပြည့် curlx_uztoui(size_t uznum)
{
#အကယ်၍ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  ပြန်ပေး (unsigned ကိန်းပြည့်)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
ကိန်းပြည့် curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#အကယ်၍ INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  ပြန်ပေး (ကိန်းပြည့်)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned ကိန်းပြည့် curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#အကယ်၍ UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  ပြန်ပေး (unsigned ကိန်းပြည့်)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  ပြန်ပေး (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  ပြန်ပေး (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  ပြန်ပေး (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
ကိန်းပြည့် curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#အကယ်၍ INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  ပြန်ပေး (ကိန်းပြည့်)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned ကိန်းပြည့် uinum)
{
  DEBUGASSERT(uinum <= (unsigned ကိန်းပြည့်)CURL_MASK_USHORT);
  ပြန်ပေး (unsigned short)(uinum & (unsigned ကိန်းပြည့်)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(ကိန်းပြည့် sinum)
{
  DEBUGASSERT(sinum >= 0);
  ပြန်ပေး (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

size_t curlx_uitouz(unsigned ကိန်းပြည့် uinum)
{
  ပြန်ပေး (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  အကယ်၍(sonum < 0)
    ပြန်ပေး uzmin;
#အကယ်၍ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  အကယ်၍(sonum > SIZE_MAX)
    ပြန်ပေး uzmax;
#endif
  ပြန်ပေး CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  အကယ်၍(sznum < 0) {
    *puznum = 0;
    ပြန်ပေး FALSE;
  }
  *puznum = (size_t)sznum;
  ပြန်ပေး TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  အကယ်၍(sonum < 0) {
    *puznum = 0;
    ပြန်ပေး FALSE;
  }
#အကယ်၍ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  အကယ်၍(sonum > SIZE_MAX) {
    *puznum = 0;
    ပြန်ပေး FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  ပြန်ပေး TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  အကယ်၍(slnum < 0) {
    *puznum = 0;
    ပြန်ပေး FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  ပြန်ပေး TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#အကယ်၍ SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  အကယ်၍(uznum > (size_t)CURL_OFF_T_MAX)
    ပြန်ပေး CURL_OFF_T_MAX;
#endif
  ပြန်ပေး (curl_off_t)uznum;
}
