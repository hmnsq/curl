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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multihandle.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "multi_ntfy.h"

#എങ്കിൽ CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ഘടന mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

ഘടന mntfy_chunk {
  ഘടന mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  ഘടന mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

സ്ഥിതം ഘടന mntfy_chunk *mnfty_chunk_create(ശൂന്യം)
{
  തിരികെ curlx_calloc(1, വലുപ്പം(ഘടന mntfy_chunk));
}

സ്ഥിതം ശൂന്യം mnfty_chunk_destroy(ഘടന mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

സ്ഥിതം ശൂന്യം mnfty_chunk_reset(ഘടന mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

സ്ഥിതം bool mntfy_chunk_append(ഘടന mntfy_chunk *chunk,
                               ഘടന Curl_easy *data,
                               uint32_t type)
{
  ഘടന mntfy_entry *e;

  എങ്കിൽ(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    തിരികെ FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  തിരികെ TRUE;
}

സ്ഥിതം ഘടന mntfy_chunk *mntfy_non_full_tail(ഘടന curl_multi_ntfy *mntfy)
{
  ഘടന mntfy_chunk *chunk;
  എങ്കിൽ(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    എങ്കിൽ(!chunk)
      തിരികെ ശൂന്യ;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    തിരികെ chunk;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    തിരികെ mntfy->tail;
  അല്ലെങ്കിൽ { /* tail is full. */
    chunk = mnfty_chunk_create();
    എങ്കിൽ(!chunk)
      തിരികെ ശൂന്യ;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    തിരികെ chunk;
  }
}

സ്ഥിതം ശൂന്യം mntfy_chunk_dispatch_all(ഘടന Curl_multi *multi,
                                     ഘടന mntfy_chunk *chunk)
{
  ഘടന mntfy_entry *e;
  ഘടന Curl_easy *data;

  ആകുമ്പോൾ(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    എങ്കിൽ(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

ശൂന്യം Curl_mntfy_init(ഘടന Curl_multi *multi)
{
  memset(&multi->ntfy, 0, വലുപ്പം(multi->ntfy));
}

ശൂന്യം Curl_mntfy_cleanup(ഘടന Curl_multi *multi)
{
  ആകുമ്പോൾ(multi->ntfy.head) {
    ഘടന mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = ശൂന്യ;
}

CURLMcode Curl_mntfy_enable(ഘടന Curl_multi *multi, unsigned പൂർണ്ണസംഖ്യ type)
{
  എങ്കിൽ(type >= CURLMNOTIFY_LAST)
    തിരികെ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  തിരികെ CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ഘടന Curl_multi *multi, unsigned പൂർണ്ണസംഖ്യ type)
{
  എങ്കിൽ(type >= CURLMNOTIFY_LAST)
    തിരികെ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  തിരികെ CURLM_OK;
}

ശൂന്യം Curl_mntfy_add(ഘടന Curl_easy *data, unsigned പൂർണ്ണസംഖ്യ type)
{
  ഘടന Curl_multi *multi = data ? data->multi : ശൂന്യ;
  എങ്കിൽ(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ഘടന mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    എങ്കിൽ(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    അല്ലെങ്കിൽ
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ഘടന Curl_multi *multi)
{
  ഘടന Curl_mapi_guard guard;

  എങ്കിൽ(!multi)
    തിരികെ CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  ആകുമ്പോൾ(multi->ntfy.head && !multi->ntfy.failure) {
    ഘടന mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    എങ്കിൽ(chunk == multi->ntfy.tail) /* last one, keep */
      നിർത്തുക;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  എങ്കിൽ(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    തിരികെ mresult;
  }
  അല്ലെങ്കിൽ
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  തിരികെ CURLM_OK;
}
