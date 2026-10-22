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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
ଶୂନ୍ୟ curlx_dyn_init(ସଂରଚନା dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = ଖାଲି;
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
ଶୂନ୍ୟ curlx_dyn_free(ସଂରଚନା dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
ସ୍ଥିର CURLcode dyn_nappend(ସଂରଚନା dynbuf *s,
                            ଅଚଳ unsigned ଅକ୍ଷର *mem, size_t len)
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

  ଯଦି(fit > s->toobig) {
    curlx_dyn_free(s);
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  }
  ଅନ୍ୟଥା ଯଦି(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    ଯଦି(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    ଅନ୍ୟଥା ଯଦି(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    ଅନ୍ୟଥା
      a = fit;
  }
  ଅନ୍ୟଥା {
    ଯେତେବେଳେ(a < fit)
      a *= 2;
    ଯଦି(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  ଯଦି(a != s->allc) {
    ଶୂନ୍ୟ *p = curlx_realloc(s->bufr, a);
    ଯଦି(!p) {
      curlx_dyn_free(s);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  ଯଦି(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
ଶୂନ୍ୟ curlx_dyn_reset(ସଂରଚନା dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଯଦି(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(ସଂରଚନା dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଯଦି(trail > s->leng)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଅନ୍ୟଥା ଯଦି(trail == s->leng)
    ଫେରନ୍ତୁ CURLE_OK;
  ଅନ୍ୟଥା ଯଦି(!trail) {
    curlx_dyn_reset(s);
  }
  ଅନ୍ୟଥା {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(ସଂରଚନା dynbuf *s, ଅଚଳ ଶୂନ୍ୟ *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଫେରନ୍ତୁ dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(ସଂରଚନା dynbuf *s, ଅଚଳ ଅକ୍ଷର *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  ଫେରନ୍ତୁ dyn_nappend(s, (ଅଚଳ unsigned ଅକ୍ଷର *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(ସଂରଚନା dynbuf *s, ଅଚଳ ଅକ୍ଷର *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  ଯଦି(!rc)
    ଫେରନ୍ତୁ CURLE_OK;
  ଅନ୍ୟଥା ଯଦି(rc == MERR_TOO_LARGE)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
#ଅନ୍ୟଥା
  ଅକ୍ଷର *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  ଯଦି(str) {
    CURLcode result = dyn_nappend(s, (ଅଚଳ unsigned ଅକ୍ଷର *)str, strlen(str));
    curl_free(str);
    ଫେରନ୍ତୁ result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(ସଂରଚନା dynbuf *s, ଅଚଳ ଅକ୍ଷର *fmt, ...)
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
  ଫେରନ୍ତୁ result;
}

/*
 * Returns a pointer to the buffer.
 */
ଅକ୍ଷର *curlx_dyn_ptr(ଅଚଳ ସଂରଚନା dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଫେରନ୍ତୁ s->bufr;
}

ଅକ୍ଷର *curlx_dyn_take(ସଂରଚନା dynbuf *s, size_t *plen)
{
  ଅକ୍ଷର *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ଖାଲି;
  s->leng = 0;
  s->allc = 0;
  ଫେରନ୍ତୁ ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned ଅକ୍ଷର *curlx_dyn_uptr(ଅଚଳ ସଂରଚନା dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(ଅଚଳ ସଂରଚନା dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଫେରନ୍ତୁ s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(ସଂରଚନା dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ଯଦି(set > s->leng)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  ଫେରନ୍ତୁ CURLE_OK;
}
