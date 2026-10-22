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
#ضمن "curl_setup.h"

#ضمن "curlx/dynbuf.h"
#ضمن "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
فراغ curlx_dyn_init(بنية dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = عدم;
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
فراغ curlx_dyn_free(بنية dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ساكن CURLcode dyn_nappend(بنية dynbuf *s,
                            ثابت unsigned حرف *mem, size_t len)
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

  إذا(fit > s->toobig) {
    curlx_dyn_free(s);
    إرجاع CURLE_TOO_LARGE;
  }
  وإلا إذا(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    إذا(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    وإلا إذا(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    وإلا
      a = fit;
  }
  وإلا {
    بينما(a < fit)
      a *= 2;
    إذا(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  إذا(a != s->allc) {
    فراغ *p = curlx_realloc(s->bufr, a);
    إذا(!p) {
      curlx_dyn_free(s);
      إرجاع CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  إذا(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  إرجاع CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
فراغ curlx_dyn_reset(بنية dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إذا(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(بنية dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إذا(trail > s->leng)
    إرجاع CURLE_BAD_FUNCTION_ARGUMENT;
  وإلا إذا(trail == s->leng)
    إرجاع CURLE_OK;
  وإلا إذا(!trail) {
    curlx_dyn_reset(s);
  }
  وإلا {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  إرجاع CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(بنية dynbuf *s, ثابت فراغ *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إرجاع dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(بنية dynbuf *s, ثابت حرف *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  إرجاع dyn_nappend(s, (ثابت unsigned حرف *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(بنية dynbuf *s, ثابت حرف *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  صحيح rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  إذا(!rc)
    إرجاع CURLE_OK;
  وإلا إذا(rc == MERR_TOO_LARGE)
    إرجاع CURLE_TOO_LARGE;
  إرجاع CURLE_OUT_OF_MEMORY;
#وإلا
  حرف *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  إذا(str) {
    CURLcode result = dyn_nappend(s, (ثابت unsigned حرف *)str, strlen(str));
    curl_free(str);
    إرجاع result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  إرجاع CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(بنية dynbuf *s, ثابت حرف *fmt, ...)
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
  إرجاع result;
}

/*
 * Returns a pointer to the buffer.
 */
حرف *curlx_dyn_ptr(ثابت بنية dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إرجاع s->bufr;
}

حرف *curlx_dyn_take(بنية dynbuf *s, size_t *plen)
{
  حرف *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = عدم;
  s->leng = 0;
  s->allc = 0;
  إرجاع ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned حرف *curlx_dyn_uptr(ثابت بنية dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إرجاع (unsigned حرف *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ثابت بنية dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إرجاع s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(بنية dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  إذا(set > s->leng)
    إرجاع CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  إرجاع CURLE_OK;
}
