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

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((अचिन्हित वर्ण)~0)

#define CURL_MASK_USHORT  ((अचिन्हित लघु)~0)

#define CURL_MASK_UINT    ((अचिन्हित पूर्णाङ्क)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((अचिन्हित दीर्घ)~0)

#define CURL_MASK_USIZE_T ((माप_प्रकार)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(धकेलो)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
अचिन्हित वर्ण curlx_ultouc(अचिन्हित दीर्घ ulnum)
{
  DEBUGASSERT(ulnum <= (अचिन्हित दीर्घ)CURL_MASK_UCHAR);
  प्रत्यागम (अचिन्हित वर्ण)(ulnum & (अचिन्हित दीर्घ)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
पूर्णाङ्क curlx_uztosi(माप_प्रकार uznum)
{
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_SINT);
  प्रत्यागम (पूर्णाङ्क)(uznum & (माप_प्रकार)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
अचिन्हित दीर्घ curlx_uztoul(माप_प्रकार uznum)
{
#यदि अच_दीर्घ_उच्च < SIZE_MAX
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_ULONG);
#endif
  प्रत्यागम (अचिन्हित दीर्घ)(uznum & (माप_प्रकार)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
अचिन्हित पूर्णाङ्क curlx_uztoui(माप_प्रकार uznum)
{
#यदि अच_पूर्णांक_उच्च < SIZE_MAX
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_UINT);
#endif
  प्रत्यागम (अचिन्हित पूर्णाङ्क)(uznum & (माप_प्रकार)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
पूर्णाङ्क curlx_sltosi(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
#यदि पूर्णांक_उच्च < दीर्घ_उच्च
  DEBUGASSERT((अचिन्हित दीर्घ)slnum <= (अचिन्हित दीर्घ)CURL_MASK_SINT);
#endif
  प्रत्यागम (पूर्णाङ्क)(slnum & (दीर्घ)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
अचिन्हित पूर्णाङ्क curlx_sltoui(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
#यदि अच_पूर्णांक_उच्च < दीर्घ_उच्च
  DEBUGASSERT((अचिन्हित दीर्घ)slnum <= (अचिन्हित दीर्घ)CURL_MASK_UINT);
#endif
  प्रत्यागम (अचिन्हित पूर्णाङ्क)(slnum & (दीर्घ)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
अचिन्हित लघु curlx_sltous(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((अचिन्हित दीर्घ)slnum <= (अचिन्हित दीर्घ)CURL_MASK_USHORT);
  प्रत्यागम (अचिन्हित लघु)(slnum & (दीर्घ)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(माप_प्रकार uznum)
{
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_SSIZE_T);
  प्रत्यागम (ssize_t)(uznum & (माप_प्रकार)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
माप_प्रकार curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  प्रत्यागम (माप_प्रकार)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
पूर्णाङ्क curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#यदि पूर्णांक_उच्च < SSIZE_MAX
  DEBUGASSERT((माप_प्रकार)sznum <= (माप_प्रकार)CURL_MASK_SINT);
#endif
  प्रत्यागम (पूर्णाङ्क)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
अचिन्हित लघु curlx_uitous(अचिन्हित पूर्णाङ्क uinum)
{
  DEBUGASSERT(uinum <= (अचिन्हित पूर्णाङ्क)CURL_MASK_USHORT);
  प्रत्यागम (अचिन्हित लघु)(uinum & (अचिन्हित पूर्णाङ्क)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
माप_प्रकार curlx_sitouz(पूर्णाङ्क sinum)
{
  DEBUGASSERT(sinum >= 0);
  प्रत्यागम (माप_प्रकार)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(निकालो)
#endif

माप_प्रकार curlx_uitouz(अचिन्हित पूर्णाङ्क uinum)
{
  प्रत्यागम (माप_प्रकार)uinum;
}

माप_प्रकार curlx_sotouz_range(curl_off_t sonum, माप_प्रकार uzmin, माप_प्रकार uzmax)
{
  यदि(sonum < 0)
    प्रत्यागम uzmin;
#यदि SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  यदि(sonum > SIZE_MAX)
    प्रत्यागम uzmax;
#endif
  प्रत्यागम CURLMIN(CURLMAX((माप_प्रकार)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, माप_प्रकार *puznum)
{
  यदि(sznum < 0) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
  *puznum = (माप_प्रकार)sznum;
  प्रत्यागम TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, माप_प्रकार *puznum)
{
  यदि(sonum < 0) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
#यदि SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  यदि(sonum > SIZE_MAX) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
#endif
  *puznum = (माप_प्रकार)sonum;
  प्रत्यागम TRUE;
}

bool curlx_sltouz(दीर्घ slnum, माप_प्रकार *puznum)
{
  यदि(slnum < 0) {
    *puznum = 0;
    प्रत्यागम FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (माप_प्रकार)slnum;
  प्रत्यागम TRUE;
}

curl_off_t curlx_uztoso(माप_प्रकार uznum)
{
#यदि SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  यदि(uznum > (माप_प्रकार)CURL_OFF_T_MAX)
    प्रत्यागम CURL_OFF_T_MAX;
#endif
  प्रत्यागम (curl_off_t)uznum;
}
