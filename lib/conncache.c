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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "progress.h"
#شامل "multiif.h"
#شامل "curl_trc.h"
#شامل "cshutdn.h"
#شامل "conncache.h"
#شامل "curl_share.h"
#شامل "sigpipe.h"
#شامل "uint-spbset.h"


#تعریف_کرو CPOOL_IS_LOCKED(c)    ((c) && (c)->locked)

#تعریف_کرو CPOOL_تالا(c, d)                                                \
  کرو {                                                                  \
    اگر(c) {                                                             \
      اگر(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_تالا_share((c)->share, (d), CURL_تالا_ڈیٹا_CONNECT,  \
                        CURL_تالا_ACCESS_SINGLE);                       \
      DEBUGASSERT(!(c)->locked);                                        \
      (c)->locked = TRUE;                                               \
    }                                                                   \
  } جب(0)

#تعریف_کرو CPOOL_UNLOCK(c, d)                                              \
  کرو {                                                                  \
    اگر(c) {                                                             \
      DEBUGASSERT((c)->locked);                                         \
      (c)->locked = FALSE;                                              \
      اگر(CURL_SHARE_KEEP_CONNECT((c)->share))                           \
        Curl_share_unlock_share((c)->share, (d), CURL_تالا_ڈیٹا_CONNECT); \
    }                                                                   \
  } جب(0)

/* A set of connection pool ids to the same destination. */
ساخت cpool_bundle {
  ساخت uint32_spbset ids; /* bitset of connection `pool_id`s  */
  uint32_t refcount;
};

ساکن خالی cpool_bundle_unlink(ساخت cpool_bundle **pbundle)
{
  اگر(*pbundle) {
    ساخت cpool_bundle *bundle = *pbundle;

    DEBUGASSERT(bundle->refcount);
    *pbundle = تہی;
    اگر(bundle->refcount)
      bundle->refcount--;
    اگر(!bundle->refcount) {
      Curl_uint32_spbset_destroy(&bundle->ids);
      curlx_free(bundle);
    }
  }
}

ساکن خالی cpool_bundle_link(ساخت cpool_bundle **pdest,
                              ساخت cpool_bundle *src)
{
  اگر(*pdest != src) {
    cpool_bundle_unlink(pdest);
    *pdest = src;
    اگر(src) {
      DEBUGASSERT(src->refcount < UINT32_MAX);
      src->refcount++;
    }
  }
}

ساکن ساخت cpool_bundle *cpool_create_bundle(خالی)
{
  /* created with refcount == 1 */
  ساخت cpool_bundle *bundle = curlx_calloc(1, حجم(*bundle));
  اگر(!bundle)
    واپس تہی;

  Curl_uint32_spbset_init(&bundle->ids);
  bundle->refcount = 1;
  واپس bundle;
}

/* Add a connection to a bundle */
ساکن bool cpool_bundle_جوڑو_id(ساخت cpool *cpool,
                                ساخت cpool_bundle *bundle,
                                ثابت حرف *destination,
                                uint32_t pool_id)
{
  اگر(!Curl_uint32_spbset_جوڑو(&bundle->ids, pool_id))
    واپس FALSE;
  اگر(Curl_uint32_spbset_گنتی(&bundle->ids) == 1) {
    /* first one added, register bundle */
    ساخت cpool_bundle *new_ref = تہی;
    cpool_bundle_link(&new_ref, bundle);
    اگر(!Curl_hash_جوڑو(&cpool->dest2bundle,
                      destination, لڑی_لمبائی(destination), new_ref)) {
      cpool_bundle_unlink(&new_ref);
      واپس FALSE;
    }
  }
  واپس TRUE;
}

/* Remove a connection from a bundle */
ساکن خالی cpool_bundle_remove_id(ساخت cpool *cpool,
                                   ساخت cpool_bundle *bundle,
                                   ثابت حرف *destination,
                                   uint32_t pool_id)
{
  Curl_uint32_spbset_remove(&bundle->ids, pool_id);
  اگر(!Curl_uint32_spbset_گنتی(&bundle->ids)) {
    /* Now empty, unregister bundle */
    Curl_hash_delete(&cpool->dest2bundle, destination, لڑی_لمبائی(destination));
  }
}

ساکن خالی cpool_bundle_hash_dtor(خالی *اندراج_2)
{
  ساخت cpool_bundle *bundle = اندراج_2;
  cpool_bundle_unlink(&bundle);
}

ساکن ساخت cpool_bundle *cpool_get_bundle(ساخت cpool *cpool,
                                             ثابت حرف *destination)
{
  ساخت cpool_bundle *bundle = تہی;
  cpool_bundle_link(&bundle, Curl_hash_pick(
    &cpool->dest2bundle, destination, لڑی_لمبائی(destination)));
  واپس bundle;
}

ساکن خالی cpool_remove_conn(ساخت cpool *cpool,
                              ساخت connectdata *conn)
{
  uint32_t cpid = conn->cpid;

  DEBUGASSERT(cpool);
  اگر(cpid != UINT32_MAX) {
    ساخت cpool_bundle *bundle;

#اگر_تعریف_شدہ DEBUGBUILD
    {
      ساخت connectdata *pooled_conn =
        Curl_uint32_tbl_get(&cpool->conns, cpid);
      DEBUGASSERT(conn == pooled_conn);
    }
#اگر_ختم
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    Curl_uint32_bset_remove(&cpool->idles, cpid);
    conn->cpid = UINT32_MAX;

    bundle = cpool_get_bundle(cpool, conn->destination);
    DEBUGASSERT(bundle);
    اگر(bundle)
      cpool_bundle_remove_id(cpool, bundle, conn->destination, cpid);
    cpool_bundle_unlink(&bundle);
  }
}

ساکن خالی cpool_discard_conn(ساخت cpool *cpool,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               bool aborted)
{
  ساخت Curl_easy *admin;
  bool مکمل = FALSE;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(!ڈیٹا->conn);
  DEBUGASSERT(cpool);
  DEBUGASSERT(conn->cpid == UINT32_MAX);

  admin = Curl_get_admin(ڈیٹا);
  /* We MUST NOT discard a connection that is still in use and would
   * trigger Use-After-Free. */
  اگر(CONN_INUSE(conn)) {
    CURL_TRC_M(admin, "[CPOOL] not discarding #%" FMT_OFF_T
               " still in use by %u transfers", conn->connection_id,
               conn->attached_xfers);
    DEBUGASSERT(0);
    واپس;
  }

  /* treat the connection as aborted in CONNECT_ONLY situations, we do
   * not know what the APP did with it. */
  اگر(conn->بٹ.connect_only)
    aborted = TRUE;
  conn->بٹ.aborted = aborted;

  /* We do not shutdown dead connections. The term 'dead' can be misleading
   * here, as we also mark errored connections/transfers as 'dead'.
   * If we do a shutdown for an aborted transfer, the server might think
   * it was successful otherwise (for example an ftps: upload). This is
   * not what we want. */
  اگر(aborted)
    مکمل = TRUE;
  اگر(!مکمل) {
    /* Attempt to shutdown the connection right away. */
    Curl_cshutdn_try_once(admin, conn, &مکمل);
  }

  اگر(مکمل || !ڈیٹا->multi)
    Curl_cshutdn_terminate(admin, conn, FALSE);
  ورنہ {
    ساخت Curl_multi *multi = ڈیٹا->multi;
    size_t max_shutdowns = multi->max_کل_connections;
    uint32_t عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);

    اگر(عدد_conns < max_shutdowns)
      max_shutdowns -= عدد_conns;
    ورنہ اگر(max_shutdowns)
      max_shutdowns = 1;
    ورنہ /* no connection limit set, let's restrict growth nevertheless */
      max_shutdowns = CURLMAX(عدد_conns / 4, 128);
    Curl_cshutdn_جوڑو(&multi->cshutdn, multi, conn, max_shutdowns);
  }
}

