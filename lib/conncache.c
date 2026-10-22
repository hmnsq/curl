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
    만약(c) {                                                             \
      만약(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } 동안(0)

#define CPOOL_UNLOCK(c, d)                                              \
  do {                                                                  \
    만약(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      만약(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } 동안(0)

/* A set of connection pool ids to the same destination. */
struct cpool_bundle {
  struct uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

static void cpool_bundle_unlink(struct cpool_bundle **pbundle)
{
  만약(*pbundle) {
    struct cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = NULL;
    만약(bundle->refcount)
      bundle->refcount--;
    만약(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

static void cpool_bundle_link(struct cpool_bundle **pdest,
                              struct cpool_bundle *src)
{
  만약(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    만약(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

static struct cpool_bundle *cpool_create_bundle(void)
{
  /* created with refcount == 1 */
  struct cpool_bundle *bundle = curlx_calloc(1, sizeof(*bundle));
  만약(!bundle)
    반환 NULL;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  반환 bundle;
}

/* Add a connection to a bundle */
static bool cpool_bundle_add_id(struct cpool *cpool,
                                struct cpool_bundle *bundle,
                                const char *destination,
                                uint32_t pool_id)
{
  만약(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    반환 FALSE;
  만약(Curl_uint32_spbset_count(&bundle->ids) == 1) {
    /* first one added, register bundle */
    struct cpool_bundle *new_ref = NULL;
    cpool_bundle_link(&new_ref, bundle);
    만약(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      반환 FALSE;
    }
  }
  반환 TRUE;
}

/* Remove a connection from a bundle */
static void cpool_bundle_remove_id(struct cpool *cpool,
                                   struct cpool_bundle *bundle,
                                   const char *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  만약(!Curl_uint32_spbset_count(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

static void cpool_bundle_hash_dtor(void *entry)
{
  struct cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

static struct cpool_bundle *cpool_get_bundle(struct cpool *cpool,
                                             const char *destination)
{
  struct cpool_bundle *bundle = NULL;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  반환 bundle;
}

static void cpool_remove_conn(struct cpool *cpool,
                              struct connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  만약(cpid != UINT32_MAX) {
    struct cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      struct connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    만약(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

static void cpool_discard_conn(struct cpool *cpool,
                               struct Curl_easy *data,
                               struct connectdata *conn,
                               bool aborted)
{
  struct Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  만약(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    반환;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  만약(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  만약(aborted)
    done = TRUE;
  만약(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  만약(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  아니면 {
    struct Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t num_conns = Curl_uint32_tbl_count(&cpool->conns);

    만약(num_conns < max_shutdowns)
      max_shutdowns -= num_conns;
    아니면 만약(max_shutdowns)
      max_shutdowns = 1;
    아니면 /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(num_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

void Curl_cpool_init(struct cpool *cpool,
                     struct Curl_share *share,
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

void Curl_cpool_destroy(struct cpool *cpool, struct Curl_easy *admin)
{
  만약(cpool && cpool->initialized && admin) {
    void *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_count(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    만약(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      struct Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      do {
        struct connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } 동안(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

static struct cpool *cpool_get_instance(struct Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  만약(CURL_SHARE_KEEP_CONNECT(data->share))
    반환 &data->share->cpool;
  아니면 만약(data->multi_easy)
    반환 &data->multi_easy->cpool;
  아니면 만약(data->multi)
    반환 &data->multi->cpool;
  반환 NULL;
}

struct cpool *Curl_cpool_get_instance(struct Curl_easy *data)
{
  반환 cpool_get_instance(data);
}

void Curl_cpool_xfer_init(struct Curl_easy *data)
{
  struct cpool *cpool = cpool_get_instance(data);

  만약(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    만약(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  아니면 {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

static bool cpool_conn_can_be_closed(struct connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  반환 (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

static struct connectdata *cpool_bundle_get_oldest_idle(
  struct cpool *cpool,
  struct cpool_bundle *bundle,
  const struct curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  만약(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    do {
      struct connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      만약(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        만약(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      아니면 만약(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } 동안(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  반환 Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

static struct connectdata *cpool_get_oldest_idle(struct cpool *cpool,
                                                 const struct curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  만약(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    do {
      struct connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      만약(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        만약((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      아니면 {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } 동안(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  반환 Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

static void cpool_conn_close(struct cpool *cpool,
                             struct Curl_easy *data,
                             struct connectdata *conn,
                             bool aborted)
{
  struct Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  만약(!cpool)
    반환;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  만약(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    반환;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  만약(do_lock)
    CPOOL_LOCK(cpool, admin);

  만약(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  만약(conn->bits.connect_only)
    aborted = TRUE;

  만약(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  아니면 {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  만약(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

void Curl_conn_close(struct Curl_easy *data,
                     struct connectdata *conn,
                     bool aborted)
{
  struct cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
static void cpool_evict_conn(struct cpool *cpool,
                             struct Curl_easy *admin,
                             struct connectdata *conn)
{
  만약(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  아니면
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

static 정수 cpool_check_limits(struct Curl_easy *data,
                              uint32_t max_total,
                              struct connectdata *to_add,
                              uint32_t max_host,
                              struct cpool_bundle *bundle,
                              const struct curltime *pnow)
{
  struct cpool *cpool = cpool_get_instance(data);
  struct cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : NULL;
  struct Curl_easy *admin = NULL;

  만약(!cpool)
    반환 CPOOL_LIMIT_OK;

  만약(!max_total && !max_host)
    반환 CPOOL_LIMIT_OK;

  만약(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    만약(bundle) {
      live = Curl_uint32_spbset_count(&bundle->ids);
      만약(live >= max_host) {
        size_t over = live - max_host + 1;
        위해(; over; --over) {
          struct connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          만약(!oldest_idle)
            break;
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
    만약(cshutdn) {
      shutdowns = Curl_cshutdn_dest_count(cshutdn, to_add->destination);
      만약(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    만약((live + shutdowns) >= max_host)
      반환 CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  만약(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t num_conns = Curl_uint32_tbl_count(&cpool->conns);

    만약(!admin)
      admin = Curl_get_admin(data);
    만약(num_conns >= max_total) {
      size_t over = num_conns - max_total + 1;
      위해(; over; --over) {
        struct connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        만약(!oldest_idle)
          break;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    num_conns = Curl_uint32_tbl_count(&cpool->conns);
    만약(cshutdn) {
      shutdowns = Curl_cshutdn_count(cshutdn);
      만약(shutdowns && (num_conns + shutdowns) >= max_total) {
        size_t over = num_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, NULL, over);
      }
    }
    만약((num_conns + shutdowns) >= max_total)
      반환 CPOOL_LIMIT_TOTAL;
  }

  반환 CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(struct Curl_easy *data,
                        struct connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        const struct curltime *pnow)
{
  CURLcode result = CURLE_OK;
  struct cpool_bundle *bundle = NULL;
  struct cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  만약(!cpool)
    반환 CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  switch(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  case CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    goto out;
  case CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    goto out;
  default:
    break;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  만약(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    goto out;
  }
  만약(Curl_uint32_tbl_count(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    만약((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    아니면
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    만약(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  만약(!bundle) {
    bundle = cpool_create_bundle();
    만약(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  만약(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  conn->cpid = cpid;

  만약(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
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

  반환 result;
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
static bool cpool_foreach(struct Curl_easy *data,
                          struct cpool *cpool,
                          void *param,
                          정수 (*func)(struct cpool *cpool,
                                      struct Curl_easy *data,
                                      struct connectdata *conn, void *param))
{
  void *entry;
  uint32_t cpid;

  만약(!cpool)
    반환 FALSE;

  만약(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    do {
      struct connectdata *conn = entry;
      만약(conn && func(cpool, data, conn, param) == 1)
        반환 TRUE;
    } 동안(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  반환 FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
static bool cpool_conn_now_idle(struct cpool *cpool,
                                struct Curl_easy *data,
                                struct connectdata *conn,
                                const struct curltime *pnow)
{
  struct connectdata *oldest_idle = NULL;
  struct Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  만약(!data || !data->multi)
    반환 kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  만약(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  아니면 {
    maxconnects = data->multi->maxconnects;
  }

  만약(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t num_conns = Curl_uint32_tbl_count(&cpool->conns);
    admin = Curl_get_admin(data);
    만약(num_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            num_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      만약(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  반환 kept;
}

static void cpool_prune_dead(struct cpool *cpool,
                             struct Curl_easy *data,
                             const struct curltime *pnow);

static bool cpool_find_act(struct cpool *cpool,
                           struct Curl_easy *data,
                           struct connectdata *conn,
                           cpool_match_result match)
{
  switch(match) {
  case CPOOL_MATCH_FOUND:
    반환 TRUE;
  case CPOOL_MATCH_TOO_OLD:
    만약(!cpool_conn_can_be_closed(conn))
      반환 FALSE;
    FALLTHROUGH();
  case CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    break;
  default:
    break;
  }
  반환 FALSE;
}

bool Curl_cpool_find(struct Curl_easy *data,
                     const char *destination,
                     bool prune_dead,
                     const struct curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     void *userdata)
{
  struct cpool *cpool = cpool_get_instance(data);
  struct cpool_bundle *bundle = NULL;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  만약(!cpool)
    반환 FALSE;

  CPOOL_LOCK(cpool, data);

  만약(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  만약(data->state.last_cpid != UINT32_MAX) {
    struct connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    만약(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      만약(found)
        goto out;
    }
    아니면 {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  만약(bundle) {
    uint32_t cpid;
    만약(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      do {
        struct connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        만약(conn) {
          만약(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            만약(found)
              goto out;
          }
        }
        아니면 {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } 동안(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  만약(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  반환 found;
}

struct cpool_reaper_ctx {
  size_t reaped;
  const struct curltime *pnow;
};

static 정수 cpool_reap_dead_cb(struct cpool *cpool,
                              struct Curl_easy *admin,
                              struct connectdata *conn, void *param)
{
  struct cpool_reaper_ctx *reaper = param;

  만약(cpool_conn_can_be_closed(conn)) {
    만약(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  반환 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
static void cpool_prune_dead(struct cpool *cpool,
                             struct Curl_easy *data,
                             const struct curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  만약(elapsed_ms >= 1000L) {
    struct Curl_easy *admin = Curl_get_admin(data);
    struct cpool_reaper_ctx reaper;

    memset(&reaper, 0, sizeof(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

static 정수 conn_upkeep(struct cpool *cpool,
                       struct Curl_easy *admin,
                       struct connectdata *conn,
                       void *param)
{
  const struct curltime *pnow = Curl_pgrs_now(admin);

  (void)param;
  만약((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    만약(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  반환 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(struct Curl_easy *data)
{
  struct cpool *cpool = cpool_get_instance(data);
  struct Curl_easy *admin = Curl_get_admin(data);

  만약(!cpool)
    반환 CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, NULL, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  반환 CURLE_OK;
}

struct connectdata *Curl_cpool_get_last_conn(struct Curl_easy *data)
{
  struct cpool *cpool = cpool_get_instance(data);
  만약(cpool && (data->state.last_cpid != UINT32_MAX)) {
    struct connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    만약(conn && (data->state.last_conn_id == conn->connection_id))
      반환 conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  반환 NULL;
}

void Curl_cpool_return(struct Curl_easy *data,
                       struct connectdata *conn,
                       Curl_cpool_return_cb *cb, void *cbdata,
                       const struct curltime *pnow)
{
  struct cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  switch(cb(data, conn, cbdata, pnow)) {
  case CPOOL_DO_KEEP:
    break;
  case CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    만약(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    아니면 { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    break;
  case CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    break;
  case CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    break;
  }

  CPOOL_UNLOCK(cpool, data);
}

static 정수 cpool_reap_no_reuse(struct cpool *cpool,
                               struct Curl_easy *admin,
                               struct connectdata *conn, void *param)
{
  (void)param;
  conn->bits.no_reuse = TRUE;
  만약(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  반환 0; /* continue iteration */
}

void Curl_cpool_nw_changed(struct cpool *cpool, struct Curl_easy *admin)
{
  만약(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, NULL, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
static bool cpool_conn_too_old(struct Curl_easy *data,
                               struct connectdata *conn,
                               const struct curltime *pnow)
{
  timediff_t age_ms;

  만약(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    만약(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      반환 TRUE;
    }
  }

  만약(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    만약(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      반환 TRUE;
    }
  }

  반환 FALSE;
}

bool Curl_cpool_conn_seems_healthy(struct connectdata *conn,
                                   struct Curl_easy *data,
                                   const struct curltime *pnow)
{
  struct Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_input;

  DEBUGASSERT(!data->conn);
  만약(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    반환 FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_input = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  만약(!poll_input &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    반환 TRUE;

  admin = Curl_get_admin(data);
  만약(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  아니면 {
    bool input_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &input_pending);
    Curl_detach_connection(admin);
    만약(healthy && input_pending && poll_input) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  만약(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  반환 healthy;
}

void Curl_cpool_conn_was_used(struct Curl_easy *data,
                              struct connectdata *conn,
                              const struct curltime *pnow)
{
  (void)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(struct Curl_easy *data,
                                  struct connectdata *conn,
                                  const struct curltime *pnow)
{
  (void)data;
  반환 curlx_ptimediff_ms(pnow, &conn->created);
}
