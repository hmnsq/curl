#اگر_غیر_تعریف_شدہ سرخی_CURL_SLIST_H
#تعریف_کرو سرخی_CURL_SLIST_H
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
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ساخت curl_slist *Curl_slist_duplicate(ساخت curl_slist *inlist);

/*
 * Curl_slist_append_nodup() takes ownership of the given string and appends
 * it to the list.
 */
ساخت curl_slist *Curl_slist_append_nodup(ساخت curl_slist *list,
                                           ثابت حرف *ڈیٹا);

#اگر_ختم /* HEADER_CURL_SLIST_H */
