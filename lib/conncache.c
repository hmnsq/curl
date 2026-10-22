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


#تعریف_کړه CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#تعریف_کړه CPOOL_قلف(c, d)                                                \
  افعل {                                                                  \
    إذا(c) {                                                             \
      إذا(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_قلف_share((c)->share, (d), CURL_قلف_معلومات_CONNECT,  \
                        CURL_قلف_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } طالما(0)

#تعریف_کړه CPOOL_UNLOCK(c, d)                                              \
  افعل {                                                                  \
    إذا(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      إذا(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_قلف_معلومات_CONNECT); \
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
ساكن bool cpool_bundle_جمع_کړه_id(بنية cpool *cpool,
                                بنية cpool_bundle *bundle,
                                ثابت حرف *destination,
                                uint32_t pool_id)
{
  إذا(!Curl_uint32_spbset_جمع_کړه(&bundle->ids, pool_id))
    أرجع FALSE;
  إذا(Curl_uint32_spbset_شمېر(&bundle->ids) == 1) {
    /* first one added, register bundle */
    بنية cpool_bundle *new_ref = عدم;
    cpool_bundle_link(&new_ref, bundle);
    إذا(!Curl_hash_جمع_کړه(&cpool->dest2bundle,
                      destination, مزي_اوږدوالی(destination), new_ref)) {
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
  إذا(!Curl_uint32_spbset_شمېر(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, مزي_اوږدوالی(destination));
  }
}

ساكن فراغ cpool_bundle_hash_dtor(فراغ *ثبت)
{
  بنية cpool_bundle *bundle = ثبت;
  cpool_bundle_unlink(&bundle);
}

ساكن بنية cpool_bundle *cpool_get_bundle(بنية cpool *cpool,
                                             ثابت حرف *destination)
{
  بنية cpool_bundle *bundle = عدم;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, مزي_اوږدوالی(destination)));
  أرجع bundle;
}

ساكن فراغ cpool_remove_conn(بنية cpool *cpool,
                              بنية connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  إذا(cpid != UINT32_MAX) {
    بنية cpool_bundle *bundle;

#که_تعریف_شوی DEBUGBUILD
    {
      بنية connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#د_که_پای
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
  bool بشپړ = FALSE;

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
  إذا(conn->بټونه.connect_only)
    aborted = TRUE;
  conn->بټونه.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  إذا(aborted)
    بشپړ = TRUE;
  إذا(!بشپړ) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &بشپړ);
  }

  إذا(بشپړ || !معلومات->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  وإلا {
    بنية Curl_multi *multi = معلومات->multi;
    size_t max_shutdowns = multi->max_ټول_connections;
    uint32_t عدد_conns = Curl_uint32_tbl_شمېر(&cpool->conns);

    إذا(عدد_conns < max_shutdowns)
      max_shutdowns -= عدد_conns;
    وإلا إذا(max_shutdowns)
      max_shutdowns = 1;
    وإلا /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(عدد_conns / 4, 128);
    Curl_cshutdn_جمع_کړه(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

فراغ Curl_cpool_init(بنية cpool *cpool,
                     بنية Curl_share *share,
                     size_t اندازه)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, اندازه, CURL_HASH_TYPE_بایټونه,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->دننه_shutdown = FALSE;
  cpool->locked = FALSE;
}

فراغ Curl_cpool_destroy(بنية cpool *cpool, بنية Curl_easy *admin)
{
  إذا(cpool && cpool->initialized && admin) {
    فراغ *ثبت;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_شمېر(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_قلف(cpool, admin);
    إذا(Curl_uint32_tbl_لومړی(&cpool->conns, &cpid, &ثبت)) {
      بنية Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      افعل {
        بنية connectdata *conn = ثبت;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } طالما(Curl_uint32_tbl_بل(&cpool->conns, cpid, &cpid, &ثبت));
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
    CPOOL_قلف(cpool, معلومات);
    /* the identifier inside the connection cache */
    معلومات->id = cpool->بل_easy_id++;
    إذا(cpool->بل_easy_id == CURL_OFF_T_MAX)
      cpool->بل_easy_id = 0;
    معلومات->state.وروستی_conn_id = -1;
    معلومات->state.وروستی_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, معلومات);
  }
  وإلا {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    معلومات->id = 0;
    معلومات->state.وروستی_conn_id = -1;
    معلومات->state.وروستی_cpid = UINT32_MAX;
  }
}

ساكن bool cpool_conn_can_be_closed(بنية connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  أرجع (conn && !CONN_INUSE(conn) && !conn->بټونه.connect_only);
}

ساكن بنية connectdata *cpool_bundle_get_oldest_idle(
  بنية cpool *cpool,
  بنية cpool_bundle *bundle,
  ثابت بنية curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t ناکارول_شوی_ms;
  timediff_t oldest_ms = -1;

  إذا(Curl_uint32_spbset_لومړی(&bundle->ids, &cpid)) {
    افعل {
      بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      إذا(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        ناکارول_شوی_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        إذا(ناکارول_شوی_ms > oldest_ms) {
          oldest_ms = ناکارول_شوی_ms;
          oldest_id = cpid;
        }
      }
      وإلا إذا(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } طالما(Curl_uint32_spbset_بل(&bundle->ids, cpid, &cpid));
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
  إذا(Curl_uint32_bset_لومړی(&cpool->idles, &cpid)) {
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
    } طالما(Curl_uint32_bset_بل(&cpool->idles, cpid, &cpid));
  }
  أرجع Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ساكن فراغ cpool_conn_close(بنية cpool *cpool,
                             بنية Curl_easy *معلومات,
                             بنية connectdata *conn,
                             bool aborted)
{
  بنية Curl_easy *admin;
  bool do_قلف;

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
  do_قلف = !CPOOL_IS_LOCKED(cpool);
  إذا(do_قلف)
    CPOOL_قلف(cpool, admin);

  إذا(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  إذا(conn->بټونه.connect_only)
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

  إذا(do_قلف)
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

#تعریف_کړه CPOOL_حد_OK     0
#تعریف_کړه CPOOL_حد_DEST   1
#تعریف_کړه CPOOL_حد_ټول  2

ساكن صحيح cpool_check_limits(بنية Curl_easy *معلومات,
                              uint32_t max_ټول,
                              بنية connectdata *منزل_جمع_کړه,
                              uint32_t max_host,
                              بنية cpool_bundle *bundle,
                              ثابت بنية curltime *pnow)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  بنية cshutdn *cshutdn = معلومات->multi ? &معلومات->multi->cshutdn : عدم;
  بنية Curl_easy *admin = عدم;

  إذا(!cpool)
    أرجع CPOOL_حد_OK;

  إذا(!max_ټول && !max_host)
    أرجع CPOOL_حد_OK;

  إذا(max_host) {
    size_t ژوندی = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(معلومات);
    إذا(bundle) {
      ژوندی = Curl_uint32_spbset_شمېر(&bundle->ids);
      إذا(ژوندی >= max_host) {
        size_t over = ژوندی - max_host + 1;
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
        ژوندی = Curl_uint32_spbset_شمېر(&bundle->ids);
      }
    }
    إذا(cshutdn) {
      shutdowns = Curl_cshutdn_dest_شمېر(cshutdn, منزل_جمع_کړه->destination);
      إذا(shutdowns && ((ژوندی + shutdowns) >= max_host)) {
        size_t over = ژوندی + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               منزل_جمع_کړه->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    إذا((ژوندی + shutdowns) >= max_host)
      أرجع CPOOL_حد_DEST;
  }

  /* Internal transfers are outside the total limit */
  إذا(max_ټول && !معلومات->state.دننی) {
    size_t shutdowns = 0;
    uint32_t عدد_conns = Curl_uint32_tbl_شمېر(&cpool->conns);

    إذا(!admin)
      admin = Curl_get_admin(معلومات);
    إذا(عدد_conns >= max_ټول) {
      size_t over = عدد_conns - max_ټول + 1;
      لكل(; over; --over) {
        بنية connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        إذا(!oldest_idle)
          اكسر;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_ټول);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    عدد_conns = Curl_uint32_tbl_شمېر(&cpool->conns);
    إذا(cshutdn) {
      shutdowns = Curl_cshutdn_شمېر(cshutdn);
      إذا(shutdowns && (عدد_conns + shutdowns) >= max_ټول) {
        size_t over = عدد_conns + shutdowns - max_ټول + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, عدم, over);
      }
    }
    إذا((عدد_conns + shutdowns) >= max_ټول)
      أرجع CPOOL_حد_ټول;
  }

  أرجع CPOOL_حد_OK;
}

CURLcode Curl_cpool_جمع_کړه(بنية Curl_easy *معلومات,
                        بنية connectdata *conn,
                        uint32_t max_ټول,
                        uint32_t max_host,
                        ثابت بنية curltime *pnow)
{
  CURLcode پایله = CURLE_OK;
  بنية cpool_bundle *bundle = عدم;
  بنية cpool *cpool = cpool_get_instance(معلومات);
  uint32_t ظرفیت, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  إذا(!cpool)
    أرجع CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.پیل_ms[FIRSTSOCKET] =
    conn->shutdown.پیل_ms[SECONDARYSOCKET] = -1;

  CPOOL_قلف(cpool, معلومات);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  بدل(cpool_check_limits(معلومات, max_ټول, conn, max_host, bundle, pnow)) {
  حالة CPOOL_حد_DEST:
    infof(معلومات, "No more connections allowed to host");
    پایله = CURLE_NO_CONNECTION_شته;
    اذهب بهر;
  حالة CPOOL_حد_ټول:
    infof(معلومات, "No connections available, total of %u reached.",
          معلومات->multi->max_ټول_connections);
    پایله = CURLE_NO_CONNECTION_شته;
    اذهب بهر;
  افتراضي:
    اكسر;
  }

  /* We add the connection */
  ظرفیت = Curl_uint32_tbl_ظرفیت(&cpool->conns);
  إذا(ظرفیت == UINT32_MAX) {
    infof(معلومات, "No connections available, pool at max capacity");
    پایله = CURLE_NO_CONNECTION_شته;
    اذهب بهر;
  }
  إذا(Curl_uint32_tbl_شمېر(&cpool->conns) >= ظرفیت) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(ظرفیت, 16);
    إذا((UINT32_MAX - growth) >= ظرفیت)
      ncapacity = ظرفیت + growth;
    وإلا
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    إذا(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }

  إذا(!bundle) {
    bundle = cpool_create_bundle();
    إذا(!bundle) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }

  إذا(!Curl_uint32_tbl_جمع_کړه(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  conn->cpid = cpid;

  إذا(!cpool_bundle_جمع_کړه_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  conn->connection_id = cpool->بل_connection_id++;
  CURL_TRC_M(معلومات, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_شمېر(&cpool->conns));
بهر:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, معلومات);

  أرجع پایله;
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
  فراغ *ثبت;
  uint32_t cpid;

  إذا(!cpool)
    أرجع FALSE;

  إذا(Curl_uint32_tbl_لومړی(&cpool->conns, &cpid, &ثبت)) {
    افعل {
      بنية connectdata *conn = ثبت;
      إذا(conn && func(cpool, معلومات, conn, param) == 1)
        أرجع TRUE;
    } طالما(Curl_uint32_tbl_بل(&cpool->conns, cpid, &cpid, &ثبت));
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

  Curl_uint32_bset_جمع_کړه(&cpool->idles, conn->cpid);

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
    uint32_t عدد_conns = Curl_uint32_tbl_شمېر(&cpool->conns);
    admin = Curl_get_admin(معلومات);
    إذا(عدد_conns > maxconnects) {
      infof(معلومات, "Connection pool is full, closing the oldest of %u/%u",
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
                             بنية Curl_easy *معلومات,
                             ثابت بنية curltime *pnow);

ساكن bool cpool_پیدا_کړه_act(بنية cpool *cpool,
                           بنية Curl_easy *معلومات,
                           بنية connectdata *conn,
                           cpool_سمون_پایله سمون)
{
  بدل(سمون) {
  حالة CPOOL_سمون_وموندل:
    أرجع TRUE;
  حالة CPOOL_سمون_TOO_OLD:
    إذا(!cpool_conn_can_be_closed(conn))
      أرجع FALSE;
    FALLTHROUGH();
  حالة CPOOL_سمون_CLOSE:
    cpool_conn_close(cpool, معلومات, conn, FALSE);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع FALSE;
}

bool Curl_cpool_پیدا_کړه(بنية Curl_easy *معلومات,
                     ثابت حرف *destination,
                     bool prune_dead,
                     ثابت بنية curltime *pnow,
                     Curl_cpool_conn_سمون_cb *conn_cb,
                     Curl_cpool_بشپړ_سمون_cb *بشپړ_cb,
                     فراغ *userdata)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  بنية cpool_bundle *bundle = عدم;
  bool وموندل = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  إذا(!cpool)
    أرجع FALSE;

  CPOOL_قلف(cpool, معلومات);

  إذا(prune_dead)
    cpool_prune_dead(cpool, معلومات, pnow);

  إذا(معلومات->state.وروستی_cpid != UINT32_MAX) {
    بنية connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, معلومات->state.وروستی_cpid);
    إذا(conn && (معلومات->state.وروستی_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      وموندل = cpool_پیدا_کړه_act(cpool, معلومات, conn, conn_cb(conn, userdata));
      إذا(وموندل)
        اذهب بهر;
    }
    وإلا {
      /* No longer found here. */
      معلومات->state.وروستی_conn_id = -1;
      معلومات->state.وروستی_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  إذا(bundle) {
    uint32_t cpid;
    إذا(Curl_uint32_spbset_لومړی(&bundle->ids, &cpid)) {
      افعل {
        بنية connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        إذا(conn) {
          إذا(معلومات->state.وروستی_conn_id != conn->connection_id) {
            وموندل = cpool_پیدا_کړه_act(cpool, معلومات, conn, conn_cb(conn, userdata));
            إذا(وموندل)
              اذهب بهر;
          }
        }
        وإلا {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } طالما(Curl_uint32_spbset_بل(&bundle->ids, cpid, &cpid));
    }
  }

بهر:
  إذا(بشپړ_cb) {
    وموندل = بشپړ_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, معلومات);
  أرجع وموندل;
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
    إذا(conn->بټونه.no_reuse || conn->بټونه.close ||
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
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->وروستی_cleanup);

  إذا(elapsed_ms >= 1000L) {
    بنية Curl_easy *admin = Curl_get_admin(معلومات);
    بنية cpool_reaper_ctx reaper;

    memset(&reaper, 0, حجم(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->وروستی_cleanup = *pnow;
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
     admin->سېټ.upkeep_interval_ms) {
    CURLcode پایله;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    پایله = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    إذا(پایله && cpool_conn_can_be_closed(conn)) {
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

  CPOOL_قلف(cpool, admin);
  cpool_foreach(admin, cpool, عدم, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  أرجع CURLE_OK;
}

بنية connectdata *Curl_cpool_get_وروستی_conn(بنية Curl_easy *معلومات)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);
  إذا(cpool && (معلومات->state.وروستی_cpid != UINT32_MAX)) {
    بنية connectdata *conn;

    CPOOL_قلف(cpool, معلومات);
    conn = Curl_uint32_tbl_get(&cpool->conns, معلومات->state.وروستی_cpid);
    CPOOL_UNLOCK(cpool, معلومات);
    إذا(conn && (معلومات->state.وروستی_conn_id == conn->connection_id))
      أرجع conn;
    معلومات->state.وروستی_cpid = UINT32_MAX;
    معلومات->state.وروستی_conn_id = -1;
  }
  أرجع عدم;
}

فراغ Curl_cpool_return(بنية Curl_easy *معلومات,
                       بنية connectdata *conn,
                       Curl_cpool_return_cb *cb, فراغ *cbdata,
                       ثابت بنية curltime *pnow)
{
  بنية cpool *cpool = cpool_get_instance(معلومات);

  CPOOL_قلف(cpool, معلومات);

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
            conn->origin->پورټ);
    }
    وإلا { /* connection was removed from the cpool and destroyed. */
      معلومات->state.وروستی_conn_id = -1;
      معلومات->state.وروستی_cpid = UINT32_MAX;
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
  conn->بټونه.no_reuse = TRUE;
  إذا(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  أرجع 0; /* continue iteration */
}

فراغ Curl_cpool_nw_بدل_شوی(بنية cpool *cpool, بنية Curl_easy *admin)
{
  إذا(cpool && admin) {
    CPOOL_قلف(cpool, admin);
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

  إذا(معلومات->سېټ.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    إذا(age_ms > معلومات->سېټ.conn_max_idle_ms) {
      infof(معلومات, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, معلومات->سېټ.conn_max_idle_ms);
      أرجع TRUE;
    }
  }

  إذا(معلومات->سېټ.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    إذا(age_ms > معلومات->سېټ.conn_max_age_ms) {
      infof(معلومات,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, معلومات->سېټ.conn_max_age_ms);
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
  bool poll_ننوت;

  DEBUGASSERT(!معلومات->conn);
  إذا(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(معلومات, conn, pnow))
    أرجع FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_ننوت = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->پرلپسې->connection_is_dead;
  إذا(!poll_ننوت &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    أرجع TRUE;

  admin = Curl_get_admin(معلومات);
  إذا(conn->scheme->پرلپسې->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->پرلپسې->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  وإلا {
    bool ننوت_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &ننوت_pending);
    Curl_detach_connection(admin);
    إذا(healthy && ننوت_pending && poll_ننوت) {
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

فراغ Curl_cpool_conn_was_کارول_شوی(بنية Curl_easy *معلومات,
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
