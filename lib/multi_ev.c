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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "multi_ev.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "uint-bset.h"
#અન્તર્ભાવ "uint-spbset.h"
#અન્તર્ભાવ "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
સંરચના mev_sh_entry {
  સંરચના uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  સંરચના connectdata *conn; /* connection using this socket or NULL */
  શૂન્ય *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned પૂર્ણાંક action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned પૂર્ણાંક readers; /* this many transfers want to read */
  unsigned પૂર્ણાંક writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned પૂર્ણાંક magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
સ્થિર શૂન્ય mev_sh_entry_dtor(શૂન્ય *freethis)
{
  સંરચના mev_sh_entry *entry = (સંરચના mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
સ્થિર સંરચના mev_sh_entry *mev_sh_entry_get(સંરચના Curl_hash *sh,
                                             curl_socket_t s)
{
  જો(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    પાછા Curl_hash_pick(sh, (અચલ અક્ષર *)&s, કદ(curl_socket_t));
  }
  પાછા ખાલી;
}

/* make sure this socket is present in the hash for this handle */
સ્થિર સંરચના mev_sh_entry *mev_sh_entry_add(સંરચના Curl_hash *sh,
                                             curl_socket_t s)
{
  સંરચના mev_sh_entry *there = mev_sh_entry_get(sh, s);
  સંરચના mev_sh_entry *check;

  જો(there) {
    /* it is present, return fine */
    પાછા there;
  }

  /* not present, add it */
  check = curlx_calloc(1, કદ(સંરચના mev_sh_entry));
  જો(!check)
    પાછા ખાલી; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  જો(!Curl_hash_add(sh, (અક્ષર *)&s, કદ(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    પાછા ખાલી; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  પાછા check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
સ્થિર શૂન્ય mev_sh_entry_kill(સંરચના Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (અક્ષર *)&s, કદ(curl_socket_t));
}

સ્થિર size_t mev_sh_entry_user_ગણતરી(સંરચના mev_sh_entry *e)
{
  પાછા Curl_uint32_spbset_ગણતરી(&e->xfers) + (e->conn ? 1 : 0);
}

સ્થિર bool mev_sh_entry_xfer_known(સંરચના mev_sh_entry *e,
                                    સંરચના Curl_easy *data)
{
  પાછા Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

સ્થિર bool mev_sh_entry_conn_known(સંરચના mev_sh_entry *e,
                                    સંરચના connectdata *conn)
{
  પાછા (e->conn == conn);
}

સ્થિર શૂન્ય mev_pollset_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના easy_pollset *ps = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  જો(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

સ્થિર સંરચના easy_pollset *mev_add_new_conn_pollset(સંરચના connectdata *conn)
{
  સંરચના easy_pollset *ps;

  ps = Curl_pollset_create();
  જો(!ps)
    પાછા ખાલી;
  જો(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    પાછા ખાલી;
  પાછા ps;
}

સ્થિર સંરચના easy_pollset *mev_add_new_xfer_pollset(સંરચના Curl_easy *data)
{
  સંરચના easy_pollset *ps;

  ps = Curl_pollset_create();
  જો(!ps)
    પાછા ખાલી;
  જો(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    પાછા ખાલી;
  પાછા ps;
}

સ્થિર સંરચના easy_pollset *mev_get_last_pollset(સંરચના Curl_easy *data,
                                                 સંરચના connectdata *conn)
{
  જો(data) {
    જો(conn)
      પાછા Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    પાછા Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  પાછા ખાલી;
}

સ્થિર bool mev_sh_entry_xfer_add(સંરચના mev_sh_entry *e,
                                  સંરચના Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_ગણતરી(e) < 100000);
  પાછા Curl_uint32_spbset_add(&e->xfers, data->mid);
}

સ્થિર bool mev_sh_entry_conn_add(સંરચના mev_sh_entry *e,
                                  સંરચના connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_ગણતરી(e) < 100000);
  DEBUGASSERT(!e->conn);
  જો(e->conn)
    પાછા FALSE;
  e->conn = conn;
  પાછા TRUE;
}

સ્થિર bool mev_sh_entry_xfer_remove(સંરચના mev_sh_entry *e,
                                     સંરચના Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  જો(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  પાછા present;
}

સ્થિર bool mev_sh_entry_conn_remove(સંરચના mev_sh_entry *e,
                                     સંરચના connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  જો(e->conn == conn) {
    e->conn = ખાલી;
    પાછા TRUE;
  }
  પાછા FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
સ્થિર CURLMcode mev_forget_socket(સંરચના Curl_multi *multi,
                                   સંરચના Curl_easy *data,
                                   curl_socket_t s,
                                   અચલ અક્ષર *cause)
{
  સંરચના mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  પૂર્ણાંક rc = 0;

  જો(!entry) /* we never knew or already forgot about this socket */
    પાછા CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  જો(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    કરો {
      સંરચના Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      જો(sdata) {
        સંરચના easy_pollset *ps = mev_get_last_pollset(sdata, ખાલી);
        જો(ps)
          Curl_pollset_remove(ps, s);
      }
    } જ્યારે(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  જો(entry->conn) {
    સંરચના easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    જો(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  જો(entry->announced && multi->socket_cb) {
    સંરચના Curl_mapi_guard guard;

    NOVERBOSE((શૂન્ય)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    જો(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  જો(rc == -1) {
    multi->dead = TRUE;
    પાછા CURLM_ABORTED_BY_CALLBACK;
  }
  પાછા CURLM_OK;
}

સ્થિર CURLMcode mev_sh_entry_update(સંરચના Curl_multi *multi,
                                     સંરચના Curl_easy *data,
                                     સંરચના mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned અક્ષર last_action,
                                     unsigned અક્ષર cur_action)
{
  પૂર્ણાંક rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  જો(!multi->socket_cb)
    પાછા CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  જો(last_action == cur_action)  /* nothing from `data` changed */
    પાછા CURLM_OK;

  જો(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    જો(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  નહીંતર જો(cur_action & CURL_POLL_IN)
    entry->readers++;

  જો(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    જો(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  નહીંતર જો(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_ગણતરી(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_ગણતરી(entry));
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
  જો(((પૂર્ણાંક)entry->action == comboaction)) /* nothing for socket changed */
    પાછા CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  જો(rc == -1) {
    multi->dead = TRUE;
    પાછા CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  જો(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned પૂર્ણાંક)comboaction;
  }
  પાછા CURLM_OK;
}

સ્થિર CURLMcode mev_pollset_diff(સંરચના Curl_multi *multi,
                                  સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  સંરચના easy_pollset *ps,
                                  સંરચના easy_pollset *prev_ps)
{
  સંરચના mev_sh_entry *entry;
  curl_socket_t s;
  unsigned પૂર્ણાંક i, j;
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
  માટે(i = 0; i < ps->n; i++) {
    unsigned અક્ષર last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    જો(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      જો(!entry) /* fatal */
        પાછા CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    નહીંતર જો(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    નહીંતર {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    જો(first_time) {
      જો(conn) {
        જો(!mev_sh_entry_conn_add(entry, conn))
          પાછા CURLM_OUT_OF_MEMORY;
      }
      નહીંતર {
        જો(!mev_sh_entry_xfer_add(entry, data))
          પાછા CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_ગણતરી(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    નહીંતર {
      માટે(j = 0; j < prev_ps->n; j++) {
        જો(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          તોડો;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    જો(mresult)
      પાછા mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  માટે(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    માટે(j = 0; j < ps->n; j++) {
      જો(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        તોડો;
      }
    }
    જો(stillused)
      ચાલુ;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    જો(!entry)
      ચાલુ;

    જો(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ખાલી);
      ચાલુ;
    }

    જો(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ખાલી);
      ચાલુ;
    }

    જો(mev_sh_entry_user_ગણતરી(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      જો(mresult)
        પાછા mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_ગણતરી(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    નહીંતર {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      જો(mresult)
        પાછા mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  પાછા CURLM_OK;
}

સ્થિર CURLMcode mev_assess(સંરચના Curl_multi *multi,
                            સંરચના Curl_easy *data,
                            સંરચના connectdata *conn)
{
  સંરચના easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  જો(!multi || !multi->socket_cb)
    પાછા CURLM_OK;

  Curl_pollset_init(&ps);
  જો(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    જો(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      જાઓ out;
    }
  }
  નહીંતર
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  જો(!last_ps && ps.n) {
    જો(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    નહીંતર
      last_ps = mev_add_new_xfer_pollset(data);
    જો(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

  જો(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  નહીંતર
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  પાછા mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(સંરચના Curl_multi *multi,
                                    સંરચના Curl_easy *data)
{
  પાછા mev_assess(multi, data, ખાલી);
}

CURLMcode Curl_multi_ev_assess_conn(સંરચના Curl_multi *multi,
                                    સંરચના Curl_easy *data,
                                    સંરચના connectdata *conn)
{
  પાછા mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(સંરચના Curl_multi *multi,
                                         સંરચના uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  જો(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    કરો {
      સંરચના Curl_easy *data = Curl_multi_get_easy(multi, mid);
      જો(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } જ્યારે(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  પાછા mresult;
}

CURLMcode Curl_multi_ev_assign(સંરચના Curl_multi *multi,
                               curl_socket_t s,
                               શૂન્ય *user_data)
{
  સંરચના mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  જો(!e)
    પાછા CURLM_BAD_SOCKET;
  e->user_data = user_data;
  પાછા CURLM_OK;
}

શૂન્ય Curl_multi_ev_dirty_xfers(સંરચના Curl_multi *multi,
                               curl_socket_t s)
{
  સંરચના mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  જો(entry) {
    સંરચના Curl_easy *data;
    uint32_t mid;

    જો(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      કરો {
        data = Curl_multi_get_easy(multi, mid);
        જો(data) {
          Curl_multi_mark_dirty(data);
        }
        નહીંતર {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } જ્યારે(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    જો(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

શૂન્ય Curl_multi_ev_socket_done(સંરચના Curl_multi *multi,
                               સંરચના Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

શૂન્ય Curl_multi_ev_xfer_done(સંરચના Curl_multi *multi,
                             સંરચના Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (શૂન્ય)mev_assess(multi, data, ખાલી);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

શૂન્ય Curl_multi_ev_conn_done(સંરચના Curl_multi *multi,
                             સંરચના Curl_easy *data,
                             સંરચના connectdata *conn)
{
  (શૂન્ય)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

શૂન્ય Curl_multi_ev_init(સંરચના Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

શૂન્ય Curl_multi_ev_cleanup(સંરચના Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
