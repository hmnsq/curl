#ifndef HEADER_CURL_VTLS_SCACHE_H
#define HEADER_CURL_VTLS_SCACHE_H
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

#ifdef USE_SSL

#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "urldata.h"

સંરચના Curl_cfilter;
સંરચના Curl_easy;
સંરચના Curl_ssl_scache;
સંરચના Curl_ssl_session;
સંરચના ssl_peer;

/* RFC 8446 (TLSv1.3) restrict lifetime to one week max, for
 * other, less secure versions, we restrict it to a day */
#define CURL_SCACHE_MAX_13_LIFETIME_SEC    (60 * 60 * 24 * 7)
#define CURL_SCACHE_MAX_12_LIFETIME_SEC    (60 * 60 * 24)

/* Create a session cache for up to max_peers endpoints with a total
 * of up to max_sessions SSL sessions per peer */
CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                સંરચના Curl_ssl_scache **pscache);

શૂન્ય Curl_ssl_scache_destroy(સંરચના Curl_ssl_scache *scache);

/* Create a key from peer and TLS configuration information that is
 * unique for how the connection filter wants to establish a TLS
 * connection to the peer.
 * If the filter is a TLS proxy filter, it uses the proxy relevant
 * information.
 * @param peer    the peer the filter wants to talk to
 * @param sslc    the relevant ssl configuration
 * @param tls_id  identifier of TLS implementation for sessions. Should
 *                include full version if session data from other versions
 *                is to be avoided.
 * @param ppeer_key on successful return, the key generated
 */
CURLcode Curl_ssl_peer_ચાવી_make(અચલ સંરચના ssl_peer *peer,
                                સંરચના ssl_filter_config *sslc,
                                અચલ અક્ષર *tls_id,
                                અક્ષર **ppeer_ચાવી);

/* Return if there is a session cache shall be used.
 * An SSL session might not be configured or not available for
 * "connect-only" transfers.
 */
bool Curl_ssl_scache_use(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data);

/* Lock session cache mutex.
 * Call this before calling other Curl_ssl_*session* functions
 * Caller should unlock this mutex as soon as possible, as it may block
 * other SSL connection from making progress.
 * The purpose of explicitly locking SSL session cache data is to allow
 * individual SSL engines to manage session lifetime in their specific way.
 */
શૂન્ય Curl_ssl_scache_lock(સંરચના Curl_easy *data);

/* Unlock session cache mutex */
શૂન્ય Curl_ssl_scache_unlock(સંરચના Curl_easy *data);

/* Get TLS session object from the cache for the ssl_peer_key.
 * scache mutex must be locked (see Curl_ssl_scache_lock).
 * Caller must make sure that the ownership of returned session object
 * is properly taken (e.g. its refcount is incremented
 * under scache mutex).
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @retval sobj   the object for the peer key or NULL
 */
શૂન્ય *Curl_ssl_scache_get_obj(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              અચલ અક્ષર *ssl_peer_ચાવી);

પ્રકારવ્યાખ્યા શૂન્ય Curl_ssl_scache_obj_dtor(શૂન્ય *sobj);

/* Add a TLS session related object to the cache.
 * Replaces an existing object with the same peer_key.
 * scache mutex must be locked (see Curl_ssl_scache_lock).
 * Call takes ownership of `sobj`, using `sobj_dtor_cb`
 * to deallocate it. Is called in all outcomes, either right away or
 * later when the session cache is cleaned up.
 * Caller must ensure that it has properly shared ownership of `sobj`
 * with cache (e.g. incrementing refcount on success)
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param sobj    the TLS session object
 * @param sobj_free_cb callback to free the session object
 */
CURLcode Curl_ssl_scache_add_obj(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 શૂન્ય *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb);

