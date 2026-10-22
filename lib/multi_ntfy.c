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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multihandle.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "multi_ntfy.h"

#ଯଦି CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ସଂରଚନା mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

ସଂରଚନା mntfy_chunk {
  ସଂରଚନା mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  ସଂରଚନା mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

ସ୍ଥିର ସଂରଚନା mntfy_chunk *mnfty_chunk_create(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ curlx_calloc(1, ଆକାର(ସଂରଚନା mntfy_chunk));
}

ସ୍ଥିର ଶୂନ୍ୟ mnfty_chunk_destroy(ସଂରଚନା mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ସ୍ଥିର ଶୂନ୍ୟ mnfty_chunk_reset(ସଂରଚନା mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

ସ୍ଥିର bool mntfy_chunk_append(ସଂରଚନା mntfy_chunk *chunk,
                               ସଂରଚନା Curl_easy *data,
                               uint32_t type)
{
  ସଂରଚନା mntfy_entry *e;

  ଯଦି(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    ଫେରନ୍ତୁ FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ସଂରଚନା mntfy_chunk *mntfy_non_full_tail(ସଂରଚନା curl_multi_ntfy *mntfy)
{
  ସଂରଚନା mntfy_chunk *chunk;
  ଯଦି(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    ଯଦି(!chunk)
      ଫେରନ୍ତୁ ଖାଲି;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    ଫେରନ୍ତୁ chunk;
  }
  ଅନ୍ୟଥା ଯଦି(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    ଫେରନ୍ତୁ mntfy->tail;
  ଅନ୍ୟଥା { /* tail is full. */
    chunk = mnfty_chunk_create();
    ଯଦି(!chunk)
      ଫେରନ୍ତୁ ଖାଲି;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    ଫେରନ୍ତୁ chunk;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ mntfy_chunk_dispatch_all(ସଂରଚନା Curl_multi *multi,
                                     ସଂରଚନା mntfy_chunk *chunk)
{
  ସଂରଚନା mntfy_entry *e;
  ସଂରଚନା Curl_easy *data;

  ଯେତେବେଳେ(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    ଯଦି(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

ଶୂନ୍ୟ Curl_mntfy_init(ସଂରଚନା Curl_multi *multi)
{
  memset(&multi->ntfy, 0, ଆକାର(multi->ntfy));
}

ଶୂନ୍ୟ Curl_mntfy_cleanup(ସଂରଚନା Curl_multi *multi)
{
  ଯେତେବେଳେ(multi->ntfy.head) {
    ସଂରଚନା mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = ଖାଲି;
}

CURLMcode Curl_mntfy_enable(ସଂରଚନା Curl_multi *multi, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
  ଯଦି(type >= CURLMNOTIFY_LAST)
    ଫେରନ୍ତୁ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  ଫେରନ୍ତୁ CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ସଂରଚନା Curl_multi *multi, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
  ଯଦି(type >= CURLMNOTIFY_LAST)
    ଫେରନ୍ତୁ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  ଫେରନ୍ତୁ CURLM_OK;
}

ଶୂନ୍ୟ Curl_mntfy_add(ସଂରଚନା Curl_easy *data, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
  ସଂରଚନା Curl_multi *multi = data ? data->multi : ଖାଲି;
  ଯଦି(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ସଂରଚନା mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    ଯଦି(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    ଅନ୍ୟଥା
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ସଂରଚନା Curl_multi *multi)
{
  ସଂରଚନା Curl_mapi_guard guard;

  ଯଦି(!multi)
    ଫେରନ୍ତୁ CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  ଯେତେବେଳେ(multi->ntfy.head && !multi->ntfy.failure) {
    ସଂରଚନା mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    ଯଦି(chunk == multi->ntfy.tail) /* last one, keep */
      ଭାଙ୍ଗ;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  ଯଦି(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    ଫେରନ୍ତୁ mresult;
  }
  ଅନ୍ୟଥା
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  ଫେରନ୍ତୁ CURLM_OK;
}
