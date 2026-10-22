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

#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
शून्य curlx_dyn_init(संरचना dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = रिक्त;
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
शून्य curlx_dyn_free(संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
स्थिर CURLcode dyn_nappend(संरचना dynbuf *s,
                            ध्रुव unsigned वर्ण *mem, size_t len)
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

  जइ(fit > s->toobig) {
    curlx_dyn_free(s);
    प्रत्यागम CURLE_TOO_LARGE;
  }
  अण्णहा जइ(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    जइ(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    अण्णहा जइ(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    अण्णहा
      a = fit;
  }
  अण्णहा {
    जाव(a < fit)
      a *= 2;
    जइ(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  जइ(a != s->allc) {
    शून्य *p = curlx_realloc(s->bufr, a);
    जइ(!p) {
      curlx_dyn_free(s);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  जइ(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  प्रत्यागम CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
शून्य curlx_dyn_reset(संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  जइ(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(संरचना dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  जइ(trail > s->leng)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  अण्णहा जइ(trail == s->leng)
    प्रत्यागम CURLE_OK;
  अण्णहा जइ(!trail) {
    curlx_dyn_reset(s);
  }
  अण्णहा {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(संरचना dynbuf *s, ध्रुव शून्य *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  प्रत्यागम dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(संरचना dynbuf *s, ध्रुव वर्ण *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  प्रत्यागम dyn_nappend(s, (ध्रुव unsigned वर्ण *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(संरचना dynbuf *s, ध्रुव वर्ण *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  पूर्णाङ्क rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  जइ(!rc)
    प्रत्यागम CURLE_OK;
  अण्णहा जइ(rc == MERR_TOO_LARGE)
    प्रत्यागम CURLE_TOO_LARGE;
  प्रत्यागम CURLE_OUT_OF_MEMORY;
#अण्णहा
  वर्ण *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  जइ(str) {
    CURLcode result = dyn_nappend(s, (ध्रुव unsigned वर्ण *)str, strlen(str));
    curl_free(str);
    प्रत्यागम result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  प्रत्यागम CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(संरचना dynbuf *s, ध्रुव वर्ण *fmt, ...)
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
  प्रत्यागम result;
}

/*
 * Returns a pointer to the buffer.
 */
वर्ण *curlx_dyn_ptr(ध्रुव संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  प्रत्यागम s->bufr;
}

वर्ण *curlx_dyn_take(संरचना dynbuf *s, size_t *plen)
{
  वर्ण *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = रिक्त;
  s->leng = 0;
  s->allc = 0;
  प्रत्यागम ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned वर्ण *curlx_dyn_uptr(ध्रुव संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  प्रत्यागम (unsigned वर्ण *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ध्रुव संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  प्रत्यागम s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(संरचना dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  जइ(set > s->leng)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  प्रत्यागम CURLE_OK;
}
