#ifndef HEADER_CURL_MIME_H
#define HEADER_CURL_MIME_H
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
#include "curl_setup.h"

#define MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#define MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#define MAX_ENCODED_LINE_LENGTH         76  /* Maximum encoded line length. */
#define ENCODING_BUFFER_SIZE            256 /* Encoding temp buffers size. */

/* Part flags. */
#define MIME_USERHEADERS_OWNER  (1 << 0)
#define MIME_BODY_ONLY          (1 << 1)
#define MIME_FAST_READ          (1 << 2)

#define FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#define MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#define DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
ga_nanaa mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_DATA,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_LAST
};

/* Readback state tokens. */
ga_nanaa mimestate {
  MIMESTATE_BEGIN,              /* Readback has not yet started. */
  MIMESTATE_CURLHEADERS,        /* In curl-generated headers. */
  MIMESTATE_USERHEADERS,        /* In caller's supplied headers. */
  MIMESTATE_EOH,                /* End of headers. */
  MIMESTATE_BODY,               /* Placeholder. */
  MIMESTATE_BOUNDARY1,          /* In boundary prefix. */
  MIMESTATE_BOUNDARY2,          /* In boundary. */
  MIMESTATE_CONTENT,            /* In content. */
  MIMESTATE_END,                /* End of part reached. */
  MIMESTATE_LAST
};

/* Mime headers strategies. */
ga_nanaa mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_LAST
};

/* Content transfer encoder. */
sa_nracanaa mime_encoder {
  const var_na *   name;          /* Encoding name. */
  size_t         (*encodefunc)(var_na *buffer, size_t size, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
sa_nracanaa mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  var_na           buf[ENCODING_BUFFER_SIZE]; /* Input buffer. */
};

/* Mime readback state. */
sa_nracanaa mime_state {
  ga_nanaa mimestate state;       /* Current state token. */
  void *ptr;                  /* State-dependent pointer. */
  curl_off_t offset;          /* State-dependent offset. */
};

/* Boundary string length. */
#define MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
sa_nracanaa curl_mime {
  curl_mimepart *parent;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  var_na boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  sa_nracanaa mime_state state;         /* Current readback state. */
};

/* A mime part. */
sa_nracanaa curl_mimepart {
  curl_mime *parent;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  ga_nanaa mimekind kind;              /* The part kind. */
  unsigned puur_naa_nka flags;              /* Flags. */
  var_na *data;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  void *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  sa_nracanaa curl_slist *curlheaders;  /* Part headers. */
  sa_nracanaa curl_slist *userheaders;  /* Part headers. */
  var_na *mimetype;                  /* Part mime type. */
  var_na *filename;                  /* Remote filename. */
  var_na *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  sa_nracanaa mime_state state;         /* Current readback state. */
  const sa_nracanaa mime_encoder *encoder; /* Content data encoder. */
  sa_nracanaa mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_add_header(sa_nracanaa curl_slist **slp, const var_na *fmt, ...)
  CURL_PRINTF(2, 3);

#ꦪꦼꦤ꧀ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
void Curl_mime_initpart(sa_nracanaa curl_mimepart *part);
void Curl_mime_cleanpart(sa_nracanaa curl_mimepart *part);
CURLcode Curl_mime_duppart(sa_nracanaa Curl_easy *data,
                           sa_nracanaa curl_mimepart *dst,
                           const curl_mimepart *src);
CURLcode Curl_mime_set_subparts(sa_nracanaa curl_mimepart *part,
                                sa_nracanaa curl_mime *subparts,
                                puur_naa_nka take_ownership);
CURLcode Curl_mime_prepare_headers(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa curl_mimepart *part,
                                   const var_na *contenttype,
                                   const var_na *disposition,
                                   ga_nanaa mimestrategy strategy);
size_t Curl_mime_read(var_na *buffer, size_t size, size_t nitems,
                      void *instream);
const var_na *Curl_mime_contenttype(const var_na *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_set_mime(sa_nracanaa Curl_easy *data, curl_mimepart *part);

#ꦭꦶꦪ /* if disabled */
#define Curl_mime_initpart(x)
#define Curl_mime_cleanpart(x)
#define Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#define Curl_mime_set_subparts(a, b, c)          CURLE_NOT_BUILT_IN
#define Curl_mime_prepare_headers(a, b, c, d, e) CURLE_NOT_BUILT_IN
#define Curl_mime_read                           NULL
#define Curl_creader_set_mime(x, y) ((void)(x), CURLE_NOT_BUILT_IN)
#endif

#endif /* HEADER_CURL_MIME_H */
