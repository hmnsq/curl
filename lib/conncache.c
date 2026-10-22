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


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  ಕುರು {                                                                  \
    ಯದಿ(c) {                                                             \
      ಯದಿ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } ಯಾವತ್(0)

#define CPOOL_UNLOCK(c, d)                                              \
  ಕುರು {                                                                  \
    ಯದಿ(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      ಯದಿ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } ಯಾವತ್(0)

/* A set of connection pool ids to the same destination. */
ಸಂರಚನಾ cpool_bundle {
  ಸಂರಚನಾ uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_unlink(ಸಂರಚನಾ cpool_bundle **pbundle)
{
  ಯದಿ(*pbundle) {
    ಸಂರಚನಾ cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ರಿಕ್ತ;
    ಯದಿ(bundle->refcount)
      bundle->refcount--;
    ಯದಿ(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_link(ಸಂರಚನಾ cpool_bundle **pdest,
                              ಸಂರಚನಾ cpool_bundle *src)
{
  ಯದಿ(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    ಯದಿ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ಸ್ಥಿರ ಸಂರಚನಾ cpool_bundle *cpool_create_bundle(ಶೂನ್ಯ)
{
  /* created with refcount == 1 */
  ಸಂರಚನಾ cpool_bundle *bundle = curlx_calloc(1, ಪರಿಮಾಣ(*bundle));
  ಯದಿ(!bundle)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  ಪ್ರತ್ಯಾಗಮ bundle;
}

/* Add a connection to a bundle */
ಸ್ಥಿರ bool cpool_bundle_add_id(ಸಂರಚನಾ cpool *cpool,
                                ಸಂರಚನಾ cpool_bundle *bundle,
                                ಧ್ರುವ ವರ್ಣ *destination,
                                uint32_t pool_id)
{
  ಯದಿ(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids) == 1) {
    /* first one added, register bundle */
    ಸಂರಚನಾ cpool_bundle *new_ref = ರಿಕ್ತ;
    cpool_bundle_link(&new_ref, bundle);
    ಯದಿ(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

/* Remove a connection from a bundle */
ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_remove_id(ಸಂರಚನಾ cpool *cpool,
                                   ಸಂರಚನಾ cpool_bundle *bundle,
                                   ಧ್ರುವ ವರ್ಣ *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  ಯದಿ(!Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_bundle_hash_dtor(ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

ಸ್ಥಿರ ಸಂರಚನಾ cpool_bundle *cpool_get_bundle(ಸಂರಚನಾ cpool *cpool,
                                             ಧ್ರುವ ವರ್ಣ *destination)
{
  ಸಂರಚನಾ cpool_bundle *bundle = ರಿಕ್ತ;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  ಪ್ರತ್ಯಾಗಮ bundle;
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_remove_conn(ಸಂರಚನಾ cpool *cpool,
                              ಸಂರಚನಾ connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  ಯದಿ(cpid != UINT32_MAX) {
    ಸಂರಚನಾ cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      ಸಂರಚನಾ connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    ಯದಿ(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_discard_conn(ಸಂರಚನಾ cpool *cpool,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               bool aborted)
{
  ಸಂರಚನಾ Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  ಯದಿ(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  ಯದಿ(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  ಯದಿ(aborted)
    done = TRUE;
  ಯದಿ(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  ಯದಿ(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  ಅನ್ಯಥಾ {
    ಸಂರಚನಾ Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);

    ಯದಿ(ಅಂಕೆ_conns < max_shutdowns)
      max_shutdowns -= ಅಂಕೆ_conns;
    ಅನ್ಯಥಾ ಯದಿ(max_shutdowns)
      max_shutdowns = 1;
    ಅನ್ಯಥಾ /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(ಅಂಕೆ_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

ಶೂನ್ಯ Curl_cpool_init(ಸಂರಚನಾ cpool *cpool,
                     ಸಂರಚನಾ Curl_share *share,
                     size_t size)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, size, CURL_HASH_TYPE_BYTES,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->in_shutdown = FALSE;
  cpool->locked = FALSE;
}

ಶೂನ್ಯ Curl_cpool_destroy(ಸಂರಚನಾ cpool *cpool, ಸಂರಚನಾ Curl_easy *admin)
{
  ಯದಿ(cpool && cpool->initialized && admin) {
    ಶೂನ್ಯ *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    ಯದಿ(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      ಸಂರಚನಾ Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      ಕುರು {
        ಸಂರಚನಾ connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } ಯಾವತ್(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ಸ್ಥಿರ ಸಂರಚನಾ cpool *cpool_get_instance(ಸಂರಚನಾ Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  ಯದಿ(CURL_SHARE_KEEP_CONNECT(data->share))
    ಪ್ರತ್ಯಾಗಮ &data->share->cpool;
  ಅನ್ಯಥಾ ಯದಿ(data->multi_easy)
    ಪ್ರತ್ಯಾಗಮ &data->multi_easy->cpool;
  ಅನ್ಯಥಾ ಯದಿ(data->multi)
    ಪ್ರತ್ಯಾಗಮ &data->multi->cpool;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸಂರಚನಾ cpool *Curl_cpool_get_instance(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ cpool_get_instance(data);
}

ಶೂನ್ಯ Curl_cpool_xfer_init(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);

  ಯದಿ(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    ಯದಿ(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  ಅನ್ಯಥಾ {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

ಸ್ಥಿರ bool cpool_conn_can_be_closed(ಸಂರಚನಾ connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  ಪ್ರತ್ಯಾಗಮ (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

ಸ್ಥಿರ ಸಂರಚನಾ connectdata *cpool_bundle_get_oldest_idle(
  ಸಂರಚನಾ cpool *cpool,
  ಸಂರಚನಾ cpool_bundle *bundle,
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  ಯದಿ(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    ಕುರು {
      ಸಂರಚನಾ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ಯದಿ(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ಯದಿ(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } ಯಾವತ್(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ಸ್ಥಿರ ಸಂರಚನಾ connectdata *cpool_get_oldest_idle(ಸಂರಚನಾ cpool *cpool,
                                                 ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  ಯದಿ(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    ಕುರು {
      ಸಂರಚನಾ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ಯದಿ(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ಯದಿ((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      ಅನ್ಯಥಾ {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } ಯಾವತ್(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_conn_close(ಸಂರಚನಾ cpool *cpool,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn,
                             bool aborted)
{
  ಸಂರಚನಾ Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  ಯದಿ(!cpool)
    ಪ್ರತ್ಯಾಗಮ;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  ಯದಿ(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    ಪ್ರತ್ಯಾಗಮ;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  ಯದಿ(do_lock)
    CPOOL_LOCK(cpool, admin);

  ಯದಿ(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  ಯದಿ(conn->bits.connect_only)
    aborted = TRUE;

  ಯದಿ(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  ಅನ್ಯಥಾ {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  ಯದಿ(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

ಶೂನ್ಯ Curl_conn_close(ಸಂರಚನಾ Curl_easy *data,
                     ಸಂರಚನಾ connectdata *conn,
                     bool aborted)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ಸ್ಥಿರ ಶೂನ್ಯ cpool_evict_conn(ಸಂರಚನಾ cpool *cpool,
                             ಸಂರಚನಾ Curl_easy *admin,
                             ಸಂರಚನಾ connectdata *conn)
{
  ಯದಿ(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  ಅನ್ಯಥಾ
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cpool_check_limits(ಸಂರಚನಾ Curl_easy *data,
                              uint32_t max_total,
                              ಸಂರಚನಾ connectdata *to_add,
                              uint32_t max_host,
                              ಸಂರಚನಾ cpool_bundle *bundle,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);
  ಸಂರಚನಾ cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_easy *admin = ರಿಕ್ತ;

  ಯದಿ(!cpool)
    ಪ್ರತ್ಯಾಗಮ CPOOL_LIMIT_OK;

  ಯದಿ(!max_total && !max_host)
    ಪ್ರತ್ಯಾಗಮ CPOOL_LIMIT_OK;

  ಯದಿ(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    ಯದಿ(bundle) {
      live = Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids);
      ಯದಿ(live >= max_host) {
        size_t over = live - max_host + 1;
        ಪ್ರತಿ(; over; --over) {
          ಸಂರಚನಾ connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          ಯದಿ(!oldest_idle)
            ಭಙ್ಗ;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_ಎಣಿಕೆ(&bundle->ids);
      }
    }
    ಯದಿ(cshutdn) {
      shutdowns = Curl_cshutdn_dest_ಎಣಿಕೆ(cshutdn, to_add->destination);
      ಯದಿ(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    ಯದಿ((live + shutdowns) >= max_host)
      ಪ್ರತ್ಯಾಗಮ CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  ಯದಿ(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);

    ಯದಿ(!admin)
      admin = Curl_get_admin(data);
    ಯದಿ(ಅಂಕೆ_conns >= max_total) {
      size_t over = ಅಂಕೆ_conns - max_total + 1;
      ಪ್ರತಿ(; over; --over) {
        ಸಂರಚನಾ connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        ಯದಿ(!oldest_idle)
          ಭಙ್ಗ;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);
    ಯದಿ(cshutdn) {
      shutdowns = Curl_cshutdn_ಎಣಿಕೆ(cshutdn);
      ಯದಿ(shutdowns && (ಅಂಕೆ_conns + shutdowns) >= max_total) {
        size_t over = ಅಂಕೆ_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ರಿಕ್ತ, over);
      }
    }
    ಯದಿ((ಅಂಕೆ_conns + shutdowns) >= max_total)
      ಪ್ರತ್ಯಾಗಮ CPOOL_LIMIT_TOTAL;
  }

  ಪ್ರತ್ಯಾಗಮ CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cpool_bundle *bundle = ರಿಕ್ತ;
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  ಯದಿ(!cpool)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  ವಿಕಲ್ಪ(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  ಪಕ್ಷ CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    ಗಚ್ಛ out;
  ಪಕ್ಷ CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    ಗಚ್ಛ out;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  ಯದಿ(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    ಗಚ್ಛ out;
  }
  ಯದಿ(Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    ಯದಿ((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    ಅನ್ಯಥಾ
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    ಯದಿ(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(!bundle) {
    bundle = cpool_create_bundle();
    ಯದಿ(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  conn->cpid = cpid;

  ಯದಿ(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ bool cpool_foreach(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ cpool *cpool,
                          ಶೂನ್ಯ *param,
                          ಪೂರ್ಣಾಙ್ಕ (*func)(ಸಂರಚನಾ cpool *cpool,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn, ಶೂನ್ಯ *param))
{
  ಶೂನ್ಯ *entry;
  uint32_t cpid;

  ಯದಿ(!cpool)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    ಕುರು {
      ಸಂರಚನಾ connectdata *conn = entry;
      ಯದಿ(conn && func(cpool, data, conn, param) == 1)
        ಪ್ರತ್ಯಾಗಮ TRUE;
    } ಯಾವತ್(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ಸ್ಥಿರ bool cpool_conn_now_idle(ಸಂರಚನಾ cpool *cpool,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn,
                                ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ connectdata *oldest_idle = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  ಯದಿ(!data || !data->multi)
    ಪ್ರತ್ಯಾಗಮ kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  ಯದಿ(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  ಅನ್ಯಥಾ {
    maxconnects = data->multi->maxconnects;
  }

  ಯದಿ(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t ಅಂಕೆ_conns = Curl_uint32_tbl_ಎಣಿಕೆ(&cpool->conns);
    admin = Curl_get_admin(data);
    ಯದಿ(ಅಂಕೆ_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            ಅಂಕೆ_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      ಯದಿ(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ kept;
}

ಸ್ಥಿರ ಶೂನ್ಯ cpool_prune_dead(ಸಂರಚನಾ cpool *cpool,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);

ಸ್ಥಿರ bool cpool_find_act(ಸಂರಚನಾ cpool *cpool,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ connectdata *conn,
                           cpool_match_result match)
{
  ವಿಕಲ್ಪ(match) {
  ಪಕ್ಷ CPOOL_MATCH_FOUND:
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪಕ್ಷ CPOOL_MATCH_TOO_OLD:
    ಯದಿ(!cpool_conn_can_be_closed(conn))
      ಪ್ರತ್ಯಾಗಮ FALSE;
    FALLTHROUGH();
  ಪಕ್ಷ CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_cpool_find(ಸಂರಚನಾ Curl_easy *data,
                     ಧ್ರುವ ವರ್ಣ *destination,
                     bool prune_dead,
                     ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     ಶೂನ್ಯ *userdata)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);
  ಸಂರಚನಾ cpool_bundle *bundle = ರಿಕ್ತ;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  ಯದಿ(!cpool)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  CPOOL_LOCK(cpool, data);

  ಯದಿ(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  ಯದಿ(data->state.last_cpid != UINT32_MAX) {
    ಸಂರಚನಾ connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    ಯದಿ(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      ಯದಿ(found)
        ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  ಯದಿ(bundle) {
    uint32_t cpid;
    ಯದಿ(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      ಕುರು {
        ಸಂರಚನಾ connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        ಯದಿ(conn) {
          ಯದಿ(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            ಯದಿ(found)
              ಗಚ್ಛ out;
          }
        }
        ಅನ್ಯಥಾ {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } ಯಾವತ್(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  ಯದಿ(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  ಪ್ರತ್ಯಾಗಮ found;
}

ಸಂರಚನಾ cpool_reaper_ctx {
  size_t reaped;
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cpool_reap_dead_cb(ಸಂರಚನಾ cpool *cpool,
                              ಸಂರಚನಾ Curl_easy *admin,
                              ಸಂರಚನಾ connectdata *conn, ಶೂನ್ಯ *param)
{
  ಸಂರಚನಾ cpool_reaper_ctx *reaper = param;

  ಯದಿ(cpool_conn_can_be_closed(conn)) {
    ಯದಿ(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ಸ್ಥಿರ ಶೂನ್ಯ cpool_prune_dead(ಸಂರಚನಾ cpool *cpool,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  ಯದಿ(elapsed_ms >= 1000L) {
    ಸಂರಚನಾ Curl_easy *admin = Curl_get_admin(data);
    ಸಂರಚನಾ cpool_reaper_ctx reaper;

    memset(&reaper, 0, ಪರಿಮಾಣ(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conn_upkeep(ಸಂರಚನಾ cpool *cpool,
                       ಸಂರಚನಾ Curl_easy *admin,
                       ಸಂರಚನಾ connectdata *conn,
                       ಶೂನ್ಯ *param)
{
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(admin);

  (ಶೂನ್ಯ)param;
  ಯದಿ((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    ಯದಿ(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);
  ಸಂರಚನಾ Curl_easy *admin = Curl_get_admin(data);

  ಯದಿ(!cpool)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, ರಿಕ್ತ, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ connectdata *Curl_cpool_get_last_conn(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);
  ಯದಿ(cpool && (data->state.last_cpid != UINT32_MAX)) {
    ಸಂರಚನಾ connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    ಯದಿ(conn && (data->state.last_conn_id == conn->connection_id))
      ಪ್ರತ್ಯಾಗಮ conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ Curl_cpool_return(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ connectdata *conn,
                       Curl_cpool_return_cb *cb, ಶೂನ್ಯ *cbdata,
                       ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  ವಿಕಲ್ಪ(cb(data, conn, cbdata, pnow)) {
  ಪಕ್ಷ CPOOL_DO_KEEP:
    ಭಙ್ಗ;
  ಪಕ್ಷ CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    ಯದಿ(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    ಅನ್ಯಥಾ { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    ಭಙ್ಗ;
  ಪಕ್ಷ CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    ಭಙ್ಗ;
  }

  CPOOL_UNLOCK(cpool, data);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cpool_reap_no_reuse(ಸಂರಚನಾ cpool *cpool,
                               ಸಂರಚನಾ Curl_easy *admin,
                               ಸಂರಚನಾ connectdata *conn, ಶೂನ್ಯ *param)
{
  (ಶೂನ್ಯ)param;
  conn->bits.no_reuse = TRUE;
  ಯದಿ(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  ಪ್ರತ್ಯಾಗಮ 0; /* continue iteration */
}

ಶೂನ್ಯ Curl_cpool_nw_changed(ಸಂರಚನಾ cpool *cpool, ಸಂರಚನಾ Curl_easy *admin)
{
  ಯದಿ(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, ರಿಕ್ತ, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ಸ್ಥಿರ bool cpool_conn_too_old(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  timediff_t age_ms;

  ಯದಿ(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    ಯದಿ(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }

  ಯದಿ(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    ಯದಿ(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_cpool_conn_seems_healthy(ಸಂರಚನಾ connectdata *conn,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_ಒಳಹರಿವು;

  DEBUGASSERT(!data->conn);
  ಯದಿ(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_ಒಳಹರಿವು = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  ಯದಿ(!poll_ಒಳಹರಿವು &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    ಪ್ರತ್ಯಾಗಮ TRUE;

  admin = Curl_get_admin(data);
  ಯದಿ(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  ಅನ್ಯಥಾ {
    bool ಒಳಹರಿವು_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &ಒಳಹರಿವು_pending);
    Curl_detach_connection(admin);
    ಯದಿ(healthy && ಒಳಹರಿವು_pending && poll_ಒಳಹರಿವು) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  ಯದಿ(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  ಪ್ರತ್ಯಾಗಮ healthy;
}

ಶೂನ್ಯ Curl_cpool_conn_was_used(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  (ಶೂನ್ಯ)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ curlx_ptimediff_ms(pnow, &conn->created);
}
