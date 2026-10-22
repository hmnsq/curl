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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "cshutdn.h"
#અન્તર્ભાવ "conncache.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "sigpipe.h"
#અન્તર્ભાવ "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  કરો {                                                                  \
    જો(c) {                                                             \
      જો(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } જ્યારે(0)

#define CPOOL_UNLOCK(c, d)                                              \
  કરો {                                                                  \
    જો(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      જો(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } જ્યારે(0)

/* A set of connection pool ids to the same destination. */
સંરચના cpool_bundle {
  સંરચના uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

સ્થિર શૂન્ય cpool_bundle_unlink(સંરચના cpool_bundle **pbundle)
{
  જો(*pbundle) {
    સંરચના cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ખાલી;
    જો(bundle->refcount)
      bundle->refcount--;
    જો(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

સ્થિર શૂન્ય cpool_bundle_link(સંરચના cpool_bundle **pdest,
                              સંરચના cpool_bundle *src)
{
  જો(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    જો(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

સ્થિર સંરચના cpool_bundle *cpool_create_bundle(શૂન્ય)
{
  /* created with refcount == 1 */
  સંરચના cpool_bundle *bundle = curlx_calloc(1, કદ(*bundle));
  જો(!bundle)
    પાછા ખાલી;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  પાછા bundle;
}

/* Add a connection to a bundle */
સ્થિર bool cpool_bundle_add_id(સંરચના cpool *cpool,
                                સંરચના cpool_bundle *bundle,
                                અચલ અક્ષર *destination,
                                uint32_t pool_id)
{
  જો(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    પાછા FALSE;
  જો(Curl_uint32_spbset_ગણતરી(&bundle->ids) == 1) {
    /* first one added, register bundle */
    સંરચના cpool_bundle *new_ref = ખાલી;
    cpool_bundle_link(&new_ref, bundle);
    જો(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      પાછા FALSE;
    }
  }
  પાછા TRUE;
}

/* Remove a connection from a bundle */
સ્થિર શૂન્ય cpool_bundle_remove_id(સંરચના cpool *cpool,
                                   સંરચના cpool_bundle *bundle,
                                   અચલ અક્ષર *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  જો(!Curl_uint32_spbset_ગણતરી(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

સ્થિર શૂન્ય cpool_bundle_hash_dtor(શૂન્ય *entry)
{
  સંરચના cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

સ્થિર સંરચના cpool_bundle *cpool_get_bundle(સંરચના cpool *cpool,
                                             અચલ અક્ષર *destination)
{
  સંરચના cpool_bundle *bundle = ખાલી;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  પાછા bundle;
}

સ્થિર શૂન્ય cpool_remove_conn(સંરચના cpool *cpool,
                              સંરચના connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  જો(cpid != UINT32_MAX) {
    સંરચના cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      સંરચના connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    જો(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

સ્થિર શૂન્ય cpool_discard_conn(સંરચના cpool *cpool,
                               સંરચના Curl_easy *data,
                               સંરચના connectdata *conn,
                               bool aborted)
{
  સંરચના Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  જો(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    પાછા;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  જો(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  જો(aborted)
    done = TRUE;
  જો(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  જો(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  નહીંતર {
    સંરચના Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t અંક_conns = Curl_uint32_tbl_ગણતરી(&cpool->conns);

    જો(અંક_conns < max_shutdowns)
      max_shutdowns -= અંક_conns;
    નહીંતર જો(max_shutdowns)
      max_shutdowns = 1;
    નહીંતર /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(અંક_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

શૂન્ય Curl_cpool_init(સંરચના cpool *cpool,
                     સંરચના Curl_share *share,
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

શૂન્ય Curl_cpool_destroy(સંરચના cpool *cpool, સંરચના Curl_easy *admin)
{
  જો(cpool && cpool->initialized && admin) {
    શૂન્ય *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_ગણતરી(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    જો(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      સંરચના Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      કરો {
        સંરચના connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } જ્યારે(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

સ્થિર સંરચના cpool *cpool_get_instance(સંરચના Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  જો(CURL_SHARE_KEEP_CONNECT(data->share))
    પાછા &data->share->cpool;
  નહીંતર જો(data->multi_easy)
    પાછા &data->multi_easy->cpool;
  નહીંતર જો(data->multi)
    પાછા &data->multi->cpool;
  પાછા ખાલી;
}

સંરચના cpool *Curl_cpool_get_instance(સંરચના Curl_easy *data)
{
  પાછા cpool_get_instance(data);
}

શૂન્ય Curl_cpool_xfer_init(સંરચના Curl_easy *data)
{
  સંરચના cpool *cpool = cpool_get_instance(data);

  જો(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    જો(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  નહીંતર {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

સ્થિર bool cpool_conn_can_be_closed(સંરચના connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  પાછા (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

સ્થિર સંરચના connectdata *cpool_bundle_get_oldest_idle(
  સંરચના cpool *cpool,
  સંરચના cpool_bundle *bundle,
  અચલ સંરચના curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  જો(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    કરો {
      સંરચના connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      જો(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        જો(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      નહીંતર જો(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } જ્યારે(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  પાછા Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

સ્થિર સંરચના connectdata *cpool_get_oldest_idle(સંરચના cpool *cpool,
                                                 અચલ સંરચના curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  જો(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    કરો {
      સંરચના connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      જો(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        જો((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      નહીંતર {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } જ્યારે(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  પાછા Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

સ્થિર શૂન્ય cpool_conn_close(સંરચના cpool *cpool,
                             સંરચના Curl_easy *data,
                             સંરચના connectdata *conn,
                             bool aborted)
{
  સંરચના Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  જો(!cpool)
    પાછા;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  જો(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    પાછા;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  જો(do_lock)
    CPOOL_LOCK(cpool, admin);

  જો(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  જો(conn->bits.connect_only)
    aborted = TRUE;

  જો(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  નહીંતર {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  જો(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

શૂન્ય Curl_conn_close(સંરચના Curl_easy *data,
                     સંરચના connectdata *conn,
                     bool aborted)
{
  સંરચના cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
સ્થિર શૂન્ય cpool_evict_conn(સંરચના cpool *cpool,
                             સંરચના Curl_easy *admin,
                             સંરચના connectdata *conn)
{
  જો(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  નહીંતર
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

સ્થિર પૂર્ણાંક cpool_check_limits(સંરચના Curl_easy *data,
                              uint32_t max_total,
                              સંરચના connectdata *to_add,
                              uint32_t max_host,
                              સંરચના cpool_bundle *bundle,
                              અચલ સંરચના curltime *pnow)
{
  સંરચના cpool *cpool = cpool_get_instance(data);
  સંરચના cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : ખાલી;
  સંરચના Curl_easy *admin = ખાલી;

  જો(!cpool)
    પાછા CPOOL_LIMIT_OK;

  જો(!max_total && !max_host)
    પાછા CPOOL_LIMIT_OK;

  જો(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    જો(bundle) {
      live = Curl_uint32_spbset_ગણતરી(&bundle->ids);
      જો(live >= max_host) {
        size_t over = live - max_host + 1;
        માટે(; over; --over) {
          સંરચના connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          જો(!oldest_idle)
            તોડો;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_ગણતરી(&bundle->ids);
      }
    }
    જો(cshutdn) {
      shutdowns = Curl_cshutdn_dest_ગણતરી(cshutdn, to_add->destination);
      જો(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    જો((live + shutdowns) >= max_host)
      પાછા CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  જો(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t અંક_conns = Curl_uint32_tbl_ગણતરી(&cpool->conns);

    જો(!admin)
      admin = Curl_get_admin(data);
    જો(અંક_conns >= max_total) {
      size_t over = અંક_conns - max_total + 1;
      માટે(; over; --over) {
        સંરચના connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        જો(!oldest_idle)
          તોડો;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    અંક_conns = Curl_uint32_tbl_ગણતરી(&cpool->conns);
    જો(cshutdn) {
      shutdowns = Curl_cshutdn_ગણતરી(cshutdn);
      જો(shutdowns && (અંક_conns + shutdowns) >= max_total) {
        size_t over = અંક_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ખાલી, over);
      }
    }
    જો((અંક_conns + shutdowns) >= max_total)
      પાછા CPOOL_LIMIT_TOTAL;
  }

  પાછા CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(સંરચના Curl_easy *data,
                        સંરચના connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        અચલ સંરચના curltime *pnow)
{
  CURLcode result = CURLE_OK;
  સંરચના cpool_bundle *bundle = ખાલી;
  સંરચના cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  જો(!cpool)
    પાછા CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  પસંદગી(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  સ્થિતિ CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    જાઓ out;
  સ્થિતિ CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    જાઓ out;
  મૂળભૂત:
    તોડો;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  જો(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    જાઓ out;
  }
  જો(Curl_uint32_tbl_ગણતરી(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    જો((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    નહીંતર
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    જો(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

  જો(!bundle) {
    bundle = cpool_create_bundle();
    જો(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

  જો(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  conn->cpid = cpid;

  જો(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_ગણતરી(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  પાછા result;
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
સ્થિર bool cpool_foreach(સંરચના Curl_easy *data,
                          સંરચના cpool *cpool,
                          શૂન્ય *param,
                          પૂર્ણાંક (*func)(સંરચના cpool *cpool,
                                      સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn, શૂન્ય *param))
{
  શૂન્ય *entry;
  uint32_t cpid;

  જો(!cpool)
    પાછા FALSE;

  જો(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    કરો {
      સંરચના connectdata *conn = entry;
      જો(conn && func(cpool, data, conn, param) == 1)
        પાછા TRUE;
    } જ્યારે(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  પાછા FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
સ્થિર bool cpool_conn_now_idle(સંરચના cpool *cpool,
                                સંરચના Curl_easy *data,
                                સંરચના connectdata *conn,
                                અચલ સંરચના curltime *pnow)
{
  સંરચના connectdata *oldest_idle = ખાલી;
  સંરચના Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  જો(!data || !data->multi)
    પાછા kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  જો(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  નહીંતર {
    maxconnects = data->multi->maxconnects;
  }

  જો(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t અંક_conns = Curl_uint32_tbl_ગણતરી(&cpool->conns);
    admin = Curl_get_admin(data);
    જો(અંક_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            અંક_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      જો(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  પાછા kept;
}

સ્થિર શૂન્ય cpool_prune_dead(સંરચના cpool *cpool,
                             સંરચના Curl_easy *data,
                             અચલ સંરચના curltime *pnow);

સ્થિર bool cpool_find_act(સંરચના cpool *cpool,
                           સંરચના Curl_easy *data,
                           સંરચના connectdata *conn,
                           cpool_match_result match)
{
  પસંદગી(match) {
  સ્થિતિ CPOOL_MATCH_FOUND:
    પાછા TRUE;
  સ્થિતિ CPOOL_MATCH_TOO_OLD:
    જો(!cpool_conn_can_be_closed(conn))
      પાછા FALSE;
    FALLTHROUGH();
  સ્થિતિ CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા FALSE;
}

bool Curl_cpool_find(સંરચના Curl_easy *data,
                     અચલ અક્ષર *destination,
                     bool prune_dead,
                     અચલ સંરચના curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     શૂન્ય *userdata)
{
  સંરચના cpool *cpool = cpool_get_instance(data);
  સંરચના cpool_bundle *bundle = ખાલી;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  જો(!cpool)
    પાછા FALSE;

  CPOOL_LOCK(cpool, data);

  જો(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  જો(data->state.last_cpid != UINT32_MAX) {
    સંરચના connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    જો(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      જો(found)
        જાઓ out;
    }
    નહીંતર {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  જો(bundle) {
    uint32_t cpid;
    જો(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      કરો {
        સંરચના connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        જો(conn) {
          જો(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            જો(found)
              જાઓ out;
          }
        }
        નહીંતર {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } જ્યારે(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  જો(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  પાછા found;
}

સંરચના cpool_reaper_ctx {
  size_t reaped;
  અચલ સંરચના curltime *pnow;
};

સ્થિર પૂર્ણાંક cpool_reap_dead_cb(સંરચના cpool *cpool,
                              સંરચના Curl_easy *admin,
                              સંરચના connectdata *conn, શૂન્ય *param)
{
  સંરચના cpool_reaper_ctx *reaper = param;

  જો(cpool_conn_can_be_closed(conn)) {
    જો(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  પાછા 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
સ્થિર શૂન્ય cpool_prune_dead(સંરચના cpool *cpool,
                             સંરચના Curl_easy *data,
                             અચલ સંરચના curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  જો(elapsed_ms >= 1000L) {
    સંરચના Curl_easy *admin = Curl_get_admin(data);
    સંરચના cpool_reaper_ctx reaper;

    memset(&reaper, 0, કદ(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

સ્થિર પૂર્ણાંક conn_upkeep(સંરચના cpool *cpool,
                       સંરચના Curl_easy *admin,
                       સંરચના connectdata *conn,
                       શૂન્ય *param)
{
  અચલ સંરચના curltime *pnow = Curl_pgrs_now(admin);

  (શૂન્ય)param;
  જો((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    જો(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  પાછા 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(સંરચના Curl_easy *data)
{
  સંરચના cpool *cpool = cpool_get_instance(data);
  સંરચના Curl_easy *admin = Curl_get_admin(data);

  જો(!cpool)
    પાછા CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, ખાલી, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  પાછા CURLE_OK;
}

સંરચના connectdata *Curl_cpool_get_last_conn(સંરચના Curl_easy *data)
{
  સંરચના cpool *cpool = cpool_get_instance(data);
  જો(cpool && (data->state.last_cpid != UINT32_MAX)) {
    સંરચના connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    જો(conn && (data->state.last_conn_id == conn->connection_id))
      પાછા conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  પાછા ખાલી;
}

શૂન્ય Curl_cpool_return(સંરચના Curl_easy *data,
                       સંરચના connectdata *conn,
                       Curl_cpool_return_cb *cb, શૂન્ય *cbdata,
                       અચલ સંરચના curltime *pnow)
{
  સંરચના cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  પસંદગી(cb(data, conn, cbdata, pnow)) {
  સ્થિતિ CPOOL_DO_KEEP:
    તોડો;
  સ્થિતિ CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    જો(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    નહીંતર { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    તોડો;
  સ્થિતિ CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    તોડો;
  સ્થિતિ CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    તોડો;
  }

  CPOOL_UNLOCK(cpool, data);
}

સ્થિર પૂર્ણાંક cpool_reap_no_reuse(સંરચના cpool *cpool,
                               સંરચના Curl_easy *admin,
                               સંરચના connectdata *conn, શૂન્ય *param)
{
  (શૂન્ય)param;
  conn->bits.no_reuse = TRUE;
  જો(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  પાછા 0; /* continue iteration */
}

શૂન્ય Curl_cpool_nw_changed(સંરચના cpool *cpool, સંરચના Curl_easy *admin)
{
  જો(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, ખાલી, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
સ્થિર bool cpool_conn_too_old(સંરચના Curl_easy *data,
                               સંરચના connectdata *conn,
                               અચલ સંરચના curltime *pnow)
{
  timediff_t age_ms;

  જો(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    જો(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      પાછા TRUE;
    }
  }

  જો(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    જો(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      પાછા TRUE;
    }
  }

  પાછા FALSE;
}

bool Curl_cpool_conn_seems_healthy(સંરચના connectdata *conn,
                                   સંરચના Curl_easy *data,
                                   અચલ સંરચના curltime *pnow)
{
  સંરચના Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_આગત;

  DEBUGASSERT(!data->conn);
  જો(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    પાછા FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_આગત = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  જો(!poll_આગત &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    પાછા TRUE;

  admin = Curl_get_admin(data);
  જો(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  નહીંતર {
    bool આગત_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &આગત_pending);
    Curl_detach_connection(admin);
    જો(healthy && આગત_pending && poll_આગત) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  જો(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  પાછા healthy;
}

શૂન્ય Curl_cpool_conn_was_used(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn,
                              અચલ સંરચના curltime *pnow)
{
  (શૂન્ય)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  અચલ સંરચના curltime *pnow)
{
  (શૂન્ય)data;
  પાછા curlx_ptimediff_ms(pnow, &conn->created);
}
