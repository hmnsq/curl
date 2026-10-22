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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "cshutdn.h"
#ଅନ୍ତର୍ଭାଵ "conncache.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "sigpipe.h"
#ଅନ୍ତର୍ଭାଵ "uint-spbset.h"


#define CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#define CPOOL_LOCK(c, d)                                                \
  କର {                                                                  \
    ଯଦି(c) {                                                             \
      ଯଦି(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } ଯେତେବେଳେ(0)

#define CPOOL_UNLOCK(c, d)                                              \
  କର {                                                                  \
    ଯଦି(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      ଯଦି(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_DATA_CONNECT); \
    }                                                                   \
  } ଯେତେବେଳେ(0)

/* A set of connection pool ids to the same destination. */
ସଂରଚନା cpool_bundle {
  ସଂରଚନା uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ସ୍ଥିର ଶୂନ୍ୟ cpool_bundle_unlink(ସଂରଚନା cpool_bundle **pbundle)
{
  ଯଦି(*pbundle) {
    ସଂରଚନା cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = ଖାଲି;
    ଯଦି(bundle->refcount)
      bundle->refcount--;
    ଯଦି(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cpool_bundle_link(ସଂରଚନା cpool_bundle **pdest,
                              ସଂରଚନା cpool_bundle *src)
{
  ଯଦି(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    ଯଦି(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ସ୍ଥିର ସଂରଚନା cpool_bundle *cpool_create_bundle(ଶୂନ୍ୟ)
{
  /* created with refcount == 1 */
  ସଂରଚନା cpool_bundle *bundle = curlx_calloc(1, ଆକାର(*bundle));
  ଯଦି(!bundle)
    ଫେରନ୍ତୁ ଖାଲି;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  ଫେରନ୍ତୁ bundle;
}

/* Add a connection to a bundle */
ସ୍ଥିର bool cpool_bundle_add_id(ସଂରଚନା cpool *cpool,
                                ସଂରଚନା cpool_bundle *bundle,
                                ଅଚଳ ଅକ୍ଷର *destination,
                                uint32_t pool_id)
{
  ଯଦି(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(Curl_uint32_spbset_ଗଣନା(&bundle->ids) == 1) {
    /* first one added, register bundle */
    ସଂରଚନା cpool_bundle *new_ref = ଖାଲି;
    cpool_bundle_link(&new_ref, bundle);
    ଯଦି(!Curl_hash_add(&cpool->dest2bundle,
                      destination, strlen(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      ଫେରନ୍ତୁ FALSE;
    }
  }
  ଫେରନ୍ତୁ TRUE;
}

/* Remove a connection from a bundle */
ସ୍ଥିର ଶୂନ୍ୟ cpool_bundle_remove_id(ସଂରଚନା cpool *cpool,
                                   ସଂରଚନା cpool_bundle *bundle,
                                   ଅଚଳ ଅକ୍ଷର *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  ଯଦି(!Curl_uint32_spbset_ଗଣନା(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, strlen(destination));
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cpool_bundle_hash_dtor(ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

ସ୍ଥିର ସଂରଚନା cpool_bundle *cpool_get_bundle(ସଂରଚନା cpool *cpool,
                                             ଅଚଳ ଅକ୍ଷର *destination)
{
  ସଂରଚନା cpool_bundle *bundle = ଖାଲି;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, strlen(destination)));
  ଫେରନ୍ତୁ bundle;
}

ସ୍ଥିର ଶୂନ୍ୟ cpool_remove_conn(ସଂରଚନା cpool *cpool,
                              ସଂରଚନା connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  ଯଦି(cpid != UINT32_MAX) {
    ସଂରଚନା cpool_bundle *bundle;

#ifdef DEBUGBUILD
    {
      ସଂରଚନା connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#endif
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    ଯଦି(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cpool_discard_conn(ସଂରଚନା cpool *cpool,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               bool aborted)
{
  ସଂରଚନା Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(data);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  ଯଦି(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  ଯଦି(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  ଯଦି(aborted)
    done = TRUE;
  ଯଦି(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  ଯଦି(done || !data->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  ଅନ୍ୟଥା {
    ସଂରଚନା Curl_multi *multi = data->multi;
    size_t max_shutdowns = multi->max_total_connections;
    uint32_t ଅଙ୍କ_conns = Curl_uint32_tbl_ଗଣନା(&cpool->conns);

    ଯଦି(ଅଙ୍କ_conns < max_shutdowns)
      max_shutdowns -= ଅଙ୍କ_conns;
    ଅନ୍ୟଥା ଯଦି(max_shutdowns)
      max_shutdowns = 1;
    ଅନ୍ୟଥା /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(ଅଙ୍କ_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

ଶୂନ୍ୟ Curl_cpool_init(ସଂରଚନା cpool *cpool,
                     ସଂରଚନା Curl_share *share,
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

ଶୂନ୍ୟ Curl_cpool_destroy(ସଂରଚନା cpool *cpool, ସଂରଚନା Curl_easy *admin)
{
  ଯଦି(cpool && cpool->initialized && admin) {
    ଶୂନ୍ୟ *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_ଗଣନା(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    ଯଦି(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      ସଂରଚନା Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      କର {
        ସଂରଚନା connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } ଯେତେବେଳେ(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ସ୍ଥିର ସଂରଚନା cpool *cpool_get_instance(ସଂରଚନା Curl_easy *data)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(data->mid);
  ଯଦି(CURL_SHARE_KEEP_CONNECT(data->share))
    ଫେରନ୍ତୁ &data->share->cpool;
  ଅନ୍ୟଥା ଯଦି(data->multi_easy)
    ଫେରନ୍ତୁ &data->multi_easy->cpool;
  ଅନ୍ୟଥା ଯଦି(data->multi)
    ଫେରନ୍ତୁ &data->multi->cpool;
  ଫେରନ୍ତୁ ଖାଲି;
}

ସଂରଚନା cpool *Curl_cpool_get_instance(ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ cpool_get_instance(data);
}

ଶୂନ୍ୟ Curl_cpool_xfer_init(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);

  ଯଦି(cpool) {
    CPOOL_LOCK(cpool, data);
    /* the identifier inside the connection cache */
    data->id = cpool->next_easy_id++;
    ଯଦି(cpool->next_easy_id == CURL_OFF_T_MAX)
      cpool->next_easy_id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, data);
  }
  ଅନ୍ୟଥା {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    data->id = 0;
    data->state.last_conn_id = -1;
    data->state.last_cpid = UINT32_MAX;
  }
}

ସ୍ଥିର bool cpool_conn_can_be_closed(ସଂରଚନା connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  ଫେରନ୍ତୁ (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

ସ୍ଥିର ସଂରଚନା connectdata *cpool_bundle_get_oldest_idle(
  ସଂରଚନା cpool *cpool,
  ସଂରଚନା cpool_bundle *bundle,
  ଅଚଳ ସଂରଚନା curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  ଯଦି(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    କର {
      ସଂରଚନା connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ଯଦି(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ଯଦି(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      ଅନ୍ୟଥା ଯଦି(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } ଯେତେବେଳେ(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
  }
  ଫେରନ୍ତୁ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ସ୍ଥିର ସଂରଚନା connectdata *cpool_get_oldest_idle(ସଂରଚନା cpool *cpool,
                                                 ଅଚଳ ସଂରଚନା curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  ଯଦି(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    କର {
      ସଂରଚନା connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      ଯଦି(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        ଯଦି((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      ଅନ୍ୟଥା {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } ଯେତେବେଳେ(Curl_uint32_bset_next(&cpool->idles, cpid, &cpid));
  }
  ଫେରନ୍ତୁ Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ସ୍ଥିର ଶୂନ୍ୟ cpool_conn_close(ସଂରଚନା cpool *cpool,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn,
                             bool aborted)
{
  ସଂରଚନା Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(data && !data->conn);
  ଯଦି(!cpool)
    ଫେରନ୍ତୁ;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  ଯଦି(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(data, "conn terminate when inuse: %u", conn->attached_xfers));
    ଫେରନ୍ତୁ;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(data);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  ଯଦି(do_lock)
    CPOOL_LOCK(cpool, admin);

  ଯଦି(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  ଯଦି(conn->bits.connect_only)
    aborted = TRUE;

  ଯଦି(data->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(data, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&data->multi->cpool, data, conn, aborted);
  }
  ଅନ୍ୟଥା {
    /* No multi available, terminate */
    infof(data, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  ଯଦି(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

ଶୂନ୍ୟ Curl_conn_close(ସଂରଚନା Curl_easy *data,
                     ସଂରଚନା connectdata *conn,
                     bool aborted)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);
  cpool_conn_close(cpool, data, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ସ୍ଥିର ଶୂନ୍ୟ cpool_evict_conn(ସଂରଚନା cpool *cpool,
                             ସଂରଚନା Curl_easy *admin,
                             ସଂରଚନା connectdata *conn)
{
  ଯଦି(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  ଅନ୍ୟଥା
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#define CPOOL_LIMIT_OK     0
#define CPOOL_LIMIT_DEST   1
#define CPOOL_LIMIT_TOTAL  2

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpool_check_limits(ସଂରଚନା Curl_easy *data,
                              uint32_t max_total,
                              ସଂରଚନା connectdata *to_add,
                              uint32_t max_host,
                              ସଂରଚନା cpool_bundle *bundle,
                              ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);
  ସଂରଚନା cshutdn *cshutdn = data->multi ? &data->multi->cshutdn : ଖାଲି;
  ସଂରଚନା Curl_easy *admin = ଖାଲି;

  ଯଦି(!cpool)
    ଫେରନ୍ତୁ CPOOL_LIMIT_OK;

  ଯଦି(!max_total && !max_host)
    ଫେରନ୍ତୁ CPOOL_LIMIT_OK;

  ଯଦି(max_host) {
    size_t live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(data);
    ଯଦି(bundle) {
      live = Curl_uint32_spbset_ଗଣନା(&bundle->ids);
      ଯଦି(live >= max_host) {
        size_t over = live - max_host + 1;
        ପାଇଁ(; over; --over) {
          ସଂରଚନା connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          ଯଦି(!oldest_idle)
            ଭାଙ୍ଗ;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_ଗଣନା(&bundle->ids);
      }
    }
    ଯଦି(cshutdn) {
      shutdowns = Curl_cshutdn_dest_ଗଣନା(cshutdn, to_add->destination);
      ଯଦି(shutdowns && ((live + shutdowns) >= max_host)) {
        size_t over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    ଯଦି((live + shutdowns) >= max_host)
      ଫେରନ୍ତୁ CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  ଯଦି(max_total && !data->state.internal) {
    size_t shutdowns = 0;
    uint32_t ଅଙ୍କ_conns = Curl_uint32_tbl_ଗଣନା(&cpool->conns);

    ଯଦି(!admin)
      admin = Curl_get_admin(data);
    ଯଦି(ଅଙ୍କ_conns >= max_total) {
      size_t over = ଅଙ୍କ_conns - max_total + 1;
      ପାଇଁ(; over; --over) {
        ସଂରଚନା connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        ଯଦି(!oldest_idle)
          ଭାଙ୍ଗ;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_total);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    ଅଙ୍କ_conns = Curl_uint32_tbl_ଗଣନା(&cpool->conns);
    ଯଦି(cshutdn) {
      shutdowns = Curl_cshutdn_ଗଣନା(cshutdn);
      ଯଦି(shutdowns && (ଅଙ୍କ_conns + shutdowns) >= max_total) {
        size_t over = ଅଙ୍କ_conns + shutdowns - max_total + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, ଖାଲି, over);
      }
    }
    ଯଦି((ଅଙ୍କ_conns + shutdowns) >= max_total)
      ଫେରନ୍ତୁ CPOOL_LIMIT_TOTAL;
  }

  ଫେରନ୍ତୁ CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        ଅଚଳ ସଂରଚନା curltime *pnow)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା cpool_bundle *bundle = ଖାଲି;
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  ଯଦି(!cpool)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, data);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  ବାଛ(cpool_check_limits(data, max_total, conn, max_host, bundle, pnow)) {
  ସ୍ଥିତି CPOOL_LIMIT_DEST:
    infof(data, "No more connections allowed to host");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    ଯାଅ out;
  ସ୍ଥିତି CPOOL_LIMIT_TOTAL:
    infof(data, "No connections available, total of %u reached.",
          data->multi->max_total_connections);
    result = CURLE_NO_CONNECTION_AVAILABLE;
    ଯାଅ out;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  ଯଦି(capacity == UINT32_MAX) {
    infof(data, "No connections available, pool at max capacity");
    result = CURLE_NO_CONNECTION_AVAILABLE;
    ଯାଅ out;
  }
  ଯଦି(Curl_uint32_tbl_ଗଣନା(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    ଯଦି((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    ଅନ୍ୟଥା
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    ଯଦି(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

  ଯଦି(!bundle) {
    bundle = cpool_create_bundle();
    ଯଦି(!bundle) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

  ଯଦି(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  conn->cpid = cpid;

  ଯଦି(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  conn->connection_id = cpool->next_connection_id++;
  CURL_TRC_M(data, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_ଗଣନା(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର bool cpool_foreach(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା cpool *cpool,
                          ଶୂନ୍ୟ *param,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*func)(ସଂରଚନା cpool *cpool,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn, ଶୂନ୍ୟ *param))
{
  ଶୂନ୍ୟ *entry;
  uint32_t cpid;

  ଯଦି(!cpool)
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    କର {
      ସଂରଚନା connectdata *conn = entry;
      ଯଦି(conn && func(cpool, data, conn, param) == 1)
        ଫେରନ୍ତୁ TRUE;
    } ଯେତେବେଳେ(Curl_uint32_tbl_next(&cpool->conns, cpid, &cpid, &entry));
  }
  ଫେରନ୍ତୁ FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ସ୍ଥିର bool cpool_conn_now_idle(ସଂରଚନା cpool *cpool,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn,
                                ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା connectdata *oldest_idle = ଖାଲି;
  ସଂରଚନା Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  ଯଦି(!data || !data->multi)
    ଫେରନ୍ତୁ kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  ଯଦି(!data->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(data->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  ଅନ୍ୟଥା {
    maxconnects = data->multi->maxconnects;
  }

  ଯଦି(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t ଅଙ୍କ_conns = Curl_uint32_tbl_ଗଣନା(&cpool->conns);
    admin = Curl_get_admin(data);
    ଯଦି(ଅଙ୍କ_conns > maxconnects) {
      infof(data, "Connection pool is full, closing the oldest of %u/%u",
            ଅଙ୍କ_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      ଯଦି(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  ଫେରନ୍ତୁ kept;
}

ସ୍ଥିର ଶୂନ୍ୟ cpool_prune_dead(ସଂରଚନା cpool *cpool,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ସଂରଚନା curltime *pnow);

ସ୍ଥିର bool cpool_find_act(ସଂରଚନା cpool *cpool,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା connectdata *conn,
                           cpool_match_result match)
{
  ବାଛ(match) {
  ସ୍ଥିତି CPOOL_MATCH_FOUND:
    ଫେରନ୍ତୁ TRUE;
  ସ୍ଥିତି CPOOL_MATCH_TOO_OLD:
    ଯଦି(!cpool_conn_can_be_closed(conn))
      ଫେରନ୍ତୁ FALSE;
    FALLTHROUGH();
  ସ୍ଥିତି CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, data, conn, FALSE);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_cpool_find(ସଂରଚନା Curl_easy *data,
                     ଅଚଳ ଅକ୍ଷର *destination,
                     bool prune_dead,
                     ଅଚଳ ସଂରଚନା curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     ଶୂନ୍ୟ *userdata)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);
  ସଂରଚନା cpool_bundle *bundle = ଖାଲି;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  ଯଦି(!cpool)
    ଫେରନ୍ତୁ FALSE;

  CPOOL_LOCK(cpool, data);

  ଯଦି(prune_dead)
    cpool_prune_dead(cpool, data, pnow);

  ଯଦି(data->state.last_cpid != UINT32_MAX) {
    ସଂରଚନା connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    ଯଦି(conn && (data->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
      ଯଦି(found)
        ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      /* No longer found here. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  ଯଦି(bundle) {
    uint32_t cpid;
    ଯଦି(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      କର {
        ସଂରଚନା connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        ଯଦି(conn) {
          ଯଦି(data->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, data, conn, conn_cb(conn, userdata));
            ଯଦି(found)
              ଯାଅ out;
          }
        }
        ଅନ୍ୟଥା {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } ଯେତେବେଳେ(Curl_uint32_spbset_next(&bundle->ids, cpid, &cpid));
    }
  }

out:
  ଯଦି(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, data);
  ଫେରନ୍ତୁ found;
}

ସଂରଚନା cpool_reaper_ctx {
  size_t reaped;
  ଅଚଳ ସଂରଚନା curltime *pnow;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpool_reap_dead_cb(ସଂରଚନା cpool *cpool,
                              ସଂରଚନା Curl_easy *admin,
                              ସଂରଚନା connectdata *conn, ଶୂନ୍ୟ *param)
{
  ସଂରଚନା cpool_reaper_ctx *reaper = param;

  ଯଦି(cpool_conn_can_be_closed(conn)) {
    ଯଦି(conn->bits.no_reuse || conn->bits.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ଫେରନ୍ତୁ 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ସ୍ଥିର ଶୂନ୍ୟ cpool_prune_dead(ସଂରଚନା cpool *cpool,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ସଂରଚନା curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  ଯଦି(elapsed_ms >= 1000L) {
    ସଂରଚନା Curl_easy *admin = Curl_get_admin(data);
    ସଂରଚନା cpool_reaper_ctx reaper;

    memset(&reaper, 0, ଆକାର(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା conn_upkeep(ସଂରଚନା cpool *cpool,
                       ସଂରଚନା Curl_easy *admin,
                       ସଂରଚନା connectdata *conn,
                       ଶୂନ୍ୟ *param)
{
  ଅଚଳ ସଂରଚନା curltime *pnow = Curl_pgrs_now(admin);

  (ଶୂନ୍ୟ)param;
  ଯଦି((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode result;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    result = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    ଯଦି(result && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  ଫେରନ୍ତୁ 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);
  ସଂରଚନା Curl_easy *admin = Curl_get_admin(data);

  ଯଦି(!cpool)
    ଫେରନ୍ତୁ CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, ଖାଲି, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା connectdata *Curl_cpool_get_last_conn(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);
  ଯଦି(cpool && (data->state.last_cpid != UINT32_MAX)) {
    ସଂରଚନା connectdata *conn;

    CPOOL_LOCK(cpool, data);
    conn = Curl_uint32_tbl_get(&cpool->conns, data->state.last_cpid);
    CPOOL_UNLOCK(cpool, data);
    ଯଦି(conn && (data->state.last_conn_id == conn->connection_id))
      ଫେରନ୍ତୁ conn;
    data->state.last_cpid = UINT32_MAX;
    data->state.last_conn_id = -1;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ Curl_cpool_return(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା connectdata *conn,
                       Curl_cpool_return_cb *cb, ଶୂନ୍ୟ *cbdata,
                       ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା cpool *cpool = cpool_get_instance(data);

  CPOOL_LOCK(cpool, data);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  ବାଛ(cb(data, conn, cbdata, pnow)) {
  ସ୍ଥିତି CPOOL_DO_KEEP:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    ଯଦି(cpool_conn_now_idle(cpool, data, conn, pnow)) {
      /* connection kept in the cpool */
      infof(data, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    ଅନ୍ୟଥା { /* connection was removed from the cpool and destroyed. */
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CPOOL_DO_CLOSE:
    Curl_conn_close(data, conn, FALSE);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CPOOL_DO_TERMINATE:
    Curl_conn_close(data, conn, TRUE);
    ଭାଙ୍ଗ;
  }

  CPOOL_UNLOCK(cpool, data);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpool_reap_no_reuse(ସଂରଚନା cpool *cpool,
                               ସଂରଚନା Curl_easy *admin,
                               ସଂରଚନା connectdata *conn, ଶୂନ୍ୟ *param)
{
  (ଶୂନ୍ୟ)param;
  conn->bits.no_reuse = TRUE;
  ଯଦି(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  ଫେରନ୍ତୁ 0; /* continue iteration */
}

ଶୂନ୍ୟ Curl_cpool_nw_changed(ସଂରଚନା cpool *cpool, ସଂରଚନା Curl_easy *admin)
{
  ଯଦି(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, ଖାଲି, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ସ୍ଥିର bool cpool_conn_too_old(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               ଅଚଳ ସଂରଚନା curltime *pnow)
{
  timediff_t age_ms;

  ଯଦି(data->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    ଯଦି(age_ms > data->set.conn_max_idle_ms) {
      infof(data, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_idle_ms);
      ଫେରନ୍ତୁ TRUE;
    }
  }

  ଯଦି(data->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    ଯଦି(age_ms > data->set.conn_max_age_ms) {
      infof(data,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, data->set.conn_max_age_ms);
      ଫେରନ୍ତୁ TRUE;
    }
  }

  ଫେରନ୍ତୁ FALSE;
}

bool Curl_cpool_conn_seems_healthy(ସଂରଚନା connectdata *conn,
                                   ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_ନିବେଶ;

  DEBUGASSERT(!data->conn);
  ଯଦି(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(data, conn, pnow))
    ଫେରନ୍ତୁ FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_ନିବେଶ = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  ଯଦି(!poll_ନିବେଶ &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    ଫେରନ୍ତୁ TRUE;

  admin = Curl_get_admin(data);
  ଯଦି(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  ଅନ୍ୟଥା {
    bool ନିବେଶ_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &ନିବେଶ_pending);
    Curl_detach_connection(admin);
    ଯଦି(healthy && ନିବେଶ_pending && poll_ନିବେଶ) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(data, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  ଯଦି(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  ଫେରନ୍ତୁ healthy;
}

ଶୂନ୍ୟ Curl_cpool_conn_was_used(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn,
                              ଅଚଳ ସଂରଚନା curltime *pnow)
{
  (ଶୂନ୍ୟ)data;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  ଅଚଳ ସଂରଚନା curltime *pnow)
{
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ curlx_ptimediff_ms(pnow, &conn->created);
}
