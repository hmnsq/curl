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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "multi_ev.h"
#شامل "select.h"
#شامل "uint-bset.h"
#شامل "uint-spbset.h"
#شامل "multihandle.h"

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو SH_اندراج_MAGIC 0x570091d
#اگر_ختم

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ساخت mev_sh_اندراج {
  ساخت uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ساخت connectdata *conn; /* connection using this socket or NULL */
  خالی *user_ڈیٹا;      /* libcurl app data via curl_multi_assign() */
  غیرعلامتی صحیح action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  غیرعلامتی صحیح readers; /* this many transfers want to read */
  غیرعلامتی صحیح writers; /* this many transfers want to write */
#اگر_تعریف_شدہ DEBUGBUILD
  غیرعلامتی صحیح magic;
#اگر_ختم
  ثنائی_ہندسہ(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ساکن خالی mev_sh_اندراج_dtor(خالی *freethis)
{
  ساخت mev_sh_اندراج *اندراج_2 = (ساخت mev_sh_اندراج *)freethis;
  Curl_uint32_spbset_destroy(&اندراج_2->xfers);
#اگر_تعریف_شدہ DEBUGBUILD
  اندراج_2->magic = 0;
#اگر_ختم
  curlx_free(اندراج_2);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ساکن ساخت mev_sh_اندراج *mev_sh_اندراج_get(ساخت Curl_hash *sh,
                                             curl_socket_t s)
{
  اگر(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    واپس Curl_hash_pick(sh, (ثابت حرف *)&s, حجم(curl_socket_t));
  }
  واپس تہی;
}

/* make sure this socket is present in the hash for this handle */
ساکن ساخت mev_sh_اندراج *mev_sh_اندراج_جوڑو(ساخت Curl_hash *sh,
                                             curl_socket_t s)
{
  ساخت mev_sh_اندراج *there = mev_sh_اندراج_get(sh, s);
  ساخت mev_sh_اندراج *check;

  اگر(there) {
    /* it is present, return fine */
    واپس there;
  }

  /* not present, add it */
  check = curlx_calloc(1, حجم(ساخت mev_sh_اندراج));
  اگر(!check)
    واپس تہی; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  اگر(!Curl_hash_جوڑو(sh, (حرف *)&s, حجم(curl_socket_t), check)) {
    mev_sh_اندراج_dtor(check);
    واپس تہی; /* major failure */
  }
#اگر_تعریف_شدہ DEBUGBUILD
  check->magic = SH_اندراج_MAGIC;
#اگر_ختم
  واپس check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ساکن خالی mev_sh_اندراج_kill(ساخت Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (حرف *)&s, حجم(curl_socket_t));
}

ساکن size_t mev_sh_اندراج_user_گنتی(ساخت mev_sh_اندراج *e)
{
  واپس Curl_uint32_spbset_گنتی(&e->xfers) + (e->conn ? 1 : 0);
}

ساکن bool mev_sh_اندراج_xfer_معلوم(ساخت mev_sh_اندراج *e,
                                    ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_uint32_spbset_شامل_ہے(&e->xfers, ڈیٹا->mid);
}

ساکن bool mev_sh_اندراج_conn_معلوم(ساخت mev_sh_اندراج *e,
                                    ساخت connectdata *conn)
{
  واپس (e->conn == conn);
}

ساکن خالی mev_pollset_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت easy_pollset *ps = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  اگر(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ساکن ساخت easy_pollset *mev_جوڑو_new_conn_pollset(ساخت connectdata *conn)
{
  ساخت easy_pollset *ps;

  ps = Curl_pollset_create();
  اگر(!ps)
    واپس تہی;
  اگر(Curl_conn_meta_سیٹ(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    واپس تہی;
  واپس ps;
}

ساکن ساخت easy_pollset *mev_جوڑو_new_xfer_pollset(ساخت Curl_easy *ڈیٹا)
{
  ساخت easy_pollset *ps;

  ps = Curl_pollset_create();
  اگر(!ps)
    واپس تہی;
  اگر(Curl_meta_سیٹ(ڈیٹا, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    واپس تہی;
  واپس ps;
}

ساکن ساخت easy_pollset *mev_get_آخری_pollset(ساخت Curl_easy *ڈیٹا,
                                                 ساخت connectdata *conn)
{
  اگر(ڈیٹا) {
    اگر(conn)
      واپس Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    واپس Curl_meta_get(ڈیٹا, CURL_META_MEV_POLLSET);
  }
  واپس تہی;
}

ساکن bool mev_sh_اندراج_xfer_جوڑو(ساخت mev_sh_اندراج *e,
                                  ساخت Curl_easy *ڈیٹا)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_اندراج_user_گنتی(e) < 100000);
  واپس Curl_uint32_spbset_جوڑو(&e->xfers, ڈیٹا->mid);
}

ساکن bool mev_sh_اندراج_conn_جوڑو(ساخت mev_sh_اندراج *e,
                                  ساخت connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_اندراج_user_گنتی(e) < 100000);
  DEBUGASSERT(!e->conn);
  اگر(e->conn)
    واپس FALSE;
  e->conn = conn;
  واپس TRUE;
}

ساکن bool mev_sh_اندراج_xfer_remove(ساخت mev_sh_اندراج *e,
                                     ساخت Curl_easy *ڈیٹا)
{
  bool present = Curl_uint32_spbset_شامل_ہے(&e->xfers, ڈیٹا->mid);
  اگر(present)
    Curl_uint32_spbset_remove(&e->xfers, ڈیٹا->mid);
  واپس present;
}

ساکن bool mev_sh_اندراج_conn_remove(ساخت mev_sh_اندراج *e,
                                     ساخت connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  اگر(e->conn == conn) {
    e->conn = تہی;
    واپس TRUE;
  }
  واپس FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ساکن CURLMcode mev_forget_socket(ساخت Curl_multi *multi,
                                   ساخت Curl_easy *ڈیٹا,
                                   curl_socket_t s,
                                   ثابت حرف *cause)
{
  ساخت mev_sh_اندراج *اندراج_2 = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  صحیح rc = 0;

  اگر(!اندراج_2) /* we never knew or already forgot about this socket */
    واپس CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  اگر(Curl_uint32_spbset_پہلا(&اندراج_2->xfers, &mid)) {
    کرو {
      ساخت Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      اگر(sdata) {
        ساخت easy_pollset *ps = mev_get_آخری_pollset(sdata, تہی);
        اگر(ps)
          Curl_pollset_remove(ps, s);
      }
    } جب(Curl_uint32_spbset_اگلا(&اندراج_2->xfers, mid, &mid));
  }

  اگر(اندراج_2->conn) {
    ساخت easy_pollset *ps = mev_get_آخری_pollset(ڈیٹا, اندراج_2->conn);
    اگر(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  اگر(اندراج_2->announced && multi->socket_cb) {
    ساخت Curl_mapi_guard guard;

    NOVERBOSE((خالی)cause);
    CURL_TRC_M(ڈیٹا, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_آغاز(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(ڈیٹا, s, CURL_POLL_REMOVE,
                          multi->socket_userp, اندراج_2->user_ڈیٹا);
    CURL_CBAPI_انتہا(&guard);
    اندراج_2 = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
    اگر(اندراج_2)
      اندراج_2->announced = FALSE;
  }

  mev_sh_اندراج_kill(multi, s);
  اگر(rc == -1) {
    multi->dead = TRUE;
    واپس CURLM_ABORTED_BY_CALLBACK;
  }
  واپس CURLM_OK;
}

ساکن CURLMcode mev_sh_اندراج_update(ساخت Curl_multi *multi,
                                     ساخت Curl_easy *ڈیٹا,
                                     ساخت mev_sh_اندراج *اندراج_2,
                                     curl_socket_t s,
                                     غیرعلامتی حرف آخری_action,
                                     غیرعلامتی حرف cur_action)
{
  صحیح rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(اندراج_2->magic == SH_اندراج_MAGIC);
  اگر(!multi->socket_cb)
    واپس CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  اگر(آخری_action == cur_action)  /* nothing from `data` changed */
    واپس CURLM_OK;

  اگر(آخری_action & CURL_POLL_اندر) {
    DEBUGASSERT(اندراج_2->readers);
    اگر(!(cur_action & CURL_POLL_اندر))
      اندراج_2->readers--;
  }
  ورنہ اگر(cur_action & CURL_POLL_اندر)
    اندراج_2->readers++;

  اگر(آخری_action & CURL_POLL_برآمد) {
    DEBUGASSERT(اندراج_2->writers);
    اگر(!(cur_action & CURL_POLL_برآمد))
      اندراج_2->writers--;
  }
  ورنہ اگر(cur_action & CURL_POLL_برآمد)
    اندراج_2->writers++;

  DEBUGASSERT(اندراج_2->readers <= mev_sh_اندراج_user_گنتی(اندراج_2));
  DEBUGASSERT(اندراج_2->writers <= mev_sh_اندراج_user_گنتی(اندراج_2));
  DEBUGASSERT(اندراج_2->writers + اندراج_2->readers);

  CURL_TRC_M(ڈیٹا, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (آخری_action & CURL_POLL_اندر) ? "IN" : "",
             (آخری_action & CURL_POLL_برآمد) ? "OUT" : "",
             (cur_action & CURL_POLL_اندر) ? "IN" : "",
             (cur_action & CURL_POLL_برآمد) ? "OUT" : "",
             اندراج_2->readers, اندراج_2->writers);

  comboaction = (اندراج_2->writers ? CURL_POLL_برآمد : 0) |
                (اندراج_2->readers ? CURL_POLL_اندر : 0);
  اگر(((صحیح)اندراج_2->action == comboaction)) /* nothing for socket changed */
    واپس CURLM_OK;

  CURL_TRC_M(ڈیٹا, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_اندر) ? "IN" : "",
             (comboaction & CURL_POLL_برآمد) ? "OUT" : "");
  {
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_آغاز(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(ڈیٹا, s, comboaction, multi->socket_userp,
                          اندراج_2->user_ڈیٹا);
    CURL_CBAPI_MULTI_انتہا(&guard);
  }
  اگر(rc == -1) {
    multi->dead = TRUE;
    واپس CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  اندراج_2 = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
  اگر(اندراج_2) {
    DEBUGASSERT(اندراج_2->magic == SH_اندراج_MAGIC);
    اندراج_2->announced = TRUE;
    اندراج_2->action = (غیرعلامتی صحیح)comboaction;
  }
  واپس CURLM_OK;
}

ساکن CURLMcode mev_pollset_diff(ساخت Curl_multi *multi,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  ساخت easy_pollset *ps,
                                  ساخت easy_pollset *پچھلا_ps)
{
  ساخت mev_sh_اندراج *اندراج_2;
  curl_socket_t s;
  غیرعلامتی صحیح i, j;
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
  DEBUGASSERT(پچھلا_ps);

  /* Handle changes to sockets the transfer is interested in. */
  برائے(i = 0; i < ps->n; i++) {
    غیرعلامتی حرف آخری_action;
    bool پہلا_وقت = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    اندراج_2 = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
    اگر(!اندراج_2) {
      /* new socket, add new entry */
      پہلا_وقت = TRUE;
      اندراج_2 = mev_sh_اندراج_جوڑو(&multi->ev.sh_entries, s);
      اگر(!اندراج_2) /* fatal */
        واپس CURLM_برآمد_کا_حافظہ;
      CURL_TRC_M(ڈیٹا, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    ورنہ اگر(conn) {
      پہلا_وقت = !mev_sh_اندراج_conn_معلوم(اندراج_2, conn);
    }
    ورنہ {
      پہلا_وقت = !mev_sh_اندراج_xfer_معلوم(اندراج_2, ڈیٹا);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    آخری_action = 0;
    اگر(پہلا_وقت) {
      اگر(conn) {
        اگر(!mev_sh_اندراج_conn_جوڑو(اندراج_2, conn))
          واپس CURLM_برآمد_کا_حافظہ;
      }
      ورنہ {
        اگر(!mev_sh_اندراج_xfer_جوڑو(اندراج_2, ڈیٹا))
          واپس CURLM_برآمد_کا_حافظہ;
      }
      CURL_TRC_M(ڈیٹا, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : ڈیٹا->mid,
                 Curl_uint32_spbset_گنتی(&اندراج_2->xfers),
                 اندراج_2->conn ? 1 : 0);
    }
    ورنہ {
      برائے(j = 0; j < پچھلا_ps->n; j++) {
        اگر(s == پچھلا_ps->sockets[j]) {
          آخری_action = پچھلا_ps->actions[j];
          توڑو;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_اندراج_update(multi, ڈیٹا, اندراج_2, s,
                                  آخری_action, ps->actions[i]);
    اگر(mresult)
      واپس mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  برائے(i = 0; i < پچھلا_ps->n; i++) {
    bool stillused = FALSE;

    s = پچھلا_ps->sockets[i];
    برائے(j = 0; j < ps->n; j++) {
      اگر(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        توڑو;
      }
    }
    اگر(stillused)
      جاری;

    اندراج_2 = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    اگر(!اندراج_2)
      جاری;

    اگر(conn && !mev_sh_اندراج_conn_remove(اندراج_2, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(ڈیٹا, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(تہی);
      جاری;
    }

    اگر(!conn && !mev_sh_اندراج_xfer_remove(اندراج_2, ڈیٹا)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(ڈیٹا, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(تہی);
      جاری;
    }

    اگر(mev_sh_اندراج_user_گنتی(اندراج_2)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_اندراج_update(multi, ڈیٹا, اندراج_2, s,
                                    پچھلا_ps->actions[i], 0);
      اگر(mresult)
        واپس mresult;
      CURL_TRC_M(ڈیٹا, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_گنتی(&اندراج_2->xfers),
                 اندراج_2->conn ? 1 : 0);
    }
    ورنہ {
      mresult = mev_forget_socket(multi, ڈیٹا, s, "last user gone");
      اگر(mresult)
        واپس mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(پچھلا_ps, ps);
  واپس CURLM_OK;
}

ساکن CURLMcode mev_assess(ساخت Curl_multi *multi,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت connectdata *conn)
{
  ساخت easy_pollset ps, *آخری_ps;
  CURLMcode mresult = CURLM_OK;

  اگر(!multi || !multi->socket_cb)
    واپس CURLM_OK;

  Curl_pollset_init(&ps);
  اگر(conn) {
    CURLcode نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, conn, &ps);
    اگر(نتیجہ) {
      mresult = (نتیجہ == CURLE_برآمد_کا_حافظہ) ?
        CURLM_برآمد_کا_حافظہ : CURLM_اندرونی_ERROR;
      جاؤ برآمد;
    }
  }
  ورنہ
    Curl_multi_pollset(ڈیٹا, &ps);
  آخری_ps = mev_get_آخری_pollset(ڈیٹا, conn);

  اگر(!آخری_ps && ps.n) {
    اگر(conn)
      آخری_ps = mev_جوڑو_new_conn_pollset(conn);
    ورنہ
      آخری_ps = mev_جوڑو_new_xfer_pollset(ڈیٹا);
    اگر(!آخری_ps) {
      mresult = CURLM_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  اگر(آخری_ps)
    mresult = mev_pollset_diff(multi, ڈیٹا, conn, &ps, آخری_ps);
  ورنہ
    DEBUGASSERT(!ps.n);
برآمد:
  Curl_pollset_cleanup(&ps);
  واپس mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا)
{
  واپس mev_assess(multi, ڈیٹا, تہی);
}

CURLMcode Curl_multi_ev_assess_conn(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn)
{
  واپس mev_assess(multi, ڈیٹا, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ساخت Curl_multi *multi,
                                         ساخت uint32_bset *سیٹ)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  اگر(multi && multi->socket_cb && Curl_uint32_bset_پہلا(سیٹ, &mid)) {
    کرو {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(ڈیٹا) {
        mresult = Curl_multi_ev_assess_xfer(multi, ڈیٹا);
      }
    } جب(!mresult && Curl_uint32_bset_اگلا(سیٹ, mid, &mid));
  }
  واپس mresult;
}

CURLMcode Curl_multi_ev_تفویض(ساخت Curl_multi *multi,
                               curl_socket_t s,
                               خالی *user_ڈیٹا)
{
  ساخت mev_sh_اندراج *e = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
  اگر(!e)
    واپس CURLM_BAD_SOCKET;
  e->user_ڈیٹا = user_ڈیٹا;
  واپس CURLM_OK;
}

خالی Curl_multi_ev_dirty_xfers(ساخت Curl_multi *multi,
                               curl_socket_t s)
{
  ساخت mev_sh_اندراج *اندراج_2;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  اندراج_2 = mev_sh_اندراج_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  اگر(اندراج_2) {
    ساخت Curl_easy *ڈیٹا;
    uint32_t mid;

    اگر(Curl_uint32_spbset_پہلا(&اندراج_2->xfers, &mid)) {
      کرو {
        ڈیٹا = Curl_multi_get_easy(multi, mid);
        اگر(ڈیٹا) {
          Curl_multi_mark_dirty(ڈیٹا);
        }
        ورنہ {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&اندراج_2->xfers, mid);
        }
      } جب(Curl_uint32_spbset_اگلا(&اندراج_2->xfers, mid, &mid));
    }

    اگر(اندراج_2->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

خالی Curl_multi_ev_socket_مکمل(ساخت Curl_multi *multi,
                               ساخت Curl_easy *ڈیٹا, curl_socket_t s)
{
  mev_forget_socket(multi, ڈیٹا, s, "socket done");
}

خالی Curl_multi_ev_xfer_مکمل(ساخت Curl_multi *multi,
                             ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(!ڈیٹا->conn); /* transfer should have been detached */
  (خالی)mev_assess(multi, ڈیٹا, تہی);
  Curl_meta_remove(ڈیٹا, CURL_META_MEV_POLLSET);
}

خالی Curl_multi_ev_conn_مکمل(ساخت Curl_multi *multi,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn)
{
  (خالی)mev_assess(multi, ڈیٹا, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

خالی Curl_multi_ev_init(ساخت Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_اندراج_dtor);
}

خالی Curl_multi_ev_cleanup(ساخت Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
