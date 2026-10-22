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


#تعریف_کرو CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#تعریف_کرو CPOOL_جندرا(c, d)                                                \
  افعل {                                                                  \
    إذا(c) {                                                             \
      إذا(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_جندرا_share((c)->share, (d), CURL_جندرا_انکڑے_CONNECT,  \
                        CURL_جندرا_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } طالما(0)

#تعریف_کرو CPOOL_UNLOCK(c, d)                                              \
  افعل {                                                                  \
    إذا(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      إذا(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_جندرا_انکڑے_CONNECT); \
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
ساكن bool cpool_bundle_جوڑو_id(بنية cpool *cpool,
                                بنية cpool_bundle *bundle,
                                ثابت حرف *destination,
                                uint32_t pool_id)
{
  إذا(!Curl_uint32_spbset_جوڑو(&bundle->ids, pool_id))
    أرجع FALSE;
  إذا(Curl_uint32_spbset_گنتی(&bundle->ids) == 1) {
    /* first one added, register bundle */
    بنية cpool_bundle *new_ref = عدم;
    cpool_bundle_link(&new_ref, bundle);
    إذا(!Curl_hash_جوڑو(&cpool->dest2bundle,
                      destination, لڑی_لمبائی(destination), new_ref)) {
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
  إذا(!Curl_uint32_spbset_گنتی(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, لڑی_لمبائی(destination));
  }
}

ساكن فراغ cpool_bundle_hash_dtor(فراغ *اندراج)
{
  بنية cpool_bundle *bundle = اندراج;
  cpool_bundle_unlink(&bundle);
}

ساكن بنية cpool_bundle *cpool_get_bundle(بنية cpool *cpool,
                                             ثابت حرف *destination)
{
  بنية cpool_bundle *bundle = عدم;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, لڑی_لمبائی(destination)));
  أرجع bundle;
}

ساكن فراغ cpool_remove_conn(بنية cpool *cpool,
                              بنية connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  إذا(cpid != UINT32_MAX) {
    بنية cpool_bundle *bundle;

#جے_تعریف_شدہ DEBUGBUILD
    {
      بنية connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#جے_انت
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
                               بنية Curl_easy *انکڑے,
                               بنية connectdata *conn,
                               bool aborted)
{
  بنية Curl_easy *admin;
  bool پورا = FALSE;

  DEBUGASSERT(انکڑے);
  DEBUGASSERT(!انکڑے->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(انکڑے);
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
  إذا(conn->بٹ.connect_only)
    aborted = TRUE;
  conn->بٹ.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  إذا(aborted)
    پورا = TRUE;
  إذا(!پورا) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &پورا);
  }

  إذا(پورا || !انکڑے->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  وإلا {
    بنية Curl_multi *multi = انکڑے->multi;
    size_t max_shutdowns = multi->max_کل_connections;
    uint32_t عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);

    إذا(عدد_conns < max_shutdowns)
      max_shutdowns -= عدد_conns;
    وإلا إذا(max_shutdowns)
      max_shutdowns = 1;
    وإلا /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(عدد_conns / 4, 128);
    Curl_cshutdn_جوڑو(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

فراغ Curl_cpool_init(بنية cpool *cpool,
                     بنية Curl_share *share,
                     size_t ناپ)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, ناپ, CURL_HASH_TYPE_بائٹاں,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->وچ_shutdown = FALSE;
  cpool->locked = FALSE;
}

فراغ Curl_cpool_destroy(بنية cpool *cpool, بنية Curl_easy *admin)
{
  إذا(cpool && cpool->initialized && admin) {
    فراغ *اندراج;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_گنتی(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_جندرا(cpool, admin);
    إذا(Curl_uint32_tbl_پہلا(&cpool->conns, &cpid, &اندراج)) {
      بنية Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      افعل {
        بنية connectdata *conn = اندراج;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } طالما(Curl_uint32_tbl_اگلا(&cpool->conns, cpid, &cpid, &اندراج));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ساكن بنية cpool *cpool_get_instance(بنية Curl_easy *انکڑے)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(انکڑے->mid);
  إذا(CURL_SHARE_KEEP_CONNECT(انکڑے->share))
    أرجع &انکڑے->share->cpool;
  وإلا إذا(انکڑے->multi_easy)
    أرجع &انکڑے->multi_easy->cpool;
  وإلا إذا(انکڑے->multi)
    أرجع &انکڑے->multi->cpool;
  أرجع عدم;
}

بنية cpool *Curl_cpool_get_instance(بنية Curl_easy *انکڑے)
{
  أرجع cpool_get_instance(انکڑے);
}

فراغ Curl_cpool_xfer_init(بنية Curl_easy *انکڑے)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);

  إذا(cpool) {
    CPOOL_جندرا(cpool, انکڑے);
    /* the identifier inside the connection cache */
    انکڑے->id = cpool->اگلا_easy_id++;
    إذا(cpool->اگلا_easy_id == CURL_OFF_T_MAX)
      cpool->اگلا_easy_id = 0;
    انکڑے->state.آخری_conn_id = -1;
    انکڑے->state.آخری_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, انکڑے);
  }
  وإلا {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    انکڑے->id = 0;
    انکڑے->state.آخری_conn_id = -1;
    انکڑے->state.آخری_cpid = UINT32_MAX;
  }
}

ساكن bool cpool_conn_can_be_closed(بنية connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  أرجع (conn && !CONN_INUSE(conn) && !conn->بٹ.connect_only);
}

ساكن بنية connectdata *cpool_bundle_get_oldest_idle(
  بنية cpool *cpool,
  بنية cpool_bundle *bundle,
  ثابت بنية curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t بن_ورتیا_ms;
  timediff_t oldest_ms = -1;

  إذا(Curl_uint32_spbset_پہلا(&bundle->ids, &cpid)) {
    افعل {
      بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      إذا(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        بن_ورتیا_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        إذا(بن_ورتیا_ms > oldest_ms) {
          oldest_ms = بن_ورتیا_ms;
          oldest_id = cpid;
        }
      }
      وإلا إذا(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } طالما(Curl_uint32_spbset_اگلا(&bundle->ids, cpid, &cpid));
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
  إذا(Curl_uint32_bset_پہلا(&cpool->idles, &cpid)) {
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
    } طالما(Curl_uint32_bset_اگلا(&cpool->idles, cpid, &cpid));
  }
  أرجع Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ساكن فراغ cpool_conn_close(بنية cpool *cpool,
                             بنية Curl_easy *انکڑے,
                             بنية connectdata *conn,
                             bool aborted)
{
  بنية Curl_easy *admin;
  bool do_جندرا;

  DEBUGASSERT(cpool);
  DEBUGASSERT(انکڑے && !انکڑے->conn);
  إذا(!cpool)
    أرجع;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  إذا(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(انکڑے, "conn terminate when inuse: %u", conn->attached_xfers));
    أرجع;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(انکڑے);
  do_جندرا = !CPOOL_IS_LOCKED(cpool);
  إذا(do_جندرا)
    CPOOL_جندرا(cpool, admin);

  إذا(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  إذا(conn->بٹ.connect_only)
    aborted = TRUE;

  إذا(انکڑے->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(انکڑے, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&انکڑے->multi->cpool, انکڑے, conn, aborted);
  }
  وإلا {
    /* No multi available, terminate */
    infof(انکڑے, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  إذا(do_جندرا)
    CPOOL_UNLOCK(cpool, admin);
}

فراغ Curl_conn_close(بنية Curl_easy *انکڑے,
                     بنية connectdata *conn,
                     bool aborted)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);
  cpool_conn_close(cpool, انکڑے, conn, aborted);
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

#تعریف_کرو CPOOL_حد_بندی_OK     0
#تعریف_کرو CPOOL_حد_بندی_DEST   1
#تعریف_کرو CPOOL_حد_بندی_کل  2

ساكن صحيح cpool_check_limits(بنية Curl_easy *انکڑے,
                              uint32_t max_کل,
                              بنية connectdata *منزل_جوڑو,
                              uint32_t max_host,
                              بنية cpool_bundle *bundle,
                              ثابت بنية curltime *pnow)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);
  بنية cshutdn *cshutdn = انکڑے->multi ? &انکڑے->multi->cshutdn : عدم;
  بنية Curl_easy *admin = عدم;

  إذا(!cpool)
    أرجع CPOOL_حد_بندی_OK;

  إذا(!max_کل && !max_host)
    أرجع CPOOL_حد_بندی_OK;

  إذا(max_host) {
    size_t جیوندا = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(انکڑے);
    إذا(bundle) {
      جیوندا = Curl_uint32_spbset_گنتی(&bundle->ids);
      إذا(جیوندا >= max_host) {
        size_t over = جیوندا - max_host + 1;
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
        جیوندا = Curl_uint32_spbset_گنتی(&bundle->ids);
      }
    }
    إذا(cshutdn) {
      shutdowns = Curl_cshutdn_dest_گنتی(cshutdn, منزل_جوڑو->destination);
      إذا(shutdowns && ((جیوندا + shutdowns) >= max_host)) {
        size_t over = جیوندا + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               منزل_جوڑو->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    إذا((جیوندا + shutdowns) >= max_host)
      أرجع CPOOL_حد_بندی_DEST;
  }

  /* Internal transfers are outside the total limit */
  إذا(max_کل && !انکڑے->state.اندرونی) {
    size_t shutdowns = 0;
    uint32_t عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);

    إذا(!admin)
      admin = Curl_get_admin(انکڑے);
    إذا(عدد_conns >= max_کل) {
      size_t over = عدد_conns - max_کل + 1;
      لكل(; over; --over) {
        بنية connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        إذا(!oldest_idle)
          اكسر;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_کل);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);
    إذا(cshutdn) {
      shutdowns = Curl_cshutdn_گنتی(cshutdn);
      إذا(shutdowns && (عدد_conns + shutdowns) >= max_کل) {
        size_t over = عدد_conns + shutdowns - max_کل + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, عدم, over);
      }
    }
    إذا((عدد_conns + shutdowns) >= max_کل)
      أرجع CPOOL_حد_بندی_کل;
  }

  أرجع CPOOL_حد_بندی_OK;
}

CURLcode Curl_cpool_جوڑو(بنية Curl_easy *انکڑے,
                        بنية connectdata *conn,
                        uint32_t max_کل,
                        uint32_t max_host,
                        ثابت بنية curltime *pnow)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية cpool_bundle *bundle = عدم;
  بنية cpool *cpool = cpool_get_instance(انکڑے);
  uint32_t سمرتھا, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  إذا(!cpool)
    أرجع CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.شروع_ms[FIRSTSOCKET] =
    conn->shutdown.شروع_ms[SECONDARYSOCKET] = -1;

  CPOOL_جندرا(cpool, انکڑے);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  بدل(cpool_check_limits(انکڑے, max_کل, conn, max_host, bundle, pnow)) {
  حالة CPOOL_حد_بندی_DEST:
    infof(انکڑے, "No more connections allowed to host");
    نتیجہ = CURLE_NO_CONNECTION_دستیاب;
    اذهب باہرلا;
  حالة CPOOL_حد_بندی_کل:
    infof(انکڑے, "No connections available, total of %u reached.",
          انکڑے->multi->max_کل_connections);
    نتیجہ = CURLE_NO_CONNECTION_دستیاب;
    اذهب باہرلا;
  افتراضي:
    اكسر;
  }

  /* We add the connection */
  سمرتھا = Curl_uint32_tbl_سمرتھا(&cpool->conns);
  إذا(سمرتھا == UINT32_MAX) {
    infof(انکڑے, "No connections available, pool at max capacity");
    نتیجہ = CURLE_NO_CONNECTION_دستیاب;
    اذهب باہرلا;
  }
  إذا(Curl_uint32_tbl_گنتی(&cpool->conns) >= سمرتھا) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(سمرتھا, 16);
    إذا((UINT32_MAX - growth) >= سمرتھا)
      ncapacity = سمرتھا + growth;
    وإلا
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    إذا(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  إذا(!bundle) {
    bundle = cpool_create_bundle();
    إذا(!bundle) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  إذا(!Curl_uint32_tbl_جوڑو(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  conn->cpid = cpid;

  إذا(!cpool_bundle_جوڑو_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  conn->connection_id = cpool->اگلا_connection_id++;
  CURL_TRC_M(انکڑے, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_گنتی(&cpool->conns));
باہرلا:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, انکڑے);

  أرجع نتیجہ;
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
ساكن bool cpool_foreach(بنية Curl_easy *انکڑے,
                          بنية cpool *cpool,
                          فراغ *param,
                          صحيح (*func)(بنية cpool *cpool,
                                      بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn, فراغ *param))
{
  فراغ *اندراج;
  uint32_t cpid;

  إذا(!cpool)
    أرجع FALSE;

  إذا(Curl_uint32_tbl_پہلا(&cpool->conns, &cpid, &اندراج)) {
    افعل {
      بنية connectdata *conn = اندراج;
      إذا(conn && func(cpool, انکڑے, conn, param) == 1)
        أرجع TRUE;
    } طالما(Curl_uint32_tbl_اگلا(&cpool->conns, cpid, &cpid, &اندراج));
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
                                بنية Curl_easy *انکڑے,
                                بنية connectdata *conn,
                                ثابت بنية curltime *pnow)
{
  بنية connectdata *oldest_idle = عدم;
  بنية Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  إذا(!انکڑے || !انکڑے->multi)
    أرجع kept;

  Curl_uint32_bset_جوڑو(&cpool->idles, conn->cpid);

  إذا(!انکڑے->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(انکڑے->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  وإلا {
    maxconnects = انکڑے->multi->maxconnects;
  }

  إذا(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);
    admin = Curl_get_admin(انکڑے);
    إذا(عدد_conns > maxconnects) {
      infof(انکڑے, "Connection pool is full, closing the oldest of %u/%u",
            عدد_conns, maxconnects);
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
                             بنية Curl_easy *انکڑے,
                             ثابت بنية curltime *pnow);

ساكن bool cpool_لبھو_act(بنية cpool *cpool,
                           بنية Curl_easy *انکڑے,
                           بنية connectdata *conn,
                           cpool_میل_نتیجہ میل)
{
  بدل(میل) {
  حالة CPOOL_میل_لبھیا:
    أرجع TRUE;
  حالة CPOOL_میل_TOO_OLD:
    إذا(!cpool_conn_can_be_closed(conn))
      أرجع FALSE;
    FALLTHROUGH();
  حالة CPOOL_میل_CLOSE:
    cpool_conn_close(cpool, انکڑے, conn, FALSE);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع FALSE;
}

bool Curl_cpool_لبھو(بنية Curl_easy *انکڑے,
                     ثابت حرف *destination,
                     bool prune_dead,
                     ثابت بنية curltime *pnow,
                     Curl_cpool_conn_میل_cb *conn_cb,
                     Curl_cpool_پورا_میل_cb *پورا_cb,
                     فراغ *userdata)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);
  بنية cpool_bundle *bundle = عدم;
  bool لبھیا = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  إذا(!cpool)
    أرجع FALSE;

  CPOOL_جندرا(cpool, انکڑے);

  إذا(prune_dead)
    cpool_prune_dead(cpool, انکڑے, pnow);

  إذا(انکڑے->state.آخری_cpid != UINT32_MAX) {
    بنية connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, انکڑے->state.آخری_cpid);
    إذا(conn && (انکڑے->state.آخری_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      لبھیا = cpool_لبھو_act(cpool, انکڑے, conn, conn_cb(conn, userdata));
      إذا(لبھیا)
        اذهب باہرلا;
    }
    وإلا {
      /* No longer found here. */
      انکڑے->state.آخری_conn_id = -1;
      انکڑے->state.آخری_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  إذا(bundle) {
    uint32_t cpid;
    إذا(Curl_uint32_spbset_پہلا(&bundle->ids, &cpid)) {
      افعل {
        بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        إذا(conn) {
          إذا(انکڑے->state.آخری_conn_id != conn->connection_id) {
            لبھیا = cpool_لبھو_act(cpool, انکڑے, conn, conn_cb(conn, userdata));
            إذا(لبھیا)
              اذهب باہرلا;
          }
        }
        وإلا {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } طالما(Curl_uint32_spbset_اگلا(&bundle->ids, cpid, &cpid));
    }
  }

باہرلا:
  إذا(پورا_cb) {
    لبھیا = پورا_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, انکڑے);
  أرجع لبھیا;
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
    إذا(conn->بٹ.no_reuse || conn->بٹ.close ||
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
                             بنية Curl_easy *انکڑے,
                             ثابت بنية curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->آخری_cleanup);

  إذا(elapsed_ms >= 1000L) {
    بنية Curl_easy *admin = Curl_get_admin(انکڑے);
    بنية cpool_reaper_ctx reaper;

    memset(&reaper, 0, حجم(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->آخری_cleanup = *pnow;
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
     admin->سیٹ.upkeep_interval_ms) {
    CURLcode نتیجہ;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    نتیجہ = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    إذا(نتیجہ && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  أرجع 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(بنية Curl_easy *انکڑے)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);
  بنية Curl_easy *admin = Curl_get_admin(انکڑے);

  إذا(!cpool)
    أرجع CURLE_OK;

  CPOOL_جندرا(cpool, admin);
  cpool_foreach(admin, cpool, عدم, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  أرجع CURLE_OK;
}

بنية connectdata *Curl_cpool_get_آخری_conn(بنية Curl_easy *انکڑے)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);
  إذا(cpool && (انکڑے->state.آخری_cpid != UINT32_MAX)) {
    بنية connectdata *conn;

    CPOOL_جندرا(cpool, انکڑے);
    conn = Curl_uint32_tbl_get(&cpool->conns, انکڑے->state.آخری_cpid);
    CPOOL_UNLOCK(cpool, انکڑے);
    إذا(conn && (انکڑے->state.آخری_conn_id == conn->connection_id))
      أرجع conn;
    انکڑے->state.آخری_cpid = UINT32_MAX;
    انکڑے->state.آخری_conn_id = -1;
  }
  أرجع عدم;
}

فراغ Curl_cpool_return(بنية Curl_easy *انکڑے,
                       بنية connectdata *conn,
                       Curl_cpool_return_cb *cb, فراغ *cbdata,
                       ثابت بنية curltime *pnow)
{
  بنية cpool *cpool = cpool_get_instance(انکڑے);

  CPOOL_جندرا(cpool, انکڑے);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  بدل(cb(انکڑے, conn, cbdata, pnow)) {
  حالة CPOOL_DO_KEEP:
    اكسر;
  حالة CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    إذا(cpool_conn_now_idle(cpool, انکڑے, conn, pnow)) {
      /* connection kept in the cpool */
      infof(انکڑے, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->پورٹ);
    }
    وإلا { /* connection was removed from the cpool and destroyed. */
      انکڑے->state.آخری_conn_id = -1;
      انکڑے->state.آخری_cpid = UINT32_MAX;
    }
    اكسر;
  حالة CPOOL_DO_CLOSE:
    Curl_conn_close(انکڑے, conn, FALSE);
    اكسر;
  حالة CPOOL_DO_TERMINATE:
    Curl_conn_close(انکڑے, conn, TRUE);
    اكسر;
  }

  CPOOL_UNLOCK(cpool, انکڑے);
}

ساكن صحيح cpool_reap_no_reuse(بنية cpool *cpool,
                               بنية Curl_easy *admin,
                               بنية connectdata *conn, فراغ *param)
{
  (فراغ)param;
  conn->بٹ.no_reuse = TRUE;
  إذا(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  أرجع 0; /* continue iteration */
}

فراغ Curl_cpool_nw_بدلیا(بنية cpool *cpool, بنية Curl_easy *admin)
{
  إذا(cpool && admin) {
    CPOOL_جندرا(cpool, admin);
    cpool_foreach(admin, cpool, عدم, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ساكن bool cpool_conn_too_old(بنية Curl_easy *انکڑے,
                               بنية connectdata *conn,
                               ثابت بنية curltime *pnow)
{
  timediff_t age_ms;

  إذا(انکڑے->سیٹ.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    إذا(age_ms > انکڑے->سیٹ.conn_max_idle_ms) {
      infof(انکڑے, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, انکڑے->سیٹ.conn_max_idle_ms);
      أرجع TRUE;
    }
  }

  إذا(انکڑے->سیٹ.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    إذا(age_ms > انکڑے->سیٹ.conn_max_age_ms) {
      infof(انکڑے,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, انکڑے->سیٹ.conn_max_age_ms);
      أرجع TRUE;
    }
  }

  أرجع FALSE;
}

bool Curl_cpool_conn_seems_healthy(بنية connectdata *conn,
                                   بنية Curl_easy *انکڑے,
                                   ثابت بنية curltime *pnow)
{
  بنية Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_داخلہ;

  DEBUGASSERT(!انکڑے->conn);
  إذا(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(انکڑے, conn, pnow))
    أرجع FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_داخلہ = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->لگاتار->connection_is_dead;
  إذا(!poll_داخلہ &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    أرجع TRUE;

  admin = Curl_get_admin(انکڑے);
  إذا(conn->scheme->لگاتار->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->لگاتار->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  وإلا {
    bool داخلہ_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &داخلہ_pending);
    Curl_detach_connection(admin);
    إذا(healthy && داخلہ_pending && poll_داخلہ) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(انکڑے, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  إذا(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  أرجع healthy;
}

فراغ Curl_cpool_conn_was_ورتیا(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn,
                              ثابت بنية curltime *pnow)
{
  (فراغ)انکڑے;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn,
                                  ثابت بنية curltime *pnow)
{
  (فراغ)انکڑے;
  أرجع curlx_ptimediff_ms(pnow, &conn->created);
}
