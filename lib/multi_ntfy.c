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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multihandle.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "multi_ntfy.h"

#જો CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

સંરચના mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

સંરચના mntfy_chunk {
  સંરચના mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  સંરચના mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

સ્થિર સંરચના mntfy_chunk *mnfty_chunk_create(શૂન્ય)
{
  પાછા curlx_calloc(1, કદ(સંરચના mntfy_chunk));
}

સ્થિર શૂન્ય mnfty_chunk_destroy(સંરચના mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

સ્થિર શૂન્ય mnfty_chunk_reset(સંરચના mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

સ્થિર bool mntfy_chunk_append(સંરચના mntfy_chunk *chunk,
                               સંરચના Curl_easy *data,
                               uint32_t type)
{
  સંરચના mntfy_entry *e;

  જો(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    પાછા FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  પાછા TRUE;
}

સ્થિર સંરચના mntfy_chunk *mntfy_non_full_tail(સંરચના curl_multi_ntfy *mntfy)
{
  સંરચના mntfy_chunk *chunk;
  જો(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    જો(!chunk)
      પાછા ખાલી;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    પાછા chunk;
  }
  નહીંતર જો(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    પાછા mntfy->tail;
  નહીંતર { /* tail is full. */
    chunk = mnfty_chunk_create();
    જો(!chunk)
      પાછા ખાલી;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    પાછા chunk;
  }
}

સ્થિર શૂન્ય mntfy_chunk_dispatch_all(સંરચના Curl_multi *multi,
                                     સંરચના mntfy_chunk *chunk)
{
  સંરચના mntfy_entry *e;
  સંરચના Curl_easy *data;

  જ્યારે(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    જો(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

શૂન્ય Curl_mntfy_init(સંરચના Curl_multi *multi)
{
  memset(&multi->ntfy, 0, કદ(multi->ntfy));
}

શૂન્ય Curl_mntfy_cleanup(સંરચના Curl_multi *multi)
{
  જ્યારે(multi->ntfy.head) {
    સંરચના mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = ખાલી;
}

CURLMcode Curl_mntfy_enable(સંરચના Curl_multi *multi, unsigned પૂર્ણાંક type)
{
  જો(type >= CURLMNOTIFY_LAST)
    પાછા CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  પાછા CURLM_OK;
}

CURLMcode Curl_mntfy_disable(સંરચના Curl_multi *multi, unsigned પૂર્ણાંક type)
{
  જો(type >= CURLMNOTIFY_LAST)
    પાછા CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  પાછા CURLM_OK;
}

શૂન્ય Curl_mntfy_add(સંરચના Curl_easy *data, unsigned પૂર્ણાંક type)
{
  સંરચના Curl_multi *multi = data ? data->multi : ખાલી;
  જો(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    સંરચના mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    જો(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    નહીંતર
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(સંરચના Curl_multi *multi)
{
  સંરચના Curl_mapi_guard guard;

  જો(!multi)
    પાછા CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  જ્યારે(multi->ntfy.head && !multi->ntfy.failure) {
    સંરચના mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    જો(chunk == multi->ntfy.tail) /* last one, keep */
      તોડો;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  જો(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    પાછા mresult;
  }
  નહીંતર
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  પાછા CURLM_OK;
}
