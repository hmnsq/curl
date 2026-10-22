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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
ಶೂನ್ಯ curlx_dyn_init(ಸಂರಚನಾ dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = ರಿಕ್ತ;
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
ಶೂನ್ಯ curlx_dyn_free(ಸಂರಚನಾ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ಸ್ಥಿರ CURLcode dyn_nappend(ಸಂರಚನಾ dynbuf *s,
                            ಧ್ರುವ unsigned ವರ್ಣ *mem, size_t len)
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

  ಯದಿ(fit > s->toobig) {
    curlx_dyn_free(s);
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  }
  ಅನ್ಯಥಾ ಯದಿ(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    ಯದಿ(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    ಅನ್ಯಥಾ ಯದಿ(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    ಅನ್ಯಥಾ
      a = fit;
  }
  ಅನ್ಯಥಾ {
    ಯಾವತ್(a < fit)
      a *= 2;
    ಯದಿ(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  ಯದಿ(a != s->allc) {
    ಶೂನ್ಯ *p = curlx_realloc(s->bufr, a);
    ಯದಿ(!p) {
      curlx_dyn_free(s);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  ಯದಿ(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
ಶೂನ್ಯ curlx_dyn_reset(ಸಂರಚನಾ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಯದಿ(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(ಸಂರಚನಾ dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಯದಿ(trail > s->leng)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಅನ್ಯಥಾ ಯದಿ(trail == s->leng)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಅನ್ಯಥಾ ಯದಿ(!trail) {
    curlx_dyn_reset(s);
  }
  ಅನ್ಯಥಾ {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ಸಂರಚನಾ dynbuf *s, ಧ್ರುವ ಶೂನ್ಯ *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಪ್ರತ್ಯಾಗಮ dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(ಸಂರಚನಾ dynbuf *s, ಧ್ರುವ ವರ್ಣ *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  ಪ್ರತ್ಯಾಗಮ dyn_nappend(s, (ಧ್ರುವ unsigned ವರ್ಣ *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ಸಂರಚನಾ dynbuf *s, ಧ್ರುವ ವರ್ಣ *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  ಪೂರ್ಣಾಙ್ಕ rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  ಯದಿ(!rc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಅನ್ಯಥಾ ಯದಿ(rc == MERR_TOO_LARGE)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
#ಅನ್ಯಥಾ
  ವರ್ಣ *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  ಯದಿ(str) {
    CURLcode result = dyn_nappend(s, (ಧ್ರುವ unsigned ವರ್ಣ *)str, strlen(str));
    curl_free(str);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ಸಂರಚನಾ dynbuf *s, ಧ್ರುವ ವರ್ಣ *fmt, ...)
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
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Returns a pointer to the buffer.
 */
ವರ್ಣ *curlx_dyn_ptr(ಧ್ರುವ ಸಂರಚನಾ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಪ್ರತ್ಯಾಗಮ s->bufr;
}

ವರ್ಣ *curlx_dyn_take(ಸಂರಚನಾ dynbuf *s, size_t *plen)
{
  ವರ್ಣ *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ರಿಕ್ತ;
  s->leng = 0;
  s->allc = 0;
  ಪ್ರತ್ಯಾಗಮ ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned ವರ್ಣ *curlx_dyn_uptr(ಧ್ರುವ ಸಂರಚನಾ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ಧ್ರುವ ಸಂರಚನಾ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಪ್ರತ್ಯಾಗಮ s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ಸಂರಚನಾ dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಯದಿ(set > s->leng)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
