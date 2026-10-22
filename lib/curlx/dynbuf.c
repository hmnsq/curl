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

#شامل "curlx/dynbuf.h"
#شامل "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
تهی curlx_dyn_init(ساختار dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = پوچ;
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
تهی curlx_dyn_free(ساختار dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ایستا CURLcode dyn_nappend(ساختار dynbuf *s,
                            ثابت unsigned نویسه *mem, size_t len)
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

  اگر(fit > s->toobig) {
    curlx_dyn_free(s);
    بازگشت CURLE_TOO_LARGE;
  }
  وگرنه اگر(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    اگر(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    وگرنه اگر(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    وگرنه
      a = fit;
  }
  وگرنه {
    وقتی(a < fit)
      a *= 2;
    اگر(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  اگر(a != s->allc) {
    تهی *p = curlx_realloc(s->bufr, a);
    اگر(!p) {
      curlx_dyn_free(s);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  اگر(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  بازگشت CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
تهی curlx_dyn_reset(ساختار dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  اگر(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(ساختار dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  اگر(trail > s->leng)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  وگرنه اگر(trail == s->leng)
    بازگشت CURLE_OK;
  وگرنه اگر(!trail) {
    curlx_dyn_reset(s);
  }
  وگرنه {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  بازگشت CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ساختار dynbuf *s, ثابت تهی *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  بازگشت dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(ساختار dynbuf *s, ثابت نویسه *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  بازگشت dyn_nappend(s, (ثابت unsigned نویسه *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ساختار dynbuf *s, ثابت نویسه *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  صحیح rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  اگر(!rc)
    بازگشت CURLE_OK;
  وگرنه اگر(rc == MERR_TOO_LARGE)
    بازگشت CURLE_TOO_LARGE;
  بازگشت CURLE_OUT_OF_MEMORY;
#وگرنه
  نویسه *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  اگر(str) {
    CURLcode result = dyn_nappend(s, (ثابت unsigned نویسه *)str, strlen(str));
    curl_free(str);
    بازگشت result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  بازگشت CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ساختار dynbuf *s, ثابت نویسه *fmt, ...)
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
  بازگشت result;
}

/*
 * Returns a pointer to the buffer.
 */
نویسه *curlx_dyn_ptr(ثابت ساختار dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  بازگشت s->bufr;
}

نویسه *curlx_dyn_take(ساختار dynbuf *s, size_t *plen)
{
  نویسه *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = پوچ;
  s->leng = 0;
  s->allc = 0;
  بازگشت ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned نویسه *curlx_dyn_uptr(ثابت ساختار dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  بازگشت (unsigned نویسه *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ثابت ساختار dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  بازگشت s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ساختار dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  اگر(set > s->leng)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  بازگشت CURLE_OK;
}
