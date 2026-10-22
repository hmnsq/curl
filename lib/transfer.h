#ifndef HEADER_CURL_TRANSFER_H
#define HEADER_CURL_TRANSFER_H
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
#define Curl_headersep(x) ((((x) == ':') || ((x) == ';')))

वर्ण *Curl_checkheaders(अचर संरचना Curl_easy *data,
                        अचर वर्ण *thisheader,
                        अचर माप_प्रकार thislen);

शून्य Curl_init_CONNECT(संरचना Curl_easy *data);

CURLcode Curl_pretransfer(संरचना Curl_easy *data);

CURLcode Curl_sendrecv(संरचना Curl_easy *data);
CURLcode Curl_retry_request(संरचना Curl_easy *data, वर्ण **url);
bool Curl_meets_timecondition(संरचना Curl_easy *data, समय_प्रकार timeofdoc);

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
CURLcode Curl_xfer_write_resp(संरचना Curl_easy *data,
                              अचर वर्ण *buf, माप_प्रकार blen,
                              bool is_eos);

bool Curl_xfer_write_is_paused(संरचना Curl_easy *data);

/**
 * Write a single "header" line from a server response.
 * @param hd0      the null-terminated, single header line
 * @param hdlen    the length of the header line
 * @param is_eos   TRUE iff this is the end of the response
 */
CURLcode Curl_xfer_write_resp_hd(संरचना Curl_easy *data,
                                 अचर वर्ण *hd0, माप_प्रकार hdlen, bool is_eos);

/* The transfer is neither receiving nor sending. */
शून्य Curl_xfer_setup_nop(संरचना Curl_easy *data);

/* The transfer sends data on the given socket index */
शून्य Curl_xfer_setup_send(संरचना Curl_easy *data,
                          int8_t sockindex);

/* The transfer receives data on the given socket index, the
 * amount to receive (or -1 if unknown). */
शून्य Curl_xfer_setup_recv(संरचना Curl_easy *data,
                          int8_t sockindex,
                          curl_off_t recv_size);

/* *After* Curl_xfer_setup_xxx(), tell the transfer to shutdown the
 * connection at the end. Let the transfer either fail or ignore any
 * errors during shutdown. */
शून्य Curl_xfer_set_shutdown(संरचना Curl_easy *data,
                            bool shutdown,
                            bool ignore_errors);

/**
 * The transfer will use socket 1 to send/recv. `recv_size` is
 * the amount to receive or -1 if unknown.
 */
शून्य Curl_xfer_setup_sendrecv(संरचना Curl_easy *data,
                              int8_t sockindex,
                              curl_off_t recv_size);

/**
 * Multi has set transfer to DONE. Last chance to trigger
 * missing response things like writing an EOS to the client.
 */
CURLcode Curl_xfer_write_done(संरचना Curl_easy *data, bool premature);

/**
 * Return TRUE iff transfer has pending data to send. Checks involved
 * connection filters.
 */
bool Curl_xfer_needs_flush(संरचना Curl_easy *data);

/**
 * Flush any pending send data on the transfer connection.
 */
CURLcode Curl_xfer_flush(संरचना Curl_easy *data);

/**
 * Send data on the socket/connection filter designated
 * for transfer's outgoing data.
 * Return CURLE_OK on blocking with (*pnwritten == 0).
 */
CURLcode Curl_xfer_send(संरचना Curl_easy *data,
                        अचर शून्य *buf, माप_प्रकार blen, bool eos,
                        माप_प्रकार *pnwritten);

/**
 * Receive data on the socket/connection filter designated
 * for transfer's incoming data.
 * Return CURLE_AGAIN on blocking with (*pnrcvd == 0).
 */
CURLcode Curl_xfer_recv(संरचना Curl_easy *data,
                        वर्ण *buf, माप_प्रकार blen,
                        माप_प्रकार *pnrcvd);

CURLcode Curl_xfer_send_close(संरचना Curl_easy *data);
CURLcode Curl_xfer_send_shutdown(संरचना Curl_easy *data, bool *done);

/* Return TRUE if the transfer is not done, but further progress
 * is blocked. For example when it is only receiving and its writer
 * is PAUSED. */
bool Curl_xfer_is_blocked(संरचना Curl_easy *data);

/* Query if send/recv for transfer is paused. */
bool Curl_xfer_send_is_paused(संरचना Curl_easy *data);
bool Curl_xfer_recv_is_paused(संरचना Curl_easy *data);

/* Enable/Disable pausing of send/recv for the transfer. */
CURLcode Curl_xfer_pause_send(संरचना Curl_easy *data, bool enable);
CURLcode Curl_xfer_pause_recv(संरचना Curl_easy *data, bool enable);

/* TRUE if the transfer is secure, e.g. uses TLS and does not
 * use a forward proxy. */
bool Curl_xfer_is_secure(संरचना Curl_easy *data);

/* Internal variant of the API function */
CURLcode Curl_easy_recv(संरचना Curl_easy *data,
                        शून्य *buffer, माप_प्रकार buflen, माप_प्रकार *n);

#endif /* HEADER_CURL_TRANSFER_H */
