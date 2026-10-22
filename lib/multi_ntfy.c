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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multihandle.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "multi_ntfy.h"

#যদি CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

কাঠামো mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_আকার 128

কাঠামো mntfy_chunk {
  কাঠামো mntfy_chunk *পরবর্তী;
  size_t r_offset;
  size_t w_offset;
  কাঠামো mntfy_entry entries[CURL_MNTFY_CHUNK_আকার];
};

স্থির কাঠামো mntfy_chunk *mnfty_chunk_create(শূন্য)
{
  ফেরত curlx_calloc(1, আকার(কাঠামো mntfy_chunk));
}

স্থির শূন্য mnfty_chunk_destroy(কাঠামো mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

স্থির শূন্য mnfty_chunk_reset(কাঠামো mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

স্থির bool mntfy_chunk_append(কাঠামো mntfy_chunk *chunk,
                               কাঠামো Curl_easy *তথ্য,
                               uint32_t type)
{
  কাঠামো mntfy_entry *e;

  যদি(chunk->w_offset >= CURL_MNTFY_CHUNK_আকার)
    ফেরত FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = তথ্য->mid;
  e->type = type;
  ফেরত TRUE;
}

স্থির কাঠামো mntfy_chunk *mntfy_non_full_লেজ(কাঠামো curl_multi_ntfy *mntfy)
{
  কাঠামো mntfy_chunk *chunk;
  যদি(!mntfy->লেজ) {
    chunk = mnfty_chunk_create();
    যদি(!chunk)
      ফেরত ফাঁকা;
    DEBUGASSERT(!mntfy->শীর্ষ);
    mntfy->শীর্ষ = mntfy->লেজ = chunk;
    ফেরত chunk;
  }
  অন্যথা যদি(mntfy->লেজ->w_offset < CURL_MNTFY_CHUNK_আকার)
    ফেরত mntfy->লেজ;
  অন্যথা { /* tail is full. */
    chunk = mnfty_chunk_create();
    যদি(!chunk)
      ফেরত ফাঁকা;
    DEBUGASSERT(mntfy->শীর্ষ);
    mntfy->লেজ->পরবর্তী = chunk;
    mntfy->লেজ = chunk;
    ফেরত chunk;
  }
}

স্থির শূন্য mntfy_chunk_dispatch_all(কাঠামো Curl_multi *multi,
                                     কাঠামো mntfy_chunk *chunk)
{
  কাঠামো mntfy_entry *e;
  কাঠামো Curl_easy *তথ্য;

  যখন(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    তথ্য = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    যদি(তথ্য && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, তথ্য, multi->ntfy.ntfy_cb_তথ্য);
    }
    /* once dispatched, safe to increment */
    chunk->r_offset++;
  }
  mnfty_chunk_reset(chunk);
}

শূন্য Curl_mntfy_init(কাঠামো Curl_multi *multi)
{
  memset(&multi->ntfy, 0, আকার(multi->ntfy));
}

শূন্য Curl_mntfy_cleanup(কাঠামো Curl_multi *multi)
{
  যখন(multi->ntfy.শীর্ষ) {
    কাঠামো mntfy_chunk *chunk = multi->ntfy.শীর্ষ;
    multi->ntfy.শীর্ষ = chunk->পরবর্তী;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.লেজ = ফাঁকা;
}

CURLMcode Curl_mntfy_enable(কাঠামো Curl_multi *multi, unsigned পূর্ণসংখ্যা type)
{
  যদি(type >= CURLMNOTIFY_LAST)
    ফেরত CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  ফেরত CURLM_OK;
}

CURLMcode Curl_mntfy_disable(কাঠামো Curl_multi *multi, unsigned পূর্ণসংখ্যা type)
{
  যদি(type >= CURLMNOTIFY_LAST)
    ফেরত CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  ফেরত CURLM_OK;
}

শূন্য Curl_mntfy_add(কাঠামো Curl_easy *তথ্য, unsigned পূর্ণসংখ্যা type)
{
  কাঠামো Curl_multi *multi = তথ্য ? তথ্য->multi : ফাঁকা;
  যদি(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    কাঠামো mntfy_chunk *লেজ = mntfy_non_full_লেজ(&multi->ntfy);
    CURL_TRC_M(তথ্য, "[NTFY] add %u for xfer %u", type, তথ্য->mid);
    যদি(লেজ)
      mntfy_chunk_append(লেজ, তথ্য, (uint32_t)type);
    অন্যথা
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(কাঠামো Curl_multi *multi)
{
  কাঠামো Curl_mapi_guard guard;

  যদি(!multi)
    ফেরত CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  যখন(multi->ntfy.শীর্ষ && !multi->ntfy.failure) {
    কাঠামো mntfy_chunk *chunk = multi->ntfy.শীর্ষ;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    যদি(chunk == multi->ntfy.লেজ) /* last one, keep */
      বিরতি;
    DEBUGASSERT(chunk->পরবর্তী);
    DEBUGASSERT(multi->ntfy.শীর্ষ != multi->ntfy.লেজ);
    multi->ntfy.শীর্ষ = chunk->পরবর্তী;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  যদি(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    ফেরত mresult;
  }
  অন্যথা
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  ফেরত CURLM_OK;
}
