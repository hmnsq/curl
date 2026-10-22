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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು SH_ನಮೂದು_MAGIC 0x570091d
#ಆದರೆ_ಅಂತ್ಯ

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
ರಚನೆ mev_sh_ನಮೂದು {
  ರಚನೆ uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  ರಚನೆ connectdata *conn; /* connection using this socket or NULL */
  ಶೂನ್ಯ *user_ದತ್ತಾಂಶ;      /* libcurl app data via curl_multi_assign() */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ readers; /* this many transfers want to read */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ writers; /* this many transfers want to write */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ magic;
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
ಸ್ಥಿರ ಶೂನ್ಯ mev_sh_ನಮೂದು_dtor(ಶೂನ್ಯ *freethis)
{
  ರಚನೆ mev_sh_ನಮೂದು *ನಮೂದು = (ರಚನೆ mev_sh_ನಮೂದು *)freethis;
  Curl_uint32_spbset_destroy(&ನಮೂದು->xfers);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ನಮೂದು->magic = 0;
#ಆದರೆ_ಅಂತ್ಯ
  curlx_free(ನಮೂದು);
}

/* look up a given socket in the socket hash, skip invalid sockets */
ಸ್ಥಿರ ರಚನೆ mev_sh_ನಮೂದು *mev_sh_ನಮೂದು_get(ರಚನೆ Curl_hash *sh,
                                             curl_socket_t s)
{
  ಆದರೆ(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    ಹಿಂತಿರುಗಿ Curl_hash_pick(sh, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&s, ಗಾತ್ರ(curl_socket_t));
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/* make sure this socket is present in the hash for this handle */
ಸ್ಥಿರ ರಚನೆ mev_sh_ನಮೂದು *mev_sh_ನಮೂದು_ಕೂಡಿಸು(ರಚನೆ Curl_hash *sh,
                                             curl_socket_t s)
{
  ರಚನೆ mev_sh_ನಮೂದು *there = mev_sh_ನಮೂದು_get(sh, s);
  ರಚನೆ mev_sh_ನಮೂದು *check;

  ಆದರೆ(there) {
    /* it is present, return fine */
    ಹಿಂತಿರುಗಿ there;
  }

  /* not present, add it */
  check = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ mev_sh_ನಮೂದು));
  ಆದರೆ(!check)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  ಆದರೆ(!Curl_hash_ಕೂಡಿಸು(sh, (ಅಕ್ಷರ *)&s, ಗಾತ್ರ(curl_socket_t), check)) {
    mev_sh_ನಮೂದು_dtor(check);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* major failure */
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  check->magic = SH_ನಮೂದು_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
ಸ್ಥಿರ ಶೂನ್ಯ mev_sh_ನಮೂದು_kill(ರಚನೆ Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (ಅಕ್ಷರ *)&s, ಗಾತ್ರ(curl_socket_t));
}

ಸ್ಥಿರ size_t mev_sh_ನಮೂದು_user_ಎಣಿಕೆ(ರಚನೆ mev_sh_ನಮೂದು *e)
{
  ಹಿಂತಿರುಗಿ Curl_uint32_spbset_ಎಣಿಕೆ(&e->xfers) + (e->conn ? 1 : 0);
}

ಸ್ಥಿರ bool mev_sh_ನಮೂದು_xfer_ತಿಳಿದ(ರಚನೆ mev_sh_ನಮೂದು *e,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_uint32_spbset_ಒಳಗೊಂಡಿದೆ(&e->xfers, ದತ್ತಾಂಶ->mid);
}

ಸ್ಥಿರ bool mev_sh_ನಮೂದು_conn_ತಿಳಿದ(ರಚನೆ mev_sh_ನಮೂದು *e,
                                    ರಚನೆ connectdata *conn)
{
  ಹಿಂತಿರುಗಿ (e->conn == conn);
}

ಸ್ಥಿರ ಶೂನ್ಯ mev_pollset_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ easy_pollset *ps = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  ಆದರೆ(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

ಸ್ಥಿರ ರಚನೆ easy_pollset *mev_ಕೂಡಿಸು_new_conn_pollset(ರಚನೆ connectdata *conn)
{
  ರಚನೆ easy_pollset *ps;

  ps = Curl_pollset_create();
  ಆದರೆ(!ps)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ(Curl_conn_meta_ಗಣ(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ ps;
}

ಸ್ಥಿರ ರಚನೆ easy_pollset *mev_ಕೂಡಿಸು_new_xfer_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ easy_pollset *ps;

  ps = Curl_pollset_create();
  ಆದರೆ(!ps)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ(Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ ps;
}

ಸ್ಥಿರ ರಚನೆ easy_pollset *mev_get_ಕೊನೆಯ_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ connectdata *conn)
{
  ಆದರೆ(ದತ್ತಾಂಶ) {
    ಆದರೆ(conn)
      ಹಿಂತಿರುಗಿ Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    ಹಿಂತಿರುಗಿ Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MEV_POLLSET);
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ bool mev_sh_ನಮೂದು_xfer_ಕೂಡಿಸು(ರಚನೆ mev_sh_ನಮೂದು *e,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_ನಮೂದು_user_ಎಣಿಕೆ(e) < 100000);
  ಹಿಂತಿರುಗಿ Curl_uint32_spbset_ಕೂಡಿಸು(&e->xfers, ದತ್ತಾಂಶ->mid);
}

ಸ್ಥಿರ bool mev_sh_ನಮೂದು_conn_ಕೂಡಿಸು(ರಚನೆ mev_sh_ನಮೂದು *e,
                                  ರಚನೆ connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_ನಮೂದು_user_ಎಣಿಕೆ(e) < 100000);
  DEBUGASSERT(!e->conn);
  ಆದರೆ(e->conn)
    ಹಿಂತಿರುಗಿ FALSE;
  e->conn = conn;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool mev_sh_ನಮೂದು_xfer_remove(ರಚನೆ mev_sh_ನಮೂದು *e,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  bool present = Curl_uint32_spbset_ಒಳಗೊಂಡಿದೆ(&e->xfers, ದತ್ತಾಂಶ->mid);
  ಆದರೆ(present)
    Curl_uint32_spbset_remove(&e->xfers, ದತ್ತಾಂಶ->mid);
  ಹಿಂತಿರುಗಿ present;
}

ಸ್ಥಿರ bool mev_sh_ನಮೂದು_conn_remove(ರಚನೆ mev_sh_ನಮೂದು *e,
                                     ರಚನೆ connectdata *conn)
{
  DEBUGASSERT(e->conn == conn);
  ಆದರೆ(e->conn == conn) {
    e->conn = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/* Purge any information about socket `s`.
 * Let the socket callback know as well when necessary */
ಸ್ಥಿರ CURLMcode mev_forget_socket(ರಚನೆ Curl_multi *multi,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   curl_socket_t s,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cause)
{
  ರಚನೆ mev_sh_ನಮೂದು *ನಮೂದು = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  ಪೂರ್ಣಾಂಕ rc = 0;

  ಆದರೆ(!ನಮೂದು) /* we never knew or already forgot about this socket */
    ಹಿಂತಿರುಗಿ CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  ಆದರೆ(Curl_uint32_spbset_ಮೊದಲ(&ನಮೂದು->xfers, &mid)) {
    ಮಾಡು {
      ರಚನೆ Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(sdata) {
        ರಚನೆ easy_pollset *ps = mev_get_ಕೊನೆಯ_pollset(sdata, ಖಾಲಿ);
        ಆದರೆ(ps)
          Curl_pollset_remove(ps, s);
      }
    } ಆಗ(Curl_uint32_spbset_ಮುಂದಿನ(&ನಮೂದು->xfers, mid, &mid));
  }

  ಆದರೆ(ನಮೂದು->conn) {
    ರಚನೆ easy_pollset *ps = mev_get_ಕೊನೆಯ_pollset(ದತ್ತಾಂಶ, ನಮೂದು->conn);
    ಆದರೆ(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  ಆದರೆ(ನಮೂದು->announced && multi->socket_cb) {
    ರಚನೆ Curl_mapi_guard guard;

    NOVERBOSE((ಶೂನ್ಯ)cause);
    CURL_TRC_M(ದತ್ತಾಂಶ, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_ಆರಂಭ(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(ದತ್ತಾಂಶ, s, CURL_POLL_REMOVE,
                          multi->socket_userp, ನಮೂದು->user_ದತ್ತಾಂಶ);
    CURL_CBAPI_ಕೊನೆ(&guard);
    ನಮೂದು = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);
    ಆದರೆ(ನಮೂದು)
      ನಮೂದು->announced = FALSE;
  }

  mev_sh_ನಮೂದು_kill(multi, s);
  ಆದರೆ(rc == -1) {
    multi->dead = TRUE;
    ಹಿಂತಿರುಗಿ CURLM_ABORTED_BY_CALLBACK;
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode mev_sh_ನಮೂದು_update(ರಚನೆ Curl_multi *multi,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ mev_sh_ನಮೂದು *ನಮೂದು,
                                     curl_socket_t s,
                                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಕೊನೆಯ_action,
                                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ cur_action)
{
  ಪೂರ್ಣಾಂಕ rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(ನಮೂದು->magic == SH_ನಮೂದು_MAGIC);
  ಆದರೆ(!multi->socket_cb)
    ಹಿಂತಿರುಗಿ CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  ಆದರೆ(ಕೊನೆಯ_action == cur_action)  /* nothing from `data` changed */
    ಹಿಂತಿರುಗಿ CURLM_OK;

  ಆದರೆ(ಕೊನೆಯ_action & CURL_POLL_ಒಳ) {
    DEBUGASSERT(ನಮೂದು->readers);
    ಆದರೆ(!(cur_action & CURL_POLL_ಒಳ))
      ನಮೂದು->readers--;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cur_action & CURL_POLL_ಒಳ)
    ನಮೂದು->readers++;

  ಆದರೆ(ಕೊನೆಯ_action & CURL_POLL_ಹೊರಹೋಗು) {
    DEBUGASSERT(ನಮೂದು->writers);
    ಆದರೆ(!(cur_action & CURL_POLL_ಹೊರಹೋಗು))
      ನಮೂದು->writers--;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cur_action & CURL_POLL_ಹೊರಹೋಗು)
    ನಮೂದು->writers++;

  DEBUGASSERT(ನಮೂದು->readers <= mev_sh_ನಮೂದು_user_ಎಣಿಕೆ(ನಮೂದು));
  DEBUGASSERT(ನಮೂದು->writers <= mev_sh_ನಮೂದು_user_ಎಣಿಕೆ(ನಮೂದು));
  DEBUGASSERT(ನಮೂದು->writers + ನಮೂದು->readers);

  CURL_TRC_M(ದತ್ತಾಂಶ, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (ಕೊನೆಯ_action & CURL_POLL_ಒಳ) ? "IN" : "",
             (ಕೊನೆಯ_action & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "",
             (cur_action & CURL_POLL_ಒಳ) ? "IN" : "",
             (cur_action & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "",
             ನಮೂದು->readers, ನಮೂದು->writers);

  comboaction = (ನಮೂದು->writers ? CURL_POLL_ಹೊರಹೋಗು : 0) |
                (ನಮೂದು->readers ? CURL_POLL_ಒಳ : 0);
  ಆದರೆ(((ಪೂರ್ಣಾಂಕ)ನಮೂದು->action == comboaction)) /* nothing for socket changed */
    ಹಿಂತಿರುಗಿ CURLM_OK;

  CURL_TRC_M(ದತ್ತಾಂಶ, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_ಒಳ) ? "IN" : "",
             (comboaction & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "");
  {
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_ಆರಂಭ(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(ದತ್ತಾಂಶ, s, comboaction, multi->socket_userp,
                          ನಮೂದು->user_ದತ್ತಾಂಶ);
    CURL_CBAPI_MULTI_ಕೊನೆ(&guard);
  }
  ಆದರೆ(rc == -1) {
    multi->dead = TRUE;
    ಹಿಂತಿರುಗಿ CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  ನಮೂದು = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);
  ಆದರೆ(ನಮೂದು) {
    DEBUGASSERT(ನಮೂದು->magic == SH_ನಮೂದು_MAGIC);
    ನಮೂದು->announced = TRUE;
    ನಮೂದು->action = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)comboaction;
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode mev_pollset_diff(ರಚನೆ Curl_multi *multi,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ರಚನೆ easy_pollset *ps,
                                  ರಚನೆ easy_pollset *prev_ps)
{
  ರಚನೆ mev_sh_ನಮೂದು *ನಮೂದು;
  curl_socket_t s;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i, j;
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
  ಗಾಗಿ(i = 0; i < ps->n; i++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಕೊನೆಯ_action;
    bool ಮೊದಲ_ಸಮಯ = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    ನಮೂದು = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);
    ಆದರೆ(!ನಮೂದು) {
      /* new socket, add new entry */
      ಮೊದಲ_ಸಮಯ = TRUE;
      ನಮೂದು = mev_sh_ನಮೂದು_ಕೂಡಿಸು(&multi->ev.sh_entries, s);
      ಆದರೆ(!ನಮೂದು) /* fatal */
        ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      CURL_TRC_M(ದತ್ತಾಂಶ, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn) {
      ಮೊದಲ_ಸಮಯ = !mev_sh_ನಮೂದು_conn_ತಿಳಿದ(ನಮೂದು, conn);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಮೊದಲ_ಸಮಯ = !mev_sh_ನಮೂದು_xfer_ತಿಳಿದ(ನಮೂದು, ದತ್ತಾಂಶ);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    ಕೊನೆಯ_action = 0;
    ಆದರೆ(ಮೊದಲ_ಸಮಯ) {
      ಆದರೆ(conn) {
        ಆದರೆ(!mev_sh_ನಮೂದು_conn_ಕೂಡಿಸು(ನಮೂದು, conn))
          ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(!mev_sh_ನಮೂದು_xfer_ಕೂಡಿಸು(ನಮೂದು, ದತ್ತಾಂಶ))
          ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      CURL_TRC_M(ದತ್ತಾಂಶ, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : ದತ್ತಾಂಶ->mid,
                 Curl_uint32_spbset_ಎಣಿಕೆ(&ನಮೂದು->xfers),
                 ನಮೂದು->conn ? 1 : 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಗಾಗಿ(j = 0; j < prev_ps->n; j++) {
        ಆದರೆ(s == prev_ps->sockets[j]) {
          ಕೊನೆಯ_action = prev_ps->actions[j];
          ನಿಲ್ಲಿಸು;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_ನಮೂದು_update(multi, ದತ್ತಾಂಶ, ನಮೂದು, s,
                                  ಕೊನೆಯ_action, ps->actions[i]);
    ಆದರೆ(mresult)
      ಹಿಂತಿರುಗಿ mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  ಗಾಗಿ(i = 0; i < prev_ps->n; i++) {
    bool stillused = FALSE;

    s = prev_ps->sockets[i];
    ಗಾಗಿ(j = 0; j < ps->n; j++) {
      ಆದರೆ(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        ನಿಲ್ಲಿಸು;
      }
    }
    ಆದರೆ(stillused)
      ಮುಂದುವರಿ;

    ನಮೂದು = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    ಆದರೆ(!ನಮೂದು)
      ಮುಂದುವರಿ;

    ಆದರೆ(conn && !mev_sh_ನಮೂದು_conn_remove(ನಮೂದು, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(ದತ್ತಾಂಶ, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ಖಾಲಿ);
      ಮುಂದುವರಿ;
    }

    ಆದರೆ(!conn && !mev_sh_ನಮೂದು_xfer_remove(ನಮೂದು, ದತ್ತಾಂಶ)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(ದತ್ತಾಂಶ, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(ಖಾಲಿ);
      ಮುಂದುವರಿ;
    }

    ಆದರೆ(mev_sh_ನಮೂದು_user_ಎಣಿಕೆ(ನಮೂದು)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_ನಮೂದು_update(multi, ದತ್ತಾಂಶ, ನಮೂದು, s,
                                    prev_ps->actions[i], 0);
      ಆದರೆ(mresult)
        ಹಿಂತಿರುಗಿ mresult;
      CURL_TRC_M(ದತ್ತಾಂಶ, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_ಎಣಿಕೆ(&ನಮೂದು->xfers),
                 ನಮೂದು->conn ? 1 : 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      mresult = mev_forget_socket(multi, ದತ್ತಾಂಶ, s, "last user gone");
      ಆದರೆ(mresult)
        ಹಿಂತಿರುಗಿ mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(prev_ps, ps);
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode mev_assess(ರಚನೆ Curl_multi *multi,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn)
{
  ರಚನೆ easy_pollset ps, *ಕೊನೆಯ_ps;
  CURLMcode mresult = CURLM_OK;

  ಆದರೆ(!multi || !multi->socket_cb)
    ಹಿಂತಿರುಗಿ CURLM_OK;

  Curl_pollset_init(&ps);
  ಆದರೆ(conn) {
    CURLcode ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, conn, &ps);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      mresult = (ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) ?
        CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : CURLM_ಆಂತರಿಕ_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_multi_pollset(ದತ್ತಾಂಶ, &ps);
  ಕೊನೆಯ_ps = mev_get_ಕೊನೆಯ_pollset(ದತ್ತಾಂಶ, conn);

  ಆದರೆ(!ಕೊನೆಯ_ps && ps.n) {
    ಆದರೆ(conn)
      ಕೊನೆಯ_ps = mev_ಕೂಡಿಸು_new_conn_pollset(conn);
    ಇಲ್ಲದಿದ್ದರೆ
      ಕೊನೆಯ_ps = mev_ಕೂಡಿಸು_new_xfer_pollset(ದತ್ತಾಂಶ);
    ಆದರೆ(!ಕೊನೆಯ_ps) {
      mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(ಕೊನೆಯ_ps)
    mresult = mev_pollset_diff(multi, ದತ್ತಾಂಶ, conn, &ps, ಕೊನೆಯ_ps);
  ಇಲ್ಲದಿದ್ದರೆ
    DEBUGASSERT(!ps.n);
ಹೊರಹೋಗು:
  Curl_pollset_cleanup(&ps);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ mev_assess(multi, ದತ್ತಾಂಶ, ಖಾಲಿ);
}

CURLMcode Curl_multi_ev_assess_conn(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn)
{
  ಹಿಂತಿರುಗಿ mev_assess(multi, ದತ್ತಾಂಶ, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(ರಚನೆ Curl_multi *multi,
                                         ರಚನೆ uint32_bset *ಗಣ)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  ಆದರೆ(multi && multi->socket_cb && Curl_uint32_bset_ಮೊದಲ(ಗಣ, &mid)) {
    ಮಾಡು {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(ದತ್ತಾಂಶ) {
        mresult = Curl_multi_ev_assess_xfer(multi, ದತ್ತಾಂಶ);
      }
    } ಆಗ(!mresult && Curl_uint32_bset_ಮುಂದಿನ(ಗಣ, mid, &mid));
  }
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode Curl_multi_ev_ನಿಯೋಜಿಸು(ರಚನೆ Curl_multi *multi,
                               curl_socket_t s,
                               ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ mev_sh_ನಮೂದು *e = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);
  ಆದರೆ(!e)
    ಹಿಂತಿರುಗಿ CURLM_BAD_SOCKET;
  e->user_ದತ್ತಾಂಶ = user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಶೂನ್ಯ Curl_multi_ev_dirty_xfers(ರಚನೆ Curl_multi *multi,
                               curl_socket_t s)
{
  ರಚನೆ mev_sh_ನಮೂದು *ನಮೂದು;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  ನಮೂದು = mev_sh_ನಮೂದು_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  ಆದರೆ(ನಮೂದು) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
    uint32_t mid;

    ಆದರೆ(Curl_uint32_spbset_ಮೊದಲ(&ನಮೂದು->xfers, &mid)) {
      ಮಾಡು {
        ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
        ಆದರೆ(ದತ್ತಾಂಶ) {
          Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&ನಮೂದು->xfers, mid);
        }
      } ಆಗ(Curl_uint32_spbset_ಮುಂದಿನ(&ನಮೂದು->xfers, mid, &mid));
    }

    ಆದರೆ(ನಮೂದು->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

ಶೂನ್ಯ Curl_multi_ev_socket_ಮುಗಿದ(ರಚನೆ Curl_multi *multi,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t s)
{
  mev_forget_socket(multi, ದತ್ತಾಂಶ, s, "socket done");
}

ಶೂನ್ಯ Curl_multi_ev_xfer_ಮುಗಿದ(ರಚನೆ Curl_multi *multi,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(!ದತ್ತಾಂಶ->conn); /* transfer should have been detached */
  (ಶೂನ್ಯ)mev_assess(multi, ದತ್ತಾಂಶ, ಖಾಲಿ);
  Curl_meta_remove(ದತ್ತಾಂಶ, CURL_META_MEV_POLLSET);
}

ಶೂನ್ಯ Curl_multi_ev_conn_ಮುಗಿದ(ರಚನೆ Curl_multi *multi,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn)
{
  (ಶೂನ್ಯ)mev_assess(multi, ದತ್ತಾಂಶ, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

ಶೂನ್ಯ Curl_multi_ev_init(ರಚನೆ Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_ನಮೂದು_dtor);
}

ಶೂನ್ಯ Curl_multi_ev_cleanup(ರಚನೆ Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