خالی Curl_cpool_init(ساخت cpool *cpool,
                     ساخت Curl_share *share,
                     size_t سائز)
{
  Curl_uint32_tbl_init(&cpool->conns);
  Curl_uint32_bset_init(&cpool->idles);
  Curl_hash_init(&cpool->dest2bundle, سائز, CURL_HASH_TYPE_بائٹیں,
                 cpool_bundle_hash_dtor);
  cpool->share = share;
  cpool->initialized = TRUE;
  cpool->اندر_shutdown = FALSE;
  cpool->locked = FALSE;
}

خالی Curl_cpool_destroy(ساخت cpool *cpool, ساخت Curl_easy *admin)
{
  اگر(cpool && cpool->initialized && admin) {
    خالی *اندراج_2;
    uint32_t cpid;

    CURL_TRC_M(admin, "%s[CPOOL] destroy, %u connections",
               cpool->share ? "[SHARE] " : "",
               Curl_uint32_tbl_گنتی(&cpool->conns));
    /* Move all connections to the shutdown list */
    CPOOL_تالا(cpool, admin);
    اگر(Curl_uint32_tbl_پہلا(&cpool->conns, &cpid, &اندراج_2)) {
      ساخت Curl_sigpipe_ctx pipe_ctx;

      sigpipe_init(&pipe_ctx);
      sigpipe_apply(admin, &pipe_ctx);
      کرو {
        ساخت connectdata *conn = اندراج_2;
        cpool_remove_conn(cpool, conn);
        cpool_discard_conn(cpool, admin, conn, FALSE);
      } جب(Curl_uint32_tbl_اگلا(&cpool->conns, cpid, &cpid, &اندراج_2));
      sigpipe_restore(&pipe_ctx);
    }
    CPOOL_UNLOCK(cpool, admin);
    Curl_hash_destroy(&cpool->dest2bundle);
    Curl_uint32_bset_destroy(&cpool->idles);
    Curl_uint32_tbl_destroy(&cpool->conns);
  }
}

