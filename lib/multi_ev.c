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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "multi_ev.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "uint-bset.h"
#অন্তর্ভাৱ "uint-spbset.h"
#অন্তর্ভাৱ "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
সংরচনা mev_sh_entry {
  সংরচনা uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  সংরচনা connectdata *conn; /* connection using this socket or NULL */
  শূন্য *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned পূর্ণাঙ্ক action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned পূর্ণাঙ্ক readers; /* this many transfers want to read */
  unsigned পূর্ণাঙ্ক writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned পূর্ণাঙ্ক magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
স্থির শূন্য mev_sh_entry_dtor(শূন্য *freethis)
{
  সংরচনা mev_sh_entry *entry = (সংরচনা mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
স্থির সংরচনা mev_sh_entry *mev_sh_entry_get(সংরচনা Curl_hash *sh,
                                             curl_socket_t s)
{
  যদি(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ঘূৰাই Curl_hash_pick(sh, (ধ্রুৱ ৱর্ণ *)&s, পরিমাণ(curl_socket_t));
  }
  ঘূৰাই রিক্ত;
}

/* make sure this socket is present in the hash for this handle */
স্থির সংরচনা mev_sh_entry *mev_sh_entry_add(সংরচনা Curl_hash *sh,
                                             curl_socket_t s)
{
  সংরচনা mev_sh_entry *there = mev_sh_entry_get(sh, s);
  সংরচনা mev_sh_entry *check;

  যদি(there) {
    /* it is present, return fine */
    ঘূৰাই there;
  }

  /* not present, add it */
  check = curlx_calloc(1, পরিমাণ(সংরচনা mev_sh_entry));
  যদি(!check)
    ঘূৰাই রিক্ত; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  যদি(!Curl_hash_add(sh, (ৱর্ণ *)&s, পরিমাণ(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    ঘূৰাই রিক্ত; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  ঘূৰাই check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
স্থির শূন্য mev_sh_entry_kill(সংরচনা Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (ৱর্ণ *)&s, পরিমাণ(curl_socket_t));
}

স্থির size_t mev_sh_entry_user_গুনতি(সংরচনা mev_sh_entry *e)
{
  ঘূৰাই Curl_uint32_spbset_গুনতি(&e->xfers) + (e->conn ? 1 : 0);
}

স্থির bool mev_sh_entry_xfer_known(সংরচনা mev_sh_entry *e,
                                    সংরচনা Curl_easy *data)
{
  ঘূৰাই Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

স্থির bool mev_sh_entry_conn_known(সংরচনা mev_sh_entry *e,
                                    সংরচনা connectdata *conn)
{
  ঘূৰাই (e->conn == conn);
}

স্থির শূন্য mev_pollset_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা easy_pollset *ps = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  যদি(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

স্থির সংরচনা easy_pollset *mev_add_new_conn_pollset(সংরচনা connectdata *conn)
{
  সংরচনা easy_pollset *ps;

  ps = Curl_pollset_create();
  যদি(!ps)
    ঘূৰাই রিক্ত;
  যদি(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ঘূৰাই রিক্ত;
  ঘূৰাই ps;
}

স্থির সংরচনা easy_pollset *mev_add_new_xfer_pollset(সংরচনা Curl_easy *data)
{
  সংরচনা easy_pollset *ps;

  ps = Curl_pollset_create();
  যদি(!ps)
    ঘূৰাই রিক্ত;
  যদি(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ঘূৰাই রিক্ত;
  ঘূৰাই ps;
}

স্থির সংরচনা easy_pollset *mev_get_last_pollset(সংরচনা Curl_easy *data,
                                                 সংরচনা connectdata *conn)
{
  যদি(data) {
    যদি(conn)
      ঘূৰাই Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ঘূৰাই Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  ঘূৰাই রিক্ত;
}

স্থির bool mev_sh_entry_xfer_add(সংরচনা mev_sh_entry *e,
                                  সংরচনা Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_গুনতি(e) < 100000);
  ঘূৰাই Curl_uint32_spbset_add(&e->xfers, data->mid);
}

স্থির bool mev_sh_entry_conn_add(সংরচনা mev_sh_entry *e,
                                  সংরচনা connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_গুনতি(e) < 100000);
  DEBUGASSERT(!e->conn);
  যদি(e->conn)
    ঘূৰাই FALSE;
  e->conn = conn;
  ঘূৰাই TRUE;
}

স্থির bool mev_sh_entry_xfer_remove(সংরচনা mev_sh_entry *e,
                                     সংরচনা Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  যদি(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  ঘূৰাই present;
}

স্থির bool mev_sh_entry_conn_remove(সংরচনা mev_sh_entry *e,
                                     সংরচনা connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  যদি(e->conn == conn) {
    e->conn = রিক্ত;
    ঘূৰাই TRUE;
  }
  ঘূৰাই FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
স্থির CURLMcode mev_forget_socket(সংরচনা Curl_multi *multi,
                                   সংরচনা Curl_easy *data,
                                   curl_socket_t s,
                                   ধ্রুৱ ৱর্ণ *cause)
{
  সংরচনা mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  পূর্ণাঙ্ক rc = 0;

  যদি(!entry) /* we never knew or already forgot about this socket */
    ঘূৰাই CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  যদি(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    কৰা {
      সংরচনা Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      যদি(sdata) {
        সংরচনা easy_pollset *ps = mev_get_last_pollset(sdata, রিক্ত);
        যদি(ps)
          Curl_pollset_remove(ps, s);
      }
    } যেতিয়ালৈ(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  যদি(entry->conn) {
    সংরচনা easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    যদি(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  যদি(entry->announced && multi->socket_cb) {
    সংরচনা Curl_mapi_guard guard;

    NOVERBOSE((শূন্য)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    যদি(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  যদি(rc == -1) {
    multi->dead = TRUE;
    ঘূৰাই CURLM_ABORTED_BY_CALLBACK;
  }
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode mev_sh_entry_update(সংরচনা Curl_multi *multi,
                                     সংরচনা Curl_easy *data,
                                     সংরচনা mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned ৱর্ণ last_action,
                                     unsigned ৱর্ণ cur_action)
{
  পূর্ণাঙ্ক rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  যদি(!multi->socket_cb)
    ঘূৰাই CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  যদি(last_action == cur_action)  /* nothing from `data` changed */
    ঘূৰাই CURLM_OK;

  যদি(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    যদি(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  অন্যথা যদি(cur_action & CURL_POLL_IN)
    entry->readers++;

  যদি(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    যদি(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  অন্যথা যদি(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_গুনতি(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_গুনতি(entry));
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
  যদি(((পূর্ণাঙ্ক)entry->action == comboaction)) /* nothing for socket changed */
    ঘূৰাই CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    সংরচনা Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  যদি(rc == -1) {
    multi->dead = TRUE;
    ঘূৰাই CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  যদি(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned পূর্ণাঙ্ক)comboaction;
  }
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode mev_pollset_diff(সংরচনা Curl_multi *multi,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn,
                                  সংরচনা easy_pollset *ps,
                                  সংরচনা easy_pollset *prev_ps)
{
  সংরচনা mev_sh_entry *entry;
  curl_socket_t s;
  unsigned পূর্ণাঙ্ক i, j;
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
  বাবে(i = 0; i < ps->n; i++) {
    unsigned ৱর্ণ last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    যদি(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      যদি(!entry) /* fatal */
        ঘূৰাই CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    অন্যথা যদি(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    অন্যথা {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    যদি(first_time) {
      যদি(conn) {
        যদি(!mev_sh_entry_conn_add(entry, conn))
          ঘূৰাই CURLM_OUT_OF_MEMORY;
      }
      অন্যথা {
        যদি(!mev_sh_entry_xfer_add(entry, data))
          ঘূৰাই CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_গুনতি(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    অন্যথা {
      বাবে(j = 0; j < prev_ps->n; j++) {
        যদি(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          ভঙ্গ;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    যদি(mresult)
      ঘূৰাই mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  বাবে(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    বাবে(j = 0; j < ps->n; j++) {
      যদি(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        ভঙ্গ;
      }
    }
    যদি(stillused)
      অনুৱর্ত;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    যদি(!entry)
      অনুৱর্ত;

    যদি(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(রিক্ত);
      অনুৱর্ত;
    }

    যদি(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(রিক্ত);
      অনুৱর্ত;
    }

    যদি(mev_sh_entry_user_গুনতি(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      যদি(mresult)
        ঘূৰাই mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_গুনতি(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    অন্যথা {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      যদি(mresult)
        ঘূৰাই mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode mev_assess(সংরচনা Curl_multi *multi,
                            সংরচনা Curl_easy *data,
                            সংরচনা connectdata *conn)
{
  সংরচনা easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  যদি(!multi || !multi->socket_cb)
    ঘূৰাই CURLM_OK;

  Curl_pollset_init(&ps);
  যদি(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    যদি(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      গচ্ছ out;
    }
  }
  অন্যথা
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  যদি(!last_ps && ps.n) {
    যদি(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    অন্যথা
      last_ps = mev_add_new_xfer_pollset(data);
    যদি(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }

  যদি(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  অন্যথা
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  ঘূৰাই mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(সংরচনা Curl_multi *multi,
                                    সংরচনা Curl_easy *data)
{
  ঘূৰাই mev_assess(multi, data, রিক্ত);
}

CURLMcode Curl_multi_ev_assess_conn(সংরচনা Curl_multi *multi,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা connectdata *conn)
{
  ঘূৰাই mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(সংরচনা Curl_multi *multi,
                                         সংরচনা uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  যদি(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    কৰা {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      যদি(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } যেতিয়ালৈ(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  ঘূৰাই mresult;
}

CURLMcode Curl_multi_ev_assign(সংরচনা Curl_multi *multi,
                               curl_socket_t s,
                               শূন্য *user_data)
{
  সংরচনা mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  যদি(!e)
    ঘূৰাই CURLM_BAD_SOCKET;
  e->user_data = user_data;
  ঘূৰাই CURLM_OK;
}

শূন্য Curl_multi_ev_dirty_xfers(সংরচনা Curl_multi *multi,
                               curl_socket_t s)
{
  সংরচনা mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  যদি(entry) {
    সংরচনা Curl_easy *data;
    uint32_t mid;

    যদি(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      কৰা {
        data = Curl_multi_get_easy(multi, mid);
        যদি(data) {
          Curl_multi_mark_dirty(data);
        }
        অন্যথা {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } যেতিয়ালৈ(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    যদি(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

শূন্য Curl_multi_ev_socket_done(সংরচনা Curl_multi *multi,
                               সংরচনা Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

শূন্য Curl_multi_ev_xfer_done(সংরচনা Curl_multi *multi,
                             সংরচনা Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (শূন্য)mev_assess(multi, data, রিক্ত);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

শূন্য Curl_multi_ev_conn_done(সংরচনা Curl_multi *multi,
                             সংরচনা Curl_easy *data,
                             সংরচনা connectdata *conn)
{
  (শূন্য)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

শূন্য Curl_multi_ev_init(সংরচনা Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

শূন্য Curl_multi_ev_cleanup(সংরচনা Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
