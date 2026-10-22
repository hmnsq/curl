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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "multi_ev.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "uint-bset.h"
#अन्तर्भाव "uint-spbset.h"
#अन्तर्भाव "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
संरचना mev_sh_entry {
  संरचना uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  संरचना connectdata *conn; /* connection using this socket or NULL */
  शून्य *user_data;      /* libcurl app data via curl_multi_assign() */
  अचिन्हित पूर्णाङ्क action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  अचिन्हित पूर्णाङ्क readers; /* this many transfers want to read */
  अचिन्हित पूर्णाङ्क writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  अचिन्हित पूर्णाङ्क magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
स्थिर शून्य mev_sh_entry_dtor(शून्य *freethis)
{
  संरचना mev_sh_entry *entry = (संरचना mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
स्थिर संरचना mev_sh_entry *mev_sh_entry_get(संरचना Curl_hash *sh,
                                             curl_socket_t s)
{
  यदि(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    प्रत्यागम Curl_hash_pick(sh, (ध्रुव वर्ण *)&s, परिमाण(curl_socket_t));
  }
  प्रत्यागम रिक्त;
}

/* make sure this socket is present in the hash for this handle */
स्थिर संरचना mev_sh_entry *mev_sh_entry_add(संरचना Curl_hash *sh,
                                             curl_socket_t s)
{
  संरचना mev_sh_entry *there = mev_sh_entry_get(sh, s);
  संरचना mev_sh_entry *check;

  यदि(there) {
    /* it is present, return fine */
    प्रत्यागम there;
  }

  /* not present, add it */
  check = curlx_calloc(1, परिमाण(संरचना mev_sh_entry));
  यदि(!check)
    प्रत्यागम रिक्त; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  यदि(!Curl_hash_add(sh, (वर्ण *)&s, परिमाण(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    प्रत्यागम रिक्त; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  प्रत्यागम check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
स्थिर शून्य mev_sh_entry_kill(संरचना Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (वर्ण *)&s, परिमाण(curl_socket_t));
}

स्थिर माप_प्रकार mev_sh_entry_user_गिनती(संरचना mev_sh_entry *e)
{
  प्रत्यागम Curl_uint32_spbset_गिनती(&e->xfers) + (e->conn ? 1 : 0);
}

स्थिर bool mev_sh_entry_xfer_known(संरचना mev_sh_entry *e,
                                    संरचना Curl_easy *data)
{
  प्रत्यागम Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

स्थिर bool mev_sh_entry_conn_known(संरचना mev_sh_entry *e,
                                    संरचना connectdata *conn)
{
  प्रत्यागम (e->conn == conn);
}

स्थिर शून्य mev_pollset_dtor(ध्रुव शून्य *कुंजी, माप_प्रकार klen, शून्य *entry)
{
  संरचना easy_pollset *ps = entry;
  (शून्य)कुंजी;
  (शून्य)klen;
  यदि(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

स्थिर संरचना easy_pollset *mev_add_new_conn_pollset(संरचना connectdata *conn)
{
  संरचना easy_pollset *ps;

  ps = Curl_pollset_create();
  यदि(!ps)
    प्रत्यागम रिक्त;
  यदि(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    प्रत्यागम रिक्त;
  प्रत्यागम ps;
}

स्थिर संरचना easy_pollset *mev_add_new_xfer_pollset(संरचना Curl_easy *data)
{
  संरचना easy_pollset *ps;

  ps = Curl_pollset_create();
  यदि(!ps)
    प्रत्यागम रिक्त;
  यदि(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    प्रत्यागम रिक्त;
  प्रत्यागम ps;
}

स्थिर संरचना easy_pollset *mev_get_last_pollset(संरचना Curl_easy *data,
                                                 संरचना connectdata *conn)
{
  यदि(data) {
    यदि(conn)
      प्रत्यागम Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    प्रत्यागम Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  प्रत्यागम रिक्त;
}

स्थिर bool mev_sh_entry_xfer_add(संरचना mev_sh_entry *e,
                                  संरचना Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_गिनती(e) < 100000);
  प्रत्यागम Curl_uint32_spbset_add(&e->xfers, data->mid);
}

स्थिर bool mev_sh_entry_conn_add(संरचना mev_sh_entry *e,
                                  संरचना connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_गिनती(e) < 100000);
  DEBUGASSERT(!e->conn);
  यदि(e->conn)
    प्रत्यागम FALSE;
  e->conn = conn;
  प्रत्यागम TRUE;
}

स्थिर bool mev_sh_entry_xfer_remove(संरचना mev_sh_entry *e,
                                     संरचना Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  यदि(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  प्रत्यागम present;
}

स्थिर bool mev_sh_entry_conn_remove(संरचना mev_sh_entry *e,
                                     संरचना connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  यदि(e->conn == conn) {
    e->conn = रिक्त;
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
स्थिर CURLMcode mev_forget_socket(संरचना Curl_multi *multi,
                                   संरचना Curl_easy *data,
                                   curl_socket_t s,
                                   ध्रुव वर्ण *cause)
{
  संरचना mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  पूर्णाङ्क rc = 0;

  यदि(!entry) /* we never knew or already forgot about this socket */
    प्रत्यागम CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  यदि(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    कुरु {
      संरचना Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      यदि(sdata) {
        संरचना easy_pollset *ps = mev_get_last_pollset(sdata, रिक्त);
        यदि(ps)
          Curl_pollset_remove(ps, s);
      }
    } यावत्(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  यदि(entry->conn) {
    संरचना easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    यदि(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  यदि(entry->announced && multi->socket_cb) {
    संरचना Curl_mapi_guard guard;

    NOVERBOSE((शून्य)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    यदि(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  यदि(rc == -1) {
    multi->dead = TRUE;
    प्रत्यागम CURLM_ABORTED_BY_CALLBACK;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode mev_sh_entry_update(संरचना Curl_multi *multi,
                                     संरचना Curl_easy *data,
                                     संरचना mev_sh_entry *entry,
                                     curl_socket_t s,
                                     अचिन्हित वर्ण last_action,
                                     अचिन्हित वर्ण cur_action)
{
  पूर्णाङ्क rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  यदि(!multi->socket_cb)
    प्रत्यागम CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  यदि(last_action == cur_action)  /* nothing from `data` changed */
    प्रत्यागम CURLM_OK;

  यदि(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    यदि(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  अन्यथा यदि(cur_action & CURL_POLL_IN)
    entry->readers++;

  यदि(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    यदि(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  अन्यथा यदि(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_गिनती(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_गिनती(entry));
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
  यदि(((पूर्णाङ्क)entry->action == comboaction)) /* nothing for socket changed */
    प्रत्यागम CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  यदि(rc == -1) {
    multi->dead = TRUE;
    प्रत्यागम CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  यदि(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (अचिन्हित पूर्णाङ्क)comboaction;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode mev_pollset_diff(संरचना Curl_multi *multi,
                                  संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  संरचना easy_pollset *ps,
                                  संरचना easy_pollset *prev_ps)
{
  संरचना mev_sh_entry *entry;
  curl_socket_t s;
  अचिन्हित पूर्णाङ्क i, j;
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
  प्रति(i = 0; i < ps->n; i++) {
    अचिन्हित वर्ण last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    यदि(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      यदि(!entry) /* fatal */
        प्रत्यागम CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    अन्यथा यदि(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    अन्यथा {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    यदि(first_time) {
      यदि(conn) {
        यदि(!mev_sh_entry_conn_add(entry, conn))
          प्रत्यागम CURLM_OUT_OF_MEMORY;
      }
      अन्यथा {
        यदि(!mev_sh_entry_xfer_add(entry, data))
          प्रत्यागम CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_गिनती(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    अन्यथा {
      प्रति(j = 0; j < prev_ps->n; j++) {
        यदि(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          भङ्ग;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    यदि(mresult)
      प्रत्यागम mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  प्रति(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    प्रति(j = 0; j < ps->n; j++) {
      यदि(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        भङ्ग;
      }
    }
    यदि(stillused)
      अनुवर्त;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    यदि(!entry)
      अनुवर्त;

    यदि(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(रिक्त);
      अनुवर्त;
    }

    यदि(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(रिक्त);
      अनुवर्त;
    }

    यदि(mev_sh_entry_user_गिनती(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      यदि(mresult)
        प्रत्यागम mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_गिनती(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    अन्यथा {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      यदि(mresult)
        प्रत्यागम mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode mev_assess(संरचना Curl_multi *multi,
                            संरचना Curl_easy *data,
                            संरचना connectdata *conn)
{
  संरचना easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  यदि(!multi || !multi->socket_cb)
    प्रत्यागम CURLM_OK;

  Curl_pollset_init(&ps);
  यदि(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    यदि(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      गच्छ out;
    }
  }
  अन्यथा
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  यदि(!last_ps && ps.n) {
    यदि(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    अन्यथा
      last_ps = mev_add_new_xfer_pollset(data);
    यदि(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

  यदि(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  अन्यथा
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  प्रत्यागम mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *data)
{
  प्रत्यागम mev_assess(multi, data, रिक्त);
}

CURLMcode Curl_multi_ev_assess_conn(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *data,
                                    संरचना connectdata *conn)
{
  प्रत्यागम mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(संरचना Curl_multi *multi,
                                         संरचना uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  यदि(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    कुरु {
      संरचना Curl_easy *data = Curl_multi_get_easy(multi, mid);
      यदि(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } यावत्(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  प्रत्यागम mresult;
}

CURLMcode Curl_multi_ev_assign(संरचना Curl_multi *multi,
                               curl_socket_t s,
                               शून्य *user_data)
{
  संरचना mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  यदि(!e)
    प्रत्यागम CURLM_BAD_SOCKET;
  e->user_data = user_data;
  प्रत्यागम CURLM_OK;
}

शून्य Curl_multi_ev_dirty_xfers(संरचना Curl_multi *multi,
                               curl_socket_t s)
{
  संरचना mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  यदि(entry) {
    संरचना Curl_easy *data;
    uint32_t mid;

    यदि(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      कुरु {
        data = Curl_multi_get_easy(multi, mid);
        यदि(data) {
          Curl_multi_mark_dirty(data);
        }
        अन्यथा {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } यावत्(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    यदि(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

शून्य Curl_multi_ev_socket_done(संरचना Curl_multi *multi,
                               संरचना Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

शून्य Curl_multi_ev_xfer_done(संरचना Curl_multi *multi,
                             संरचना Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (शून्य)mev_assess(multi, data, रिक्त);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

शून्य Curl_multi_ev_conn_done(संरचना Curl_multi *multi,
                             संरचना Curl_easy *data,
                             संरचना connectdata *conn)
{
  (शून्य)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

शून्य Curl_multi_ev_init(संरचना Curl_multi *multi, माप_प्रकार hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

शून्य Curl_multi_ev_cleanup(संरचना Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
