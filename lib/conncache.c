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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "progress.h"
#समावेश "multiif.h"
#समावेश "curl_trc.h"
#समावेश "cshutdn.h"
#समावेश "conncache.h"
#समावेश "curl_share.h"
#समावेश "sigpipe.h"
#समावेश "uint-spbset.h"


#घोषणा CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#घोषणा CPOOL_LOCK(c, d)                                                \
  करो {                                                                  \
    अगर(c) {                                                             \
      अगर(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_lock_share((c)->share, (d), CURL_LOCK_तथ्य_CONNECT,  \
                        CURL_LOCK_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } जबतक(0)

#घोषणा CPOOL_UNLOCK(c, d)                                              \
  करो {                                                                  \
    अगर(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      अगर(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_LOCK_तथ्य_CONNECT); \
    }                                                                   \
  } जबतक(0)

/* A set of connection pool ids to the same destination. */
काष्ठा cpool_bundle {
  काष्ठा uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

अटल व्योम cpool_bundle_unlink(काष्ठा cpool_bundle **pbundle)
{
  अगर(*pbundle) {
    काष्ठा cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = शून्य;
    अगर(bundle->refcount)
      bundle->refcount--;
    अगर(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

अटल व्योम cpool_bundle_link(काष्ठा cpool_bundle **pdest,
                              काष्ठा cpool_bundle *src)
{
  अगर(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    अगर(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

अटल काष्ठा cpool_bundle *cpool_create_bundle(व्योम)
{
  /* created with refcount == 1 */
  काष्ठा cpool_bundle *bundle = curlx_calloc(1, माप(*bundle));
  अगर(!bundle)
    वापस शून्य;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  वापस bundle;
}

/* Add a connection to a bundle */
अटल bool cpool_bundle_add_id(काष्ठा cpool *cpool,
                                काष्ठा cpool_bundle *bundle,
                                स्थिर अक्षर *destination,
                                uint32_t pool_id)
{
  अगर(!Curl_uint32_spbset_add(&bundle->ids, pool_id))
    वापस FALSE;
  अगर(Curl_uint32_spbset_गिनती(&bundle->ids) == 1) {
    /* first one added, register bundle */
    काष्ठा cpool_bundle *new_ref = शून्य;
    cpool_bundle_link(&new_ref, bundle);
    अगर(!Curl_hash_add(&cpool->dest2bundle,
                      destination, म_माप(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      वापस FALSE;
    }
  }
  वापस TRUE;
}

/* Remove a connection from a bundle */
अटल व्योम cpool_bundle_remove_id(काष्ठा cpool *cpool,
                                   काष्ठा cpool_bundle *bundle,
                                   स्थिर अक्षर *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  अगर(!Curl_uint32_spbset_गिनती(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, म_माप(destination));
  }
}

अटल व्योम cpool_bundle_hash_dtor(व्योम *entry)
{
  काष्ठा cpool_bundle *bundle = entry;
  cpool_bundle_unlink(&bundle);
}

अटल काष्ठा cpool_bundle *cpool_get_bundle(काष्ठा cpool *cpool,
                                             स्थिर अक्षर *destination)
{
  काष्ठा cpool_bundle *bundle = शून्य;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, म_माप(destination)));
  वापस bundle;
}

अटल व्योम cpool_remove_conn(काष्ठा cpool *cpool,
                              काष्ठा connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  अगर(cpid != UINT32_MAX) {
    काष्ठा cpool_bundle *bundle;

#अगर_घोषित DEBUGBUILD
    {
      काष्ठा connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#पूर्ण_अगर
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    अगर(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

अटल व्योम cpool_discard_conn(काष्ठा cpool *cpool,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               bool aborted)
{
  काष्ठा Curl_easy *admin;
  bool done = FALSE;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(!तथ्य->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(तथ्य);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  अगर(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    वापस;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  अगर(conn->bits.connect_only)
    aborted = TRUE;
  conn->bits.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  अगर(aborted)
    done = TRUE;
  अगर(!done) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &done);
  }

  अगर(done || !तथ्य->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  अन्यथा {
    काष्ठा Curl_multi *multi = तथ्य->multi;
    माप_प्रकार max_shutdowns = multi->max_योग_connections;
    uint32_t अंक_conns = Curl_uint32_tbl_गिनती(&cpool->conns);

    अगर(अंक_conns < max_shutdowns)
      max_shutdowns -= अंक_conns;
    अन्यथा अगर(max_shutdowns)
      max_shutdowns = 1;
    अन्यथा /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(अंक_conns / 4, 128);
    Curl_cshutdn_add(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

व्योम Curl_cpool_init(काष्ठा cpool *cpool,
                     काष्ठा Curl_share *share,
                     माप_प्रकार आकार)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, आकार, CURL_HASH_TYPE_BYTES,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->in_shutdown = FALSE;
  cpool->locked = FALSE;
}

व्योम Curl_cpool_destroy(काष्ठा cpool *cpool, काष्ठा Curl_easy *admin)
{
  अगर(cpool && cpool->initialized && admin) {
    व्योम *entry;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_गिनती(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_LOCK(cpool, admin);
    अगर(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
      काष्ठा Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      करो {
        काष्ठा connectdata *conn = entry;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } जबतक(Curl_uint32_tbl_अग्र(&cpool->conns, cpid, &cpid, &entry));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

अटल काष्ठा cpool *cpool_get_instance(काष्ठा Curl_easy *तथ्य)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(तथ्य->mid);
  अगर(CURL_SHARE_KEEP_CONNECT(तथ्य->share))
    वापस &तथ्य->share->cpool;
  अन्यथा अगर(तथ्य->multi_easy)
    वापस &तथ्य->multi_easy->cpool;
  अन्यथा अगर(तथ्य->multi)
    वापस &तथ्य->multi->cpool;
  वापस शून्य;
}

काष्ठा cpool *Curl_cpool_get_instance(काष्ठा Curl_easy *तथ्य)
{
  वापस cpool_get_instance(तथ्य);
}

व्योम Curl_cpool_xfer_init(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);

  अगर(cpool) {
    CPOOL_LOCK(cpool, तथ्य);
    /* the identifier inside the connection cache */
    तथ्य->id = cpool->अग्र_easy_id++;
    अगर(cpool->अग्र_easy_id == CURL_OFF_T_MAX)
      cpool->अग्र_easy_id = 0;
    तथ्य->state.last_conn_id = -1;
    तथ्य->state.last_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, तथ्य);
  }
  अन्यथा {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    तथ्य->id = 0;
    तथ्य->state.last_conn_id = -1;
    तथ्य->state.last_cpid = UINT32_MAX;
  }
}

अटल bool cpool_conn_can_be_closed(काष्ठा connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  वापस (conn && !CONN_INUSE(conn) && !conn->bits.connect_only);
}

अटल काष्ठा connectdata *cpool_bundle_get_oldest_idle(
  काष्ठा cpool *cpool,
  काष्ठा cpool_bundle *bundle,
  स्थिर काष्ठा curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t unused_ms;
  timediff_t oldest_ms = -1;

  अगर(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    करो {
      काष्ठा connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      अगर(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        unused_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        अगर(unused_ms > oldest_ms) {
          oldest_ms = unused_ms;
          oldest_id = cpid;
        }
      }
      अन्यथा अगर(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } जबतक(Curl_uint32_spbset_अग्र(&bundle->ids, cpid, &cpid));
  }
  वापस Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

अटल काष्ठा connectdata *cpool_get_oldest_idle(काष्ठा cpool *cpool,
                                                 स्थिर काष्ठा curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  अगर(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    करो {
      काष्ठा connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      अगर(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        अगर((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      अन्यथा {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } जबतक(Curl_uint32_bset_अग्र(&cpool->idles, cpid, &cpid));
  }
  वापस Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

अटल व्योम cpool_conn_close(काष्ठा cpool *cpool,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn,
                             bool aborted)
{
  काष्ठा Curl_easy *admin;
  bool do_lock;

  DEBUGASSERT(cpool);
  DEBUGASSERT(तथ्य && !तथ्य->conn);
  अगर(!cpool)
    वापस;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  अगर(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(तथ्य, "conn terminate when inuse: %u", conn->attached_xfers));
    वापस;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(तथ्य);
  do_lock = !CPOOL_IS_LOCKED(cpool);
  अगर(do_lock)
    CPOOL_LOCK(cpool, admin);

  अगर(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  अगर(conn->bits.connect_only)
    aborted = TRUE;

  अगर(तथ्य->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(तथ्य, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&तथ्य->multi->cpool, तथ्य, conn, aborted);
  }
  अन्यथा {
    /* No multi available, terminate */
    infof(तथ्य, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  अगर(do_lock)
    CPOOL_UNLOCK(cpool, admin);
}

व्योम Curl_conn_close(काष्ठा Curl_easy *तथ्य,
                     काष्ठा connectdata *conn,
                     bool aborted)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);
  cpool_conn_close(cpool, तथ्य, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
अटल व्योम cpool_evict_conn(काष्ठा cpool *cpool,
                             काष्ठा Curl_easy *admin,
                             काष्ठा connectdata *conn)
{
  अगर(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  अन्यथा
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#घोषणा CPOOL_LIMIT_OK     0
#घोषणा CPOOL_LIMIT_DEST   1
#घोषणा CPOOL_LIMIT_योग  2

अटल पूर्णांक cpool_check_limits(काष्ठा Curl_easy *तथ्य,
                              uint32_t max_योग,
                              काष्ठा connectdata *to_add,
                              uint32_t max_host,
                              काष्ठा cpool_bundle *bundle,
                              स्थिर काष्ठा curltime *pnow)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);
  काष्ठा cshutdn *cshutdn = तथ्य->multi ? &तथ्य->multi->cshutdn : शून्य;
  काष्ठा Curl_easy *admin = शून्य;

  अगर(!cpool)
    वापस CPOOL_LIMIT_OK;

  अगर(!max_योग && !max_host)
    वापस CPOOL_LIMIT_OK;

  अगर(max_host) {
    माप_प्रकार live = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(तथ्य);
    अगर(bundle) {
      live = Curl_uint32_spbset_गिनती(&bundle->ids);
      अगर(live >= max_host) {
        माप_प्रकार over = live - max_host + 1;
        क्रम(; over; --over) {
          काष्ठा connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          अगर(!oldest_idle)
            अवरोध;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        live = Curl_uint32_spbset_गिनती(&bundle->ids);
      }
    }
    अगर(cshutdn) {
      shutdowns = Curl_cshutdn_dest_गिनती(cshutdn, to_add->destination);
      अगर(shutdowns && ((live + shutdowns) >= max_host)) {
        माप_प्रकार over = live + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               to_add->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    अगर((live + shutdowns) >= max_host)
      वापस CPOOL_LIMIT_DEST;
  }

  /* Internal transfers are outside the total limit */
  अगर(max_योग && !तथ्य->state.internal) {
    माप_प्रकार shutdowns = 0;
    uint32_t अंक_conns = Curl_uint32_tbl_गिनती(&cpool->conns);

    अगर(!admin)
      admin = Curl_get_admin(तथ्य);
    अगर(अंक_conns >= max_योग) {
      माप_प्रकार over = अंक_conns - max_योग + 1;
      क्रम(; over; --over) {
        काष्ठा connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        अगर(!oldest_idle)
          अवरोध;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_योग);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    अंक_conns = Curl_uint32_tbl_गिनती(&cpool->conns);
    अगर(cshutdn) {
      shutdowns = Curl_cshutdn_गिनती(cshutdn);
      अगर(shutdowns && (अंक_conns + shutdowns) >= max_योग) {
        माप_प्रकार over = अंक_conns + shutdowns - max_योग + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, शून्य, over);
      }
    }
    अगर((अंक_conns + shutdowns) >= max_योग)
      वापस CPOOL_LIMIT_योग;
  }

  वापस CPOOL_LIMIT_OK;
}

CURLcode Curl_cpool_add(काष्ठा Curl_easy *तथ्य,
                        काष्ठा connectdata *conn,
                        uint32_t max_योग,
                        uint32_t max_host,
                        स्थिर काष्ठा curltime *pnow)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cpool_bundle *bundle = शून्य;
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);
  uint32_t capacity, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  अगर(!cpool)
    वापस CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_LOCK(cpool, तथ्य);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  चयन(cpool_check_limits(तथ्य, max_योग, conn, max_host, bundle, pnow)) {
  हाल CPOOL_LIMIT_DEST:
    infof(तथ्य, "No more connections allowed to host");
    परिणाम = CURLE_NO_CONNECTION_AVAILABLE;
    जाओ out;
  हाल CPOOL_LIMIT_योग:
    infof(तथ्य, "No connections available, total of %u reached.",
          तथ्य->multi->max_योग_connections);
    परिणाम = CURLE_NO_CONNECTION_AVAILABLE;
    जाओ out;
  शेष:
    अवरोध;
  }

  /* We add the connection */
  capacity = Curl_uint32_tbl_capacity(&cpool->conns);
  अगर(capacity == UINT32_MAX) {
    infof(तथ्य, "No connections available, pool at max capacity");
    परिणाम = CURLE_NO_CONNECTION_AVAILABLE;
    जाओ out;
  }
  अगर(Curl_uint32_tbl_गिनती(&cpool->conns) >= capacity) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(capacity, 16);
    अगर((UINT32_MAX - growth) >= capacity)
      ncapacity = capacity + growth;
    अन्यथा
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    अगर(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  अगर(!bundle) {
    bundle = cpool_create_bundle();
    अगर(!bundle) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  अगर(!Curl_uint32_tbl_add(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  conn->cpid = cpid;

  अगर(!cpool_bundle_add_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  conn->connection_id = cpool->अग्र_connection_id++;
  CURL_TRC_M(तथ्य, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_गिनती(&cpool->conns));
out:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, तथ्य);

  वापस परिणाम;
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
अटल bool cpool_foreach(काष्ठा Curl_easy *तथ्य,
                          काष्ठा cpool *cpool,
                          व्योम *param,
                          पूर्णांक (*func)(काष्ठा cpool *cpool,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn, व्योम *param))
{
  व्योम *entry;
  uint32_t cpid;

  अगर(!cpool)
    वापस FALSE;

  अगर(Curl_uint32_tbl_first(&cpool->conns, &cpid, &entry)) {
    करो {
      काष्ठा connectdata *conn = entry;
      अगर(conn && func(cpool, तथ्य, conn, param) == 1)
        वापस TRUE;
    } जबतक(Curl_uint32_tbl_अग्र(&cpool->conns, cpid, &cpid, &entry));
  }
  वापस FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
अटल bool cpool_conn_now_idle(काष्ठा cpool *cpool,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn,
                                स्थिर काष्ठा curltime *pnow)
{
  काष्ठा connectdata *oldest_idle = शून्य;
  काष्ठा Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  अगर(!तथ्य || !तथ्य->multi)
    वापस kept;

  Curl_uint32_bset_add(&cpool->idles, conn->cpid);

  अगर(!तथ्य->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(तथ्य->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  अन्यथा {
    maxconnects = तथ्य->multi->maxconnects;
  }

  अगर(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t अंक_conns = Curl_uint32_tbl_गिनती(&cpool->conns);
    admin = Curl_get_admin(तथ्य);
    अगर(अंक_conns > maxconnects) {
      infof(तथ्य, "Connection pool is full, closing the oldest of %u/%u",
            अंक_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      अगर(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  वापस kept;
}

अटल व्योम cpool_prune_dead(काष्ठा cpool *cpool,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा curltime *pnow);

अटल bool cpool_find_act(काष्ठा cpool *cpool,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा connectdata *conn,
                           cpool_match_परिणाम match)
{
  चयन(match) {
  हाल CPOOL_MATCH_FOUND:
    वापस TRUE;
  हाल CPOOL_MATCH_TOO_OLD:
    अगर(!cpool_conn_can_be_closed(conn))
      वापस FALSE;
    FALLTHROUGH();
  हाल CPOOL_MATCH_CLOSE:
    cpool_conn_close(cpool, तथ्य, conn, FALSE);
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस FALSE;
}

bool Curl_cpool_find(काष्ठा Curl_easy *तथ्य,
                     स्थिर अक्षर *destination,
                     bool prune_dead,
                     स्थिर काष्ठा curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     व्योम *userdata)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);
  काष्ठा cpool_bundle *bundle = शून्य;
  bool found = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  अगर(!cpool)
    वापस FALSE;

  CPOOL_LOCK(cpool, तथ्य);

  अगर(prune_dead)
    cpool_prune_dead(cpool, तथ्य, pnow);

  अगर(तथ्य->state.last_cpid != UINT32_MAX) {
    काष्ठा connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, तथ्य->state.last_cpid);
    अगर(conn && (तथ्य->state.last_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      found = cpool_find_act(cpool, तथ्य, conn, conn_cb(conn, userdata));
      अगर(found)
        जाओ out;
    }
    अन्यथा {
      /* No longer found here. */
      तथ्य->state.last_conn_id = -1;
      तथ्य->state.last_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  अगर(bundle) {
    uint32_t cpid;
    अगर(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      करो {
        काष्ठा connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        अगर(conn) {
          अगर(तथ्य->state.last_conn_id != conn->connection_id) {
            found = cpool_find_act(cpool, तथ्य, conn, conn_cb(conn, userdata));
            अगर(found)
              जाओ out;
          }
        }
        अन्यथा {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } जबतक(Curl_uint32_spbset_अग्र(&bundle->ids, cpid, &cpid));
    }
  }

out:
  अगर(done_cb) {
    found = done_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, तथ्य);
  वापस found;
}

काष्ठा cpool_reaper_ctx {
  माप_प्रकार reaped;
  स्थिर काष्ठा curltime *pnow;
};

अटल पूर्णांक cpool_reap_dead_cb(काष्ठा cpool *cpool,
                              काष्ठा Curl_easy *admin,
                              काष्ठा connectdata *conn, व्योम *param)
{
  काष्ठा cpool_reaper_ctx *reaper = param;

  अगर(cpool_conn_can_be_closed(conn)) {
    अगर(conn->bits.no_reuse || conn->bits.बंद ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  वापस 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
अटल व्योम cpool_prune_dead(काष्ठा cpool *cpool,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->last_cleanup);

  अगर(elapsed_ms >= 1000L) {
    काष्ठा Curl_easy *admin = Curl_get_admin(तथ्य);
    काष्ठा cpool_reaper_ctx reaper;

    स_रखो(&reaper, 0, माप(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->last_cleanup = *pnow;
  }
}

अटल पूर्णांक conn_upkeep(काष्ठा cpool *cpool,
                       काष्ठा Curl_easy *admin,
                       काष्ठा connectdata *conn,
                       व्योम *param)
{
  स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(admin);

  (व्योम)param;
  अगर((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->set.upkeep_interval_ms) {
    CURLcode परिणाम;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    परिणाम = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    अगर(परिणाम && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  वापस 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);
  काष्ठा Curl_easy *admin = Curl_get_admin(तथ्य);

  अगर(!cpool)
    वापस CURLE_OK;

  CPOOL_LOCK(cpool, admin);
  cpool_foreach(admin, cpool, शून्य, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  वापस CURLE_OK;
}

काष्ठा connectdata *Curl_cpool_get_last_conn(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);
  अगर(cpool && (तथ्य->state.last_cpid != UINT32_MAX)) {
    काष्ठा connectdata *conn;

    CPOOL_LOCK(cpool, तथ्य);
    conn = Curl_uint32_tbl_get(&cpool->conns, तथ्य->state.last_cpid);
    CPOOL_UNLOCK(cpool, तथ्य);
    अगर(conn && (तथ्य->state.last_conn_id == conn->connection_id))
      वापस conn;
    तथ्य->state.last_cpid = UINT32_MAX;
    तथ्य->state.last_conn_id = -1;
  }
  वापस शून्य;
}

व्योम Curl_cpool_return(काष्ठा Curl_easy *तथ्य,
                       काष्ठा connectdata *conn,
                       Curl_cpool_return_cb *cb, व्योम *cbdata,
                       स्थिर काष्ठा curltime *pnow)
{
  काष्ठा cpool *cpool = cpool_get_instance(तथ्य);

  CPOOL_LOCK(cpool, तथ्य);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  चयन(cb(तथ्य, conn, cbdata, pnow)) {
  हाल CPOOL_DO_KEEP:
    अवरोध;
  हाल CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    अगर(cpool_conn_now_idle(cpool, तथ्य, conn, pnow)) {
      /* connection kept in the cpool */
      infof(तथ्य, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->port);
    }
    अन्यथा { /* connection was removed from the cpool and destroyed. */
      तथ्य->state.last_conn_id = -1;
      तथ्य->state.last_cpid = UINT32_MAX;
    }
    अवरोध;
  हाल CPOOL_DO_CLOSE:
    Curl_conn_close(तथ्य, conn, FALSE);
    अवरोध;
  हाल CPOOL_DO_TERMINATE:
    Curl_conn_close(तथ्य, conn, TRUE);
    अवरोध;
  }

  CPOOL_UNLOCK(cpool, तथ्य);
}

अटल पूर्णांक cpool_reap_no_reuse(काष्ठा cpool *cpool,
                               काष्ठा Curl_easy *admin,
                               काष्ठा connectdata *conn, व्योम *param)
{
  (व्योम)param;
  conn->bits.no_reuse = TRUE;
  अगर(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  वापस 0; /* continue iteration */
}

व्योम Curl_cpool_nw_changed(काष्ठा cpool *cpool, काष्ठा Curl_easy *admin)
{
  अगर(cpool && admin) {
    CPOOL_LOCK(cpool, admin);
    cpool_foreach(admin, cpool, शून्य, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
अटल bool cpool_conn_too_old(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               स्थिर काष्ठा curltime *pnow)
{
  timediff_t age_ms;

  अगर(तथ्य->set.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    अगर(age_ms > तथ्य->set.conn_max_idle_ms) {
      infof(तथ्य, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, तथ्य->set.conn_max_idle_ms);
      वापस TRUE;
    }
  }

  अगर(तथ्य->set.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    अगर(age_ms > तथ्य->set.conn_max_age_ms) {
      infof(तथ्य,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, तथ्य->set.conn_max_age_ms);
      वापस TRUE;
    }
  }

  वापस FALSE;
}

bool Curl_cpool_conn_seems_healthy(काष्ठा connectdata *conn,
                                   काष्ठा Curl_easy *तथ्य,
                                   स्थिर काष्ठा curltime *pnow)
{
  काष्ठा Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_निवेश;

  DEBUGASSERT(!तथ्य->conn);
  अगर(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(तथ्य, conn, pnow))
    वापस FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_निवेश = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->run->connection_is_dead;
  अगर(!poll_निवेश &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    वापस TRUE;

  admin = Curl_get_admin(तथ्य);
  अगर(conn->scheme->run->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->run->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  अन्यथा {
    bool निवेश_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &निवेश_pending);
    Curl_detach_connection(admin);
    अगर(healthy && निवेश_pending && poll_निवेश) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(तथ्य, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  अगर(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  वापस healthy;
}

व्योम Curl_cpool_conn_was_used(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn,
                              स्थिर काष्ठा curltime *pnow)
{
  (व्योम)तथ्य;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  स्थिर काष्ठा curltime *pnow)
{
  (व्योम)तथ्य;
  वापस curlx_ptimediff_ms(pnow, &conn->created);
}
