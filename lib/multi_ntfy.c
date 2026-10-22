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

#ᨀᨘ CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

sa_nracanaa mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

sa_nracanaa mntfy_chunk {
  sa_nracanaa mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  sa_nracanaa mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

static sa_nracanaa mntfy_chunk *mnfty_chunk_create(void)
{
  ᨀᨘᨒ curlx_calloc(1, parimaa_na(sa_nracanaa mntfy_chunk));
}

static void mnfty_chunk_destroy(sa_nracanaa mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

static void mnfty_chunk_reset(sa_nracanaa mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

static bool mntfy_chunk_append(sa_nracanaa mntfy_chunk *chunk,
                               sa_nracanaa Curl_easy *data,
                               uint32_t type)
{
  sa_nracanaa mntfy_entry *e;

  ᨀᨘ(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    ᨀᨘᨒ FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  ᨀᨘᨒ TRUE;
}

static sa_nracanaa mntfy_chunk *mntfy_non_full_tail(sa_nracanaa curl_multi_ntfy *mntfy)
{
  sa_nracanaa mntfy_chunk *chunk;
  ᨀᨘ(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    ᨀᨘ(!chunk)
      ᨀᨘᨒ NULL;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    ᨀᨘᨒ chunk;
  }
  ᨀᨘᨁ ᨀᨘ(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    ᨀᨘᨒ mntfy->tail;
  ᨀᨘᨁ { /* tail is full. */
    chunk = mnfty_chunk_create();
    ᨀᨘ(!chunk)
      ᨀᨘᨒ NULL;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    ᨀᨘᨒ chunk;
  }
}

static void mntfy_chunk_dispatch_all(sa_nracanaa Curl_multi *multi,
                                     sa_nracanaa mntfy_chunk *chunk)
{
  sa_nracanaa mntfy_entry *e;
  sa_nracanaa Curl_easy *data;

  ᨀᨛ(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    ᨀᨘ(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

void Curl_mntfy_init(sa_nracanaa Curl_multi *multi)
{
  memset(&multi->ntfy, 0, parimaa_na(multi->ntfy));
}

void Curl_mntfy_cleanup(sa_nracanaa Curl_multi *multi)
{
  ᨀᨛ(multi->ntfy.head) {
    sa_nracanaa mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = NULL;
}

CURLMcode Curl_mntfy_enable(sa_nracanaa Curl_multi *multi, unsigned puur_naa_nka type)
{
  ᨀᨘ(type >= CURLMNOTIFY_LAST)
    ᨀᨘᨒ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  ᨀᨘᨒ CURLM_OK;
}

CURLMcode Curl_mntfy_disable(sa_nracanaa Curl_multi *multi, unsigned puur_naa_nka type)
{
  ᨀᨘ(type >= CURLMNOTIFY_LAST)
    ᨀᨘᨒ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  ᨀᨘᨒ CURLM_OK;
}

void Curl_mntfy_add(sa_nracanaa Curl_easy *data, unsigned puur_naa_nka type)
{
  sa_nracanaa Curl_multi *multi = data ? data->multi : NULL;
  ᨀᨘ(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    sa_nracanaa mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    ᨀᨘ(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    ᨀᨘᨁ
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(sa_nracanaa Curl_multi *multi)
{
  sa_nracanaa Curl_mapi_guard guard;

  ᨀᨘ(!multi)
    ᨀᨘᨒ CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  ᨀᨛ(multi->ntfy.head && !multi->ntfy.failure) {
    sa_nracanaa mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    ᨀᨘ(chunk == multi->ntfy.tail) /* last one, keep */
      bha_nga;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  ᨀᨘ(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    ᨀᨘᨒ mresult;
  }
  ᨀᨘᨁ
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  ᨀᨘᨒ CURLM_OK;
}
