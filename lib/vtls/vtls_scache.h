#यदि_अपरिभाषितम् शीर्षकम्_CURL_VTLS_SCACHE_H
#परिभाषय शीर्षकम्_CURL_VTLS_SCACHE_H
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

#यदि_परिभाषितम् USE_SSL

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "urldata.h"

संरचना Curl_cfilter;
संरचना Curl_easy;
संरचना Curl_ssl_scache;
संरचना Curl_ssl_session;
संरचना ssl_peer;

/* RFC 8446 (TLSv1.3) restrict lifetime to one week max, for
 * other, less secure versions, we restrict it to a day */
#परिभाषय CURL_SCACHE_MAX_13_LIFETIME_SEC    (60 * 60 * 24 * 7)
#परिभाषय CURL_SCACHE_MAX_12_LIFETIME_SEC    (60 * 60 * 24)

/* Create a session cache for up to max_peers endpoints with a total
 * of up to max_sessions SSL sessions per peer */
CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_प्रत्येकम्_peer,
                                संरचना Curl_ssl_scache **pscache);

शून्य Curl_ssl_scache_destroy(संरचना Curl_ssl_scache *scache);

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
CURLcode Curl_ssl_peer_कुञ्चिका_make(ध्रुव संरचना ssl_peer *peer,
                                संरचना ssl_filter_config *sslc,
                                ध्रुव वर्ण *tls_id,
                                वर्ण **ppeer_कुञ्चिका);

/* Return if there is a session cache shall be used.
 * An SSL session might not be configured or not available for
 * "connect-only" transfers.
 */
bool Curl_ssl_scache_use(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त);

/* Lock session cache mutex.
 * Call this before calling other Curl_ssl_*session* functions
 * Caller should unlock this mutex as soon as possible, as it may block
 * other SSL connection from making progress.
 * The purpose of explicitly locking SSL session cache data is to allow
 * individual SSL engines to manage session lifetime in their specific way.
 */
शून्य Curl_ssl_scache_तालकम्(संरचना Curl_easy *दत्त);

/* Unlock session cache mutex */
शून्य Curl_ssl_scache_unlock(संरचना Curl_easy *दत्त);

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
शून्य *Curl_ssl_scache_get_obj(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *ssl_peer_कुञ्चिका);

प्रकारनिर्देश शून्य Curl_ssl_scache_obj_dtor(शून्य *sobj);

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
CURLcode Curl_ssl_scache_सङ्कलय_obj(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 शून्य *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb);

/* All about an SSL session ticket */
संरचना Curl_ssl_session {
  uint8_t *sdata;              /* session ticket data, plain bytes */
  size_t sdata_len;            /* number of bytes in sdata */
  curl_off_t valid_until;      /* seconds since EPOCH until ticket expires */
  पूर्णाङ्क ietf_tls_id;             /* TLS protocol identifier negotiated */
  वर्ण *alpn;                  /* APLN TLS negotiated protocol string */
  size_t earlydata_max;        /* max 0-RTT data supported by peer */
  uint8_t *quic_tp;            /* Optional QUIC transport param bytes */
  size_t quic_tp_len;          /* number of bytes in quic_tp */
  संरचना Curl_llist_ग्रन्थिः list; /*  internal storage handling */
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
CURLcode Curl_ssl_session_create(शून्य *sdata, size_t sdata_len,
                                 पूर्णाङ्क ietf_tls_id, ध्रुव वर्ण *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 संरचना Curl_ssl_session **psession);

/* Variation of session creation with quic transport parameter bytes,
 * Takes ownership of `quic_tp` regardless of return code. */
CURLcode Curl_ssl_session_create2(शून्य *sdata, size_t sdata_len,
                                  पूर्णाङ्क ietf_tls_id, ध्रुव वर्ण *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  अचिह्नित वर्ण *quic_tp, size_t quic_tp_len,
                                  संरचना Curl_ssl_session **psession);

/* Duplicate an ssl session */
CURLcode Curl_ssl_session_dup(संरचना Curl_ssl_session *src,
                              संरचना Curl_ssl_session **pdest);

/* Destroy a `session` instance. Can be called with NULL.
 * Does NOT need locking. */
शून्य Curl_ssl_session_destroy(संरचना Curl_ssl_session *s);

/* Put the scache session into the cache. Does NOT need locking.
 * Call takes ownership of `s` in all outcomes.
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param s       the scache session object
 */
CURLcode Curl_ssl_scache_put(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                             संरचना Curl_ssl_session *s);

/* Take a matching scache session from the cache. Does NOT need locking.
 * @param cf      the connection filter wanting to use it
 * @param data    the transfer involved
 * @param ssl_peer_key the key for lookup
 * @param s       on return, the scache session object or NULL
 */
CURLcode Curl_ssl_scache_take(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                              संरचना Curl_ssl_session **ps);

/* Return a taken scache session to the cache. Does NOT need locking.
 * Depending on TLS version and other criteria, it may cache it again
 * or destroy it. Maybe called with a NULL session.
 */
शून्य Curl_ssl_scache_return(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                            संरचना Curl_ssl_session *s);

/* Remove all sessions and obj for the peer_key. Does NOT need locking. */
शून्य Curl_ssl_scache_remove_all(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                ध्रुव वर्ण *ssl_peer_कुञ्चिका);

bool Curl_ssl_scache_is_locked_by_current_thread(संरचना Curl_easy *दत्त);

#यदि_परिभाषितम् USE_SSLS_EXPORT

CURLcode Curl_ssl_session_import(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 ध्रुव अचिह्नित वर्ण *shmac, size_t shmac_len,
                                 ध्रुव शून्य *sdata, size_t sdata_len);

CURLcode Curl_ssl_session_export(संरचना Curl_easy *दत्त,
                                 curl_ssls_export_cb *export_fn,
                                 शून्य *userptr);

#यदि_अन्तः /* USE_SSLS_EXPORT */
#यदि_अन्तः /* USE_SSL */

#यदि_अन्तः /* HEADER_CURL_VTLS_SCACHE_H */
