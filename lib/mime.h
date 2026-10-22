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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#define MIME_BOUNDARY_DASHES            24  /* leading boundary dashes */
#define MIME_RAND_BOUNDARY_CHARS        22  /* Nb. of random boundary chars. */
#define MAX_ENCODED_ಸಾಲು_LENGTH         76  /* Maximum encoded line length. */
#define ENCODING_BUFFER_SIZE            256 /* Encoding temp buffers size. */

/* Part flags. */
#define MIME_USERHEADERS_OWNER  (1 << 0)
#define MIME_BODY_ONLY          (1 << 1)
#define MIME_FAST_READ          (1 << 2)

#define FILE_CONTENTTYPE_DEFAULT        "application/octet-stream"
#define MULTIPART_CONTENTTYPE_DEFAULT   "multipart/mixed"
#define DISPOSITION_DEFAULT             "attachment"

/* Part source kinds. */
ಗಣನಾ mimekind {
  MIMEKIND_NONE = 0,            /* Part not set. */
  MIMEKIND_DATA,                /* Allocated mime data. */
  MIMEKIND_FILE,                /* Data from file. */
  MIMEKIND_CALLBACK,            /* Data from `read' callback. */
  MIMEKIND_MULTIPART,           /* Data is a mime subpart. */
  MIMEKIND_LAST
};

/* Readback state tokens. */
ಗಣನಾ mimestate {
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
ಗಣನಾ mimestrategy {
  MIMESTRATEGY_MAIL,            /* Mime mail. */
  MIMESTRATEGY_FORM,            /* HTTP post form. */
  MIMESTRATEGY_LAST
};

/* Content transfer encoder. */
ಸಂರಚನಾ mime_encoder {
  ಧ್ರುವ ವರ್ಣ *   name;          /* Encoding name. */
  size_t         (*encodefunc)(ವರ್ಣ *buffer, size_t size, bool ateof,
                               curl_mimepart *part);  /* Encoded read. */
  curl_off_t     (*sizefunc)(curl_mimepart *part);  /* Encoded size. */
};

/* Content transfer encoder state. */
ಸಂರಚನಾ mime_encoder_state {
  size_t         pos;           /* Position on output line. */
  size_t         bufbeg;        /* Next data index in input buffer. */
  size_t         bufend;        /* First unused byte index in input buffer. */
  ವರ್ಣ           buf[ENCODING_BUFFER_SIZE]; /* Input buffer. */
};

/* Mime readback state. */
ಸಂರಚನಾ mime_state {
  ಗಣನಾ mimestate state;       /* Current state token. */
  ಶೂನ್ಯ *ptr;                  /* State-dependent pointer. */
  curl_off_t offset;          /* State-dependent offset. */
};

/* Boundary string length. */
#define MIME_BOUNDARY_LEN (MIME_BOUNDARY_DASHES + MIME_RAND_BOUNDARY_CHARS)

/* A mime multipart. */
ಸಂರಚನಾ curl_mime {
  curl_mimepart *parent;           /* Parent part. */
  curl_mimepart *firstpart;        /* First part. */
  curl_mimepart *lastpart;         /* Last part. */
  ವರ್ಣ boundary[MIME_BOUNDARY_LEN + 1]; /* The part boundary. */
  ಸಂರಚನಾ mime_state state;         /* Current readback state. */
};

/* A mime part. */
ಸಂರಚನಾ curl_mimepart {
  curl_mime *parent;               /* Parent mime structure. */
  curl_mimepart *nextpart;         /* Forward linked list. */
  ಗಣನಾ mimekind kind;              /* The part kind. */
  unsigned ಪೂರ್ಣಾಙ್ಕ flags;              /* Flags. */
  ವರ್ಣ *data;                      /* Memory data or filename. */
  curl_read_callback readfunc;     /* Read function. */
  curl_seek_callback seekfunc;     /* Seek function. */
  curl_free_callback freefunc;     /* Argument free function. */
  ಶೂನ್ಯ *arg;                       /* Argument to callback functions. */
  FILE *fp;                        /* File pointer. */
  ಸಂರಚನಾ curl_slist *curlheaders;  /* Part headers. */
  ಸಂರಚನಾ curl_slist *userheaders;  /* Part headers. */
  ವರ್ಣ *mimetype;                  /* Part mime type. */
  ವರ್ಣ *filename;                  /* Remote filename. */
  ವರ್ಣ *name;                      /* Data name. */
  curl_off_t datasize;             /* Expected data size. */
  ಸಂರಚನಾ mime_state state;         /* Current readback state. */
  ಧ್ರುವ ಸಂರಚನಾ mime_encoder *encoder; /* Content data encoder. */
  ಸಂರಚನಾ mime_encoder_state encstate; /* Data encoder state. */
  size_t lastreadstatus;           /* Last read callback returned status. */
};

CURLcode Curl_mime_add_header(ಸಂರಚನಾ curl_slist **slp, ಧ್ರುವ ವರ್ಣ *fmt, ...)
  CURL_PRINTF(2, 3);

#ಯದಿ !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

/* Prototypes. */
ಶೂನ್ಯ Curl_mime_initpart(ಸಂರಚನಾ curl_mimepart *part);
ಶೂನ್ಯ Curl_mime_cleanpart(ಸಂರಚನಾ curl_mimepart *part);
CURLcode Curl_mime_duppart(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ curl_mimepart *dst,
                           ಧ್ರುವ curl_mimepart *src);
CURLcode Curl_mime_set_subparts(ಸಂರಚನಾ curl_mimepart *part,
                                ಸಂರಚನಾ curl_mime *subparts,
                                ಪೂರ್ಣಾಙ್ಕ take_ownership);
CURLcode Curl_mime_prepare_headers(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ curl_mimepart *part,
                                   ಧ್ರುವ ವರ್ಣ *contenttype,
                                   ಧ್ರುವ ವರ್ಣ *disposition,
                                   ಗಣನಾ mimestrategy strategy);
size_t Curl_mime_read(ವರ್ಣ *buffer, size_t size, size_t nitems,
                      ಶೂನ್ಯ *instream);
ಧ್ರುವ ವರ್ಣ *Curl_mime_contenttype(ಧ್ರುವ ವರ್ಣ *filename);

/**
 * Install a client reader as upload source that reads the given
 * mime part.
 */
CURLcode Curl_creader_set_mime(ಸಂರಚನಾ Curl_easy *data, curl_mimepart *part);

#ಅನ್ಯಥಾ /* if disabled */
#define Curl_mime_initpart(x)
#define Curl_mime_cleanpart(x)
#define Curl_mime_duppart(x, y, z) CURLE_OK /* Nothing to duplicate. Succeed */
#define Curl_mime_set_subparts(a, b, c)          CURLE_NOT_BUILT_IN
#define Curl_mime_prepare_headers(a, b, c, d, e) CURLE_NOT_BUILT_IN
#define Curl_mime_read                           ರಿಕ್ತ
#define Curl_creader_set_mime(x, y) ((ಶೂನ್ಯ)(x), CURLE_NOT_BUILT_IN)
#endif

#endif /* HEADER_CURL_MIME_H */
