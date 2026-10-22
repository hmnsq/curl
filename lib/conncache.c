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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cshutdn.h"
#অন্তর্ভাব "conncache.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "sigpipe.h"
#অন্তর্ভাব "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  করো {                                                                  \
    যদি(c) {                                                             \
      যদি(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_তথ্য_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } যখন(0)

#define CPOOL_UNLOCK(c, d)                                              \
  করো {                                                                  \
    যদি(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      যদি(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_তথ্য_CONNECT); \
    }                                                                   \
  } যখন(0)

/* A set of connection pool ids to the same destination. */
কাঠামো cpool_bundle {
  কাঠামো uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

স্থির শূন্য cpool_bundle_unlink(কাঠামো cpool_bundle **pbundle)
{
  যদি(*pbundle) {
    কাঠামো cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ফাঁকা;
    যদি(bundle->refcount)
      bundle->refcount--;
    যদি(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

স্থির শূন্য cpool_bundle_link(কাঠামো cpool_bundle **pdest,
                              কাঠামো cpool_bundle *src)
{
  যদি(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    যদি(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

স্থির কাঠামো cpool_bundle *cpool_create_bundle(শূন্য)
{
  /* created with refcount == 1 */
  কাঠামো cpool_bundle *bundle = curlx_calloc(1, আকার(*bundle));
  যদি(!bundle)
    ফেরত ফাঁকা;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  ফেরত bundle;
}

/* Add a connection to a bundle */
স্থির bool cpool_bundle_add_id(কাঠামো cpool *cpool,
                                কাঠামো cpool_bundle *bundle,
                                ধ্রুবক অক্ষর *destination,
                                uint32_t pool_id)
{
  যদি(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    ফেরত FALSE;
  যদি(Curl_uint32_spbset_গুনতি(&bundle->ids) == 1) {
    /* first one added, register bundle */
    কাঠামো cpool_bundle *new_ref = ফাঁকা;
    cpool_bundle_link(&new_ref, bundle);
    যদি(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      ফেরত FALSE;
    }
  }
  ফেরত TRUE;
}

/* Remove a connection from a bundle */
স্থির শূন্য cpool_bundle_remove_id(কাঠামো cpool *cpool,
                                   কাঠামো cpool_bundle *bundle,
                                   ধ্রুবক অক্ষর *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  যদি(!Curl_uint32_spbset_গুনতি(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

স্থির শূন্য cpool_bundle_hash_dtor(শূন্য *entry)
{
  কাঠামো cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

স্থির কাঠামো cpool_bundle *cpool_get_bundle(কাঠামো cpool *cpool,
                                             ধ্রুবক অক্ষর *destination)
{
  কাঠামো cpool_bundle *bundle = ফাঁকা;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  ফেরত bundle;
}

স্থির শূন্য cpool_remove_conn(কাঠামো cpool *cpool,
                              কাঠামো connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  যদি(cpid != UINT32_MAX) {
    কাঠামো cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      কাঠামো connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    যদি(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

স্থির শূন্য cpool_discard_conn(কাঠামো cpool *cpool,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               bool aborted)
{
  কাঠামো Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(!তথ্য->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(তথ্য);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  যদি(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    ফেরত;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  যদি(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  যদি(aborted)
    done = TRUE;
  যদি(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  যদি(done || !তথ্য->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  অন্যথা {
    কাঠামো Curl_multi *multi = তথ্য->multi;
    size_t max_shutdowns = multi->max_যোগ_connections;
    uint32_t অঙ্ক_conns = Curl_uint32_tbl_গুনতি(&cpool->conns);

    যদি(অঙ্ক_conns < max_shutdowns)
      max_shutdowns -= অঙ্ক_conns;
    অন্যথা যদি(max_shutdowns)
      max_shutdowns = 1;
    অন্যথা /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(অঙ্ক_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

শূন্য Curl_cpool_init(কাঠামো cpool *cpool,
                     কাঠামো Curl_share *share,
                     size_t আকার_2)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, আকার_2, CURL_HASH_TYPE_BYTES,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->in_shutdown = FALSE;
  cpool->locked = FALSE;
}

শূন্য Curl_cpool_destroy(কাঠামো cpool *cpool, কাঠামো Curl_easy *admin)
{
  যদি(cpool && cpool->initialized && admin) {
    শূন্য *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_গুনতি(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    যদি(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      কাঠামো Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      করো {
        কাঠামো connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } যখন(Curl_uint32_tbl_পরবর্তী(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

স্থির কাঠামো cpool *cpool_get_instance(কাঠামো Curl_easy *তথ্য)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(তথ্য->mid);
  যদি(CURL_SHARE_KEEP_CONNECT(তথ্য->share))
    ফেরত &তথ্য->share->cpool;
  অন্যথা যদি(তথ্য->multi_easy)
    ফেরত &তথ্য->multi_easy->cpool;
  অন্যথা যদি(তথ্য->multi)
    ফেরত &তথ্য->multi->cpool;
  ফেরত ফাঁকা;
}

কাঠামো cpool *Curl_cpool_get_instance(কাঠামো Curl_easy *তথ্য)
{
  ফেরত cpool_get_instance(তথ্য);
}

শূন্য Curl_cpool_xfer_init(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);

  যদি(cpool) {
    CPOOL_LOCK(cpool, তথ্য);
    /* the identifier inside the connection cache */
    তথ্য->id = cpool->পরবর্তী_easy_id++;
    যদি(cpool->পরবর্তী_easy_id == CURL_OFF_T_MAX)
      cpool->পরবর্তী_easy_id = 0;
    তথ্য->state.last_conn_id = -1;
    তথ্য->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, তথ্য);
  }
  অন্যথা {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    তথ্য->id = 0;
    তথ্য->state.last_conn_id = -1;
    তথ্য->state.last_cpid = UINT32_MAX;
  }
}

স্থির bool cpool_conn_can_be_closed(কাঠামো connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  ফেরত (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

স্থির কাঠামো connectdata *cpool_bundle_get_oldest_idle(
  কাঠামো cpool *cpool,
  কাঠামো cpool_bundle *bundle,
  ধ্রুবক কাঠামো curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  যদি(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    করো {
      কাঠামো connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      যদি(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        যদি(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      অন্যথা যদি(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } যখন(Curl_uint32_spbset_পরবর্তী(&bundle->ids, cpid, &cpid));
  }
  ফেরত Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

স্থির কাঠামো connectdata *cpool_get_oldest_idle(কাঠামো cpool *cpool,
                                                 ধ্রুবক কাঠামো curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  যদি(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    করো {
      কাঠামো connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      যদি(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        যদি((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      অন্যথা {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } যখন(Curl_uint32_bset_পরবর্তী(&cpool->idles, cpid, &cpid));
  }
  ফেরত Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

স্থির শূন্য cpool_conn_close(কাঠামো cpool *cpool,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn,
                             bool aborted)
{
  কাঠামো Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(তথ্য && !তথ্য->conn);
  যদি(!cpool)
    ফেরত;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  যদি(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(তথ্য, "conn terminate when inuse: %u", conn->attached_xfers));
    ফেরত;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(তথ্য);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  যদি(do_lock)
    CPOOL_LOCK(cpool, admin);

  যদি(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  যদি(conn->bits.connect_only)
    aborted = TRUE;

  যদি(তথ্য->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(তথ্য, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&তথ্য->multi->cpool, তথ্য, conn, aborted);
  }
  অন্যথা {
    /* No multi available, terminate */
    infof(তথ্য, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  যদি(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

শূন্য Curl_conn_close(কাঠামো Curl_easy *তথ্য,
                     কাঠামো connectdata *conn,
                     bool aborted)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);
  cpool_conn_close(cpool, তথ্য, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
স্থির শূন্য cpool_evict_conn(কাঠামো cpool *cpool,
                             কাঠামো Curl_easy *admin,
                             কাঠামো connectdata *conn)
{
  যদি(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  অন্যথা
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_যোগ  2

স্থির পূর্ণসংখ্যা cpool_check_limits(কাঠামো Curl_easy *তথ্য,
                              uint32_t max_যোগ,
                              কাঠামো connectdata *to_add,
                              uint32_t max_host,
                              কাঠামো cpool_bundle *bundle,
                              ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);
  কাঠামো cshutdn *cshutdn = তথ্য->multi ? &তথ্য->multi->cshutdn : ফাঁকা;
  কাঠামো Curl_easy *admin = ফাঁকা;

  যদি(!cpool)
    ফেরত CPOOL_LIMIT_OK;

  যদি(!max_যোগ && !max_host)
    ফেরত CPOOL_LIMIT_OK;

  যদি(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(তথ্য);
    যদি(bundle) {
      live = Curl_uint32_spbset_গুনতি(&bundle->ids);
      যদি(live >= max_host) {
        size_t over = live - max_host + 1;
        জন্য(; over; --over) {
          কাঠামো connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          যদি(!oldest_idle)
            বিরতি;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_গুনতি(&bundle->ids);
      }
    }
    যদি(cshutdn) {
      shutdowns = Curl_cshutdn_dest_গুনতি(cshutdn, to_add->destination);
      যদি(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    যদি((live + shutdowns) >= max_host)
      ফেরত CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  যদি(max_যোগ && !তথ্য->state.internal) {
    size_t shutdowns = 0;
    uint32_t অঙ্ক_conns = Curl_uint32_tbl_গুনতি(&cpool->conns);

    যদি(!admin)
      admin = Curl_get_admin(তথ্য);
    যদি(অঙ্ক_conns >= max_যোগ) {
      size_t over = অঙ্ক_conns - max_যোগ + 1;
      জন্য(; over; --over) {
        কাঠামো connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        যদি(!oldest_idle)
          বিরতি;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_যোগ);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    অঙ্ক_conns = Curl_uint32_tbl_গুনতি(&cpool->conns);
    যদি(cshutdn) {
      shutdowns = Curl_cshutdn_গুনতি(cshutdn);
      যদি(shutdowns && (অঙ্ক_conns + shutdowns) >= max_যোগ) {
        size_t over = অঙ্ক_conns + shutdowns - max_যোগ + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ফাঁকা, over);
      }
    }
    যদি((অঙ্ক_conns + shutdowns) >= max_যোগ)
      ফেরত CPOOL_LIMIT_যোগ;
  }

  ফেরত CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(কাঠামো Curl_easy *তথ্য,
                        কাঠামো connectdata *conn,
                        uint32_t max_যোগ,
                        uint32_t max_host,
                        ধ্রুবক কাঠামো curltime *pnow)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cpool_bundle *bundle = ফাঁকা;
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  যদি(!cpool)
    ফেরত CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, তথ্য);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  নির্বাচন(cpool_check_limits(তথ্য, max_যোগ, conn, max_host, bundle, pnow)) {
  ক্ষেত্র CPOOL_LIMIT_DEST:
    infof(তথ্য, "No more connections allowed to host");
    ফলাফল = CURLE_NO_CONNECTION_AVAILABLE;
    যাও out;
  ক্ষেত্র CPOOL_LIMIT_যোগ:
    infof(তথ্য, "No connections available, total of %u reached.",
          তথ্য->multi->max_যোগ_connections);
    ফলাফল = CURLE_NO_CONNECTION_AVAILABLE;
    যাও out;
  পূর্বনির্ধারিত:
    বিরতি;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  যদি(capacity == UINT32_MAX) {
    infof(তথ্য, "No connections available, pool at max capacity");
    ফলাফল = CURLE_NO_CONNECTION_AVAILABLE;
    যাও out;
  }
  যদি(Curl_uint32_tbl_গুনতি(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    যদি((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    অন্যথা
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    যদি(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

  যদি(!bundle) {
    bundle = cpool_create_bundle();
    যদি(!bundle) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

  যদি(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  conn->cpid = cpid;

  যদি(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  conn->connection_id = cpool->পরবর্তী_connection_id++;
  CURL_TRC_M(তথ্য, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_গুনতি(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, তথ্য);

  ফেরত ফলাফল;
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
স্থির bool cpool_foreach(কাঠামো Curl_easy *তথ্য,
                          কাঠামো cpool *cpool,
                          শূন্য *param,
                          পূর্ণসংখ্যা (*func)(কাঠামো cpool *cpool,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn, শূন্য *param))
{
  শূন্য *entry;
  uint32_t cpid;

  যদি(!cpool)
    ফেরত FALSE;

  যদি(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    করো {
      কাঠামো connectdata *conn = entry;
      যদি(conn && func(cpool, তথ্য, conn, param) == 1)
        ফেরত TRUE;
    } যখন(Curl_uint32_tbl_পরবর্তী(&cpool->conns, cpid, &cpid, &entry));
  }
  ফেরত FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
স্থির bool cpool_conn_now_idle(কাঠামো cpool *cpool,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn,
                                ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো connectdata *oldest_idle = ফাঁকা;
  কাঠামো Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  যদি(!তথ্য || !তথ্য->multi)
    ফেরত kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  যদি(!তথ্য->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(তথ্য->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  অন্যথা {
    maxconnects = তথ্য->multi->maxconnects;
  }

  যদি(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t অঙ্ক_conns = Curl_uint32_tbl_গুনতি(&cpool->conns);
    admin = Curl_get_admin(তথ্য);
    যদি(অঙ্ক_conns > maxconnects) {
      infof(তথ্য, "Connection pool is full, closing the oldest of %u/%u",
            অঙ্ক_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      যদি(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  ফেরত kept;
}

স্থির শূন্য cpool_prune_dead(কাঠামো cpool *cpool,
                             কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো curltime *pnow);

স্থির bool cpool_find_act(কাঠামো cpool *cpool,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো connectdata *conn,
                           cpool_match_ফলাফল match)
{
  নির্বাচন(match) {
  ক্ষেত্র CPOOL_MATCH_FOUND:
    ফেরত TRUE;
  ক্ষেত্র CPOOL_MATCH_TOO_OLD:
    যদি(!cpool_conn_can_be_closed(conn))
      ফেরত FALSE;
    FALLTHROUGH();
  ক্ষেত্র CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, তথ্য, conn, FALSE);
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত FALSE;
}

bool Curl_cpool_find(কাঠামো Curl_easy *তথ্য,
                     ধ্রুবক অক্ষর *destination,
                     bool prune_dead,
                     ধ্রুবক কাঠামো curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     শূন্য *userdata)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);
  কাঠামো cpool_bundle *bundle = ফাঁকা;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  যদি(!cpool)
    ফেরত FALSE;

  CPOOL_LOCK(cpool, তথ্য);

  যদি(prune_dead)
    cpool_prune_dead(cpool, তথ্য, pnow);

  যদি(তথ্য->state.last_cpid != UINT32_MAX) {
    কাঠামো connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, তথ্য->state.last_cpid);
    যদি(conn && (তথ্য->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, তথ্য, conn, conn_cb(conn, userdata));
      যদি(found)
        যাও out;
    }
    অন্যথা {
      /* No longer found here. */
      তথ্য->state.last_conn_id = -1;
      তথ্য->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  যদি(bundle) {
    uint32_t cpid;
    যদি(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      করো {
        কাঠামো connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        যদি(conn) {
          যদি(তথ্য->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, তথ্য, conn, conn_cb(conn, userdata));
            যদি(found)
              যাও out;
          }
        }
        অন্যথা {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } যখন(Curl_uint32_spbset_পরবর্তী(&bundle->ids, cpid, &cpid));
    }
  }

out:
  যদি(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, তথ্য);
  ফেরত found;
}

কাঠামো cpool_reaper_ctx {
  size_t reaped;
  ধ্রুবক কাঠামো curltime *pnow;
};

স্থির পূর্ণসংখ্যা cpool_reap_dead_cb(কাঠামো cpool *cpool,
                              কাঠামো Curl_easy *admin,
                              কাঠামো connectdata *conn, শূন্য *param)
{
  কাঠামো cpool_reaper_ctx *reaper = param;

  যদি(cpool_conn_can_be_closed(conn)) {
    যদি(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ফেরত 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
স্থির শূন্য cpool_prune_dead(কাঠামো cpool *cpool,
                             কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  যদি(elapsed_ms >= 1000L) {
    কাঠামো Curl_easy *admin = Curl_get_admin(তথ্য);
    কাঠামো cpool_reaper_ctx reaper;

    memset(&reaper, 0, আকার(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

স্থির পূর্ণসংখ্যা conn_upkeep(কাঠামো cpool *cpool,
                       কাঠামো Curl_easy *admin,
                       কাঠামো connectdata *conn,
                       শূন্য *param)
{
  ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(admin);

  (শূন্য)param;
  যদি((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode ফলাফল;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    ফলাফল = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    যদি(ফলাফল && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ফেরত 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);
  কাঠামো Curl_easy *admin = Curl_get_admin(তথ্য);

  যদি(!cpool)
    ফেরত CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, ফাঁকা, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  ফেরত CURLE_OK;
}

কাঠামো connectdata *Curl_cpool_get_last_conn(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);
  যদি(cpool && (তথ্য->state.last_cpid != UINT32_MAX)) {
    কাঠামো connectdata *conn;

    CPOOL_LOCK(cpool, তথ্য);
    conn = Curl_uint32_tbl_get(&cpool->conns, তথ্য->state.last_cpid);
    CPOOL_UNLOCK(cpool, তথ্য);
    যদি(conn && (তথ্য->state.last_conn_id == conn->connection_id))
      ফেরত conn;
    তথ্য->state.last_cpid = UINT32_MAX;
    তথ্য->state.last_conn_id = -1;
  }
  ফেরত ফাঁকা;
}

শূন্য Curl_cpool_return(কাঠামো Curl_easy *তথ্য,
                       কাঠামো connectdata *conn,
                       Curl_cpool_return_cb *cb, শূন্য *cbdata,
                       ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো cpool *cpool = cpool_get_instance(তথ্য);

  CPOOL_LOCK(cpool, তথ্য);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  নির্বাচন(cb(তথ্য, conn, cbdata, pnow)) {
  ক্ষেত্র CPOOL_DO_KEEP:
    বিরতি;
  ক্ষেত্র CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    যদি(cpool_conn_now_idle(cpool, তথ্য, conn, pnow)) {
      /* connection kept in the cpool */
      infof(তথ্য, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    অন্যথা { /* connection was removed from the cpool and destroyed. */
      তথ্য->state.last_conn_id = -1;
      তথ্য->state.last_cpid = UINT32_MAX;
    }
    বিরতি;
  ক্ষেত্র CPOOL_DO_CLOSE:
    Curl_conn_close(তথ্য, conn, FALSE);
    বিরতি;
  ক্ষেত্র CPOOL_DO_TERMINATE:
    Curl_conn_close(তথ্য, conn, TRUE);
    বিরতি;
  }

  CPOOL_UNLOCK(cpool, তথ্য);
}

স্থির পূর্ণসংখ্যা cpool_reap_no_reuse(কাঠামো cpool *cpool,
                               কাঠামো Curl_easy *admin,
                               কাঠামো connectdata *conn, শূন্য *param)
{
  (শূন্য)param;
  conn->bits.no_reuse = TRUE;
  যদি(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  ফেরত 0; /* continue iteration */
}

শূন্য Curl_cpool_nw_changed(কাঠামো cpool *cpool, কাঠামো Curl_easy *admin)
{
  যদি(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, ফাঁকা, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
স্থির bool cpool_conn_too_old(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               ধ্রুবক কাঠামো curltime *pnow)
{
  timediff_t age_ms;

  যদি(তথ্য->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    যদি(age_ms > তথ্য->set.conn_max_idle_ms) {
      infof(তথ্য, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, তথ্য->set.conn_max_idle_ms);
      ফেরত TRUE;
    }
  }

  যদি(তথ্য->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    যদি(age_ms > তথ্য->set.conn_max_age_ms) {
      infof(তথ্য,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, তথ্য->set.conn_max_age_ms);
      ফেরত TRUE;
    }
  }

  ফেরত FALSE;
}

bool Curl_cpool_conn_seems_healthy(কাঠামো connectdata *conn,
                                   কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_প্রবেশ;

  DEBUGASSERT(!তথ্য->conn);
  যদি(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(তথ্য, conn, pnow))
    ফেরত FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_প্রবেশ = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  যদি(!poll_প্রবেশ &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    ফেরত TRUE;

  admin = Curl_get_admin(তথ্য);
  যদি(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  অন্যথা {
    bool প্রবেশ_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &প্রবেশ_pending);
    Curl_detach_connection(admin);
    যদি(healthy && প্রবেশ_pending && poll_প্রবেশ) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(তথ্য, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  যদি(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  ফেরত healthy;
}

শূন্য Curl_cpool_conn_was_used(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn,
                              ধ্রুবক কাঠামো curltime *pnow)
{
  (শূন্য)তথ্য;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  ধ্রুবক কাঠামো curltime *pnow)
{
  (শূন্য)তথ্য;
  ফেরত curlx_ptimediff_ms(pnow, &conn->created);
}
