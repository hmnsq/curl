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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cshutdn.h"
#అన్తర్భావ "conncache.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "sigpipe.h"
#అన్తర్భావ "uint-spbset.h"


#నిర్వచించు CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#నిర్వచించు CPOOL_తాళం(c, d)                                                \
  చేయి {                                                                  \
    ఒకవేళ(c) {                                                             \
      ఒకవేళ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_తాళం_share((c)->share, (d), CURL_తాళం_దత్తాంశం_CONNECT,  \
                        CURL_తాళం_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } అయితే(0)

#నిర్వచించు CPOOL_UNLOCK(c, d)                                              \
  చేయి {                                                                  \
    ఒకవేళ(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      ఒకవేళ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_తాళం_దత్తాంశం_CONNECT); \
    }                                                                   \
  } అయితే(0)

/* A set of connection pool ids to the same destination. */
నిర్మాణం cpool_bundle {
  నిర్మాణం uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

స్థిర శూన్య cpool_bundle_unlink(నిర్మాణం cpool_bundle **pbundle)
{
  ఒకవేళ(*pbundle) {
    నిర్మాణం cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ఖాళీ;
    ఒకవేళ(bundle->refcount)
      bundle->refcount--;
    ఒకవేళ(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

స్థిర శూన్య cpool_bundle_link(నిర్మాణం cpool_bundle **pdest,
                              నిర్మాణం cpool_bundle *src)
{
  ఒకవేళ(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    ఒకవేళ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

స్థిర నిర్మాణం cpool_bundle *cpool_create_bundle(శూన్య)
{
  /* created with refcount == 1 */
  నిర్మాణం cpool_bundle *bundle = curlx_calloc(1, పరిమాణం(*bundle));
  ఒకవేళ(!bundle)
    తిరిగి ఖాళీ;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  తిరిగి bundle;
}

/* Add a connection to a bundle */
స్థిర bool cpool_bundle_కూడు_id(నిర్మాణం cpool *cpool,
                                నిర్మాణం cpool_bundle *bundle,
                                స్థిరరాశి అక్షరం *destination,
                                uint32_t pool_id)
{
  ఒకవేళ(!Curl_uint32_spbset_కూడు(&bundle->ids, pool_id))
    తిరిగి FALSE;
  ఒకవేళ(Curl_uint32_spbset_లెక్క(&bundle->ids) == 1) {
    /* first one added, register bundle */
    నిర్మాణం cpool_bundle *new_ref = ఖాళీ;
    cpool_bundle_link(&new_ref, bundle);
    ఒకవేళ(!Curl_hash_కూడు(&cpool->dest2bundle,
                      destination, అక్షరశ్రేణి_పొడవు(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      తిరిగి FALSE;
    }
  }
  తిరిగి TRUE;
}

/* Remove a connection from a bundle */
స్థిర శూన్య cpool_bundle_remove_id(నిర్మాణం cpool *cpool,
                                   నిర్మాణం cpool_bundle *bundle,
                                   స్థిరరాశి అక్షరం *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  ఒకవేళ(!Curl_uint32_spbset_లెక్క(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, అక్షరశ్రేణి_పొడవు(destination));
  }
}

స్థిర శూన్య cpool_bundle_hash_dtor(శూన్య *నమోదు_2)
{
  నిర్మాణం cpool_bundle *bundle = నమోదు_2;
  cpool_bundle_unlink(&bundle);
}

స్థిర నిర్మాణం cpool_bundle *cpool_get_bundle(నిర్మాణం cpool *cpool,
                                             స్థిరరాశి అక్షరం *destination)
{
  నిర్మాణం cpool_bundle *bundle = ఖాళీ;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, అక్షరశ్రేణి_పొడవు(destination)));
  తిరిగి bundle;
}

స్థిర శూన్య cpool_remove_conn(నిర్మాణం cpool *cpool,
                              నిర్మాణం connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  ఒకవేళ(cpid != UINT32_MAX) {
    నిర్మాణం cpool_bundle *bundle;

#నిర్వచితమైతే DEBUGBUILD
    {
      నిర్మాణం connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#అయితే_ముగింపు
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    ఒకవేళ(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

స్థిర శూన్య cpool_discard_conn(నిర్మాణం cpool *cpool,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               bool aborted)
{
  నిర్మాణం Curl_easy *admin;
  bool పూర్తి = FALSE;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(!దత్తాంశం->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(దత్తాంశం);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  ఒకవేళ(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    తిరిగి;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  ఒకవేళ(conn->బిట్లు.connect_only)
    aborted = TRUE;
  conn->బిట్లు.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  ఒకవేళ(aborted)
    పూర్తి = TRUE;
  ఒకవేళ(!పూర్తి) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &పూర్తి);
  }

  ఒకవేళ(పూర్తి || !దత్తాంశం->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  లేకపోతే {
    నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
    size_t max_shutdowns = multi->max_మొత్తం_connections;
    uint32_t అంకె_conns = Curl_uint32_tbl_లెక్క(&cpool->conns);

    ఒకవేళ(అంకె_conns < max_shutdowns)
      max_shutdowns -= అంకె_conns;
    లేకపోతే ఒకవేళ(max_shutdowns)
      max_shutdowns = 1;
    లేకపోతే /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(అంకె_conns / 4, 128);
    Curl_cshutdn_కూడు(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

శూన్య Curl_cpool_init(నిర్మాణం cpool *cpool,
                     నిర్మాణం Curl_share *share,
                     size_t కొలత)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, కొలత, CURL_HASH_TYPE_బైట్లు,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->లోపల_shutdown = FALSE;
  cpool->locked = FALSE;
}

శూన్య Curl_cpool_destroy(నిర్మాణం cpool *cpool, నిర్మాణం Curl_easy *admin)
{
  ఒకవేళ(cpool && cpool->initialized && admin) {
    శూన్య *నమోదు_2;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_లెక్క(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_తాళం(cpool, admin);
    ఒకవేళ(Curl_uint32_tbl_మొదటి(&cpool->conns, &cpid, &నమోదు_2)) {
      నిర్మాణం Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      చేయి {
        నిర్మాణం connectdata *conn = నమోదు_2;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } అయితే(Curl_uint32_tbl_తదుపరి(&cpool->conns, cpid, &cpid, &నమోదు_2));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

స్థిర నిర్మాణం cpool *cpool_get_instance(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(దత్తాంశం->mid);
  ఒకవేళ(CURL_SHARE_KEEP_CONNECT(దత్తాంశం->share))
    తిరిగి &దత్తాంశం->share->cpool;
  లేకపోతే ఒకవేళ(దత్తాంశం->multi_easy)
    తిరిగి &దత్తాంశం->multi_easy->cpool;
  లేకపోతే ఒకవేళ(దత్తాంశం->multi)
    తిరిగి &దత్తాంశం->multi->cpool;
  తిరిగి ఖాళీ;
}

నిర్మాణం cpool *Curl_cpool_get_instance(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి cpool_get_instance(దత్తాంశం);
}

శూన్య Curl_cpool_xfer_init(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);

  ఒకవేళ(cpool) {
    CPOOL_తాళం(cpool, దత్తాంశం);
    /* the identifier inside the connection cache */
    దత్తాంశం->id = cpool->తదుపరి_easy_id++;
    ఒకవేళ(cpool->తదుపరి_easy_id == CURL_OFF_T_MAX)
      cpool->తదుపరి_easy_id = 0;
    దత్తాంశం->state.చివరి_conn_id = -1;
    దత్తాంశం->state.చివరి_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, దత్తాంశం);
  }
  లేకపోతే {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    దత్తాంశం->id = 0;
    దత్తాంశం->state.చివరి_conn_id = -1;
    దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
  }
}

స్థిర bool cpool_conn_can_be_closed(నిర్మాణం connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  తిరిగి (conn && !CONN_INUSE(conn) && !conn->బిట్లు.connect_only);
}

స్థిర నిర్మాణం connectdata *cpool_bundle_get_oldest_idle(
  నిర్మాణం cpool *cpool,
  నిర్మాణం cpool_bundle *bundle,
  స్థిరరాశి నిర్మాణం curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t వాడని_ms;
  timediff_t oldest_ms = -1;

  ఒకవేళ(Curl_uint32_spbset_మొదటి(&bundle->ids, &cpid)) {
    చేయి {
      నిర్మాణం connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ఒకవేళ(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        వాడని_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ఒకవేళ(వాడని_ms > oldest_ms) {
          oldest_ms = వాడని_ms;
          oldest_id = cpid;
        }
      }
      లేకపోతే ఒకవేళ(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } అయితే(Curl_uint32_spbset_తదుపరి(&bundle->ids, cpid, &cpid));
  }
  తిరిగి Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

స్థిర నిర్మాణం connectdata *cpool_get_oldest_idle(నిర్మాణం cpool *cpool,
                                                 స్థిరరాశి నిర్మాణం curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  ఒకవేళ(Curl_uint32_bset_మొదటి(&cpool->idles, &cpid)) {
    చేయి {
      నిర్మాణం connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ఒకవేళ(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ఒకవేళ((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      లేకపోతే {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } అయితే(Curl_uint32_bset_తదుపరి(&cpool->idles, cpid, &cpid));
  }
  తిరిగి Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

స్థిర శూన్య cpool_conn_close(నిర్మాణం cpool *cpool,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn,
                             bool aborted)
{
  నిర్మాణం Curl_easy *admin;
  bool do_తాళం;

  DEBUGASSERT(cpool);
  DEBUGASSERT(దత్తాంశం && !దత్తాంశం->conn);
  ఒకవేళ(!cpool)
    తిరిగి;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  ఒకవేళ(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(దత్తాంశం, "conn terminate when inuse: %u", conn->attached_xfers));
    తిరిగి;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(దత్తాంశం);
  do_తాళం = !CPOOL_IS_LOCKED(cpool);
  ఒకవేళ(do_తాళం)
    CPOOL_తాళం(cpool, admin);

  ఒకవేళ(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  ఒకవేళ(conn->బిట్లు.connect_only)
    aborted = TRUE;

  ఒకవేళ(దత్తాంశం->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(దత్తాంశం, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&దత్తాంశం->multi->cpool, దత్తాంశం, conn, aborted);
  }
  లేకపోతే {
    /* No multi available, terminate */
    infof(దత్తాంశం, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  ఒకవేళ(do_తాళం)
    CPOOL_UNLOCK(cpool, admin);
}

శూన్య Curl_conn_close(నిర్మాణం Curl_easy *దత్తాంశం,
                     నిర్మాణం connectdata *conn,
                     bool aborted)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);
  cpool_conn_close(cpool, దత్తాంశం, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
స్థిర శూన్య cpool_evict_conn(నిర్మాణం cpool *cpool,
                             నిర్మాణం Curl_easy *admin,
                             నిర్మాణం connectdata *conn)
{
  ఒకవేళ(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  లేకపోతే
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#నిర్వచించు CPOOL_హద్దు_OK     0
#నిర్వచించు CPOOL_హద్దు_DEST   1
#నిర్వచించు CPOOL_హద్దు_మొత్తం  2

స్థిర పూర్ణాంకం cpool_check_limits(నిర్మాణం Curl_easy *దత్తాంశం,
                              uint32_t max_మొత్తం,
                              నిర్మాణం connectdata *గమ్యం_కూడు,
                              uint32_t max_host,
                              నిర్మాణం cpool_bundle *bundle,
                              స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);
  నిర్మాణం cshutdn *cshutdn = దత్తాంశం->multi ? &దత్తాంశం->multi->cshutdn : ఖాళీ;
  నిర్మాణం Curl_easy *admin = ఖాళీ;

  ఒకవేళ(!cpool)
    తిరిగి CPOOL_హద్దు_OK;

  ఒకవేళ(!max_మొత్తం && !max_host)
    తిరిగి CPOOL_హద్దు_OK;

  ఒకవేళ(max_host) {
    size_t సజీవ = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(దత్తాంశం);
    ఒకవేళ(bundle) {
      సజీవ = Curl_uint32_spbset_లెక్క(&bundle->ids);
      ఒకవేళ(సజీవ >= max_host) {
        size_t over = సజీవ - max_host + 1;
        కొరకు(; over; --over) {
          నిర్మాణం connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          ఒకవేళ(!oldest_idle)
            ఆపు;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        సజీవ = Curl_uint32_spbset_లెక్క(&bundle->ids);
      }
    }
    ఒకవేళ(cshutdn) {
      shutdowns = Curl_cshutdn_dest_లెక్క(cshutdn, గమ్యం_కూడు->destination);
      ఒకవేళ(shutdowns && ((సజీవ + shutdowns) >= max_host)) {
        size_t over = సజీవ + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               గమ్యం_కూడు->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    ఒకవేళ((సజీవ + shutdowns) >= max_host)
      తిరిగి CPOOL_హద్దు_DEST;
  }

  /* Internal transfers are outside the total limit */
  ఒకవేళ(max_మొత్తం && !దత్తాంశం->state.అంతర్గత) {
    size_t shutdowns = 0;
    uint32_t అంకె_conns = Curl_uint32_tbl_లెక్క(&cpool->conns);

    ఒకవేళ(!admin)
      admin = Curl_get_admin(దత్తాంశం);
    ఒకవేళ(అంకె_conns >= max_మొత్తం) {
      size_t over = అంకె_conns - max_మొత్తం + 1;
      కొరకు(; over; --over) {
        నిర్మాణం connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        ఒకవేళ(!oldest_idle)
          ఆపు;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_మొత్తం);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    అంకె_conns = Curl_uint32_tbl_లెక్క(&cpool->conns);
    ఒకవేళ(cshutdn) {
      shutdowns = Curl_cshutdn_లెక్క(cshutdn);
      ఒకవేళ(shutdowns && (అంకె_conns + shutdowns) >= max_మొత్తం) {
        size_t over = అంకె_conns + shutdowns - max_మొత్తం + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ఖాళీ, over);
      }
    }
    ఒకవేళ((అంకె_conns + shutdowns) >= max_మొత్తం)
      తిరిగి CPOOL_హద్దు_మొత్తం;
  }

  తిరిగి CPOOL_హద్దు_OK;
}

CURLcode Curl_cpool_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం connectdata *conn,
                        uint32_t max_మొత్తం,
                        uint32_t max_host,
                        స్థిరరాశి నిర్మాణం curltime *pnow)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cpool_bundle *bundle = ఖాళీ;
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);
  uint32_t సామర్థ్యం, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  ఒకవేళ(!cpool)
    తిరిగి CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.ప్రారంభం_ms[FIRSTSOCKET] =
    conn->shutdown.ప్రారంభం_ms[SECONDARYSOCKET] = -1;

  CPOOL_తాళం(cpool, దత్తాంశం);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  ఎంపిక(cpool_check_limits(దత్తాంశం, max_మొత్తం, conn, max_host, bundle, pnow)) {
  స్థితి CPOOL_హద్దు_DEST:
    infof(దత్తాంశం, "No more connections allowed to host");
    ఫలితం = CURLE_NO_CONNECTION_అందుబాటు;
    వెళ్ళు బహిర్గతం;
  స్థితి CPOOL_హద్దు_మొత్తం:
    infof(దత్తాంశం, "No connections available, total of %u reached.",
          దత్తాంశం->multi->max_మొత్తం_connections);
    ఫలితం = CURLE_NO_CONNECTION_అందుబాటు;
    వెళ్ళు బహిర్గతం;
  అప్రమేయ:
    ఆపు;
  }

  /* We add the connection */
  సామర్థ్యం = Curl_uint32_tbl_సామర్థ్యం(&cpool->conns);
  ఒకవేళ(సామర్థ్యం == UINT32_MAX) {
    infof(దత్తాంశం, "No connections available, pool at max capacity");
    ఫలితం = CURLE_NO_CONNECTION_అందుబాటు;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(Curl_uint32_tbl_లెక్క(&cpool->conns) >= సామర్థ్యం) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(సామర్థ్యం, 16);
    ఒకవేళ((UINT32_MAX - growth) >= సామర్థ్యం)
      ncapacity = సామర్థ్యం + growth;
    లేకపోతే
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    ఒకవేళ(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(!bundle) {
    bundle = cpool_create_bundle();
    ఒకవేళ(!bundle) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(!Curl_uint32_tbl_కూడు(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  conn->cpid = cpid;

  ఒకవేళ(!cpool_bundle_కూడు_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  conn->connection_id = cpool->తదుపరి_connection_id++;
  CURL_TRC_M(దత్తాంశం, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_లెక్క(&cpool->conns));
బహిర్గతం:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, దత్తాంశం);

  తిరిగి ఫలితం;
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
స్థిర bool cpool_foreach(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం cpool *cpool,
                          శూన్య *param,
                          పూర్ణాంకం (*func)(నిర్మాణం cpool *cpool,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn, శూన్య *param))
{
  శూన్య *నమోదు_2;
  uint32_t cpid;

  ఒకవేళ(!cpool)
    తిరిగి FALSE;

  ఒకవేళ(Curl_uint32_tbl_మొదటి(&cpool->conns, &cpid, &నమోదు_2)) {
    చేయి {
      నిర్మాణం connectdata *conn = నమోదు_2;
      ఒకవేళ(conn && func(cpool, దత్తాంశం, conn, param) == 1)
        తిరిగి TRUE;
    } అయితే(Curl_uint32_tbl_తదుపరి(&cpool->conns, cpid, &cpid, &నమోదు_2));
  }
  తిరిగి FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
స్థిర bool cpool_conn_now_idle(నిర్మాణం cpool *cpool,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn,
                                స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం connectdata *oldest_idle = ఖాళీ;
  నిర్మాణం Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->multi)
    తిరిగి kept;

  Curl_uint32_bset_కూడు(&cpool->idles, conn->cpid);

  ఒకవేళ(!దత్తాంశం->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(దత్తాంశం->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  లేకపోతే {
    maxconnects = దత్తాంశం->multi->maxconnects;
  }

  ఒకవేళ(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t అంకె_conns = Curl_uint32_tbl_లెక్క(&cpool->conns);
    admin = Curl_get_admin(దత్తాంశం);
    ఒకవేళ(అంకె_conns > maxconnects) {
      infof(దత్తాంశం, "Connection pool is full, closing the oldest of %u/%u",
            అంకె_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      ఒకవేళ(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  తిరిగి kept;
}

స్థిర శూన్య cpool_prune_dead(నిర్మాణం cpool *cpool,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం curltime *pnow);

స్థిర bool cpool_కనుగొను_act(నిర్మాణం cpool *cpool,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం connectdata *conn,
                           cpool_సరిపోలిక_ఫలితం సరిపోలిక)
{
  ఎంపిక(సరిపోలిక) {
  స్థితి CPOOL_సరిపోలిక_దొరికింది:
    తిరిగి TRUE;
  స్థితి CPOOL_సరిపోలిక_TOO_OLD:
    ఒకవేళ(!cpool_conn_can_be_closed(conn))
      తిరిగి FALSE;
    FALLTHROUGH();
  స్థితి CPOOL_సరిపోలిక_CLOSE:
    cpool_conn_close(cpool, దత్తాంశం, conn, FALSE);
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి FALSE;
}

bool Curl_cpool_కనుగొను(నిర్మాణం Curl_easy *దత్తాంశం,
                     స్థిరరాశి అక్షరం *destination,
                     bool prune_dead,
                     స్థిరరాశి నిర్మాణం curltime *pnow,
                     Curl_cpool_conn_సరిపోలిక_cb *conn_cb,
                     Curl_cpool_పూర్తి_సరిపోలిక_cb *పూర్తి_cb,
                     శూన్య *userdata)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);
  నిర్మాణం cpool_bundle *bundle = ఖాళీ;
  bool దొరికింది = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  ఒకవేళ(!cpool)
    తిరిగి FALSE;

  CPOOL_తాళం(cpool, దత్తాంశం);

  ఒకవేళ(prune_dead)
    cpool_prune_dead(cpool, దత్తాంశం, pnow);

  ఒకవేళ(దత్తాంశం->state.చివరి_cpid != UINT32_MAX) {
    నిర్మాణం connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, దత్తాంశం->state.చివరి_cpid);
    ఒకవేళ(conn && (దత్తాంశం->state.చివరి_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      దొరికింది = cpool_కనుగొను_act(cpool, దత్తాంశం, conn, conn_cb(conn, userdata));
      ఒకవేళ(దొరికింది)
        వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      /* No longer found here. */
      దత్తాంశం->state.చివరి_conn_id = -1;
      దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  ఒకవేళ(bundle) {
    uint32_t cpid;
    ఒకవేళ(Curl_uint32_spbset_మొదటి(&bundle->ids, &cpid)) {
      చేయి {
        నిర్మాణం connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        ఒకవేళ(conn) {
          ఒకవేళ(దత్తాంశం->state.చివరి_conn_id != conn->connection_id) {
            దొరికింది = cpool_కనుగొను_act(cpool, దత్తాంశం, conn, conn_cb(conn, userdata));
            ఒకవేళ(దొరికింది)
              వెళ్ళు బహిర్గతం;
          }
        }
        లేకపోతే {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } అయితే(Curl_uint32_spbset_తదుపరి(&bundle->ids, cpid, &cpid));
    }
  }

బహిర్గతం:
  ఒకవేళ(పూర్తి_cb) {
    దొరికింది = పూర్తి_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, దత్తాంశం);
  తిరిగి దొరికింది;
}

నిర్మాణం cpool_reaper_ctx {
  size_t reaped;
  స్థిరరాశి నిర్మాణం curltime *pnow;
};

స్థిర పూర్ణాంకం cpool_reap_dead_cb(నిర్మాణం cpool *cpool,
                              నిర్మాణం Curl_easy *admin,
                              నిర్మాణం connectdata *conn, శూన్య *param)
{
  నిర్మాణం cpool_reaper_ctx *reaper = param;

  ఒకవేళ(cpool_conn_can_be_closed(conn)) {
    ఒకవేళ(conn->బిట్లు.no_reuse || conn->బిట్లు.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  తిరిగి 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
స్థిర శూన్య cpool_prune_dead(నిర్మాణం cpool *cpool,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->చివరి_cleanup);

  ఒకవేళ(elapsed_ms >= 1000L) {
    నిర్మాణం Curl_easy *admin = Curl_get_admin(దత్తాంశం);
    నిర్మాణం cpool_reaper_ctx reaper;

    memset(&reaper, 0, పరిమాణం(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->చివరి_cleanup = *pnow;
  }
}

స్థిర పూర్ణాంకం conn_upkeep(నిర్మాణం cpool *cpool,
                       నిర్మాణం Curl_easy *admin,
                       నిర్మాణం connectdata *conn,
                       శూన్య *param)
{
  స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(admin);

  (శూన్య)param;
  ఒకవేళ((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->సమితి.upkeep_interval_ms) {
    CURLcode ఫలితం;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    ఫలితం = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    ఒకవేళ(ఫలితం && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  తిరిగి 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);
  నిర్మాణం Curl_easy *admin = Curl_get_admin(దత్తాంశం);

  ఒకవేళ(!cpool)
    తిరిగి CURLE_OK;

  CPOOL_తాళం(cpool, admin);
  cpool_foreach(admin, cpool, ఖాళీ, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  తిరిగి CURLE_OK;
}

నిర్మాణం connectdata *Curl_cpool_get_చివరి_conn(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);
  ఒకవేళ(cpool && (దత్తాంశం->state.చివరి_cpid != UINT32_MAX)) {
    నిర్మాణం connectdata *conn;

    CPOOL_తాళం(cpool, దత్తాంశం);
    conn = Curl_uint32_tbl_get(&cpool->conns, దత్తాంశం->state.చివరి_cpid);
    CPOOL_UNLOCK(cpool, దత్తాంశం);
    ఒకవేళ(conn && (దత్తాంశం->state.చివరి_conn_id == conn->connection_id))
      తిరిగి conn;
    దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
    దత్తాంశం->state.చివరి_conn_id = -1;
  }
  తిరిగి ఖాళీ;
}

శూన్య Curl_cpool_return(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం connectdata *conn,
                       Curl_cpool_return_cb *cb, శూన్య *cbdata,
                       స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం cpool *cpool = cpool_get_instance(దత్తాంశం);

  CPOOL_తాళం(cpool, దత్తాంశం);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  ఎంపిక(cb(దత్తాంశం, conn, cbdata, pnow)) {
  స్థితి CPOOL_DO_KEEP:
    ఆపు;
  స్థితి CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    ఒకవేళ(cpool_conn_now_idle(cpool, దత్తాంశం, conn, pnow)) {
      /* connection kept in the cpool */
      infof(దత్తాంశం, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->పోర్టు);
    }
    లేకపోతే { /* connection was removed from the cpool and destroyed. */
      దత్తాంశం->state.చివరి_conn_id = -1;
      దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
    }
    ఆపు;
  స్థితి CPOOL_DO_CLOSE:
    Curl_conn_close(దత్తాంశం, conn, FALSE);
    ఆపు;
  స్థితి CPOOL_DO_TERMINATE:
    Curl_conn_close(దత్తాంశం, conn, TRUE);
    ఆపు;
  }

  CPOOL_UNLOCK(cpool, దత్తాంశం);
}

స్థిర పూర్ణాంకం cpool_reap_no_reuse(నిర్మాణం cpool *cpool,
                               నిర్మాణం Curl_easy *admin,
                               నిర్మాణం connectdata *conn, శూన్య *param)
{
  (శూన్య)param;
  conn->బిట్లు.no_reuse = TRUE;
  ఒకవేళ(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  తిరిగి 0; /* continue iteration */
}

శూన్య Curl_cpool_nw_మారింది(నిర్మాణం cpool *cpool, నిర్మాణం Curl_easy *admin)
{
  ఒకవేళ(cpool && admin) {
    CPOOL_తాళం(cpool, admin);
    cpool_foreach(admin, cpool, ఖాళీ, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
స్థిర bool cpool_conn_too_old(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               స్థిరరాశి నిర్మాణం curltime *pnow)
{
  timediff_t age_ms;

  ఒకవేళ(దత్తాంశం->సమితి.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    ఒకవేళ(age_ms > దత్తాంశం->సమితి.conn_max_idle_ms) {
      infof(దత్తాంశం, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, దత్తాంశం->సమితి.conn_max_idle_ms);
      తిరిగి TRUE;
    }
  }

  ఒకవేళ(దత్తాంశం->సమితి.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    ఒకవేళ(age_ms > దత్తాంశం->సమితి.conn_max_age_ms) {
      infof(దత్తాంశం,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, దత్తాంశం->సమితి.conn_max_age_ms);
      తిరిగి TRUE;
    }
  }

  తిరిగి FALSE;
}

bool Curl_cpool_conn_seems_healthy(నిర్మాణం connectdata *conn,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_ఆదానం;

  DEBUGASSERT(!దత్తాంశం->conn);
  ఒకవేళ(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(దత్తాంశం, conn, pnow))
    తిరిగి FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_ఆదానం = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->నిరంతరం->connection_is_dead;
  ఒకవేళ(!poll_ఆదానం &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    తిరిగి TRUE;

  admin = Curl_get_admin(దత్తాంశం);
  ఒకవేళ(conn->scheme->నిరంతరం->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->నిరంతరం->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  లేకపోతే {
    bool ఆదానం_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &ఆదానం_pending);
    Curl_detach_connection(admin);
    ఒకవేళ(healthy && ఆదానం_pending && poll_ఆదానం) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(దత్తాంశం, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  ఒకవేళ(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  తిరిగి healthy;
}

శూన్య Curl_cpool_conn_was_వాడిన(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn,
                              స్థిరరాశి నిర్మాణం curltime *pnow)
{
  (శూన్య)దత్తాంశం;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  స్థిరరాశి నిర్మాణం curltime *pnow)
{
  (శూన్య)దత్తాంశం;
  తిరిగి curlx_ptimediff_ms(pnow, &conn->created);
}
