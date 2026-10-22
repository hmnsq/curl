#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MULTIIF_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MULTIIF_H
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

ಶೂನ್ಯ Curl_expire(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, timediff_t milli, expire_id eid);
ಶೂನ್ಯ Curl_expire_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     expire_id eid, timediff_t ms,
                     ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);
ಶೂನ್ಯ Curl_expire_clear(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, expire_id eid);
ಶೂನ್ಯ Curl_expire_clear_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLMcode Curl_update_timer(ರಚನೆ Curl_multi *multi) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
ಶೂನ್ಯ Curl_attach_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn,
                            bool matched);
ಶೂನ್ಯ Curl_detach_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
bool Curl_multiplex_wanted(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_multi *multi);
CURLcode Curl_preconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
bool Curl_is_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಶೂನ್ಯ Curl_multi_connchanged(ರಚನೆ Curl_multi *multi);

CURLMcode Curl_multi_ಕೂಡಿಸು_handle(ರಚನೆ Curl_multi *multi,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLMcode Curl_multi_remove_handle(ರಚನೆ Curl_multi *multi,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Internal version of curl_multi_init() accepts size parameters for the
   socket, connection and dns hashes */
ರಚನೆ Curl_multi *Curl_multi_handle(uint32_t xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ,
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
ಶೂನ್ಯ Curl_multi_will_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t s);

/*
 * Add a handle and move it into PERFORM state at once. For pushed streams.
 */
CURLMcode Curl_multi_ಕೂಡಿಸು_perform(ರಚನೆ Curl_multi *multi,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ connectdata *conn);

/* Return the value of the CURLMOPT_MAX_CONCURRENT_STREAMS option */
uint32_t Curl_multi_max_concurrent_streams(ರಚನೆ Curl_multi *multi);

CURLMcode Curl_multi_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps);

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
CURLcode Curl_multi_xfer_buf_borrow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಅಕ್ಷರ **pbuf, size_t *pbuflen);
/**
 * Release the borrowed buffer. All references into the buffer become
 * invalid after this.
 * @param buf the buffer pointer borrowed for coding error checks.
 */
ಶೂನ್ಯ Curl_multi_xfer_buf_release(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf);

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
CURLcode Curl_multi_xfer_ulbuf_borrow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ಅಕ್ಷರ **pbuf, size_t *pbuflen);

/**
 * Release the borrowed upload buffer. All references into the buffer become
 * invalid after this.
 * @param buf the upload buffer pointer borrowed for coding error checks.
 */
ಶೂನ್ಯ Curl_multi_xfer_ulbuf_release(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf);

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
CURLcode Curl_multi_xfer_sockbuf_borrow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        size_t blen, ಅಕ್ಷರ **pbuf);
/**
 * Release the borrowed buffer. All references into the buffer become
 * invalid after this.
 * @param buf the buffer pointer borrowed for coding error checks.
 */
ಶೂನ್ಯ Curl_multi_xfer_sockbuf_release(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf);

/**
 * Get the easy handle for the given mid.
 * Returns NULL if not found or not a GOOD_EASY_HANDLE()
 */
ರಚನೆ Curl_easy *Curl_multi_get_easy(ರಚನೆ Curl_multi *multi,
                                      uint32_t mid);

/* TRUE if multi knows about data via its `mid` */
bool Curl_multi_knows_easy(ರಚನೆ Curl_multi *multi, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Get the # of transfers attached to the multi, without the internal
 * admin handle. */
uint32_t Curl_multi_xfers_attached(ರಚನೆ Curl_multi *multi);

/* Mark a transfer as dirty, e.g. to be rerun at earliest convenience.
 * A cheap operation, can be done many times repeatedly. */
ಶೂನ್ಯ Curl_multi_mark_dirty(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
/* Clear transfer from the dirty set. */
ಶೂನ್ಯ Curl_multi_clear_dirty(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಶೂನ್ಯ Curl_multi_ಗಣ_now(ರಚನೆ Curl_multi *multi);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
ಶೂನ್ಯ Curl_multi_wakeup_ಆಂತರಿಕ(ರಚನೆ Curl_multi *multi);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MULTIIF_H */
