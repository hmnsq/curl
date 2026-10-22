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
#شامل "curl_setup.h"

#شامل "curlx/warnless.h"

#اگر defined(__INTEL_COMPILER) && defined(__unix__)

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#اگر_ختم /* __INTEL_COMPILER && __unix__ */

#تعریف_کرو CURL_نقاب_UCHAR   ((غیرعلامتی حرف)~0)

#تعریف_کرو CURL_نقاب_USHORT  ((غیرعلامتی مختصر)~0)

#تعریف_کرو CURL_نقاب_UINT    ((غیرعلامتی صحیح)~0)
#تعریف_کرو CURL_نقاب_SINT    (CURL_نقاب_UINT >> 1)

#تعریف_کرو CURL_نقاب_ULONG   ((غیرعلامتی طویل)~0)

#تعریف_کرو CURL_نقاب_USIZE_T ((size_t)~0)
#تعریف_کرو CURL_نقاب_SSIZE_T (CURL_نقاب_USIZE_T >> 1)

#اگر_تعریف_شدہ __INTEL_COMPILER
#ہدایت warning(ڈالو)
#ہدایت warning(disable:810) /* conversion may lose significant bits */
#اگر_ختم

/*
 * unsigned long to unsigned char
 */
غیرعلامتی حرف curlx_ultouc(غیرعلامتی طویل ulnum)
{
  DEBUGASSERT(ulnum <= (غیرعلامتی طویل)CURL_نقاب_UCHAR);
  واپس (غیرعلامتی حرف)(ulnum & (غیرعلامتی طویل)CURL_نقاب_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
صحیح curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_SINT);
  واپس (صحیح)(uznum & (size_t)CURL_نقاب_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
غیرعلامتی طویل curlx_uztoul(size_t uznum)
{
#اگر ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_ULONG);
#اگر_ختم
  واپس (غیرعلامتی طویل)(uznum & (size_t)CURL_نقاب_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
غیرعلامتی صحیح curlx_uztoui(size_t uznum)
{
#اگر UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_UINT);
#اگر_ختم
  واپس (غیرعلامتی صحیح)(uznum & (size_t)CURL_نقاب_UINT);
}

/*
 * signed long to signed int
 */
صحیح curlx_sltosi(طویل slnum)
{
  DEBUGASSERT(slnum >= 0);
#اگر زیادہ_سے_زیادہ_عدد < LONG_MAX
  DEBUGASSERT((غیرعلامتی طویل)slnum <= (غیرعلامتی طویل)CURL_نقاب_SINT);
#اگر_ختم
  واپس (صحیح)(slnum & (طویل)CURL_نقاب_SINT);
}

/*
 * signed long to unsigned int
 */
غیرعلامتی صحیح curlx_sltoui(طویل slnum)
{
  DEBUGASSERT(slnum >= 0);
#اگر UINT_MAX < LONG_MAX
  DEBUGASSERT((غیرعلامتی طویل)slnum <= (غیرعلامتی طویل)CURL_نقاب_UINT);
#اگر_ختم
  واپس (غیرعلامتی صحیح)(slnum & (طویل)CURL_نقاب_UINT);
}

/*
 * signed long to unsigned short
 */
غیرعلامتی مختصر curlx_sltous(طویل slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((غیرعلامتی طویل)slnum <= (غیرعلامتی طویل)CURL_نقاب_USHORT);
  واپس (غیرعلامتی مختصر)(slnum & (طویل)CURL_نقاب_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_SSIZE_T);
  واپس (ssize_t)(uznum & (size_t)CURL_نقاب_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  واپس (size_t)(sonum & (curl_off_t)CURL_نقاب_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
صحیح curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#اگر زیادہ_سے_زیادہ_عدد < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_نقاب_SINT);
#اگر_ختم
  واپس (صحیح)(sznum & (ssize_t)CURL_نقاب_SINT);
}

/*
 * unsigned int to unsigned short
 */
غیرعلامتی مختصر curlx_uitous(غیرعلامتی صحیح uinum)
{
  DEBUGASSERT(uinum <= (غیرعلامتی صحیح)CURL_نقاب_USHORT);
  واپس (غیرعلامتی مختصر)(uinum & (غیرعلامتی صحیح)CURL_نقاب_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(صحیح sinum)
{
  DEBUGASSERT(sinum >= 0);
  واپس (size_t)sinum;
}

#اگر_تعریف_شدہ __INTEL_COMPILER
#ہدایت warning(نکالو)
#اگر_ختم

size_t curlx_uitouz(غیرعلامتی صحیح uinum)
{
  واپس (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  اگر(sonum < 0)
    واپس uzmin;
#اگر SIZEOF_CURL_OFF_T > SIZEOF_سائز_T
  اگر(sonum > SIZE_MAX)
    واپس uzmax;
#اگر_ختم
  واپس CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  اگر(sznum < 0) {
    *puznum = 0;
    واپس FALSE;
  }
  *puznum = (size_t)sznum;
  واپس TRUE;
}

bool curlx_sotouz_سماتا(curl_off_t sonum, size_t *puznum)
{
  اگر(sonum < 0) {
    *puznum = 0;
    واپس FALSE;
  }
#اگر SIZEOF_CURL_OFF_T > SIZEOF_سائز_T
  اگر(sonum > SIZE_MAX) {
    *puznum = 0;
    واپس FALSE;
  }
#اگر_ختم
  *puznum = (size_t)sonum;
  واپس TRUE;
}

bool curlx_sltouz(طویل slnum, size_t *puznum)
{
  اگر(slnum < 0) {
    *puznum = 0;
    واپس FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  واپس TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#اگر SIZEOF_سائز_T >= SIZEOF_CURL_OFF_T
  اگر(uznum > (size_t)CURL_OFF_T_MAX)
    واپس CURL_OFF_T_MAX;
#اگر_ختم
  واپس (curl_off_t)uznum;
}
