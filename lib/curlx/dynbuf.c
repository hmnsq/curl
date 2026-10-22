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
#समावेश "curl_setup.h"

#समावेश "curlx/dynbuf.h"
#समावेश "curl_printf.h"

#घोषणा MIN_FIRST_ALLOC 32

#अगर_घोषित DEBUGBUILD
#घोषणा DYNINIT 0xbee51da /* random pattern */
#पूर्ण_अगर

/*
 * Init a dynbuf struct.
 */
व्योम curlx_dyn_init(काष्ठा dynbuf *s, माप_प्रकार toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_आकार); /* catch crazy mistakes */
  s->bufr = शून्य;
  s->leng = 0;
  s->allc = 0;
  s->toobig = toobig;
#अगर_घोषित DEBUGBUILD
  s->init = DYNINIT;
#पूर्ण_अगर
}

/*
 * free the buffer and re-init the necessary fields. It does not touch the
 * 'init' field and thus this buffer can be reused to add data to again.
 */
व्योम curlx_dyn_free(काष्ठा dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
अटल CURLcode dyn_nappend(काष्ठा dynbuf *s,
                            स्थिर अचिन्हित अक्षर *mem, माप_प्रकार len)
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

  अगर(fit > s->toobig) {
    curlx_dyn_free(s);
    वापस CURLE_TOO_LARGE;
  }
  अन्यथा अगर(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    अगर(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    अन्यथा अगर(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    अन्यथा
      a = fit;
  }
  अन्यथा {
    जबतक(a < fit)
      a *= 2;
    अगर(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  अगर(a != s->allc) {
    व्योम *p = curlx_realloc(s->bufr, a);
    अगर(!p) {
      curlx_dyn_free(s);
      वापस CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  अगर(len)
    स_नकल(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  वापस CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
व्योम curlx_dyn_reset(काष्ठा dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  अगर(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_पुच्छ(काष्ठा dynbuf *s, माप_प्रकार trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  अगर(trail > s->leng)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अन्यथा अगर(trail == s->leng)
    वापस CURLE_OK;
  अन्यथा अगर(!trail) {
    curlx_dyn_reset(s);
  }
  अन्यथा {
    स_हटाओ(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  वापस CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(काष्ठा dynbuf *s, स्थिर व्योम *mem, माप_प्रकार len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  वापस dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(काष्ठा dynbuf *s, स्थिर अक्षर *str)
{
  माप_प्रकार n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = म_माप(str);
  वापस dyn_nappend(s, (स्थिर अचिन्हित अक्षर *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(काष्ठा dynbuf *s, स्थिर अक्षर *fmt, बहु_सूची ap)
{
#अगर_घोषित BUILDING_LIBCURL
  पूर्णांक rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  अगर(!rc)
    वापस CURLE_OK;
  अन्यथा अगर(rc == MERR_TOO_LARGE)
    वापस CURLE_TOO_LARGE;
  वापस CURLE_OUT_OF_MEMORY;
#अन्यथा
  अक्षर *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  अगर(str) {
    CURLcode परिणाम = dyn_nappend(s, (स्थिर अचिन्हित अक्षर *)str, म_माप(str));
    curl_free(str);
    वापस परिणाम;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  वापस CURLE_OUT_OF_MEMORY;
#पूर्ण_अगर
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(काष्ठा dynbuf *s, स्थिर अक्षर *fmt, ...)
{
  CURLcode परिणाम;
  बहु_सूची ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(म_भेद(fmt, "%s")); /* use curlx_dyn_add instead */
  बहु_शुरू(ap, fmt);
  परिणाम = curlx_dyn_vaddf(s, fmt, ap);
  बहु_पूर्ण(ap);
  वापस परिणाम;
}

/*
 * Returns a pointer to the buffer.
 */
अक्षर *curlx_dyn_ptr(स्थिर काष्ठा dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  वापस s->bufr;
}

अक्षर *curlx_dyn_take(काष्ठा dynbuf *s, माप_प्रकार *plen)
{
  अक्षर *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = शून्य;
  s->leng = 0;
  s->allc = 0;
  वापस ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
अचिन्हित अक्षर *curlx_dyn_uptr(स्थिर काष्ठा dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  वापस (अचिन्हित अक्षर *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
माप_प्रकार curlx_dyn_len(स्थिर काष्ठा dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  वापस s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(काष्ठा dynbuf *s, माप_प्रकार set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  अगर(set > s->leng)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  वापस CURLE_OK;
}
