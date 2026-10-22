#ifndef HEADER_CURL_KEYLOG_H
#define HEADER_CURL_KEYLOG_H
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
#অন্তর্ভাব "curl_setup.h"

#define KEYLOG_LABEL_MAXLEN CURL_CSTRLEN("CLIENT_HANDSHAKE_TRAFFIC_SECRET")

#define CLIENT_RANDOM_SIZE  32

/*
 * The master secret in TLS 1.2 and before is always 48 bytes. In TLS 1.3, the
 * secret size depends on the cipher suite's hash function which is 32 bytes
 * for SHA-256 and 48 bytes for SHA-384.
 */
#define SECRET_MAXLEN       48

/*
 * Opens the TLS key log file if requested by the user. The SSLKEYLOGFILE
 * environment variable specifies the output file.
 */
শূন্য Curl_tls_keylog_open(শূন্য);

/*
 * Closes the TLS key log file if not already.
 */
শূন্য Curl_tls_keylog_close(শূন্য);

/*
 * Returns true if the user successfully enabled the TLS key log file.
 */
bool Curl_tls_keylog_enabled(শূন্য);

/*
 * Returns a pointer to the filename keys are being written to, if enabled.
 */
ধ্রুব বর্ণ *Curl_tls_keylog_file_name(শূন্য);

/*
 * Appends a key log file entry.
 * Returns true iff the key log file is open and a valid entry was provided.
 */
bool Curl_tls_keylog_write(ধ্রুব বর্ণ *label,
                           ধ্রুব unsigned বর্ণ *client_random,
                           size_t random_size,
                           ধ্রুব unsigned বর্ণ *secret, size_t secretlen);

/*
 * Appends a line to the key log file, ensure it is terminated by an LF.
 * Returns true iff the key log file is open and a valid line was provided.
 */
bool Curl_tls_keylog_write_সারি(ধ্রুব বর্ণ *সারি);

#endif /* HEADER_CURL_KEYLOG_H */