ساکن ساخت cpool *cpool_get_instance(ساخت Curl_easy *ڈیٹا)
{
  /* admin handles do not necessarily find the correct pool */
  DEBUGASSERT(ڈیٹا->mid);
  اگر(CURL_SHARE_KEEP_CONNECT(ڈیٹا->share))
    واپس &ڈیٹا->share->cpool;
  ورنہ اگر(ڈیٹا->multi_easy)
    واپس &ڈیٹا->multi_easy->cpool;
  ورنہ اگر(ڈیٹا->multi)
    واپس &ڈیٹا->multi->cpool;
  واپس تہی;
}

ساخت cpool *Curl_cpool_get_instance(ساخت Curl_easy *ڈیٹا)
{
  واپس cpool_get_instance(ڈیٹا);
}

خالی Curl_cpool_xfer_init(ساخت Curl_easy *ڈیٹا)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);

  اگر(cpool) {
    CPOOL_تالا(cpool, ڈیٹا);
    /* the identifier inside the connection cache */
    ڈیٹا->id = cpool->اگلا_easy_id++;
    اگر(cpool->اگلا_easy_id == CURL_OFF_T_MAX)
      cpool->اگلا_easy_id = 0;
    ڈیٹا->state.آخری_conn_id = -1;
    ڈیٹا->state.آخری_cpid = UINT32_MAX;

    CPOOL_UNLOCK(cpool, ڈیٹا);
  }
  ورنہ {
    /* We should not get here, but in a non-debug build, do something */
    DEBUGASSERT(0);
    ڈیٹا->id = 0;
    ڈیٹا->state.آخری_conn_id = -1;
    ڈیٹا->state.آخری_cpid = UINT32_MAX;
  }
}

ساکن bool cpool_conn_can_be_closed(ساخت connectdata *conn)
{
  /* CONNECT_ONLY sockets remain in use by the application. */
  واپس (conn && !CONN_INUSE(conn) && !conn->بٹ.connect_only);
}

ساکن ساخت connectdata *cpool_bundle_get_oldest_idle(
  ساخت cpool *cpool,
  ساخت cpool_bundle *bundle,
  ثابت ساخت curltime *pnow)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t غیر_مستعمل_ms;
  timediff_t oldest_ms = -1;

  اگر(Curl_uint32_spbset_پہلا(&bundle->ids, &cpid)) {
    کرو {
      ساخت connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      اگر(cpool_conn_can_be_closed(conn)) {
        /* Set higher score for the age passed since the connection was used */
        غیر_مستعمل_ms =
          curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        اگر(غیر_مستعمل_ms > oldest_ms) {
          oldest_ms = غیر_مستعمل_ms;
          oldest_id = cpid;
        }
      }
      ورنہ اگر(!conn) {
        DEBUGASSERT(0);
        Curl_uint32_spbset_remove(&bundle->ids, cpid);
      }
    } جب(Curl_uint32_spbset_اگلا(&bundle->ids, cpid, &cpid));
  }
  واپس Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ساکن ساخت connectdata *cpool_get_oldest_idle(ساخت cpool *cpool,
                                                 ثابت ساخت curltime *pnow,
                                                 timediff_t min_age_ms)
{
  uint32_t cpid, oldest_id = UINT32_MAX;
  timediff_t oldest_idle_ms = -1;
  timediff_t idle_ms;

  /* Iterate over all connections that once were idle. They may
   * no longer be, in which case we remove them from the set again. */
  اگر(Curl_uint32_bset_پہلا(&cpool->idles, &cpid)) {
    کرو {
      ساخت connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
      اگر(cpool_conn_can_be_closed(conn)) {
        idle_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
        اگر((idle_ms >= min_age_ms) && (idle_ms > oldest_idle_ms)) {
          oldest_idle_ms = idle_ms;
          oldest_id = cpid;
        }
      }
      ورنہ {
        DEBUGASSERT(conn);
        Curl_uint32_bset_remove(&cpool->idles, cpid);
      }
    } جب(Curl_uint32_bset_اگلا(&cpool->idles, cpid, &cpid));
  }
  واپس Curl_uint32_tbl_get(&cpool->conns, oldest_id);
}

ساکن خالی cpool_conn_close(ساخت cpool *cpool,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn,
                             bool aborted)
{
  ساخت Curl_easy *admin;
  bool do_تالا;

  DEBUGASSERT(cpool);
  DEBUGASSERT(ڈیٹا && !ڈیٹا->conn);
  اگر(!cpool)
    واپس;

  /* If this connection is not marked to force-close, leave it open if there
   * are other users of it */
  اگر(CONN_INUSE(conn) && !aborted) {
    DEBUGASSERT(0); /* does this ever happen? */
    DEBUGF(infof(ڈیٹا, "conn terminate when inuse: %u", conn->attached_xfers));
    واپس;
  }

  /* This method may be called while we are under lock, e.g. from a
   * user callback in find. */
  admin = Curl_get_admin(ڈیٹا);
  do_تالا = !CPOOL_IS_LOCKED(cpool);
  اگر(do_تالا)
    CPOOL_تالا(cpool, admin);

  اگر(conn->cpid != UINT32_MAX) {
    cpool_remove_conn(cpool, conn);
    DEBUGASSERT(conn->cpid == UINT32_MAX);
  }

  /* treat the connection as aborted in CONNECT_ONLY situations,
   * so no graceful shutdown is attempted. */
  اگر(conn->بٹ.connect_only)
    aborted = TRUE;

  اگر(ڈیٹا->multi) {
    /* Add it to the multi's cpool for shutdown handling */
    infof(ڈیٹا, "%s connection #%" FMT_OFF_T,
          aborted ? "closing" : "shutting down", conn->connection_id);
    cpool_discard_conn(&ڈیٹا->multi->cpool, ڈیٹا, conn, aborted);
  }
  ورنہ {
    /* No multi available, terminate */
    infof(ڈیٹا, "closing connection #%" FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, !aborted);
  }

  اگر(do_تالا)
    CPOOL_UNLOCK(cpool, admin);
}

