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

#تعریف_کرو MIN_پہلا_ALLOC 32

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو DYNINIT 0xbee51da /* random pattern */
#اگر_ختم

/*
 * Init a dynbuf struct.
 */
خالی curlx_dyn_init(ساخت dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_سائز); /* catch crazy mistakes */
  s->bufr = تہی;
  s->leng = 0;
  s->allc = 0;
  s->toobig = toobig;
#اگر_تعریف_شدہ DEBUGBUILD
  s->init = DYNINIT;
#اگر_ختم
}

/*
 * free the buffer and re-init the necessary fields. It does not touch the
 * 'init' field and thus this buffer can be reused to add data to again.
 */
خالی curlx_dyn_free(ساخت dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ساکن CURLcode dyn_nappend(ساخت dynbuf *s,
                            ثابت غیرعلامتی حرف *mem, size_t len)
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
    واپس CURLE_TOO_LARGE;
  }
  ورنہ اگر(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    اگر(MIN_پہلا_ALLOC > s->toobig)
      a = s->toobig;
    ورنہ اگر(fit < MIN_پہلا_ALLOC)
      a = MIN_پہلا_ALLOC;
    ورنہ
      a = fit;
  }
  ورنہ {
    جب(a < fit)
      a *= 2;
    اگر(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  اگر(a != s->allc) {
    خالی *p = curlx_realloc(s->bufr, a);
    اگر(!p) {
      curlx_dyn_free(s);
      واپس CURLE_برآمد_کا_حافظہ;
    }
    s->bufr = p;
    s->allc = a;
  }

  اگر(len)
    حافظہ_نقل(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  واپس CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
خالی curlx_dyn_reset(ساخت dynbuf *s)
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
CURLcode curlx_dyn_دم(ساخت dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  اگر(trail > s->leng)
    واپس CURLE_BAD_FUNCTION_دلیل;
  ورنہ اگر(trail == s->leng)
    واپس CURLE_OK;
  ورنہ اگر(!trail) {
    curlx_dyn_reset(s);
  }
  ورنہ {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  واپس CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ساخت dynbuf *s, ثابت خالی *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  واپس dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_جوڑو(ساخت dynbuf *s, ثابت حرف *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = لڑی_لمبائی(str);
  واپس dyn_nappend(s, (ثابت غیرعلامتی حرف *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ساخت dynbuf *s, ثابت حرف *fmt, va_list ap)
{
#اگر_تعریف_شدہ BUILDING_LIBCURL
  صحیح rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  اگر(!rc)
    واپس CURLE_OK;
  ورنہ اگر(rc == MERR_TOO_LARGE)
    واپس CURLE_TOO_LARGE;
  واپس CURLE_برآمد_کا_حافظہ;
#ورنہ
  حرف *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  اگر(str) {
    CURLcode نتیجہ = dyn_nappend(s, (ثابت غیرعلامتی حرف *)str, لڑی_لمبائی(str));
    curl_free(str);
    واپس نتیجہ;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  واپس CURLE_برآمد_کا_حافظہ;
#اگر_ختم
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ساخت dynbuf *s, ثابت حرف *fmt, ...)
{
  CURLcode نتیجہ;
  va_list ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(strcmp(fmt, "%s")); /* use curlx_dyn_add instead */
  va_start(ap, fmt);
  نتیجہ = curlx_dyn_vaddf(s, fmt, ap);
  va_end(ap);
  واپس نتیجہ;
}

/*
 * Returns a pointer to the buffer.
 */
حرف *curlx_dyn_ptr(ثابت ساخت dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  واپس s->bufr;
}

حرف *curlx_dyn_take(ساخت dynbuf *s, size_t *plen)
{
  حرف *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = تہی;
  s->leng = 0;
  s->allc = 0;
  واپس ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
غیرعلامتی حرف *curlx_dyn_uptr(ثابت ساخت dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  واپس (غیرعلامتی حرف *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ثابت ساخت dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  واپس s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ساخت dynbuf *s, size_t سیٹ)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  اگر(سیٹ > s->leng)
    واپس CURLE_BAD_FUNCTION_دلیل;
  s->leng = سیٹ;
  s->bufr[s->leng] = 0;
  واپس CURLE_OK;
}
