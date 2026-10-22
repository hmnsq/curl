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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#define MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#define MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#define MAX_ENCODED_ଧାଡ଼ି_LENGTH         76  /* Maximum encoded line length. */
#define ENCODING_BUFFER_SIZE            256 /* Encoding temp buffers size. */

/* Part flags. */
#define MIME_USERHEADERS_OWNER  (1 << 0)
#define MIME_BODY_ONLY          (1 << 1)
#define MIME_FAST_READ          (1 << 2)

#define FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#define MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#define DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
ଗଣନା mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_DATA,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_LAST
};

/* Readback state tokens. */
ଗଣନା mimestate {
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
ଗଣନା mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_LAST
};

/* Content transfer encoder. */
ସଂରଚନା mime_encoder {
  ଅଚଳ ଅକ୍ଷର *   name;          /* Encoding name. */
  size_t         (*encodefunc)(ଅକ୍ଷର *buffer, size_t size, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
ସଂରଚନା mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  ଅକ୍ଷର           buf[ENCODING_BUFFER_SIZE]; /* Input buffer. */
};

/* Mime readback state. */
ସଂରଚନା mime_state {
  ଗଣନା mimestate state;       /* Current state token. */
  ଶୂନ୍ୟ *ptr;                  /* State-dependent pointer. */
  curl_off_t offset;          /* State-dependent offset. */
};

/* Boundary string length. */
#define MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
ସଂରଚନା curl_mime {
  curl_mimepart *parent;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  ଅକ୍ଷର boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  ସଂରଚନା mime_state state;         /* Current readback state. */
};

/* A mime part. */
ସଂରଚନା curl_mimepart {
  curl_mime *parent;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  ଗଣନା mimekind kind;              /* The part kind. */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;              /* Flags. */
  ଅକ୍ଷର *data;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  ଶୂନ୍ୟ *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  ସଂରଚନା curl_slist *curlheaders;  /* Part headers. */
  ସଂରଚନା curl_slist *userheaders;  /* Part headers. */
  ଅକ୍ଷର *mimetype;                  /* Part mime type. */
  ଅକ୍ଷର *filename;                  /* Remote filename. */
  ଅକ୍ଷର *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  ସଂରଚନା mime_state state;         /* Current readback state. */
  ଅଚଳ ସଂରଚନା mime_encoder *encoder; /* Content data encoder. */
  ସଂରଚନା mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_add_header(ସଂରଚନା curl_slist **slp, ଅଚଳ ଅକ୍ଷର *fmt, ...)
  CURL_PRINTF(2, 3);

#ଯଦି !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
ଶୂନ୍ୟ Curl_mime_initpart(ସଂରଚନା curl_mimepart *part);
ଶୂନ୍ୟ Curl_mime_cleanpart(ସଂରଚନା curl_mimepart *part);
CURLcode Curl_mime_duppart(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା curl_mimepart *dst,
                           ଅଚଳ curl_mimepart *src);
CURLcode Curl_mime_set_subparts(ସଂରଚନା curl_mimepart *part,
                                ସଂରଚନା curl_mime *subparts,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_ownership);
CURLcode Curl_mime_prepare_headers(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା curl_mimepart *part,
                                   ଅଚଳ ଅକ୍ଷର *contenttype,
                                   ଅଚଳ ଅକ୍ଷର *disposition,
                                   ଗଣନା mimestrategy strategy);
size_t Curl_mime_read(ଅକ୍ଷର *buffer, size_t size, size_t nitems,
                      ଶୂନ୍ୟ *instream);
ଅଚଳ ଅକ୍ଷର *Curl_mime_contenttype(ଅଚଳ ଅକ୍ଷର *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_set_mime(ସଂରଚନା Curl_easy *data, curl_mimepart *part);

#ଅନ୍ୟଥା /* if disabled */
#define Curl_mime_initpart(x)
#define Curl_mime_cleanpart(x)
#define Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#define Curl_mime_set_subparts(a, b, c)          CURLE_NOT_BUILT_IN
#define Curl_mime_prepare_headers(a, b, c, d, e) CURLE_NOT_BUILT_IN
#define Curl_mime_read                           ଖାଲି
#define Curl_creader_set_mime(x, y) ((ଶୂନ୍ୟ)(x), CURLE_NOT_BUILT_IN)
#endif

#endif /* HEADER_CURL_MIME_H */