/* All about an SSL session ticket */
સંરચના Curl_ssl_session {
  uint8_t *sdata;              /* session ticket data, plain bytes */
  size_t sdata_len;            /* number of bytes in sdata */
  curl_off_t valid_until;      /* seconds since EPOCH until ticket expires */
  પૂર્ણાંક ietf_tls_id;             /* TLS protocol identifier negotiated */
  અક્ષર *alpn;                  /* APLN TLS negotiated protocol string */
  size_t earlydata_max;        /* max 0-RTT data supported by peer */
  uint8_t *quic_tp;            /* Optional QUIC transport param bytes */
  size_t quic_tp_len;          /* number of bytes in quic_tp */
  સંરચના Curl_llist_ગાંઠ list; /*  internal storage handling */
  BIT(sectrust_verified);      /* session comes from sectrust verified TLS */
};

/* Create a `session` instance. Does NOT need locking.
 * Takes ownership of `sdata` and `sobj` regardless of return code.
 * @param sdata     bytes of SSL session data or NULL (sobj then required)
 * @param sdata_len amount of session data bytes
 * @param ietf_tls_id  IETF protocol version, e.g. 0x304 for TLSv1.3
 * @param alpn      ALPN protocol selected or NULL
 * @param valid_until seconds since EPOCH when session expires, pass 0
 *                  in case this is not known.
 * @param psession on return the scached session instance created
 */
CURLcode Curl_ssl_session_create(શૂન્ય *sdata, size_t sdata_len,
                                 પૂર્ણાંક ietf_tls_id, અચલ અક્ષર *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 સંરચના Curl_ssl_session **psession);

/* Variation of session creation with quic transport parameter bytes,
 * Takes ownership of `quic_tp` regardless of return code. */
CURLcode Curl_ssl_session_create2(શૂન્ય *sdata, size_t sdata_len,
                                  પૂર્ણાંક ietf_tls_id, અચલ અક્ષર *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned અક્ષર *quic_tp, size_t quic_tp_len,
                                  સંરચના Curl_ssl_session **psession);

/* Duplicate an ssl session */
CURLcode Curl_ssl_session_dup(સંરચના Curl_ssl_session *src,
                              સંરચના Curl_ssl_session **pdest);

/* Destroy a `session` instance. Can be called with NULL.
 * Does NOT need locking. */
શૂન્ય Curl_ssl_session_destroy(સંરચના Curl_ssl_session *s);

/* Put the scache session into the cache. Does NOT need locking.
 * Call takes ownership of `s` in all outcomes.
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param s       the scache session object
 */
CURLcode Curl_ssl_scache_put(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             અચલ અક્ષર *ssl_peer_ચાવી,
                             સંરચના Curl_ssl_session *s);

/* Take a matching scache session from the cache. Does NOT need locking.
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param s       on return, the scache session object or NULL
 */
CURLcode Curl_ssl_scache_take(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              અચલ અક્ષર *ssl_peer_ચાવી,
                              સંરચના Curl_ssl_session **ps);

/* Return a taken scache session to the cache. Does NOT need locking.
 * Depending on TLS version and other criteria, it may cache it again
 * or destroy it. Maybe called with a NULL session.
 */
શૂન્ય Curl_ssl_scache_return(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            અચલ અક્ષર *ssl_peer_ચાવી,
                            સંરચના Curl_ssl_session *s);

/* Remove all sessions and obj for the peer_key. Does NOT need locking. */
શૂન્ય Curl_ssl_scache_remove_all(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                અચલ અક્ષર *ssl_peer_ચાવી);

bool Curl_ssl_scache_is_locked_by_current_thread(સંરચના Curl_easy *data);

#ifdef USE_SSLS_EXPORT

CURLcode Curl_ssl_session_import(સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 અચલ unsigned અક્ષર *shmac, size_t shmac_len,
                                 અચલ શૂન્ય *sdata, size_t sdata_len);

CURLcode Curl_ssl_session_export(સંરચના Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 શૂન્ય *userptr);

#endif /* USE_SSLS_EXPORT */
#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_SCACHE_H */
