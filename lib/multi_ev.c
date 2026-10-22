/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "multi_ev.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "uint-bset.h"
#అన్తర్భావ "uint-spbset.h"
#అన్తర్భావ "multihandle.h"

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు SH_నమోదు_MAGIC 0x570091d
#అయితే_ముగింపు

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
నిర్మాణం mev_sh_నమోదు {
  నిర్మాణం uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  నిర్మాణం connectdata *conn; /* connection using this socket or NULL */
  శూన్య *user_దత్తాంశం;      /* libcurl app data via curl_multi_assign() */
  గుర్తులేని పూర్ణాంకం action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  గుర్తులేని పూర్ణాంకం readers; /* this many transfers want to read */
  గుర్తులేని పూర్ణాంకం writers; /* this many transfers want to write */
#నిర్వచితమైతే DEBUGBUILD
  గుర్తులేని పూర్ణాంకం magic;
#అయితే_ముగింపు
  బిట్టు(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
స్థిర శూన్య mev_sh_నమోదు_dtor(శూన్య *freethis)
{
  నిర్మాణం mev_sh_నమోదు *నమోదు_2 = (నిర్మాణం mev_sh_నమోదు *)freethis;
  Curl_uint32_spbset_destroy(&నమోదు_2->xfers);
#నిర్వచితమైతే DEBUGBUILD
  నమోదు_2->magic = 0;
#అయితే_ముగింపు
  curlx_free(నమోదు_2);
}

/* look up a given socket in the socket hash, skip invalid sockets */
స్థిర నిర్మాణం mev_sh_నమోదు *mev_sh_నమోదు_get(నిర్మాణం Curl_hash *sh,
                                             curl_socket_t s)
{
  ఒకవేళ(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    తిరిగి Curl_hash_pick(sh, (స్థిరరాశి అక్షరం *)&s, పరిమాణం(curl_socket_t));
  }
  తిరిగి ఖాళీ;
}

/* make sure this socket is present in the hash for this handle */
స్థిర నిర్మాణం mev_sh_నమోదు *mev_sh_నమోదు_కూడు(నిర్మాణం Curl_hash *sh,
                                             curl_socket_t s)
{
  నిర్మాణం mev_sh_నమోదు *there = mev_sh_నమోదు_get(sh, s);
  నిర్మాణం mev_sh_నమోదు *check;

  ఒకవేళ(there) {
    /* it is present, return fine */
    తిరిగి there;
  }

  /* not present, add it */
  check = curlx_calloc(1, పరిమాణం(నిర్మాణం mev_sh_నమోదు));
  ఒకవేళ(!check)
    తిరిగి ఖాళీ; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  ఒకవేళ(!Curl_hash_కూడు(sh, (అక్షరం *)&s, పరిమాణం(curl_socket_t), check)) {
    mev_sh_నమోదు_dtor(check);
    తిరిగి ఖాళీ; /* major failure */
  }
#నిర్వచితమైతే DEBUGBUILD
  check->magic = SH_నమోదు_MAGIC;
#అయితే_ముగింపు
  తిరిగి check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
స్థిర శూన్య mev_sh_నమోదు_kill(నిర్మాణం Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (అక్షరం *)&s, పరిమాణం(curl_socket_t));
}

స్థిర size_t mev_sh_నమోదు_user_లెక్క(నిర్మాణం mev_sh_నమోదు *e)
{
  తిరిగి Curl_uint32_spbset_లెక్క(&e->xfers) + (e->conn ? 1 : 0);
}

స్థిర bool mev_sh_నమోదు_xfer_తెలిసిన(నిర్మాణం mev_sh_నమోదు *e,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_uint32_spbset_కలిగియుంది(&e->xfers, దత్తాంశం->mid);
}

స్థిర bool mev_sh_నమోదు_conn_తెలిసిన(నిర్మాణం mev_sh_నమోదు *e,
                                    నిర్మాణం connectdata *conn)
{
  తిరిగి (e->conn == conn);
}

స్థిర శూన్య mev_pollset_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం easy_pollset *ps = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  ఒకవేళ(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

స్థిర నిర్మాణం easy_pollset *mev_కూడు_new_conn_pollset(నిర్మాణం connectdata *conn)
{
  నిర్మాణం easy_pollset *ps;

  ps = Curl_pollset_create();
  ఒకవేళ(!ps)
    తిరిగి ఖాళీ;
  ఒకవేళ(Curl_conn_meta_సమితి(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    తిరిగి ఖాళీ;
  తిరిగి ps;
}

స్థిర నిర్మాణం easy_pollset *mev_కూడు_new_xfer_pollset(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం easy_pollset *ps;

  ps = Curl_pollset_create();
  ఒకవేళ(!ps)
    తిరిగి ఖాళీ;
  ఒకవేళ(Curl_meta_సమితి(దత్తాంశం, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    తిరిగి ఖాళీ;
  తిరిగి ps;
}

స్థిర నిర్మాణం easy_pollset *mev_get_చివరి_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం connectdata *conn)
{
  ఒకవేళ(దత్తాంశం) {
    ఒకవేళ(conn)
      తిరిగి Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    తిరిగి Curl_meta_get(దత్తాంశం, CURL_META_MEV_POLLSET);
  }
  తిరిగి ఖాళీ;
}

స్థిర bool mev_sh_నమోదు_xfer_కూడు(నిర్మాణం mev_sh_నమోదు *e,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_నమోదు_user_లెక్క(e) < 100000);
  తిరిగి Curl_uint32_spbset_కూడు(&e->xfers, దత్తాంశం->mid);
}

స్థిర bool mev_sh_నమోదు_conn_కూడు(నిర్మాణం mev_sh_నమోదు *e,
                                  నిర్మాణం connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_నమోదు_user_లెక్క(e) < 100000);
  DEBUGASSERT(!e->conn);
  ఒకవేళ(e->conn)
    తిరిగి FALSE;
  e->conn = conn;
  తిరిగి TRUE;
}

స్థిర bool mev_sh_నమోదు_xfer_remove(నిర్మాణం mev_sh_నమోదు *e,
                                     నిర్మాణం Curl_easy *దత్తాంశం)
{
  bool present = Curl_uint32_spbset_కలిగియుంది(&e->xfers, దత్తాంశం->mid);
  ఒకవేళ(present)
    Curl_uint32_spbset_remove(&e->xfers, దత్తాంశం->mid);
  తిరిగి present;
}

స్థిర bool mev_sh_నమోదు_conn_remove(నిర్మాణం mev_sh_నమోదు *e,
                                     నిర్మాణం connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  ఒకవేళ(e->conn == conn) {
    e->conn = ఖాళీ;
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
స్థిర CURLMcode mev_forget_socket(నిర్మాణం Curl_multi *multi,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   curl_socket_t s,
                                   స్థిరరాశి అక్షరం *cause)
{
  నిర్మాణం mev_sh_నమోదు *నమోదు_2 = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  పూర్ణాంకం rc = 0;

  ఒకవేళ(!నమోదు_2) /* we never knew or already forgot about this socket */
    తిరిగి CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  ఒకవేళ(Curl_uint32_spbset_మొదటి(&నమోదు_2->xfers, &mid)) {
    చేయి {
      నిర్మాణం Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(sdata) {
        నిర్మాణం easy_pollset *ps = mev_get_చివరి_pollset(sdata, ఖాళీ);
        ఒకవేళ(ps)
          Curl_pollset_remove(ps, s);
      }
    } అయితే(Curl_uint32_spbset_తదుపరి(&నమోదు_2->xfers, mid, &mid));
  }

  ఒకవేళ(నమోదు_2->conn) {
    నిర్మాణం easy_pollset *ps = mev_get_చివరి_pollset(దత్తాంశం, నమోదు_2->conn);
    ఒకవేళ(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  ఒకవేళ(నమోదు_2->announced && multi->socket_cb) {
    నిర్మాణం Curl_mapi_guard guard;

    NOVERBOSE((శూన్య)cause);
    CURL_TRC_M(దత్తాంశం, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_ప్రారంభం(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(దత్తాంశం, s, CURL_POLL_REMOVE,
                          multi->socket_userp, నమోదు_2->user_దత్తాంశం);
    CURL_CBAPI_చివర(&guard);
    నమోదు_2 = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);
    ఒకవేళ(నమోదు_2)
      నమోదు_2->announced = FALSE;
  }

  mev_sh_నమోదు_kill(multi, s);
  ఒకవేళ(rc == -1) {
    multi->dead = TRUE;
    తిరిగి CURLM_ABORTED_BY_CALLBACK;
  }
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode mev_sh_నమోదు_update(నిర్మాణం Curl_multi *multi,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం mev_sh_నమోదు *నమోదు_2,
                                     curl_socket_t s,
                                     గుర్తులేని అక్షరం చివరి_action,
                                     గుర్తులేని అక్షరం cur_action)
{
  పూర్ణాంకం rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(నమోదు_2->magic == SH_నమోదు_MAGIC);
  ఒకవేళ(!multi->socket_cb)
    తిరిగి CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  ఒకవేళ(చివరి_action == cur_action)  /* nothing from `data` changed */
    తిరిగి CURLM_OK;

  ఒకవేళ(చివరి_action & CURL_POLL_లోపల) {
    DEBUGASSERT(నమోదు_2->readers);
    ఒకవేళ(!(cur_action & CURL_POLL_లోపల))
      నమోదు_2->readers--;
  }
  లేకపోతే ఒకవేళ(cur_action & CURL_POLL_లోపల)
    నమోదు_2->readers++;

  ఒకవేళ(చివరి_action & CURL_POLL_బహిర్గతం) {
    DEBUGASSERT(నమోదు_2->writers);
    ఒకవేళ(!(cur_action & CURL_POLL_బహిర్గతం))
      నమోదు_2->writers--;
  }
  లేకపోతే ఒకవేళ(cur_action & CURL_POLL_బహిర్గతం)
    నమోదు_2->writers++;

  DEBUGASSERT(నమోదు_2->readers <= mev_sh_నమోదు_user_లెక్క(నమోదు_2));
  DEBUGASSERT(నమోదు_2->writers <= mev_sh_నమోదు_user_లెక్క(నమోదు_2));
  DEBUGASSERT(నమోదు_2->writers + నమోదు_2->readers);

  CURL_TRC_M(దత్తాంశం, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (చివరి_action & CURL_POLL_లోపల) ? "IN" : "",
             (చివరి_action & CURL_POLL_బహిర్గతం) ? "OUT" : "",
             (cur_action & CURL_POLL_లోపల) ? "IN" : "",
             (cur_action & CURL_POLL_బహిర్గతం) ? "OUT" : "",
             నమోదు_2->readers, నమోదు_2->writers);

  comboaction = (నమోదు_2->writers ? CURL_POLL_బహిర్గతం : 0) |
                (నమోదు_2->readers ? CURL_POLL_లోపల : 0);
  ఒకవేళ(((పూర్ణాంకం)నమోదు_2->action == comboaction)) /* nothing for socket changed */
    తిరిగి CURLM_OK;

  CURL_TRC_M(దత్తాంశం, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_లోపల) ? "IN" : "",
             (comboaction & CURL_POLL_బహిర్గతం) ? "OUT" : "");
  {
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_ప్రారంభం(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(దత్తాంశం, s, comboaction, multi->socket_userp,
                          నమోదు_2->user_దత్తాంశం);
    CURL_CBAPI_MULTI_చివర(&guard);
  }
  ఒకవేళ(rc == -1) {
    multi->dead = TRUE;
    తిరిగి CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  నమోదు_2 = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);
  ఒకవేళ(నమోదు_2) {
    DEBUGASSERT(నమోదు_2->magic == SH_నమోదు_MAGIC);
    నమోదు_2->announced = TRUE;
    నమోదు_2->action = (గుర్తులేని పూర్ణాంకం)comboaction;
  }
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode mev_pollset_diff(నిర్మాణం Curl_multi *multi,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  నిర్మాణం easy_pollset *ps,
                                  నిర్మాణం easy_pollset *prev_ps)
{
  నిర్మాణం mev_sh_నమోదు *నమోదు_2;
  curl_socket_t s;
  గుర్తులేని పూర్ణాంకం i, j;
  CURLMcode mresult;

  /* The transfer `data` reports in `ps` the sockets it is interested
   * in and which combination of CURL_POLL_IN/CURL_POLL_OUT it wants
   * to have monitored for events.
   * There can be more than 1 transfer interested in the same socket
   * and 1 transfer might be interested in more than 1 socket.
   * `prev_ps` is the pollset copy from the previous call here. On
   * the 1st call it will be empty.
   */
  DEBUGASSERT(ps);
  DEBUGASSERT(prev_ps);

  /* Handle changes to sockets the transfer is interested in. */
  కొరకు(i = 0; i < ps->n; i++) {
    గుర్తులేని అక్షరం చివరి_action;
    bool మొదటి_సమయం = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    నమోదు_2 = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);
    ఒకవేళ(!నమోదు_2) {
      /* new socket, add new entry */
      మొదటి_సమయం = TRUE;
      నమోదు_2 = mev_sh_నమోదు_కూడు(&multi->ev.sh_entries, s);
      ఒకవేళ(!నమోదు_2) /* fatal */
        తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      CURL_TRC_M(దత్తాంశం, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    లేకపోతే ఒకవేళ(conn) {
      మొదటి_సమయం = !mev_sh_నమోదు_conn_తెలిసిన(నమోదు_2, conn);
    }
    లేకపోతే {
      మొదటి_సమయం = !mev_sh_నమోదు_xfer_తెలిసిన(నమోదు_2, దత్తాంశం);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    చివరి_action = 0;
    ఒకవేళ(మొదటి_సమయం) {
      ఒకవేళ(conn) {
        ఒకవేళ(!mev_sh_నమోదు_conn_కూడు(నమోదు_2, conn))
          తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      లేకపోతే {
        ఒకవేళ(!mev_sh_నమోదు_xfer_కూడు(నమోదు_2, దత్తాంశం))
          తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      CURL_TRC_M(దత్తాంశం, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : దత్తాంశం->mid,
                 Curl_uint32_spbset_లెక్క(&నమోదు_2->xfers),
                 నమోదు_2->conn ? 1 : 0);
    }
    లేకపోతే {
      కొరకు(j = 0; j < prev_ps->n; j++) {
        ఒకవేళ(s == prev_ps->sockets[j]) {
          చివరి_action = prev_ps->actions[j];
          ఆపు;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_నమోదు_update(multi, దత్తాంశం, నమోదు_2, s,
                                  చివరి_action, ps->actions[i]);
    ఒకవేళ(mresult)
      తిరిగి mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  కొరకు(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    కొరకు(j = 0; j < ps->n; j++) {
      ఒకవేళ(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        ఆపు;
      }
    }
    ఒకవేళ(stillused)
      కొనసాగు;

    నమోదు_2 = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    ఒకవేళ(!నమోదు_2)
      కొనసాగు;

    ఒకవేళ(conn && !mev_sh_నమోదు_conn_remove(నమోదు_2, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(దత్తాంశం, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ఖాళీ);
      కొనసాగు;
    }

    ఒకవేళ(!conn && !mev_sh_నమోదు_xfer_remove(నమోదు_2, దత్తాంశం)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(దత్తాంశం, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ఖాళీ);
      కొనసాగు;
    }

    ఒకవేళ(mev_sh_నమోదు_user_లెక్క(నమోదు_2)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_నమోదు_update(multi, దత్తాంశం, నమోదు_2, s,
                                    prev_ps->actions[i], 0);
      ఒకవేళ(mresult)
        తిరిగి mresult;
      CURL_TRC_M(దత్తాంశం, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_లెక్క(&నమోదు_2->xfers),
                 నమోదు_2->conn ? 1 : 0);
    }
    లేకపోతే {
      mresult = mev_forget_socket(multi, దత్తాంశం, s, "last user gone");
      ఒకవేళ(mresult)
        తిరిగి mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode mev_assess(నిర్మాణం Curl_multi *multi,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం connectdata *conn)
{
  నిర్మాణం easy_pollset ps, *చివరి_ps;
  CURLMcode mresult = CURLM_OK;

  ఒకవేళ(!multi || !multi->socket_cb)
    తిరిగి CURLM_OK;

  Curl_pollset_init(&ps);
  ఒకవేళ(conn) {
    CURLcode ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, conn, &ps);
    ఒకవేళ(ఫలితం) {
      mresult = (ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం) ?
        CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం : CURLM_అంతర్గత_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే
    Curl_multi_pollset(దత్తాంశం, &ps);
  చివరి_ps = mev_get_చివరి_pollset(దత్తాంశం, conn);

  ఒకవేళ(!చివరి_ps && ps.n) {
    ఒకవేళ(conn)
      చివరి_ps = mev_కూడు_new_conn_pollset(conn);
    లేకపోతే
      చివరి_ps = mev_కూడు_new_xfer_pollset(దత్తాంశం);
    ఒకవేళ(!చివరి_ps) {
      mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(చివరి_ps)
    mresult = mev_pollset_diff(multi, దత్తాంశం, conn, &ps, చివరి_ps);
  లేకపోతే
    DEBUGASSERT(!ps.n);
బహిర్గతం:
  Curl_pollset_cleanup(&ps);
  తిరిగి mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి mev_assess(multi, దత్తాంశం, ఖాళీ);
}

CURLMcode Curl_multi_ev_assess_conn(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn)
{
  తిరిగి mev_assess(multi, దత్తాంశం, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(నిర్మాణం Curl_multi *multi,
                                         నిర్మాణం uint32_bset *సమితి)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  ఒకవేళ(multi && multi->socket_cb && Curl_uint32_bset_మొదటి(సమితి, &mid)) {
    చేయి {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(దత్తాంశం) {
        mresult = Curl_multi_ev_assess_xfer(multi, దత్తాంశం);
      }
    } అయితే(!mresult && Curl_uint32_bset_తదుపరి(సమితి, mid, &mid));
  }
  తిరిగి mresult;
}

CURLMcode Curl_multi_ev_కేటాయించు(నిర్మాణం Curl_multi *multi,
                               curl_socket_t s,
                               శూన్య *user_దత్తాంశం)
{
  నిర్మాణం mev_sh_నమోదు *e = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);
  ఒకవేళ(!e)
    తిరిగి CURLM_BAD_SOCKET;
  e->user_దత్తాంశం = user_దత్తాంశం;
  తిరిగి CURLM_OK;
}

శూన్య Curl_multi_ev_dirty_xfers(నిర్మాణం Curl_multi *multi,
                               curl_socket_t s)
{
  నిర్మాణం mev_sh_నమోదు *నమోదు_2;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  నమోదు_2 = mev_sh_నమోదు_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  ఒకవేళ(నమోదు_2) {
    నిర్మాణం Curl_easy *దత్తాంశం;
    uint32_t mid;

    ఒకవేళ(Curl_uint32_spbset_మొదటి(&నమోదు_2->xfers, &mid)) {
      చేయి {
        దత్తాంశం = Curl_multi_get_easy(multi, mid);
        ఒకవేళ(దత్తాంశం) {
          Curl_multi_mark_dirty(దత్తాంశం);
        }
        లేకపోతే {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&నమోదు_2->xfers, mid);
        }
      } అయితే(Curl_uint32_spbset_తదుపరి(&నమోదు_2->xfers, mid, &mid));
    }

    ఒకవేళ(నమోదు_2->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

శూన్య Curl_multi_ev_socket_పూర్తి(నిర్మాణం Curl_multi *multi,
                               నిర్మాణం Curl_easy *దత్తాంశం, curl_socket_t s)
{
  mev_forget_socket(multi, దత్తాంశం, s, "socket done");
}

శూన్య Curl_multi_ev_xfer_పూర్తి(నిర్మాణం Curl_multi *multi,
                             నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(!దత్తాంశం->conn); /* transfer should have been detached */
  (శూన్య)mev_assess(multi, దత్తాంశం, ఖాళీ);
  Curl_meta_remove(దత్తాంశం, CURL_META_MEV_POLLSET);
}

శూన్య Curl_multi_ev_conn_పూర్తి(నిర్మాణం Curl_multi *multi,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn)
{
  (శూన్య)mev_assess(multi, దత్తాంశం, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

శూన్య Curl_multi_ev_init(నిర్మాణం Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_నమోదు_dtor);
}

శూన్య Curl_multi_ev_cleanup(నిర్మాణం Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
