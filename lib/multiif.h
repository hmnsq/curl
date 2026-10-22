#अगर_अघोषित HEADER_CURL_MULTIIF_H
#घोषणा HEADER_CURL_MULTIIF_H
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

व्योम Curl_expire(काष्ठा Curl_easy *तथ्य, timediff_t milli, expire_id eid);
व्योम Curl_expire_set(काष्ठा Curl_easy *तथ्य,
                     expire_id eid, timediff_t ms,
                     स्थिर काष्ठा curltime *pnow);
व्योम Curl_expire_clear(काष्ठा Curl_easy *तथ्य, expire_id eid);
व्योम Curl_expire_clear_all(काष्ठा Curl_easy *तथ्य);
CURLMcode Curl_update_timer(काष्ठा Curl_multi *multi) WARN_UNUSED_परिणाम;
व्योम Curl_attach_connection(काष्ठा Curl_easy *तथ्य,
                            काष्ठा connectdata *conn,
                            bool matched);
व्योम Curl_detach_connection(काष्ठा Curl_easy *तथ्य);
bool Curl_multiplex_wanted(स्थिर काष्ठा Curl_multi *multi);
CURLcode Curl_preconnect(काष्ठा Curl_easy *तथ्य);
bool Curl_is_connecting(काष्ठा Curl_easy *तथ्य);

व्योम Curl_multi_connchanged(काष्ठा Curl_multi *multi);

CURLMcode Curl_multi_add_handle(काष्ठा Curl_multi *multi,
                                काष्ठा Curl_easy *तथ्य);
CURLMcode Curl_multi_remove_handle(काष्ठा Curl_multi *multi,
                                   काष्ठा Curl_easy *तथ्य);

/* Internal version of curl_multi_init() accepts size parameters for the
   socket, connection and dns hashes */
काष्ठा Curl_multi *Curl_multi_handle(uint32_t xfer_table_आकार,
                                     माप_प्रकार ev_hashsize,
                                     माप_प्रकार chashsize,
                                     माप_प्रकार dnssize,
                                     माप_प्रकार sesssize);

/**
 * Let the multi handle know that the socket is about to be closed.
 * The multi will then remove anything it knows about the socket, so
 * when the OS is using this socket (number) again subsequently,
 * the internal book keeping will not get confused.
 */
व्योम Curl_multi_will_close(काष्ठा Curl_easy *तथ्य, curl_socket_t s);

/*
 * Add a handle and move it into PERFORM state at once. For pushed streams.
 */
CURLMcode Curl_multi_add_perform(काष्ठा Curl_multi *multi,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा connectdata *conn);

/* Return the value of the CURLMOPT_MAX_CONCURRENT_STREAMS option */
uint32_t Curl_multi_max_concurrent_streams(काष्ठा Curl_multi *multi);

CURLMcode Curl_multi_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps);

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
CURLcode Curl_multi_xfer_buf_borrow(काष्ठा Curl_easy *तथ्य,
                                    अक्षर **pbuf, माप_प्रकार *pbuflen);
/**
 * Release the borrowed buffer. All references into the buffer become
 * invalid after this.
 * @param buf the buffer pointer borrowed for coding error checks.
 */
व्योम Curl_multi_xfer_buf_release(काष्ठा Curl_easy *तथ्य, अक्षर *buf);

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
CURLcode Curl_multi_xfer_ulbuf_borrow(काष्ठा Curl_easy *तथ्य,
                                      अक्षर **pbuf, माप_प्रकार *pbuflen);

/**
 * Release the borrowed upload buffer. All references into the buffer become
 * invalid after this.
 * @param buf the upload buffer pointer borrowed for coding error checks.
 */
व्योम Curl_multi_xfer_ulbuf_release(काष्ठा Curl_easy *तथ्य, अक्षर *buf);

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
CURLcode Curl_multi_xfer_sockbuf_borrow(काष्ठा Curl_easy *तथ्य,
                                        माप_प्रकार blen, अक्षर **pbuf);
/**
 * Release the borrowed buffer. All references into the buffer become
 * invalid after this.
 * @param buf the buffer pointer borrowed for coding error checks.
 */
व्योम Curl_multi_xfer_sockbuf_release(काष्ठा Curl_easy *तथ्य, अक्षर *buf);

/**
 * Get the easy handle for the given mid.
 * Returns NULL if not found or not a GOOD_EASY_HANDLE()
 */
काष्ठा Curl_easy *Curl_multi_get_easy(काष्ठा Curl_multi *multi,
                                      uint32_t mid);

/* TRUE if multi knows about data via its `mid` */
bool Curl_multi_knows_easy(काष्ठा Curl_multi *multi, काष्ठा Curl_easy *तथ्य);

/* Get the # of transfers attached to the multi, without the internal
 * admin handle. */
uint32_t Curl_multi_xfers_attached(काष्ठा Curl_multi *multi);

/* Mark a transfer as dirty, e.g. to be rerun at earliest convenience.
 * A cheap operation, can be done many times repeatedly. */
व्योम Curl_multi_mark_dirty(काष्ठा Curl_easy *तथ्य);
/* Clear transfer from the dirty set. */
व्योम Curl_multi_clear_dirty(काष्ठा Curl_easy *तथ्य);

व्योम Curl_multi_set_now(काष्ठा Curl_multi *multi);

#अगर_घोषित ENABLE_INTERNAL_WAKEUP
व्योम Curl_multi_wakeup_internal(काष्ठा Curl_multi *multi);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_MULTIIF_H */
