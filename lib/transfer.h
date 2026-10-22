#నిర్వచితం_కాకపోతే శీర్షిక_CURL_TRANSFER_H
#నిర్వచించు శీర్షిక_CURL_TRANSFER_H
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
#నిర్వచించు Curl_headersep(x) ((((x) == ':') || ((x) == ';')))

అక్షరం *Curl_checkheaders(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *thisheader,
                        స్థిరరాశి size_t thislen);

శూన్య Curl_init_CONNECT(నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_pretransfer(నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_sendrecv(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_retry_request(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం **url);
bool Curl_meets_timecondition(నిర్మాణం Curl_easy *దత్తాంశం, time_t timeofdoc);

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
CURLcode Curl_xfer_write_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *buf, size_t blen,
                              bool is_eos);

bool Curl_xfer_write_is_paused(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Write a single "header" line from a server response.
 * @param hd0      the null-terminated, single header line
 * @param hdlen    the length of the header line
 * @param is_eos   TRUE iff this is the end of the response
 */
CURLcode Curl_xfer_write_resp_hd(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *hd0, size_t hdlen, bool is_eos);

/* The transfer is neither receiving nor sending. */
శూన్య Curl_xfer_అమరిక_nop(నిర్మాణం Curl_easy *దత్తాంశం);

/* The transfer sends data on the given socket index */
శూన్య Curl_xfer_అమరిక_send(నిర్మాణం Curl_easy *దత్తాంశం,
                          int8_t sockindex);

/* The transfer receives data on the given socket index, the
 * amount to receive (or -1 if unknown). */
శూన్య Curl_xfer_అమరిక_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                          int8_t sockindex,
                          curl_off_t recv_కొలత);

/* *After* Curl_xfer_setup_xxx(), tell the transfer to shutdown the
 * connection at the end. Let the transfer either fail or ignore any
 * errors during shutdown. */
శూన్య Curl_xfer_సమితి_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool shutdown,
                            bool ignore_errors);

/**
 * The transfer will use socket 1 to send/recv. `recv_size` is
 * the amount to receive or -1 if unknown.
 */
శూన్య Curl_xfer_అమరిక_sendrecv(నిర్మాణం Curl_easy *దత్తాంశం,
                              int8_t sockindex,
                              curl_off_t recv_కొలత);

/**
 * Multi has set transfer to DONE. Last chance to trigger
 * missing response things like writing an EOS to the client.
 */
CURLcode Curl_xfer_write_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, bool premature);

/**
 * Return TRUE iff transfer has pending data to send. Checks involved
 * connection filters.
 */
bool Curl_xfer_needs_flush(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Flush any pending send data on the transfer connection.
 */
CURLcode Curl_xfer_flush(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Send data on the socket/connection filter designated
 * for transfer's outgoing data.
 * Return CURLE_OK on blocking with (*pnwritten == 0).
 */
CURLcode Curl_xfer_send(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి శూన్య *buf, size_t blen, bool eos,
                        size_t *pnwritten);

/**
 * Receive data on the socket/connection filter designated
 * for transfer's incoming data.
 * Return CURLE_AGAIN on blocking with (*pnrcvd == 0).
 */
CURLcode Curl_xfer_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                        అక్షరం *buf, size_t blen,
                        size_t *pnrcvd);

CURLcode Curl_xfer_send_close(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_xfer_send_shutdown(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);

/* Return TRUE if the transfer is not done, but further progress
 * is blocked. For example when it is only receiving and its writer
 * is PAUSED. */
bool Curl_xfer_is_blocked(నిర్మాణం Curl_easy *దత్తాంశం);

/* Query if send/recv for transfer is paused. */
bool Curl_xfer_send_is_paused(నిర్మాణం Curl_easy *దత్తాంశం);
bool Curl_xfer_recv_is_paused(నిర్మాణం Curl_easy *దత్తాంశం);

/* Enable/Disable pausing of send/recv for the transfer. */
CURLcode Curl_xfer_pause_send(నిర్మాణం Curl_easy *దత్తాంశం, bool enable);
CURLcode Curl_xfer_pause_recv(నిర్మాణం Curl_easy *దత్తాంశం, bool enable);

/* TRUE if the transfer is secure, e.g. uses TLS and does not
 * use a forward proxy. */
bool Curl_xfer_is_secure(నిర్మాణం Curl_easy *దత్తాంశం);

/* Internal variant of the API function */
CURLcode Curl_easy_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                        శూన్య *బఫరు, size_t buflen, size_t *n);

#అయితే_ముగింపు /* HEADER_CURL_TRANSFER_H */
