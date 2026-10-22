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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "multi_ntfy.h"

#ಯದಿ CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ಸಂರಚನಾ mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

ಸಂರಚನಾ mntfy_chunk {
  ಸಂರಚನಾ mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  ಸಂರಚನಾ mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

ಸ್ಥಿರ ಸಂರಚನಾ mntfy_chunk *mnfty_chunk_create(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mntfy_chunk));
}

ಸ್ಥಿರ ಶೂನ್ಯ mnfty_chunk_destroy(ಸಂರಚನಾ mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ಸ್ಥಿರ ಶೂನ್ಯ mnfty_chunk_reset(ಸಂರಚನಾ mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

ಸ್ಥಿರ bool mntfy_chunk_append(ಸಂರಚನಾ mntfy_chunk *chunk,
                               ಸಂರಚನಾ Curl_easy *data,
                               uint32_t type)
{
  ಸಂರಚನಾ mntfy_entry *e;

  ಯದಿ(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಸಂರಚನಾ mntfy_chunk *mntfy_non_full_tail(ಸಂರಚನಾ curl_multi_ntfy *mntfy)
{
  ಸಂರಚನಾ mntfy_chunk *chunk;
  ಯದಿ(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    ಯದಿ(!chunk)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    ಪ್ರತ್ಯಾಗಮ chunk;
  }
  ಅನ್ಯಥಾ ಯದಿ(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    ಪ್ರತ್ಯಾಗಮ mntfy->tail;
  ಅನ್ಯಥಾ { /* tail is full. */
    chunk = mnfty_chunk_create();
    ಯದಿ(!chunk)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    ಪ್ರತ್ಯಾಗಮ chunk;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ mntfy_chunk_dispatch_all(ಸಂರಚನಾ Curl_multi *multi,
                                     ಸಂರಚನಾ mntfy_chunk *chunk)
{
  ಸಂರಚನಾ mntfy_entry *e;
  ಸಂರಚನಾ Curl_easy *data;

  ಯಾವತ್(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    ಯದಿ(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

ಶೂನ್ಯ Curl_mntfy_init(ಸಂರಚನಾ Curl_multi *multi)
{
  memset(&multi->ntfy, 0, ಪರಿಮಾಣ(multi->ntfy));
}

ಶೂನ್ಯ Curl_mntfy_cleanup(ಸಂರಚನಾ Curl_multi *multi)
{
  ಯಾವತ್(multi->ntfy.head) {
    ಸಂರಚನಾ mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = ರಿಕ್ತ;
}

CURLMcode Curl_mntfy_enable(ಸಂರಚನಾ Curl_multi *multi, unsigned ಪೂರ್ಣಾಙ್ಕ type)
{
  ಯದಿ(type >= CURLMNOTIFY_LAST)
    ಪ್ರತ್ಯಾಗಮ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ಸಂರಚನಾ Curl_multi *multi, unsigned ಪೂರ್ಣಾಙ್ಕ type)
{
  ಯದಿ(type >= CURLMNOTIFY_LAST)
    ಪ್ರತ್ಯಾಗಮ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಶೂನ್ಯ Curl_mntfy_add(ಸಂರಚನಾ Curl_easy *data, unsigned ಪೂರ್ಣಾಙ್ಕ type)
{
  ಸಂರಚನಾ Curl_multi *multi = data ? data->multi : ರಿಕ್ತ;
  ಯದಿ(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ಸಂರಚನಾ mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    ಯದಿ(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    ಅನ್ಯಥಾ
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ಸಂರಚನಾ Curl_multi *multi)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;

  ಯದಿ(!multi)
    ಪ್ರತ್ಯಾಗಮ CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  ಯಾವತ್(multi->ntfy.head && !multi->ntfy.failure) {
    ಸಂರಚನಾ mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    ಯದಿ(chunk == multi->ntfy.tail) /* last one, keep */
      ಭಙ್ಗ;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  ಯದಿ(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    ಪ್ರತ್ಯಾಗಮ mresult;
  }
  ಅನ್ಯಥಾ
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}
