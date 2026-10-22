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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "progress.h"
#شامل "multiif.h"
#شامل "curl_trc.h"
#شامل "cshutdn.h"
#شامل "conncache.h"
#شامل "curl_share.h"
#شامل "sigpipe.h"
#شامل "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  انجامبده {                                                                  \
    اگر(c) {                                                             \
      اگر(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } وقتی(0)

#define CPOOL_UNLOCK(c, d)                                              \
  انجامبده {                                                                  \
    اگر(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      اگر(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } وقتی(0)

/* A set of connection pool ids to the same destination. */
ساختار cpool_bundle {
  ساختار uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ایستا تهی cpool_bundle_unlink(ساختار cpool_bundle **pbundle)
{
  اگر(*pbundle) {
    ساختار cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = پوچ;
    اگر(bundle->refcount)
      bundle->refcount--;
    اگر(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ایستا تهی cpool_bundle_link(ساختار cpool_bundle **pdest,
                              ساختار cpool_bundle *src)
{
  اگر(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    اگر(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ایستا ساختار cpool_bundle *cpool_create_bundle(تهی)
{
  /* created with refcount == 1 */
  ساختار cpool_bundle *bundle = curlx_calloc(1, اندازه(*bundle));
  اگر(!bundle)
    بازگشت پوچ;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  بازگشت bundle;
}

/* Add a connection to a bundle */
ایستا bool cpool_bundle_add_id(ساختار cpool *cpool,
                                ساختار cpool_bundle *bundle,
                                ثابت نویسه *destination,
                                uint32_t pool_id)
{
  اگر(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    بازگشت FALSE;
  اگر(Curl_uint32_spbset_شمار(&bundle->ids) == 1) {
    /* first one added, register bundle */
    ساختار cpool_bundle *new_ref = پوچ;
    cpool_bundle_link(&new_ref, bundle);
    اگر(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      بازگشت FALSE;
    }
  }
  بازگشت TRUE;
}

/* Remove a connection from a bundle */
ایستا تهی cpool_bundle_remove_id(ساختار cpool *cpool,
                                   ساختار cpool_bundle *bundle,
                                   ثابت نویسه *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  اگر(!Curl_uint32_spbset_شمار(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

ایستا تهی cpool_bundle_hash_dtor(تهی *entry)
{
  ساختار cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

ایستا ساختار cpool_bundle *cpool_get_bundle(ساختار cpool *cpool,
                                             ثابت نویسه *destination)
{
  ساختار cpool_bundle *bundle = پوچ;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  بازگشت bundle;
}

ایستا تهی cpool_remove_conn(ساختار cpool *cpool,
                              ساختار connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  اگر(cpid != UINT32_MAX) {
    ساختار cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      ساختار connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    اگر(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ایستا تهی cpool_discard_conn(ساختار cpool *cpool,
                               ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               bool aborted)
{
  ساختار Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  اگر(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    بازگشت;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  اگر(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  اگر(aborted)
    done = TRUE;
  اگر(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  اگر(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  وگرنه {
    ساختار Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t رقم_conns = Curl_uint32_tbl_شمار(&cpool->conns);

    اگر(رقم_conns < max_shutdowns)
      max_shutdowns -= رقم_conns;
    وگرنه اگر(max_shutdowns)
      max_shutdowns = 1;
    وگرنه /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(رقم_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

تهی Curl_cpool_init(ساختار cpool *cpool,
                     ساختار Curl_share *share,
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

تهی Curl_cpool_destroy(ساختار cpool *cpool, ساختار Curl_easy *admin)
{
  اگر(cpool && cpool->initialized && admin) {
    تهی *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_شمار(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    اگر(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      ساختار Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      انجامبده {
        ساختار connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } وقتی(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ایستا ساختار cpool *cpool_get_instance(ساختار Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  اگر(CURL_SHARE_KEEP_CONNECT(data->share))
    بازگشت &data->share->cpool;
  وگرنه اگر(data->multi_easy)
    بازگشت &data->multi_easy->cpool;
  وگرنه اگر(data->multi)
    بازگشت &data->multi->cpool;
  بازگشت پوچ;
}

ساختار cpool *Curl_cpool_get_instance(ساختار Curl_easy *data)
{
  بازگشت cpool_get_instance(data);
}

تهی Curl_cpool_xfer_init(ساختار Curl_easy *data)
{
  ساختار cpool *cpool = cpool_get_instance(data);

  اگر(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    اگر(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  وگرنه {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

ایستا bool cpool_conn_can_be_closed(ساختار connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  بازگشت (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

ایستا ساختار connectdata *cpool_bundle_get_oldest_idle(
  ساختار cpool *cpool,
  ساختار cpool_bundle *bundle,
  ثابت ساختار curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  اگر(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    انجامبده {
      ساختار connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      اگر(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        اگر(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      وگرنه اگر(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } وقتی(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  بازگشت Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ایستا ساختار connectdata *cpool_get_oldest_idle(ساختار cpool *cpool,
                                                 ثابت ساختار curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  اگر(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    انجامبده {
      ساختار connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      اگر(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        اگر((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      وگرنه {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } وقتی(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  بازگشت Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ایستا تهی cpool_conn_close(ساختار cpool *cpool,
                             ساختار Curl_easy *data,
                             ساختار connectdata *conn,
                             bool aborted)
{
  ساختار Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  اگر(!cpool)
    بازگشت;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  اگر(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    بازگشت;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  اگر(do_lock)
    CPOOL_LOCK(cpool, admin);

  اگر(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  اگر(conn->bits.connect_only)
    aborted = TRUE;

  اگر(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  وگرنه {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  اگر(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

تهی Curl_conn_close(ساختار Curl_easy *data,
                     ساختار connectdata *conn,
                     bool aborted)
{
  ساختار cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ایستا تهی cpool_evict_conn(ساختار cpool *cpool,
                             ساختار Curl_easy *admin,
                             ساختار connectdata *conn)
{
  اگر(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  وگرنه
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

ایستا صحیح cpool_check_limits(ساختار Curl_easy *data,
                              uint32_t max_total,
                              ساختار connectdata *to_add,
                              uint32_t max_host,
                              ساختار cpool_bundle *bundle,
                              ثابت ساختار curltime *pnow)
{
  ساختار cpool *cpool = cpool_get_instance(data);
  ساختار cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : پوچ;
  ساختار Curl_easy *admin = پوچ;

  اگر(!cpool)
    بازگشت CPOOL_LIMIT_OK;

  اگر(!max_total && !max_host)
    بازگشت CPOOL_LIMIT_OK;

  اگر(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    اگر(bundle) {
      live = Curl_uint32_spbset_شمار(&bundle->ids);
      اگر(live >= max_host) {
        size_t over = live - max_host + 1;
        برای(; over; --over) {
          ساختار connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          اگر(!oldest_idle)
            بشکن;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_شمار(&bundle->ids);
      }
    }
    اگر(cshutdn) {
      shutdowns = Curl_cshutdn_dest_شمار(cshutdn, to_add->destination);
      اگر(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    اگر((live + shutdowns) >= max_host)
      بازگشت CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  اگر(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t رقم_conns = Curl_uint32_tbl_شمار(&cpool->conns);

    اگر(!admin)
      admin = Curl_get_admin(data);
    اگر(رقم_conns >= max_total) {
      size_t over = رقم_conns - max_total + 1;
      برای(; over; --over) {
        ساختار connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        اگر(!oldest_idle)
          بشکن;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    رقم_conns = Curl_uint32_tbl_شمار(&cpool->conns);
    اگر(cshutdn) {
      shutdowns = Curl_cshutdn_شمار(cshutdn);
      اگر(shutdowns && (رقم_conns + shutdowns) >= max_total) {
        size_t over = رقم_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, پوچ, over);
      }
    }
    اگر((رقم_conns + shutdowns) >= max_total)
      بازگشت CPOOL_LIMIT_TOTAL;
  }

  بازگشت CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(ساختار Curl_easy *data,
                        ساختار connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        ثابت ساختار curltime *pnow)
{
  CURLcode result = CURLE_OK;
  ساختار cpool_bundle *bundle = پوچ;
  ساختار cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  اگر(!cpool)
    بازگشت CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  انتخاب(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  حالت CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    برو out;
  حالت CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    برو out;
  پیشفرض:
    بشکن;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  اگر(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    برو out;
  }
  اگر(Curl_uint32_tbl_شمار(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    اگر((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    وگرنه
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    اگر(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }

  اگر(!bundle) {
    bundle = cpool_create_bundle();
    اگر(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }

  اگر(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  conn->cpid = cpid;

  اگر(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_شمار(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  بازگشت result;
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
ایستا bool cpool_foreach(ساختار Curl_easy *data,
                          ساختار cpool *cpool,
                          تهی *param,
                          صحیح (*func)(ساختار cpool *cpool,
                                      ساختار Curl_easy *data,
                                      ساختار connectdata *conn, تهی *param))
{
  تهی *entry;
  uint32_t cpid;

  اگر(!cpool)
    بازگشت FALSE;

  اگر(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    انجامبده {
      ساختار connectdata *conn = entry;
      اگر(conn && func(cpool, data, conn, param) == 1)
        بازگشت TRUE;
    } وقتی(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  بازگشت FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ایستا bool cpool_conn_now_idle(ساختار cpool *cpool,
                                ساختار Curl_easy *data,
                                ساختار connectdata *conn,
                                ثابت ساختار curltime *pnow)
{
  ساختار connectdata *oldest_idle = پوچ;
  ساختار Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  اگر(!data || !data->multi)
    بازگشت kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  اگر(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  وگرنه {
    maxconnects = data->multi->maxconnects;
  }

  اگر(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t رقم_conns = Curl_uint32_tbl_شمار(&cpool->conns);
    admin = Curl_get_admin(data);
    اگر(رقم_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            رقم_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      اگر(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  بازگشت kept;
}

ایستا تهی cpool_prune_dead(ساختار cpool *cpool,
                             ساختار Curl_easy *data,
                             ثابت ساختار curltime *pnow);

ایستا bool cpool_find_act(ساختار cpool *cpool,
                           ساختار Curl_easy *data,
                           ساختار connectdata *conn,
                           cpool_match_result match)
{
  انتخاب(match) {
  حالت CPOOL_MATCH_FOUND:
    بازگشت TRUE;
  حالت CPOOL_MATCH_TOO_OLD:
    اگر(!cpool_conn_can_be_closed(conn))
      بازگشت FALSE;
    FALLTHROUGH();
  حالت CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت FALSE;
}

bool Curl_cpool_find(ساختار Curl_easy *data,
                     ثابت نویسه *destination,
                     bool prune_dead,
                     ثابت ساختار curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     تهی *userdata)
{
  ساختار cpool *cpool = cpool_get_instance(data);
  ساختار cpool_bundle *bundle = پوچ;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  اگر(!cpool)
    بازگشت FALSE;

  CPOOL_LOCK(cpool, data);

  اگر(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  اگر(data->state.last_cpid != UINT32_MAX) {
    ساختار connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    اگر(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      اگر(found)
        برو out;
    }
    وگرنه {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  اگر(bundle) {
    uint32_t cpid;
    اگر(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      انجامبده {
        ساختار connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        اگر(conn) {
          اگر(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            اگر(found)
              برو out;
          }
        }
        وگرنه {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } وقتی(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  اگر(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  بازگشت found;
}

ساختار cpool_reaper_ctx {
  size_t reaped;
  ثابت ساختار curltime *pnow;
};

ایستا صحیح cpool_reap_dead_cb(ساختار cpool *cpool,
                              ساختار Curl_easy *admin,
                              ساختار connectdata *conn, تهی *param)
{
  ساختار cpool_reaper_ctx *reaper = param;

  اگر(cpool_conn_can_be_closed(conn)) {
    اگر(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  بازگشت 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ایستا تهی cpool_prune_dead(ساختار cpool *cpool,
                             ساختار Curl_easy *data,
                             ثابت ساختار curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  اگر(elapsed_ms >= 1000L) {
    ساختار Curl_easy *admin = Curl_get_admin(data);
    ساختار cpool_reaper_ctx reaper;

    memset(&reaper, 0, اندازه(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

ایستا صحیح conn_upkeep(ساختار cpool *cpool,
                       ساختار Curl_easy *admin,
                       ساختار connectdata *conn,
                       تهی *param)
{
  ثابت ساختار curltime *pnow = Curl_pgrs_now(admin);

  (تهی)param;
  اگر((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    اگر(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  بازگشت 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(ساختار Curl_easy *data)
{
  ساختار cpool *cpool = cpool_get_instance(data);
  ساختار Curl_easy *admin = Curl_get_admin(data);

  اگر(!cpool)
    بازگشت CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, پوچ, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  بازگشت CURLE_OK;
}

ساختار connectdata *Curl_cpool_get_last_conn(ساختار Curl_easy *data)
{
  ساختار cpool *cpool = cpool_get_instance(data);
  اگر(cpool && (data->state.last_cpid != UINT32_MAX)) {
    ساختار connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    اگر(conn && (data->state.last_conn_id == conn->connection_id))
      بازگشت conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  بازگشت پوچ;
}

تهی Curl_cpool_return(ساختار Curl_easy *data,
                       ساختار connectdata *conn,
                       Curl_cpool_return_cb *cb, تهی *cbdata,
                       ثابت ساختار curltime *pnow)
{
  ساختار cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  انتخاب(cb(data, conn, cbdata, pnow)) {
  حالت CPOOL_DO_KEEP:
    بشکن;
  حالت CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    اگر(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    وگرنه { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    بشکن;
  حالت CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    بشکن;
  حالت CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    بشکن;
  }

  CPOOL_UNLOCK(cpool, data);
}

ایستا صحیح cpool_reap_no_reuse(ساختار cpool *cpool,
                               ساختار Curl_easy *admin,
                               ساختار connectdata *conn, تهی *param)
{
  (تهی)param;
  conn->bits.no_reuse = TRUE;
  اگر(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  بازگشت 0; /* continue iteration */
}

تهی Curl_cpool_nw_changed(ساختار cpool *cpool, ساختار Curl_easy *admin)
{
  اگر(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, پوچ, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ایستا bool cpool_conn_too_old(ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               ثابت ساختار curltime *pnow)
{
  timediff_t age_ms;

  اگر(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    اگر(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      بازگشت TRUE;
    }
  }

  اگر(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    اگر(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      بازگشت TRUE;
    }
  }

  بازگشت FALSE;
}

bool Curl_cpool_conn_seems_healthy(ساختار connectdata *conn,
                                   ساختار Curl_easy *data,
                                   ثابت ساختار curltime *pnow)
{
  ساختار Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_درون_داد;

  DEBUGASSERT(!data->conn);
  اگر(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    بازگشت FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_درون_داد = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  اگر(!poll_درون_داد &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    بازگشت TRUE;

  admin = Curl_get_admin(data);
  اگر(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  وگرنه {
    bool درون_داد_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &درون_داد_pending);
    Curl_detach_connection(admin);
    اگر(healthy && درون_داد_pending && poll_درون_داد) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  اگر(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  بازگشت healthy;
}

تهی Curl_cpool_conn_was_used(ساختار Curl_easy *data,
                              ساختار connectdata *conn,
                              ثابت ساختار curltime *pnow)
{
  (تهی)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  ثابت ساختار curltime *pnow)
{
  (تهی)data;
  بازگشت curlx_ptimediff_ms(pnow, &conn->created);
}
