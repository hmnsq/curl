#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_TRANSFER_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_TRANSFER_H
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_headersep(x) ((((x) == ':') || ((x) == ';')))

ਅੱਖਰ *Curl_checkheaders(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *thisheader,
                        ਅਚਲ size_t thislen);

ਖਾਲੀ Curl_init_CONNECT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_pretransfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_sendrecv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_retry_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ **url);
bool Curl_meets_timecondition(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, time_t timeofdoc);

/**
 * Write the transfer raw response bytes, as received from the connection.
 * Handle all passed bytes or return an error. By default, this writes
 * the bytes as BODY to the client. Protocols may provide a
 * "write_resp" callback in their handler to add specific treatment. E.g.
 * HTTP parses response headers and passes them differently to the client.
 * @param data     the transfer
 * @param buf      the raw response bytes
 * @param blen     the amount of bytes in `buf`
 * @param is_eos   TRUE iff the connection indicates this to be the last
 *                 bytes of the response
 */
CURLcode Curl_xfer_write_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *buf, size_t blen,
                              bool is_eos);

bool Curl_xfer_write_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Write a single "header" line from a server response.
 * @param hd0      the null-terminated, single header line
 * @param hdlen    the length of the header line
 * @param is_eos   TRUE iff this is the end of the response
 */
CURLcode Curl_xfer_write_resp_hd(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *hd0, size_t hdlen, bool is_eos);

/* The transfer is neither receiving nor sending. */
ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_nop(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* The transfer sends data on the given socket index */
ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          int8_t sockindex);

/* The transfer receives data on the given socket index, the
 * amount to receive (or -1 if unknown). */
ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          int8_t sockindex,
                          curl_off_t recv_ਮਾਪ);

/* *After* Curl_xfer_setup_xxx(), tell the transfer to shutdown the
 * connection at the end. Let the transfer either fail or ignore any
 * errors during shutdown. */
ਖਾਲੀ Curl_xfer_ਸੈੱਟ_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool shutdown,
                            bool ignore_errors);

/**
 * The transfer will use socket 1 to send/recv. `recv_size` is
 * the amount to receive or -1 if unknown.
 */
ਖਾਲੀ Curl_xfer_ਤਿਆਰੀ_sendrecv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              int8_t sockindex,
                              curl_off_t recv_ਮਾਪ);

/**
 * Multi has set transfer to DONE. Last chance to trigger
 * missing response things like writing an EOS to the client.
 */
CURLcode Curl_xfer_write_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool premature);

/**
 * Return TRUE iff transfer has pending data to send. Checks involved
 * connection filters.
 */
bool Curl_xfer_needs_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Flush any pending send data on the transfer connection.
 */
CURLcode Curl_xfer_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * Send data on the socket/connection filter designated
 * for transfer's outgoing data.
 * Return CURLE_OK on blocking with (*pnwritten == 0).
 */
CURLcode Curl_xfer_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਖਾਲੀ *buf, size_t blen, bool eos,
                        size_t *pnwritten);

/**
 * Receive data on the socket/connection filter designated
 * for transfer's incoming data.
 * Return CURLE_AGAIN on blocking with (*pnrcvd == 0).
 */
CURLcode Curl_xfer_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅੱਖਰ *buf, size_t blen,
                        size_t *pnrcvd);

CURLcode Curl_xfer_send_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_xfer_send_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);

/* Return TRUE if the transfer is not done, but further progress
 * is blocked. For example when it is only receiving and its writer
 * is PAUSED. */
bool Curl_xfer_is_blocked(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Query if send/recv for transfer is paused. */
bool Curl_xfer_send_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
bool Curl_xfer_recv_is_paused(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Enable/Disable pausing of send/recv for the transfer. */
CURLcode Curl_xfer_pause_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable);
CURLcode Curl_xfer_pause_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable);

/* TRUE if the transfer is secure, e.g. uses TLS and does not
 * use a forward proxy. */
bool Curl_xfer_is_secure(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Internal variant of the API function */
CURLcode Curl_easy_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਖਾਲੀ *ਬਫਰ, size_t buflen, size_t *n);

#ਜੇ_ਅੰਤ /* HEADER_CURL_TRANSFER_H */
