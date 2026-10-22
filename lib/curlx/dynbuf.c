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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "curl_printf.h"

#నిర్వచించు MIN_మొదటి_ALLOC 32

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు DYNINIT 0xbee51da /* random pattern */
#అయితే_ముగింపు

/*
 * Init a dynbuf struct.
 */
శూన్య curlx_dyn_init(నిర్మాణం dynbuf *s, size_t toobig)
{
  DEBUGASSERT(s);
  DEBUGASSERT(toobig);
  DEBUGASSERT(toobig <= MAX_DYNBUF_కొలత); /* catch crazy mistakes */
  s->bufr = ఖాళీ;
  s->leng = 0;
  s->allc = 0;
  s->toobig = toobig;
#నిర్వచితమైతే DEBUGBUILD
  s->init = DYNINIT;
#అయితే_ముగింపు
}

/*
 * free the buffer and re-init the necessary fields. It does not touch the
 * 'init' field and thus this buffer can be reused to add data to again.
 */
శూన్య curlx_dyn_free(నిర్మాణం dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  curlx_safefree(s->bufr);
  s->leng = s->allc = 0;
}

/*
 * Store/append an chunk of memory to the dynbuf.
 */
స్థిర CURLcode dyn_nappend(నిర్మాణం dynbuf *s,
                            స్థిరరాశి గుర్తులేని అక్షరం *mem, size_t len)
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

  ఒకవేళ(fit > s->toobig) {
    curlx_dyn_free(s);
    తిరిగి CURLE_TOO_LARGE;
  }
  లేకపోతే ఒకవేళ(!a) {
    DEBUGASSERT(!idx);
    /* first invoke */
    ఒకవేళ(MIN_మొదటి_ALLOC > s->toobig)
      a = s->toobig;
    లేకపోతే ఒకవేళ(fit < MIN_మొదటి_ALLOC)
      a = MIN_మొదటి_ALLOC;
    లేకపోతే
      a = fit;
  }
  లేకపోతే {
    అయితే(a < fit)
      a *= 2;
    ఒకవేళ(a > s->toobig)
      /* no point in allocating a larger buffer than this is allowed to use */
      a = s->toobig;
  }

  ఒకవేళ(a != s->allc) {
    శూన్య *p = curlx_realloc(s->bufr, a);
    ఒకవేళ(!p) {
      curlx_dyn_free(s);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    s->bufr = p;
    s->allc = a;
  }

  ఒకవేళ(len)
    జ్ఞాపకం_నకలు(&s->bufr[idx], mem, len);
  s->leng = idx + len;
  s->bufr[s->leng] = 0;
  తిరిగి CURLE_OK;
}

/*
 * Clears the string, keeps the allocation. This can also be called on a
 * buffer that already was freed.
 */
శూన్య curlx_dyn_reset(నిర్మాణం dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ఒకవేళ(s->leng)
    s->bufr[0] = 0;
  s->leng = 0;
}

/*
 * Specify the size of the tail to keep (number of bytes from the end of the
 * buffer). The rest is dropped.
 */
CURLcode curlx_dyn_తోక(నిర్మాణం dynbuf *s, size_t trail)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ఒకవేళ(trail > s->leng)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  లేకపోతే ఒకవేళ(trail == s->leng)
    తిరిగి CURLE_OK;
  లేకపోతే ఒకవేళ(!trail) {
    curlx_dyn_reset(s);
  }
  లేకపోతే {
    memmove(&s->bufr[0], &s->bufr[s->leng - trail], trail);
    s->leng = trail;
    s->bufr[s->leng] = 0;
  }
  తిరిగి CURLE_OK;
}

/*
 * Appends a buffer with length.
 */
CURLcode curlx_dyn_addn(నిర్మాణం dynbuf *s, స్థిరరాశి శూన్య *mem, size_t len)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  తిరిగి dyn_nappend(s, mem, len);
}

/*
 * Append a null-terminated string at the end.
 */
CURLcode curlx_dyn_కూడు(నిర్మాణం dynbuf *s, స్థిరరాశి అక్షరం *str)
{
  size_t n;
  DEBUGASSERT(str);
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  n = అక్షరశ్రేణి_పొడవు(str);
  తిరిగి dyn_nappend(s, (స్థిరరాశి గుర్తులేని అక్షరం *)str, n);
}

/*
 * Append a string vprintf()-style
 */
CURLcode curlx_dyn_vaddf(నిర్మాణం dynbuf *s, స్థిరరాశి అక్షరం *fmt, va_list ap)
{
#నిర్వచితమైతే BUILDING_LIBCURL
  పూర్ణాంకం rc;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(fmt);
  rc = curlx_dyn_vprintf(s, fmt, ap);

  ఒకవేళ(!rc)
    తిరిగి CURLE_OK;
  లేకపోతే ఒకవేళ(rc == MERR_TOO_LARGE)
    తిరిగి CURLE_TOO_LARGE;
  తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
#లేకపోతే
  అక్షరం *str;
  str = curl_mvaprintf(fmt, ap); /* this allocs a new string to append */

  ఒకవేళ(str) {
    CURLcode ఫలితం = dyn_nappend(s, (స్థిరరాశి గుర్తులేని అక్షరం *)str, అక్షరశ్రేణి_పొడవు(str));
    curl_free(str);
    తిరిగి ఫలితం;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  curlx_dyn_free(s);
  తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
#అయితే_ముగింపు
}

/*
 * Append a string printf()-style
 */
CURLcode curlx_dyn_addf(నిర్మాణం dynbuf *s, స్థిరరాశి అక్షరం *fmt, ...)
{
  CURLcode ఫలితం;
  va_list ap;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  DEBUGASSERT(strcmp(fmt, "%s")); /* use curlx_dyn_add instead */
  va_start(ap, fmt);
  ఫలితం = curlx_dyn_vaddf(s, fmt, ap);
  va_end(ap);
  తిరిగి ఫలితం;
}

/*
 * Returns a pointer to the buffer.
 */
అక్షరం *curlx_dyn_ptr(స్థిరరాశి నిర్మాణం dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  తిరిగి s->bufr;
}

అక్షరం *curlx_dyn_take(నిర్మాణం dynbuf *s, size_t *plen)
{
  అక్షరం *ptr = s->bufr;
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  *plen = s->leng;
  s->bufr = ఖాళీ;
  s->leng = 0;
  s->allc = 0;
  తిరిగి ptr;
}

/*
 * Returns an unsigned pointer to the buffer.
 */
గుర్తులేని అక్షరం *curlx_dyn_uptr(స్థిరరాశి నిర్మాణం dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  తిరిగి (గుర్తులేని అక్షరం *)s->bufr;
}

/*
 * Returns the length of the buffer.
 */
size_t curlx_dyn_len(స్థిరరాశి నిర్మాణం dynbuf *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  తిరిగి s->leng;
}

/*
 * Set a new (smaller) length.
 */
CURLcode curlx_dyn_setlen(నిర్మాణం dynbuf *s, size_t సమితి)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->init == DYNINIT);
  DEBUGASSERT(!s->leng || s->bufr);
  ఒకవేళ(సమితి > s->leng)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  s->leng = సమితి;
  s->bufr[s->leng] = 0;
  తిరిగి CURLE_OK;
}
