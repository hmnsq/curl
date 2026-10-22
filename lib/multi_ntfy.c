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

#إذا CURLMNOTIFY_آخری > 31
#غلطی "notification types must fit below the curl_multi_ntfy high flag bit"
#جے_انت

#تعریف_کرو CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

بنية mntfy_اندراج {
  uint32_t mid;
  uint32_t type;
};

#تعریف_کرو CURL_MNTFY_CHUNK_ناپ 128

بنية mntfy_chunk {
  بنية mntfy_chunk *اگلا;
  size_t r_وتھ;
  size_t w_وتھ;
  بنية mntfy_اندراج entries[CURL_MNTFY_CHUNK_ناپ];
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
  chunk->r_وتھ = chunk->w_وتھ = 0;
}

ساكن bool mntfy_chunk_append(بنية mntfy_chunk *chunk,
                               بنية Curl_easy *انکڑے,
                               uint32_t type)
{
  بنية mntfy_اندراج *e;

  إذا(chunk->w_وتھ >= CURL_MNTFY_CHUNK_ناپ)
    أرجع FALSE;
  e = &chunk->entries[chunk->w_وتھ++];
  e->mid = انکڑے->mid;
  e->type = type;
  أرجع TRUE;
}

ساكن بنية mntfy_chunk *mntfy_non_بھریا_پوچھ(بنية curl_multi_ntfy *mntfy)
{
  بنية mntfy_chunk *chunk;
  إذا(!mntfy->پوچھ) {
    chunk = mnfty_chunk_create();
    إذا(!chunk)
      أرجع عدم;
    DEBUGASSERT(!mntfy->سر);
    mntfy->سر = mntfy->پوچھ = chunk;
    أرجع chunk;
  }
  وإلا إذا(mntfy->پوچھ->w_وتھ < CURL_MNTFY_CHUNK_ناپ)
    أرجع mntfy->پوچھ;
  وإلا { /* tail is full. */
    chunk = mnfty_chunk_create();
    إذا(!chunk)
      أرجع عدم;
    DEBUGASSERT(mntfy->سر);
    mntfy->پوچھ->اگلا = chunk;
    mntfy->پوچھ = chunk;
    أرجع chunk;
  }
}

ساكن فراغ mntfy_chunk_dispatch_all(بنية Curl_multi *multi,
                                     بنية mntfy_chunk *chunk)
{
  بنية mntfy_اندراج *e;
  بنية Curl_easy *انکڑے;

  طالما(multi->ntfy.ntfy_cb &&
        (chunk->r_وتھ < chunk->w_وتھ) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_وتھ];
    انکڑے = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    إذا(انکڑے && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, انکڑے, multi->ntfy.ntfy_cb_انکڑے);
    }
    /* once dispatched, safe to increment */
    chunk->r_وتھ++;
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
    multi->ntfy.سر = chunk->اگلا;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.پوچھ = عدم;
}

CURLMcode Curl_mntfy_enable(بنية Curl_multi *multi, بنانشانی صحيح type)
{
  إذا(type >= CURLMNOTIFY_آخری)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

CURLMcode Curl_mntfy_disable(بنية Curl_multi *multi, بنانشانی صحيح type)
{
  إذا(type >= CURLMNOTIFY_آخری)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

فراغ Curl_mntfy_جوڑو(بنية Curl_easy *انکڑے, بنانشانی صحيح type)
{
  بنية Curl_multi *multi = انکڑے ? انکڑے->multi : عدم;
  إذا(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_آخری &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    بنية mntfy_chunk *پوچھ = mntfy_non_بھریا_پوچھ(&multi->ntfy);
    CURL_TRC_M(انکڑے, "[NTFY] add %u for xfer %u", type, انکڑے->mid);
    إذا(پوچھ)
      mntfy_chunk_append(پوچھ, انکڑے, (uint32_t)type);
    وإلا
      multi->ntfy.failure = CURLM_باہرلا_دا_حافظہ;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(بنية Curl_multi *multi)
{
  بنية Curl_mapi_guard guard;

  إذا(!multi)
    أرجع CURLM_BAD_FUNCTION_دلیل;

  CURL_CBAPI_MULTI_شروع(&guard, multi, multi_ntfy_cb);

  طالما(multi->ntfy.سر && !multi->ntfy.failure) {
    بنية mntfy_chunk *chunk = multi->ntfy.سر;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_وتھ == chunk->w_وتھ);

    إذا(chunk == multi->ntfy.پوچھ) /* last one, keep */
      اكسر;
    DEBUGASSERT(chunk->اگلا);
    DEBUGASSERT(multi->ntfy.سر != multi->ntfy.پوچھ);
    multi->ntfy.سر = chunk->اگلا;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_سرا(&guard);

  إذا(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    أرجع mresult;
  }
  وإلا
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  أرجع CURLM_OK;
}
