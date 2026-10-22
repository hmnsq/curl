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

সংরচনা mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

সংরচনা mntfy_chunk {
  সংরচনা mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  সংরচনা mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

স্থির সংরচনা mntfy_chunk *mnfty_chunk_create(শূন্য)
{
  প্রত্যাগম curlx_calloc(1, পরিমাণ(সংরচনা mntfy_chunk));
}

স্থির শূন্য mnfty_chunk_destroy(সংরচনা mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

স্থির শূন্য mnfty_chunk_reset(সংরচনা mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

স্থির bool mntfy_chunk_append(সংরচনা mntfy_chunk *chunk,
                               সংরচনা Curl_easy *data,
                               uint32_t type)
{
  সংরচনা mntfy_entry *e;

  যদি(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    প্রত্যাগম FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  প্রত্যাগম TRUE;
}

স্থির সংরচনা mntfy_chunk *mntfy_non_full_tail(সংরচনা curl_multi_ntfy *mntfy)
{
  সংরচনা mntfy_chunk *chunk;
  যদি(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    যদি(!chunk)
      প্রত্যাগম রিক্ত;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    প্রত্যাগম chunk;
  }
  অন্যথা যদি(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    প্রত্যাগম mntfy->tail;
  অন্যথা { /* tail is full. */
    chunk = mnfty_chunk_create();
    যদি(!chunk)
      প্রত্যাগম রিক্ত;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    প্রত্যাগম chunk;
  }
}

স্থির শূন্য mntfy_chunk_dispatch_all(সংরচনা Curl_multi *multi,
                                     সংরচনা mntfy_chunk *chunk)
{
  সংরচনা mntfy_entry *e;
  সংরচনা Curl_easy *data;

  যাবত্(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    যদি(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

শূন্য Curl_mntfy_init(সংরচনা Curl_multi *multi)
{
  memset(&multi->ntfy, 0, পরিমাণ(multi->ntfy));
}

শূন্য Curl_mntfy_cleanup(সংরচনা Curl_multi *multi)
{
  যাবত্(multi->ntfy.head) {
    সংরচনা mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = রিক্ত;
}

CURLMcode Curl_mntfy_enable(সংরচনা Curl_multi *multi, unsigned পূর্ণাঙ্ক type)
{
  যদি(type >= CURLMNOTIFY_LAST)
    প্রত্যাগম CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  প্রত্যাগম CURLM_OK;
}

CURLMcode Curl_mntfy_disable(সংরচনা Curl_multi *multi, unsigned পূর্ণাঙ্ক type)
{
  যদি(type >= CURLMNOTIFY_LAST)
    প্রত্যাগম CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  প্রত্যাগম CURLM_OK;
}

শূন্য Curl_mntfy_add(সংরচনা Curl_easy *data, unsigned পূর্ণাঙ্ক type)
{
  সংরচনা Curl_multi *multi = data ? data->multi : রিক্ত;
  যদি(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    সংরচনা mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    যদি(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    অন্যথা
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(সংরচনা Curl_multi *multi)
{
  সংরচনা Curl_mapi_guard guard;

  যদি(!multi)
    প্রত্যাগম CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  যাবত্(multi->ntfy.head && !multi->ntfy.failure) {
    সংরচনা mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    যদি(chunk == multi->ntfy.tail) /* last one, keep */
      ভঙ্গ;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  যদি(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    প্রত্যাগম mresult;
  }
  অন্যথা
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  প্রত্যাগম CURLM_OK;
}
