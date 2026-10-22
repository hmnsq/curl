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

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय SH_प्रविष्टिः_MAGIC 0x570091d
#यदि_अन्तः

/* Information about a socket for which we inform the libcurl application
 * what to supervise (CURL_POLL_IN/CURL_POLL_OUT/CURL_POLL_REMOVE)
 */
संरचना mev_sh_प्रविष्टिः {
  संरचना uint32_spbset xfers; /* bitset of transfers `mid`s on this socket */
  संरचना connectdata *conn; /* connection using this socket or NULL */
  शून्य *user_दत्त;      /* libcurl app data via curl_multi_assign() */
  अचिह्नित पूर्णाङ्क action;  /* CURL_POLL_IN/CURL_POLL_OUT we last told the
                         * libcurl application to watch out for */
  अचिह्नित पूर्णाङ्क readers; /* this many transfers want to read */
  अचिह्नित पूर्णाङ्क writers; /* this many transfers want to write */
#यदि_परिभाषितम् DEBUGBUILD
  अचिह्नित पूर्णाङ्क magic;
#यदि_अन्तः
  BIT(announced);       /* this socket has been passed to the socket
                           callback at least once */
};

/* sockhash entry destructor callback */
स्थिर शून्य mev_sh_प्रविष्टिः_dtor(शून्य *freethis)
{
  संरचना mev_sh_प्रविष्टिः *प्रविष्टिः = (संरचना mev_sh_प्रविष्टिः *)freethis;
  Curl_uint32_spbset_destroy(&प्रविष्टिः->xfers);
#यदि_परिभाषितम् DEBUGBUILD
  प्रविष्टिः->magic = 0;
#यदि_अन्तः
  curlx_free(प्रविष्टिः);
}

/* look up a given socket in the socket hash, skip invalid sockets */
स्थिर संरचना mev_sh_प्रविष्टिः *mev_sh_प्रविष्टिः_get(संरचना Curl_hash *sh,
                                             curl_socket_t s)
{
  यदि(s != CURL_SOCKET_BAD) {
    /* only look for proper sockets */
    प्रत्यागम Curl_hash_pick(sh, (ध्रुव वर्ण *)&s, परिमाण(curl_socket_t));
  }
  प्रत्यागम रिक्त;
}

/* make sure this socket is present in the hash for this handle */
स्थिर संरचना mev_sh_प्रविष्टिः *mev_sh_प्रविष्टिः_सङ्कलय(संरचना Curl_hash *sh,
                                             curl_socket_t s)
{
  संरचना mev_sh_प्रविष्टिः *there = mev_sh_प्रविष्टिः_get(sh, s);
  संरचना mev_sh_प्रविष्टिः *check;

  यदि(there) {
    /* it is present, return fine */
    प्रत्यागम there;
  }

  /* not present, add it */
  check = curlx_calloc(1, परिमाण(संरचना mev_sh_प्रविष्टिः));
  यदि(!check)
    प्रत्यागम रिक्त; /* major failure */

  Curl_uint32_spbset_init(&check->xfers);

  /* make/add new hash entry */
  यदि(!Curl_hash_सङ्कलय(sh, (वर्ण *)&s, परिमाण(curl_socket_t), check)) {
    mev_sh_प्रविष्टिः_dtor(check);
    प्रत्यागम रिक्त; /* major failure */
  }
#यदि_परिभाषितम् DEBUGBUILD
  check->magic = SH_प्रविष्टिः_MAGIC;
#यदि_अन्तः
  प्रत्यागम check; /* things are good in sockhash land */
}

/* delete the given socket entry from the hash */
स्थिर शून्य mev_sh_प्रविष्टिः_kill(संरचना Curl_multi *multi, curl_socket_t s)
{
  Curl_hash_delete(&multi->ev.sh_entries, (वर्ण *)&s, परिमाण(curl_socket_t));
}

स्थिर size_t mev_sh_प्रविष्टिः_user_गणनम्(संरचना mev_sh_प्रविष्टिः *e)
{
  प्रत्यागम Curl_uint32_spbset_गणनम्(&e->xfers) + (e->conn ? 1 : 0);
}

