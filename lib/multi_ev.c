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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ev.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-bset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-spbset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SH_ਇੰਦਰਾਜ_MAGIC 0x570091d
#ਜੇ_ਅੰਤ

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ {
  ਬਣਤਰ uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ਬਣਤਰ connectdata *conn; /* connection using this socket or NULL */
  ਖਾਲੀ *user_ਅੰਕੜੇ;      /* libcurl app data via curl_multi_assign() */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ readers; /* this many transfers want to read */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ writers; /* this many transfers want to write */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ magic;
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ਸਥਿਰ ਖਾਲੀ mev_sh_ਇੰਦਰਾਜ_dtor(ਖਾਲੀ *freethis)
{
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = (ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *)freethis;
  Curl_uint32_spbset_destroy(&ਇੰਦਰਾਜ->xfers);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਇੰਦਰਾਜ->magic = 0;
#ਜੇ_ਅੰਤ
  curlx_free(ਇੰਦਰਾਜ);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ਸਥਿਰ ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *mev_sh_ਇੰਦਰਾਜ_get(ਬਣਤਰ Curl_hash *sh,
                                             curl_socket_t s)
{
  ਜੇ(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ਵਾਪਸ Curl_hash_pick(sh, (ਅਚਲ ਅੱਖਰ *)&s, ਆਕਾਰ(curl_socket_t));
  }
  ਵਾਪਸ ਨਿਰਾ;
}

/* make sure this socket is present in the hash for this handle */
ਸਥਿਰ ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *mev_sh_ਇੰਦਰਾਜ_ਜੋੜੋ(ਬਣਤਰ Curl_hash *sh,
                                             curl_socket_t s)
{
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *there = mev_sh_ਇੰਦਰਾਜ_get(sh, s);
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *check;

  ਜੇ(there) {
    /* it is present, return fine */
    ਵਾਪਸ there;
  }

  /* not present, add it */
  check = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ));
  ਜੇ(!check)
    ਵਾਪਸ ਨਿਰਾ; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  ਜੇ(!Curl_hash_ਜੋੜੋ(sh, (ਅੱਖਰ *)&s, ਆਕਾਰ(curl_socket_t), check)) {
    mev_sh_ਇੰਦਰਾਜ_dtor(check);
    ਵਾਪਸ ਨਿਰਾ; /* major failure */
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  check->magic = SH_ਇੰਦਰਾਜ_MAGIC;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ਸਥਿਰ ਖਾਲੀ mev_sh_ਇੰਦਰਾਜ_kill(ਬਣਤਰ Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (ਅੱਖਰ *)&s, ਆਕਾਰ(curl_socket_t));
}

ਸਥਿਰ size_t mev_sh_ਇੰਦਰਾਜ_user_ਗਿਣਤੀ(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e)
{
  ਵਾਪਸ Curl_uint32_spbset_ਗਿਣਤੀ(&e->xfers) + (e->conn ? 1 : 0);
}

ਸਥਿਰ bool mev_sh_ਇੰਦਰਾਜ_xfer_ਗਿਆਤ(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_uint32_spbset_ਸ਼ਾਮਲ_ਹੈ(&e->xfers, ਅੰਕੜੇ->mid);
}

ਸਥਿਰ bool mev_sh_ਇੰਦਰਾਜ_conn_ਗਿਆਤ(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e,
                                    ਬਣਤਰ connectdata *conn)
{
  ਵਾਪਸ (e->conn == conn);
}

ਸਥਿਰ ਖਾਲੀ mev_pollset_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ easy_pollset *ps = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  ਜੇ(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ਸਥਿਰ ਬਣਤਰ easy_pollset *mev_ਜੋੜੋ_new_conn_pollset(ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ easy_pollset *ps;

  ps = Curl_pollset_create();
  ਜੇ(!ps)
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ(Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ ps;
}

ਸਥਿਰ ਬਣਤਰ easy_pollset *mev_ਜੋੜੋ_new_xfer_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ easy_pollset *ps;

  ps = Curl_pollset_create();
  ਜੇ(!ps)
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ(Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ ps;
}

ਸਥਿਰ ਬਣਤਰ easy_pollset *mev_get_ਆਖਰੀ_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ connectdata *conn)
{
  ਜੇ(ਅੰਕੜੇ) {
    ਜੇ(conn)
      ਵਾਪਸ Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ਵਾਪਸ Curl_meta_get(ਅੰਕੜੇ, CURL_META_MEV_POLLSET);
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ bool mev_sh_ਇੰਦਰਾਜ_xfer_ਜੋੜੋ(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_ਇੰਦਰਾਜ_user_ਗਿਣਤੀ(e) < 100000);
  ਵਾਪਸ Curl_uint32_spbset_ਜੋੜੋ(&e->xfers, ਅੰਕੜੇ->mid);
}

ਸਥਿਰ bool mev_sh_ਇੰਦਰਾਜ_conn_ਜੋੜੋ(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e,
                                  ਬਣਤਰ connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_ਇੰਦਰਾਜ_user_ਗਿਣਤੀ(e) < 100000);
  DEBUGASSERT(!e->conn);
  ਜੇ(e->conn)
    ਵਾਪਸ FALSE;
  e->conn = conn;
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool mev_sh_ਇੰਦਰਾਜ_xfer_remove(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  bool present = Curl_uint32_spbset_ਸ਼ਾਮਲ_ਹੈ(&e->xfers, ਅੰਕੜੇ->mid);
  ਜੇ(present)
    Curl_uint32_spbset_remove(&e->xfers, ਅੰਕੜੇ->mid);
  ਵਾਪਸ present;
}

ਸਥਿਰ bool mev_sh_ਇੰਦਰਾਜ_conn_remove(ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e,
                                     ਬਣਤਰ connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  ਜੇ(e->conn == conn) {
    e->conn = ਨਿਰਾ;
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ਸਥਿਰ CURLMcode mev_forget_socket(ਬਣਤਰ Curl_multi *multi,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   curl_socket_t s,
                                   ਅਚਲ ਅੱਖਰ *cause)
{
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  ਪੂਰਨ rc = 0;

  ਜੇ(!ਇੰਦਰਾਜ) /* we never knew or already forgot about this socket */
    ਵਾਪਸ CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  ਜੇ(Curl_uint32_spbset_ਪਹਿਲਾ(&ਇੰਦਰਾਜ->xfers, &mid)) {
    ਕਰੋ {
      ਬਣਤਰ Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      ਜੇ(sdata) {
        ਬਣਤਰ easy_pollset *ps = mev_get_ਆਖਰੀ_pollset(sdata, ਨਿਰਾ);
        ਜੇ(ps)
          Curl_pollset_remove(ps, s);
      }
    } ਜਦੋਂ(Curl_uint32_spbset_ਅਗਲਾ(&ਇੰਦਰਾਜ->xfers, mid, &mid));
  }

  ਜੇ(ਇੰਦਰਾਜ->conn) {
    ਬਣਤਰ easy_pollset *ps = mev_get_ਆਖਰੀ_pollset(ਅੰਕੜੇ, ਇੰਦਰਾਜ->conn);
    ਜੇ(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  ਜੇ(ਇੰਦਰਾਜ->announced && multi->socket_cb) {
    ਬਣਤਰ Curl_mapi_guard guard;

    NOVERBOSE((ਖਾਲੀ)cause);
    CURL_TRC_M(ਅੰਕੜੇ, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_ਸ਼ੁਰੂ(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(ਅੰਕੜੇ, s, CURL_POLL_REMOVE,
                          multi->socket_userp, ਇੰਦਰਾਜ->user_ਅੰਕੜੇ);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);
    ਜੇ(ਇੰਦਰਾਜ)
      ਇੰਦਰਾਜ->announced = FALSE;
  }

  mev_sh_ਇੰਦਰਾਜ_kill(multi, s);
  ਜੇ(rc == -1) {
    multi->dead = TRUE;
    ਵਾਪਸ CURLM_ABORTED_BY_CALLBACK;
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode mev_sh_ਇੰਦਰਾਜ_update(ਬਣਤਰ Curl_multi *multi,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ,
                                     curl_socket_t s,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਖਰੀ_action,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ cur_action)
{
  ਪੂਰਨ rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(ਇੰਦਰਾਜ->magic == SH_ਇੰਦਰਾਜ_MAGIC);
  ਜੇ(!multi->socket_cb)
    ਵਾਪਸ CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  ਜੇ(ਆਖਰੀ_action == cur_action)  /* nothing from `data` changed */
    ਵਾਪਸ CURLM_OK;

  ਜੇ(ਆਖਰੀ_action & CURL_POLL_ਵਿੱਚ) {
    DEBUGASSERT(ਇੰਦਰਾਜ->readers);
    ਜੇ(!(cur_action & CURL_POLL_ਵਿੱਚ))
      ਇੰਦਰਾਜ->readers--;
  }
  ਨਹੀਂਤਾਂ ਜੇ(cur_action & CURL_POLL_ਵਿੱਚ)
    ਇੰਦਰਾਜ->readers++;

  ਜੇ(ਆਖਰੀ_action & CURL_POLL_ਬਾਹਰਲਾ) {
    DEBUGASSERT(ਇੰਦਰਾਜ->writers);
    ਜੇ(!(cur_action & CURL_POLL_ਬਾਹਰਲਾ))
      ਇੰਦਰਾਜ->writers--;
  }
  ਨਹੀਂਤਾਂ ਜੇ(cur_action & CURL_POLL_ਬਾਹਰਲਾ)
    ਇੰਦਰਾਜ->writers++;

  DEBUGASSERT(ਇੰਦਰਾਜ->readers <= mev_sh_ਇੰਦਰਾਜ_user_ਗਿਣਤੀ(ਇੰਦਰਾਜ));
  DEBUGASSERT(ਇੰਦਰਾਜ->writers <= mev_sh_ਇੰਦਰਾਜ_user_ਗਿਣਤੀ(ਇੰਦਰਾਜ));
  DEBUGASSERT(ਇੰਦਰਾਜ->writers + ਇੰਦਰਾਜ->readers);

  CURL_TRC_M(ਅੰਕੜੇ, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (ਆਖਰੀ_action & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
             (ਆਖਰੀ_action & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "",
             (cur_action & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
             (cur_action & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "",
             ਇੰਦਰਾਜ->readers, ਇੰਦਰਾਜ->writers);

  comboaction = (ਇੰਦਰਾਜ->writers ? CURL_POLL_ਬਾਹਰਲਾ : 0) |
                (ਇੰਦਰਾਜ->readers ? CURL_POLL_ਵਿੱਚ : 0);
  ਜੇ(((ਪੂਰਨ)ਇੰਦਰਾਜ->action == comboaction)) /* nothing for socket changed */
    ਵਾਪਸ CURLM_OK;

  CURL_TRC_M(ਅੰਕੜੇ, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
             (comboaction & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "");
  {
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_ਸ਼ੁਰੂ(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(ਅੰਕੜੇ, s, comboaction, multi->socket_userp,
                          ਇੰਦਰਾਜ->user_ਅੰਕੜੇ);
    CURL_CBAPI_MULTI_ਸਿਰਾ(&guard);
  }
  ਜੇ(rc == -1) {
    multi->dead = TRUE;
    ਵਾਪਸ CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);
  ਜੇ(ਇੰਦਰਾਜ) {
    DEBUGASSERT(ਇੰਦਰਾਜ->magic == SH_ਇੰਦਰਾਜ_MAGIC);
    ਇੰਦਰਾਜ->announced = TRUE;
    ਇੰਦਰਾਜ->action = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)comboaction;
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode mev_pollset_diff(ਬਣਤਰ Curl_multi *multi,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  ਬਣਤਰ easy_pollset *ps,
                                  ਬਣਤਰ easy_pollset *prev_ps)
{
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ;
  curl_socket_t s;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i, j;
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
  ਲਈ(i = 0; i < ps->n; i++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਖਰੀ_action;
    bool ਪਹਿਲਾ_ਸਮਾਂ = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);
    ਜੇ(!ਇੰਦਰਾਜ) {
      /* new socket, add new entry */
      ਪਹਿਲਾ_ਸਮਾਂ = TRUE;
      ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_ਜੋੜੋ(&multi->ev.sh_entries, s);
      ਜੇ(!ਇੰਦਰਾਜ) /* fatal */
        ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      CURL_TRC_M(ਅੰਕੜੇ, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    ਨਹੀਂਤਾਂ ਜੇ(conn) {
      ਪਹਿਲਾ_ਸਮਾਂ = !mev_sh_ਇੰਦਰਾਜ_conn_ਗਿਆਤ(ਇੰਦਰਾਜ, conn);
    }
    ਨਹੀਂਤਾਂ {
      ਪਹਿਲਾ_ਸਮਾਂ = !mev_sh_ਇੰਦਰਾਜ_xfer_ਗਿਆਤ(ਇੰਦਰਾਜ, ਅੰਕੜੇ);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    ਆਖਰੀ_action = 0;
    ਜੇ(ਪਹਿਲਾ_ਸਮਾਂ) {
      ਜੇ(conn) {
        ਜੇ(!mev_sh_ਇੰਦਰਾਜ_conn_ਜੋੜੋ(ਇੰਦਰਾਜ, conn))
          ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(!mev_sh_ਇੰਦਰਾਜ_xfer_ਜੋੜੋ(ਇੰਦਰਾਜ, ਅੰਕੜੇ))
          ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      CURL_TRC_M(ਅੰਕੜੇ, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : ਅੰਕੜੇ->mid,
                 Curl_uint32_spbset_ਗਿਣਤੀ(&ਇੰਦਰਾਜ->xfers),
                 ਇੰਦਰਾਜ->conn ? 1 : 0);
    }
    ਨਹੀਂਤਾਂ {
      ਲਈ(j = 0; j < prev_ps->n; j++) {
        ਜੇ(s == prev_ps->sockets[j]) {
          ਆਖਰੀ_action = prev_ps->actions[j];
          ਰੋਕੋ;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_ਇੰਦਰਾਜ_update(multi, ਅੰਕੜੇ, ਇੰਦਰਾਜ, s,
                                  ਆਖਰੀ_action, ps->actions[i]);
    ਜੇ(mresult)
      ਵਾਪਸ mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  ਲਈ(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    ਲਈ(j = 0; j < ps->n; j++) {
      ਜੇ(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        ਰੋਕੋ;
      }
    }
    ਜੇ(stillused)
      ਜਾਰੀ;

    ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    ਜੇ(!ਇੰਦਰਾਜ)
      ਜਾਰੀ;

    ਜੇ(conn && !mev_sh_ਇੰਦਰਾਜ_conn_remove(ਇੰਦਰਾਜ, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(ਅੰਕੜੇ, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ਨਿਰਾ);
      ਜਾਰੀ;
    }

    ਜੇ(!conn && !mev_sh_ਇੰਦਰਾਜ_xfer_remove(ਇੰਦਰਾਜ, ਅੰਕੜੇ)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(ਅੰਕੜੇ, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ਨਿਰਾ);
      ਜਾਰੀ;
    }

    ਜੇ(mev_sh_ਇੰਦਰਾਜ_user_ਗਿਣਤੀ(ਇੰਦਰਾਜ)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_ਇੰਦਰਾਜ_update(multi, ਅੰਕੜੇ, ਇੰਦਰਾਜ, s,
                                    prev_ps->actions[i], 0);
      ਜੇ(mresult)
        ਵਾਪਸ mresult;
      CURL_TRC_M(ਅੰਕੜੇ, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_ਗਿਣਤੀ(&ਇੰਦਰਾਜ->xfers),
                 ਇੰਦਰਾਜ->conn ? 1 : 0);
    }
    ਨਹੀਂਤਾਂ {
      mresult = mev_forget_socket(multi, ਅੰਕੜੇ, s, "last user gone");
      ਜੇ(mresult)
        ਵਾਪਸ mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode mev_assess(ਬਣਤਰ Curl_multi *multi,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ easy_pollset ps, *ਆਖਰੀ_ps;
  CURLMcode mresult = CURLM_OK;

  ਜੇ(!multi || !multi->socket_cb)
    ਵਾਪਸ CURLM_OK;

  Curl_pollset_init(&ps);
  ਜੇ(conn) {
    CURLcode ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, conn, &ps);
    ਜੇ(ਨਤੀਜਾ) {
      mresult = (ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) ?
        CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : CURLM_ਅੰਦਰੂਨੀ_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ
    Curl_multi_pollset(ਅੰਕੜੇ, &ps);
  ਆਖਰੀ_ps = mev_get_ਆਖਰੀ_pollset(ਅੰਕੜੇ, conn);

  ਜੇ(!ਆਖਰੀ_ps && ps.n) {
    ਜੇ(conn)
      ਆਖਰੀ_ps = mev_ਜੋੜੋ_new_conn_pollset(conn);
    ਨਹੀਂਤਾਂ
      ਆਖਰੀ_ps = mev_ਜੋੜੋ_new_xfer_pollset(ਅੰਕੜੇ);
    ਜੇ(!ਆਖਰੀ_ps) {
      mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(ਆਖਰੀ_ps)
    mresult = mev_pollset_diff(multi, ਅੰਕੜੇ, conn, &ps, ਆਖਰੀ_ps);
  ਨਹੀਂਤਾਂ
    DEBUGASSERT(!ps.n);
ਬਾਹਰਲਾ:
  Curl_pollset_cleanup(&ps);
  ਵਾਪਸ mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ mev_assess(multi, ਅੰਕੜੇ, ਨਿਰਾ);
}

CURLMcode Curl_multi_ev_assess_conn(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn)
{
  ਵਾਪਸ mev_assess(multi, ਅੰਕੜੇ, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ਬਣਤਰ Curl_multi *multi,
                                         ਬਣਤਰ uint32_bset *ਸੈੱਟ)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  ਜੇ(multi && multi->socket_cb && Curl_uint32_bset_ਪਹਿਲਾ(ਸੈੱਟ, &mid)) {
    ਕਰੋ {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(ਅੰਕੜੇ) {
        mresult = Curl_multi_ev_assess_xfer(multi, ਅੰਕੜੇ);
      }
    } ਜਦੋਂ(!mresult && Curl_uint32_bset_ਅਗਲਾ(ਸੈੱਟ, mid, &mid));
  }
  ਵਾਪਸ mresult;
}

CURLMcode Curl_multi_ev_ਸੌਂਪੋ(ਬਣਤਰ Curl_multi *multi,
                               curl_socket_t s,
                               ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *e = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);
  ਜੇ(!e)
    ਵਾਪਸ CURLM_BAD_SOCKET;
  e->user_ਅੰਕੜੇ = user_ਅੰਕੜੇ;
  ਵਾਪਸ CURLM_OK;
}

ਖਾਲੀ Curl_multi_ev_dirty_xfers(ਬਣਤਰ Curl_multi *multi,
                               curl_socket_t s)
{
  ਬਣਤਰ mev_sh_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  ਇੰਦਰਾਜ = mev_sh_ਇੰਦਰਾਜ_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  ਜੇ(ਇੰਦਰਾਜ) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
    uint32_t mid;

    ਜੇ(Curl_uint32_spbset_ਪਹਿਲਾ(&ਇੰਦਰਾਜ->xfers, &mid)) {
      ਕਰੋ {
        ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
        ਜੇ(ਅੰਕੜੇ) {
          Curl_multi_mark_dirty(ਅੰਕੜੇ);
        }
        ਨਹੀਂਤਾਂ {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&ਇੰਦਰਾਜ->xfers, mid);
        }
      } ਜਦੋਂ(Curl_uint32_spbset_ਅਗਲਾ(&ਇੰਦਰਾਜ->xfers, mid, &mid));
    }

    ਜੇ(ਇੰਦਰਾਜ->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

ਖਾਲੀ Curl_multi_ev_socket_ਪੂਰਾ(ਬਣਤਰ Curl_multi *multi,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_socket_t s)
{
  mev_forget_socket(multi, ਅੰਕੜੇ, s, "socket done");
}

ਖਾਲੀ Curl_multi_ev_xfer_ਪੂਰਾ(ਬਣਤਰ Curl_multi *multi,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(!ਅੰਕੜੇ->conn); /* transfer should have been detached */
  (ਖਾਲੀ)mev_assess(multi, ਅੰਕੜੇ, ਨਿਰਾ);
  Curl_meta_remove(ਅੰਕੜੇ, CURL_META_MEV_POLLSET);
}

ਖਾਲੀ Curl_multi_ev_conn_ਪੂਰਾ(ਬਣਤਰ Curl_multi *multi,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn)
{
  (ਖਾਲੀ)mev_assess(multi, ਅੰਕੜੇ, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

ਖਾਲੀ Curl_multi_ev_init(ਬਣਤਰ Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_ਇੰਦਰਾਜ_dtor);
}

ਖਾਲੀ Curl_multi_ev_cleanup(ਬਣਤਰ Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
