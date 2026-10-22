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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "multi_ev.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "uint-bset.h"
#அந்தர்பாவ "uint-spbset.h"
#அந்தர்பாவ "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
கட்டமைப்பு mev_sh_entry {
  கட்டமைப்பு uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  கட்டமைப்பு connectdata *conn; /* connection using this socket or NULL */
  வெறுமை *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned முழு action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned முழு readers; /* this many transfers want to read */
  unsigned முழு writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned முழு magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
நிலையான வெறுமை mev_sh_entry_dtor(வெறுமை *freethis)
{
  கட்டமைப்பு mev_sh_entry *entry = (கட்டமைப்பு mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
நிலையான கட்டமைப்பு mev_sh_entry *mev_sh_entry_get(கட்டமைப்பு Curl_hash *sh,
                                             curl_socket_t s)
{
  எனில்(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    திரும்பு Curl_hash_pick(sh, (மாறிலி எழுத்து *)&s, அளவு(curl_socket_t));
  }
  திரும்பு வெற்று;
}

/* make sure this socket is present in the hash for this handle */
நிலையான கட்டமைப்பு mev_sh_entry *mev_sh_entry_add(கட்டமைப்பு Curl_hash *sh,
                                             curl_socket_t s)
{
  கட்டமைப்பு mev_sh_entry *there = mev_sh_entry_get(sh, s);
  கட்டமைப்பு mev_sh_entry *check;

  எனில்(there) {
    /* it is present, return fine */
    திரும்பு there;
  }

  /* not present, add it */
  check = curlx_calloc(1, அளவு(கட்டமைப்பு mev_sh_entry));
  எனில்(!check)
    திரும்பு வெற்று; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  எனில்(!Curl_hash_add(sh, (எழுத்து *)&s, அளவு(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    திரும்பு வெற்று; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  திரும்பு check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
நிலையான வெறுமை mev_sh_entry_kill(கட்டமைப்பு Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (எழுத்து *)&s, அளவு(curl_socket_t));
}

நிலையான size_t mev_sh_entry_user_மொத்தம்(கட்டமைப்பு mev_sh_entry *e)
{
  திரும்பு Curl_uint32_spbset_மொத்தம்(&e->xfers) + (e->conn ? 1 : 0);
}

நிலையான bool mev_sh_entry_xfer_known(கட்டமைப்பு mev_sh_entry *e,
                                    கட்டமைப்பு Curl_easy *data)
{
  திரும்பு Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

நிலையான bool mev_sh_entry_conn_known(கட்டமைப்பு mev_sh_entry *e,
                                    கட்டமைப்பு connectdata *conn)
{
  திரும்பு (e->conn == conn);
}

நிலையான வெறுமை mev_pollset_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு easy_pollset *ps = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  எனில்(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

நிலையான கட்டமைப்பு easy_pollset *mev_add_new_conn_pollset(கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு easy_pollset *ps;

  ps = Curl_pollset_create();
  எனில்(!ps)
    திரும்பு வெற்று;
  எனில்(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    திரும்பு வெற்று;
  திரும்பு ps;
}

நிலையான கட்டமைப்பு easy_pollset *mev_add_new_xfer_pollset(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு easy_pollset *ps;

  ps = Curl_pollset_create();
  எனில்(!ps)
    திரும்பு வெற்று;
  எனில்(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    திரும்பு வெற்று;
  திரும்பு ps;
}

நிலையான கட்டமைப்பு easy_pollset *mev_get_last_pollset(கட்டமைப்பு Curl_easy *data,
                                                 கட்டமைப்பு connectdata *conn)
{
  எனில்(data) {
    எனில்(conn)
      திரும்பு Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    திரும்பு Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  திரும்பு வெற்று;
}

நிலையான bool mev_sh_entry_xfer_add(கட்டமைப்பு mev_sh_entry *e,
                                  கட்டமைப்பு Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_மொத்தம்(e) < 100000);
  திரும்பு Curl_uint32_spbset_add(&e->xfers, data->mid);
}

நிலையான bool mev_sh_entry_conn_add(கட்டமைப்பு mev_sh_entry *e,
                                  கட்டமைப்பு connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_மொத்தம்(e) < 100000);
  DEBUGASSERT(!e->conn);
  எனில்(e->conn)
    திரும்பு FALSE;
  e->conn = conn;
  திரும்பு TRUE;
}

நிலையான bool mev_sh_entry_xfer_remove(கட்டமைப்பு mev_sh_entry *e,
                                     கட்டமைப்பு Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  எனில்(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  திரும்பு present;
}

நிலையான bool mev_sh_entry_conn_remove(கட்டமைப்பு mev_sh_entry *e,
                                     கட்டமைப்பு connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  எனில்(e->conn == conn) {
    e->conn = வெற்று;
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
நிலையான CURLMcode mev_forget_socket(கட்டமைப்பு Curl_multi *multi,
                                   கட்டமைப்பு Curl_easy *data,
                                   curl_socket_t s,
                                   மாறிலி எழுத்து *cause)
{
  கட்டமைப்பு mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  முழு rc = 0;

  எனில்(!entry) /* we never knew or already forgot about this socket */
    திரும்பு CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  எனில்(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    செய் {
      கட்டமைப்பு Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      எனில்(sdata) {
        கட்டமைப்பு easy_pollset *ps = mev_get_last_pollset(sdata, வெற்று);
        எனில்(ps)
          Curl_pollset_remove(ps, s);
      }
    } போது(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  எனில்(entry->conn) {
    கட்டமைப்பு easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    எனில்(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  எனில்(entry->announced && multi->socket_cb) {
    கட்டமைப்பு Curl_mapi_guard guard;

    NOVERBOSE((வெறுமை)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    எனில்(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  எனில்(rc == -1) {
    multi->dead = TRUE;
    திரும்பு CURLM_ABORTED_BY_CALLBACK;
  }
  திரும்பு CURLM_OK;
}

நிலையான CURLMcode mev_sh_entry_update(கட்டமைப்பு Curl_multi *multi,
                                     கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned எழுத்து last_action,
                                     unsigned எழுத்து cur_action)
{
  முழு rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  எனில்(!multi->socket_cb)
    திரும்பு CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  எனில்(last_action == cur_action)  /* nothing from `data` changed */
    திரும்பு CURLM_OK;

  எனில்(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    எனில்(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  இல்லையெனில் எனில்(cur_action & CURL_POLL_IN)
    entry->readers++;

  எனில்(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    எனில்(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  இல்லையெனில் எனில்(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_மொத்தம்(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_மொத்தம்(entry));
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
  எனில்(((முழு)entry->action == comboaction)) /* nothing for socket changed */
    திரும்பு CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  எனில்(rc == -1) {
    multi->dead = TRUE;
    திரும்பு CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  எனில்(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned முழு)comboaction;
  }
  திரும்பு CURLM_OK;
}

நிலையான CURLMcode mev_pollset_diff(கட்டமைப்பு Curl_multi *multi,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  கட்டமைப்பு easy_pollset *ps,
                                  கட்டமைப்பு easy_pollset *prev_ps)
{
  கட்டமைப்பு mev_sh_entry *entry;
  curl_socket_t s;
  unsigned முழு i, j;
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
  க்கு(i = 0; i < ps->n; i++) {
    unsigned எழுத்து last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    எனில்(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      எனில்(!entry) /* fatal */
        திரும்பு CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    இல்லையெனில் எனில்(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    இல்லையெனில் {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    எனில்(first_time) {
      எனில்(conn) {
        எனில்(!mev_sh_entry_conn_add(entry, conn))
          திரும்பு CURLM_OUT_OF_MEMORY;
      }
      இல்லையெனில் {
        எனில்(!mev_sh_entry_xfer_add(entry, data))
          திரும்பு CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_மொத்தம்(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    இல்லையெனில் {
      க்கு(j = 0; j < prev_ps->n; j++) {
        எனில்(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          நிறுத்து;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    எனில்(mresult)
      திரும்பு mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  க்கு(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    க்கு(j = 0; j < ps->n; j++) {
      எனில்(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        நிறுத்து;
      }
    }
    எனில்(stillused)
      தொடர்;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    எனில்(!entry)
      தொடர்;

    எனில்(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(வெற்று);
      தொடர்;
    }

    எனில்(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(வெற்று);
      தொடர்;
    }

    எனில்(mev_sh_entry_user_மொத்தம்(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      எனில்(mresult)
        திரும்பு mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_மொத்தம்(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    இல்லையெனில் {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      எனில்(mresult)
        திரும்பு mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  திரும்பு CURLM_OK;
}

நிலையான CURLMcode mev_assess(கட்டமைப்பு Curl_multi *multi,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  எனில்(!multi || !multi->socket_cb)
    திரும்பு CURLM_OK;

  Curl_pollset_init(&ps);
  எனில்(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    எனில்(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      செல் out;
    }
  }
  இல்லையெனில்
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  எனில்(!last_ps && ps.n) {
    எனில்(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    இல்லையெனில்
      last_ps = mev_add_new_xfer_pollset(data);
    எனில்(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      செல் out;
    }
  }

  எனில்(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  இல்லையெனில்
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  திரும்பு mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(கட்டமைப்பு Curl_multi *multi,
                                    கட்டமைப்பு Curl_easy *data)
{
  திரும்பு mev_assess(multi, data, வெற்று);
}

CURLMcode Curl_multi_ev_assess_conn(கட்டமைப்பு Curl_multi *multi,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn)
{
  திரும்பு mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(கட்டமைப்பு Curl_multi *multi,
                                         கட்டமைப்பு uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  எனில்(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    செய் {
      கட்டமைப்பு Curl_easy *data = Curl_multi_get_easy(multi, mid);
      எனில்(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } போது(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  திரும்பு mresult;
}

CURLMcode Curl_multi_ev_assign(கட்டமைப்பு Curl_multi *multi,
                               curl_socket_t s,
                               வெறுமை *user_data)
{
  கட்டமைப்பு mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  எனில்(!e)
    திரும்பு CURLM_BAD_SOCKET;
  e->user_data = user_data;
  திரும்பு CURLM_OK;
}

வெறுமை Curl_multi_ev_dirty_xfers(கட்டமைப்பு Curl_multi *multi,
                               curl_socket_t s)
{
  கட்டமைப்பு mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  எனில்(entry) {
    கட்டமைப்பு Curl_easy *data;
    uint32_t mid;

    எனில்(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      செய் {
        data = Curl_multi_get_easy(multi, mid);
        எனில்(data) {
          Curl_multi_mark_dirty(data);
        }
        இல்லையெனில் {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } போது(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    எனில்(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

வெறுமை Curl_multi_ev_socket_done(கட்டமைப்பு Curl_multi *multi,
                               கட்டமைப்பு Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

வெறுமை Curl_multi_ev_xfer_done(கட்டமைப்பு Curl_multi *multi,
                             கட்டமைப்பு Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (வெறுமை)mev_assess(multi, data, வெற்று);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

வெறுமை Curl_multi_ev_conn_done(கட்டமைப்பு Curl_multi *multi,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn)
{
  (வெறுமை)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

வெறுமை Curl_multi_ev_init(கட்டமைப்பு Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

வெறுமை Curl_multi_ev_cleanup(கட்டமைப்பு Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