خالی Curl_conn_close(ساخت Curl_easy *ڈیٹا,
                     ساخت connectdata *conn,
                     bool aborted)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);
  cpool_conn_close(cpool, ڈیٹا, conn, aborted);
}

/* Evict an idle connection to make room in the pool. A pool owned by
 * a share has no multi that could perform a controlled shutdown of the
 * connection; terminate it right away. Otherwise, hand it to the
 * transfer's multi for shutdown. Expects the pool to be locked. */
ساکن خالی cpool_evict_conn(ساخت cpool *cpool,
                             ساخت Curl_easy *admin,
                             ساخت connectdata *conn)
{
  اگر(cpool->share) {
    cpool_remove_conn(cpool, conn);
    Curl_cshutdn_terminate(admin, conn, TRUE);
  }
  ورنہ
    cpool_conn_close(cpool, admin, conn, FALSE);
}

#تعریف_کرو CPOOL_حد_بندی_OK     0
#تعریف_کرو CPOOL_حد_بندی_DEST   1
#تعریف_کرو CPOOL_حد_بندی_کل  2

ساکن صحیح cpool_check_limits(ساخت Curl_easy *ڈیٹا,
                              uint32_t max_کل,
                              ساخت connectdata *مقصد_جوڑو,
                              uint32_t max_host,
                              ساخت cpool_bundle *bundle,
                              ثابت ساخت curltime *pnow)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);
  ساخت cshutdn *cshutdn = ڈیٹا->multi ? &ڈیٹا->multi->cshutdn : تہی;
  ساخت Curl_easy *admin = تہی;

  اگر(!cpool)
    واپس CPOOL_حد_بندی_OK;

  اگر(!max_کل && !max_host)
    واپس CPOOL_حد_بندی_OK;

  اگر(max_host) {
    size_t زندہ = 0, shutdowns = 0;
    /* if we are at or above `dest_limit`, try to get rid of connections
     * in shutdown and, if that does not lower it, evict idle connections
     * from the pool. */
    admin = Curl_get_admin(ڈیٹا);
    اگر(bundle) {
      زندہ = Curl_uint32_spbset_گنتی(&bundle->ids);
      اگر(زندہ >= max_host) {
        size_t over = زندہ - max_host + 1;
        برائے(; over; --over) {
          ساخت connectdata *oldest_idle =
            cpool_bundle_get_oldest_idle(cpool, bundle, pnow);
          اگر(!oldest_idle)
            توڑو;
          /* disconnect the old conn and continue */
          CURL_TRC_M(admin, "Shutting down connection #%" FMT_OFF_T
                     " to '%s' due to destination limit of %u",
                     oldest_idle->connection_id, oldest_idle->destination,
                     max_host);
          cpool_evict_conn(cpool, admin, oldest_idle);
        }
        زندہ = Curl_uint32_spbset_گنتی(&bundle->ids);
      }
    }
    اگر(cshutdn) {
      shutdowns = Curl_cshutdn_dest_گنتی(cshutdn, مقصد_جوڑو->destination);
      اگر(shutdowns && ((زندہ + shutdowns) >= max_host)) {
        size_t over = زندہ + shutdowns - max_host + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin,
                                               مقصد_جوڑو->destination, over);
      }
    }
    /* We may not have gotten rid of as many connections as we wanted to */
    اگر((زندہ + shutdowns) >= max_host)
      واپس CPOOL_حد_بندی_DEST;
  }

  /* Internal transfers are outside the total limit */
  اگر(max_کل && !ڈیٹا->state.اندرونی_2) {
    size_t shutdowns = 0;
    uint32_t عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);

    اگر(!admin)
      admin = Curl_get_admin(ڈیٹا);
    اگر(عدد_conns >= max_کل) {
      size_t over = عدد_conns - max_کل + 1;
      برائے(; over; --over) {
        ساخت connectdata *oldest_idle =
          cpool_get_oldest_idle(cpool, pnow, 0);
        اگر(!oldest_idle)
          توڑو;
        /* disconnect the old conn and continue */
        CURL_TRC_M(admin, "Shutting down idle connection #%"
                   FMT_OFF_T " to '%s' due to total limit of %u",
                   oldest_idle->connection_id, oldest_idle->destination,
                   max_کل);
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }

    عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);
    اگر(cshutdn) {
      shutdowns = Curl_cshutdn_گنتی(cshutdn);
      اگر(shutdowns && (عدد_conns + shutdowns) >= max_کل) {
        size_t over = عدد_conns + shutdowns - max_کل + 1;
        shutdowns -= Curl_cshutdn_close_oldest(cshutdn, admin, تہی, over);
      }
    }
    اگر((عدد_conns + shutdowns) >= max_کل)
      واپس CPOOL_حد_بندی_کل;
  }

  واپس CPOOL_حد_بندی_OK;
}

