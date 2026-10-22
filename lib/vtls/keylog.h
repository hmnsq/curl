#अगर_अघोषित HEADER_CURL_KEYLOG_H
#घोषणा HEADER_CURL_KEYLOG_H
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
#समावेश "curl_setup.h"

#घोषणा KEYLOG_LABEL_MAXLEN CURL_CSTRLEN("CLIENT_HANDSHAKE_TRAFFIC_SECRET")

#घोषणा CLIENT_RANDOM_आकार  32

/*
 * The master secret in TLS 1.2 and before is always 48 bytes. In TLS 1.3, the
 * secret size depends on the cipher suite's hash function which is 32 bytes
 * for SHA-256 and 48 bytes for SHA-384.
 */
#घोषणा SECRET_MAXLEN       48

/*
 * Opens the TLS key log file if requested by the user. The SSLKEYLOGFILE
 * environment variable specifies the output file.
 */
व्योम Curl_tls_keylog_open(व्योम);

/*
 * Closes the TLS key log file if not already.
 */
व्योम Curl_tls_keylog_close(व्योम);

/*
 * Returns true if the user successfully enabled the TLS key log file.
 */
bool Curl_tls_keylog_enabled(व्योम);

/*
 * Returns a pointer to the filename keys are being written to, if enabled.
 */
स्थिर अक्षर *Curl_tls_keylog_file_name(व्योम);

/*
 * Appends a key log file entry.
 * Returns true iff the key log file is open and a valid entry was provided.
 */
bool Curl_tls_keylog_write(स्थिर अक्षर *label,
                           स्थिर अचिन्हित अक्षर *client_random,
                           माप_प्रकार random_आकार,
                           स्थिर अचिन्हित अक्षर *secret, माप_प्रकार secretlen);

/*
 * Appends a line to the key log file, ensure it is terminated by an LF.
 * Returns true iff the key log file is open and a valid line was provided.
 */
bool Curl_tls_keylog_write_पंक्ति(स्थिर अक्षर *पंक्ति);

#पूर्ण_अगर /* HEADER_CURL_KEYLOG_H */
