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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cshutdn.h"
#अन्तर्भाव "conncache.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "sigpipe.h"
#अन्तर्भाव "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  करेहि {                                                                  \
    जइ(c) {                                                             \
      जइ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } जाव(0)

#define CPOOL_UNLOCK(c, d)                                              \
  करेहि {                                                                  \
    जइ(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      जइ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } जाव(0)

/* A set of connection pool ids to the same destination. */
संरचना cpool_bundle {
  संरचना uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

स्थिर शून्य cpool_bundle_unlink(संरचना cpool_bundle **pbundle)
{
  जइ(*pbundle) {
    संरचना cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = रिक्त;
    जइ(bundle->refcount)
      bundle->refcount--;
    जइ(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

स्थिर शून्य cpool_bundle_link(संरचना cpool_bundle **pdest,
                              संरचना cpool_bundle *src)
{
  जइ(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    जइ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

स्थिर संरचना cpool_bundle *cpool_create_bundle(शून्य)
{
  /* created with refcount == 1 */
  संरचना cpool_bundle *bundle = curlx_calloc(1, परिमाण(*bundle));
  जइ(!bundle)
    प्रत्यागम रिक्त;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  प्रत्यागम bundle;
}

/* Add a connection to a bundle */
स्थिर bool cpool_bundle_add_id(संरचना cpool *cpool,
                                संरचना cpool_bundle *bundle,
                                ध्रुव वर्ण *destination,
                                uint32_t pool_id)
{
  जइ(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    प्रत्यागम FALSE;
  जइ(Curl_uint32_spbset_गणनम्(&bundle->ids) == 1) {
    /* first one added, register bundle */
    संरचना cpool_bundle *new_ref = रिक्त;
    cpool_bundle_link(&new_ref, bundle);
    जइ(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
}

/* Remove a connection from a bundle */
स्थिर शून्य cpool_bundle_remove_id(संरचना cpool *cpool,
                                   संरचना cpool_bundle *bundle,
                                   ध्रुव वर्ण *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  जइ(!Curl_uint32_spbset_गणनम्(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

स्थिर शून्य cpool_bundle_hash_dtor(शून्य *entry)
{
  संरचना cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

स्थिर संरचना cpool_bundle *cpool_get_bundle(संरचना cpool *cpool,
                                             ध्रुव वर्ण *destination)
{
  संरचना cpool_bundle *bundle = रिक्त;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  प्रत्यागम bundle;
}

स्थिर शून्य cpool_remove_conn(संरचना cpool *cpool,
                              संरचना connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  जइ(cpid != UINT32_MAX) {
    संरचना cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      संरचना connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    जइ(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

स्थिर शून्य cpool_discard_conn(संरचना cpool *cpool,
                               संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               bool aborted)
{
  संरचना Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  जइ(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    प्रत्यागम;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  जइ(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  जइ(aborted)
    done = TRUE;
  जइ(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  जइ(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  अण्णहा {
    संरचना Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);

    जइ(अङ्कः_conns < max_shutdowns)
      max_shutdowns -= अङ्कः_conns;
    अण्णहा जइ(max_shutdowns)
      max_shutdowns = 1;
    अण्णहा /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(अङ्कः_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

शून्य Curl_cpool_init(संरचना cpool *cpool,
                     संरचना Curl_share *share,
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

शून्य Curl_cpool_destroy(संरचना cpool *cpool, संरचना Curl_easy *admin)
{
  जइ(cpool && cpool->initialized && admin) {
    शून्य *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_गणनम्(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    जइ(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      संरचना Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      करेहि {
        संरचना connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } जाव(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

स्थिर संरचना cpool *cpool_get_instance(संरचना Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  जइ(CURL_SHARE_KEEP_CONNECT(data->share))
    प्रत्यागम &data->share->cpool;
  अण्णहा जइ(data->multi_easy)
    प्रत्यागम &data->multi_easy->cpool;
  अण्णहा जइ(data->multi)
    प्रत्यागम &data->multi->cpool;
  प्रत्यागम रिक्त;
}

संरचना cpool *Curl_cpool_get_instance(संरचना Curl_easy *data)
{
  प्रत्यागम cpool_get_instance(data);
}

शून्य Curl_cpool_xfer_init(संरचना Curl_easy *data)
{
  संरचना cpool *cpool = cpool_get_instance(data);

  जइ(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    जइ(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  अण्णहा {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

स्थिर bool cpool_conn_can_be_closed(संरचना connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  प्रत्यागम (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

स्थिर संरचना connectdata *cpool_bundle_get_oldest_idle(
  संरचना cpool *cpool,
  संरचना cpool_bundle *bundle,
  ध्रुव संरचना curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  जइ(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    करेहि {
      संरचना connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      जइ(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        जइ(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      अण्णहा जइ(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } जाव(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  प्रत्यागम Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

स्थिर संरचना connectdata *cpool_get_oldest_idle(संरचना cpool *cpool,
                                                 ध्रुव संरचना curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  जइ(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    करेहि {
      संरचना connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      जइ(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        जइ((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      अण्णहा {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } जाव(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  प्रत्यागम Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

स्थिर शून्य cpool_conn_close(संरचना cpool *cpool,
                             संरचना Curl_easy *data,
                             संरचना connectdata *conn,
                             bool aborted)
{
  संरचना Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  जइ(!cpool)
    प्रत्यागम;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  जइ(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    प्रत्यागम;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  जइ(do_lock)
    CPOOL_LOCK(cpool, admin);

  जइ(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  जइ(conn->bits.connect_only)
    aborted = TRUE;

  जइ(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  अण्णहा {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  जइ(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

शून्य Curl_conn_close(संरचना Curl_easy *data,
                     संरचना connectdata *conn,
                     bool aborted)
{
  संरचना cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
स्थिर शून्य cpool_evict_conn(संरचना cpool *cpool,
                             संरचना Curl_easy *admin,
                             संरचना connectdata *conn)
{
  जइ(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  अण्णहा
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

स्थिर पूर्णाङ्क cpool_check_limits(संरचना Curl_easy *data,
                              uint32_t max_total,
                              संरचना connectdata *to_add,
                              uint32_t max_host,
                              संरचना cpool_bundle *bundle,
                              ध्रुव संरचना curltime *pnow)
{
  संरचना cpool *cpool = cpool_get_instance(data);
  संरचना cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : रिक्त;
  संरचना Curl_easy *admin = रिक्त;

  जइ(!cpool)
    प्रत्यागम CPOOL_LIMIT_OK;

  जइ(!max_total && !max_host)
    प्रत्यागम CPOOL_LIMIT_OK;

  जइ(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    जइ(bundle) {
      live = Curl_uint32_spbset_गणनम्(&bundle->ids);
      जइ(live >= max_host) {
        size_t over = live - max_host + 1;
        प्रति(; over; --over) {
          संरचना connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          जइ(!oldest_idle)
            भङ्ग;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_गणनम्(&bundle->ids);
      }
    }
    जइ(cshutdn) {
      shutdowns = Curl_cshutdn_dest_गणनम्(cshutdn, to_add->destination);
      जइ(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    जइ((live + shutdowns) >= max_host)
      प्रत्यागम CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  जइ(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);

    जइ(!admin)
      admin = Curl_get_admin(data);
    जइ(अङ्कः_conns >= max_total) {
      size_t over = अङ्कः_conns - max_total + 1;
      प्रति(; over; --over) {
        संरचना connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        जइ(!oldest_idle)
          भङ्ग;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);
    जइ(cshutdn) {
      shutdowns = Curl_cshutdn_गणनम्(cshutdn);
      जइ(shutdowns && (अङ्कः_conns + shutdowns) >= max_total) {
        size_t over = अङ्कः_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, रिक्त, over);
      }
    }
    जइ((अङ्कः_conns + shutdowns) >= max_total)
      प्रत्यागम CPOOL_LIMIT_TOTAL;
  }

  प्रत्यागम CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(संरचना Curl_easy *data,
                        संरचना connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        ध्रुव संरचना curltime *pnow)
{
  CURLcode result = CURLE_OK;
  संरचना cpool_bundle *bundle = रिक्त;
  संरचना cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  जइ(!cpool)
    प्रत्यागम CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  विकल्प(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  पक्ष CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    गच्छ out;
  पक्ष CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    गच्छ out;
  स्वभाव:
    भङ्ग;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  जइ(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    गच्छ out;
  }
  जइ(Curl_uint32_tbl_गणनम्(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    जइ((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    अण्णहा
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    जइ(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

  जइ(!bundle) {
    bundle = cpool_create_bundle();
    जइ(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

  जइ(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  conn->cpid = cpid;

  जइ(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_गणनम्(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  प्रत्यागम result;
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
स्थिर bool cpool_foreach(संरचना Curl_easy *data,
                          संरचना cpool *cpool,
                          शून्य *param,
                          पूर्णाङ्क (*func)(संरचना cpool *cpool,
                                      संरचना Curl_easy *data,
                                      संरचना connectdata *conn, शून्य *param))
{
  शून्य *entry;
  uint32_t cpid;

  जइ(!cpool)
    प्रत्यागम FALSE;

  जइ(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    करेहि {
      संरचना connectdata *conn = entry;
      जइ(conn && func(cpool, data, conn, param) == 1)
        प्रत्यागम TRUE;
    } जाव(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  प्रत्यागम FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
स्थिर bool cpool_conn_now_idle(संरचना cpool *cpool,
                                संरचना Curl_easy *data,
                                संरचना connectdata *conn,
                                ध्रुव संरचना curltime *pnow)
{
  संरचना connectdata *oldest_idle = रिक्त;
  संरचना Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  जइ(!data || !data->multi)
    प्रत्यागम kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  जइ(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  अण्णहा {
    maxconnects = data->multi->maxconnects;
  }

  जइ(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);
    admin = Curl_get_admin(data);
    जइ(अङ्कः_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            अङ्कः_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      जइ(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  प्रत्यागम kept;
}

स्थिर शून्य cpool_prune_dead(संरचना cpool *cpool,
                             संरचना Curl_easy *data,
                             ध्रुव संरचना curltime *pnow);

स्थिर bool cpool_find_act(संरचना cpool *cpool,
                           संरचना Curl_easy *data,
                           संरचना connectdata *conn,
                           cpool_match_result match)
{
  विकल्प(match) {
  पक्ष CPOOL_MATCH_FOUND:
    प्रत्यागम TRUE;
  पक्ष CPOOL_MATCH_TOO_OLD:
    जइ(!cpool_conn_can_be_closed(conn))
      प्रत्यागम FALSE;
    FALLTHROUGH();
  पक्ष CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम FALSE;
}

bool Curl_cpool_find(संरचना Curl_easy *data,
                     ध्रुव वर्ण *destination,
                     bool prune_dead,
                     ध्रुव संरचना curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     शून्य *userdata)
{
  संरचना cpool *cpool = cpool_get_instance(data);
  संरचना cpool_bundle *bundle = रिक्त;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  जइ(!cpool)
    प्रत्यागम FALSE;

  CPOOL_LOCK(cpool, data);

  जइ(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  जइ(data->state.last_cpid != UINT32_MAX) {
    संरचना connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    जइ(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      जइ(found)
        गच्छ out;
    }
    अण्णहा {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  जइ(bundle) {
    uint32_t cpid;
    जइ(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      करेहि {
        संरचना connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        जइ(conn) {
          जइ(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            जइ(found)
              गच्छ out;
          }
        }
        अण्णहा {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } जाव(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  जइ(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  प्रत्यागम found;
}

संरचना cpool_reaper_ctx {
  size_t reaped;
  ध्रुव संरचना curltime *pnow;
};

स्थिर पूर्णाङ्क cpool_reap_dead_cb(संरचना cpool *cpool,
                              संरचना Curl_easy *admin,
                              संरचना connectdata *conn, शून्य *param)
{
  संरचना cpool_reaper_ctx *reaper = param;

  जइ(cpool_conn_can_be_closed(conn)) {
    जइ(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  प्रत्यागम 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
स्थिर शून्य cpool_prune_dead(संरचना cpool *cpool,
                             संरचना Curl_easy *data,
                             ध्रुव संरचना curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  जइ(elapsed_ms >= 1000L) {
    संरचना Curl_easy *admin = Curl_get_admin(data);
    संरचना cpool_reaper_ctx reaper;

    memset(&reaper, 0, परिमाण(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

स्थिर पूर्णाङ्क conn_upkeep(संरचना cpool *cpool,
                       संरचना Curl_easy *admin,
                       संरचना connectdata *conn,
                       शून्य *param)
{
  ध्रुव संरचना curltime *pnow = Curl_pgrs_now(admin);

  (शून्य)param;
  जइ((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    जइ(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  प्रत्यागम 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(संरचना Curl_easy *data)
{
  संरचना cpool *cpool = cpool_get_instance(data);
  संरचना Curl_easy *admin = Curl_get_admin(data);

  जइ(!cpool)
    प्रत्यागम CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, रिक्त, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  प्रत्यागम CURLE_OK;
}

संरचना connectdata *Curl_cpool_get_last_conn(संरचना Curl_easy *data)
{
  संरचना cpool *cpool = cpool_get_instance(data);
  जइ(cpool && (data->state.last_cpid != UINT32_MAX)) {
    संरचना connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    जइ(conn && (data->state.last_conn_id == conn->connection_id))
      प्रत्यागम conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  प्रत्यागम रिक्त;
}

शून्य Curl_cpool_return(संरचना Curl_easy *data,
                       संरचना connectdata *conn,
                       Curl_cpool_return_cb *cb, शून्य *cbdata,
                       ध्रुव संरचना curltime *pnow)
{
  संरचना cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  विकल्प(cb(data, conn, cbdata, pnow)) {
  पक्ष CPOOL_DO_KEEP:
    भङ्ग;
  पक्ष CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    जइ(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    अण्णहा { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    भङ्ग;
  पक्ष CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    भङ्ग;
  पक्ष CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    भङ्ग;
  }

  CPOOL_UNLOCK(cpool, data);
}

स्थिर पूर्णाङ्क cpool_reap_no_reuse(संरचना cpool *cpool,
                               संरचना Curl_easy *admin,
                               संरचना connectdata *conn, शून्य *param)
{
  (शून्य)param;
  conn->bits.no_reuse = TRUE;
  जइ(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  प्रत्यागम 0; /* continue iteration */
}

शून्य Curl_cpool_nw_changed(संरचना cpool *cpool, संरचना Curl_easy *admin)
{
  जइ(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, रिक्त, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
स्थिर bool cpool_conn_too_old(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               ध्रुव संरचना curltime *pnow)
{
  timediff_t age_ms;

  जइ(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    जइ(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      प्रत्यागम TRUE;
    }
  }

  जइ(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    जइ(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      प्रत्यागम TRUE;
    }
  }

  प्रत्यागम FALSE;
}

bool Curl_cpool_conn_seems_healthy(संरचना connectdata *conn,
                                   संरचना Curl_easy *data,
                                   ध्रुव संरचना curltime *pnow)
{
  संरचना Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_निवेशः;

  DEBUGASSERT(!data->conn);
  जइ(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    प्रत्यागम FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_निवेशः = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  जइ(!poll_निवेशः &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    प्रत्यागम TRUE;

  admin = Curl_get_admin(data);
  जइ(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  अण्णहा {
    bool निवेशः_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &निवेशः_pending);
    Curl_detach_connection(admin);
    जइ(healthy && निवेशः_pending && poll_निवेशः) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  जइ(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  प्रत्यागम healthy;
}

शून्य Curl_cpool_conn_was_used(संरचना Curl_easy *data,
                              संरचना connectdata *conn,
                              ध्रुव संरचना curltime *pnow)
{
  (शून्य)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  ध्रुव संरचना curltime *pnow)
{
  (शून्य)data;
  प्रत्यागम curlx_ptimediff_ms(pnow, &conn->created);
}