CURLcode Curl_cpool_جوڑو(ساخت Curl_easy *ڈیٹا,
                        ساخت connectdata *conn,
                        uint32_t max_کل,
                        uint32_t max_host,
                        ثابت ساخت curltime *pnow)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت cpool_bundle *bundle = تہی;
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);
  uint32_t استعداد, cpid;
  DEBUGASSERT(conn);

  DEBUGASSERT(cpool);
  اگر(!cpool)
    واپس CURLE_FAILED_INIT;

  DEBUGASSERT(conn->cpid == UINT32_MAX);
  conn->created = *pnow;
  conn->shutdown.آغاز_ms[FIRSTSOCKET] =
    conn->shutdown.آغاز_ms[SECONDARYSOCKET] = -1;

  CPOOL_تالا(cpool, ڈیٹا);
  /* Find the bundle, should it exist, and check the limits */
  bundle = cpool_get_bundle(cpool, conn->destination);

  انتخاب(cpool_check_limits(ڈیٹا, max_کل, conn, max_host, bundle, pnow)) {
  حالت CPOOL_حد_بندی_DEST:
    infof(ڈیٹا, "No more connections allowed to host");
    نتیجہ = CURLE_NO_CONNECTION_دستیاب;
    جاؤ برآمد;
  حالت CPOOL_حد_بندی_کل:
    infof(ڈیٹا, "No connections available, total of %u reached.",
          ڈیٹا->multi->max_کل_connections);
    نتیجہ = CURLE_NO_CONNECTION_دستیاب;
    جاؤ برآمد;
  طےشدہ:
    توڑو;
  }

  /* We add the connection */
  استعداد = Curl_uint32_tbl_استعداد(&cpool->conns);
  اگر(استعداد == UINT32_MAX) {
    infof(ڈیٹا, "No connections available, pool at max capacity");
    نتیجہ = CURLE_NO_CONNECTION_دستیاب;
    جاؤ برآمد;
  }
  اگر(Curl_uint32_tbl_گنتی(&cpool->conns) >= استعداد) {
    uint32_t growth, ncapacity;

    growth = CURLMAX(استعداد, 16);
    اگر((UINT32_MAX - growth) >= استعداد)
      ncapacity = استعداد + growth;
    ورنہ
      ncapacity = UINT32_MAX;
    /* Grow bitset first, so table resize failure does not leave a too
     * short bitset should that one fail. */
    اگر(Curl_uint32_bset_resize(&cpool->idles, ncapacity) ||
       Curl_uint32_tbl_resize(&cpool->conns, ncapacity)) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  اگر(!bundle) {
    bundle = cpool_create_bundle();
    اگر(!bundle) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  اگر(!Curl_uint32_tbl_جوڑو(&cpool->conns, conn, &cpid)) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  conn->cpid = cpid;

  اگر(!cpool_bundle_جوڑو_id(cpool, bundle, conn->destination, cpid)) {
    Curl_uint32_tbl_remove(&cpool->conns, cpid);
    conn->cpid = UINT32_MAX;
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  conn->connection_id = cpool->اگلا_connection_id++;
  CURL_TRC_M(ڈیٹا, "[CPOOL] added connection %" FMT_OFF_T ". "
             "The cache now contains %u members",
             conn->connection_id, Curl_uint32_tbl_گنتی(&cpool->conns));
برآمد:
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, ڈیٹا);

  واپس نتیجہ;
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
ساکن bool cpool_foreach(ساخت Curl_easy *ڈیٹا,
                          ساخت cpool *cpool,
                          خالی *param,
                          صحیح (*func)(ساخت cpool *cpool,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn, خالی *param))
{
  خالی *اندراج_2;
  uint32_t cpid;

  اگر(!cpool)
    واپس FALSE;

  اگر(Curl_uint32_tbl_پہلا(&cpool->conns, &cpid, &اندراج_2)) {
    کرو {
      ساخت connectdata *conn = اندراج_2;
      اگر(conn && func(cpool, ڈیٹا, conn, param) == 1)
        واپس TRUE;
    } جب(Curl_uint32_tbl_اگلا(&cpool->conns, cpid, &cpid, &اندراج_2));
  }
  واپس FALSE;
}

/*
 * A connection (already in the pool) has become idle. Do any
 * cleanups in regard to the pool's limits.
 *
 * Return TRUE if idle connection kept in pool, FALSE if closed.
 */
ساکن bool cpool_conn_now_idle(ساخت cpool *cpool,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn,
                                ثابت ساخت curltime *pnow)
{
  ساخت connectdata *oldest_idle = تہی;
  ساخت Curl_easy *admin;
  uint32_t maxconnects;
  bool kept = TRUE;
  timediff_t min_age_ms = 0;

  اگر(!ڈیٹا || !ڈیٹا->multi)
    واپس kept;

  Curl_uint32_bset_جوڑو(&cpool->idles, conn->cpid);

  اگر(!ڈیٹا->multi->maxconnects) {
    /* Attached transfers is a weak indicator of business. */
    uint32_t attached = Curl_multi_xfers_attached(ڈیٹا->multi);
    maxconnects = (attached <= UINT32_MAX / 2) ? attached * 2 : UINT32_MAX;
    /* We are guessing. So, only evict a "seemingly superfluous" connection
     * when has not been used for this long, */
    min_age_ms = 1000;
  }
  ورنہ {
    maxconnects = ڈیٹا->multi->maxconnects;
  }

  اگر(cpool && maxconnects && (maxconnects < UINT32_MAX)) {
    uint32_t عدد_conns = Curl_uint32_tbl_گنتی(&cpool->conns);
    admin = Curl_get_admin(ڈیٹا);
    اگر(عدد_conns > maxconnects) {
      infof(ڈیٹا, "Connection pool is full, closing the oldest of %u/%u",
            عدد_conns, maxconnects);
      oldest_idle = cpool_get_oldest_idle(cpool, pnow, min_age_ms);
      kept = (oldest_idle != conn);
      اگر(oldest_idle) {
        cpool_evict_conn(cpool, admin, oldest_idle);
      }
    }
  }

  واپس kept;
}

ساکن خالی cpool_prune_dead(ساخت cpool *cpool,
                             ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت curltime *pnow);

ساکن bool cpool_ڈھونڈو_act(ساخت cpool *cpool,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت connectdata *conn,
                           cpool_مطابقت_نتیجہ مطابقت)
{
  انتخاب(مطابقت) {
  حالت CPOOL_مطابقت_ملا:
    واپس TRUE;
  حالت CPOOL_مطابقت_TOO_OLD:
    اگر(!cpool_conn_can_be_closed(conn))
      واپس FALSE;
    FALLTHROUGH();
  حالت CPOOL_مطابقت_CLOSE:
    cpool_conn_close(cpool, ڈیٹا, conn, FALSE);
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس FALSE;
}

bool Curl_cpool_ڈھونڈو(ساخت Curl_easy *ڈیٹا,
                     ثابت حرف *destination,
                     bool prune_dead,
                     ثابت ساخت curltime *pnow,
                     Curl_cpool_conn_مطابقت_cb *conn_cb,
                     Curl_cpool_مکمل_مطابقت_cb *مکمل_cb,
                     خالی *userdata)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);
  ساخت cpool_bundle *bundle = تہی;
  bool ملا = FALSE;

  DEBUGASSERT(cpool);
  DEBUGASSERT(conn_cb);
  اگر(!cpool)
    واپس FALSE;

  CPOOL_تالا(cpool, ڈیٹا);

  اگر(prune_dead)
    cpool_prune_dead(cpool, ڈیٹا, pnow);

  اگر(ڈیٹا->state.آخری_cpid != UINT32_MAX) {
    ساخت connectdata *conn =
      Curl_uint32_tbl_get(&cpool->conns, ڈیٹا->state.آخری_cpid);
    اگر(conn && (ڈیٹا->state.آخری_conn_id == conn->connection_id)) {
      /* Indeed the last connection used by `data` still exists.
       * Check that first, so that connection use gets "sticky". */
      ملا = cpool_ڈھونڈو_act(cpool, ڈیٹا, conn, conn_cb(conn, userdata));
      اگر(ملا)
        جاؤ برآمد;
    }
    ورنہ {
      /* No longer found here. */
      ڈیٹا->state.آخری_conn_id = -1;
      ڈیٹا->state.آخری_cpid = UINT32_MAX;
    }
  }

  bundle = cpool_get_bundle(cpool, destination);
  اگر(bundle) {
    uint32_t cpid;
    اگر(Curl_uint32_spbset_پہلا(&bundle->ids, &cpid)) {
      کرو {
        ساخت connectdata *conn = Curl_uint32_tbl_get(&cpool->conns, cpid);
        اگر(conn) {
          اگر(ڈیٹا->state.آخری_conn_id != conn->connection_id) {
            ملا = cpool_ڈھونڈو_act(cpool, ڈیٹا, conn, conn_cb(conn, userdata));
            اگر(ملا)
              جاؤ برآمد;
          }
        }
        ورنہ {
          DEBUGASSERT(0);
          Curl_uint32_spbset_remove(&bundle->ids, cpid);
        }
      } جب(Curl_uint32_spbset_اگلا(&bundle->ids, cpid, &cpid));
    }
  }

برآمد:
  اگر(مکمل_cb) {
    ملا = مکمل_cb(userdata);
  }
  cpool_bundle_unlink(&bundle);
  CPOOL_UNLOCK(cpool, ڈیٹا);
  واپس ملا;
}

ساخت cpool_reaper_ctx {
  size_t reaped;
  ثابت ساخت curltime *pnow;
};

ساکن صحیح cpool_reap_dead_cb(ساخت cpool *cpool,
                              ساخت Curl_easy *admin,
                              ساخت connectdata *conn, خالی *param)
{
  ساخت cpool_reaper_ctx *reaper = param;

  اگر(cpool_conn_can_be_closed(conn)) {
    اگر(conn->بٹ.no_reuse || conn->بٹ.close ||
       !Curl_cpool_conn_seems_healthy(conn, admin, reaper->pnow)) {
      /* terminate conn and stop the iteration */
      reaper->reaped++;
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  واپس 0; /* continue iteration */
}

/*
 * This function scans the data's connection pool for half-open/dead
 * connections, closes and removes them.
 * The cleanup is done at most once per second.
 */
ساکن خالی cpool_prune_dead(ساخت cpool *cpool,
                             ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت curltime *pnow)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(pnow, &cpool->آخری_cleanup);

  اگر(elapsed_ms >= 1000L) {
    ساخت Curl_easy *admin = Curl_get_admin(ڈیٹا);
    ساخت cpool_reaper_ctx reaper;

    memset(&reaper, 0, حجم(reaper));
    reaper.pnow = pnow;
    cpool_foreach(admin, cpool, &reaper, cpool_reap_dead_cb);
    cpool->آخری_cleanup = *pnow;
  }
}

ساکن صحیح conn_upkeep(ساخت cpool *cpool,
                       ساخت Curl_easy *admin,
                       ساخت connectdata *conn,
                       خالی *param)
{
  ثابت ساخت curltime *pnow = Curl_pgrs_now(admin);

  (خالی)param;
  اگر((curlx_ptimediff_ms(pnow, &conn->created) - conn->lastupkeep_ms) >=
     admin->سیٹ.upkeep_interval_ms) {
    CURLcode نتیجہ;

    conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
    /* briefly attach for action */
    Curl_attach_connection(admin, conn, FALSE);
    نتیجہ = Curl_conn_keep_alive(admin, conn);
    Curl_detach_connection(admin);

    اگر(نتیجہ && cpool_conn_can_be_closed(conn)) {
      cpool_conn_close(cpool, admin, conn, FALSE);
    }
  }
  واپس 0; /* continue iteration */
}

CURLcode Curl_cpool_upkeep(ساخت Curl_easy *ڈیٹا)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);
  ساخت Curl_easy *admin = Curl_get_admin(ڈیٹا);

  اگر(!cpool)
    واپس CURLE_OK;

  CPOOL_تالا(cpool, admin);
  cpool_foreach(admin, cpool, تہی, conn_upkeep);
  CPOOL_UNLOCK(cpool, admin);
  واپس CURLE_OK;
}

