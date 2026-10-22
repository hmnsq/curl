#यदि_अपरिभाषितम् शीर्षकम्_CURL_ALTSVC_H
#परिभाषय शीर्षकम्_CURL_ALTSVC_H
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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#अन्तर्भाव "llist.h"

संरचना Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#परिभाषय MAX_ALTSVC_ENTRIES 5000

संरचना althost {
  वर्ण *host;
  uint16_t पत्तनम्;
  गणना alpnid alpnid;
};

संरचना altsvc {
  संरचना althost src;
  संरचना althost dst;
  time_t expires;
  संरचना Curl_llist_ग्रन्थिः ग्रन्थिः;
  BIT(persist);
};

संरचना altsvcinfo {
  वर्ण *filename;
  संरचना Curl_llist list; /* list of entries */
  दीर्घ flags; /* the publicly set bitmask */
};

ध्रुव वर्ण *Curl_alpnid2str(गणना alpnid id);
संरचना altsvcinfo *Curl_altsvc_init(शून्य);
CURLcode Curl_altsvc_आरोपय(संरचना altsvcinfo *asi, ध्रुव वर्ण *file);
CURLcode Curl_altsvc_save(संरचना Curl_easy *दत्त,
                          संरचना altsvcinfo *asi, ध्रुव वर्ण *file);
CURLcode Curl_altsvc_ctrl(संरचना Curl_easy *दत्त, ध्रुव दीर्घ ctrl);
शून्य Curl_altsvc_cleanup(संरचना altsvcinfo **asi);
CURLcode Curl_altsvc_parse(संरचना Curl_easy *दत्त,
                           संरचना altsvcinfo *asi, ध्रुव वर्ण *मानम्,
                           संरचना Curl_peer *origin,
                           गणना alpnid origin_alpnid);
bool Curl_altsvc_lookup(संरचना altsvcinfo *asi,
                        संरचना Curl_peer *origin,
                        गणना alpnid origin_alpnid,
                        संरचना altsvc **dstentry,
                        ध्रुव पूर्णाङ्क versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#अन्यथा
/* disabled */
#परिभाषय Curl_altsvc_save(a, b, c)
#परिभाषय Curl_altsvc_cleanup(x)
#यदि_अन्तः /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#यदि_अन्तः /* HEADER_CURL_ALTSVC_H */
