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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "progress.h"
#ضمن "multiif.h"
#ضمن "curl_trc.h"
#ضمن "cshutdn.h"
#ضمن "conncache.h"
#ضمن "curl_share.h"
#ضمن "sigpipe.h"
#ضمن "uint-spbset.h"


#وصف_ڪريو CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#وصف_ڪريو CPOOL_تالو(c, d)                                                \
  افعل {                                                                  \
    إذا(c) {                                                             \
      إذا(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_تالو_share((c)->share, (d), CURL_تالو_معلومات_CONNECT,  \
                        CURL_تالو_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } طالما(0)

#وصف_ڪريو CPOOL_UNLOCK(c, d)                                              \
  افعل {                                                                  \
    إذا(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      إذا(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_تالو_معلومات_CONNECT); \
    }                                                                   \
  } طالما(0)

/* A set of connection pool ids to the same destination. */
بنية cpool_bundle {
  بنية uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ساكن فراغ cpool_bundle_unlink(بنية cpool_bundle **pbundle)
{
  إذا(*pbundle) {
    بنية cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = عدم;
    إذا(bundle->refcount)
      bundle->refcount--;
    إذا(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ساكن فراغ cpool_bundle_link(بنية cpool_bundle **pdest,
                              بنية cpool_bundle *src)
{
  إذا(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    إذا(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ساكن بنية cpool_bundle *cpool_create_bundle(فراغ)
{
  /* created with refcount == 1 */
  بنية cpool_bundle *bundle = curlx_calloc(1, حجم(*bundle));
  إذا(!bundle)
    أرجع عدم;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  أرجع bundle;
}

/* Add a connection to a bundle */
ساكن bool cpool_bundle_جوڙيو_id(بنية cpool *cpool,
                                بنية cpool_bundle *bundle,
                                ثابت حرف *destination,
                                uint32_t pool_id)
{
  إذا(!Curl_uint32_spbset_جوڙيو(&bundle->ids, pool_id))
    أرجع FALSE;
  إذا(Curl_uint32_spbset_ڳڻپ(&bundle->ids) == 1) {
    /* first one added, register bundle */
    بنية cpool_bundle *new_ref = عدم;
    cpool_bundle_link(&new_ref, bundle);
    إذا(!Curl_hash_جوڙيو(&cpool->dest2bundle,
                      destination, لڙي_ڊيگهه(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      أرجع FALSE;
    }
  }
  أرجع TRUE;
}

/* Remove a connection from a bundle */
ساكن فراغ cpool_bundle_remove_id(بنية cpool *cpool,
                                   بنية cpool_bundle *bundle,
                                   ثابت حرف *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  إذا(!Curl_uint32_spbset_ڳڻپ(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, لڙي_ڊيگهه(destination));
  }
}

ساكن فراغ cpool_bundle_hash_dtor(فراغ *داخلا_2)
{
  بنية cpool_bundle *bundle = داخلا_2;
  cpool_bundle_unlink(&bundle);
}

ساكن بنية cpool_bundle *cpool_get_bundle(بنية cpool *cpool,
                                             ثابت حرف *destination)
{
  بنية cpool_bundle *bundle = عدم;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, لڙي_ڊيگهه(destination)));
  أرجع bundle;
}

ساكن فراغ cpool_remove_conn(بنية cpool *cpool,
                              بنية connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  إذا(cpid != UINT32_MAX) {
    بنية cpool_bundle *bundle;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    {
      بنية connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#جيڪڏهن_پورو
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    إذا(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ساكن فراغ cpool_discard_conn(بنية cpool *cpool,
                               بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               bool aborted)
{
  بنية Curl_easy *admin;
  bool پورو = FALSE;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(!معلومات->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(معلومات);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  إذا(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    أرجع;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  إذا(conn->بٽ.connect_only)
    aborted = TRUE;
  conn->بٽ.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  إذا(aborted)
    پورو = TRUE;
  إذا(!پورو) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &پورو);
  }

  إذا(پورو || !معلومات->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  وإلا {
    بنية Curl_multi *multi = معلومات->multi;
    size_t max_shutdowns = multi->max_ڪل_connections;
    uint32_t انگ_conns = Curl_uint32_tbl_ڳڻپ(&cpool->conns);

    إذا(انگ_conns < max_shutdowns)
      max_shutdowns -= انگ_conns;
    وإلا إذا(max_shutdowns)
      max_shutdowns = 1;
    وإلا /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(انگ_conns / 4, 128);
    Curl_cshutdn_جوڙيو(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

فراغ Curl_cpool_init(بنية cpool *cpool,
                     بنية Curl_share *share,
                     size_t سائيز)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, سائيز, CURL_HASH_TYPE_بائيٽون,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->اندريون_shutdown = FALSE;
  cpool->locked = FALSE;
}

فراغ Curl_cpool_destroy(بنية cpool *cpool, بنية Curl_easy *admin)
{
  إذا(cpool && cpool->initialized && admin) {
    فراغ *داخلا_2;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_ڳڻپ(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_تالو(cpool, admin);
    إذا(Curl_uint32_tbl_پهريون(&cpool->conns, &cpid, &داخلا_2)) {
      بنية Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      افعل {
        بنية connectdata *conn = داخلا_2;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } طالما(Curl_uint32_tbl_ايندڙ(&cpool->conns, cpid, &cpid, &داخلا_2));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ساكن بنية cpool *cpool_get_instance(بنية Curl_easy *معلومات)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(معلومات->mid);
  إذا(CURL_SHARE_KEEP_CONNECT(معلومات->share))
    أرجع &معلومات->share->cpool;
  وإلا إذا(معلومات->multi_easy)
    أرجع &معلومات->multi_easy->cpool;
  وإلا إذا(معلومات->multi)
    أرجع &معلومات->multi->cpool;
  أرجع عدم;
}

بنية cpool *Curl_cpool_get_instance(بنية Curl_easy *معلومات)
{
  أرجع cpool_get_instance(معلومات);
}

فراغ Curl_cpool_xfer_init(بنية Curl_easy *معلومات)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);

  إذا(cpool) {
    CPOOL_تالو(cpool, معلومات);
    /* the identifier inside the connection cache */
    معلومات->id = cpool->ايندڙ_easy_id++;
    إذا(cpool->ايندڙ_easy_id == CURL_OFF_T_MAX)
      cpool->ايندڙ_easy_id = 0;
    معلومات->state.آخري_conn_id = -1;
    معلومات->state.آخري_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, معلومات);
  }
  وإلا {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    معلومات->id = 0;
    معلومات->state.آخري_conn_id = -1;
    معلومات->state.آخري_cpid = UINT32_MAX;
  }
}

ساكن bool cpool_conn_can_be_closed(بنية connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  أرجع (conn && !CONN_INUSE(conn) && !conn->بٽ.connect_only);
}

ساكن بنية connectdata *cpool_bundle_get_oldest_idle(
  بنية cpool *cpool,
  بنية cpool_bundle *bundle,
  ثابت بنية curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t اڻ_استعمال_ٿيل_ms;
  timediff_t oldest_ms = -1;

  إذا(Curl_uint32_spbset_پهريون(&bundle->ids, &cpid)) {
    افعل {
      بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      إذا(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        اڻ_استعمال_ٿيل_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        إذا(اڻ_استعمال_ٿيل_ms > oldest_ms) {
          oldest_ms = اڻ_استعمال_ٿيل_ms;
          oldest_id = cpid;
        }
      }
      وإلا إذا(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } طالما(Curl_uint32_spbset_ايندڙ(&bundle->ids, cpid, &cpid));
  }
  أرجع Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ساكن بنية connectdata *cpool_get_oldest_idle(بنية cpool *cpool,
                                                 ثابت بنية curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  إذا(Curl_uint32_bset_پهريون(&cpool->idles, &cpid)) {
    افعل {
      بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      إذا(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        إذا((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      وإلا {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } طالما(Curl_uint32_bset_ايندڙ(&cpool->idles, cpid, &cpid));
  }
  أرجع Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ساكن فراغ cpool_conn_close(بنية cpool *cpool,
                             بنية Curl_easy *معلومات,
                             بنية connectdata *conn,
                             bool aborted)
{
  بنية Curl_easy *admin;
  bool do_تالو;

  DEBUGASSERT(cpool);
  DEBUGASSERT(معلومات && !معلومات->conn);
  إذا(!cpool)
    أرجع;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  إذا(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(معلومات, "conn terminate when inuse: %u", conn->attached_xfers));
    أرجع;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(معلومات);
  do_تالو = !CPOOL_IS_LOCKED(cpool);
  إذا(do_تالو)
    CPOOL_تالو(cpool, admin);

  إذا(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  إذا(conn->بٽ.connect_only)
    aborted = TRUE;

  إذا(معلومات->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(معلومات, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&معلومات->multi->cpool, معلومات, conn, aborted);
  }
  وإلا {
    /* No multi available, terminate */
    infof(معلومات, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  إذا(do_تالو)
    CPOOL_UNLOCK(cpool, admin);
}

فراغ Curl_conn_close(بنية Curl_easy *معلومات,
                     بنية connectdata *conn,
                     bool aborted)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  cpool_conn_close(cpool, معلومات, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ساكن فراغ cpool_evict_conn(بنية cpool *cpool,
                             بنية Curl_easy *admin,
                             بنية connectdata *conn)
{
  إذا(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  وإلا
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#وصف_ڪريو CPOOL_حد_بندي_OK     0
#وصف_ڪريو CPOOL_حد_بندي_DEST   1
#وصف_ڪريو CPOOL_حد_بندي_ڪل  2

ساكن صحيح cpool_check_limits(بنية Curl_easy *معلومات,
                              uint32_t max_ڪل,
                              بنية connectdata *منزل_جوڙيو,
                              uint32_t max_host,
                              بنية cpool_bundle *bundle,
                              ثابت بنية curltime *pnow)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  بنية cshutdn *cshutdn = معلومات->multi ? &معلومات->multi->cshutdn : عدم;
  بنية Curl_easy *admin = عدم;

  إذا(!cpool)
    أرجع CPOOL_حد_بندي_OK;

  إذا(!max_ڪل && !max_host)
    أرجع CPOOL_حد_بندي_OK;

  إذا(max_host) {
    size_t جيئرو = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(معلومات);
    إذا(bundle) {
      جيئرو = Curl_uint32_spbset_ڳڻپ(&bundle->ids);
      إذا(جيئرو >= max_host) {
        size_t over = جيئرو - max_host + 1;
        لكل(; over; --over) {
          بنية connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          إذا(!oldest_idle)
            اكسر;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        جيئرو = Curl_uint32_spbset_ڳڻپ(&bundle->ids);
      }
    }
    إذا(cshutdn) {
      shutdowns = Curl_cshutdn_dest_ڳڻپ(cshutdn, منزل_جوڙيو->destination);
      إذا(shutdowns && ((جيئرو + shutdowns) >= max_host)) {
        size_t over = جيئرو + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               منزل_جوڙيو->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    إذا((جيئرو + shutdowns) >= max_host)
      أرجع CPOOL_حد_بندي_DEST;
  }

  /* Internal transfers are outside the total limit */
  إذا(max_ڪل && !معلومات->state.اندروني) {
    size_t shutdowns = 0;
    uint32_t انگ_conns = Curl_uint32_tbl_ڳڻپ(&cpool->conns);

    إذا(!admin)
      admin = Curl_get_admin(معلومات);
    إذا(انگ_conns >= max_ڪل) {
      size_t over = انگ_conns - max_ڪل + 1;
      لكل(; over; --over) {
        بنية connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        إذا(!oldest_idle)
          اكسر;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_ڪل);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    انگ_conns = Curl_uint32_tbl_ڳڻپ(&cpool->conns);
    إذا(cshutdn) {
      shutdowns = Curl_cshutdn_ڳڻپ(cshutdn);
      إذا(shutdowns && (انگ_conns + shutdowns) >= max_ڪل) {
        size_t over = انگ_conns + shutdowns - max_ڪل + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, عدم, over);
      }
    }
    إذا((انگ_conns + shutdowns) >= max_ڪل)
      أرجع CPOOL_حد_بندي_ڪل;
  }

  أرجع CPOOL_حد_بندي_OK;
}

CURLcode Curl_cpool_جوڙيو(بنية Curl_easy *معلومات,
                        بنية connectdata *conn,
                        uint32_t max_ڪل,
                        uint32_t max_host,
                        ثابت بنية curltime *pnow)
{
  CURLcode نتيجو = CURLE_OK;
  بنية cpool_bundle *bundle = عدم;
  بنية cpool *cpool = cpool_get_instance(معلومات);
  uint32_t صلاحيت, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  إذا(!cpool)
    أرجع CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.شروعات_ms[FIRSTSOCKET] =
    conn->shutdown.شروعات_ms[SECONDARYSOCKET] = -1;

  CPOOL_تالو(cpool, معلومات);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  بدل(cpool_check_limits(معلومات, max_ڪل, conn, max_host, bundle, pnow)) {
  حالة CPOOL_حد_بندي_DEST:
    infof(معلومات, "No more connections allowed to host");
    نتيجو = CURLE_NO_CONNECTION_موجود;
    اذهب ٻاهريون;
  حالة CPOOL_حد_بندي_ڪل:
    infof(معلومات, "No connections available, total of %u reached.",
          معلومات->multi->max_ڪل_connections);
    نتيجو = CURLE_NO_CONNECTION_موجود;
    اذهب ٻاهريون;
  افتراضي:
    اكسر;
  }

  /* We add the connection */
  صلاحيت = Curl_uint32_tbl_صلاحيت(&cpool->conns);
  إذا(صلاحيت == UINT32_MAX) {
    infof(معلومات, "No connections available, pool at max capacity");
    نتيجو = CURLE_NO_CONNECTION_موجود;
    اذهب ٻاهريون;
  }
  إذا(Curl_uint32_tbl_ڳڻپ(&cpool->conns) >= صلاحيت) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(صلاحيت, 16);
    إذا((UINT32_MAX - growth) >= صلاحيت)
      ncapacity = صلاحيت + growth;
    وإلا
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    إذا(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

  إذا(!bundle) {
    bundle = cpool_create_bundle();
    إذا(!bundle) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

  إذا(!Curl_uint32_tbl_جوڙيو(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  conn->cpid = cpid;

  إذا(!cpool_bundle_جوڙيو_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  conn->connection_id = cpool->ايندڙ_connection_id++;
  CURL_TRC_M(معلومات, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_ڳڻپ(&cpool->conns));
ٻاهريون:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, معلومات);

  أرجع نتيجو;
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
ساكن bool cpool_foreach(بنية Curl_easy *معلومات,
                          بنية cpool *cpool,
                          فراغ *param,
                          صحيح (*func)(بنية cpool *cpool,
                                      بنية Curl_easy *معلومات,
                                      بنية connectdata *conn, فراغ *param))
{
  فراغ *داخلا_2;
  uint32_t cpid;

  إذا(!cpool)
    أرجع FALSE;

  إذا(Curl_uint32_tbl_پهريون(&cpool->conns, &cpid, &داخلا_2)) {
    افعل {
      بنية connectdata *conn = داخلا_2;
      إذا(conn && func(cpool, معلومات, conn, param) == 1)
        أرجع TRUE;
    } طالما(Curl_uint32_tbl_ايندڙ(&cpool->conns, cpid, &cpid, &داخلا_2));
  }
  أرجع FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ساكن bool cpool_conn_now_idle(بنية cpool *cpool,
                                بنية Curl_easy *معلومات,
                                بنية connectdata *conn,
                                ثابت بنية curltime *pnow)
{
  بنية connectdata *oldest_idle = عدم;
  بنية Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  إذا(!معلومات || !معلومات->multi)
    أرجع kept;

  Curl_uint32_bset_جوڙيو(&cpool->idles, conn->cpid);

  إذا(!معلومات->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(معلومات->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  وإلا {
    maxconnects = معلومات->multi->maxconnects;
  }

  إذا(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t انگ_conns = Curl_uint32_tbl_ڳڻپ(&cpool->conns);
    admin = Curl_get_admin(معلومات);
    إذا(انگ_conns > maxconnects) {
      infof(معلومات, "Connection pool is full, closing the oldest of %u/%u",
            انگ_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      إذا(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  أرجع kept;
}

ساكن فراغ cpool_prune_dead(بنية cpool *cpool,
                             بنية Curl_easy *معلومات,
                             ثابت بنية curltime *pnow);

ساكن bool cpool_ڳوليو_act(بنية cpool *cpool,
                           بنية Curl_easy *معلومات,
                           بنية connectdata *conn,
                           cpool_ميلاپ_نتيجو ميلاپ)
{
  بدل(ميلاپ) {
  حالة CPOOL_ميلاپ_مليو:
    أرجع TRUE;
  حالة CPOOL_ميلاپ_TOO_OLD:
    إذا(!cpool_conn_can_be_closed(conn))
      أرجع FALSE;
    FALLTHROUGH();
  حالة CPOOL_ميلاپ_CLOSE:
    cpool_conn_close(cpool, معلومات, conn, FALSE);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع FALSE;
}

bool Curl_cpool_ڳوليو(بنية Curl_easy *معلومات,
                     ثابت حرف *destination,
                     bool prune_dead,
                     ثابت بنية curltime *pnow,
                     Curl_cpool_conn_ميلاپ_cb *conn_cb,
                     Curl_cpool_پورو_ميلاپ_cb *پورو_cb,
                     فراغ *userdata)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  بنية cpool_bundle *bundle = عدم;
  bool مليو = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  إذا(!cpool)
    أرجع FALSE;

  CPOOL_تالو(cpool, معلومات);

  إذا(prune_dead)
    cpool_prune_dead(cpool, معلومات, pnow);

  إذا(معلومات->state.آخري_cpid != UINT32_MAX) {
    بنية connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, معلومات->state.آخري_cpid);
    إذا(conn && (معلومات->state.آخري_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      مليو = cpool_ڳوليو_act(cpool, معلومات, conn, conn_cb(conn, userdata));
      إذا(مليو)
        اذهب ٻاهريون;
    }
    وإلا {
      /* No longer found here. */
      معلومات->state.آخري_conn_id = -1;
      معلومات->state.آخري_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  إذا(bundle) {
    uint32_t cpid;
    إذا(Curl_uint32_spbset_پهريون(&bundle->ids, &cpid)) {
      افعل {
        بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        إذا(conn) {
          إذا(معلومات->state.آخري_conn_id != conn->connection_id) {
            مليو = cpool_ڳوليو_act(cpool, معلومات, conn, conn_cb(conn, userdata));
            إذا(مليو)
              اذهب ٻاهريون;
          }
        }
        وإلا {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } طالما(Curl_uint32_spbset_ايندڙ(&bundle->ids, cpid, &cpid));
    }
  }

ٻاهريون:
  إذا(پورو_cb) {
    مليو = پورو_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, معلومات);
  أرجع مليو;
}

بنية cpool_reaper_ctx {
  size_t reaped;
  ثابت بنية curltime *pnow;
};

ساكن صحيح cpool_reap_dead_cb(بنية cpool *cpool,
                              بنية Curl_easy *admin,
                              بنية connectdata *conn, فراغ *param)
{
  بنية cpool_reaper_ctx *reaper = param;

  إذا(cpool_conn_can_be_closed(conn)) {
    إذا(conn->بٽ.no_reuse || conn->بٽ.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  أرجع 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ساكن فراغ cpool_prune_dead(بنية cpool *cpool,
                             بنية Curl_easy *معلومات,
                             ثابت بنية curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->آخري_cleanup);

  إذا(elapsed_ms >= 1000L) {
    بنية Curl_easy *admin = Curl_get_admin(معلومات);
    بنية cpool_reaper_ctx reaper;

    memset(&reaper, 0, حجم(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->آخري_cleanup = *pnow;
  }
}

ساكن صحيح conn_upkeep(بنية cpool *cpool,
                       بنية Curl_easy *admin,
                       بنية connectdata *conn,
                       فراغ *param)
{
  ثابت بنية curltime *pnow = Curl_pgrs_now(admin);

  (فراغ)param;
  إذا((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->سيٽ.upkeep_interval_ms) {
    CURLcode نتيجو;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    نتيجو = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    إذا(نتيجو && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  أرجع 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(بنية Curl_easy *معلومات)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  بنية Curl_easy *admin = Curl_get_admin(معلومات);

  إذا(!cpool)
    أرجع CURLE_OK;

  CPOOL_تالو(cpool, admin);
  cpool_foreach(admin, cpool, عدم, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  أرجع CURLE_OK;
}

بنية connectdata *Curl_cpool_get_آخري_conn(بنية Curl_easy *معلومات)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  إذا(cpool && (معلومات->state.آخري_cpid != UINT32_MAX)) {
    بنية connectdata *conn;

    CPOOL_تالو(cpool, معلومات);
    conn = Curl_uint32_tbl_get(&cpool->conns, معلومات->state.آخري_cpid);
    CPOOL_UNLOCK(cpool, معلومات);
    إذا(conn && (معلومات->state.آخري_conn_id == conn->connection_id))
      أرجع conn;
    معلومات->state.آخري_cpid = UINT32_MAX;
    معلومات->state.آخري_conn_id = -1;
  }
  أرجع عدم;
}

فراغ Curl_cpool_return(بنية Curl_easy *معلومات,
                       بنية connectdata *conn,
                       Curl_cpool_return_cb *cb, فراغ *cbdata,
                       ثابت بنية curltime *pnow)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);

  CPOOL_تالو(cpool, معلومات);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  بدل(cb(معلومات, conn, cbdata, pnow)) {
  حالة CPOOL_DO_KEEP:
    اكسر;
  حالة CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    إذا(cpool_conn_now_idle(cpool, معلومات, conn, pnow)) {
      /* connection kept in the cpool */
      infof(معلومات, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->پورٽ);
    }
    وإلا { /* connection was removed from the cpool and destroyed. */
      معلومات->state.آخري_conn_id = -1;
      معلومات->state.آخري_cpid = UINT32_MAX;
    }
    اكسر;
  حالة CPOOL_DO_CLOSE:
    Curl_conn_close(معلومات, conn, FALSE);
    اكسر;
  حالة CPOOL_DO_TERMINATE:
    Curl_conn_close(معلومات, conn, TRUE);
    اكسر;
  }

  CPOOL_UNLOCK(cpool, معلومات);
}

ساكن صحيح cpool_reap_no_reuse(بنية cpool *cpool,
                               بنية Curl_easy *admin,
                               بنية connectdata *conn, فراغ *param)
{
  (فراغ)param;
  conn->بٽ.no_reuse = TRUE;
  إذا(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  أرجع 0; /* continue iteration */
}

فراغ Curl_cpool_nw_بدليو(بنية cpool *cpool, بنية Curl_easy *admin)
{
  إذا(cpool && admin) {
    CPOOL_تالو(cpool, admin);
    cpool_foreach(admin, cpool, عدم, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ساكن bool cpool_conn_too_old(بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               ثابت بنية curltime *pnow)
{
  timediff_t age_ms;

  إذا(معلومات->سيٽ.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    إذا(age_ms > معلومات->سيٽ.conn_max_idle_ms) {
      infof(معلومات, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, معلومات->سيٽ.conn_max_idle_ms);
      أرجع TRUE;
    }
  }

  إذا(معلومات->سيٽ.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    إذا(age_ms > معلومات->سيٽ.conn_max_age_ms) {
      infof(معلومات,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, معلومات->سيٽ.conn_max_age_ms);
      أرجع TRUE;
    }
  }

  أرجع FALSE;
}

bool Curl_cpool_conn_seems_healthy(بنية connectdata *conn,
                                   بنية Curl_easy *معلومات,
                                   ثابت بنية curltime *pnow)
{
  بنية Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_داخلا;

  DEBUGASSERT(!معلومات->conn);
  إذا(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(معلومات, conn, pnow))
    أرجع FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_داخلا = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->لاڳيتو->connection_is_dead;
  إذا(!poll_داخلا &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    أرجع TRUE;

  admin = Curl_get_admin(معلومات);
  إذا(conn->scheme->لاڳيتو->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->لاڳيتو->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  وإلا {
    bool داخلا_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &داخلا_pending);
    Curl_detach_connection(admin);
    إذا(healthy && داخلا_pending && poll_داخلا) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(معلومات, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  إذا(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  أرجع healthy;
}

فراغ Curl_cpool_conn_was_استعمال_ٿيل(بنية Curl_easy *معلومات,
                              بنية connectdata *conn,
                              ثابت بنية curltime *pnow)
{
  (فراغ)معلومات;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  ثابت بنية curltime *pnow)
{
  (فراغ)معلومات;
  أرجع curlx_ptimediff_ms(pnow, &conn->created);
}
