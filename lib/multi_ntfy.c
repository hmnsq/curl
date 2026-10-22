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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "multihandle.h"
#ضمن "multiif.h"
#ضمن "multi_ntfy.h"

#إذا CURLMNOTIFY_آخرین > 31
#خطا "notification types must fit below the curl_multi_ntfy high flag bit"
#پایان_اگر

#تعریف_کن CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

بنية mntfy_مدخل {
  uint32_t mid;
  uint32_t type;
};

#تعریف_کن CURL_MNTFY_CHUNK_اندازه 128

بنية mntfy_chunk {
  بنية mntfy_chunk *بعدی;
  size_t r_جابه_جایی;
  size_t w_جابه_جایی;
  بنية mntfy_مدخل entries[CURL_MNTFY_CHUNK_اندازه];
};

ساكن بنية mntfy_chunk *mnfty_chunk_create(فراغ)
{
  أرجع curlx_calloc(1, حجم(بنية mntfy_chunk));
}

ساكن فراغ mnfty_chunk_destroy(بنية mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ساكن فراغ mnfty_chunk_reset(بنية mntfy_chunk *chunk)
{
  chunk->r_جابه_جایی = chunk->w_جابه_جایی = 0;
}

ساكن bool mntfy_chunk_append(بنية mntfy_chunk *chunk,
                               بنية Curl_easy *داده,
                               uint32_t type)
{
  بنية mntfy_مدخل *e;

  إذا(chunk->w_جابه_جایی >= CURL_MNTFY_CHUNK_اندازه)
    أرجع FALSE;
  e = &chunk->entries[chunk->w_جابه_جایی++];
  e->mid = داده->mid;
  e->type = type;
  أرجع TRUE;
}

ساكن بنية mntfy_chunk *mntfy_non_پر_دم(بنية curl_multi_ntfy *mntfy)
{
  بنية mntfy_chunk *chunk;
  إذا(!mntfy->دم) {
    chunk = mnfty_chunk_create();
    إذا(!chunk)
      أرجع عدم;
    DEBUGASSERT(!mntfy->سر);
    mntfy->سر = mntfy->دم = chunk;
    أرجع chunk;
  }
  وإلا إذا(mntfy->دم->w_جابه_جایی < CURL_MNTFY_CHUNK_اندازه)
    أرجع mntfy->دم;
  وإلا { /* tail is full. */
    chunk = mnfty_chunk_create();
    إذا(!chunk)
      أرجع عدم;
    DEBUGASSERT(mntfy->سر);
    mntfy->دم->بعدی = chunk;
    mntfy->دم = chunk;
    أرجع chunk;
  }
}

ساكن فراغ mntfy_chunk_dispatch_all(بنية Curl_multi *multi,
                                     بنية mntfy_chunk *chunk)
{
  بنية mntfy_مدخل *e;
  بنية Curl_easy *داده;

  طالما(multi->ntfy.ntfy_cb &&
        (chunk->r_جابه_جایی < chunk->w_جابه_جایی) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_جابه_جایی];
    داده = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    إذا(داده && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, داده, multi->ntfy.ntfy_cb_داده);
    }
    /* once dispatched, safe to increment */
    chunk->r_جابه_جایی++;
  }
  mnfty_chunk_reset(chunk);
}

فراغ Curl_mntfy_init(بنية Curl_multi *multi)
{
  memset(&multi->ntfy, 0, حجم(multi->ntfy));
}

فراغ Curl_mntfy_cleanup(بنية Curl_multi *multi)
{
  طالما(multi->ntfy.سر) {
    بنية mntfy_chunk *chunk = multi->ntfy.سر;
    multi->ntfy.سر = chunk->بعدی;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.دم = عدم;
}

CURLMcode Curl_mntfy_enable(بنية Curl_multi *multi, بیعلامت صحيح type)
{
  إذا(type >= CURLMNOTIFY_آخرین)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

CURLMcode Curl_mntfy_disable(بنية Curl_multi *multi, بیعلامت صحيح type)
{
  إذا(type >= CURLMNOTIFY_آخرین)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

فراغ Curl_mntfy_جمع_کن(بنية Curl_easy *داده, بیعلامت صحيح type)
{
  بنية Curl_multi *multi = داده ? داده->multi : عدم;
  إذا(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_آخرین &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    بنية mntfy_chunk *دم = mntfy_non_پر_دم(&multi->ntfy);
    CURL_TRC_M(داده, "[NTFY] add %u for xfer %u", type, داده->mid);
    إذا(دم)
      mntfy_chunk_append(دم, داده, (uint32_t)type);
    وإلا
      multi->ntfy.failure = CURLM_بیرون_از_حافظه;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(بنية Curl_multi *multi)
{
  بنية Curl_mapi_guard guard;

  إذا(!multi)
    أرجع CURLM_BAD_FUNCTION_آرگومان;

  CURL_CBAPI_MULTI_شروع(&guard, multi, multi_ntfy_cb);

  طالما(multi->ntfy.سر && !multi->ntfy.failure) {
    بنية mntfy_chunk *chunk = multi->ntfy.سر;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_جابه_جایی == chunk->w_جابه_جایی);

    إذا(chunk == multi->ntfy.دم) /* last one, keep */
      اكسر;
    DEBUGASSERT(chunk->بعدی);
    DEBUGASSERT(multi->ntfy.سر != multi->ntfy.دم);
    multi->ntfy.سر = chunk->بعدی;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_انتها(&guard);

  إذا(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    أرجع mresult;
  }
  وإلا
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  أرجع CURLM_OK;
}
