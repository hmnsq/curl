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

#إذا CURLMNOTIFY_آخري > 31
#غلطي "notification types must fit below the curl_multi_ntfy high flag bit"
#جيڪڏهن_پورو

#وصف_ڪريو CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

بنية mntfy_داخلا {
  uint32_t mid;
  uint32_t type;
};

#وصف_ڪريو CURL_MNTFY_CHUNK_سائيز 128

بنية mntfy_chunk {
  بنية mntfy_chunk *ايندڙ;
  size_t r_وٿي;
  size_t w_وٿي;
  بنية mntfy_داخلا entries[CURL_MNTFY_CHUNK_سائيز];
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
  chunk->r_وٿي = chunk->w_وٿي = 0;
}

ساكن bool mntfy_chunk_append(بنية mntfy_chunk *chunk,
                               بنية Curl_easy *معلومات,
                               uint32_t type)
{
  بنية mntfy_داخلا *e;

  إذا(chunk->w_وٿي >= CURL_MNTFY_CHUNK_سائيز)
    أرجع FALSE;
  e = &chunk->entries[chunk->w_وٿي++];
  e->mid = معلومات->mid;
  e->type = type;
  أرجع TRUE;
}

ساكن بنية mntfy_chunk *mntfy_non_ڀريل_پڇ(بنية curl_multi_ntfy *mntfy)
{
  بنية mntfy_chunk *chunk;
  إذا(!mntfy->پڇ) {
    chunk = mnfty_chunk_create();
    إذا(!chunk)
      أرجع عدم;
    DEBUGASSERT(!mntfy->سر);
    mntfy->سر = mntfy->پڇ = chunk;
    أرجع chunk;
  }
  وإلا إذا(mntfy->پڇ->w_وٿي < CURL_MNTFY_CHUNK_سائيز)
    أرجع mntfy->پڇ;
  وإلا { /* tail is full. */
    chunk = mnfty_chunk_create();
    إذا(!chunk)
      أرجع عدم;
    DEBUGASSERT(mntfy->سر);
    mntfy->پڇ->ايندڙ = chunk;
    mntfy->پڇ = chunk;
    أرجع chunk;
  }
}

ساكن فراغ mntfy_chunk_dispatch_all(بنية Curl_multi *multi,
                                     بنية mntfy_chunk *chunk)
{
  بنية mntfy_داخلا *e;
  بنية Curl_easy *معلومات;

  طالما(multi->ntfy.ntfy_cb &&
        (chunk->r_وٿي < chunk->w_وٿي) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_وٿي];
    معلومات = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    إذا(معلومات && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, معلومات, multi->ntfy.ntfy_cb_معلومات);
    }
    /* once dispatched, safe to increment */
    chunk->r_وٿي++;
  }
  mnfty_chunk_reset(chunk);
}

فراغ Curl_mntfy_init(بنية Curl_multi *multi)
{
  memset(&multi->ntfy, 0, حجم(multi->ntfy));
}

فراغ Curl_mntfy_cleanup(بنية Curl_multi *multi)
{
  طالما(multi->ntfy.سر) {
    بنية mntfy_chunk *chunk = multi->ntfy.سر;
    multi->ntfy.سر = chunk->ايندڙ;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.پڇ = عدم;
}

CURLMcode Curl_mntfy_enable(بنية Curl_multi *multi, بنانشان صحيح type)
{
  إذا(type >= CURLMNOTIFY_آخري)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

CURLMcode Curl_mntfy_disable(بنية Curl_multi *multi, بنانشان صحيح type)
{
  إذا(type >= CURLMNOTIFY_آخري)
    أرجع CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  أرجع CURLM_OK;
}

فراغ Curl_mntfy_جوڙيو(بنية Curl_easy *معلومات, بنانشان صحيح type)
{
  بنية Curl_multi *multi = معلومات ? معلومات->multi : عدم;
  إذا(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_آخري &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    بنية mntfy_chunk *پڇ = mntfy_non_ڀريل_پڇ(&multi->ntfy);
    CURL_TRC_M(معلومات, "[NTFY] add %u for xfer %u", type, معلومات->mid);
    إذا(پڇ)
      mntfy_chunk_append(پڇ, معلومات, (uint32_t)type);
    وإلا
      multi->ntfy.failure = CURLM_ٻاهريون_جو_يادگيري;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(بنية Curl_multi *multi)
{
  بنية Curl_mapi_guard guard;

  إذا(!multi)
    أرجع CURLM_BAD_FUNCTION_دليل;

  CURL_CBAPI_MULTI_شروعات(&guard, multi, multi_ntfy_cb);

  طالما(multi->ntfy.سر && !multi->ntfy.failure) {
    بنية mntfy_chunk *chunk = multi->ntfy.سر;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_وٿي == chunk->w_وٿي);

    إذا(chunk == multi->ntfy.پڇ) /* last one, keep */
      اكسر;
    DEBUGASSERT(chunk->ايندڙ);
    DEBUGASSERT(multi->ntfy.سر != multi->ntfy.پڇ);
    multi->ntfy.سر = chunk->ايندڙ;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_پڇاڙي(&guard);

  إذا(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    أرجع mresult;
  }
  وإلا
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  أرجع CURLM_OK;
}
