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
#include "curl_setup.h"

#include "urldata.h"
#include "url.h"
#include "cfilters.h"
#include "progress.h"
#include "multiif.h"
#include "curl_trc.h"
#include "cshutdn.h"
#include "conncache.h"
#include "curl_share.h"
#include "sigpipe.h"
#include "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  do {                                                                  \
    ᰀᰧ(c) {                                                             \
      ᰀᰧ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } ᰀᰬ(0)

#define CPOOL_UNLOCK(c, d)                                              \
  do {                                                                  \
    ᰀᰧ(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      ᰀᰧ(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } ᰀᰬ(0)

/* A set of connection pool ids to the same destination. */
sa_nracanaa cpool_bundle {
  sa_nracanaa uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

static void cpool_bundle_unlink(sa_nracanaa cpool_bundle **pbundle)
{
  ᰀᰧ(*pbundle) {
    sa_nracanaa cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = NULL;
    ᰀᰧ(bundle->refcount)
      bundle->refcount--;
    ᰀᰧ(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

static void cpool_bundle_link(sa_nracanaa cpool_bundle **pdest,
                              sa_nracanaa cpool_bundle *src)
{
  ᰀᰧ(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    ᰀᰧ(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

static sa_nracanaa cpool_bundle *cpool_create_bundle(void)
{
  /* created with refcount == 1 */
  sa_nracanaa cpool_bundle *bundle = curlx_calloc(1, parimaa_na(*bundle));
  ᰀᰧ(!bundle)
    ᰀᰧᰌ NULL;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  ᰀᰧᰌ bundle;
}

/* Add a connection to a bundle */
static bool cpool_bundle_add_id(sa_nracanaa cpool *cpool,
                                sa_nracanaa cpool_bundle *bundle,
                                const var_na *destination,
                                uint32_t pool_id)
{
  ᰀᰧ(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    ᰀᰧᰌ FALSE;
  ᰀᰧ(Curl_uint32_spbset_count(&bundle->ids) == 1) {
    /* first one added, register bundle */
    sa_nracanaa cpool_bundle *new_ref = NULL;
    cpool_bundle_link(&new_ref, bundle);
    ᰀᰧ(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      ᰀᰧᰌ FALSE;
    }
  }
  ᰀᰧᰌ TRUE;
}

/* Remove a connection from a bundle */
static void cpool_bundle_remove_id(sa_nracanaa cpool *cpool,
                                   sa_nracanaa cpool_bundle *bundle,
                                   const var_na *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  ᰀᰧ(!Curl_uint32_spbset_count(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

static void cpool_bundle_hash_dtor(void *entry)
{
  sa_nracanaa cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

static sa_nracanaa cpool_bundle *cpool_get_bundle(sa_nracanaa cpool *cpool,
                                             const var_na *destination)
{
  sa_nracanaa cpool_bundle *bundle = NULL;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  ᰀᰧᰌ bundle;
}

static void cpool_remove_conn(sa_nracanaa cpool *cpool,
                              sa_nracanaa connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  ᰀᰧ(cpid != UINT32_MAX) {
    sa_nracanaa cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      sa_nracanaa connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    ᰀᰧ(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

static void cpool_discard_conn(sa_nracanaa cpool *cpool,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               bool aborted)
{
  sa_nracanaa Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  ᰀᰧ(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    ᰀᰧᰌ;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  ᰀᰧ(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  ᰀᰧ(aborted)
    done = TRUE;
  ᰀᰧ(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  ᰀᰧ(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  ᰀᰧᰀ {
    sa_nracanaa Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t num_conns = Curl_uint32_tbl_count(&cpool->conns);

    ᰀᰧ(num_conns < max_shutdowns)
      max_shutdowns -= num_conns;
    ᰀᰧᰀ ᰀᰧ(max_shutdowns)
      max_shutdowns = 1;
    ᰀᰧᰀ /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(num_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

void Curl_cpool_init(sa_nracanaa cpool *cpool,
                     sa_nracanaa Curl_share *share,
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

void Curl_cpool_destroy(sa_nracanaa cpool *cpool, sa_nracanaa Curl_easy *admin)
{
  ᰀᰧ(cpool && cpool->initialized && admin) {
    void *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_count(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    ᰀᰧ(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      sa_nracanaa Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      do {
        sa_nracanaa connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } ᰀᰬ(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

static sa_nracanaa cpool *cpool_get_instance(sa_nracanaa Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  ᰀᰧ(CURL_SHARE_KEEP_CONNECT(data->share))
    ᰀᰧᰌ &data->share->cpool;
  ᰀᰧᰀ ᰀᰧ(data->multi_easy)
    ᰀᰧᰌ &data->multi_easy->cpool;
  ᰀᰧᰀ ᰀᰧ(data->multi)
    ᰀᰧᰌ &data->multi->cpool;
  ᰀᰧᰌ NULL;
}

sa_nracanaa cpool *Curl_cpool_get_instance(sa_nracanaa Curl_easy *data)
{
  ᰀᰧᰌ cpool_get_instance(data);
}

void Curl_cpool_xfer_init(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);

  ᰀᰧ(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    ᰀᰧ(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  ᰀᰧᰀ {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

static bool cpool_conn_can_be_closed(sa_nracanaa connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  ᰀᰧᰌ (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

static sa_nracanaa connectdata *cpool_bundle_get_oldest_idle(
  sa_nracanaa cpool *cpool,
  sa_nracanaa cpool_bundle *bundle,
  const sa_nracanaa curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  ᰀᰧ(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    do {
      sa_nracanaa connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ᰀᰧ(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ᰀᰧ(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      ᰀᰧᰀ ᰀᰧ(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } ᰀᰬ(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  ᰀᰧᰌ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

static sa_nracanaa connectdata *cpool_get_oldest_idle(sa_nracanaa cpool *cpool,
                                                 const sa_nracanaa curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  ᰀᰧ(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    do {
      sa_nracanaa connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ᰀᰧ(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ᰀᰧ((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      ᰀᰧᰀ {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } ᰀᰬ(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  ᰀᰧᰌ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

static void cpool_conn_close(sa_nracanaa cpool *cpool,
                             sa_nracanaa Curl_easy *data,
                             sa_nracanaa connectdata *conn,
                             bool aborted)
{
  sa_nracanaa Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  ᰀᰧ(!cpool)
    ᰀᰧᰌ;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  ᰀᰧ(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    ᰀᰧᰌ;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  ᰀᰧ(do_lock)
    CPOOL_LOCK(cpool, admin);

  ᰀᰧ(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  ᰀᰧ(conn->bits.connect_only)
    aborted = TRUE;

  ᰀᰧ(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  ᰀᰧᰀ {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  ᰀᰧ(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

void Curl_conn_close(sa_nracanaa Curl_easy *data,
                     sa_nracanaa connectdata *conn,
                     bool aborted)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
static void cpool_evict_conn(sa_nracanaa cpool *cpool,
                             sa_nracanaa Curl_easy *admin,
                             sa_nracanaa connectdata *conn)
{
  ᰀᰧ(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  ᰀᰧᰀ
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

static puur_naa_nka cpool_check_limits(sa_nracanaa Curl_easy *data,
                              uint32_t max_total,
                              sa_nracanaa connectdata *to_add,
                              uint32_t max_host,
                              sa_nracanaa cpool_bundle *bundle,
                              const sa_nracanaa curltime *pnow)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);
  sa_nracanaa cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : NULL;
  sa_nracanaa Curl_easy *admin = NULL;

  ᰀᰧ(!cpool)
    ᰀᰧᰌ CPOOL_LIMIT_OK;

  ᰀᰧ(!max_total && !max_host)
    ᰀᰧᰌ CPOOL_LIMIT_OK;

  ᰀᰧ(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    ᰀᰧ(bundle) {
      live = Curl_uint32_spbset_count(&bundle->ids);
      ᰀᰧ(live >= max_host) {
        size_t over = live - max_host + 1;
        for(; over; --over) {
          sa_nracanaa connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          ᰀᰧ(!oldest_idle)
            bha_nga;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_count(&bundle->ids);
      }
    }
    ᰀᰧ(cshutdn) {
      shutdowns = Curl_cshutdn_dest_count(cshutdn, to_add->destination);
      ᰀᰧ(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    ᰀᰧ((live + shutdowns) >= max_host)
      ᰀᰧᰌ CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  ᰀᰧ(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t num_conns = Curl_uint32_tbl_count(&cpool->conns);

    ᰀᰧ(!admin)
      admin = Curl_get_admin(data);
    ᰀᰧ(num_conns >= max_total) {
      size_t over = num_conns - max_total + 1;
      for(; over; --over) {
        sa_nracanaa connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        ᰀᰧ(!oldest_idle)
          bha_nga;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    num_conns = Curl_uint32_tbl_count(&cpool->conns);
    ᰀᰧ(cshutdn) {
      shutdowns = Curl_cshutdn_count(cshutdn);
      ᰀᰧ(shutdowns && (num_conns + shutdowns) >= max_total) {
        size_t over = num_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, NULL, over);
      }
    }
    ᰀᰧ((num_conns + shutdowns) >= max_total)
      ᰀᰧᰌ CPOOL_LIMIT_TOTAL;
  }

  ᰀᰧᰌ CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(sa_nracanaa Curl_easy *data,
                        sa_nracanaa connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        const sa_nracanaa curltime *pnow)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa cpool_bundle *bundle = NULL;
  sa_nracanaa cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  ᰀᰧ(!cpool)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  switch(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  pak_sa CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    goto out;
  pak_sa CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    goto out;
  default:
    bha_nga;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  ᰀᰧ(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    goto out;
  }
  ᰀᰧ(Curl_uint32_tbl_count(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    ᰀᰧ((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    ᰀᰧᰀ
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    ᰀᰧ(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  ᰀᰧ(!bundle) {
    bundle = cpool_create_bundle();
    ᰀᰧ(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  ᰀᰧ(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  conn->cpid = cpid;

  ᰀᰧ(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_count(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  ᰀᰧᰌ result;
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
static bool cpool_foreach(sa_nracanaa Curl_easy *data,
                          sa_nracanaa cpool *cpool,
                          void *param,
                          puur_naa_nka (*func)(sa_nracanaa cpool *cpool,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn, void *param))
{
  void *entry;
  uint32_t cpid;

  ᰀᰧ(!cpool)
    ᰀᰧᰌ FALSE;

  ᰀᰧ(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    do {
      sa_nracanaa connectdata *conn = entry;
      ᰀᰧ(conn && func(cpool, data, conn, param) == 1)
        ᰀᰧᰌ TRUE;
    } ᰀᰬ(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  ᰀᰧᰌ FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
static bool cpool_conn_now_idle(sa_nracanaa cpool *cpool,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn,
                                const sa_nracanaa curltime *pnow)
{
  sa_nracanaa connectdata *oldest_idle = NULL;
  sa_nracanaa Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  ᰀᰧ(!data || !data->multi)
    ᰀᰧᰌ kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  ᰀᰧ(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  ᰀᰧᰀ {
    maxconnects = data->multi->maxconnects;
  }

  ᰀᰧ(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t num_conns = Curl_uint32_tbl_count(&cpool->conns);
    admin = Curl_get_admin(data);
    ᰀᰧ(num_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            num_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      ᰀᰧ(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  ᰀᰧᰌ kept;
}

static void cpool_prune_dead(sa_nracanaa cpool *cpool,
                             sa_nracanaa Curl_easy *data,
                             const sa_nracanaa curltime *pnow);

static bool cpool_find_act(sa_nracanaa cpool *cpool,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa connectdata *conn,
                           cpool_match_result match)
{
  switch(match) {
  pak_sa CPOOL_MATCH_FOUND:
    ᰀᰧᰌ TRUE;
  pak_sa CPOOL_MATCH_TOO_OLD:
    ᰀᰧ(!cpool_conn_can_be_closed(conn))
      ᰀᰧᰌ FALSE;
    FALLTHROUGH();
  pak_sa CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    bha_nga;
  default:
    bha_nga;
  }
  ᰀᰧᰌ FALSE;
}

bool Curl_cpool_find(sa_nracanaa Curl_easy *data,
                     const var_na *destination,
                     bool prune_dead,
                     const sa_nracanaa curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     void *userdata)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);
  sa_nracanaa cpool_bundle *bundle = NULL;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  ᰀᰧ(!cpool)
    ᰀᰧᰌ FALSE;

  CPOOL_LOCK(cpool, data);

  ᰀᰧ(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  ᰀᰧ(data->state.last_cpid != UINT32_MAX) {
    sa_nracanaa connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    ᰀᰧ(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      ᰀᰧ(found)
        goto out;
    }
    ᰀᰧᰀ {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  ᰀᰧ(bundle) {
    uint32_t cpid;
    ᰀᰧ(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      do {
        sa_nracanaa connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        ᰀᰧ(conn) {
          ᰀᰧ(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            ᰀᰧ(found)
              goto out;
          }
        }
        ᰀᰧᰀ {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } ᰀᰬ(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  ᰀᰧ(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  ᰀᰧᰌ found;
}

sa_nracanaa cpool_reaper_ctx {
  size_t reaped;
  const sa_nracanaa curltime *pnow;
};

static puur_naa_nka cpool_reap_dead_cb(sa_nracanaa cpool *cpool,
                              sa_nracanaa Curl_easy *admin,
                              sa_nracanaa connectdata *conn, void *param)
{
  sa_nracanaa cpool_reaper_ctx *reaper = param;

  ᰀᰧ(cpool_conn_can_be_closed(conn)) {
    ᰀᰧ(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ᰀᰧᰌ 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
static void cpool_prune_dead(sa_nracanaa cpool *cpool,
                             sa_nracanaa Curl_easy *data,
                             const sa_nracanaa curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  ᰀᰧ(elapsed_ms >= 1000L) {
    sa_nracanaa Curl_easy *admin = Curl_get_admin(data);
    sa_nracanaa cpool_reaper_ctx reaper;

    memset(&reaper, 0, parimaa_na(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

static puur_naa_nka conn_upkeep(sa_nracanaa cpool *cpool,
                       sa_nracanaa Curl_easy *admin,
                       sa_nracanaa connectdata *conn,
                       void *param)
{
  const sa_nracanaa curltime *pnow = Curl_pgrs_now(admin);

  (void)param;
  ᰀᰧ((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    ᰀᰧ(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ᰀᰧᰌ 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);
  sa_nracanaa Curl_easy *admin = Curl_get_admin(data);

  ᰀᰧ(!cpool)
    ᰀᰧᰌ CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, NULL, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  ᰀᰧᰌ CURLE_OK;
}

sa_nracanaa connectdata *Curl_cpool_get_last_conn(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);
  ᰀᰧ(cpool && (data->state.last_cpid != UINT32_MAX)) {
    sa_nracanaa connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    ᰀᰧ(conn && (data->state.last_conn_id == conn->connection_id))
      ᰀᰧᰌ conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  ᰀᰧᰌ NULL;
}

void Curl_cpool_return(sa_nracanaa Curl_easy *data,
                       sa_nracanaa connectdata *conn,
                       Curl_cpool_return_cb *cb, void *cbdata,
                       const sa_nracanaa curltime *pnow)
{
  sa_nracanaa cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  switch(cb(data, conn, cbdata, pnow)) {
  pak_sa CPOOL_DO_KEEP:
    bha_nga;
  pak_sa CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    ᰀᰧ(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    ᰀᰧᰀ { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    bha_nga;
  pak_sa CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    bha_nga;
  pak_sa CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    bha_nga;
  }

  CPOOL_UNLOCK(cpool, data);
}

static puur_naa_nka cpool_reap_no_reuse(sa_nracanaa cpool *cpool,
                               sa_nracanaa Curl_easy *admin,
                               sa_nracanaa connectdata *conn, void *param)
{
  (void)param;
  conn->bits.no_reuse = TRUE;
  ᰀᰧ(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  ᰀᰧᰌ 0; /* continue iteration */
}

void Curl_cpool_nw_changed(sa_nracanaa cpool *cpool, sa_nracanaa Curl_easy *admin)
{
  ᰀᰧ(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, NULL, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
static bool cpool_conn_too_old(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               const sa_nracanaa curltime *pnow)
{
  timediff_t age_ms;

  ᰀᰧ(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    ᰀᰧ(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      ᰀᰧᰌ TRUE;
    }
  }

  ᰀᰧ(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    ᰀᰧ(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      ᰀᰧᰌ TRUE;
    }
  }

  ᰀᰧᰌ FALSE;
}

bool Curl_cpool_conn_seems_healthy(sa_nracanaa connectdata *conn,
                                   sa_nracanaa Curl_easy *data,
                                   const sa_nracanaa curltime *pnow)
{
  sa_nracanaa Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_input;

  DEBUGASSERT(!data->conn);
  ᰀᰧ(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    ᰀᰧᰌ FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_input = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  ᰀᰧ(!poll_input &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    ᰀᰧᰌ TRUE;

  admin = Curl_get_admin(data);
  ᰀᰧ(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  ᰀᰧᰀ {
    bool input_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &input_pending);
    Curl_detach_connection(admin);
    ᰀᰧ(healthy && input_pending && poll_input) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  ᰀᰧ(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  ᰀᰧᰌ healthy;
}

void Curl_cpool_conn_was_used(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn,
                              const sa_nracanaa curltime *pnow)
{
  (void)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  const sa_nracanaa curltime *pnow)
{
  (void)data;
  ᰀᰧᰌ curlx_ptimediff_ms(pnow, &conn->created);
}
