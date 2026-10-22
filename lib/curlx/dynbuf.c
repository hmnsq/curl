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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
শূন্য curlx_dyn_init(সংরচনা dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = রিক্ত;
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
শূন্য curlx_dyn_free(সংরচনা dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
স্থির CURLcode dyn_nappend(সংরচনা dynbuf *s,
                            ধ্রুৱ unsigned ৱর্ণ *mem, size_t len)
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
    ঘূৰাই CURLE_TOO_LARGE;
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
    যেতিয়ালৈ(a < fit)
      a *= 2;
    যদি(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  যদি(a != s->allc) {
    শূন্য *p = curlx_realloc(s->bufr, a);
    যদি(!p) {
      curlx_dyn_free(s);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  যদি(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ঘূৰাই CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
শূন্য curlx_dyn_reset(সংরচনা dynbuf *s)
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
CURLcode curlx_dyn_tail(সংরচনা dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  যদি(trail > s->leng)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  অন্যথা যদি(trail == s->leng)
    ঘূৰাই CURLE_OK;
  অন্যথা যদি(!trail) {
    curlx_dyn_reset(s);
  }
  অন্যথা {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ঘূৰাই CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(সংরচনা dynbuf *s, ধ্রুৱ শূন্য *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ঘূৰাই dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(সংরচনা dynbuf *s, ধ্রুৱ ৱর্ণ *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  ঘূৰাই dyn_nappend(s, (ধ্রুৱ unsigned ৱর্ণ *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(সংরচনা dynbuf *s, ধ্রুৱ ৱর্ণ *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  পূর্ণাঙ্ক rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  যদি(!rc)
    ঘূৰাই CURLE_OK;
  অন্যথা যদি(rc == MERR_TOO_LARGE)
    ঘূৰাই CURLE_TOO_LARGE;
  ঘূৰাই CURLE_OUT_OF_MEMORY;
#অন্যথা
  ৱর্ণ *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  যদি(str) {
    CURLcode result = dyn_nappend(s, (ধ্রুৱ unsigned ৱর্ণ *)str, strlen(str));
    curl_free(str);
    ঘূৰাই result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ঘূৰাই CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(সংরচনা dynbuf *s, ধ্রুৱ ৱর্ণ *fmt, ...)
{
  CURLcode result;
  va_list ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(strcmp(fmt, "%s")); /* use curlx_dyn_add instead */
  va_start(ap, fmt);
  result = curlx_dyn_vaddf(s, fmt, ap);
  va_end(ap);
  ঘূৰাই result;
}

/*
 * Returns a pointer to the buffer.
 */
ৱর্ণ *curlx_dyn_ptr(ধ্রুৱ সংরচনা dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ঘূৰাই s->bufr;
}

ৱর্ণ *curlx_dyn_take(সংরচনা dynbuf *s, size_t *plen)
{
  ৱর্ণ *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = রিক্ত;
  s->leng = 0;
  s->allc = 0;
  ঘূৰাই ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned ৱর্ণ *curlx_dyn_uptr(ধ্রুৱ সংরচনা dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ঘূৰাই (unsigned ৱর্ণ *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ধ্রুৱ সংরচনা dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ঘূৰাই s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(সংরচনা dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  যদি(set > s->leng)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  ঘূৰাই CURLE_OK;
}
