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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "multi_ntfy.h"

#सचे CURLMNOTIFY_LAST > 31
#error "notification types must fit below the curl_multi_ntfy high flag bit"
#endif

#define CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

संरचना mntfy_entry {
  uint32_t mid;
  uint32_t type;
};

#define CURL_MNTFY_CHUNK_SIZE 128

संरचना mntfy_chunk {
  संरचना mntfy_chunk *next;
  size_t r_offset;
  size_t w_offset;
  संरचना mntfy_entry entries[CURL_MNTFY_CHUNK_SIZE];
};

स्थिर संरचना mntfy_chunk *mnfty_chunk_create(शून्य)
{
  पटिनिवत्त curlx_calloc(1, परिमाण(संरचना mntfy_chunk));
}

स्थिर शून्य mnfty_chunk_destroy(संरचना mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

स्थिर शून्य mnfty_chunk_reset(संरचना mntfy_chunk *chunk)
{
  chunk->r_offset = chunk->w_offset = 0;
}

स्थिर bool mntfy_chunk_append(संरचना mntfy_chunk *chunk,
                               संरचना Curl_easy *data,
                               uint32_t type)
{
  संरचना mntfy_entry *e;

  सचे(chunk->w_offset >= CURL_MNTFY_CHUNK_SIZE)
    पटिनिवत्त FALSE;
  e = &chunk->entries[chunk->w_offset++];
  e->mid = data->mid;
  e->type = type;
  पटिनिवत्त TRUE;
}

स्थिर संरचना mntfy_chunk *mntfy_non_full_tail(संरचना curl_multi_ntfy *mntfy)
{
  संरचना mntfy_chunk *chunk;
  सचे(!mntfy->tail) {
    chunk = mnfty_chunk_create();
    सचे(!chunk)
      पटिनिवत्त रिक्त;
    DEBUGASSERT(!mntfy->head);
    mntfy->head = mntfy->tail = chunk;
    पटिनिवत्त chunk;
  }
  अञ्ञथा सचे(mntfy->tail->w_offset < CURL_MNTFY_CHUNK_SIZE)
    पटिनिवत्त mntfy->tail;
  अञ्ञथा { /* tail is full. */
    chunk = mnfty_chunk_create();
    सचे(!chunk)
      पटिनिवत्त रिक्त;
    DEBUGASSERT(mntfy->head);
    mntfy->tail->next = chunk;
    mntfy->tail = chunk;
    पटिनिवत्त chunk;
  }
}

स्थिर शून्य mntfy_chunk_dispatch_all(संरचना Curl_multi *multi,
                                     संरचना mntfy_chunk *chunk)
{
  संरचना mntfy_entry *e;
  संरचना Curl_easy *data;

  याव(multi->ntfy.ntfy_cb &&
        (chunk->r_offset < chunk->w_offset) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_offset];
    data = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    सचे(data && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
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

शून्य Curl_mntfy_init(संरचना Curl_multi *multi)
{
  memset(&multi->ntfy, 0, परिमाण(multi->ntfy));
}

शून्य Curl_mntfy_cleanup(संरचना Curl_multi *multi)
{
  याव(multi->ntfy.head) {
    संरचना mntfy_chunk *chunk = multi->ntfy.head;
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.tail = रिक्त;
}

CURLMcode Curl_mntfy_enable(संरचना Curl_multi *multi, unsigned पूर्णाङ्क type)
{
  सचे(type >= CURLMNOTIFY_LAST)
    पटिनिवत्त CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  पटिनिवत्त CURLM_OK;
}

CURLMcode Curl_mntfy_disable(संरचना Curl_multi *multi, unsigned पूर्णाङ्क type)
{
  सचे(type >= CURLMNOTIFY_LAST)
    पटिनिवत्त CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  पटिनिवत्त CURLM_OK;
}

शून्य Curl_mntfy_add(संरचना Curl_easy *data, unsigned पूर्णाङ्क type)
{
  संरचना Curl_multi *multi = data ? data->multi : रिक्त;
  सचे(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_LAST &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    संरचना mntfy_chunk *tail = mntfy_non_full_tail(&multi->ntfy);
    CURL_TRC_M(data, "[NTFY] add %u for xfer %u", type, data->mid);
    सचे(tail)
      mntfy_chunk_append(tail, data, (uint32_t)type);
    अञ्ञथा
      multi->ntfy.failure = CURLM_OUT_OF_MEMORY;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(संरचना Curl_multi *multi)
{
  संरचना Curl_mapi_guard guard;

  सचे(!multi)
    पटिनिवत्त CURLM_BAD_FUNCTION_ARGUMENT;

  CURL_CBAPI_MULTI_START(&guard, multi, multi_ntfy_cb);

  याव(multi->ntfy.head && !multi->ntfy.failure) {
    संरचना mntfy_chunk *chunk = multi->ntfy.head;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_offset == chunk->w_offset);

    सचे(chunk == multi->ntfy.tail) /* last one, keep */
      भङ्ग;
    DEBUGASSERT(chunk->next);
    DEBUGASSERT(multi->ntfy.head != multi->ntfy.tail);
    multi->ntfy.head = chunk->next;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_END(&guard);

  सचे(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    पटिनिवत्त mresult;
  }
  अञ्ञथा
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  पटिनिवत्त CURLM_OK;
}
