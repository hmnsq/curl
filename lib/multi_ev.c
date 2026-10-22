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

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ساختار mev_sh_entry {
  ساختار uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ساختار connectdata *conn; /* connection using this socket or NULL */
  تهی *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned صحیح action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned صحیح readers; /* this many transfers want to read */
  unsigned صحیح writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned صحیح magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ایستا تهی mev_sh_entry_dtor(تهی *freethis)
{
  ساختار mev_sh_entry *entry = (ساختار mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ایستا ساختار mev_sh_entry *mev_sh_entry_get(ساختار Curl_hash *sh,
                                             curl_socket_t s)
{
  اگر(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    بازگشت Curl_hash_pick(sh, (ثابت نویسه *)&s, اندازه(curl_socket_t));
  }
  بازگشت پوچ;
}

/* make sure this socket is present in the hash for this handle */
ایستا ساختار mev_sh_entry *mev_sh_entry_add(ساختار Curl_hash *sh,
                                             curl_socket_t s)
{
  ساختار mev_sh_entry *there = mev_sh_entry_get(sh, s);
  ساختار mev_sh_entry *check;

  اگر(there) {
    /* it is present, return fine */
    بازگشت there;
  }

  /* not present, add it */
  check = curlx_calloc(1, اندازه(ساختار mev_sh_entry));
  اگر(!check)
    بازگشت پوچ; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  اگر(!Curl_hash_add(sh, (نویسه *)&s, اندازه(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    بازگشت پوچ; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  بازگشت check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ایستا تهی mev_sh_entry_kill(ساختار Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (نویسه *)&s, اندازه(curl_socket_t));
}

ایستا size_t mev_sh_entry_user_شمار(ساختار mev_sh_entry *e)
{
  بازگشت Curl_uint32_spbset_شمار(&e->xfers) + (e->conn ? 1 : 0);
}

ایستا bool mev_sh_entry_xfer_known(ساختار mev_sh_entry *e,
                                    ساختار Curl_easy *data)
{
  بازگشت Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

ایستا bool mev_sh_entry_conn_known(ساختار mev_sh_entry *e,
                                    ساختار connectdata *conn)
{
  بازگشت (e->conn == conn);
}

ایستا تهی mev_pollset_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار easy_pollset *ps = entry;
  (تهی)کلید;
  (تهی)klen;
  اگر(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ایستا ساختار easy_pollset *mev_add_new_conn_pollset(ساختار connectdata *conn)
{
  ساختار easy_pollset *ps;

  ps = Curl_pollset_create();
  اگر(!ps)
    بازگشت پوچ;
  اگر(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    بازگشت پوچ;
  بازگشت ps;
}

ایستا ساختار easy_pollset *mev_add_new_xfer_pollset(ساختار Curl_easy *data)
{
  ساختار easy_pollset *ps;

  ps = Curl_pollset_create();
  اگر(!ps)
    بازگشت پوچ;
  اگر(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    بازگشت پوچ;
  بازگشت ps;
}

ایستا ساختار easy_pollset *mev_get_last_pollset(ساختار Curl_easy *data,
                                                 ساختار connectdata *conn)
{
  اگر(data) {
    اگر(conn)
      بازگشت Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    بازگشت Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  بازگشت پوچ;
}

ایستا bool mev_sh_entry_xfer_add(ساختار mev_sh_entry *e,
                                  ساختار Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_شمار(e) < 100000);
  بازگشت Curl_uint32_spbset_add(&e->xfers, data->mid);
}

ایستا bool mev_sh_entry_conn_add(ساختار mev_sh_entry *e,
                                  ساختار connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_شمار(e) < 100000);
  DEBUGASSERT(!e->conn);
  اگر(e->conn)
    بازگشت FALSE;
  e->conn = conn;
  بازگشت TRUE;
}

ایستا bool mev_sh_entry_xfer_remove(ساختار mev_sh_entry *e,
                                     ساختار Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  اگر(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  بازگشت present;
}

ایستا bool mev_sh_entry_conn_remove(ساختار mev_sh_entry *e,
                                     ساختار connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  اگر(e->conn == conn) {
    e->conn = پوچ;
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ایستا CURLMcode mev_forget_socket(ساختار Curl_multi *multi,
                                   ساختار Curl_easy *data,
                                   curl_socket_t s,
                                   ثابت نویسه *cause)
{
  ساختار mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  صحیح rc = 0;

  اگر(!entry) /* we never knew or already forgot about this socket */
    بازگشت CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  اگر(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    انجامبده {
      ساختار Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      اگر(sdata) {
        ساختار easy_pollset *ps = mev_get_last_pollset(sdata, پوچ);
        اگر(ps)
          Curl_pollset_remove(ps, s);
      }
    } وقتی(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  اگر(entry->conn) {
    ساختار easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    اگر(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  اگر(entry->announced && multi->socket_cb) {
    ساختار Curl_mapi_guard guard;

    NOVERBOSE((تهی)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    اگر(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  اگر(rc == -1) {
    multi->dead = TRUE;
    بازگشت CURLM_ABORTED_BY_CALLBACK;
  }
  بازگشت CURLM_OK;
}

ایستا CURLMcode mev_sh_entry_update(ساختار Curl_multi *multi,
                                     ساختار Curl_easy *data,
                                     ساختار mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned نویسه last_action,
                                     unsigned نویسه cur_action)
{
  صحیح rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  اگر(!multi->socket_cb)
    بازگشت CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  اگر(last_action == cur_action)  /* nothing from `data` changed */
    بازگشت CURLM_OK;

  اگر(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    اگر(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  وگرنه اگر(cur_action & CURL_POLL_IN)
    entry->readers++;

  اگر(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    اگر(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  وگرنه اگر(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_شمار(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_شمار(entry));
  DEBUGASSERT(entry->writers + entry->readers);

  CURL_TRC_M(data, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (last_action & CURL_POLL_IN) ? "IN" : "",
             (last_action & CURL_POLL_OUT) ? "OUT" : "",
             (cur_action & CURL_POLL_IN) ? "IN" : "",
             (cur_action & CURL_POLL_OUT) ? "OUT" : "",
             entry->readers, entry->writers);

  comboaction = (entry->writers ? CURL_POLL_OUT : 0) |
                (entry->readers ? CURL_POLL_IN : 0);
  اگر(((صحیح)entry->action == comboaction)) /* nothing for socket changed */
    بازگشت CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  اگر(rc == -1) {
    multi->dead = TRUE;
    بازگشت CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  اگر(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned صحیح)comboaction;
  }
  بازگشت CURLM_OK;
}

ایستا CURLMcode mev_pollset_diff(ساختار Curl_multi *multi,
                                  ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  ساختار easy_pollset *ps,
                                  ساختار easy_pollset *prev_ps)
{
  ساختار mev_sh_entry *entry;
  curl_socket_t s;
  unsigned صحیح i, j;
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
  برای(i = 0; i < ps->n; i++) {
    unsigned نویسه last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    اگر(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      اگر(!entry) /* fatal */
        بازگشت CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    وگرنه اگر(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    وگرنه {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    اگر(first_time) {
      اگر(conn) {
        اگر(!mev_sh_entry_conn_add(entry, conn))
          بازگشت CURLM_OUT_OF_MEMORY;
      }
      وگرنه {
        اگر(!mev_sh_entry_xfer_add(entry, data))
          بازگشت CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_شمار(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    وگرنه {
      برای(j = 0; j < prev_ps->n; j++) {
        اگر(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          بشکن;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    اگر(mresult)
      بازگشت mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  برای(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    برای(j = 0; j < ps->n; j++) {
      اگر(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        بشکن;
      }
    }
    اگر(stillused)
      ادامه;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    اگر(!entry)
      ادامه;

    اگر(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(پوچ);
      ادامه;
    }

    اگر(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(پوچ);
      ادامه;
    }

    اگر(mev_sh_entry_user_شمار(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      اگر(mresult)
        بازگشت mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_شمار(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    وگرنه {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      اگر(mresult)
        بازگشت mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  بازگشت CURLM_OK;
}

ایستا CURLMcode mev_assess(ساختار Curl_multi *multi,
                            ساختار Curl_easy *data,
                            ساختار connectdata *conn)
{
  ساختار easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  اگر(!multi || !multi->socket_cb)
    بازگشت CURLM_OK;

  Curl_pollset_init(&ps);
  اگر(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    اگر(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      برو out;
    }
  }
  وگرنه
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  اگر(!last_ps && ps.n) {
    اگر(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    وگرنه
      last_ps = mev_add_new_xfer_pollset(data);
    اگر(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      برو out;
    }
  }

  اگر(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  وگرنه
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  بازگشت mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ساختار Curl_multi *multi,
                                    ساختار Curl_easy *data)
{
  بازگشت mev_assess(multi, data, پوچ);
}

CURLMcode Curl_multi_ev_assess_conn(ساختار Curl_multi *multi,
                                    ساختار Curl_easy *data,
                                    ساختار connectdata *conn)
{
  بازگشت mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ساختار Curl_multi *multi,
                                         ساختار uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  اگر(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    انجامبده {
      ساختار Curl_easy *data = Curl_multi_get_easy(multi, mid);
      اگر(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } وقتی(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  بازگشت mresult;
}

CURLMcode Curl_multi_ev_assign(ساختار Curl_multi *multi,
                               curl_socket_t s,
                               تهی *user_data)
{
  ساختار mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  اگر(!e)
    بازگشت CURLM_BAD_SOCKET;
  e->user_data = user_data;
  بازگشت CURLM_OK;
}

تهی Curl_multi_ev_dirty_xfers(ساختار Curl_multi *multi,
                               curl_socket_t s)
{
  ساختار mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  اگر(entry) {
    ساختار Curl_easy *data;
    uint32_t mid;

    اگر(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      انجامبده {
        data = Curl_multi_get_easy(multi, mid);
        اگر(data) {
          Curl_multi_mark_dirty(data);
        }
        وگرنه {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } وقتی(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    اگر(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

تهی Curl_multi_ev_socket_done(ساختار Curl_multi *multi,
                               ساختار Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

تهی Curl_multi_ev_xfer_done(ساختار Curl_multi *multi,
                             ساختار Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (تهی)mev_assess(multi, data, پوچ);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

تهی Curl_multi_ev_conn_done(ساختار Curl_multi *multi,
                             ساختار Curl_easy *data,
                             ساختار connectdata *conn)
{
  (تهی)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

تهی Curl_multi_ev_init(ساختار Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

تهی Curl_multi_ev_cleanup(ساختار Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
