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

#include "urldata.h"
#include "curl_trc.h"
#include "multihandle.h"
#include "multiif.h"
#include "multi_ntfy.h"

#nếu CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

struct mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

struct mntfy_chunk {
  struct mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  struct mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

static struct mntfy_chunk *mnfty_chunk_create(void)
{
  trả_về curlx_calloc(1, sizeof(struct mntfy_chunk));
}

static void mnfty_chunk_destroy(struct mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

static void mnfty_chunk_reset(struct mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

static bool mntfy_chunk_append(struct mntfy_chunk *chunk,
                               struct Curl_easy *data,
                               uint32_t type)
{
  struct mntfy_entry *e;

  nếu(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    trả_về FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  trả_về TRUE;
}

static struct mntfy_chunk *mntfy_non_full_tail(struct curl_multi_ntfy *mntfy)
{
  struct mntfy_chunk *chunk;
  nếu(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    nếu(!chunk)
      trả_về NULL;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    trả_về chunk;
  }
  không_thì nếu(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    trả_về mntfy->tail;
  không_thì { /* tail is full. */
    chunk = mnfty_chunk_create();
    nếu(!chunk)
      trả_về NULL;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    trả_về chunk;
  }
}

static void mntfy_chunk_dispatch_all(struct Curl_multi *multi,
                                     struct mntfy_chunk *chunk)
{
  struct mntfy_entry *e;
  struct Curl_easy *data;

  trong_khi(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    nếu(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

void Curl_mntfy_init(struct Curl_multi *multi)
{
  memset(&multi->ntfy, 0, sizeof(multi->ntfy));
}

void Curl_mntfy_cleanup(struct Curl_multi *multi)
{
  trong_khi(multi->ntfy.head) {
    struct mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = NULL;
}

CURLMcode Curl_mntfy_enable(struct Curl_multi *multi, unsigned số_nguyên type)
{
  nếu(type >= CURLMNOTIFY_LAST)
    trả_về CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  trả_về CURLM_OK;
}

CURLMcode Curl_mntfy_disable(struct Curl_multi *multi, unsigned số_nguyên type)
{
  nếu(type >= CURLMNOTIFY_LAST)
    trả_về CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  trả_về CURLM_OK;
}

void Curl_mntfy_add(struct Curl_easy *data, unsigned số_nguyên type)
{
  struct Curl_multi *multi = data ? data->multi : NULL;
  nếu(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    struct mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    nếu(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    không_thì
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(struct Curl_multi *multi)
{
  struct Curl_mapi_guard guard;

  nếu(!multi)
    trả_về CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  trong_khi(multi->ntfy.head && !multi->ntfy.failure) {
    struct mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    nếu(chunk == multi->ntfy.tail) /* last one, keep */
      break;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  nếu(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    trả_về mresult;
  }
  không_thì
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  trả_về CURLM_OK;
}
