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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
ശൂന്യം curlx_dyn_init(ഘടന dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = ശൂന്യ;
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
ശൂന്യം curlx_dyn_free(ഘടന dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
സ്ഥിതം CURLcode dyn_nappend(ഘടന dynbuf *s,
                            സ്ഥിരം unsigned അക്ഷരം *mem, size_t len)
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

  എങ്കിൽ(fit > s->toobig) {
    curlx_dyn_free(s);
    തിരികെ CURLE_TOO_LARGE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    എങ്കിൽ(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    അല്ലെങ്കിൽ എങ്കിൽ(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    അല്ലെങ്കിൽ
      a = fit;
  }
  അല്ലെങ്കിൽ {
    ആകുമ്പോൾ(a < fit)
      a *= 2;
    എങ്കിൽ(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  എങ്കിൽ(a != s->allc) {
    ശൂന്യം *p = curlx_realloc(s->bufr, a);
    എങ്കിൽ(!p) {
      curlx_dyn_free(s);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  എങ്കിൽ(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  തിരികെ CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
ശൂന്യം curlx_dyn_reset(ഘടന dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  എങ്കിൽ(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(ഘടന dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  എങ്കിൽ(trail > s->leng)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  അല്ലെങ്കിൽ എങ്കിൽ(trail == s->leng)
    തിരികെ CURLE_OK;
  അല്ലെങ്കിൽ എങ്കിൽ(!trail) {
    curlx_dyn_reset(s);
  }
  അല്ലെങ്കിൽ {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  തിരികെ CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ഘടന dynbuf *s, സ്ഥിരം ശൂന്യം *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  തിരികെ dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(ഘടന dynbuf *s, സ്ഥിരം അക്ഷരം *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  തിരികെ dyn_nappend(s, (സ്ഥിരം unsigned അക്ഷരം *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ഘടന dynbuf *s, സ്ഥിരം അക്ഷരം *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  പൂർണ്ണസംഖ്യ rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  എങ്കിൽ(!rc)
    തിരികെ CURLE_OK;
  അല്ലെങ്കിൽ എങ്കിൽ(rc == MERR_TOO_LARGE)
    തിരികെ CURLE_TOO_LARGE;
  തിരികെ CURLE_OUT_OF_MEMORY;
#അല്ലെങ്കിൽ
  അക്ഷരം *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  എങ്കിൽ(str) {
    CURLcode result = dyn_nappend(s, (സ്ഥിരം unsigned അക്ഷരം *)str, strlen(str));
    curl_free(str);
    തിരികെ result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  തിരികെ CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ഘടന dynbuf *s, സ്ഥിരം അക്ഷരം *fmt, ...)
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
  തിരികെ result;
}

/*
 * Returns a pointer to the buffer.
 */
അക്ഷരം *curlx_dyn_ptr(സ്ഥിരം ഘടന dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  തിരികെ s->bufr;
}

അക്ഷരം *curlx_dyn_take(ഘടന dynbuf *s, size_t *plen)
{
  അക്ഷരം *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ശൂന്യ;
  s->leng = 0;
  s->allc = 0;
  തിരികെ ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned അക്ഷരം *curlx_dyn_uptr(സ്ഥിരം ഘടന dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  തിരികെ (unsigned അക്ഷരം *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(സ്ഥിരം ഘടന dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  തിരികെ s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ഘടന dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  എങ്കിൽ(set > s->leng)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  തിരികെ CURLE_OK;
}
