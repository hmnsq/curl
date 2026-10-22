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

#అన్తర్భావ "curlx/warnless.h"

#ఒకవేళ defined(__INTEL_COMPILER) && defined(__unix__)

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#అయితే_ముగింపు /* __INTEL_COMPILER && __unix__ */

#నిర్వచించు CURL_ముసుగు_UCHAR   ((గుర్తులేని అక్షరం)~0)

#నిర్వచించు CURL_ముసుగు_USHORT  ((గుర్తులేని పొట్టి)~0)

#నిర్వచించు CURL_ముసుగు_UINT    ((గుర్తులేని పూర్ణాంకం)~0)
#నిర్వచించు CURL_ముసుగు_SINT    (CURL_ముసుగు_UINT >> 1)

#నిర్వచించు CURL_ముసుగు_ULONG   ((గుర్తులేని దీర్ఘ)~0)

#నిర్వచించు CURL_ముసుగు_USIZE_T ((size_t)~0)
#నిర్వచించు CURL_ముసుగు_SSIZE_T (CURL_ముసుగు_USIZE_T >> 1)

#నిర్వచితమైతే __INTEL_COMPILER
#సూచన warning(నెట్టు)
#సూచన warning(disable:810) /* conversion may lose significant bits */
#అయితే_ముగింపు

/*
 * unsigned long to unsigned char
 */
గుర్తులేని అక్షరం curlx_ultouc(గుర్తులేని దీర్ఘ ulnum)
{
  DEBUGASSERT(ulnum <= (గుర్తులేని దీర్ఘ)CURL_ముసుగు_UCHAR);
  తిరిగి (గుర్తులేని అక్షరం)(ulnum & (గుర్తులేని దీర్ఘ)CURL_ముసుగు_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
పూర్ణాంకం curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ముసుగు_SINT);
  తిరిగి (పూర్ణాంకం)(uznum & (size_t)CURL_ముసుగు_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
గుర్తులేని దీర్ఘ curlx_uztoul(size_t uznum)
{
#ఒకవేళ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ముసుగు_ULONG);
#అయితే_ముగింపు
  తిరిగి (గుర్తులేని దీర్ఘ)(uznum & (size_t)CURL_ముసుగు_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
గుర్తులేని పూర్ణాంకం curlx_uztoui(size_t uznum)
{
#ఒకవేళ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ముసుగు_UINT);
#అయితే_ముగింపు
  తిరిగి (గుర్తులేని పూర్ణాంకం)(uznum & (size_t)CURL_ముసుగు_UINT);
}

/*
 * signed long to signed int
 */
పూర్ణాంకం curlx_sltosi(దీర్ఘ slnum)
{
  DEBUGASSERT(slnum >= 0);
#ఒకవేళ గరిష్ఠ_పూర్ణాంకం < LONG_MAX
  DEBUGASSERT((గుర్తులేని దీర్ఘ)slnum <= (గుర్తులేని దీర్ఘ)CURL_ముసుగు_SINT);
#అయితే_ముగింపు
  తిరిగి (పూర్ణాంకం)(slnum & (దీర్ఘ)CURL_ముసుగు_SINT);
}

/*
 * signed long to unsigned int
 */
గుర్తులేని పూర్ణాంకం curlx_sltoui(దీర్ఘ slnum)
{
  DEBUGASSERT(slnum >= 0);
#ఒకవేళ UINT_MAX < LONG_MAX
  DEBUGASSERT((గుర్తులేని దీర్ఘ)slnum <= (గుర్తులేని దీర్ఘ)CURL_ముసుగు_UINT);
#అయితే_ముగింపు
  తిరిగి (గుర్తులేని పూర్ణాంకం)(slnum & (దీర్ఘ)CURL_ముసుగు_UINT);
}

/*
 * signed long to unsigned short
 */
గుర్తులేని పొట్టి curlx_sltous(దీర్ఘ slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((గుర్తులేని దీర్ఘ)slnum <= (గుర్తులేని దీర్ఘ)CURL_ముసుగు_USHORT);
  తిరిగి (గుర్తులేని పొట్టి)(slnum & (దీర్ఘ)CURL_ముసుగు_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ముసుగు_SSIZE_T);
  తిరిగి (ssize_t)(uznum & (size_t)CURL_ముసుగు_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  తిరిగి (size_t)(sonum & (curl_off_t)CURL_ముసుగు_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
పూర్ణాంకం curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#ఒకవేళ గరిష్ఠ_పూర్ణాంకం < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_ముసుగు_SINT);
#అయితే_ముగింపు
  తిరిగి (పూర్ణాంకం)(sznum & (ssize_t)CURL_ముసుగు_SINT);
}

/*
 * unsigned int to unsigned short
 */
గుర్తులేని పొట్టి curlx_uitous(గుర్తులేని పూర్ణాంకం uinum)
{
  DEBUGASSERT(uinum <= (గుర్తులేని పూర్ణాంకం)CURL_ముసుగు_USHORT);
  తిరిగి (గుర్తులేని పొట్టి)(uinum & (గుర్తులేని పూర్ణాంకం)CURL_ముసుగు_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(పూర్ణాంకం sinum)
{
  DEBUGASSERT(sinum >= 0);
  తిరిగి (size_t)sinum;
}

#నిర్వచితమైతే __INTEL_COMPILER
#సూచన warning(తీయి)
#అయితే_ముగింపు

size_t curlx_uitouz(గుర్తులేని పూర్ణాంకం uinum)
{
  తిరిగి (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  ఒకవేళ(sonum < 0)
    తిరిగి uzmin;
#ఒకవేళ SIZEOF_CURL_OFF_T > SIZEOF_కొలత_T
  ఒకవేళ(sonum > SIZE_MAX)
    తిరిగి uzmax;
#అయితే_ముగింపు
  తిరిగి CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  ఒకవేళ(sznum < 0) {
    *puznum = 0;
    తిరిగి FALSE;
  }
  *puznum = (size_t)sznum;
  తిరిగి TRUE;
}

bool curlx_sotouz_సరిపోతుంది(curl_off_t sonum, size_t *puznum)
{
  ఒకవేళ(sonum < 0) {
    *puznum = 0;
    తిరిగి FALSE;
  }
#ఒకవేళ SIZEOF_CURL_OFF_T > SIZEOF_కొలత_T
  ఒకవేళ(sonum > SIZE_MAX) {
    *puznum = 0;
    తిరిగి FALSE;
  }
#అయితే_ముగింపు
  *puznum = (size_t)sonum;
  తిరిగి TRUE;
}

bool curlx_sltouz(దీర్ఘ slnum, size_t *puznum)
{
  ఒకవేళ(slnum < 0) {
    *puznum = 0;
    తిరిగి FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  తిరిగి TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#ఒకవేళ SIZEOF_కొలత_T >= SIZEOF_CURL_OFF_T
  ఒకవేళ(uznum > (size_t)CURL_OFF_T_MAX)
    తిరిగి CURL_OFF_T_MAX;
#అయితే_ముగింపు
  తిరిగి (curl_off_t)uznum;
}