ساخت connectdata *Curl_cpool_get_آخری_conn(ساخت Curl_easy *ڈیٹا)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);
  اگر(cpool && (ڈیٹا->state.آخری_cpid != UINT32_MAX)) {
    ساخت connectdata *conn;

    CPOOL_تالا(cpool, ڈیٹا);
    conn = Curl_uint32_tbl_get(&cpool->conns, ڈیٹا->state.آخری_cpid);
    CPOOL_UNLOCK(cpool, ڈیٹا);
    اگر(conn && (ڈیٹا->state.آخری_conn_id == conn->connection_id))
      واپس conn;
    ڈیٹا->state.آخری_cpid = UINT32_MAX;
    ڈیٹا->state.آخری_conn_id = -1;
  }
  واپس تہی;
}

خالی Curl_cpool_return(ساخت Curl_easy *ڈیٹا,
                       ساخت connectdata *conn,
                       Curl_cpool_return_cb *cb, خالی *cbdata,
                       ثابت ساخت curltime *pnow)
{
  ساخت cpool *cpool = cpool_get_instance(ڈیٹا);

  CPOOL_تالا(cpool, ڈیٹا);

  /* remember times, connection had been used just before */
  conn->lastchecked_ms = conn->lastupkeep_ms = conn->lastused_ms =
    curlx_ptimediff_ms(pnow, &conn->created);

  انتخاب(cb(ڈیٹا, conn, cbdata, pnow)) {
  حالت CPOOL_DO_KEEP:
    توڑو;
  حالت CPOOL_DO_IDLE:
    /* the connection is no longer in use by any transfer */
    اگر(cpool_conn_now_idle(cpool, ڈیٹا, conn, pnow)) {
      /* connection kept in the cpool */
      infof(ڈیٹا, "Connection #%" FMT_OFF_T " to host %s:%u left intact",
            conn->connection_id, conn->origin->user_hostname,
            conn->origin->پورٹ);
    }
    ورنہ { /* connection was removed from the cpool and destroyed. */
      ڈیٹا->state.آخری_conn_id = -1;
      ڈیٹا->state.آخری_cpid = UINT32_MAX;
    }
    توڑو;
  حالت CPOOL_DO_CLOSE:
    Curl_conn_close(ڈیٹا, conn, FALSE);
    توڑو;
  حالت CPOOL_DO_TERMINATE:
    Curl_conn_close(ڈیٹا, conn, TRUE);
    توڑو;
  }

  CPOOL_UNLOCK(cpool, ڈیٹا);
}

