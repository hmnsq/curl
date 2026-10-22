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

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multihandle.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "multi_ntfy.h"

#எனில் CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

கட்டமைப்பு mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

கட்டமைப்பு mntfy_chunk {
  கட்டமைப்பு mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  கட்டமைப்பு mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

நிலையான கட்டமைப்பு mntfy_chunk *mnfty_chunk_create(வெறுமை)
{
  திரும்பு curlx_calloc(1, அளவு(கட்டமைப்பு mntfy_chunk));
}

நிலையான வெறுமை mnfty_chunk_destroy(கட்டமைப்பு mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

நிலையான வெறுமை mnfty_chunk_reset(கட்டமைப்பு mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

நிலையான bool mntfy_chunk_append(கட்டமைப்பு mntfy_chunk *chunk,
                               கட்டமைப்பு Curl_easy *data,
                               uint32_t type)
{
  கட்டமைப்பு mntfy_entry *e;

  எனில்(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    திரும்பு FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  திரும்பு TRUE;
}

நிலையான கட்டமைப்பு mntfy_chunk *mntfy_non_full_tail(கட்டமைப்பு curl_multi_ntfy *mntfy)
{
  கட்டமைப்பு mntfy_chunk *chunk;
  எனில்(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    எனில்(!chunk)
      திரும்பு வெற்று;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    திரும்பு chunk;
  }
  இல்லையெனில் எனில்(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    திரும்பு mntfy->tail;
  இல்லையெனில் { /* tail is full. */
    chunk = mnfty_chunk_create();
    எனில்(!chunk)
      திரும்பு வெற்று;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    திரும்பு chunk;
  }
}

நிலையான வெறுமை mntfy_chunk_dispatch_all(கட்டமைப்பு Curl_multi *multi,
                                     கட்டமைப்பு mntfy_chunk *chunk)
{
  கட்டமைப்பு mntfy_entry *e;
  கட்டமைப்பு Curl_easy *data;

  போது(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    எனில்(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, data, multi->ntfy.ntfy_cb_data);
    }
    /* once dispatched, safe to increment */
    chunk->r_offset++;
  }
  mnfty_chunk_reset(chunk);
}

வெறுமை Curl_mntfy_init(கட்டமைப்பு Curl_multi *multi)
{
  memset(&multi->ntfy, 0, அளவு(multi->ntfy));
}

வெறுமை Curl_mntfy_cleanup(கட்டமைப்பு Curl_multi *multi)
{
  போது(multi->ntfy.head) {
    கட்டமைப்பு mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = வெற்று;
}

CURLMcode Curl_mntfy_enable(கட்டமைப்பு Curl_multi *multi, unsigned முழு type)
{
  எனில்(type >= CURLMNOTIFY_LAST)
    திரும்பு CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  திரும்பு CURLM_OK;
}

CURLMcode Curl_mntfy_disable(கட்டமைப்பு Curl_multi *multi, unsigned முழு type)
{
  எனில்(type >= CURLMNOTIFY_LAST)
    திரும்பு CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  திரும்பு CURLM_OK;
}

வெறுமை Curl_mntfy_add(கட்டமைப்பு Curl_easy *data, unsigned முழு type)
{
  கட்டமைப்பு Curl_multi *multi = data ? data->multi : வெற்று;
  எனில்(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    கட்டமைப்பு mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    எனில்(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    இல்லையெனில்
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(கட்டமைப்பு Curl_multi *multi)
{
  கட்டமைப்பு Curl_mapi_guard guard;

  எனில்(!multi)
    திரும்பு CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  போது(multi->ntfy.head && !multi->ntfy.failure) {
    கட்டமைப்பு mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    எனில்(chunk == multi->ntfy.tail) /* last one, keep */
      நிறுத்து;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  எனில்(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    திரும்பு mresult;
  }
  இல்லையெனில்
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  திரும்பு CURLM_OK;
}
