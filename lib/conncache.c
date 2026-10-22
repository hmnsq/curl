/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Linus Nielsen Feltzing, <linus@haxx.se>
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
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cshutdn.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "sigpipe.h"
#ಅನ್ತರ್ಭಾವ "uint-spbset.h"


#ವ್ಯಾಖ್ಯಾನಿಸು CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#ವ್ಯಾಖ್ಯಾನಿಸು CPOOL_ಬೀಗ(c, d)                                                \
  ಮಾಡು {                                                                  \
    ಆದರೆ(c) {                                                             \
      ಆದರೆ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_ಬೀಗ_share((c)->share, (d), CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT,  \
                        CURL_ಬೀಗ_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CPOOL_UNLOCK(c, d)                                              \
  ಮಾಡು {                                                                  \
    ಆದರೆ(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      ಆದರೆ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT); \
    }                                                                   \
  } ಆಗ(0)

/* A set of connection pool ids to the same destination. */
ರಚನೆ cpool_bundle {
  ರಚನೆ uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_unlink(ರಚನೆ cpool_bundle **pbundle)
{
  ಆದರೆ(*pbundle) {
    ರಚನೆ cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ಖಾಲಿ;
    ಆದರೆ(bundle->refcount)
      bundle->refcount--;
    ಆದರೆ(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_link(ರಚನೆ cpool_bundle **pdest,
                              ರಚನೆ cpool_bundle *src)
{
  ಆದರೆ(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    ಆದರೆ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ಸ್ಥಿರ ರಚನೆ cpool_bundle *cpool_create_bundle(ಶೂನ್ಯ)
{
  /* created with refcount == 1 */
  ರಚನೆ cpool_bundle *bundle = curlx_calloc(1, ಗಾತ್ರ(*bundle));
  ಆದರೆ(!bundle)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  ಹಿಂತಿರುಗಿ bundle;
}

/* Add a connection to a bundle */
ಸ್ಥಿರ bool cpool_bundle_ಕೂಡಿಸು_id(ರಚನೆ cpool *cpool,
                                ರಚನೆ cpool_bundle *bundle,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                                uint32_t pool_id)
{
  ಆದರೆ(!Curl_uint32_spbset_ಕೂಡಿಸು(&bundle->ids, pool_id))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids) == 1) {
    /* first one added, register bundle */
    ರಚನೆ cpool_bundle *new_ref = ಖಾಲಿ;
    cpool_bundle_link(&new_ref, bundle);
    ಆದರೆ(!Curl_hash_ಕೂಡಿಸು(&cpool->dest2bundle,
                      destination, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

/* Remove a connection from a bundle */
ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_remove_id(ರಚನೆ cpool *cpool,
                                   ರಚನೆ cpool_bundle *bundle,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  ಆದರೆ(!Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(destination));
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_hash_dtor(ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ cpool_bundle *bundle = ನಮೂದು;
  cpool_bundle_unlink(&bundle);
}

ಸ್ಥಿರ ರಚನೆ cpool_bundle *cpool_get_bundle(ರಚನೆ cpool *cpool,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination)
{
  ರಚನೆ cpool_bundle *bundle = ಖಾಲಿ;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(destination)));
  ಹಿಂತಿರುಗಿ bundle;
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_remove_conn(ರಚನೆ cpool *cpool,
                              ರಚನೆ connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  ಆದರೆ(cpid != UINT32_MAX) {
    ರಚನೆ cpool_bundle *bundle;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    {
      ರಚನೆ connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#ಆದರೆ_ಅಂತ್ಯ
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    ಆದರೆ(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_discard_conn(ರಚನೆ cpool *cpool,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               bool aborted)
{
  ರಚನೆ Curl_easy *admin;
  bool ಮುಗಿದ = FALSE;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(!ದತ್ತಾಂಶ->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(ದತ್ತಾಂಶ);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  ಆದರೆ(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.connect_only)
    aborted = TRUE;
  conn->ಬಿಟ್ಟುಗಳು.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  ಆದರೆ(aborted)
    ಮುಗಿದ = TRUE;
  ಆದರೆ(!ಮುಗಿದ) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &ಮುಗಿದ);
  }

  ಆದರೆ(ಮುಗಿದ || !ದತ್ತಾಂಶ->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
    size_t max_shutdowns = multi->max_ಒಟ್ಟು_connections;
    uint32_t ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);

    ಆದರೆ(ಅಂಕೆ_conns < max_shutdowns)
      max_shutdowns -= ಅಂಕೆ_conns;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(max_shutdowns)
      max_shutdowns = 1;
    ಇಲ್ಲದಿದ್ದರೆ /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(ಅಂಕೆ_conns / 4, 128);
    Curl_cshutdn_ಕೂಡಿಸು(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

ಶೂನ್ಯ Curl_cpool_init(ರಚನೆ cpool *cpool,
                     ರಚನೆ Curl_share *share,
                     size_t ಗಾತ್ರ_2)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, ಗಾತ್ರ_2, CURL_HASH_TYPE_ಬೈಟುಗಳು,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->ಒಳ_shutdown = FALSE;
  cpool->locked = FALSE;
}

ಶೂನ್ಯ Curl_cpool_destroy(ರಚನೆ cpool *cpool, ರಚನೆ Curl_easy *admin)
{
  ಆದರೆ(cpool && cpool->initialized && admin) {
    ಶೂನ್ಯ *ನಮೂದು;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_ಬೀಗ(cpool, admin);
    ಆದರೆ(Curl_uint32_tbl_ಮೊದಲ(&cpool->conns, &cpid, &ನಮೂದು)) {
      ರಚನೆ Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      ಮಾಡು {
        ರಚನೆ connectdata *conn = ನಮೂದು;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } ಆಗ(Curl_uint32_tbl_ಮುಂದಿನ(&cpool->conns, cpid, &cpid, &ನಮೂದು));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ಸ್ಥಿರ ರಚನೆ cpool *cpool_get_instance(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(ದತ್ತಾಂಶ->mid);
  ಆದರೆ(CURL_SHARE_KEEP_CONNECT(ದತ್ತಾಂಶ->share))
    ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->share->cpool;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->multi_easy)
    ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->multi_easy->cpool;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->multi)
    ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->multi->cpool;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ರಚನೆ cpool *Curl_cpool_get_instance(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ cpool_get_instance(ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ Curl_cpool_xfer_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);

  ಆದರೆ(cpool) {
    CPOOL_ಬೀಗ(cpool, ದತ್ತಾಂಶ);
    /* the identifier inside the connection cache */
    ದತ್ತಾಂಶ->id = cpool->ಮುಂದಿನ_easy_id++;
    ಆದರೆ(cpool->ಮುಂದಿನ_easy_id == CURL_OFF_T_MAX)
      cpool->ಮುಂದಿನ_easy_id = 0;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    ದತ್ತಾಂಶ->id = 0;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
  }
}

ಸ್ಥಿರ bool cpool_conn_can_be_closed(ರಚನೆ connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  ಹಿಂತಿರುಗಿ (conn && !CONN_INUSE(conn) && !conn->ಬಿಟ್ಟುಗಳು.connect_only);
}

ಸ್ಥಿರ ರಚನೆ connectdata *cpool_bundle_get_oldest_idle(
  ರಚನೆ cpool *cpool,
  ರಚನೆ cpool_bundle *bundle,
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t ಬಳಸದ_ms;
  timediff_t oldest_ms = -1;

  ಆದರೆ(Curl_uint32_spbset_ಮೊದಲ(&bundle->ids, &cpid)) {
    ಮಾಡು {
      ರಚನೆ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ಆದರೆ(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        ಬಳಸದ_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ಆದರೆ(ಬಳಸದ_ms > oldest_ms) {
          oldest_ms = ಬಳಸದ_ms;
          oldest_id = cpid;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } ಆಗ(Curl_uint32_spbset_ಮುಂದಿನ(&bundle->ids, cpid, &cpid));
  }
  ಹಿಂತಿರುಗಿ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ಸ್ಥಿರ ರಚನೆ connectdata *cpool_get_oldest_idle(ರಚನೆ cpool *cpool,
                                                 ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&cpool->idles, &cpid)) {
    ಮಾಡು {
      ರಚನೆ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ಆದರೆ(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ಆದರೆ((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&cpool->idles, cpid, &cpid));
  }
  ಹಿಂತಿರುಗಿ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_conn_close(ರಚನೆ cpool *cpool,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn,
                             bool aborted)
{
  ರಚನೆ Curl_easy *admin;
  bool do_ಬೀಗ;

  DEBUGASSERT(cpool);
  DEBUGASSERT(ದತ್ತಾಂಶ && !ದತ್ತಾಂಶ->conn);
  ಆದರೆ(!cpool)
    ಹಿಂತಿರುಗಿ;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  ಆದರೆ(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(ದತ್ತಾಂಶ, "conn terminate when inuse: %u", conn->attached_xfers));
    ಹಿಂತಿರುಗಿ;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(ದತ್ತಾಂಶ);
  do_ಬೀಗ = !CPOOL_IS_LOCKED(cpool);
  ಆದರೆ(do_ಬೀಗ)
    CPOOL_ಬೀಗ(cpool, admin);

  ಆದರೆ(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.connect_only)
    aborted = TRUE;

  ಆದರೆ(ದತ್ತಾಂಶ->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(ದತ್ತಾಂಶ, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&ದತ್ತಾಂಶ->multi->cpool, ದತ್ತಾಂಶ, conn, aborted);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* No multi available, terminate */
    infof(ದತ್ತಾಂಶ, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  ಆದರೆ(do_ಬೀಗ)
    CPOOL_UNLOCK(cpool, admin);
}

ಶೂನ್ಯ Curl_conn_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ರಚನೆ connectdata *conn,
                     bool aborted)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);
  cpool_conn_close(cpool, ದತ್ತಾಂಶ, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ಸ್ಥಿರ ಶೂನ್ಯ cpool_evict_conn(ರಚನೆ cpool *cpool,
                             ರಚನೆ Curl_easy *admin,
                             ರಚನೆ connectdata *conn)
{
  ಆದರೆ(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#ವ್ಯಾಖ್ಯಾನಿಸು CPOOL_ಎಲ್ಲೆ_OK     0
#ವ್ಯಾಖ್ಯಾನಿಸು CPOOL_ಎಲ್ಲೆ_DEST   1
#ವ್ಯಾಖ್ಯಾನಿಸು CPOOL_ಎಲ್ಲೆ_ಒಟ್ಟು  2

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cpool_check_limits(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              uint32_t max_ಒಟ್ಟು,
                              ರಚನೆ connectdata *ಗಮ್ಯ_ಕೂಡಿಸು,
                              uint32_t max_host,
                              ರಚನೆ cpool_bundle *bundle,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);
  ರಚನೆ cshutdn *cshutdn = ದತ್ತಾಂಶ->multi ? &ದತ್ತಾಂಶ->multi->cshutdn : ಖಾಲಿ;
  ರಚನೆ Curl_easy *admin = ಖಾಲಿ;

  ಆದರೆ(!cpool)
    ಹಿಂತಿರುಗಿ CPOOL_ಎಲ್ಲೆ_OK;

  ಆದರೆ(!max_ಒಟ್ಟು && !max_host)
    ಹಿಂತಿರುಗಿ CPOOL_ಎಲ್ಲೆ_OK;

  ಆದರೆ(max_host) {
    size_t ಜೀವಂತ = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(ದತ್ತಾಂಶ);
    ಆದರೆ(bundle) {
      ಜೀವಂತ = Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids);
      ಆದರೆ(ಜೀವಂತ >= max_host) {
        size_t over = ಜೀವಂತ - max_host + 1;
        ಗಾಗಿ(; over; --over) {
          ರಚನೆ connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          ಆದರೆ(!oldest_idle)
            ನಿಲ್ಲಿಸು;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        ಜೀವಂತ = Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids);
      }
    }
    ಆದರೆ(cshutdn) {
      shutdowns = Curl_cshutdn_dest_ಎಣಿಕೆ(cshutdn, ಗಮ್ಯ_ಕೂಡಿಸು->destination);
      ಆದರೆ(shutdowns && ((ಜೀವಂತ + shutdowns) >= max_host)) {
        size_t over = ಜೀವಂತ + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               ಗಮ್ಯ_ಕೂಡಿಸು->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    ಆದರೆ((ಜೀವಂತ + shutdowns) >= max_host)
      ಹಿಂತಿರುಗಿ CPOOL_ಎಲ್ಲೆ_DEST;
  }

  /* Internal transfers are outside the total limit */
  ಆದರೆ(max_ಒಟ್ಟು && !ದತ್ತಾಂಶ->state.ಆಂತರಿಕ) {
    size_t shutdowns = 0;
    uint32_t ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);

    ಆದರೆ(!admin)
      admin = Curl_get_admin(ದತ್ತಾಂಶ);
    ಆದರೆ(ಅಂಕೆ_conns >= max_ಒಟ್ಟು) {
      size_t over = ಅಂಕೆ_conns - max_ಒಟ್ಟು + 1;
      ಗಾಗಿ(; over; --over) {
        ರಚನೆ connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        ಆದರೆ(!oldest_idle)
          ನಿಲ್ಲಿಸು;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_ಒಟ್ಟು);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);
    ಆದರೆ(cshutdn) {
      shutdowns = Curl_cshutdn_ಎಣಿಕೆ(cshutdn);
      ಆದರೆ(shutdowns && (ಅಂಕೆ_conns + shutdowns) >= max_ಒಟ್ಟು) {
        size_t over = ಅಂಕೆ_conns + shutdowns - max_ಒಟ್ಟು + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ಖಾಲಿ, over);
      }
    }
    ಆದರೆ((ಅಂಕೆ_conns + shutdowns) >= max_ಒಟ್ಟು)
      ಹಿಂತಿರುಗಿ CPOOL_ಎಲ್ಲೆ_ಒಟ್ಟು;
  }

  ಹಿಂತಿರುಗಿ CPOOL_ಎಲ್ಲೆ_OK;
}

CURLcode Curl_cpool_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ connectdata *conn,
                        uint32_t max_ಒಟ್ಟು,
                        uint32_t max_host,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cpool_bundle *bundle = ಖಾಲಿ;
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);
  uint32_t ಸಾಮರ್ಥ್ಯ, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  ಆದರೆ(!cpool)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.ಆರಂಭ_ms[FIRSTSOCKET] =
    conn->shutdown.ಆರಂಭ_ms[SECONDARYSOCKET] = -1;

  CPOOL_ಬೀಗ(cpool, ದತ್ತಾಂಶ);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  ಆಯ್ಕೆ(cpool_check_limits(ದತ್ತಾಂಶ, max_ಒಟ್ಟು, conn, max_host, bundle, pnow)) {
  ಸ್ಥಿತಿ CPOOL_ಎಲ್ಲೆ_DEST:
    infof(ದತ್ತಾಂಶ, "No more connections allowed to host");
    ಫಲಿತಾಂಶ = CURLE_NO_CONNECTION_ಲಭ್ಯ;
    ಹೋಗು ಹೊರಹೋಗು;
  ಸ್ಥಿತಿ CPOOL_ಎಲ್ಲೆ_ಒಟ್ಟು:
    infof(ದತ್ತಾಂಶ, "No connections available, total of %u reached.",
          ದತ್ತಾಂಶ->multi->max_ಒಟ್ಟು_connections);
    ಫಲಿತಾಂಶ = CURLE_NO_CONNECTION_ಲಭ್ಯ;
    ಹೋಗು ಹೊರಹೋಗು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  /* We add the connection */
  ಸಾಮರ್ಥ್ಯ = Curl_uint32_tbl_ಸಾಮರ್ಥ್ಯ(&cpool->conns);
  ಆದರೆ(ಸಾಮರ್ಥ್ಯ == UINT32_MAX) {
    infof(ದತ್ತಾಂಶ, "No connections available, pool at max capacity");
    ಫಲಿತಾಂಶ = CURLE_NO_CONNECTION_ಲಭ್ಯ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns) >= ಸಾಮರ್ಥ್ಯ) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(ಸಾಮರ್ಥ್ಯ, 16);
    ಆದರೆ((UINT32_MAX - growth) >= ಸಾಮರ್ಥ್ಯ)
      ncapacity = ಸಾಮರ್ಥ್ಯ + growth;
    ಇಲ್ಲದಿದ್ದರೆ
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    ಆದರೆ(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(!bundle) {
    bundle = cpool_create_bundle();
    ಆದರೆ(!bundle) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(!Curl_uint32_tbl_ಕೂಡಿಸು(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  conn->cpid = cpid;

  ಆದರೆ(!cpool_bundle_ಕೂಡಿಸು_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  conn->connection_id = cpool->ಮುಂದಿನ_connection_id++;
  CURL_TRC_M(ದತ್ತಾಂಶ, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns));
ಹೊರಹೋಗು:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* This function iterates the entire connection pool and calls the function
   func() with the connection pointer as the first argument and the supplied
   'param' argument as the other.

   The cpool lock is still held when the callback is called. It needs it,
   so that it can safely continue traversing the lists once the callback
   returns.

   Returns TRUE if the loop was aborted due to the callback's return code.

   Return 0 from func() to continue the loop, return 1 to abort it.
 */
ಸ್ಥಿರ bool cpool_foreach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ cpool *cpool,
                          ಶೂನ್ಯ *param,
                          ಪೂರ್ಣಾಂಕ (*func)(ರಚನೆ cpool *cpool,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn, ಶೂನ್ಯ *param))
{
  ಶೂನ್ಯ *ನಮೂದು;
  uint32_t cpid;

  ಆದರೆ(!cpool)
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(Curl_uint32_tbl_ಮೊದಲ(&cpool->conns, &cpid, &ನಮೂದು)) {
    ಮಾಡು {
      ರಚನೆ connectdata *conn = ನಮೂದು;
      ಆದರೆ(conn && func(cpool, ದತ್ತಾಂಶ, conn, param) == 1)
        ಹಿಂತಿರುಗಿ TRUE;
    } ಆಗ(Curl_uint32_tbl_ಮುಂದಿನ(&cpool->conns, cpid, &cpid, &ನಮೂದು));
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ಸ್ಥಿರ bool cpool_conn_now_idle(ರಚನೆ cpool *cpool,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ connectdata *oldest_idle = ಖಾಲಿ;
  ರಚನೆ Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->multi)
    ಹಿಂತಿರುಗಿ kept;

  Curl_uint32_bset_ಕೂಡಿಸು(&cpool->idles, conn->cpid);

  ಆದರೆ(!ದತ್ತಾಂಶ->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(ದತ್ತಾಂಶ->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    maxconnects = ದತ್ತಾಂಶ->multi->maxconnects;
  }

  ಆದರೆ(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);
    admin = Curl_get_admin(ದತ್ತಾಂಶ);
    ಆದರೆ(ಅಂಕೆ_conns > maxconnects) {
      infof(ದತ್ತಾಂಶ, "Connection pool is full, closing the oldest of %u/%u",
            ಅಂಕೆ_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      ಆದರೆ(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  ಹಿಂತಿರುಗಿ kept;
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_prune_dead(ರಚನೆ cpool *cpool,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

ಸ್ಥಿರ bool cpool_ಕಂಡುಹಿಡಿ_act(ರಚನೆ cpool *cpool,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ connectdata *conn,
                           cpool_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ ಹೊಂದಿಕೆ)
{
  ಆಯ್ಕೆ(ಹೊಂದಿಕೆ) {
  ಸ್ಥಿತಿ CPOOL_ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು:
    ಹಿಂತಿರುಗಿ TRUE;
  ಸ್ಥಿತಿ CPOOL_ಹೊಂದಿಕೆ_TOO_OLD:
    ಆದರೆ(!cpool_conn_can_be_closed(conn))
      ಹಿಂತಿರುಗಿ FALSE;
    FALLTHROUGH();
  ಸ್ಥಿತಿ CPOOL_ಹೊಂದಿಕೆ_CLOSE:
    cpool_conn_close(cpool, ದತ್ತಾಂಶ, conn, FALSE);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_cpool_ಕಂಡುಹಿಡಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                     bool prune_dead,
                     ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                     Curl_cpool_conn_ಹೊಂದಿಕೆ_cb *conn_cb,
                     Curl_cpool_ಮುಗಿದ_ಹೊಂದಿಕೆ_cb *ಮುಗಿದ_cb,
                     ಶೂನ್ಯ *userdata)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);
  ರಚನೆ cpool_bundle *bundle = ಖಾಲಿ;
  bool ಸಿಕ್ಕಿತು = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  ಆದರೆ(!cpool)
    ಹಿಂತಿರುಗಿ FALSE;

  CPOOL_ಬೀಗ(cpool, ದತ್ತಾಂಶ);

  ಆದರೆ(prune_dead)
    cpool_prune_dead(cpool, ದತ್ತಾಂಶ, pnow);

  ಆದರೆ(ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid != UINT32_MAX) {
    ರಚನೆ connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid);
    ಆದರೆ(conn && (ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      ಸಿಕ್ಕಿತು = cpool_ಕಂಡುಹಿಡಿ_act(cpool, ದತ್ತಾಂಶ, conn, conn_cb(conn, userdata));
      ಆದರೆ(ಸಿಕ್ಕಿತು)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* No longer found here. */
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  ಆದರೆ(bundle) {
    uint32_t cpid;
    ಆದರೆ(Curl_uint32_spbset_ಮೊದಲ(&bundle->ids, &cpid)) {
      ಮಾಡು {
        ರಚನೆ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        ಆದರೆ(conn) {
          ಆದರೆ(ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id != conn->connection_id) {
            ಸಿಕ್ಕಿತು = cpool_ಕಂಡುಹಿಡಿ_act(cpool, ದತ್ತಾಂಶ, conn, conn_cb(conn, userdata));
            ಆದರೆ(ಸಿಕ್ಕಿತು)
              ಹೋಗು ಹೊರಹೋಗು;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } ಆಗ(Curl_uint32_spbset_ಮುಂದಿನ(&bundle->ids, cpid, &cpid));
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಮುಗಿದ_cb) {
    ಸಿಕ್ಕಿತು = ಮುಗಿದ_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

ರಚನೆ cpool_reaper_ctx {
  size_t reaped;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cpool_reap_dead_cb(ರಚನೆ cpool *cpool,
                              ರಚನೆ Curl_easy *admin,
                              ರಚನೆ connectdata *conn, ಶೂನ್ಯ *param)
{
  ರಚನೆ cpool_reaper_ctx *reaper = param;

  ಆದರೆ(cpool_conn_can_be_closed(conn)) {
    ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.no_reuse || conn->ಬಿಟ್ಟುಗಳು.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ಹಿಂತಿರುಗಿ 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ಸ್ಥಿರ ಶೂನ್ಯ cpool_prune_dead(ರಚನೆ cpool *cpool,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->ಕೊನೆಯ_cleanup);

  ಆದರೆ(elapsed_ms >= 1000L) {
    ರಚನೆ Curl_easy *admin = Curl_get_admin(ದತ್ತಾಂಶ);
    ರಚನೆ cpool_reaper_ctx reaper;

    memset(&reaper, 0, ಗಾತ್ರ(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->ಕೊನೆಯ_cleanup = *pnow;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conn_upkeep(ರಚನೆ cpool *cpool,
                       ರಚನೆ Curl_easy *admin,
                       ರಚನೆ connectdata *conn,
                       ಶೂನ್ಯ *param)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(admin);

  (ಶೂನ್ಯ)param;
  ಆದರೆ((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->ಗಣ.upkeep_interval_ms) {
    CURLcode ಫಲಿತಾಂಶ;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    ಫಲಿತಾಂಶ = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    ಆದರೆ(ಫಲಿತಾಂಶ && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ಹಿಂತಿರುಗಿ 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);
  ರಚನೆ Curl_easy *admin = Curl_get_admin(ದತ್ತಾಂಶ);

  ಆದರೆ(!cpool)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  CPOOL_ಬೀಗ(cpool, admin);
  cpool_foreach(admin, cpool, ಖಾಲಿ, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ connectdata *Curl_cpool_get_ಕೊನೆಯ_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);
  ಆದರೆ(cpool && (ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid != UINT32_MAX)) {
    ರಚನೆ connectdata *conn;

    CPOOL_ಬೀಗ(cpool, ದತ್ತಾಂಶ);
    conn = Curl_uint32_tbl_get(&cpool->conns, ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid);
    CPOOL_UNLOCK(cpool, ದತ್ತಾಂಶ);
    ಆದರೆ(conn && (ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id == conn->connection_id))
      ಹಿಂತಿರುಗಿ conn;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ Curl_cpool_return(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ connectdata *conn,
                       Curl_cpool_return_cb *cb, ಶೂನ್ಯ *cbdata,
                       ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ cpool *cpool = cpool_get_instance(ದತ್ತಾಂಶ);

  CPOOL_ಬೀಗ(cpool, ದತ್ತಾಂಶ);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  ಆಯ್ಕೆ(cb(ದತ್ತಾಂಶ, conn, cbdata, pnow)) {
  ಸ್ಥಿತಿ CPOOL_DO_KEEP:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    ಆದರೆ(cpool_conn_now_idle(cpool, ದತ್ತಾಂಶ, conn, pnow)) {
      /* connection kept in the cpool */
      infof(ದತ್ತಾಂಶ, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->ಪೋರ್ಟ್);
    }
    ಇಲ್ಲದಿದ್ದರೆ { /* connection was removed from the cpool and destroyed. */
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CPOOL_DO_CLOSE:
    Curl_conn_close(ದತ್ತಾಂಶ, conn, FALSE);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CPOOL_DO_TERMINATE:
    Curl_conn_close(ದತ್ತಾಂಶ, conn, TRUE);
    ನಿಲ್ಲಿಸು;
  }

  CPOOL_UNLOCK(cpool, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cpool_reap_no_reuse(ರಚನೆ cpool *cpool,
                               ರಚನೆ Curl_easy *admin,
                               ರಚನೆ connectdata *conn, ಶೂನ್ಯ *param)
{
  (ಶೂನ್ಯ)param;
  conn->ಬಿಟ್ಟುಗಳು.no_reuse = TRUE;
  ಆದರೆ(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  ಹಿಂತಿರುಗಿ 0; /* continue iteration */
}

ಶೂನ್ಯ Curl_cpool_nw_ಬದಲಾಯಿತು(ರಚನೆ cpool *cpool, ರಚನೆ Curl_easy *admin)
{
  ಆದರೆ(cpool && admin) {
    CPOOL_ಬೀಗ(cpool, admin);
    cpool_foreach(admin, cpool, ಖಾಲಿ, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ಸ್ಥಿರ bool cpool_conn_too_old(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  timediff_t age_ms;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    ಆದರೆ(age_ms > ದತ್ತಾಂಶ->ಗಣ.conn_max_idle_ms) {
      infof(ದತ್ತಾಂಶ, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, ದತ್ತಾಂಶ->ಗಣ.conn_max_idle_ms);
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    ಆದರೆ(age_ms > ದತ್ತಾಂಶ->ಗಣ.conn_max_age_ms) {
      infof(ದತ್ತಾಂಶ,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, ದತ್ತಾಂಶ->ಗಣ.conn_max_age_ms);
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_cpool_conn_seems_healthy(ರಚನೆ connectdata *conn,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_ಒಳಹರಿವು;

  DEBUGASSERT(!ದತ್ತಾಂಶ->conn);
  ಆದರೆ(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(ದತ್ತಾಂಶ, conn, pnow))
    ಹಿಂತಿರುಗಿ FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_ಒಳಹರಿವು = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->ಸತತ->connection_is_dead;
  ಆದರೆ(!poll_ಒಳಹರಿವು &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    ಹಿಂತಿರುಗಿ TRUE;

  admin = Curl_get_admin(ದತ್ತಾಂಶ);
  ಆದರೆ(conn->scheme->ಸತತ->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->ಸತತ->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    bool ಒಳಹರಿವು_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &ಒಳಹರಿವು_pending);
    Curl_detach_connection(admin);
    ಆದರೆ(healthy && ಒಳಹರಿವು_pending && poll_ಒಳಹರಿವು) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(ದತ್ತಾಂಶ, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  ಆದರೆ(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  ಹಿಂತಿರುಗಿ healthy;
}

ಶೂನ್ಯ Curl_cpool_conn_was_ಬಳಸಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ curlx_ptimediff_ms(pnow, &conn->created);
}
