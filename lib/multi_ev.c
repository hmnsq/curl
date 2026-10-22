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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "multi_ev.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "uint-bset.h"
#ଅନ୍ତର୍ଭାଵ "uint-spbset.h"
#ଅନ୍ତର୍ଭାଵ "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ସଂରଚନା mev_sh_entry {
  ସଂରଚନା uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ସଂରଚନା connectdata *conn; /* connection using this socket or NULL */
  ଶୂନ୍ୟ *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା readers; /* this many transfers want to read */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ସ୍ଥିର ଶୂନ୍ୟ mev_sh_entry_dtor(ଶୂନ୍ୟ *freethis)
{
  ସଂରଚନା mev_sh_entry *entry = (ସଂରଚନା mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ସ୍ଥିର ସଂରଚନା mev_sh_entry *mev_sh_entry_get(ସଂରଚନା Curl_hash *sh,
                                             curl_socket_t s)
{
  ଯଦି(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ଫେରନ୍ତୁ Curl_hash_pick(sh, (ଅଚଳ ଅକ୍ଷର *)&s, ଆକାର(curl_socket_t));
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

/* make sure this socket is present in the hash for this handle */
ସ୍ଥିର ସଂରଚନା mev_sh_entry *mev_sh_entry_add(ସଂରଚନା Curl_hash *sh,
                                             curl_socket_t s)
{
  ସଂରଚନା mev_sh_entry *there = mev_sh_entry_get(sh, s);
  ସଂରଚନା mev_sh_entry *check;

  ଯଦି(there) {
    /* it is present, return fine */
    ଫେରନ୍ତୁ there;
  }

  /* not present, add it */
  check = curlx_calloc(1, ଆକାର(ସଂରଚନା mev_sh_entry));
  ଯଦି(!check)
    ଫେରନ୍ତୁ ଖାଲି; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  ଯଦି(!Curl_hash_add(sh, (ଅକ୍ଷର *)&s, ଆକାର(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    ଫେରନ୍ତୁ ଖାଲି; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  ଫେରନ୍ତୁ check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ସ୍ଥିର ଶୂନ୍ୟ mev_sh_entry_kill(ସଂରଚନା Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (ଅକ୍ଷର *)&s, ଆକାର(curl_socket_t));
}

ସ୍ଥିର size_t mev_sh_entry_user_ଗଣନା(ସଂରଚନା mev_sh_entry *e)
{
  ଫେରନ୍ତୁ Curl_uint32_spbset_ଗଣନା(&e->xfers) + (e->conn ? 1 : 0);
}

ସ୍ଥିର bool mev_sh_entry_xfer_known(ସଂରଚନା mev_sh_entry *e,
                                    ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

ସ୍ଥିର bool mev_sh_entry_conn_known(ସଂରଚନା mev_sh_entry *e,
                                    ସଂରଚନା connectdata *conn)
{
  ଫେରନ୍ତୁ (e->conn == conn);
}

ସ୍ଥିର ଶୂନ୍ୟ mev_pollset_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା easy_pollset *ps = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  ଯଦି(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ସ୍ଥିର ସଂରଚନା easy_pollset *mev_add_new_conn_pollset(ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା easy_pollset *ps;

  ps = Curl_pollset_create();
  ଯଦି(!ps)
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ ps;
}

ସ୍ଥିର ସଂରଚନା easy_pollset *mev_add_new_xfer_pollset(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା easy_pollset *ps;

  ps = Curl_pollset_create();
  ଯଦି(!ps)
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ ps;
}

ସ୍ଥିର ସଂରଚନା easy_pollset *mev_get_last_pollset(ସଂରଚନା Curl_easy *data,
                                                 ସଂରଚନା connectdata *conn)
{
  ଯଦି(data) {
    ଯଦି(conn)
      ଫେରନ୍ତୁ Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ଫେରନ୍ତୁ Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର bool mev_sh_entry_xfer_add(ସଂରଚନା mev_sh_entry *e,
                                  ସଂରଚନା Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_ଗଣନା(e) < 100000);
  ଫେରନ୍ତୁ Curl_uint32_spbset_add(&e->xfers, data->mid);
}

ସ୍ଥିର bool mev_sh_entry_conn_add(ସଂରଚନା mev_sh_entry *e,
                                  ସଂରଚନା connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_ଗଣନା(e) < 100000);
  DEBUGASSERT(!e->conn);
  ଯଦି(e->conn)
    ଫେରନ୍ତୁ FALSE;
  e->conn = conn;
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର bool mev_sh_entry_xfer_remove(ସଂରଚନା mev_sh_entry *e,
                                     ସଂରଚନା Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  ଯଦି(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  ଫେରନ୍ତୁ present;
}

ସ୍ଥିର bool mev_sh_entry_conn_remove(ସଂରଚନା mev_sh_entry *e,
                                     ସଂରଚନା connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  ଯଦି(e->conn == conn) {
    e->conn = ଖାଲି;
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ସ୍ଥିର CURLMcode mev_forget_socket(ସଂରଚନା Curl_multi *multi,
                                   ସଂରଚନା Curl_easy *data,
                                   curl_socket_t s,
                                   ଅଚଳ ଅକ୍ଷର *cause)
{
  ସଂରଚନା mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

  ଯଦି(!entry) /* we never knew or already forgot about this socket */
    ଫେରନ୍ତୁ CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  ଯଦି(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    କର {
      ସଂରଚନା Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      ଯଦି(sdata) {
        ସଂରଚନା easy_pollset *ps = mev_get_last_pollset(sdata, ଖାଲି);
        ଯଦି(ps)
          Curl_pollset_remove(ps, s);
      }
    } ଯେତେବେଳେ(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  ଯଦି(entry->conn) {
    ସଂରଚନା easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    ଯଦି(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  ଯଦି(entry->announced && multi->socket_cb) {
    ସଂରଚନା Curl_mapi_guard guard;

    NOVERBOSE((ଶୂନ୍ୟ)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    ଯଦି(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  ଯଦି(rc == -1) {
    multi->dead = TRUE;
    ଫେରନ୍ତୁ CURLM_ABORTED_BY_CALLBACK;
  }
  ଫେରନ୍ତୁ CURLM_OK;
}

ସ୍ଥିର CURLMcode mev_sh_entry_update(ସଂରଚନା Curl_multi *multi,
                                     ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned ଅକ୍ଷର last_action,
                                     unsigned ଅକ୍ଷର cur_action)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  ଯଦି(!multi->socket_cb)
    ଫେରନ୍ତୁ CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  ଯଦି(last_action == cur_action)  /* nothing from `data` changed */
    ଫେରନ୍ତୁ CURLM_OK;

  ଯଦି(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    ଯଦି(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  ଅନ୍ୟଥା ଯଦି(cur_action & CURL_POLL_IN)
    entry->readers++;

  ଯଦି(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    ଯଦି(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  ଅନ୍ୟଥା ଯଦି(cur_action & CURL_POLL_OUT)
    entry->writers++;

  DEBUGASSERT(entry->readers <= mev_sh_entry_user_ଗଣନା(entry));
  DEBUGASSERT(entry->writers <= mev_sh_entry_user_ଗଣନା(entry));
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
  ଯଦି(((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)entry->action == comboaction)) /* nothing for socket changed */
    ଫେରନ୍ତୁ CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  ଯଦି(rc == -1) {
    multi->dead = TRUE;
    ଫେରନ୍ତୁ CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  ଯଦି(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)comboaction;
  }
  ଫେରନ୍ତୁ CURLM_OK;
}

ସ୍ଥିର CURLMcode mev_pollset_diff(ସଂରଚନା Curl_multi *multi,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  ସଂରଚନା easy_pollset *ps,
                                  ସଂରଚନା easy_pollset *prev_ps)
{
  ସଂରଚନା mev_sh_entry *entry;
  curl_socket_t s;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;
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
  ପାଇଁ(i = 0; i < ps->n; i++) {
    unsigned ଅକ୍ଷର last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    ଯଦି(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      ଯଦି(!entry) /* fatal */
        ଫେରନ୍ତୁ CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    ଅନ୍ୟଥା ଯଦି(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    ଅନ୍ୟଥା {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    ଯଦି(first_time) {
      ଯଦି(conn) {
        ଯଦି(!mev_sh_entry_conn_add(entry, conn))
          ଫେରନ୍ତୁ CURLM_OUT_OF_MEMORY;
      }
      ଅନ୍ୟଥା {
        ଯଦି(!mev_sh_entry_xfer_add(entry, data))
          ଫେରନ୍ତୁ CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_ଗଣନା(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    ଅନ୍ୟଥା {
      ପାଇଁ(j = 0; j < prev_ps->n; j++) {
        ଯଦି(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          ଭାଙ୍ଗ;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    ଯଦି(mresult)
      ଫେରନ୍ତୁ mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  ପାଇଁ(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    ପାଇଁ(j = 0; j < ps->n; j++) {
      ଯଦି(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        ଭାଙ୍ଗ;
      }
    }
    ଯଦି(stillused)
      ଜାରି;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    ଯଦି(!entry)
      ଜାରି;

    ଯଦି(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ଖାଲି);
      ଜାରି;
    }

    ଯଦି(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ଖାଲି);
      ଜାରି;
    }

    ଯଦି(mev_sh_entry_user_ଗଣନା(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      ଯଦି(mresult)
        ଫେରନ୍ତୁ mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_ଗଣନା(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    ଅନ୍ୟଥା {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      ଯଦି(mresult)
        ଫେରନ୍ତୁ mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  ଫେରନ୍ତୁ CURLM_OK;
}

ସ୍ଥିର CURLMcode mev_assess(ସଂରଚନା Curl_multi *multi,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  ଯଦି(!multi || !multi->socket_cb)
    ଫେରନ୍ତୁ CURLM_OK;

  Curl_pollset_init(&ps);
  ଯଦି(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    ଯଦି(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  ଯଦି(!last_ps && ps.n) {
    ଯଦି(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    ଅନ୍ୟଥା
      last_ps = mev_add_new_xfer_pollset(data);
    ଯଦି(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

  ଯଦି(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  ଅନ୍ୟଥା
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  ଫେରନ୍ତୁ mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ସଂରଚନା Curl_multi *multi,
                                    ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ mev_assess(multi, data, ଖାଲି);
}

CURLMcode Curl_multi_ev_assess_conn(ସଂରଚନା Curl_multi *multi,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn)
{
  ଫେରନ୍ତୁ mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ସଂରଚନା Curl_multi *multi,
                                         ସଂରଚନା uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  ଯଦି(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    କର {
      ସଂରଚନା Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ଯଦି(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } ଯେତେବେଳେ(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  ଫେରନ୍ତୁ mresult;
}

CURLMcode Curl_multi_ev_assign(ସଂରଚନା Curl_multi *multi,
                               curl_socket_t s,
                               ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  ଯଦି(!e)
    ଫେରନ୍ତୁ CURLM_BAD_SOCKET;
  e->user_data = user_data;
  ଫେରନ୍ତୁ CURLM_OK;
}

ଶୂନ୍ୟ Curl_multi_ev_dirty_xfers(ସଂରଚନା Curl_multi *multi,
                               curl_socket_t s)
{
  ସଂରଚନା mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  ଯଦି(entry) {
    ସଂରଚନା Curl_easy *data;
    uint32_t mid;

    ଯଦି(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      କର {
        data = Curl_multi_get_easy(multi, mid);
        ଯଦି(data) {
          Curl_multi_mark_dirty(data);
        }
        ଅନ୍ୟଥା {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } ଯେତେବେଳେ(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    ଯଦି(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

ଶୂନ୍ୟ Curl_multi_ev_socket_done(ସଂରଚନା Curl_multi *multi,
                               ସଂରଚନା Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

ଶୂନ୍ୟ Curl_multi_ev_xfer_done(ସଂରଚନା Curl_multi *multi,
                             ସଂରଚନା Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (ଶୂନ୍ୟ)mev_assess(multi, data, ଖାଲି);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

ଶୂନ୍ୟ Curl_multi_ev_conn_done(ସଂରଚନା Curl_multi *multi,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn)
{
  (ଶୂନ୍ୟ)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

ଶୂନ୍ୟ Curl_multi_ev_init(ସଂରଚନା Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

ଶୂନ୍ୟ Curl_multi_ev_cleanup(ସଂରଚନା Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
