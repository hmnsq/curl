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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "multi_ev.h"
#समावेश "select.h"
#समावेश "uint-bset.h"
#समावेश "uint-spbset.h"
#समावेश "multihandle.h"

#अगर_घोषित DEBUGBUILD
#घोषणा SH_ENTRY_MAGIC 0x570091d
#पूर्ण_अगर

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
काष्ठा mev_sh_entry {
  काष्ठा uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  काष्ठा connectdata *conn; /* connection using this socket or NULL */
  व्योम *user_तथ्य;      /* libcurl app data via curl_multi_assign() */
  अचिन्हित पूर्णांक action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  अचिन्हित पूर्णांक readers; /* this many transfers want to read */
  अचिन्हित पूर्णांक writers; /* this many transfers want to write */
#अगर_घोषित DEBUGBUILD
  अचिन्हित पूर्णांक magic;
#पूर्ण_अगर
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
अटल व्योम mev_sh_entry_dtor(व्योम *freethis)
{
  काष्ठा mev_sh_entry *entry = (काष्ठा mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#अगर_घोषित DEBUGBUILD
  entry->magic = 0;
#पूर्ण_अगर
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
अटल काष्ठा mev_sh_entry *mev_sh_entry_get(काष्ठा Curl_hash *sh,
                                             curl_socket_t s)
{
  अगर(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    वापस Curl_hash_pick(sh, (स्थिर अक्षर *)&s, माप(curl_socket_t));
  }
  वापस शून्य;
}

/* make sure this socket is present in the hash for this handle */
अटल काष्ठा mev_sh_entry *mev_sh_entry_add(काष्ठा Curl_hash *sh,
                                             curl_socket_t s)
{
  काष्ठा mev_sh_entry *there = mev_sh_entry_get(sh, s);
  काष्ठा mev_sh_entry *check;

  अगर(there) {
    /* it is present, return fine */
    वापस there;
  }

  /* not present, add it */
  check = curlx_calloc(1, माप(काष्ठा mev_sh_entry));
  अगर(!check)
    वापस शून्य; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  अगर(!Curl_hash_add(sh, (अक्षर *)&s, माप(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    वापस शून्य; /* major failure */
  }
#अगर_घोषित DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#पूर्ण_अगर
  वापस check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
अटल व्योम mev_sh_entry_kill(काष्ठा Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (अक्षर *)&s, माप(curl_socket_t));
}

अटल माप_प्रकार mev_sh_entry_user_गिनती(काष्ठा mev_sh_entry *e)
{
  वापस Curl_uint32_spbset_गिनती(&e->xfers) + (e->conn ? 1 : 0);
}

अटल bool mev_sh_entry_xfer_known(काष्ठा mev_sh_entry *e,
                                    काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_uint32_spbset_contains(&e->xfers, तथ्य->mid);
}

अटल bool mev_sh_entry_conn_known(काष्ठा mev_sh_entry *e,
                                    काष्ठा connectdata *conn)
{
  वापस (e->conn == conn);
}

अटल व्योम mev_pollset_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा easy_pollset *ps = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  अगर(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

अटल काष्ठा easy_pollset *mev_add_new_conn_pollset(काष्ठा connectdata *conn)
{
  काष्ठा easy_pollset *ps;

  ps = Curl_pollset_create();
  अगर(!ps)
    वापस शून्य;
  अगर(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    वापस शून्य;
  वापस ps;
}

अटल काष्ठा easy_pollset *mev_add_new_xfer_pollset(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा easy_pollset *ps;

  ps = Curl_pollset_create();
  अगर(!ps)
    वापस शून्य;
  अगर(Curl_meta_set(तथ्य, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    वापस शून्य;
  वापस ps;
}

अटल काष्ठा easy_pollset *mev_get_last_pollset(काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा connectdata *conn)
{
  अगर(तथ्य) {
    अगर(conn)
      वापस Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    वापस Curl_meta_get(तथ्य, CURL_META_MEV_POLLSET);
  }
  वापस शून्य;
}

अटल bool mev_sh_entry_xfer_add(काष्ठा mev_sh_entry *e,
                                  काष्ठा Curl_easy *तथ्य)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_गिनती(e) < 100000);
  वापस Curl_uint32_spbset_add(&e->xfers, तथ्य->mid);
}

अटल bool mev_sh_entry_conn_add(काष्ठा mev_sh_entry *e,
                                  काष्ठा connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_गिनती(e) < 100000);
  DEBUGASSERT(!e->conn);
  अगर(e->conn)
    वापस FALSE;
  e->conn = conn;
  वापस TRUE;
}

अटल bool mev_sh_entry_xfer_remove(काष्ठा mev_sh_entry *e,
                                     काष्ठा Curl_easy *तथ्य)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, तथ्य->mid);
  अगर(present)
    Curl_uint32_spbset_remove(&e->xfers, तथ्य->mid);
  वापस present;
}

अटल bool mev_sh_entry_conn_remove(काष्ठा mev_sh_entry *e,
                                     काष्ठा connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  अगर(e->conn == conn) {
    e->conn = शून्य;
    वापस TRUE;
  }
  वापस FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
अटल CURLMcode mev_forget_socket(काष्ठा Curl_multi *multi,
                                   काष्ठा Curl_easy *तथ्य,
                                   curl_socket_t s,
                                   स्थिर अक्षर *cause)
{
  काष्ठा mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  पूर्णांक rc = 0;

  अगर(!entry) /* we never knew or already forgot about this socket */
    वापस CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  अगर(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    करो {
      काष्ठा Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      अगर(sdata) {
        काष्ठा easy_pollset *ps = mev_get_last_pollset(sdata, शून्य);
        अगर(ps)
          Curl_pollset_remove(ps, s);
      }
    } जबतक(Curl_uint32_spbset_अग्र(&entry->xfers, mid, &mid));
  }

  अगर(entry->conn) {
    काष्ठा easy_pollset *ps = mev_get_last_pollset(तथ्य, entry->conn);
    अगर(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  अगर(entry->announced && multi->socket_cb) {
    काष्ठा Curl_mapi_guard guard;

    NOVERBOSE((व्योम)cause);
    CURL_TRC_M(तथ्य, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(तथ्य, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_तथ्य);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    अगर(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  अगर(rc == -1) {
    multi->dead = TRUE;
    वापस CURLM_ABORTED_BY_CALLBACK;
  }
  वापस CURLM_OK;
}

अटल CURLMcode mev_sh_entry_update(काष्ठा Curl_multi *multi,
                                     काष्ठा Curl_easy *तथ्य,
                                     काष्ठा mev_sh_entry *entry,
                                     curl_socket_t s,
                                     अचिन्हित अक्षर last_action,
                                     अचिन्हित अक्षर cur_action)
{
  पूर्णांक rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  अगर(!multi->socket_cb)
    वापस CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  अगर(last_action == cur_action)  /* nothing from `data` changed */
    वापस CURLM_OK;

  अगर(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    अगर(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  अन्यथा अगर(cur_action & CURL_POLL_IN)
    entry->readers++;

  अगर(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    अगर(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  अन्यथा अगर(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_गिनती(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_गिनती(entry));
  DEBUGASSERT(entry->writers + entry->readers);

  CURL_TRC_M(तथ्य, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (last_action & CURL_POLL_IN) ? "IN" : "",
             (last_action & CURL_POLL_OUT) ? "OUT" : "",
             (cur_action & CURL_POLL_IN) ? "IN" : "",
             (cur_action & CURL_POLL_OUT) ? "OUT" : "",
             entry->readers, entry->writers);

  comboaction = (entry->writers ? CURL_POLL_OUT : 0) |
                (entry->readers ? CURL_POLL_IN : 0);
  अगर(((पूर्णांक)entry->action == comboaction)) /* nothing for socket changed */
    वापस CURLM_OK;

  CURL_TRC_M(तथ्य, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(तथ्य, s, comboaction, multi->socket_userp,
                          entry->user_तथ्य);
    CURL_CBAPI_MULTI_END(&guard);
  }
  अगर(rc == -1) {
    multi->dead = TRUE;
    वापस CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  अगर(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (अचिन्हित पूर्णांक)comboaction;
  }
  वापस CURLM_OK;
}

अटल CURLMcode mev_pollset_diff(काष्ठा Curl_multi *multi,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  काष्ठा easy_pollset *ps,
                                  काष्ठा easy_pollset *पूर्व_ps)
{
  काष्ठा mev_sh_entry *entry;
  curl_socket_t s;
  अचिन्हित पूर्णांक i, j;
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
  DEBUGASSERT(पूर्व_ps);

  /* Handle changes to sockets the transfer is interested in. */
  क्रम(i = 0; i < ps->n; i++) {
    अचिन्हित अक्षर last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    अगर(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      अगर(!entry) /* fatal */
        वापस CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(तथ्य, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    अन्यथा अगर(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    अन्यथा {
      first_time = !mev_sh_entry_xfer_known(entry, तथ्य);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    अगर(first_time) {
      अगर(conn) {
        अगर(!mev_sh_entry_conn_add(entry, conn))
          वापस CURLM_OUT_OF_MEMORY;
      }
      अन्यथा {
        अगर(!mev_sh_entry_xfer_add(entry, तथ्य))
          वापस CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(तथ्य, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : तथ्य->mid,
                 Curl_uint32_spbset_गिनती(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    अन्यथा {
      क्रम(j = 0; j < पूर्व_ps->n; j++) {
        अगर(s == पूर्व_ps->sockets[j]) {
          last_action = पूर्व_ps->actions[j];
          अवरोध;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, तथ्य, entry, s,
                                  last_action, ps->actions[i]);
    अगर(mresult)
      वापस mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  क्रम(i = 0; i < पूर्व_ps->n; i++) {
    bool stillused = FALSE;

    s = पूर्व_ps->sockets[i];
    क्रम(j = 0; j < ps->n; j++) {
      अगर(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        अवरोध;
      }
    }
    अगर(stillused)
      जारी;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    अगर(!entry)
      जारी;

    अगर(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(तथ्य, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(शून्य);
      जारी;
    }

    अगर(!conn && !mev_sh_entry_xfer_remove(entry, तथ्य)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(तथ्य, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(शून्य);
      जारी;
    }

    अगर(mev_sh_entry_user_गिनती(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, तथ्य, entry, s,
                                    पूर्व_ps->actions[i], 0);
      अगर(mresult)
        वापस mresult;
      CURL_TRC_M(तथ्य, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_गिनती(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    अन्यथा {
      mresult = mev_forget_socket(multi, तथ्य, s, "last user gone");
      अगर(mresult)
        वापस mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(पूर्व_ps, ps);
  वापस CURLM_OK;
}

अटल CURLMcode mev_assess(काष्ठा Curl_multi *multi,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा connectdata *conn)
{
  काष्ठा easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  अगर(!multi || !multi->socket_cb)
    वापस CURLM_OK;

  Curl_pollset_init(&ps);
  अगर(conn) {
    CURLcode परिणाम = Curl_conn_adjust_pollset(तथ्य, conn, &ps);
    अगर(परिणाम) {
      mresult = (परिणाम == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      जाओ out;
    }
  }
  अन्यथा
    Curl_multi_pollset(तथ्य, &ps);
  last_ps = mev_get_last_pollset(तथ्य, conn);

  अगर(!last_ps && ps.n) {
    अगर(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    अन्यथा
      last_ps = mev_add_new_xfer_pollset(तथ्य);
    अगर(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  अगर(last_ps)
    mresult = mev_pollset_diff(multi, तथ्य, conn, &ps, last_ps);
  अन्यथा
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  वापस mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य)
{
  वापस mev_assess(multi, तथ्य, शून्य);
}

CURLMcode Curl_multi_ev_assess_conn(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn)
{
  वापस mev_assess(multi, तथ्य, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(काष्ठा Curl_multi *multi,
                                         काष्ठा uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  अगर(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    करो {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(तथ्य) {
        mresult = Curl_multi_ev_assess_xfer(multi, तथ्य);
      }
    } जबतक(!mresult && Curl_uint32_bset_अग्र(set, mid, &mid));
  }
  वापस mresult;
}

CURLMcode Curl_multi_ev_assign(काष्ठा Curl_multi *multi,
                               curl_socket_t s,
                               व्योम *user_तथ्य)
{
  काष्ठा mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  अगर(!e)
    वापस CURLM_BAD_SOCKET;
  e->user_तथ्य = user_तथ्य;
  वापस CURLM_OK;
}

व्योम Curl_multi_ev_dirty_xfers(काष्ठा Curl_multi *multi,
                               curl_socket_t s)
{
  काष्ठा mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  अगर(entry) {
    काष्ठा Curl_easy *तथ्य;
    uint32_t mid;

    अगर(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      करो {
        तथ्य = Curl_multi_get_easy(multi, mid);
        अगर(तथ्य) {
          Curl_multi_mark_dirty(तथ्य);
        }
        अन्यथा {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } जबतक(Curl_uint32_spbset_अग्र(&entry->xfers, mid, &mid));
    }

    अगर(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

व्योम Curl_multi_ev_socket_done(काष्ठा Curl_multi *multi,
                               काष्ठा Curl_easy *तथ्य, curl_socket_t s)
{
  mev_forget_socket(multi, तथ्य, s, "socket done");
}

व्योम Curl_multi_ev_xfer_done(काष्ठा Curl_multi *multi,
                             काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(!तथ्य->conn); /* transfer should have been detached */
  (व्योम)mev_assess(multi, तथ्य, शून्य);
  Curl_meta_remove(तथ्य, CURL_META_MEV_POLLSET);
}

व्योम Curl_multi_ev_conn_done(काष्ठा Curl_multi *multi,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn)
{
  (व्योम)mev_assess(multi, तथ्य, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

व्योम Curl_multi_ev_init(काष्ठा Curl_multi *multi, माप_प्रकार hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

व्योम Curl_multi_ev_cleanup(काष्ठा Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
