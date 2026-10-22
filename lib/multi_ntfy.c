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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ntfy.h"

#ਜੇ CURLMNOTIFY_ਆਖਰੀ > 31
#ਗਲਤੀ "notification types must fit below the curl_multi_ntfy high flag bit"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ਬਣਤਰ mntfy_ਇੰਦਰਾਜ {
  uint32_t mid;
  uint32_t type;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MNTFY_CHUNK_ਮਾਪ 128

ਬਣਤਰ mntfy_chunk {
  ਬਣਤਰ mntfy_chunk *ਅਗਲਾ;
  size_t r_ਵਿੱਥ;
  size_t w_ਵਿੱਥ;
  ਬਣਤਰ mntfy_ਇੰਦਰਾਜ entries[CURL_MNTFY_CHUNK_ਮਾਪ];
};

ਸਥਿਰ ਬਣਤਰ mntfy_chunk *mnfty_chunk_create(ਖਾਲੀ)
{
  ਵਾਪਸ curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ mntfy_chunk));
}

ਸਥਿਰ ਖਾਲੀ mnfty_chunk_destroy(ਬਣਤਰ mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ਸਥਿਰ ਖਾਲੀ mnfty_chunk_reset(ਬਣਤਰ mntfy_chunk *chunk)
{
  chunk->r_ਵਿੱਥ = chunk->w_ਵਿੱਥ = 0;
}

ਸਥਿਰ bool mntfy_chunk_append(ਬਣਤਰ mntfy_chunk *chunk,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               uint32_t type)
{
  ਬਣਤਰ mntfy_ਇੰਦਰਾਜ *e;

  ਜੇ(chunk->w_ਵਿੱਥ >= CURL_MNTFY_CHUNK_ਮਾਪ)
    ਵਾਪਸ FALSE;
  e = &chunk->entries[chunk->w_ਵਿੱਥ++];
  e->mid = ਅੰਕੜੇ->mid;
  e->type = type;
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਬਣਤਰ mntfy_chunk *mntfy_non_ਭਰਿਆ_ਪੂਛ(ਬਣਤਰ curl_multi_ntfy *mntfy)
{
  ਬਣਤਰ mntfy_chunk *chunk;
  ਜੇ(!mntfy->ਪੂਛ) {
    chunk = mnfty_chunk_create();
    ਜੇ(!chunk)
      ਵਾਪਸ ਨਿਰਾ;
    DEBUGASSERT(!mntfy->ਸਿਰ);
    mntfy->ਸਿਰ = mntfy->ਪੂਛ = chunk;
    ਵਾਪਸ chunk;
  }
  ਨਹੀਂਤਾਂ ਜੇ(mntfy->ਪੂਛ->w_ਵਿੱਥ < CURL_MNTFY_CHUNK_ਮਾਪ)
    ਵਾਪਸ mntfy->ਪੂਛ;
  ਨਹੀਂਤਾਂ { /* tail is full. */
    chunk = mnfty_chunk_create();
    ਜੇ(!chunk)
      ਵਾਪਸ ਨਿਰਾ;
    DEBUGASSERT(mntfy->ਸਿਰ);
    mntfy->ਪੂਛ->ਅਗਲਾ = chunk;
    mntfy->ਪੂਛ = chunk;
    ਵਾਪਸ chunk;
  }
}

ਸਥਿਰ ਖਾਲੀ mntfy_chunk_dispatch_all(ਬਣਤਰ Curl_multi *multi,
                                     ਬਣਤਰ mntfy_chunk *chunk)
{
  ਬਣਤਰ mntfy_ਇੰਦਰਾਜ *e;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;

  ਜਦੋਂ(multi->ntfy.ntfy_cb &&
        (chunk->r_ਵਿੱਥ < chunk->w_ਵਿੱਥ) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_ਵਿੱਥ];
    ਅੰਕੜੇ = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    ਜੇ(ਅੰਕੜੇ && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, ਅੰਕੜੇ, multi->ntfy.ntfy_cb_ਅੰਕੜੇ);
    }
    /* once dispatched, safe to increment */
    chunk->r_ਵਿੱਥ++;
  }
  mnfty_chunk_reset(chunk);
}

ਖਾਲੀ Curl_mntfy_init(ਬਣਤਰ Curl_multi *multi)
{
  memset(&multi->ntfy, 0, ਆਕਾਰ(multi->ntfy));
}

ਖਾਲੀ Curl_mntfy_cleanup(ਬਣਤਰ Curl_multi *multi)
{
  ਜਦੋਂ(multi->ntfy.ਸਿਰ) {
    ਬਣਤਰ mntfy_chunk *chunk = multi->ntfy.ਸਿਰ;
    multi->ntfy.ਸਿਰ = chunk->ਅਗਲਾ;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.ਪੂਛ = ਨਿਰਾ;
}

CURLMcode Curl_mntfy_enable(ਬਣਤਰ Curl_multi *multi, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type)
{
  ਜੇ(type >= CURLMNOTIFY_ਆਖਰੀ)
    ਵਾਪਸ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  ਵਾਪਸ CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ਬਣਤਰ Curl_multi *multi, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type)
{
  ਜੇ(type >= CURLMNOTIFY_ਆਖਰੀ)
    ਵਾਪਸ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  ਵਾਪਸ CURLM_OK;
}

ਖਾਲੀ Curl_mntfy_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type)
{
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ ? ਅੰਕੜੇ->multi : ਨਿਰਾ;
  ਜੇ(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_ਆਖਰੀ &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ਬਣਤਰ mntfy_chunk *ਪੂਛ = mntfy_non_ਭਰਿਆ_ਪੂਛ(&multi->ntfy);
    CURL_TRC_M(ਅੰਕੜੇ, "[NTFY] add %u for xfer %u", type, ਅੰਕੜੇ->mid);
    ਜੇ(ਪੂਛ)
      mntfy_chunk_append(ਪੂਛ, ਅੰਕੜੇ, (uint32_t)type);
    ਨਹੀਂਤਾਂ
      multi->ntfy.failure = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ਬਣਤਰ Curl_multi *multi)
{
  ਬਣਤਰ Curl_mapi_guard guard;

  ਜੇ(!multi)
    ਵਾਪਸ CURLM_BAD_FUNCTION_ਦਲੀਲ;

  CURL_CBAPI_MULTI_ਸ਼ੁਰੂ(&guard, multi, multi_ntfy_cb);

  ਜਦੋਂ(multi->ntfy.ਸਿਰ && !multi->ntfy.failure) {
    ਬਣਤਰ mntfy_chunk *chunk = multi->ntfy.ਸਿਰ;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_ਵਿੱਥ == chunk->w_ਵਿੱਥ);

    ਜੇ(chunk == multi->ntfy.ਪੂਛ) /* last one, keep */
      ਰੋਕੋ;
    DEBUGASSERT(chunk->ਅਗਲਾ);
    DEBUGASSERT(multi->ntfy.ਸਿਰ != multi->ntfy.ਪੂਛ);
    multi->ntfy.ਸਿਰ = chunk->ਅਗਲਾ;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_ਸਿਰਾ(&guard);

  ਜੇ(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    ਵਾਪਸ mresult;
  }
  ਨਹੀਂਤਾਂ
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  ਵਾਪਸ CURLM_OK;
}
