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
शून्य curlx_dyn_init(संरचना dynbuf *s, माप_प्रकार toobig)
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
                            स्थिरांक अचिन्हित अक्षर *mem, माप_प्रकार len)
{
  माप_प्रकार idx = s->leng;
  माप_प्रकार a = s->allc;
  माप_प्रकार fit = len + idx + 1; /* new string + old string + zero byte */

  /* try to detect if there is rubbish in the struct */
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(s->toobig);
  DEBUGASSERT(idx < s->toobig);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(a <= s->toobig);
  DEBUGASSERT(!len || mem);

  जर(fit > s->toobig) {
    curlx_dyn_free(s);
    परत CURLE_TOO_LARGE;
  }
  अन्यथा जर(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    जर(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    अन्यथा जर(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    अन्यथा
      a = fit;
  }
  अन्यथा {
    पर्यंत(a < fit)
      a *= 2;
    जर(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  जर(a != s->allc) {
    शून्य *p = curlx_realloc(s->bufr, a);
    जर(!p) {
      curlx_dyn_free(s);
      परत CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  जर(len)
    स_नकल(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  परत CURLE_OK;
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
  जर(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(संरचना dynbuf *s, माप_प्रकार trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  जर(trail > s->leng)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  अन्यथा जर(trail == s->leng)
    परत CURLE_OK;
  अन्यथा जर(!trail) {
    curlx_dyn_reset(s);
  }
  अन्यथा {
    स_हटाओ(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  परत CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(संरचना dynbuf *s, स्थिरांक शून्य *mem, माप_प्रकार len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  परत dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(संरचना dynbuf *s, स्थिरांक अक्षर *str)
{
  माप_प्रकार n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = म_माप(str);
  परत dyn_nappend(s, (स्थिरांक अचिन्हित अक्षर *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(संरचना dynbuf *s, स्थिरांक अक्षर *fmt, बहु_सूची ap)
{
#ifdef BUILDING_LIBCURL
  पूर्णांक rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  जर(!rc)
    परत CURLE_OK;
  अन्यथा जर(rc == MERR_TOO_LARGE)
    परत CURLE_TOO_LARGE;
  परत CURLE_OUT_OF_MEMORY;
#अन्यथा
  अक्षर *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  जर(str) {
    CURLcode result = dyn_nappend(s, (स्थिरांक अचिन्हित अक्षर *)str, म_माप(str));
    curl_free(str);
    परत result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  परत CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(संरचना dynbuf *s, स्थिरांक अक्षर *fmt, ...)
{
  CURLcode result;
  बहु_सूची ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(म_भेद(fmt, "%s")); /* use curlx_dyn_add instead */
  बहु_शुरू(ap, fmt);
  result = curlx_dyn_vaddf(s, fmt, ap);
  बहु_पूर्ण(ap);
  परत result;
}

/*
 * Returns a pointer to the buffer.
 */
अक्षर *curlx_dyn_ptr(स्थिरांक संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  परत s->bufr;
}

अक्षर *curlx_dyn_take(संरचना dynbuf *s, माप_प्रकार *plen)
{
  अक्षर *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = रिक्त;
  s->leng = 0;
  s->allc = 0;
  परत ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
अचिन्हित अक्षर *curlx_dyn_uptr(स्थिरांक संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  परत (अचिन्हित अक्षर *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
माप_प्रकार curlx_dyn_len(स्थिरांक संरचना dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  परत s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(संरचना dynbuf *s, माप_प्रकार set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  जर(set > s->leng)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  परत CURLE_OK;
}