ساکن صحیح cpool_reap_no_reuse(ساخت cpool *cpool,
                               ساخت Curl_easy *admin,
                               ساخت connectdata *conn, خالی *param)
{
  (خالی)param;
  conn->بٹ.no_reuse = TRUE;
  اگر(cpool_conn_can_be_closed(conn))
    cpool_conn_close(cpool, admin, conn, FALSE);
  واپس 0; /* continue iteration */
}

خالی Curl_cpool_nw_بدلا(ساخت cpool *cpool, ساخت Curl_easy *admin)
{
  اگر(cpool && admin) {
    CPOOL_تالا(cpool, admin);
    cpool_foreach(admin, cpool, تہی, cpool_reap_no_reuse);
    CPOOL_UNLOCK(cpool, admin);
  }
}

/* A connection has to have been idle for less than 'conn_max_idle_ms'
   (the success rate is too low after this), or created less than
   'conn_max_age_ms' ago, to be subject for reuse. */
ساکن bool cpool_conn_too_old(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               ثابت ساخت curltime *pnow)
{
  timediff_t age_ms;

  اگر(ڈیٹا->سیٹ.conn_max_idle_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created) - conn->lastused_ms;
    اگر(age_ms > ڈیٹا->سیٹ.conn_max_idle_ms) {
      infof(ڈیٹا, "Too old connection (%" FMT_TIMEDIFF_T
            " ms idle, max idle is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, ڈیٹا->سیٹ.conn_max_idle_ms);
      واپس TRUE;
    }
  }

  اگر(ڈیٹا->سیٹ.conn_max_age_ms) {
    age_ms = curlx_ptimediff_ms(pnow, &conn->created);
    اگر(age_ms > ڈیٹا->سیٹ.conn_max_age_ms) {
      infof(ڈیٹا,
            "Too old connection (created %" FMT_TIMEDIFF_T
            " ms ago, max lifetime is %" FMT_TIMEDIFF_T " ms), disconnect it",
            age_ms, ڈیٹا->سیٹ.conn_max_age_ms);
      واپس TRUE;
    }
  }

  واپس FALSE;
}

