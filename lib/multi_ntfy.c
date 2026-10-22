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

#𐡄𐡍 CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

بنية mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

بنية mntfy_chunk {
  بنية mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  بنية mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
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
  chunk->r_offset = chunk->w_offset = 0;
}

ساكن bool mntfy_chunk_append(بنية mntfy_chunk *chunk,
                               بنية Curl_easy *data,
                               uint32_t type)
{
  بنية mntfy_entry *e;

  𐡄𐡍(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    أرجع FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  أرجع TRUE;
}

ساكن بنية mntfy_chunk *mntfy_non_full_tail(بنية curl_multi_ntfy *mntfy)
{
  بنية mntfy_chunk *chunk;
  𐡄𐡍(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    𐡄𐡍(!chunk)
      أرجع عدم;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    أرجع chunk;
  }
  وإلا 𐡄𐡍(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    أرجع mntfy->tail;
  وإلا { /* tail is full. */
    chunk = mnfty_chunk_create();
    𐡄𐡍(!chunk)
      أرجع عدم;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    أرجع chunk;
  }
}

ساكن فراغ mntfy_chunk_dispatch_all(بنية Curl_multi *multi,
                                     بنية mntfy_chunk *chunk)
{
  بنية mntfy_entry *e;
  بنية Curl_easy *data;

  طالما(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    𐡄𐡍(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

فراغ Curl_mntfy_init(بنية Curl_multi *multi)
{
  memset(&multi->ntfy, 0, حجم(multi->ntfy));
}

فراغ Curl_mntfy_cleanup(بنية Curl_multi *multi)
{
  طالما(multi->ntfy.head) {
    بنية mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = عدم;
}

CURLMcode Curl_mntfy_enable(بنية Curl_multi *multi, unsigned صحيح type)
{
  𐡄𐡍(type >= CURLMNOTIFY_LAST)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

CURLMcode Curl_mntfy_disable(بنية Curl_multi *multi, unsigned صحيح type)
{
  𐡄𐡍(type >= CURLMNOTIFY_LAST)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

فراغ Curl_mntfy_add(بنية Curl_easy *data, unsigned صحيح type)
{
  بنية Curl_multi *multi = data ? data->multi : عدم;
  𐡄𐡍(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    بنية mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    𐡄𐡍(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    وإلا
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(بنية Curl_multi *multi)
{
  بنية Curl_mapi_guard guard;

  𐡄𐡍(!multi)
    أرجع CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  طالما(multi->ntfy.head && !multi->ntfy.failure) {
    بنية mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    𐡄𐡍(chunk == multi->ntfy.tail) /* last one, keep */
      اكسر;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  𐡄𐡍(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    أرجع mresult;
  }
  وإلا
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  أرجع CURLM_OK;
}
