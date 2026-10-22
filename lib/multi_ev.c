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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "multi_ev.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "uint-bset.h"
#অন্তর্ভাব "uint-spbset.h"
#অন্তর্ভাব "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
কাঠামো mev_sh_entry {
  কাঠামো uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  কাঠামো connectdata *conn; /* connection using this socket or NULL */
  শূন্য *user_তথ্য;      /* libcurl app data via curl_multi_assign() */
  unsigned পূর্ণসংখ্যা action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned পূর্ণসংখ্যা readers; /* this many transfers want to read */
  unsigned পূর্ণসংখ্যা writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned পূর্ণসংখ্যা magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
স্থির শূন্য mev_sh_entry_dtor(শূন্য *freethis)
{
  কাঠামো mev_sh_entry *entry = (কাঠামো mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
স্থির কাঠামো mev_sh_entry *mev_sh_entry_get(কাঠামো Curl_hash *sh,
                                             curl_socket_t s)
{
  যদি(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ফেরত Curl_hash_pick(sh, (ধ্রুবক অক্ষর *)&s, আকার(curl_socket_t));
  }
  ফেরত ফাঁকা;
}

/* make sure this socket is present in the hash for this handle */
স্থির কাঠামো mev_sh_entry *mev_sh_entry_add(কাঠামো Curl_hash *sh,
                                             curl_socket_t s)
{
  কাঠামো mev_sh_entry *there = mev_sh_entry_get(sh, s);
  কাঠামো mev_sh_entry *check;

  যদি(there) {
    /* it is present, return fine */
    ফেরত there;
  }

  /* not present, add it */
  check = curlx_calloc(1, আকার(কাঠামো mev_sh_entry));
  যদি(!check)
    ফেরত ফাঁকা; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  যদি(!Curl_hash_add(sh, (অক্ষর *)&s, আকার(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    ফেরত ফাঁকা; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  ফেরত check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
স্থির শূন্য mev_sh_entry_kill(কাঠামো Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (অক্ষর *)&s, আকার(curl_socket_t));
}

স্থির size_t mev_sh_entry_user_গুনতি(কাঠামো mev_sh_entry *e)
{
  ফেরত Curl_uint32_spbset_গুনতি(&e->xfers) + (e->conn ? 1 : 0);
}

স্থির bool mev_sh_entry_xfer_known(কাঠামো mev_sh_entry *e,
                                    কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_uint32_spbset_contains(&e->xfers, তথ্য->mid);
}

স্থির bool mev_sh_entry_conn_known(কাঠামো mev_sh_entry *e,
                                    কাঠামো connectdata *conn)
{
  ফেরত (e->conn == conn);
}

স্থির শূন্য mev_pollset_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো easy_pollset *ps = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  যদি(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

স্থির কাঠামো easy_pollset *mev_add_new_conn_pollset(কাঠামো connectdata *conn)
{
  কাঠামো easy_pollset *ps;

  ps = Curl_pollset_create();
  যদি(!ps)
    ফেরত ফাঁকা;
  যদি(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ফেরত ফাঁকা;
  ফেরত ps;
}

স্থির কাঠামো easy_pollset *mev_add_new_xfer_pollset(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো easy_pollset *ps;

  ps = Curl_pollset_create();
  যদি(!ps)
    ফেরত ফাঁকা;
  যদি(Curl_meta_set(তথ্য, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ফেরত ফাঁকা;
  ফেরত ps;
}

স্থির কাঠামো easy_pollset *mev_get_last_pollset(কাঠামো Curl_easy *তথ্য,
                                                 কাঠামো connectdata *conn)
{
  যদি(তথ্য) {
    যদি(conn)
      ফেরত Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ফেরত Curl_meta_get(তথ্য, CURL_META_MEV_POLLSET);
  }
  ফেরত ফাঁকা;
}

স্থির bool mev_sh_entry_xfer_add(কাঠামো mev_sh_entry *e,
                                  কাঠামো Curl_easy *তথ্য)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_গুনতি(e) < 100000);
  ফেরত Curl_uint32_spbset_add(&e->xfers, তথ্য->mid);
}

স্থির bool mev_sh_entry_conn_add(কাঠামো mev_sh_entry *e,
                                  কাঠামো connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_গুনতি(e) < 100000);
  DEBUGASSERT(!e->conn);
  যদি(e->conn)
    ফেরত FALSE;
  e->conn = conn;
  ফেরত TRUE;
}

স্থির bool mev_sh_entry_xfer_remove(কাঠামো mev_sh_entry *e,
                                     কাঠামো Curl_easy *তথ্য)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, তথ্য->mid);
  যদি(present)
    Curl_uint32_spbset_remove(&e->xfers, তথ্য->mid);
  ফেরত present;
}

স্থির bool mev_sh_entry_conn_remove(কাঠামো mev_sh_entry *e,
                                     কাঠামো connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  যদি(e->conn == conn) {
    e->conn = ফাঁকা;
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
স্থির CURLMcode mev_forget_socket(কাঠামো Curl_multi *multi,
                                   কাঠামো Curl_easy *তথ্য,
                                   curl_socket_t s,
                                   ধ্রুবক অক্ষর *cause)
{
  কাঠামো mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  পূর্ণসংখ্যা rc = 0;

  যদি(!entry) /* we never knew or already forgot about this socket */
    ফেরত CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  যদি(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    করো {
      কাঠামো Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      যদি(sdata) {
        কাঠামো easy_pollset *ps = mev_get_last_pollset(sdata, ফাঁকা);
        যদি(ps)
          Curl_pollset_remove(ps, s);
      }
    } যখন(Curl_uint32_spbset_পরবর্তী(&entry->xfers, mid, &mid));
  }

  যদি(entry->conn) {
    কাঠামো easy_pollset *ps = mev_get_last_pollset(তথ্য, entry->conn);
    যদি(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  যদি(entry->announced && multi->socket_cb) {
    কাঠামো Curl_mapi_guard guard;

    NOVERBOSE((শূন্য)cause);
    CURL_TRC_M(তথ্য, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(তথ্য, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_তথ্য);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    যদি(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  যদি(rc == -1) {
    multi->dead = TRUE;
    ফেরত CURLM_ABORTED_BY_CALLBACK;
  }
  ফেরত CURLM_OK;
}

স্থির CURLMcode mev_sh_entry_update(কাঠামো Curl_multi *multi,
                                     কাঠামো Curl_easy *তথ্য,
                                     কাঠামো mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned অক্ষর last_action,
                                     unsigned অক্ষর cur_action)
{
  পূর্ণসংখ্যা rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  যদি(!multi->socket_cb)
    ফেরত CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  যদি(last_action == cur_action)  /* nothing from `data` changed */
    ফেরত CURLM_OK;

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

  CURL_TRC_M(তথ্য, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (last_action & CURL_POLL_IN) ? "IN" : "",
             (last_action & CURL_POLL_OUT) ? "OUT" : "",
             (cur_action & CURL_POLL_IN) ? "IN" : "",
             (cur_action & CURL_POLL_OUT) ? "OUT" : "",
             entry->readers, entry->writers);

  comboaction = (entry->writers ? CURL_POLL_OUT : 0) |
                (entry->readers ? CURL_POLL_IN : 0);
  যদি(((পূর্ণসংখ্যা)entry->action == comboaction)) /* nothing for socket changed */
    ফেরত CURLM_OK;

  CURL_TRC_M(তথ্য, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(তথ্য, s, comboaction, multi->socket_userp,
                          entry->user_তথ্য);
    CURL_CBAPI_MULTI_END(&guard);
  }
  যদি(rc == -1) {
    multi->dead = TRUE;
    ফেরত CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  যদি(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned পূর্ণসংখ্যা)comboaction;
  }
  ফেরত CURLM_OK;
}

স্থির CURLMcode mev_pollset_diff(কাঠামো Curl_multi *multi,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  কাঠামো easy_pollset *ps,
                                  কাঠামো easy_pollset *পূর্ব_ps)
{
  কাঠামো mev_sh_entry *entry;
  curl_socket_t s;
  unsigned পূর্ণসংখ্যা i, j;
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
  DEBUGASSERT(পূর্ব_ps);

  /* Handle changes to sockets the transfer is interested in. */
  জন্য(i = 0; i < ps->n; i++) {
    unsigned অক্ষর last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    যদি(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      যদি(!entry) /* fatal */
        ফেরত CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(তথ্য, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    অন্যথা যদি(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    অন্যথা {
      first_time = !mev_sh_entry_xfer_known(entry, তথ্য);
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
          ফেরত CURLM_OUT_OF_MEMORY;
      }
      অন্যথা {
        যদি(!mev_sh_entry_xfer_add(entry, তথ্য))
          ফেরত CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(তথ্য, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : তথ্য->mid,
                 Curl_uint32_spbset_গুনতি(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    অন্যথা {
      জন্য(j = 0; j < পূর্ব_ps->n; j++) {
        যদি(s == পূর্ব_ps->sockets[j]) {
          last_action = পূর্ব_ps->actions[j];
          বিরতি;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, তথ্য, entry, s,
                                  last_action, ps->actions[i]);
    যদি(mresult)
      ফেরত mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  জন্য(i = 0; i < পূর্ব_ps->n; i++) {
    bool stillused = FALSE;

    s = পূর্ব_ps->sockets[i];
    জন্য(j = 0; j < ps->n; j++) {
      যদি(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        বিরতি;
      }
    }
    যদি(stillused)
      চালু;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    যদি(!entry)
      চালু;

    যদি(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(তথ্য, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ফাঁকা);
      চালু;
    }

    যদি(!conn && !mev_sh_entry_xfer_remove(entry, তথ্য)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(তথ্য, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ফাঁকা);
      চালু;
    }

    যদি(mev_sh_entry_user_গুনতি(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, তথ্য, entry, s,
                                    পূর্ব_ps->actions[i], 0);
      যদি(mresult)
        ফেরত mresult;
      CURL_TRC_M(তথ্য, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_গুনতি(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    অন্যথা {
      mresult = mev_forget_socket(multi, তথ্য, s, "last user gone");
      যদি(mresult)
        ফেরত mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(পূর্ব_ps, ps);
  ফেরত CURLM_OK;
}

স্থির CURLMcode mev_assess(কাঠামো Curl_multi *multi,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো connectdata *conn)
{
  কাঠামো easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  যদি(!multi || !multi->socket_cb)
    ফেরত CURLM_OK;

  Curl_pollset_init(&ps);
  যদি(conn) {
    CURLcode ফলাফল = Curl_conn_adjust_pollset(তথ্য, conn, &ps);
    যদি(ফলাফল) {
      mresult = (ফলাফল == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      যাও out;
    }
  }
  অন্যথা
    Curl_multi_pollset(তথ্য, &ps);
  last_ps = mev_get_last_pollset(তথ্য, conn);

  যদি(!last_ps && ps.n) {
    যদি(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    অন্যথা
      last_ps = mev_add_new_xfer_pollset(তথ্য);
    যদি(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      যাও out;
    }
  }

  যদি(last_ps)
    mresult = mev_pollset_diff(multi, তথ্য, conn, &ps, last_ps);
  অন্যথা
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  ফেরত mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(কাঠামো Curl_multi *multi,
                                    কাঠামো Curl_easy *তথ্য)
{
  ফেরত mev_assess(multi, তথ্য, ফাঁকা);
}

CURLMcode Curl_multi_ev_assess_conn(কাঠামো Curl_multi *multi,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn)
{
  ফেরত mev_assess(multi, তথ্য, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(কাঠামো Curl_multi *multi,
                                         কাঠামো uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  যদি(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    করো {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(তথ্য) {
        mresult = Curl_multi_ev_assess_xfer(multi, তথ্য);
      }
    } যখন(!mresult && Curl_uint32_bset_পরবর্তী(set, mid, &mid));
  }
  ফেরত mresult;
}

CURLMcode Curl_multi_ev_assign(কাঠামো Curl_multi *multi,
                               curl_socket_t s,
                               শূন্য *user_তথ্য)
{
  কাঠামো mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  যদি(!e)
    ফেরত CURLM_BAD_SOCKET;
  e->user_তথ্য = user_তথ্য;
  ফেরত CURLM_OK;
}

শূন্য Curl_multi_ev_dirty_xfers(কাঠামো Curl_multi *multi,
                               curl_socket_t s)
{
  কাঠামো mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  যদি(entry) {
    কাঠামো Curl_easy *তথ্য;
    uint32_t mid;

    যদি(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      করো {
        তথ্য = Curl_multi_get_easy(multi, mid);
        যদি(তথ্য) {
          Curl_multi_mark_dirty(তথ্য);
        }
        অন্যথা {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } যখন(Curl_uint32_spbset_পরবর্তী(&entry->xfers, mid, &mid));
    }

    যদি(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

শূন্য Curl_multi_ev_socket_done(কাঠামো Curl_multi *multi,
                               কাঠামো Curl_easy *তথ্য, curl_socket_t s)
{
  mev_forget_socket(multi, তথ্য, s, "socket done");
}

শূন্য Curl_multi_ev_xfer_done(কাঠামো Curl_multi *multi,
                             কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(!তথ্য->conn); /* transfer should have been detached */
  (শূন্য)mev_assess(multi, তথ্য, ফাঁকা);
  Curl_meta_remove(তথ্য, CURL_META_MEV_POLLSET);
}

শূন্য Curl_multi_ev_conn_done(কাঠামো Curl_multi *multi,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn)
{
  (শূন্য)mev_assess(multi, তথ্য, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

শূন্য Curl_multi_ev_init(কাঠামো Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

শূন্য Curl_multi_ev_cleanup(কাঠামো Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
