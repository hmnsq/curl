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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_SSL

#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"

ସଂରଚନା Curl_cfilter;
ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_ssl_scache;
ସଂରଚନା Curl_ssl_session;
ସଂରଚନା ssl_peer;

/* RFC 8446 (TLSv1.3) restrict lifetime to one week max, for
 * other, less secure versions, we restrict it to a day */
#define CURL_SCACHE_MAX_13_LIFETIME_SEC    (60 * 60 * 24 * 7)
#define CURL_SCACHE_MAX_12_LIFETIME_SEC    (60 * 60 * 24)

/* Create a session cache for up to max_peers endpoints with a total
 * of up to max_sessions SSL sessions per peer */
CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                ସଂରଚନା Curl_ssl_scache **pscache);

ଶୂନ୍ୟ Curl_ssl_scache_destroy(ସଂରଚନା Curl_ssl_scache *scache);

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
CURLcode Curl_ssl_peer_ଚାବି_make(ଅଚଳ ସଂରଚନା ssl_peer *peer,
                                ସଂରଚନା ssl_filter_config *sslc,
                                ଅଚଳ ଅକ୍ଷର *tls_id,
                                ଅକ୍ଷର **ppeer_ଚାବି);

/* Return if there is a session cache shall be used.
 * An SSL session might not be configured or not available for
 * "connect-only" transfers.
 */
bool Curl_ssl_scache_use(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data);

/* Lock session cache mutex.
 * Call this before calling other Curl_ssl_*session* functions
 * Caller should unlock this mutex as soon as possible, as it may block
 * other SSL connection from making progress.
 * The purpose of explicitly locking SSL session cache data is to allow
 * individual SSL engines to manage session lifetime in their specific way.
 */
ଶୂନ୍ୟ Curl_ssl_scache_lock(ସଂରଚନା Curl_easy *data);

/* Unlock session cache mutex */
ଶୂନ୍ୟ Curl_ssl_scache_unlock(ସଂରଚନା Curl_easy *data);

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
ଶୂନ୍ୟ *Curl_ssl_scache_get_obj(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ Curl_ssl_scache_obj_dtor(ଶୂନ୍ୟ *sobj);

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
CURLcode Curl_ssl_scache_add_obj(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                 ଶୂନ୍ୟ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb);

/* All about an SSL session ticket */
ସଂରଚନା Curl_ssl_session {
  uint8_t *sdata;              /* session ticket data, plain bytes */
  size_t sdata_len;            /* number of bytes in sdata */
  curl_off_t valid_until;      /* seconds since EPOCH until ticket expires */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id;             /* TLS protocol identifier negotiated */
  ଅକ୍ଷର *alpn;                  /* APLN TLS negotiated protocol string */
  size_t earlydata_max;        /* max 0-RTT data supported by peer */
  uint8_t *quic_tp;            /* Optional QUIC transport param bytes */
  size_t quic_tp_len;          /* number of bytes in quic_tp */
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି list; /*  internal storage handling */
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
CURLcode Curl_ssl_session_create(ଶୂନ୍ୟ *sdata, size_t sdata_len,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id, ଅଚଳ ଅକ୍ଷର *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ସଂରଚନା Curl_ssl_session **psession);

/* Variation of session creation with quic transport parameter bytes,
 * Takes ownership of `quic_tp` regardless of return code. */
CURLcode Curl_ssl_session_create2(ଶୂନ୍ୟ *sdata, size_t sdata_len,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id, ଅଚଳ ଅକ୍ଷର *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned ଅକ୍ଷର *quic_tp, size_t quic_tp_len,
                                  ସଂରଚନା Curl_ssl_session **psession);

/* Duplicate an ssl session */
CURLcode Curl_ssl_session_dup(ସଂରଚନା Curl_ssl_session *src,
                              ସଂରଚନା Curl_ssl_session **pdest);

/* Destroy a `session` instance. Can be called with NULL.
 * Does NOT need locking. */
ଶୂନ୍ୟ Curl_ssl_session_destroy(ସଂରଚନା Curl_ssl_session *s);

/* Put the scache session into the cache. Does NOT need locking.
 * Call takes ownership of `s` in all outcomes.
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param s       the scache session object
 */
CURLcode Curl_ssl_scache_put(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                             ସଂରଚନା Curl_ssl_session *s);

/* Take a matching scache session from the cache. Does NOT need locking.
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param s       on return, the scache session object or NULL
 */
CURLcode Curl_ssl_scache_take(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                              ସଂରଚନା Curl_ssl_session **ps);

/* Return a taken scache session to the cache. Does NOT need locking.
 * Depending on TLS version and other criteria, it may cache it again
 * or destroy it. Maybe called with a NULL session.
 */
ଶୂନ୍ୟ Curl_ssl_scache_return(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                            ସଂରଚନା Curl_ssl_session *s);

/* Remove all sessions and obj for the peer_key. Does NOT need locking. */
ଶୂନ୍ୟ Curl_ssl_scache_remove_all(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି);

bool Curl_ssl_scache_is_locked_by_current_thread(ସଂରଚନା Curl_easy *data);

#ifdef USE_SSLS_EXPORT

CURLcode Curl_ssl_session_import(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                 ଅଚଳ unsigned ଅକ୍ଷର *shmac, size_t shmac_len,
                                 ଅଚଳ ଶୂନ୍ୟ *sdata, size_t sdata_len);

CURLcode Curl_ssl_session_export(ସଂରଚନା Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 ଶୂନ୍ୟ *userptr);

#endif /* USE_SSLS_EXPORT */
#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_SCACHE_H */
