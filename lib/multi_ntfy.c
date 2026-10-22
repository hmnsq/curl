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

#ಆದರೆ CURLMNOTIFY_ಕೊನೆಯ > 31
#ದೋಷ "notification types must fit below the curl_multi_ntfy high flag bit"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MNTFY_TYPE_FLAG(type)  ((uint32_t)1 << (type))

ರಚನೆ mntfy_ನಮೂದು {
  uint32_t mid;
  uint32_t type;
};

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MNTFY_CHUNK_ಗಾತ್ರ 128

ರಚನೆ mntfy_chunk {
  ರಚನೆ mntfy_chunk *ಮುಂದಿನ;
  size_t r_ಸ್ಥಾನಾಂತರ;
  size_t w_ಸ್ಥಾನಾಂತರ;
  ರಚನೆ mntfy_ನಮೂದು entries[CURL_MNTFY_CHUNK_ಗಾತ್ರ];
};

ಸ್ಥಿರ ರಚನೆ mntfy_chunk *mnfty_chunk_create(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ mntfy_chunk));
}

ಸ್ಥಿರ ಶೂನ್ಯ mnfty_chunk_destroy(ರಚನೆ mntfy_chunk *chunk)
{
  curlx_free(chunk);
}

ಸ್ಥಿರ ಶೂನ್ಯ mnfty_chunk_reset(ರಚನೆ mntfy_chunk *chunk)
{
  chunk->r_ಸ್ಥಾನಾಂತರ = chunk->w_ಸ್ಥಾನಾಂತರ = 0;
}

