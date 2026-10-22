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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "multi_ev.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "uint-bset.h"
#അന്തര്ഭാവ "uint-spbset.h"
#അന്തര്ഭാവ "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ഘടന mev_sh_entry {
  ഘടന uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ഘടന connectdata *conn; /* connection using this socket or NULL */
  ശൂന്യം *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned പൂർണ്ണസംഖ്യ action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned പൂർണ്ണസംഖ്യ readers; /* this many transfers want to read */
  unsigned പൂർണ്ണസംഖ്യ writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned പൂർണ്ണസംഖ്യ magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
സ്ഥിതം ശൂന്യം mev_sh_entry_dtor(ശൂന്യം *freethis)
{
  ഘടന mev_sh_entry *entry = (ഘടന mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
സ്ഥിതം ഘടന mev_sh_entry *mev_sh_entry_get(ഘടന Curl_hash *sh,
                                             curl_socket_t s)
{
  എങ്കിൽ(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    തിരികെ Curl_hash_pick(sh, (സ്ഥിരം അക്ഷരം *)&s, വലുപ്പം(curl_socket_t));
  }
  തിരികെ ശൂന്യ;
}

/* make sure this socket is present in the hash for this handle */
സ്ഥിതം ഘടന mev_sh_entry *mev_sh_entry_add(ഘടന Curl_hash *sh,
                                             curl_socket_t s)
{
  ഘടന mev_sh_entry *there = mev_sh_entry_get(sh, s);
  ഘടന mev_sh_entry *check;

  എങ്കിൽ(there) {
    /* it is present, return fine */
    തിരികെ there;
  }

  /* not present, add it */
  check = curlx_calloc(1, വലുപ്പം(ഘടന mev_sh_entry));
  എങ്കിൽ(!check)
    തിരികെ ശൂന്യ; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  എങ്കിൽ(!Curl_hash_add(sh, (അക്ഷരം *)&s, വലുപ്പം(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    തിരികെ ശൂന്യ; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  തിരികെ check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
സ്ഥിതം ശൂന്യം mev_sh_entry_kill(ഘടന Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (അക്ഷരം *)&s, വലുപ്പം(curl_socket_t));
}

സ്ഥിതം size_t mev_sh_entry_user_എണ്ണം(ഘടന mev_sh_entry *e)
{
  തിരികെ Curl_uint32_spbset_എണ്ണം(&e->xfers) + (e->conn ? 1 : 0);
}

സ്ഥിതം bool mev_sh_entry_xfer_known(ഘടന mev_sh_entry *e,
                                    ഘടന Curl_easy *data)
{
  തിരികെ Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

സ്ഥിതം bool mev_sh_entry_conn_known(ഘടന mev_sh_entry *e,
                                    ഘടന connectdata *conn)
{
  തിരികെ (e->conn == conn);
}

സ്ഥിതം ശൂന്യം mev_pollset_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന easy_pollset *ps = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  എങ്കിൽ(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

സ്ഥിതം ഘടന easy_pollset *mev_add_new_conn_pollset(ഘടന connectdata *conn)
{
  ഘടന easy_pollset *ps;

  ps = Curl_pollset_create();
  എങ്കിൽ(!ps)
    തിരികെ ശൂന്യ;
  എങ്കിൽ(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    തിരികെ ശൂന്യ;
  തിരികെ ps;
}

സ്ഥിതം ഘടന easy_pollset *mev_add_new_xfer_pollset(ഘടന Curl_easy *data)
{
  ഘടന easy_pollset *ps;

  ps = Curl_pollset_create();
  എങ്കിൽ(!ps)
    തിരികെ ശൂന്യ;
  എങ്കിൽ(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    തിരികെ ശൂന്യ;
  തിരികെ ps;
}

സ്ഥിതം ഘടന easy_pollset *mev_get_last_pollset(ഘടന Curl_easy *data,
                                                 ഘടന connectdata *conn)
{
  എങ്കിൽ(data) {
    എങ്കിൽ(conn)
      തിരികെ Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    തിരികെ Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം bool mev_sh_entry_xfer_add(ഘടന mev_sh_entry *e,
                                  ഘടന Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_എണ്ണം(e) < 100000);
  തിരികെ Curl_uint32_spbset_add(&e->xfers, data->mid);
}

സ്ഥിതം bool mev_sh_entry_conn_add(ഘടന mev_sh_entry *e,
                                  ഘടന connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_എണ്ണം(e) < 100000);
  DEBUGASSERT(!e->conn);
  എങ്കിൽ(e->conn)
    തിരികെ FALSE;
  e->conn = conn;
  തിരികെ TRUE;
}

സ്ഥിതം bool mev_sh_entry_xfer_remove(ഘടന mev_sh_entry *e,
                                     ഘടന Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  എങ്കിൽ(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  തിരികെ present;
}

സ്ഥിതം bool mev_sh_entry_conn_remove(ഘടന mev_sh_entry *e,
                                     ഘടന connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  എങ്കിൽ(e->conn == conn) {
    e->conn = ശൂന്യ;
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
സ്ഥിതം CURLMcode mev_forget_socket(ഘടന Curl_multi *multi,
                                   ഘടന Curl_easy *data,
                                   curl_socket_t s,
                                   സ്ഥിരം അക്ഷരം *cause)
{
  ഘടന mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  പൂർണ്ണസംഖ്യ rc = 0;

  എങ്കിൽ(!entry) /* we never knew or already forgot about this socket */
    തിരികെ CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  എങ്കിൽ(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    ചെയ്യുക {
      ഘടന Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      എങ്കിൽ(sdata) {
        ഘടന easy_pollset *ps = mev_get_last_pollset(sdata, ശൂന്യ);
        എങ്കിൽ(ps)
          Curl_pollset_remove(ps, s);
      }
    } ആകുമ്പോൾ(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  എങ്കിൽ(entry->conn) {
    ഘടന easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    എങ്കിൽ(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  എങ്കിൽ(entry->announced && multi->socket_cb) {
    ഘടന Curl_mapi_guard guard;

    NOVERBOSE((ശൂന്യം)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    എങ്കിൽ(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  എങ്കിൽ(rc == -1) {
    multi->dead = TRUE;
    തിരികെ CURLM_ABORTED_BY_CALLBACK;
  }
  തിരികെ CURLM_OK;
}

സ്ഥിതം CURLMcode mev_sh_entry_update(ഘടന Curl_multi *multi,
                                     ഘടന Curl_easy *data,
                                     ഘടന mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned അക്ഷരം last_action,
                                     unsigned അക്ഷരം cur_action)
{
  പൂർണ്ണസംഖ്യ rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  എങ്കിൽ(!multi->socket_cb)
    തിരികെ CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  എങ്കിൽ(last_action == cur_action)  /* nothing from `data` changed */
    തിരികെ CURLM_OK;

  എങ്കിൽ(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    എങ്കിൽ(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(cur_action & CURL_POLL_IN)
    entry->readers++;

  എങ്കിൽ(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    എങ്കിൽ(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_എണ്ണം(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_എണ്ണം(entry));
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
  എങ്കിൽ(((പൂർണ്ണസംഖ്യ)entry->action == comboaction)) /* nothing for socket changed */
    തിരികെ CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  എങ്കിൽ(rc == -1) {
    multi->dead = TRUE;
    തിരികെ CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  എങ്കിൽ(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned പൂർണ്ണസംഖ്യ)comboaction;
  }
  തിരികെ CURLM_OK;
}

സ്ഥിതം CURLMcode mev_pollset_diff(ഘടന Curl_multi *multi,
                                  ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  ഘടന easy_pollset *ps,
                                  ഘടന easy_pollset *prev_ps)
{
  ഘടന mev_sh_entry *entry;
  curl_socket_t s;
  unsigned പൂർണ്ണസംഖ്യ i, j;
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
  വേണ്ടി(i = 0; i < ps->n; i++) {
    unsigned അക്ഷരം last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    എങ്കിൽ(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      എങ്കിൽ(!entry) /* fatal */
        തിരികെ CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    അല്ലെങ്കിൽ {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    എങ്കിൽ(first_time) {
      എങ്കിൽ(conn) {
        എങ്കിൽ(!mev_sh_entry_conn_add(entry, conn))
          തിരികെ CURLM_OUT_OF_MEMORY;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(!mev_sh_entry_xfer_add(entry, data))
          തിരികെ CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_എണ്ണം(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    അല്ലെങ്കിൽ {
      വേണ്ടി(j = 0; j < prev_ps->n; j++) {
        എങ്കിൽ(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          നിർത്തുക;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    എങ്കിൽ(mresult)
      തിരികെ mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  വേണ്ടി(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    വേണ്ടി(j = 0; j < ps->n; j++) {
      എങ്കിൽ(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        നിർത്തുക;
      }
    }
    എങ്കിൽ(stillused)
      തുടരുക;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    എങ്കിൽ(!entry)
      തുടരുക;

    എങ്കിൽ(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ശൂന്യ);
      തുടരുക;
    }

    എങ്കിൽ(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ശൂന്യ);
      തുടരുക;
    }

    എങ്കിൽ(mev_sh_entry_user_എണ്ണം(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      എങ്കിൽ(mresult)
        തിരികെ mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_എണ്ണം(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    അല്ലെങ്കിൽ {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      എങ്കിൽ(mresult)
        തിരികെ mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  തിരികെ CURLM_OK;
}

സ്ഥിതം CURLMcode mev_assess(ഘടന Curl_multi *multi,
                            ഘടന Curl_easy *data,
                            ഘടന connectdata *conn)
{
  ഘടന easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  എങ്കിൽ(!multi || !multi->socket_cb)
    തിരികെ CURLM_OK;

  Curl_pollset_init(&ps);
  എങ്കിൽ(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    എങ്കിൽ(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      പോകുക out;
    }
  }
  അല്ലെങ്കിൽ
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  എങ്കിൽ(!last_ps && ps.n) {
    എങ്കിൽ(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    അല്ലെങ്കിൽ
      last_ps = mev_add_new_xfer_pollset(data);
    എങ്കിൽ(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      പോകുക out;
    }
  }

  എങ്കിൽ(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  അല്ലെങ്കിൽ
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  തിരികെ mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ഘടന Curl_multi *multi,
                                    ഘടന Curl_easy *data)
{
  തിരികെ mev_assess(multi, data, ശൂന്യ);
}

CURLMcode Curl_multi_ev_assess_conn(ഘടന Curl_multi *multi,
                                    ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn)
{
  തിരികെ mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ഘടന Curl_multi *multi,
                                         ഘടന uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  എങ്കിൽ(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    ചെയ്യുക {
      ഘടന Curl_easy *data = Curl_multi_get_easy(multi, mid);
      എങ്കിൽ(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } ആകുമ്പോൾ(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  തിരികെ mresult;
}

CURLMcode Curl_multi_ev_assign(ഘടന Curl_multi *multi,
                               curl_socket_t s,
                               ശൂന്യം *user_data)
{
  ഘടന mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  എങ്കിൽ(!e)
    തിരികെ CURLM_BAD_SOCKET;
  e->user_data = user_data;
  തിരികെ CURLM_OK;
}

ശൂന്യം Curl_multi_ev_dirty_xfers(ഘടന Curl_multi *multi,
                               curl_socket_t s)
{
  ഘടന mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  എങ്കിൽ(entry) {
    ഘടന Curl_easy *data;
    uint32_t mid;

    എങ്കിൽ(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      ചെയ്യുക {
        data = Curl_multi_get_easy(multi, mid);
        എങ്കിൽ(data) {
          Curl_multi_mark_dirty(data);
        }
        അല്ലെങ്കിൽ {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } ആകുമ്പോൾ(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    എങ്കിൽ(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

ശൂന്യം Curl_multi_ev_socket_done(ഘടന Curl_multi *multi,
                               ഘടന Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

ശൂന്യം Curl_multi_ev_xfer_done(ഘടന Curl_multi *multi,
                             ഘടന Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (ശൂന്യം)mev_assess(multi, data, ശൂന്യ);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

ശൂന്യം Curl_multi_ev_conn_done(ഘടന Curl_multi *multi,
                             ഘടന Curl_easy *data,
                             ഘടന connectdata *conn)
{
  (ശൂന്യം)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

ശൂന്യം Curl_multi_ev_init(ഘടന Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

ശൂന്യം Curl_multi_ev_cleanup(ഘടന Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
