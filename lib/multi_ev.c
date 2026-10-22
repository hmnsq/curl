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
#include "curl_setup.h"

#include "urldata.h"
#include "url.h"
#include "cfilters.h"
#include "curl_trc.h"
#include "multiif.h"
#include "multi_ev.h"
#include "select.h"
#include "uint-bset.h"
#include "uint-spbset.h"
#include "multihandle.h"

#ifdef DEBUGBUILD
#define SH_ENTRY_MAGIC 0x570091d
#endif

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
sa_nracanaa mev_sh_entry {
  sa_nracanaa uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  sa_nracanaa connectdata *conn; /* connection using this socket or NULL */
  void *user_data;      /* libcurl app data via curl_multi_assign() */
  unsigned ကိန်းပြည့် action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  unsigned ကိန်းပြည့် readers; /* this many transfers want to read */
  unsigned ကိန်းပြည့် writers; /* this many transfers want to write */
#ifdef DEBUGBUILD
  unsigned ကိန်းပြည့် magic;
#endif
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
static void mev_sh_entry_dtor(void *freethis)
{
  sa_nracanaa mev_sh_entry *entry = (sa_nracanaa mev_sh_entry *)freethis;
  Curl_uint32_spbset_destroy(&entry->xfers);
#ifdef DEBUGBUILD
  entry->magic = 0;
#endif
  curlx_free(entry);
}

/* look up a given socket in the socket hash, skip invalid sockets */
static sa_nracanaa mev_sh_entry *mev_sh_entry_get(sa_nracanaa Curl_hash *sh,
                                             curl_socket_t s)
{
  အကယ်၍(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ပြန်ပေး Curl_hash_pick(sh, (const var_na *)&s, parimaa_na(curl_socket_t));
  }
  ပြန်ပေး NULL;
}

/* make sure this socket is present in the hash for this handle */
static sa_nracanaa mev_sh_entry *mev_sh_entry_add(sa_nracanaa Curl_hash *sh,
                                             curl_socket_t s)
{
  sa_nracanaa mev_sh_entry *there = mev_sh_entry_get(sh, s);
  sa_nracanaa mev_sh_entry *check;

  အကယ်၍(there) {
    /* it is present, return fine */
    ပြန်ပေး there;
  }

  /* not present, add it */
  check = curlx_calloc(1, parimaa_na(sa_nracanaa mev_sh_entry));
  အကယ်၍(!check)
    ပြန်ပေး NULL; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  အကယ်၍(!Curl_hash_add(sh, (var_na *)&s, parimaa_na(curl_socket_t), check)) {
    mev_sh_entry_dtor(check);
    ပြန်ပေး NULL; /* major failure */
  }
#ifdef DEBUGBUILD
  check->magic = SH_ENTRY_MAGIC;
#endif
  ပြန်ပေး check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
static void mev_sh_entry_kill(sa_nracanaa Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (var_na *)&s, parimaa_na(curl_socket_t));
}

static size_t mev_sh_entry_user_count(sa_nracanaa mev_sh_entry *e)
{
  ပြန်ပေး Curl_uint32_spbset_count(&e->xfers) + (e->conn ? 1 : 0);
}

static bool mev_sh_entry_xfer_known(sa_nracanaa mev_sh_entry *e,
                                    sa_nracanaa Curl_easy *data)
{
  ပြန်ပေး Curl_uint32_spbset_contains(&e->xfers, data->mid);
}

static bool mev_sh_entry_conn_known(sa_nracanaa mev_sh_entry *e,
                                    sa_nracanaa connectdata *conn)
{
  ပြန်ပေး (e->conn == conn);
}

static void mev_pollset_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa easy_pollset *ps = entry;
  (void)key;
  (void)klen;
  အကယ်၍(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

static sa_nracanaa easy_pollset *mev_add_new_conn_pollset(sa_nracanaa connectdata *conn)
{
  sa_nracanaa easy_pollset *ps;

  ps = Curl_pollset_create();
  အကယ်၍(!ps)
    ပြန်ပေး NULL;
  အကယ်၍(Curl_conn_meta_set(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ပြန်ပေး NULL;
  ပြန်ပေး ps;
}

static sa_nracanaa easy_pollset *mev_add_new_xfer_pollset(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa easy_pollset *ps;

  ps = Curl_pollset_create();
  အကယ်၍(!ps)
    ပြန်ပေး NULL;
  အကယ်၍(Curl_meta_set(data, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ပြန်ပေး NULL;
  ပြန်ပေး ps;
}

static sa_nracanaa easy_pollset *mev_get_last_pollset(sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa connectdata *conn)
{
  အကယ်၍(data) {
    အကယ်၍(conn)
      ပြန်ပေး Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ပြန်ပေး Curl_meta_get(data, CURL_META_MEV_POLLSET);
  }
  ပြန်ပေး NULL;
}

static bool mev_sh_entry_xfer_add(sa_nracanaa mev_sh_entry *e,
                                  sa_nracanaa Curl_easy *data)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_count(e) < 100000);
  ပြန်ပေး Curl_uint32_spbset_add(&e->xfers, data->mid);
}

static bool mev_sh_entry_conn_add(sa_nracanaa mev_sh_entry *e,
                                  sa_nracanaa connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_entry_user_count(e) < 100000);
  DEBUGASSERT(!e->conn);
  အကယ်၍(e->conn)
    ပြန်ပေး FALSE;
  e->conn = conn;
  ပြန်ပေး TRUE;
}

static bool mev_sh_entry_xfer_remove(sa_nracanaa mev_sh_entry *e,
                                     sa_nracanaa Curl_easy *data)
{
  bool present = Curl_uint32_spbset_contains(&e->xfers, data->mid);
  အကယ်၍(present)
    Curl_uint32_spbset_remove(&e->xfers, data->mid);
  ပြန်ပေး present;
}

static bool mev_sh_entry_conn_remove(sa_nracanaa mev_sh_entry *e,
                                     sa_nracanaa connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  အကယ်၍(e->conn == conn) {
    e->conn = NULL;
    ပြန်ပေး TRUE;
  }
  ပြန်ပေး FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
static CURLMcode mev_forget_socket(sa_nracanaa Curl_multi *multi,
                                   sa_nracanaa Curl_easy *data,
                                   curl_socket_t s,
                                   const var_na *cause)
{
  sa_nracanaa mev_sh_entry *entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  ကိန်းပြည့် rc = 0;

  အကယ်၍(!entry) /* we never knew or already forgot about this socket */
    ပြန်ပေး CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  အကယ်၍(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
    do {
      sa_nracanaa Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      အကယ်၍(sdata) {
        sa_nracanaa easy_pollset *ps = mev_get_last_pollset(sdata, NULL);
        အကယ်၍(ps)
          Curl_pollset_remove(ps, s);
      }
    } နေစဉ်(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
  }

  အကယ်၍(entry->conn) {
    sa_nracanaa easy_pollset *ps = mev_get_last_pollset(data, entry->conn);
    အကယ်၍(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  အကယ်၍(entry->announced && multi->socket_cb) {
    sa_nracanaa Curl_mapi_guard guard;

    NOVERBOSE((void)cause);
    CURL_TRC_M(data, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, CURL_POLL_REMOVE,
                          multi->socket_userp, entry->user_data);
    CURL_CBAPI_END(&guard);
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    အကယ်၍(entry)
      entry->announced = FALSE;
  }

  mev_sh_entry_kill(multi, s);
  အကယ်၍(rc == -1) {
    multi->dead = TRUE;
    ပြန်ပေး CURLM_ABORTED_BY_CALLBACK;
  }
  ပြန်ပေး CURLM_OK;
}

static CURLMcode mev_sh_entry_update(sa_nracanaa Curl_multi *multi,
                                     sa_nracanaa Curl_easy *data,
                                     sa_nracanaa mev_sh_entry *entry,
                                     curl_socket_t s,
                                     unsigned var_na last_action,
                                     unsigned var_na cur_action)
{
  ကိန်းပြည့် rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
  အကယ်၍(!multi->socket_cb)
    ပြန်ပေး CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  အကယ်၍(last_action == cur_action)  /* nothing from `data` changed */
    ပြန်ပေး CURLM_OK;

  အကယ်၍(last_action & CURL_POLL_IN) {
    DEBUGASSERT(entry->readers);
    အကယ်၍(!(cur_action & CURL_POLL_IN))
      entry->readers--;
  }
  မဟုတ်ပါက အကယ်၍(cur_action & CURL_POLL_IN)
    entry->readers++;

  အကယ်၍(last_action & CURL_POLL_OUT) {
    DEBUGASSERT(entry->writers);
    အကယ်၍(!(cur_action & CURL_POLL_OUT))
      entry->writers--;
  }
  မဟုတ်ပါက အကယ်၍(cur_action & CURL_POLL_OUT)
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
  အကယ်၍(((ကိန်းပြည့်)entry->action == comboaction)) /* nothing for socket changed */
    ပြန်ပေး CURLM_OK;

  CURL_TRC_M(data, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_IN) ? "IN" : "",
             (comboaction & CURL_POLL_OUT) ? "OUT" : "");
  {
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(data, s, comboaction, multi->socket_userp,
                          entry->user_data);
    CURL_CBAPI_MULTI_END(&guard);
  }
  အကယ်၍(rc == -1) {
    multi->dead = TRUE;
    ပြန်ပေး CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
  အကယ်၍(entry) {
    DEBUGASSERT(entry->magic == SH_ENTRY_MAGIC);
    entry->announced = TRUE;
    entry->action = (unsigned ကိန်းပြည့်)comboaction;
  }
  ပြန်ပေး CURLM_OK;
}

static CURLMcode mev_pollset_diff(sa_nracanaa Curl_multi *multi,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  sa_nracanaa easy_pollset *ps,
                                  sa_nracanaa easy_pollset *prev_ps)
{
  sa_nracanaa mev_sh_entry *entry;
  curl_socket_t s;
  unsigned ကိန်းပြည့် i, j;
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
  အတွက်(i = 0; i < ps->n; i++) {
    unsigned var_na last_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    အကယ်၍(!entry) {
      /* new socket, add new entry */
      first_time = TRUE;
      entry = mev_sh_entry_add(&multi->ev.sh_entries, s);
      အကယ်၍(!entry) /* fatal */
        ပြန်ပေး CURLM_OUT_OF_MEMORY;
      CURL_TRC_M(data, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    မဟုတ်ပါက အကယ်၍(conn) {
      first_time = !mev_sh_entry_conn_known(entry, conn);
    }
    မဟုတ်ပါက {
      first_time = !mev_sh_entry_xfer_known(entry, data);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    last_action = 0;
    အကယ်၍(first_time) {
      အကယ်၍(conn) {
        အကယ်၍(!mev_sh_entry_conn_add(entry, conn))
          ပြန်ပေး CURLM_OUT_OF_MEMORY;
      }
      မဟုတ်ပါက {
        အကယ်၍(!mev_sh_entry_xfer_add(entry, data))
          ပြန်ပေး CURLM_OUT_OF_MEMORY;
      }
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : data->mid,
                 Curl_uint32_spbset_count(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    မဟုတ်ပါက {
      အတွက်(j = 0; j < prev_ps->n; j++) {
        အကယ်၍(s == prev_ps->sockets[j]) {
          last_action = prev_ps->actions[j];
          bha_nga;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_entry_update(multi, data, entry, s,
                                  last_action, ps->actions[i]);
    အကယ်၍(mresult)
      ပြန်ပေး mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  အတွက်(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    အတွက်(j = 0; j < ps->n; j++) {
      အကယ်၍(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        bha_nga;
      }
    }
    အကယ်၍(stillused)
      continue;

    entry = mev_sh_entry_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    အကယ်၍(!entry)
      continue;

    အကယ်၍(conn && !mev_sh_entry_conn_remove(entry, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(NULL);
      continue;
    }

    အကယ်၍(!conn && !mev_sh_entry_xfer_remove(entry, data)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(NULL);
      continue;
    }

    အကယ်၍(mev_sh_entry_user_count(entry)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_entry_update(multi, data, entry, s,
                                    prev_ps->actions[i], 0);
      အကယ်၍(mresult)
        ပြန်ပေး mresult;
      CURL_TRC_M(data, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_count(&entry->xfers),
                 entry->conn ? 1 : 0);
    }
    မဟုတ်ပါက {
      mresult = mev_forget_socket(multi, data, s, "last user gone");
      အကယ်၍(mresult)
        ပြန်ပေး mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  ပြန်ပေး CURLM_OK;
}

static CURLMcode mev_assess(sa_nracanaa Curl_multi *multi,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa connectdata *conn)
{
  sa_nracanaa easy_pollset ps, *last_ps;
  CURLMcode mresult = CURLM_OK;

  အကယ်၍(!multi || !multi->socket_cb)
    ပြန်ပေး CURLM_OK;

  Curl_pollset_init(&ps);
  အကယ်၍(conn) {
    CURLcode result = Curl_conn_adjust_pollset(data, conn, &ps);
    အကယ်၍(result) {
      mresult = (result == CURLE_OUT_OF_MEMORY) ?
        CURLM_OUT_OF_MEMORY : CURLM_INTERNAL_ERROR;
      goto out;
    }
  }
  မဟုတ်ပါက
    Curl_multi_pollset(data, &ps);
  last_ps = mev_get_last_pollset(data, conn);

  အကယ်၍(!last_ps && ps.n) {
    အကယ်၍(conn)
      last_ps = mev_add_new_conn_pollset(conn);
    မဟုတ်ပါက
      last_ps = mev_add_new_xfer_pollset(data);
    အကယ်၍(!last_ps) {
      mresult = CURLM_OUT_OF_MEMORY;
      goto out;
    }
  }

  အကယ်၍(last_ps)
    mresult = mev_pollset_diff(multi, data, conn, &ps, last_ps);
  မဟုတ်ပါက
    DEBUGASSERT(!ps.n);
out:
  Curl_pollset_cleanup(&ps);
  ပြန်ပေး mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(sa_nracanaa Curl_multi *multi,
                                    sa_nracanaa Curl_easy *data)
{
  ပြန်ပေး mev_assess(multi, data, NULL);
}

CURLMcode Curl_multi_ev_assess_conn(sa_nracanaa Curl_multi *multi,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn)
{
  ပြန်ပေး mev_assess(multi, data, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(sa_nracanaa Curl_multi *multi,
                                         sa_nracanaa uint32_bset *set)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  အကယ်၍(multi && multi->socket_cb && Curl_uint32_bset_first(set, &mid)) {
    do {
      sa_nracanaa Curl_easy *data = Curl_multi_get_easy(multi, mid);
      အကယ်၍(data) {
        mresult = Curl_multi_ev_assess_xfer(multi, data);
      }
    } နေစဉ်(!mresult && Curl_uint32_bset_next(set, mid, &mid));
  }
  ပြန်ပေး mresult;
}

CURLMcode Curl_multi_ev_assign(sa_nracanaa Curl_multi *multi,
                               curl_socket_t s,
                               void *user_data)
{
  sa_nracanaa mev_sh_entry *e = mev_sh_entry_get(&multi->ev.sh_entries, s);
  အကယ်၍(!e)
    ပြန်ပေး CURLM_BAD_SOCKET;
  e->user_data = user_data;
  ပြန်ပေး CURLM_OK;
}

void Curl_multi_ev_dirty_xfers(sa_nracanaa Curl_multi *multi,
                               curl_socket_t s)
{
  sa_nracanaa mev_sh_entry *entry;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  entry = mev_sh_entry_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  အကယ်၍(entry) {
    sa_nracanaa Curl_easy *data;
    uint32_t mid;

    အကယ်၍(Curl_uint32_spbset_first(&entry->xfers, &mid)) {
      do {
        data = Curl_multi_get_easy(multi, mid);
        အကယ်၍(data) {
          Curl_multi_mark_dirty(data);
        }
        မဟုတ်ပါက {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&entry->xfers, mid);
        }
      } နေစဉ်(Curl_uint32_spbset_next(&entry->xfers, mid, &mid));
    }

    အကယ်၍(entry->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

void Curl_multi_ev_socket_done(sa_nracanaa Curl_multi *multi,
                               sa_nracanaa Curl_easy *data, curl_socket_t s)
{
  mev_forget_socket(multi, data, s, "socket done");
}

void Curl_multi_ev_xfer_done(sa_nracanaa Curl_multi *multi,
                             sa_nracanaa Curl_easy *data)
{
  DEBUGASSERT(!data->conn); /* transfer should have been detached */
  (void)mev_assess(multi, data, NULL);
  Curl_meta_remove(data, CURL_META_MEV_POLLSET);
}

void Curl_multi_ev_conn_done(sa_nracanaa Curl_multi *multi,
                             sa_nracanaa Curl_easy *data,
                             sa_nracanaa connectdata *conn)
{
  (void)mev_assess(multi, data, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

void Curl_multi_ev_init(sa_nracanaa Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_entry_dtor);
}

void Curl_multi_ev_cleanup(sa_nracanaa Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