ಸ್ಥಿರ bool mntfy_chunk_append(ರಚನೆ mntfy_chunk *chunk,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               uint32_t type)
{
  ರಚನೆ mntfy_ನಮೂದು *e;

  ಆದರೆ(chunk->w_ಸ್ಥಾನಾಂತರ >= CURL_MNTFY_CHUNK_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ FALSE;
  e = &chunk->entries[chunk->w_ಸ್ಥಾನಾಂತರ++];
  e->mid = ದತ್ತಾಂಶ->mid;
  e->type = type;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ರಚನೆ mntfy_chunk *mntfy_non_ತುಂಬಿದ_ಬಾಲ(ರಚನೆ curl_multi_ntfy *mntfy)
{
  ರಚನೆ mntfy_chunk *chunk;
  ಆದರೆ(!mntfy->ಬಾಲ) {
    chunk = mnfty_chunk_create();
    ಆದರೆ(!chunk)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    DEBUGASSERT(!mntfy->ತಲೆ);
    mntfy->ತಲೆ = mntfy->ಬಾಲ = chunk;
    ಹಿಂತಿರುಗಿ chunk;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(mntfy->ಬಾಲ->w_ಸ್ಥಾನಾಂತರ < CURL_MNTFY_CHUNK_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ mntfy->ಬಾಲ;
  ಇಲ್ಲದಿದ್ದರೆ { /* tail is full. */
    chunk = mnfty_chunk_create();
    ಆದರೆ(!chunk)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    DEBUGASSERT(mntfy->ತಲೆ);
    mntfy->ಬಾಲ->ಮುಂದಿನ = chunk;
    mntfy->ಬಾಲ = chunk;
    ಹಿಂತಿರುಗಿ chunk;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ mntfy_chunk_dispatch_all(ರಚನೆ Curl_multi *multi,
                                     ರಚನೆ mntfy_chunk *chunk)
{
  ರಚನೆ mntfy_ನಮೂದು *e;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;

  ಆಗ(multi->ntfy.ntfy_cb &&
        (chunk->r_ಸ್ಥಾನಾಂತರ < chunk->w_ಸ್ಥಾನಾಂತರ) && !multi->ntfy.failure) {
    e = &chunk->entries[chunk->r_ಸ್ಥಾನಾಂತರ];
    ದತ್ತಾಂಶ = e->mid ? Curl_multi_get_easy(multi, e->mid) : multi->admin;
    /* only when notification has not been disabled in the meantime */
    ಆದರೆ(ದತ್ತಾಂಶ && (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(e->type))) {
      /* this may cause new notifications to be added! */
      CURL_TRC_M(multi->admin, "[NTFY] dispatch %u to xfer %u",
                 e->type, e->mid);
      multi->ntfy.ntfy_cb(multi, e->type, ದತ್ತಾಂಶ, multi->ntfy.ntfy_cb_ದತ್ತಾಂಶ);
    }
    /* once dispatched, safe to increment */
    chunk->r_ಸ್ಥಾನಾಂತರ++;
  }
  mnfty_chunk_reset(chunk);
}

ಶೂನ್ಯ Curl_mntfy_init(ರಚನೆ Curl_multi *multi)
{
  memset(&multi->ntfy, 0, ಗಾತ್ರ(multi->ntfy));
}

ಶೂನ್ಯ Curl_mntfy_cleanup(ರಚನೆ Curl_multi *multi)
{
  ಆಗ(multi->ntfy.ತಲೆ) {
    ರಚನೆ mntfy_chunk *chunk = multi->ntfy.ತಲೆ;
    multi->ntfy.ತಲೆ = chunk->ಮುಂದಿನ;
    mnfty_chunk_destroy(chunk);
  }
  multi->ntfy.ಬಾಲ = ಖಾಲಿ;
}

CURLMcode Curl_mntfy_enable(ರಚನೆ Curl_multi *multi, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type)
{
  ಆದರೆ(type >= CURLMNOTIFY_ಕೊನೆಯ)
    ಹಿಂತಿರುಗಿ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags |= CURL_MNTFY_TYPE_FLAG(type);
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

CURLMcode Curl_mntfy_disable(ರಚನೆ Curl_multi *multi, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type)
{
  ಆದರೆ(type >= CURLMNOTIFY_ಕೊನೆಯ)
    ಹಿಂತಿರುಗಿ CURLM_UNKNOWN_OPTION;
  multi->ntfy.flags &= ~CURL_MNTFY_TYPE_FLAG(type);
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಶೂನ್ಯ Curl_mntfy_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type)
{
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->multi : ಖಾಲಿ;
  ಆದರೆ(multi && multi->ntfy.ntfy_cb && !multi->ntfy.failure &&
     type < CURLMNOTIFY_ಕೊನೆಯ &&
     (multi->ntfy.flags & CURL_MNTFY_TYPE_FLAG(type))) {
    /* append to list of outstanding notifications */
    ರಚನೆ mntfy_chunk *ಬಾಲ = mntfy_non_ತುಂಬಿದ_ಬಾಲ(&multi->ntfy);
    CURL_TRC_M(ದತ್ತಾಂಶ, "[NTFY] add %u for xfer %u", type, ದತ್ತಾಂಶ->mid);
    ಆದರೆ(ಬಾಲ)
      mntfy_chunk_append(ಬಾಲ, ದತ್ತಾಂಶ, (uint32_t)type);
    ಇಲ್ಲದಿದ್ದರೆ
      multi->ntfy.failure = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    multi->ntfy.flags |= CURL_MNTFY_FLAG_HAS_ENTRIES;
  }
}

CURLMcode Curl_mntfy_dispatch_all(ರಚನೆ Curl_multi *multi)
{
  ರಚನೆ Curl_mapi_guard guard;

  ಆದರೆ(!multi)
    ಹಿಂತಿರುಗಿ CURLM_BAD_FUNCTION_ವಾದ;

  CURL_CBAPI_MULTI_ಆರಂಭ(&guard, multi, multi_ntfy_cb);

  ಆಗ(multi->ntfy.ತಲೆ && !multi->ntfy.failure) {
    ರಚನೆ mntfy_chunk *chunk = multi->ntfy.ತಲೆ;
    /* this may cause new notifications to be added! */
    mntfy_chunk_dispatch_all(multi, chunk);
    DEBUGASSERT(chunk->r_ಸ್ಥಾನಾಂತರ == chunk->w_ಸ್ಥಾನಾಂತರ);

    ಆದರೆ(chunk == multi->ntfy.ಬಾಲ) /* last one, keep */
      ನಿಲ್ಲಿಸು;
    DEBUGASSERT(chunk->ಮುಂದಿನ);
    DEBUGASSERT(multi->ntfy.ತಲೆ != multi->ntfy.ಬಾಲ);
    multi->ntfy.ತಲೆ = chunk->ಮುಂದಿನ;
    mnfty_chunk_destroy(chunk);
  }

  CURL_CBAPI_MULTI_ಕೊನೆ(&guard);

  ಆದರೆ(multi->ntfy.failure) {
    CURLMcode mresult = multi->ntfy.failure;
    multi->ntfy.failure = CURLM_OK; /* reset, once delivered */
    ಹಿಂತಿರುಗಿ mresult;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    multi->ntfy.flags &= ~CURL_MNTFY_FLAG_HAS_ENTRIES;
  ಹಿಂತಿರುಗಿ CURLM_OK;
}
