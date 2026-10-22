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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "multi_ev.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "uint-bset.h"
#ಅನ್ತರ್ಭಾವ "uint-spbset.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ಸಂರಚನಾ mev_sh_entry {
  ಸಂರಚನಾ uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ಸಂರಚನಾ connectdata *conn; /* connection using this socket or NULL */
  ಶೂನ್ಯ *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned ಪೂರ್ಣಾಙ್ಕ action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned ಪೂರ್ಣಾಙ್ಕ readers; /* this many transfers want to read */
  unsigned ಪೂರ್ಣಾಙ್ಕ writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned ಪೂರ್ಣಾಙ್ಕ magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ಸ್ಥಿರ ಶೂನ್ಯ mev_sh_entry_dtor(ಶೂನ್ಯ *freethis)
{
  ಸಂರಚನಾ mev_sh_entry *entry = (ಸಂರಚನಾ mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ಸ್ಥಿರ ಸಂರಚನಾ mev_sh_entry *mev_sh_entry_get(ಸಂರಚನಾ Curl_hash *sh,
                                             curl_socket_t s)
{
  ಯದಿ(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ಪ್ರತ್ಯಾಗಮ Curl_hash_pick(sh, (ಧ್ರುವ ವರ್ಣ *)&s, ಪರಿಮಾಣ(curl_socket_t));
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/* make sure this socket is present in the hash for this handle */
ಸ್ಥಿರ ಸಂರಚನಾ mev_sh_entry *mev_sh_entry_add(ಸಂರಚನಾ Curl_hash *sh,
                                             curl_socket_t s)
{
  ಸಂರಚನಾ mev_sh_entry *there = mev_sh_entry_get(sh, s);
  ಸಂರಚನಾ mev_sh_entry *check;

  ಯದಿ(there) {
    /* it is present, return fine */
    ಪ್ರತ್ಯಾಗಮ there;
  }

  /* not present, add it */
  check = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mev_sh_entry));
  ಯದಿ(!check)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  ಯದಿ(!Curl_hash_add(sh, (ವರ್ಣ *)&s, ಪರಿಮಾಣ(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  ಪ್ರತ್ಯಾಗಮ check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ಸ್ಥಿರ ಶೂನ್ಯ mev_sh_entry_kill(ಸಂರಚನಾ Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (ವರ್ಣ *)&s, ಪರಿಮಾಣ(curl_socket_t));
}

ಸ್ಥಿರ size_t mev_sh_entry_user_ಎಣಿಕೆ(ಸಂರಚನಾ mev_sh_entry *e)
{
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_spbset_ಎಣಿಕೆ(&e->xfers) + (e->conn ? 1 : 0);
}

ಸ್ಥಿರ bool mev_sh_entry_xfer_known(ಸಂರಚನಾ mev_sh_entry *e,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

ಸ್ಥಿರ bool mev_sh_entry_conn_known(ಸಂರಚನಾ mev_sh_entry *e,
                                    ಸಂರಚನಾ connectdata *conn)
{
  ಪ್ರತ್ಯಾಗಮ (e->conn == conn);
}

ಸ್ಥಿರ ಶೂನ್ಯ mev_pollset_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ easy_pollset *ps = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  ಯದಿ(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ಸ್ಥಿರ ಸಂರಚನಾ easy_pollset *mev_add_new_conn_pollset(ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ easy_pollset *ps;

  ps = Curl_pollset_create();
  ಯದಿ(!ps)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಯದಿ(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ ps;
}

ಸ್ಥಿರ ಸಂರಚನಾ easy_pollset *mev_add_new_xfer_pollset(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ easy_pollset *ps;

  ps = Curl_pollset_create();
  ಯದಿ(!ps)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಯದಿ(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ ps;
}

ಸ್ಥಿರ ಸಂರಚನಾ easy_pollset *mev_get_last_pollset(ಸಂರಚನಾ Curl_easy *data,
                                                 ಸಂರಚನಾ connectdata *conn)
{
  ಯದಿ(data) {
    ಯದಿ(conn)
      ಪ್ರತ್ಯಾಗಮ Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ಪ್ರತ್ಯಾಗಮ Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ bool mev_sh_entry_xfer_add(ಸಂರಚನಾ mev_sh_entry *e,
                                  ಸಂರಚನಾ Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_ಎಣಿಕೆ(e) < 100000);
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_spbset_add(&e->xfers, data->mid);
}

ಸ್ಥಿರ bool mev_sh_entry_conn_add(ಸಂರಚನಾ mev_sh_entry *e,
                                  ಸಂರಚನಾ connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_ಎಣಿಕೆ(e) < 100000);
  DEBUGASSERT(!e->conn);
  ಯದಿ(e->conn)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  e->conn = conn;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ bool mev_sh_entry_xfer_remove(ಸಂರಚನಾ mev_sh_entry *e,
                                     ಸಂರಚನಾ Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  ಯದಿ(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  ಪ್ರತ್ಯಾಗಮ present;
}

ಸ್ಥಿರ bool mev_sh_entry_conn_remove(ಸಂರಚನಾ mev_sh_entry *e,
                                     ಸಂರಚನಾ connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  ಯದಿ(e->conn == conn) {
    e->conn = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ಸ್ಥಿರ CURLMcode mev_forget_socket(ಸಂರಚನಾ Curl_multi *multi,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   curl_socket_t s,
                                   ಧ್ರುವ ವರ್ಣ *cause)
{
  ಸಂರಚನಾ mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;

  ಯದಿ(!entry) /* we never knew or already forgot about this socket */
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  ಯದಿ(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    ಕುರು {
      ಸಂರಚನಾ Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      ಯದಿ(sdata) {
        ಸಂರಚನಾ easy_pollset *ps = mev_get_last_pollset(sdata, ರಿಕ್ತ);
        ಯದಿ(ps)
          Curl_pollset_remove(ps, s);
      }
    } ಯಾವತ್(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  ಯದಿ(entry->conn) {
    ಸಂರಚನಾ easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    ಯದಿ(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  ಯದಿ(entry->announced && multi->socket_cb) {
    ಸಂರಚನಾ Curl_mapi_guard guard;

    NOVERBOSE((ಶೂನ್ಯ)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    ಯದಿ(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  ಯದಿ(rc == -1) {
    multi->dead = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLM_ABORTED_BY_CALLBACK;
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode mev_sh_entry_update(ಸಂರಚನಾ Curl_multi *multi,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned ವರ್ಣ last_action,
                                     unsigned ವರ್ಣ cur_action)
{
  ಪೂರ್ಣಾಙ್ಕ rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  ಯದಿ(!multi->socket_cb)
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  ಯದಿ(last_action == cur_action)  /* nothing from `data` changed */
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  ಯದಿ(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    ಯದಿ(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  ಅನ್ಯಥಾ ಯದಿ(cur_action & CURL_POLL_IN)
    entry->readers++;

  ಯದಿ(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    ಯದಿ(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  ಅನ್ಯಥಾ ಯದಿ(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_ಎಣಿಕೆ(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_ಎಣಿಕೆ(entry));
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
  ಯದಿ(((ಪೂರ್ಣಾಙ್ಕ)entry->action == comboaction)) /* nothing for socket changed */
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  ಯದಿ(rc == -1) {
    multi->dead = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  ಯದಿ(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned ಪೂರ್ಣಾಙ್ಕ)comboaction;
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode mev_pollset_diff(ಸಂರಚನಾ Curl_multi *multi,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  ಸಂರಚನಾ easy_pollset *ps,
                                  ಸಂರಚನಾ easy_pollset *prev_ps)
{
  ಸಂರಚನಾ mev_sh_entry *entry;
  curl_socket_t s;
  unsigned ಪೂರ್ಣಾಙ್ಕ i, j;
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
  ಪ್ರತಿ(i = 0; i < ps->n; i++) {
    unsigned ವರ್ಣ last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    ಯದಿ(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      ಯದಿ(!entry) /* fatal */
        ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    ಅನ್ಯಥಾ ಯದಿ(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    ಅನ್ಯಥಾ {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    ಯದಿ(first_time) {
      ಯದಿ(conn) {
        ಯದಿ(!mev_sh_entry_conn_add(entry, conn))
          ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ(!mev_sh_entry_xfer_add(entry, data))
          ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_ಎಣಿಕೆ(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    ಅನ್ಯಥಾ {
      ಪ್ರತಿ(j = 0; j < prev_ps->n; j++) {
        ಯದಿ(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          ಭಙ್ಗ;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    ಯದಿ(mresult)
      ಪ್ರತ್ಯಾಗಮ mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  ಪ್ರತಿ(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    ಪ್ರತಿ(j = 0; j < ps->n; j++) {
      ಯದಿ(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        ಭಙ್ಗ;
      }
    }
    ಯದಿ(stillused)
      ಅನುವರ್ತ;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    ಯದಿ(!entry)
      ಅನುವರ್ತ;

    ಯದಿ(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ರಿಕ್ತ);
      ಅನುವರ್ತ;
    }

    ಯದಿ(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ರಿಕ್ತ);
      ಅನುವರ್ತ;
    }

    ಯದಿ(mev_sh_entry_user_ಎಣಿಕೆ(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      ಯದಿ(mresult)
        ಪ್ರತ್ಯಾಗಮ mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_ಎಣಿಕೆ(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    ಅನ್ಯಥಾ {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      ಯದಿ(mresult)
        ಪ್ರತ್ಯಾಗಮ mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode mev_assess(ಸಂರಚನಾ Curl_multi *multi,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  ಯದಿ(!multi || !multi->socket_cb)
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  Curl_pollset_init(&ps);
  ಯದಿ(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    ಯದಿ(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  ಯದಿ(!last_ps && ps.n) {
    ಯದಿ(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    ಅನ್ಯಥಾ
      last_ps = mev_add_new_xfer_pollset(data);
    ಯದಿ(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  ಅನ್ಯಥಾ
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ mev_assess(multi, data, ರಿಕ್ತ);
}

CURLMcode Curl_multi_ev_assess_conn(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn)
{
  ಪ್ರತ್ಯಾಗಮ mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ಸಂರಚನಾ Curl_multi *multi,
                                         ಸಂರಚನಾ uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  ಯದಿ(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    ಕುರು {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } ಯಾವತ್(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode Curl_multi_ev_assign(ಸಂರಚನಾ Curl_multi *multi,
                               curl_socket_t s,
                               ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  ಯದಿ(!e)
    ಪ್ರತ್ಯಾಗಮ CURLM_BAD_SOCKET;
  e->user_data = user_data;
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಶೂನ್ಯ Curl_multi_ev_dirty_xfers(ಸಂರಚನಾ Curl_multi *multi,
                               curl_socket_t s)
{
  ಸಂರಚನಾ mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  ಯದಿ(entry) {
    ಸಂರಚನಾ Curl_easy *data;
    uint32_t mid;

    ಯದಿ(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      ಕುರು {
        data = Curl_multi_get_easy(multi, mid);
        ಯದಿ(data) {
          Curl_multi_mark_dirty(data);
        }
        ಅನ್ಯಥಾ {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } ಯಾವತ್(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    ಯದಿ(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

ಶೂನ್ಯ Curl_multi_ev_socket_done(ಸಂರಚನಾ Curl_multi *multi,
                               ಸಂರಚನಾ Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

ಶೂನ್ಯ Curl_multi_ev_xfer_done(ಸಂರಚನಾ Curl_multi *multi,
                             ಸಂರಚನಾ Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (ಶೂನ್ಯ)mev_assess(multi, data, ರಿಕ್ತ);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

ಶೂನ್ಯ Curl_multi_ev_conn_done(ಸಂರಚನಾ Curl_multi *multi,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn)
{
  (ಶೂನ್ಯ)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

ಶೂನ್ಯ Curl_multi_ev_init(ಸಂರಚನಾ Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

ಶೂನ್ಯ Curl_multi_ev_cleanup(ಸಂರಚನಾ Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
