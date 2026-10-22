#ifndef HEADER_CURL_MULTIIF_H
#define HEADER_CURL_MULTIIF_H
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
/*
 * Prototypes for library-wide functions provided by multi.c
 */

શૂન્ય Curl_expire(સંરચના Curl_easy *data, timediff_t milli, expire_id eid);
શૂન્ય Curl_expire_set(સંરચના Curl_easy *data,
                     expire_id eid, timediff_t ms,
                     અચલ સંરચના curltime *pnow);
શૂન્ય Curl_expire_clear(સંરચના Curl_easy *data, expire_id eid);
શૂન્ય Curl_expire_clear_all(સંરચના Curl_easy *data);
CURLMcode Curl_update_timer(સંરચના Curl_multi *multi) WARN_UNUSED_RESULT;
શૂન્ય Curl_attach_connection(સંરચના Curl_easy *data,
                            સંરચના connectdata *conn,
                            bool matched);
શૂન્ય Curl_detach_connection(સંરચના Curl_easy *data);
bool Curl_multiplex_wanted(અચલ સંરચના Curl_multi *multi);
CURLcode Curl_preconnect(સંરચના Curl_easy *data);
bool Curl_is_connecting(સંરચના Curl_easy *data);

શૂન્ય Curl_multi_connchanged(સંરચના Curl_multi *multi);

CURLMcode Curl_multi_add_handle(સંરચના Curl_multi *multi,
                                સંરચના Curl_easy *data);
CURLMcode Curl_multi_remove_handle(સંરચના Curl_multi *multi,
                                   સંરચના Curl_easy *data);

/* Internal version of curl_multi_init() accepts size parameters for the
   socket, connection and dns hashes */
સંરચના Curl_multi *Curl_multi_handle(uint32_t xfer_table_size,
                                     size_t ev_hashsize,
                                     size_t chashsize,
                                     size_t dnssize,
                                     size_t sesssize);

/**
 * Let the multi handle know that the socket is about to be closed.
 * The multi will then remove anything it knows about the socket, so
 * when the OS is using this socket (number) again subsequently,
 * the internal book keeping will not get confused.
 */
શૂન્ય Curl_multi_will_close(સંરચના Curl_easy *data, curl_socket_t s);

/*
 * Add a handle and move it into PERFORM state at once. For pushed streams.
 */
CURLMcode Curl_multi_add_perform(સંરચના Curl_multi *multi,
                                 સંરચના Curl_easy *data,
                                 સંરચના connectdata *conn);

/* Return the value of the CURLMOPT_MAX_CONCURRENT_STREAMS option */
uint32_t Curl_multi_max_concurrent_streams(સંરચના Curl_multi *multi);

CURLMcode Curl_multi_pollset(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps);

/**
 * Borrow the transfer buffer from the multi, suitable
 * for the given transfer `data`. The buffer may only be used in one
 * multi processing of the easy handle. It MUST be returned to the
 * multi before it can be borrowed again.
 * Pointers into the buffer remain only valid as long as it is borrowed.
 *
 * @param data    the easy handle
 * @param pbuf    on return, the buffer to use or NULL on error
 * @param pbuflen on return, the size of *pbuf or 0 on error
 * @return CURLE_OK when buffer is available and is returned.
 *         CURLE_OUT_OF_MEMORY on failure to allocate the buffer,
 *         CURLE_FAILED_INIT if the easy handle is without multi.
 *         CURLE_AGAIN if the buffer is borrowed already.
 */
CURLcode Curl_multi_xfer_buf_borrow(સંરચના Curl_easy *data,
                                    અક્ષર **pbuf, size_t *pbuflen);
/**
 * Release the borrowed buffer. All references into the buffer become
 * invalid after this.
 * @param buf the buffer pointer borrowed for coding error checks.
 */
શૂન્ય Curl_multi_xfer_buf_release(સંરચના Curl_easy *data, અક્ષર *buf);

/**
 * Borrow the upload buffer from the multi, suitable
 * for the given transfer `data`. The buffer may only be used in one
 * multi processing of the easy handle. It MUST be returned to the
 * multi before it can be borrowed again.
 * Pointers into the buffer remain only valid as long as it is borrowed.
 *
 * @param data    the easy handle
 * @param pbuf    on return, the buffer to use or NULL on error
 * @param pbuflen on return, the size of *pbuf or 0 on error
 * @return CURLE_OK when buffer is available and is returned.
 *         CURLE_OUT_OF_MEMORY on failure to allocate the buffer,
 *         CURLE_FAILED_INIT if the easy handle is without multi.
 *         CURLE_AGAIN if the buffer is borrowed already.
 */
CURLcode Curl_multi_xfer_ulbuf_borrow(સંરચના Curl_easy *data,
                                      અક્ષર **pbuf, size_t *pbuflen);

/**
 * Release the borrowed upload buffer. All references into the buffer become
 * invalid after this.
 * @param buf the upload buffer pointer borrowed for coding error checks.
 */
શૂન્ય Curl_multi_xfer_ulbuf_release(સંરચના Curl_easy *data, અક્ષર *buf);

/**
 * Borrow the socket scratch buffer from the multi, suitable
 * for the given transfer `data`. The buffer may only be used for
 * direct socket I/O operation by one connection at a time and MUST be
 * returned to the multi before the I/O call returns.
 * Pointers into the buffer remain only valid as long as it is borrowed.
 *
 * @param data    the easy handle
 * @param blen    requested length of the buffer
 * @param pbuf    on return, the buffer to use or NULL on error
 * @return CURLE_OK when buffer is available and is returned.
 *         CURLE_OUT_OF_MEMORY on failure to allocate the buffer,
 *         CURLE_AGAIN if the buffer is borrowed already.
 */
CURLcode Curl_multi_xfer_sockbuf_borrow(સંરચના Curl_easy *data,
                                        size_t blen, અક્ષર **pbuf);
/**
 * Release the borrowed buffer. All references into the buffer become
 * invalid after this.
 * @param buf the buffer pointer borrowed for coding error checks.
 */
શૂન્ય Curl_multi_xfer_sockbuf_release(સંરચના Curl_easy *data, અક્ષર *buf);

/**
 * Get the easy handle for the given mid.
 * Returns NULL if not found or not a GOOD_EASY_HANDLE()
 */
સંરચના Curl_easy *Curl_multi_get_easy(સંરચના Curl_multi *multi,
                                      uint32_t mid);

/* TRUE if multi knows about data via its `mid` */
bool Curl_multi_knows_easy(સંરચના Curl_multi *multi, સંરચના Curl_easy *data);

/* Get the # of transfers attached to the multi, without the internal
 * admin handle. */
uint32_t Curl_multi_xfers_attached(સંરચના Curl_multi *multi);

/* Mark a transfer as dirty, e.g. to be rerun at earliest convenience.
 * A cheap operation, can be done many times repeatedly. */
શૂન્ય Curl_multi_mark_dirty(સંરચના Curl_easy *data);
/* Clear transfer from the dirty set. */
શૂન્ય Curl_multi_clear_dirty(સંરચના Curl_easy *data);

શૂન્ય Curl_multi_set_now(સંરચના Curl_multi *multi);

#ifdef ENABLE_INTERNAL_WAKEUP
શૂન્ય Curl_multi_wakeup_internal(સંરચના Curl_multi *multi);
#endif

#endif /* HEADER_CURL_MULTIIF_H */
