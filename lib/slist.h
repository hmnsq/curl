#अगर_अघोषित HEADER_CURL_SLIST_H
#घोषणा HEADER_CURL_SLIST_H
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
काष्ठा curl_slist *Curl_slist_duplicate(काष्ठा curl_slist *inlist);

/*
 * Curl_slist_append_nodup() takes ownership of the given string and appends
 * it to the list.
 */
काष्ठा curl_slist *Curl_slist_append_nodup(काष्ठा curl_slist *list,
                                           स्थिर अक्षर *तथ्य);

#पूर्ण_अगर /* HEADER_CURL_SLIST_H */