स्थिर bool mev_sh_प्रविष्टिः_xfer_ज्ञातम्(संरचना mev_sh_प्रविष्टिः *e,
                                    संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_uint32_spbset_अन्तर्भवति(&e->xfers, दत्त->mid);
}

स्थिर bool mev_sh_प्रविष्टिः_conn_ज्ञातम्(संरचना mev_sh_प्रविष्टिः *e,
                                    संरचना connectdata *conn)
{
  प्रत्यागम (e->conn == conn);
}

स्थिर शून्य mev_pollset_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना easy_pollset *ps = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  यदि(ps) {
    Curl_pollset_cleanup(ps);
    curlx_free(ps);
  }
}

स्थिर संरचना easy_pollset *mev_सङ्कलय_new_conn_pollset(संरचना connectdata *conn)
{
  संरचना easy_pollset *ps;

  ps = Curl_pollset_create();
  यदि(!ps)
    प्रत्यागम रिक्त;
  यदि(Curl_conn_meta_समुच्चयः(conn, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    प्रत्यागम रिक्त;
  प्रत्यागम ps;
}

स्थिर संरचना easy_pollset *mev_सङ्कलय_new_xfer_pollset(संरचना Curl_easy *दत्त)
{
  संरचना easy_pollset *ps;

  ps = Curl_pollset_create();
  यदि(!ps)
    प्रत्यागम रिक्त;
  यदि(Curl_meta_समुच्चयः(दत्त, CURL_META_MEV_POLLSET, ps, mev_pollset_dtor))
    प्रत्यागम रिक्त;
  प्रत्यागम ps;
}

स्थिर संरचना easy_pollset *mev_get_चरमः_pollset(संरचना Curl_easy *दत्त,
                                                 संरचना connectdata *conn)
{
  यदि(दत्त) {
    यदि(conn)
      प्रत्यागम Curl_conn_meta_get(conn, CURL_META_MEV_POLLSET);
    प्रत्यागम Curl_meta_get(दत्त, CURL_META_MEV_POLLSET);
  }
  प्रत्यागम रिक्त;
}

स्थिर bool mev_sh_प्रविष्टिः_xfer_सङ्कलय(संरचना mev_sh_प्रविष्टिः *e,
                                  संरचना Curl_easy *दत्त)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_प्रविष्टिः_user_गणनम्(e) < 100000);
  प्रत्यागम Curl_uint32_spbset_सङ्कलय(&e->xfers, दत्त->mid);
}

स्थिर bool mev_sh_प्रविष्टिः_conn_सङ्कलय(संरचना mev_sh_प्रविष्टिः *e,
                                  संरचना connectdata *conn)
{
   /* detect weird values */
  DEBUGASSERT(mev_sh_प्रविष्टिः_user_गणनम्(e) < 100000);
  DEBUGASSERT(!e->conn);
  यदि(e->conn)
    प्रत्यागम FALSE;
  e->conn = conn;
  प्रत्यागम TRUE;
}

स्थिर bool mev_sh_प्रविष्टिः_xfer_remove(संरचना mev_sh_प्रविष्टिः *e,
                                     संरचना Curl_easy *दत्त)
{
  bool present = Curl_uint32_spbset_अन्तर्भवति(&e->xfers, दत्त->mid);
  यदि(present)
    Curl_uint32_spbset_remove(&e->xfers, दत्त->mid);
  प्रत्यागम present;
}

