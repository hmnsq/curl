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

#अन्तर्भाव "curlx/warnless.h"

#यदि defined(__INTEL_COMPILER) && defined(__unix__)

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः

#यदि_अन्तः /* __INTEL_COMPILER && __unix__ */

#परिभाषय CURL_आवरणम्_UCHAR   ((अचिह्नित वर्ण)~0)

#परिभाषय CURL_आवरणम्_USHORT  ((अचिह्नित ह्रस्व)~0)

#परिभाषय CURL_आवरणम्_UINT    ((अचिह्नित पूर्णाङ्क)~0)
#परिभाषय CURL_आवरणम्_SINT    (CURL_आवरणम्_UINT >> 1)

#परिभाषय CURL_आवरणम्_ULONG   ((अचिह्नित दीर्घ)~0)

#परिभाषय CURL_आवरणम्_USIZE_T ((size_t)~0)
#परिभाषय CURL_आवरणम्_SSIZE_T (CURL_आवरणम्_USIZE_T >> 1)

#यदि_परिभाषितम् __INTEL_COMPILER
#निर्देशः warning(प्रक्षिप)
#निर्देशः warning(disable:810) /* conversion may lose significant bits */
#यदि_अन्तः

/*
 * unsigned long to unsigned char
 */
अचिह्नित वर्ण curlx_ultouc(अचिह्नित दीर्घ ulnum)
{
  DEBUGASSERT(ulnum <= (अचिह्नित दीर्घ)CURL_आवरणम्_UCHAR);
  प्रत्यागम (अचिह्नित वर्ण)(ulnum & (अचिह्नित दीर्घ)CURL_आवरणम्_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
पूर्णाङ्क curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_आवरणम्_SINT);
  प्रत्यागम (पूर्णाङ्क)(uznum & (size_t)CURL_आवरणम्_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
अचिह्नित दीर्घ curlx_uztoul(size_t uznum)
{
#यदि ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_आवरणम्_ULONG);
#यदि_अन्तः
  प्रत्यागम (अचिह्नित दीर्घ)(uznum & (size_t)CURL_आवरणम्_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
अचिह्नित पूर्णाङ्क curlx_uztoui(size_t uznum)
{
#यदि UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_आवरणम्_UINT);
#यदि_अन्तः
  प्रत्यागम (अचिह्नित पूर्णाङ्क)(uznum & (size_t)CURL_आवरणम्_UINT);
}

/*
 * signed long to signed int
 */
पूर्णाङ्क curlx_sltosi(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
#यदि अधिकतम_पूर्णाङ्कः < LONG_MAX
  DEBUGASSERT((अचिह्नित दीर्घ)slnum <= (अचिह्नित दीर्घ)CURL_आवरणम्_SINT);
#यदि_अन्तः
  प्रत्यागम (पूर्णाङ्क)(slnum & (दीर्घ)CURL_आवरणम्_SINT);
}

/*
 * signed long to unsigned int
 */
अचिह्नित पूर्णाङ्क curlx_sltoui(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
#यदि UINT_MAX < LONG_MAX
  DEBUGASSERT((अचिह्नित दीर्घ)slnum <= (अचिह्नित दीर्घ)CURL_आवरणम्_UINT);
#यदि_अन्तः
  प्रत्यागम (अचिह्नित पूर्णाङ्क)(slnum & (दीर्घ)CURL_आवरणम्_UINT);
}

/*
 * signed long to unsigned short
 */
अचिह्नित ह्रस्व curlx_sltous(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((अचिह्नित दीर्घ)slnum <= (अचिह्नित दीर्घ)CURL_आवरणम्_USHORT);
  प्रत्यागम (अचिह्नित ह्रस्व)(slnum & (दीर्घ)CURL_आवरणम्_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_आवरणम्_SSIZE_T);
  प्रत्यागम (ssize_t)(uznum & (size_t)CURL_आवरणम्_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  प्रत्यागम (size_t)(sonum & (curl_off_t)CURL_आवरणम्_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
पूर्णाङ्क curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#यदि अधिकतम_पूर्णाङ्कः < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_आवरणम्_SINT);
#यदि_अन्तः
  प्रत्यागम (पूर्णाङ्क)(sznum & (ssize_t)CURL_आवरणम्_SINT);
}

/*
 * unsigned int to unsigned short
 */
अचिह्नित ह्रस्व curlx_uitous(अचिह्नित पूर्णाङ्क uinum)
{
  DEBUGASSERT(uinum <= (अचिह्नित पूर्णाङ्क)CURL_आवरणम्_USHORT);
  प्रत्यागम (अचिह्नित ह्रस्व)(uinum & (अचिह्नित पूर्णाङ्क)CURL_आवरणम्_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(पूर्णाङ्क sinum)
{
  DEBUGASSERT(sinum >= 0);
  प्रत्यागम (size_t)sinum;
}

#यदि_परिभाषितम् __INTEL_COMPILER
#निर्देशः warning(निष्कासय)
#यदि_अन्तः

size_t curlx_uitouz(अचिह्नित पूर्णाङ्क uinum)
{
  प्रत्यागम (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  यदि(sonum < 0)
    प्रत्यागम uzmin;
#यदि SIZEOF_CURL_OFF_T > SIZEOF_आकार_T
  यदि(sonum > SIZE_MAX)
    प्रत्यागम uzmax;
#यदि_अन्तः
  प्रत्यागम CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  यदि(sznum < 0) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
  *puznum = (size_t)sznum;
  प्रत्यागम TRUE;
}

bool curlx_sotouz_माति(curl_off_t sonum, size_t *puznum)
{
  यदि(sonum < 0) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
#यदि SIZEOF_CURL_OFF_T > SIZEOF_आकार_T
  यदि(sonum > SIZE_MAX) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
#यदि_अन्तः
  *puznum = (size_t)sonum;
  प्रत्यागम TRUE;
}

bool curlx_sltouz(दीर्घ slnum, size_t *puznum)
{
  यदि(slnum < 0) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  प्रत्यागम TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#यदि SIZEOF_आकार_T >= SIZEOF_CURL_OFF_T
  यदि(uznum > (size_t)CURL_OFF_T_MAX)
    प्रत्यागम CURL_OFF_T_MAX;
#यदि_अन्तः
  प्रत्यागम (curl_off_t)uznum;
}
