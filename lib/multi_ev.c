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

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو SH_داخلا_MAGIC 0x570091d
#جيڪڏهن_پورو

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
بنية mev_sh_داخلا {
  بنية uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  بنية connectdata *conn; /* connection using this socket or NULL */
  فراغ *user_معلومات;      /* libcurl app data via curl_multi_assign() */
  بنانشان صحيح action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  بنانشان صحيح readers; /* this many transfers want to read */
  بنانشان صحيح writers; /* this many transfers want to write */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  بنانشان صحيح magic;
#جيڪڏهن_پورو
  ٻٽو_انگ(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ساكن فراغ mev_sh_داخلا_dtor(فراغ *freethis)
{
  بنية mev_sh_داخلا *داخلا_2 = (بنية mev_sh_داخلا *)freethis;
  Curl_uint32_spbset_destroy(&داخلا_2->xfers);
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  داخلا_2->magic = 0;
#جيڪڏهن_پورو
  curlx_free(داخلا_2);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ساكن بنية mev_sh_داخلا *mev_sh_داخلا_get(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  إذا(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    أرجع Curl_hash_pick(sh, (ثابت حرف *)&s, حجم(curl_socket_t));
  }
  أرجع عدم;
}

/* make sure this socket is present in the hash for this handle */
ساكن بنية mev_sh_داخلا *mev_sh_داخلا_جوڙيو(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  بنية mev_sh_داخلا *there = mev_sh_داخلا_get(sh, s);
  بنية mev_sh_داخلا *check;

  إذا(there) {
    /* it is present, return fine */
    أرجع there;
  }

  /* not present, add it */
  check = curlx_calloc(1, حجم(بنية mev_sh_داخلا));
  إذا(!check)
    أرجع عدم; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  إذا(!Curl_hash_جوڙيو(sh, (حرف *)&s, حجم(curl_socket_t), check)) {
    mev_sh_داخلا_dtor(check);
    أرجع عدم; /* major failure */
  }
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  check->magic = SH_داخلا_MAGIC;
#جيڪڏهن_پورو
  أرجع check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ساكن فراغ mev_sh_داخلا_kill(بنية Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (حرف *)&s, حجم(curl_socket_t));
}

ساكن size_t mev_sh_داخلا_user_ڳڻپ(بنية mev_sh_داخلا *e)
{
  أرجع Curl_uint32_spbset_ڳڻپ(&e->xfers) + (e->conn ? 1 : 0);
}

ساكن bool mev_sh_داخلا_xfer_معلوم(بنية mev_sh_داخلا *e,
                                    بنية Curl_easy *معلومات)
{
  أرجع Curl_uint32_spbset_شامل_آهي(&e->xfers, معلومات->mid);
}

ساكن bool mev_sh_داخلا_conn_معلوم(بنية mev_sh_داخلا *e,
                                    بنية connectdata *conn)
{
  أرجع (e->conn == conn);
}

ساكن فراغ mev_pollset_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية easy_pollset *ps = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  إذا(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ساكن بنية easy_pollset *mev_جوڙيو_new_conn_pollset(بنية connectdata *conn)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  إذا(!ps)
    أرجع عدم;
  إذا(Curl_conn_meta_سيٽ(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    أرجع عدم;
  أرجع ps;
}

ساكن بنية easy_pollset *mev_جوڙيو_new_xfer_pollset(بنية Curl_easy *معلومات)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  إذا(!ps)
    أرجع عدم;
  إذا(Curl_meta_سيٽ(معلومات, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    أرجع عدم;
  أرجع ps;
}

ساكن بنية easy_pollset *mev_get_آخري_pollset(بنية Curl_easy *معلومات,
                                                 بنية connectdata *conn)
{
  إذا(معلومات) {
    إذا(conn)
      أرجع Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    أرجع Curl_meta_get(معلومات, CURL_META_MEV_POLLSET);
  }
  أرجع عدم;
}

ساكن bool mev_sh_داخلا_xfer_جوڙيو(بنية mev_sh_داخلا *e,
                                  بنية Curl_easy *معلومات)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_داخلا_user_ڳڻپ(e) < 100000);
  أرجع Curl_uint32_spbset_جوڙيو(&e->xfers, معلومات->mid);
}

ساكن bool mev_sh_داخلا_conn_جوڙيو(بنية mev_sh_داخلا *e,
                                  بنية connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_داخلا_user_ڳڻپ(e) < 100000);
  DEBUGASSERT(!e->conn);
  إذا(e->conn)
    أرجع FALSE;
  e->conn = conn;
  أرجع TRUE;
}

ساكن bool mev_sh_داخلا_xfer_remove(بنية mev_sh_داخلا *e,
                                     بنية Curl_easy *معلومات)
{
  bool present = Curl_uint32_spbset_شامل_آهي(&e->xfers, معلومات->mid);
  إذا(present)
    Curl_uint32_spbset_remove(&e->xfers, معلومات->mid);
  أرجع present;
}

ساكن bool mev_sh_داخلا_conn_remove(بنية mev_sh_داخلا *e,
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
                                   بنية Curl_easy *معلومات,
                                   curl_socket_t s,
                                   ثابت حرف *cause)
{
  بنية mev_sh_داخلا *داخلا_2 = mev_sh_داخلا_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  صحيح rc = 0;

  إذا(!داخلا_2) /* we never knew or already forgot about this socket */
    أرجع CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  إذا(Curl_uint32_spbset_پهريون(&داخلا_2->xfers, &mid)) {
    افعل {
      بنية Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      إذا(sdata) {
        بنية easy_pollset *ps = mev_get_آخري_pollset(sdata, عدم);
        إذا(ps)
          Curl_pollset_remove(ps, s);
      }
    } طالما(Curl_uint32_spbset_ايندڙ(&داخلا_2->xfers, mid, &mid));
  }

  إذا(داخلا_2->conn) {
    بنية easy_pollset *ps = mev_get_آخري_pollset(معلومات, داخلا_2->conn);
    إذا(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  إذا(داخلا_2->announced && multi->socket_cb) {
    بنية Curl_mapi_guard guard;

    NOVERBOSE((فراغ)cause);
    CURL_TRC_M(معلومات, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_شروعات(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(معلومات, s, CURL_POLL_REMOVE,
                          multi->socket_userp, داخلا_2->user_معلومات);
    CURL_CBAPI_پڇاڙي(&guard);
    داخلا_2 = mev_sh_داخلا_get(&multi->ev.sh_entries, s);
    إذا(داخلا_2)
      داخلا_2->announced = FALSE;
  }

  mev_sh_داخلا_kill(multi, s);
  إذا(rc == -1) {
    multi->dead = TRUE;
    أرجع CURLM_ABORTED_BY_CALLBACK;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_sh_داخلا_update(بنية Curl_multi *multi,
                                     بنية Curl_easy *معلومات,
                                     بنية mev_sh_داخلا *داخلا_2,
                                     curl_socket_t s,
                                     بنانشان حرف آخري_action,
                                     بنانشان حرف cur_action)
{
  صحيح rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(داخلا_2->magic == SH_داخلا_MAGIC);
  إذا(!multi->socket_cb)
    أرجع CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  إذا(آخري_action == cur_action)  /* nothing from `data` changed */
    أرجع CURLM_OK;

  إذا(آخري_action & CURL_POLL_اندريون) {
    DEBUGASSERT(داخلا_2->readers);
    إذا(!(cur_action & CURL_POLL_اندريون))
      داخلا_2->readers--;
  }
  وإلا إذا(cur_action & CURL_POLL_اندريون)
    داخلا_2->readers++;

  إذا(آخري_action & CURL_POLL_ٻاهريون) {
    DEBUGASSERT(داخلا_2->writers);
    إذا(!(cur_action & CURL_POLL_ٻاهريون))
      داخلا_2->writers--;
  }
  وإلا إذا(cur_action & CURL_POLL_ٻاهريون)
    داخلا_2->writers++;

  DEBUGASSERT(داخلا_2->readers <= mev_sh_داخلا_user_ڳڻپ(داخلا_2));
  DEBUGASSERT(داخلا_2->writers <= mev_sh_داخلا_user_ڳڻپ(داخلا_2));
  DEBUGASSERT(داخلا_2->writers + داخلا_2->readers);

  CURL_TRC_M(معلومات, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (آخري_action & CURL_POLL_اندريون) ? "IN" : "",
             (آخري_action & CURL_POLL_ٻاهريون) ? "OUT" : "",
             (cur_action & CURL_POLL_اندريون) ? "IN" : "",
             (cur_action & CURL_POLL_ٻاهريون) ? "OUT" : "",
             داخلا_2->readers, داخلا_2->writers);

  comboaction = (داخلا_2->writers ? CURL_POLL_ٻاهريون : 0) |
                (داخلا_2->readers ? CURL_POLL_اندريون : 0);
  إذا(((صحيح)داخلا_2->action == comboaction)) /* nothing for socket changed */
    أرجع CURLM_OK;

  CURL_TRC_M(معلومات, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_اندريون) ? "IN" : "",
             (comboaction & CURL_POLL_ٻاهريون) ? "OUT" : "");
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_شروعات(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(معلومات, s, comboaction, multi->socket_userp,
                          داخلا_2->user_معلومات);
    CURL_CBAPI_MULTI_پڇاڙي(&guard);
  }
  إذا(rc == -1) {
    multi->dead = TRUE;
    أرجع CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  داخلا_2 = mev_sh_داخلا_get(&multi->ev.sh_entries, s);
  إذا(داخلا_2) {
    DEBUGASSERT(داخلا_2->magic == SH_داخلا_MAGIC);
    داخلا_2->announced = TRUE;
    داخلا_2->action = (بنانشان صحيح)comboaction;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_pollset_diff(بنية Curl_multi *multi,
                                  بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps,
                                  بنية easy_pollset *prev_ps)
{
  بنية mev_sh_داخلا *داخلا_2;
  curl_socket_t s;
  بنانشان صحيح i, j;
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
    بنانشان حرف آخري_action;
    bool پهريون_وقت = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    داخلا_2 = mev_sh_داخلا_get(&multi->ev.sh_entries, s);
    إذا(!داخلا_2) {
      /* new socket, add new entry */
      پهريون_وقت = TRUE;
      داخلا_2 = mev_sh_داخلا_جوڙيو(&multi->ev.sh_entries, s);
      إذا(!داخلا_2) /* fatal */
        أرجع CURLM_ٻاهريون_جو_يادگيري;
      CURL_TRC_M(معلومات, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    وإلا إذا(conn) {
      پهريون_وقت = !mev_sh_داخلا_conn_معلوم(داخلا_2, conn);
    }
    وإلا {
      پهريون_وقت = !mev_sh_داخلا_xfer_معلوم(داخلا_2, معلومات);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    آخري_action = 0;
    إذا(پهريون_وقت) {
      إذا(conn) {
        إذا(!mev_sh_داخلا_conn_جوڙيو(داخلا_2, conn))
          أرجع CURLM_ٻاهريون_جو_يادگيري;
      }
      وإلا {
        إذا(!mev_sh_داخلا_xfer_جوڙيو(داخلا_2, معلومات))
          أرجع CURLM_ٻاهريون_جو_يادگيري;
      }
      CURL_TRC_M(معلومات, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : معلومات->mid,
                 Curl_uint32_spbset_ڳڻپ(&داخلا_2->xfers),
                 داخلا_2->conn ? 1 : 0);
    }
    وإلا {
      لكل(j = 0; j < prev_ps->n; j++) {
        إذا(s == prev_ps->sockets[j]) {
          آخري_action = prev_ps->actions[j];
          اكسر;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_داخلا_update(multi, معلومات, داخلا_2, s,
                                  آخري_action, ps->actions[i]);
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

    داخلا_2 = mev_sh_داخلا_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    إذا(!داخلا_2)
      استمر;

    إذا(conn && !mev_sh_داخلا_conn_remove(داخلا_2, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(معلومات, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    إذا(!conn && !mev_sh_داخلا_xfer_remove(داخلا_2, معلومات)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(معلومات, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    إذا(mev_sh_داخلا_user_ڳڻپ(داخلا_2)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_داخلا_update(multi, معلومات, داخلا_2, s,
                                    prev_ps->actions[i], 0);
      إذا(mresult)
        أرجع mresult;
      CURL_TRC_M(معلومات, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_ڳڻپ(&داخلا_2->xfers),
                 داخلا_2->conn ? 1 : 0);
    }
    وإلا {
      mresult = mev_forget_socket(multi, معلومات, s, "last user gone");
      إذا(mresult)
        أرجع mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  أرجع CURLM_OK;
}

ساكن CURLMcode mev_assess(بنية Curl_multi *multi,
                            بنية Curl_easy *معلومات,
                            بنية connectdata *conn)
{
  بنية easy_pollset ps, *آخري_ps;
  CURLMcode mresult = CURLM_OK;

  إذا(!multi || !multi->socket_cb)
    أرجع CURLM_OK;

  Curl_pollset_init(&ps);
  إذا(conn) {
    CURLcode نتيجو = Curl_conn_adjust_pollset(معلومات, conn, &ps);
    إذا(نتيجو) {
      mresult = (نتيجو == CURLE_ٻاهريون_جو_يادگيري) ?
        CURLM_ٻاهريون_جو_يادگيري : CURLM_اندروني_ERROR;
      اذهب ٻاهريون;
    }
  }
  وإلا
    Curl_multi_pollset(معلومات, &ps);
  آخري_ps = mev_get_آخري_pollset(معلومات, conn);

  إذا(!آخري_ps && ps.n) {
    إذا(conn)
      آخري_ps = mev_جوڙيو_new_conn_pollset(conn);
    وإلا
      آخري_ps = mev_جوڙيو_new_xfer_pollset(معلومات);
    إذا(!آخري_ps) {
      mresult = CURLM_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

  إذا(آخري_ps)
    mresult = mev_pollset_diff(multi, معلومات, conn, &ps, آخري_ps);
  وإلا
    DEBUGASSERT(!ps.n);
ٻاهريون:
  Curl_pollset_cleanup(&ps);
  أرجع mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات)
{
  أرجع mev_assess(multi, معلومات, عدم);
}

CURLMcode Curl_multi_ev_assess_conn(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات,
                                    بنية connectdata *conn)
{
  أرجع mev_assess(multi, معلومات, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(بنية Curl_multi *multi,
                                         بنية uint32_bset *سيٽ)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  إذا(multi && multi->socket_cb && Curl_uint32_bset_پهريون(سيٽ, &mid)) {
    افعل {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      إذا(معلومات) {
        mresult = Curl_multi_ev_assess_xfer(multi, معلومات);
      }
    } طالما(!mresult && Curl_uint32_bset_ايندڙ(سيٽ, mid, &mid));
  }
  أرجع mresult;
}

CURLMcode Curl_multi_ev_سونپيو(بنية Curl_multi *multi,
                               curl_socket_t s,
                               فراغ *user_معلومات)
{
  بنية mev_sh_داخلا *e = mev_sh_داخلا_get(&multi->ev.sh_entries, s);
  إذا(!e)
    أرجع CURLM_BAD_SOCKET;
  e->user_معلومات = user_معلومات;
  أرجع CURLM_OK;
}

فراغ Curl_multi_ev_dirty_xfers(بنية Curl_multi *multi,
                               curl_socket_t s)
{
  بنية mev_sh_داخلا *داخلا_2;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  داخلا_2 = mev_sh_داخلا_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  إذا(داخلا_2) {
    بنية Curl_easy *معلومات;
    uint32_t mid;

    إذا(Curl_uint32_spbset_پهريون(&داخلا_2->xfers, &mid)) {
      افعل {
        معلومات = Curl_multi_get_easy(multi, mid);
        إذا(معلومات) {
          Curl_multi_mark_dirty(معلومات);
        }
        وإلا {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&داخلا_2->xfers, mid);
        }
      } طالما(Curl_uint32_spbset_ايندڙ(&داخلا_2->xfers, mid, &mid));
    }

    إذا(داخلا_2->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

فراغ Curl_multi_ev_socket_پورو(بنية Curl_multi *multi,
                               بنية Curl_easy *معلومات, curl_socket_t s)
{
  mev_forget_socket(multi, معلومات, s, "socket done");
}

فراغ Curl_multi_ev_xfer_پورو(بنية Curl_multi *multi,
                             بنية Curl_easy *معلومات)
{
  DEBUGASSERT(!معلومات->conn); /* transfer should have been detached */
  (فراغ)mev_assess(multi, معلومات, عدم);
  Curl_meta_remove(معلومات, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_conn_پورو(بنية Curl_multi *multi,
                             بنية Curl_easy *معلومات,
                             بنية connectdata *conn)
{
  (فراغ)mev_assess(multi, معلومات, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_init(بنية Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_داخلا_dtor);
}

فراغ Curl_multi_ev_cleanup(بنية Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
