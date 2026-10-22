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

#ವ್ಯಾಖ್ಯಾನಿಸು MIN_ಮೊದಲ_ALLOC 32

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು DYNINIT 0xbee51da /* random pattern */
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Init a dynbuf struct.
 */
ಶೂನ್ಯ curlx_dyn_init(ರಚನೆ dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_ಗಾತ್ರ); /* catch crazy mistakes */
  s->bufr = ಖಾಲಿ;
  s->leng = 0;
  s->allc = 0;
  s->toobig = toobig;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  s->init = DYNINIT;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * free the buffer and re-init the necessary fields. It does not touch the
 * 'init' field and thus this buffer can be reused to add data to again.
 */
ಶೂನ್ಯ curlx_dyn_free(ರಚನೆ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ಸ್ಥಿರ CURLcode dyn_nappend(ರಚನೆ dynbuf *s,
                            ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *mem, size_t len)
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

  ಆದರೆ(fit > s->toobig) {
    curlx_dyn_free(s);
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    ಆದರೆ(MIN_ಮೊದಲ_ALLOC > s->toobig)
      a = s->toobig;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(fit < MIN_ಮೊದಲ_ALLOC)
      a = MIN_ಮೊದಲ_ALLOC;
    ಇಲ್ಲದಿದ್ದರೆ
      a = fit;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆಗ(a < fit)
      a *= 2;
    ಆದರೆ(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  ಆದರೆ(a != s->allc) {
    ಶೂನ್ಯ *p = curlx_realloc(s->bufr, a);
    ಆದರೆ(!p) {
      curlx_dyn_free(s);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    s->bufr = p;
    s->allc = a;
  }

  ಆದರೆ(len)
    ಸ್ಮರಣೆ_ನಕಲು(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
ಶೂನ್ಯ curlx_dyn_reset(ರಚನೆ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಆದರೆ(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_ಬಾಲ(ರಚನೆ dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಆದರೆ(trail > s->leng)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(trail == s->leng)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!trail) {
    curlx_dyn_reset(s);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಹಿಂತಿರುಗಿ dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_ಕೂಡಿಸು(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
  ಹಿಂತಿರುಗಿ dyn_nappend(s, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BUILDING_LIBCURL
  ಪೂರ್ಣಾಂಕ rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  ಆದರೆ(!rc)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MERR_TOO_LARGE)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  ಆದರೆ(str) {
    CURLcode ಫಲಿತಾಂಶ = dyn_nappend(s, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)str, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str));
    curl_free(str);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  CURLcode ಫಲಿತಾಂಶ;
  va_list ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(strcmp(fmt, "%s")); /* use curlx_dyn_add instead */
  va_start(ap, fmt);
  ಫಲಿತಾಂಶ = curlx_dyn_vaddf(s, fmt, ap);
  va_end(ap);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Returns a pointer to the buffer.
 */
ಅಕ್ಷರ *curlx_dyn_ptr(ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಹಿಂತಿರುಗಿ s->bufr;
}

ಅಕ್ಷರ *curlx_dyn_take(ರಚನೆ dynbuf *s, size_t *plen)
{
  ಅಕ್ಷರ *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ಖಾಲಿ;
  s->leng = 0;
  s->allc = 0;
  ಹಿಂತಿರುಗಿ ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *curlx_dyn_uptr(ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಹಿಂತಿರುಗಿ s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ರಚನೆ dynbuf *s, size_t ಗಣ)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ಆದರೆ(ಗಣ > s->leng)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  s->leng = ಗಣ;
  s->bufr[s->leng] = 0;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
