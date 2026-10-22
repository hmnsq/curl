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
#include "curl_setup.h"

#include "curlx/dynbuf.h"
#include "curl_printf.h"

#define MIN_FIRST_ALLOC 32

#ifdef DEBUGBUILD
#define DYNINIT 0xbee51da /* random pattern */
#endif

/*
 * Init a dynbuf struct.
 */
void curlx_dyn_init(sa_nracanaa dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_SIZE); /* catch crazy mistakes */
  s->bufr = NULL;
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
void curlx_dyn_free(sa_nracanaa dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
static CURLcode dyn_nappend(sa_nracanaa dynbuf *s,
                            const unsigned var_na *mem, size_t len)
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

  ᮚᮦᮔ᮪(fit > s->toobig) {
    curlx_dyn_free(s);
    ᮝᮜᮤ CURLE_TOO_LARGE;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    ᮚᮦᮔ᮪(MIN_FIRST_ALLOC > s->toobig)
      a = s->toobig;
    ᮜᮤᮚ ᮚᮦᮔ᮪(fit < MIN_FIRST_ALLOC)
      a = MIN_FIRST_ALLOC;
    ᮜᮤᮚ
      a = fit;
  }
  ᮜᮤᮚ {
    ᮞᮨᮓᮨᮜ(a < fit)
      a *= 2;
    ᮚᮦᮔ᮪(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  ᮚᮦᮔ᮪(a != s->allc) {
    void *p = curlx_realloc(s->bufr, a);
    ᮚᮦᮔ᮪(!p) {
      curlx_dyn_free(s);
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    }
    s->bufr = p;
    s->allc = a;
  }

  ᮚᮦᮔ᮪(len)
    memcpy(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  ᮝᮜᮤ CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
void curlx_dyn_reset(sa_nracanaa dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮚᮦᮔ᮪(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_tail(sa_nracanaa dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮚᮦᮔ᮪(trail > s->leng)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;
  ᮜᮤᮚ ᮚᮦᮔ᮪(trail == s->leng)
    ᮝᮜᮤ CURLE_OK;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!trail) {
    curlx_dyn_reset(s);
  }
  ᮜᮤᮚ {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  ᮝᮜᮤ CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(sa_nracanaa dynbuf *s, const void *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮝᮜᮤ dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_add(sa_nracanaa dynbuf *s, const var_na *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = strlen(str);
  ᮝᮜᮤ dyn_nappend(s, (const unsigned var_na *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(sa_nracanaa dynbuf *s, const var_na *fmt, va_list ap)
{
#ifdef BUILDING_LIBCURL
  puur_naa_nka rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  ᮚᮦᮔ᮪(!rc)
    ᮝᮜᮤ CURLE_OK;
  ᮜᮤᮚ ᮚᮦᮔ᮪(rc == MERR_TOO_LARGE)
    ᮝᮜᮤ CURLE_TOO_LARGE;
  ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
#ᮜᮤᮚ
  var_na *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  ᮚᮦᮔ᮪(str) {
    CURLcode result = dyn_nappend(s, (const unsigned var_na *)str, strlen(str));
    curl_free(str);
    ᮝᮜᮤ result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
#endif
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(sa_nracanaa dynbuf *s, const var_na *fmt, ...)
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
  ᮝᮜᮤ result;
}

/*
 * Returns a pointer to the buffer.
 */
var_na *curlx_dyn_ptr(const sa_nracanaa dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮝᮜᮤ s->bufr;
}

var_na *curlx_dyn_take(sa_nracanaa dynbuf *s, size_t *plen)
{
  var_na *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = NULL;
  s->leng = 0;
  s->allc = 0;
  ᮝᮜᮤ ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
unsigned var_na *curlx_dyn_uptr(const sa_nracanaa dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮝᮜᮤ (unsigned var_na *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(const sa_nracanaa dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮝᮜᮤ s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(sa_nracanaa dynbuf *s, size_t set)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ᮚᮦᮔ᮪(set > s->leng)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;
  s->leng = set;
  s->bufr[s->leng] = 0;
  ᮝᮜᮤ CURLE_OK;
}
