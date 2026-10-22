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

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
بنية mev_sh_entry {
  بنية uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  بنية connectdata *conn; /* connection using this socket or NULL */
  فراغ *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned صحيح action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned صحيح readers; /* this many transfers want to read */
  unsigned صحيح writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned صحيح magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ساكن فراغ mev_sh_entry_dtor(فراغ *freethis)
{
  بنية mev_sh_entry *entry = (بنية mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ساكن بنية mev_sh_entry *mev_sh_entry_get(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  אם(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    החזר Curl_hash_pick(sh, (ثابت حرف *)&s, حجم(curl_socket_t));
  }
  החזר عدم;
}

/* make sure this socket is present in the hash for this handle */
ساكن بنية mev_sh_entry *mev_sh_entry_add(بنية Curl_hash *sh,
                                             curl_socket_t s)
{
  بنية mev_sh_entry *there = mev_sh_entry_get(sh, s);
  بنية mev_sh_entry *check;

  אם(there) {
    /* it is present, return fine */
    החזר there;
  }

  /* not present, add it */
  check = curlx_calloc(1, حجم(بنية mev_sh_entry));
  אם(!check)
    החזר عدم; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  אם(!Curl_hash_add(sh, (حرف *)&s, حجم(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    החזר عدم; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  החזר check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ساكن فراغ mev_sh_entry_kill(بنية Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (حرف *)&s, حجم(curl_socket_t));
}

ساكن size_t mev_sh_entry_user_count(بنية mev_sh_entry *e)
{
  החזר Curl_uint32_spbset_count(&e->xfers) + (e->conn ? 1 : 0);
}

ساكن bool mev_sh_entry_xfer_known(بنية mev_sh_entry *e,
                                    بنية Curl_easy *data)
{
  החזר Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

ساكن bool mev_sh_entry_conn_known(بنية mev_sh_entry *e,
                                    بنية connectdata *conn)
{
  החזר (e->conn == conn);
}

ساكن فراغ mev_pollset_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية easy_pollset *ps = entry;
  (فراغ)key;
  (فراغ)klen;
  אם(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ساكن بنية easy_pollset *mev_add_new_conn_pollset(بنية connectdata *conn)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  אם(!ps)
    החזר عدم;
  אם(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    החזר عدم;
  החזר ps;
}

ساكن بنية easy_pollset *mev_add_new_xfer_pollset(بنية Curl_easy *data)
{
  بنية easy_pollset *ps;

  ps = Curl_pollset_create();
  אם(!ps)
    החזר عدم;
  אם(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    החזר عدم;
  החזר ps;
}

ساكن بنية easy_pollset *mev_get_last_pollset(بنية Curl_easy *data,
                                                 بنية connectdata *conn)
{
  אם(data) {
    אם(conn)
      החזר Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    החזר Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  החזר عدم;
}

ساكن bool mev_sh_entry_xfer_add(بنية mev_sh_entry *e,
                                  بنية Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_count(e) < 100000);
  החזר Curl_uint32_spbset_add(&e->xfers, data->mid);
}

ساكن bool mev_sh_entry_conn_add(بنية mev_sh_entry *e,
                                  بنية connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_count(e) < 100000);
  DEBUGASSERT(!e->conn);
  אם(e->conn)
    החזר FALSE;
  e->conn = conn;
  החזר TRUE;
}

ساكن bool mev_sh_entry_xfer_remove(بنية mev_sh_entry *e,
                                     بنية Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  אם(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  החזר present;
}

ساكن bool mev_sh_entry_conn_remove(بنية mev_sh_entry *e,
                                     بنية connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  אם(e->conn == conn) {
    e->conn = عدم;
    החזר TRUE;
  }
  החזר FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ساكن CURLMcode mev_forget_socket(بنية Curl_multi *multi,
                                   بنية Curl_easy *data,
                                   curl_socket_t s,
                                   ثابت حرف *cause)
{
  بنية mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  صحيح rc = 0;

  אם(!entry) /* we never knew or already forgot about this socket */
    החזר CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  אם(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    עשה {
      بنية Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      אם(sdata) {
        بنية easy_pollset *ps = mev_get_last_pollset(sdata, عدم);
        אם(ps)
          Curl_pollset_remove(ps, s);
      }
    } כלעוד(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  אם(entry->conn) {
    بنية easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    אם(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  אם(entry->announced && multi->socket_cb) {
    بنية Curl_mapi_guard guard;

    NOVERBOSE((فراغ)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    אם(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  אם(rc == -1) {
    multi->dead = TRUE;
    החזר CURLM_ABORTED_BY_CALLBACK;
  }
  החזר CURLM_OK;
}

ساكن CURLMcode mev_sh_entry_update(بنية Curl_multi *multi,
                                     بنية Curl_easy *data,
                                     بنية mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned حرف last_action,
                                     unsigned حرف cur_action)
{
  صحيح rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  אם(!multi->socket_cb)
    החזר CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  אם(last_action == cur_action)  /* nothing from `data` changed */
    החזר CURLM_OK;

  אם(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    אם(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  אחרת אם(cur_action & CURL_POLL_IN)
    entry->readers++;

  אם(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    אם(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  אחרת אם(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_count(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_count(entry));
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
  אם(((صحيح)entry->action == comboaction)) /* nothing for socket changed */
    החזר CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  אם(rc == -1) {
    multi->dead = TRUE;
    החזר CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  אם(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned صحيح)comboaction;
  }
  החזר CURLM_OK;
}

ساكن CURLMcode mev_pollset_diff(بنية Curl_multi *multi,
                                  بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps,
                                  بنية easy_pollset *prev_ps)
{
  بنية mev_sh_entry *entry;
  curl_socket_t s;
  unsigned صحيح i, j;
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
  עבור(i = 0; i < ps->n; i++) {
    unsigned حرف last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    אם(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      אם(!entry) /* fatal */
        החזר CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    אחרת אם(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    אחרת {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    אם(first_time) {
      אם(conn) {
        אם(!mev_sh_entry_conn_add(entry, conn))
          החזר CURLM_OUT_OF_MEMORY;
      }
      אחרת {
        אם(!mev_sh_entry_xfer_add(entry, data))
          החזר CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_count(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    אחרת {
      עבור(j = 0; j < prev_ps->n; j++) {
        אם(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          اكسر;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    אם(mresult)
      החזר mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  עבור(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    עבור(j = 0; j < ps->n; j++) {
      אם(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        اكسر;
      }
    }
    אם(stillused)
      استمر;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    אם(!entry)
      استمر;

    אם(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    אם(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(عدم);
      استمر;
    }

    אם(mev_sh_entry_user_count(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      אם(mresult)
        החזר mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_count(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    אחרת {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      אם(mresult)
        החזר mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  החזר CURLM_OK;
}

ساكن CURLMcode mev_assess(بنية Curl_multi *multi,
                            بنية Curl_easy *data,
                            بنية connectdata *conn)
{
  بنية easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  אם(!multi || !multi->socket_cb)
    החזר CURLM_OK;

  Curl_pollset_init(&ps);
  אם(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    אם(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      اذهب out;
    }
  }
  אחרת
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  אם(!last_ps && ps.n) {
    אם(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    אחרת
      last_ps = mev_add_new_xfer_pollset(data);
    אם(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      اذهب out;
    }
  }

  אם(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  אחרת
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  החזר mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(بنية Curl_multi *multi,
                                    بنية Curl_easy *data)
{
  החזר mev_assess(multi, data, عدم);
}

CURLMcode Curl_multi_ev_assess_conn(بنية Curl_multi *multi,
                                    بنية Curl_easy *data,
                                    بنية connectdata *conn)
{
  החזר mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(بنية Curl_multi *multi,
                                         بنية uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  אם(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    עשה {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      אם(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } כלעוד(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  החזר mresult;
}

CURLMcode Curl_multi_ev_assign(بنية Curl_multi *multi,
                               curl_socket_t s,
                               فراغ *user_data)
{
  بنية mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  אם(!e)
    החזר CURLM_BAD_SOCKET;
  e->user_data = user_data;
  החזר CURLM_OK;
}

فراغ Curl_multi_ev_dirty_xfers(بنية Curl_multi *multi,
                               curl_socket_t s)
{
  بنية mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  אם(entry) {
    بنية Curl_easy *data;
    uint32_t mid;

    אם(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      עשה {
        data = Curl_multi_get_easy(multi, mid);
        אם(data) {
          Curl_multi_mark_dirty(data);
        }
        אחרת {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } כלעוד(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    אם(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

فراغ Curl_multi_ev_socket_done(بنية Curl_multi *multi,
                               بنية Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

فراغ Curl_multi_ev_xfer_done(بنية Curl_multi *multi,
                             بنية Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (فراغ)mev_assess(multi, data, عدم);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_conn_done(بنية Curl_multi *multi,
                             بنية Curl_easy *data,
                             بنية connectdata *conn)
{
  (فراغ)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

فراغ Curl_multi_ev_init(بنية Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

فراغ Curl_multi_ev_cleanup(بنية Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
