#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MIME_H
#నిర్వచించు శీర్షిక_CURL_MIME_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచించు MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#నిర్వచించు MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#నిర్వచించు MAX_ENCODED_పంక్తి_నిడివి         76  /* Maximum encoded line length. */
#నిర్వచించు ENCODING_బఫరు_కొలత            256 /* Encoding temp buffers size. */

/* Part flags. */
#నిర్వచించు MIME_USERHEADERS_OWNER  (1 << 0)
#నిర్వచించు MIME_BODY_ONLY          (1 << 1)
#నిర్వచించు MIME_FAST_READ          (1 << 2)

#నిర్వచించు FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#నిర్వచించు MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#నిర్వచించు DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
గణన mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_దత్తాంశం,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_చివరి
};

/* Readback state tokens. */
గణన mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_చివర,                /* End of part reached. */
  MIMESTATE_చివరి
};

/* Mime headers strategies. */
గణన mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_చివరి
};

/* Content transfer encoder. */
నిర్మాణం mime_encoder {
  స్థిరరాశి అక్షరం *   name;          /* Encoding name. */
  size_t         (*encodefunc)(అక్షరం *బఫరు, size_t కొలత, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
నిర్మాణం mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  అక్షరం           buf[ENCODING_బఫరు_కొలత]; /* Input buffer. */
};

/* Mime readback state. */
నిర్మాణం mime_state {
  గణన mimestate state;       /* Current state token. */
  శూన్య *ptr;                  /* State-dependent pointer. */
  curl_off_t స్థానభ్రంశం;          /* State-dependent offset. */
};

/* Boundary string length. */
#నిర్వచించు MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
నిర్మాణం curl_mime {
  curl_mimepart *మాతృ;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  అక్షరం boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  నిర్మాణం mime_state state;         /* Current readback state. */
};

/* A mime part. */
నిర్మాణం curl_mimepart {
  curl_mime *మాతృ;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  గణన mimekind రకం;              /* The part kind. */
  గుర్తులేని పూర్ణాంకం flags;              /* Flags. */
  అక్షరం *దత్తాంశం;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  శూన్య *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  నిర్మాణం curl_slist *curlheaders;  /* Part headers. */
  నిర్మాణం curl_slist *userheaders;  /* Part headers. */
  అక్షరం *mimetype;                  /* Part mime type. */
  అక్షరం *filename;                  /* Remote filename. */
  అక్షరం *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  నిర్మాణం mime_state state;         /* Current readback state. */
  స్థిరరాశి నిర్మాణం mime_encoder *encoder; /* Content data encoder. */
  నిర్మాణం mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_కూడు_శీర్షిక(నిర్మాణం curl_slist **slp, స్థిరరాశి అక్షరం *fmt, ...)
  CURL_PRINTF(2, 3);

#ఒకవేళ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
శూన్య Curl_mime_initpart(నిర్మాణం curl_mimepart *part);
శూన్య Curl_mime_cleanpart(నిర్మాణం curl_mimepart *part);
CURLcode Curl_mime_duppart(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం curl_mimepart *dst,
                           స్థిరరాశి curl_mimepart *src);
CURLcode Curl_mime_సమితి_subparts(నిర్మాణం curl_mimepart *part,
                                నిర్మాణం curl_mime *subparts,
                                పూర్ణాంకం take_ownership);
CURLcode Curl_mime_prepare_శీర్షికలు(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం curl_mimepart *part,
                                   స్థిరరాశి అక్షరం *contenttype,
                                   స్థిరరాశి అక్షరం *disposition,
                                   గణన mimestrategy strategy);
size_t Curl_mime_read(అక్షరం *బఫరు, size_t కొలత, size_t nitems,
                      శూన్య *instream);
స్థిరరాశి అక్షరం *Curl_mime_contenttype(స్థిరరాశి అక్షరం *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_సమితి_mime(నిర్మాణం Curl_easy *దత్తాంశం, curl_mimepart *part);

#లేకపోతే /* if disabled */
#నిర్వచించు Curl_mime_initpart(x)
#నిర్వచించు Curl_mime_cleanpart(x)
#నిర్వచించు Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#నిర్వచించు Curl_mime_సమితి_subparts(a, b, c)          CURLE_NOT_BUILT_లోపల
#నిర్వచించు Curl_mime_prepare_శీర్షికలు(a, b, c, d, e) CURLE_NOT_BUILT_లోపల
#నిర్వచించు Curl_mime_read                           ఖాళీ
#నిర్వచించు Curl_creader_సమితి_mime(x, y) ((శూన్య)(x), CURLE_NOT_BUILT_లోపల)
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_MIME_H */