स्थिर bool mev_sh_प्रविष्टिः_conn_remove(संरचना mev_sh_प्रविष्टिः *e,
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
                                   संरचना Curl_easy *दत्त,
                                   curl_socket_t s,
                                   ध्रुव वर्ण *cause)
{
  संरचना mev_sh_प्रविष्टिः *प्रविष्टिः = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);
  uint32_t mid;
  पूर्णाङ्क rc = 0;

  यदि(!प्रविष्टिः) /* we never knew or already forgot about this socket */
    प्रत्यागम CURLM_OK;

  /* Remove the socket from any pollset that is still registered. */
  यदि(Curl_uint32_spbset_first(&प्रविष्टिः->xfers, &mid)) {
    कुरु {
      संरचना Curl_easy *sdata = Curl_multi_get_easy(multi, mid);
      यदि(sdata) {
        संरचना easy_pollset *ps = mev_get_चरमः_pollset(sdata, रिक्त);
        यदि(ps)
          Curl_pollset_remove(ps, s);
      }
    } यावत्(Curl_uint32_spbset_अग्र(&प्रविष्टिः->xfers, mid, &mid));
  }

  यदि(प्रविष्टिः->conn) {
    संरचना easy_pollset *ps = mev_get_चरमः_pollset(दत्त, प्रविष्टिः->conn);
    यदि(ps)
      Curl_pollset_remove(ps, s);
  }

  /* We managed this socket before, tell the socket callback to forget it. */
  यदि(प्रविष्टिः->announced && multi->socket_cb) {
    संरचना Curl_mapi_guard guard;

    NOVERBOSE((शून्य)cause);
    CURL_TRC_M(दत्त, "ev %s, call(fd=%" FMT_SOCKET_T ", ev=REMOVE)", cause, s);
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(दत्त, s, CURL_POLL_REMOVE,
                          multi->socket_userp, प्रविष्टिः->user_दत्त);
    CURL_CBAPI_अन्तः(&guard);
    प्रविष्टिः = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);
    यदि(प्रविष्टिः)
      प्रविष्टिः->announced = FALSE;
  }

  mev_sh_प्रविष्टिः_kill(multi, s);
  यदि(rc == -1) {
    multi->dead = TRUE;
    प्रत्यागम CURLM_ABORTED_BY_CALLBACK;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode mev_sh_प्रविष्टिः_update(संरचना Curl_multi *multi,
                                     संरचना Curl_easy *दत्त,
                                     संरचना mev_sh_प्रविष्टिः *प्रविष्टिः,
                                     curl_socket_t s,
                                     अचिह्नित वर्ण चरमः_action,
                                     अचिह्नित वर्ण cur_action)
{
  पूर्णाङ्क rc, comboaction;

  /* we should only be called when the callback exists */
  DEBUGASSERT(multi->socket_cb);
  DEBUGASSERT(प्रविष्टिः->magic == SH_प्रविष्टिः_MAGIC);
  यदि(!multi->socket_cb)
    प्रत्यागम CURLM_OK;

  /* Transfer `data` goes from `last_action` to `cur_action` on socket `s`
   * with `multi->ev.sh_entries` entry `entry`. Update `entry` and trigger
   * `multi->socket_cb` on change, if the callback is set. */
  यदि(चरमः_action == cur_action)  /* nothing from `data` changed */
    प्रत्यागम CURLM_OK;

  यदि(चरमः_action & CURL_POLL_अन्तर्) {
    DEBUGASSERT(प्रविष्टिः->readers);
    यदि(!(cur_action & CURL_POLL_अन्तर्))
      प्रविष्टिः->readers--;
  }
  अन्यथा यदि(cur_action & CURL_POLL_अन्तर्)
    प्रविष्टिः->readers++;

  यदि(चरमः_action & CURL_POLL_बहिर्गमनम्) {
    DEBUGASSERT(प्रविष्टिः->writers);
    यदि(!(cur_action & CURL_POLL_बहिर्गमनम्))
      प्रविष्टिः->writers--;
  }
  अन्यथा यदि(cur_action & CURL_POLL_बहिर्गमनम्)
    प्रविष्टिः->writers++;

  DEBUGASSERT(प्रविष्टिः->readers <= mev_sh_प्रविष्टिः_user_गणनम्(प्रविष्टिः));
  DEBUGASSERT(प्रविष्टिः->writers <= mev_sh_प्रविष्टिः_user_गणनम्(प्रविष्टिः));
  DEBUGASSERT(प्रविष्टिः->writers + प्रविष्टिः->readers);

  CURL_TRC_M(दत्त, "ev update fd=%" FMT_SOCKET_T ", action '%s%s' -> '%s%s'"
             " (%u/%u r/w)", s,
             (चरमः_action & CURL_POLL_अन्तर्) ? "IN" : "",
             (चरमः_action & CURL_POLL_बहिर्गमनम्) ? "OUT" : "",
             (cur_action & CURL_POLL_अन्तर्) ? "IN" : "",
             (cur_action & CURL_POLL_बहिर्गमनम्) ? "OUT" : "",
             प्रविष्टिः->readers, प्रविष्टिः->writers);

  comboaction = (प्रविष्टिः->writers ? CURL_POLL_बहिर्गमनम् : 0) |
                (प्रविष्टिः->readers ? CURL_POLL_अन्तर् : 0);
  यदि(((पूर्णाङ्क)प्रविष्टिः->action == comboaction)) /* nothing for socket changed */
    प्रत्यागम CURLM_OK;

  CURL_TRC_M(दत्त, "ev update call(fd=%" FMT_SOCKET_T ", ev=%s%s)",
             s, (comboaction & CURL_POLL_अन्तर्) ? "IN" : "",
             (comboaction & CURL_POLL_बहिर्गमनम्) ? "OUT" : "");
  {
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_socket_cb);
    rc = multi->socket_cb(दत्त, s, comboaction, multi->socket_userp,
                          प्रविष्टिः->user_दत्त);
    CURL_CBAPI_MULTI_अन्तः(&guard);
  }
  यदि(rc == -1) {
    multi->dead = TRUE;
    प्रत्यागम CURLM_ABORTED_BY_CALLBACK;
  }
  /* curl_easy_pause() is documented as callable from any callback; it
   * re-enters mev_assess() which may free this 'entry'. Re-fetch. */
  प्रविष्टिः = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);
  यदि(प्रविष्टिः) {
    DEBUGASSERT(प्रविष्टिः->magic == SH_प्रविष्टिः_MAGIC);
    प्रविष्टिः->announced = TRUE;
    प्रविष्टिः->action = (अचिह्नित पूर्णाङ्क)comboaction;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode mev_pollset_diff(संरचना Curl_multi *multi,
                                  संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  संरचना easy_pollset *ps,
                                  संरचना easy_pollset *पूर्व_ps)
{
  संरचना mev_sh_प्रविष्टिः *प्रविष्टिः;
  curl_socket_t s;
  अचिह्नित पूर्णाङ्क i, j;
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
  प्रति(i = 0; i < ps->n; i++) {
    अचिह्नित वर्ण चरमः_action;
    bool first_time = FALSE; /* data/conn appears first time on socket */

    s = ps->sockets[i];
    /* Have we handled this socket before? */
    प्रविष्टिः = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);
    यदि(!प्रविष्टिः) {
      /* new socket, add new entry */
      first_time = TRUE;
      प्रविष्टिः = mev_sh_प्रविष्टिः_सङ्कलय(&multi->ev.sh_entries, s);
      यदि(!प्रविष्टिः) /* fatal */
        प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      CURL_TRC_M(दत्त, "ev new entry fd=%" FMT_SOCKET_T, s);
    }
    अन्यथा यदि(conn) {
      first_time = !mev_sh_प्रविष्टिः_conn_ज्ञातम्(प्रविष्टिः, conn);
    }
    अन्यथा {
      first_time = !mev_sh_प्रविष्टिः_xfer_ज्ञातम्(प्रविष्टिः, दत्त);
    }

    /* What was the previous action the transfer had regarding this socket?
     * If the transfer is new to the socket, disregard the information
     * in `last_poll`, because the socket might have been destroyed and
     * reopened. We would have cleared the sh_entry for that, but the socket
     * might still be mentioned in the hashed pollsets. */
    चरमः_action = 0;
    यदि(first_time) {
      यदि(conn) {
        यदि(!mev_sh_प्रविष्टिः_conn_सङ्कलय(प्रविष्टिः, conn))
          प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      अन्यथा {
        यदि(!mev_sh_प्रविष्टिः_xfer_सङ्कलय(प्रविष्टिः, दत्त))
          प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      CURL_TRC_M(दत्त, "ev entry fd=%" FMT_SOCKET_T ", added %s #%" FMT_OFF_T
                 ", total=%u/%d (xfer/conn)", s,
                 conn ? "connection" : "transfer",
                 conn ? conn->connection_id : दत्त->mid,
                 Curl_uint32_spbset_गणनम्(&प्रविष्टिः->xfers),
                 प्रविष्टिः->conn ? 1 : 0);
    }
    अन्यथा {
      प्रति(j = 0; j < पूर्व_ps->n; j++) {
        यदि(s == पूर्व_ps->sockets[j]) {
          चरमः_action = पूर्व_ps->actions[j];
          भङ्ग;
        }
      }
    }
    /* track readers/writers changes and report to socket callback */
    mresult = mev_sh_प्रविष्टिः_update(multi, दत्त, प्रविष्टिः, s,
                                  चरमः_action, ps->actions[i]);
    यदि(mresult)
      प्रत्यागम mresult;
  }

  /* Handle changes to sockets the transfer is NO LONGER interested in. */
  प्रति(i = 0; i < पूर्व_ps->n; i++) {
    bool stillused = FALSE;

    s = पूर्व_ps->sockets[i];
    प्रति(j = 0; j < ps->n; j++) {
      यदि(s == ps->sockets[j]) {
        /* socket is still supervised */
        stillused = TRUE;
        भङ्ग;
      }
    }
    यदि(stillused)
      अनुवर्त;

    प्रविष्टिः = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);
    /* if entry does not exist, we were either never told about it or
     * have already cleaned up this socket via Curl_multi_ev_socket_done().
     * In other words: this is perfectly normal */
    यदि(!प्रविष्टिः)
      अनुवर्त;

    यदि(conn && !mev_sh_प्रविष्टिः_conn_remove(प्रविष्टिः, conn)) {
      /* `conn` says in `prev_ps` that it had been using a socket,
       * but `conn` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(दत्त, "ev entry fd=%" FMT_SOCKET_T ", conn lost "
                 "interest but is not registered", s);
      DEBUGASSERT(रिक्त);
      अनुवर्त;
    }

    यदि(!conn && !mev_sh_प्रविष्टिः_xfer_remove(प्रविष्टिः, दत्त)) {
      /* `data` says in `prev_ps` that it had been using a socket,
       * but `data` has not been registered for it.
       * This should not happen if our book-keeping is correct? */
      CURL_TRC_M(दत्त, "ev entry fd=%" FMT_SOCKET_T ", transfer lost "
                 "interest but is not registered", s);
      DEBUGASSERT(रिक्त);
      अनुवर्त;
    }

    यदि(mev_sh_प्रविष्टिः_user_गणनम्(प्रविष्टिः)) {
      /* track readers/writers changes and report to socket callback */
      mresult = mev_sh_प्रविष्टिः_update(multi, दत्त, प्रविष्टिः, s,
                                    पूर्व_ps->actions[i], 0);
      यदि(mresult)
        प्रत्यागम mresult;
      CURL_TRC_M(दत्त, "ev entry fd=%" FMT_SOCKET_T ", removed transfer, "
                 "total=%u/%d (xfer/conn)", s,
                 Curl_uint32_spbset_गणनम्(&प्रविष्टिः->xfers),
                 प्रविष्टिः->conn ? 1 : 0);
    }
    अन्यथा {
      mresult = mev_forget_socket(multi, दत्त, s, "last user gone");
      यदि(mresult)
        प्रत्यागम mresult;
    }
  } /* for loop over num */

  /* Remember for next time */
  Curl_pollset_move(पूर्व_ps, ps);
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode mev_assess(संरचना Curl_multi *multi,
                            संरचना Curl_easy *दत्त,
                            संरचना connectdata *conn)
{
  संरचना easy_pollset ps, *चरमः_ps;
  CURLMcode mresult = CURLM_OK;

  यदि(!multi || !multi->socket_cb)
    प्रत्यागम CURLM_OK;

  Curl_pollset_init(&ps);
  यदि(conn) {
    CURLcode परिणाम = Curl_conn_adjust_pollset(दत्त, conn, &ps);
    यदि(परिणाम) {
      mresult = (परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) ?
        CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः : CURLM_आन्तरिकम्_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा
    Curl_multi_pollset(दत्त, &ps);
  चरमः_ps = mev_get_चरमः_pollset(दत्त, conn);

  यदि(!चरमः_ps && ps.n) {
    यदि(conn)
      चरमः_ps = mev_सङ्कलय_new_conn_pollset(conn);
    अन्यथा
      चरमः_ps = mev_सङ्कलय_new_xfer_pollset(दत्त);
    यदि(!चरमः_ps) {
      mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(चरमः_ps)
    mresult = mev_pollset_diff(multi, दत्त, conn, &ps, चरमः_ps);
  अन्यथा
    DEBUGASSERT(!ps.n);
बहिर्गमनम्:
  Curl_pollset_cleanup(&ps);
  प्रत्यागम mresult;
}

CURLMcode Curl_multi_ev_assess_xfer(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *दत्त)
{
  प्रत्यागम mev_assess(multi, दत्त, रिक्त);
}

CURLMcode Curl_multi_ev_assess_conn(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn)
{
  प्रत्यागम mev_assess(multi, दत्त, conn);
}

CURLMcode Curl_multi_ev_assess_xfer_bset(संरचना Curl_multi *multi,
                                         संरचना uint32_bset *समुच्चयः)
{
  uint32_t mid;
  CURLMcode mresult = CURLM_OK;

  यदि(multi && multi->socket_cb && Curl_uint32_bset_first(समुच्चयः, &mid)) {
    कुरु {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      यदि(दत्त) {
        mresult = Curl_multi_ev_assess_xfer(multi, दत्त);
      }
    } यावत्(!mresult && Curl_uint32_bset_अग्र(समुच्चयः, mid, &mid));
  }
  प्रत्यागम mresult;
}

CURLMcode Curl_multi_ev_नियोजय(संरचना Curl_multi *multi,
                               curl_socket_t s,
                               शून्य *user_दत्त)
{
  संरचना mev_sh_प्रविष्टिः *e = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);
  यदि(!e)
    प्रत्यागम CURLM_BAD_SOCKET;
  e->user_दत्त = user_दत्त;
  प्रत्यागम CURLM_OK;
}

शून्य Curl_multi_ev_dirty_xfers(संरचना Curl_multi *multi,
                               curl_socket_t s)
{
  संरचना mev_sh_प्रविष्टिः *प्रविष्टिः;

  DEBUGASSERT(s != CURL_SOCKET_TIMEOUT);
  प्रविष्टिः = mev_sh_प्रविष्टिः_get(&multi->ev.sh_entries, s);

  /* Unmatched socket, we cannot act on it but we ignore this fact. In
     real-world tests it has been proved that libevent can in fact give
     the application actions even though the socket was previously
     asked to get removed, so thus we better survive stray socket actions
     and move on. */
  यदि(प्रविष्टिः) {
    संरचना Curl_easy *दत्त;
    uint32_t mid;

    यदि(Curl_uint32_spbset_first(&प्रविष्टिः->xfers, &mid)) {
      कुरु {
        दत्त = Curl_multi_get_easy(multi, mid);
        यदि(दत्त) {
          Curl_multi_mark_dirty(दत्त);
        }
        अन्यथा {
          CURL_TRC_M(multi->admin, "socket transfer %u no longer found", mid);
          Curl_uint32_spbset_remove(&प्रविष्टिः->xfers, mid);
        }
      } यावत्(Curl_uint32_spbset_अग्र(&प्रविष्टिः->xfers, mid, &mid));
    }

    यदि(प्रविष्टिः->conn)
      Curl_multi_mark_dirty(multi->admin);
  }
}

शून्य Curl_multi_ev_socket_समाप्तम्(संरचना Curl_multi *multi,
                               संरचना Curl_easy *दत्त, curl_socket_t s)
{
  mev_forget_socket(multi, दत्त, s, "socket done");
}

शून्य Curl_multi_ev_xfer_समाप्तम्(संरचना Curl_multi *multi,
                             संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(!दत्त->conn); /* transfer should have been detached */
  (शून्य)mev_assess(multi, दत्त, रिक्त);
  Curl_meta_remove(दत्त, CURL_META_MEV_POLLSET);
}

शून्य Curl_multi_ev_conn_समाप्तम्(संरचना Curl_multi *multi,
                             संरचना Curl_easy *दत्त,
                             संरचना connectdata *conn)
{
  (शून्य)mev_assess(multi, दत्त, conn);
  Curl_conn_meta_remove(conn, CURL_META_MEV_POLLSET);
}

शून्य Curl_multi_ev_init(संरचना Curl_multi *multi, size_t hashsize)
{
  Curl_hash_init(&multi->ev.sh_entries, hashsize, CURL_HASH_TYPE_SOCKET,
                 mev_sh_प्रविष्टिः_dtor);
}

शून्य Curl_multi_ev_cleanup(संरचना Curl_multi *multi)
{
  Curl_hash_destroy(&multi->ev.sh_entries);
}
