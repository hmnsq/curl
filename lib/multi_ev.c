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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "multi_ev.h"
#ضمن "select.h"
#ضمن "uint-bset.h"
#ضمن "uint-spbset.h"
#ضمن "multihandle.h"

#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو SH_اندراج_MAGIC 0x570091d
#جے_انت

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
بنية mev_sh_اندراج {
  بنية uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  بنية connectdata *conn; /* connection using this socket or NULL */
  فراغ *user_انکڑے;      /* libcurl app data via curl_multi_assign() */
  بنانشانی صحيح action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  بنانشانی صحيح readers; /* this many transfers want to read */
  بنانشانی صحيح writers; /* this many transfers want to write */
#جے_تعریف_شدہ DEBUGBUILD
  بنانشانی صحيح magic;
#جے_انت
  ثنائی_ہندسہ(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ساكن فراغ mev_sh_اندراج_dtor(فراغ *freethis)
{
  بنية mev_sh_اندراج *اندراج = (بنية mev_sh_اندراج *)freethis;
  Curl_uint32_spbset_destroy(&اندراج->xfers);
#جے_تعریف_شدہ DEBUGBUILD
  اندراج->magic = 0;
#جے_انت
  curlx_free(اندراج);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ساكن بنية mev_sh_اندراج *mev_sh_اندراج_get(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  إذا(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    أرجع Curl_hash_pick(sh, (ثابت حرف *)&s, حجم(curl_socket_t));
  }
  أرجع عدم;
}

/* make sure this socket is present in the hash for this handle */
ساكن بنية mev_sh_اندراج *mev_sh_اندراج_جوڑو(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  بنية mev_sh_اندراج *there = mev_sh_اندراج_get(sh, s);
  بنية mev_sh_اندراج *check;

  إذا(there) {
    /* it is present, return fine */
    أرجع there;
  }

  /* not present, add it */
  check = curlx_calloc(1, حجم(بنية mev_sh_اندراج));
  إذا(!check)
    أرجع عدم; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  إذا(!Curl_hash_جوڑو(sh, (حرف *)&s, حجم(curl_socket_t), check)) {
    mev_sh_اندراج_dtor(check);
    أرجع عدم; /* major failure */
  }
#جے_تعریف_شدہ DEBUGBUILD
  check->magic = SH_اندراج_MAGIC;
#جے_انت
  أرجع check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ساكن فراغ mev_sh_اندراج_kill(بنية Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (حرف *)&s, حجم(curl_socket_t));
}

ساكن size_t mev_sh_اندراج_user_گنتی(بنية mev_sh_اندراج *e)
{
  أرجع Curl_uint32_spbset_گنتی(&e->xfers) + (e->conn ? 1 : 0);
}

ساكن bool mev_sh_اندراج_xfer_معلوم(بنية mev_sh_اندراج *e,
                                    بنية Curl_easy *انکڑے)
{
  أرجع Curl_uint32_spbset_شامل_اے(&e->xfers, انکڑے->mid);
}

ساكن bool mev_sh_اندراج_conn_معلوم(بنية mev_sh_اندراج *e,
                                    بنية connectdata *conn)
{
  أرجع (e->conn == conn);
}

ساكن فراغ mev_pollset_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية easy_pollset *ps = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  إذا(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ساكن بنية easy_pollset *mev_جوڑو_new_conn_pollset(بنية connectdata *conn)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  إذا(!ps)
    أرجع عدم;
  إذا(Curl_conn_meta_سیٹ(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    أرجع عدم;
  أرجع ps;
}

ساكن بنية easy_pollset *mev_جوڑو_new_xfer_pollset(بنية Curl_easy *انکڑے)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  إذا(!ps)
    أرجع عدم;
  إذا(Curl_meta_سیٹ(انکڑے, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    أرجع عدم;
  أرجع ps;
}

ساكن بنية easy_pollset *mev_get_آخری_pollset(بنية Curl_easy *انکڑے,
                                                 بنية connectdata *conn)
{
  إذا(انکڑے) {
    إذا(conn)
      أرجع Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    أرجع Curl_meta_get(انکڑے, CURL_META_MEV_POLLSET);
  }
  أرجع عدم;
}

ساكن bool mev_sh_اندراج_xfer_جوڑو(بنية mev_sh_اندراج *e,
                                  بنية Curl_easy *انکڑے)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_اندراج_user_گنتی(e) < 100000);
  أرجع Curl_uint32_spbset_جوڑو(&e->xfers, انکڑے->mid);
}

ساكن bool mev_sh_اندراج_conn_جوڑو(بنية mev_sh_اندراج *e,
                                  بنية connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_اندراج_user_گنتی(e) < 100000);
  DEBUGASSERT(!e->conn);
  إذا(e->conn)
    أرجع FALSE;
  e->conn = conn;
  أرجع TRUE;
}

ساكن bool mev_sh_اندراج_xfer_remove(بنية mev_sh_اندراج *e,
                                     بنية Curl_easy *انکڑے)
{
  bool present = Curl_uint32_spbset_شامل_اے(&e->xfers, انکڑے->mid);
  إذا(present)
    Curl_uint32_spbset_remove(&e->xfers, انکڑے->mid);
  أرجع present;
}

ساكن bool mev_sh_اندراج_conn_remove(بنية mev_sh_اندراج *e,
                                     بنية connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  إذا(e->conn == conn) {
    e->conn = عدم;
    أرجع TRUE;
  }
  أرجع FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ساكن CURLMcode mev_forget_socket(بنية Curl_multi *multi,
                                   بنية Curl_easy *انکڑے,
                                   curl_socket_t s,
                                   ثابت حرف *cause)
{
  بنية mev_sh_اندراج *اندراج = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  صحيح rc = 0;

  إذا(!اندراج) /* we never knew or already forgot about this socket */
    أرجع CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  إذا(Curl_uint32_spbset_پہلا(&اندراج->xfers, &mid)) {
    افعل {
      بنية Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      إذا(sdata) {
        بنية easy_pollset *ps = mev_get_آخری_pollset(sdata, عدم);
        إذا(ps)
          Curl_pollset_remove(ps, s);
      }
    } طالما(Curl_uint32_spbset_اگلا(&اندراج->xfers, mid, &mid));
  }

  إذا(اندراج->conn) {
    بنية easy_pollset *ps = mev_get_آخری_pollset(انکڑے, اندراج->conn);
    إذا(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  إذا(اندراج->announced && multi->socket_cb) {
    بنية Curl_mapi_guard guard;

    NOVERBOSE((فراغ)cause);
    CURL_TRC_M(انکڑے, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_شروع(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(انکڑے, s, CURL_POLL_REMOVE,
                          multi->socket_userp, اندراج->user_انکڑے);
    CURL_CBAPI_سرا(&guard);
    اندراج = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
    إذا(اندراج)
      اندراج->announced = FALSE;
  }

  mev_sh_اندراج_kill(multi, s);
  إذا(rc == -1) {
    multi->dead = TRUE;
    أرجع CURLM_ABORTED_BY_CALLBACK;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_sh_اندراج_update(بنية Curl_multi *multi,
                                     بنية Curl_easy *انکڑے,
                                     بنية mev_sh_اندراج *اندراج,
                                     curl_socket_t s,
                                     بنانشانی حرف آخری_action,
                                     بنانشانی حرف cur_action)
{
  صحيح rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(اندراج->magic == SH_اندراج_MAGIC);
  إذا(!multi->socket_cb)
    أرجع CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  إذا(آخری_action == cur_action)  /* nothing from `data` changed */
    أرجع CURLM_OK;

  إذا(آخری_action & CURL_POLL_وچ) {
    DEBUGASSERT(اندراج->readers);
    إذا(!(cur_action & CURL_POLL_وچ))
      اندراج->readers--;
  }
  وإلا إذا(cur_action & CURL_POLL_وچ)
    اندراج->readers++;

  إذا(آخری_action & CURL_POLL_باہرلا) {
    DEBUGASSERT(اندراج->writers);
    إذا(!(cur_action & CURL_POLL_باہرلا))
      اندراج->writers--;
  }
  وإلا إذا(cur_action & CURL_POLL_باہرلا)
    اندراج->writers++;

  DEBUGASSERT(اندراج->readers <= mev_sh_اندراج_user_گنتی(اندراج));
  DEBUGASSERT(اندراج->writers <= mev_sh_اندراج_user_گنتی(اندراج));
  DEBUGASSERT(اندراج->writers + اندراج->readers);

  CURL_TRC_M(انکڑے, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (آخری_action & CURL_POLL_وچ) ? "IN" : "",
             (آخری_action & CURL_POLL_باہرلا) ? "OUT" : "",
             (cur_action & CURL_POLL_وچ) ? "IN" : "",
             (cur_action & CURL_POLL_باہرلا) ? "OUT" : "",
             اندراج->readers, اندراج->writers);

  comboaction = (اندراج->writers ? CURL_POLL_باہرلا : 0) |
                (اندراج->readers ? CURL_POLL_وچ : 0);
  إذا(((صحيح)اندراج->action == comboaction)) /* nothing for socket changed */
    أرجع CURLM_OK;

  CURL_TRC_M(انکڑے, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_وچ) ? "IN" : "",
             (comboaction & CURL_POLL_باہرلا) ? "OUT" : "");
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_شروع(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(انکڑے, s, comboaction, multi->socket_userp,
                          اندراج->user_انکڑے);
    CURL_CBAPI_MULTI_سرا(&guard);
  }
  إذا(rc == -1) {
    multi->dead = TRUE;
    أرجع CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  اندراج = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
  إذا(اندراج) {
    DEBUGASSERT(اندراج->magic == SH_اندراج_MAGIC);
    اندراج->announced = TRUE;
    اندراج->action = (بنانشانی صحيح)comboaction;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_pollset_diff(بنية Curl_multi *multi,
                                  بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps,
                                  بنية easy_pollset *prev_ps)
{
  بنية mev_sh_اندراج *اندراج;
  curl_socket_t s;
  بنانشانی صحيح i, j;
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
  لكل(i = 0; i < ps->n; i++) {
    بنانشانی حرف آخری_action;
    bool پہلا_ویلا = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    اندراج = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
    إذا(!اندراج) {
      /* new socket, add new entry */
      پہلا_ویلا = TRUE;
      اندراج = mev_sh_اندراج_جوڑو(&multi->ev.sh_entries, s);
      إذا(!اندراج) /* fatal */
        أرجع CURLM_باہرلا_دا_حافظہ;
      CURL_TRC_M(انکڑے, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    وإلا إذا(conn) {
      پہلا_ویلا = !mev_sh_اندراج_conn_معلوم(اندراج, conn);
    }
    وإلا {
      پہلا_ویلا = !mev_sh_اندراج_xfer_معلوم(اندراج, انکڑے);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    آخری_action = 0;
    إذا(پہلا_ویلا) {
      إذا(conn) {
        إذا(!mev_sh_اندراج_conn_جوڑو(اندراج, conn))
          أرجع CURLM_باہرلا_دا_حافظہ;
      }
      وإلا {
        إذا(!mev_sh_اندراج_xfer_جوڑو(اندراج, انکڑے))
          أرجع CURLM_باہرلا_دا_حافظہ;
      }
      CURL_TRC_M(انکڑے, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : انکڑے->mid,
                 Curl_uint32_spbset_گنتی(&اندراج->xfers),
                 اندراج->conn ? 1 : 0);
    }
    وإلا {
      لكل(j = 0; j < prev_ps->n; j++) {
        إذا(s == prev_ps->sockets[j]) {
          آخری_action = prev_ps->actions[j];
          اكسر;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_اندراج_update(multi, انکڑے, اندراج, s,
                                  آخری_action, ps->actions[i]);
    إذا(mresult)
      أرجع mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  لكل(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    لكل(j = 0; j < ps->n; j++) {
      إذا(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        اكسر;
      }
    }
    إذا(stillused)
      استمر;

    اندراج = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    إذا(!اندراج)
      استمر;

    إذا(conn && !mev_sh_اندراج_conn_remove(اندراج, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(انکڑے, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    إذا(!conn && !mev_sh_اندراج_xfer_remove(اندراج, انکڑے)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(انکڑے, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    إذا(mev_sh_اندراج_user_گنتی(اندراج)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_اندراج_update(multi, انکڑے, اندراج, s,
                                    prev_ps->actions[i], 0);
      إذا(mresult)
        أرجع mresult;
      CURL_TRC_M(انکڑے, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_گنتی(&اندراج->xfers),
                 اندراج->conn ? 1 : 0);
    }
    وإلا {
      mresult = mev_forget_socket(multi, انکڑے, s, "last user gone");
      إذا(mresult)
        أرجع mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_assess(بنية Curl_multi *multi,
                            بنية Curl_easy *انکڑے,
                            بنية connectdata *conn)
{
  بنية easy_pollset ps, *آخری_ps;
  CURLMcode mresult = CURLM_OK;

  إذا(!multi || !multi->socket_cb)
    أرجع CURLM_OK;

  Curl_pollset_init(&ps);
  إذا(conn) {
    CURLcode نتیجہ = Curl_conn_adjust_pollset(انکڑے, conn, &ps);
    إذا(نتیجہ) {
      mresult = (نتیجہ == CURLE_باہرلا_دا_حافظہ) ?
        CURLM_باہرلا_دا_حافظہ : CURLM_اندرونی_ERROR;
      اذهب باہرلا;
    }
  }
  وإلا
    Curl_multi_pollset(انکڑے, &ps);
  آخری_ps = mev_get_آخری_pollset(انکڑے, conn);

  إذا(!آخری_ps && ps.n) {
    إذا(conn)
      آخری_ps = mev_جوڑو_new_conn_pollset(conn);
    وإلا
      آخری_ps = mev_جوڑو_new_xfer_pollset(انکڑے);
    إذا(!آخری_ps) {
      mresult = CURLM_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  إذا(آخری_ps)
    mresult = mev_pollset_diff(multi, انکڑے, conn, &ps, آخری_ps);
  وإلا
    DEBUGASSERT(!ps.n);
باہرلا:
  Curl_pollset_cleanup(&ps);
  أرجع mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(بنية Curl_multi *multi,
                                    بنية Curl_easy *انکڑے)
{
  أرجع mev_assess(multi, انکڑے, عدم);
}

CURLMcode Curl_multi_ev_assess_conn(بنية Curl_multi *multi,
                                    بنية Curl_easy *انکڑے,
                                    بنية connectdata *conn)
{
  أرجع mev_assess(multi, انکڑے, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(بنية Curl_multi *multi,
                                         بنية uint32_bset *سیٹ)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  إذا(multi && multi->socket_cb && Curl_uint32_bset_پہلا(سیٹ, &mid)) {
    افعل {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(انکڑے) {
        mresult = Curl_multi_ev_assess_xfer(multi, انکڑے);
      }
    } طالما(!mresult && Curl_uint32_bset_اگلا(سیٹ, mid, &mid));
  }
  أرجع mresult;
}

CURLMcode Curl_multi_ev_سونپو(بنية Curl_multi *multi,
                               curl_socket_t s,
                               فراغ *user_انکڑے)
{
  بنية mev_sh_اندراج *e = mev_sh_اندراج_get(&multi->ev.sh_entries, s);
  إذا(!e)
    أرجع CURLM_BAD_SOCKET;
  e->user_انکڑے = user_انکڑے;
  أرجع CURLM_OK;
}

فراغ Curl_multi_ev_dirty_xfers(بنية Curl_multi *multi,
                               curl_socket_t s)
{
  بنية mev_sh_اندراج *اندراج;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  اندراج = mev_sh_اندراج_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  إذا(اندراج) {
    بنية Curl_easy *انکڑے;
    uint32_t mid;

    إذا(Curl_uint32_spbset_پہلا(&اندراج->xfers, &mid)) {
      افعل {
        انکڑے = Curl_multi_get_easy(multi, mid);
        إذا(انکڑے) {
          Curl_multi_mark_dirty(انکڑے);
        }
        وإلا {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&اندراج->xfers, mid);
        }
      } طالما(Curl_uint32_spbset_اگلا(&اندراج->xfers, mid, &mid));
    }

    إذا(اندراج->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

فراغ Curl_multi_ev_socket_پورا(بنية Curl_multi *multi,
                               بنية Curl_easy *انکڑے, curl_socket_t s)
{
  mev_forget_socket(multi, انکڑے, s, "socket done");
}

فراغ Curl_multi_ev_xfer_پورا(بنية Curl_multi *multi,
                             بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(!انکڑے->conn); /* transfer should have been detached */
  (فراغ)mev_assess(multi, انکڑے, عدم);
  Curl_meta_remove(انکڑے, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_conn_پورا(بنية Curl_multi *multi,
                             بنية Curl_easy *انکڑے,
                             بنية connectdata *conn)
{
  (فراغ)mev_assess(multi, انکڑے, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_init(بنية Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_اندراج_dtor);
}

فراغ Curl_multi_ev_cleanup(بنية Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
