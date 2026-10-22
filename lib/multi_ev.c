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

#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن SH_مدخل_MAGIC 0x570091d
#پایان_اگر

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
بنية mev_sh_مدخل {
  بنية uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  بنية connectdata *conn; /* connection using this socket or NULL */
  فراغ *user_داده;      /* libcurl app data via curl_multi_assign() */
  بیعلامت صحيح action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  بیعلامت صحيح readers; /* this many transfers want to read */
  بیعلامت صحيح writers; /* this many transfers want to write */
#اگر_تعریف_شده DEBUGBUILD
  بیعلامت صحيح magic;
#پایان_اگر
  بیت(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ساكن فراغ mev_sh_مدخل_dtor(فراغ *freethis)
{
  بنية mev_sh_مدخل *مدخل = (بنية mev_sh_مدخل *)freethis;
  Curl_uint32_spbset_destroy(&مدخل->xfers);
#اگر_تعریف_شده DEBUGBUILD
  مدخل->magic = 0;
#پایان_اگر
  curlx_free(مدخل);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ساكن بنية mev_sh_مدخل *mev_sh_مدخل_get(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  إذا(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    أرجع Curl_hash_pick(sh, (ثابت حرف *)&s, حجم(curl_socket_t));
  }
  أرجع عدم;
}

/* make sure this socket is present in the hash for this handle */
ساكن بنية mev_sh_مدخل *mev_sh_مدخل_جمع_کن(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  بنية mev_sh_مدخل *there = mev_sh_مدخل_get(sh, s);
  بنية mev_sh_مدخل *check;

  إذا(there) {
    /* it is present, return fine */
    أرجع there;
  }

  /* not present, add it */
  check = curlx_calloc(1, حجم(بنية mev_sh_مدخل));
  إذا(!check)
    أرجع عدم; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  إذا(!Curl_hash_جمع_کن(sh, (حرف *)&s, حجم(curl_socket_t), check)) {
    mev_sh_مدخل_dtor(check);
    أرجع عدم; /* major failure */
  }
#اگر_تعریف_شده DEBUGBUILD
  check->magic = SH_مدخل_MAGIC;
#پایان_اگر
  أرجع check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ساكن فراغ mev_sh_مدخل_kill(بنية Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (حرف *)&s, حجم(curl_socket_t));
}

ساكن size_t mev_sh_مدخل_user_شمار(بنية mev_sh_مدخل *e)
{
  أرجع Curl_uint32_spbset_شمار(&e->xfers) + (e->conn ? 1 : 0);
}

ساكن bool mev_sh_مدخل_xfer_معلوم(بنية mev_sh_مدخل *e,
                                    بنية Curl_easy *داده)
{
  أرجع Curl_uint32_spbset_شامل_است(&e->xfers, داده->mid);
}

ساكن bool mev_sh_مدخل_conn_معلوم(بنية mev_sh_مدخل *e,
                                    بنية connectdata *conn)
{
  أرجع (e->conn == conn);
}

ساكن فراغ mev_pollset_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية easy_pollset *ps = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  إذا(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ساكن بنية easy_pollset *mev_جمع_کن_new_conn_pollset(بنية connectdata *conn)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  إذا(!ps)
    أرجع عدم;
  إذا(Curl_conn_meta_مجموعه(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    أرجع عدم;
  أرجع ps;
}

ساكن بنية easy_pollset *mev_جمع_کن_new_xfer_pollset(بنية Curl_easy *داده)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  إذا(!ps)
    أرجع عدم;
  إذا(Curl_meta_مجموعه(داده, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    أرجع عدم;
  أرجع ps;
}

ساكن بنية easy_pollset *mev_get_آخرین_pollset(بنية Curl_easy *داده,
                                                 بنية connectdata *conn)
{
  إذا(داده) {
    إذا(conn)
      أرجع Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    أرجع Curl_meta_get(داده, CURL_META_MEV_POLLSET);
  }
  أرجع عدم;
}

ساكن bool mev_sh_مدخل_xfer_جمع_کن(بنية mev_sh_مدخل *e,
                                  بنية Curl_easy *داده)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_مدخل_user_شمار(e) < 100000);
  أرجع Curl_uint32_spbset_جمع_کن(&e->xfers, داده->mid);
}

ساكن bool mev_sh_مدخل_conn_جمع_کن(بنية mev_sh_مدخل *e,
                                  بنية connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_مدخل_user_شمار(e) < 100000);
  DEBUGASSERT(!e->conn);
  إذا(e->conn)
    أرجع FALSE;
  e->conn = conn;
  أرجع TRUE;
}

ساكن bool mev_sh_مدخل_xfer_remove(بنية mev_sh_مدخل *e,
                                     بنية Curl_easy *داده)
{
  bool present = Curl_uint32_spbset_شامل_است(&e->xfers, داده->mid);
  إذا(present)
    Curl_uint32_spbset_remove(&e->xfers, داده->mid);
  أرجع present;
}

ساكن bool mev_sh_مدخل_conn_remove(بنية mev_sh_مدخل *e,
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
                                   بنية Curl_easy *داده,
                                   curl_socket_t s,
                                   ثابت حرف *cause)
{
  بنية mev_sh_مدخل *مدخل = mev_sh_مدخل_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  صحيح rc = 0;

  إذا(!مدخل) /* we never knew or already forgot about this socket */
    أرجع CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  إذا(Curl_uint32_spbset_اولی(&مدخل->xfers, &mid)) {
    افعل {
      بنية Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      إذا(sdata) {
        بنية easy_pollset *ps = mev_get_آخرین_pollset(sdata, عدم);
        إذا(ps)
          Curl_pollset_remove(ps, s);
      }
    } طالما(Curl_uint32_spbset_بعدی(&مدخل->xfers, mid, &mid));
  }

  إذا(مدخل->conn) {
    بنية easy_pollset *ps = mev_get_آخرین_pollset(داده, مدخل->conn);
    إذا(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  إذا(مدخل->announced && multi->socket_cb) {
    بنية Curl_mapi_guard guard;

    NOVERBOSE((فراغ)cause);
    CURL_TRC_M(داده, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_شروع(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(داده, s, CURL_POLL_REMOVE,
                          multi->socket_userp, مدخل->user_داده);
    CURL_CBAPI_انتها(&guard);
    مدخل = mev_sh_مدخل_get(&multi->ev.sh_entries, s);
    إذا(مدخل)
      مدخل->announced = FALSE;
  }

  mev_sh_مدخل_kill(multi, s);
  إذا(rc == -1) {
    multi->dead = TRUE;
    أرجع CURLM_ABORTED_BY_CALLBACK;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_sh_مدخل_update(بنية Curl_multi *multi,
                                     بنية Curl_easy *داده,
                                     بنية mev_sh_مدخل *مدخل,
                                     curl_socket_t s,
                                     بیعلامت حرف آخرین_action,
                                     بیعلامت حرف cur_action)
{
  صحيح rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(مدخل->magic == SH_مدخل_MAGIC);
  إذا(!multi->socket_cb)
    أرجع CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  إذا(آخرین_action == cur_action)  /* nothing from `data` changed */
    أرجع CURLM_OK;

  إذا(آخرین_action & CURL_POLL_درون) {
    DEBUGASSERT(مدخل->readers);
    إذا(!(cur_action & CURL_POLL_درون))
      مدخل->readers--;
  }
  وإلا إذا(cur_action & CURL_POLL_درون)
    مدخل->readers++;

  إذا(آخرین_action & CURL_POLL_بیرون) {
    DEBUGASSERT(مدخل->writers);
    إذا(!(cur_action & CURL_POLL_بیرون))
      مدخل->writers--;
  }
  وإلا إذا(cur_action & CURL_POLL_بیرون)
    مدخل->writers++;

  DEBUGASSERT(مدخل->readers <= mev_sh_مدخل_user_شمار(مدخل));
  DEBUGASSERT(مدخل->writers <= mev_sh_مدخل_user_شمار(مدخل));
  DEBUGASSERT(مدخل->writers + مدخل->readers);

  CURL_TRC_M(داده, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (آخرین_action & CURL_POLL_درون) ? "IN" : "",
             (آخرین_action & CURL_POLL_بیرون) ? "OUT" : "",
             (cur_action & CURL_POLL_درون) ? "IN" : "",
             (cur_action & CURL_POLL_بیرون) ? "OUT" : "",
             مدخل->readers, مدخل->writers);

  comboaction = (مدخل->writers ? CURL_POLL_بیرون : 0) |
                (مدخل->readers ? CURL_POLL_درون : 0);
  إذا(((صحيح)مدخل->action == comboaction)) /* nothing for socket changed */
    أرجع CURLM_OK;

  CURL_TRC_M(داده, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_درون) ? "IN" : "",
             (comboaction & CURL_POLL_بیرون) ? "OUT" : "");
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_شروع(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(داده, s, comboaction, multi->socket_userp,
                          مدخل->user_داده);
    CURL_CBAPI_MULTI_انتها(&guard);
  }
  إذا(rc == -1) {
    multi->dead = TRUE;
    أرجع CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  مدخل = mev_sh_مدخل_get(&multi->ev.sh_entries, s);
  إذا(مدخل) {
    DEBUGASSERT(مدخل->magic == SH_مدخل_MAGIC);
    مدخل->announced = TRUE;
    مدخل->action = (بیعلامت صحيح)comboaction;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_pollset_diff(بنية Curl_multi *multi,
                                  بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps,
                                  بنية easy_pollset *prev_ps)
{
  بنية mev_sh_مدخل *مدخل;
  curl_socket_t s;
  بیعلامت صحيح i, j;
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
    بیعلامت حرف آخرین_action;
    bool اولی_زمان = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    مدخل = mev_sh_مدخل_get(&multi->ev.sh_entries, s);
    إذا(!مدخل) {
      /* new socket, add new entry */
      اولی_زمان = TRUE;
      مدخل = mev_sh_مدخل_جمع_کن(&multi->ev.sh_entries, s);
      إذا(!مدخل) /* fatal */
        أرجع CURLM_بیرون_از_حافظه;
      CURL_TRC_M(داده, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    وإلا إذا(conn) {
      اولی_زمان = !mev_sh_مدخل_conn_معلوم(مدخل, conn);
    }
    وإلا {
      اولی_زمان = !mev_sh_مدخل_xfer_معلوم(مدخل, داده);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    آخرین_action = 0;
    إذا(اولی_زمان) {
      إذا(conn) {
        إذا(!mev_sh_مدخل_conn_جمع_کن(مدخل, conn))
          أرجع CURLM_بیرون_از_حافظه;
      }
      وإلا {
        إذا(!mev_sh_مدخل_xfer_جمع_کن(مدخل, داده))
          أرجع CURLM_بیرون_از_حافظه;
      }
      CURL_TRC_M(داده, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : داده->mid,
                 Curl_uint32_spbset_شمار(&مدخل->xfers),
                 مدخل->conn ? 1 : 0);
    }
    وإلا {
      لكل(j = 0; j < prev_ps->n; j++) {
        إذا(s == prev_ps->sockets[j]) {
          آخرین_action = prev_ps->actions[j];
          اكسر;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_مدخل_update(multi, داده, مدخل, s,
                                  آخرین_action, ps->actions[i]);
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

    مدخل = mev_sh_مدخل_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    إذا(!مدخل)
      استمر;

    إذا(conn && !mev_sh_مدخل_conn_remove(مدخل, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(داده, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    إذا(!conn && !mev_sh_مدخل_xfer_remove(مدخل, داده)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(داده, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    إذا(mev_sh_مدخل_user_شمار(مدخل)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_مدخل_update(multi, داده, مدخل, s,
                                    prev_ps->actions[i], 0);
      إذا(mresult)
        أرجع mresult;
      CURL_TRC_M(داده, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_شمار(&مدخل->xfers),
                 مدخل->conn ? 1 : 0);
    }
    وإلا {
      mresult = mev_forget_socket(multi, داده, s, "last user gone");
      إذا(mresult)
        أرجع mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_assess(بنية Curl_multi *multi,
                            بنية Curl_easy *داده,
                            بنية connectdata *conn)
{
  بنية easy_pollset ps, *آخرین_ps;
  CURLMcode mresult = CURLM_OK;

  إذا(!multi || !multi->socket_cb)
    أرجع CURLM_OK;

  Curl_pollset_init(&ps);
  إذا(conn) {
    CURLcode نتیجه = Curl_conn_adjust_pollset(داده, conn, &ps);
    إذا(نتیجه) {
      mresult = (نتیجه == CURLE_بیرون_از_حافظه) ?
        CURLM_بیرون_از_حافظه : CURLM_داخلی_ERROR;
      اذهب بیرون;
    }
  }
  وإلا
    Curl_multi_pollset(داده, &ps);
  آخرین_ps = mev_get_آخرین_pollset(داده, conn);

  إذا(!آخرین_ps && ps.n) {
    إذا(conn)
      آخرین_ps = mev_جمع_کن_new_conn_pollset(conn);
    وإلا
      آخرین_ps = mev_جمع_کن_new_xfer_pollset(داده);
    إذا(!آخرین_ps) {
      mresult = CURLM_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

  إذا(آخرین_ps)
    mresult = mev_pollset_diff(multi, داده, conn, &ps, آخرین_ps);
  وإلا
    DEBUGASSERT(!ps.n);
بیرون:
  Curl_pollset_cleanup(&ps);
  أرجع mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(بنية Curl_multi *multi,
                                    بنية Curl_easy *داده)
{
  أرجع mev_assess(multi, داده, عدم);
}

CURLMcode Curl_multi_ev_assess_conn(بنية Curl_multi *multi,
                                    بنية Curl_easy *داده,
                                    بنية connectdata *conn)
{
  أرجع mev_assess(multi, داده, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(بنية Curl_multi *multi,
                                         بنية uint32_bset *مجموعه)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  إذا(multi && multi->socket_cb && Curl_uint32_bset_اولی(مجموعه, &mid)) {
    افعل {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      إذا(داده) {
        mresult = Curl_multi_ev_assess_xfer(multi, داده);
      }
    } طالما(!mresult && Curl_uint32_bset_بعدی(مجموعه, mid, &mid));
  }
  أرجع mresult;
}

CURLMcode Curl_multi_ev_تخصیص_بده(بنية Curl_multi *multi,
                               curl_socket_t s,
                               فراغ *user_داده)
{
  بنية mev_sh_مدخل *e = mev_sh_مدخل_get(&multi->ev.sh_entries, s);
  إذا(!e)
    أرجع CURLM_BAD_SOCKET;
  e->user_داده = user_داده;
  أرجع CURLM_OK;
}

فراغ Curl_multi_ev_dirty_xfers(بنية Curl_multi *multi,
                               curl_socket_t s)
{
  بنية mev_sh_مدخل *مدخل;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  مدخل = mev_sh_مدخل_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  إذا(مدخل) {
    بنية Curl_easy *داده;
    uint32_t mid;

    إذا(Curl_uint32_spbset_اولی(&مدخل->xfers, &mid)) {
      افعل {
        داده = Curl_multi_get_easy(multi, mid);
        إذا(داده) {
          Curl_multi_mark_dirty(داده);
        }
        وإلا {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&مدخل->xfers, mid);
        }
      } طالما(Curl_uint32_spbset_بعدی(&مدخل->xfers, mid, &mid));
    }

    إذا(مدخل->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

فراغ Curl_multi_ev_socket_انجامشده(بنية Curl_multi *multi,
                               بنية Curl_easy *داده, curl_socket_t s)
{
  mev_forget_socket(multi, داده, s, "socket done");
}

فراغ Curl_multi_ev_xfer_انجامشده(بنية Curl_multi *multi,
                             بنية Curl_easy *داده)
{
  DEBUGASSERT(!داده->conn); /* transfer should have been detached */
  (فراغ)mev_assess(multi, داده, عدم);
  Curl_meta_remove(داده, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_conn_انجامشده(بنية Curl_multi *multi,
                             بنية Curl_easy *داده,
                             بنية connectdata *conn)
{
  (فراغ)mev_assess(multi, داده, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_init(بنية Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_مدخل_dtor);
}

فراغ Curl_multi_ev_cleanup(بنية Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
