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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
வெறுமை curlx_dyn_init(கட்டமைப்பு dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = வெற்று;
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
வெறுமை curlx_dyn_free(கட்டமைப்பு dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
நிலையான CURLcode dyn_nappend(கட்டமைப்பு dynbuf *s,
                            மாறிலி unsigned எழுத்து *mem, size_t len)
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

  எனில்(fit > s->toobig) {
    curlx_dyn_free(s);
    திரும்பு CURLE_TOO_LARGE;
  }
  இல்லையெனில் எனில்(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    எனில்(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    இல்லையெனில் எனில்(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    இல்லையெனில்
      a = fit;
  }
  இல்லையெனில் {
    போது(a < fit)
      a *= 2;
    எனில்(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  எனில்(a != s->allc) {
    வெறுமை *p = curlx_realloc(s->bufr, a);
    எனில்(!p) {
      curlx_dyn_free(s);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  எனில்(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  திரும்பு CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
வெறுமை curlx_dyn_reset(கட்டமைப்பு dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  எனில்(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(கட்டமைப்பு dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  எனில்(trail > s->leng)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  இல்லையெனில் எனில்(trail == s->leng)
    திரும்பு CURLE_OK;
  இல்லையெனில் எனில்(!trail) {
    curlx_dyn_reset(s);
  }
  இல்லையெனில் {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  திரும்பு CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(கட்டமைப்பு dynbuf *s, மாறிலி வெறுமை *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  திரும்பு dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(கட்டமைப்பு dynbuf *s, மாறிலி எழுத்து *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  திரும்பு dyn_nappend(s, (மாறிலி unsigned எழுத்து *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(கட்டமைப்பு dynbuf *s, மாறிலி எழுத்து *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  முழு rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  எனில்(!rc)
    திரும்பு CURLE_OK;
  இல்லையெனில் எனில்(rc == MERR_TOO_LARGE)
    திரும்பு CURLE_TOO_LARGE;
  திரும்பு CURLE_OUT_OF_MEMORY;
#இல்லையெனில்
  எழுத்து *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  எனில்(str) {
    CURLcode result = dyn_nappend(s, (மாறிலி unsigned எழுத்து *)str, strlen(str));
    curl_free(str);
    திரும்பு result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  திரும்பு CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(கட்டமைப்பு dynbuf *s, மாறிலி எழுத்து *fmt, ...)
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
  திரும்பு result;
}

/*
 * Returns a pointer to the buffer.
 */
எழுத்து *curlx_dyn_ptr(மாறிலி கட்டமைப்பு dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  திரும்பு s->bufr;
}

எழுத்து *curlx_dyn_take(கட்டமைப்பு dynbuf *s, size_t *plen)
{
  எழுத்து *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = வெற்று;
  s->leng = 0;
  s->allc = 0;
  திரும்பு ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned எழுத்து *curlx_dyn_uptr(மாறிலி கட்டமைப்பு dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  திரும்பு (unsigned எழுத்து *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(மாறிலி கட்டமைப்பு dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  திரும்பு s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(கட்டமைப்பு dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  எனில்(set > s->leng)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  திரும்பு CURLE_OK;
}
