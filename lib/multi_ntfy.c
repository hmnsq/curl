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

#اگر CURLMNOTIFY_آخری > 31
#غلطی "notification types must fit below the curl_multi_ntfy high flag bit"
#اگر_ختم

#تعریف_کرو CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ساخت mntfy_اندراج {
  uint32_t mid;
  uint32_t type;
};

#تعریف_کرو CURL_MNTFY_CHUNK_سائز 128

ساخت mntfy_chunk {
  ساخت mntfy_chunk *اگلا;
  size_t r_انحراف;
  size_t w_انحراف;
  ساخت mntfy_اندراج entries[CURL_MNTFY_CHUNK_سائز];
};

ساکن ساخت mntfy_chunk *mnfty_chunk_create(خالی)
{
  واپس curlx_calloc(1, حجم(ساخت mntfy_chunk));
}

ساکن خالی mnfty_chunk_destroy(ساخت mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ساکن خالی mnfty_chunk_reset(ساخت mntfy_chunk *chunk)
{
  chunk->r_انحراف = chunk->w_انحراف = 0;
}

ساکن bool mntfy_chunk_append(ساخت mntfy_chunk *chunk,
                               ساخت Curl_easy *ڈیٹا,
                               uint32_t type)
{
  ساخت mntfy_اندراج *e;

  اگر(chunk->w_انحراف >= CURL_MNTFY_CHUNK_سائز)
    واپس FALSE;
  e = &chunk->entries[chunk->w_انحراف++];
  e->mid = ڈیٹا->mid;
  e->type = type;
  واپس TRUE;
}

ساکن ساخت mntfy_chunk *mntfy_non_بھرا_دم(ساخت curl_multi_ntfy *mntfy)
{
  ساخت mntfy_chunk *chunk;
  اگر(!mntfy->دم) {
    chunk = mnfty_chunk_create();
    اگر(!chunk)
      واپس تہی;
    DEBUGASSERT(!mntfy->سر);
    mntfy->سر = mntfy->دم = chunk;
    واپس chunk;
  }
  ورنہ اگر(mntfy->دم->w_انحراف < CURL_MNTFY_CHUNK_سائز)
    واپس mntfy->دم;
  ورنہ { /* tail is full. */
    chunk = mnfty_chunk_create();
    اگر(!chunk)
      واپس تہی;
    DEBUGASSERT(mntfy->سر);
    mntfy->دم->اگلا = chunk;
    mntfy->دم = chunk;
    واپس chunk;
  }
}

ساکن خالی mntfy_chunk_dispatch_all(ساخت Curl_multi *multi,
                                     ساخت mntfy_chunk *chunk)
{
  ساخت mntfy_اندراج *e;
  ساخت Curl_easy *ڈیٹا;

  جب(multi->ntfy.ntfy_cb &&
        (chunk->r_انحراف < chunk->w_انحراف) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_انحراف];
    ڈیٹا = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    اگر(ڈیٹا && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, ڈیٹا, multi->ntfy.ntfy_cb_ڈیٹا);
    }
    /* once dispatched, safe to increment */
    chunk->r_انحراف++;
  }
  mnfty_chunk_reset(chunk);
}

خالی Curl_mntfy_init(ساخت Curl_multi *multi)
{
  memset(&multi->ntfy, 0, حجم(multi->ntfy));
}

خالی Curl_mntfy_cleanup(ساخت Curl_multi *multi)
{
  جب(multi->ntfy.سر) {
    ساخت mntfy_chunk *chunk = multi->ntfy.سر;
    multi->ntfy.سر = chunk->اگلا;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.دم = تہی;
}

CURLMcode Curl_mntfy_enable(ساخت Curl_multi *multi, غیرعلامتی صحیح type)
{
  اگر(type >= CURLMNOTIFY_آخری)
    واپس CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  واپس CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ساخت Curl_multi *multi, غیرعلامتی صحیح type)
{
  اگر(type >= CURLMNOTIFY_آخری)
    واپس CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  واپس CURLM_OK;
}

خالی Curl_mntfy_جوڑو(ساخت Curl_easy *ڈیٹا, غیرعلامتی صحیح type)
{
  ساخت Curl_multi *multi = ڈیٹا ? ڈیٹا->multi : تہی;
  اگر(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_آخری &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ساخت mntfy_chunk *دم = mntfy_non_بھرا_دم(&multi->ntfy);
    CURL_TRC_M(ڈیٹا, "[NTFY] add %u for xfer %u", type, ڈیٹا->mid);
    اگر(دم)
      mntfy_chunk_append(دم, ڈیٹا, (uint32_t)type);
    ورنہ
      multi->ntfy.failure = CURLM_برآمد_کا_حافظہ;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ساخت Curl_multi *multi)
{
  ساخت Curl_mapi_guard guard;

  اگر(!multi)
    واپس CURLM_BAD_FUNCTION_دلیل;

  CURL_CBAPI_MULTI_آغاز(&guard, multi, multi_ntfy_cb);

  جب(multi->ntfy.سر && !multi->ntfy.failure) {
    ساخت mntfy_chunk *chunk = multi->ntfy.سر;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_انحراف == chunk->w_انحراف);

    اگر(chunk == multi->ntfy.دم) /* last one, keep */
      توڑو;
    DEBUGASSERT(chunk->اگلا);
    DEBUGASSERT(multi->ntfy.سر != multi->ntfy.دم);
    multi->ntfy.سر = chunk->اگلا;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_انتہا(&guard);

  اگر(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    واپس mresult;
  }
  ورنہ
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  واپس CURLM_OK;
}
