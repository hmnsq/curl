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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cshutdn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sigpipe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-spbset.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPOOL_ਤਾਲਾ(c, d)                                                \
  ਕਰੋ {                                                                  \
    ਜੇ(c) {                                                             \
      ਜੇ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_ਤਾਲਾ_share((c)->share, (d), CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT,  \
                        CURL_ਤਾਲਾ_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPOOL_UNLOCK(c, d)                                              \
  ਕਰੋ {                                                                  \
    ਜੇ(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      ਜੇ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT); \
    }                                                                   \
  } ਜਦੋਂ(0)

/* A set of connection pool ids to the same destination. */
ਬਣਤਰ cpool_bundle {
  ਬਣਤਰ uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ਸਥਿਰ ਖਾਲੀ cpool_bundle_unlink(ਬਣਤਰ cpool_bundle **pbundle)
{
  ਜੇ(*pbundle) {
    ਬਣਤਰ cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ਨਿਰਾ;
    ਜੇ(bundle->refcount)
      bundle->refcount--;
    ਜੇ(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ਸਥਿਰ ਖਾਲੀ cpool_bundle_link(ਬਣਤਰ cpool_bundle **pdest,
                              ਬਣਤਰ cpool_bundle *src)
{
  ਜੇ(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    ਜੇ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ਸਥਿਰ ਬਣਤਰ cpool_bundle *cpool_create_bundle(ਖਾਲੀ)
{
  /* created with refcount == 1 */
  ਬਣਤਰ cpool_bundle *bundle = curlx_calloc(1, ਆਕਾਰ(*bundle));
  ਜੇ(!bundle)
    ਵਾਪਸ ਨਿਰਾ;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  ਵਾਪਸ bundle;
}

/* Add a connection to a bundle */
ਸਥਿਰ bool cpool_bundle_ਜੋੜੋ_id(ਬਣਤਰ cpool *cpool,
                                ਬਣਤਰ cpool_bundle *bundle,
                                ਅਚਲ ਅੱਖਰ *destination,
                                uint32_t pool_id)
{
  ਜੇ(!Curl_uint32_spbset_ਜੋੜੋ(&bundle->ids, pool_id))
    ਵਾਪਸ FALSE;
  ਜੇ(Curl_uint32_spbset_ਗਿਣਤੀ(&bundle->ids) == 1) {
    /* first one added, register bundle */
    ਬਣਤਰ cpool_bundle *new_ref = ਨਿਰਾ;
    cpool_bundle_link(&new_ref, bundle);
    ਜੇ(!Curl_hash_ਜੋੜੋ(&cpool->dest2bundle,
                      destination, ਲੜੀ_ਲੰਬਾਈ(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      ਵਾਪਸ FALSE;
    }
  }
  ਵਾਪਸ TRUE;
}

/* Remove a connection from a bundle */
ਸਥਿਰ ਖਾਲੀ cpool_bundle_remove_id(ਬਣਤਰ cpool *cpool,
                                   ਬਣਤਰ cpool_bundle *bundle,
                                   ਅਚਲ ਅੱਖਰ *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  ਜੇ(!Curl_uint32_spbset_ਗਿਣਤੀ(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, ਲੜੀ_ਲੰਬਾਈ(destination));
  }
}

ਸਥਿਰ ਖਾਲੀ cpool_bundle_hash_dtor(ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ cpool_bundle *bundle = ਇੰਦਰਾਜ;
  cpool_bundle_unlink(&bundle);
}

ਸਥਿਰ ਬਣਤਰ cpool_bundle *cpool_get_bundle(ਬਣਤਰ cpool *cpool,
                                             ਅਚਲ ਅੱਖਰ *destination)
{
  ਬਣਤਰ cpool_bundle *bundle = ਨਿਰਾ;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, ਲੜੀ_ਲੰਬਾਈ(destination)));
  ਵਾਪਸ bundle;
}

ਸਥਿਰ ਖਾਲੀ cpool_remove_conn(ਬਣਤਰ cpool *cpool,
                              ਬਣਤਰ connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  ਜੇ(cpid != UINT32_MAX) {
    ਬਣਤਰ cpool_bundle *bundle;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    {
      ਬਣਤਰ connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#ਜੇ_ਅੰਤ
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    ਜੇ(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ਸਥਿਰ ਖਾਲੀ cpool_discard_conn(ਬਣਤਰ cpool *cpool,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               bool aborted)
{
  ਬਣਤਰ Curl_easy *admin;
  bool ਪੂਰਾ = FALSE;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(!ਅੰਕੜੇ->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(ਅੰਕੜੇ);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  ਜੇ(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  ਜੇ(conn->ਬਿੱਟ.connect_only)
    aborted = TRUE;
  conn->ਬਿੱਟ.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  ਜੇ(aborted)
    ਪੂਰਾ = TRUE;
  ਜੇ(!ਪੂਰਾ) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &ਪੂਰਾ);
  }

  ਜੇ(ਪੂਰਾ || !ਅੰਕੜੇ->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
    size_t max_shutdowns = multi->max_ਕੁੱਲ_connections;
    uint32_t ਅੰਕ_conns = Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns);

    ਜੇ(ਅੰਕ_conns < max_shutdowns)
      max_shutdowns -= ਅੰਕ_conns;
    ਨਹੀਂਤਾਂ ਜੇ(max_shutdowns)
      max_shutdowns = 1;
    ਨਹੀਂਤਾਂ /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(ਅੰਕ_conns / 4, 128);
    Curl_cshutdn_ਜੋੜੋ(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

ਖਾਲੀ Curl_cpool_init(ਬਣਤਰ cpool *cpool,
                     ਬਣਤਰ Curl_share *share,
                     size_t ਮਾਪ)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, ਮਾਪ, CURL_HASH_TYPE_ਬਾਈਟਾਂ,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->ਵਿੱਚ_shutdown = FALSE;
  cpool->locked = FALSE;
}

ਖਾਲੀ Curl_cpool_destroy(ਬਣਤਰ cpool *cpool, ਬਣਤਰ Curl_easy *admin)
{
  ਜੇ(cpool && cpool->initialized && admin) {
    ਖਾਲੀ *ਇੰਦਰਾਜ;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_ਤਾਲਾ(cpool, admin);
    ਜੇ(Curl_uint32_tbl_ਪਹਿਲਾ(&cpool->conns, &cpid, &ਇੰਦਰਾਜ)) {
      ਬਣਤਰ Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      ਕਰੋ {
        ਬਣਤਰ connectdata *conn = ਇੰਦਰਾਜ;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } ਜਦੋਂ(Curl_uint32_tbl_ਅਗਲਾ(&cpool->conns, cpid, &cpid, &ਇੰਦਰਾਜ));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ਸਥਿਰ ਬਣਤਰ cpool *cpool_get_instance(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(ਅੰਕੜੇ->mid);
  ਜੇ(CURL_SHARE_KEEP_CONNECT(ਅੰਕੜੇ->share))
    ਵਾਪਸ &ਅੰਕੜੇ->share->cpool;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->multi_easy)
    ਵਾਪਸ &ਅੰਕੜੇ->multi_easy->cpool;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->multi)
    ਵਾਪਸ &ਅੰਕੜੇ->multi->cpool;
  ਵਾਪਸ ਨਿਰਾ;
}

ਬਣਤਰ cpool *Curl_cpool_get_instance(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ cpool_get_instance(ਅੰਕੜੇ);
}

ਖਾਲੀ Curl_cpool_xfer_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);

  ਜੇ(cpool) {
    CPOOL_ਤਾਲਾ(cpool, ਅੰਕੜੇ);
    /* the identifier inside the connection cache */
    ਅੰਕੜੇ->id = cpool->ਅਗਲਾ_easy_id++;
    ਜੇ(cpool->ਅਗਲਾ_easy_id == CURL_OFF_T_MAX)
      cpool->ਅਗਲਾ_easy_id = 0;
    ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
    ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    ਅੰਕੜੇ->id = 0;
    ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
    ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
  }
}

ਸਥਿਰ bool cpool_conn_can_be_closed(ਬਣਤਰ connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  ਵਾਪਸ (conn && !CONN_INUSE(conn) && !conn->ਬਿੱਟ.connect_only);
}

ਸਥਿਰ ਬਣਤਰ connectdata *cpool_bundle_get_oldest_idle(
  ਬਣਤਰ cpool *cpool,
  ਬਣਤਰ cpool_bundle *bundle,
  ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t ਅਣਵਰਤਿਆ_ms;
  timediff_t oldest_ms = -1;

  ਜੇ(Curl_uint32_spbset_ਪਹਿਲਾ(&bundle->ids, &cpid)) {
    ਕਰੋ {
      ਬਣਤਰ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ਜੇ(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        ਅਣਵਰਤਿਆ_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ਜੇ(ਅਣਵਰਤਿਆ_ms > oldest_ms) {
          oldest_ms = ਅਣਵਰਤਿਆ_ms;
          oldest_id = cpid;
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } ਜਦੋਂ(Curl_uint32_spbset_ਅਗਲਾ(&bundle->ids, cpid, &cpid));
  }
  ਵਾਪਸ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ਸਥਿਰ ਬਣਤਰ connectdata *cpool_get_oldest_idle(ਬਣਤਰ cpool *cpool,
                                                 ਅਚਲ ਬਣਤਰ curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&cpool->idles, &cpid)) {
    ਕਰੋ {
      ਬਣਤਰ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ਜੇ(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ਜੇ((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      ਨਹੀਂਤਾਂ {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&cpool->idles, cpid, &cpid));
  }
  ਵਾਪਸ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ਸਥਿਰ ਖਾਲੀ cpool_conn_close(ਬਣਤਰ cpool *cpool,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn,
                             bool aborted)
{
  ਬਣਤਰ Curl_easy *admin;
  bool do_ਤਾਲਾ;

  DEBUGASSERT(cpool);
  DEBUGASSERT(ਅੰਕੜੇ && !ਅੰਕੜੇ->conn);
  ਜੇ(!cpool)
    ਵਾਪਸ;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  ਜੇ(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(ਅੰਕੜੇ, "conn terminate when inuse: %u", conn->attached_xfers));
    ਵਾਪਸ;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(ਅੰਕੜੇ);
  do_ਤਾਲਾ = !CPOOL_IS_LOCKED(cpool);
  ਜੇ(do_ਤਾਲਾ)
    CPOOL_ਤਾਲਾ(cpool, admin);

  ਜੇ(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  ਜੇ(conn->ਬਿੱਟ.connect_only)
    aborted = TRUE;

  ਜੇ(ਅੰਕੜੇ->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(ਅੰਕੜੇ, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&ਅੰਕੜੇ->multi->cpool, ਅੰਕੜੇ, conn, aborted);
  }
  ਨਹੀਂਤਾਂ {
    /* No multi available, terminate */
    infof(ਅੰਕੜੇ, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  ਜੇ(do_ਤਾਲਾ)
    CPOOL_UNLOCK(cpool, admin);
}

ਖਾਲੀ Curl_conn_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਬਣਤਰ connectdata *conn,
                     bool aborted)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);
  cpool_conn_close(cpool, ਅੰਕੜੇ, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ਸਥਿਰ ਖਾਲੀ cpool_evict_conn(ਬਣਤਰ cpool *cpool,
                             ਬਣਤਰ Curl_easy *admin,
                             ਬਣਤਰ connectdata *conn)
{
  ਜੇ(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  ਨਹੀਂਤਾਂ
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPOOL_ਹੱਦਬੰਦੀ_OK     0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPOOL_ਹੱਦਬੰਦੀ_DEST   1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPOOL_ਹੱਦਬੰਦੀ_ਕੁੱਲ  2

ਸਥਿਰ ਪੂਰਨ cpool_check_limits(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              uint32_t max_ਕੁੱਲ,
                              ਬਣਤਰ connectdata *ਮੰਜ਼ਿਲ_ਜੋੜੋ,
                              uint32_t max_host,
                              ਬਣਤਰ cpool_bundle *bundle,
                              ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);
  ਬਣਤਰ cshutdn *cshutdn = ਅੰਕੜੇ->multi ? &ਅੰਕੜੇ->multi->cshutdn : ਨਿਰਾ;
  ਬਣਤਰ Curl_easy *admin = ਨਿਰਾ;

  ਜੇ(!cpool)
    ਵਾਪਸ CPOOL_ਹੱਦਬੰਦੀ_OK;

  ਜੇ(!max_ਕੁੱਲ && !max_host)
    ਵਾਪਸ CPOOL_ਹੱਦਬੰਦੀ_OK;

  ਜੇ(max_host) {
    size_t ਜੀਵਤ = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(ਅੰਕੜੇ);
    ਜੇ(bundle) {
      ਜੀਵਤ = Curl_uint32_spbset_ਗਿਣਤੀ(&bundle->ids);
      ਜੇ(ਜੀਵਤ >= max_host) {
        size_t over = ਜੀਵਤ - max_host + 1;
        ਲਈ(; over; --over) {
          ਬਣਤਰ connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          ਜੇ(!oldest_idle)
            ਰੋਕੋ;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        ਜੀਵਤ = Curl_uint32_spbset_ਗਿਣਤੀ(&bundle->ids);
      }
    }
    ਜੇ(cshutdn) {
      shutdowns = Curl_cshutdn_dest_ਗਿਣਤੀ(cshutdn, ਮੰਜ਼ਿਲ_ਜੋੜੋ->destination);
      ਜੇ(shutdowns && ((ਜੀਵਤ + shutdowns) >= max_host)) {
        size_t over = ਜੀਵਤ + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               ਮੰਜ਼ਿਲ_ਜੋੜੋ->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    ਜੇ((ਜੀਵਤ + shutdowns) >= max_host)
      ਵਾਪਸ CPOOL_ਹੱਦਬੰਦੀ_DEST;
  }

  /* Internal transfers are outside the total limit */
  ਜੇ(max_ਕੁੱਲ && !ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ) {
    size_t shutdowns = 0;
    uint32_t ਅੰਕ_conns = Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns);

    ਜੇ(!admin)
      admin = Curl_get_admin(ਅੰਕੜੇ);
    ਜੇ(ਅੰਕ_conns >= max_ਕੁੱਲ) {
      size_t over = ਅੰਕ_conns - max_ਕੁੱਲ + 1;
      ਲਈ(; over; --over) {
        ਬਣਤਰ connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        ਜੇ(!oldest_idle)
          ਰੋਕੋ;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_ਕੁੱਲ);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    ਅੰਕ_conns = Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns);
    ਜੇ(cshutdn) {
      shutdowns = Curl_cshutdn_ਗਿਣਤੀ(cshutdn);
      ਜੇ(shutdowns && (ਅੰਕ_conns + shutdowns) >= max_ਕੁੱਲ) {
        size_t over = ਅੰਕ_conns + shutdowns - max_ਕੁੱਲ + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ਨਿਰਾ, over);
      }
    }
    ਜੇ((ਅੰਕ_conns + shutdowns) >= max_ਕੁੱਲ)
      ਵਾਪਸ CPOOL_ਹੱਦਬੰਦੀ_ਕੁੱਲ;
  }

  ਵਾਪਸ CPOOL_ਹੱਦਬੰਦੀ_OK;
}

CURLcode Curl_cpool_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ connectdata *conn,
                        uint32_t max_ਕੁੱਲ,
                        uint32_t max_host,
                        ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cpool_bundle *bundle = ਨਿਰਾ;
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);
  uint32_t ਸਮਰੱਥਾ, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  ਜੇ(!cpool)
    ਵਾਪਸ CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.ਸ਼ੁਰੂ_ms[FIRSTSOCKET] =
    conn->shutdown.ਸ਼ੁਰੂ_ms[SECONDARYSOCKET] = -1;

  CPOOL_ਤਾਲਾ(cpool, ਅੰਕੜੇ);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  ਚੋਣ(cpool_check_limits(ਅੰਕੜੇ, max_ਕੁੱਲ, conn, max_host, bundle, pnow)) {
  ਹਾਲਤ CPOOL_ਹੱਦਬੰਦੀ_DEST:
    infof(ਅੰਕੜੇ, "No more connections allowed to host");
    ਨਤੀਜਾ = CURLE_NO_CONNECTION_ਉਪਲਬਧ;
    ਜਾਓ ਬਾਹਰਲਾ;
  ਹਾਲਤ CPOOL_ਹੱਦਬੰਦੀ_ਕੁੱਲ:
    infof(ਅੰਕੜੇ, "No connections available, total of %u reached.",
          ਅੰਕੜੇ->multi->max_ਕੁੱਲ_connections);
    ਨਤੀਜਾ = CURLE_NO_CONNECTION_ਉਪਲਬਧ;
    ਜਾਓ ਬਾਹਰਲਾ;
  ਮੂਲ:
    ਰੋਕੋ;
  }

  /* We add the connection */
  ਸਮਰੱਥਾ = Curl_uint32_tbl_ਸਮਰੱਥਾ(&cpool->conns);
  ਜੇ(ਸਮਰੱਥਾ == UINT32_MAX) {
    infof(ਅੰਕੜੇ, "No connections available, pool at max capacity");
    ਨਤੀਜਾ = CURLE_NO_CONNECTION_ਉਪਲਬਧ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns) >= ਸਮਰੱਥਾ) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(ਸਮਰੱਥਾ, 16);
    ਜੇ((UINT32_MAX - growth) >= ਸਮਰੱਥਾ)
      ncapacity = ਸਮਰੱਥਾ + growth;
    ਨਹੀਂਤਾਂ
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    ਜੇ(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(!bundle) {
    bundle = cpool_create_bundle();
    ਜੇ(!bundle) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(!Curl_uint32_tbl_ਜੋੜੋ(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  conn->cpid = cpid;

  ਜੇ(!cpool_bundle_ਜੋੜੋ_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  conn->connection_id = cpool->ਅਗਲਾ_connection_id++;
  CURL_TRC_M(ਅੰਕੜੇ, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns));
ਬਾਹਰਲਾ:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, ਅੰਕੜੇ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ bool cpool_foreach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ cpool *cpool,
                          ਖਾਲੀ *param,
                          ਪੂਰਨ (*func)(ਬਣਤਰ cpool *cpool,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn, ਖਾਲੀ *param))
{
  ਖਾਲੀ *ਇੰਦਰਾਜ;
  uint32_t cpid;

  ਜੇ(!cpool)
    ਵਾਪਸ FALSE;

  ਜੇ(Curl_uint32_tbl_ਪਹਿਲਾ(&cpool->conns, &cpid, &ਇੰਦਰਾਜ)) {
    ਕਰੋ {
      ਬਣਤਰ connectdata *conn = ਇੰਦਰਾਜ;
      ਜੇ(conn && func(cpool, ਅੰਕੜੇ, conn, param) == 1)
        ਵਾਪਸ TRUE;
    } ਜਦੋਂ(Curl_uint32_tbl_ਅਗਲਾ(&cpool->conns, cpid, &cpid, &ਇੰਦਰਾਜ));
  }
  ਵਾਪਸ FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ਸਥਿਰ bool cpool_conn_now_idle(ਬਣਤਰ cpool *cpool,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn,
                                ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ connectdata *oldest_idle = ਨਿਰਾ;
  ਬਣਤਰ Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->multi)
    ਵਾਪਸ kept;

  Curl_uint32_bset_ਜੋੜੋ(&cpool->idles, conn->cpid);

  ਜੇ(!ਅੰਕੜੇ->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(ਅੰਕੜੇ->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  ਨਹੀਂਤਾਂ {
    maxconnects = ਅੰਕੜੇ->multi->maxconnects;
  }

  ਜੇ(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t ਅੰਕ_conns = Curl_uint32_tbl_ਗਿਣਤੀ(&cpool->conns);
    admin = Curl_get_admin(ਅੰਕੜੇ);
    ਜੇ(ਅੰਕ_conns > maxconnects) {
      infof(ਅੰਕੜੇ, "Connection pool is full, closing the oldest of %u/%u",
            ਅੰਕ_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      ਜੇ(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  ਵਾਪਸ kept;
}

ਸਥਿਰ ਖਾਲੀ cpool_prune_dead(ਬਣਤਰ cpool *cpool,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ curltime *pnow);

ਸਥਿਰ bool cpool_ਲੱਭੋ_act(ਬਣਤਰ cpool *cpool,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ connectdata *conn,
                           cpool_ਮੇਲ_ਨਤੀਜਾ ਮੇਲ)
{
  ਚੋਣ(ਮੇਲ) {
  ਹਾਲਤ CPOOL_ਮੇਲ_ਮਿਲਿਆ:
    ਵਾਪਸ TRUE;
  ਹਾਲਤ CPOOL_ਮੇਲ_TOO_OLD:
    ਜੇ(!cpool_conn_can_be_closed(conn))
      ਵਾਪਸ FALSE;
    FALLTHROUGH();
  ਹਾਲਤ CPOOL_ਮੇਲ_CLOSE:
    cpool_conn_close(cpool, ਅੰਕੜੇ, conn, FALSE);
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ FALSE;
}

bool Curl_cpool_ਲੱਭੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਅਚਲ ਅੱਖਰ *destination,
                     bool prune_dead,
                     ਅਚਲ ਬਣਤਰ curltime *pnow,
                     Curl_cpool_conn_ਮੇਲ_cb *conn_cb,
                     Curl_cpool_ਪੂਰਾ_ਮੇਲ_cb *ਪੂਰਾ_cb,
                     ਖਾਲੀ *userdata)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);
  ਬਣਤਰ cpool_bundle *bundle = ਨਿਰਾ;
  bool ਮਿਲਿਆ = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  ਜੇ(!cpool)
    ਵਾਪਸ FALSE;

  CPOOL_ਤਾਲਾ(cpool, ਅੰਕੜੇ);

  ਜੇ(prune_dead)
    cpool_prune_dead(cpool, ਅੰਕੜੇ, pnow);

  ਜੇ(ਅੰਕੜੇ->state.ਆਖਰੀ_cpid != UINT32_MAX) {
    ਬਣਤਰ connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, ਅੰਕੜੇ->state.ਆਖਰੀ_cpid);
    ਜੇ(conn && (ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      ਮਿਲਿਆ = cpool_ਲੱਭੋ_act(cpool, ਅੰਕੜੇ, conn, conn_cb(conn, userdata));
      ਜੇ(ਮਿਲਿਆ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      /* No longer found here. */
      ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
      ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  ਜੇ(bundle) {
    uint32_t cpid;
    ਜੇ(Curl_uint32_spbset_ਪਹਿਲਾ(&bundle->ids, &cpid)) {
      ਕਰੋ {
        ਬਣਤਰ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        ਜੇ(conn) {
          ਜੇ(ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id != conn->connection_id) {
            ਮਿਲਿਆ = cpool_ਲੱਭੋ_act(cpool, ਅੰਕੜੇ, conn, conn_cb(conn, userdata));
            ਜੇ(ਮਿਲਿਆ)
              ਜਾਓ ਬਾਹਰਲਾ;
          }
        }
        ਨਹੀਂਤਾਂ {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } ਜਦੋਂ(Curl_uint32_spbset_ਅਗਲਾ(&bundle->ids, cpid, &cpid));
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਪੂਰਾ_cb) {
    ਮਿਲਿਆ = ਪੂਰਾ_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, ਅੰਕੜੇ);
  ਵਾਪਸ ਮਿਲਿਆ;
}

ਬਣਤਰ cpool_reaper_ctx {
  size_t reaped;
  ਅਚਲ ਬਣਤਰ curltime *pnow;
};

ਸਥਿਰ ਪੂਰਨ cpool_reap_dead_cb(ਬਣਤਰ cpool *cpool,
                              ਬਣਤਰ Curl_easy *admin,
                              ਬਣਤਰ connectdata *conn, ਖਾਲੀ *param)
{
  ਬਣਤਰ cpool_reaper_ctx *reaper = param;

  ਜੇ(cpool_conn_can_be_closed(conn)) {
    ਜੇ(conn->ਬਿੱਟ.no_reuse || conn->ਬਿੱਟ.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ਵਾਪਸ 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ਸਥਿਰ ਖਾਲੀ cpool_prune_dead(ਬਣਤਰ cpool *cpool,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->ਆਖਰੀ_cleanup);

  ਜੇ(elapsed_ms >= 1000L) {
    ਬਣਤਰ Curl_easy *admin = Curl_get_admin(ਅੰਕੜੇ);
    ਬਣਤਰ cpool_reaper_ctx reaper;

    memset(&reaper, 0, ਆਕਾਰ(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->ਆਖਰੀ_cleanup = *pnow;
  }
}

ਸਥਿਰ ਪੂਰਨ conn_upkeep(ਬਣਤਰ cpool *cpool,
                       ਬਣਤਰ Curl_easy *admin,
                       ਬਣਤਰ connectdata *conn,
                       ਖਾਲੀ *param)
{
  ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(admin);

  (ਖਾਲੀ)param;
  ਜੇ((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->ਸੈੱਟ.upkeep_interval_ms) {
    CURLcode ਨਤੀਜਾ;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    ਨਤੀਜਾ = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    ਜੇ(ਨਤੀਜਾ && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ਵਾਪਸ 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);
  ਬਣਤਰ Curl_easy *admin = Curl_get_admin(ਅੰਕੜੇ);

  ਜੇ(!cpool)
    ਵਾਪਸ CURLE_OK;

  CPOOL_ਤਾਲਾ(cpool, admin);
  cpool_foreach(admin, cpool, ਨਿਰਾ, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ connectdata *Curl_cpool_get_ਆਖਰੀ_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);
  ਜੇ(cpool && (ਅੰਕੜੇ->state.ਆਖਰੀ_cpid != UINT32_MAX)) {
    ਬਣਤਰ connectdata *conn;

    CPOOL_ਤਾਲਾ(cpool, ਅੰਕੜੇ);
    conn = Curl_uint32_tbl_get(&cpool->conns, ਅੰਕੜੇ->state.ਆਖਰੀ_cpid);
    CPOOL_UNLOCK(cpool, ਅੰਕੜੇ);
    ਜੇ(conn && (ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id == conn->connection_id))
      ਵਾਪਸ conn;
    ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
    ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ Curl_cpool_return(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ connectdata *conn,
                       Curl_cpool_return_cb *cb, ਖਾਲੀ *cbdata,
                       ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ cpool *cpool = cpool_get_instance(ਅੰਕੜੇ);

  CPOOL_ਤਾਲਾ(cpool, ਅੰਕੜੇ);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  ਚੋਣ(cb(ਅੰਕੜੇ, conn, cbdata, pnow)) {
  ਹਾਲਤ CPOOL_DO_KEEP:
    ਰੋਕੋ;
  ਹਾਲਤ CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    ਜੇ(cpool_conn_now_idle(cpool, ਅੰਕੜੇ, conn, pnow)) {
      /* connection kept in the cpool */
      infof(ਅੰਕੜੇ, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->ਪੋਰਟ);
    }
    ਨਹੀਂਤਾਂ { /* connection was removed from the cpool and destroyed. */
      ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
      ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CPOOL_DO_CLOSE:
    Curl_conn_close(ਅੰਕੜੇ, conn, FALSE);
    ਰੋਕੋ;
  ਹਾਲਤ CPOOL_DO_TERMINATE:
    Curl_conn_close(ਅੰਕੜੇ, conn, TRUE);
    ਰੋਕੋ;
  }

  CPOOL_UNLOCK(cpool, ਅੰਕੜੇ);
}

ਸਥਿਰ ਪੂਰਨ cpool_reap_no_reuse(ਬਣਤਰ cpool *cpool,
                               ਬਣਤਰ Curl_easy *admin,
                               ਬਣਤਰ connectdata *conn, ਖਾਲੀ *param)
{
  (ਖਾਲੀ)param;
  conn->ਬਿੱਟ.no_reuse = TRUE;
  ਜੇ(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  ਵਾਪਸ 0; /* continue iteration */
}

ਖਾਲੀ Curl_cpool_nw_ਬਦਲਿਆ(ਬਣਤਰ cpool *cpool, ਬਣਤਰ Curl_easy *admin)
{
  ਜੇ(cpool && admin) {
    CPOOL_ਤਾਲਾ(cpool, admin);
    cpool_foreach(admin, cpool, ਨਿਰਾ, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ਸਥਿਰ bool cpool_conn_too_old(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  timediff_t age_ms;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    ਜੇ(age_ms > ਅੰਕੜੇ->ਸੈੱਟ.conn_max_idle_ms) {
      infof(ਅੰਕੜੇ, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, ਅੰਕੜੇ->ਸੈੱਟ.conn_max_idle_ms);
      ਵਾਪਸ TRUE;
    }
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    ਜੇ(age_ms > ਅੰਕੜੇ->ਸੈੱਟ.conn_max_age_ms) {
      infof(ਅੰਕੜੇ,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, ਅੰਕੜੇ->ਸੈੱਟ.conn_max_age_ms);
      ਵਾਪਸ TRUE;
    }
  }

  ਵਾਪਸ FALSE;
}

bool Curl_cpool_conn_seems_healthy(ਬਣਤਰ connectdata *conn,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_ਨਿਵੇਸ਼;

  DEBUGASSERT(!ਅੰਕੜੇ->conn);
  ਜੇ(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(ਅੰਕੜੇ, conn, pnow))
    ਵਾਪਸ FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_ਨਿਵੇਸ਼ = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->ਲੜੀਵਾਰ->connection_is_dead;
  ਜੇ(!poll_ਨਿਵੇਸ਼ &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    ਵਾਪਸ TRUE;

  admin = Curl_get_admin(ਅੰਕੜੇ);
  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->ਲੜੀਵਾਰ->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  ਨਹੀਂਤਾਂ {
    bool ਨਿਵੇਸ਼_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &ਨਿਵੇਸ਼_pending);
    Curl_detach_connection(admin);
    ਜੇ(healthy && ਨਿਵੇਸ਼_pending && poll_ਨਿਵੇਸ਼) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(ਅੰਕੜੇ, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  ਜੇ(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  ਵਾਪਸ healthy;
}

ਖਾਲੀ Curl_cpool_conn_was_ਵਰਤਿਆ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn,
                              ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ curlx_ptimediff_ms(pnow, &conn->created);
}
