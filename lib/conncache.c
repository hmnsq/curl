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


#परिभाषय CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#परिभाषय CPOOL_तालकम्(c, d)                                                \
  कुरु {                                                                  \
    यदि(c) {                                                             \
      यदि(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_तालकम्_share((c)->share, (d), CURL_तालकम्_दत्त_CONNECT,  \
                        CURL_तालकम्_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } यावत्(0)

#परिभाषय CPOOL_UNLOCK(c, d)                                              \
  कुरु {                                                                  \
    यदि(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      यदि(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_तालकम्_दत्त_CONNECT); \
    }                                                                   \
  } यावत्(0)

/* A set of connection pool ids to the same destination. */
संरचना cpool_bundle {
  संरचना uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

स्थिर शून्य cpool_bundle_unlink(संरचना cpool_bundle **pbundle)
{
  यदि(*pbundle) {
    संरचना cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = रिक्त;
    यदि(bundle->refcount)
      bundle->refcount--;
    यदि(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

स्थिर शून्य cpool_bundle_link(संरचना cpool_bundle **pdest,
                              संरचना cpool_bundle *src)
{
  यदि(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    यदि(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

स्थिर संरचना cpool_bundle *cpool_create_bundle(शून्य)
{
  /* created with refcount == 1 */
  संरचना cpool_bundle *bundle = curlx_calloc(1, परिमाण(*bundle));
  यदि(!bundle)
    प्रत्यागम रिक्त;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  प्रत्यागम bundle;
}

/* Add a connection to a bundle */
स्थिर bool cpool_bundle_सङ्कलय_id(संरचना cpool *cpool,
                                संरचना cpool_bundle *bundle,
                                ध्रुव वर्ण *destination,
                                uint32_t pool_id)
{
  यदि(!Curl_uint32_spbset_सङ्कलय(&bundle->ids, pool_id))
    प्रत्यागम FALSE;
  यदि(Curl_uint32_spbset_गणनम्(&bundle->ids) == 1) {
    /* first one added, register bundle */
    संरचना cpool_bundle *new_ref = रिक्त;
    cpool_bundle_link(&new_ref, bundle);
    यदि(!Curl_hash_सङ्कलय(&cpool->dest2bundle,
                      destination, वाक्यदैर्घ्यम्(destination), new_ref)) {
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
  यदि(!Curl_uint32_spbset_गणनम्(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, वाक्यदैर्घ्यम्(destination));
  }
}

स्थिर शून्य cpool_bundle_hash_dtor(शून्य *प्रविष्टिः)
{
  संरचना cpool_bundle *bundle = प्रविष्टिः;
  cpool_bundle_unlink(&bundle);
}

स्थिर संरचना cpool_bundle *cpool_get_bundle(संरचना cpool *cpool,
                                             ध्रुव वर्ण *destination)
{
  संरचना cpool_bundle *bundle = रिक्त;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, वाक्यदैर्घ्यम्(destination)));
  प्रत्यागम bundle;
}

स्थिर शून्य cpool_remove_conn(संरचना cpool *cpool,
                              संरचना connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  यदि(cpid != UINT32_MAX) {
    संरचना cpool_bundle *bundle;

#यदि_परिभाषितम् DEBUGBUILD
    {
      संरचना connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#यदि_अन्तः
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    यदि(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

स्थिर शून्य cpool_discard_conn(संरचना cpool *cpool,
                               संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               bool aborted)
{
  संरचना Curl_easy *admin;
  bool समाप्तम् = FALSE;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(!दत्त->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(दत्त);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  यदि(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    प्रत्यागम;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  यदि(conn->द्वयङ्काः.connect_only)
    aborted = TRUE;
  conn->द्वयङ्काः.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  यदि(aborted)
    समाप्तम् = TRUE;
  यदि(!समाप्तम्) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &समाप्तम्);
  }

  यदि(समाप्तम् || !दत्त->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  अन्यथा {
    संरचना Curl_multi *multi = दत्त->multi;
    size_t max_shutdowns = multi->max_योग_connections;
    uint32_t अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);

    यदि(अङ्कः_conns < max_shutdowns)
      max_shutdowns -= अङ्कः_conns;
    अन्यथा यदि(max_shutdowns)
      max_shutdowns = 1;
    अन्यथा /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(अङ्कः_conns / 4, 128);
    Curl_cshutdn_सङ्कलय(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

शून्य Curl_cpool_init(संरचना cpool *cpool,
                     संरचना Curl_share *share,
                     size_t आकार)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, आकार, CURL_HASH_TYPE_अष्टकानि,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->अन्तर्_shutdown = FALSE;
  cpool->locked = FALSE;
}

शून्य Curl_cpool_destroy(संरचना cpool *cpool, संरचना Curl_easy *admin)
{
  यदि(cpool && cpool->initialized && admin) {
    शून्य *प्रविष्टिः;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_गणनम्(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_तालकम्(cpool, admin);
    यदि(Curl_uint32_tbl_first(&cpool->conns, &cpid, &प्रविष्टिः)) {
      संरचना Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      कुरु {
        संरचना connectdata *conn = प्रविष्टिः;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } यावत्(Curl_uint32_tbl_अग्र(&cpool->conns, cpid, &cpid, &प्रविष्टिः));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

स्थिर संरचना cpool *cpool_get_instance(संरचना Curl_easy *दत्त)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(दत्त->mid);
  यदि(CURL_SHARE_KEEP_CONNECT(दत्त->share))
    प्रत्यागम &दत्त->share->cpool;
  अन्यथा यदि(दत्त->multi_easy)
    प्रत्यागम &दत्त->multi_easy->cpool;
  अन्यथा यदि(दत्त->multi)
    प्रत्यागम &दत्त->multi->cpool;
  प्रत्यागम रिक्त;
}

संरचना cpool *Curl_cpool_get_instance(संरचना Curl_easy *दत्त)
{
  प्रत्यागम cpool_get_instance(दत्त);
}

शून्य Curl_cpool_xfer_init(संरचना Curl_easy *दत्त)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);

  यदि(cpool) {
    CPOOL_तालकम्(cpool, दत्त);
    /* the identifier inside the connection cache */
    दत्त->id = cpool->अग्र_easy_id++;
    यदि(cpool->अग्र_easy_id == CURL_OFF_T_MAX)
      cpool->अग्र_easy_id = 0;
    दत्त->state.चरमः_conn_id = -1;
    दत्त->state.चरमः_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, दत्त);
  }
  अन्यथा {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    दत्त->id = 0;
    दत्त->state.चरमः_conn_id = -1;
    दत्त->state.चरमः_cpid = UINT32_MAX;
  }
}

स्थिर bool cpool_conn_can_be_closed(संरचना connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  प्रत्यागम (conn && !CONN_INUSE(conn) && !conn->द्वयङ्काः.connect_only);
}

स्थिर संरचना connectdata *cpool_bundle_get_oldest_idle(
  संरचना cpool *cpool,
  संरचना cpool_bundle *bundle,
  ध्रुव संरचना curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t अप्रयुक्तम्_ms;
  timediff_t oldest_ms = -1;

  यदि(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
    कुरु {
      संरचना connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      यदि(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        अप्रयुक्तम्_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        यदि(अप्रयुक्तम्_ms > oldest_ms) {
          oldest_ms = अप्रयुक्तम्_ms;
          oldest_id = cpid;
        }
      }
      अन्यथा यदि(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } यावत्(Curl_uint32_spbset_अग्र(&bundle->ids, cpid, &cpid));
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
  यदि(Curl_uint32_bset_first(&cpool->idles, &cpid)) {
    कुरु {
      संरचना connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      यदि(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        यदि((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      अन्यथा {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } यावत्(Curl_uint32_bset_अग्र(&cpool->idles, cpid, &cpid));
  }
  प्रत्यागम Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

स्थिर शून्य cpool_conn_close(संरचना cpool *cpool,
                             संरचना Curl_easy *दत्त,
                             संरचना connectdata *conn,
                             bool aborted)
{
  संरचना Curl_easy *admin;
  bool do_तालकम्;

  DEBUGASSERT(cpool);
  DEBUGASSERT(दत्त && !दत्त->conn);
  यदि(!cpool)
    प्रत्यागम;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  यदि(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(दत्त, "conn terminate when inuse: %u", conn->attached_xfers));
    प्रत्यागम;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(दत्त);
  do_तालकम् = !CPOOL_IS_LOCKED(cpool);
  यदि(do_तालकम्)
    CPOOL_तालकम्(cpool, admin);

  यदि(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  यदि(conn->द्वयङ्काः.connect_only)
    aborted = TRUE;

  यदि(दत्त->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(दत्त, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&दत्त->multi->cpool, दत्त, conn, aborted);
  }
  अन्यथा {
    /* No multi available, terminate */
    infof(दत्त, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  यदि(do_तालकम्)
    CPOOL_UNLOCK(cpool, admin);
}

शून्य Curl_conn_close(संरचना Curl_easy *दत्त,
                     संरचना connectdata *conn,
                     bool aborted)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);
  cpool_conn_close(cpool, दत्त, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
स्थिर शून्य cpool_evict_conn(संरचना cpool *cpool,
                             संरचना Curl_easy *admin,
                             संरचना connectdata *conn)
{
  यदि(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  अन्यथा
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#परिभाषय CPOOL_सीमा_OK     0
#परिभाषय CPOOL_सीमा_DEST   1
#परिभाषय CPOOL_सीमा_योग  2

स्थिर पूर्णाङ्क cpool_check_limits(संरचना Curl_easy *दत्त,
                              uint32_t max_योग,
                              संरचना connectdata *गन्तव्यम्_सङ्कलय,
                              uint32_t max_host,
                              संरचना cpool_bundle *bundle,
                              ध्रुव संरचना curltime *pnow)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);
  संरचना cshutdn *cshutdn = दत्त->multi ? &दत्त->multi->cshutdn : रिक्त;
  संरचना Curl_easy *admin = रिक्त;

  यदि(!cpool)
    प्रत्यागम CPOOL_सीमा_OK;

  यदि(!max_योग && !max_host)
    प्रत्यागम CPOOL_सीमा_OK;

  यदि(max_host) {
    size_t जीवितम् = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(दत्त);
    यदि(bundle) {
      जीवितम् = Curl_uint32_spbset_गणनम्(&bundle->ids);
      यदि(जीवितम् >= max_host) {
        size_t over = जीवितम् - max_host + 1;
        प्रति(; over; --over) {
          संरचना connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          यदि(!oldest_idle)
            भङ्ग;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        जीवितम् = Curl_uint32_spbset_गणनम्(&bundle->ids);
      }
    }
    यदि(cshutdn) {
      shutdowns = Curl_cshutdn_dest_गणनम्(cshutdn, गन्तव्यम्_सङ्कलय->destination);
      यदि(shutdowns && ((जीवितम् + shutdowns) >= max_host)) {
        size_t over = जीवितम् + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               गन्तव्यम्_सङ्कलय->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    यदि((जीवितम् + shutdowns) >= max_host)
      प्रत्यागम CPOOL_सीमा_DEST;
  }

  /* Internal transfers are outside the total limit */
  यदि(max_योग && !दत्त->state.आन्तरिकम्) {
    size_t shutdowns = 0;
    uint32_t अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);

    यदि(!admin)
      admin = Curl_get_admin(दत्त);
    यदि(अङ्कः_conns >= max_योग) {
      size_t over = अङ्कः_conns - max_योग + 1;
      प्रति(; over; --over) {
        संरचना connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        यदि(!oldest_idle)
          भङ्ग;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_योग);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);
    यदि(cshutdn) {
      shutdowns = Curl_cshutdn_गणनम्(cshutdn);
      यदि(shutdowns && (अङ्कः_conns + shutdowns) >= max_योग) {
        size_t over = अङ्कः_conns + shutdowns - max_योग + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, रिक्त, over);
      }
    }
    यदि((अङ्कः_conns + shutdowns) >= max_योग)
      प्रत्यागम CPOOL_सीमा_योग;
  }

  प्रत्यागम CPOOL_सीमा_OK;
}

CURLcode Curl_cpool_सङ्कलय(संरचना Curl_easy *दत्त,
                        संरचना connectdata *conn,
                        uint32_t max_योग,
                        uint32_t max_host,
                        ध्रुव संरचना curltime *pnow)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना cpool_bundle *bundle = रिक्त;
  संरचना cpool *cpool = cpool_get_instance(दत्त);
  uint32_t क्षमता, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  यदि(!cpool)
    प्रत्यागम CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

  CPOOL_तालकम्(cpool, दत्त);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  विकल्प(cpool_check_limits(दत्त, max_योग, conn, max_host, bundle, pnow)) {
  पक्ष CPOOL_सीमा_DEST:
    infof(दत्त, "No more connections allowed to host");
    परिणाम = CURLE_NO_CONNECTION_उपलब्धम्;
    गच्छ बहिर्गमनम्;
  पक्ष CPOOL_सीमा_योग:
    infof(दत्त, "No connections available, total of %u reached.",
          दत्त->multi->max_योग_connections);
    परिणाम = CURLE_NO_CONNECTION_उपलब्धम्;
    गच्छ बहिर्गमनम्;
  स्वभाव:
    भङ्ग;
  }

  /* We add the connection */
  क्षमता = Curl_uint32_tbl_क्षमता(&cpool->conns);
  यदि(क्षमता == UINT32_MAX) {
    infof(दत्त, "No connections available, pool at max capacity");
    परिणाम = CURLE_NO_CONNECTION_उपलब्धम्;
    गच्छ बहिर्गमनम्;
  }
  यदि(Curl_uint32_tbl_गणनम्(&cpool->conns) >= क्षमता) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(क्षमता, 16);
    यदि((UINT32_MAX - growth) >= क्षमता)
      ncapacity = क्षमता + growth;
    अन्यथा
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    यदि(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(!bundle) {
    bundle = cpool_create_bundle();
    यदि(!bundle) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(!Curl_uint32_tbl_सङ्कलय(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  conn->cpid = cpid;

  यदि(!cpool_bundle_सङ्कलय_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  conn->connection_id = cpool->अग्र_connection_id++;
  CURL_TRC_M(दत्त, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_गणनम्(&cpool->conns));
बहिर्गमनम्:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, दत्त);

  प्रत्यागम परिणाम;
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
स्थिर bool cpool_foreach(संरचना Curl_easy *दत्त,
                          संरचना cpool *cpool,
                          शून्य *param,
                          पूर्णाङ्क (*func)(संरचना cpool *cpool,
                                      संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn, शून्य *param))
{
  शून्य *प्रविष्टिः;
  uint32_t cpid;

  यदि(!cpool)
    प्रत्यागम FALSE;

  यदि(Curl_uint32_tbl_first(&cpool->conns, &cpid, &प्रविष्टिः)) {
    कुरु {
      संरचना connectdata *conn = प्रविष्टिः;
      यदि(conn && func(cpool, दत्त, conn, param) == 1)
        प्रत्यागम TRUE;
    } यावत्(Curl_uint32_tbl_अग्र(&cpool->conns, cpid, &cpid, &प्रविष्टिः));
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
                                संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn,
                                ध्रुव संरचना curltime *pnow)
{
  संरचना connectdata *oldest_idle = रिक्त;
  संरचना Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  यदि(!दत्त || !दत्त->multi)
    प्रत्यागम kept;

  Curl_uint32_bset_सङ्कलय(&cpool->idles, conn->cpid);

  यदि(!दत्त->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(दत्त->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  अन्यथा {
    maxconnects = दत्त->multi->maxconnects;
  }

  यदि(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t अङ्कः_conns = Curl_uint32_tbl_गणनम्(&cpool->conns);
    admin = Curl_get_admin(दत्त);
    यदि(अङ्कः_conns > maxconnects) {
      infof(दत्त, "Connection pool is full, closing the oldest of %u/%u",
            अङ्कः_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      यदि(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  प्रत्यागम kept;
}

स्थिर शून्य cpool_prune_dead(संरचना cpool *cpool,
                             संरचना Curl_easy *दत्त,
                             ध्रुव संरचना curltime *pnow);

स्थिर bool cpool_find_act(संरचना cpool *cpool,
                           संरचना Curl_easy *दत्त,
                           संरचना connectdata *conn,
                           cpool_मेलनम्_परिणाम मेलनम्)
{
  विकल्प(मेलनम्) {
  पक्ष CPOOL_मेलनम्_प्राप्तम्:
    प्रत्यागम TRUE;
  पक्ष CPOOL_मेलनम्_TOO_OLD:
    यदि(!cpool_conn_can_be_closed(conn))
      प्रत्यागम FALSE;
    FALLTHROUGH();
  पक्ष CPOOL_मेलनम्_CLOSE:
    cpool_conn_close(cpool, दत्त, conn, FALSE);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम FALSE;
}

bool Curl_cpool_find(संरचना Curl_easy *दत्त,
                     ध्रुव वर्ण *destination,
                     bool prune_dead,
                     ध्रुव संरचना curltime *pnow,
                     Curl_cpool_conn_मेलनम्_cb *conn_cb,
                     Curl_cpool_समाप्तम्_मेलनम्_cb *समाप्तम्_cb,
                     शून्य *userdata)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);
  संरचना cpool_bundle *bundle = रिक्त;
  bool प्राप्तम् = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  यदि(!cpool)
    प्रत्यागम FALSE;

  CPOOL_तालकम्(cpool, दत्त);

  यदि(prune_dead)
    cpool_prune_dead(cpool, दत्त, pnow);

  यदि(दत्त->state.चरमः_cpid != UINT32_MAX) {
    संरचना connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, दत्त->state.चरमः_cpid);
    यदि(conn && (दत्त->state.चरमः_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      प्राप्तम् = cpool_find_act(cpool, दत्त, conn, conn_cb(conn, userdata));
      यदि(प्राप्तम्)
        गच्छ बहिर्गमनम्;
    }
    अन्यथा {
      /* No longer found here. */
      दत्त->state.चरमः_conn_id = -1;
      दत्त->state.चरमः_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  यदि(bundle) {
    uint32_t cpid;
    यदि(Curl_uint32_spbset_first(&bundle->ids, &cpid)) {
      कुरु {
        संरचना connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        यदि(conn) {
          यदि(दत्त->state.चरमः_conn_id != conn->connection_id) {
            प्राप्तम् = cpool_find_act(cpool, दत्त, conn, conn_cb(conn, userdata));
            यदि(प्राप्तम्)
              गच्छ बहिर्गमनम्;
          }
        }
        अन्यथा {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } यावत्(Curl_uint32_spbset_अग्र(&bundle->ids, cpid, &cpid));
    }
  }

बहिर्गमनम्:
  यदि(समाप्तम्_cb) {
    प्राप्तम् = समाप्तम्_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, दत्त);
  प्रत्यागम प्राप्तम्;
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

  यदि(cpool_conn_can_be_closed(conn)) {
    यदि(conn->द्वयङ्काः.no_reuse || conn->द्वयङ्काः.close ||
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
                             संरचना Curl_easy *दत्त,
                             ध्रुव संरचना curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->चरमः_cleanup);

  यदि(elapsed_ms >= 1000L) {
    संरचना Curl_easy *admin = Curl_get_admin(दत्त);
    संरचना cpool_reaper_ctx reaper;

    स्मृतिं_पूरय(&reaper, 0, परिमाण(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->चरमः_cleanup = *pnow;
  }
}

स्थिर पूर्णाङ्क conn_upkeep(संरचना cpool *cpool,
                       संरचना Curl_easy *admin,
                       संरचना connectdata *conn,
                       शून्य *param)
{
  ध्रुव संरचना curltime *pnow = Curl_pgrs_now(admin);

  (शून्य)param;
  यदि((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->समुच्चयः.upkeep_interval_ms) {
    CURLcode परिणाम;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    परिणाम = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    यदि(परिणाम && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  प्रत्यागम 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(संरचना Curl_easy *दत्त)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);
  संरचना Curl_easy *admin = Curl_get_admin(दत्त);

  यदि(!cpool)
    प्रत्यागम CURLE_OK;

  CPOOL_तालकम्(cpool, admin);
  cpool_foreach(admin, cpool, रिक्त, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  प्रत्यागम CURLE_OK;
}

संरचना connectdata *Curl_cpool_get_चरमः_conn(संरचना Curl_easy *दत्त)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);
  यदि(cpool && (दत्त->state.चरमः_cpid != UINT32_MAX)) {
    संरचना connectdata *conn;

    CPOOL_तालकम्(cpool, दत्त);
    conn = Curl_uint32_tbl_get(&cpool->conns, दत्त->state.चरमः_cpid);
    CPOOL_UNLOCK(cpool, दत्त);
    यदि(conn && (दत्त->state.चरमः_conn_id == conn->connection_id))
      प्रत्यागम conn;
    दत्त->state.चरमः_cpid = UINT32_MAX;
    दत्त->state.चरमः_conn_id = -1;
  }
  प्रत्यागम रिक्त;
}

शून्य Curl_cpool_return(संरचना Curl_easy *दत्त,
                       संरचना connectdata *conn,
                       Curl_cpool_return_cb *cb, शून्य *cbdata,
                       ध्रुव संरचना curltime *pnow)
{
  संरचना cpool *cpool = cpool_get_instance(दत्त);

  CPOOL_तालकम्(cpool, दत्त);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  विकल्प(cb(दत्त, conn, cbdata, pnow)) {
  पक्ष CPOOL_DO_KEEP:
    भङ्ग;
  पक्ष CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    यदि(cpool_conn_now_idle(cpool, दत्त, conn, pnow)) {
      /* connection kept in the cpool */
      infof(दत्त, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->पत्तनम्);
    }
    अन्यथा { /* connection was removed from the cpool and destroyed. */
      दत्त->state.चरमः_conn_id = -1;
      दत्त->state.चरमः_cpid = UINT32_MAX;
    }
    भङ्ग;
  पक्ष CPOOL_DO_CLOSE:
    Curl_conn_close(दत्त, conn, FALSE);
    भङ्ग;
  पक्ष CPOOL_DO_TERMINATE:
    Curl_conn_close(दत्त, conn, TRUE);
    भङ्ग;
  }

  CPOOL_UNLOCK(cpool, दत्त);
}

स्थिर पूर्णाङ्क cpool_reap_no_reuse(संरचना cpool *cpool,
                               संरचना Curl_easy *admin,
                               संरचना connectdata *conn, शून्य *param)
{
  (शून्य)param;
  conn->द्वयङ्काः.no_reuse = TRUE;
  यदि(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  प्रत्यागम 0; /* continue iteration */
}

शून्य Curl_cpool_nw_परिवर्तितम्(संरचना cpool *cpool, संरचना Curl_easy *admin)
{
  यदि(cpool && admin) {
    CPOOL_तालकम्(cpool, admin);
    cpool_foreach(admin, cpool, रिक्त, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
स्थिर bool cpool_conn_too_old(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               ध्रुव संरचना curltime *pnow)
{
  timediff_t age_ms;

  यदि(दत्त->समुच्चयः.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    यदि(age_ms > दत्त->समुच्चयः.conn_max_idle_ms) {
      infof(दत्त, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, दत्त->समुच्चयः.conn_max_idle_ms);
      प्रत्यागम TRUE;
    }
  }

  यदि(दत्त->समुच्चयः.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    यदि(age_ms > दत्त->समुच्चयः.conn_max_age_ms) {
      infof(दत्त,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, दत्त->समुच्चयः.conn_max_age_ms);
      प्रत्यागम TRUE;
    }
  }

  प्रत्यागम FALSE;
}

bool Curl_cpool_conn_seems_healthy(संरचना connectdata *conn,
                                   संरचना Curl_easy *दत्त,
                                   ध्रुव संरचना curltime *pnow)
{
  संरचना Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_निवेशः;

  DEBUGASSERT(!दत्त->conn);
  यदि(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(दत्त, conn, pnow))
    प्रत्यागम FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_निवेशः = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->सततम्->connection_is_dead;
  यदि(!poll_निवेशः &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    प्रत्यागम TRUE;

  admin = Curl_get_admin(दत्त);
  यदि(conn->scheme->सततम्->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->सततम्->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  अन्यथा {
    bool निवेशः_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &निवेशः_pending);
    Curl_detach_connection(admin);
    यदि(healthy && निवेशः_pending && poll_निवेशः) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(दत्त, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  यदि(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  प्रत्यागम healthy;
}

शून्य Curl_cpool_conn_was_प्रयुक्तम्(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn,
                              ध्रुव संरचना curltime *pnow)
{
  (शून्य)दत्त;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  ध्रुव संरचना curltime *pnow)
{
  (शून्य)दत्त;
  प्रत्यागम curlx_ptimediff_ms(pnow, &conn->created);
}
