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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curlx/warnless.h"

#எனில் defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned எழுத்து)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned முழு)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((unsigned long)~0)

#define CURL_MASK_USIZE_T ((size_t)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(தள்ளு)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
unsigned எழுத்து curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  திரும்பு (unsigned எழுத்து)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
முழு curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  திரும்பு (முழு)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#எனில் ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  திரும்பு (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned முழு curlx_uztoui(size_t uznum)
{
#எனில் UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  திரும்பு (unsigned முழு)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
முழு curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#எனில் INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  திரும்பு (முழு)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned முழு curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#எனில் UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  திரும்பு (unsigned முழு)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  திரும்பு (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  திரும்பு (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  திரும்பு (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
முழு curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#எனில் INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  திரும்பு (முழு)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned முழு uinum)
{
  DEBUGASSERT(uinum <= (unsigned முழு)CURL_MASK_USHORT);
  திரும்பு (unsigned short)(uinum & (unsigned முழு)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(முழு sinum)
{
  DEBUGASSERT(sinum >= 0);
  திரும்பு (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(எடு)
#endif

size_t curlx_uitouz(unsigned முழு uinum)
{
  திரும்பு (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  எனில்(sonum < 0)
    திரும்பு uzmin;
#எனில் SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  எனில்(sonum > SIZE_MAX)
    திரும்பு uzmax;
#endif
  திரும்பு CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  எனில்(sznum < 0) {
    *puznum = 0;
    திரும்பு FALSE;
  }
  *puznum = (size_t)sznum;
  திரும்பு TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  எனில்(sonum < 0) {
    *puznum = 0;
    திரும்பு FALSE;
  }
#எனில் SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  எனில்(sonum > SIZE_MAX) {
    *puznum = 0;
    திரும்பு FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  திரும்பு TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  எனில்(slnum < 0) {
    *puznum = 0;
    திரும்பு FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  திரும்பு TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#எனில் SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  எனில்(uznum > (size_t)CURL_OFF_T_MAX)
    திரும்பு CURL_OFF_T_MAX;
#endif
  திரும்பு (curl_off_t)uznum;
}
