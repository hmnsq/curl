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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_printf.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN_ਪਹਿਲਾ_ALLOC 32

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYNINIT 0xbee51da /* random pattern */
#ਜੇ_ਅੰਤ

/*
 * Init a dynbuf struct.
 */
ਖਾਲੀ curlx_dyn_init(ਬਣਤਰ dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_ਮਾਪ); /* catch crazy mistakes */
  s->bufr = ਨਿਰਾ;
  s->leng = 0;
  s->allc = 0;
  s->toobig = toobig;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  s->init = DYNINIT;
#ਜੇ_ਅੰਤ
}

/*
 * free the buffer and re-init the necessary fields. It does not touch the
 * 'init' field and thus this buffer can be reused to add data to again.
 */
ਖਾਲੀ curlx_dyn_free(ਬਣਤਰ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ਸਥਿਰ CURLcode dyn_nappend(ਬਣਤਰ dynbuf *s,
                            ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *mem, size_t len)
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

  ਜੇ(fit > s->toobig) {
    curlx_dyn_free(s);
    ਵਾਪਸ CURLE_TOO_LARGE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    ਜੇ(MIN_ਪਹਿਲਾ_ALLOC > s->toobig)
      a = s->toobig;
    ਨਹੀਂਤਾਂ ਜੇ(fit < MIN_ਪਹਿਲਾ_ALLOC)
      a = MIN_ਪਹਿਲਾ_ALLOC;
    ਨਹੀਂਤਾਂ
      a = fit;
  }
  ਨਹੀਂਤਾਂ {
    ਜਦੋਂ(a < fit)
      a *= 2;
    ਜੇ(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  ਜੇ(a != s->allc) {
    ਖਾਲੀ *p = curlx_realloc(s->bufr, a);
    ਜੇ(!p) {
      curlx_dyn_free(s);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    s->bufr = p;
    s->allc = a;
  }

  ਜੇ(len)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ਵਾਪਸ CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
ਖਾਲੀ curlx_dyn_reset(ਬਣਤਰ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਜੇ(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_ਪੂਛ(ਬਣਤਰ dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਜੇ(trail > s->leng)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਨਹੀਂਤਾਂ ਜੇ(trail == s->leng)
    ਵਾਪਸ CURLE_OK;
  ਨਹੀਂਤਾਂ ਜੇ(!trail) {
    curlx_dyn_reset(s);
  }
  ਨਹੀਂਤਾਂ {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ਬਣਤਰ dynbuf *s, ਅਚਲ ਖਾਲੀ *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਵਾਪਸ dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_ਜੋੜੋ(ਬਣਤਰ dynbuf *s, ਅਚਲ ਅੱਖਰ *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = ਲੜੀ_ਲੰਬਾਈ(str);
  ਵਾਪਸ dyn_nappend(s, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ਬਣਤਰ dynbuf *s, ਅਚਲ ਅੱਖਰ *fmt, va_list ap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BUILDING_LIBCURL
  ਪੂਰਨ rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  ਜੇ(!rc)
    ਵਾਪਸ CURLE_OK;
  ਨਹੀਂਤਾਂ ਜੇ(rc == MERR_TOO_LARGE)
    ਵਾਪਸ CURLE_TOO_LARGE;
  ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
#ਨਹੀਂਤਾਂ
  ਅੱਖਰ *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  ਜੇ(str) {
    CURLcode ਨਤੀਜਾ = dyn_nappend(s, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)str, ਲੜੀ_ਲੰਬਾਈ(str));
    curl_free(str);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
#ਜੇ_ਅੰਤ
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ਬਣਤਰ dynbuf *s, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  CURLcode ਨਤੀਜਾ;
  va_list ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(strcmp(fmt, "%s")); /* use curlx_dyn_add instead */
  va_start(ap, fmt);
  ਨਤੀਜਾ = curlx_dyn_vaddf(s, fmt, ap);
  va_end(ap);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Returns a pointer to the buffer.
 */
ਅੱਖਰ *curlx_dyn_ptr(ਅਚਲ ਬਣਤਰ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਵਾਪਸ s->bufr;
}

ਅੱਖਰ *curlx_dyn_take(ਬਣਤਰ dynbuf *s, size_t *plen)
{
  ਅੱਖਰ *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ਨਿਰਾ;
  s->leng = 0;
  s->allc = 0;
  ਵਾਪਸ ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *curlx_dyn_uptr(ਅਚਲ ਬਣਤਰ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ਅਚਲ ਬਣਤਰ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਵਾਪਸ s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ਬਣਤਰ dynbuf *s, size_t ਸੈੱਟ)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ਜੇ(ਸੈੱਟ > s->leng)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  s->leng = ਸੈੱਟ;
  s->bufr[s->leng] = 0;
  ਵਾਪਸ CURLE_OK;
}
