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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "multihandle.h"
#شامل "multiif.h"
#شامل "multi_ntfy.h"

#اگر CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ساختار mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

ساختار mntfy_chunk {
  ساختار mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  ساختار mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

ایستا ساختار mntfy_chunk *mnfty_chunk_create(تهی)
{
  بازگشت curlx_calloc(1, اندازه(ساختار mntfy_chunk));
}

ایستا تهی mnfty_chunk_destroy(ساختار mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ایستا تهی mnfty_chunk_reset(ساختار mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

ایستا bool mntfy_chunk_append(ساختار mntfy_chunk *chunk,
                               ساختار Curl_easy *data,
                               uint32_t type)
{
  ساختار mntfy_entry *e;

  اگر(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    بازگشت FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  بازگشت TRUE;
}

ایستا ساختار mntfy_chunk *mntfy_non_full_tail(ساختار curl_multi_ntfy *mntfy)
{
  ساختار mntfy_chunk *chunk;
  اگر(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    اگر(!chunk)
      بازگشت پوچ;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    بازگشت chunk;
  }
  وگرنه اگر(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    بازگشت mntfy->tail;
  وگرنه { /* tail is full. */
    chunk = mnfty_chunk_create();
    اگر(!chunk)
      بازگشت پوچ;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    بازگشت chunk;
  }
}

ایستا تهی mntfy_chunk_dispatch_all(ساختار Curl_multi *multi,
                                     ساختار mntfy_chunk *chunk)
{
  ساختار mntfy_entry *e;
  ساختار Curl_easy *data;

  وقتی(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    اگر(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

تهی Curl_mntfy_init(ساختار Curl_multi *multi)
{
  memset(&multi->ntfy, 0, اندازه(multi->ntfy));
}

تهی Curl_mntfy_cleanup(ساختار Curl_multi *multi)
{
  وقتی(multi->ntfy.head) {
    ساختار mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = پوچ;
}

CURLMcode Curl_mntfy_enable(ساختار Curl_multi *multi, unsigned صحیح type)
{
  اگر(type >= CURLMNOTIFY_LAST)
    بازگشت CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  بازگشت CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ساختار Curl_multi *multi, unsigned صحیح type)
{
  اگر(type >= CURLMNOTIFY_LAST)
    بازگشت CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  بازگشت CURLM_OK;
}

تهی Curl_mntfy_add(ساختار Curl_easy *data, unsigned صحیح type)
{
  ساختار Curl_multi *multi = data ? data->multi : پوچ;
  اگر(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ساختار mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    اگر(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    وگرنه
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ساختار Curl_multi *multi)
{
  ساختار Curl_mapi_guard guard;

  اگر(!multi)
    بازگشت CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  وقتی(multi->ntfy.head && !multi->ntfy.failure) {
    ساختار mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    اگر(chunk == multi->ntfy.tail) /* last one, keep */
      بشکن;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  اگر(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    بازگشت mresult;
  }
  وگرنه
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  بازگشت CURLM_OK;
}
