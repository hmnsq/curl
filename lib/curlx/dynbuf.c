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

#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
শূন্য curlx_dyn_init(কাঠামো dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_আকার); /* catch crazy mistakes */
  s->bufr = ফাঁকা;
  s->leng = 0;
  s->allc = 0;
  s->toobig = toobig;
#ifdef DEBUGBUILD
  s->init = DYNINIT;
#endif
}

/*
 * free the buffer and re-init the necessary fields. It does not touch the
 * 'init' field and thus this buffer can be reused to add data to again.
 */
শূন্য curlx_dyn_free(কাঠামো dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
স্থির CURLcode dyn_nappend(কাঠামো dynbuf *s,
                            ধ্রুবক unsigned অক্ষর *mem, size_t len)
{
  size_t idx = s->leng;
  size_t a = s->allc;
  size_t fit = len + idx + 1; /* new string + old string + zero byte */

  /* try to detect if there is rubbish in the struct */
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(s->toobig);
  DEBUGASSERT(idx < s->toobig);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(a <= s->toobig);
  DEBUGASSERT(!len || mem);

  যদি(fit > s->toobig) {
    curlx_dyn_free(s);
    ফেরত CURLE_TOO_LARGE;
  }
  অন্যথা যদি(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    যদি(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    অন্যথা যদি(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    অন্যথা
      a = fit;
  }
  অন্যথা {
    যখন(a < fit)
      a *= 2;
    যদি(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  যদি(a != s->allc) {
    শূন্য *p = curlx_realloc(s->bufr, a);
    যদি(!p) {
      curlx_dyn_free(s);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  যদি(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ফেরত CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
শূন্য curlx_dyn_reset(কাঠামো dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  যদি(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_লেজ(কাঠামো dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  যদি(trail > s->leng)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  অন্যথা যদি(trail == s->leng)
    ফেরত CURLE_OK;
  অন্যথা যদি(!trail) {
    curlx_dyn_reset(s);
  }
  অন্যথা {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ফেরত CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(কাঠামো dynbuf *s, ধ্রুবক শূন্য *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ফেরত dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(কাঠামো dynbuf *s, ধ্রুবক অক্ষর *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  ফেরত dyn_nappend(s, (ধ্রুবক unsigned অক্ষর *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(কাঠামো dynbuf *s, ধ্রুবক অক্ষর *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  পূর্ণসংখ্যা rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  যদি(!rc)
    ফেরত CURLE_OK;
  অন্যথা যদি(rc == MERR_TOO_LARGE)
    ফেরত CURLE_TOO_LARGE;
  ফেরত CURLE_OUT_OF_MEMORY;
#অন্যথা
  অক্ষর *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  যদি(str) {
    CURLcode ফলাফল = dyn_nappend(s, (ধ্রুবক unsigned অক্ষর *)str, strlen(str));
    curl_free(str);
    ফেরত ফলাফল;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ফেরত CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(কাঠামো dynbuf *s, ধ্রুবক অক্ষর *fmt, ...)
{
  CURLcode ফলাফল;
  va_list ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(strcmp(fmt, "%s")); /* use curlx_dyn_add instead */
  va_start(ap, fmt);
  ফলাফল = curlx_dyn_vaddf(s, fmt, ap);
  va_end(ap);
  ফেরত ফলাফল;
}

/*
 * Returns a pointer to the buffer.
 */
অক্ষর *curlx_dyn_ptr(ধ্রুবক কাঠামো dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ফেরত s->bufr;
}

অক্ষর *curlx_dyn_take(কাঠামো dynbuf *s, size_t *plen)
{
  অক্ষর *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ফাঁকা;
  s->leng = 0;
  s->allc = 0;
  ফেরত ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned অক্ষর *curlx_dyn_uptr(ধ্রুবক কাঠামো dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ফেরত (unsigned অক্ষর *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ধ্রুবক কাঠামো dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ফেরত s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(কাঠামো dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  যদি(set > s->leng)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  ফেরত CURLE_OK;
}
