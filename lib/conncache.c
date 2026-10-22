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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "cshutdn.h"
#அந்தர்பாவ "conncache.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "sigpipe.h"
#அந்தர்பாவ "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  செய் {                                                                  \
    எனில்(c) {                                                             \
      எனில்(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } போது(0)

#define CPOOL_UNLOCK(c, d)                                              \
  செய் {                                                                  \
    எனில்(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      எனில்(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } போது(0)

/* A set of connection pool ids to the same destination. */
கட்டமைப்பு cpool_bundle {
  கட்டமைப்பு uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

நிலையான வெறுமை cpool_bundle_unlink(கட்டமைப்பு cpool_bundle **pbundle)
{
  எனில்(*pbundle) {
    கட்டமைப்பு cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = வெற்று;
    எனில்(bundle->refcount)
      bundle->refcount--;
    எனில்(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

நிலையான வெறுமை cpool_bundle_link(கட்டமைப்பு cpool_bundle **pdest,
                              கட்டமைப்பு cpool_bundle *src)
{
  எனில்(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    எனில்(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

நிலையான கட்டமைப்பு cpool_bundle *cpool_create_bundle(வெறுமை)
{
  /* created with refcount == 1 */
  கட்டமைப்பு cpool_bundle *bundle = curlx_calloc(1, அளவு(*bundle));
  எனில்(!bundle)
    திரும்பு வெற்று;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  திரும்பு bundle;
}

/* Add a connection to a bundle */
நிலையான bool cpool_bundle_add_id(கட்டமைப்பு cpool *cpool,
                                கட்டமைப்பு cpool_bundle *bundle,
                                மாறிலி எழுத்து *destination,
                                uint32_t pool_id)
{
  எனில்(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    திரும்பு FALSE;
  எனில்(Curl_uint32_spbset_மொத்தம்(&bundle->ids) == 1) {
    /* first one added, register bundle */
    கட்டமைப்பு cpool_bundle *new_ref = வெற்று;
    cpool_bundle_link(&new_ref, bundle);
    எனில்(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      திரும்பு FALSE;
    }
  }
  திரும்பு TRUE;
}

/* Remove a connection from a bundle */
நிலையான வெறுமை cpool_bundle_remove_id(கட்டமைப்பு cpool *cpool,
                                   கட்டமைப்பு cpool_bundle *bundle,
                                   மாறிலி எழுத்து *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  எனில்(!Curl_uint32_spbset_மொத்தம்(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

நிலையான வெறுமை cpool_bundle_hash_dtor(வெறுமை *entry)
{
  கட்டமைப்பு cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

நிலையான கட்டமைப்பு cpool_bundle *cpool_get_bundle(கட்டமைப்பு cpool *cpool,
                                             மாறிலி எழுத்து *destination)
{
  கட்டமைப்பு cpool_bundle *bundle = வெற்று;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  திரும்பு bundle;
}

நிலையான வெறுமை cpool_remove_conn(கட்டமைப்பு cpool *cpool,
                              கட்டமைப்பு connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  எனில்(cpid != UINT32_MAX) {
    கட்டமைப்பு cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      கட்டமைப்பு connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    எனில்(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

நிலையான வெறுமை cpool_discard_conn(கட்டமைப்பு cpool *cpool,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               bool aborted)
{
  கட்டமைப்பு Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  எனில்(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    திரும்பு;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  எனில்(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  எனில்(aborted)
    done = TRUE;
  எனில்(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  எனில்(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  இல்லையெனில் {
    கட்டமைப்பு Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t இலக்கம்_conns = Curl_uint32_tbl_மொத்தம்(&cpool->conns);

    எனில்(இலக்கம்_conns < max_shutdowns)
      max_shutdowns -= இலக்கம்_conns;
    இல்லையெனில் எனில்(max_shutdowns)
      max_shutdowns = 1;
    இல்லையெனில் /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(இலக்கம்_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

வெறுமை Curl_cpool_init(கட்டமைப்பு cpool *cpool,
                     கட்டமைப்பு Curl_share *share,
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

வெறுமை Curl_cpool_destroy(கட்டமைப்பு cpool *cpool, கட்டமைப்பு Curl_easy *admin)
{
  எனில்(cpool && cpool->initialized && admin) {
    வெறுமை *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_மொத்தம்(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    எனில்(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      கட்டமைப்பு Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      செய் {
        கட்டமைப்பு connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } போது(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

நிலையான கட்டமைப்பு cpool *cpool_get_instance(கட்டமைப்பு Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  எனில்(CURL_SHARE_KEEP_CONNECT(data->share))
    திரும்பு &data->share->cpool;
  இல்லையெனில் எனில்(data->multi_easy)
    திரும்பு &data->multi_easy->cpool;
  இல்லையெனில் எனில்(data->multi)
    திரும்பு &data->multi->cpool;
  திரும்பு வெற்று;
}

கட்டமைப்பு cpool *Curl_cpool_get_instance(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு cpool_get_instance(data);
}

வெறுமை Curl_cpool_xfer_init(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);

  எனில்(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    எனில்(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  இல்லையெனில் {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

நிலையான bool cpool_conn_can_be_closed(கட்டமைப்பு connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  திரும்பு (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

நிலையான கட்டமைப்பு connectdata *cpool_bundle_get_oldest_idle(
  கட்டமைப்பு cpool *cpool,
  கட்டமைப்பு cpool_bundle *bundle,
  மாறிலி கட்டமைப்பு curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  எனில்(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    செய் {
      கட்டமைப்பு connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      எனில்(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        எனில்(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      இல்லையெனில் எனில்(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } போது(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  திரும்பு Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

நிலையான கட்டமைப்பு connectdata *cpool_get_oldest_idle(கட்டமைப்பு cpool *cpool,
                                                 மாறிலி கட்டமைப்பு curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  எனில்(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    செய் {
      கட்டமைப்பு connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      எனில்(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        எனில்((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      இல்லையெனில் {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } போது(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  திரும்பு Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

நிலையான வெறுமை cpool_conn_close(கட்டமைப்பு cpool *cpool,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn,
                             bool aborted)
{
  கட்டமைப்பு Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  எனில்(!cpool)
    திரும்பு;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  எனில்(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    திரும்பு;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  எனில்(do_lock)
    CPOOL_LOCK(cpool, admin);

  எனில்(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  எனில்(conn->bits.connect_only)
    aborted = TRUE;

  எனில்(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  இல்லையெனில் {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  எனில்(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

வெறுமை Curl_conn_close(கட்டமைப்பு Curl_easy *data,
                     கட்டமைப்பு connectdata *conn,
                     bool aborted)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
நிலையான வெறுமை cpool_evict_conn(கட்டமைப்பு cpool *cpool,
                             கட்டமைப்பு Curl_easy *admin,
                             கட்டமைப்பு connectdata *conn)
{
  எனில்(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  இல்லையெனில்
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

நிலையான முழு cpool_check_limits(கட்டமைப்பு Curl_easy *data,
                              uint32_t max_total,
                              கட்டமைப்பு connectdata *to_add,
                              uint32_t max_host,
                              கட்டமைப்பு cpool_bundle *bundle,
                              மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);
  கட்டமைப்பு cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : வெற்று;
  கட்டமைப்பு Curl_easy *admin = வெற்று;

  எனில்(!cpool)
    திரும்பு CPOOL_LIMIT_OK;

  எனில்(!max_total && !max_host)
    திரும்பு CPOOL_LIMIT_OK;

  எனில்(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    எனில்(bundle) {
      live = Curl_uint32_spbset_மொத்தம்(&bundle->ids);
      எனில்(live >= max_host) {
        size_t over = live - max_host + 1;
        க்கு(; over; --over) {
          கட்டமைப்பு connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          எனில்(!oldest_idle)
            நிறுத்து;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_மொத்தம்(&bundle->ids);
      }
    }
    எனில்(cshutdn) {
      shutdowns = Curl_cshutdn_dest_மொத்தம்(cshutdn, to_add->destination);
      எனில்(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    எனில்((live + shutdowns) >= max_host)
      திரும்பு CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  எனில்(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t இலக்கம்_conns = Curl_uint32_tbl_மொத்தம்(&cpool->conns);

    எனில்(!admin)
      admin = Curl_get_admin(data);
    எனில்(இலக்கம்_conns >= max_total) {
      size_t over = இலக்கம்_conns - max_total + 1;
      க்கு(; over; --over) {
        கட்டமைப்பு connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        எனில்(!oldest_idle)
          நிறுத்து;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    இலக்கம்_conns = Curl_uint32_tbl_மொத்தம்(&cpool->conns);
    எனில்(cshutdn) {
      shutdowns = Curl_cshutdn_மொத்தம்(cshutdn);
      எனில்(shutdowns && (இலக்கம்_conns + shutdowns) >= max_total) {
        size_t over = இலக்கம்_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, வெற்று, over);
      }
    }
    எனில்((இலக்கம்_conns + shutdowns) >= max_total)
      திரும்பு CPOOL_LIMIT_TOTAL;
  }

  திரும்பு CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        மாறிலி கட்டமைப்பு curltime *pnow)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cpool_bundle *bundle = வெற்று;
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  எனில்(!cpool)
    திரும்பு CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  தேர்வு(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  நிலை CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    செல் out;
  நிலை CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    செல் out;
  இயல்பு:
    நிறுத்து;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  எனில்(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    செல் out;
  }
  எனில்(Curl_uint32_tbl_மொத்தம்(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    எனில்((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    இல்லையெனில்
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    எனில்(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

  எனில்(!bundle) {
    bundle = cpool_create_bundle();
    எனில்(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

  எனில்(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  conn->cpid = cpid;

  எனில்(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_மொத்தம்(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  திரும்பு result;
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
நிலையான bool cpool_foreach(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு cpool *cpool,
                          வெறுமை *param,
                          முழு (*func)(கட்டமைப்பு cpool *cpool,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn, வெறுமை *param))
{
  வெறுமை *entry;
  uint32_t cpid;

  எனில்(!cpool)
    திரும்பு FALSE;

  எனில்(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    செய் {
      கட்டமைப்பு connectdata *conn = entry;
      எனில்(conn && func(cpool, data, conn, param) == 1)
        திரும்பு TRUE;
    } போது(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  திரும்பு FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
நிலையான bool cpool_conn_now_idle(கட்டமைப்பு cpool *cpool,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn,
                                மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு connectdata *oldest_idle = வெற்று;
  கட்டமைப்பு Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  எனில்(!data || !data->multi)
    திரும்பு kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  எனில்(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  இல்லையெனில் {
    maxconnects = data->multi->maxconnects;
  }

  எனில்(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t இலக்கம்_conns = Curl_uint32_tbl_மொத்தம்(&cpool->conns);
    admin = Curl_get_admin(data);
    எனில்(இலக்கம்_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            இலக்கம்_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      எனில்(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  திரும்பு kept;
}

நிலையான வெறுமை cpool_prune_dead(கட்டமைப்பு cpool *cpool,
                             கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு curltime *pnow);

நிலையான bool cpool_find_act(கட்டமைப்பு cpool *cpool,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு connectdata *conn,
                           cpool_match_result match)
{
  தேர்வு(match) {
  நிலை CPOOL_MATCH_FOUND:
    திரும்பு TRUE;
  நிலை CPOOL_MATCH_TOO_OLD:
    எனில்(!cpool_conn_can_be_closed(conn))
      திரும்பு FALSE;
    FALLTHROUGH();
  நிலை CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு FALSE;
}

bool Curl_cpool_find(கட்டமைப்பு Curl_easy *data,
                     மாறிலி எழுத்து *destination,
                     bool prune_dead,
                     மாறிலி கட்டமைப்பு curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     வெறுமை *userdata)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);
  கட்டமைப்பு cpool_bundle *bundle = வெற்று;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  எனில்(!cpool)
    திரும்பு FALSE;

  CPOOL_LOCK(cpool, data);

  எனில்(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  எனில்(data->state.last_cpid != UINT32_MAX) {
    கட்டமைப்பு connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    எனில்(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      எனில்(found)
        செல் out;
    }
    இல்லையெனில் {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  எனில்(bundle) {
    uint32_t cpid;
    எனில்(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      செய் {
        கட்டமைப்பு connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        எனில்(conn) {
          எனில்(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            எனில்(found)
              செல் out;
          }
        }
        இல்லையெனில் {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } போது(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  எனில்(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  திரும்பு found;
}

கட்டமைப்பு cpool_reaper_ctx {
  size_t reaped;
  மாறிலி கட்டமைப்பு curltime *pnow;
};

நிலையான முழு cpool_reap_dead_cb(கட்டமைப்பு cpool *cpool,
                              கட்டமைப்பு Curl_easy *admin,
                              கட்டமைப்பு connectdata *conn, வெறுமை *param)
{
  கட்டமைப்பு cpool_reaper_ctx *reaper = param;

  எனில்(cpool_conn_can_be_closed(conn)) {
    எனில்(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  திரும்பு 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
நிலையான வெறுமை cpool_prune_dead(கட்டமைப்பு cpool *cpool,
                             கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  எனில்(elapsed_ms >= 1000L) {
    கட்டமைப்பு Curl_easy *admin = Curl_get_admin(data);
    கட்டமைப்பு cpool_reaper_ctx reaper;

    memset(&reaper, 0, அளவு(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

நிலையான முழு conn_upkeep(கட்டமைப்பு cpool *cpool,
                       கட்டமைப்பு Curl_easy *admin,
                       கட்டமைப்பு connectdata *conn,
                       வெறுமை *param)
{
  மாறிலி கட்டமைப்பு curltime *pnow = Curl_pgrs_now(admin);

  (வெறுமை)param;
  எனில்((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    எனில்(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  திரும்பு 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);
  கட்டமைப்பு Curl_easy *admin = Curl_get_admin(data);

  எனில்(!cpool)
    திரும்பு CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, வெற்று, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  திரும்பு CURLE_OK;
}

கட்டமைப்பு connectdata *Curl_cpool_get_last_conn(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);
  எனில்(cpool && (data->state.last_cpid != UINT32_MAX)) {
    கட்டமைப்பு connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    எனில்(conn && (data->state.last_conn_id == conn->connection_id))
      திரும்பு conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  திரும்பு வெற்று;
}

வெறுமை Curl_cpool_return(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு connectdata *conn,
                       Curl_cpool_return_cb *cb, வெறுமை *cbdata,
                       மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  தேர்வு(cb(data, conn, cbdata, pnow)) {
  நிலை CPOOL_DO_KEEP:
    நிறுத்து;
  நிலை CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    எனில்(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    இல்லையெனில் { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    நிறுத்து;
  நிலை CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    நிறுத்து;
  நிலை CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    நிறுத்து;
  }

  CPOOL_UNLOCK(cpool, data);
}

நிலையான முழு cpool_reap_no_reuse(கட்டமைப்பு cpool *cpool,
                               கட்டமைப்பு Curl_easy *admin,
                               கட்டமைப்பு connectdata *conn, வெறுமை *param)
{
  (வெறுமை)param;
  conn->bits.no_reuse = TRUE;
  எனில்(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  திரும்பு 0; /* continue iteration */
}

வெறுமை Curl_cpool_nw_changed(கட்டமைப்பு cpool *cpool, கட்டமைப்பு Curl_easy *admin)
{
  எனில்(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, வெற்று, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
நிலையான bool cpool_conn_too_old(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               மாறிலி கட்டமைப்பு curltime *pnow)
{
  timediff_t age_ms;

  எனில்(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    எனில்(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      திரும்பு TRUE;
    }
  }

  எனில்(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    எனில்(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      திரும்பு TRUE;
    }
  }

  திரும்பு FALSE;
}

bool Curl_cpool_conn_seems_healthy(கட்டமைப்பு connectdata *conn,
                                   கட்டமைப்பு Curl_easy *data,
                                   மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_உள்ளீடு;

  DEBUGASSERT(!data->conn);
  எனில்(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    திரும்பு FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_உள்ளீடு = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  எனில்(!poll_உள்ளீடு &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    திரும்பு TRUE;

  admin = Curl_get_admin(data);
  எனில்(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  இல்லையெனில் {
    bool உள்ளீடு_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &உள்ளீடு_pending);
    Curl_detach_connection(admin);
    எனில்(healthy && உள்ளீடு_pending && poll_உள்ளீடு) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  எனில்(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  திரும்பு healthy;
}

வெறுமை Curl_cpool_conn_was_used(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn,
                              மாறிலி கட்டமைப்பு curltime *pnow)
{
  (வெறுமை)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  மாறிலி கட்டமைப்பு curltime *pnow)
{
  (வெறுமை)data;
  திரும்பு curlx_ptimediff_ms(pnow, &conn->created);
}