bool Curl_cpool_conn_seems_healthy(ساخت connectdata *conn,
                                   ساخت Curl_easy *ڈیٹا,
                                   ثابت ساخت curltime *pnow)
{
  ساخت Curl_easy *admin;
  bool healthy = TRUE;
  bool poll_داخلہ;

  DEBUGASSERT(!ڈیٹا->conn);
  اگر(cpool_conn_can_be_closed(conn) && cpool_conn_too_old(ڈیٹا, conn, pnow))
    واپس FALSE;

  /* An idle, non-multiplexed connection must not be reused when it has
   * input pending (RFC 9112, section 9.2), so that is polled on every
   * reuse. The once a second limit only applies to the other checks. */
  poll_داخلہ = cpool_conn_can_be_closed(conn) &&
    !Curl_conn_is_multiplex(conn, FIRSTSOCKET) &&
    !conn->scheme->تسلسل->connection_is_dead;
  اگر(!poll_داخلہ &&
     (curlx_ptimediff_ms(pnow, &conn->created) - conn->lastchecked_ms) < 1000)
    واپس TRUE;

  admin = Curl_get_admin(ڈیٹا);
  اگر(conn->scheme->تسلسل->connection_is_dead) {
    Curl_attach_connection(admin, conn, FALSE);
    healthy = !conn->scheme->تسلسل->connection_is_dead(admin, conn);
    Curl_detach_connection(admin);
  }
  ورنہ {
    bool داخلہ_pending = FALSE;

    Curl_attach_connection(admin, conn, FALSE);
    healthy = Curl_conn_is_alive(admin, conn, &داخلہ_pending);
    Curl_detach_connection(admin);
    اگر(healthy && داخلہ_pending && poll_داخلہ) {
      /* Non-multiplexed connections without attached transfers should
       * not have input pending. The input might be a TLS Notify Close,
       * for all we know. */
      DEBUGF(infof(ڈیٹا, "connection has no transfer but input, not healthy"));
      healthy = FALSE;
    }
  }

  اگر(healthy)
    conn->lastchecked_ms = curlx_ptimediff_ms(pnow, &conn->created);
  واپس healthy;
}

خالی Curl_cpool_conn_was_مستعمل(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn,
                              ثابت ساخت curltime *pnow)
{
  (خالی)ڈیٹا;
  conn->lastupkeep_ms = curlx_ptimediff_ms(pnow, &conn->created);
}

timediff_t Curl_cpool_conn_age_ms(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  ثابت ساخت curltime *pnow)
{
  (خالی)ڈیٹا;
  واپس curlx_ptimediff_ms(pnow, &conn->created);
}
